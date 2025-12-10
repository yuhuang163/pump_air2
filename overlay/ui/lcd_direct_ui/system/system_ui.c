/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h" // IWYU pragma: keep
#include <stdint.h>

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static lv_obj_t *qrcode;
#define QRSTR_COUNT   6
static const char *system_qr_str[QRSTR_COUNT] = {
    "https://lvgl.io",
    "AAAAAAAAAAAAAAAA",
    "Hello World!",
    "LVGL IMAGES",
    "BES",
    "OK, All is done!",
};
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
int ui_display_logo_prepare(ui_manager_context_t *thiz)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);
    if (qrcode == NULL) {
        qrcode = lv_qrcode_create(lv_screen_active());
    }
    lv_qrcode_set_size(qrcode, 180);
    lv_qrcode_set_dark_color(qrcode, fg_color);
    lv_qrcode_set_light_color(qrcode, bg_color);
    lv_obj_align(qrcode, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_border_color(qrcode, bg_color, 0);
    lv_obj_set_style_border_width(qrcode, 5, 0);
    lv_qrcode_update(qrcode, system_qr_str[0], strlen(system_qr_str[0]));
    return 0;
}

int ui_display_logo_remove(ui_manager_context_t *thiz)
{
    if (qrcode) {
        lv_obj_del(qrcode);
        qrcode = NULL;
    }
    return 0;
}

int ui_display_process_logo(ui_manager_context_t *thiz)
{
    static uint32_t index = 0;
    lv_qrcode_update(qrcode, system_qr_str[index % QRSTR_COUNT], strlen(system_qr_str[index % QRSTR_COUNT]));
    TRACE(0, "system_qr_str[%d]: %s", index % QRSTR_COUNT, system_qr_str[index % QRSTR_COUNT]);
    index++;
    return 0;
}

void ui_display_boot_qrcode_prepare(ui_manager_context_t *thiz)
{

}

void ui_display_boot_qrcode_remove(ui_manager_context_t *thiz)
{

}

int ui_display_process_boot_qrcode(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}

void ui_display_unlock_tip_prepare(ui_manager_context_t *thiz)
{

}

void ui_display_unlock_tip_remove(ui_manager_context_t *thiz)
{

}

int ui_display_process_unlock_tip(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}