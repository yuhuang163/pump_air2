/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_i2c.h" // IWYU pragma: keep
#include "bsp_config.h"
#include "bsp_internal.h"
#include "bsp_cap_sensor_internal.h"
#include "hal_cap_sensor.h"
#include "hx90xx.h"

/*
* GLOBAL — BSP 设备语义（ioctl 与上层数据格式），与芯片 I2C 实现解耦
*****************************************************************************************
*/
int bsp_cap_sensor_init(void)
{
    return hx90xx_init() ? BSP_OK : BSP_ERROR;
}

int bsp_cap_sensor_deinit(void)
{
    hx90xx_timer_stop();
    return BSP_OK;
}

int bsp_cap_sensor_get_wear_flag(void *args)
{
    uint8_t *wear_flag = (uint8_t *)args;

    if (wear_flag == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    *wear_flag = hx90xx_get_wear_status();

    return BSP_OK;
}

int bsp_cap_sensor_get_touch_flag(void *args)
{
    uint8_t *touch_flag = (uint8_t *)args;

    if (touch_flag == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    *touch_flag = hx90xx_get_touch_status();

    return BSP_OK;
}

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static rt_device_node_t *node = NULL;

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int cap_sensor_dev_open(struct rt_device_node *dev)
{
    bsp_i2c_init(CAP_SENSOR_I2C_ID);
    msleep(10);
    return bsp_cap_sensor_init();
}

static int cap_sensor_dev_read(struct rt_device_node *dev, void *buf, uint32_t n_bytes)
{
    (void)dev;
    (void)n_bytes;
    return bsp_cap_sensor_get_wear_flag(buf);
}

static int cap_sensor_dev_ioctl(struct rt_device_node *dev, uint32_t op_code, void *args)
{
    (void)dev;

    switch (op_code) {
        case OP_CODE_CAP_SENSOR_GET_WEAR_FLAG:
            return bsp_cap_sensor_get_wear_flag(args);
        case OP_CODE_CAP_SENSOR_GET_TOUCH_FLAG:
            return bsp_cap_sensor_get_touch_flag(args);
        case OP_CODE_CAP_SENSOR_IS_READY:
            return true;
        default:
            return BSP_ERROR;
    }
}

static int cap_sensor_dev_suspend(struct rt_device_node *dev)
{
    (void)dev;
    return bsp_cap_sensor_deinit();
}

static int cap_sensor_dev_resume(struct rt_device_node *dev)
{
    (void)dev;
    return bsp_cap_sensor_init();
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void *register_device_for_cap_sensor(list_head_t *list_head)
{
    node = u_malloz(sizeof(rt_device_node_t));
    node->name = "cap_sensor";
    node->id = ID_DEVICE_CAP_SENSOR;
    node->open = cap_sensor_dev_open;
    node->read = cap_sensor_dev_read;
    node->suspend = cap_sensor_dev_suspend;
    node->resume = cap_sensor_dev_resume;
    node->ioctl = cap_sensor_dev_ioctl;
    __list_add_tail(&node->list, list_head);
    LOG_PBUS("dev: %s registered", node->name);

    return node;
}
