#ifndef __HAL_MOTOR_H__
#define __HAL_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <stdint.h>

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
 * @brief 描述电机操作码
*/
typedef enum {
    OP_CODE_MOTOR_START_PLAY = 1,
    OP_CODE_MOTOR_STOP_PLAY = 2,
    OP_CODE_MOTOR_SET_PARAMETER = 3,
    OP_CODE_MOTOR_GET_STATUS = 4,
    OP_CODE_MOTOR_RESET = 5,
    OP_CODE_MOTOR_SET_POWER = 6,
    OP_CODE_MOTOR_ADD_CB_UPGRADE = 7,
    OP_CODE_MOTOR_ADD_CB_CALI = 8,
    OP_CODE_MOTOR_ADD_CB_ERROR = 9,
    OP_CODE_MOTOR_SET_PWM_DUTY = 10,
    OP_CODE_MOTOR_DUTY_CLIMB = 11,
} motor_dev_op_code_e;

/**
 * @brief 描述电机控制类型
*/
typedef enum {
    PWM_PUMP_1 = 0,
    PWM_PUMP_2,
    PWM_RELIEF_VALVE_1,
    PWM_RELIEF_VALVE_2,
    PWM_ID_MAX,
} pwm_control_id_e;

/**
 * @brief 描述吸奶器控制类型
*/
typedef enum {
    PUMP_DATA_CONTROL = 0,        // 透传数据控制
    PUMP_LOOKUP_TABLE_CONTROL,    // 查表控制
} pump_contorl_type_e;

/**
 * @brief 负压参数，电池阀动作时间与负压力度成正比
*/
typedef struct {
    uint8_t  pump_level;    /// 泵力度
    uint16_t pumping_time;  /// 抽气时间
    uint16_t valve_on_time; /// 电磁阀动作时间
} negative_pressure_para_t;

/**
 * @brief 描述吸奶器电机参数
*/
typedef struct {
    negative_pressure_para_t n_pressure; /// 负压参数
    uint16_t pump_hold_time;     /// 负压保持时间
} pump_motor_para_t;

/**
 * @brief 描述pwm配置
*/
typedef struct {
    uint8_t duty;
    uint16_t freq;
} pwm_cfg_t;

/**
 * @brief 描述电机混合参数
*/
typedef struct {
    union {
        pump_motor_para_t pump;
        pwm_cfg_t pwm;
    } un; 
    uint16_t cycles_num;       /// 循环次数
    uint16_t cycles_interval;  /// 循环间隔
} motor_para_t;
 
/**
 * @brief 描述电机配置
*/
typedef struct {  
    uint8_t new_time_policy;   /// 新的定时策略
    uint8_t speed;      /// 负压生成速度，低/中/高
    uint8_t level;      /// 负压等级 0：10KPA  1：11KPA，范围：10-39KPA
    uint16_t cycles_num;       /// 循环次数
    uint16_t cycles_interval;  /// 循环间隔
    uint16_t pump_hold_time;   /// 负压时间, 单位ms
    uint16_t hold_time_s;      /// 策略保持时间， 单位s
} motor_cfg_t;

#ifdef __cplusplus
}
#endif

#endif 
