/**
  ******************************************************************************
  * @file           : hx90xx.c  
  * @version        : v1.0
  * @brief          : Operate hx90xx chip 
  ******************************************************************************
  *
  * 
  *
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "bsp_common.h"
#include "bsp_config.h"
#include "bsp_internal.h" // IWYU pragma: keep
#include "bsp_i2c.h" // IWYU pragma: keep
#include "cmsis_os_api.h"
#include <stdint.h>
#include <string.h>
#include "hal_timer.h"
#include "hx90xx.h"


//------------------------------------------------------
/*阈值与配置寄存器对照表，
0x00	00000: 128
0x01	00001: 144
0x02	00010: 160
0x03	00011: 176
0x04	00100: 192
0x05	00101: 208
0x06	00110: 224
0x07	00111: 240
0x08	01000: 256
0x09	01001: 320
0x0A	01010: 384
0x0B	01011: 448
0x0C	01100: 512
0x0D	01101: 640
0x0E	01110: 768
0x0F	01111: 896
0x10	10000: 1024
0x11	10001: 1280
0x12	10010: 1536
0x13	10011: 1792
0x14	10100: 2048
0x15	10101: 2560
0x16	10110: 3072
0x17	10111: 3584
0x18	11000: 4096
0x19	11001: 5120
0x1A	11010: 6144
0x1B	11011: 7168
0x1C	11100: 8192
0x1D	11101: 12288
0x1E	11110: 16384
0x1F	11111: 24576
*/


uint16_t g_HX90XXThresArray[] =
{ 
  128,144,160,176,192,208,224,240,256,320,384,448,512,640,768,896,1024,1280,1536,1792,2048,2560,3072,
	3584,4096,5120,6144,7168,8192,12288,16384,24576, 120};


//
#define CS0 0
#define CS1 1
#define CS2 2
#define CS3 3 
#define CS4 4
#define NA  10
//
#define CH0_POS CS0
#define CH0_NEG CS1
#define CH1_POS NA
#define CH1_NEG NA
#define CH2_POS NA
#define CH2_NEG NA
#define CH3_POS NA
#define CH3_NEG NA
#define CH4_POS NA
#define CH4_NEG NA

#define  TOUCH_HIGH_DIFF   3072
// define  wear channel and touch channel

// user  to config
#define WEAR0   CH1
#define WEAR1   CH3
#define TOUCH   CH4
#define REF0    CH0
#define REF1    CH2
#define TKEY    (0x01<<TOUCH)

int16_t wear0_high_diff = 2048;
int16_t wear0_low_diff = 1792;
int16_t wear1_high_diff = 2048;
int16_t wear1_low_diff = 1792;
int16_t touch_high_diff = TOUCH_HIGH_DIFF;
//int16_t touch_low_diff = 2048;

uint32_t delay_count = 0;
uint8_t hx90xx_chip_id = 0;

uint8_t  nv_dac_swap = 0;
#define WIN_SIZE   4
int16_t  window_raw0[WIN_SIZE] = {0};
int16_t  window_raw1[WIN_SIZE] = {0};
int16_t glp[2] = {0};
int16_t min_glp[2] = {0};
uint16_t nv_wear_offset[2] = {0};
int16_t  nv_base_data[2] = {0};
int16_t  nv_base_data_backup[2] = {0};
const uint16_t one_offset_code = 490;

uint16_t nv_wear_K0 = 0;
uint16_t nv_wear_K1 = 0;
uint16_t nv_touch_K2 = 0;


static uint8_t reoffset_flag = 0;
uint8_t  is_offset_manual = 0;  // 0：相对值模式 1：绝对值模式
uint8_t  force_to_auto_mode = 0;  //采用自动 相对值模式
uint8_t  is_factory_mode = 0;  // 工厂测试模式
uint8_t  is_unwear_mode = 0;  // 对空校准模式
uint8_t start_updata_nv = 0;


#define hx_min(x,y)  ( (x)>(y)?(y):(x) )
#define hx_abs(x)  ((x)>0?(x):(-(x)))


#ifdef __TIMER_DEBUG__
static uint8_t timer_debug = 1;
#else
static uint8_t timer_debug = 0;
#endif

	
uint8_t prox_status = 0, prox_status_pre = 0;
static uint8_t wear_status = 0;
static uint8_t touch_status = 0;
static uint8_t pre_touch_status = 0;
static uint8_t pre_wear_status = 0;
uint8_t is_reset_check = 0; // 是否复位校准
#ifdef TYHX_TOUCH_FUNTION
uint32_t timer_count = 0;
uint32_t conv_rdy_count = 0; //转换中断计数
static uint8_t motion_status = 0;
uint32_t touch_key_count = 0; // 按键次数
uint32_t touch_time_press_start = 0; // 按键开始按下的时间
uint32_t touch_release_timer = 0; // 按键释放的时间
uint8_t is_conv_int_on = 0; // 打开转换模式
#endif

#define  ARRAY_NUM  7  // 144ms  1s
//int32_t  arr_diff0[ARRAY_NUM] = {0};
int32_t  arr_diff1[ARRAY_NUM] = {0};
int32_t table_touch_cnt = 0;
int32_t table_count = 0;

#define  CALCU_CNT    3
int16_t  wear_cnt[2] = {0};
int16_t  unwear_cnt[2] = {0};
uint8_t  wear_s[2] = {0};
uint8_t  pre_wear_s[2] = {0};

#define HX90XX_POLL_INTERVAL_MS 150
static os_timer_id hx90xx_poll_timer = NULL;
static thread_context_t hx90xx_poll_thd = {0};
static uint8_t hx90xx_poll_worker_inited = 0;
uint8_t  timer_start = 0;
static void hx90xx_poll_timer_handler(void *param);

static void hx90xx_poll_worker_thread(void *arg)
{
    uint8_t msg = 0;
    (void)arg;
    while (1) {
        MSG_QUEUE_RECV(hx90xx_poll_thd.msg_id, (void *)&msg, OS_WAIT_FOREVER);
        if (timer_start) {
            hx90xx_report_diff_prox_status();
        }
    }
}

static bool hx90xx_poll_worker_init(void)
{
    if (hx90xx_poll_worker_inited) {
        return true;
    }
    CREATE_MSG_TYPE_QUEUE(&hx90xx_poll_thd.msg_id, 4, 1);
    if (hx90xx_poll_thd.msg_id == NULL) {
        LOGE("hx90xx poll queue create fail");
        return false;
    }
    if (RUN_NEW_THREAD(&hx90xx_poll_thd, "cap_poll", hx90xx_poll_worker_thread,
                       RT_PRIO_SENSOR_INPUT_DISPATCH_THREAD, INPUT_SENSOR_THREAD_STACKSIZE) != 0) {
        LOGE("hx90xx poll thread create fail");
        return false;
    }
    CREATE_TIMER(&hx90xx_poll_timer, "hx90xx_poll", NULL, OS_TIMER_PERIODIC, hx90xx_poll_timer_handler);
    if (hx90xx_poll_timer == NULL) {
        LOGE("hx90xx poll timer create fail");
        return false;
    }
    hx90xx_poll_worker_inited = 1;
    return true;
}

static void hx90xx_poll_timer_handler(void *param)
{
	(void)param;
    uint8_t msg = 0x5A;
    if (timer_start && hx90xx_poll_thd.msg_id != NULL) {
        (void)MSG_QUEUE_SEND(hx90xx_poll_thd.msg_id, (void *)&msg, 0);
    }
}

// 150ms 启动定时器轮询， uint8_t hx90xx_report_diff_prox_status(void)
void hx90xx_timer_start(void)
{
	if (!hx90xx_poll_worker_init())
	{
		timer_start = 0;
		return;
	}
	(void)STOP_TIMER(hx90xx_poll_timer);
	if (START_TIMER(hx90xx_poll_timer, HX90XX_POLL_INTERVAL_MS) != osOK)
	{
		LOG_PBUS("hx90xx timer start fail\r\n");
		timer_start = 0;
		return;
	}
	timer_start = 1;
}

// 停止定时器
void hx90xx_timer_stop(void)
{
	timer_start = 0;
	if (hx90xx_poll_timer == NULL)
	{
		return;
	}
	if (STOP_TIMER(hx90xx_poll_timer) != osOK)
	{
		LOG_PBUS("hx90xx timer stop fail\r\n");
	}
}




/* 延时函数， 根据平台完成接口*/
void delay_ms(uint16_t t)
{
	msleep(t);
}


/* i2c 写函数， 根据平台完成接口*/
bool hx9033s_write_reg(uint8_t addr, uint8_t data) 
{
    int ret = 0;

    i2c_master_data_t i2c_data = {
        .dev_address = hx9033s_ADDR,
        .reg_addr = &addr,
        .reg_addr_len = 1,
        .data = &data,
        .len = 1,
    };

    for (uint8_t i = 0; i < 3; i++) {
        ret = bsp_i2c_write_byte(CAP_SENSOR_I2C_ID, &i2c_data);
        if (ret == BSP_OK) {
            return true;
        }
        msleep(10);
    }

    return false;
}

/* i2c 读函数， 根据平台完成接口*/
uint8_t hx9033s_read_reg(uint8_t addr) 
{
    int ret = 0;
    uint8_t read_data;

    i2c_master_data_t i2c_data = {
        .dev_address = hx9033s_ADDR,
        .reg_addr = &addr,
        .reg_addr_len = 1,
        .data = &read_data,
        .len = 1,
    };
    
	for (uint8_t j = 0; j < 3; j++) {
		ret = bsp_i2c_read_byte(CAP_SENSOR_I2C_ID, &i2c_data);
		if (ret == BSP_OK) {
			return read_data;
		}
		msleep(10);
	}

    return 0;   
}


