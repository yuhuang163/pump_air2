#ifndef __WINDOW_SCREEN_LOGO_H__
#define __WINDOW_SCREEN_LOGO_H__

#include "peripheral_bus_api.h"
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep
#include "windows_manager.h" // IWYU pragma: keep
#include "windows_internal_api.h" // IWYU pragma: keep
#include "input_key_code.h" // IWYU pragma: keep

typedef struct ui_proc_logo {
    uint32_t ani_timer_cnt;
    window_context_t w_ctx;
    // ui_page_handle_t bridge;
    uint32_t margic; /// 窗口标识.
} ui_proc_logo_t;

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
void start_logo_ui_animation(ui_proc_logo_t *proc);

#ifdef __cplusplus
}
#endif

#endif 


