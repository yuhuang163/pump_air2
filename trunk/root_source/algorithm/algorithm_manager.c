/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "algorithm_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static algo_manager_context_t *algo_obj = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void on_sensor_data_algo_cb(void *user_data, uint8_t *data, int data_len)
{
    algo_manager_context_t *ctx = user_data;

    if (ctx->hook && ctx->hook->on_sensor_data_cb_route) {
        ctx->hook->on_sensor_data_cb_route(ctx, data, data_len);
    }
}

static int algo_version_get(void)
{
    return 0;
}

static void  hdr_broadcast_sys_boot_completed(hdr_post_args_t *args)
{
    algo_manager_context_t *ctx = algo_obj;
    sensor_manager_context_t *sensor = (sensor_manager_context_t *)find_module(ID_MODULE_SENSOR);
    sensor->add_sensor_evt_lisenser(ctx, IMU_EVT_POLLING, 1, on_sensor_data_algo_cb);
    // root_algo_init(ctx);
    ctx->priv.algo_inited = 1;
    LOG_ALGO("algo_inited ok, get_version: %d", algo_version_get());
}

static void algo_on_receive_internal(broadcast_mask_id_t id, void *context, int args1, int args2, broadcast_priv_data_t *pdata)
{
    algo_manager_context_t *ctx = context;

    switch (id.code) {
        case ID_BROADCAST_SYS_BOOT_COMPLETED:
            {
                LOG_ALGO("algo_inited");
                POST_TASK_HANDLER_DELAYED_ARG0(hdr_broadcast_sys_boot_completed, 0, "algo_fusion");
            }
            break;
        case ID_BROADCAST_PM_STATE_CHANGE: 
            {
                int pm_state = args1;
                if (pm_state == ID_SYS_PM_SUSPEND) {
                    LOG_ALGO("algo cannot use");
                } else {
                    /// 算法初始化完成才能向算法端传数据
                    if (ctx->priv.algo_inited == 1) {
                        // root_algo_reset(ctx);
                    }
                    LOG_ALGO("algo_inited:ok");
                }
            }
            break;
        case ID_BROADCAST_BREATH_PUMP_STATE_CHANGE:
            {
                LOGW("brush change: %d", args2);
            }
            break;

        default:
            break;
    }
}

static int on_perpare_algo_init(void *self)
{
    algo_manager_context_t *ctx = self;
    return 0;
}

static int on_post_algo_init(void *ptr)
{
    algo_manager_context_t *ctx = ptr;
    broadcast_obj_t *bdc = &ctx->priv.m_bcd;
    bdc->id.code |= ID_BROADCAST_PM_STATE_CHANGE;
    bdc->id.code |= ID_BROADCAST_SYS_BOOT_COMPLETED;
    bdc->id.code |= ID_BROADCAST_BREATH_PUMP_STATE_CHANGE;
    bdc->context = ctx;
    bdc->name = "algo_fusion";
    bdc->receiver_cb = algo_on_receive_internal;
    register_broadcast_receiver(bdc);

    return 0;
}

static algo_breath_pump_data_t *get_breath_pump_info(int index)
{
    if (index < ALGO_BREATH_PUMP_DATA_MAX) {
        return &algo_obj->priv.algo_breath_pump[index];
    }
    LOGE("error input index: %d", index);
    return NULL;
}
 
/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/


void *register_module_for_algo_fusion_new(list_head_t *list_head)
{
    MALLOC_FOR_PTR(ptr, algo_manager_context_t);

    ptr->get_version = algo_version_get;
    ptr->get_breath_pump_info = get_breath_pump_info;
    ptr->priv.algo_inited = 0;
    algo_obj = ptr;

    return ADD_MODULE("algo", ID_MODULE_ALGO_HUB, on_perpare_algo_init, 
        on_post_algo_init, ptr, list_head, sizeof(algo_manager_context_t));
}
 