/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "algorithm_manager.h"  // IWYU pragma: keep
#include "root_algo_core.h" // IWYU pragma: keep
#include "root_algo_acq_proc.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define LUX_LEVEL_BUFFER_SIZE 10  /// lux_level历史数据缓冲区大小

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
/// lux_level历史数据缓冲区，用于过滤和均值计算
static uint16_t lux_level_buffer[LUX_LEVEL_BUFFER_SIZE] = {0};
static uint8_t lux_level_buffer_count = 0;  /// 当前缓冲区中的数据个数
static uint8_t lux_level_buffer_index = 0;  /// 当前写入位置（循环缓冲区）

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
}

static void algo_light_handle(algo_manager_context_t *ctx, algo_light_sensor_data_input_t *input, algo_light_data_output_t *output)
{
    EXCEPTION_CHECK_VOID(ctx);
    EXCEPTION_CHECK_VOID(input);
    EXCEPTION_CHECK_VOID(output);
    
    uint16_t lux_level = input->lux_level;
    uint16_t filtered_avg = 0;
    
    /// 将新的lux_level添加到缓冲区（循环缓冲区）
    lux_level_buffer[lux_level_buffer_index] = lux_level;
    lux_level_buffer_index = (lux_level_buffer_index + 1) % LUX_LEVEL_BUFFER_SIZE;
    
    /// 更新缓冲区计数
    if (lux_level_buffer_count < LUX_LEVEL_BUFFER_SIZE) {
        lux_level_buffer_count++;
    }
    
    /// 如果数据量少于3个，直接使用当前值（无法过滤最大最小值）
    if (lux_level_buffer_count < 3) {
        filtered_avg = lux_level;
    } else {
        /// 从循环缓冲区中提取有效数据（按时间顺序）
        uint16_t valid_buffer[LUX_LEVEL_BUFFER_SIZE];
        uint8_t valid_count = lux_level_buffer_count;
        
        /// 如果缓冲区已满，从当前索引开始提取最近的数据
        if (lux_level_buffer_count == LUX_LEVEL_BUFFER_SIZE) {
            for (uint8_t i = 0; i < valid_count; i++) {
                uint8_t idx = (lux_level_buffer_index + i) % LUX_LEVEL_BUFFER_SIZE;
                valid_buffer[i] = lux_level_buffer[idx];
            }
        } else {
            /// 缓冲区未满，直接复制
            for (uint8_t i = 0; i < valid_count; i++) {
                valid_buffer[i] = lux_level_buffer[i];
            }
        }
        
        /// 找到最大值和最小值，同时计算总和
        uint16_t min_val = valid_buffer[0];
        uint16_t max_val = valid_buffer[0];
        uint32_t sum = valid_buffer[0];
        
        for (uint8_t i = 1; i < valid_count; i++) {
            if (valid_buffer[i] < min_val) {
                min_val = valid_buffer[i];
            }
            if (valid_buffer[i] > max_val) {
                max_val = valid_buffer[i];
            }
            sum += valid_buffer[i];
        }
        
        /// 过滤最大最小值，计算平均值
        /// 如果最大值和最小值相同，说明所有值都相同，直接使用该值
        if (min_val == max_val) {
            filtered_avg = min_val;
        } else {
            /// 减去最大值和最小值，计算剩余值的平均值
            sum = sum - min_val - max_val;
            filtered_avg = sum / (valid_count - 2);
        }
    }
    
    /// 更新输出数据
    output->timestamp = input->timestamp;
    /// 这里可以根据filtered_avg计算setting_level和pwm_duty
    /// 暂时保持原有逻辑，后续可以根据需求添加映射关系
    //ADC最大1.8v，最小0.3v，1.5v对应100，0.3v对应0
    //filtered_avg对应0-100
    //filtered_avg = (filtered_avg - 300) * 100 / (1800 - 300);
    output->pwm_duty = (filtered_avg - 300) * 100 / (1800 - 300);
    output->setting_level = output->pwm_duty/10;
    
    //LOG_ALGO("algo_light_handle: raw_lux=%d, filtered_avg=%d, buffer_count=%d, pwm_duty=%d, setting_level=%d", 
    //         lux_level, filtered_avg, lux_level_buffer_count, output->pwm_duty, output->setting_level);
}

static void on_light_sensor_data_algo_cb(void *user_data, uint8_t *data, int data_len)
{
    algo_manager_context_t *ctx = user_data;
    EXCEPTION_CHECK_VOID(ctx);
    sensor_wrapper_data_t *sensor_data = (sensor_wrapper_data_t *)data;
    algo_light_sensor_data_input_t data_input = {
        .timestamp = get_timestamp_ms(0),   //todo
        .lux_level = sensor_data->light_sensor_data.lux_level,
    };
    algo_light_handle(ctx, &data_input, &ctx->priv.light_algo_data);
    //LOG_ALGO("on_light_sensor_data_algo_cb: lux_level: %d", data_input.lux_level);
}

static void  hdr_broadcast_sys_boot_completed(hdr_post_args_t *args)
{
    algo_manager_context_t *ctx = args->user_data;
    EXCEPTION_CHECK_VOID(ctx);
    sensor_manager_context_t *sensor = (sensor_manager_context_t *)find_module(ID_MODULE_SENSOR);
    sensor->add_sensor_evt_lisenser(ctx, AIR_PRESSURE_EVT_POLLING, 1, on_sensor_data_algo_cb);
    sensor->add_sensor_evt_lisenser(ctx, LIGHT_SENSOR_EVT_POLLING, 1, on_light_sensor_data_algo_cb);
    ctx->priv.algo_inited = 1;
    LOG_ALGO("algo_inited ok, get_version: %d", ctx->get_algo_version());
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
