/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "window_screen_pump.h" // IWYU pragma: keep
#include "breast_pump_ui.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define BREASTFEEDING_MODE_MAX_COUNT             (4)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
typedef struct {
    lv_obj_t * root;
    res_img_info_t icon_infos;
} bf_mode_display_info_t;

static bf_mode_display_info_t *bf_mode_info;

const static uint16_t breastfeeding_icon_ids[BREASTFEEDING_MODE_MAX_COUNT] = {
    RESOURCE_ID_PERSONALIZATION_MODE,
    RESOURCE_ID_PP_MODE,
    RESOURCE_ID_MILK_MODE,
    RESOURCE_ID_INITIATING_MODE,
};
/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static void breastfeeding_mode_img_update(ui_manager_context_t *thiz, uint8_t mode_index)
{
    if (mode_index >= BREASTFEEDING_MODE_MAX_COUNT) {
        return;
    }

    if (bf_mode_info->root == NULL || !lv_obj_is_valid(bf_mode_info->root)) {
        bf_mode_info->root = lv_image_create(lv_screen_active());
    }

#if 0
    const uint16_t img_id = breastfeeding_icon_ids[mode_index];
    if (s_breastfeeding_last_img_id == img_id && bf_mode_info->icon_infos.data &&
    bf_mode_info->icon_infos.size) {
        return;
    }

    /* Drop cache of previous image to avoid cache growth when switching. */
    if (bf_mode_info->root && lv_obj_is_valid(bf_mode_info->root)) {
        const void * old_src = lv_image_get_src(bf_mode_info->root);
        if (old_src) {
            lv_image_cache_drop(old_src);
        }
    }
    common_ui_img_resource_init(thiz, img_id, &bf_mode_info->icon_infos);
#else
    common_ui_img_resource_init(thiz, breastfeeding_icon_ids[mode_index], 
        &bf_mode_info->icon_infos);
#endif

    EXCEPTION_CHECK_VOID(bf_mode_info->icon_infos.data);
    lv_fs_path_ex_t breastfeeding_img_path;
    lv_fs_make_path_from_buffer(&breastfeeding_img_path, LV_FS_MEMFS_LETTER, 
        bf_mode_info->icon_infos.data, bf_mode_info->icon_infos.size, "baj");
    lv_image_set_src(bf_mode_info->root, breastfeeding_img_path.path);
    lv_obj_align(bf_mode_info->root, LV_ALIGN_CENTER, 0, 0);
}


/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_breastfeeding_mode_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (bf_mode_info == NULL) {
        bf_mode_info = (bf_mode_display_info_t *)u_malloz(sizeof(bf_mode_display_info_t));
        EXCEPTION_CHECK_VOID(bf_mode_info);
    }
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
}

void ui_display_breastfeeding_mode_remove(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (bf_mode_info == NULL) {
        return;
    }
    common_ui_img_resource_deinit(thiz, &bf_mode_info->icon_infos);
    LVGL_OBJ_DELETE(bf_mode_info->root);
    u_free(bf_mode_info);
    bf_mode_info = NULL;
}

int ui_display_breastfeeding_mode(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    ui_proc_pump_t *proc = (ui_proc_pump_t *)thiz->priv.play_info.user_data;
    EXCEPTION_CHECK_INT(proc->pump);
    uint8_t mode = proc->pump->get_breast_pump_mode();
    if (mode == BREAST_PUMP_CUSTOM_MODE) {
        breastfeeding_mode_img_update(thiz, 0);
    } else if (mode == BREAST_PUMP_PP_MILK_TRACK_MODE) {
        breastfeeding_mode_img_update(thiz, 1);
    } else if (mode == BREAST_PUMP_MILK_BOOST_MODE) {
        breastfeeding_mode_img_update(thiz, 2);
    } else {
        breastfeeding_mode_img_update(thiz, 3);
    }
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOG_UI("===> breastfeeding mode fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}
