/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_config.h"
#include "bsp_i2c.h" // IWYU pragma: keep
#include "bsp_charge.h"
#include "bsp_ntc_map.h" // IWYU pragma: keep
#include "bsp_battery_internal.h"
#include "sgm41519.h"


#if IS_SUPPORT_CHARGE_IC_SGM41519 
/*
* DEFINES (omit if already defined by bsp_charge.c when embedded)
*****************************************************************************************
*/
#ifndef CHARGE_I2C_ADDR
#define CHARGING_STATUS_VALUE               0x02
#define CHARHE_FULL_STATUS_VALUE            0x01
#define UNCHARGE_STATUS_VALUE               0x00 

#define CHARHE_CHECK_NO_BATT_0              0xFF 
#define CHARHE_CHECK_NO_BATT_1              0xFE 
#define CHARGE_I2C_ADDR                     0x1A
#define CHARGE_RETRY_COUNT                  3
#define CHARGE_GET_10BIT_DATA(data)         (((data[1] & 0xFF03) << 8) | data[0])

#define CHARGE_ABNORMAL_MAX_COUNT           10
#endif

/*
* LOCAL VARIABLE DEFINITIONS)
*****************************************************************************************
*/
static uint16_t sgm41519_abn_cnt = 0;
static int sgm41519_ic_ready = 0;
static uint8_t sgm41519_ship_mode = 0;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static int sgm41519_write_nbyte(uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int ret = 0;

    if (sgm41519_ic_ready == 0) {
        LOGE("charge is not ready");
        return BSP_NOT_READY;
    }

    i2c_master_data_t i2c_data = {
        .dev_address = CHARGE_I2C_ADDR,
        .reg_addr = &reg_addr,
        .reg_addr_len = 1,
        .data = data,
        .len = len,
    };

    for (uint8_t i = 0; i < CHARGE_RETRY_COUNT; i++) {
        ret = bsp_i2c_write_byte(CHARGE_I2C_ID, &i2c_data);
        if (ret == BSP_OK) {
            sgm41519_abn_cnt = 0;
            return BSP_OK;
        }
        msleep(10);
    }

    sgm41519_abn_cnt++;
    if (sgm41519_abn_cnt > CHARGE_ABNORMAL_MAX_COUNT) {
        sgm41519_ic_ready = 0;
    }

    return BSP_ERROR;
}

static int sgm41519_read_nbyte(uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int ret = 0;

    i2c_master_data_t i2c_data = {
        .dev_address = CHARGE_I2C_ADDR,
        .reg_addr = &reg_addr,
        .reg_addr_len = 1,
        .data = data,
        .len = len,
    };

    for (uint8_t i = 0; i < CHARGE_RETRY_COUNT; i++) {
        ret = bsp_i2c_read_byte(CHARGE_I2C_ID, &i2c_data);
        if (ret == BSP_OK) {
            sgm41519_abn_cnt = 0;
            return BSP_OK;
        }
        msleep(10);
    }

    sgm41519_abn_cnt++;
    if (sgm41519_abn_cnt > CHARGE_ABNORMAL_MAX_COUNT) {
        sgm41519_ic_ready = 0;
    }

    return BSP_ERROR;
}

static int sgm41519_read_nbyte_no_retry(uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int ret = 0;

    i2c_master_data_t i2c_data = {
        .dev_address = CHARGE_I2C_ADDR,
        .reg_addr = &reg_addr,
        .reg_addr_len = 1,
        .data = data,
        .len = len,
    };

    ret = bsp_i2c_read_byte(CHARGE_I2C_ID, &i2c_data);
    if (ret == BSP_OK) {
        sgm41519_abn_cnt = 0;
        return BSP_OK;
    }

    sgm41519_abn_cnt++;
    if (sgm41519_abn_cnt > CHARGE_ABNORMAL_MAX_COUNT) {
        sgm41519_ic_ready = 0;
    }

    return BSP_ERROR;
}

static int sgm41519_chip_is_ready(void)
{
    int ret = 0;
    uint8_t value = 0;
    for (uint8_t i = 0; i < 5; i++) {
        ret = sgm41519_read_nbyte_no_retry(SGM41519_CHARGE_ID_REG, &value, 1);
        if (((value >> 3) & 0x0F) == SGM41519_CHARGE_ID_VALUE) {
            sgm41519_ic_ready = 1;
            sgm41519_abn_cnt = 0;
            return BSP_OK;
        }
        msleep(20);
    }
    return BSP_ERROR;
}

