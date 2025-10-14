/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "persist_storage.h"
#include "root_common_api.h"
#include "breath_pump_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define PUMP_LOCK_LOG(...)
#define PUMP_THD_WAKELOCK                     "pump_wakelock"

#define MUTEX_LOCK_BHT(__tag, __lock) \
do { \
    PUMP_LOCK_LOG("Take lock=>__tag: %s 00", __tag); \
    MUTEX_LOCK(__lock); \
} while (0)

#define MUTEX_UNLOCK_BHT(__tag, __lock) \
do { \
    MUTEX_UNLOCK(__lock); \
    PUMP_LOCK_LOG("Give lock=>__tag: %s", __tag); \
} while (0)

typedef struct {
    void *user_data;
    on_breath_pump_result_cb_ptr cb;
    list_head_t list;
} breath_pump_evt_client_node_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static breath_pump_manager_context_t *breath_pump_obj = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void notify_breath_pump_completed(void)
{
    int need_save = 1;
    breath_pump_manager_context_t *ctx = breath_pump_obj;
    algo_manager_context_t *algo = (algo_manager_context_t *)find_module(ID_MODULE_ALGO_HUB);

    /// 外部挂接点.
    struct __breath_pump_hook *hook = breath_pump_obj->hook;
    if (hook && hook->breath_pump_completed_route) {
        need_save = hook->breath_pump_completed_route(breath_pump_obj);
        LOG_ALGO("save history");
    }

    if (need_save) {
        algo_breath_pump_data_t *pumping_info = algo->get_breath_pump_info(ALGO_BREATH_PUMP_DATA_PUMPING);
        USER_DATA_TO_PRIV_DATA(pumping_info, pdata);
        SEND_BROADCAST_ARGS4_EXTRA(ID_BROADCAST_SYS_PERSIST_INFO_SAVE, 1, pdata, sizeof(pdata));
        LOG_PMP("save success, work valid time: %lu", pumping_info->work_valid_time);
    }
}

static int get_breath_pump_status_async(void)
{
    uint8_t status = breath_pump_obj->priv.breath_pump_info.status;

    if (status > BREATH_PUMP_FINISH) {
        LOG_PMP("breath pump state: %d", status);
        status = BREATH_PUMP_IDLE;
    }

    return status;
}

static void on_sensor_data_pump_callback(void *user_data, uint8_t *data, int data_len)
{

}

static void start_breath_pump_work(void)
{
    int status = get_breath_pump_status_async();
    breath_pump_manager_context_t *ctx = breath_pump_obj;

    if (ctx->hook && ctx->hook->start_breath_pump_work_route) {
        if (ctx->hook->start_breath_pump_work_route(ctx, status) < 0) {
            return;
        }
    }

    /// 注册sensor回调.
    sensor_manager_context_t *sensor = (sensor_manager_context_t *)find_module(ID_MODULE_SENSOR);
    sensor->add_sensor_evt_lisenser(&ctx->priv.m_result, F_SENSOR_EVT_POLLING, 1, on_sensor_data_pump_callback);

    /// 启动马达.
    CHECK_IF_VALID(ctx->priv.motor, {
        ctx->priv.motor->start_motor(MOTOR_POLICY_START);
    });

    /// 启动计时器
    STOP_TIMER(ctx->priv.breath_pump_work_timer);
    START_TIMER(ctx->priv.breath_pump_work_timer, BREATH_PUMP_WORK_STEP_TIME_MS);
}

static void stop_breath_pump_work(int breath_pump_state)
{
    breath_pump_manager_context_t *ctx = breath_pump_obj;

    /// 关闭马达.
    CHECK_IF_VALID(ctx->priv.motor, {
        ctx->priv.motor->stop_motor();
    });

    /// 停止计时器
    STOP_TIMER(ctx->priv.breath_pump_work_timer);

    /// 注销sensor回调.
    sensor_manager_context_t *sensor = (sensor_manager_context_t *)find_module(ID_MODULE_SENSOR);
    sensor->remove_sensor_evt_lisenser(on_sensor_data_pump_callback);

    if (ctx->hook && ctx->hook->stop_breath_pump_work_route) {
        ctx->hook->stop_breath_pump_work_route(ctx, breath_pump_state);
    }
}

