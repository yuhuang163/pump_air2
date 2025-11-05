#ifndef __HAL_PANEL_H__
#define __HAL_PANEL_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <stdint.h> // IWYU pragma: keep

/**
 * @brief 描述面板操作码
*/
typedef enum {
    OP_CODE_PANEL_CLEAR = 1,
    OP_CODE_PANEL_SET_BL = 2,
    OP_CODE_PANEL_ADD_READY_CB = 3,
    OP_CODE_PANEL_EVENT_CB = 4,
    OP_CODE_PANEL_DATA_SEND = 5,
} panel_dev_op_code_e;

/// 面板数据类型
#define LCD_TLVS_DATA_MAX_SIZE       14
#define LCD_TLVS_HEADER_SIZE         2
typedef struct {
    uint8_t type;
    uint8_t data_size;
    uint8_t data[LCD_TLVS_DATA_MAX_SIZE];
} lcd_tlvs_t;

/// 面板数据类型
#define LCD_TLVS_DATA_TYPE_MAX_SIZE   6
typedef enum {
    LCD_IMG_ID = 1,   /// 图片ID数据类型
    LCD_DEVICE_RUNNING_TIME = 2,   /// 运行时长
    LCD_PUMP_LEVEL = 3,   /// 泵力度
    LCD_BLE_STATUS = 4,   /// 蓝牙状态
    LCD_BATTERY_STATUS = 5,   /// 电池状态
    LCD_PUMP_MODE = 6,   /// 泵模式
    LCD_RSP_ERROR_CODE = 0xE0,   /// 错误码
} lcd_data_type_e;

/// 面板数据服务ID
typedef enum {
    LCD_DISPLAY_EVENT_SRV = 1,  /// 显示事件服务ID
    LCD_KNOB_BUTTON_EVENT_SRV = 2,   /// 旋钮按键事件服务ID
    LCD_MAX_SRV_ID,
} lcd_data_srv_id_e;


typedef void (*panel_recv_event_cb_func)(int arg1, int arg2, void *param);
#ifdef __cplusplus
}
#endif

#endif 
