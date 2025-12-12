/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#define QRSTR_COUNT   6
static const char *system_qr_str[QRSTR_COUNT] = {
    "https://lvgl.io",
    "AAAAAAAAAAAAAAAA",
    "Hello World!",
    "LVGL IMAGES",
    "BES",
    "OK, All is done!",
};

static lv_obj_t *home_root;
/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static int system_test_qrcode_init(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);
    if (home_root == NULL || !lv_obj_is_valid(home_root)) {
        home_root = lv_qrcode_create(lv_screen_active());
    }
    lv_qrcode_set_size(home_root, 320);
    lv_qrcode_set_dark_color(home_root, fg_color);
    lv_qrcode_set_light_color(home_root, bg_color);
    lv_obj_align(home_root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_border_color(home_root, bg_color, 0);
    lv_obj_set_style_border_width(home_root, 5, 0);
    lv_qrcode_update(home_root, system_qr_str[0], strlen(system_qr_str[0]));
    return 0;
}

static int system_test_qrcode_process(void)
{
    static uint32_t index = 0;
    lv_qrcode_update(home_root, system_qr_str[index % QRSTR_COUNT], strlen(system_qr_str[index % QRSTR_COUNT]));
    index++;
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_home_prepare(ui_manager_context_t *thiz)
{
    system_test_qrcode_init();
}

void ui_display_home_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(home_root);
}

int ui_display_home_process(ui_manager_context_t *thiz)
{
    system_test_qrcode_process();
    return 0;
}
