#ifndef __PROJECT_SUPPORT_OVERLAY_H__
#define __PROJECT_SUPPORT_OVERLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "product_info.h" // IWYU pragma: keep

/// 模块支持
#define IS_SUPPORT_MODULE_PBUS                           (1)
#define IS_SUPPORT_MODULE_PM                             (1)
#define IS_SUPPORT_MODULE_STORAGE                        (1)
#define IS_SUPPORT_MODULE_TIME_SYSTEM                    (1)
#define IS_SUPPORT_MODULE_WM                             (0)
#define IS_SUPPORT_MODULE_UI                             (0)
#define IS_SUPPORT_MODULE_KEY_INPUT                      (1)
#define IS_SUPPORT_MODULE_SENSOR                         (1)
#define IS_SUPPORT_MODULE_BLE                            (1)
#define IS_SUPPORT_MODULE_OTA                            (1)
#define IS_SUPPORT_MODULE_MISC                           (1)
#define IS_SUPPORT_MODULE_MOTOR                          (1)
#define IS_SUPPORT_MODULE_BREAST_PUMP                    (1)
#define IS_SUPPORT_MODULE_TESTS                          (1)
#define IS_SUPPORT_MODULE_US_ALGO                        (1)
#define IS_SUPPORT_MODULE_AUDIO                          (0)
#define IS_SUPPORT_MODULE_AIRPLANE_MODE                  (1)
#define IS_SUPPORT_MODULE_DATA_BURIAL                    (0)
#define IS_SUPPORT_WEEKLY_STAT                           (0)
#define IS_SUPPORT_MODULE_PROTOCOL                       (1)
#define IS_SUPPORT_MODULE_SERVERS                        (1)
#define IS_SUPPORT_MODULE_BATTERY                        (1)
#define IS_SUPPORT_MODULE_LED                            (1)

/// 设备支持
#define IS_SUPPORT_DEVICE_BATTERY                        (1)
#define IS_SUPPORT_DEVICE_LED                            (1)
#define IS_SUPPORT_DEVICE_IRQ                            (1)
#define IS_SUPPORT_DEVICE_FLASH                          (1)
#define IS_SUPPORT_DEVICE_DATABASE                       (1)
#define IS_SUPPORT_DEVICE_INPUT_KEY                      (1)
#define IS_SUPPORT_DEVICE_TOUCH_KEY                      (0)
#define IS_SUPPORT_DEVICE_TOUCH_KEY                      (0)
#define IS_SUPPORT_DEVICE_POWER                          (1)
#define IS_SUPPORT_DEVICE_PMU                            (1)
#define IS_SUPPORT_DEVICE_IMU                            (0)
#define IS_SUPPORT_DEVICE_FSENSOR                        (0)
#define IS_SUPPORT_DEVICE_PRESSURE_KEY                   (0)
#define IS_SUPPORT_DEVICE_AIR_PRESSURE                   (1)
#define IS_SUPPORT_DEVICE_DIFF_PRESSURE                  (1)
#define IS_SUPPORT_DEVICE_MOTOR                          (1)
#define IS_SUPPORT_DEVICE_AUDIO                          (0)
#define IS_SUPPORT_DEVICE_LCD                            (0)
#define IS_SUPPORT_DEVICE_LCD                            (0)
#define IS_SUPPORT_DEVICE_BLE                            (1)
#define IS_SUPPORT_DEVICE_FS                             (0)
#define IS_SUPPORT_DEVICE_WIFI                           (0)
/// 叠图功能
#define IS_SUPPORT_OVERLAY_IMG                           (0)
#define IS_SUPPORT_ASYNC_RESET_UI_PLAY_INFO              (0)
#define IS_SUPPORT_OVERLAY_IMG                           (0)
#define IS_SUPPORT_ASYNC_RESET_UI_PLAY_INFO              (0)
/// 振动马达震动功能
#define IS_SUPPORT_MOTOR_SHAKE                           (0)
/// 使用在线陀螺仪校准
#define IS_USE_GYRO_ONLINE_CALI                          (0) 
/// 使用IMU FIFO
#define IS_SUPPORT_IMU_FIFO                              (0)
/// 测试支持
#define IS_SUPPORT_PRESSURE_TEST                         (1)
/// 支持爬升策略
#define IS_SUPPORT_CLIMB_POLICY                          (1)
/// 支持长按软复位
#define IS_SUPPORT_KEY_SOFT_RESET                        (1)
/// 支持LVGL
#define IS_SUPPORT_LVGL                                  (0)
/// 支持PD充电
#define IS_SUPPORT_PD_CHARGE                             (0)
/// 支持PD充电适配器不匹配
#define IS_SUPPORT_PD_CHARGE_ADAPTER_MISMATCH            (0)
/// 支持旋钮
#define IS_SUPPORT_DEVICE_KNOB                           (0)
/// 支持电池直采
#define IS_SUPPORT_BATTERY_DIRECT_SAMPLING               (0)
/// 支持UART
#define IS_SUPPORT_DEVICE_UART                           (1)
/// 支持光传感器
#define IS_SUPPORT_DEVICE_LIGHT_SENSOR                   (0)
#define IS_SUPPORT_DEVICE_LIGHT_SENSOR                   (0)
/// 支持光传感器校准
#define IS_SUPPORT_DEVICE_LIGHT_SENSOR_CALIB             (0)
/// 支持资源管理
#define IS_SUPPORT_MODULE_RES                            (0)
/// 支持自适应背光  
#define IS_SUPPORT_ADAPTIVE_DIMMING                      (0)
/// 支持堵转检测
#define IS_SUPPORT_MOTOR_STALL_DETECT                    (1)
/// 支持睡眠关机
#define IS_SUPPORT_SLEEP_POWER_OFF                       (1)
/// 支持关机提醒
#if IS_SUPPORT_SLEEP_POWER_OFF
#define IS_SUPPORT_POWER_OFF_REMINDER                    (1)
#else
#define IS_SUPPORT_POWER_OFF_REMINDER                    (0)
#endif
/// 支持UI主题资源
#define IS_SUPPORT_UI_THEME_RES                          (0)
/// 支持泵奶时间累计
#define IS_SUPPORT_MODE_WORK_TIME_ACCUMULATE             (0)

/// 支持漏气提醒
#define IS_SUPPORT_AIR_LEAKAGE_WARNING                   (0)
/// 支持算法采集
#define IS_SUPPORT_ALGO_SAMPLING                         (1)
/// 支持充电
#define IS_SUPPORT_DEVICE_CHARGE                         (1)
/// 支持泵占空比动态调整
#define IS_SUPPORT_PUMP_DUTY_ADJUST                      (0)
/// 支持插电自动开机
#define IS_SUPPORT_PWR_ON_WHEN_PLUG_IN                   (1)
/// 支持夜灯
#define IS_SUPPORT_NIGHT_LIGHT                           (0)
/// 支持LOGO界面背景颜色测试
#define IS_SUPPORT_LOGO_BG_COLOR_TEST                    (0)
/// 支持LOGO界面按键背光自适应调节测试（仅测试使用，默认关闭）
#define IS_SUPPORT_LOGO_KEY_LIGHT_ADAPTIVE_TEST          (0)    
/// 支持触摸芯片固件升级
#define IS_SUPPORT_TOUCH_IC_OTA                          (0)
/// 支持触摸按键背光灯自适应亮度调节
#define IS_SUPPORT_KEY_LIGHT_ADAPTIVE                    (0)

#ifdef __cplusplus
}
#endif

#endif


