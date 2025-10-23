#ifndef __UI_TIME_WARPPER_H__
#define __UI_TIME_WARPPER_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep


struct _ui_timer_t;
typedef void (*ui_timer_cb_t)(struct _ui_timer_t *);

typedef struct _ui_timer_t {
    uint32_t period; /**< How often the timer should run*/
    uint32_t last_run; /**< Last time the timer ran*/
    ui_timer_cb_t timer_cb; /**< Timer function*/
    void * user_data; /**< Custom user data*/
    int32_t repeat_count; /**< 1: One time;  -1 : infinity;  n>0: residual times*/
    uint32_t paused : 1;
} ui_timer_t;

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
ui_timer_t * ui_timer_create(ui_timer_cb_t timer_xcb, uint32_t period, void * user_data);
void ui_timer_set_cb(ui_timer_t * timer, ui_timer_cb_t timer_cb);
void ui_timer_del(ui_timer_t * timer);
void ui_timer_pause(ui_timer_t * timer);
void ui_timer_resume(ui_timer_t * timer);
void ui_timer_set_period(ui_timer_t * timer, uint32_t period);
void ui_timer_ready(ui_timer_t * timer);
void ui_timer_set_repeat_count(ui_timer_t * timer, int32_t repeat_count);
void ui_timer_reset(ui_timer_t * timer);
void ui_timer_enable(bool en);
uint8_t ui_timer_get_idle(void);
ui_timer_t * ui_timer_get_next(ui_timer_t * timer);

#ifdef __cplusplus
}
#endif

#endif 