static int sgm41519_charge_is_full(void)
{
    uint8_t reg_data = 0;
    int ret = sgm41519_read_nbyte(SGM41519_INPUT_POWER_STATUS_REG, &reg_data, 1);
    if (ret != BSP_OK) {
        LOGE("sgm41519 charge is full poll failed, ret: %d", ret);
        return ret;
    }
    return (reg_data & 0x18) == 0x18 ? true : false;
}

static int sgm41519_charge_status_poll(charge_chip_status_t *status)
{
    uint8_t reg_data = 0;
    int ret = sgm41519_read_nbyte(SGM41519_INPUT_POWER_STATUS_REG, &reg_data, 1);
    if (ret != BSP_OK || status == NULL) {
        return BSP_ERROR;
    }
    status->status = reg_data;
    status->charge_full = ((reg_data & 0x18) == 0x18) ? 1 : 0;
    status->plug_in = ((reg_data & 0x07) != 0) ? 1 : 0;
    return BSP_OK;
}

static int sgm41519_update_bits(uint8_t reg, uint8_t mask, uint8_t value)
{
    uint8_t reg_data = 0;
    int ret = sgm41519_read_nbyte(reg, &reg_data, 1);
    if (ret != BSP_OK) {
        return ret;
    }
    reg_data = (uint8_t)((reg_data & (~mask)) | (value & mask));
    return sgm41519_write_nbyte(reg, &reg_data, 1);
}

static uint8_t sgm41519_ibat_ma_to_reg02(uint16_t current_ma)
{
    if (current_ma <= 300U) {
        return SGM41519_ICHG_REG02_300MA;
    }
    if (current_ma <= 600U) {
        return SGM41519_ICHG_REG02_600MA;
    }
    return SGM41519_ICHG_REG02_1320MA;
}

