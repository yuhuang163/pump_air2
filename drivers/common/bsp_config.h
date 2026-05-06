#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "hal_iomux.h" // IWYU pragma: keep
#include "hal_gpio.h" // IWYU pragma: keep
#include "hal_gpadc.h" // IWYU pragma: keep
#include "project_config.h" // IWYU pragma: keep
#include "project_support_overlay.h" // IWYU pragma: keep (for IS_SUPPORT_TOUCH_KEY_HYN)

/// GPIO控制宏定义
#define GPIO_PIN_CTRL(pin, status)      ((status) ? hal_gpio_pin_set(pin) : hal_gpio_pin_clr(pin))


/// ADC相关定义
#define BATT_ADC_CHANNEL                HAL_GPADC_CHAN_4
#define PUMP1_ADC_CHANNEL               HAL_GPADC_CHAN_2
#define PUMP2_ADC_CHANNEL               HAL_GPADC_CHAN_2    /// no used
#define LIGHT_ADC_CHANNEL               HAL_GPADC_CHAN_4    /// no used
#define HARD_VERSION_CHANNEL            HAL_GPADC_CHAN_3    /// 版本识别定义


/// 泵阀相关定义
#define PWM_PUMP1_PIN                   HAL_GPIO_PIN_P3_7    /// 气泵PWM引脚
#define PWM_VALVE1_PIN                  HAL_GPIO_PIN_P3_6    /// 阀门PWM引脚
#define MOTOR_PUMP1_PWM_ID              HAL_PWM1_ID_3
#define MOTOR_PUMP1_PWM_IMUX            hal_iomux_set_pwm7
#define MOTOR_VALVE1_PWM_ID             HAL_PWM1_ID_2
#define MOTOR_VALVE1_PWM_IMUX           hal_iomux_set_pwm6

/// 按键相关定义
#define POWER_KEY_PIN                   HAL_GPIO_PIN_P1_5
#define UP_KEY_PIN                      HAL_GPIO_PIN_P4_6
#define DOWN_KEY_PIN                    HAL_GPIO_PIN_P4_5

/// LED相关定义
#define LED1_PIN                        HAL_GPIO_PIN_P4_4
#define LED2_PIN                        HAL_GPIO_PIN_P4_3
#define LED3_PIN                        HAL_GPIO_PIN_P4_2

/// 通信串口
#define LOG_UART_TX_PIN1                 HAL_GPIO_PIN_P1_0
#define LOG_UART_RX_PIN1                 HAL_GPIO_PIN_P1_1

/// 日志串口
#define LOG_UART_TX_PIN                 HAL_GPIO_PIN_P2_3
#define LOG_UART_RX_PIN                 HAL_GPIO_PIN_P2_2

/// I2C相关定义
#define I2C0_SCL_PIN                    HAL_GPIO_PIN_P0_0
#define I2C0_SDA_PIN                    HAL_GPIO_PIN_P0_1
#define I2C1_SCL_PIN                    HAL_GPIO_PIN_P0_2
#define I2C1_SDA_PIN                    HAL_GPIO_PIN_P0_3
#define I2C3_SCL_PIN                    HAL_GPIO_PIN_P0_6
#define I2C3_SDA_PIN                    HAL_GPIO_PIN_P0_7
#define FUEL_GAUGE_I2C_ID               I2C_MASTER_ID_0 /// 电量计IC
#define CHARGE_I2C_ID                   I2C_MASTER_ID_0 /// 充电芯片IC
#define IMU_I2C_ID                      I2C_MASTER_ID_1
#define CAP_SENSOR_I2C_ID               I2C_MASTER_ID_1
#define AIR_PRESSURE_I2C_ID1            I2C_MASTER_ID_2
#define AIR_PRESSURE_I2C_ID2            I2C_MASTER_ID_2 /// no use
#define PRESSURE_SENSOR_I2C_ID          I2C_MASTER_ID_2

/// 充电芯片相关定义
#define CHARGE_POWER_PIN                HAL_GPIO_PIN_P3_4
#define CHARGE_STATUS_PIN               HAL_GPIO_PIN_P3_3
/// 中断引脚
#define P_SENSOR1_INT_PIN               HAL_GPIO_PIN_P0_4
#define FUEL_GAUGE_INT_PIN              HAL_GPIO_PIN_P2_5   /// 电量计中断
#define IMU_INT_PIN                     HAL_GPIO_PIN_P2_4   /// IMU中断
#define P_SENSOR2_INT_PIN               HAL_GPIO_PIN_P1_6   /// 奶阵奶量sensor中断
#define CAP_SENSOR_INT_PIN              HAL_GPIO_PIN_P1_7   /// 佩戴检测中断
#define CHARGE_INT_PIN                  HAL_GPIO_PIN_P3_5   /// 充电状态中断

/// HALL引脚
#define HALL_OUT_PIN                    HAL_GPIO_PIN_P1_4
/// 左右耳识别引脚
#define LEFT_OR_RIGHT_PIN               HAL_GPIO_PIN_P0_5

/// 外设电源相关定义
#define PERIPHERAL_POWER_PIN            HAL_GPIO_PIN_P2_6
#define SENSOR_POWER_PIN                HAL_GPIO_PIN_P2_7

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif 
