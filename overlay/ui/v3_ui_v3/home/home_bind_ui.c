/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "window_screen_home.h"   // IWYU pragma: keep
#include "home_ui.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_HOME_BIND_COMP_BG                         (0)
#define IS_SUPPORT_HOME_BIND_LABEL_ICON                      (0)
#define HOME_BIND_BG_COLOR                                   0xFFFFFF
#define HOME_BIND_FG_COLOR                                   0x000000
#define HOME_BIND_BORDER_COLOR                               (0xFFEAE6)  // 0xA6A6A6
#define HOME_BIND_BORDER_RADIUS                              (4)
#define HOME_BIND_TEXT_COLOR                                 (0x6A5A5B)
#define HOME_BIND_X_OFFSET                                   (111)
#define HOME_BIND_Y_OFFSET                                   (69)
#define HOME_BIND_TEXT_Y_OFFSET                              (223)
#define HOME_BIND_IP_X_OFFSET                                (0)
#define HOME_BIND_IP_Y_OFFSET                                (64)
#define HOME_BIND_STATUS_TEXT_X_OFFSET                       (0)
#define HOME_BIND_STATUS_TEXT_Y_OFFSET                       (278)
#define HOME_BIND_LABEL_X_OFFSET                             (12)
#define HOME_BIND_LABEL_Y_OFFSET                             (111)

typedef struct {
    uint8_t force_update;
    uint8_t popup_ui_index;
    uint8_t bind_state;
    uint8_t rsv[13];
    char qr_str[120];
} home_bind_display_info_t;

typedef struct {
    const char *text;
    uint16_t text_x;
    uint16_t text_y;
    uint32_t text_color;
} home_bind_text_info_t;

typedef enum {
    HOME_BIND_AND_DISCONNECT = 0, /// 绑定并断开状态
    HOME_BIND_AND_CONNECTED, /// 绑定并连接状态
    HOME_BIND_AND_CONNECTING, /// 绑定并连接中状态
    HOME_UNBIND_STATE, /// 未绑定状态
    HOME_BIND_STATE_MAX, /// 绑定状态最大值
} home_device_bind_state_e;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static home_bind_display_info_t *home_bind_info;
const static char *bind_qr_str[] = {
    "https://qr.momcozy.com/apps/?v=1&type=device&model=V3&sn=",
};

#if IS_SUPPORT_HOME_BIND_COMP_BG
static PSRAM_DATA_LOC uint8_t home_bg_data[] = {
    // #include "comparison_png2txt/home_ble_cmp_360x360.txt"
    #include "comparison_png2txt/home_ble_connected_360x360.txt"
    // #include "comparison_png2txt/home_ble_connecting_360x360.txt"
};
#endif

const static home_bind_text_info_t bind_text_infos[2] = {
    // {"Scan to connect to the app",
    //     &lv_font_oppo_sans_20_semibold, 0, HOME_BIND_TEXT_Y_OFFSET, 0x423032},
    // {"Smart pumping, milk tracking, easy\nremote control", 
    //     &lv_font_oppo_sans_14_medium, 0, 256, HOME_BIND_TEXT_COLOR}
};

const static home_bind_text_info_t bind_disconnect_infos[2] = {
    // {"Disconnect", &lv_font_oppo_sans_20_medium, HOME_BIND_STATUS_TEXT_X_OFFSET, HOME_BIND_STATUS_TEXT_Y_OFFSET, 0xB25962},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, HOME_BIND_TEXT_COLOR}
};

const static home_bind_text_info_t bind_connecting_infos[2] = {
    // {"Connecting", &lv_font_oppo_sans_20_medium, HOME_BIND_STATUS_TEXT_X_OFFSET, HOME_BIND_STATUS_TEXT_Y_OFFSET, 0xB25962},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, HOME_BIND_TEXT_COLOR}
};

