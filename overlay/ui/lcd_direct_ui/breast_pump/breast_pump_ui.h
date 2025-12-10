#ifndef __BREAST_PUMP_UI_H__
#define __BREAST_PUMP_UI_H__

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
/// 泵奶页面
int ui_display_pump_home_screen(ui_manager_context_t *thiz, ui_animation_play_t *ani);
int ui_display_pump_left_level(ui_manager_context_t *thiz, ui_animation_play_t *ani);
int ui_display_pump_right_level(ui_manager_context_t *thiz, ui_animation_play_t *ani);
int ui_display_pump_complete_tip(ui_manager_context_t *thiz, ui_animation_play_t *ani);
int ui_display_pump_mode(ui_manager_context_t *thiz, ui_animation_play_t *ani);
int ui_display_breastfeeding_mode(ui_manager_context_t *thiz, ui_animation_play_t *ani);
int ui_display_pump_speed(ui_manager_context_t *thiz, ui_animation_play_t *ani);

#ifdef __cplusplus
}
#endif
#endif// __UI_DISPLAY_H__

