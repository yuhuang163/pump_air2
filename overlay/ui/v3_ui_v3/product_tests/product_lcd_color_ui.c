/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "product_tests_ui.h" // IWYU pragma: keep
#include "window_screen_product_test.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
typedef struct {
    uint8_t force_update;
    uint8_t rsv[15];
} product_lcd_color_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static product_lcd_color_display_info_t *product_info = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void lcd_color_test_ui_init(void)
{

}

static void lcd_color_test_update(ui_manager_context_t *thiz)
{
    // ui_proc_product_test_t *proc = (ui_proc_product_test_t *)thiz->priv.play_info.user_data;
    // EXCEPTION_CHECK_VOID(proc);
    // if (product_info->root == NULL || !lv_obj_is_valid(product_info->root)) {
    //     return;
    // }
    // lv_color_t color = lv_color_hex(proc->lcd_color);
    // if (lv_color_eq(color, product_info->s_color) && !product_info->force_update) {
    //     return;
    // }
    // product_info->s_color = color;
    // LOG_UI("red: %x, green: %x, blue: %x, color: %x", color.red, color.green, color.blue, proc->lcd_color);
    // lv_obj_set_style_bg_color(product_info->root, color, 0);
    // product_info->force_update = false;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_lcd_color_test_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (product_info == NULL) {
    //     product_info = (product_lcd_color_display_info_t *)u_malloz(sizeof(product_lcd_color_display_info_t));
    //     EXCEPTION_CHECK_VOID(product_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // lcd_color_test_ui_init();
    // product_info->force_update = true;
}

void ui_display_lcd_color_test_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // EXCEPTION_CHECK_VOID(product_info);
    // LVGL_OBJ_DELETE(product_info->root);
    // product_info->root = NULL;
    // u_free(product_info);
    // product_info = NULL;
}

int ui_display_lcd_color_test_process(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_INT(thiz);
    // ui_proc_product_test_t *proc = (ui_proc_product_test_t *)thiz->priv.play_info.user_data;
    // EXCEPTION_CHECK_INT(proc);
    // thiz->priv.fps_count++;
    // product_info->force_update = true;
    // lcd_color_test_update(thiz);
    // if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
    //     thiz->priv.fps_start_time = get_timestamp();
    //     LOG_UI("===> lcd_color fps: %d", thiz->priv.fps_count);
    //     thiz->priv.fps_count = 0;
    // }
    return 0;
}
