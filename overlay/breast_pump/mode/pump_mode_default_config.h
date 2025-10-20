#ifndef __PUMP_MODE_DEFAULT_CONFIG_H__
#define __PUMP_MODE_DEFAULT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "motor_policy.h"

/// 常规模式默认工作时长15分钟，单位：秒
#define NORMAL_MODE_DEFAULT_WORK_TIME   4096
/// 无效工作时长，子模式不需要指定工作时长
#define INVALID_WORK_TIME   0  
/// 刺激模式默认循环间隔，单位：毫秒
#define STIM_MODE_DEFAULT_SLEEP_TIME   20  
/// 刺激模式默认保持时间，单位：毫秒
#define STIM_MODE_DEFAULT_HOLD_TIME   50  
/// 吸乳模式默认循环间隔，单位：毫秒
#define PUMP_MODE_DEFAULT_SLEEP_TIME   30  
/// 吸乳模式默认保持时间，单位：毫秒
#define PUMP_MODE_DEFAULT_HOLD_TIME   80  
/// 个性化模式按摩默认时长2分钟，单位：秒
#define CUSTOM_MODE_DEFAULT_MASSAGE_TIME   120  
/// 个性化模式吸乳默认时长28分钟，单位：秒
#define CUSTOM_MODE_DEFAULT_PUMP_TIME   1680  
/// 2+8奶量提升模式按摩默认时长2分钟，单位：秒
#define MILK_BOOST_MODE_DEFAULT_MASSAGE_TIME   10 //120  
/// 2+8奶量提升模式吸乳默认时长8分钟，单位：秒
#define MILK_BOOST_MODE_DEFAULT_PUMP_TIME   20//480 
/// 开奶模式按摩默认时长2分钟，单位：秒
#define LACTATION_START_MODE_DEFAULT_MASSAGE_TIME   120  
/// 开奶模式吸乳默认时长8分钟，单位：秒
#define LACTATION_START_MODE_DEFAULT_PUMP_TIME   480 
/// 追奶模式吸乳时长20分钟，单位：秒
#define MILK_TRACK_MODE_PUMP_TIME_0   20//1200 
/// 追奶模式吸乳时长10分钟，单位：秒
#define MILK_TRACK_MODE_PUMP_TIME_1   10//600  
/// 追奶模式休息时长10分钟，单位：秒
#define MILK_TRACK_MODE_IDLE_TIME     10//600  

/// 刺激
static const motor_policy_cfg_t stimulation_default[STIMULATION_MODE_LEN] = {
    {MOTOR_TIME_POLICY, NORMAL_MODE_DEFAULT_WORK_TIME, {
        {SUCTION_SPEED_LOW, SUCTION_LEVEL_11KPA, SUCTION_MODE_SHORT},
        {1, STIM_MODE_DEFAULT_SLEEP_TIME, STIM_MODE_DEFAULT_HOLD_TIME}}
        },
};

/// 吸乳
static const motor_policy_cfg_t pump_default[PUMP_MODE_LEN] = {
    {MOTOR_TIME_POLICY, NORMAL_MODE_DEFAULT_WORK_TIME, {
    {SUCTION_SPEED_LOW, SUCTION_LEVEL_28KPA, SUCTION_MODE_LONG},
    {1, PUMP_MODE_DEFAULT_SLEEP_TIME, PUMP_MODE_DEFAULT_HOLD_TIME}}
        },
};

/// 混合
static const motor_policy_cfg_t mix_default[MIX_MODE_LEN] = {
    {MOTOR_TIME_POLICY, NORMAL_MODE_DEFAULT_WORK_TIME, {
        {SUCTION_SPEED_LOW, SUCTION_LEVEL_11KPA, SUCTION_MODE_SHORT},
        {5, STIM_MODE_DEFAULT_SLEEP_TIME, STIM_MODE_DEFAULT_HOLD_TIME}}
       },
    {MOTOR_TIME_SUB_POLICY, INVALID_WORK_TIME, {
        {SUCTION_SPEED_LOW, SUCTION_LEVEL_28KPA, SUCTION_MODE_LONG},
        {1, PUMP_MODE_DEFAULT_SLEEP_TIME, PUMP_MODE_DEFAULT_HOLD_TIME}}
       },
};

