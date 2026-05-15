/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h"
#include "bsp_config.h"
#include "bsp_i2c.h"
#include "bsp_proximity_sensor_internal.h"
#include "stk33562.h"

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static rt_device_node_t *node = NULL;
static proximity_cali_data_t s_cali;

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int bsp_proximity_sensor_init(void)
{
    return stk33562_init(&s_cali);
}

int bsp_proximity_sensor_deinit(void)
{
    return stk33562_deinit();
}

int bsp_read_proximity_status(void *args)
{
    if (args == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    return stk33562_get_cached_status((proximity_status_t *)args);
}

int bsp_proximity_sensor_is_ready(void *args)
{
    (void)args;
    return stk33562_is_ready() ? BSP_OK : BSP_NOT_READY;
}

int bsp_proximity_set_cali(const proximity_cali_data_t *cali)
{
    if (cali != NULL) {
        s_cali = *cali;
    } else {
        memset(&s_cali, 0, sizeof(s_cali));
    }
    return stk33562_set_cali(&s_cali);
}

int bsp_proximity_get_chip_id(void *args)
{
    uint8_t *out = (uint8_t *)args;

    if (out == NULL) {
        return BSP_ERR_POINTER_NULL;
    }
    return stk33562_get_chip_id(out);
}

int bsp_proximity_set_notify_callback(void (*cb)(void))
{
    return stk33562_set_notify_callback(cb);
}

int bsp_proximity_enable_irq(void)
{
    return stk33562_enable_irq();
}

int bsp_proximity_disable_irq(void)
{
    return stk33562_disable_irq();
}

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int proximity_dev_open(struct rt_device_node *dev)
{
    (void)dev;
    bsp_i2c_init(P_SENSOR_I2C_ID);
    msleep(10);
    return bsp_proximity_sensor_init();
}

static int proximity_dev_ioctl(struct rt_device_node *dev, uint32_t op_code, void *args)
{
    (void)dev;

    switch (op_code) {
        case OP_CODE_PROXIMITY_GET_STATUS:
            return bsp_read_proximity_status(args);
        case OP_CODE_PROXIMITY_IS_READY:
            return bsp_proximity_sensor_is_ready(args);
        case OP_CODE_PROXIMITY_IS_SUPPORT:
            return true;
        case OP_CODE_PROXIMITY_SET_CALI:
            return bsp_proximity_set_cali((const proximity_cali_data_t *)args);
        case OP_CODE_PROXIMITY_GET_CHIP_ID:
            return bsp_proximity_get_chip_id(args);
        case OP_CODE_PROXIMITY_ADD_IRQ_CALLBACK:
            return bsp_proximity_set_notify_callback((void (*)(void))args);
        case OP_CODE_PROXIMITY_ENABLE_IRQ:
            return bsp_proximity_enable_irq();
        case OP_CODE_PROXIMITY_DISABLE_IRQ:
            return bsp_proximity_disable_irq();
        default:
            return BSP_ERROR;
    }
}

static int proximity_dev_suspend(struct rt_device_node *dev)
{
    (void)dev;
    (void)bsp_proximity_disable_irq();
    return bsp_proximity_sensor_deinit();
}

static int proximity_dev_resume(struct rt_device_node *dev)
{
    int ret;

    (void)dev;
    ret = bsp_proximity_sensor_init();
    if (ret == BSP_OK) {
        (void)bsp_proximity_enable_irq();
    }
    return ret;
}

void *register_device_for_proximity_sensor(list_head_t *list_head)
{
    node = u_malloz(sizeof(rt_device_node_t));
    node->name = "proximity_sensor";
    node->id = ID_DEVICE_PROXIMITY_SENSOR;
    node->open = proximity_dev_open;
    node->suspend = proximity_dev_suspend;
    node->resume = proximity_dev_resume;
    node->ioctl = proximity_dev_ioctl;
    __list_add_tail(&node->list, list_head);
    LOG_PBUS("dev: %s registered", node->name);

    return node;
}
