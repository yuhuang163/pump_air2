#ifndef __WINDOW_SCREEN_DUMMY_H__
#define __WINDOW_SCREEN_DUMMY_H__

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

typedef enum {
    HDR_ACTION_NIGHT_LIGHT_ONOFF = 100,
    HDR_ACTION_NIGHT_LIGHT_LEVEL,
    HDR_ACTION_NIGHT_LIGHT_ADAPTIVE,
    HDR_ACTION_MAX
} dummy_ui_hdr_action_e;

typedef struct {
    window_context_t w_ctx;
    broadcast_obj_t receiver;
} ui_proc_dummy_t;

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
int switch_dummy_night_light_onoff_ui(ui_proc_dummy_t *proc);

#ifdef __cplusplus
}
#endif

#endif 


