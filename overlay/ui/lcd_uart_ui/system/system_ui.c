/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"

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
void ui_fill_img_id(lcd_tlvs_t *tlv, uint16_t img_id)
{
    tlv->type = LCD_IMG_ID;
    tlv->data_size = 1;
    tlv->data[0] = img_id;
}


void ui_display_logo_prepare(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_logo_prepare");
}

void ui_display_logo_remove(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_logo_remove");
}

int ui_display_process_logo(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    lcd_tlvs_msg_t msg = {
        .srv_id = LCD_DISPLAY_EVENT_SRV,
        .tlvs = u_malloz(sizeof(lcd_tlvs_t)),
        .tlvs_num = 1,
    };
    EXCEPTION_CHECK_UINT(msg.tlvs);
    ui_fill_img_id(&msg.tlvs[0], UI_IMG_ID_LOGO);
    LOG_UI("ui_display_process_logo: %d", UI_IMG_ID_LOGO);
    thiz->priv.panel_dev->ioctl(thiz->priv.panel_dev, OP_CODE_PANEL_DATA_SEND, (void *)&msg);
    u_free(msg.tlvs);
    return 0;
}

void ui_display_boot_qrcode_prepare(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_boot_qrcode_prepare");
}

void ui_display_boot_qrcode_remove(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_unlock_tip_prepare");
}

int ui_display_process_boot_qrcode(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    lcd_tlvs_msg_t msg = {
        .srv_id = LCD_DISPLAY_EVENT_SRV,
        .tlvs = u_malloz(sizeof(lcd_tlvs_t)),
        .tlvs_num = 1,
    };
    EXCEPTION_CHECK_UINT(msg.tlvs);
    ui_fill_img_id(&msg.tlvs[0], UI_IMG_ID_BOOT_QRCODE);
    thiz->priv.panel_dev->ioctl(thiz->priv.panel_dev, OP_CODE_PANEL_DATA_SEND, (void *)&msg);
    u_free(msg.tlvs);
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
    lcd_tlvs_msg_t msg = {
        .srv_id = LCD_DISPLAY_EVENT_SRV,
        .tlvs = u_malloz(sizeof(lcd_tlvs_t)),
        .tlvs_num = 1,
    };
    EXCEPTION_CHECK_UINT(msg.tlvs);
    ui_fill_img_id(&msg.tlvs[0], UI_IMG_ID_UNLOCK_TIP);
    thiz->priv.panel_dev->ioctl(thiz->priv.panel_dev, OP_CODE_PANEL_DATA_SEND, (void *)&msg);
    u_free(msg.tlvs);
    return 0;
}