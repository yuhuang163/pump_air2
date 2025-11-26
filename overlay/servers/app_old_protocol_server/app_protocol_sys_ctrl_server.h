#ifndef __APP_PROTOCOL_SYS_CTRL_SERVER_H__
#define __APP_PROTOCOL_SYS_CTRL_SERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep

typedef enum {
    APP_OLD_COMM_SYS_POWER_OFF = 0x01,
    APP_OLD_COMM_SYS_REBOOT = 0x02,
    APP_OLD_COMM_SYS_OTA = 0x03,
    APP_OLD_COMM_SYS_OFF_OTA_RECOVERY = 0x04,
} app_old_commd_sys_ctrl_e;

void app_old_comm_handle_set_bind_req(uint8_t cal, uint8_t *cab);
void app_old_comm_handle_heartbeat_req(void);
void app_old_comm_handle_set_rtc_req(uint8_t cal, uint8_t *cab);
void app_old_comm_handle_sys_ctrl_req(uint8_t cal, uint8_t *cab);
void app_old_comm_handle_get_encrypt_req(void);
void app_old_comm_handle_set_travel_lock_req(uint8_t cal, uint8_t *cab);
void app_old_comm_report_discon_status(void);
void app_old_comm_report_bind_status(uint8_t bind_status, uint8_t left_right);
#ifdef __cplusplus
}
#endif

#endif 
