#ifndef __UI_DISPLAY_H__
#define __WINDOWS_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "ui_manager_api.h" // IWYU pragma: keep

#ifndef LVGL_TIME_TICK_PERIOD
#define LVGL_TIME_TICK_PERIOD (10)
#endif

#define LVGL_HDR_TASK_PERIOD (16)

#ifdef __cplusplus
}
#endif


/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
void register_ui_display(ui_manager_context_t *thiz);
#endif// __UI_DISPLAY_H__

