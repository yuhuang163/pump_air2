#ifndef __BATTERY_UI_H__
#define __BATTERY_UI_H__

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
/// 电池页面
int ui_display_charge_screen(ui_manager_context_t *thiz, ui_animation_play_t *ani);
#ifdef __cplusplus
}
#endif
#endif// __UI_DISPLAY_H__

