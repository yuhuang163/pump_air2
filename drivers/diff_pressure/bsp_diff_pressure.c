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
#include "hal_air_pressure.h"
#include "sdp3x.h"
#include "sdp3x_chip_api.h"
#include "diff_pressure_flow_model.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* GLOBAL — BSP 设备语义（ioctl 与上层数据格式），与芯片 I2C 实现解耦
*****************************************************************************************
*/
int bsp_diff_pressure_init(void)
{
    return app_sdp3x_init(SDP3X_I2C_ADDR);
}

int bsp_diff_pressure_deinit(void)
{
    app_sdp3x_deinit();
    return BSP_OK;
}

int bsp_read_diff_pressure_status(void *args)
{
    air_pressue_status_t *out_status = (air_pressue_status_t *)args;

    if (out_status == NULL) {
        return BSP_INVALID_PARAMETER;
    }

    memset(out_status, 0, sizeof(air_pressue_status_t) * DIFF_PRESSURE_NUM);

    float pa = 0.0f;
    float temp_c = 0.0f;
    int ret = sdp3x_chip_measurement_read(&pa, &temp_c);
    if (ret != BSP_OK) {
        return ret;
    }

    for (uint8_t i = 0; i < DIFF_PRESSURE_NUM; i++) {
        out_status[i].pressure = pa;
        out_status[i].temp = temp_c;
        float rd = pa * 1000.0f;
        if (rd > 2147483647.0f) {
            rd = 2147483647.0f;
        } else if (rd < -2147483648.0f) {
            rd = -2147483648.0f;
        }
        out_status[i].raw_data = (uint32_t)(int32_t)rd;
    }

    diff_pressure_flow_model_on_sample(pa);

#if SDP3X_LOG_DEBUG
    LOG_PBUS("sdp3x Pa: %f, T: %f",
        out_status[0].pressure,
        out_status[0].temp);
#endif
    return BSP_OK;
}

int bsp_diff_pressure_get_flow_status(void *args)
{
#if !SDP3X_FLOW_PIPELINE_ENABLE
    (void)args;
    return BSP_ERROR;
#else
    diff_pressure_flow_status_t *out = (diff_pressure_flow_status_t *)args;

    if (out == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    diff_pressure_flow_model_get_status(out);
    return BSP_OK;
#endif
}

int bsp_diff_pressure_is_ready(void *args)
{
    uint8_t *pressure_is_ready_status = (uint8_t *)args;

    if (pressure_is_ready_status == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    sdp3x_chip_copy_ready_status(pressure_is_ready_status, DIFF_PRESSURE_NUM);
    return BSP_OK;
}

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
