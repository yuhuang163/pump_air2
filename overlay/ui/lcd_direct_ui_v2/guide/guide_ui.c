/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep
#include <stdint.h>


typedef enum {
    GUIDE_PAGE_ID_1 = 0,
    GUIDE_PAGE_ID_2,
    GUIDE_PAGE_ID_3,
    GUIDE_PAGE_ID_4,
    GUIDE_PAGE_ID_5,
    GUIDE_PAGE_ID_6,
    GUIDE_PAGE_ID_8
} guide_page_id_t;

#define UI_GUIDE_ICON_MAX_COUNT                  (10)   

#define ID_TEXT_AREA_ICON_X                      (100)
#define ID_TEXT_AREA_ICON_Y                      (100)

#define ID_TEXT1_AREA_ICON_X                     (100)
#define ID_TEXT1_AREA_ICON_Y                     (100)

#define UI_GUIDE_QR_CODE_INDEX                    (1)
#define ID_QR_CODE_ICON_X                        (112)
#define ID_QR_CODE_ICON_Y                        (72)

#define UI_GUIDE_KNR_START_INDEX                 (7)
#define ID_KNR_START_ICON_X                      (100)
#define ID_KNR_START_ICON_Y                      (100)

#define ID_PLUGIN_ICON_X                         (100)
#define ID_PLUGIN_ICON_Y                         (100)

#define UI_GUIDE_PUMPING_THUB_INDEX              (6)
#define ID_THUMBNAIL_ICON_X                      (100)
#define ID_THUMBNAIL_ICON_Y                      (100)

#define UI_GUIDE_KNR_TURN_INDEX                  (8)
#define ID_KNR_TURN_ICON_X                       (140)
#define ID_KNR_TURN_ICON_Y                       (47)

#define ID_BUTTON_ICON_X                         (80)
#define ID_BUTTON_ICON_Y                         (80)

#define UI_GUIDE_ARROW_INDEX                     (5)
#define ID_ARROW_ICON_X                          (80)
#define ID_ARROW_ICON_Y                          (80)

#define ID_TEXT_COLOR                            (0xB25962)
#define GUIDE_TEXT01_LABEL_W                     (280)
#define GUIDE_TEXT01_LABEL_H                     (40)
#define GUIDE_TEXT01_TOP_OFFSET                  (204)
#define GUIDE_MORE_OPTIONS_COLOR                 (0xB25962)
#define GUIDE_MORE_OPTIONS_OPA                   (191)  /* 75% of 255 */

/*
* DEFINES
*****************************************************************************************
*/
#define LOGO_ARC_COLOR                  (0xB25962)
#define LOGO_ARC_BG_COLOR               (0xFFFFFF)  /* 背景圆弧颜色：黑色 */
#define LOGO_ARC_BG_OPA                 (38)        /* 背景圆弧透明度 15%（255*15/100≈38） */
#define LOGO_ARC_SIZE                   (28)    /* 直径（原80缩小20%） */
#define LOGO_ARC_WIDTH                  (3)     /* 圆弧线宽（原4加粗50%） */
#define LOGO_ARC_ANGLE_SPAN             (90)    /* 25% of 360 */
#define LOGO_ARC_LOOP_MS_PER_DEG        (3)    /* ms per degree，越小转得越快（原15约5.4s/圈，现约2.2s/圈） */
#define LOGO_LABEL_GAP                  (16)   /* 圆弧与文字间距 */

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
typedef struct {
    lv_obj_t * root;
    lv_obj_t * backgroud;
    res_img_info_t background_info;
    lv_draw_buf_t *background_snapshot;  /// 背景快照，避免重复解码

    lv_obj_t * guide_icon_id01;
    res_img_info_t guide_icon_id01_info;

    lv_obj_t * guide_icon_id02;
    res_img_info_t guide_icon_id02_info;

    lv_obj_t * guide_icon_id03;
    res_img_info_t guide_icon_id03_info;

    lv_obj_t * guide_text_label01; 

    lv_obj_t * guide_text_label02; 

    lv_obj_t * guide_text_label03;

    lv_obj_t *logo_arc;

    lv_obj_t *logo_label;

    res_img_info_t logo_img_info;

    uint8_t force_update;

    uint32_t start_time;

    lv_fs_path_ex_t background_path;
    lv_fs_path_ex_t guide_icon_id01_path;
    lv_fs_path_ex_t guide_icon_id02_path;
    lv_fs_path_ex_t guide_icon_id03_path;

} ui_guide_info_t;

typedef struct {
    const char *name;
    uint16_t img_id;
    uint16_t icon_x;
    uint16_t icon_y;
} ui_guide_icon_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

static ui_guide_info_t *ui_guide_info;

