/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_PNG_TEST                1
#define IS_SUPPORT_JPEG_TEST               0
#define IS_SUPPORT_BAJ_TEST                0
#define IS_SUPPORT_BG_TEST                 0

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_PNG_TEST
#define PNG_IMG_COUNT  6
static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data0[] = {
    // #include "../../thirdparty/lvgl/bes/examples/res/png_200x200_icon.txt"
    // #include "../../../resources/img_bin/18_296x296.txt"
    // #include "../../../resources/img_bin_png/home22.txt"
    #include "../../../resources/img_bin_png/ver_img_360x360.txt"
    // #include "../../../resources/img_bin/home21_270x270.txt"
    // #include "../../../resources/img_bin/home31.txt"
};
static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data1[] = {
    // #include "../../../resources/img_bin/18.txt"
    // #include "../../../resources/img_bin_png/home32.txt"
    #include "../../../resources/img_bin_png/ver_img_360x360.txt"
};

static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data2[] = {
    // #include "../../thirdparty/lvgl/bes/examples/res/png_128x128_mario.txt"
    #include "../../../resources/img_bin_png/charge_1.txt"
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

static ui_img_data_t png_images[PNG_IMG_COUNT] = {
    { png_data0, sizeof(png_data0) },
    { png_data1, sizeof(png_data1) },
    { png_data2, sizeof(png_data2) },
    { png_data0, sizeof(png_data0) },
    { png_data0, sizeof(png_data0) },
    { png_data0, sizeof(png_data0) },
};
#endif

#if IS_SUPPORT_JPEG_TEST
#define JPEG_IMG_COUNT  2

static PSRAM_DATA_LOC uint8_t jpeg_data0[] = {
    // #include "../../thirdparty/lvgl/bes/examples/res/jpeg_yuv422_128x128.txt"
    #include "../../tests/hwtest/jpegdec/jpeg_yuv444_128x128.txt"
    // #include "../../tests/hwtest/jpegdec/jpeg_yuv422_128x128.txt"
    // #include "../../tests/hwtest/jpegdec/jpeg_yuv444_480x480.txt"
};

static PSRAM_DATA_LOC uint8_t jpeg_data1[] = {
    #include "../../thirdparty/lvgl/bes/examples/res/jpeg_yuv444_100x68.txt"
};

static ui_img_data_t jpeg_images[JPEG_IMG_COUNT] = {
    { jpeg_data0, sizeof(jpeg_data0) },
    { jpeg_data1, sizeof(jpeg_data1) },
};
#endif

#if IS_SUPPORT_BAJ_TEST
#define BAJ_IMG_COUNT  1
static PSRAM_DATA_LOC uint8_t baj_data0[] = {
    // #include "../../../resources/img_bin/home22_360x360_a4_q99_422.txt"
    // #include "../../../resources/img_bin/home32_360x360_a4_q99_422.txt"
    // #include "../../../resources/img_bin/logo_360x360_a4_q99_422.txt"
    // #include "../../../resources/img_bin/pp_mode_360x360_a4_q99_422.txt"
    // #include "../../../resources/img_bin/user_qrcode_360x360_a4_q99_422.txt"
    #include "../../../resources/img_bin/logo_221x29_a4_q99_422.txt"
    // #include "../../../resources/img_bin/home_com_bg_360x360_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t baj_data1[] = {
    // #include "../../../resources/img_bin/home22_360x360_a4_q99_422.txt"
    // #include "../../../resources/img_bin/home32_360x360_a4_q99_422.txt"
    // #include "../../../resources/img_bin/logo_360x360_a4_q99_422.txt"
    // #include "../../../resources/img_bin/pp_mode_360x360_a4_q99_422.txt"
    // #include "../../../resources/img_bin/user_qrcode_360x360_a4_q99_422.txt"
};

static ui_img_data_t baj_images[BAJ_IMG_COUNT] = {
    { baj_data0, sizeof(baj_data0) },
    // { baj_data1, sizeof(baj_data1) },
};
#endif

static lv_obj_t *logo_root;
static uint32_t start_time = 0;
static uint32_t rgb_color_change_time = 0;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_PNG_TEST
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
    const uint8_t *png_data = png_images[index % PNG_IMG_COUNT].data;
    uint32_t png_size = png_images[index % PNG_IMG_COUNT].size;
    lv_fs_make_path_from_buffer(&png_path, LV_FS_MEMFS_LETTER, png_data, png_size, "png");
    lv_image_set_src(logo_root, (const void *)&png_path);
#if 1
    LOGI("===> test 7 png: %d", index);
    if (get_timestamp() - start_time > 5000) {
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
#endif

#if IS_SUPPORT_JPEG_TEST
static int system_test_jpeg_init(void)
{
    if (logo_root == NULL || !lv_obj_is_valid(logo_root)) {
        logo_root = lv_image_create(lv_screen_active());
    }
    lv_fs_path_ex_t jpg_path;
    lv_fs_make_path_from_buffer(&jpg_path, LV_FS_MEMFS_LETTER, jpeg_data0, sizeof(jpeg_data0), "jpg");
    lv_image_set_src(logo_root, (const void *)&jpg_path);
    lv_obj_align(logo_root, LV_ALIGN_CENTER, 0, 0);
    return 0;
}

static int system_test_jpeg_process(void)
{
    static uint32_t index = 0;
    lv_fs_path_ex_t jpg_path;
    uint8_t *jpeg_data = jpeg_images[index % JPEG_IMG_COUNT].data;
    uint32_t jpeg_size = jpeg_images[index % JPEG_IMG_COUNT].size;
    lv_fs_make_path_from_buffer(&jpg_path, LV_FS_MEMFS_LETTER, jpeg_data, jpeg_size, "jpg");
    lv_image_set_src(logo_root, (const void *)&jpg_path);
    LOGI("===> system_test_jpeg_process: %d", index);
    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        index++;
    }
    return 0;
}
#endif

#if IS_SUPPORT_BAJ_TEST
static int system_test_baj_init(void)
{
    if (logo_root == NULL || !lv_obj_is_valid(logo_root)) {
        logo_root = lv_image_create(lv_screen_active());
    }
    lv_obj_set_style_bg_color(logo_root, lv_color_white(), 0);     /// 设置背景颜色为白色
    lv_fs_path_ex_t baj_path;
    lv_fs_make_path_from_buffer(&baj_path, LV_FS_MEMFS_LETTER, baj_data0, sizeof(baj_data0), "baj");
    lv_image_set_src(logo_root, (const void *)&baj_path);
    lv_obj_align(logo_root, LV_ALIGN_CENTER, 0, 0);
#if 0
    lv_obj_t * arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(arc, 200, 200);
    lv_obj_center(arc);
    
    lv_arc_set_bg_angles(arc, 135, 405);
    lv_arc_set_angles(arc, 135, 300);
    
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(arc, lv_color_hex(0xEFEFEF), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 14, LV_PART_MAIN);
    
    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(arc, lv_color_hex(0xB86A6A), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 14, LV_PART_INDICATOR);
    
    /* 去掉旋钮 */
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
#endif
    return 0;
}

static int system_test_baj_process(void)
{
    static uint32_t index = 0;
    lv_fs_path_ex_t baj_path;
    const uint8_t *baj_data = baj_images[index % BAJ_IMG_COUNT].data;
    uint32_t baj_size = baj_images[index % BAJ_IMG_COUNT].size;
    lv_obj_set_style_bg_color(logo_root, lv_color_white(), 0);     /// 设置背景颜色为白色
    lv_fs_make_path_from_buffer(&baj_path, LV_FS_MEMFS_LETTER, baj_data, baj_size, "baj");
    lv_image_set_src(logo_root, (const void *)&baj_path);
    LOGI("===> test 5 baj: %d", index);
    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        index++;
    }
    return 0;
}
#endif


