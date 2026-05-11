#ifndef __BSP_CHARGE_H__
#define __BSP_CHARGE_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "bsp_internal.h" // IWYU pragma: keep

/**@brief Firmware information define*/

/// TODO: 电量计与充电芯片尽量不要耦合
/// 电量计状态（CW221x）
typedef struct {
    uint8_t ready;          /// 电量计数据是否有效
    uint8_t soc;            /// SOC %
    uint8_t soh;            /// SOH %
    uint16_t voltage_mv;    /// 电池电压 mV
    int32_t current_ma;     /// 电池电流 mA
    int16_t temperature_c;  /// 温度 ℃
    uint16_t cycle_count;   /// 循环次数
    uint16_t alert;         /// 告警位
} fuel_gauge_status_t;

/// 充电状态（充电策略视角）
typedef struct {
    uint16_t vbus; /// 总线电压 mV
    uint16_t ibus; /// 总线电流 mA
    uint16_t vbat; /// 电池电压 mV
    uint16_t ibat; /// 电池电流 mA
    uint16_t status; /// 状态寄存器
    float ntc_resistance; /// ntc电阻值 KΩ
    uint8_t charge_full; /// 满电状态
    uint8_t plug_in; /// 插拔状态
    int8_t temp; /// 温度℃
    fuel_gauge_status_t fg; /// 电量计原始指标
} charge_chip_status_t;

/// 输入电流限制
#define IBUS_SCALE_CFG                      IBUS_SCALE_2
#define CHARGE_INPUT_CURRENT_LIMIT          3000
#if IBUS_SCALE_CFG == IBUS_SCALE_2
#define CHARGE_INPUT_CURRENT_LIMIT_SCALE    50
#define CHARGE_ADC_CURRENT_LIMIT_SCALE      12.5f
#else
#define CHARGE_INPUT_CURRENT_LIMIT_SCALE    25
#define CHARGE_ADC_CURRENT_LIMIT_SCALE      6.25f
#endif

/// 输入电压限制
#define VBUS_SCALE_CFG                      VBUS_SCALE_2
#define CHARGE_INPUT_VOLTAGE_LIMIT          4200
#if VBUS_SCALE_CFG == VBUS_SCALE_2
#define CHARGE_INPUT_VOLTAGE_LIMIT_SCALE    40
#else
#define CHARGE_INPUT_VOLTAGE_LIMIT_SCALE    20
#endif

/// 电池输入/输出电流限制
#define IBAT_SCALE_CFG                      IBAT_SCALE_2
#define BATT_INPUT_CURRENT_LIMIT            2000
#define BATT_OUTPUT_CURRENT_LIMIT           3000
#define TEMP_ABNORMAL_CHARGING_CURRENT      600
#if IBAT_SCALE_CFG == IBAT_SCALE_2
#define BATT_CURRENT_LIMIT_SCALE            50
#define BATT_ADC_CURRENT_LIMIT_SCALE        12.5f
#else
#define BATT_CURRENT_LIMIT_SCALE            25
#define BATT_ADC_CURRENT_LIMIT_SCALE        6.25f
#endif

/// 电池输入/输出电压限制
#define VBAT_SCALE_CFG                      VBAT_SCALE_2
#if VBAT_SCALE_CFG == VBAT_SCALE_2
#define BATT_VOLTAGE_SCALE                  40
#else
#define BATT_VOLTAGE_SCALE                  20
#endif

/// ntc电压比例
#define NTC_VOLTAGE_SCALE                   2
/// ntc电流系数
#define NTC_CURRENT_COEFF                   30.0f

#define CHARGE_NORMAL_TEMP_MIN              15
#define CHARGE_NORMAL_TEMP_MAX              45
#define CHARGE_DERATED_TEMP_MIN             0
#define CHARGE_DERATED_TEMP_MAX             60

typedef enum {
    CHARGE_TEMP_UNKNOW,
    CHARGE_TEMP_NORMAL,   /// 正常电流充电
    CHARGE_TEMP_DERATED,  /// 降低电流充电
    CHARGE_TEMP_DISABLE,  /// 禁止充电
} charging_temp_status_e;

/** 与 JW3702/SGM ADC 状态字一致的满充标志位 */
#define CHARGE_STATUS_BAT_FULL_MSK          ((uint16_t)(1u << 9))

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif 