const static ui_guide_icon_info_t ui_guide_icon_infos[UI_GUIDE_ICON_MAX_COUNT] = {
    { "idle_text_label", 0, 
        ID_TEXT_AREA_ICON_X,       ID_TEXT_AREA_ICON_Y, 
    },
    { "idle_qr_icon", RESOURCE_ID_GUIDE_QR_ICON, 
      ID_QR_CODE_ICON_X,       ID_QR_CODE_ICON_Y, 
    },
    { "idle_lrm_icon", RESOURCE_ID_GUIDE_LRM_ICON,     
      ID_BUTTON_ICON_X,              ID_BUTTON_ICON_Y, 
    },
    { "idle_playpause_icon", RESOURCE_ID_GUIDE_PLAYPAUSE_ICON, 
      ID_BUTTON_ICON_X,       ID_BUTTON_ICON_Y, 
    },
    { "idle_pragram_icon", RESOURCE_ID_GUIDE_MODESTAR_ICON,     
      ID_BUTTON_ICON_X,              ID_BUTTON_ICON_Y, 
    },
    { "idle_arrow_icon", RESOURCE_ID_GUIDE_ARROW_ICON, 
      ID_ARROW_ICON_X,       ID_ARROW_ICON_Y, 
    },
    { "idle_pumping_abb_icon", RESOURCE_ID_GUIDE_PUMPING_ABB_ICON, 
      ID_ARROW_ICON_X,       ID_ARROW_ICON_Y, 
    },
    { "idle_pumping_start_abb_icon", RESOURCE_ID_GUIDE_PUMPING_START_ABB_ICON, 
      ID_KNR_TURN_ICON_X,       ID_KNR_TURN_ICON_Y, 
    },
    { "idle_kn_abb_0_icon", RESOURCE_ID_GUIDE_KN_ABB_0, 
      0,       ID_KNR_TURN_ICON_Y, 
    },
    { "idle_kn_abb_1_icon", RESOURCE_ID_GUIDE_KN_ABB_1, 
      0,       ID_KNR_TURN_ICON_Y, 
    }
};
/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void ui_guide_root_init(void)
{
    if (ui_guide_info->root == NULL || !lv_obj_is_valid(ui_guide_info->root)) {
        ui_guide_info->root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(ui_guide_info->root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(ui_guide_info->root, lv_color_white(), 0);     /// 设置背景颜色为黑色
    lv_obj_set_style_bg_opa(ui_guide_info->root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(ui_guide_info->root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(ui_guide_info->root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(ui_guide_info->root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(ui_guide_info->root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(ui_guide_info->root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
}

static void ui_guide_root01_init(void)
{
    if (ui_guide_info->root == NULL || !lv_obj_is_valid(ui_guide_info->root)) {
        ui_guide_info->root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(ui_guide_info->root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    //lv_obj_set_style_bg_color(ui_guide_info->root, lv_color_white(), 0);     /// 设置背景颜色为黑色
    lv_obj_set_style_bg_opa(ui_guide_info->root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(ui_guide_info->root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(ui_guide_info->root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(ui_guide_info->root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(ui_guide_info->root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(ui_guide_info->root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
}

static void ui_guide_bg_update(ui_manager_context_t *thiz)
{
    if (ui_guide_info->force_update == false) {
        return;
    }

    if (ui_guide_info->backgroud == NULL || !lv_obj_is_valid(ui_guide_info->backgroud)) {
        ui_guide_info->backgroud = lv_image_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->backgroud);
#if LV_USE_SNAPSHOT
        if (ui_guide_info->background_snapshot != NULL) {
            lv_draw_buf_destroy(ui_guide_info->background_snapshot);
            ui_guide_info->background_snapshot = NULL;
        }
#endif
        common_ui_img_resource_init(thiz, RESOURCE_ID_PUMP_HOME_BG, &ui_guide_info->background_info);
        EXCEPTION_CHECK_VOID(ui_guide_info->background_info.data);
        lv_fs_make_path_from_buffer(&ui_guide_info->background_path,
                                    LV_FS_MEMFS_LETTER,
                                    ui_guide_info->background_info.data,
                                    ui_guide_info->background_info.size,
                                    "baj");
        lv_image_set_src(ui_guide_info->backgroud, (const void *)&ui_guide_info->background_path);
        lv_obj_align(ui_guide_info->backgroud, LV_ALIGN_CENTER, 0, 0);
#if LV_USE_SNAPSHOT
        lv_refr_now(NULL); /// 等待渲染完成后再创建快照
        ui_guide_info->background_snapshot = lv_snapshot_take(ui_guide_info->backgroud, LV_COLOR_FORMAT_NATIVE_WITH_ALPHA);
#endif
    }

#if LV_USE_SNAPSHOT
    if (ui_guide_info->background_snapshot != NULL) {
        lv_image_set_src(ui_guide_info->backgroud, ui_guide_info->background_snapshot);
    }
#endif
    lv_obj_move_background(ui_guide_info->backgroud);
    lv_obj_move_to_index(ui_guide_info->backgroud, 0);
}

static void ui_guide_pid1_update(ui_manager_context_t *thiz)
{
    if (ui_guide_info == NULL) {
        return;
    }
    /* 创建 More pumping options 文本框：277x40，左右居中，距上204，OPPO Sans 14 Medium/Bold，#B25962 75% */
    if (ui_guide_info->guide_text_label01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label01)) {
        ui_guide_info->guide_text_label01 = lv_label_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label01);
            lv_obj_set_size(ui_guide_info->guide_text_label01, 264, 24);
            lv_label_set_text(ui_guide_info->guide_text_label01, "Pumping Trip");
            lv_label_set_long_mode(ui_guide_info->guide_text_label01, LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(ui_guide_info->guide_text_label01, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_set_style_text_color(ui_guide_info->guide_text_label01, lv_color_hex(ID_TEXT_COLOR), 0);
            lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label01, 11, 0);  /* line height 24px (font ~13 + 11) */
            lv_obj_set_style_text_font(ui_guide_info->guide_text_label01, &lv_font_oppo_sans_20_medium, 0);
            lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
            lv_obj_clear_flag(ui_guide_info->guide_text_label01, LV_OBJ_FLAG_SCROLLABLE);
            //lv_obj_align_to(ui_guide_info->guide_text_label01, ui_guide_info->guide_icon_id01, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
            lv_obj_align(ui_guide_info->guide_text_label01, LV_ALIGN_TOP_MID, 0, 48);
    }
    
    /* 2. 副文案 "2 min Stimulation..."：在 label01 下方，14px */
    if (ui_guide_info->guide_text_label03 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label03)) {
        ui_guide_info->guide_text_label03 = lv_label_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label03);
        lv_obj_set_size(ui_guide_info->guide_text_label03, 264, 20);
        lv_label_set_long_mode(ui_guide_info->guide_text_label03, LV_LABEL_LONG_WRAP);
        lv_label_set_text(ui_guide_info->guide_text_label03, "Turn right to start");
        lv_obj_set_style_text_align(ui_guide_info->guide_text_label03, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(ui_guide_info->guide_text_label03, lv_color_hex(ID_TEXT_COLOR), 0);
        lv_obj_set_style_text_font(ui_guide_info->guide_text_label03, &lv_font_oppo_sans_14_medium, 0);
        lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label03, 9, 0);  /* line height 24px (font ~13 + 11) */
        lv_obj_set_style_text_opa(ui_guide_info->guide_text_label03, 191, 0);  /* 75% 透明度 (255*75/100) */
        lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label03, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_opa(ui_guide_info->guide_text_label03, LV_OPA_TRANSP, 0);
        lv_obj_clear_flag(ui_guide_info->guide_text_label03, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_align_to(ui_guide_info->guide_text_label03, ui_guide_info->guide_text_label01, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    }
    /* 3. 图标 guide_icon_id01：在 label03 下方 */
    const ui_guide_icon_info_t *icon_id01 = &ui_guide_icon_infos[9];
    if (icon_id01->img_id != 0 && (ui_guide_info->guide_icon_id01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id01))) {
        ui_guide_info->guide_icon_id01 = lv_image_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01);
        common_ui_img_resource_init(thiz, icon_id01->img_id, &ui_guide_info->guide_icon_id01_info);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01_info.data);
        lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id01_path,
                                    LV_FS_MEMFS_LETTER,
                                    ui_guide_info->guide_icon_id01_info.data,
                                    ui_guide_info->guide_icon_id01_info.size,
                                    "png");
        lv_image_set_src(ui_guide_info->guide_icon_id01, (const void *)&ui_guide_info->guide_icon_id01_path);
        /* RESOURCE_ID_GUIDE_KN_ABB_0/1：距左 57，距右 81，距顶 122（TOP_LEFT 57,122） */
        lv_obj_align(ui_guide_info->guide_icon_id01, LV_ALIGN_TOP_LEFT, 57, 122);
    }
        /* 创建文本框下方的箭头图标 */
        const ui_guide_icon_info_t *icon_id03 = &ui_guide_icon_infos[UI_GUIDE_ARROW_INDEX]; /* RESOURCE_ID_GUIDE_ARROW_ICON */
        if (icon_id03->img_id != 0) {
            if (ui_guide_info->guide_icon_id03 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
                ui_guide_info->guide_icon_id03 = lv_image_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03);
                common_ui_img_resource_init(thiz, icon_id03->img_id, &ui_guide_info->guide_icon_id03_info);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03_info.data);
                lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id03_path,
                                        LV_FS_MEMFS_LETTER,
                                        ui_guide_info->guide_icon_id03_info.data,
                                        ui_guide_info->guide_icon_id03_info.size,
                                        "png");
                lv_image_set_src(ui_guide_info->guide_icon_id03, (const void *)&ui_guide_info->guide_icon_id03_path);
                // lv_obj_align_to(ui_guide_info->guide_icon_id03, ui_guide_info->guide_icon_id02, 
                //                 LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
               lv_obj_align(ui_guide_info->guide_icon_id03, LV_ALIGN_TOP_MID, 0, 300);
            }
        }

        /* 创建箭头下方的文本框 */
        if (ui_guide_info->guide_icon_id03 != NULL && lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
            if (ui_guide_info->guide_text_label02 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label02)) {
                ui_guide_info->guide_text_label02 = lv_label_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label02);
                lv_obj_set_size(ui_guide_info->guide_text_label02, 264, 16);
                lv_label_set_text(ui_guide_info->guide_text_label02, "Press down to exit");
                lv_label_set_long_mode(ui_guide_info->guide_text_label02, LV_LABEL_LONG_WRAP);
                lv_obj_set_style_text_align(ui_guide_info->guide_text_label02, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_style_text_color(ui_guide_info->guide_text_label02, lv_color_hex(ID_TEXT_COLOR), 0);
                lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label02, 7, 0);  /* line height 24px (font ~13 + 11) */
                lv_obj_set_style_text_opa(ui_guide_info->guide_text_label02, 191, 0);  /* 75% 透明度 (255*75/100) */
                lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_set_style_border_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_clear_flag(ui_guide_info->guide_text_label02, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_align_to(ui_guide_info->guide_text_label02, ui_guide_info->guide_icon_id03,
                                LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
            }
        }
    
}

static void ui_guide_pid2_update(ui_manager_context_t *thiz)
{
    if (ui_guide_info == NULL) {
        return;
    }
    /* 创建 More pumping options 文本框：277x40，左右居中，距上204，OPPO Sans 14 Medium/Bold，#B25962 75% */
    if (ui_guide_info->guide_text_label01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label01)) {
        ui_guide_info->guide_text_label01 = lv_label_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label01);
            lv_obj_set_size(ui_guide_info->guide_text_label01, 264, 24);
            lv_label_set_text(ui_guide_info->guide_text_label01, "Regular Pumping");
            lv_label_set_long_mode(ui_guide_info->guide_text_label01, LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(ui_guide_info->guide_text_label01, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_set_style_text_color(ui_guide_info->guide_text_label01, lv_color_hex(ID_TEXT_COLOR), 0);
            lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label01, 7, 0);  /* line height 24px (font ~13 + 11) */
            lv_obj_set_style_text_font(ui_guide_info->guide_text_label01, &lv_font_oppo_sans_20_medium, 0);
            lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
        lv_obj_clear_flag(ui_guide_info->guide_text_label01, LV_OBJ_FLAG_SCROLLABLE);
        //lv_obj_align_to(ui_guide_info->guide_text_label01, ui_guide_info->guide_icon_id01, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
        lv_obj_align(ui_guide_info->guide_text_label01, LV_ALIGN_TOP_MID, 0, 48);
    }
    
    /* 2. 副文案 "2 min Stimulation..."：在 label01 下方，14px */
    if (ui_guide_info->guide_text_label03 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label03)) {
        ui_guide_info->guide_text_label03 = lv_label_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label03);
        lv_obj_set_size(ui_guide_info->guide_text_label03, 264, GUIDE_TEXT01_LABEL_H);
        lv_label_set_long_mode(ui_guide_info->guide_text_label03, LV_LABEL_LONG_WRAP);
        lv_label_set_text(ui_guide_info->guide_text_label03, "2 min Stimulation + 20 min Expression for most moms. Press to start");
        lv_obj_set_style_text_align(ui_guide_info->guide_text_label03, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(ui_guide_info->guide_text_label03, lv_color_hex(ID_TEXT_COLOR), 0);
        lv_obj_set_style_text_opa(ui_guide_info->guide_text_label03, 191, 0);  /* 75% 透明度 (255*75/100) */
        lv_obj_set_style_text_font(ui_guide_info->guide_text_label03, &lv_font_oppo_sans_14_medium, 0);
        lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label03, 9, 0);  /* line height 24px (font ~13 + 11) */
        lv_obj_set_style_text_opa(ui_guide_info->guide_text_label03, 191, 0);  /* 75% 透明度 (255*75/100) */
        lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label03, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_opa(ui_guide_info->guide_text_label03, LV_OPA_TRANSP, 0);
        lv_obj_clear_flag(ui_guide_info->guide_text_label03, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_align_to(ui_guide_info->guide_text_label03, ui_guide_info->guide_text_label01, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    }
    /* 3. 图标 guide_icon_id01：在 label03 下方 */
    const ui_guide_icon_info_t *icon_id01 = &ui_guide_icon_infos[UI_GUIDE_KNR_START_INDEX];
    if (icon_id01->img_id != 0 && (ui_guide_info->guide_icon_id01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id01))) {
        ui_guide_info->guide_icon_id01 = lv_image_create(ui_guide_info->root);
    // const ui_guide_icon_info_t *icon_id01 = &ui_guide_icon_infos[UI_GUIDE_KNR_START_INDEX];
    // if (icon_id01->img_id == 0) {
    //     return;  /* 避免用 img_id=0 加载，资源里无 id=0 */
    // }
    // if (ui_guide_info->guide_icon_id01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id01)) {
    //     ui_guide_info->guide_icon_id01 = lv_image_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01);
        common_ui_img_resource_init(thiz, icon_id01->img_id, &ui_guide_info->guide_icon_id01_info);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01_info.data);
        lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id01_path,
                                    LV_FS_MEMFS_LETTER,
                                    ui_guide_info->guide_icon_id01_info.data,
                                    ui_guide_info->guide_icon_id01_info.size,
                                    "png");
        lv_image_set_src(ui_guide_info->guide_icon_id01, (const void *)&ui_guide_info->guide_icon_id01_path);
        /* 左右居中，距离背景图上部32 */
        //lv_obj_align_to(ui_guide_info->guide_icon_id01, ui_guide_info->guide_text_label01, LV_ALIGN_OUT_BOTTOM_MID, 0, 59);
        //lv_obj_align(ui_guide_info->guide_icon_id01, LV_ALIGN_TOP_MID, 0, 101);
        lv_obj_align(ui_guide_info->guide_icon_id01, LV_ALIGN_TOP_MID, 0, 95);
    }

        /* 创建文本框下方的箭头图标 */
        const ui_guide_icon_info_t *icon_id03 = &ui_guide_icon_infos[UI_GUIDE_ARROW_INDEX]; /* RESOURCE_ID_GUIDE_ARROW_ICON */
        if (icon_id03->img_id != 0) {
            if (ui_guide_info->guide_icon_id03 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
                ui_guide_info->guide_icon_id03 = lv_image_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03);
                common_ui_img_resource_init(thiz, icon_id03->img_id, &ui_guide_info->guide_icon_id03_info);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03_info.data);
                lv_fs_path_ex_t arrow_img_path;
                lv_fs_make_path_from_buffer(&arrow_img_path, LV_FS_MEMFS_LETTER,
                    ui_guide_info->guide_icon_id03_info.data, ui_guide_info->guide_icon_id03_info.size, "png");
                lv_image_set_src(ui_guide_info->guide_icon_id03, (const void *)&arrow_img_path);
                // lv_obj_align_to(ui_guide_info->guide_icon_id03, ui_guide_info->guide_icon_id02, 
                //                 LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
               lv_obj_align(ui_guide_info->guide_icon_id03, LV_ALIGN_TOP_MID, 0, 300);
            }
        }

        /* 创建箭头下方的文本框 */
        if (ui_guide_info->guide_icon_id03 != NULL && lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
            if (ui_guide_info->guide_text_label02 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label02)) {
                ui_guide_info->guide_text_label02 = lv_label_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label02);
                lv_obj_set_size(ui_guide_info->guide_text_label02, 264, 16);
                lv_label_set_text(ui_guide_info->guide_text_label02, "Press down to exit");
                lv_label_set_long_mode(ui_guide_info->guide_text_label02, LV_LABEL_LONG_WRAP);
                lv_obj_set_style_text_align(ui_guide_info->guide_text_label02, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_style_text_color(ui_guide_info->guide_text_label02, lv_color_hex(ID_TEXT_COLOR), 0);
                lv_obj_set_style_text_font(ui_guide_info->guide_text_label02, &lv_font_oppo_sans_14_medium, 0);
                lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label02, 7, 0);  /* line height 24px (font ~13 + 11) */
                lv_obj_set_style_text_opa(ui_guide_info->guide_text_label02, 191, 0);  /* 75% 透明度 (255*75/100) */
                lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_set_style_border_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_clear_flag(ui_guide_info->guide_text_label02, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_align_to(ui_guide_info->guide_text_label02, ui_guide_info->guide_icon_id03,
                                LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
            }
        }
    
}

static void ui_guide_pid3_update(ui_manager_context_t *thiz)
{
    if (ui_guide_info == NULL) {
        return;
    }
    /* index 6 = RESOURCE_ID_GUIDE_KN_ABB，用于顶部旋钮示意图 */
    const ui_guide_icon_info_t *icon_id01 = &ui_guide_icon_infos[UI_GUIDE_PUMPING_THUB_INDEX];
    if (icon_id01->img_id == 0) {
        return;  /* 避免用 img_id=0 加载，资源里无 id=0 */
    }
    if (ui_guide_info->guide_icon_id01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id01)) {
        ui_guide_info->guide_icon_id01 = lv_image_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01);
        common_ui_img_resource_init(thiz, icon_id01->img_id, &ui_guide_info->guide_icon_id01_info);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01_info.data);
        lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id01_path,
                                    LV_FS_MEMFS_LETTER,
                                    ui_guide_info->guide_icon_id01_info.data,
                                    ui_guide_info->guide_icon_id01_info.size,
                                    "png");
        lv_image_set_src(ui_guide_info->guide_icon_id01, (const void *)&ui_guide_info->guide_icon_id01_path);
        /* 左右居中，距离背景图上部32 */
        lv_obj_align(ui_guide_info->guide_icon_id01, LV_ALIGN_TOP_MID, 0, 0);
    }

    /* 创建 More pumping options 文本框：277x40，左右居中，距上204，OPPO Sans 14 Medium/Bold，#B25962 75% */
    if (ui_guide_info->guide_text_label01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label01)) {
        ui_guide_info->guide_text_label01 = lv_label_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label01);
        lv_obj_set_size(ui_guide_info->guide_text_label01, 280, 40);
        lv_label_set_text(ui_guide_info->guide_text_label01,
            "Adjust suction and speed per side by\npressing buttons or knobs, or switch modes");
        lv_label_set_long_mode(ui_guide_info->guide_text_label01, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(ui_guide_info->guide_text_label01, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(ui_guide_info->guide_text_label01, lv_color_hex(ID_TEXT_COLOR), 0);
        lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label01, 9, 0);  /* line height 24px (font ~13 + 11) */
        lv_obj_set_style_text_opa(ui_guide_info->guide_text_label01, 191, 0);  /* 75% 透明度 (255*75/100) */
        lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
        lv_obj_set_style_text_font(ui_guide_info->guide_text_label01, &lv_font_oppo_sans_14_medium, 0);
        lv_obj_set_style_border_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
        lv_obj_clear_flag(ui_guide_info->guide_text_label01, LV_OBJ_FLAG_SCROLLABLE);
        //lv_obj_align_to(ui_guide_info->guide_text_label01, ui_guide_info->guide_icon_id01, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
        lv_obj_align(ui_guide_info->guide_text_label01, LV_ALIGN_TOP_MID, 0, GUIDE_TEXT01_TOP_OFFSET);
    }

    const ui_guide_icon_info_t *icon_id02 = &ui_guide_icon_infos[2]; /* RESOURCE_ID_GUIDE_MODESTAR_ICON */
    if (icon_id02->img_id != 0) {
        if (ui_guide_info->guide_icon_id02 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id02)) {
            ui_guide_info->guide_icon_id02 = lv_image_create(ui_guide_info->root);
            EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id02);
            common_ui_img_resource_init(thiz, icon_id02->img_id, &ui_guide_info->guide_icon_id02_info);
            EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id02_info.data);
            lv_fs_path_ex_t thumbnail_img_path;
            lv_fs_make_path_from_buffer(&thumbnail_img_path, LV_FS_MEMFS_LETTER,
                ui_guide_info->guide_icon_id02_info.data, ui_guide_info->guide_icon_id02_info.size, "png");
            lv_image_set_src(ui_guide_info->guide_icon_id02, (const void *)&thumbnail_img_path);
            /* 距离背景图中间顶端252，左右间距140（相对 root 水平居中） */
            lv_obj_align_to(ui_guide_info->guide_icon_id02, ui_guide_info->guide_text_label01, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
            //lv_obj_align(ui_guide_info->guide_icon_id02, LV_ALIGN_TOP_MID, 0, 252);
        }
    }

        /* 创建文本框下方的箭头图标 */
        const ui_guide_icon_info_t *icon_id03 = &ui_guide_icon_infos[UI_GUIDE_ARROW_INDEX]; /* RESOURCE_ID_GUIDE_ARROW_ICON */
        if (icon_id03->img_id != 0) {
            if (ui_guide_info->guide_icon_id03 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
                ui_guide_info->guide_icon_id03 = lv_image_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03);
                common_ui_img_resource_init(thiz, icon_id03->img_id, &ui_guide_info->guide_icon_id03_info);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03_info.data);
                lv_fs_path_ex_t arrow_img_path;
                lv_fs_make_path_from_buffer(&arrow_img_path, LV_FS_MEMFS_LETTER,
                    ui_guide_info->guide_icon_id03_info.data, ui_guide_info->guide_icon_id03_info.size, "png");
                lv_image_set_src(ui_guide_info->guide_icon_id03, (const void *)&arrow_img_path);
                lv_obj_align_to(ui_guide_info->guide_icon_id03, ui_guide_info->guide_icon_id02, 
                                LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
               //lv_obj_align(ui_guide_info->arrow_icon, LV_ALIGN_TOP_MID, 0, 300);
            }
        }

        /* 创建箭头下方的文本框 */
        if (ui_guide_info->guide_icon_id03 != NULL && lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
            if (ui_guide_info->guide_text_label02 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label02)) {
                ui_guide_info->guide_text_label02 = lv_label_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label02);
                lv_obj_set_size(ui_guide_info->guide_text_label02, 264, 16);
                lv_label_set_text(ui_guide_info->guide_text_label02, "Press down to exit");
                lv_label_set_long_mode(ui_guide_info->guide_text_label02, LV_LABEL_LONG_WRAP);
                lv_obj_set_style_text_align(ui_guide_info->guide_text_label02, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_style_text_color(ui_guide_info->guide_text_label02, lv_color_hex(ID_TEXT_COLOR), 0);
                lv_obj_set_style_text_font(ui_guide_info->guide_text_label02, &lv_font_oppo_sans_14_medium, 0);
                lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label02, 7, 0);  /* line height 24px (font ~13 + 11) */
                lv_obj_set_style_text_opa(ui_guide_info->guide_text_label02, 191, 0);  /* 75% 透明度 (255*75/100) */
                lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_set_style_border_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_clear_flag(ui_guide_info->guide_text_label02, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_align_to(ui_guide_info->guide_text_label02, ui_guide_info->guide_icon_id03,
                                LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
            }
        }
    
}

