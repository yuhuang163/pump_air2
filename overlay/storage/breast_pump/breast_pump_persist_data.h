#ifndef __BREAST_PUMP_PERSIST_DATA_H__
#define __BREAST_PUMP_PERSIST_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep
#include "storage_manager.h" // IWYU pragma: keep

/*
* INTERNAL FUNCTION DECLARATION
*****************************************************************************************
*/
// 泵奶配置接口
void storage_reset_pump_info(storage_manager_context_t *ctx, sys_pump_info_t *pump_info);
int storage_write_pump_info_to_flash(store_func_args_t args);
int storage_read_pump_info_from_flash(store_func_args_t args);

// 泵奶数据子项接口
void storage_reset_pump_data_head(storage_manager_context_t *ctx, breast_pump_data_head_t *pump_data_head);
int storage_write_pump_data_head_to_flash(store_func_args_t args);
int storage_read_pump_data_head_from_flash(store_func_args_t args);
int storage_write_pump_data_body_to_flash(store_func_args_t args);
int storage_read_pump_data_body_from_flash(store_func_args_t args);
int storage_del_pump_data_body_from_flash(store_func_args_t args);

// 刷牙配置子项接口
int storage_write_pump_time_to_ram(store_func_args_t args);
int storage_write_pump_exception_remind_to_ram(store_func_args_t args);
int storage_write_pump_mode_change_remind_to_ram(store_func_args_t args);
int storage_write_pump_finish_remind_to_ram(store_func_args_t args);

// 泵奶模式力度信息接口
int storage_write_pump_level_to_ram(store_func_args_t args);
int storage_read_pump_level_from_ram(store_func_args_t args);

int pump_data_info_sync_flash(storage_manager_context_t *ctx, pump_result_info_t *body);
int get_pump_data_info_from_flash(int num, void *body);
int delete_pump_data_info_from_flash(int num);
int fill_in_pump_data_info(int num);
#ifdef __cplusplus
}
#endif

#endif 