int16_t hx90xx_read_two_reg_low_triple(uint8_t addh, uint8_t addl)
{
		uint16_t ret_val = 0;
		uint8_t  databuf[2] = {0};

#ifdef man_lock
		hx9033s_write_reg(RW_ED_DSP_CONFIG_CTRL1,0x90);
		databuf[1] = hx9033s_read_reg(addl); 
		databuf[0] = hx9033s_read_reg(addh);
		hx9033s_write_reg(RW_ED_DSP_CONFIG_CTRL1,0x80);
#else
		databuf[1] = hx9033s_read_reg(addl); 
		databuf[1] = hx9033s_read_reg(addl); 
		databuf[1] = hx9033s_read_reg(addl); 
		databuf[0] = hx9033s_read_reg(addh);
#endif
		
		//    de_printf("byte_high 0x%.2x byte_low 0x%.2x   ", databuf[0],databuf[1]);
    
	  ret_val = (uint16_t)((databuf[0]<<8)|(databuf[1]));  
	if (ret_val > 32767) //if data[24:8], 8388608  change 32768    
		return (int16_t)(ret_val  - 65536);
	else  
		return (int16_t)ret_val;
}


void hx90xx_power_up(void)
{
    // HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);//LDO power on
    // //    LOG_PBUS("LDO power on.\n" );
    // delay_ms(50);    //上电1ms就可以生效,稳妥变为10ms
}

void hx90xx_power_down(void)
{
    // HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);//LDO power on
    // //    LOG_PBUS("LDO power down.\n" );
    // delay_ms(50);
}

/*上电复位*/
void hx90xx_power_restart(void)
{
    hx90xx_power_down();
    hx90xx_power_up();
}


uint8_t hx90xx_check_device_id(void)
{
    uint8_t device_id = 0x00;
	
    device_id = hx9033s_read_reg(RO_01_DEVICE_ID);

    LOG_PBUS("device_id is : 0x%.2x !\r\n", device_id);
		return device_id;
}

void hx90xx_channel_config(void)
{
		uint16_t ch_cfg = 0; 

    ch_cfg = (uint16_t) ((0x03<<(CH0_POS*2)) + (0x02<<(CH0_NEG*2)));
    hx9033s_write_reg(RW_68_CH0_CFG_0, (uint8_t)(ch_cfg));      //ch0 pad config
    hx9033s_write_reg(RW_69_CH0_CFG_1, (uint8_t)(ch_cfg>>8));   //ch0 pad config
    ch_cfg = (uint16_t)((0x03<<(CH1_POS*2)) + (0x02<<(CH1_NEG*2)));
    hx9033s_write_reg(RW_6A_CH1_CFG_0, (uint8_t)(ch_cfg));      //ch1 pad config
    hx9033s_write_reg(RW_6B_CH1_CFG_1, (uint8_t)(ch_cfg>>8));   //ch1 pad config
    ch_cfg = (uint16_t)((0x03<<(CH2_POS*2)) + (0x02<<(CH2_NEG*2)));
    hx9033s_write_reg(RW_6C_CH2_CFG_0, (uint8_t)(ch_cfg));      //ch2 pad config
    hx9033s_write_reg(RW_6D_CH2_CFG_1, (uint8_t)(ch_cfg>>8));   //ch2 pad config
    ch_cfg = (uint16_t)((0x03<<(CH3_POS*2)) + (0x02<<(CH3_NEG*2)));
    hx9033s_write_reg(RW_6E_CH3_CFG_0, (uint8_t)(ch_cfg));      //ch3 pad config
    hx9033s_write_reg(RW_6F_CH3_CFG_1, (uint8_t)(ch_cfg>>8));   //ch3 pad config 
//	
    ch_cfg = (uint16_t) ((0x03<<(CH4_POS*2)) + (0x02<<(CH4_NEG*2)));
    hx9033s_write_reg(RW_70_CH4_CFG_0, (uint8_t)(ch_cfg));      //ch4 pad config
    hx9033s_write_reg(RW_71_CH4_CFG_1, (uint8_t)(ch_cfg>>8));   //ch4 pad config
		
}


uint16_t  hx90xx_read_offset(uint8_t chx_offset)
{
	uint16_t offset = 0;
	uint8_t dac_h = 0;
	uint8_t dac_l = 0;
	uint16_t byte_high =0,  byte_low = 0;
	if (chx_offset == NA)
	{
		LOG_PBUS("90XX ch read error: chx_offset is NA\r\n");
		return 0;
	}
		
	byte_high = RW_59_OFFSET_DAC0_1 + chx_offset*2;
	byte_low =  RW_58_OFFSET_DAC0_0 + chx_offset*2;
	dac_h = hx9033s_read_reg(byte_high);
	dac_l = hx9033s_read_reg(byte_low);
	offset = ((uint16_t)(dac_h)<<8) | dac_l;
	return offset;
}

void  hx90xx_write_offset(uint16_t offset_value, uint8_t chx_offset)
{
	uint8_t  write_val = 0;
	uint16_t byte_high =0,  byte_low = 0;

		if (chx_offset == NA)
		{
			LOG_PBUS("hx90xx_write_offset error: chx_offset is NA\r\n");
			return;
		}
	byte_high = RW_59_OFFSET_DAC0_1 + chx_offset*2;
	byte_low =  RW_58_OFFSET_DAC0_0 + chx_offset*2;

	write_val = (uint8_t)(offset_value >> 8);
	hx9033s_write_reg(byte_high,write_val);
	write_val = (uint8_t)offset_value;
	hx9033s_write_reg(byte_low,write_val); 

}

int16_t  hx90xx_read_raw_bl(uint8_t chx_raw)
{
	int16_t raw = 0;
	uint16_t byte_high =0,  byte_low = 0;
	if (chx_raw == NA)
	{
		LOG_PBUS("hx90xx_read_raw_bl error: chx_raw is NA\r\n");
		return -1;
	}
	byte_high = RO_0F_RAW_BL_CH0_2 + chx_raw*3;
	byte_low =  RO_0E_RAW_BL_CH0_1 + chx_raw*3;


	raw = hx90xx_read_two_reg_low_triple(byte_high,byte_low);

	return raw;
}


int16_t  hx90xx_read_diff_lp(uint8_t chx_diff)
{
	int16_t diff = 0;
	uint16_t byte_high =0,  byte_low = 0;
	if (chx_diff == NA)
	{
		LOG_PBUS("hx90xx_read_diff_lp error: chx_diff is NA\r\n");
		return -1;
	}
	byte_high = RO_27_LP_DIFF_CH0_2 + chx_diff*3;
	byte_low =  RO_26_LP_DIFF_CH0_1 + chx_diff*3;

	diff = hx90xx_read_two_reg_low_triple(byte_high,byte_low);

	return diff;
}

/* chx set: ch0: 0,ch1:1, ch2:2 */
void hx90xx_config_thres(uint8_t chx,int16_t high_thres, int16_t base_data)
{
	int16_t ArraySize = 0;  
	int32_t nCoe = 0;  
	uint16_t index = 0;
	int16_t thres = 0;
	thres = high_thres + base_data;
	if (thres <= 0 || chx == NA )
	{
		return;
	}

	ArraySize = (sizeof (g_HX90XXThresArray)) / (sizeof(uint16_t));  

	LOG_PBUS("hx90xx_config_thres:ch%d  high_thres: %d, base_data: %d , ArraySize: %d\r\n", chx, high_thres, base_data, ArraySize);
	
	for (index = 0; index < ArraySize-1; index++)
	{
		if(thres <= (g_HX90XXThresArray[index] + g_HX90XXThresArray[index+1])/2)
		{
			 nCoe = (thres)*64/g_HX90XXThresArray[index];
			 if (nCoe>0)
			 {
				//hx9033s_write_reg(PAGE0_B9_CH0_COE_CFG_0+2*chx, nCoe);
			 }
			 hx9033s_write_reg(RW_C9_PROX_HIGH_THRES1_CFG0+chx, index);
			 break;
		}
	}


	LOG_PBUS("90XX read ch%d high: 0x%x, nCoe: %d \r\n", chx, hx9033s_read_reg(RW_C9_PROX_HIGH_THRES1_CFG0+chx), nCoe);
			
}

void hx90xx_config_low_thres(uint8_t chx,uint8_t low_flag)
{
	hx9033s_write_reg(RW_41_PROX_LOW_THRES1_CFG0+1*chx, low_flag);
}