static void ui_guide_pid4_update(ui_manager_context_t *thiz)
{
    if (ui_guide_info == NULL) {
        return;
    }
    /* index 6 = RESOURCE_ID_GUIDE_KN_ABB，用于顶部旋钮示意图 */
    const ui_guide_icon_info_t *icon_id01 = &ui_guide_icon_infos[UI_GUIDE_PUMPING_THUB_INDEX];
    if (icon_id01->img_id != 0 && (ui_guide_info->guide_icon_id01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id01))) {
        ui_guide_info->guide_icon_id01 = lv_image_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01);
        common_ui_img_resource_init(thiz, icon_id01->img_id, &ui_guide_info->guide_icon_id01_info);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01_info.data);
        lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id01_path,
                                    LV_FS_MEMFS_LETTER,
                                    ui_guide_info->guide_icon_id01_info.data,
                                    ui_guide_info->guide_icon_id01_info.size,
                                    "png");
        lv_image_set_src(ui_guide_info->guide_icon_id01, (const void *)&ui_guide_info->guide_icon_id01_path);
        /* 左右居中，距离背景图上部32 */
        lv_obj_align(ui_guide_info->guide_icon_id01, LV_ALIGN_TOP_MID, 0, 0);
    }

    /* 创建 More pumping options 文本框：277x40，左右居中，距上204，OPPO Sans 14 Medium/Bold，#B25962 75% */
    if (ui_guide_info->guide_text_label01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label01)) {
        ui_guide_info->guide_text_label01 = lv_label_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label01);
        lv_obj_set_size(ui_guide_info->guide_text_label01, GUIDE_TEXT01_LABEL_W, GUIDE_TEXT01_LABEL_H);
        lv_label_set_text(ui_guide_info->guide_text_label01, "Press and hold for 3 seconds to stop pumping");
        lv_label_set_long_mode(ui_guide_info->guide_text_label01, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(ui_guide_info->guide_text_label01, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(ui_guide_info->guide_text_label01, lv_color_hex(ID_TEXT_COLOR), 0);
        lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label01, 9, 0);  /* line height 24px (font ~13 + 11) */
        lv_obj_set_style_text_opa(ui_guide_info->guide_text_label01, 191, 0);  /* 75% 透明度 (255*75/100) */
        lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
        lv_obj_set_style_text_font(ui_guide_info->guide_text_label01, &lv_font_oppo_sans_14_medium, 0);
        lv_obj_set_style_border_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
        lv_obj_clear_flag(ui_guide_info->guide_text_label01, LV_OBJ_FLAG_SCROLLABLE);
        //lv_obj_align_to(ui_guide_info->guide_text_label01, ui_guide_info->guide_icon_id01, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
        lv_obj_align(ui_guide_info->guide_text_label01, LV_ALIGN_TOP_MID, 0, GUIDE_TEXT01_TOP_OFFSET);
    }

    const ui_guide_icon_info_t *icon_id02 = &ui_guide_icon_infos[3]; /* RESOURCE_ID_GUIDE_MODESTAR_ICON */
    if (icon_id02->img_id != 0) {
        if (ui_guide_info->guide_icon_id02 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id02)) {
            ui_guide_info->guide_icon_id02 = lv_image_create(ui_guide_info->root);
            EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id02);
            common_ui_img_resource_init(thiz, icon_id02->img_id, &ui_guide_info->guide_icon_id02_info);
            EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id02_info.data);
        lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id02_path,
                                    LV_FS_MEMFS_LETTER,
                                    ui_guide_info->guide_icon_id02_info.data,
                                    ui_guide_info->guide_icon_id02_info.size,
                                    "png");
            lv_image_set_src(ui_guide_info->guide_icon_id02, (const void *)&ui_guide_info->guide_icon_id02_path);
            /* 距离背景图中间顶端252，左右间距140（相对 root 水平居中） */
            //lv_obj_align_to(ui_guide_info->guide_icon_id02, ui_guide_info->guide_text_label01, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
            lv_obj_align(ui_guide_info->guide_icon_id02, LV_ALIGN_TOP_MID, 0, 252);
        }
    }

        /* 创建文本框下方的箭头图标 */
        const ui_guide_icon_info_t *icon_id03 = &ui_guide_icon_infos[UI_GUIDE_ARROW_INDEX]; /* RESOURCE_ID_GUIDE_ARROW_ICON */
        if (icon_id03->img_id != 0) {
            if (ui_guide_info->guide_icon_id03 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
                ui_guide_info->guide_icon_id03 = lv_image_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03);
                common_ui_img_resource_init(thiz, icon_id03->img_id, &ui_guide_info->guide_icon_id03_info);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03_info.data);
                lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id03_path,
                                            LV_FS_MEMFS_LETTER,
                                            ui_guide_info->guide_icon_id03_info.data,
                                            ui_guide_info->guide_icon_id03_info.size,
                                            "png");
                lv_image_set_src(ui_guide_info->guide_icon_id03, (const void *)&ui_guide_info->guide_icon_id03_path);
                lv_obj_align_to(ui_guide_info->guide_icon_id03, ui_guide_info->guide_icon_id02, 
                                LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
               //lv_obj_align(ui_guide_info->arrow_icon, LV_ALIGN_TOP_MID, 0, 300);
            }
        }

        /* 创建箭头下方的文本框 */
        if (ui_guide_info->guide_icon_id03 != NULL && lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
            if (ui_guide_info->guide_text_label02 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label02)) {
                ui_guide_info->guide_text_label02 = lv_label_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label02);
                lv_obj_set_size(ui_guide_info->guide_text_label02, 264, 16);
                lv_label_set_text(ui_guide_info->guide_text_label02, "Press down to exit");
                lv_label_set_long_mode(ui_guide_info->guide_text_label02, LV_LABEL_LONG_WRAP);
                lv_obj_set_style_text_align(ui_guide_info->guide_text_label02, LV_TEXT_ALIGN_CENTER, 0);     
                lv_obj_set_style_text_color(ui_guide_info->guide_text_label02, lv_color_hex(ID_TEXT_COLOR), 0);
                lv_obj_set_style_text_font(ui_guide_info->guide_text_label02, &lv_font_oppo_sans_14_medium, 0);
                lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label02, 7, 0);  /* line height 24px (font ~13 + 11) */
                lv_obj_set_style_text_opa(ui_guide_info->guide_text_label02, 191, 0);  /* 75% 透明度 (255*75/100) */
                lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_set_style_border_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_clear_flag(ui_guide_info->guide_text_label02, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_align_to(ui_guide_info->guide_text_label02, ui_guide_info->guide_icon_id03,
                                LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
            }
        }
    
}

