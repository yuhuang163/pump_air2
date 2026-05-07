/*
 * 差压流量/体积模型：仅依赖 flow_algo 与配置宏，不包含 I2C/SDP3x 命令。
 */
#include "diff_pressure_flow_model.h"
#include "sdp3x_remain_detection_config.h"

#if SDP3X_FLOW_PIPELINE_ENABLE
#include "flow_algo.h"
#endif

#include <string.h>

#define PRESSURE_BUFFER_SIZE           50
#define BASELINE_SAMPLES               10

#if SDP3X_FLOW_PIPELINE_ENABLE
static FlowContext s_flow_ctx;
static FlowResult s_flow_results[5];
#endif
static float s_pressure_buffer[PRESSURE_BUFFER_SIZE];
static uint16_t s_pressure_buffer_index = 0;
static diff_pressure_flow_status_t s_latest_flow_status;
static float s_pressure_baseline = 0.0f;
static uint8_t s_baseline_calibrated = 0;
static uint8_t s_baseline_sample_count = 0;
static float s_baseline_sum = 0.0f;

#if SDP3X_FLOW_PIPELINE_ENABLE
static void feed_from_sample(float pressure_sample)
{
    if (!s_baseline_calibrated && s_baseline_sample_count < BASELINE_SAMPLES) {
        s_baseline_sum += pressure_sample;
        s_baseline_sample_count++;
        if (s_baseline_sample_count >= BASELINE_SAMPLES) {
            s_pressure_baseline = s_baseline_sum / (float)BASELINE_SAMPLES;
            s_baseline_calibrated = 1;
        }
    }

    float rel = pressure_sample;
    if (s_baseline_calibrated) {
        rel = pressure_sample - s_pressure_baseline;
    }

    int repeats = (int)SDP3X_FLOW_VIRTUAL_10MS_SAMPLES_PER_READ;
    if (repeats < 1) {
        repeats = 1;
    }
    for (int r = 0; r < repeats; r++) {
        if (s_pressure_buffer_index < PRESSURE_BUFFER_SIZE) {
            s_pressure_buffer[s_pressure_buffer_index++] = rel;
        }
    }

    if (s_pressure_buffer_index >= PRESSURE_BUFFER_SIZE) {
        int n = process_pressure_batch(&s_flow_ctx, s_pressure_buffer, PRESSURE_BUFFER_SIZE, s_flow_results,
                                       (int)(sizeof(s_flow_results) / sizeof(s_flow_results[0])));
        s_pressure_buffer_index = 0;
        if (n > 0) {
            FlowResult *last = &s_flow_results[n - 1];
            s_latest_flow_status.valid = 1;
            s_latest_flow_status.volume_ml = last->volume;
            s_latest_flow_status.flow_rate_ml_min = last->flow_rate;
            s_latest_flow_status.algo_time_s = last->timestamp;
        }
    }
}
#endif

void diff_pressure_flow_model_reset(void)
{
    s_pressure_buffer_index = 0;
    s_pressure_baseline = 0.0f;
    s_baseline_calibrated = 0;
    s_baseline_sample_count = 0;
    s_baseline_sum = 0.0f;
    memset(&s_latest_flow_status, 0, sizeof(s_latest_flow_status));
#if SDP3X_FLOW_PIPELINE_ENABLE
    flow_algo_init(&s_flow_ctx);
#endif
}

void diff_pressure_flow_model_on_sample(float pressure_pa)
{
#if SDP3X_FLOW_PIPELINE_ENABLE
    feed_from_sample(pressure_pa);
#else
    (void)pressure_pa;
#endif
}

void diff_pressure_flow_model_get_status(diff_pressure_flow_status_t *out)
{
    if (out == NULL) {
        return;
    }
    *out = s_latest_flow_status;
}
