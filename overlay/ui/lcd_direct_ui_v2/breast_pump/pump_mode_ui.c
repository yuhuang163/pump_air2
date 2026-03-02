/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep
#include <stdint.h>

#define UI_MODE_ICON_MAX_COUNT                  (15)   

#define ID_AREA_ICON_X                      (0)
#define ID_AREA_ICON_Y                      (0)

#define ID_TEXT_COLOR                            (0xB25962)
#define GUIDE_TEXT01_LABEL_W                     (280)
#define GUIDE_TEXT01_LABEL_H                     (40)
#define GUIDE_TEXT01_TOP_OFFSET                  (204)
#define GUIDE_MORE_OPTIONS_COLOR                 (0xB25962)
#define GUIDE_MORE_OPTIONS_OPA                   (191)  /* 75% of 255 */

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

typedef struct {
    lv_obj_t * root;
    #if IS_SUPPORT_MODE_COMP_BG
    lv_obj_t * backgroud;
    #endif
    lv_obj_t * mode_icon_id[5];
    res_img_info_t mode_icon_id_info[5];

    lv_obj_t * mode_bg_dot[6];   

    lv_obj_t * mode_heart_circle;  /* 48px 圆形 0xF5A6AB 12% 透明，位于 HEART_ICON 下 */
    lv_obj_t * custom_mode_uread__icon;  
    lv_obj_t * custom_mode_readed__icon; 

    lv_obj_t * mode_text_label[3];

    uint8_t force_update;

    lv_fs_path_ex_t background_path;
    lv_fs_path_ex_t mode_icon_id_path[5];

} ui_mode_info_t;

typedef struct {
    const char *name;
    uint16_t img_id;
    uint16_t icon_x;
    uint16_t icon_y;
} ui_mode_icon_info_t;

typedef enum{
    mode_type =0,
    program_type,
    speed_type
} type_id_enum_t;

