/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "motor_manager_api.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"
#include "ui_page_define.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "motor_manager_api.h" // IWYU pragma: keep
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define JPEG_IMG_COUNT  6
#define PNG_IMG_COUNT  6

struct img_data_t {
    uint8_t *data;
    uint32_t size;
};

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/// JPEG
static PSRAM_DATA_LOC uint8_t jpeg_data0[] = { 
    #include "jpeg_yuv422_128x128.txt"
};

static PSRAM_DATA_LOC uint8_t jpeg_data1[] = {
    #include "jpeg_yuv444_100x68.txt"
};

static PSRAM_DATA_LOC uint8_t jpeg_data2[] = {
    #include "jpeg_yuv444_65x65.txt"
};
static PSRAM_DATA_LOC uint8_t jpeg_data3[] = {
    #include "jpeg_yuv444_61x58.txt"
};
static PSRAM_DATA_LOC uint8_t jpeg_data4[] = {
    #include "jpeg_yuv420_100x72.txt"
};
static PSRAM_DATA_LOC uint8_t jpeg_data5[] = {
    #include "jpeg_yuv420_75x100.txt"
};

static struct img_data_t jpeg_images[JPEG_IMG_COUNT] = {
    { jpeg_data0, sizeof(jpeg_data0) },
    { jpeg_data1, sizeof(jpeg_data1) },
    { jpeg_data2, sizeof(jpeg_data2) },
    { jpeg_data3, sizeof(jpeg_data3) },
    { jpeg_data4, sizeof(jpeg_data4) },
    { jpeg_data5, sizeof(jpeg_data5) },
};

/// PNG
static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data0[] = {
#include "png_200x200_icon.txt"
};
static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data1[] = {
#include "png_180x180_seashore.txt"
};
static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data2[] = {
#include "png_128x128_mario.txt"
};
static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data3[] = {
#include "png_200x100_30fps.txt"
};
static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data4[] = {
#include "png_80x40_battery.txt"
};
static PSRAM_DATA_LOC ALIGNED(64) uint8_t png_data5[] = {
#include "png_128x128_lena.txt"
};

static struct img_data_t png_images[PNG_IMG_COUNT] = {
    { png_data0, sizeof(png_data0) },
    { png_data1, sizeof(png_data1) },
    { png_data2, sizeof(png_data2) },
    { png_data3, sizeof(png_data3) },
    { png_data4, sizeof(png_data4) },
    { png_data5, sizeof(png_data5) },
};

