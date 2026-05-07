#include "flow_algo.h"
#include <string.h>

/* 避免 <math.h> 在该 arm-none-eabi newlib 组合下触发 wint_t / wchar 依赖链 */
#define flow_fabsf(x) __builtin_fabsf(x)
#define flow_sqrtf(x) __builtin_sqrtf(x)

void flow_algo_init(FlowContext* ctx)
{
    if (!ctx) {
        return;
    }
    memset(ctx, 0, sizeof(FlowContext));
}

static float calculate_flow_rate_slope(FlowContext* ctx) {
    // 线性回归计算流速
    // 遍历历史记录，只取最近 WINDOW_SIZE_SEC 内的数据
    
    float sum_x = 0.0f;
    float sum_y = 0.0f;
    float sum_xy = 0.0f;
    float sum_xx = 0.0f;
    int n = 0;
    
    float start_time = ctx->current_time - WINDOW_SIZE_SEC;
    
    // 从最新的数据开始回溯遍历
    for (int i = 0; i < ctx->history_count; i++) {
        // 计算环形缓冲区索引 (倒序)
        int idx = (ctx->history_head - 1 - i + MAX_HISTORY_SIZE) % MAX_HISTORY_SIZE;
        
        float t = ctx->history[idx].t;
        float v = ctx->history[idx].val;
        
        if (t < start_time) {
            // 超出时间窗口，停止查找
            break;
        }
        
        sum_x += t;
        sum_y += v;
        sum_xy += (t * v);
        sum_xx += (t * t);
        n++;
    }
    
    if (n < 2) {
        return 0.0f;
    }
    
    // 最小二乘法斜率公式: m = (n*sum(xy) - sum(x)*sum(y)) / (n*sum(x^2) - sum(x)^2)
    float numerator = (float)n * sum_xy - sum_x * sum_y;
    float denominator = (float)n * sum_xx - sum_x * sum_x;
    
    if (flow_fabsf(denominator) < 1e-6f) {
        return 0.0f;
    }
    
    float slope = numerator / denominator;
    return slope; // 单位: mL/s
}

int process_pressure_batch(FlowContext* ctx, const float* pressure_data, int data_len, FlowResult* results, int max_results)
{
    if (!ctx || !pressure_data || !results) {
        return 0;
    }

    int result_count = 0;

    // 处理每个压力数据点（SAMPLE_INTERVAL 与 data_len 约定由调用端保持 10ms 步进）
    for (int i = 0; i < data_len; i++) {
        float dP = pressure_data[i];

        // 1. 更新时间
        ctx->current_time += SAMPLE_INTERVAL;
        
        // 2. 积分计算
        // processed_pressure = sign(dP) * sqrt(abs(dP))
        float dP_abs = flow_fabsf(dP);
        float processed = (dP >= 0 ? 1.0f : -1.0f) * flow_sqrtf(dP_abs);
        
        float integration_term = processed * SAMPLE_INTERVAL;
        ctx->current_cumsum += integration_term;
    }
    
    // 3. 固定时间间隔输出（每秒输出一次）
    // 检查是否已经过了1秒（从上次输出时间开始计算）
    float time_since_last_output = ctx->current_time - ctx->last_output_time;
    if (time_since_last_output >= 1.0f) {
        // 计算体积
        float vol = ctx->current_cumsum * K_VOL;
        if (vol < 0) vol = 0.0f;
        
        // 存入历史记录 (用于流速计算)
        ctx->history[ctx->history_head].t = ctx->current_time;
        ctx->history[ctx->history_head].val = vol;
        
        ctx->history_head = (ctx->history_head + 1) % MAX_HISTORY_SIZE;
        if (ctx->history_count < MAX_HISTORY_SIZE) {
            ctx->history_count++;
        }
        
        // 计算流速
        float flow_rate_ml_min = 0.0f;
        
        // 仅当时间超过窗口大小 (5s) 时才计算流速，避免初始波动
        if (ctx->current_time >= WINDOW_SIZE_SEC) {
            float flow_rate_ml_s = calculate_flow_rate_slope(ctx);
            flow_rate_ml_min = flow_rate_ml_s * 60.0f;
            if (flow_rate_ml_min < 0) flow_rate_ml_min = 0.0f;
        }
        
        // 输出结果
        if (result_count < max_results) {
            results[result_count].timestamp = ctx->current_time;
            results[result_count].volume = vol;
            results[result_count].flow_rate = flow_rate_ml_min;
            result_count++;
        }
        
        // 更新上次输出时间（对齐到整秒）
        ctx->last_output_time = ctx->current_time;
    }
    
    return result_count;
}

