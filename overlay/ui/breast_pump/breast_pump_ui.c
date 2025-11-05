/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"
#include "ui_page_define.h"
/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void ui_fill_running_time(lcd_tlvs_t *tlv, uint16_t time_seconds)
{
    tlv->type = LCD_DEVICE_RUNNING_TIME;
    tlv->data_size = 2;
    tlv->data[0] = time_seconds >> 8;
    tlv->data[1] = time_seconds & 0xFF;
}

static void ui_fill_pump_level(lcd_tlvs_t *tlv, const pump_result_info_t *result)
{
    if (result) {
        tlv->type = LCD_PUMP_LEVEL;
        tlv->data_size = 4;
        tlv->data[0] = result->setting.speed + 1;
        tlv->data[1] = result->setting.level[0] + 1;
        tlv->data[2] = result->setting.speed + 1;
        tlv->data[3] = result->setting.level[1] + 1;
    }
}

static void ui_fill_pump_mode(lcd_tlvs_t *tlv, const pump_result_info_t *result)
{
    if (result) {
        tlv->type = LCD_PUMP_MODE;
        tlv->data_size = 1;
        tlv->data[0] = result->mode;
    }
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int ui_display_pump_home_screen(ui_animation_play_t *ani, lcd_tlvs_t **tlvs)
{
    breast_pump_manager_context_t *pump = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    const pump_result_info_t *result = pump->get_breast_pump_result(BREAST_PUMP_RESULT_PUMPING);
    uint16_t tlvs_num = 6;
    uint16_t hold_time = result->work_valid_time / 1000;
    lcd_tlvs_t *p = u_malloz(sizeof(lcd_tlvs_t) * tlvs_num);
    EXCEPTION_CHECK_UINT(p);
    ui_fill_img_id(&p[0], UI_IMG_ID_HOME_SCREEN);
    ui_fill_running_time(&p[1], hold_time);
    ui_fill_pump_level(&p[2], result);
    ui_fill_ble_status(&p[3]);
    ui_fill_battery_status(&p[4]);
    ui_fill_pump_mode(&p[5], result);
    *tlvs = p;
    LOG_UI("ui_display_pump_home_screen:%p, tlvs_num: %d", p, tlvs_num);
    return tlvs_num;
}

int ui_display_pump_left_level(ui_animation_play_t *ani, lcd_tlvs_t **tlvs)
{
    return 0;
}

int ui_display_pump_right_level(ui_animation_play_t *ani, lcd_tlvs_t **tlvs)
{
    return 0;
}

int ui_display_pump_complete_tip(ui_animation_play_t *ani, lcd_tlvs_t **tlvs)
{
    lcd_tlvs_t *p = u_malloz(sizeof(lcd_tlvs_t));
    EXCEPTION_CHECK_UINT(p);
    ui_fill_img_id(p, UI_IMG_ID_PUMP_COMPLETE_TIP);
    *tlvs = p;
    return 1;
}

int ui_display_pump_mode(ui_animation_play_t *ani, lcd_tlvs_t **tlvs)
{
    return 0;
}

int ui_display_chasing_milk_mode(ui_animation_play_t *ani, lcd_tlvs_t **tlvs)
{
    return 0;
}
