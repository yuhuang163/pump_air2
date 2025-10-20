#ifndef __SYSTEM_PERSIST_DATA_H__
#define __SYSTEM_PERSIST_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "storage_manager_api.h" // IWYU pragma: keep
#include "persist_storage.h" // IWYU pragma: keep

/*
* INTERNAL FUNCTION DECLARATION
*****************************************************************************************
*/
/// 系统配置接口
void storage_reset_sys_config_info(storage_manager_context_t *ctx, sys_config_info_t *sys_config);
int storage_write_sys_config_info_to_flash(store_func_args_t args);
int storage_read_sys_config_info_from_flash(store_func_args_t args);
/// 系统状态接口
void storage_reset_sys_state_info(storage_manager_context_t *ctx, sys_state_info_t *sys_state);
int storage_write_sys_state_info_to_flash(store_func_args_t args);
int storage_read_sys_state_info_from_flash(store_func_args_t args);
/// 用户绑定接口
void storage_reset_user_bind_info(storage_manager_context_t *ctx, user_bind_info_t *user_bind);
int storage_write_user_bind_info_to_flash(store_func_args_t args);
int storage_read_user_bind_info_from_flash(store_func_args_t args);
int storage_user_is_binded_from_ram(store_func_args_t args);
/// UTC时间接口
int storage_write_utc_timestamp_to_flash(store_func_args_t args);
int storage_read_utc_timestamp_from_flash(store_func_args_t args);
/// 系统时区
int storage_write_sys_time_zone_to_ram(store_func_args_t args);
int storage_read_sys_time_zone_from_ram(store_func_args_t args);
/// 飞行/旅行锁
int storage_write_ap_mode_onoff_to_ram(store_func_args_t args);
int storage_read_ap_mode_onoff_from_ram(store_func_args_t args);
/// 静默升级
int storage_write_slient_upgrade_onoff_to_ram(store_func_args_t args);
int storage_read_slient_upgrade_onoff_from_ram(store_func_args_t args);
/// UI风格(主题)
int storage_write_sys_ui_style_to_ram(store_func_args_t args);
int storage_read_sys_ui_style_from_ram(store_func_args_t args);
/// 日志开关
int storage_write_log_switch_to_ram(store_func_args_t args);
int storage_read_log_switch_from_ram(store_func_args_t args);
/// 系统语言
int storage_write_language_to_ram(store_func_args_t args);
int storage_read_language_from_ram(store_func_args_t args);
/// UI部署(引导)
int storage_write_guide_deployed_to_ram(store_func_args_t args);
int storage_read_guide_deployed_from_ram(store_func_args_t args);
/// 电池健康状态
int storage_write_battery_health_to_ram(store_func_args_t args);
int storage_read_battery_health_from_ram(store_func_args_t args);
/// 上一次数据同步时间
int storage_write_sync_sys_time_to_ram(store_func_args_t args);
int storage_read_sync_sys_time_from_ram(store_func_args_t args);
/// 日志过滤
int storage_write_log_filter_to_ram(store_func_args_t args);
int storage_read_log_filter_from_ram(store_func_args_t args);
#ifdef __cplusplus
}
#endif

#endif 







