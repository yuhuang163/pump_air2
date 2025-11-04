/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static bool timer_created = false;
static bool timer_deleted = false;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void ui_update_cb(ui_timer_t *tmr)
{   
    ui_play_context_t *ui_ctx = tmr->user_data;
    ui_animation_play_t *ani = ui_ctx->ani;
    ui_process_cb_parms_t *parms = ui_ctx->parms;
    ui_draw_property_t *draw = ui_ctx->draw;

    #if DEBUG_ANI_FPS
    IS_FILTER(FILTER_UI) {
        LOG_FPS(play->tag, LOG_UI);
    }
    #endif

    ui_play_animation_lock(NULL);

    if (ani != NULL && ani->update_timer != NULL) {
        if (ani->played_count < ani->res_count) {
            /// bg资源数据
            ui_ctx->draw_count = 0;
            FULL_UI_PROPERTY_AUTO(&draw[ui_ctx->draw_count++], ani->res_array[ani->played_count], parms->x, parms->y);
            /// 叠图接口，如果某个页面需要叠图，在页面实现 overlay_img_display_cb 即可
            if (ui_ctx->overlay_img_display_cb) {
                ui_ctx->overlay_img_display_cb(ui_ctx, OVERLAY_IMG_DIS_STEP_FULL_BG);
            }
            /// 合并背景之前
            if (ani->progress_cb) {
                ani->progress_cb(parms, ani->played_count, ANI_TIMER_DRAW_BEFORE);
            }
            /// 合并叠图之后
            if (ui_ctx->overlay_img_display_cb) {
                ui_ctx->overlay_img_display_cb(ui_ctx, OVERLAY_IMG_DIS_STEP_SYNC_FB);
            }
            /// 合并到显示缓冲区
            // ui_draw_sync_fb(parms, draw, (ui_ctx->draw_count));
            /// 合并背景之后,提供直接更新显示缓冲区的接口.
            if (ani->progress_cb) {
                ani->progress_cb(parms, ani->played_count, ANI_TIMER_DRAW_AFTER);
            }
            /// 送显.
            if (ui_ctx->overlay_img_display_cb) {
                ui_ctx->overlay_img_display_cb(ui_ctx, OVERLAY_IMG_DIS_STEP_LVGL_SHOW);
            }
            /// 送显lvgl图片
            if (parms->bg_img_obj) { 
                ui_img_set_src(parms->bg_img_obj, &ani->img_wrap.img_dsc);
                ui_obj_set_pos(parms->bg_img_obj, 0, 0);
                ui_img_set_angle(parms->bg_img_obj, 0); 
            }
            /// 如果定义了它则交给UI接管.
            if (parms->animation_progress_hook) {
                ani->played_count = parms->animation_progress_hook(parms, ani->played_count, ani->res_count);
                if (ani->played_count < 0) {
                    LOG_UI("(%s)animation exit by user.", ani->tag);
                    goto exit;
                }
            } else {
                ani->played_count++;
            }
            /// 循环播放.
            if (parms->ani_loop_play && (ani->played_count == ani->res_count)) {
                ani->played_count = 0;
            }
        } else {
        exit:
            LOG_UI("(%s)animation FINISH.", ani->tag);
            ui_timer_pause(ani->update_timer);
            /// 退出动画
            if (ani->progress_cb) {
                ani->progress_cb(parms, 0, ANI_TIMER_DRAW_FINISH);
            }
        }

        #if IS_SUPPORT_ASYNC_RESET_UI_PLAY_INFO
        if (g_is_need_reset) {
            if (ui_is_reset_play_info_valid(s_play_info)) {
                LOG_UI("(%s)animation timer reset.", s_play_info.tag);
                ui_timer_pause(ani->update_timer);
                ui_animation_play_init(ani, s_play_info);
                ui_timer_set_period(ani->update_timer, (1000 / s_play_info.fps));
                ui_timer_resume(ani->update_timer);
            }
            g_is_need_reset = false;
        }
        #endif
    }
    ui_play_animation_unlock(NULL);
}

static int ui_play_animation(ui_play_info_t play_info)
{
    ui_play_context_t *ui_ctx = (ui_play_context_t *)play_info.user_data;
    ui_animation_play_t *ani = ui_ctx->ani;
    ui_process_cb_parms_t *parms = ui_ctx->parms;
    int reset_ani = 0;
    ui_animation_play_init(ani, play_info);
    parms->w = parms->w >= PANLE_DISPLAY_W ? PANLE_DISPLAY_W : parms->w;
    parms->h = parms->h >= PANLE_DISPLAY_H ? PANLE_DISPLAY_H : parms->h;
    if (reset_ani == 1) {
        LOG_UI("(%s)ui_timer_resume", play_info.tag);
        ui_timer_resume(ani->update_timer);
    } else {
        // 充满整个画布.
        UI_IMG_DSC_PARMS_INIT(&ani->img_wrap, PANLE_DISPLAY_W, PANLE_DISPLAY_H, 0, 0, 0);
        #if CUST_LVGL_SUPPORT
        ani->img_wrap.img_dsc.data = parms->wm->frame_buffer;
        #endif
        ///  是叠图图片数据初始化接口
        #if IS_SUPPORT_OVERLAY_IMG
        if (ui_ctx->overlay_img_wrap_init_cb) {
            ui_ctx->overlay_img_wrap_init_cb(ui_ctx);
        }
        #endif

        ani->update_timer = ui_timer_create(ui_update_cb, (1000 /play_info.fps),
            ui_ctx);
        ui_timer_ready(ani->update_timer);
    }
    ui_play_animation_unlock("ui_play_animation");
    return 0;
}

int ui_reset_animation(ui_play_info_t play_info)
{
    ui_play_context_t *ui_ctx = (ui_play_context_t *)play_info.user_data;
    ui_animation_play_t *ani = ui_ctx->ani;
    ui_play_animation_lock("ui_reset_animation");
    if ((ani == NULL) || (ani->update_timer == NULL)) {
        LOGE("ani error");
        ui_play_animation_unlock("ui_reset_animation");
        return -1;
    }
    LOG_UI("(%s)animation timer pause", ani->tag);
    ui_timer_pause(ani->update_timer);
    msleep(1 * 1000 /play_info.fps);
    ui_animation_play_init(ani, play_info);
    LOG_UI("(%s)animation timer ready", ani->tag);
    ui_timer_set_period(ani->update_timer, (1000 /play_info.fps));
    ui_timer_resume(ani->update_timer);
    ui_play_animation_unlock("ui_reset_animation");

    return 0;
}

void ui_stop_animation_if_need(void *user_data)
{
    ui_play_context_t *ui_ctx = (ui_play_context_t *)user_data;
    ui_animation_play_t *ani = ui_ctx->ani;

    ui_play_animation_lock("ui_stop_animation_if_need");
    if (ani && ani->update_timer) {
        ui_timer_del(ani->update_timer);
        LOG_UI("(%s)animation stoped.", ani->tag);
        ani->update_timer = NULL;
    }

    if (ani) {
        LOG_UI("(%s)animation u_free: %p", ani->tag, ani);
        u_free(ani);
        ui_ctx->ani = NULL;
    }
    ui_play_animation_unlock("ui_stop_animation_if_need");
}
/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
