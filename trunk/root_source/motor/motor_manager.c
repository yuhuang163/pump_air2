/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h"
#include "motor_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define MOTOR_PLAY_THD_WAKELOCK                            "motor_paly_wakelock"
#define MOTOR_PLAY_WAKELOCK_TIMEOUT                        (15 * 60)
#define DEBUG_MOTOR_TASK_PLAY                              1

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static motor_manager_context_t *motor_obj = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void motor_play_work_thread(void *args)
{
    motor_task_play_para_t play_para = {0};
    thread_context_t *thd = (thread_context_t *)args;
    power_manager_context_t *pm = (power_manager_context_t *)find_module(ID_MODULE_POWER);
    motor_manager_context_t *ctx = motor_obj;
    CREATE_MSG_TYPE_QUEUE(&thd->msg_id, MOTOR_PLAY_MESQ_NUM, sizeof(motor_task_play_para_t));
    for (;;) {
        set_thread_running(thd, false);
        LOG_MOTOR("idle");
        MSG_QUEUE_RECV(thd->msg_id, &play_para, OS_WAIT_FOREVER);
        LOG_MOTOR("running timestamp: %ld, id: %d", play_para.timestamp, play_para.play_mask_id);
        set_thread_running(thd, true);
        if (pm->acquire_wakelock(MOTOR_PLAY_THD_WAKELOCK, (MOTOR_PLAY_WAKELOCK_TIMEOUT)) == 0) {
        } else {
            LOGE("acquire wakelock fail");
        }

        if (pm->release_wakelock(MOTOR_PLAY_THD_WAKELOCK) < 0) {
            LOGE("release wakelock fail");
        }
    } 
}

static void update_play_interrupt_reason(const char *reason)
{
    if (motor_obj->priv.interrupt_play_reason != reason) {
        motor_obj->priv.interrupt_play_reason = (char *)reason;
    }
}

static int breath_pump_policy_register_internal(void)
{
    return 0;
}

static void start_motor_with_user_para(pump_contorl_type_e type, void *user_data)
{
    uint32_t *tmp = (uint32_t *)user_data;
    uint32_t args[2] = {0};
    EXCEPTION_CHECK_VOID(tmp);
    DEVICE_IOCTL_ARGS(motor_obj->priv.motor_dev, OP_CODE_MOTOR_SET_PARAMETER, args);
}

static int internal_common_mode_start()
{
    int ret = 0;
    rt_device_node_t *m = motor_obj->priv.motor_dev;

    if (m) {
        LOG_MOTOR("start");
        DEVICE_IOCTL(m, OP_CODE_MOTOR_START_PLAY);
        // uint32_t args[] = {PUMP_DATA_CONTROL, (uint32_t) &motor_obj->priv.current_policy};
        // ret = DEVICE_IOCTL_ARGS(m, OP_CODE_MOTOR_SET_PARAMETER, args);
    }
    return ret;
}

static int internal_policy_mode_start(uint8_t action)
{
    if (action >= MOTOR_POLICY_ACTION_MAX) {
        action = MOTOR_POLICY_START;
    }
    
    motor_task_play_para_t play_para = {
        .timestamp = get_timestamp(),
        .play_mask_id = ID_MASK_PLAY_BREATH_PUMP_POLICY,
        .para.pump_policy.action_id = action, 
    };

    os_message_id msg_id = motor_obj->priv.motor_play_thd.msg_id;
    update_play_interrupt_reason("play-new-task");
    if (msg_id) {
        LOG_MOTOR("timestamp: %ld, id: %d", play_para.timestamp, play_para.play_mask_id);
        MSG_QUEUE_SEND(msg_id, &play_para, 0);
    }

    return 0;
}
 
static int internal_start_motor(uint8_t action)
{
    uint32_t curr_mode = motor_obj->priv.pump_ctx->get_breath_pump_mode();
    switch (curr_mode) {
        case BREATH_PUMP_MODE_STIMULATION:
        case BREATH_PUMP_MODE_BREASTFEEDING:
        case BREATH_PUMP_MODE_HYBRID:
        case BREATH_PUMP_MODE_PERSONALIZED:
        case BREATH_PUMP_MODE_PP:
        case BREATH_PUMP_MODE_UPGRADE:
        case BREATH_PUMP_MODE_MILK_INIT:
            internal_policy_mode_start(action);
            break;
        default:
            internal_common_mode_start();
            break;
    }
    return 0;
}
 
static int internal_stop_motor(void)
{
    int ret = -1;

    if (motor_obj->priv.motor_dev && motor_obj->priv.motor_dev->ioctl) {
        ret = DEVICE_IOCTL(motor_obj->priv.motor_dev, OP_CODE_MOTOR_STOP_PLAY);
    }
    LOG_MOTOR("ret: %d", ret);

    return ret;
}
 
