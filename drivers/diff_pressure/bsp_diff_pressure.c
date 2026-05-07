/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_i2c.h" // IWYU pragma: keep
#include "bsp_config.h"
#include "bsp_internal.h"
#include "bsp_diff_pressure_internal.h"
#include "hal_diff_pressure.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static rt_device_node_t *node = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int diff_pressure_dev_open(struct rt_device_node *dev)
{
    bsp_i2c_init(DIFF_PRESSURE_I2C_ID);
    msleep(10);
    return bsp_diff_pressure_init();
}

static int diff_pressure_dev_ioctl(struct rt_device_node *dev, uint32_t op_code, void *args)
{
    switch (op_code) {
        case OP_CODE_DIFF_PRESSURE_GET_STATUS:
            return bsp_read_diff_pressure_status(args);
        case OP_CODE_DIFF_PRESSURE_IS_READY:
            return bsp_diff_pressure_is_ready(args);
        case OP_CODE_DIFF_PRESSURE_IS_SUPPORT:
            return true;
        case OP_CODE_DIFF_PRESSURE_GET_FLOW_STATUS:
            return bsp_diff_pressure_get_flow_status(args);
        default:
            return -1;
    }
}

static int diff_pressure_dev_suspend(struct rt_device_node *dev)
{
    return bsp_diff_pressure_deinit();
}

static int diff_pressure_dev_resume(struct rt_device_node *dev)
{
    return bsp_diff_pressure_init();
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void *register_device_for_diff_pressure(list_head_t *list_head)
{
    node = u_malloz(sizeof(rt_device_node_t));
    node->name = "diff_pressure";
    node->id = ID_DEVICE_DIFF_PRESSURE;
    node->open = diff_pressure_dev_open;
    node->suspend = diff_pressure_dev_suspend;
    node->resume = diff_pressure_dev_resume;
    node->ioctl = diff_pressure_dev_ioctl;
    __list_add_tail(&node->list, list_head);
    LOG_PBUS("dev: %s registered", node->name);

    return node;
}