static void breath_pump_work_timer_cb(os_timer_id *timer)
{
    uint8_t msg = 0;
    MSG_QUEUE_SEND(breath_pump_obj->priv.breath_pump_thd.msg_id, &msg, 0);
}

static void convent_algo_breath_pump_to_ui_breath_pump_result(algo_breath_pump_data_t *algo_brush)
{
    breath_pump_result_t *br = &breath_pump_obj->priv.m_result;
    br->elapse_time_sec = algo_brush->work_time_total / 1000;
    br->real_valid_time_sec = algo_brush->work_valid_time / 1000;
    br->work_time_total = algo_brush->work_time_total;
    br->breath_pump_state = breath_pump_obj->priv.breath_pump_info.status;
}

static void make_progress(void)
{
    int notify_id = -1;
    struct __breath_pump_hook *hook = breath_pump_obj->hook;

    if (hook && hook->make_progress_route) {
        // notify_id = hook->make_progress_route(ctx, algo_brush, ctx->get_breath_pump_mode(), ctx->get_breath_pump_level());
        // convent_algo_breath_pump_to_ui_breath_pump_result(algo_brush);
    }
}

static void breath_pump_work_thread(const void *args)
{
    uint8_t msg = 0;
    thread_context_t *thd = (thread_context_t *)args;
    CREATE_MSG_TYPE_QUEUE(&thd->msg_id, BREATH_PUMP_WORK_DISPATCH_MESQ_NUM, 1);
    breath_pump_manager_context_t *ctx = breath_pump_obj;
    CREATE_TIMER(&ctx->priv.breath_pump_work_timer, "breath_pump_work", ctx, OS_TIMER_PERIODIC, breath_pump_work_timer_cb);
    do {
        MSG_QUEUE_RECV(thd->msg_id, &msg, OS_WAIT_FOREVER);
        /// 进度单元.
        make_progress();
        /// callback结果给各个客户端,如更新UI，ble上报等.
        ctx->dispatch_to_client(0);

    } while (1);
}

static void on_breath_pump_broadcast_receiver(broadcast_mask_id_t id, void *context, int arg1, int arg2, broadcast_priv_data_t *pdata)
{
    breath_pump_manager_context_t *bth_ctx = context;
    
    switch (id.code) {
        case ID_BROADCAST_PM_STATE_CHANGE:
            break;
        default:
            break;
    }
}

static int on_post_breath_pump_init(void *ptr)
{
    breath_pump_manager_context_t *ctx = ptr;

    ctx->priv.breath_pump_info.mode                     = BREATH_PUMP_NORMAL_MODE_DEFAULT;      
    ctx->priv.breath_pump_info.level                    = 0;
    ctx->priv.breath_pump_info.do_work                  = 0;
    ctx->priv.breath_pump_info.status                   = BREATH_PUMP_IDLE;

    /// 注册广播监听.
    ctx->priv.m_bdc.context = ctx;
    ctx->priv.m_bdc.name = "breath_pump_manager";
    ctx->priv.m_bdc.id.code |= ID_BROADCAST_PM_STATE_CHANGE;
    ctx->priv.m_bdc.receiver_cb = on_breath_pump_broadcast_receiver;
    register_broadcast_receiver(&ctx->priv.m_bdc);
    RUN_NEW_THREAD(&ctx->priv.breath_pump_thd, "breath_pump_thd", breath_pump_work_thread, 
        RT_PRIO_BREATH_PUMP_DISPATCH_THREAD, BTH_THREAD_STACKSIZE);

    return 0;
}

