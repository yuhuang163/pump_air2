/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_page_define.h" // IWYU pragma: keep
#include "window_screen_product_test.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void lcd_color_test_ui_after_policy(ui_manager_context_t *ui, int index)
{
    ui_display_lcd_color_test_process(ui);
}

static void lcd_color_test_animation_progress_cb(void *user_data, int index, ui_ani_progress_state_e state)
{
    static int ani_interval = 0;
    ui_manager_context_t *ui = user_data;
    switch (state) {
        case ANI_TIMER_DRAW_FINISH:
            break;
        case ANI_TIMER_DRAW_BEFORE:
            break;
        case ANI_TIMER_DRAW_AFTER:
            lcd_color_test_ui_after_policy(ui, index);
            break;
        default:
            break;
    }
}

static void lcd_color_test_ui_display_prepare(void *user_data)
{
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    ui_display_lcd_color_test_prepare(ui);
}

static void lcd_color_test_ui_display_remove(void *user_data)
{
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    ui_display_lcd_color_test_remove(ui);
}

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
void show_lcd_color_test_ui(ui_proc_product_test_t *proc)
{
    ui_manager_context_t *ui = proc->ui;
    EXCEPTION_CHECK_VOID(ui->hook && ui->hook->ui_animation_play_init);
    if (proc->current_ui_type == PRODUCT_TEST_UI_TYPE_LCD_COLOR) {
        proc->last_progress_ani_time = get_timestamp();
        return;
    }

    ui_play_info_t play_info = {
        .tag = "lcd_color_test_screen",
        .fps = PRODUCT_TEST_ANIMATION_FREQ,
        .progress_cb = lcd_color_test_animation_progress_cb,
        .display_prepare_cb = lcd_color_test_ui_display_prepare,
        .display_remove_cb = lcd_color_test_ui_display_remove,
        .user_data = proc,
    };
    proc->lcd_color = 0xFFFFFF;
    proc->last_progress_ani_time = get_timestamp();
    ui->hook->ui_animation_play_init(ui, play_info);
    proc->current_ui_type = PRODUCT_TEST_UI_TYPE_LCD_COLOR;
}

void lcd_color_test_keyevent_cb(ui_proc_product_test_t *proc, uint16_t keycode, uint16_t keytype)
{
    switch (keycode) {
        case KEY_CODE_POWER:
            break;
        case KEY_CODE_START:
            proc->lcd_color = 0xFF0000;
            break;
        case KEY_CODE_MODE:
            proc->lcd_color = 0x000000;
            break;
        case KEY_CODE_FREQUENCY:
            proc->lcd_color = 0xFFFF00;
            break;
        case KEY_CODE_BREASTFEEDING:
            proc->lcd_color = 0x0000FF;
            break;
        case KEY_CODE_LEFT_CONTROL:
            proc->lcd_color = 0xFFFFFF;
            break;
        case KEY_CODE_RIGHT_CONTROL:
            proc->lcd_color = 0x00FF00;
            break;
        default:
            LOG_WM("unknow keycode: %d, ignore.", keycode);
            break;
    }
}