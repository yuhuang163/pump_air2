#ifndef __APP_PROTOCOL_PUMP_CTRL_SERVER_H__
#define __APP_PROTOCOL_PUMP_CTRL_SERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep
#include "app_protocol_parse.h"

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

//pump_ctrl
void app_old_comm_handle_ctrl_dev_req(uint8_t cal, uint8_t *cab);
void app_old_comm_report_device_status(app_old_commd_l_r_ctrl_e lr_ct);
void app_old_comm_handle_get_status_req(app_old_commd_l_r_ctrl_e lr_ct);
void app_old_comm_handle_ctrl_dev_req(uint8_t cal, uint8_t *cab);
void app_old_comm_handle_set_user_define_req(uint8_t cal, uint8_t *cab);
void app_old_comm_handle_get_dev_info_req(uint8_t cal, uint8_t *cab);

#ifdef __cplusplus
}
#endif

#endif 