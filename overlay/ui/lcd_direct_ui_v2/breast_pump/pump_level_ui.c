/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h"
#include "ui_img_define.h"
#include "ui_page_define.h"
#include "ui_manager.h"
#include "window_screen_pump.h"
#include "breast_pump_ui.h"
#include <stdint.h>

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_LEVEL_COMP_BG                        (0)
#define IS_SUPPORT_PUMP_LEVEL_COMP_BG                   (0)

// 颜色定义 - 优化渐变起始/结束色，让渐变更自然
#define PUMP_LEVEL_ARC_UNSEL_COLOR                      0xF7EFEF
#define PUMP_LEVEL_LINE_COLOR                           0xD9D9D9
#define PUMP_LEVEL_TEXT_COLOR                           0x393535
#define PUMP_LEVEL_VALUE_COLOR                          0x1C1C20
#define PUMP_LEVEL_LABLE_UNSEL_COLOR                    0xF7EFEF
#define PUMP_LEVEL_LABLE_SEL_COLOR                      0xB3686D
#define PUMP_LEVEL_TEXT_SEL_COLOR                       0xB3686D
#define PUMP_LEVEL_TEXT_UNSEL_COLOR                     0xFFFFFF

// 等级指示器配置
#define INDICATOR_SIZE                                  308
#define RING_WIDTH                                      36
#define TOTAL_LEVEL                                     15      // 总等级
#define GRADIENT_SEGMENT_COUNT                           30     // 渐变段数量（360/12=30）
#define GRADIENT_SEGMENT_ANGLE                          12      // 每个渐变段的角度
#define ARC_START_ANGLE                                270     // 圆弧起始角度（正上方）
#define ARC_TOTAL_ANGLE                                360     // 圆弧总角度（整圆）
#define ARC_END_ANGLE                                  (ARC_START_ANGLE + ARC_TOTAL_ANGLE)
#define ARC_SIZE                                        (360-40) // 圆弧尺寸（之前使用的值）

// 渐变颜色配置 - 定义完整的渐变色谱（从浅粉到深粉）
#define GRADIENT_COLOR_START                            0xF0989B  // 起始色（浅粉）
#define GRADIENT_COLOR_MID1                             0xEA8B8F  // 中间色1
#define GRADIENT_COLOR_MID2                             0xE48186  // 中间色2
#define GRADIENT_COLOR_MID3                             0xD77A83  // 中间色3
#define GRADIENT_COLOR_MID4                             0xCA6E78  // 中间色4
#define GRADIENT_COLOR_END                              0xB25962  // 结束色（深粉）

// 异常检查宏
#ifndef EXCEPTION_CHECK_VOID
#define EXCEPTION_CHECK_VOID(expr) do { if (!(expr)) { return; } } while(0)
#endif

#ifndef EXCEPTION_CHECK_INT
#define EXCEPTION_CHECK_INT(expr) do { if (!(expr)) { return -1; } } while(0)
#endif

#ifndef LVGL_OBJ_DELETE
#define LVGL_OBJ_DELETE(obj) do { if (obj && lv_obj_is_valid(obj)) { lv_obj_del(obj); obj = NULL; } } while(0)
#endif

typedef struct {
    lv_obj_t * root;
    lv_obj_t * background;
    res_img_info_t background_info;
    #if LV_USE_SNAPSHOT
    lv_draw_buf_t *background_snapshot;
    #endif
    lv_obj_t * arc_progress;               // 背景圆弧
    lv_obj_t * arc_progress_segments[GRADIENT_SEGMENT_COUNT];  // 渐变圆弧段


    uint8_t force_update;
    uint8_t left_level;
    uint8_t right_level;
    uint8_t level_change;
    uint8_t control_type;
} level_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_LEVEL_COMP_BG
static PSRAM_DATA_LOC uint8_t setting_bg_data[] = {
    #include "png2txt/pump_level_bg_300x300.txt"
};
#endif

static level_display_info_t *level_info;

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
/**
 * @brief 基础颜色插值函数
 * @param color1 起始颜色
 * @param color2 结束颜色
 * @param ratio 插值比例（0.0-1.0）
 * @return 插值后的颜色值
 */
 static uint32_t color_interpolate(uint32_t color1, uint32_t color2, float ratio)
 {
     if (ratio < 0.0f) ratio = 0.0f;
     if (ratio > 1.0f) ratio = 1.0f;
     
     // 提取RGB分量
     uint8_t r1 = (color1 >> 16) & 0xFF;
     uint8_t g1 = (color1 >> 8) & 0xFF;
     uint8_t b1 = color1 & 0xFF;
     
     uint8_t r2 = (color2 >> 16) & 0xFF;
     uint8_t g2 = (color2 >> 8) & 0xFF;
     uint8_t b2 = color2 & 0xFF;
     
     // 线性插值
     uint8_t r = (uint8_t)(r1 + (r2 - r1) * ratio);
     uint8_t g = (uint8_t)(g1 + (g2 - g1) * ratio);
     uint8_t b = (uint8_t)(b1 + (b2 - b1) * ratio);
     
     return (r << 16) | (g << 8) | b;
 }
 