static void on_motor_broadcast_receiver(broadcast_mask_id_t id, void *context, int arg1, int arg2, broadcast_priv_data_t *pdata)
{
    motor_manager_context_t *thiz = context;
    struct __motor_manager_hook *hook = thiz->hook;
    int breath_pump_state = -1;
    int breath_pump_mode = -1;

    if (hook && hook->on_motor_receiver_route && (hook->on_motor_receiver_route(id, thiz, arg1, arg2, pdata))) {
        LOG_MOTOR("routed return: %lx", id.code);
        return;
    }

    switch (id.code) {
        case ID_BROADCAST_BREATH_PUMP_STATE_CHANGE:
            {
                breath_pump_state = arg2;
                LOG_MOTOR("state change: %d", breath_pump_state);
            }
            break;
        case ID_BROADCAST_BREATH_PUMP_MODE_CHANGE:
            {
                breath_pump_mode = arg1;
                internal_start_motor(MOTOR_POLICY_CHANGE_MODE);
                LOG_MOTOR("mode change: %d", breath_pump_mode);
            }
            break;
        case ID_BROADCAST_PM_STATE_CHANGE:
            {
                if (arg1 == ID_SYS_PM_RESUME) {
                    LOG_MOTOR("reset history_node");
                }
            }
            break;
        case ID_BROADCAST_REQ_PM_SUSPEND_BY_USER:
            {
                update_play_interrupt_reason("SUSPEND_BY_USER");
            }
            break;
        
        default:
            break;
    }
}

static void on_sensor_data_motor_callback(void *user_data, uint8_t *data, int data_len)
{

}

static int on_post_motor_init(void *ptr)
{
    motor_manager_context_t *thiz = (motor_manager_context_t *)ptr;
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    thiz->priv.pump_ctx = (breath_pump_manager_context_t *)find_module(ID_MODULE_BREATH_PUMP);
    thiz->priv.motor_dev = pbus->find_device(ID_DEVICE_MOTOR);

    /// 初始化模式配置参数
    if (thiz->hook && thiz->hook->normal_mode_config_pasrse) {
        thiz->hook->normal_mode_config_pasrse();
    }

    // thiz->curr_motor_cfg = &thiz->motor_cfg.smart[0];
    // brush_mode_config_init(thiz);      /// 各模式参数拆解

    /// 注册广播监听.
    thiz->priv.m_bdc.context = thiz;
    thiz->priv.m_bdc.name = "motor_manager";
    thiz->priv.m_bdc.id.code |= ID_BROADCAST_BREATH_PUMP_STATE_CHANGE;
    thiz->priv.m_bdc.id.code |= ID_BROADCAST_BREATH_PUMP_MODE_CHANGE;
    thiz->priv.m_bdc.id.code |= ID_BROADCAST_PM_STATE_CHANGE;
    thiz->priv.m_bdc.id.code |= ID_BROADCAST_UPDATE_MOTOR_WORK_PARAMETER;
    thiz->priv.m_bdc.id.code |= ID_BROADCAST_REQ_PM_SUSPEND_BY_USER;
    thiz->priv.m_bdc.receiver_cb = on_motor_broadcast_receiver;
    register_broadcast_receiver(&thiz->priv.m_bdc);

    /// 注册sensor数据.
    sensor_manager_context_t *sensor = (sensor_manager_context_t *)find_module(ID_MODULE_SENSOR);
    sensor->add_sensor_evt_lisenser(thiz, F_SENSOR_EVT_POLLING, 1, on_sensor_data_motor_callback);

    /// 吸奶策略注册.
    breath_pump_policy_register_internal();
    MUTEX_INIT(&thiz->priv.motor_play_thd.lock_id, m_motor_play_lock);
    RUN_NEW_THREAD(&thiz->priv.motor_play_thd, "motor_play_thd", motor_play_work_thread, 
                   RT_PRIO_MOTOR_DISPATCH_THREAD, MOTOR_PLAY_THREAD_STACKSIZE);

    return 0;
}

static int start_motor(motor_policy_action_e action)
{
    motor_obj->priv.is_user_stoped = 0;
    return internal_start_motor(MOTOR_POLICY_START);   
}

static int stop_motor(void)
{
    motor_obj->priv.is_user_stoped = 1;
    return internal_stop_motor();
}

static int get_motor_state(void)
{
    return (int)motor_obj->priv.motor_state;
}

static bool is_on_motor_task_running(motor_manager_context_t *ctx)
{
    EXCEPTION_CHECK_BOOL(ctx);
    return get_thread_running(&ctx->priv.motor_play_thd);
}

int motor_is_can_play_now(void)
{
    int is_on_pumping = motor_obj->priv.pump_ctx->is_on_working();

    if (motor_obj->priv.interrupt_play_reason) {
        LOG_MOTOR("exit reason: %s", motor_obj->priv.interrupt_play_reason);
        return 0;
    }

    if (is_on_pumping == 0) {
        LOG_MOTOR("exit reason: not on pumping");
        update_play_interrupt_reason("pumping_state:err");
        return 0;
    }
    return 1;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void *register_module_for_motor_manager_new(list_head_t *list_head)
{
    MALLOC_FOR_PTR(ptr, motor_manager_context_t);
    ptr->get_motor_state = get_motor_state;
    ptr->start_motor = start_motor;
    ptr->stop_motor = stop_motor;
    ptr->is_on_motor_task_running = is_on_motor_task_running;
    ptr->start_motor_with_user_para = start_motor_with_user_para;
    ptr->is_can_play_now = motor_is_can_play_now;
    motor_obj = ptr;
    
    return ADD_MODULE("motor", ID_MODULE_MOTOR, 0, on_post_motor_init, 
        ptr, list_head, sizeof(motor_manager_context_t));
}