#if IS_SUPPORT_BG_TEST
static int system_lcd_test_init(void)
{
    /// 设置初始背景色为红色
    lv_obj_t *scr = lv_screen_active();
    if (scr != NULL && lv_obj_is_valid(scr)) {
        lv_lock();
        lv_obj_set_style_bg_color(scr, lv_color_hex(0xFF0000), LV_PART_MAIN);
        lv_unlock();
    }
    rgb_color_change_time = get_timestamp();
    return 0;
}

static int system_lcd_test_process(void)
{
    /// 五色交替：每 1 秒切换一次颜色（红、绿、蓝、黑、白）
    static uint8_t color_index = 0;  /// 0=红色, 1=绿色, 2=蓝色, 3=黑色, 4=白色
    uint32_t color_colors[5] = {0xFF0000, 0x00FF00, 0x0000FF, 0x000000, 0xFFFFFF};  /// 红、绿、蓝、黑、白
    
    if (get_timestamp() - rgb_color_change_time > 1000) {
        rgb_color_change_time = get_timestamp();
        color_index = (color_index + 1) % 5;
        
        /// 添加对象有效性检查和线程安全保护
        /// 注意：在 VGLite 渲染线程正在工作时修改对象可能导致崩溃
        /// 使用 lv_refr_now() 确保重绘完成，避免与 VGLite 渲染线程冲突
        lv_obj_t *scr = lv_screen_active();
        if (scr != NULL && lv_obj_is_valid(scr)) {
            lv_lock();
            lv_obj_set_style_bg_color(scr, lv_color_hex(color_colors[color_index]), LV_PART_MAIN);
            /// 立即触发重绘并等待完成，确保 VGLite 渲染线程有时间完成当前任务
            /// 这样可以避免在 GPU 等待中断期间修改对象导致的崩溃
            lv_refr_now(NULL);
            lv_unlock();
            
            LOGI("===> lcd test color: %d", color_index);
        } else {
            LOGE("===> lcd test: invalid screen object");
        }
    }
    return 0;
}
#endif

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int ui_display_logo_prepare(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_PNG_TEST
    system_test_png_init();
#endif
#if IS_SUPPORT_BG_TEST
    system_lcd_test_init();
#endif
#if IS_SUPPORT_JPEG_TEST
    system_test_jpeg_init();
#endif
#if IS_SUPPORT_BAJ_TEST
    system_test_baj_init();
#endif
    return 0;
}

int ui_display_logo_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(logo_root);
    return 0;
}

int ui_display_logo_process(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_PNG_TEST
    system_test_png_process();
#endif
#if IS_SUPPORT_BG_TEST
    system_lcd_test_process();
#endif
#if IS_SUPPORT_JPEG_TEST
    system_test_jpeg_process();
#endif
#if IS_SUPPORT_BAJ_TEST
    system_test_baj_process();
#endif
    return 0;
}
