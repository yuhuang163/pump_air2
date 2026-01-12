/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "algorithm_manager.h"  // IWYU pragma: keep
#include "root_algo_core.h" // IWYU pragma: keep
#include "root_algo_acq_proc.h" // IWYU pragma: keep
#include "algo_port.h"
/*
* DEFINES
*****************************************************************************************
*/
#define ALGOLIB_PORT_SUPPORT 1
#define ALGOLIB_PORT_SUPPORT_LOG 0  

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static algo_manager_context_t *algo_ctx = NULL;

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
    EXCEPTION_CHECK_VOID(ctx);
    if (ctx->priv.is_sampling) {
        algo_sampling_data_cb(ctx, data, data_len);
    }
#if ALGOLIB_PORT_SUPPORT
    algo_port_pressure_data_t pressure_data = {0};
    sensor_wrapper_data_t *sensor_data = (sensor_wrapper_data_t *)data;
    for (int i = 0; i < AIR_PRESSURE_NUM; i++) {
        pressure_data.pressure_union.pressure[i] = sensor_data->air_pressure_data[i].pressure;
    }
    algo_port_set_data_to_ring_buffer(
        ALGO_PORT_DATA_TYPE_PRESSURE,
        sizeof(pressure_data.pressure_union.pressure_data),
        (uint8_t *)&pressure_data.pressure_union.pressure_data);
    algo_port_msg_type_queue_t msg = {
        .type = ALGO_PORT_DATA_TYPE_PRESSURE,
        .data = NULL,
        .size = 0,
    };
#if ALGOLIB_PORT_SUPPORT_LOG
    LOG_ALGO("on_sensor_data_algo_cb: pressure_data: %f, %f", 
              pressure_data.pressure_union.pressure[0], pressure_data.pressure_union.pressure[1]);
#endif
    if (algo_port_run_trigger(&msg) != 0) {
        LOG_ALGO("on_sensor_data_algo_cb: algo_port_run_trigger failed");
    }
#endif
}

#if ALGOLIB_PORT_SUPPORT
static int on_algo_pressure_result_cb(void *user_data, algo_port_result_t *result)
{
    if (result->data_type == ALGO_PORT_DATA_TYPE_PRESSURE) {
#if ALGOLIB_PORT_SUPPORT_LOG
        LOG_ALGO("on_algo_pressure_result_cb: pressure: timestamp: %d, "
               "wear_detection_result: %d, %d",
               result->data_union.pressure_result.timestamp,
               result->data_union.pressure_result.wear_detection_result[0],
               result->data_union.pressure_result.wear_detection_result[1]);
#endif
               algo_ctx->priv.wear_detection_algo_data.timestamp =
          result->data_union.pressure_result.timestamp;
        /// 将气压传感器数据保存到算法上下文中
        memcpy(
          algo_ctx->priv.wear_detection_algo_data.wear_detection_result,
          result->data_union.pressure_result.wear_detection_result,
          sizeof(
              algo_ctx->priv.wear_detection_algo_data.wear_detection_result));
    }
    else if (result->data_type == ALGO_PORT_DATA_TYPE_LIGHT) {
#if ALGOLIB_PORT_SUPPORT_LOG
        LOG_ALGO("on_algo_light_result_cb: timestamp: %d, pwm_duty: %d, setting_level: %d", 
              result->data_union.light_result.timestamp, result->data_union.light_result.pwm_duty, result->data_union.light_result.setting_level);
#endif
              /// 将光传感器数据保存到算法上下文中
        algo_ctx->priv.light_algo_data.timestamp = result->data_union.light_result.timestamp;
        algo_ctx->priv.light_algo_data.setting_level = result->data_union.light_result.setting_level;
        algo_ctx->priv.light_algo_data.pwm_duty = result->data_union.light_result.pwm_duty;
    }
    else {
        LOG_ALGO("on_algo_result_cb: unknown data type: %d", result->data_type);
        return -1;
    }
    return 0;
}
#endif

