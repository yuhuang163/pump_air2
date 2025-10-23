#ifndef __WINDOW_SCREEN_PUMP_H__
#define __WINDOW_SCREEN_PUMP_H__

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

typedef enum {
    HDR_ACTION_START_PUMP = 100,
    HDR_ACTION_CHANGE_PUMP_STATUS,
    HDR_ACTION_LOOP_SHOW,
    HDR_ACTION_SHOW_ONCE,
    HDR_ACTION_REGISTER_KEYEVENT,
    HDR_ACTION_SHOW_THEMES_ANI,
    HDR_ACTION_RECOVER_DEFAULT_ANI,
    HDR_ACTION_RESET_PLAY_ANI_FPS,
    HDR_ACTION_PROGRESS_START,
    HDR_ACTION_PROGRESS_STOP,
    HDR_ACTION_PUMP_COMPLETE,
    HDR_ACTION_GOTO_CHARGE_UI,
    HDR_ACTION_MAX
} pump_ui_hdr_action_e;

typedef enum {
    LOOP_SHOW_SWITCH_MODE = 0,
    LOOP_SHOW_PUMPING,
    LOOP_SHOW_DENTAL_MOLD,
    LOOP_SHOW_RESULT_RISK_WARN,
    LOOP_SHOW_RESULT_RISK_COMPARE,
    LOOP_SHOW_THEMES_ANI,
    LOOP_SHOW_SETTING_ENTER,
    LOOP_SHOW_MAX
} pump_loop_show_type_e;

/// 泵奶模式切换超时时间
#define PUMP_MODE_CHANGE_TIMEOUT (2000)

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif 


