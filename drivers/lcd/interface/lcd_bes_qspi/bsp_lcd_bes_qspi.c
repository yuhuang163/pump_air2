/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_config.h" // IWYU pragma: keep
#include "bsp_lcd_internal.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static bool lcd_qspi_ready = false;

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
int bsp_lcd_init(void) 
{
    lv_init();
    lv_tick_set_cb(get_timestamp);
    lv_delay_set_cb(msleep);
    lv_hal_disp_init();
    return BSP_OK;
}

int bsp_lcd_deinit(void) 
{
    lv_deinit();
    return BSP_OK;
}

int bsp_lcd_uart_dev_ioctl(struct rt_device_node *dev, uint32_t op_code, void *args)
{
    return BSP_OK;
}

int bsp_lcd_refresh(const void *buf, uint32_t n_bytes)
{
    return BSP_OK;
}