bool hx90xx_reg_init(void)
{
  bool write_ok = true;
	uint8_t RA_ch_num_cfg = 0x0f;  //  1=ch0, 3= ch0+ch1  7=ch0+ch1+ch2, 0x1f= ch0+ch1+ch2+ch3+ch4+ch5
	uint8_t RA_prf_cfg = 0x07;  //  odr_base 6=14ms;  7= 18ms; 8=22ms
	uint8_t RA_scan_factor_ch0 = 0;  // odr = odr_base * scan_factor:  0=1; 1=2; 2=4;3=8, 4= 16,5=32,6=64,7=128;
	uint8_t RA_scan_factor_ch1 = 0;
	uint8_t RA_scan_factor_ch2 = 0; // 6: 6=64;  ref
	uint8_t RA_scan_factor_ch3 = 0;
	uint8_t RA_scan_factor_ch4 = 0; //如果定义为touch 或是slide 通道， 建议为0
	
	uint8_t RA_offset_cali_ch0 = 0; // 手动配置offset_dac 使能， 写1有效
	uint8_t RA_offset_cali_ch1 = 1;
	uint8_t RA_offset_cali_ch2 = 0;
	uint8_t RA_offset_cali_ch3 = 1;
	uint8_t RA_offset_cali_ch4 = 0;

	uint8_t RA_lp_alpha_ch0 = 0; // 0=1; 1= 1/2; 2=1/4; 3= 1/8; 4=1/16;5=1/32
	uint8_t RA_lp_alpha_ch1 = 0;
	uint8_t RA_lp_alpha_ch2 = 0;
	uint8_t RA_lp_alpha_ch3 = 0;
	uint8_t RA_lp_alpha_ch4 = 0; // 如果定义为touch 通道， 建议为0
	
	uint8_t RA_bl_up_alpha_ch0 = 9; // 7 = 1/64; 8 = 1/128; 9=1/256  10 = 1/512
	uint8_t RA_bl_up_alpha_ch1 = 0; // 佩戴通道 先设定为1
	uint8_t RA_bl_up_alpha_ch2 = 9; 
	uint8_t RA_bl_up_alpha_ch3 = 0; // 佩戴通道 先设定为1
	uint8_t RA_bl_up_alpha_ch4 = 8; // 如果定义为touch 通道， 建议为8

	if (is_factory_mode == 1)
	{
		RA_lp_alpha_ch0 = 1; // 0=1; 1= 1/2; 2=1/4; 3= 1/8; 4=1/16;5=1/32
		RA_lp_alpha_ch1 = 1;
		RA_lp_alpha_ch2 = 1;
		RA_lp_alpha_ch3 = 1;
	}
	
	hx9033s_write_reg(RW_03_CH_NUM_CFG,                0x00);
	hx9033s_write_reg(RW_06_GLOBAL_CTRL2,              0x00); // [4:0]close chop 可分通道打开
	hx9033s_write_reg(RW_40_PRF_CFG,                   RA_prf_cfg);	//odr
	hx9033s_write_reg(RW_E6_CH10_SCAN_FACTOR,          RA_scan_factor_ch0|(RA_scan_factor_ch1<<4));
	hx9033s_write_reg(RW_E7_CH32_SCAN_FACTOR,          RA_scan_factor_ch2|(RA_scan_factor_ch3<<4));
	hx9033s_write_reg(RW_EC_CH4_FACTOR_CTRL,           RA_scan_factor_ch4);//[3:0] ch4_scan_factor,[7:4] ch4_doze

	//doze open
	//hx9033s_write_reg(RW_E9_CH10_DOZE_FACTOR,          0x88); // bit00 2X  bit3 en
	//hx9033s_write_reg(RW_EA_CH32_DOZE_FACTOR,          0x08);

	hx9033s_write_reg(RW_4E_TC_NOSR_CFG0,              0x22); 
	hx9033s_write_reg(RW_4F_TC_NOSR_CFG1,              0x22);
	hx9033s_write_reg(RW_50_TC_NOSR_CFG1,              0x01);
	hx9033s_write_reg(RW_52_TC_AVG_CFG0,               0x11);
	hx9033s_write_reg(RW_53_TC_AVG_CFG1,               0x11);
	hx9033s_write_reg(RW_54_TC_AVG_CFG2,               0x00);

	hx9033s_write_reg(RW_78_RANGE_7_0  ,               0x11); // 0=1.25pf; 1= 2.5pf;2 = 3.75pf  3= 5pf, 4 = 6.25pf
	hx9033s_write_reg(RW_79_RANGE_9_8  ,               0x11); // modify wear ch, the one_offset_code must be modify
	hx9033s_write_reg(RW_7A_RANGE_18_16,               0x00);
	
	hx9033s_write_reg(RW_ED_DSP_CONFIG_CTRL1,				 0x80); //close rd_lock


	hx9033s_write_reg(RW_9A_LP_DIFF_CFG,          0x00);  // 0: lp， 1: diff

	hx9033s_write_reg(RW_9B_RAW_BL_CFG,          0x1F);  // 0:raw, 1: bl 

	hx9033s_write_reg(RW_A1_INTERRUPT_CFG1,            0x00);  //INT_RDY_EN
	hx9033s_write_reg(RW_A2_INTERRUPT_CFG2,            0x1F);	//INT_PROX_LOW1_EN
	hx9033s_write_reg(RW_A5_INTERRUPT_CFG5,            0x1F);	//INT_PROX_HIGH1_EN

	hx9033s_write_reg(RW_AC_BL_UP_CFG0,                RA_bl_up_alpha_ch0 | (RA_bl_up_alpha_ch1<<4));
	hx9033s_write_reg(RW_AD_BL_UP_CFG1,                RA_bl_up_alpha_ch2 | (RA_bl_up_alpha_ch3<<4));
	hx9033s_write_reg(RW_AE_BL_UP_CFG2,                RA_bl_up_alpha_ch4);
	hx9033s_write_reg(RW_B2_BL_DN_CFG2,                0x01); // touch glitch

	hx9033s_write_reg(RW_B5_LP_ALP_1_0_CFG ,               RA_lp_alpha_ch0 | (RA_lp_alpha_ch1<<4));
	hx9033s_write_reg(RW_B6_LP_ALP_3_2_CFG ,               RA_lp_alpha_ch2 | (RA_lp_alpha_ch3<<4));
	hx9033s_write_reg(RW_B7_LP_ALP_4_CFG ,               RA_lp_alpha_ch4);
	
	hx9033s_write_reg(RW_07_MOD_RST_CFG,		 0x4F); // settle rst
	hx9033s_write_reg(RW_E2_SAMPLE_NUM_7_0,				 	 0x08);
	hx9033s_write_reg(RW_E4_INTEGRATION_NUM_7_0,		 0x08);
	//hx9033s_write_reg(RW_7D_REG_TO_ANA2,				     0x01); //CS0 pull dn
	//hx9033s_write_reg(RW_7E_REG_TO_ANA3,				     0x01); //CS4 pull dn
	hx9033s_write_reg(RW_7C_DAC_SWAP_CFG,		 nv_dac_swap); // dac_swap ch3:0x08, ch1: 0x02


#if 0
	hx9033s_write_reg(RW_ED_DSP_CONFIG_CTRL1,				 0x90); //defatlt 0x90, diff_en_ch01(bit1), diff_en_ch23(bit2)
	if(1){
		hx9033s_write_reg(RW_B9_DSP_CONFIG_CTRL6,				 0x40); //CH0_COE
		hx9033s_write_reg(RW_BD_DSP_CONFIG_CTRL8,				 0x40); //CH2_COE
	}
	else{
		hx9033s_write_reg(RW_B9_DSP_CONFIG_CTRL6,				 0x40); //CH0_COE
		hx9033s_write_reg(RW_BD_DSP_CONFIG_CTRL8,				 0x40); //CH2_COE
	}
#endif
	
		hx90xx_config_low_thres(WEAR0,0x2); //low thres control bit: 00:1/2 01:3/4 10:7/8 11 1   
		hx90xx_config_low_thres(WEAR1,0x2);

	hx90xx_channel_config();																				 
#ifdef TYHX_TOUCH_FUNTION
		hx90xx_config_thres(TOUCH,touch_high_diff,0); //key0
#endif

	if (is_offset_manual == 0)
	{
		write_ok &= hx9033s_write_reg(RW_E8_DSP_CONFIG_CTRL0,((0x1<<WEAR0)|(0x1<<WEAR1))); //wear ch cap_ini_force_zero open,  
		write_ok &= hx9033s_write_reg(RW_95_DSP_CONFIG_CTRL5,0x10); //[0] ch4 ch cap_ini_force_zero_en
		write_ok &= hx9033s_write_reg(RW_EE_DSP_CONFIG_CTRL2,0x00); //ch0~3 prox_data_sel,default
		write_ok &= hx9033s_write_reg(RW_94_DSP_CONFIG_CTRL4,0x00); //[1:0] ch4 prox_data_sel,default
		
		write_ok &= hx9033s_write_reg(RW_56_OFFSET_CALI_CTRL,0x00);  //ch0 手动配置offset 使能 关闭
		hx90xx_config_thres(WEAR0,wear0_high_diff,0); // wear0
		hx90xx_config_thres(WEAR1,wear1_high_diff,0); // wear1


		write_ok &= hx9033s_write_reg(RW_03_CH_NUM_CFG,RA_ch_num_cfg);
		
		is_reset_check = 1;
		if (is_reset_check ==1)
		{
			//hx9033s_write_reg(RW_A1_INTERRUPT_CFG1,0x01<<REF0); // 打开ch0 wear channel 转换中断
		}
		
	}
	else
	{		
		write_ok &= hx9033s_write_reg(RW_E8_DSP_CONFIG_CTRL0,((0x1<<WEAR0)|(0x1<<WEAR1))); //wear ch cap_ini_force_zero open, 
		write_ok &= hx9033s_write_reg(RW_95_DSP_CONFIG_CTRL5,0x10); //[0] ch4 ch cap_ini_force_zero_en
		write_ok &= hx9033s_write_reg(RW_EE_DSP_CONFIG_CTRL2,(2<<(WEAR0<<1))|(2<<(WEAR1<<1))); //wear WEAR0 WEAR1 diff = lp
		write_ok &= hx9033s_write_reg(RW_94_DSP_CONFIG_CTRL4,0x00); //[1:0] ch4 prox_data_sel,default
		hx90xx_config_thres(WEAR0,wear0_high_diff,nv_base_data[0]); // wear0
		hx90xx_config_thres(WEAR1,wear1_high_diff,nv_base_data[1]); // wear1
		#if 0
		hx9033s_write_reg(PAGE0_ED_LPOUT_DELTA_CFG,			 ~((0x1<<WEAR0)|(0x1<<WEAR1)));  //1: close usefilter
		hx9033s_write_reg(PAGE1_A3_LP_OUT_DELTA_THRES_CH1_CFG0,			0xFF); 
		hx9033s_write_reg(PAGE1_A4_LP_OUT_DELTA_THRES_CH1_CFG1,			0xFF); 
		hx9033s_write_reg(PAGE1_A9_LP_OUT_DELTA_THRES_CH3_CFG0,			0xFF);  
		hx9033s_write_reg(PAGE1_AA_LP_OUT_DELTA_THRES_CH3_CFG1,			0xFF);  
		#endif
		write_ok &= hx9033s_write_reg(RW_03_CH_NUM_CFG,RA_ch_num_cfg);
		hx9033s_write_reg(RW_56_OFFSET_CALI_CTRL,RA_offset_cali_ch0|(RA_offset_cali_ch1<<1)|(RA_offset_cali_ch2<<2)|(RA_offset_cali_ch3<<3));  
		hx90xx_write_offset(nv_wear_offset[0], WEAR0); // 写ch0 offset
		hx90xx_write_offset(nv_wear_offset[1], WEAR1); // 写ch1 offset	
		
		is_reset_check = 1;
		if (is_reset_check ==1)
		{
			//hx9033s_write_reg(RW_A1_INTERRUPT_CFG1,0x01<<REF0); // 打开ch0 wear channel 转换中断
		}
		

	}
	
    delay_ms(50);//delay 50ms
	return write_ok;
}



