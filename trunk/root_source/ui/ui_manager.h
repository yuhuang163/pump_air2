#ifndef __UI_MANAGER_H__
#define __UI_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_time_warpper.h"
#include "project_config.h" // IWYU pragma: keep

#if !CUST_LVGL_SUPPORT
typedef uint16_t ui_color_t;
typedef void * ui_obj_t;
typedef struct {

    uint32_t cf : 5;          /*Color format: See `lv_img_color_format_t`*/
    uint32_t always_zero : 3; /*It the upper bits of the first byte. Always zero to look like a
                                 non-printable character*/

    uint32_t reserved : 2; /*Reserved to be used later*/

    uint32_t w : 11; /*Width of the image map*/
    uint32_t h : 11; /*Height of the image map*/
} ui_img_header_t;

typedef struct {
    ui_img_header_t header; /**< A header describing the basics of the image*/
    uint32_t data_size;     /**< Size of the image in bytes*/
    const uint8_t * data;   /**< Pointer to the data of the image*/
} ui_img_dsc_t;

#endif


/// 图片数据包装初始化.
#if CUST_LVGL_SUPPORT 
#define UI_IMG_DSC_PARMS_INIT(lv, w0, h0, x0, y0, alloc_flag) \
do { \
    memset((lv), 0, sizeof(ui_img_dsc_wrapper_t)); \
    (lv)->img_dsc.header.cf = UI_IMG_CF_TRUE_COLOR; \
    (lv)->img_dsc.header.w = (w0); \
    (lv)->img_dsc.header.h = (h0); \
    (lv)->img_dsc.data_size = IMG_SIZE_RGB565((w0), (h0)); \
    if (alloc_flag) { \
        (lv)->img_dsc.data = u_malloz(IMG_SIZE_RGB565((w0), (h0))); \
        if (!(lv)->img_dsc.data) { \
            LOGE("err: malloc failure."); \
        } \
    } \
    (lv)->x = (x0); \
    (lv)->y = (y0); \
} while (0)
#else
#define UI_IMG_DSC_PARMS_INIT(lv, w0, h0, x0, y0, alloc_flag) \
do { \
    memset((lv), 0, sizeof(ui_img_dsc_wrapper_t)); \
    (lv)->x = (x0); \
    (lv)->y = (y0); \
} while (0)
#endif

/// 图片数据包装.
typedef struct {
    int exit_flag;
    int res_index;
    int lang_index;
    int mode_index;
    uint16_t x;
    uint16_t y;
    ui_img_dsc_t img_dsc;
} ui_img_dsc_wrapper_t;

/// 叠图送给LVGL显示的步骤
 typedef enum {
    OVERLAY_IMG_DIS_STEP_FULL_BG,
    OVERLAY_IMG_DIS_STEP_SYNC_FB,
    OVERLAY_IMG_DIS_STEP_LVGL_SHOW,
    OVERLAY_IMG_DIS_STEP_MAX
} overlay_img_dis_step_e;

/// 获取图片资源回调.
typedef void *(*ui_get_img_res_loop_fptr)(void *user_data, int args1, int args2);
typedef struct {
    ui_get_img_res_loop_fptr get_img_res_cb;
    const char *(*get_dest_text_show)(int args1, int args2);
    void (*show_ui_text)(int *argv, int argc, const char *text);
    void (*set_bg_img_color)(int args, ui_color_t color);
    void *(*draw_qrcode_view)(void *user_data, int qr_size, int x, int y, const char *text);
    int (*animation_progress_hook)(void *user_data, int index, int total);
    uint16_t ani_loop_play;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    void *bg_img_obj;
    #if IS_SUPPORT_OVERLAY_IMG
    uint8_t overlay_img_num;
    void *overlay_img_bg_img_obj[OVERLAY_IMG_NUM_MAX];
    ui_obj_t *overlay_img_layer[OVERLAY_IMG_NUM_MAX];
    #endif
} ui_process_cb_parms_t;

/// UI绘制.
#define FULL_COORDINATE(x0, y0) do { x = x0; y = y0; } while (0)

#define FULL_UI_PROPERTY_AUTO(draw, _id, _x, _y) \
    FULL_UI_PROPERTY(draw, _id, -1, -1, _x, _y)

#define FULL_UI_PROPERTY(draw, _id, _w, _h2, _x, _y) \
    FULL_UI_PROPERTY_OFFSET(draw, _id, _w, _x, _y, _w, _h2, 0, 0)

#define FULL_UI_PROPERTY_OFFSET(draw, _id, _w, _x, _y, _w2, _h2, _x0, _y0) \
    do { \
        ui_draw_property_t *d = (draw); \
        (d)->x = (_x); \
        (d)->y = (_y); \
        (d)->w = (_w); \
        (d)->x0 = (_x0); \
        (d)->y0 = (_y0); \
        (d)->w2 = (_w2); \
        (d)->h2 = (_h2); \
        (d)->res_id = (_id); \
        (d)->full_color = (0); \
    } while (0)

