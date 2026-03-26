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
#define HOME_BATTERY_TEXT_RING_SIZE                (310 - 4)
#define HOME_BATTERY_TEXT_RING_R                   (HOME_BATTERY_TEXT_RING_SIZE / 2)
#define HOME_BATTERY_TEXT_ANGLE_DEG                (285)

typedef struct {    
    lv_obj_t * cap_icon;
    res_img_info_t cap_icon_info;
    lv_obj_t * state_icon;
    res_img_info_t stat_icon_info;
    uint8_t percent;
    ui_battery_state_e state;
    uint8_t force_update;
} battery_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static battery_display_info_t *battery_info;

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
        return RESOURCE_ID_BATT_CAP_100_ICON;
    } else if (battery_percent >= 60) {
        return RESOURCE_ID_BATT_CAP_80_ICON;
    } else if (battery_percent >= 40) {
        return RESOURCE_ID_BATT_CAP_60_ICON;
    } else if (battery_percent >= 20) {
        return RESOURCE_ID_BATT_CAP_40_ICON;
    } else if (battery_percent >= 2) {
        return RESOURCE_ID_BATT_CAP_20_ICON;
    }
    return RESOURCE_ID_BATT_CAP_0_ICON;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int battery_ui_update(ui_manager_context_t *thiz, lv_obj_t *root, battery_manager_context_t *battery, ui_battery_state_e state)
{
    EXCEPTION_CHECK_INT(battery);
    uint8_t percent = battery->get_battery_percent();
    if (battery_info->force_update == false && battery_info->percent == percent && battery_info->state == state) {
        return 0;
    }

    EXCEPTION_CHECK_INT(battery_info);
    if (battery_info->cap_icon == NULL || !lv_obj_is_valid(battery_info->cap_icon)) {
        battery_info->cap_icon = lv_image_create(root);
        EXCEPTION_CHECK_INT(battery_info->cap_icon);
        lv_obj_align(battery_info->cap_icon, LV_ALIGN_TOP_LEFT, 156 + 3, 22);
    }
    common_ui_img_resource_init(thiz, get_battery_img_id(percent), &battery_info->cap_icon_info);
    EXCEPTION_CHECK_INT(battery_info->cap_icon_info.data);
    lv_fs_path_ex_t battery_icon_img_path;
    lv_fs_make_path_from_buffer(&battery_icon_img_path, LV_FS_MEMFS_LETTER, battery_info->cap_icon_info.data,
        battery_info->cap_icon_info.size, "baj");
    lv_image_set_src(battery_info->cap_icon, (const void *)&battery_icon_img_path);

    if (battery_info->state_icon ==  NULL || !lv_obj_is_valid(battery_info->state_icon)) {
        battery_info->state_icon = lv_image_create(root);
        EXCEPTION_CHECK_INT(battery_info->state_icon);
    }

    if (state == UI_BATTERY_STATE_CHARGING) {
        lv_obj_clear_flag(battery_info->state_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_align(battery_info->state_icon, LV_ALIGN_TOP_LEFT, 140 + 3, 27);
        common_ui_img_resource_init(thiz, RESOURCE_ID_CHARGE_ICON, &battery_info->stat_icon_info);
    } else if (state == UI_BATTERY_STATE_LOW_POWER) {
        lv_obj_clear_flag(battery_info->state_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_align(battery_info->state_icon, LV_ALIGN_TOP_LEFT, 140 + 3, 27);
        common_ui_img_resource_init(thiz, RESOURCE_ID_LOW_POWER_ICON, &battery_info->stat_icon_info);
    } else {
        lv_obj_add_flag(battery_info->state_icon, LV_OBJ_FLAG_HIDDEN);
        goto DISPLAY_EXIT;
    }
    EXCEPTION_CHECK_INT(battery_info->stat_icon_info.data);
    lv_fs_path_ex_t state_icon_img_path;
    lv_fs_make_path_from_buffer(&state_icon_img_path, LV_FS_MEMFS_LETTER, battery_info->stat_icon_info.data,
        battery_info->stat_icon_info.size, "baj");
    lv_image_set_src(battery_info->state_icon, (const void *)&state_icon_img_path);

DISPLAY_EXIT:    
    battery_info->force_update = false;
    battery_info->percent = percent;
    battery_info->state = state;
    return 0;
}

void battery_ui_init(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (battery_info == NULL) {
        battery_info = (battery_display_info_t *)u_malloz(sizeof(battery_display_info_t));
        EXCEPTION_CHECK_VOID(battery_info);
    }
    battery_info->percent = 0;
    battery_info->state = UI_BATTERY_STATE_NONE;
    battery_info->force_update = true;
}

void battery_ui_deinit(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    common_ui_img_resource_deinit(thiz, &battery_info->cap_icon_info);
    common_ui_img_resource_deinit(thiz, &battery_info->stat_icon_info);
    u_free(battery_info);
    battery_info = NULL;
}