void hx90xx_turn_on_ch(uint8_t ch_num)
{
	hx9033s_write_reg(RW_03_CH_NUM_CFG,ch_num);
}

void hx90xx_turn_off_ch(void)
{
		hx9033s_write_reg(RW_03_CH_NUM_CFG,0x00);
}

#ifdef TYHX_TOUCH_FUNTION
void hx90xx_touch_key_reset(void)
{
	touch_time_press_start = 0;
	touch_release_timer = 0;
	touch_key_count = 0;
	timer_count = 0;
	hx90xx_conv_int_off();
	LOG_PBUS(">>>hx90xx_touch_key_reset\r\n");
}

void hx90xx_touch_channel_on(void)
{
	uint8_t reg_0x03 = 0;
	reg_0x03=hx9033s_read_reg(RW_03_CH_NUM_CFG);
	reg_0x03 = reg_0x03|TKEY;
	hx9033s_write_reg(RW_03_CH_NUM_CFG,reg_0x03);
	
	LOG_PBUS(">>>hx90xx_touch_channel_on, 0x%x \r\n", reg_0x03);
}

void hx90xx_touch_channel_off(void)
{
	uint8_t reg_0x03 = 0;
	if (is_factory_mode == 0)
	{
		reg_0x03=hx9033s_read_reg(RW_03_CH_NUM_CFG);
		reg_0x03 = reg_0x03 & (~(TKEY));
		hx9033s_write_reg(RW_03_CH_NUM_CFG,reg_0x03);
	}
	LOG_PBUS(">>>hx90xx_touch_channel_off, 0x%x \r\n", reg_0x03);
}

void hx90xx_wearch_conv_int_on(void)
{
	uint8_t cov_int_reg = 0;

	cov_int_reg = hx9033s_read_reg(RW_A1_INTERRUPT_CFG1);
	cov_int_reg = cov_int_reg|(0x01<<WEAR0);
	hx9033s_write_reg(RW_A1_INTERRUPT_CFG1,cov_int_reg); // 打开佩戴转换 通道
	LOG_PBUS(">>>hx90xx_wearch_conv_int_on 0x%x, \r\n", hx9033s_read_reg(RW_A1_INTERRUPT_CFG1));

}

void hx90xx_wearch_conv_int_off(void)
{
	uint8_t cov_int_reg = 0;

	cov_int_reg = hx9033s_read_reg(RW_A1_INTERRUPT_CFG1);
	cov_int_reg = cov_int_reg&(~(0x01<<WEAR0));
	hx9033s_write_reg(RW_A1_INTERRUPT_CFG1,cov_int_reg); // 关闭转换中断
	LOG_PBUS(">>>hx90xx_wearch_conv_int_off 0x%x, \r\n", hx9033s_read_reg(RW_A1_INTERRUPT_CFG1));
}

void hx90xx_conv_int_on(void)
{
	uint8_t cov_int_reg = 0;
	if (is_conv_int_on == 0)
	{
		cov_int_reg = hx9033s_read_reg(RW_A1_INTERRUPT_CFG1);
		cov_int_reg = cov_int_reg|(0x01<<TOUCH);
		hx9033s_write_reg(RW_A1_INTERRUPT_CFG1,cov_int_reg); // 打开touch 转换通道
		LOG_PBUS(">>>hx90xx_conv_int_on 0x%x, \r\n", hx9033s_read_reg(RW_A1_INTERRUPT_CFG1));
	}
	is_conv_int_on = 1;
}

void hx90xx_conv_int_off(void)
{
	uint8_t cov_int_reg = 0;
	if (is_conv_int_on == 1)
	{
			cov_int_reg = hx9033s_read_reg(RW_A1_INTERRUPT_CFG1);
			cov_int_reg = cov_int_reg&(~(0x01<<TOUCH));
			hx9033s_write_reg(RW_A1_INTERRUPT_CFG1,cov_int_reg); // 关闭转换中断
		
		LOG_PBUS(">>>hx90xx_conv_int_off 0x%x, \r\n", hx9033s_read_reg(RW_A1_INTERRUPT_CFG1));
	}
	is_conv_int_on = 0;
}

uint8_t  long_count = 0;

void hx90xx_touch_call_reset(void)
{
	hx9033s_write_reg(RW_57_OFFSET_CALI_CTRL1,0x1<<TOUCH);
}

void hx90xx_long_touch_call_back(void)
{
	long_count++; // 计算多少s
	if (long_count == 4)
	{
		hx90xx_touch_call_reset();
	}
}
void hx90xx_long_touch_timer_start(void)
{
	long_count = 0;
	//启动定时器  1s 一次
}

void hx90xx_long_touch_timer_stop(void)
{
	long_count = 0;
	//停止定时器
}

uint8_t hx90xx_get_touch_motion(void)
{	
	int32_t diff = 0;
	int16_t lp = 0;
	int16_t bl = 0;	
	TOUCH_MOTION touch_motion = NO_MOTION;
	

	if (1) //(wear_status) // 佩戴状态下
	{
		if (timer_count)
		{
			timer_count++;
		}
		
		#if 1
		lp = hx90xx_read_diff_lp(TOUCH);
		bl = hx90xx_read_raw_bl(TOUCH);
		diff = lp - bl;
		LOG_PBUS(">>>TYtouch diff: %d,timer_count: %d, prox_status:%d\r\n", diff,timer_count,prox_status);
		#endif
		
		if ((prox_status&TKEY) != 0x00)
		{

			touch_status = 1;
			if (touch_status != pre_touch_status&&timer_count == 0)
			{
				hx90xx_conv_int_on();
				timer_count++;
				touch_time_press_start = timer_count; // 按键开始按下的初始时间大于0
				LOG_PBUS(">>>TYtouch prox_status:%d, touch_time_press_start:%d \r\n", prox_status,touch_time_press_start);
			}
			
		}else if ((prox_status&TKEY) == 0)
		{
			// release
			touch_status = 0;
			if (touch_status != pre_touch_status)
			{
				if (touch_time_press_start != 0)
				{
					touch_release_timer = timer_count;
					touch_key_count++;
				}
				else
				{
					hx90xx_touch_key_reset();

				}
				hx90xx_long_touch_timer_stop();
				LOG_PBUS(">>>TYtouch prox_release_status:%d, touch_time_press_start:%d,touch_release_timer:%d,touch_key_count:%d \r\n", prox_status,touch_time_press_start,touch_release_timer,touch_key_count);
		
			}
		}

		
		if (touch_time_press_start > 0)
		{	
			
			// 长按处理  1500ms
			if (((timer_count - touch_release_timer) > 84)&&(touch_key_count == 0)) // 1个timer_count 18ms  1800ms
			{
				// 长按处理函数
				touch_motion = LONG_TOUCH;
				LOG_PBUS(">>>TYtouch long touch %d\n", touch_time_press_start);
				hx90xx_touch_key_reset();
				hx90xx_long_touch_timer_start();

			}
			
			//敲击处理, >350ms
		    if((timer_count - touch_release_timer > 19 /*|| (touch_key_count == 3)*/)&& (touch_status == 0)) //after touch release > 500ms or triple touch
			{
				  if(touch_key_count == 1) //one touch
				  {
						touch_motion = SINGLE_TOUCH;
					  LOG_PBUS(">>>TYtouch one touch timer:%d\n", timer_count);

				  }
				  else if(touch_key_count == 2) //twice touch
				  {
						touch_motion = DOUBLE_TOUCH;
						LOG_PBUS(">>>TYtouch twice touch timer:%d\n", timer_count);

				  }
				  else if(touch_key_count >= 3) //triple touch
				  {
					touch_motion = TRI_TOUCH;
					LOG_PBUS(">>>TYtouch triple touch timer: %d\n", timer_count);

				  }
				hx90xx_touch_key_reset();
			}
			
		}
	}
	else
	{
		// 关闭转换中断
		hx90xx_conv_int_off();
		timer_count = 0;
	}
	
	pre_touch_status = touch_status;
	
	return touch_motion;
}
#endif

