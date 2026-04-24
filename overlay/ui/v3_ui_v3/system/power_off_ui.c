/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define PROGRESS_ARC_SIZE                       (28)    /* 直径（原80缩小20%） */
#define PROGRESS_ARC_WIDTH                      (3)    /* 圆弧线宽（原4加粗50%） */
#define PROGRESS_ARC_BG_COLOR                   (0xFFFFFF)    /* 背景圆弧颜色：黑色 */
#define PROGRESS_ARC_BG_OPA                     (38)        /* 背景圆弧透明度 15%（255*15/100≈38） */
#define PROGRESS_ARC_COLOR                      (0xB25962)    /* 圆弧颜色：粉色 */
#define PROGRESS_ARC_LOOP_MS_PER_DEG            (3)    /* ms per degree，越小转得越快（原15约5.4s/圈，现约2.2s/圈） */
#define PROGRESS_ARC_ANGLE_SPAN                 (90)   /* 25% of 360 */
#define PROGRESS_ARC_TEXT_COLOR                 (0x767676)    /* 圆弧颜色：粉色 */

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
typedef struct {
    uint32_t start_time;
    uint8_t force_update;
    uint8_t rsv[15];
} ui_power_off_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static ui_power_off_info_t *ui_info;

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void ui_power_off_root_init(void)
{

}

static void ui_power_off_update(ui_manager_context_t *thiz)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_power_off_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (ui_info == NULL) {
    //     ui_info = (ui_power_off_info_t *)u_malloz(sizeof(ui_power_off_info_t));
    //     EXCEPTION_CHECK_VOID(ui_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // ui_power_off_root_init();
    // ui_info->force_update = true;
}

void ui_display_power_off_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // EXCEPTION_CHECK_VOID(ui_info);
    // LVGL_OBJ_DELETE(ui_info->root);
    // u_free(ui_info);
    // ui_info = NULL;
}

int ui_display_power_off_process(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_INT(thiz);
    // EXCEPTION_CHECK_INT(ui_info);
    // EXCEPTION_CHECK_INT(ui_info->root);
    // ui_power_off_update(thiz);
    // ui_info->force_update = false;
    // thiz->priv.fps_count++;
    // if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
    //     thiz->priv.fps_start_time = get_timestamp();
    //     LOG_UI("===> power off fps: %d", thiz->priv.fps_count);
    //     thiz->priv.fps_count = 0;
    // }
    return 0;
}
