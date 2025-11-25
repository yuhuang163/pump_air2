#ifndef __APP_PROTOCOL_PUMP_CTRL_SERVER_H__
#define __APP_PROTOCOL_PUMP_CTRL_SERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep
#include "app_old_protocol_parser.h"

typedef enum {
    APP_OLD_COMM_SYS_STOP      = 0x00,
    APP_OLD_COMM_SYS_RUN       = 0x01,
    APP_OLD_COMM_SYS_CHANRGE   = 0x02,
    APP_OLD_COMM_SYS_OFF       = 0x03,
    APP_OLD_COMM_PUMP_USE_DEFINE_RUN = 0x04,
} app_old_commd_sys_status_e;

typedef enum {
    APP_OLD_COMM_LEFT_CT       = 0x00,
    APP_OLD_COMM_RIGHT_CT      = 0x01,
} app_old_commd_l_r_ctrl_e;

// Breast pumping mode definitions
typedef enum {
    APP_OLD_COMM_MODE_MASSAGE = 0x01,         // Massage
    APP_OLD_COMM_MODE_SUCTION = 0x02,         // Suction
    APP_OLD_COMM_MODE_MIXED = 0x03,           // Mixed
    APP_OLD_COMM_MODE_CUSTOM = 0x04,          // Custom
    APP_OLD_COMM_MODE_SMART = 0x05,           // Smart suction
    APP_OLD_COMM_MODE_PP_MILK_TRACK = 0X06,   /// PP追奶模式
    APP_OLD_COMM_MODE_MILK_BOOST = 0X07,      /// 2+8奶量提升模式
    APP_OLD_COMM_MODE_LACTATION_START = 0X08, /// 开奶模式
} app_old_comm_pump_mode;

typedef enum {
    APP_OLD_COMM_MODE_SHORT = 0x01, // Massage
    APP_OLD_COMM_MODE_LONG = 0x02,  // Suction
    APP_OLD_COMM_MODE_MIX1 = 0x03,  // Mix1
    APP_OLD_COMM_MODE_MIX2 = 0x04,  // Mix2
} app_old_comm_custom_pump_step_mode;

typedef struct {
    uint8_t pump_mode;
    uint8_t normal_pump_level;
    uint8_t user_define_pump_step_mode;
    uint8_t user_define_pump_level;
    uint8_t user_define_pump_speed;
    uint8_t pump_mode_lr_or_all;
    uint8_t normal_pump_speed;
} app_old_comm_pump_ctrl;

//pump_ctrl
void app_old_comm_handle_ctrl_dev_req(uint8_t cal, uint8_t *cab);
void app_old_comm_report_device_status(app_old_commd_l_r_ctrl_e lr_ct);
void app_old_comm_handle_get_status_req(app_old_commd_l_r_ctrl_e lr_ct);
void app_old_comm_handle_ctrl_dev_req(uint8_t cal, uint8_t *cab);
void app_old_comm_handle_set_user_define_req(uint8_t cal, uint8_t *cab);
void app_old_comm_handle_get_dev_info_req(uint8_t cal, uint8_t *cab);
void app_old_comm_handle_set_user_define_wr_flash_req(uint8_t cal, uint8_t *cab);

#ifdef __cplusplus
}
#endif

#endif 