#define FULL_UI_COLOR(draw, _color0, _w0, _h0, _x0, _y0) \
    do { \
        ui_draw_property_t *d = (draw); \
        (d)->x = (_x0); \
        (d)->y = (_y0); \
        (d)->w = (_w0); \
        (d)->h = (_h0); \
        (d)->color = (_color0); \
        (d)->full_color = (1); \
    } while (0)

/// 动画绘制参数.
typedef struct {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    int16_t x0;
    int16_t y0;
    int16_t w2;
    int16_t h2;
    uint16_t res_id;
    uint16_t color;
    uint8_t full_color;
} ui_draw_property_t;

/// 动画绘制参数.
#define FULL_UI_ANI_PROPERTY(_user_data, _x, _y)  \
    do {    \
        ui_process_cb_parms_t *_user_bridge = (ui_process_cb_parms_t *)_user_data; \
        _user_bridge->x = (_x); \
        _user_bridge->y = (_y); \
    }while(0)

typedef enum {
    ANI_TIMER_DRAW_BEFORE = 0, /// 背景合并之前
    ANI_TIMER_DRAW_AFTER, /// 背景合并之后
    ANI_TIMER_DRAW_FINISH, /// 退出.
} ui_ani_progress_state_e;

/// 动画绘制进度回调.
typedef void (*progress_cb_fptr)(void *user_data, int index, ui_ani_progress_state_e state);

/// 动画绘制信息.
typedef struct ui_play_info {
    const char *tag;
    void *user_data;
    const uint16_t *res_array;
    uint16_t res_count;
    int fps;
    ui_process_cb_parms_t *parms;
    progress_cb_fptr progress_cb;
} ui_play_info_t;

/// 动画绘制对象.
typedef struct ui_animation_play {
    /// private:
    char *tag;
    void *user_data;
    uint16_t *res_array;
    uint16_t res_count;
    uint16_t played_count;
    ui_img_dsc_wrapper_t img_wrap;
    #if IS_SUPPORT_OVERLAY_IMG
    ui_img_dsc_wrapper_t img_wrap_overlay[OVERLAY_IMG_NUM_MAX];
    #endif
    ui_timer_t *update_timer;
    /// public:
    void (*progress_cb)(void *user_data, int index, ui_ani_progress_state_e state);
    void (*reset_play_count_cb)(struct ui_animation_play *thiz);
} ui_animation_play_t;

/// 动画绘制合并数组长度.
#ifndef MAX_UI_DRAW_PROP
#define MAX_UI_DRAW_PROP (8)
#endif

typedef struct _ui_play_context_t {
    /// 动画绘制上下文参数.
    ui_animation_play_t *ani;
    /// 动画绘制合并数组
    ui_draw_property_t draw[MAX_UI_DRAW_PROP];
    /// 合并数组长度.
    uint16_t draw_count;
    /// 动画绘制参数.
    ui_process_cb_parms_t *parms;
    /// 获取资源缓冲区
    // res_obj_desc_t *(*get_res_buf)(get_res_buf_args_t * p_args);

    #if IS_SUPPORT_OVERLAY_IMG
    /// 叠图图片数据初始化接口
    void (*overlay_img_wrap_init_cb)(void *user_data);
    /// 叠图接口，如果某个页面需要叠图，在页面实现 overlay_img_display_cb 即可
    void (*overlay_img_display_cb)(void *user_data, overlay_img_dis_step_e step);
    /// 动画绘制合并数组
    ui_draw_property_t overlay_img_draw[OVERLAY_IMG_NUM_MAX][MAX_UI_DRAW_PROP];
    /// 合并数组长度
    uint16_t overlay_img_draw_count[OVERLAY_IMG_NUM_MAX];
    /// 是否叠图显示
    uint8_t is_display_overlay_img_mode;
    #endif
} ui_play_context_t;

typedef struct {
    /// 传入 get_res_buf使用，一般为 window_context_t
    void *user_data;
    /// 需要获取的图片资源ID号
    uint16_t res_id; 
    /// 存放图片数据 draw_buffer
    uint8_t *dest;
    /// 移动的指针，用来提高刷新率，加载不同的图片时移动 dest_addr 位置即可，不需要memcpy
    uint8_t **dest_addr;
    /// 移动的指针 dest_addr 原始地址，当在内存列表里面找不到图片时，让 dest_addr 回到原位再从Flash读取数据
    uint8_t **dest_addr_base;
} get_res_buf_args_t;

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
void ui_draw_sync_fb(void *user_data, ui_draw_property_t *draw, int length);
#if IS_SUPPORT_OVERLAY_IMG
void ui_draw_sync_fb_overlay_img(void *user_data, ui_draw_property_t *draw, int length, window_manager_context_t *wm, uint8_t layer_idx);
#endif
void full_data_to_frame_buffer_offset(uint8_t *fb, uint8_t *src, int w1, int w2, int h2, int x0, int y0, int x, int y);

#ifdef __cplusplus
}
#endif

#endif 


