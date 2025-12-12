/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
LV_IMAGE_DECLARE(img_cogwheel_argb);
LV_IMAGE_DECLARE(img_cogwheel_rgb);
LV_IMAGE_DECLARE(img_cogwheel_indexed16);
LV_IMAGE_DECLARE(img_mario_i2);
LV_IMAGE_DECLARE(img_mario_i4);
LV_IMAGE_DECLARE(img_mario_i8);

#define VAR_IMG_COUNT   6
static const lv_image_dsc_t *var_images[VAR_IMG_COUNT] = {
    &img_cogwheel_argb,
    &img_cogwheel_rgb,
    &img_cogwheel_indexed16,
    &img_mario_i2,
    &img_mario_i4,
    &img_mario_i8,
};

static lv_obj_t *dummy_root;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int system_test_var_init(void)
{
    if (dummy_root == NULL || !lv_obj_is_valid(dummy_root)) {
        dummy_root = lv_image_create(lv_screen_active());
    }
    lv_image_set_src(dummy_root, &img_cogwheel_argb);
    lv_obj_align(dummy_root, LV_ALIGN_CENTER, 0, 0);
    return 0;
}

static int system_test_var_process(void)
{
    static uint32_t index = 0;
    lv_image_set_src(dummy_root, var_images[index % VAR_IMG_COUNT]);
    index++;
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_dummy_prepare(ui_manager_context_t *thiz)
{
    LVGL_OBJ_CLEAN();
    system_test_var_init();
    LOG_UI("ui_display_dummy_prepare");
}

void ui_display_dummy_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(dummy_root);
}

int ui_display_dummy_process(ui_manager_context_t *thiz)
{
    system_test_var_process();
    return 0;
}