/**
 * @brief 增强版颜色插值函数，支持多节点渐变
 * @param ratio 插值比例（0.0-1.0）
 * @return 插值后的RGB颜色值
 */
static uint32_t gradient_color_calculate(float ratio)
{
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    uint32_t color;
    // 五段式渐变：0-0.2, 0.2-0.4, 0.4-0.6, 0.6-0.8, 0.8-1.0
    if (ratio <= 0.2f) {
        // 起始色 -> 中间色1
        float seg_ratio = ratio / 0.2f;
        color = color_interpolate(GRADIENT_COLOR_START, GRADIENT_COLOR_MID1, seg_ratio);
    } else if (ratio <= 0.4f) {
        // 中间色1 -> 中间色2
        float seg_ratio = (ratio - 0.2f) / 0.2f;
        color = color_interpolate(GRADIENT_COLOR_MID1, GRADIENT_COLOR_MID2, seg_ratio);
    } else if (ratio <= 0.6f) {
        // 中间色2 -> 中间色3
        float seg_ratio = (ratio - 0.4f) / 0.2f;
        color = color_interpolate(GRADIENT_COLOR_MID2, GRADIENT_COLOR_MID3, seg_ratio);
    } else if (ratio <= 0.8f) {
        // 中间色3 -> 中间色4
        float seg_ratio = (ratio - 0.6f) / 0.2f;
        color = color_interpolate(GRADIENT_COLOR_MID3, GRADIENT_COLOR_MID4, seg_ratio);
    } else {
        // 中间色4 -> 结束色
        float seg_ratio = (ratio - 0.8f) / 0.2f;
        color = color_interpolate(GRADIENT_COLOR_MID4, GRADIENT_COLOR_END, seg_ratio);
    }

    return color;
}

