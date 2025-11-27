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
static void app_old_comm_set_night_light(uint8_t onoff, uint8_t level)
{
    storage_manager_context_t *st = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    EXCEPTION_CHECK_VOID(st);
    EXCEPTION_CHECK_VOID(pbus);

    struct rt_device_node *led_dev = pbus->find_device(ID_DEVICE_LED);
    EXCEPTION_CHECK_VOID(led_dev);

    night_light_info_t night_light_info = {0};
    st->get_store_ram_val(STORE_ID_SYS_NIGHT_LIGHT_INFO, 0, 0, &night_light_info);

    // Update night light info
    night_light_info.onoff = onoff;
    night_light_info.level = level;

    st->put_store_ram_val(STORE_ID_SYS_NIGHT_LIGHT_INFO, 0, 0, &night_light_info);

    led_status_t led_status = {
        .mode = LED_MODE_LEVEL,
        .un.luminous.level = night_light_info.onoff ? night_light_info.level : 0,
    };
    int args[] = {NIGHT_LIGHT_LED_ID_0, (int)&led_status};
    led_dev->ioctl(led_dev, OP_CODE_NIGHT_LIGHT_STATUS_SET, (void *)args);

    LOG_PROTOCOL(" Night light set: onoff=%d, level=%d\n", night_light_info.onoff, night_light_info.level);
}

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

void app_old_comm_handle_ctrl_dev_sys_status_req(uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling ctrl_dev_sys_status request\n");
    EXCEPTION_CHECK_VOID(cab);

    if (cal != 0x07) {
        LOG_PROTOCOL(" Invalid data length for CTRL_DEV_STATUS (expected 7, got %d)\n", cal);
        return;
    }

    app_old_comm_sys_status_ctrl sys_status;

    // Byte 0: Language type (0x00-English, 0x01-Chinese, 0x02-French, 0x03-German, 
    //                        0x04-Spanish, 0x05-Italian, 0x06-Japanese)
    sys_status.sys_language_e = cab[0];

    // Byte 1: Screen lock time (5-30s, 0xFF = no lock)
    sys_status.screem_lock_time = cab[1];

    // Byte 2: Screen brightness level (0:25%, 1:50%, 2:75%, 3:100%)
    sys_status.screem_lock_light_level = cab[2];

    // Byte 3-4: Massage duration in seconds (Little Endian, range: 30s - 600s)
    sys_status.motor_Duration = (uint16_t)(cab[3] | (cab[4] << 8));

    // Byte 5: Night light switch (0-off, 1-on)
    sys_status.night_light_switch = cab[5];

    // Byte 6: Night light brightness level (1-10)
    sys_status.night_light_level = cab[6];

    LOG_PROTOCOL(" Sys status: lang=%d, lock_time=%d, brightness=%d, massage_time=%d, night_sw=%d, night_lvl=%d\n",
                 sys_status.sys_language_e,
                 sys_status.screem_lock_time,
                 sys_status.screem_lock_light_level,
                 sys_status.motor_Duration,
                 sys_status.night_light_switch,
                 sys_status.night_light_level);

    // Apply night light settings
    app_old_comm_set_night_light(sys_status.night_light_switch, sys_status.night_light_level);

    // TODO: Apply other sys_status settings (language, screen lock, brightness, massage time)

    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_CTRL_DEV_STATUS, cal, cab);
}


