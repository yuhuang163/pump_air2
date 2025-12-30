/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "home_ui.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_SCAN_QRCODE_TIP                             0
#define FACTORY_HOME_CONTENT_COUNT                             1
#define FACTORY_HOME_CONTENT_COLOR                             0x3C3D40
#define FACTORY_HOME_QRCODE_BG_COLOR                           0xFFFFFF
#define FACTORY_HOME_QRCODE_FG_COLOR                           0x000000
#define FACTORY_HOME_QRCODE_BORDER_COLOR                       0xA6A6A6
#define FACTORY_HOME_MAC_NAME_COLOR                            0x01D65A // 0x3C3D40
#if IS_SUPPORT_SCAN_QRCODE_TIP
#define FACTORY_HOME_MAC_NAME_X                                (105 + 4)
#define FACTORY_HOME_MAC_NAME_Y                                (80)
#define FACTORY_HOME_QRCODE_X                                  (FACTORY_HOME_MAC_NAME_X)
#define FACTORY_HOME_QRCODE_Y                                  (FACTORY_HOME_MAC_NAME_Y + 20 + 5)
#define FACTORY_HOME_QRCODE_SIZE                               (150)
#else
#define FACTORY_HOME_MAC_NAME_X                                (90 + 4)
#define FACTORY_HOME_MAC_NAME_Y                                (80)
#define FACTORY_HOME_QRCODE_X                                  (FACTORY_HOME_MAC_NAME_X)
#define FACTORY_HOME_QRCODE_Y                                  (FACTORY_HOME_MAC_NAME_Y + 20 + 10)
#define FACTORY_HOME_QRCODE_SIZE                               (180)
#endif

typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
} factory_content_info_t;

typedef struct {  
    lv_obj_t * root;
    lv_obj_t * qrcode;
    lv_obj_t * mac_name;
#if IS_SUPPORT_SCAN_QRCODE_TIP
    lv_obj_t * text;
#endif
} factory_home_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_SCAN_QRCODE_TIP
const static factory_content_info_t factory_content_infos[FACTORY_HOME_CONTENT_COUNT] = {
    { "Please scan the QR code\nto quickly connect the device", (63 + 6), (250 + 20)},
};  
#endif

static factory_home_display_info_t *factory_info = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void factory_ui_root_init(void)
{
    if (factory_info->root == NULL || !lv_obj_is_valid(factory_info->root)) {
        factory_info->root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(factory_info->root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(factory_info->root, lv_color_white(), 0);     /// 设置背景颜色为白色
    lv_obj_set_style_bg_opa(factory_info->root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(factory_info->root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(factory_info->root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(factory_info->root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(factory_info->root, LV_OBJ_FLAG_CLICKABLE);           /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(factory_info->root, LV_OBJ_FLAG_SCROLLABLE);          /// 禁止滚动
}

#if IS_SUPPORT_SCAN_QRCODE_TIP
static void factory_text_update(ui_manager_context_t *thiz)
{
    if (factory_info->text == NULL || !lv_obj_is_valid(factory_info->text)) {
        factory_info->text = lv_label_create(factory_info->root);
    }

    lv_label_set_text(factory_info->text, factory_content_infos[0].name);
    lv_obj_set_style_text_color(factory_info->text, lv_color_hex(FACTORY_HOME_CONTENT_COLOR), 0);
    lv_obj_set_style_text_font(factory_info->text, &lv_font_roboto_17_regular, 0);  /// 设置字体
    lv_obj_set_style_text_line_space(factory_info->text, 4, 0);  /// 设置行间距（像素）
    // lv_obj_set_width(factory_info->text, LV_PCT(100));  /// 设置label宽度为屏幕宽度以便文本居中
    lv_obj_set_style_text_align(factory_info->text, LV_TEXT_ALIGN_CENTER, 0);  /// 设置文本居中对齐
    lv_obj_align(factory_info->text, LV_ALIGN_TOP_LEFT, 
        factory_content_infos[0].text_x, factory_content_infos[0].text_y);
}
#endif

static void factory_mac_name_update(ui_manager_context_t *thiz)
{
    if (factory_info->mac_name == NULL || !lv_obj_is_valid(factory_info->mac_name)) {
        factory_info->mac_name = lv_label_create(factory_info->root);
    }
    
    char mac_name[16] = "V3-BF03";
    lv_label_set_text(factory_info->mac_name, mac_name);
    lv_obj_set_style_text_color(factory_info->mac_name, lv_color_hex(FACTORY_HOME_MAC_NAME_COLOR), 0);
    lv_obj_set_style_text_font(factory_info->mac_name, &lv_font_roboto_25_medium, 0);  /// 设置字体
    // lv_obj_set_style_text_line_space(factory_info->mac_name, 4, 0);  /// 设置行间距（像素）
    // lv_obj_set_style_text_align(factory_info->mac_name, LV_TEXT_ALIGN_CENTER, 0);  /// 设置文本居中对齐
    lv_obj_align(factory_info->mac_name, LV_ALIGN_TOP_LEFT, 
        FACTORY_HOME_MAC_NAME_X, FACTORY_HOME_MAC_NAME_Y);
}

static void factory_qrcode_update(ui_manager_context_t *thiz)
{      
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);
    if (factory_info->qrcode == NULL || !lv_obj_is_valid(factory_info->qrcode)) {
        factory_info->qrcode = lv_qrcode_create(factory_info->root);
        EXCEPTION_CHECK_VOID(factory_info->qrcode);
    }
    
    lv_qrcode_set_size(factory_info->qrcode, FACTORY_HOME_QRCODE_SIZE);
    lv_qrcode_set_dark_color(factory_info->qrcode, fg_color);
    lv_qrcode_set_light_color(factory_info->qrcode, bg_color);
    lv_obj_set_style_border_color(factory_info->qrcode, bg_color, 0);
    char qr_str[32] = {0};
    snprintf(qr_str, sizeof(qr_str), "BA:03:00:00:00:00");
    lv_qrcode_update(factory_info->qrcode, qr_str, strlen(qr_str));
    lv_obj_center(factory_info->qrcode);
    lv_obj_align(factory_info->qrcode, LV_ALIGN_TOP_LEFT, FACTORY_HOME_QRCODE_X, FACTORY_HOME_QRCODE_Y);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_factory_home_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (factory_info == NULL) {
        factory_info = (factory_home_display_info_t *)u_malloz(sizeof(factory_home_display_info_t));
        EXCEPTION_CHECK_VOID(factory_info);
    }
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    factory_ui_root_init();
}

void ui_display_factory_home_remove(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (factory_info == NULL) {
        return;
    }
    LVGL_OBJ_DELETE(factory_info->root);
    u_free(factory_info);
    factory_info = NULL;
}

int ui_display_factory_home_process(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(factory_info);
    EXCEPTION_CHECK_INT(factory_info->root);
    ble_ui_update(thiz, factory_info->root, 1);
    battery_icon_update(thiz, factory_info->root, 1, 0);
    factory_qrcode_update(thiz);
#if IS_SUPPORT_SCAN_QRCODE_TIP
    factory_text_update(thiz);
#endif
    factory_mac_name_update(thiz);
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOGI("===> factory home fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }

    return 0;
}
