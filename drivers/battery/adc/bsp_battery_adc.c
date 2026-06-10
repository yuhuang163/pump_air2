/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h"
#include "bsp_config.h"
#include "bsp_adc.h"
#include "bsp_battery_internal.h"

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
    11,
    6000,
    15000,
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
    // LOG_PBUS("voltage:%d, %d", *batt_voltage, voltage);
    if (*batt_voltage < sg_batt_adc.voltage_min || *batt_voltage > sg_batt_adc.voltage_max) {
        LOG_PBUS("voltage out of range:%d, %d", *batt_voltage, voltage);
        return BSP_ERROR;
    }
    return BSP_OK;
}