static int on_perpare_breath_pump_init(void *self)
{
    breath_pump_manager_context_t *ctx = (breath_pump_manager_context_t *)self;
    ctx->priv.st = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    ctx->priv.motor = (motor_manager_context_t *)find_module(ID_MODULE_MOTOR);
    ctx->priv.pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);

    return 0;
}

static int local_is_on_working(void)
{
    return breath_pump_obj->priv.breath_pump_info.do_work == 1;
}

static void dispatch_to_client(uint32_t oneshot_mask)
{
    list_head_t *head = &breath_pump_obj->priv.client_list;
    breath_pump_result_t result = {0};

    MUTEX_LOCK(breath_pump_obj->priv.m_lock);

    memcpy(&result, &breath_pump_obj->priv.m_result, sizeof(breath_pump_result_t));
    if (oneshot_mask) {
        LOG_BLE("oneshot_mask %lu", oneshot_mask);
        result.oneshot_mask = oneshot_mask;
    }

    LIST_FOR_EACH(pos, (head)) {
        breath_pump_evt_client_node_t *node = CONTAINER_OF(pos, breath_pump_evt_client_node_t, list);
        if (node->cb) {
            node->cb(node->user_data, &result);
        }
    }

    MUTEX_UNLOCK(breath_pump_obj->priv.m_lock);
}

static void *list_del_condition_cb(void *data, list_head_t *pos)
{
    on_breath_pump_result_cb_ptr cb = (on_breath_pump_result_cb_ptr)data;
    breath_pump_evt_client_node_t *node = CONTAINER_OF(pos, breath_pump_evt_client_node_t, list);

    if ((node->cb) == (cb)) {
        return node;
    }
    return NULL;
}

static int local_add_result_listener(void *user_data, on_breath_pump_result_cb_ptr cb)
{
    list_head_t *head = &breath_pump_obj->priv.client_list;

    MUTEX_LOCK(breath_pump_obj->priv.m_lock);

    LIST_FOR_EACH(pos, (head)) {
        breath_pump_evt_client_node_t *node = CONTAINER_OF(pos, breath_pump_evt_client_node_t, list);
        if (node->cb == cb && node->user_data == user_data) {
            LOG_PMP("ignore duplicate add.");
            MUTEX_UNLOCK(breath_pump_obj->priv.m_lock);
            return -1;
        }
    }

    breath_pump_evt_client_node_t *node = u_malloz(sizeof(breath_pump_evt_client_node_t));
    node->cb = cb;
    node->user_data = user_data;
    __list_add(&node->list, head); 
    MUTEX_UNLOCK(breath_pump_obj->priv.m_lock);

    return 0;
}

static void local_remove_result_listener(on_breath_pump_result_cb_ptr cb)
{
    list_head_t *head = &breath_pump_obj->priv.client_list;

    MUTEX_LOCK(breath_pump_obj->priv.m_lock);
    __list_del(head, cb, list_del_condition_cb);
    MUTEX_UNLOCK(breath_pump_obj->priv.m_lock);
}

static void local_reset_breath_pump_result(void)
{
    memset(&breath_pump_obj->priv.m_result, 0, sizeof(breath_pump_result_t));
    LOG_PMP("reset result.");
}

static int local_get_breath_pump_level(void)
{
    return breath_pump_obj->priv.breath_pump_info.level;
}

static void local_set_breath_pump_level(breath_pump_mode_e mode)
{
    uint16_t level_info[16];
    breath_pump_obj->priv.st->get_store_val(STORE_ID_BREATH_PUMP_LEVEL, 0, 0, level_info);
    breath_pump_obj->priv.breath_pump_info.level = level_info[0];
}

static int local_get_breath_pump_mode(void)
{
    if (breath_pump_obj->priv.breath_pump_info.mode >= BREATH_PUMP_MODE_MAX ||
        breath_pump_obj->priv.breath_pump_info.mode == BREATH_PUMP_MODE_INVALID) {
        LOG_PMP("invalid, return default mode: %d", BREATH_PUMP_NORMAL_MODE_DEFAULT);
        return BREATH_PUMP_NORMAL_MODE_DEFAULT;
    }
    return breath_pump_obj->priv.breath_pump_info.mode;
}