void swap(int16_t *a, int16_t *b) 
{
	int32_t temp = *a;
	*a = *b;
	*b = temp;
}

// 窗口数据初始化
void window_init(int16_t *window, int16_t size, int16_t init_data) 
{
	int16_t  i = 0;
	for (i = 0; i<size; i++)
	{
		window[i] = init_data;
	}
}

// 排序
int16_t window_sort(int16_t *window, int16_t size) 
{
	int i = 0;
	for (int i = 0; i < size - 1; i++) {
			for (int j = 0; j < size - i - 1; j++) {
					if (window[j] > window[j + 1]) {
							swap(&window[j], &window[j + 1]);
					}
			}
	}
	
	return window[2];
}

int16_t WinFilter(uint8_t ch,int16_t pre_lp,int16_t raw_data, int16_t *window, int16_t size)
{
	int16_t  window_temp[WIN_SIZE] = {0};
	int16_t deglitch_raw = 0;
	int16_t  lp = 0;
	int16_t i = 0;
	for (i=0; i< size-1; i++)
	{
		window[i] = window[i+1];
		window_temp[i] = window[i];
	}
	window[size-1] = raw_data;
	window_temp[size-1] = window[size-1];
	window_sort(window_temp, size);
	//LOG_PBUS("raw_data%d, %d\r\n", ch,raw_data);
	//if (window_temp[size-1] - window_temp[0] > 300)
	if ((window_temp[0]<nv_base_data[ch]-100)&&(is_offset_manual == 1))
	{
		deglitch_raw = window_temp[size/2];
		//LOG_PBUS("window_temp%d, %d, %d, %d, %d\r\n", ch,window_temp[0],window_temp[1],window_temp[2],window_temp[3]);
	}
	else if ((window_temp[0]<min_glp[ch]-100)&&(is_offset_manual == 0))
	{
		deglitch_raw = window_temp[size/2];
	}
	else
	{
		deglitch_raw = raw_data;
	}
	lp = pre_lp/2 + deglitch_raw/2;
	//LOG_PBUS("deglitch_raw%d, %d, lp:%d\r\n", ch,deglitch_raw, lp);
	return lp;
}

	// adiff is unwear pad, tdiff is table pad, need to modify
uint8_t hx90xx_unwear_table_detect(int32_t adiff, int32_t tdiff,uint8_t status,int16_t adiff_thres,int16_t tdiff_thres)
{	
	int32_t i;
	int32_t rtn_ret = 1;
	static int32_t  min_tdiff = 10000;
	static int32_t  max_tdiff = 0;
		
	if (table_count > 37) // 5s 后退出
	{
		return status;
	}

	table_count++;
	
	for (i=0; i< ARRAY_NUM-1; i++)
	{
		arr_diff1[i] = arr_diff1[i+1];
	}
	arr_diff1[ARRAY_NUM-1] = tdiff;
	
//  custmer
	if (adiff < adiff_thres && tdiff<tdiff_thres)
	{
		table_touch_cnt++;
	}
	else{
		table_touch_cnt = 0;
	}
	
	if (table_touch_cnt > 14)// 2S
	{
			min_tdiff = 10000;
			max_tdiff = 0;
			for (i=0; i< ARRAY_NUM; i++)
			{
				if (arr_diff1[i] < min_tdiff)
				{
					min_tdiff = arr_diff1[i];
				}
				if (arr_diff1[i] > max_tdiff)
				{
					max_tdiff = arr_diff1[i];
				}
				
			LOG_PBUS("table ARRAY_NUM, diff[%d] = %d\r\n",i,arr_diff1[i]);
			}
			
			if ((max_tdiff - min_tdiff< 200)&&(max_tdiff - min_tdiff>0))
			{
					rtn_ret = 0;
			}
	
	}

	LOG_PBUS("table %d, diff0~1, %d, %d, table_touch_cnt,%d, rtn,%d\r\n",table_count,adiff, tdiff,table_touch_cnt, rtn_ret);
	return rtn_ret;
}

// 中断调用touch 事件
uint8_t hx90xx_report_touch_status(void)
{
	  prox_status = hx9033s_read_reg(RO_A8_PROX_STATUS);
#ifdef TYHX_TOUCH_FUNTION
		motion_status = hx90xx_get_touch_motion();
#endif

}

uint8_t hx90xx_report_diff_prox_status(void)
{
	int16_t raw[2] = {0};
	int32_t diff[2] = {0};
	
  prox_status = hx9033s_read_reg(RO_A8_PROX_STATUS);

	raw[0] = hx90xx_read_diff_lp(WEAR0);
	// raw[1] = hx90xx_read_diff_lp(WEAR1);
	if (start_updata_nv ==1)
	{
		glp[0] = WinFilter(0,glp[0],raw[0], window_raw0,WIN_SIZE);
		// glp[1] = WinFilter(1,glp[1],raw[1], window_raw1,WIN_SIZE);
		
		if (is_offset_manual ==1)
		{
			diff[0] = glp[0] - nv_base_data[0];
			// diff[1] = glp[1] - nv_base_data[1];
		}
		else
		{
			diff[0] = glp[0] - min_glp[0];
			// diff[1] = glp[1] - min_glp[1];
		}
		LOG_PBUS("raw:%d, glp:%d, diff:%d\r\n", raw[0], glp[0], diff[0]); 
		
		LOG_PBUS("0x6b: 0x%.2x, 0x%x\r\n", prox_status, wear_status); 
		
		if (diff[0] > wear0_high_diff)
		{
			unwear_cnt[0] = 0;
			wear_cnt[0]++;
			if (wear_cnt[0] >= CALCU_CNT)
			{
				wear_cnt[0] = CALCU_CNT;
				wear_s[0] = 1;
			}
		}

		// if (diff[1] > wear1_high_diff)
		// {
		// 	unwear_cnt[1] = 0;
		// 	wear_cnt[1]++;
		// 	if (wear_cnt[1] >= CALCU_CNT)
		// 	{
		// 		wear_cnt[1] = CALCU_CNT;
		// 		wear_s[1] = 1;
		// 	}
		// }
		
		if (diff[0] < wear0_low_diff)
		{
			wear_cnt[0] = 0;
			unwear_cnt[0]++;
			if (unwear_cnt[0] > CALCU_CNT)
			{
				unwear_cnt[0] = CALCU_CNT;
				wear_s[0] = 0;
			}
		}
		
		// if (diff[1] < wear1_low_diff)
		// {
		// 	wear_cnt[1] = 0;
		// 	unwear_cnt[1]++;
		// 	if (unwear_cnt[1] > CALCU_CNT)
		// 	{
		// 		unwear_cnt[1] = CALCU_CNT;
		// 		wear_s[1] = 0;
		// 	}
		// }

		// if ((wear_s[0] == 1) && (wear_s[1] == 1))
		// {
		// 	wear_status = 1;
		// }
		
		// if ((wear_s[0] == 0) && (wear_s[1] == 0))
		// {
		// 	wear_status = 0;
		// }
		wear_status = wear_s[0];
#if 0
		if ((wear_status ==1)&&(wear_s[0] == 0))
		{
			if (pre_wear_s[0] !=wear_s[0])
			{
				table_touch_cnt = 0;
				table_count = 0;
			}
			wear_status = hx90xx_unwear_table_detect(diff[0],diff[1], wear_status,250, 4000);
		}
		else if ((wear_status ==1)&&(wear_s[1] == 0))
		{
			if (pre_wear_s[1] !=wear_s[1])
			{
				table_touch_cnt = 0;
				table_count = 0;
			}
			wear_status = hx90xx_unwear_table_detect(diff[1],diff[0], wear_status,250, 4000);
		}
#endif
		
		if (wear_status != pre_wear_status)
		{
			if (wear_status == 1){
			//hx90xx_touch_channel_on();
				LOG_PBUS(">>> ear ch Wear\r\n");
			}else{
				//hx90xx_touch_channel_off();
				LOG_PBUS(">>>ear ch unWear\r\n");
			}
		}
		
	}	
	
	if (is_reset_check)
	{
//		int16_t  wear_lp0 = 0, wear_lp1 = 0;
//		uint16_t  cur_offset[2] = {0};
//		cur_offset[0] = hx90xx_read_offset(WEAR0);
//		cur_offset[1] = hx90xx_read_offset(WEAR1);
//		wear_lp0 = hx90xx_read_diff_lp(WEAR0);
//		wear_lp1 = hx90xx_read_diff_lp(WEAR1);
		if (reoffset_flag == 0) // 定时器时间159ms
		{
			uint8_t RA_scan_factor_ch0 = 4;  // odr = odr_base * scan_factor:  0=1; 1=2; 2=4;3=8, 4= 16,5=32,6=64,7=128;
			uint8_t RA_scan_factor_ch1 = 3;
			uint8_t RA_scan_factor_ch2 = 4; // 6: 6=64;  ref
			uint8_t RA_scan_factor_ch3 = 3;
			
			hx9033s_write_reg(RW_E6_CH10_SCAN_FACTOR, RA_scan_factor_ch0|(RA_scan_factor_ch1<<4));
			hx9033s_write_reg(RW_E7_CH32_SCAN_FACTOR, RA_scan_factor_ch2|(RA_scan_factor_ch3<<4));
			
			hx9033s_write_reg(RW_A1_INTERRUPT_CFG1,0x00); // 关闭转换中断
			is_reset_check = 0;
			glp[0] = raw[0];
			glp[1] = raw[1];
			min_glp[0] = glp[0]+ 50;
			min_glp[1] = glp[1]+ 50;
			window_init(window_raw0, WIN_SIZE,  raw[0]);
			window_init(window_raw1, WIN_SIZE,  raw[1]);
			start_updata_nv = 1;
			LOG_PBUS("start_updata_nv = 1,glp 0~1, %d, %d\r\n",glp[0],glp[1]);
		}	
		// LOG_PBUS("reoffset_flag:%d, cur_offset0:%d,cur_offset1:%d,wear_lp0:%d,wear_lp1:%d\r\n", reoffset_flag, cur_offset[0],cur_offset[1], wear_lp0, wear_lp1);
		reoffset_flag++;
	}

	
	if (timer_debug ==1)
	{
		hx90xx_timer_report_data();
	}
	
	hx90xx_update_min_nv_data(glp);
	hx90xx_update_temp_follow(glp,diff);
	pre_wear_status = wear_status;
	prox_status_pre = prox_status;
	pre_wear_s[0] = wear_s[0];
	pre_wear_s[1] = wear_s[1];
	

	if (is_offset_manual == 0&&start_updata_nv ==1&&is_factory_mode == 0&&reoffset_flag ==1)
	{
		// 停止定时器， 转绝对值
		reoffset_flag++;
		//hx90xx_update_nv_data();
	}

	return wear_status;
}