#if IS_SUPPORT_DEVICE_LIGHT_SENSOR
static void on_light_sensor_data_algo_cb(void *user_data, uint8_t *data, int data_len)
{
    algo_manager_context_t *ctx = user_data;
    EXCEPTION_CHECK_VOID(ctx);
    sensor_wrapper_data_t *sensor_data = (sensor_wrapper_data_t *)data;
    algo_light_sensor_data_input_t data_input = {
        .timestamp = get_timestamp_ms(0),   //todo
        .lux_level = sensor_data->light_sensor_data.lux_level,
    };

    #if ALGOLIB_PORT_SUPPORT
    //LOG_ALGO("on_light_sensor_data_algo_cb: lux_level: %d", data_input.lux_level);
    algo_port_light_data_t light_data = {
        .light_union.light = data_input.lux_level,
    };
    algo_port_set_data_to_ring_buffer(
        ALGO_PORT_DATA_TYPE_LIGHT,
        sizeof(light_data.light_union.light_data),
        (uint8_t *)&light_data.light_union.light_data);
    algo_port_msg_type_queue_t msg = {
        .type = ALGO_PORT_DATA_TYPE_LIGHT,
        .data = NULL,
        .size = 0,
    };
    if (algo_port_run_trigger(&msg) != 0) {
        LOG_ALGO("on_light_sensor_data_algo_cb: algo_port_run_trigger failed");
    }
    #endif
}
#endif

static void  hdr_broadcast_sys_boot_completed(hdr_post_args_t *args)
{
    algo_manager_context_t *ctx = args->user_data;
    EXCEPTION_CHECK_VOID(ctx);
    sensor_manager_context_t *sensor = (sensor_manager_context_t *)find_module(ID_MODULE_SENSOR);
    sensor->add_sensor_evt_lisenser(ctx, AIR_PRESSURE_EVT_POLLING, 1, on_sensor_data_algo_cb);
    #if IS_SUPPORT_DEVICE_LIGHT_SENSOR
    sensor->add_sensor_evt_lisenser(ctx, LIGHT_SENSOR_EVT_POLLING, 1, on_light_sensor_data_algo_cb);
    #endif
    ctx->priv.algo_inited = 1;
    LOG_ALGO("algo_inited ok, get_version: %d", ctx->get_algo_version());

#if ALGOLIB_PORT_SUPPORT
    /// 启动气压传感器算法
    algo_port_run_trigger_t run_trigger = {
        .type = ALGO_PORT_DATA_TYPE_PRESSURE,
        .run_trigger = 1,
    };
    algo_port_start(&run_trigger);
    /// 启动光传感器算法
    run_trigger.type = ALGO_PORT_DATA_TYPE_LIGHT;
    algo_port_start(&run_trigger);
#endif
}

static int on_algo_receiver_route(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data)
{
    algo_manager_context_t *ctx = context;
    switch (id.code) {
        case ID_BROADCAST_SYS_BOOT_COMPLETED:
            {
                LOG_ALGO("algo_inited");
                POST_TASK_HANDLER_DELAYED_ARG1(hdr_broadcast_sys_boot_completed, 0, ctx, "algo_fusion");
            }
            break;
        case ID_BROADCAST_PM_STATE_CHANGE: 
            {
                if (arg1 == ID_SYS_PM_SUSPEND) {
                    LOG_ALGO("algo cannot use");
                } else {
                    if (ctx->priv.algo_inited == 1) {
                        // root_algo_reset(ctx);
                    }
                }
            }
            break;
        default:
            break;
    }
    return 0;
}

static int on_post_algo_init_route(algo_manager_context_t *ctx)
{
    algo_ctx = ctx;
#if ALGOLIB_PORT_SUPPORT
    algo_port_init((algo_port_result_cb)on_algo_pressure_result_cb, ctx);
#endif
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void register_algo_hook(algo_manager_context_t *ctx)
{
    ctx->hook = u_malloz(sizeof(struct __algo_manager_hook));
    ctx->hook->on_post_init_route = on_post_algo_init_route;
    ctx->hook->on_algo_receiver_route = on_algo_receiver_route;
#if IS_SUPPORT_ALGO_SAMPLING
    ctx->hook->algo_sampling_start = algo_sampling_start;
    ctx->hook->algo_sampling_stop = algo_sampling_stop;
    ctx->hook->algo_sampling_is_sampling = algo_sampling_is_sampling;
    ctx->hook->algo_change_sampling_interval = algo_change_sampling_interval;
#endif
}
