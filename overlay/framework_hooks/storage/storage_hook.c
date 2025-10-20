/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "factory_persist_data.h"
#include "system_persist_data.h"
#include "breast_pump_persist_data.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
const static put_store_process_handle_t put_store_process_list[] = {
    #if IS_SUPPORT_DEVICE_IMU
    {STORE_ID_FACTORY_IMU_CALI_DATA, storage_write_imu_cali_data_to_flash},
    #endif
    {STORE_ID_FACTORY_DEPLOY_FLAG, storage_write_factory_deploy_flag_to_flash},
    {STORE_ID_FACTORY_AGE_FLAG, storage_write_factory_age_flag_to_flash},
    {STORE_ID_FACTORY_AGE_DISPLAY_FLAG, storage_write_factory_age_display_flag_to_flash},
    #if IS_SUPPORT_DEVICE_FSENSOR
    {STORE_ID_FACTORY_FSENSOR_CALI_FLAG, storage_write_factory_fsensor_cali_flag_to_flash},
    #endif
    {STORE_ID_SYS_CONFIG_INFO, storage_write_sys_config_info_to_flash},
    {STORE_ID_SYS_UTC_TIMESTAMP, storage_write_utc_timestamp_to_flash},
    {STORE_ID_SYS_STATE_INFO, storage_write_sys_state_info_to_flash},    
    {STORE_ID_USER_BIND_INFO, storage_write_user_bind_info_to_flash},
};

const static get_store_process_handle_t get_store_process_list[] = {
    {STORE_ID_FACTORY_DEV_INFO, storage_read_factory_dev_info_from_flash},
    #if IS_SUPPORT_DEVICE_IMU
    {STORE_ID_FACTORY_IMU_CALI_DATA, storage_read_imu_cali_data_from_flash},
    #endif
    {STORE_ID_SYS_CONFIG_INFO, storage_read_sys_config_info_from_flash},
    {STORE_ID_SYS_UTC_TIMESTAMP, storage_read_utc_timestamp_from_flash},
    {STORE_ID_SYS_STATE_INFO, storage_read_sys_state_info_from_flash},    
    {STORE_ID_USER_BIND_INFO, storage_read_user_bind_info_from_flash},
};

const static del_store_process_handle_t del_store_process_list[] = {
    {STORE_ID_BREAST_PUMP_DATA_BODY, storage_del_pump_data_body_from_flash},
};

const static put_store_ram_val_handle_t put_store_ram_val_list[] = {
    {STORE_ID_SYS_TIME_ZONE, storage_write_sys_time_zone_to_ram},
    {STORE_ID_SYS_AP_MODE_ONOFF, storage_write_ap_mode_onoff_to_ram},
    {STORE_ID_SYS_SILENT_UPGRADE, storage_write_slient_upgrade_onoff_to_ram},
    {STORE_ID_SYS_UI_STYLE, storage_write_sys_ui_style_to_ram},
    {STORE_ID_SYS_LOG_SWITCH, storage_write_log_switch_to_ram},
    {STORE_ID_SYS_LANGUAGE, storage_write_language_to_ram},
    {STORE_ID_SYS_GUIDE_DEPLOYED, storage_write_guide_deployed_to_ram},
    {STORE_ID_SYS_BATTERY_HEALTH, storage_write_battery_health_to_ram},
    {STORE_ID_SYS_SYNC_SYS_TIME, storage_write_sync_sys_time_to_ram},
};

