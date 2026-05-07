#ifndef FLOW_ALGO_H
#define FLOW_ALGO_H

#include <stdint.h>
#include <stddef.h>

// 算法常量配置（与 SDP3x 流水相关的开关见 sdp3x_remain_detection_config.h）
#define SAMPLE_INTERVAL 0.01f      // 10ms
#define K_VOL 0.0576f              // 体积系数
#define WINDOW_SIZE_SEC 5.0f       // 流速计算窗口 5秒
#define MAX_HISTORY_SIZE 512       // 环形历史，用于 WINDOW_SIZE_SEC 窗口内线性回归

// 简单的点结构，用于历史记录（优化：使用float节省内存）
typedef struct {
    float t;   // 时间戳 (秒)
    float val; // 值 (体积)
} FlowPoint;

// 算法上下文结构体，用于保存状态（优化：使用float节省内存）
typedef struct {
    // 状态变量
    float current_time;           // 当前总时间
    float current_cumsum;         // 累积积分值

    // 历史数据缓存 (用于计算流速的线性回归)
    FlowPoint history[MAX_HISTORY_SIZE];
    uint16_t history_head;        // 环形缓冲区写入头索引（优化：uint16_t）
    uint16_t history_count;       // 当前缓存中的有效点数（优化：uint16_t）
    
    float last_output_time;       // 上一次输出的时间 (用于固定时间间隔输出)
} FlowContext;

// 算法输出结果结构体（优化：使用float节省内存）
typedef struct {
    float timestamp;  // 发生过零点的时间
    float volume;     // 计算出的体积 (mL)
    float flow_rate;  // 计算出的流速 (mL/min)
} FlowResult;

/**
 * @brief 初始化算法上下文
 * @param ctx 上下文指针
 */
void flow_algo_init(FlowContext* ctx);

/**
 * @brief 处理一批气压数据
 * 
 * @param ctx 算法上下文
 * @param pressure_data 输入的气压数据数组 (Pa)
 * @param data_len 数据长度 (通常为50，代表0.5秒数据)
 * @param results 输出结果数组，用于存储计算结果
 * @param max_results results数组的最大容量
 * @return int 产生的结果数量（固定每秒输出一次）
 */
int process_pressure_batch(FlowContext* ctx, const float* pressure_data, int data_len, FlowResult* results, int max_results);

#endif // FLOW_ALGO_H

