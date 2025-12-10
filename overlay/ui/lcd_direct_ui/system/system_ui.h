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
/// 系统页面
int ui_display_process_logo(ui_manager_context_t *thiz, ui_animation_play_t *ani);
int ui_display_process_boot_qrcode(ui_manager_context_t *thiz, ui_animation_play_t *ani);
int ui_display_process_unlock_tip(ui_manager_context_t *thiz, ui_animation_play_t *ani);
#ifdef __cplusplus
}
#endif
#endif// __UI_DISPLAY_H__