const static get_store_ram_val_handle_t get_store_ram_val_list[] = {
    {STORE_ID_FACTORY_DEPLOY_FLAG, storage_read_factory_deploy_flag_from_ram},
    {STORE_ID_FACTORY_AGE_FLAG, storage_read_factory_age_flag_from_ram},
    {STORE_ID_FACTORY_AGE_DISPLAY_FLAG, storage_read_factory_age_display_flag_from_ram},
    #if IS_SUPPORT_DEVICE_FSENSOR
    {STORE_ID_FACTORY_FSENSOR_CALI_FLAG, storage_read_factory_fsensor_cali_flag_from_ram},
    #endif
    {STORE_ID_SYS_TIME_ZONE, storage_read_sys_time_zone_from_ram},
    {STORE_ID_SYS_AP_MODE_ONOFF, storage_read_ap_mode_onoff_from_ram},
    {STORE_ID_SYS_SILENT_UPGRADE, storage_read_slient_upgrade_onoff_from_ram},
    {STORE_ID_SYS_UI_STYLE, storage_read_sys_ui_style_from_ram},
    {STORE_ID_SYS_LOG_SWITCH, storage_read_log_switch_from_ram},
    {STORE_ID_SYS_LANGUAGE, storage_read_language_from_ram},
    {STORE_ID_SYS_GUIDE_DEPLOYED, storage_read_guide_deployed_from_ram},
    {STORE_ID_SYS_BATTERY_HEALTH, storage_read_battery_health_from_ram},
    {STORE_ID_SYS_SYNC_SYS_TIME, storage_read_sync_sys_time_from_ram},
    {STORE_ID_USER_BIND_INFO, storage_user_is_binded_from_ram},
};

static storage_manager_context_t *thiz = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int init_system_ver_info(storage_manager_context_t *ctx)
{
    sec_sys_ver_t *info = &ctx->priv.ver_info;
    memset(info, 0, sizeof(sec_sys_ver_t));
    info->sys_local_protocol_id = ROOT_BLE_PROTOCOL_VERSION;
    info->sys_peer_app_protocol_id = 0;
    info->sys_product_id = ROOT_PRODUCT_ID;
    info->ui_res_id = 101;

    info->sw_version = ROOT_FIRMWARE_VERSION;
    info->sw_version_type = ROOT_FIRMWARE_TYPE;
    info->dfu_type = 0; 
    strcpy(info->device_name, ROOT_DEVICE_NAME);
    return 0;
}

static int init_system_persist_info(storage_manager_context_t *ctx)
{
    sys_persist_info_t *sys_info = &ctx->priv.sys_info;
    memset(sys_info, 0, sizeof(sys_persist_info_t));
    store_func_args_t args = {
        .ctx = ctx,
        .arg1 = 0,
        .arg2 = 0,
        .val = NULL,
    };
    args.val = &sys_info->config;
    storage_read_sys_config_info_from_flash(args);
    args.val = &sys_info->state;
    storage_read_sys_state_info_from_flash(args);
    args.val = &sys_info->bind_info;
    storage_read_user_bind_info_from_flash(args);
    args.val = &sys_info->pump_info;
    storage_read_pump_info_from_flash(args);
    ctx->priv.sys_info_crc[STORE_CRC_TYPE_SYS_CONFIG] = CRC16(&sys_info->config, sizeof(sys_config_info_t));
    ctx->priv.sys_info_crc[STORE_CRC_TYPE_SYS_STATE]  = CRC16(&sys_info->state, sizeof(sys_state_info_t));
    ctx->priv.sys_info_crc[STORE_CRC_TYPE_PUMP_INFO]  = CRC16(&sys_info->pump_info, sizeof(sys_pump_info_t));
    
    return 0;
}

static int system_persist_info_sync_flash(storage_manager_context_t *ctx)
{
    sys_persist_info_t *sys_info = &ctx->priv.sys_info;
    uint16_t info_crc = 0;
    store_func_args_t args = {
        .ctx = ctx,
        .arg1 = 0,
        .arg2 = 0,
        .val = NULL,
    };

    ///  系统配置
    info_crc = CRC16(&sys_info->config, sizeof(sys_config_info_t));
    if (info_crc != ctx->priv.sys_info_crc[STORE_CRC_TYPE_SYS_CONFIG]) {
        args.val = &sys_info->config;
        storage_write_sys_config_info_to_flash(args);
    }

    ///  系统状态
    info_crc = CRC16(&sys_info->state, sizeof(sys_state_info_t));
    if (info_crc != ctx->priv.sys_info_crc[STORE_CRC_TYPE_SYS_STATE]) {
        args.val = &sys_info->state;
        storage_write_sys_state_info_to_flash(args);
    }

    ///  泵奶信息
    info_crc = CRC16(&sys_info->pump_info, sizeof(sys_pump_info_t));
    if (info_crc != ctx->priv.sys_info_crc[STORE_CRC_TYPE_PUMP_INFO]) {
        args.val = &sys_info->pump_info;
        storage_write_pump_info_to_flash(args);
    }

    return 0;
}


