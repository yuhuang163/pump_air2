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

    /* 通用组 [1..100] */
    RESOURCE_ID_BLE_CONNECT = 0x01,            /// 蓝牙连接图标
    RESOURCE_ID_BLE_DISCONNECT,                /// 蓝牙断开图标

    /* 主界面组 [100..150] */
    RESOURCE_ID_COMMON_BG = 100,               /// 通用背景
    RESOURCE_ID_START_LOGO_ICON,               /// 启动LOGO
    RESOURCE_ID_HOME_IDLE_LABLE_ICON,           /// 空闲界面文本区域图标
    RESOURCE_ID_HOME_IDLE_BUTTOM_ICON,              /// 空闲界面按钮指示图标
    RESOURCE_ID_AIRPLANE_UNLOCK_BUTTON_ICON,   /// 飞行模式解锁按钮图标
    RESOURCE_ID_HOME_DUMMY_ICON,                    /// dummy图标
    RESOURCE_ID_CHARGE_ADAPTER_ICON,           /// 首页充电适配器图标

    /* 引导组 [150..200] */
    RESOURCE_ID_PUMP_SETTING_GUIDE_ICON = 150,          /// 左+右+模式
    RESOURCE_ID_PLAY_PAUSE_GUIDE_ICON,          /// 启动暂停
    RESOURCE_ID_MODE_SEL_GUIDE_ICON,           /// Mode+Star
    RESOURCE_ID_ARROW_GUIDE_ICON,              /// 向下箭头
    RESOURCE_ID_EXPERT_PUMP_GUIDE_ICON,        /// pumping缩略图
    RESOURCE_ID_PUMP_START_GUIDE_ICON,  /// pumping启动缩略图
    RESOURCE_ID_KNOB_GUIDE_0,                /// 旋钮指示缩略图0
    RESOURCE_ID_KNOB_GUIDE_1,                /// 旋钮指示缩略图1

    /* 模式相关组 [301..400]（大图 + 小图标） */
    RESOURCE_ID_PUMP_SPEED_SLOW_ICON = 200,    /// 泵奶首页-慢速图标
    RESOURCE_ID_PUMP_SPEED_REGULAR_ICON,       /// 泵奶首页-常规图标
    RESOURCE_ID_PUMP_SPEED_BOOST_ICON,         /// 泵奶首页-快速图标
    RESOURCE_ID_PUMP_DASHED_ARC_ICON,          /// 泵奶首页-虚线弧线图标
    RESOURCE_ID_PUMP_TIME_COLON,               /// 泵奶首页时间冒号
    RESOURCE_ID_PUMP_LEFT_LEVEL_ICON,          /// 泵奶首页左档位图标
    RESOURCE_ID_PUMP_RIGHT_LEVEL_ICON,         /// 泵奶首页右档位图标
    RESOURCE_ID_PUMP_LEFT_DIR_ICON,            /// 泵奶首页左方向图标
    RESOURCE_ID_PUMP_RIGHT_DIR_ICON,           /// 泵奶首页右方向图标

    RESOURCE_ID_FREQ_PAGE_BOOST_ICON,          /// 频率页-频率增强图标
    RESOURCE_ID_FREQ_PAGE_REGULAR_ICON,        /// 频率页-频率常规图标
    RESOURCE_ID_FREQ_PAGE_COMFORT_ICON,        /// 频率页-频率舒适图标

    RESOURCE_ID_MODE_STIMULATION_PROMPT_ICON,  /// 模式-刺激确认提示图标
    RESOURCE_ID_MODE_EXPRESSION_PROMPT_ICON,   /// 模式-泌乳确认提示图标
    RESOURCE_ID_MODE_MIX_PROMPT_ICON,          /// 模式-混合确认提示图标
    RESOURCE_ID_MODE_RELAX_PROMPT_ICON,        /// 模式-放松确认提示图标

    RESOURCE_ID_MODE_BG_ICON,                  /// 模式背景图标
    RESOURCE_ID_LAST_USED_MODE_ICON,           /// 上一次使用模式图标
    RESOURCE_ID_NEW_MODE_STEPS_PROMPT_ICON,     /// 新模式未确认图标
    RESOURCE_ID_MODE_LACTATION_ICON,           /// 模式-泌乳图标
    RESOURCE_ID_MODE_MIX_ICON,                 /// 模式-混合图标
    RESOURCE_ID_MODE_STIMULATE_ICON,           /// 模式-刺激图标

    RESOURCE_ID_MODE_CUSTOM_ICON,          /// 韵律-专家模式图标
    RESOURCE_ID_MODE_EXPERT_ICON,          /// 韵律-个性化模式图标

    RESOURCE_ID_PUMP_FINISH_ICON,              /// 泵奶完成图标
    RESOURCE_ID_PUMP_PAUSE_PROMPT_ICON,        /// 泵奶暂停提示图标
    RESOURCE_ID_PUMP_LEVEL_BACKGROUND_ICON,    /// 泵奶档位背景图标
    RESOURCE_ID_PUMP_LEVEL_ADD_LABEL_ICON,     /// 泵奶档位文本加号图标
    RESOURCE_ID_PUMP_LEVEL_SUB_LABEL_ICON,     /// 泵奶档位文本减号图标

    /* 动画组 [300...] */
    RESOURCE_ID_COMMON_MODE_ANIMATION_START = 300,   /// 泵奶模式动画-开始
    RESOURCE_ID_COMMON_MODE_ANIMATION_END = 414,     /// 泵奶模式动画-结束

    RESOURCE_ID_PERSONALIZED_MODE_ANIMATION_START = 500,   /// 个性化模式动画-开始
    RESOURCE_ID_PERSONALIZED_MODE_ANIMATION_END = 619,     /// 泵奶模式动画-结束

    RESOURCE_ID_HOME_DEFAULT_ANIMATION_START = 700,   /// 首页默认动画-开始
    RESOURCE_ID_HOME_DEFAULT_ANIMATION_END = 819,     /// 首页默认动画-结束

    RESOURCE_ID_PUMP_LEVEL_START = 900,   /// 泵奶档位图片-开始
    RESOURCE_ID_PUMP_LEVEL_END = 914,     /// 泵奶档位图片-结束

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