static void local_set_breath_pump_mode(uint32_t mode)
{
    if (mode >= BREATH_PUMP_MODE_MAX || mode == BREATH_PUMP_MODE_INVALID) {
        breath_pump_obj->priv.breath_pump_info.mode = BREATH_PUMP_NORMAL_MODE_DEFAULT;
    } else {
        breath_pump_obj->priv.breath_pump_info.mode = mode;
    }
    local_set_breath_pump_level((breath_pump_mode_e)breath_pump_obj->priv.breath_pump_info.mode);
    LOG_PMP("new mode is: %d", breath_pump_obj->priv.breath_pump_info.mode);
    SEND_BROADCAST_ARGS2(ID_BROADCAST_BREATH_PUMP_MODE_CHANGE, breath_pump_obj->priv.breath_pump_info.mode, 1);
}

static int get_breath_pump_status(void)
{
    breath_pump_manager_context_t *ctx = breath_pump_obj;
    int status;

    MUTEX_LOCK_BHT("get_status", ctx->priv.breath_pump_state_lock);
    status = get_breath_pump_status_async();
    MUTEX_UNLOCK_BHT("get_status", ctx->priv.breath_pump_state_lock);

    return status;
}

static void set_breath_pump_status(int status)
{
    breath_pump_manager_context_t *ctx = breath_pump_obj;
    power_manager_context_t *pm_ctx = (power_manager_context_t *)find_module(ID_MODULE_POWER);

    LOG_PMP("enter work: %d", ctx->priv.breath_pump_info.do_work);
    MUTEX_LOCK_BHT("set_status", ctx->priv.breath_pump_state_lock);
    if (status >= BREATH_PUMP_IDLE && status <= BREATH_PUMP_FINISH) {
        if (ctx->priv.breath_pump_info.status != status) {
            ctx->priv.breath_pump_info.status = status;
            LOG_PMP("status: %d, do_work: %d", status, ctx->priv.breath_pump_info.do_work);

            switch (status) {
                case BREATH_PUMP_IDLE:
                    if (ctx->priv.breath_pump_info.do_work) {
                        ctx->priv.breath_pump_info.do_work = 0;
                        stop_breath_pump_work(status);
                        CHECK_IF_VALID(pm_ctx, {
                            pm_ctx->release_wakelock(PUMP_THD_WAKELOCK);
                        });
                    }
                    break;
                
                case BREATH_PUMP_START:
                    LOG_PMP("=>reset_breath_pump_result");
                    ctx->reset_breath_pump_result();

                case BREATH_PUMP_CONTINUE:
                    if (!ctx->priv.breath_pump_info.do_work) {
                        ctx->priv.breath_pump_info.do_work = 1;
                        CHECK_IF_VALID(pm_ctx, {
                            pm_ctx->acquire_wakelock(PUMP_THD_WAKELOCK, -1);
                        });
                        start_breath_pump_work();
                    }
                    break;

                case BREATH_PUMP_PAUSE:
                case BREATH_PUMP_FINISH:
                    if (ctx->priv.breath_pump_info.do_work) {
                        ctx->priv.breath_pump_info.do_work = 0;
                        stop_breath_pump_work(status);
                        CHECK_IF_VALID(pm_ctx, {
                            pm_ctx->release_wakelock(PUMP_THD_WAKELOCK);
                        });
                    }
        
                    if (status == BREATH_PUMP_FINISH) {
                        LOG_PMP("brush finish, =>save brush result.");
                        notify_breath_pump_completed();
                    }
                    break;

                case BREATH_PUMP_FACTORY:
                    notify_breath_pump_completed();
                    if (ctx->priv.breath_pump_info.do_work) {
                        ctx->priv.breath_pump_info.do_work = 0;
                        stop_breath_pump_work(status);
                    }
                    break;
            }

            SEND_BROADCAST_ARGS2(ID_BROADCAST_BREATH_PUMP_STATE_CHANGE, ctx->priv.breath_pump_info.do_work,
                ctx->get_breath_pump_status_async());
        }
    }

    MUTEX_UNLOCK_BHT("set_status", ctx->priv.breath_pump_state_lock);
    LOG_PMP("exit work: %d", ctx->priv.breath_pump_info.do_work);
}

