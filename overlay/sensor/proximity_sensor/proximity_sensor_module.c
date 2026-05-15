/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h"
#if IS_SUPPORT_DEVICE_PROXIMITY_SENSOR
#include "bsp_common.h"
#include "sensor_manager.h"
#include "proximity_sensor_module.h"

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static proximity_sensor_context_t *thiz = NULL;
static sensor_wrapper_data_t s_proximity_sensor_data;

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static bool proximity_is_worn(void)
{
    proximity_status_t status;

    if (thiz == NULL || thiz->priv.dev == NULL) {
        return false;
    }
    if (DEVICE_IOCTL_ARGS(thiz->priv.dev, OP_CODE_PROXIMITY_GET_STATUS, &status) != BSP_OK) {
        return false;
    }
    return status.is_worn;
}

static int proximity_get_status(proximity_status_t *status)
{
    if (status == NULL || thiz == NULL || thiz->priv.dev == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    return DEVICE_IOCTL_ARGS(thiz->priv.dev, OP_CODE_PROXIMITY_GET_STATUS, status);
}

static void dispatch_proximity_client(sensor_wrapper_data_t *sensor_data, void *user_data)
{
    sensor_evt_client_node_t *node = (sensor_evt_client_node_t *)user_data;

    EXCEPTION_CHECK_VOID(node);
    node->cb(node->user_data, (uint8_t *)sensor_data, sizeof(sensor_wrapper_data_t));
}

static void proximity_notify_push(hdr_post_args_t *args)
{
    sensor_manager_context_t *ctx;

    (void)args;
    if (thiz == NULL || thiz->priv.dev == NULL) {
        return;
    }

    if (DEVICE_IOCTL_ARGS(thiz->priv.dev, OP_CODE_PROXIMITY_GET_STATUS,
            &s_proximity_sensor_data.proximity_data) != BSP_OK) {
        return;
    }
    s_proximity_sensor_data.proximity_serial_num++;
    s_proximity_sensor_data.sensor_mask |= SENSOR_MASK_PROXIMITY;

    ctx = (sensor_manager_context_t *)find_module(ID_MODULE_SENSOR);
    if (ctx == NULL) {
        return;
    }

    /* 中断更新缓存后，通知已注册的 sensor 客户端（仅读 RAM，无 I2C） */
    list_head_t *head = &ctx->priv.client_list;
    MUTEX_LOCK(ctx->priv.m_lock);
    LIST_FOR_EACH(pos, head) {
        sensor_evt_client_node_t *node = CONTAINER_OF(pos, sensor_evt_client_node_t, list);
        if (node->report_type == PROXIMITY_EVT_IRQ) {
            dispatch_proximity_client(&s_proximity_sensor_data, node);
        }
    }
    MUTEX_UNLOCK(ctx->priv.m_lock);
}

static void proximity_chip_notify(void)
{
    POST_TASK_HANDLER_DELAYED_ARG1(proximity_notify_push, 0, thiz, "proximity_notify");
}

static int on_post_proximity_init(void *ptr)
{
    sensor_manager_context_t *ctx = ptr;
    peripheral_bus_context_t *pbus;

    EXCEPTION_CHECK_INT(ctx);
    pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    EXCEPTION_CHECK_INT(pbus);

    thiz->priv.dev = pbus->find_device(ID_DEVICE_PROXIMITY_SENSOR);
    DEVICE_IOCTL_HAS_RET(thiz->priv.dev, OP_CODE_PROXIMITY_IS_SUPPORT, proximity_support);
    if (proximity_support <= 0) {
        LOG_SENSOR("proximity sensor not supported, skip init");
        return 0;
    }

    DEVICE_IOCTL_ARGS(thiz->priv.dev, OP_CODE_PROXIMITY_ADD_IRQ_CALLBACK, proximity_chip_notify);
    DEVICE_IOCTL_NO_RET(thiz->priv.dev, OP_CODE_PROXIMITY_ENABLE_IRQ);
    return 0;
}

void *register_module_for_proximity_sensor_new(list_head_t *list_head)
{
    MALLOC_FOR_PTR(ptr, proximity_sensor_context_t);
    ptr->is_worn = proximity_is_worn;
    ptr->get_status = proximity_get_status;
    thiz = ptr;
    return ADD_SENSOR("proximity_sensor", ID_SENSOR_PROXIMITY, on_post_proximity_init,
        ptr, list_head, sizeof(proximity_sensor_context_t));
}
#endif
