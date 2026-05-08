/**
  ******************************************************************************
  * @file           : hx90xx.h  
  * @version        : v1.0
  * @brief          : 
  ******************************************************************************
  *
  * 
  *
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HX90XX_H_
#define __HX90XX_H_

/* Includes ------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>


#define  hx9033s_ADDR  (0x18)

// #define __TIMER_DEBUG__

// #define TYHX_TOUCH_FUNTION

typedef enum {
	NO_MOTION = 0,
	LONG_TOUCH = 3,
	SINGLE_TOUCH = 4,
	DOUBLE_TOUCH = 5,
	TRI_TOUCH = 6,
} TOUCH_MOTION;

#define CH0   0
#define CH1   1
#define CH2   2
#define CH3   3
#define CH4   4

#define CH0_OFFSET   0
#define CH1_OFFSET   1
#define CH2_OFFSET   2
#define CH3_OFFSET   3
#define CH4_OFFSET   4

#define RAW0   0
#define RAW1   1
#define RAW2   2
#define RAW3   3
#define RAW4   4

#define BL0   0
#define BL1   1
#define BL2   2
#define BL3   3
#define BL4   4

#define LP0   0
#define LP1   1
#define LP2   2
#define LP3   3
#define LP4   4

#define DIFF0   0
#define DIFF1   1
#define DIFF2   2
#define DIFF3   3
#define DIFF4   4
                       
#define RO_01_DEVICE_ID                      0x01
#define RO_02_VERION_ID                      0x02
#define RW_03_CH_NUM_CFG                     0x03
#define RW_04_GLOBAL_CTRL0                   0x04
#define RW_05_GLOBAL_CTRL1                   0x05
#define RW_06_GLOBAL_CTRL2                   0x06
#define RW_07_MOD_RST_CFG                    0x07
#define RW_08_SHIELD_EN                      0x08
#define RW_09_FORCE_NO_UP_CFG                0x09
#define RW_0A_INTERRUPT_CFG                  0x0A
#define RW_0B_DITHER_CFG                     0x0B
#define RO_0D_RAW_BL_CH0_0                   0x0D
#define RO_0E_RAW_BL_CH0_1                   0x0E
#define RO_0F_RAW_BL_CH0_2                   0x0F
#define RO_10_RAW_BL_CH1_0                   0x10
#define RO_11_RAW_BL_CH1_1                   0x11
#define RO_12_RAW_BL_CH1_2                   0x12
#define RO_13_RAW_BL_CH2_0                   0x13
#define RO_14_RAW_BL_CH2_1                   0x14
#define RO_15_RAW_BL_CH2_2                   0x15
#define RO_16_RAW_BL_CH3_0                   0x16
#define RO_17_RAW_BL_CH3_1                   0x17
#define RO_18_RAW_BL_CH3_2                   0x18
#define RO_19_RAW_BL_CH4_0                   0x19
#define RO_1A_RAW_BL_CH4_1                   0x1A
#define RO_1B_RAW_BL_CH4_2                   0x1B
#define RW_1C_ANALOG_MEM0_WRDATA_7_0         0x1C
#define RW_1D_ANALOG_MEM0_WRDATA_15_8        0x1D
#define RW_1E_ANALOG_MEM0_WRDATA_23_16       0x1E
#define RW_1F_ANALOG_MEM0_WRDATA_31_24       0x1F
#define RW_20_ANALOG_PWE_PULSE_CYCLE7_0      0x20
#define RW_21_ANALOG_PWE_PULSE_CYCLE12_8     0x21
#define RW_22_ANALOG_MEM_GLOBAL_CTRL         0x22
#define RO_23_DEBUG_MEM_ADC_FSM              0x23
#define RW_24_ANALOG_MEM_GLOBAL_CTRL1        0x24
#define RO_25_LP_DIFF_CH0_0                  0x25
#define RO_26_LP_DIFF_CH0_1                  0x26
#define RO_27_LP_DIFF_CH0_2                  0x27
#define RO_28_LP_DIFF_CH1_0                  0x28
#define RO_29_LP_DIFF_CH1_1                  0x29
#define RO_2A_LP_DIFF_CH1_2                  0x2A
#define RO_2B_LP_DIFF_CH2_0                  0x2B
#define RO_2C_LP_DIFF_CH2_1                  0x2C
#define RO_2D_LP_DIFF_CH2_2                  0x2D
#define RO_2E_LP_DIFF_CH3_0                  0x2E
#define RO_2F_LP_DIFF_CH3_1                  0x2F
#define RO_30_LP_DIFF_CH3_2                  0x30
#define RO_31_LP_DIFF_CH4_0                  0x31
#define RO_32_LP_DIFF_CH4_1                  0x32
#define RO_33_LP_DIFF_CH4_2                  0x33
#define RO_34_CAP_INI_CH0_2                  0x34
#define RO_35_CAP_INI_CH1_2                  0x35
#define RO_36_CAP_INI_CH2_2                  0x36
#define RO_37_CAP_INI_CH3_2                  0x37
#define RO_38_CAP_INI_CH4_2                  0x38
#define RW_39_LP_OUT_DELTA_THRES_CH1_CFG0    0x39
#define RW_3A_LP_OUT_DELTA_THRES_CH1_CFG1    0x3A
#define RW_3B_LP_OUT_DELTA_THRES_CH3_CFG0    0x3B
#define RW_3C_LP_OUT_DELTA_THRES_CH3_CFG1    0x3C
#define RW_3D_LP_OUT_DELTA_THRES_CH4_CFG0    0x3D
#define RW_3E_LP_OUT_DELTA_THRES_CH4_CFG1    0x3E
#define RW_40_PRF_CFG                        0x40
#define RW_41_PROX_LOW_THRES1_CFG0           0x41
#define RW_42_PROX_LOW_THRES1_CFG1           0x42
#define RW_43_PROX_LOW_THRES1_CFG2           0x43
#define RW_44_PROX_LOW_THRES1_CFG3           0x44
#define RW_45_PROX_LOW_THRES1_CFG4           0x45
#define RW_4E_TC_NOSR_CFG0                   0x4E
#define RW_4F_TC_NOSR_CFG1                   0x4F
#define RW_50_TC_NOSR_CFG1                   0x50
#define RW_52_TC_AVG_CFG0                    0x52
#define RW_53_TC_AVG_CFG1                    0x53
#define RW_54_TC_AVG_CFG2                    0x54
#define RW_56_OFFSET_CALI_CTRL               0x56
#define RW_57_OFFSET_CALI_CTRL1              0x57
#define RW_58_OFFSET_DAC0_0                  0x58
#define RW_59_OFFSET_DAC0_1                  0x59
#define RW_5A_OFFSET_DAC1_0                  0x5A
#define RW_5B_OFFSET_DAC1_1                  0x5B
#define RW_5C_OFFSET_DAC2_0                  0x5C
#define RW_5D_OFFSET_DAC2_1                  0x5D
#define RW_5E_OFFSET_DAC3_0                  0x5E
#define RW_5F_OFFSET_DAC3_1                  0x5F
#define RW_60_OFFSET_DAC4_0                  0x60
#define RW_61_OFFSET_DAC4_1                  0x61
#define RW_68_CH0_CFG_0                      0x68
#define RW_69_CH0_CFG_1                      0x69
#define RW_6A_CH1_CFG_0                      0x6A
#define RW_6B_CH1_CFG_1                      0x6B
#define RW_6C_CH2_CFG_0                      0x6C
#define RW_6D_CH2_CFG_1                      0x6D
#define RW_6E_CH3_CFG_0                      0x6E
#define RW_6F_CH3_CFG_1                      0x6F
#define RW_70_CH4_CFG_0                      0x70
#define RW_71_CH4_CFG_1                      0x71
#define RW_78_RANGE_7_0                      0x78
#define RW_79_RANGE_9_8                      0x79
#define RW_7A_RANGE_18_16                    0x7A
#define RW_7C_DAC_SWAP_CFG                   0x7C
#define RO_83_TC_FSM                         0x83
#define RO_84_FLAG_RD                        0x84
#define RO_85_CONV_TIMEOUT_CNT               0x85
#define RO_87_PROX_STATUS_RD1                0x87
#define RO_88_PROX_STATUS_RD2                0x88
#define RO_89_PROX_STATUS_RD3                0x89
#define RW_8A_INT_WIDTH_CFG0                 0x8A
#define RW_8B_INT_WIDTH_CFG1                 0x8B
#define RO_8C_INT_STATE_RD0                  0x8C
#define RO_8D_INT_STATE_RD1                  0x8D
#define RO_8E_INT_STATE_RD2                  0x8E
#define RO_8F_INT_STATE_RD3                  0x8F
#define RO_90_INT_STATE_RD4                  0x90
#define RO_91_INT_STATE_RD5                  0x91
#define RO_92_INT_STATE_RD6                  0x92
#define RO_93_INT_STATE_RD7                  0x93
#define RW_94_DSP_CONFIG_CTRL4               0x94
#define RW_95_DSP_CONFIG_CTRL5               0x95
#define RW_96_DSP_CONFIG_CTRL11              0x96
#define RW_97_BL_IN_NO_UP_NUM_SEL0           0x97
#define RW_98_BL_IN_NO_UP_NUM_SEL1           0x98
#define RW_99_BL_IN_NO_UP_NUM_SEL2           0x99
#define RW_9A_LP_DIFF_CFG                    0x9A
#define RW_9B_RAW_BL_CFG                     0x9B
#define RW_9C_CALI_DIFF_CFG                  0x9C
#define RW_9D_CAP_INI_CFG                    0x9D
#define RW_A1_INTERRUPT_CFG1                 0xA1
#define RW_A2_INTERRUPT_CFG2                 0xA2
#define RW_A3_INTERRUPT_CFG3                 0xA3
#define RW_A4_INTERRUPT_CFG4                 0xA4
#define RW_A5_INTERRUPT_CFG5                 0xA5
#define RW_A6_INTERRUPT_CFG6                 0xA6
#define RW_A7_INTERRUPT_CFG7                 0xA7
#define RO_A8_PROX_STATUS                    0xA8
#define RW_AB_PROX_INT_CFG                   0xAB
#define RW_AC_BL_UP_CFG0                     0xAC
#define RW_AD_BL_UP_CFG1                     0xAD
#define RW_AE_BL_UP_CFG2                     0xAE
#define RW_B0_BL_DN_CFG0                     0xB0
#define RW_B1_BL_DN_CFG1                     0xB1
#define RW_B2_BL_DN_CFG2                     0xB2
#define RW_B4_NDL_DLY_4_CFG                  0xB4
#define RW_B5_LP_ALP_1_0_CFG                 0xB5
#define RW_B6_LP_ALP_3_2_CFG                 0xB6
#define RW_B7_LP_ALP_4_CFG                   0xB7
#define RW_B9_DSP_CONFIG_CTRL6               0xB9
#define RW_BB_DSP_CONFIG_CTRL7               0xBB
#define RW_BD_DSP_CONFIG_CTRL8               0xBD
#define RW_BF_DSP_CONFIG_CTRL9               0xBF
#define RW_C1_DSP_CONFIG_CTRL10              0xC1
#define RW_C9_PROX_HIGH_THRES1_CFG0          0xC9
#define RW_CA_PROX_HIGH_THRES1_CFG1          0xCA
#define RW_CB_PROX_HIGH_THRES1_CFG2          0xCB
#define RW_CC_PROX_HIGH_THRES1_CFG3          0xCC
#define RW_CD_PROX_HIGH_THRES1_CFG4          0xCD
#define RW_D1_PROX_HIGH_THRES2_CFG0          0xD1
#define RW_D2_PROX_HIGH_THRES2_CFG1          0xD2
#define RW_D3_PROX_HIGH_THRES2_CFG2          0xD3
#define RW_D4_PROX_HIGH_THRES2_CFG3          0xD4
#define RW_D5_PROX_HIGH_THRES2_CFG4          0xD5
#define RW_D9_PROX_HIGH_THRES3_CFG0          0xD9
#define RW_DA_PROX_HIGH_THRES3_CFG1          0xDA
#define RW_DB_PROX_HIGH_THRES3_CFG2          0xDB
#define RW_DC_PROX_HIGH_THRES3_CFG3          0xDC
#define RW_DD_PROX_HIGH_THRES3_CFG4          0xDD
#define RO_DE_CAP_INI_CH4_0                  0xDE
#define RO_DF_CAP_INI_CH4_1                  0xDF
#define RW_E0_BL_ALPHA_UP_DN_SEL             0xE0
#define RW_E2_SAMPLE_NUM_7_0                 0xE2
#define RW_E3_SAMPLE_NUM_9_8                 0xE3
#define RW_E4_INTEGRATION_NUM_7_0            0xE4
#define RW_E5_INTEGRATION_NUM_9_8            0xE5
#define RW_E6_CH10_SCAN_FACTOR               0xE6
#define RW_E7_CH32_SCAN_FACTOR               0xE7
#define RW_E8_DSP_CONFIG_CTRL0               0xE8
#define RW_E9_CH10_DOZE_FACTOR               0xE9
#define RW_EA_CH32_DOZE_FACTOR               0xEA
#define RW_EB_CH10_PROX_FACTOR               0xEB
#define RW_EC_CH4_FACTOR_CTRL                0xEC
#define RW_ED_DSP_CONFIG_CTRL1               0xED
#define RW_EE_DSP_CONFIG_CTRL2               0xEE
#define RW_EF_DSP_CONFIG_CTRL3               0xEF
#define RO_F0_DEC_DATA0                      0xF0
#define RO_F1_DEC_DATA1                      0xF1
#define RO_F2_DEC_DATA2                      0xF2
#define RO_F3_DEC_DATA3                      0xF3
#define RO_F5_CAP_INI_CH0_0                  0xF5
#define RO_F6_CAP_INI_CH0_1                  0xF6
#define RO_F7_CAP_INI_CH1_0                  0xF7
#define RO_F8_CAP_INI_CH1_1                  0xF8
#define RO_F9_CAP_INI_CH2_0                  0xF9
#define RO_FA_CAP_INI_CH2_1                  0xFA
#define RO_EB_CAP_INI_CH3_0                  0xEB
#define RO_EC_CAP_INI_CH3_1                  0xEC
#define RW_7D_REG_TO_ANA2                    0x7D
#define RW_7E_REG_TO_ANA3                    0x7E
#define RW_7F_REG_TO_ANA4                    0x7F
#define RW_80_REG_TO_ANA5                    0x80
#define RW_81_REG_TO_ANA6                    0x81
#define RW_82_REG_TO_ANA7                    0x82
        
#define DEVICE_ID_REV2                       0x2D


void delay_ms(uint16_t t);

uint8_t hx9033s_read_reg(uint8_t addr);
bool hx9033s_write_reg(uint8_t addr, uint8_t data);
//bool hx9033s_write_reg(uint8_t addr, uint8_t data); 
//uint8_t hx9033s_read_reg(uint8_t addr); 
void hx90xx_print_offset_dac(void);

bool hx90xx_button_scan(void);

void hx90xx_power_up(void);
void hx90xx_power_down(void);
void hx90xx_power_restart(void);
bool hx90xx_get_trigger_status(void);
void hx90xx_reset_trigger_status(void);
bool hx90xx_init(void);

uint16_t hx90xx_read_unsigned_data_16bit(uint8_t addr_h, uint8_t addr_l);
uint16_t hx90xx_read_unsigned_data_16bit_triple(uint8_t addr_h, uint8_t addr_l);

uint8_t hx90xx_check_device_id(void);
bool hx90xx_reg_init(void);
uint8_t hx90xx_report_diff_prox_status(void);
uint8_t hx90xx_get_wear_status(void);
uint8_t hx90xx_get_touch_status(void);
void hx90xx_timer_start(void);
void hx90xx_timer_stop(void);
void hx90xx_factory_read_data(void);
void hx90xx_factory_cali_unwear_init(void);
void hx90xx_factory_cali_wear_init(void);
void hx90xx_factory_cali_read_data(void);
void hx90xx_write_nv(uint16_t offset0, uint16_t offset1, int16_t wear_data0, int16_t wear_data1,uint8_t dac_swap);

void hx90xx_update_nv_data(void);
void hx90xx_update_min_nv_data(int16_t *lp_data);
void hx90xx_update_temp_follow(int16_t *lp_data,int32_t *diff);
void hx90xx_int_handle(void);
uint8_t hx90xx_report_touch_status(void);
void hx90xx_timer_report_data(void);

void hx90xx_conv_int_off(void);
void hx90xx_conv_int_on(void);
uint8_t hx90xx_get_touch_motion(void);
void hx90xx_read_cali_K_nv(void);
void hx90xx_write_cali_K_nv(uint16_t Wcali_K0, uint16_t Wcali_K1, uint16_t Tcali_K2);

extern __IO uint32_t uwTick;
void TIM_Init(void);
#endif