static void ui_guide_pid5_update(ui_manager_context_t *thiz)
{
    if (ui_guide_info == NULL) {
        return;
    }
    /* index 8 = RESOURCE_ID_GUIDE_KN_ABB，用于顶部旋钮示意图 */
    const ui_guide_icon_info_t *icon_id01 = &ui_guide_icon_infos[UI_GUIDE_KNR_TURN_INDEX];
    if (icon_id01->img_id == 0) {
        return;  /* 避免用 img_id=0 加载，资源里无 id=0 */
    }
    if (ui_guide_info->guide_icon_id01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id01)) {
        ui_guide_info->guide_icon_id01 = lv_image_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01);
        common_ui_img_resource_init(thiz, icon_id01->img_id, &ui_guide_info->guide_icon_id01_info);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01_info.data);
        lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id01_path,
                                    LV_FS_MEMFS_LETTER,
                                    ui_guide_info->guide_icon_id01_info.data,
                                    ui_guide_info->guide_icon_id01_info.size,
                                    "png");
        lv_image_set_src(ui_guide_info->guide_icon_id01, (const void *)&ui_guide_info->guide_icon_id01_path);
        /* 距离左边74，右边90，距离背景图上部47 */
        //lv_obj_align(ui_guide_info->guide_icon_id01, LV_ALIGN_TOP_MID, 0, 47);
        lv_obj_align(ui_guide_info->guide_icon_id01, LV_ALIGN_TOP_LEFT, 74, 47);
    }

    /* 创建 More pumping options 文本框：277x40，左右居中，距上204，OPPO Sans 14 Medium/Bold，#B25962 75% */
    if (ui_guide_info->guide_text_label01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label01)) {
        ui_guide_info->guide_text_label01 = lv_label_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label01);
        lv_obj_set_size(ui_guide_info->guide_text_label01, GUIDE_TEXT01_LABEL_W, GUIDE_TEXT01_LABEL_H);
        lv_label_set_text(ui_guide_info->guide_text_label01, "More pumping options: 3 modes, 6 programs. Turn right to switch");
        lv_label_set_long_mode(ui_guide_info->guide_text_label01, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(ui_guide_info->guide_text_label01, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(ui_guide_info->guide_text_label01, lv_color_hex(ID_TEXT_COLOR), 0);
        lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label01, 9, 0);  /* line height 24px (font ~13 + 11) */
        lv_obj_set_style_text_opa(ui_guide_info->guide_text_label01, 191, 0);  /* 75% 透明度 (255*75/100) */
        lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
        lv_obj_set_style_text_font(ui_guide_info->guide_text_label01, &lv_font_oppo_sans_14_medium, 0);
        lv_obj_set_style_border_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
        lv_obj_clear_flag(ui_guide_info->guide_text_label01, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_align(ui_guide_info->guide_text_label01, LV_ALIGN_TOP_MID, 0, GUIDE_TEXT01_TOP_OFFSET);
    }

    const ui_guide_icon_info_t *icon_id02 = &ui_guide_icon_infos[4]; /* RESOURCE_ID_GUIDE_MODESTAR_ICON */
    if (icon_id02->img_id != 0) {
        if (ui_guide_info->guide_icon_id02 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id02)) {
            ui_guide_info->guide_icon_id02 = lv_image_create(ui_guide_info->root);
            EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id02);
            common_ui_img_resource_init(thiz, icon_id02->img_id, &ui_guide_info->guide_icon_id02_info);
            EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id02_info.data);
            lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id02_path,
                                        LV_FS_MEMFS_LETTER,
                                        ui_guide_info->guide_icon_id02_info.data,
                                        ui_guide_info->guide_icon_id02_info.size,
                                        "png");
            lv_image_set_src(ui_guide_info->guide_icon_id02, (const void *)&ui_guide_info->guide_icon_id02_path);
            /* 距离背景图中间顶端252，左右间距140（相对 root 水平居中） */
            //lv_obj_align_to(ui_guide_info->thumbnail_icon, ui_guide_info->knob_right_turn_icon, 
                           // LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
           lv_obj_align(ui_guide_info->guide_icon_id02, LV_ALIGN_TOP_MID, 0, 252);
        }
    }

        /* 创建文本框下方的箭头图标 */
        const ui_guide_icon_info_t *icon_id03 = &ui_guide_icon_infos[UI_GUIDE_ARROW_INDEX]; /* RESOURCE_ID_GUIDE_ARROW_ICON */
        if (icon_id03->img_id != 0) {
            if (ui_guide_info->guide_icon_id03 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
                ui_guide_info->guide_icon_id03 = lv_image_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03);
                common_ui_img_resource_init(thiz, icon_id03->img_id, &ui_guide_info->guide_icon_id03_info);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03_info.data);
                lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id03_path,
                                            LV_FS_MEMFS_LETTER,
                                            ui_guide_info->guide_icon_id03_info.data,
                                            ui_guide_info->guide_icon_id03_info.size,
                                            "png");
                lv_image_set_src(ui_guide_info->guide_icon_id03, (const void *)&ui_guide_info->guide_icon_id03_path);
                lv_obj_align_to(ui_guide_info->guide_icon_id03, ui_guide_info->guide_icon_id02, 
                                LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
               //lv_obj_align(ui_guide_info->arrow_icon, LV_ALIGN_TOP_MID, 0, 300);
            }
        }

        /* 创建箭头下方的文本框 */
        if (ui_guide_info->guide_icon_id03 != NULL && lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
            if (ui_guide_info->guide_text_label02 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label02)) {
                ui_guide_info->guide_text_label02 = lv_label_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label02);
                lv_obj_set_size(ui_guide_info->guide_text_label02, 264, 16);
                lv_label_set_text(ui_guide_info->guide_text_label02, "Press down to exit");
                lv_label_set_long_mode(ui_guide_info->guide_text_label02, LV_LABEL_LONG_WRAP);
                lv_obj_set_style_text_align(ui_guide_info->guide_text_label02, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_style_text_color(ui_guide_info->guide_text_label02, lv_color_hex(ID_TEXT_COLOR), 0);
                lv_obj_set_style_text_font(ui_guide_info->guide_text_label02, &lv_font_oppo_sans_14_medium, 0);
                lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label02, 7, 0);  /* line height 24px (font ~13 + 11) */
                lv_obj_set_style_text_opa(ui_guide_info->guide_text_label02, 191, 0);  /* 75% 透明度 (255*75/100) */
                lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_set_style_border_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_clear_flag(ui_guide_info->guide_text_label02, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_align_to(ui_guide_info->guide_text_label02, ui_guide_info->guide_icon_id03,
                                LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
            }
        }
    
}