typedef enum{
    stimulation_mode =1,
    expression_mode,
    mix_mode,
    custom_mode_urd,
    custom_mode_rded,
    custom_mode,
    regular_pumping_mode, //5
    milk_boost_mode,
    power_pumping_mode,
    lactation_kinkstart_mode,
    emptying_boost_mode,
    fast_mode,
    slow_mode,
    regular_mode
} mode_id_enum_t;
/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static ui_mode_info_t *ui_mode_info;
#if 0
const static ui_mode_icon_info_t ui_mode_icon_infos[UI_MODE_ICON_MAX_COUNT] = {
    { "idle_text_label", 0, 
        ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_pumping_start_abb_icon", RESOURCE_ID_HEART_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_kn_abb_0_icon", RESOURCE_ID_EXPRESSION_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_arrow_icon", RESOURCE_ID_MODE_MIX_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_arrow_icon", RESOURCE_ID_STAR_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_pumping_abb_icon", RESOURCE_ID_HUE_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_pumping_start_abb_icon", RESOURCE_ID_FREQ_BOOST_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_kn_abb_1_icon", RESOURCE_ID_FREQ_COMFORT_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_kn_abb_0_icon", RESOURCE_ID_FREQ_REGULAR_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_qr_icon", RESOURCE_ID_EMODE_R_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_lrm_icon", RESOURCE_ID_EMODE_L_ICON,     
      ID_AREA_ICON_X,              ID_AREA_ICON_Y, 
    },
    { "idle_playpause_icon", RESOURCE_ID_CMODE_R_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    },
    { "idle_pragram_icon", RESOURCE_ID_CMODE_L_ICON,     
      ID_AREA_ICON_X,              ID_AREA_ICON_Y, 
    },
    { "idle_pragram_icon", RESOURCE_ID_MODE_VECTOR_ICON,     
      ID_AREA_ICON_X,              ID_AREA_ICON_Y, 
    },
    { "idle_kn_abb_1_icon", RESOURCE_ID_MODE_BG_ICON, 
      ID_AREA_ICON_X,       ID_AREA_ICON_Y, 
    }
};

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void ui_mode_root_init(void)
{
    if (ui_mode_info->root == NULL || !lv_obj_is_valid(ui_mode_info->root)) {
        ui_mode_info->root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(ui_mode_info->root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(ui_mode_info->root, lv_color_white(), 0);     /// 设置背景颜色为黑色
    lv_obj_set_style_bg_opa(ui_mode_info->root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(ui_mode_info->root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(ui_mode_info->root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(ui_mode_info->root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(ui_mode_info->root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(ui_mode_info->root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
}

static void ui_mode_bg_update(ui_manager_context_t *thiz)
{
    common_ui_bg_update(thiz, ui_mode_info->root);
}

#define MODE_DOT_Y_OFFS                (328)
#define MODE_DOT_SIZE                 (8)
#define MODE_DOT_OPA_DIM              (76)   /* 30% 透明 */
#define MODE_DOT_OPA_HIGH             (255)  /* 高亮 */

static const int32_t mode_dot_x_6[] = { 136, 152, 168, 184, 200, 216 };
/* 5 个点时各点 x 坐标（TOP_LEFT） */
static const int32_t mode_dot_x_5[] = { 144, 160, 176, 192, 208 };
/* 3 个点时：0/2 为 TOP_LEFT 的 x，1 为居中用 TOP_MID */
static const int32_t mode_dot_x_3[] = { 158, 176, 194 };

static void ui_mode_dots_create(uint8_t dot_count, uint8_t dot_opa_id)
{
    if (ui_mode_info == NULL || dot_count == 0 || dot_count > 6) {
        return;
    }

    for (uint8_t i = 0; i < dot_count; i++) {
        if (ui_mode_info->mode_bg_dot[i] == NULL || !lv_obj_is_valid(ui_mode_info->mode_bg_dot[i])) {
            ui_mode_info->mode_bg_dot[i] = lv_obj_create(ui_mode_info->root);
            EXCEPTION_CHECK_VOID(ui_mode_info->mode_bg_dot[i]);
            lv_obj_set_size(ui_mode_info->mode_bg_dot[i], MODE_DOT_SIZE, MODE_DOT_SIZE);
            lv_obj_set_style_radius(ui_mode_info->mode_bg_dot[i], 4, 0);  /* 直径 8 的圆形 */
            lv_obj_set_style_bg_color(ui_mode_info->mode_bg_dot[i], lv_color_hex(0xB25962), 0);
            lv_obj_set_style_bg_opa(ui_mode_info->mode_bg_dot[i], MODE_DOT_OPA_DIM, 0);
            lv_obj_set_style_border_width(ui_mode_info->mode_bg_dot[i], 0, 0);
            lv_obj_set_style_border_opa(ui_mode_info->mode_bg_dot[i], LV_OPA_TRANSP, 0);
            lv_obj_clear_flag(ui_mode_info->mode_bg_dot[i], LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_clear_flag(ui_mode_info->mode_bg_dot[i], LV_OBJ_FLAG_CLICKABLE);
            if (dot_count == 6) {
                    lv_obj_align(ui_mode_info->mode_bg_dot[i], LV_ALIGN_TOP_LEFT,
                             mode_dot_x_6[i], MODE_DOT_Y_OFFS);
            } else if (dot_count == 5) {
                    lv_obj_align(ui_mode_info->mode_bg_dot[i], LV_ALIGN_TOP_LEFT,
                             mode_dot_x_5[i], MODE_DOT_Y_OFFS);
            } else {
                    lv_obj_align(ui_mode_info->mode_bg_dot[i], LV_ALIGN_TOP_LEFT,
                             mode_dot_x_3[i], MODE_DOT_Y_OFFS);
                }
            }
        }

    /* 高亮当前页：dot_opa_id 1~5 对应 index 0~4，6 与 5 同（最后一格）；不超过 dot_count */
    // uint8_t high_idx = (dot_opa_id == i);
    for (uint8_t i = 0; i < dot_count; i++) {
        if (ui_mode_info->mode_bg_dot[i] != NULL && lv_obj_is_valid(ui_mode_info->mode_bg_dot[i])) {
            lv_obj_set_style_bg_opa(ui_mode_info->mode_bg_dot[i],
                                    (i == (dot_opa_id-1)) ? MODE_DOT_OPA_HIGH : MODE_DOT_OPA_DIM, 0);
        }
    }
}
static void ui_mode_icon_update(ui_manager_context_t *thiz, type_id_enum_t type_id,mode_id_enum_t mode_id)
{
    if (ui_mode_info == NULL) {
        return;
    }

    static uint8_t mode_icon_id = 0;
    static uint8_t dot_icon_index = 0;
    static uint8_t dot_icon_opa_index = 0;
    const ui_mode_icon_info_t *icon_id01 = &ui_mode_icon_infos[UI_MODE_ICON_MAX_COUNT-1]; /* RESOURCE_ID_MODE_BG_ICON */
    if (icon_id01->img_id == 0) {
        return;  /* 避免用 img_id=0 加载，资源里无 id=0 */
    }
    if (ui_mode_info->mode_icon_id[0] == NULL || !lv_obj_is_valid(ui_mode_info->mode_icon_id[0])) {
        ui_mode_info->mode_icon_id[0] = lv_image_create(ui_mode_info->root);
        EXCEPTION_CHECK_VOID(ui_mode_info->mode_icon_id[0]);
        common_ui_img_resource_init(thiz, icon_id01->img_id, &ui_mode_info->mode_icon_id_info[0]);
        EXCEPTION_CHECK_VOID(ui_mode_info->mode_icon_id_info[0].data);
        lv_fs_make_path_from_buffer(&ui_mode_info->mode_icon_id_path[0],
                                    LV_FS_MEMFS_LETTER,
                                    ui_mode_info->mode_icon_id_info[0].data,
                                    ui_mode_info->mode_icon_id_info[0].size,
                                    "baj");
        lv_image_set_src(ui_mode_info->mode_icon_id[0], (const void *)&ui_mode_info->mode_icon_id_path[0]);
        lv_obj_align(ui_mode_info->mode_icon_id[0], LV_ALIGN_TOP_MID, 0, 86);
    }


    if (ui_mode_info->mode_text_label[0] == NULL || !lv_obj_is_valid(ui_mode_info->mode_text_label[0])) {
        ui_mode_info->mode_text_label[0] = lv_label_create(ui_mode_info->root);
        EXCEPTION_CHECK_VOID(ui_mode_info->mode_text_label[0]);
        lv_obj_set_size(ui_mode_info->mode_text_label[0], 200, 24);
        switch(type_id) { /* 根据 mode_id 和 program_id 显示不同文本，示例仅展示一种情况 */
            case mode_type:
                lv_label_set_text(ui_mode_info->mode_text_label[0], "Mode");
                break;
            case program_type:
                lv_label_set_text(ui_mode_info->mode_text_label[0], "Program");
                break;
            case speed_type:
                lv_label_set_text(ui_mode_info->mode_text_label[0], "Speed");
                break;
            default:
            break;
        }
        lv_label_set_long_mode(ui_mode_info->mode_text_label[0], LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(ui_mode_info->mode_text_label[0], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(ui_mode_info->mode_text_label[0], lv_color_hex(ID_TEXT_COLOR), 0);
        lv_obj_set_style_text_line_space(ui_mode_info->mode_text_label[0], 11, 0);  /* line height 24px (font ~13 + 11) */
        lv_obj_set_style_bg_opa(ui_mode_info->mode_text_label[0], LV_OPA_TRANSP, 0);
        lv_obj_set_style_text_font(ui_mode_info->mode_text_label[0], &lv_font_oppo_sans_20_medium, 0);
        lv_obj_set_style_border_opa(ui_mode_info->mode_text_label[0], LV_OPA_TRANSP, 0);
        lv_obj_clear_flag(ui_mode_info->mode_text_label[0], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_align(ui_mode_info->mode_text_label[0], LV_ALIGN_TOP_MID, 0, 34);
    }

    const ui_mode_icon_info_t *icon_id02 = &ui_mode_icon_infos[9]; /* RESOURCE_ID_EMODE_R_ICON */
    if (icon_id02->img_id != 0) {
        if (ui_mode_info->mode_icon_id[1] == NULL || !lv_obj_is_valid(ui_mode_info->mode_icon_id[1])) {
            ui_mode_info->mode_icon_id[1] = lv_image_create(ui_mode_info->root);
            EXCEPTION_CHECK_VOID(ui_mode_info->mode_icon_id[1]);
            common_ui_img_resource_init(thiz, icon_id02->img_id, &ui_mode_info->mode_icon_id_info[1]);
            EXCEPTION_CHECK_VOID(ui_mode_info->mode_icon_id_info[1].data);
            lv_fs_make_path_from_buffer(&ui_mode_info->mode_icon_id_path[1],
                                        LV_FS_MEMFS_LETTER,
                                        ui_mode_info->mode_icon_id_info[1].data,
                                        ui_mode_info->mode_icon_id_info[1].size,
                                        "baj");
            lv_image_set_src(ui_mode_info->mode_icon_id[1], (const void *)&ui_mode_info->mode_icon_id_path[1]);
            /* 距离背景图中间顶端252，左右间距140（相对 root 水平居中） */
            //lv_obj_align_to(ui_mode_info->thumbnail_icon, ui_mode_info->knob_right_turn_icon, 
                           // LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
           lv_obj_align(ui_mode_info->mode_icon_id[1], LV_ALIGN_TOP_LEFT, 310, 62);
        }
    }

        const ui_mode_icon_info_t *icon_id03 = &ui_mode_icon_infos[10]; /* RESOURCE_ID_EMODE_L_ICON */
        if (icon_id03->img_id != 0) {
            if (ui_mode_info->mode_icon_id[2] == NULL || !lv_obj_is_valid(ui_mode_info->mode_icon_id[2])) {
                ui_mode_info->mode_icon_id[2] = lv_image_create(ui_mode_info->root);
                EXCEPTION_CHECK_VOID(ui_mode_info->mode_icon_id[2]);
                common_ui_img_resource_init(thiz, icon_id03->img_id, &ui_mode_info->mode_icon_id_info[2]);
                EXCEPTION_CHECK_VOID(ui_mode_info->mode_icon_id_info[2].data);
                lv_fs_make_path_from_buffer(&ui_mode_info->mode_icon_id_path[2],
                                            LV_FS_MEMFS_LETTER,
                                            ui_mode_info->mode_icon_id_info[2].data,
                                            ui_mode_info->mode_icon_id_info[2].size,
                                            "baj");
                lv_image_set_src(ui_mode_info->mode_icon_id[2], (const void *)&ui_mode_info->mode_icon_id_path[2]);
                // lv_obj_align_to(ui_mode_info->mode_icon_id03, ui_mode_info->mode_icon_id02, 
                //                 LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
                lv_obj_align(ui_mode_info->mode_icon_id[2], LV_ALIGN_TOP_LEFT, 0, 62);
            }
        }
            switch(mode_id) { /* 根据 mode_id 和 program_id 显示不同文本，示例仅展示一种情况 */
                case stimulation_mode:
                    mode_icon_id =1;
                    dot_icon_index = 3;
                    dot_icon_opa_index = 1;
                    break;
                case expression_mode:
                    mode_icon_id =2;
                    dot_icon_index = 3;
                    dot_icon_opa_index = 2;
                    break;
                case mix_mode:
                    mode_icon_id =3;
                    dot_icon_index = 3;
                    dot_icon_opa_index = 3;
                    break;
                case custom_mode:
                case custom_mode_urd:
                case custom_mode_rded:
                    mode_icon_id =4;
                    dot_icon_index = 6;
                    dot_icon_opa_index = 1;
                    break;
                case regular_pumping_mode:
                    mode_icon_id =5;
                    dot_icon_index = 5;
                    dot_icon_opa_index = 1;
                    break;
                case milk_boost_mode:
                    mode_icon_id =5;
                    dot_icon_index = 5;
                    dot_icon_opa_index = 2;
                    break;
                case power_pumping_mode:
                    mode_icon_id =5;
                    dot_icon_index = 5;
                    dot_icon_opa_index = 3;
                    break;
                case lactation_kinkstart_mode:
                    mode_icon_id =5;
                    dot_icon_index = 5;
                    dot_icon_opa_index = 4;
                    break;
                case emptying_boost_mode:
                    mode_icon_id =5;
                    dot_icon_index = 5;
                    dot_icon_opa_index = 5;
                    break;
                case fast_mode:
                    mode_icon_id =6;
                    dot_icon_index = 3;
                    dot_icon_opa_index = 1;
                    break;
                case slow_mode:
                    mode_icon_id =7;
                    dot_icon_index = 3;
                    dot_icon_opa_index = 2;
                    break;
                case regular_mode:
                    mode_icon_id =8;
                    dot_icon_index = 3;
                    dot_icon_opa_index = 3;
                    break;
                default:
                    break;
            }
        ui_mode_dots_create(dot_icon_index,dot_icon_opa_index);
        const ui_mode_icon_info_t *icon_id04 = &ui_mode_icon_infos[mode_icon_id]; /* RESOURCE_ID_HEART_ICON */
        if (icon_id04->img_id != 0) {
            /* 心形图标下 48px 圆形背景：0xF5A6AB，12% 透明，位于 MODE_BG 上、HEART_ICON 下 */
            if (ui_mode_info->mode_heart_circle == NULL || !lv_obj_is_valid(ui_mode_info->mode_heart_circle)) {
                ui_mode_info->mode_heart_circle = lv_obj_create(ui_mode_info->root);
                EXCEPTION_CHECK_VOID(ui_mode_info->mode_heart_circle);
                lv_obj_set_size(ui_mode_info->mode_heart_circle, 48, 48);
                lv_obj_set_style_radius(ui_mode_info->mode_heart_circle, LV_RADIUS_CIRCLE, 0);
                lv_obj_set_style_bg_color(ui_mode_info->mode_heart_circle, lv_color_hex(0xF5A6AB), 0);
                lv_obj_set_style_bg_opa(ui_mode_info->mode_heart_circle, 224, 0);  /* 88% 不透明 = 12% 透明 */
                lv_obj_set_style_border_width(ui_mode_info->mode_heart_circle, 0, 0);
                lv_obj_align_to(ui_mode_info->mode_heart_circle, ui_mode_info->mode_icon_id[0],
                                LV_ALIGN_TOP_MID, 0, 28);
                lv_obj_clear_flag(ui_mode_info->mode_heart_circle, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_clear_flag(ui_mode_info->mode_heart_circle, LV_OBJ_FLAG_CLICKABLE);
            }
            if(mode_id == custom_mode_urd)
            {
                if (ui_mode_info->custom_mode_uread__icon == NULL || !lv_obj_is_valid(ui_mode_info->custom_mode_uread__icon)) {
                    ui_mode_info->custom_mode_uread__icon = lv_obj_create(ui_mode_info->root);
                    EXCEPTION_CHECK_VOID(ui_mode_info->custom_mode_uread__icon);
                    lv_obj_set_size(ui_mode_info->custom_mode_uread__icon, 8, 8);
                    lv_obj_set_style_radius(ui_mode_info->custom_mode_uread__icon, LV_RADIUS_CIRCLE, 0);
                    lv_obj_set_style_bg_color(ui_mode_info->custom_mode_uread__icon, lv_color_hex(0xEE2E2E), 0);
                    lv_obj_set_style_bg_opa(ui_mode_info->custom_mode_uread__icon, 255, 0);  /* 88% 不透明 = 12% 透明 */
                    lv_obj_set_style_border_width(ui_mode_info->custom_mode_uread__icon, 0, 0);
                    lv_obj_align_to(ui_mode_info->custom_mode_uread__icon, ui_mode_info->mode_icon_id[0],
                                    LV_ALIGN_TOP_LEFT, 122, 30);
                    lv_obj_clear_flag(ui_mode_info->custom_mode_uread__icon, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_clear_flag(ui_mode_info->custom_mode_uread__icon, LV_OBJ_FLAG_CLICKABLE);
                }
            }
            else if(mode_id == custom_mode_rded)
            {
                if (ui_mode_info->custom_mode_readed__icon == NULL || !lv_obj_is_valid(ui_mode_info->custom_mode_readed__icon)) {
                    ui_mode_info->custom_mode_readed__icon = lv_obj_create(ui_mode_info->root);
                    EXCEPTION_CHECK_VOID(ui_mode_info->custom_mode_readed__icon);
                    lv_obj_set_size(ui_mode_info->custom_mode_readed__icon, 20, 20);
                    lv_obj_set_style_radius(ui_mode_info->custom_mode_readed__icon, LV_RADIUS_CIRCLE, 0);
                    lv_obj_set_style_bg_color(ui_mode_info->custom_mode_readed__icon, lv_color_hex(0xFFAE00), 0);
                    lv_obj_set_style_bg_opa(ui_mode_info->custom_mode_readed__icon, 255, 0);  /* 88% 不透明 = 12% 透明 */
                    lv_obj_set_style_border_width(ui_mode_info->custom_mode_readed__icon, 0, 0);
                    lv_obj_align_to(ui_mode_info->custom_mode_readed__icon, ui_mode_info->mode_icon_id[0],
                                    LV_ALIGN_TOP_LEFT, 122, 28);
                    lv_obj_clear_flag(ui_mode_info->custom_mode_readed__icon, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_clear_flag(ui_mode_info->custom_mode_readed__icon, LV_OBJ_FLAG_CLICKABLE);
                }
                const ui_mode_icon_info_t *icon_id05 = &ui_mode_icon_infos[13]; /* RESOURCE_ID_MODE_VECTOR_ICON */
                if (ui_mode_info->mode_icon_id[4] == NULL || !lv_obj_is_valid(ui_mode_info->mode_icon_id[4])) {
                    ui_mode_info->mode_icon_id[4] = lv_image_create(ui_mode_info->root);
                    EXCEPTION_CHECK_VOID(ui_mode_info->mode_icon_id[4]);
                    common_ui_img_resource_init(thiz, icon_id05->img_id, &ui_mode_info->mode_icon_id_info[4]);

                    EXCEPTION_CHECK_VOID(ui_mode_info->mode_icon_id_info[4].data);
                    lv_fs_make_path_from_buffer(&ui_mode_info->mode_icon_id_path[4],
                                                LV_FS_MEMFS_LETTER,
                                                ui_mode_info->mode_icon_id_info[4].data,
                                                ui_mode_info->mode_icon_id_info[4].size,
                                                "baj");
                    lv_image_set_src(ui_mode_info->mode_icon_id[4], (const void *)&ui_mode_info->mode_icon_id_path[4]);
                    lv_obj_align_to(ui_mode_info->mode_icon_id[4], ui_mode_info->mode_icon_id[0],
                                    LV_ALIGN_TOP_LEFT, 126, 34);
                    // lv_obj_align_to(ui_mode_info->mode_icon_id05, ui_mode_info->mode_icon_id01, 
                    //                 LV_ALIGN_TOP_MID, 0, 34);
                }
            }
/*------------------------------------------------------------------------------------------------------------------------------*/
            if (ui_mode_info->mode_icon_id[3] == NULL || !lv_obj_is_valid(ui_mode_info->mode_icon_id[3])) {
                ui_mode_info->mode_icon_id[3] = lv_image_create(ui_mode_info->root);
                EXCEPTION_CHECK_VOID(ui_mode_info->mode_icon_id[3]);
                common_ui_img_resource_init(thiz, icon_id04->img_id, &ui_mode_info->mode_icon_id_info[3]);

                EXCEPTION_CHECK_VOID(ui_mode_info->mode_icon_id_info[3].data);
                lv_fs_make_path_from_buffer(&ui_mode_info->mode_icon_id_path[3],
                                            LV_FS_MEMFS_LETTER,
                                            ui_mode_info->mode_icon_id_info[3].data,
                                            ui_mode_info->mode_icon_id_info[3].size,
                                            "baj");
                lv_image_set_src(ui_mode_info->mode_icon_id[3], (const void *)&ui_mode_info->mode_icon_id_path[3]);
                lv_obj_align(ui_mode_info->mode_icon_id[3], LV_ALIGN_TOP_MID, 0, 120);
                // lv_obj_align_to(ui_mode_info->mode_icon_id04, ui_mode_info->mode_icon_id01, 
                //                 LV_ALIGN_TOP_MID, 0, 34);
            }
        }

            if (ui_mode_info->mode_text_label[1] == NULL || !lv_obj_is_valid(ui_mode_info->mode_text_label[1])) {
                ui_mode_info->mode_text_label[1] = lv_label_create(ui_mode_info->root);
                EXCEPTION_CHECK_VOID(ui_mode_info->mode_text_label[1]);
                lv_obj_set_size(ui_mode_info->mode_text_label[1], 180, 20);
                switch(mode_id) { /* 根据 mode_id 和 program_id 显示不同文本，示例仅展示一种情况 */
                    case stimulation_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Stimulation");
                        break;
                    case expression_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Expression");
                        break;
                    case mix_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Mix");
                        break;
                    case custom_mode:
                    case custom_mode_urd:
                    case custom_mode_rded:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Custom");
                        break;
                    case regular_pumping_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Regular pumping");
                        break;
                    case milk_boost_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Milk boost");
                        break;
                    case power_pumping_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Power pumping");
                        break;
                    case lactation_kinkstart_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Lactation kinkstart");
                        break;
                    case emptying_boost_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Emptying boost");
                        break;
                    case fast_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Fast");
                        break;
                    case slow_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Slow");
                        break;
                    case regular_mode:
                        lv_label_set_text(ui_mode_info->mode_text_label[1], "Regular");
                         break;
                    default:
                        break;
                }
                lv_label_set_long_mode(ui_mode_info->mode_text_label[1], LV_LABEL_LONG_WRAP);
                lv_obj_set_style_text_align(ui_mode_info->mode_text_label[1], LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_style_text_color(ui_mode_info->mode_text_label[1], lv_color_hex(0xFFFFFF), 0);
                lv_obj_set_style_text_font(ui_mode_info->mode_text_label[1], &lv_font_oppo_sans_20_medium, 0);
                lv_obj_set_style_text_line_space(ui_mode_info->mode_text_label[1], 9, 0);  /* line height 24px (font ~13 + 11) */
                //lv_obj_set_style_text_opa(ui_mode_info->mode_text_label[1], 191, 0);  /* 75% 透明度 (255*75/100) */
                lv_obj_set_style_bg_opa(ui_mode_info->mode_text_label[1], LV_OPA_TRANSP, 0);
                lv_obj_set_style_border_opa(ui_mode_info->mode_text_label[1], LV_OPA_TRANSP, 0);
                lv_obj_clear_flag(ui_mode_info->mode_text_label[1], LV_OBJ_FLAG_SCROLLABLE);
                //lv_obj_align_to(ui_mode_info->mode_text_label[1], ui_mode_info->mode_icon_id04,LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
                lv_obj_align(ui_mode_info->mode_text_label[1], LV_ALIGN_TOP_MID, 0, 170);
            }

            if (ui_mode_info->mode_text_label[2] == NULL || !lv_obj_is_valid(ui_mode_info->mode_text_label[2])) {
                ui_mode_info->mode_text_label[2] = lv_label_create(ui_mode_info->root);
                EXCEPTION_CHECK_VOID(ui_mode_info->mode_text_label[2]);
                lv_obj_set_size(ui_mode_info->mode_text_label[2], 172, 90);
                switch(mode_id) { /* 根据 mode_id 和 program_id 显示不同文本，示例仅展示一种情况 */
                    case stimulation_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 172, 36);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Fast, gentle suction to\ntrigger let down.");
                        break;
                    case expression_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 172, 36);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Slow, deep suction for\nefficient milk expression.");
                        break;
                    case mix_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 172, 90);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Alternates 5 times of fast,\ngentle suction with 1 slow,\ndeep suction for let\ndowns and better\nemptying.");
                        break;
                    case custom_mode:
                    case custom_mode_urd:
                    case custom_mode_rded:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 180, 54);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "For mothers who have a\nhigh demand for milk\nproduction.");
                        break;
                    case regular_pumping_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 172, 90);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Automatically transitions\nfrom fast, gentle suction\nto slow, deep suction to\nmimic a baby's natural\nnursing.");
                        break;
                    case milk_boost_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 172, 54);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Smooth, gradual suction\nto help improve low milk\nsupply.");
                        break;
                    case power_pumping_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 180, 54);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Gentle, frequent stimulation\nto trigger multiple let\ndowns.");
                        break;
                    case lactation_kinkstart_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 172, 54);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Initiates, builds, and\nmaintains a healthy milk\nsupply.");
                        break;
                    case emptying_boost_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 180, 32);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Effectively empties hindmilk\nto move on with ease.");
                        break;
                    case fast_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 180, 32);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Stronger yet safe for faster\npumping.");
                        break;
                    case slow_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 180, 16);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Efficient yet gentle.");
                        break;
                    case regular_mode:
                        lv_obj_set_size(ui_mode_info->mode_text_label[2], 180, 16);
                        lv_label_set_text(ui_mode_info->mode_text_label[2], "Suitable for most moms.");
                         break;
                    default:
                        break;
                }

                lv_label_set_long_mode(ui_mode_info->mode_text_label[2], LV_LABEL_LONG_WRAP);
                lv_obj_set_style_text_align(ui_mode_info->mode_text_label[2], LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_style_text_color(ui_mode_info->mode_text_label[2], lv_color_hex(0xFFFFFF), 0);
                lv_obj_set_style_text_font(ui_mode_info->mode_text_label[2], &lv_font_oppo_sans_14_medium, 0);
                lv_obj_set_style_text_line_space(ui_mode_info->mode_text_label[2], 8, 0);  /* line height 24px (font ~13 + 11) */
                lv_obj_set_style_text_opa(ui_mode_info->mode_text_label[2], 255, 0);  /* 75% 透明度 (255*75/100) */
                lv_obj_set_style_bg_opa(ui_mode_info->mode_text_label[2], LV_OPA_TRANSP, 0);
                lv_obj_set_style_border_opa(ui_mode_info->mode_text_label[2], LV_OPA_TRANSP, 0);
                lv_obj_clear_flag(ui_mode_info->mode_text_label[2], LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_align_to(ui_mode_info->mode_text_label[2], ui_mode_info->mode_text_label[1],LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
            }
}
#endif
//*********************************************************************************************************************** */
void ui_display_pump_mode_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (ui_mode_info == NULL) {
        ui_mode_info = (ui_mode_info_t *)u_malloz(sizeof(ui_mode_info_t));
        EXCEPTION_CHECK_VOID(ui_mode_info);
    }
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    // ui_mode_root_init();
    // common_ui_bg_init(thiz);
    ui_mode_info->force_update = true;
}

void ui_display_pump_mode_remove(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(ui_mode_info);
    // for (uint8_t i = 0; i < 5; i++) {
    //     common_ui_img_resource_deinit(thiz, &ui_mode_info->mode_icon_id_info[i]);
    // }
    // common_ui_bg_deinit(thiz);
    LVGL_OBJ_DELETE(ui_mode_info->root);
    u_free(ui_mode_info);
    ui_mode_info = NULL;
}

int ui_display_pump_mode_process(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(ui_mode_info);
    EXCEPTION_CHECK_INT(ui_mode_info->root);
    // ui_mode_bg_update(thiz);
    // ui_mode_icon_update(thiz, program_type,lactation_kinkstart_mode);
    if (ui_mode_info != NULL) {
        ui_mode_info->force_update = false;
    }
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOG_UI("===> pump mode fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}