static lv_obj_t *img_jpeg;
static lv_obj_t *img_png;
static lv_obj_t *qrcode;
#define QRSTR_COUNT   6
static const char *system_qr_str[QRSTR_COUNT] = {
    "https://lvgl.io",
    "AAAAAAAAAAAAAAAA",
    "Hello World!",
    "LVGL IMAGES",
    "BES",
    "OK, All is done!",
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void img_png_prepare(void)
{
    img_png = lv_image_create(lv_screen_active());
    lv_fs_path_ex_t png_path;
    lv_fs_make_path_from_buffer(&png_path, LV_FS_MEMFS_LETTER, png_data0, sizeof(png_data0), "png");
    lv_image_set_src(img_png, (const void *)&png_path);
    lv_obj_align(img_png, LV_ALIGN_CENTER, -70, -120);
    int32_t img_width = lv_image_get_src_width(img_jpeg);
    int32_t img_height = lv_image_get_src_height(img_jpeg);
    int32_t obj_width = lv_obj_get_width(img_jpeg);
    int32_t obj_height = lv_obj_get_height(img_jpeg);
    if (img_width == 0 || img_height == 0) {
        LOGE("==>JPEG: Image decode failed! img_size=%dx%d, obj_size=%dx%d", 
             img_width, img_height, obj_width, obj_height);
    } else {
        LOGI("==>JPEG: Image loaded OK! img_size=%dx%d, obj_size=%dx%d", 
             img_width, img_height, obj_width, obj_height);
    }
}

static void img_png_change(uint32_t index)
{
    lv_fs_path_ex_t png_path;
    uint8_t *png_data = png_images[index % PNG_IMG_COUNT].data;
    uint32_t png_size = png_images[index % PNG_IMG_COUNT].size;
    lv_fs_make_path_from_buffer(&png_path, LV_FS_MEMFS_LETTER, png_data, png_size, "png");
    lv_image_set_src(img_png, (const void *)&png_path);
    int32_t img_width = lv_image_get_src_width(img_jpeg);
    int32_t img_height = lv_image_get_src_height(img_jpeg);
    int32_t obj_width = lv_obj_get_width(img_jpeg);
    int32_t obj_height = lv_obj_get_height(img_jpeg);
    if (img_width == 0 || img_height == 0) {
        LOGE("==>JPEG: Image decode failed! img_size=%dx%d, obj_size=%dx%d", 
             img_width, img_height, obj_width, obj_height);
    } else {
        LOGI("==>JPEG: Image loaded OK! img_size=%dx%d, obj_size=%dx%d", 
             img_width, img_height, obj_width, obj_height);
    }
}

static void img_jpeg_prepare(void)
{
    /// 检查JPEG数据是否有效
    uint32_t jpeg_size = sizeof(jpeg_data0);
    if (jpeg_size == 0) {
        LOGE("==>JPEG: jpeg_data0 size is 0!");
        return;
    }
    LOGI("==>JPEG: jpeg_data0 addr=%p, size=%u", (void *)jpeg_data0, jpeg_size);
    
    /// 检查JPEG数据头部（应该是0xFF 0xD8开头）
    if (jpeg_data0[0] != 0xFF || jpeg_data0[1] != 0xD8) {
        LOGE("==>JPEG: Invalid JPEG header! First 2 bytes: 0x%02X 0x%02X", jpeg_data0[0], jpeg_data0[1]);
    } else {
        LOGI("==>JPEG: JPEG header OK (0xFF 0xD8)");
    }

    /// 创建图片对象
    img_jpeg = lv_image_create(lv_screen_active());
    if (img_jpeg == NULL) {
        LOGE("==>JPEG: Failed to create image object!");
        return;
    }
    LOGI("==>JPEG: Image object created, addr=%p", img_jpeg);

    /// 创建内存文件系统路径
    lv_fs_path_ex_t jpg_path;
    lv_fs_make_path_from_buffer(&jpg_path, LV_FS_MEMFS_LETTER, jpeg_data0, jpeg_size, "jpg");
    LOGI("==>JPEG: Path created, path_str=%s", jpg_path.path);
    
    /// 检查内存文件系统是否就绪
    if (!lv_fs_is_ready(LV_FS_MEMFS_LETTER)) {
        LOGE("==>JPEG: Memory filesystem (letter='%c') is NOT ready!", LV_FS_MEMFS_LETTER);
    } else {
        LOGI("==>JPEG: Memory filesystem (letter='%c') is ready", LV_FS_MEMFS_LETTER);
    }
    
    /// 检查JPEG解码器是否启用
#ifdef LVGL_BES_JPEG
    LOGI("==>JPEG: LVGL_BES_JPEG macro is defined");
#else
    LOGE("==>JPEG: LVGL_BES_JPEG macro is NOT defined! JPEG decoder may not be enabled!");
#endif

#if LV_USE_BESJPEG
    LOGI("==>JPEG: LV_USE_BESJPEG is enabled (1)");
#else
    LOGE("==>JPEG: LV_USE_BESJPEG is disabled (0)! This is the problem - JPEG decoder is not enabled!");
#endif

    /// 设置图片源
    lv_image_set_src(img_jpeg, (const void *)&jpg_path);
    
    /// 等待一下让图片解码完成（如果需要）
    /// 检查图片尺寸
    int32_t img_width = lv_image_get_src_width(img_jpeg);
    int32_t img_height = lv_image_get_src_height(img_jpeg);
    int32_t obj_width = lv_obj_get_width(img_jpeg);
    int32_t obj_height = lv_obj_get_height(img_jpeg);
    
    if (img_width == 0 || img_height == 0) {
        LOGE("==>JPEG: Image decode failed! img_size=%dx%d, obj_size=%dx%d", 
             img_width, img_height, obj_width, obj_height);
    } else {
        LOGI("==>JPEG: Image loaded OK! img_size=%dx%d, obj_size=%dx%d", 
             img_width, img_height, obj_width, obj_height);
    }
    
    /// 设置图片位置
    lv_obj_align(img_jpeg, LV_ALIGN_CENTER, 100, -100);
    LOGI("==>JPEG: Image aligned to CENTER with offset (100, -100)");

}

static void img_jpeg_change(uint32_t index)
{
    lv_fs_path_ex_t jpg_path;
    uint8_t *jpeg_data = jpeg_images[index % JPEG_IMG_COUNT].data;
    uint32_t jpeg_size = jpeg_images[index % JPEG_IMG_COUNT].size;
    lv_fs_make_path_from_buffer(&jpg_path, LV_FS_MEMFS_LETTER, jpeg_data, jpeg_size, "jpg");
    lv_image_set_src(img_jpeg, (const void *)&jpg_path);
}

static void qrcode_prepare(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);
    qrcode = lv_qrcode_create(lv_screen_active());
    lv_qrcode_set_size(qrcode, 120);
    lv_qrcode_set_dark_color(qrcode, fg_color);
    lv_qrcode_set_light_color(qrcode, bg_color);
    lv_obj_align(qrcode, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_border_color(qrcode, bg_color, 0);
    lv_obj_set_style_border_width(qrcode, 5, 0);
    lv_qrcode_update(qrcode, system_qr_str[0], strlen(system_qr_str[0]));
}

static void qrcode_change(uint32_t index)
{
    lv_qrcode_update(qrcode, system_qr_str[index % QRSTR_COUNT], strlen(system_qr_str[index % QRSTR_COUNT]));
}

void ui_display_pump_home_screen_prepare(ui_manager_context_t *thiz)
{
    lv_obj_t *screen = lv_screen_active();
    if (screen) {
        lv_obj_clean(screen);
    }
    // img_jpeg_prepare();
    // img_png_prepare();
    qrcode_prepare();

}

void ui_display_pump_home_screen_remove(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_home_screen_remove");
}

int ui_display_pump_home_screen(ui_manager_context_t *thiz)
{
    static uint32_t index = 0;
    // img_jpeg_change(index);
    // img_png_change(index);
    qrcode_change(index);
    LOGI("==>index: %d", index);
    index++;
    return 0;
}

void ui_display_pump_left_level_prepare(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_left_level_prepare");
}

void ui_display_pump_left_level_remove(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_left_level_remove");
}

int ui_display_pump_left_level(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}

void ui_display_pump_right_level_prepare(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_right_level_prepare");
}

void ui_display_pump_right_level_remove(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_right_level_remove");
}

int ui_display_pump_right_level(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}

void ui_display_pump_complete_tip_prepare(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_complete_tip_prepare");
}

void ui_display_pump_complete_tip_remove(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_complete_tip_remove");
}

int ui_display_pump_complete_tip(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}

void ui_display_pump_mode_prepare(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_mode_prepare");
}

void ui_display_pump_mode_remove(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_mode_remove");
}

int ui_display_pump_mode(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}

void ui_display_breastfeeding_mode_prepare(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_breastfeeding_mode_prepare");
}

void ui_display_breastfeeding_mode_remove(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_breastfeeding_mode_remove");
}

int ui_display_breastfeeding_mode(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}

void ui_display_pump_speed_prepare(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_speed_prepare");
}

void ui_display_pump_speed_remove(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_speed_remove");
}

int ui_display_pump_speed(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}