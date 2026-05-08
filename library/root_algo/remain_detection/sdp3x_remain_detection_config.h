#ifndef SDP3X_REMAIN_DETECTION_CONFIG_H
#define SDP3X_REMAIN_DETECTION_CONFIG_H

/**
 * SDP3x 与 remain_detection（示例时序 + flow_algo）衔接用的可调宏。
 * 放在算法目录，避免堆在 boards/project_config 或芯片驱动目录。
 * 可在包含 sdp3x.h 之前 #define 覆盖。
 */

/// 连续测量模式：1=与 Sensirion 示例 main 一致（质量流量补偿 + Average till read），0=纯差压 DP
#ifndef SDP3X_MEASUREMENT_USE_MASS_FLOW_REFERENCE
#define SDP3X_MEASUREMENT_USE_MASS_FLOW_REFERENCE          (1)
#endif

/// 启用差压读数后进入 flow_algo 流水线
#ifndef SDP3X_FLOW_PIPELINE_ENABLE
#define SDP3X_FLOW_PIPELINE_ENABLE                         (1)
#endif

/// 每次硬件读数后向 flow_algo 注入的「虚拟 10ms 采样」次数（须 ≥1，与 SAMPLE_INTERVAL 一致）
#ifndef SDP3X_FLOW_VIRTUAL_10MS_SAMPLES_PER_READ
#define SDP3X_FLOW_VIRTUAL_10MS_SAMPLES_PER_READ           (2)
#endif

/// 串口打印每次采样（经 BSP），0=关闭
#ifndef SDP3X_LOG_DEBUG
#define SDP3X_LOG_DEBUG                         (1)
#endif

#endif /* SDP3X_REMAIN_DETECTION_CONFIG_H */
