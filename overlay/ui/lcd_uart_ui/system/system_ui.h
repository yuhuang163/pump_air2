#ifndef __SYSTEM_UI_H__
#define __SYSTEM_UI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep
#include "ui_manager_api.h" // IWYU pragma: keep

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
/// 填充图片ID
void ui_fill_img_id(lcd_tlvs_t *tlv, uint16_t img_id);
/// 开机LOGO页面
void ui_display_logo_prepare(ui_manager_context_t *thiz);
void ui_display_logo_remove(ui_manager_context_t *thiz);
int ui_display_process_logo(ui_manager_context_t *thiz);
/// 开机二维码页面
void ui_display_boot_qrcode_prepare(ui_manager_context_t *thiz);
void ui_display_boot_qrcode_remove(ui_manager_context_t *thiz);
int ui_display_process_boot_qrcode(ui_manager_context_t *thiz);
/// 解锁提示页面
void ui_display_unlock_tip_prepare(ui_manager_context_t *thiz);
void ui_display_unlock_tip_remove(ui_manager_context_t *thiz);
int ui_display_process_unlock_tip(ui_manager_context_t *thiz);
#ifdef __cplusplus
}
#endif
#endif// __UI_DISPLAY_H__

