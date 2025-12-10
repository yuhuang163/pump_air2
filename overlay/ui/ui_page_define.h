#ifndef __UI_PAGE_DEFINE_H__
#define __UI_PAGE_DEFINE_H__

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
/// 填充TLV数据
void ui_fill_ble_status(lcd_tlvs_t *tlv);
void ui_fill_battery_status(lcd_tlvs_t *tlv);
void ui_fill_charge_status(lcd_tlvs_t *tlv);
void ui_fill_img_id(lcd_tlvs_t *tlv, uint16_t img_id);

/// 系统页面
int ui_display_process_logo(ui_manager_context_t *thiz, ui_animation_play_t *ani);
int ui_display_process_boot_qrcode(ui_manager_context_t *thiz, ui_animation_play_t *ani);
int ui_display_process_unlock_tip(ui_manager_context_t *thiz, ui_animation_play_t *ani);

/// 电池页面
int ui_display_charge_screen(ui_manager_context_t *thiz, ui_animation_play_t *ani);

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

#endif 


