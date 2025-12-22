#ifndef __RESOURCE_ID_DEFINE_H__
#define __RESOURCE_ID_DEFINE_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include <stdint.h> // IWYU pragma: keep

typedef enum {
    RESOURCE_ID_COMMON_PUMP_MODE_1 = 0x01, /// 常规模式1
    RESOURCE_ID_COMMON_PUMP_MODE_2, /// 常规模式2
    RESOURCE_ID_COMMON_PUMP_MODE_3, /// 常规模式3
    RESOURCE_ID_CUSTOM_PUMP_MODE_1, /// 自定义模式1
    RESOURCE_ID_CUSTOM_PUMP_MODE_2, /// 自定义模式2
    RESOURCE_ID_CUSTOM_PUMP_MODE_3, /// 自定义模式3
    RESOURCE_ID_CUSTOM_PUMP_MODE_4, /// 自定义模式4
    RESOURCE_ID_PUMP_FREQUENCY_LOW, /// 低频
    RESOURCE_ID_PUMP_FREQUENCY_MEDIUM, /// 中频
    RESOURCE_ID_PUMP_FREQUENCY_HIGH, /// 高频
    RESOURCE_ID_TRAVEL_LOCK_TIP,     /// 旅行锁提示
    RESOURCE_ID_PUMP_COMPLETE_TIP,   /// 泵奶完成提示
    RESOURCE_ID_BATTERY_LOW_TIP,     /// 电池电量空提示
    RESOURCE_ID_BATTERY_EMPTY_TIP,   /// 电池电量空提示
    RESOURCE_ID_BATTERY_LEVEL_5_TIP, /// 电池电量5格
    RESOURCE_ID_BATTERY_LEVEL_4_TIP, /// 电池电量4格
    RESOURCE_ID_BATTERY_LEVEL_3_TIP, /// 电池电量3格
    RESOURCE_ID_BATTERY_LEVEL_2_TIP, /// 电池电量2格
    RESOURCE_ID_BATTERY_LEVEL_1_TIP, /// 电池电量1格
    RESOURCE_ID_BATTERY_CHARGING_EMPTY_TIP,   /// 电池充电电量空提示
    RESOURCE_ID_BATTERY_CHARGING_LEVEL_5_TIP, /// 电池充电电量5格
    RESOURCE_ID_BATTERY_CHARGING_LEVEL_4_TIP, /// 电池充电电量4格
    RESOURCE_ID_BATTERY_CHARGING_LEVEL_3_TIP, /// 电池充电电量3格
    RESOURCE_ID_BATTERY_CHARGING_LEVEL_2_TIP, /// 电池充电电量2格
    RESOURCE_ID_BATTERY_CHARGING_LEVEL_1_TIP, /// 电池充电电量1格
    RESOURCE_ID_BLE_DISCONNECTED_TIP, /// 蓝牙断开提示
    RESOURCE_ID_BLE_CONNECTED_TIP,    /// 蓝牙连接提示
    RESOURCE_ID_LOGO, /// 品牌logo
    RESOURCE_ID_TYPE_MAX
} resource_id_type_e;


/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif 


