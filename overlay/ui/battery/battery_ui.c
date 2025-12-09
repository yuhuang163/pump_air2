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

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_fill_battery_status(lcd_tlvs_t *tlv)
{
    misc_manager_context_t *misc = (misc_manager_context_t *)find_module(ID_MODULE_MISC);
    if (misc) {
        tlv->type = LCD_BATTERY_STATUS;
        tlv->data_size = 1;
        tlv->data[0] = misc->get_battery_percent();
    }
}

void ui_fill_charge_status(lcd_tlvs_t *tlv)
{
    misc_manager_context_t *misc = (misc_manager_context_t *)find_module(ID_MODULE_MISC);
    if (misc) {
        tlv->type = LCD_PUMP_CHARGE_STATUS;
        tlv->data_size = 1;
        tlv->data[0] = misc->get_charge_status();
    }
}

int ui_display_charge_screen(ui_manager_context_t *thiz, ui_animation_play_t *ani)
{
    lcd_tlvs_msg_t msg = {
        .srv_id = LCD_DISPLAY_EVENT_SRV,
        .tlvs = u_malloz(sizeof(lcd_tlvs_t) * 3),
        .tlvs_num = 3,
    };
    EXCEPTION_CHECK_UINT(msg.tlvs);
    ui_fill_img_id(&msg.tlvs[0], UI_IMG_ID_CHARGE);
    ui_fill_battery_status(&msg.tlvs[1]);
    ui_fill_charge_status(&msg.tlvs[2]);
    thiz->priv.panel_dev->ioctl(thiz->priv.panel_dev, OP_CODE_PANEL_DATA_SEND, (void *)&msg);
    u_free(msg.tlvs);
    return 0;
}