uint8_t hx90xx_get_wear_status(void)
{
	if (hx90xx_chip_id != DEVICE_ID_REV2)
	{
		wear_status = 1;
	}
	return wear_status;
}

uint8_t hx90xx_get_touch_status(void)
{
	return touch_status;
}

void hx90xx_update_temp_follow(int16_t *lp_data,int32_t *diff)
{
	uint16_t alpha = 256;
	if (start_updata_nv == 1)
	{
		if (diff[0] < 200)
		{
			if(is_offset_manual == 1){
				if (lp_data[0] > nv_base_data[0]+20){
					nv_base_data[0] = ((int32_t)nv_base_data[0]*128*(alpha-1)/alpha + (int32_t)lp_data[0]*128*1/alpha)/128;
					min_glp[0] = nv_base_data[0];
				}
			}else{
				if (lp_data[0] > nv_base_data[0]+20)
				{
					min_glp[0] = ((int32_t)min_glp[0]*128*(alpha-1)/alpha + (int32_t)lp_data[0]*128*1/alpha)/128;
				}
			}
		}
		
		if (diff[1] < 200)
		{
			if(is_offset_manual == 1){
				if (lp_data[1] > nv_base_data[1]+20)
				{
					nv_base_data[1] = ((int32_t)nv_base_data[1]*128*(alpha-1)/alpha + (int32_t)lp_data[1]*128*1/alpha)/128;
					min_glp[1] = nv_base_data[1];
				}
			}else{
					min_glp[1] = ((int32_t)min_glp[1]*128*(alpha-1)/alpha + (int32_t)lp_data[1]*128*1/alpha)/128;
			}
		}
		
	}
}

//更新最小值
void hx90xx_update_min_nv_data(int16_t *lp_data)
{
//	int16_t bl[3] = {0};
//	int16_t lp[3] = {0};
//	lp[0] = hx90xx_read_diff_lp(WEAR0);
//	lp[1] = hx90xx_read_diff_lp(WEAR1);
	//bl[0] = hx90xx_read_raw_bl(WEAR0);
	//bl[1] = hx90xx_read_raw_bl(WEAR1);
	uint8_t nv_change_flag = 0;
//	LOG_PBUS("90XX update_min_nv_data lp0~1,%d,%d, bl0~1, %d, %d, nv0~`: %d,%d,start_updata_nv:%d \r\n", lp[0],lp[1], bl[0],bl[1],nv_base_data[0], nv_base_data[1],start_updata_nv);
	if (start_updata_nv == 1&&is_factory_mode == 0&&lp_data[0]!=0&&lp_data[1]!=0)
	{
		// 取得最小信号量值
		if (min_glp[0] > lp_data[0]){
			min_glp[0] = lp_data[0];
		}
		
		if (min_glp[1] > lp_data[1]){
			min_glp[1] = lp_data[1];
		}
		
		if(min_glp[0] < nv_base_data[0]-50)
		{  
			nv_base_data[0] = min_glp[0];
			nv_change_flag = 1;
			
		}
		if(min_glp[1] < nv_base_data[1] - 50)
		{  
			nv_base_data[1] = min_glp[1];
			nv_change_flag = 1;
		}
		
		if (nv_change_flag == 1)
		{

			hx90xx_config_thres(WEAR0,wear0_high_diff,nv_base_data[0]); // wear0
			hx90xx_config_thres(WEAR1,wear1_high_diff,nv_base_data[1]); // wear1
			LOG_PBUS("90XX update_min_nv_data nv_base_data0: %d, nv_base_data1: %d\r\n", nv_base_data[0], nv_base_data[1]);
		}
	}
}