const char *get_breath_pump_status_name(uint8_t status)
{
    switch (status) {
        default:
        case BREATH_PUMP_IDLE:
            return "#IDLE#";
        case BREATH_PUMP_START:
            return "#START#";
        case BREATH_PUMP_PAUSE:
            return "#PAUSE#";
        case BREATH_PUMP_CONTINUE:
            return "#CONTINUE#";
        case BREATH_PUMP_FINISH:
            return "#FINISH#";
        case BREATH_PUMP_FACTORY:
            return "#FACTORY#";
    }
}

const char *get_breath_pump_mode_name(uint8_t mode)
{
    switch (mode) {
        default:
            return "@unknow";
        case BREATH_PUMP_MODE_INVALID:
            return "@pause";
        case BREATH_PUMP_MODE_STIMULATION:
            return "@stimulation";
        case BREATH_PUMP_MODE_BREASTFEEDING:
            return "@breastfeeding";
        case BREATH_PUMP_MODE_HYBRID:
            return "@hybrid";
        case BREATH_PUMP_MODE_PERSONALIZED:
            return "@personalized";
        case BREATH_PUMP_MODE_PP:
            return "@pp_catch_up";
        case BREATH_PUMP_MODE_UPGRADE:
            return "@2+8_upgrade";
        case BREATH_PUMP_MODE_MILK_INIT:
            return "@milk_initiation";
    }
}

static const breath_pump_result_t *get_curr_breath_pump_result(void)
{
    breath_pump_manager_context_t *ctx = breath_pump_obj;
    return (const breath_pump_result_t *)&ctx->priv.m_result;
}

static int get_last_breath_pump_result(breath_pump_result_t *result, uint8_t breath_pump_data_id)
{
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void *register_module_for_breath_pump_new(list_head_t *list_head)
{
    MALLOC_FOR_PTR(ptr, breath_pump_manager_context_t);

    ptr->is_on_working = local_is_on_working;
    ptr->reset_breath_pump_result = local_reset_breath_pump_result;
    ptr->add_breath_pump_result_listener = local_add_result_listener;
    ptr->remove_breath_pump_result_listener = local_remove_result_listener;
    ptr->get_breath_pump_mode = local_get_breath_pump_mode;
    ptr->set_breath_pump_mode = local_set_breath_pump_mode;
    ptr->get_breath_pump_level = local_get_breath_pump_level;
    ptr->set_breath_pump_level = local_set_breath_pump_level;
    ptr->get_breath_pump_status_async = get_breath_pump_status_async;
    ptr->get_breath_pump_status = get_breath_pump_status;
    ptr->set_breath_pump_status = set_breath_pump_status;

    ptr->get_last_breath_pump_result = get_last_breath_pump_result;
    ptr->get_curr_breath_pump_result = get_curr_breath_pump_result;
    ptr->dispatch_to_client = dispatch_to_client;

    MUTEX_INIT(&ptr->priv.m_lock, breath_pump_client_mutex);
    MUTEX_INIT(&ptr->priv.breath_pump_state_lock, brusht_state_mutex);
    MUTEX_INIT(&ptr->priv.breath_pump_mode_lock, breath_pump_mode_lock);
    USE_INIT_LIST_HEAD(&ptr->priv.client_list);
    breath_pump_obj = ptr;

    return ADD_MODULE("brush", ID_MODULE_BREATH_PUMP, on_perpare_breath_pump_init, 
        on_post_breath_pump_init, ptr, list_head, sizeof(breath_pump_manager_context_t));
}