static void pump_level_root_init(void)
{
    if (level_info->root == NULL || !lv_obj_is_valid(level_info->root)) {
        level_info->root = lv_obj_create(lv_screen_active());
        lv_obj_set_size(level_info->root, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_bg_color(level_info->root, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(level_info->root, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(level_info->root, 0, 0);
        lv_obj_set_style_pad_all(level_info->root, 0, 0);
        lv_obj_align(level_info->root, LV_ALIGN_CENTER, 0, 0);
        lv_obj_clear_flag(level_info->root, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    }
}

#if IS_SUPPORT_PUMP_LEVEL_COMP_BG
static void pump_level_root_bg_update(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(level_info);
    EXCEPTION_CHECK_VOID(level_info->root);
    
    static uint32_t start_time = 0;
    static uint8_t status = 0;
    if (level_info->background == NULL || !lv_obj_is_valid(level_info->background)) {
        level_info->background = lv_image_create(level_info->root);
        EXCEPTION_CHECK_VOID(level_info->background);
    }
    
    if (status == 0) {
        lv_obj_add_flag(level_info->background, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(level_info->background, LV_OBJ_FLAG_HIDDEN);
        lv_fs_path_ex_t bg_img_path;
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, setting_bg_data,
            sizeof(setting_bg_data), "png");
        lv_image_set_src(level_info->background, (const void *)&bg_img_path);
        lv_obj_align(level_info->background, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}
#else
static void pump_level_root_bg_update(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(level_info);
    EXCEPTION_CHECK_VOID(level_info->root);
    
    if (level_info->force_update == false) {
        return;
    }

    lv_fs_path_ex_t bg_img_path;
    if (level_info->background == NULL || !lv_obj_is_valid(level_info->background)) {
        level_info->background = lv_image_create(level_info->root);
        EXCEPTION_CHECK_VOID(level_info->background);
        
#if LV_USE_SNAPSHOT
        if (level_info->background_snapshot != NULL) {
            lv_draw_buf_destroy(level_info->background_snapshot);
            level_info->background_snapshot = NULL;
        }
#endif
        
        common_ui_img_resource_init(thiz, RESOURCE_ID_PUMP_LEVEL_BACKGROUND_ICON, &level_info->background_info);
        EXCEPTION_CHECK_VOID(level_info->background_info.data);
        
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, 
            level_info->background_info.data, level_info->background_info.size, "png");
        lv_image_set_src(level_info->background, (const void *)&bg_img_path);
        lv_obj_align(level_info->background, LV_ALIGN_CENTER, 4, 4);
        
#if LV_USE_SNAPSHOT
        lv_refr_now(NULL);
        level_info->background_snapshot = lv_snapshot_take(level_info->background, LV_COLOR_FORMAT_NATIVE_WITH_ALPHA);
        if (level_info->background_snapshot == NULL) {
            LOGE("Failed to create background snapshot");
        }
#endif
    }

#if LV_USE_SNAPSHOT
    if (level_info->background_snapshot != NULL && level_info->background != NULL) {
        lv_image_set_src(level_info->background, level_info->background_snapshot);
    }
#endif
}
#endif

/**
 * @brief 创建等级指示器（优化渐变段创建逻辑）
 */
static void pump_level_indicator_create(ui_manager_context_t *thiz, lv_obj_t *parent)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(level_info);
    EXCEPTION_CHECK_VOID(parent);

    // 创建背景圆弧
    if (level_info->arc_progress == NULL || !lv_obj_is_valid(level_info->arc_progress)) {
        level_info->arc_progress = lv_arc_create(parent);
        EXCEPTION_CHECK_VOID(level_info->arc_progress);
        
        lv_obj_set_size(level_info->arc_progress, ARC_SIZE, ARC_SIZE);
        lv_obj_center(level_info->arc_progress);
        
        // 设置背景圆弧样式
        lv_arc_set_bg_angles(level_info->arc_progress, ARC_START_ANGLE, ARC_END_ANGLE);
        lv_arc_set_angles(level_info->arc_progress, ARC_START_ANGLE, ARC_START_ANGLE);
        lv_obj_set_style_arc_color(level_info->arc_progress, 
            lv_color_hex(PUMP_LEVEL_ARC_UNSEL_COLOR), LV_PART_MAIN);
        lv_obj_set_style_arc_width(level_info->arc_progress, RING_WIDTH, LV_PART_MAIN);
        lv_obj_set_style_arc_opa(level_info->arc_progress, LV_OPA_TRANSP, LV_PART_INDICATOR);
        lv_obj_remove_style(level_info->arc_progress, NULL, LV_PART_KNOB);
    }
    
    // 创建30个渐变圆弧段（优化颜色计算）
    for (uint8_t i = 0; i < GRADIENT_SEGMENT_COUNT; i++) {
        if (level_info->arc_progress_segments[i] == NULL || !lv_obj_is_valid(level_info->arc_progress_segments[i])) {
            level_info->arc_progress_segments[i] = lv_arc_create(parent);
            EXCEPTION_CHECK_VOID(level_info->arc_progress_segments[i]);
            
            lv_obj_set_size(level_info->arc_progress_segments[i], ARC_SIZE, ARC_SIZE);
            lv_obj_center(level_info->arc_progress_segments[i]);
            
            // 计算当前段的角度范围
            uint16_t seg_start = ARC_START_ANGLE + i * GRADIENT_SEGMENT_ANGLE;
            uint16_t seg_end = seg_start + GRADIENT_SEGMENT_ANGLE;
            
            // 初始化角度（默认不显示）
            lv_arc_set_bg_angles(level_info->arc_progress_segments[i], ARC_START_ANGLE, ARC_END_ANGLE);
            lv_arc_set_angles(level_info->arc_progress_segments[i], seg_start, seg_start);
            
            // 计算当前段的渐变颜色（基于段的位置）
            float segment_ratio = (float)i / (float)GRADIENT_SEGMENT_COUNT;
            uint32_t seg_color = gradient_color_calculate(segment_ratio);
            
            // 设置圆弧样式
            lv_obj_set_style_arc_opa(level_info->arc_progress_segments[i], LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_arc_color(level_info->arc_progress_segments[i], 
                lv_color_hex(seg_color), LV_PART_INDICATOR);
            lv_obj_set_style_arc_width(level_info->arc_progress_segments[i], RING_WIDTH, LV_PART_INDICATOR);
            lv_obj_remove_style(level_info->arc_progress_segments[i], NULL, LV_PART_KNOB);
        }
    }
    

}

/**
 * @brief 更新等级指示器进度（重点优化level=15时的渐变展示）
 */
static void pump_level_arc_progress_update(ui_manager_context_t *thiz, uint8_t level)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(level_info);
    EXCEPTION_CHECK_VOID(level_info->root);
    
    // 创建指示器
    if (level_info->arc_progress == NULL || !lv_obj_is_valid(level_info->arc_progress)) {
        pump_level_indicator_create(thiz, level_info->root);
    }
    
    EXCEPTION_CHECK_VOID(level_info->arc_progress);
    
    // 边界保护
    uint8_t display_level = (level > TOTAL_LEVEL) ? TOTAL_LEVEL : level;

    
    // 计算进度结束角度
    uint16_t end_angle;
    if (display_level == 0) {
        end_angle = ARC_START_ANGLE;  // 0级时不显示
    } else if (display_level == TOTAL_LEVEL) {
        end_angle = ARC_END_ANGLE;    // 15级时显示完整圆环
    } else {
        // 线性映射：0-15级对应270-630度
        end_angle = ARC_START_ANGLE + (uint16_t)((float)ARC_TOTAL_ANGLE * display_level / TOTAL_LEVEL);
    }
    
    // 更新所有渐变段的显示
    for (uint8_t i = 0; i < GRADIENT_SEGMENT_COUNT; i++) {
        EXCEPTION_CHECK_VOID(level_info->arc_progress_segments[i]);
        
        uint16_t seg_start = ARC_START_ANGLE + i * GRADIENT_SEGMENT_ANGLE;
        uint16_t seg_end = seg_start + GRADIENT_SEGMENT_ANGLE;
        
        // 根据当前等级更新段的显示范围
        if (end_angle <= seg_start) {
            // 未达到该段，不显示
            lv_arc_set_angles(level_info->arc_progress_segments[i], seg_start, seg_start);
        } else if (end_angle >= seg_end) {
            // 完全覆盖该段，显示完整段
            lv_arc_set_angles(level_info->arc_progress_segments[i], seg_start, seg_end);
        } else {
            // 部分覆盖该段，显示到结束角度
            lv_arc_set_angles(level_info->arc_progress_segments[i], seg_start, end_angle);
        }
        
        // 【关键优化】level=15时，强制更新所有段的颜色（确保渐变完整）
        if (display_level == TOTAL_LEVEL) {
            float segment_ratio = (float)i / (float)GRADIENT_SEGMENT_COUNT;
            uint32_t seg_color = gradient_color_calculate(segment_ratio);
            lv_obj_set_style_arc_color(level_info->arc_progress_segments[i], 
                lv_color_hex(seg_color), LV_PART_INDICATOR);
        }
    }
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_level_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (level_info == NULL) {
        level_info = (level_display_info_t *)u_malloz(sizeof(level_display_info_t));
        EXCEPTION_CHECK_VOID(level_info);
        
        // 初始化所有指针为NULL
        memset(level_info, 0, sizeof(level_display_info_t));
    }
    
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    pump_level_root_init();
    level_info->force_update = true;
}

void ui_display_pump_level_remove(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (level_info == NULL) {
        return;
    }
    
#if LV_USE_SNAPSHOT
    if (level_info->background_snapshot != NULL) {
        lv_draw_buf_destroy(level_info->background_snapshot);
        level_info->background_snapshot = NULL;
    }
#endif
    
    common_ui_img_resource_deinit(thiz, &level_info->background_info);
    
    // 安全删除所有LVGL对象
    LVGL_OBJ_DELETE(level_info->background);
    LVGL_OBJ_DELETE(level_info->arc_progress);
    
    for (uint8_t i = 0; i < GRADIENT_SEGMENT_COUNT; i++) {
        LVGL_OBJ_DELETE(level_info->arc_progress_segments[i]);
    }
    
    LVGL_OBJ_DELETE(level_info->root);
    
    u_free(level_info);
    level_info = NULL;
}

int ui_display_pump_level_process(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(level_info);
    EXCEPTION_CHECK_INT(level_info->root);
    
    ui_proc_pump_t *proc = (ui_proc_pump_t *)thiz->priv.play_info.user_data;
    EXCEPTION_CHECK_INT(proc);
    EXCEPTION_CHECK_INT(proc->pump);
    EXCEPTION_CHECK_INT(proc->motor);
    
    uint8_t control_type = proc->pump->get_breast_pump_control_type();
    pump_setting_t setting;
    uint8_t time_index = proc->pump->get_breast_pump_step_index();
    proc->pump->get_breast_pump_setting(&setting, time_index >= ROOT_BREAST_PUMP_STEP_MAX ? 0 : time_index);

    level_info->left_level = setting.level[0];
    level_info->right_level = setting.level[1];
    level_info->control_type = control_type;
    level_info->level_change = 10;  // 测试值，实际应从业务逻辑获取
    
    // 根据控制类型计算当前显示等级
    uint8_t display_level;
    if (control_type == BREAST_PUMP_CONTROL_TYPE_BILATERAL) {
        display_level = level_info->level_change;
    } else {
        display_level = (control_type == BREAST_PUMP_CONTROL_TYPE_LEFT) ? 
                        level_info->left_level : level_info->right_level;
    }
    
    // 更新UI
    pump_level_root_bg_update(thiz);
    pump_level_arc_progress_update(thiz, 15);  // 使用实际计算的等级
    
    level_info->force_update = false;
    
    // FPS统计
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        LOG_UI("===> pump level fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_start_time = get_timestamp();
        thiz->priv.fps_count = 0;
    }
    
    return 0;
}