/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep
#include "window_screen_home.h" // IWYU pragma: keep
#include "ble_ui.h" // IWYU pragma: keep
/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_ROOT_HOME_SCREEN_SAVER_COMP_BG                (0)

#define HOME_SCREEN_SAVER_ICON_MAX_COUNT                         (1)
#define HOME_SCREEN_SAVER_ICON_INDEX                       (0)

#define HOME_SCREEN_SAVER_ICON_X                          (60)
#define HOME_SCREEN_SAVER_ICON_Y                          (130 + 2)

typedef struct {
    lv_obj_t * root;
    #if IS_SUPPORT_ROOT_HOME_SCREEN_SAVER_COMP_BG
    lv_obj_t * backgroud;
    #endif
    lv_obj_t * home_screen_saver_icon;
    res_img_info_t home_screen_saver_icon_info;
    uint8_t force_update;
} home_screen_saver_display_info_t;

typedef struct {
    const char *name;
    uint16_t img_id;
    uint16_t icon_x;
    uint16_t icon_y;
} home_screen_saver_icon_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static home_screen_saver_display_info_t *home_screen_saver_info;
const static home_screen_saver_icon_info_t home_screen_saver_icon_infos[HOME_SCREEN_SAVER_ICON_MAX_COUNT] = {
    { "home_screen_saver_icon", RESOURCE_ID_DUMMY_ICON, HOME_SCREEN_SAVER_ICON_X, HOME_SCREEN_SAVER_ICON_Y },
};  
/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_ROOT_DUMMY_COMP_BG
static PSRAM_DATA_LOC uint8_t home_screen_saver_bg_data[] = {
    // #include "comparison_png2txt/bg_360x360.txt"
    #include "png2txt/dummy_com_bg_360x360.txt"
    // #include "comparison_png2txt/home_comp_1_v2_360x360.txt"
    // #include "comparison_png2txt/home_comp_80_v2_360x360.txt"
    // #include "comparison_png2txt/home_comp_100_v2_360x360.txt"
};
#endif
/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void home_screen_saver_root_init(void)
{
    if (home_screen_saver_info->root == NULL || !lv_obj_is_valid(home_screen_saver_info->root)) {
        home_screen_saver_info->root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(home_screen_saver_info->root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(home_screen_saver_info->root, lv_color_white(), 0);     /// 设置背景颜色为白色
    lv_obj_set_style_bg_opa(home_screen_saver_info->root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(home_screen_saver_info->root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(home_screen_saver_info->root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(home_screen_saver_info->root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(home_screen_saver_info->root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(home_screen_saver_info->root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
}

#if IS_SUPPORT_ROOT_DUMMY_COMP_BG
static void home_screen_saver_root_bg_update(ui_manager_context_t *thiz)
{
    static uint32_t start_time = 0;
    static uint8_t status = 0;
    if (home_screen_saver_info->backgroud == NULL || !lv_obj_is_valid(home_screen_saver_info->backgroud)) {
        home_screen_saver_info->backgroud = lv_image_create(home_screen_saver_info->root);
        EXCEPTION_CHECK_VOID(home_screen_saver_info->backgroud);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(home_screen_saver_info->backgroud, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(home_screen_saver_info->backgroud, LV_OBJ_FLAG_HIDDEN);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_screen_saver_bg_data,
            sizeof(home_screen_saver_bg_data), "png");
        lv_image_set_src(home_screen_saver_info->backgroud, (const void *)&bg_img_path);
        lv_obj_align(home_screen_saver_info->backgroud, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}
#else
static void home_screen_saver_root_bg_update(ui_manager_context_t *thiz)
{
    common_ui_bg_update(thiz, home_screen_saver_info->root);
}
#endif

static void home_screen_saver_root_icon_update(ui_manager_context_t *thiz)
{   /// 设置dummy icon
    if (home_screen_saver_info->force_update == false) {
        return;
    }
    
    if (home_screen_saver_info->home_screen_saver_icon == NULL || !lv_obj_is_valid(home_screen_saver_info->home_screen_saver_icon)) {
        home_screen_saver_info->home_screen_saver_icon = lv_image_create(home_screen_saver_info->root);
        EXCEPTION_CHECK_VOID(home_screen_saver_info->home_screen_saver_icon);
    }
    
    common_ui_img_resource_init(thiz, home_screen_saver_icon_infos[HOME_SCREEN_SAVER_ICON_INDEX].img_id, &home_screen_saver_info->home_screen_saver_icon_info);
    EXCEPTION_CHECK_VOID(home_screen_saver_info->home_screen_saver_icon_info.data);
    lv_fs_path_ex_t home_screen_saver_icon_img_path;

    lv_fs_make_path_from_buffer(&home_screen_saver_icon_img_path, LV_FS_MEMFS_LETTER, 
        home_screen_saver_info->home_screen_saver_icon_info.data, home_screen_saver_info->home_screen_saver_icon_info.size, "baj");
    lv_image_set_src(home_screen_saver_info->home_screen_saver_icon, (const void *)&home_screen_saver_icon_img_path);
    lv_obj_align(home_screen_saver_info->home_screen_saver_icon, LV_ALIGN_TOP_LEFT, 
        home_screen_saver_icon_infos[HOME_SCREEN_SAVER_ICON_INDEX].icon_x, home_screen_saver_icon_infos[HOME_SCREEN_SAVER_ICON_INDEX].icon_y); /// 设置icon位置
}
/*  
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_home_screen_saver_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (home_screen_saver_info == NULL) {
        home_screen_saver_info = (home_screen_saver_display_info_t *)u_malloz(sizeof(home_screen_saver_display_info_t));
        EXCEPTION_CHECK_VOID(home_screen_saver_info);
    }
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    home_screen_saver_root_init();
    common_ui_bg_init(thiz);
    ble_ui_init(thiz);
    home_screen_saver_info->force_update = true;
}

void ui_display_home_screen_saver_remove(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (home_screen_saver_info == NULL) {
        return;
    }
    common_ui_img_resource_deinit(thiz, &home_screen_saver_info->home_screen_saver_icon_info);
    ble_ui_deinit(thiz);
    common_ui_bg_deinit(thiz);
    LVGL_OBJ_DELETE(home_screen_saver_info->root);
    u_free(home_screen_saver_info);
    home_screen_saver_info = NULL;
}

int ui_display_home_screen_saver_process(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(home_screen_saver_info);
    EXCEPTION_CHECK_INT(home_screen_saver_info->root);
    ui_proc_home_t *proc = (ui_proc_home_t *)thiz->priv.play_info.user_data;
    EXCEPTION_CHECK_INT(proc);
    EXCEPTION_CHECK_INT(proc->ble);
    home_screen_saver_root_bg_update(thiz);
    home_screen_saver_root_icon_update(thiz);
    ble_ui_update(thiz, home_screen_saver_info->root, proc->ble, 168, 22);
    home_screen_saver_info->force_update = false;
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOG_UI("===> dummy fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}
