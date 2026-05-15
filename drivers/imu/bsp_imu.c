/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h"
#include "bsp_config.h"
#include "bsp_i2c.h"
#include "bsp_internal.h"
#include "bsp_imu_internal.h"
#include "hal_imu.h"

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
static int imu_dev_open(struct rt_device_node *dev)
{
    (void)dev;
    bsp_i2c_init(IMU_I2C_ID);
    msleep(10);
    return bsp_imu_init();
}

static int imu_dev_close(struct rt_device_node *dev)
{
    (void)dev;
    return bsp_imu_deinit();
}

static int imu_dev_read(struct rt_device_node *dev, void *buf, uint32_t n_bytes)
{
    (void)dev;
    return bsp_imu_read_six_axis(buf, n_bytes);
}

static int imu_dev_ioctl(struct rt_device_node *dev, uint32_t op_code, void *args)
{
    (void)dev;

    switch (op_code) {
        case OP_CODE_IMU_ADD_IRQ_CALLBACK:
            return bsp_imu_set_irq_callback((void (*)(uint8_t *))args);
        case OP_CODE_IMU_ENABLE_IRQ:
            return bsp_imu_enable_irq();
        case OP_CODE_IMU_DISABLE_IRQ:
            return bsp_imu_disable_irq();
        case OP_CODE_IMU_GET_CHIP_ID:
            return bsp_imu_get_chip_id(args);
        case OP_CODE_IMU_GET_IMU_FIFO_DATA:
            return bsp_imu_get_fifo_data(args);
        default:
            return -1;
    }
}

static int imu_dev_suspend(struct rt_device_node *dev)
{
    (void)dev;
    (void)bsp_imu_disable_irq();
    return bsp_imu_deinit();
}

static int imu_dev_resume(struct rt_device_node *dev)
{
    int ret;

    (void)dev;
    ret = bsp_imu_init();
    if (ret == BSP_OK) {
        (void)bsp_imu_enable_irq();
    }
    return ret;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void *register_device_for_imu(list_head_t *list_head)
{
    node = u_malloz(sizeof(rt_device_node_t));
    node->name = "imu";
    node->id = ID_DEVICE_IMU;
    node->open = imu_dev_open;
    node->close = imu_dev_close;
    node->suspend = imu_dev_suspend;
    node->resume = imu_dev_resume;
    node->read = imu_dev_read;
    node->ioctl = imu_dev_ioctl;
    __list_add_tail(&node->list, list_head);
    LOG_PBUS("dev: %s registered", node->name);

    return node;
}