//拿出耳机关盖后调用一次
void hx90xx_update_nv_data(void)
{
  int16_t lp[3] = {0};
	int16_t  bl[3] = {0};	
	uint16_t offset[2] = {0};
	int16_t offset_adjust = 0;
	uint16_t base_data0 = 4000;
	uint16_t base_data1 = 4000;
	uint8_t  is_cover_in = 1;
	uint8_t nv_change_flag = 0;

	hx90xx_timer_stop();
	if (hx90xx_chip_id != DEVICE_ID_REV2 || is_factory_mode == 1)
	{
		return;
	}
	
	// 耳机在盖里面， 更新信号量
	if (is_cover_in == 1)
	{

		// 处于相对值模式， 说明工厂没有校准nv
		if (is_offset_manual == 0&&start_updata_nv == 1)
		{
			nv_wear_offset[0] = hx90xx_read_offset(WEAR0);
			nv_wear_offset[1] = hx90xx_read_offset(WEAR1);
			lp[0] = hx90xx_read_diff_lp(WEAR0);
			lp[1] = hx90xx_read_diff_lp(WEAR1);
			bl[0] = hx90xx_read_raw_bl(WEAR0);
			bl[1] = hx90xx_read_raw_bl(WEAR1);
			nv_base_data[0] = hx_min(bl[0],lp[0]);
			nv_base_data[1] = hx_min(bl[1],lp[1]);
			nv_base_data_backup[0] = nv_base_data[0];
			nv_base_data_backup[1] = nv_base_data[1];
			// 防止基准信号太小， 阈值设定出现小于0
			if (nv_base_data[0]  < base_data0)
			{
				offset_adjust = (base_data0-nv_base_data[0])/one_offset_code +1;
				if (offset_adjust > nv_wear_offset[0])
				{
					offset_adjust = nv_wear_offset[0];
				}
				if (nv_wear_offset[0] -offset_adjust >=0)
				{
					nv_wear_offset[0] = nv_wear_offset[0] -offset_adjust;
					nv_base_data[0] = nv_base_data[0] + one_offset_code*offset_adjust;
					nv_base_data_backup[0] = nv_base_data[0];
					
				}
				LOG_PBUS("90XX offset_adjust1: %d \r\n", offset_adjust);

			}

			
			if (nv_base_data[1]  < base_data1)
			{
				offset_adjust = (base_data1-nv_base_data[1])/one_offset_code +1;
				if (offset_adjust > nv_wear_offset[1])
				{
					offset_adjust = nv_wear_offset[1];
				}
				
				if (nv_wear_offset[1] -offset_adjust >=0)
				{
					nv_wear_offset[1] = nv_wear_offset[1] -offset_adjust;
					nv_base_data[1] = nv_base_data[1] + one_offset_code*offset_adjust;
					nv_base_data_backup[1] = nv_base_data[1];
				}
			
				LOG_PBUS("90XX offset_adjust2: %d \r\n", offset_adjust);

			}

			LOG_PBUS("90XX is_offset_manual = 0  offset0: %d, offset1: %d,data0: %d, data1: %d,data_backup0: %d, data_backup1: %d\r\n", nv_wear_offset[0], nv_wear_offset[1], 
			nv_base_data[0],nv_base_data[1],nv_base_data_backup[0],nv_base_data_backup[1]);
			if (nv_wear_offset[0] >=0&&nv_wear_offset[0]<128&&nv_wear_offset[1]>=0&&nv_wear_offset[1]<128)
			{
				hx90xx_write_nv(nv_wear_offset[0], nv_wear_offset[1], nv_base_data[0], nv_base_data[1],nv_dac_swap);
				hx90xx_init(); // 复位
			}
			return;
		}

		if (is_offset_manual == 1&&start_updata_nv == 1)
		{
				
			offset[0] = hx90xx_read_offset(WEAR0);
			offset[1] = hx90xx_read_offset(WEAR1);
			if (offset[0] != nv_wear_offset[0] || offset[1] != nv_wear_offset[1])
			{
				LOG_PBUS("90XX error offset[0]:%d  nv_wear_offset[0]: %d ,offset[1]:%d  nv_wear_offset[1]: %d \r\n", offset[0], nv_wear_offset[0],offset[1], nv_wear_offset[1]);
				hx90xx_init(); // 复位
				return;
			}
			
			lp[0] = hx90xx_read_diff_lp(WEAR0);
			lp[1] = hx90xx_read_diff_lp(WEAR1);

			if (lp[0] < -16384 ||lp[0] > 20000 || lp[1] < -16384 ||lp[1] > 20000)
			{
				// 数据导常， 转为相对值模式
				LOG_PBUS("90XX data error: reset to is_offset_manual == 0\r\n");
				nv_wear_offset[0] = 0xFFFF;
				nv_wear_offset[1] = 0xFFFF;
				hx90xx_write_nv(nv_wear_offset[0], nv_wear_offset[1], nv_base_data[0], nv_base_data[1],nv_dac_swap);
				hx90xx_init(); // 复位
				return;
			}	

			nv_wear_offset[0] = offset[0];
			nv_wear_offset[1] = offset[1];


			LOG_PBUS("90XX reset first offset0: %d, offset1: %d,data0: %d, data1: %d,data_backup0: %d, data_backup1: %d\r\n", nv_wear_offset[0], nv_wear_offset[1], 
			nv_base_data[0],nv_base_data[1],nv_base_data_backup[0],nv_base_data_backup[1]);

			// 取得最小信号量值
			nv_base_data[0] = hx_min(min_glp[0],lp[0]);
			nv_base_data[1] = hx_min(min_glp[1],lp[1]);
			
			// íùé?μ÷
			if ((hx_abs(nv_base_data_backup[0] - nv_base_data[0]))> 50 || (hx_abs(nv_base_data_backup[1] - nv_base_data[1])> 50))
			{
				nv_change_flag = 1;
			}
			if ((nv_base_data[0] + wear0_low_diff/2 < 0)|| (nv_base_data[1] + wear1_low_diff/2 < 0))
			{
				nv_change_flag = 1;
			}
			

			LOG_PBUS("90XX nv_change_flag : %d, nv_base_data_backup[0]: %d,nv_base_data_backup[1]: %d\r\n", nv_change_flag, nv_base_data_backup[0], nv_base_data_backup[1]);
			if (nv_change_flag == 1)
			{
				if (nv_base_data[0] > nv_base_data_backup[0] + 100)
				{
					nv_base_data[0] = nv_base_data_backup[0] + (nv_base_data[0] - nv_base_data_backup[0])/2;
					
					//LOG_PBUS("nv_base_data_backup[0] > nv_base_data[0] \r\n");
				}
				if (nv_base_data[1] > nv_base_data_backup[1] + 100)
				{
					nv_base_data[1] = nv_base_data_backup[1] + (nv_base_data[1] - nv_base_data_backup[1])/2;
					//LOG_PBUS("nv_base_data_backup[1] > nv_base_data[1] \r\n");
				}
				
					
				// 防止基准信号太小， 阈值设定出现小于0
				if (nv_base_data[0]  < base_data0/3)
				{
					offset_adjust = (base_data0-nv_base_data[0])/one_offset_code +1;
					if (nv_wear_offset[0] -offset_adjust >=0)
					{
						nv_wear_offset[0] = nv_wear_offset[0] -offset_adjust;
						nv_base_data[0] = nv_base_data[0] + one_offset_code*offset_adjust;
						nv_base_data_backup[0] = nv_base_data[0];
						
					}
					LOG_PBUS("90XX offset_adjust1: %d \r\n", offset_adjust);

				}

				if (nv_base_data[1]< base_data1/3)
				{
					offset_adjust = (base_data1-nv_base_data[1])/one_offset_code +1;
					if (nv_wear_offset[1] -offset_adjust >=0)
					{
						nv_wear_offset[1] = nv_wear_offset[1] -offset_adjust;
						nv_base_data[1] = nv_base_data[1] + one_offset_code*offset_adjust;
						nv_base_data_backup[1] = nv_base_data[1];
					}
				
					LOG_PBUS("90XX offset_adjust2: %d \r\n", offset_adjust);

				}
//				// 这段代码根据整机加上
//				if (lp[0] - nv_base_data[0] >2000)
//				{
//					nv_base_data[0] = lp[0] - 1000; // 盒内信号量的80%
//				}
//				if (lp[1] - nv_base_data[1] >2000)
//				{
//					nv_base_data[1] = lp[1] - 1000; // 盒内信号量的80%
//				}
				
				LOG_PBUS("90XX reset after  nv_change:%d,offset0: %d, offset1: %d,data0: %d, data1: %d,data_backup0: %d, data_backup1: %d\r\n",nv_change_flag, nv_wear_offset[0], nv_wear_offset[1], 
				nv_base_data[0],nv_base_data[1],nv_base_data_backup[0],nv_base_data_backup[1],nv_dac_swap);
					

				// 复位
				//hx90xx_config_thres(CH0,ch0_high_diff, ch0_low_diff,nv_base_data[0]); // wear0
				//hx90xx_config_thres(CH1,ch1_high_diff, ch1_low_diff,nv_base_data[1]); // wear1
				hx90xx_write_nv(nv_wear_offset[0], nv_wear_offset[1], nv_base_data[0], nv_base_data[1],nv_dac_swap);
				hx90xx_init(); // 复位
			}
		}
	}
	
}



//spp 或是串口打印调用， 可设定1s或是500ms打印一次
void hx90xx_timer_report_data(void)
{
	int16_t raw[5] = {0};
	int32_t diff[5] = {0};
	int16_t lp[5] = {0};
	int16_t bl[5] = {0};	
	uint16_t offset[5] = {0};
//    int16_t temp = 0;
	static uint32_t timer_index  = 0;
	uint8_t reg_b9 = 0;
	uint8_t reg_bd = 0;
	timer_index++;
	
	// 600ms 打印一次
	if (timer_index%4 != 0)
	{
		return;
	}
  prox_status = hx9033s_read_reg(RO_A8_PROX_STATUS);
	raw[0] = hx90xx_read_diff_lp(WEAR0);
	raw[1] = hx90xx_read_diff_lp(WEAR1);
	lp[0] = glp[0]; //hx90xx_read_diff_lp(WEAR0);
	lp[1] = glp[1]; //hx90xx_read_diff_lp(WEAR1);
	lp[2] = hx90xx_read_diff_lp(REF0);
	lp[3] = hx90xx_read_diff_lp(REF1);	
	//bl[0] = hx90xx_read_raw_bl(WEAR0);
	//bl[1] = hx90xx_read_raw_bl(WEAR1);
	//bl[2] = hx90xx_read_raw_bl(REF0);
	//bl[3] = hx90xx_read_raw_bl(REF1);

#ifdef TYHX_TOUCH_FUNTION
	lp[4] = hx90xx_read_diff_lp(TOUCH);
	bl[4] = hx90xx_read_raw_bl(TOUCH);
	diff[4] = lp[4]- bl[4];
#endif

	offset[0] = hx90xx_read_offset(WEAR0);
	offset[1] = hx90xx_read_offset(WEAR1);
	offset[2] = hx90xx_read_offset(REF0);
	offset[3] = hx90xx_read_offset(REF1);
#ifdef TYHX_TOUCH_FUNTION
	offset[4] = hx90xx_read_offset(TOUCH);
#endif
	reg_b9 = hx9033s_read_reg(RW_B9_DSP_CONFIG_CTRL6);
	reg_bd = hx9033s_read_reg(RW_BD_DSP_CONFIG_CTRL8);
		
	if (is_offset_manual == 1)
	{
		diff[0] = lp[0] - nv_base_data[0];
		diff[1] = lp[1] - nv_base_data[1];
		if ((timer_index%40)== 0)
		{
			LOG_PBUS("id: 0x%x, offset0-3 %d,%d,%d,%d H0,%d,L0,%d H1,%d,L1,%d,nvdata0~1,%d, %d, reg_b9_bd, %d,%d\r\n", hx90xx_chip_id,offset[0],offset[1],offset[2],offset[3],
			wear0_high_diff,wear0_low_diff, wear1_high_diff,wear1_low_diff, nv_base_data[0], nv_base_data[1],reg_b9,reg_bd);
		}else
		{
			LOG_PBUS("HX,m=1,prox,0x%x,wear,%d,diff0,%d,diff1,%d, lp0,%d,lp1,%d,raw0~1,%d,%d\r\n",prox_status,wear_status,diff[0],diff[1],lp[0],lp[1],raw[0], raw[1]); 		
		}

	}
	else
	{
		
		diff[0] = lp[0] - min_glp[0];
		diff[1] = lp[1] - min_glp[1];
		
		if ((timer_index%40)== 0)
		{
			LOG_PBUS("id: 0x%x, offset0-offset1-offset2 %d,%d,%d,%d, H0,%d, L0,%d,H1,%d,L1,%d,min_glp[0~1],%d, %d,reg_b9_bd, %d,%d\r\n", hx90xx_chip_id,offset[0],offset[1],offset[2],offset[3],
			wear0_high_diff, wear0_low_diff,wear1_high_diff, wear1_low_diff,min_glp[0], min_glp[1],reg_b9,reg_bd);
		}else
		{

			LOG_PBUS("HX,m=0,prox,0x%x,wear,%d,diff0,%d,diff1,%d,lp0~1,%d,%d,min_p0~1,%d,%d ,raw0~1,%d,%d\r\n",prox_status,wear_status,diff[0],diff[1],lp[0], lp[1],min_glp[0],min_glp[1],raw[0], raw[1]); 
		}
	}


}



// 写nv接口
void hx90xx_write_nv(uint16_t offset0, uint16_t offset1, int16_t wear_data0, int16_t wear_data1,uint8_t dac_swap)
{
	nv_wear_offset[0] = offset0; 
	nv_wear_offset[1] = offset1;
	nv_base_data[0] = wear_data0;
	nv_base_data[1] = wear_data1;
	nv_base_data_backup[0] = nv_base_data[0];
	nv_base_data_backup[1] = nv_base_data[1];

	nv_dac_swap = dac_swap;
	LOG_PBUS("90XX write_nv offset0: %d, offset1: %d,data0: %d, data1: %d,dac_swap: %d \r\n", nv_wear_offset[0], nv_wear_offset[1], nv_base_data[0],nv_base_data[1],nv_dac_swap);
}



