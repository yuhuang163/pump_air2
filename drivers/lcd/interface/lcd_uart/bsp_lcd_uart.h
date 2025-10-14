#ifndef __BSP_LCD_UART_H__
#define __BSP_LCD_UART_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include <stdint.h>
#include "bsp_common.h" // IWYU pragma: keep

/// 屏幕串口协议数据格式
#define LCD_UART_SOF                0x5C   

typedef struct {
    uint8_t type;
    uint8_t data_size;
    uint8_t *data;
} lcd_tlvs_t;

typedef struct {
    uint8_t srv_id;
    lcd_tlvs_t tlv;
} lcd_payload_t;

typedef struct {
    uint8_t sof;
    uint16_t lenght;
    uint8_t srv_id;
    uint8_t type;
    uint8_t data_size;
    uint8_t *data;
} lcd_uart_protocol_t;

/// error code
typedef enum {
    LCD_RSP_SUCCESS = 0x1000,   /// 执行成功
    LCD_RSP_OP_CODE_NO_SUPPORT = 0x1001, /// 命令/操作不支持
    LCD_RSP_INVAILD_PARAMETER = 0x1002,  /// 无效/非法参数
} lcd_operational_response_code_e;

/// data type
typedef enum {
    LCD_IMG_ID = 1,   /// 图片ID数据类型
    LCD_DEVICE_RUNNING_TIME = 2,   /// 运行时长
    LCD_PUMP_LEVEL = 3,   /// 泵力度
    LCD_BLE_STATUS = 4,   /// 蓝牙状态
    LCD_BATTERY_STATUS = 5,   /// 电池状态
    LCD_RSP_ERROR_CODE = 6,   /// 错误码
} lcd_data_type_e;

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
int bsp_lcd_uart_init(void);
void bsp_lcd_uart_register_event_cb(panel_recv_event_cb_func cb);

#ifdef __cplusplus
}
#endif

#endif 
