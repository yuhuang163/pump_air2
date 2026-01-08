/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "resource_id_define.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define HOME_BATTERY_MAX_COUNT                     (6)
#define HOME_CHARGE_BATTERY_MAX_COUNT              (5)
#define HOME_BATTERY_TEXT_COLOR                    (0xB25962)
#define HOME_BATTERY_TEXT_RING_SIZE                (310)
#define HOME_BATTERY_TEXT_RING_R                   (HOME_BATTERY_TEXT_RING_SIZE / 2)
#define HOME_BATTERY_TEXT_ANGLE_DEG                (288)


/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static lv_obj_t * battery_text;
static lv_obj_t * battery_icon_img;
static res_img_info_t battery_icon_info;
static lv_obj_t * charge_icon_img;
static res_img_info_t charge_icon_info;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int get_battery_img_id(uint8_t battery_percent)
{
    if (battery_percent >= 80) {
        return RESOURCE_ID_HOME_BATTERY_100;
    } else if (battery_percent >= 60) {
        return RESOURCE_ID_HOME_BATTERY_80;
    } else if (battery_percent >= 40) {
        return RESOURCE_ID_HOME_BATTERY_60;
    } else if (battery_percent >= 20) {
        return RESOURCE_ID_HOME_BATTERY_40;
    } else if (battery_percent >= 2) {
        return RESOURCE_ID_HOME_BATTERY_20;
    }
    return RESOURCE_ID_HOME_BATTERY_0;
}

static void battery_text_place(lv_obj_t * lab)
{
    /// 父对象 360x360，圆心(180,180)
    lv_point_t c = { .x = PANLE_DISPLAY_W / 2, .y = PANLE_DISPLAY_H / 2 };
    /// 计算圆周点
    lv_coord_t px = c.x + (lv_trigo_cos(HOME_BATTERY_TEXT_ANGLE_DEG) * HOME_BATTERY_TEXT_RING_R) / LV_TRIGO_SIN_MAX;
    lv_coord_t py = c.y + (lv_trigo_sin(HOME_BATTERY_TEXT_ANGLE_DEG) * HOME_BATTERY_TEXT_RING_R) / LV_TRIGO_SIN_MAX;
    /// 先把label中心放到该点（固定位置）
    lv_obj_align(lab, LV_ALIGN_CENTER, px - c.x, py - c.y);
    /// 旋转中心设为label自身中心（必须在 set_text 后取宽高）
    lv_coord_t w = lv_obj_get_width(lab);
    lv_coord_t h = lv_obj_get_height(lab);
    lv_obj_set_style_transform_pivot_x(lab, w / 2, 0);
    lv_obj_set_style_transform_pivot_y(lab, h / 2, 0);
    /// 切线方向：angle + 90°（单位0.1°）
    int tangent = HOME_BATTERY_TEXT_ANGLE_DEG + 90;
    /// 文字永远“正着读”, 避免在某些角度出现180°倒置
    if (tangent > 90 && tangent < 270) {
        tangent = (tangent + 180) % 360;
    }
    lv_obj_set_style_transform_angle(lab, tangent * 10, 0);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void battery_icon_update(ui_manager_context_t *thiz, lv_obj_t *root, uint8_t battery_percent, uint8_t is_charging)
{
    // battery_percent = 80; /// TODO: 测试用
    if (battery_icon_img == NULL || !lv_obj_is_valid(battery_icon_img)) {
        battery_icon_img = lv_image_create(root);
        EXCEPTION_CHECK_VOID(battery_icon_img);
    }
    common_ui_img_resource_init(thiz, get_battery_img_id(battery_percent), &battery_icon_info);
    EXCEPTION_CHECK_VOID(battery_icon_info.data);
    lv_fs_path_ex_t battery_icon_img_path;
    lv_fs_make_path_from_buffer(&battery_icon_img_path, LV_FS_MEMFS_LETTER, battery_icon_info.data,
        battery_icon_info.size, "baj");
    lv_image_set_src(battery_icon_img, (const void *)&battery_icon_img_path);
    lv_obj_align(battery_icon_img, LV_ALIGN_TOP_LEFT, 156 + 3, 22);

    if (battery_text == NULL || !lv_obj_is_valid(battery_text)) {
        battery_text = lv_label_create(root);
        EXCEPTION_CHECK_VOID(battery_text);
        battery_text_place(battery_text);
    }
    char second_value[8] = {0};
    sprintf(second_value, "%02d%%", battery_percent);
    lv_label_set_text(battery_text, second_value);
    lv_obj_set_style_text_color(battery_text, lv_color_hex(HOME_BATTERY_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(battery_text, &lv_font_oppo_sans_14_medium, 0);
  
    if (charge_icon_img == NULL || !lv_obj_is_valid(charge_icon_img)) {
        charge_icon_img = lv_image_create(root);
        EXCEPTION_CHECK_VOID(charge_icon_img);
    }
    common_ui_img_resource_init(thiz, 
        is_charging > 0 ? RESOURCE_ID_HOME_CHARGING : RESOURCE_ID_HOME_UNCHARGE, &charge_icon_info);
    EXCEPTION_CHECK_VOID(charge_icon_info.data);
    lv_fs_path_ex_t charge_icon_img_path;
    lv_fs_make_path_from_buffer(&charge_icon_img_path, LV_FS_MEMFS_LETTER, charge_icon_info.data,
        charge_icon_info.size, "baj");
    lv_image_set_src(charge_icon_img, (const void *)&charge_icon_img_path);
    lv_obj_align(charge_icon_img, LV_ALIGN_TOP_LEFT, 140 + 3, 27);
}

void battery_icon_deinit(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    battery_icon_img = NULL;
    common_ui_img_resource_deinit(thiz, &battery_icon_info);
    charge_icon_img = NULL;
    common_ui_img_resource_deinit(thiz, &charge_icon_info);
}

void ui_display_charge_screen_prepare(ui_manager_context_t *thiz)
{

}

void ui_display_charge_screen_remove(ui_manager_context_t *thiz)
{

}

int ui_display_charge_screen(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}