// 读nv接口
void hx90xx_read_nv()
{
	nv_wear_offset[0] = 0xFFFF; //181  0xFFFF  2120
	nv_wear_offset[1] = 0xFFFF; //181  0xFFFF  810
	nv_base_data[0] = 0;
	nv_base_data[1] = 0;
	nv_base_data_backup[0] = nv_base_data[0];
	nv_base_data_backup[1] = nv_base_data[1];


	// 开盖时如采用相对值模式配置force_to_auto_mode =1 
	if (nv_wear_offset[0] == 0 && nv_base_data[0] == 0&&nv_wear_offset[1] == 0 && nv_base_data[1] == 0)
	{
		nv_wear_offset[0] = 0xFFFF;
	}
	if (nv_wear_offset[0] == 0xFFFF || force_to_auto_mode == 1 )
	{
		is_offset_manual = 0;
	}
	else
	{
		is_offset_manual = 1;
	}
	LOG_PBUS("hx90xx_read_nv offset0: %d, offset1: %d,data0: %d, data1: %d,nv_dac_swap: %d \r\n", nv_wear_offset[0], nv_wear_offset[1], nv_base_data[0],nv_base_data[1],nv_dac_swap);

}

bool hx90xx_init(void)
{
    int32_t i = 0;
    bool ret = false;
    hx90xx_power_restart();
		hx90xx_read_nv();
	
    for (i = 0; i<5; i++)
    {
		hx90xx_chip_id = hx90xx_check_device_id();

		// LOG_PBUS("init device_id is : 0x%.2x\r\n", hx90xx_chip_id);
		start_updata_nv = 0; // 防止更新数据出错
        if (hx90xx_chip_id == DEVICE_ID_REV2)
        {
            hx90xx_reg_init();
            ret = true;
            break;
        }
    }

	prox_status = 0; 
	prox_status_pre = 0;
	wear_status = 0;
	pre_wear_status = 0;
	touch_status = 0;
	pre_touch_status = 0;
	reoffset_flag = 0;

#ifdef TYHX_TOUCH_FUNTION
	timer_count = 0;
	motion_status = 0;
	hx90xx_touch_key_reset();
#endif
	
	if (ret)
	{
		hx90xx_timer_start();
	}
	//hx90xx_spp_open_log();
	
	if(timer_start) {
		LOG_PBUS("init hx90xx success\r\n");
	}
    return ret;
}

// 对空时初始化采用自动校准模式， 获取offset 值
// 进入工厂测试模式请把 is_factory_mode =1
// 退出 工厂测试模式请把 is_factory_mode = 0;
void hx90xx_factory_cali_unwear_init(void)
{
	uint16_t offset0 = 0;
	uint16_t offset1 = 0;
	int16_t lp[3] = {0};
	uint8_t  offset_adust = 0;  
	is_factory_mode =1;
	hx90xx_write_nv(0,0,0,0,0);
	hx90xx_init();
#ifdef TYHX_TOUCH_FUNTION
  hx90xx_touch_channel_on();
#endif
	delay_ms(300);
	offset0 = hx90xx_read_offset(WEAR0);
	offset1 = hx90xx_read_offset(WEAR1);
	offset_adust = 6;
	if (offset_adust > offset0)
	{
		offset_adust = offset0;
	}
	nv_wear_offset[0] = offset0 - offset_adust;
	
	offset_adust = 6;
	if (offset_adust > offset1)
	{
		offset_adust = offset1;
	}
	nv_wear_offset[1] = offset1 - offset_adust;
	nv_base_data[0] = 100;
	nv_base_data[1] = 100;
	
	hx90xx_write_nv(nv_wear_offset[0], nv_wear_offset[1], nv_base_data[0], nv_base_data[1],nv_dac_swap);

	LOG_PBUS("cali_unwear_init,nv_wear_offset[0~1], %d, %d \r\n", nv_wear_offset[0],nv_wear_offset[1]);

	hx90xx_init();
	delay_ms(500);

	offset0 = hx90xx_read_offset(WEAR0);
	offset1 = hx90xx_read_offset(WEAR1);

	lp[0] = hx90xx_read_diff_lp(WEAR0);
	lp[1] = hx90xx_read_diff_lp(WEAR1);
	//lp[2] = hx90xx_read_diff_lp(TOUCH);	

	// 对空时保存数据 
	nv_wear_offset[0] = offset0;
	nv_wear_offset[1] = offset1;
	nv_base_data[0] = lp[0];
	nv_base_data[1] = lp[1];
	min_glp[0] = lp[0];
	min_glp[1] = lp[1];
	hx90xx_write_nv(nv_wear_offset[0], nv_wear_offset[1], nv_base_data[0], nv_base_data[1],nv_dac_swap);
	hx90xx_config_thres(WEAR0,wear0_high_diff,nv_base_data[0]); // wear0
	hx90xx_config_thres(WEAR1,wear1_high_diff,nv_base_data[1]); // wear1
	LOG_PBUS("cali_unwear_init,nv_wear_offset[0~1], %d, %d, data0~1, %d, %d\r\n", nv_wear_offset[0],nv_wear_offset[1],nv_base_data[0], nv_base_data[1]);
	//hx90xx_init();

}

//延时后读取数据
void hx90xx_factory_cali_read_data(void)
{
	uint16_t offset[8] = {0};

	int32_t diff[8] = {0};
	int16_t bl[8] = {0};
	int16_t lp[8] = {0};
	
	uint8_t device_id = 0x00;
	uint8_t wear_flag = 0;
	//int16_t temp = 0;

	
  device_id = hx9033s_read_reg(RO_01_DEVICE_ID);
	
	lp[0] = hx90xx_read_diff_lp(WEAR0);
	lp[1] = hx90xx_read_diff_lp(WEAR1);
	//bl[0] = hx90xx_read_raw_bl(WEAR0);
	//bl[1] = hx90xx_read_raw_bl(WEAR1);
	diff[0] = lp[0] - min_glp[0];
	diff[1] = lp[1] - min_glp[1];

	
	offset[0] = hx90xx_read_offset(WEAR0);
	offset[1] = hx90xx_read_offset(WEAR1);
	wear_flag = hx90xx_get_wear_status();

#ifdef  TYHX_TOUCH_FUNTION
	offset[4] = hx90xx_read_offset(TOUCH);
	lp[4] = hx90xx_read_diff_lp(TOUCH);
	bl[4] = hx90xx_read_raw_bl(TOUCH);
	diff[4] = lp[4]-bl[4];
#endif
	
	if (is_offset_manual ==1)
	{
		diff[0] = lp[0]- nv_base_data[0];
		diff[1] = lp[1] - nv_base_data[1];
	}
	
  LOG_PBUS("HX90XX id: 0x%x, wear_flag:%d, is_offset_manual:%d,nv_base_data[0~1]:%d, %d,offset0-1, %d,%d, wdiff[0],%d, wdiff[1],%d\r\n", device_id,wear_flag,is_offset_manual,nv_base_data[0],nv_base_data[1],offset[0],offset[1],diff[0],diff[1]);
  LOG_PBUS("HX90XX wear read_data lp[0], %d, lp[1],%d,bl[0], %d, bl[1],%d \r\n", lp[0],lp[1], bl[0],bl[1]);
	
	#ifdef  TYHX_TOUCH_FUNTION
	 LOG_PBUS("HX90XX touch read_data offset %d, diff[4], %d \r\n", offset[4], diff[4]);
	#endif
}


// 写校准系数接口
void hx90xx_write_cali_K_nv(uint16_t Wcali_K0, uint16_t Wcali_K1, uint16_t Tcali_K2)
{
	nv_wear_K0 = Wcali_K0;
	nv_wear_K1 = Wcali_K1;
	nv_touch_K2 = Tcali_K2;
	

	LOG_PBUS("90XX write_cali_K_nv: K0: %d, K1: %d, K2: %d\r\n", nv_wear_K0, nv_wear_K1, nv_touch_K2);
}



// 读nv接口
void hx90xx_read_cali_K_nv(void)
{
	nv_wear_K0 = 100;
	nv_wear_K1 = 100;
	nv_touch_K2 = 100;
	
	touch_high_diff = TOUCH_HIGH_DIFF*nv_touch_K2/100;
	wear0_high_diff = wear0_high_diff;
	wear1_high_diff = wear1_high_diff;
	LOG_PBUS("90XX read_cali_K: K0: %d, K1: %d, K2: %d, diff: %d, %d, %d\r\n", nv_wear_K0, nv_wear_K1, nv_touch_K2, touch_high_diff, wear0_high_diff, wear1_high_diff);
}

void hx90xx_factory_cali_save_K(uint16_t Wcali_K0, uint16_t Wcali_K1, uint16_t Tcali_K2)
{
	hx90xx_write_cali_K_nv(Wcali_K0, Wcali_K1, Tcali_K2);
	// 结束测试， 重新初始化
	is_factory_mode =0;
	hx90xx_init();
}

void hx90xx_factory_cali_save_nv(void)
{
	// 对空的时候才需要保存nv 
	hx90xx_write_nv(nv_wear_offset[0], nv_wear_offset[1], nv_base_data[0], nv_base_data[1],nv_dac_swap);
	

}

