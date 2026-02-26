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

    RESOURCE_ID_BLE_CONNECT = 0x01,          /// 蓝牙连接图标
    RESOURCE_ID_BLE_DISCONNECT,              /// 蓝牙断开图标

    RESOURCE_ID_HOME_BATTERY_0 = 10,         /// 首页电池电量0%
    RESOURCE_ID_HOME_BATTERY_20,             /// 首页电池电量20%
    RESOURCE_ID_HOME_BATTERY_40,             /// 首页电池电量40%
    RESOURCE_ID_HOME_BATTERY_60,             /// 首页电池电量60%
    RESOURCE_ID_HOME_BATTERY_80,             /// 首页电池电量80%
    RESOURCE_ID_HOME_BATTERY_100,            /// 首页电池电量100%

    RESOURCE_ID_HOME_CHARGING,               /// 首页充电图标
    RESOURCE_ID_HOME_UNCHARGE,               /// 首页未充电图标
    RESOURCE_ID_HOME_TIME_COLON,             /// 首页时间冒号

    RESOURCE_ID_CHARGE_ADAPTER_ICON,     /// 首页充电适配器图标
    RESOURCE_ID_DUMMY_ICON,              /// dummy图标

    RESOURCE_ID_MOM_PICTURE_ICON,              /// 妈妈图片图标
    RESOURCE_ID_IDLE_TEXT_AREA_ICON,           /// 空闲界面文本区域图标
    RESOURCE_ID_IDLE_BUTTOM_ICON,              /// 空闲界面按钮指示图标

    RESOURCE_ID_AIRPLANE_UNLOCK_BUTTON_ICON,   /// 飞行模式解锁按钮图标

    RESOURCE_ID_HOME_STIMULATE_ICON = 101,   /// 首页-刺激图标
    RESOURCE_ID_HOME_LACTATION_ICON,         /// 首页-泌乳图标
    RESOURCE_ID_HOME_MIX_ICON,               /// 首页-混合图标
    RESOURCE_ID_HOME_PERSONALIZATION_ICON,   /// 首页-个性化图标
    RESOURCE_ID_HOME_PP_ICON,                /// 首页-PP图标
    RESOURCE_ID_HOME_MILK_ICON,              /// 首页-奶量图标
    RESOURCE_ID_HOME_INITIA_ICON,            /// 首页-开奶图标

   
    RESOURCE_ID_PUMP_HOME_BG = 130,          /// 泵奶首页背景

     /* 模式组 [301..400]（大图 + 小图标） */
    RESOURCE_ID_INITIATING_MODE = 201,        /// 模式-启动界面
    RESOURCE_ID_MODE_LACTATION_ICON,          /// 模式-泌乳图标
    RESOURCE_ID_MODE_MIX_ICON,                /// 模式-混合图标
    RESOURCE_ID_MODE_STIMULATE_ICON,          /// 模式-刺激图标
    RESOURCE_ID_FREQ_BOOST_ICON,              /// 频率增强图标
    RESOURCE_ID_FREQ_REGULAR_ICON,            /// 频率常规图标
    RESOURCE_ID_FREQ_COMFORT_ICON,            /// 频率舒适图标

    RESOURCE_ID_GUIDE_QR_ICON = 250,
    RESOURCE_ID_GUIDE_LRM_ICON,
    RESOURCE_ID_GUIDE_PLAYPAUSE_ICON,
    RESOURCE_ID_GUIDE_MODESTAR_ICON,
    RESOURCE_ID_GUIDE_ARROW_ICON,
    RESOURCE_ID_GUIDE_PUMPING_ABB_ICON,
    RESOURCE_ID_GUIDE_PUMPING_START_ABB_ICON,
    RESOURCE_ID_GUIDE_KN_ABB_0,
    RESOURCE_ID_GUIDE_KN_ABB_1,

    RESOURCE_ID_EMODE_R_ICON,
    RESOURCE_ID_EMODE_L_ICON,
    RESOURCE_ID_CMODE_R_ICON,
    RESOURCE_ID_CMODE_L_ICON,
    RESOURCE_ID_STAR_ICON,
    RESOURCE_ID_HUE_ICON,
    RESOURCE_ID_HEART_ICON,
    RESOURCE_ID_EXPRESSION_ICON,
    RESOURCE_ID_MODE_BG_ICON,
    RESOURCE_ID_MODE_VECTOR_ICON,
    RESOURCE_ID_PUMP_MODE_ANIMATION_START = 300,   /// 泵奶模式动画-开始

    RESOURCE_ID_PUMP_MODE_ANIMATION_END = 449,     /// 泵奶模式动画-结束

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


