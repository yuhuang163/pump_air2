#ifndef __SGM41519_H__
#define __SGM41519_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "bsp_internal.h" // IWYU pragma: keep

/**@brief Firmware information define*/



// 充电寄存器配置
#define SGM41519_INPUT_POWER_STATUS_REG 0x08
#define SGM41519_INPUT_LIMIT_CURRENT_REG 0x00
#define SGM41519_ICHG_REG02_REG 0x02
#define SGM41519_ENABLE_CHARGE_REG 0x01
#define SGM41519_CHARGE_PRE_LIMIT_REG03 0x03
#define SGM41519_CHARGE_ID_REG 0x0B
#define SGM41519_CHARGE_ID_VALUE 0x02
// 关机寄存器配置
#define ENABLE_HIZ_MODE_REG 0x00
#define SGM41519_DISABLE_BATFET_REG 0x07
// 开机寄存器配置
#define SGM41519_BOOST_MODE_VOL_REG 0x06
#define BOOST_MODE_LIMIT_REG 0x02
#define OTG_CONFIG_REG 0x01
#define SGM41519_CHARGE_CTRL_REG04 0x04
#define SGM41519_CHARGE_CTRL_REG05 0x05
// 充电的最大电流限制
#define CHARGE_CURRENT_13A_REG_VAL 0x6D // 1.32A  ICHG[5:0]=0x2D (1320mA)
/* REG02 ICHG[5:0] 温度策略对应电流（Table 10），高 2 位与现有一致 */
#define SGM41519_ICHG_REG02_300MA 0x58  // 300mA  ICHG[5:0]=0x18
#define SGM41519_ICHG_REG02_600MA 0x61  // 600mA  ICHG[5:0]=0x21
#define SGM41519_ICHG_REG02_1320MA 0x6D // 1320mA（约 1300mA 档位）ICHG[5:0]=0x2D
/* 按 DingTalk 共享的寄存器表为 M5Pro 设定默认值
 * 充电芯片寄存器位含义（主要位）：
 *  - 0x00：bit7 HIZ 使能，bit4:0 输入限流 (50mA/step，+100mA 起始)
 *  - 0x01：bit5 OTG/Boost 使能，bit4 充电使能
 *  - 0x02：bit7 Boost 电流限位，bit5:0 充电电流 (64mA/step)
 *  - 0x06：bit5:4 Boost 输出电压档位
 *  - 0x07：bit5 船运模式（关闭 BATFET）
 */
#define SGM41519_INPUT_LIMIT_CURRENT_REG_VAL 0x17        // 2.4A    01 0111
#define SGM41519_CHARGE_ENABLE_BIT (1 << 4)              // reg0x01 bit4
#define SGM41519_OTG_ENABLE_BIT (1 << 5)                 // reg0x01 bit5
#define HIZ_MODE_BIT (1 << 7)                   // reg0x00 bit7
#define SHIP_MODE_BIT (1 << 5)                  // reg0x07 bit5
#define SGM41519_BOOST_MODE_VOL_REG_VAL (2 << 3)         // reg0x06 bits5:4=01，约 5.0V
#define BOOST_MODE_LIMIT_HIGH_VAL (1 << 7)      // reg0x02 bit7，高限流档位
#define WATCHDOG_TIMER_DISABLE (1 << 6)         // reg0x01 bit6
#define THERNAL_REGULATOR_THRESHOLD (~(1 << 1)) // reg0x05 bit1
#define CHARGE_LOW_TEMP_CURRENT_REG_VAL 0x56 // 1400mah * 0.2C =280ma     // 0.25A  010 1010



/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
void sgm41519_chip_init(void);
void sgm41519_set_charge_current(uint16_t current);
int charge_status_poll(charge_chip_status_t *status);
int sgm41519_ic_is_ready(void);
int sgm41519_set_charge_enable(uint8_t enable);
int sgm41519_set_otg_boost_enable(uint8_t enable);
int sgm41519_get_otg_boost_enable(void);
int sgm41519_enter_ship_mode(void);
int sgm41519_exit_ship_mode(void);
int sgm41519_get_ship_mode(void);
#ifdef __cplusplus
}
#endif

#endif 
