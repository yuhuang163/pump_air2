/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h"
#include "cmsis_os_api.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "bsp_adc.h"
#include "bsp_battery_internal.h"
#include <stdint.h>

/*
* DEFINES
*****************************************************************************************
*/
#define ADC_CONV_LENGTH                 (8UL)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static const battery_adc_t sg_batt_adc = {
    3.7,
    2000,
    4600,
};

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
int battery_adc_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_map = {
        (enum HAL_IOMUX_PIN_T)HAL_GPIO_PIN_P4_6,  
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_PULLUP_ENABLE
    };

    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(HAL_GPIO_PIN_P4_6, HAL_GPIO_DIR_OUT, 1);
    hal_gpio_pin_set(HAL_GPIO_PIN_P4_6);
    msleep(500);
    bsp_adc_init();
    return 0;
}

int battery_adc_resume(void)
{
    bsp_adc_init();
    return 0;
}

int battery_adc_suspend(void)
{
    bsp_adc_deinit(BATT_ADC_CHANNEL);
    return 0;
}

int battery_adc_single_conversion(uint16_t *batt_voltage)
{
    uint16_t voltage = 0;
    bsp_adc_single_conversion(BATT_ADC_CHANNEL, 10, &voltage);
    *batt_voltage = voltage * sg_batt_adc.coeff;
    // LOGI("voltage:%d, %d", *batt_voltage, voltage);
    if (*batt_voltage < sg_batt_adc.voltage_min || *batt_voltage > sg_batt_adc.voltage_max) {
        return BSP_ERROR;
    }
    return BSP_OK;
}

