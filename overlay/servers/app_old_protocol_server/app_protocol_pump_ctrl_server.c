#include "root_common_api.h" // IWYU pragma: keep
#include "app_protocol_handler.h"
#include "protocol_manager_api.h"
#include "storage_manager_api.h"
#include "app_protocol_pump_ctrl_server.h"
#include "breast_pump_manager_api.h"

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

uint8_t app_old_comm_get_sys_status(void)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    EXCEPTION_CHECK_UINT(pbus);
    struct rt_device_node *battery = pbus->find_device(ID_DEVICE_BATTERY);
    EXCEPTION_CHECK_UINT(battery);
    uint8_t chage_state = DEVICE_IOCTL(battery, OP_CODE_BAT_GET_CHARGE_STATUS);

    if (BREAST_PUMP_START == pump_ctx->get_breast_pump_status()
        || BREAST_PUMP_CONTINUE == pump_ctx->get_breast_pump_status()) {
        return APP_OLD_COMM_SYS_RUN;
    }

    if (BREAST_PUMP_START == pump_ctx->get_breast_pump_status()
        || BREAST_PUMP_FINISH == pump_ctx->get_breast_pump_status()
        || BREAST_PUMP_START == pump_ctx->get_breast_pump_status()) {
        return APP_OLD_COMM_SYS_STOP;
    }

    if (chage_state == CHARGE_STATE_CHARGING || chage_state == CHARGE_STATE_FULL) {
        return APP_OLD_COMM_SYS_CHANRGE;
    }
    if ((BREAST_PUMP_START == pump_ctx->get_breast_pump_status()
         || BREAST_PUMP_CONTINUE == pump_ctx->get_breast_pump_status())
        && BREAST_PUMP_CUSTOM_MODE == pump_ctx->get_breast_pump_mode()) {
        return APP_OLD_COMM_PUMP_USE_DEFINE_RUN;
    }
}

uint8_t app_old_comm_get_battery_percent(void)
{
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    EXCEPTION_CHECK_UINT(pbus);
    struct rt_device_node *battery = pbus->find_device(ID_DEVICE_BATTERY);
    uint8_t percent = 0;
    EXCEPTION_CHECK_UINT(battery);
    DEVICE_READ_ARGS(battery, &percent, sizeof(percent));
    return percent;
}

//0xd0
void app_old_comm_report_device_status(app_old_commd_l_r_ctrl_e lr_ct)
{
    LOG_PROTOCOL(" Reporting device status\n");
    uint8_t report_data[45] = {0};
    uint16_t data_len = 0;

    report_data[data_len++] = app_old_comm_get_battery_percent();
    report_data[data_len++] = app_old_comm_get_sys_status();
    report_data[data_len++] = lr_ct;
    report_data[data_len++] = 0x00;
    report_data[data_len++] = 0x00;
    /*report_data[data_len++] = app_old_comm_dev_status.custom_mode;
    report_data[data_len++] = app_old_comm_dev_status.custom_gear;
    report_data[data_len++] = app_old_comm_dev_status.custom_freq;
    report_data[data_len++] = (app_old_comm_dev_status.total_time >> 0) & 0xFF;
    report_data[data_len++] = (app_old_comm_dev_status.total_time >> 8) & 0xFF;
    report_data[data_len++] = (app_old_comm_dev_status.mode_time >> 0) & 0xFF;
    report_data[data_len++] = (app_old_comm_dev_status.mode_time >> 8) & 0xFF;
    report_data[data_len++] = app_old_comm_dev_status.milk_volume;
    report_data[data_len++] = app_old_comm_dev_status.error_flag;
    report_data[data_len++] = (app_old_comm_dev_status.custom_total_time >> 0) & 0xFF;
    report_data[data_len++] = (app_old_comm_dev_status.custom_total_time >> 8) & 0xFF;
    report_data[data_len++] = app_old_comm_dev_status.travel_lock;*/

    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_NOTIFY, APP_OLD_COMM_CID_NOTIFY_STATUS, data_len, report_data);
}

