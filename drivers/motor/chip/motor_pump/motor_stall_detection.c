/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h"  // IWYU pragma: keep
#include "bsp_config.h"  // IWYU pragma: keep
#include <stdint.h>

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_STALL_DETECTION_LOG      (1)
#define STALL_DETECTION_INTERVAL_MS         (HAL_GPADC_ATP_5MS / 1000)
#define MAX_STALL_DETECTION_TARGET_TIMES    (200)

typedef struct {
    uint8_t  is_running;        /// 运行状态，0：停止，1：运行
    uint16_t  target_times;     /// 目标次数
    uint16_t  work_times;       /// 工作次数
    uint16_t last_voltage;      /// 最后一次电压
    uint32_t sum_voltage;       /// 总电压
} motor_pump_stall_detection_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static motor_pump_stall_detection_t s_pump_stall_status[MOTOR_PWM_ID_MAX] = {0};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void motor1_adc_irq_cb(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
    s_pump_stall_status[0].work_times++;
    s_pump_stall_status[0].sum_voltage += volt;
    if (s_pump_stall_status[0].work_times >= s_pump_stall_status[0].target_times) {
        hal_gpadc_close(PUMP1_ADC_CHANNEL);
        s_pump_stall_status[0].is_running = false;
        s_pump_stall_status[0].last_voltage = s_pump_stall_status[0].work_times > 0 ? \
            s_pump_stall_status[0].sum_voltage / s_pump_stall_status[0].work_times : 0;
    }
}

static void motor2_adc_irq_cb(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
    s_pump_stall_status[1].work_times++;
    s_pump_stall_status[1].sum_voltage += volt;
    if (s_pump_stall_status[1].work_times >= s_pump_stall_status[1].target_times) {
        hal_gpadc_close(PUMP2_ADC_CHANNEL);
        s_pump_stall_status[1].is_running = false;
        s_pump_stall_status[1].last_voltage = s_pump_stall_status[1].work_times > 0 ? \
            s_pump_stall_status[1].sum_voltage / s_pump_stall_status[1].work_times : 0;
    }
}

static int motor_pump_stall_detection_start(uint8_t adc_channel_id, uint16_t target_times)
{
    int ret = 0;
    if (s_pump_stall_status[adc_channel_id].is_running) {
        ret = hal_gpadc_close(adc_channel_id);
        if (ret != BSP_OK) {
            LOGE("adc channel: %d close failed", adc_channel_id);
        }
    }
    s_pump_stall_status[adc_channel_id].is_running = true;
    s_pump_stall_status[adc_channel_id].work_times = 0;
    s_pump_stall_status[adc_channel_id].sum_voltage = 0;
    s_pump_stall_status[adc_channel_id].target_times = target_times;
    s_pump_stall_status[adc_channel_id].last_voltage = 0;
    ret = hal_gpadc_open(adc_channel_id, HAL_GPADC_ATP_5MS, 
        adc_channel_id == PUMP1_ADC_CHANNEL ? motor1_adc_irq_cb : motor2_adc_irq_cb);
    if (ret != BSP_OK) {
        LOGE("adc channel: %d open failed", adc_channel_id);
    }
    return ret;
}

static int motor_pump_stall_detection_stop(uint8_t adc_channel_id)
{
    int ret = 0;
    if (s_pump_stall_status[adc_channel_id].is_running) {
        ret = hal_gpadc_close(adc_channel_id);
        if (ret != BSP_OK) {
            LOGE("adc channel: %d close failed", adc_channel_id);
            return BSP_ERROR;
        }
    }
    s_pump_stall_status[adc_channel_id].is_running = false;
    return BSP_OK;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int pump_motor_stall_detection_start(uint8_t pump_id, uint16_t duration_ms)
{
    return BSP_OK;
    if (pump_id >= MOTOR_PUMP_ID_MAX) {
        LOGE("pump id is out of range: %d", pump_id);
        return BSP_ERROR;
    }
    uint16_t target_times = duration_ms / STALL_DETECTION_INTERVAL_MS;
    if (target_times > MAX_STALL_DETECTION_TARGET_TIMES) {
        target_times = MAX_STALL_DETECTION_TARGET_TIMES;
    }
    return motor_pump_stall_detection_start(pump_id == MOTOR_PUMP_1 ? PUMP1_ADC_CHANNEL : PUMP2_ADC_CHANNEL, duration_ms);
}

int pump_motor_stall_detection_stop(uint8_t pump_id)
{
    return BSP_OK;
    if (pump_id >= MOTOR_PUMP_ID_MAX) {
        LOGE("pump id is out of range: %d", pump_id);
        return BSP_ERROR;
    }
    return motor_pump_stall_detection_stop(pump_id == MOTOR_PUMP_1 ? PUMP1_ADC_CHANNEL : PUMP2_ADC_CHANNEL);
}

bool pump_motor_is_stalled(uint8_t pump_id)
{
    uint8_t is_stalled = 0;
    if (pump_id >= MOTOR_PUMP_ID_MAX) {
        LOGE("pump id is out of range: %d", pump_id);
    }
    return is_stalled;
}