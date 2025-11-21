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



//pump_ctrl
void app_old_comm_handle_ctrl_dev_req(uint8_t cal, uint8_t *cab);
void app_old_comm_report_device_status(void);
void app_old_comm_handle_get_status_req(void);
void app_old_comm_handle_ctrl_dev_req(uint8_t cal, uint8_t *cab);
void app_old_comm_handle_set_user_define_req(uint8_t cal, uint8_t *cab);
void app_old_comm_handle_get_dev_info_req(uint8_t cal, uint8_t *cab);

#ifdef __cplusplus
}
#endif

#endif 