const static home_bind_text_info_t bind_connected_infos[2] = {
    // {"Device connected", &lv_font_oppo_sans_20_medium, HOME_BIND_STATUS_TEXT_X_OFFSET, HOME_BIND_STATUS_TEXT_Y_OFFSET, 0xB25962},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, HOME_BIND_TEXT_COLOR}
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void ui_home_bind_root_init(void)
{

}

static void ui_home_bind_bg_update(ui_manager_context_t *thiz)
{
}

static void home_bind_qrcode_update(ui_manager_context_t *thiz, uint8_t is_show)
{  

}

static void home_bind_text_update(ui_manager_context_t *thiz, const home_bind_text_info_t *text_info)
{

}

static void home_bind_img_update(ui_manager_context_t *thiz, uint8_t is_show)
{

}

static void home_bind_status_ui_update(ui_manager_context_t *thiz, ui_proc_home_t *proc)
{
    // EXCEPTION_CHECK_VOID(thiz && proc && proc->ble);
    // int bind_state = proc->ble->ble_bind_state_get();
    // int ble_state = proc->ble->ble_is_connected();
    // uint8_t device_bind_state = HOME_UNBIND_STATE;
    // if (bind_state == BLE_BIND_STATE_COMPLETE) {
    //     device_bind_state = ble_state > 0 ? HOME_BIND_AND_CONNECTED : HOME_BIND_AND_DISCONNECT;
    // } else if (bind_state == BLE_BIND_STATE_KEY_GENERATE) {
    //     device_bind_state = HOME_BIND_AND_CONNECTING;
    // } else {
    //     device_bind_state = HOME_UNBIND_STATE;
    // }

    // if (home_bind_info->bind_state == device_bind_state && home_bind_info->force_update == false) {
    //     return;
    // }
    // home_bind_img_update(thiz, device_bind_state != HOME_UNBIND_STATE ? true : false);
    // home_bind_qrcode_update(thiz, device_bind_state == HOME_UNBIND_STATE ? true : false);
    // if (device_bind_state == HOME_BIND_AND_CONNECTED) {
    //     home_bind_text_update(thiz, bind_connected_infos);
    // } else if (device_bind_state == HOME_BIND_AND_DISCONNECT) {
    //     home_bind_text_update(thiz, bind_disconnect_infos);
    // } else if (device_bind_state == HOME_BIND_AND_CONNECTING) {
    //     home_bind_text_update(thiz, bind_connecting_infos);
    // } else {
    //     home_bind_text_update(thiz, bind_text_infos);
    // }
    // home_bind_info->bind_state = device_bind_state;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_home_bind_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (home_bind_info == NULL) {
    //     home_bind_info = (home_bind_display_info_t *)u_malloz(sizeof(home_bind_display_info_t));
    //     EXCEPTION_CHECK_VOID(home_bind_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // ui_home_bind_root_init(); /// 初始化 root 容器
    // common_ui_bg_init(thiz);
    // home_bind_info->force_update = true;
    // home_bind_info->popup_ui_index = ID_POPUP_UI_NONE;
    // storage_manager_context_t *storage = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    // if (storage) {
    //     factory_data_store_t *factory_data = storage->get_factory_data();
    //     factory_dev_info_t *dev_info = &factory_data->dev_info;
    //     unsigned int sn_n = dev_info->sn_len;
    //     if (sn_n > DEVICE_UNIQUE_SN_LEN) {
    //         sn_n = DEVICE_UNIQUE_SN_LEN;
    //     }
    //     snprintf(home_bind_info->qr_str, sizeof(home_bind_info->qr_str), "%s%.*s", bind_qr_str[0],
    //              (int)sn_n, (const char *)dev_info->sn);
    //     LOG_UI("%s", home_bind_info->qr_str);
    // } else {
    //     snprintf(home_bind_info->qr_str, sizeof(home_bind_info->qr_str), "%s", bind_qr_str[0]);
    //     LOG_UI("%s", home_bind_info->qr_str);
    // }
}

void ui_display_home_bind_remove(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_VOID(thiz && home_bind_info && home_bind_info->root);
//     common_ui_bg_deinit(thiz);
// #if IS_SUPPORT_HOME_BIND_LABEL_ICON
//     common_ui_img_resource_deinit(thiz, &home_bind_info->label_info);
// #endif
//     common_ui_img_resource_deinit(thiz, &home_bind_info->ip_info);
//     ui_common_operation_tip_remove(thiz);
//     LVGL_OBJ_DELETE(home_bind_info->root);
//     u_free(home_bind_info);
//     home_bind_info = NULL;
}

int ui_display_home_bind_process(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_INT(thiz && home_bind_info && home_bind_info->root);
    // ui_proc_home_t *proc = (ui_proc_home_t *)thiz->priv.play_info.user_data;
    // EXCEPTION_CHECK_INT(proc);
    // ui_home_bind_bg_update(thiz);
    // home_bind_status_ui_update(thiz, proc);
    // ui_common_operation_tip_update(thiz, proc->popup_ui_type, home_bind_info->root,
    //     &home_bind_info->popup_ui_index);
    // home_bind_info->force_update = false;
    // thiz->priv.fps_count++;
    // if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
    //     thiz->priv.fps_start_time = get_timestamp();
    //     LOG_UI("===> home bind fps: %d", thiz->priv.fps_count);
    //     thiz->priv.fps_count = 0;
    // }
    
    return 0;
}