static int user_database_reset(void)
{
    EXCEPTION_CHECK_INT(thiz);
    storage_reset_pump_info(thiz, &thiz->priv.sys_info.pump_info);
    msleep(10);
    storage_reset_user_bind_info(thiz, &thiz->priv.sys_info.bind_info);
    msleep(10);
    storage_reset_sys_config_info(thiz, &thiz->priv.sys_info.config);
    msleep(10);
    storage_reset_sys_state_info(thiz, &thiz->priv.sys_info.state);
    msleep(10);
    storage_reset_pump_data_head(thiz, &thiz->priv.pump_data_head);
    
    return 0;
}

static void hdr_storage_work_cb(hdr_post_args_t *args)
{
    if (args->arg1 == 0) {
        // system_persist_info_sync_flash(thiz);
    }
}

static void on_storage_broadcast_route(broadcast_mask_id_t id, void *context, int arg1, int arg2, broadcast_priv_data_t *pdata)
{
    storage_manager_context_t *st = context;
    window_manager_context_t *wm = (window_manager_context_t *)find_module(ID_MODULE_WM);
    pump_result_info_t *user_data = NULL;
    
    switch (id.code) {
        case ID_BROADCAST_PM_STATE_CHANGE:
            if (arg1 == ID_SYS_PM_SUSPEND) {
                POST_TASK_HANDLER_DELAYED_ARG2(hdr_storage_work_cb, 500, 0, st, "persist_store_task1");
            }
            break;
        case ID_BROADCAST_SYS_SHUTDOWN:
            LOG_STORE("-->save persist_store");
            POST_TASK_HANDLER_DELAYED_ARG2(hdr_storage_work_cb, 0, 0, st, "persist_store_task2");
            break;
        case ID_BROADCAST_SYS_PERSIST_INFO_SAVE:
                PRIV_DATA_TO_USER_DATA_V2(user_data, pdata, pump_result_info_t);
                if (user_data) {
                    LOG_STORE("-->save insert_one");
                    POST_TASK_HANDLER_DELAYED_ARG2(hdr_storage_work_cb, 0, 1, user_data, "persist_store_task3");
                }
            break;
        default:
                break;
        }
}

static int on_post_storage_init_route(storage_manager_context_t *ctx)
{
    /// 加载工厂数据
    init_factory_persist_info(ctx);
    /// 加载系统用户数据 
    init_system_persist_info(ctx);
    /// 加载系统版本信息
    init_system_ver_info(ctx);

    return 0;
}

static int on_perpare_storage_init_route(storage_manager_context_t *ctx)
{
    return 0;
}


/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void register_storage_hook(storage_manager_context_t *ctx)
{
    ctx->hook = u_malloz(sizeof(struct __storage_hook));
    ctx->hook->on_post_init_route = on_post_storage_init_route;
    ctx->hook->on_perpare_init_route = on_perpare_storage_init_route;
    ctx->hook->on_storage_broadcast_route = on_storage_broadcast_route;

    ctx->priv.put_store_process_list = put_store_process_list;
    ctx->priv.get_store_process_list = get_store_process_list;
    ctx->priv.put_store_ram_val_list = put_store_ram_val_list;
    ctx->priv.get_store_ram_val_list = get_store_ram_val_list;
    ctx->priv.del_store_process_list = del_store_process_list; 
    ctx->priv.put_store_process_list_len = LENGTH_OF_ARRAY(put_store_process_list);
    ctx->priv.get_store_process_list_len = LENGTH_OF_ARRAY(get_store_process_list);
    ctx->priv.put_store_ram_val_list_len = LENGTH_OF_ARRAY(put_store_ram_val_list);
    ctx->priv.get_store_ram_val_list_len = LENGTH_OF_ARRAY(get_store_ram_val_list);
    ctx->priv.del_store_process_list_len = LENGTH_OF_ARRAY(del_store_process_list);

    ctx->user_database_reset = user_database_reset;

    thiz = ctx;
}

 