/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h"
#include "bsp_config.h"
#include "bsp_adc.h"
#include "bsp_light_sensor_internal.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_LIGHT_SENSOR_DBG_PRINT          (0)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static rt_device_node_t *node = NULL;

#if IS_SUPPORT_LIGHT_SENSOR_DBG_PRINT
static uint16_t light_sensor_log_count = 0;
#endif

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static int light_sensor_single_conversion(uint32_t *light_voltage)
{
    uint16_t voltage = 0;
    bsp_adc_single_conversion(LIGHT_ADC_CHANNEL, 10, &voltage);
    *light_voltage = voltage;
#if IS_SUPPORT_LIGHT_SENSOR_DBG_PRINT
    light_sensor_log_count++;
    if (light_sensor_log_count % 20 == 0) {
        LOG_PBUS("voltage:%d, %d", *light_voltage, voltage);
    }
#endif
    return BSP_OK;
}

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int light_sensor_dev_open(struct rt_device_node *dev)
{
    bsp_adc_init();
    LOG_PBUS("light_sensor_dev_open");
    return 0;
}

static int light_sensor_dev_read(struct rt_device_node *dev, void *buf, uint32_t n_bytes)
{
    uint32_t *tmp = (uint32_t *)buf;
    EXCEPTION_CHECK_INT(tmp);
    return light_sensor_single_conversion(tmp);
}

static int light_sensor_dev_suspend(struct rt_device_node *dev)
{
    bsp_adc_deinit(BATT_ADC_CHANNEL);
    return 0;
}

static int light_sensor_dev_resume(struct rt_device_node *dev)
{
    bsp_adc_init();
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void *register_device_for_light_sensor(list_head_t *list_head)
{
    node = u_malloz(sizeof(rt_device_node_t));
    node->name = "light_sensor";
    node->id = ID_DEVICE_LIGHT_SENSOR;
    node->open = light_sensor_dev_open;
    node->suspend = light_sensor_dev_suspend;
    node->resume = light_sensor_dev_resume;
    node->read = light_sensor_dev_read;
    __list_add_tail(&node->list, list_head);
    LOG_PBUS("dev: %s registered", node->name);

    return node;
}

void light_sensor_test(void)
{
    uint32_t tmp;
    light_sensor_single_conversion(&tmp);
}