/// 个性化默认
static const motor_policy_cfg_t custom_default[CUSTOM_MODE_LEN] = {
    {MOTOR_TIME_POLICY, CUSTOM_MODE_DEFAULT_MASSAGE_TIME, {
        {SUCTION_SPEED_LOW, SUCTION_LEVEL_11KPA, SUCTION_MODE_SHORT},
        {1, STIM_MODE_DEFAULT_SLEEP_TIME, STIM_MODE_DEFAULT_HOLD_TIME}}
        },
    {MOTOR_TIME_POLICY, CUSTOM_MODE_DEFAULT_PUMP_TIME, {
        {SUCTION_SPEED_LOW, SUCTION_LEVEL_28KPA, SUCTION_MODE_LONG},
        {1, PUMP_MODE_DEFAULT_SLEEP_TIME, PUMP_MODE_DEFAULT_HOLD_TIME}}
        },
};

/// PP追奶
static const motor_policy_cfg_t pp_milk_track_default[PP_MODE_LEN] = {
    {MOTOR_TIME_POLICY, MILK_TRACK_MODE_PUMP_TIME_0, {
        {SUCTION_SPEED_HIGH, SUCTION_LEVEL_28KPA, SUCTION_MODE_LONG},
        {1, PUMP_MODE_DEFAULT_SLEEP_TIME, PUMP_MODE_DEFAULT_HOLD_TIME}}
        },
    {MOTOR_TIME_POLICY, MILK_TRACK_MODE_IDLE_TIME, {
        {SUCTION_SPEED_HIGH, SUCTION_LEVEL_27KPA, SUCTION_MODE_IDLE},
        {1, PUMP_MODE_DEFAULT_SLEEP_TIME, PUMP_MODE_DEFAULT_HOLD_TIME}}
        },
    {MOTOR_TIME_POLICY, MILK_TRACK_MODE_PUMP_TIME_1, {
        {SUCTION_SPEED_HIGH, SUCTION_LEVEL_26KPA, SUCTION_MODE_LONG},
        {1, PUMP_MODE_DEFAULT_SLEEP_TIME, PUMP_MODE_DEFAULT_HOLD_TIME}}
        },
    {MOTOR_TIME_POLICY, MILK_TRACK_MODE_IDLE_TIME, {
        {SUCTION_SPEED_HIGH, SUCTION_LEVEL_25KPA, SUCTION_MODE_IDLE},
        {1, PUMP_MODE_DEFAULT_SLEEP_TIME, PUMP_MODE_DEFAULT_HOLD_TIME}}
        },
    {MOTOR_TIME_POLICY, MILK_TRACK_MODE_PUMP_TIME_1, {
        {SUCTION_SPEED_HIGH, SUCTION_LEVEL_24KPA, SUCTION_MODE_LONG},
        {1, PUMP_MODE_DEFAULT_SLEEP_TIME, PUMP_MODE_DEFAULT_HOLD_TIME}}
        },    
};

/// 2+8奶量提升
static const motor_policy_cfg_t milk_boost_default[MILK_BOOST_MODE_LEN] = {
    {MOTOR_TIME_POLICY, MILK_BOOST_MODE_DEFAULT_MASSAGE_TIME, {
        {SUCTION_SPEED_HIGH, SUCTION_LEVEL_11KPA, SUCTION_MODE_SHORT},
        {1, STIM_MODE_DEFAULT_SLEEP_TIME, STIM_MODE_DEFAULT_HOLD_TIME}}
        },
    {MOTOR_TIME_POLICY, MILK_BOOST_MODE_DEFAULT_PUMP_TIME, {
        {SUCTION_SPEED_HIGH, SUCTION_LEVEL_28KPA, SUCTION_MODE_LONG},
        {1, PUMP_MODE_DEFAULT_SLEEP_TIME, PUMP_MODE_DEFAULT_HOLD_TIME}}
        },
};

/// 开奶
static const motor_policy_cfg_t lactation_start_default[LACTATION_START_MODE_LEN] = {
    {MOTOR_TIME_POLICY, LACTATION_START_MODE_DEFAULT_MASSAGE_TIME, {
        {SUCTION_SPEED_MID, SUCTION_LEVEL_11KPA, SUCTION_MODE_SHORT},
        {1, STIM_MODE_DEFAULT_SLEEP_TIME, STIM_MODE_DEFAULT_HOLD_TIME}}
        },
    {MOTOR_TIME_POLICY, LACTATION_START_MODE_DEFAULT_PUMP_TIME, {
        {SUCTION_SPEED_MID, SUCTION_LEVEL_28KPA, SUCTION_MODE_LONG},
        {1, PUMP_MODE_DEFAULT_SLEEP_TIME, PUMP_MODE_DEFAULT_HOLD_TIME}}
        },
};

#ifdef __cplusplus
}
#endif

#endif 
