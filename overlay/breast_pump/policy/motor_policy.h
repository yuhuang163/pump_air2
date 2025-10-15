#ifndef __MOTOR_POLICY_H__
#define __MOTOR_POLICY_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep
#include "root_list_api.h" // IWYU pragma: keep

/**
 * @brief 负压状态
*/
typedef enum {
    N_PRESSURE_IDLE = 0,
    N_PRESSURE_GENERATE, /// 生成
    N_PRESSURE_HOLD,     /// 保持
    N_PRESSURE_RELEASE,  /// 解除
    N_PRESSURE_SLEEP,    /// 休眠，
} negative_pressure_status_e;

/**
 * @brief 工作类型
*/
typedef enum {
    MOTOR_ACTION_IDLE = 0,
    MOTOR_PUMP_ON,
    MOTOR_PUMP_OFF,
    MOTOR_VALVE_ON,
    MOTOR_VALVE_OFF,
} motor_action_type_e;

/**
 * @brief 负压速度等级
*/
typedef enum {
    N_PRESSURE_LOW_SPEED = 0,
    N_PRESSURE_MID_SPEED,
    N_PRESSURE_HIGHT_SPEED,
    N_PRESSURE_SPEED_MAX
} negative_pressure_speed_e;

/**
 * @brief 负压等级表
*/
typedef enum {
    N_PRESSURE_10KPA = 0,
    N_PRESSURE_11KPA,
    N_PRESSURE_12KPA,
    N_PRESSURE_13KPA,
    N_PRESSURE_14KPA,
    N_PRESSURE_15KPA,
    N_PRESSURE_16KPA,
    N_PRESSURE_17KPA,
    N_PRESSURE_18KPA,
    N_PRESSURE_19KPA,
    N_PRESSURE_20KPA,
    N_PRESSURE_21KPA,
    N_PRESSURE_22KPA,
    N_PRESSURE_23KPA,
    N_PRESSURE_24KPA,
    N_PRESSURE_39KPA = 29,
    N_PRESSURE_LEVEL_MAX
} negative_pressure_level_e;

/**
 * @brief 负压参数，电池阀动作时间与负压力度成正比
*/
typedef struct {
    uint8_t  pump_level;    /// 泵力度
    uint16_t pumping_time;  /// 抽气时间
    uint16_t valve_on_time; /// 电磁阀动作时间
} negative_pressure_para_t;

/**
 * @brief 泵策略参数
*/
typedef struct {
    /// 配置
    negative_pressure_para_t n_pressure;
    uint16_t pump_hold_time;   /// 负压保持时间
    uint16_t cycles_interval;  /// 循环间隔
    uint16_t cycles_num;       /// 循环次数
    /// 状态
    uint8_t status;             /// 负压状态 negative_pressure_status_e
    uint8_t action;             /// 动作 motor_action_type_e
    uint8_t n_pressure_flag;    /// 负压标志，负压状态下不允许切换策略
    uint8_t policy_update_flag; /// 策略更新标志
    uint16_t cycles_count;      /// 循环计数
    uint32_t start_time;        /// 参数启用时间，单位ms
} negative_pressure_info_t;

/**
 * @brief 描述电机混合参数
*/
typedef struct {
    uint8_t speed;             /// 负压生成速度，negative_pressure_speed_e
    uint8_t level;             /// 负压等级 negative_pressure_level_e
    uint16_t pump_hold_time;   /// 负压保持时间，单位ms，
    uint16_t cycles_interval;  /// 循环间隔
    uint16_t cycles_num;       /// 循环次数
} motor_policy_para_t;

/**
 * @brief 描述电机配置
*/
typedef struct {  
    uint8_t new_time_policy;   /// 新的定时策略
    uint8_t speed;             /// 负压生成速度，低/中/高
    uint8_t level;             /// 负压等级 0：10KPA  1：11KPA，范围：10-39KPA
    uint16_t cycles_num;       /// 循环次数
    uint16_t cycles_interval;  /// 循环间隔
    uint16_t pump_hold_time;   /// 负压时间, 单位ms
    uint16_t hold_time_s;      /// 时间策略保持时间
} motor_policy_cfg_t;

/// 吸乳马达策略（常规模式）
typedef struct motor_policy {
    motor_policy_para_t para;            /// 当前参数配置
    struct motor_policy *next_policy; /// 下一个子策略
} motor_policy_t;

/// 吸乳马达定时策略（追奶模式）
typedef struct motor_time_policy {
    uint8_t para_num;             /// 子策略参数数量
    uint16_t hold_time_s;         /// 当前策略持续时间，单位s
    motor_policy_t policy;        /// 策略参数
    struct motor_time_policy *next_time_policy;  /// 下一个定时策略
} motor_time_policy_t;

/// 吸乳马达总策略
typedef struct motor_total_policy {
    uint8_t time_policy_num;      /// 定时策略数量
    motor_time_policy_t body;     /// 定时策略实体
} motor_total_policy_t;

/// 吸乳马达状态
typedef struct {
    motor_total_policy_t *total_obj;
    motor_time_policy_t *current_time_policy;
    motor_policy_t *current_policy;
    negative_pressure_info_t current_info;    /// 当前负压信息，有多个策略综合生成
    uint32_t time_policy_start_time;  /// 时间策略开始时间，单位ms
    uint32_t total_policy_start_time; /// 总策略开始时间，单位ms
} motor_policy_info_t;

/// 吸乳策略处理
typedef void (*breast_pump_policy_handler_cb_ptr)(void *user_data);
#ifdef __cplusplus
}
#endif

#endif 
