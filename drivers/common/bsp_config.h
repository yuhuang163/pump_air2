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


/// ADC related defines
#define BATT_ADC_POWER_PIN              HAL_GPIO_PIN_P4_4
#define BATT_ADC_CHANNEL                HAL_GPADC_CHAN_2
#define NTC_ADC_CHANNEL                 HAL_GPADC_CHAN_4

/// System power related defines
#define SYS_POWER_PIN                   HAL_IOMUX_PIN_P4_5

/// Power key related defines
#define POWER_KEY_PIN                   HAL_IOMUX_PIN_P4_7// HAL_IOMUX_PIN_P4_6

/// Touch key related defines
#define TOUCH_INT_PIN                   HAL_IOMUX_PIN_P0_0

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif 
