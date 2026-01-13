/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "v3_light_ctrl.h"
#include "algo_port.h"
#include "algorithm_manager.h"
#include "root_common_api.h"
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
static unsigned int lux_level_buffer[LUX_LEVEL_BUFFER_SIZE] = {0};
static unsigned char lux_level_buffer_count = 0;  /// 当前缓冲区中的数据个数
static unsigned char lux_level_buffer_index = 0;  /// 当前写入位置（循环缓冲区）
/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void algo_light_handle(unsigned int lux_level_input, algo_port_light_result_t *output)
{
    unsigned int lux_level = lux_level_input;
    unsigned int filtered_avg = 0;
    
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
        unsigned int valid_buffer[LUX_LEVEL_BUFFER_SIZE];
        unsigned char valid_count = lux_level_buffer_count;
        
        /// 如果缓冲区已满，从当前索引开始提取最近的数据
        if (lux_level_buffer_count == LUX_LEVEL_BUFFER_SIZE) {
            for (unsigned char i = 0; i < valid_count; i++) {
                unsigned char idx = (lux_level_buffer_index + i) % LUX_LEVEL_BUFFER_SIZE;
                valid_buffer[i] = lux_level_buffer[idx];
            }
        } else {
            /// 缓冲区未满，直接复制
            for (unsigned char i = 0; i < valid_count; i++) {
                valid_buffer[i] = lux_level_buffer[i];
            }
        }
        
        /// 找到最大值和最小值，同时计算总和
        unsigned int min_val = valid_buffer[0];
        unsigned int max_val = valid_buffer[0];
        unsigned int sum = valid_buffer[0];
        
        for (unsigned char i = 1; i < valid_count; i++) {
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
    output->timestamp = 0;
    /// 这里可以根据filtered_avg计算setting_level和pwm_duty
    /// 暂时保持原有逻辑，后续可以根据需求添加映射关系
    //ADC最大1.8v，最小0.3v，1.8v对应100，0.3v对应0，电压越高，亮度越大
    //filtered_avg对应0-100
    //filtered_avg = (filtered_avg - 300) * 100 / (1800 - 300);
    output->pwm_duty = (filtered_avg - 300) * 100 / (1800 - 300);
    output->setting_level = output->pwm_duty/10;
    //AIGO_PRINT_LOG("algo_light_handle: raw_lux=%d, filtered_avg=%d, buffer_count=%d, pwm_duty=%d, setting_level=%d \n\r", 
    //     lux_level, filtered_avg, lux_level_buffer_count, output->pwm_duty, output->setting_level);
}

void light_sensor_data_cb(algo_manager_context_t *ctx, uint8_t *data, int data_len)
{
    EXCEPTION_CHECK_VOID(ctx);
    sensor_wrapper_data_t *sensor_data = (sensor_wrapper_data_t *)data;
    algo_light_sensor_data_input_t data_input = {
        .timestamp = get_timestamp_ms(0),   //todo
        .lux_level = sensor_data->light_sensor_data.lux_level,
    };
}