//0xb0
void app_old_comm_handle_get_status_req(app_old_commd_l_r_ctrl_e lr_ct)
{
    LOG_PROTOCOL(" Handling get device status request\n");
    uint8_t ack_data[45] = {0};
    uint16_t data_len = 0;

    ack_data[data_len++] = app_old_comm_get_battery_percent();
    ack_data[data_len++] = app_old_comm_get_sys_status();
    ack_data[data_len++] = lr_ct;
    ack_data[data_len++] = 0x00;
    ack_data[data_len++] = 0x00;
    /*ack_data[data_len++] = app_old_comm_dev_status.custom_mode;
    ack_data[data_len++] = app_old_comm_dev_status.custom_gear;
    ack_data[data_len++] = app_old_comm_dev_status.custom_freq;
    ack_data[data_len++] = (app_old_comm_dev_status.total_time >> 0) & 0xFF;
    ack_data[data_len++] = (app_old_comm_dev_status.total_time >> 8) & 0xFF;
    ack_data[data_len++] = (app_old_comm_dev_status.mode_time >> 0) & 0xFF;
    ack_data[data_len++] = (app_old_comm_dev_status.mode_time >> 8) & 0xFF;
    ack_data[data_len++] = app_old_comm_dev_status.milk_volume;
    ack_data[data_len++] = app_old_comm_dev_status.error_flag;
    ack_data[data_len++] = (app_old_comm_dev_status.custom_total_time >> 0) & 0xFF;
    ack_data[data_len++] = (app_old_comm_dev_status.custom_total_time >> 8) & 0xFF;
    ack_data[data_len++] = app_old_comm_dev_status.travel_lock;*/

    LOG_PROTOCOL(" Sending device status ACK (data len: %d)\n", data_len);
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_GET_STATUS, data_len, ack_data);
}

//0xc0
void app_old_comm_handle_ctrl_dev_req(uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling device control request\n");
    if (cal < 0x06) {
        LOG_PROTOCOL(" Invalid data length for CTRL_DEV (expected >=6, got %d)\n", cal);
        return;
    }

    AppOldCommDevStatus new_status = (AppOldCommDevStatus)cab[0];
    AppOldCommMilkMode new_mode = (AppOldCommMilkMode)cab[1];
    uint8_t new_gear = cab[2];

    LOG_PROTOCOL(" Updating device status - Status: %d, Mode: %d, Gear: %d\n",
           new_status, new_mode, new_gear);

    // TODO: Send parameters to pump valve component
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_CTRL_DEV, 0, NULL);
    app_old_comm_report_device_status(APP_OLD_COMM_LEFT_CT);
    app_old_comm_report_device_status(APP_OLD_COMM_RIGHT_CT);
}

//0xc4
void app_old_comm_handle_set_user_define_req(uint8_t cal, uint8_t *cab)
{


}

//0xf0
void app_old_comm_handle_get_dev_info_req(uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling get device info request\n");
    if (cal != 0x04) {
        LOG_PROTOCOL(" Invalid data length for GET_DEV_INFO (expected 4, got %d)\n", cal);
        return;
    }
    storage_manager_context_t *ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    factory_dev_info_t factory_dev_info;
    AppOldCommDeviceInfo* comm_dev_info = app_old_comm_dev_info_get();
    EXCEPTION_CHECK_VOID(ctx);
    EXCEPTION_CHECK_VOID(cab);
    uint8_t ack_data[64] = {0};
    uint16_t data_len = 0;

    ack_data[data_len++] = ctx->priv.ver_info.sys_local_protocol_id;
    ack_data[data_len++] = ctx->priv.ver_info.soc_version;
    ack_data[data_len++] = ctx->priv.ver_info.hw_version;
    ack_data[data_len++] = ctx->priv.ver_info.sw_version;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[0];//custom_mode_id;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[1];//custom_mode_id;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[2];//custom_mode_id;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[3];//custom_mode_id;
    ack_data[data_len++] = 0x00;//milk_full_ver;
    ack_data[data_len++] = 0x00;//milk_full_switch;
    ctx->get_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &factory_dev_info);
    memcpy(ack_data + data_len, factory_dev_info.sn, factory_dev_info.sn_len);
    data_len += factory_dev_info.sn_len;
#if BLE_PAYLOAD_CRYPTION
    ack_data[data_len++] = comm_dev_info->encrypt_status;
    ack_data[data_len++] = comm_dev_info->encrypt_key;
#endif

    LOG_PROTOCOL(" Sending device info ACK (data len: %d)\n", data_len);
   app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_GET_DEV_INFO, data_len, ack_data);
}