static void ui_guide_pid6_update(ui_manager_context_t *thiz)
{
    if (ui_guide_info == NULL) {
        return;
    }
    const ui_guide_icon_info_t *icon_id01 = &ui_guide_icon_infos[UI_GUIDE_QR_CODE_INDEX];
    if (icon_id01->img_id == 0) {
        return;  /* 避免用 img_id=0 加载，资源里无 id=0 */
    }
    bool qr_code_created = false;
    if (ui_guide_info->guide_icon_id01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id01)) {
        ui_guide_info->guide_icon_id01 = lv_image_create(ui_guide_info->root);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01);
        common_ui_img_resource_init(thiz, icon_id01->img_id, &ui_guide_info->guide_icon_id01_info);
        EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id01_info.data);
        lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id01_path,
                                    LV_FS_MEMFS_LETTER,
                                    ui_guide_info->guide_icon_id01_info.data,
                                    ui_guide_info->guide_icon_id01_info.size,
                                    "png");
        lv_image_set_src(ui_guide_info->guide_icon_id01, (const void *)&ui_guide_info->guide_icon_id01_path);
        lv_obj_align(ui_guide_info->guide_icon_id01, LV_ALIGN_TOP_LEFT, icon_id01->icon_x, icon_id01->icon_y);
        qr_code_created = true;
    }

    /* 创建二维码下方的文本框 */
    if (ui_guide_info->guide_icon_id01 != NULL && lv_obj_is_valid(ui_guide_info->guide_icon_id01)) {
        if (ui_guide_info->guide_text_label01 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label01)) {
            ui_guide_info->guide_text_label01 = lv_label_create(ui_guide_info->root);
            EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label01);
            lv_obj_set_size(ui_guide_info->guide_text_label01, 280, 40);
            lv_label_set_text(ui_guide_info->guide_text_label01, "Scan to connect for tracking, remote\ncontrol, custom modes... ");
            lv_label_set_long_mode(ui_guide_info->guide_text_label01, LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(ui_guide_info->guide_text_label01, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_set_style_text_color(ui_guide_info->guide_text_label01, lv_color_hex(0x6A5A5B), 0);
            lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
            lv_obj_set_style_text_font(ui_guide_info->guide_text_label01, &lv_font_oppo_sans_14_medium, 0);
            lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label01, 9, 0);  /* line height 24px (font ~13 + 11) */
            lv_obj_set_style_border_opa(ui_guide_info->guide_text_label01, LV_OPA_TRANSP, 0);
            lv_obj_clear_flag(ui_guide_info->guide_text_label01, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_align_to(ui_guide_info->guide_text_label01, ui_guide_info->guide_icon_id01, 
                           LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
     }

        /* 创建文本框下方的箭头图标 */
        const ui_guide_icon_info_t *icon_id03 = &ui_guide_icon_infos[5]; /* RESOURCE_ID_GUIDE_ARROW_ICON */
        if (icon_id03->img_id != 0) {
            if (ui_guide_info->guide_icon_id03 == NULL || !lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
                ui_guide_info->guide_icon_id03 = lv_image_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03);
                common_ui_img_resource_init(thiz, icon_id03->img_id, &ui_guide_info->guide_icon_id03_info);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_icon_id03_info.data);
                lv_fs_make_path_from_buffer(&ui_guide_info->guide_icon_id03_path,
                                            LV_FS_MEMFS_LETTER,
                                            ui_guide_info->guide_icon_id03_info.data,
                                            ui_guide_info->guide_icon_id03_info.size,
                                            "png");
                lv_image_set_src(ui_guide_info->guide_icon_id03, (const void *)&ui_guide_info->guide_icon_id03_path);
                lv_obj_align_to(ui_guide_info->guide_icon_id03, ui_guide_info->guide_text_label01, 
                               LV_ALIGN_OUT_BOTTOM_MID, 0, 36);
            }
        }

        /* 创建箭头下方的文本框 */
        if (ui_guide_info->guide_icon_id03 != NULL && lv_obj_is_valid(ui_guide_info->guide_icon_id03)) {
            if (ui_guide_info->guide_text_label02 == NULL || !lv_obj_is_valid(ui_guide_info->guide_text_label02)) {
                ui_guide_info->guide_text_label02 = lv_label_create(ui_guide_info->root);
                EXCEPTION_CHECK_VOID(ui_guide_info->guide_text_label02);
                lv_obj_set_size(ui_guide_info->guide_text_label02, 264, 16);
                lv_label_set_text(ui_guide_info->guide_text_label02, "End Trip");
                lv_label_set_long_mode(ui_guide_info->guide_text_label02, LV_LABEL_LONG_WRAP);
                lv_obj_set_style_text_align(ui_guide_info->guide_text_label02, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_style_text_color(ui_guide_info->guide_text_label02, lv_color_hex(ID_TEXT_COLOR), 0);
                lv_obj_set_style_text_font(ui_guide_info->guide_text_label02, &lv_font_oppo_sans_14_medium, 0);
                lv_obj_set_style_text_line_space(ui_guide_info->guide_text_label02, 7, 0);  /* line height 24px (font ~13 + 11) */
                //lv_obj_set_style_text_opa(ui_guide_info->guide_text_label02, 191, 0);  /* 75% 透明度 (255*75/100) */
                lv_obj_set_style_bg_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_set_style_border_opa(ui_guide_info->guide_text_label02, LV_OPA_TRANSP, 0);
                lv_obj_clear_flag(ui_guide_info->guide_text_label02, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_align_to(ui_guide_info->guide_text_label02, ui_guide_info->guide_icon_id03,
                               LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
            }
        }
    }
}

static void ui_guide_pid8_update(ui_manager_context_t *thiz)
{

    if (ui_guide_info->logo_arc == NULL || !lv_obj_is_valid(ui_guide_info->logo_arc)) {
        ui_guide_info->logo_arc = lv_arc_create(ui_guide_info->root);
        if (ui_guide_info->logo_arc == NULL) {
            return;
        }
        lv_obj_set_size(ui_guide_info->logo_arc, LOGO_ARC_SIZE, LOGO_ARC_SIZE);
        lv_obj_center(ui_guide_info->logo_arc);
        lv_arc_set_bg_angles(ui_guide_info->logo_arc, 0, 360);
        lv_obj_set_style_arc_color(ui_guide_info->logo_arc, lv_color_hex(LOGO_ARC_BG_COLOR), LV_PART_MAIN);
        lv_obj_set_style_arc_width(ui_guide_info->logo_arc, LOGO_ARC_WIDTH, LV_PART_MAIN);
        lv_obj_set_style_arc_opa(ui_guide_info->logo_arc, LOGO_ARC_BG_OPA, LV_PART_MAIN);
        lv_obj_set_style_arc_color(ui_guide_info->logo_arc, lv_color_hex(LOGO_ARC_COLOR), LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(ui_guide_info->logo_arc, LOGO_ARC_WIDTH, LV_PART_INDICATOR);
        lv_obj_set_style_arc_opa(ui_guide_info->logo_arc, LV_OPA_COVER, LV_PART_INDICATOR);
        lv_obj_align(ui_guide_info->logo_arc, LV_ALIGN_TOP_MID, 0, 152);
        lv_obj_remove_style(ui_guide_info->logo_arc, NULL, LV_PART_KNOB);
        if (ui_guide_info->start_time == 0) {
            ui_guide_info->start_time = get_timestamp();
        }

        ui_guide_info->logo_label = lv_label_create(ui_guide_info->root);
        if (ui_guide_info->logo_label != NULL) {
            lv_label_set_text(ui_guide_info->logo_label, "Reset settings...");
            lv_obj_set_style_text_font(ui_guide_info->logo_label, &lv_font_oppo_sans_14_medium, 0);
            lv_obj_set_style_text_line_space(ui_guide_info->logo_label, 9, 0);  /* line height 24px (font ~13 + 11) */
            lv_obj_align_to(ui_guide_info->logo_label, ui_guide_info->logo_arc, LV_ALIGN_OUT_BOTTOM_MID, 0, LOGO_LABEL_GAP);
        }
    }

    {
        uint32_t elapsed = get_timestamp() - ui_guide_info->start_time;
        uint16_t start_angle = (uint16_t)((elapsed / LOGO_ARC_LOOP_MS_PER_DEG) % 360);
        uint16_t end_angle = start_angle + LOGO_ARC_ANGLE_SPAN;
        lv_arc_set_angles(ui_guide_info->logo_arc, start_angle, end_angle);
        /* 循环刷新：使圆弧/图片区域重绘 */
        lv_obj_invalidate(ui_guide_info->logo_arc);
    }
}

static void guide_ui_update(ui_manager_context_t *thiz, guide_page_id_t page_index)
{
    switch(page_index){
        case GUIDE_PAGE_ID_1:
                ui_guide_root_init();
                ui_guide_bg_update(thiz);
                ui_guide_pid1_update(thiz);
            break;
        case GUIDE_PAGE_ID_2:
                ui_guide_root_init();
                ui_guide_bg_update(thiz);
                ui_guide_pid2_update(thiz);
            break;
        case GUIDE_PAGE_ID_3:
                ui_guide_root_init();
                ui_guide_bg_update(thiz);
                ui_guide_pid3_update(thiz);
            break;
        case GUIDE_PAGE_ID_4:
                ui_guide_root_init();
                ui_guide_bg_update(thiz);
                ui_guide_pid4_update(thiz);
            break;
        case GUIDE_PAGE_ID_5:
                ui_guide_root_init();
                ui_guide_bg_update(thiz);
                ui_guide_pid5_update(thiz);
            break;
        case GUIDE_PAGE_ID_6:
                ui_guide_root_init();
                ui_guide_bg_update(thiz);
                ui_guide_pid6_update(thiz);
            break;
        case GUIDE_PAGE_ID_8:
                ui_guide_root01_init();
                ui_guide_pid8_update(thiz);
            break;
        default:
            break;
    }
}

//*********************************************************************************************************************** */
void ui_display_guide_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (ui_guide_info == NULL) {
        ui_guide_info = (ui_guide_info_t *)u_malloz(sizeof(ui_guide_info_t));
        EXCEPTION_CHECK_VOID(ui_guide_info);
    }
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    ui_guide_info->force_update = true;
}

void ui_display_guide_remove(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (ui_guide_info == NULL) {
        return;
    }
    common_ui_img_resource_deinit(thiz, &ui_guide_info->background_info);
    common_ui_img_resource_deinit(thiz, &ui_guide_info->guide_icon_id02_info);
    common_ui_img_resource_deinit(thiz, &ui_guide_info->guide_icon_id03_info);
    LVGL_OBJ_DELETE(ui_guide_info->root);
    u_free(ui_guide_info);
    ui_guide_info = NULL;
}

int ui_display_guide_process(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    guide_ui_update(thiz,GUIDE_PAGE_ID_1);
    if (ui_guide_info != NULL) {
        ui_guide_info->force_update = false;
    }
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOG_UI("===> logo fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }

    return 0;
}
