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
#define PNG_IMG_COUNT  6

struct img_data_t {
    uint8_t *data;
    uint32_t size;
};

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data0[] = {
    // #include "../../thirdparty/lvgl/bes/examples/res/png_200x200_icon.txt"
    // #include "../../../resources/img_bin/18_296x296.txt"
    #include "../../../resources/img_bin/home22.txt"
    // #include "../../../resources/img_bin/home21_270x270.txt"
    // #include "../../../resources/img_bin/home31.txt"
};
static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data1[] = {
    // #include "../../../resources/img_bin/18.txt"
    #include "../../../resources/img_bin/home32.txt"
};

static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data2[] = {
    // #include "../../thirdparty/lvgl/bes/examples/res/png_128x128_mario.txt"
    #include "../../../resources/img_bin/charge_1.txt"
};
// static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data3[] = {
//     #include "../../thirdparty/lvgl/bes/examples/res/png_200x100_30fps.txt"
// };
// static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data4[] = {
//     #include "../../thirdparty/lvgl/bes/examples/res/png_80x40_battery.txt"
// };
// static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data5[] = {
//     #include "../../thirdparty/lvgl/bes/examples/res/png_128x128_lena.txt"
// };

static struct img_data_t png_images[PNG_IMG_COUNT] = {
    { png_data0, sizeof(png_data0) },
    { png_data1, sizeof(png_data1) },
    { png_data2, sizeof(png_data2) },
    { png_data0, sizeof(png_data0) },
    { png_data0, sizeof(png_data0) },
    { png_data0, sizeof(png_data0) },
};

static lv_obj_t *logo_root;
static uint32_t start_time = 0;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static int system_test_png_init(void)
{
    if (logo_root == NULL || !lv_obj_is_valid(logo_root)) {
        logo_root = lv_image_create(lv_screen_active());
    }
    lv_fs_path_ex_t png_path;
    lv_fs_make_path_from_buffer(&png_path, LV_FS_MEMFS_LETTER, png_data0, sizeof(png_data0), "png");
    lv_image_set_src(logo_root, (const void *)&png_path);
    lv_obj_align(logo_root, LV_ALIGN_CENTER, 0, 0);
    return 0;
}

static int system_test_png_process(void)
{
    static uint32_t index = 0;
    lv_fs_path_ex_t png_path;
    uint8_t *png_data = png_images[index % PNG_IMG_COUNT].data;
    uint32_t png_size = png_images[index % PNG_IMG_COUNT].size;
    lv_fs_make_path_from_buffer(&png_path, LV_FS_MEMFS_LETTER, png_data, png_size, "png");
    lv_image_set_src(logo_root, (const void *)&png_path);
#if 1
    if (get_timestamp() - start_time > 10000) {
        start_time = get_timestamp();
        index++;
        if (index > 2) {
            index = 0;
        }
    }
#else
    index++;
#endif
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int ui_display_logo_prepare(ui_manager_context_t *thiz)
{
    system_test_png_init();
    return 0;
}

int ui_display_logo_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(logo_root);
    return 0;
}

int ui_display_logo_process(ui_manager_context_t *thiz)
{
    system_test_png_process();
    return 0;
}
