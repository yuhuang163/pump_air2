#include "root_common_api.h" // IWYU pragma: keep
#include "app_protocol_handler.h"
#include "protocol_manager_api.h"
#include "storage_manager_api.h"
#include "app_protocol_sys_ctrl_server.h"
#include "breast_pump_manager_api.h"
#include "pump_mode_default_config.h"
#include "breast_pump_mode.h"
#include "pump_mode_default_config.h"
/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

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
void app_old_comm_handle_set_bind_req(uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling set bind status request\n");
    if (cal != 0x02) {
        LOG_PROTOCOL(" Invalid data length for SET_BIND (expected 2, got %d)\n", cal);
        return;
    }
    uint8_t bind_status = cab[0];
    uint8_t left_right = cab[1];
    LOG_PROTOCOL(" Setting bind status - Status: %d, Side: %d\n", bind_status, left_right);
    // TODO: Update binding status
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_SET_BIND, 2, cab);
    app_old_comm_report_bind_status(bind_status, left_right);
}

// -------------------------- Request command handling implementations --------------------------
void app_old_comm_handle_heartbeat_req(void)
{
    LOG_PROTOCOL(" Sending heartbeat ACK\n");
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_HEARTBEAT, 0, NULL);
}

void app_old_comm_handle_set_rtc_req(uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling set RTC request\n");
    if (cal != 0x04) {
        LOG_PROTOCOL(" Invalid data length for SET_RTC (expected 4, got %d)\n", cal);
        return;
    }
    uint32_t utc_time = (cab[3] << 24) | (cab[2] << 16) | (cab[1] << 8) | cab[0];
    LOG_PROTOCOL(" Setting RTC time to 0x%08X\n", utc_time);
    // TODO: Call RTC setting interface
    platform_set_utc_time(utc_time, 0);
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_SET_RTC, 0, NULL);
}

void app_old_comm_handle_sys_ctrl_req(uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling system control request\n");
    if (cal != 0x01) {
        LOG_PROTOCOL(" Invalid data length for SYS_CTRL (expected 1, got %d)\n", cal);
        return;
    }
    uint8_t ctrl_cmd = cab[0];
    LOG_PROTOCOL(" Executing system control command 0x%02X\n", ctrl_cmd);
    if (ctrl_cmd == APP_OLD_COMM_SYS_POWER_OFF) {
        LOG_PROTOCOL("CMD-poweroff");
        SEND_BROADCAST_ARGS1(ID_BROADCAST_SYS_POWER_STATE_CHANGE, ID_BROADCAST_SYS_POWER_OFF);
    } else if (ctrl_cmd == APP_OLD_COMM_SYS_REBOOT) {
        LOG_PROTOCOL("CMD-softreset");
    } else if (ctrl_cmd == APP_OLD_COMM_SYS_OTA) {
        LOG_PROTOCOL("CMD-ota");
    } else if (ctrl_cmd == APP_OLD_COMM_SYS_OFF_OTA_RECOVERY) {
        LOG_PROTOCOL("CMD-recover");
    }
    // TODO: Execute system control logic
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_SYS_CTRL, 1, &ctrl_cmd);
}

void app_old_comm_handle_get_encrypt_req()
{
    LOG_PROTOCOL(" Handling get encryption info request\n");
    uint8_t ack_data[2] = {0};
    app_old_protocol_handler_context_t *handler_ctx = app_old_protocol_handler_ctx_get();
    ack_data[0] = handler_ctx->encrypt_status;
    ack_data[1] = handler_ctx->encrypt_key;
    LOG_PROTOCOL(" Sending encryption info ACK (status: 0x%02X, key: 0x%02X)\n",
           ack_data[0], ack_data[1]);
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_GET_ENCRYPT, 2, ack_data);
}

void app_old_comm_handle_set_travel_lock_req(uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling set travel lock request\n");
    if (cal != 0x01) {
        LOG_PROTOCOL(" Invalid data length for SET_TRAVEL_LOCK (expected 1, got %d)\n", cal);
        return;
    }
    uint8_t travel_lock = cab[0];
    LOG_PROTOCOL(" Setting travel lock status to %d\n", travel_lock);
    // TODO: Execute travel lock control logic
    airplane_mode_context_t *ap = (airplane_mode_context_t *)find_module(ID_MODULE_AIRPLANE_MODE);
    EXCEPTION_CHECK_VOID(ap);
    if (true==travel_lock)
    {
       ap->enter(AIRPLANE_MODE_BY_USER_ACTIVE_ENTER);
    }
    else
    {
        ap->exit(AIRPLANE_MODE_EXIT);
    }
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_SET_TRAVEL_LOCK, 0, NULL);
}

void app_old_comm_report_discon_status(void)
{
    LOG_PROTOCOL(" Parsing disconnection notification\n");
    // TODO: Handle BLE disconnection logic
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_NOTIFY, APP_OLD_COMM_CID_NOTIFY_DISCON, 0, NULL);
}

void app_old_comm_report_bind_status(uint8_t bind_status, uint8_t left_right)
{
    LOG_PROTOCOL(" Reporting bind status - Status: %d, Side: %d\n", bind_status, left_right);
    uint8_t report_data[2] = {bind_status, left_right};
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_NOTIFY, APP_OLD_COMM_CID_NOTIFY_BIND, 2, report_data);
}
