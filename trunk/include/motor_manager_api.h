#ifndef __MOTOR_MANAGER_API_H__
#define __MOTOR_MANAGER_API_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep
#include "broadcast_manager_api.h"
#include "motor_policy.h" // IWYU pragma: keep

typedef enum {
    NOTIFY_BREATH_PUMP_START,           ///开始泵奶
    NOTIFY_BREATH_PUMP_END
} breath_pump_notify_e;

typedef struct motor_cfg_map {
    motor_cfg_t stimulation[STIMULATION_MODE_LEN];     /// 刺激
    motor_cfg_t breastfeeding[BREASTFEEDING_MODE_LEN]; /// 吸乳
    motor_cfg_t hybrid[HYBRID_MODE_LEN];               /// 混合
    motor_cfg_t personalized[PERSONALIZED_MODE_LEN];    /// 个性化默认
    motor_cfg_t pp_catch_up[PP_MODE_LEN];     /// PP追奶
    motor_cfg_t upgrade[UPGRADE_MODE_LEN];         /// 2+8奶量提升
    motor_cfg_t milk_initiation[MILK_INIT_MODE_LEN]; /// 开奶
} motor_cfg_map_t;

/*
* 马达策略执行动作
*/
typedef enum {
    MOTOR_POLICY_START = 0,             /// 初始化启动
    MOTOR_POLICY_CONTINUE = 1,          /// 继续运行(同一个模式下)
    MOTOR_POLICY_CHANGE_MODE = 2,       /// 切换模式运行
    MOTOR_POLICY_ACTION_MAX
} motor_policy_action_e;

typedef int (*policy_finish_cb_fptr)(void *user_data);

/*
* 泵奶通用接口，泵奶策略
*/ 
typedef struct breath_pump_policy_prop {
    void *user_data;
    uint8_t action; 
    policy_finish_cb_fptr cb;
} breath_pump_policy_prop_t;

/*
* 描述motor管理对象
*/ 
struct motor_total_policy;
struct motor_manager_context;

typedef int (*breath_pump_policy_init_func)(struct motor_manager_context *ctx, struct motor_total_policy *policy_init);
typedef struct motor_total_policy *(*get_policy_obj_func)(void *user_data);
typedef void (*add_breath_pump_policy_fptr)(list_head_t *head, const char *name, int breath_pump_mode, 
    void *user_data, breath_pump_policy_init_func init_cb, get_policy_obj_func policy_cb);

/* 
* 每种策略节点容器描述.
*/
typedef struct breath_pump_policy_node {
    char *name;
    void *user_data;
    int breath_pump_mode;
    breath_pump_policy_init_func init_cb;
    get_policy_obj_func get_policy_cb;
    list_head_t list;
} breath_pump_policy_node_t;

/* 
* 每种策略类型
*/
typedef enum {
    ID_MASK_PLAY_INVALID = (-1),
    ID_MASK_PLAY_DUTY_CLIMB = 100,     /// duty_climb, 渐强振动
    ID_MASK_PLAY_BREATH_PUMP_POLICY = 101,   /// 各种泵奶策略
    ID_MASK_PLAY_END,
} motor_play_mask_id_e;

/* 
* 打断原因
*/
typedef enum {
    EXIT_REASON_EXCEPTION = -1,
    EXIT_REASON_NORMAL = 0,
    EXIT_REASON_PLAY_COMPLETED = 1,
    EXIT_REASON_PLAY_BY_USER = 2,
    EXIT_REASON_INTERRUPT = 99,
    EXIT_REASON_GOTO_NEXT_BASE = 100,
    EXIT_REASON_GOTO_BREATH_PUMP_POLICY,
} motor_run_exit_reason_e;

/// 马达播放请求参数.
typedef struct motor_task_play_para {
    uint32_t timestamp;
    /// 播放类型ID.
    int play_mask_id;
    /// 播放参数.
    union {
        /// 策略.
        struct __pump_policy_para {
            int action_id;
        } pump_policy;
    } para;
} motor_task_play_para_t;

#define MOTOR_WRITE_DATA(m, cmd0, cmd1, cmd2) \
    do { \
        uint32_t __wbuf[] = { (uint32_t)cmd0, (uint32_t)cmd1, (uint32_t)cmd2 }; \
        DEV_WRITE(m, __wbuf, sizeof(__wbuf)); \
    } while (0)

struct rt_device_node;
struct breath_pump_manager_context;
typedef struct motor_manager_context {
/// private
    struct __motor_private {
        struct breath_pump_manager_context *pump_ctx;
        /// 马达状态
        uint8_t motor_state;
        /// 模式切换
        uint8_t mode_switch;
        /// pwm爬升模式.
        uint8_t is_climb_running;
        /// 用户停止
        uint8_t is_user_stoped;
        /// 驱动节点
        struct rt_device_node *motor_dev;
        /// 马达参数集合.
        motor_cfg_map_t motor_cfg;
        /// 当前策略.
        motor_policy_info_t policy_info;
        /// 马达各种策略链表头指针.
        list_head_t breath_pump_policy_list;
        /// 监听泵奶模式改变广播.
        broadcast_obj_t m_bdc;
        /// 马达任务
        thread_context_t motor_play_thd;            /// 播放线程.
        char *interrupt_play_reason;                /// 中断播放标记原因.
    } priv;

/// public
    int (*get_motor_state)(void);
    int (*start_motor)(motor_policy_action_e action);
    int (*stop_motor)(void);
    void (*breath_pump_notify)(struct motor_manager_context *ctx, breath_pump_notify_e notify_id);  
    /// 当前是否允许播放.
    int (*is_can_play_now)(void);  
    /// 马达任务运行状态.
    bool (*is_on_motor_task_running)(struct motor_manager_context *ctx);
    /// 带参启动马达
    void (*start_motor_with_user_para)(pump_contorl_type_e type, void *user_data);
    /// 外部钩子函数.
    struct __motor_manager_hook {
        int (*breath_pump_policy_register)(struct motor_manager_context *ctx, add_breath_pump_policy_fptr add_policy_cb);
        /// 马达力度转换表索引
        int (*motor_level_convert_table_index)(int level);
        /// 获取常规模式马达默认参数
        int (*normal_mode_config_pasrse)(void);
        /// 获取常规模式马达默认参数
        int (*get_normal_mode_default_para)(int mode, int mode_idx, int side, motor_para_t *motor);
        /// 马达接收广播路由
        int (*on_motor_receiver_route)(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data);
        /// 马达参数生成器
        int (*generate_motor_para)(int mode, void *user_data, int data_size);
    } *hook;

} motor_manager_context_t;

#ifdef __cplusplus
}
#endif

#endif 


