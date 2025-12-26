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
    RESOURCE_ID_NONE = 0x00, /// 无资源

    /**
     * 资源ID分组规则：
     * - 每组预留 100 个 ID 空间
     * - 脚本会解析 enum 的数值；为保证脚本兼容，组起始值使用“纯数字字面量”显式赋值
     * - 组内资源ID连续递增，组间资源ID不连续
     */

    /* 蓝牙组 [1..100] */
    RESOURCE_ID_BLE_CONNECT = 0x01, /// 蓝牙连接图标
    RESOURCE_ID_BLE_DISCONNECT,     /// 蓝牙断开图标
    RESOURCE_ID_BLE_LINE_ICON,      /// 蓝牙连接线图标

    /* 充电/电池组 [101..200]（充电页 + 首页电池） */
    RESOURCE_ID_CHARGE_PAGE_BATT_0 = 101,   /// 充电页电量0%
    RESOURCE_ID_CHARGE_PAGE_BATT_25,        /// 充电页电量25%
    RESOURCE_ID_CHARGE_PAGE_BATT_50,        /// 充电页电量50%
    RESOURCE_ID_CHARGE_PAGE_BATT_75,        /// 充电页电量75%
    RESOURCE_ID_CHARGE_PAGE_BATT_100,       /// 充电页电量100%

    RESOURCE_ID_HOME_CHARGE_0,              /// 首页-充电电量0%
    RESOURCE_ID_HOME_CHARGE_25,             /// 首页-充电电量25%
    RESOURCE_ID_HOME_CHARGE_50,             /// 首页-充电电量50%
    RESOURCE_ID_HOME_CHARGE_75,             /// 首页-充电电量75%
    RESOURCE_ID_HOME_CHARGE_100,            /// 首页-充电电量100%

    RESOURCE_ID_HOME_UNCHARGE_BATTERY_0,    /// 首页-未充电电量0%
    RESOURCE_ID_HOME_UNCHARGE_BATTERY_25,   /// 首页-未充电电量25%
    RESOURCE_ID_HOME_UNCHARGE_BATTERY_50,   /// 首页-未充电电量50%
    RESOURCE_ID_HOME_UNCHARGE_BATTERY_75,   /// 首页-未充电电量75%
    RESOURCE_ID_HOME_UNCHARGE_BATTERY_100,  /// 首页-未充电电量100%
    RESOURCE_ID_HOME_UNCHARGE_LOW_POWER,    /// 首页-低电量提示

    /* 引导组 [201..300] */
    RESOURCE_ID_GUIDE_BOTTLE = 201, /// 引导-奶瓶
    RESOURCE_ID_GUIDE_KNOB,         /// 引导-旋钮
    RESOURCE_ID_GUIDE_PHONE,        /// 引导-手机
    RESOURCE_ID_GUIDE_PHONE_BOTTLE, /// 引导-手机+奶瓶

    /* 模式组 [301..400]（大图 + 小图标） */
    RESOURCE_ID_INITIATING_MODE = 301,      /// 模式-启动界面
    RESOURCE_ID_MILK_MODE,                  /// 模式-奶量界面
    RESOURCE_ID_PERSONALIZATION_MODE,       /// 模式-个性化界面
    RESOURCE_ID_PP_MODE,                    /// 模式-PP界面

    RESOURCE_ID_MODE_LACTATION_ICON_P,      /// 模式-泌乳图标(P)
    RESOURCE_ID_MODE_LACTATION_ICON_W,      /// 模式-泌乳图标(W)
    RESOURCE_ID_MODE_MIX_ICON_P,            /// 模式-混合图标(P)
    RESOURCE_ID_MODE_MIX_ICON_W,            /// 模式-混合图标(W)
    RESOURCE_ID_MODE_STIMULATE_ICON_P,      /// 模式-刺激图标(P)
    RESOURCE_ID_MODE_STIMULATE_ICON_W,      /// 模式-刺激图标(W)

    /* 其它组 [401..500]（首页图标 + 其它杂项） */
    RESOURCE_ID_HOME_INITIA_ICON = 401,      /// 首页-启动图标
    RESOURCE_ID_HOME_LACTATION_ICON,        /// 首页-泌乳图标
    RESOURCE_ID_HOME_MILK_ICON,             /// 首页-奶量图标
    RESOURCE_ID_HOME_MIX_ICON,              /// 首页-混合图标
    RESOURCE_ID_HOME_PERSONALIZATION_ICON,  /// 首页-个性化图标
    RESOURCE_ID_HOME_PP_ICON,               /// 首页-PP图标
    RESOURCE_ID_HOME_STIMULATE_ICON,        /// 首页-刺激图标

    RESOURCE_ID_PUMP_FINISH_ICON,           /// 泵奶完成图标
    RESOURCE_ID_START_LOGO_ICON,            /// 启动LOGO
    RESOURCE_ID_TRAVEL_LOCK,                /// 旅行锁图标
    RESOURCE_ID_TRAVEL_LOCK_CONTENT,        /// 旅行锁提示文案

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


