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

/// 吸乳马达策略（常规模式）
typedef struct motor_policy {
    motor_para_t para;            /// 当前参数配置
    list_head_t para_list;        /// 通过链表实现混合模式
} motor_policy_t;

/// 吸乳马达定时策略（追奶模式）
typedef struct motor_time_policy {
    uint8_t para_num;             /// 子策略参数数量
    uint16_t hold_time_s;         /// 当前策略持续时间，单位s
    motor_policy_t policy;        /// 策略参数
    list_head_t policy_list;      /// 通过链表实现追奶模式，为NULL时该策略为常规策略
} motor_time_policy_t;

/// 吸乳马达总策略
typedef struct motor_total_policy {
    uint8_t time_policy_num;      /// 定时策略数量
    motor_time_policy_t body;     /// 定时策略实体
} motor_total_policy_t;

/// 吸乳马达状态
typedef struct {
    uint8_t  current_policy_id;      /// 当前策略ID（追奶模式）
    uint8_t  current_para_id;        /// 当前子参数ID（常规与追奶模式）
    uint16_t cycles_count;           /// 子参数循环计数，与cycles_num搭配，如：短吸5次后执行1次长吸
    uint32_t para_start_time;        /// 参数启用时间，单位ms
    uint32_t policy_start_time;      /// 策略启用时间，单位ms
} motor_policy_info_t;

#ifdef __cplusplus
}
#endif

#endif 
