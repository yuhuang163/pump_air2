#ifndef __BSP_LCD_INTERNAL_H__
#define __BSP_LCD_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_lcd_uart.h" // IWYU pragma: keep
#include "bsp_lcd_spi.h" // IWYU pragma: keep
#include "bsp_lcd_qspi.h" // IWYU pragma: keep

/// 水平偏移量
#define LCD_H_OFFSET                    0
/// 垂直偏移量
#define LCD_V_OFFSET                    0

/// 像素大小
#define LCD_RGB565_PIXEL                16
#define LCD_RGB888_PIXEL                24

/// lcd 水平分辨率
#define LCD_H_RES                       (PANLE_DISPLAY_W)

/// lcd 垂直分辨率
#define LCD_V_RES                       (PANLE_DISPLAY_H)

/// 寄存器数据大小
#define  REGISTER_DATA_SIZE             16

/// 刷新状态
typedef enum {
    LCD_REFRESH_STATUS_IDLE = 0,  /// 空闲
    LCD_REFRESH_STATUS_WAITING_FOR_FILL = 1,  /// 等待填充
    LCD_REFRESH_STATUS_FILLED = 2,  /// 已填充
    LCD_REFRESH_STATUS_REFRESHING = 3,  /// 刷新中
} lcd_refresh_status_e;

///  lcd 芯片配置结构体
typedef struct {
    uint8_t command;
    uint8_t data_size;
    uint8_t data[REGISTER_DATA_SIZE];   
} lcd_chip_config_t;

/// lcd 刷新参数结构体
typedef struct {
    uint16_t x1;      /// 水平起始位置
    uint16_t y1;      /// 垂直起始位置
    uint16_t x2;      /// 水平结束位置
    uint16_t y2;      /// 垂直结束位置
    uint16_t *img;    /// 图像数据
    uint16_t *refresh_status;  /// 刷新状态：0=空闲, 1=已填充, 2=刷新中
} lcd_refresh_param_t;

#define IS_SUPPORT_LCD_QSPI_DRIVER      (1)

#if IS_SUPPORT_LCD_QSPI_DRIVER
#define LCD_INTERFACE_INIT()                      bsp_lcd_qspi_init()
#define LCD_WRITE_CMD_PARAM(cmd, data, data_size) bsp_lcd_qspi_write_cmd_param(cmd, data, data_size)
#define LCD_WRITE_IMG_PARAM(cmd, data, data_size) bsp_lcd_qspi_write_img_param(cmd, data, data_size)
#else
#define LCD_INTERFACE_INIT()                      bsp_lcd_spi_init()
#define LCD_WRITE_CMD_PARAM(cmd, data, data_size) bsp_lcd_spi_write_cmd_param(cmd, data, data_size)
#define LCD_WRITE_IMG_PARAM(cmd, data, data_size) bsp_lcd_spi_write_img_param(cmd, data, data_size)
#endif

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
int bsp_lcd_init(void);
void lcd_backlight_control(uint8_t level);
int bsp_lcd_data_send(uint8_t srv_id, lcd_tlvs_t *tlvs, uint8_t tlvs_num);
int lcd_chip_init(void);
void lcd_chip_sleep(void);
int lcd_draw_bitmap(int x_start, int y_start, int x_end, int y_end, const void *color_data);
#ifdef __cplusplus
}
#endif

#endif 
