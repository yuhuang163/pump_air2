/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h"  // IWYU pragma: keep
#include "ui_time_warpper.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static bool timer_created = 0;
static bool timer_deleted = 0;
static bool ui_timer_run = false;
static uint8_t ui_timer_idle_last = 0;   

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
ui_timer_t * ui_timer_create(ui_timer_cb_t timer_xcb, uint32_t period, void * user_data)
{
    ui_timer_t * new_timer = NULL;
    new_timer = u_malloz(sizeof(ui_timer_t));
    if(new_timer == NULL) {
        LOGE("Oops! u_malloz: fail %s", "new_timer");
        return NULL;
    }
    new_timer->period = period;
    new_timer->timer_cb = timer_xcb;
    new_timer->repeat_count = -1;
    new_timer->paused = 0;
    new_timer->last_run = get_timestamp();
    new_timer->user_data = user_data;
    timer_created = true;

    return new_timer;
}

void ui_timer_set_cb(ui_timer_t * timer, ui_timer_cb_t timer_cb)
{
    timer->timer_cb = timer_cb;
}

void ui_timer_del(ui_timer_t * timer)
{
    timer_deleted = true;
    u_free(timer);
}

void ui_timer_pause(ui_timer_t * timer)
{
    timer->paused = true;
}

void ui_timer_resume(ui_timer_t * timer)
{
    timer->paused = false;
}
void ui_timer_set_period(ui_timer_t * timer, uint32_t period)
{
    timer->period = period;
}

void ui_timer_ready(ui_timer_t * timer)
{
    timer->last_run = get_timestamp() - timer->period - 1;
}

void ui_timer_set_repeat_count(ui_timer_t * timer, int32_t repeat_count)
{
    timer->repeat_count = repeat_count;
}

void ui_timer_reset(ui_timer_t * timer)
{
    timer->last_run = get_timestamp();
}

void ui_timer_enable(bool en)
{
    ui_timer_run = en;
}

uint8_t ui_timer_get_idle(void)
{
    return ui_timer_idle_last;
}

ui_timer_t * ui_timer_get_next(ui_timer_t * timer)
{
    return NULL;
}