static void sgm41519_device_init(void)
{
    int ret = BSP_OK;
    uint8_t reg_data = 0;

    ret = sgm41519_chip_is_ready();
    if (ret != BSP_OK) {
        LOGE("sgm41519 device is not ready, ret: %d", ret);
        return;
    }
    /* 上电初始化：写入 DingTalk 要求的默认寄存器值 */
    reg_data = SGM41519_INPUT_LIMIT_CURRENT_REG_VAL;
    sgm41519_write_nbyte(SGM41519_INPUT_LIMIT_CURRENT_REG, &reg_data, 1);   // 输入限流 ≈2.4A，HIZ 关闭
    // sgm41519_read_nbyte_no_retry(SGM41519_INPUT_LIMIT_CURRENT_REG, &reg_data, 1);
    // LOG_PBUS("charge scan reg0x00:0x%X", reg_data);
  
    /* 温度策略：按电池 NTC 温度选择 0.2C/0.6C/1C 档位 */
    {
    //   uint8_t reg02_val = Charge_Current_By_Temp_Strategy(charge_temp);
        /// 根据温度策略选择充电电流

        reg_data = SGM41519_ICHG_REG02_1320MA;  // 最大 1C，采用 1320mA 档 */
        sgm41519_write_nbyte(SGM41519_ICHG_REG02_REG, &reg_data, 1);
        // sgm41519_read_nbyte_no_retry(SGM41519_ICHG_REG02_REG, &reg_data, 1);
        // LOG_PBUS("charge scan reg0x02:0x%X", reg_data);
    }
  
    reg_data = SGM41519_CHARGE_ENABLE_BIT | SGM41519_OTG_ENABLE_BIT;
    sgm41519_write_nbyte(SGM41519_ENABLE_CHARGE_REG, &reg_data, 1);// 使能充电，打开 OTG
    // sgm41519_read_nbyte_no_retry(SGM41519_ENABLE_CHARGE_REG, &reg_data, 1);
    // LOG_PBUS("charge scan reg0x01:0x%X", reg_data);

    reg_data = SGM41519_BOOST_MODE_VOL_REG_VAL;
    sgm41519_write_nbyte(SGM41519_BOOST_MODE_VOL_REG, &reg_data, 1); // 预设 OTG/Boost 输出电压5V
    // sgm41519_read_nbyte_no_retry(SGM41519_BOOST_MODE_VOL_REG, &reg_data, 1);
    // LOG_PBUS("charge scan reg0x06:0x%X", reg_data);

    reg_data = 0x00;
    sgm41519_write_nbyte(SGM41519_DISABLE_BATFET_REG, &reg_data, 1);// 默认关闭船运模式位，保持正常供电
    // sgm41519_read_nbyte_no_retry(SGM41519_DISABLE_BATFET_REG, &reg_data, 1);
    // LOG_PBUS("charge scan reg0x07:0x%X", reg_data);

    reg_data = 0x8D;
    sgm41519_write_nbyte(SGM41519_CHARGE_CTRL_REG05, &reg_data, 1);
    // sgm41519_read_nbyte_no_retry(SGM41519_CHARGE_CTRL_REG05, &reg_data, 1);
    // LOG_PBUS("charge scan reg0x05:0x%X", reg_data);
  
    reg_data = 0x99;
    sgm41519_write_nbyte(SGM41519_CHARGE_PRE_LIMIT_REG03, &reg_data, 1);// 写回修改后的值
    // sgm41519_read_nbyte_no_retry(SGM41519_CHARGE_PRE_LIMIT_REG03, &reg_data, 1);
    // LOG_PBUS("charge scan reg0x03:0x%X", reg_data);
  
    LOG_PBUS("sgm41519 charge chip init success");
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void sgm41519_chip_init(void)
{
    sgm41519_device_init();
}

int charge_status_poll(charge_chip_status_t *status)
{
    return sgm41519_charge_status_poll(status);
}

void sgm41519_set_charge_current(uint16_t current_ma)
{
    uint8_t reg_data = sgm41519_ibat_ma_to_reg02(current_ma);
    int ret = sgm41519_write_nbyte(SGM41519_ICHG_REG02_REG, &reg_data, 1);
    LOG_PBUS("sgm41519_set_charge_current: %dmA -> reg02 0x%02X, ret %d", current_ma, reg_data, ret);
}

POSSIBLY_UNUSED int sgm41519_ic_is_ready(void)
{
    return sgm41519_ic_ready;
}

int sgm41519_set_charge_enable(uint8_t enable)
{
    return sgm41519_update_bits(SGM41519_ENABLE_CHARGE_REG, SGM41519_CHARGE_ENABLE_BIT,
        enable ? SGM41519_CHARGE_ENABLE_BIT : 0);
}

int sgm41519_set_otg_boost_enable(uint8_t enable)
{
    return sgm41519_update_bits(SGM41519_ENABLE_CHARGE_REG, SGM41519_OTG_ENABLE_BIT,
        enable ? SGM41519_OTG_ENABLE_BIT : 0);
}

int sgm41519_get_otg_boost_enable(void)
{
    uint8_t reg_data = 0;
    if (sgm41519_read_nbyte(SGM41519_ENABLE_CHARGE_REG, &reg_data, 1) != BSP_OK) {
        return 0;
    }
    return (reg_data & SGM41519_OTG_ENABLE_BIT) ? 1 : 0;
}

int sgm41519_enter_ship_mode(void)
{
    uint8_t reg_data = 0;
    int ret = BSP_ERROR;
    const uint8_t max_retry = 5;
    for (uint8_t i = 0; i < max_retry; i++) {
        ret = sgm41519_update_bits(SGM41519_DISABLE_BATFET_REG, SHIP_MODE_BIT, SHIP_MODE_BIT);
        if (ret != BSP_OK) {
            msleep(5);
            continue;
        }
        ret = sgm41519_read_nbyte(SGM41519_DISABLE_BATFET_REG, &reg_data, 1);
        if (ret == BSP_OK && (reg_data & SHIP_MODE_BIT)) {
            sgm41519_ship_mode = 1;
            return BSP_OK;
        }
        ret = BSP_ERROR;
        msleep(5);
    }
    return ret;
}

int sgm41519_exit_ship_mode(void)
{
    int ret = sgm41519_update_bits(SGM41519_DISABLE_BATFET_REG, SHIP_MODE_BIT, 0);
    if (ret == BSP_OK) {
        sgm41519_ship_mode = 0;
    }
    return ret;
}

int sgm41519_get_ship_mode(void)
{
    uint8_t reg_data = 0;
    if (sgm41519_read_nbyte(SGM41519_DISABLE_BATFET_REG, &reg_data, 1) == BSP_OK) {
        sgm41519_ship_mode = (reg_data & SHIP_MODE_BIT) ? 1 : 0;
    }
    return sgm41519_ship_mode;
}
#endif  