#ifndef __BSP_LCD_QSPI_H__
#define __BSP_LCD_QSPI_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include <stdint.h>
#include "bsp_common.h" // IWYU pragma: keep

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
int bsp_lcd_qspi_init(void);
int bsp_lcd_qspi_init_display(int width, int height);
int bsp_lcd_qspi_resume(void);
int bsp_lcd_qspi_suspend(void);
int bsp_lcd_qspi_write_cmd_param(uint8_t cmd, const uint8_t *p_data, uint32_t data_size);
int bsp_lcd_qspi_write_img_param(uint8_t cmd, const uint8_t *p_data, uint32_t data_size);

#ifdef __cplusplus
}
#endif

#endif 
