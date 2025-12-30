#ifndef __WINDOW_SCREEN_FACTORY_RESET_H__
#define __WINDOW_SCREEN_FACTORY_RESET_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep
#include "windows_manager.h" // IWYU pragma: keep
#include "windows_map_define.h" // IWYU pragma: keep
#include "input_key_code.h" // IWYU pragma: keep


#define FACTORY_RESET_ANIMATION_FREQ                 (25)

typedef struct ui_proc_factory_reset {
    uint32_t ani_timer_cnt;
    window_context_t w_ctx;
    window_manager_context_t *wm;
    ui_manager_context_t *ui;
    power_manager_context_t *pm;
    uint32_t margic; /// 窗口标识.
} ui_proc_factory_reset_t;

/// 工厂重置动画展示时间
#define FACTORY_RESET_ANIMATION_DISPLAY_TIME      (2000) /// 2s

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
void start_factory_reset_ui_animation(ui_proc_factory_reset_t *proc);
void stop_factory_reset_ui_animation(ui_proc_factory_reset_t *proc);

#ifdef __cplusplus
}
#endif

#endif 


