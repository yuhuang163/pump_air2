/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "product_tests_ui.h" // IWYU pragma: keep
#include "window_screen_product_test.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#if (ROOT_PRODUCT_ID == ROOT_PRODUCT_V3PRO_ID)
#define IS_SUPPORT_BATTERY_PERCENT                                   1
#else
#define IS_SUPPORT_BATTERY_PERCENT                                   0
#endif

#define AGING_TEST_TEXT_BLUE_COLOR                            (0x01D65A)    // 文本颜色蓝色
#define AGING_TEST_TEXT_RED_COLOR                             (0xFF0000)    // 文本颜色红色


#define AGING_TEST_MAC_NAME_X                                (80)
#define AGING_TEST_MAC_NAME_Y                                (80)
#define AGING_TEST_AGING_INFO_X                              (AGING_TEST_MAC_NAME_X)
#define AGING_TEST_AGING_INFO_Y                              (AGING_TEST_MAC_NAME_Y + 36)
#define AGING_TEST_BATTERY_PERCENT_X                         (AGING_TEST_MAC_NAME_X )
#define AGING_TEST_BATTERY_PERCENT_Y                         (AGING_TEST_MAC_NAME_Y + 36)

typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
} aging_tests_content_info_t;

typedef struct {  
    uint8_t force_update;
    uint8_t mac_addr[6];
    bool all_color_change;
    uint8_t rsv[16];
} aging_test_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/


static aging_test_display_info_t *tests_info = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void aging_test_ui_root_init(void)
{

}

static void aging_test_ui_battery_percent_update(ui_manager_context_t *thiz,  uint16_t battery_percent, uint8_t is_red)
{

}
/// 设置背景颜色变化
static void aging_test_ui_background_color_update(ui_manager_context_t *thiz, uint8_t black_bg)
{

}


static void aging_test_ui_mac_name_update(ui_manager_context_t *thiz, uint8_t *mac_addr, uint8_t is_red)
{

}

static void aging_test_ui_aging_info_update(ui_manager_context_t *thiz, ui_proc_product_test_t *proc, uint8_t is_red)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_aging_test_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (tests_info == NULL) {
    //     tests_info = (aging_test_display_info_t *)u_malloz(sizeof(aging_test_display_info_t));
    //     EXCEPTION_CHECK_VOID(tests_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // tests_info->all_color_change = false;
    // aging_test_ui_root_init();
    // tests_info->force_update = true;
}

void ui_display_aging_test_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // EXCEPTION_CHECK_VOID(tests_info);
    // LVGL_OBJ_DELETE(tests_info->root);
    // u_free(tests_info);
    // tests_info = NULL;
}

int ui_display_aging_test_process(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_INT(thiz);
//     EXCEPTION_CHECK_INT(tests_info);
//     EXCEPTION_CHECK_INT(tests_info->root);
//     ui_proc_product_test_t *proc = (ui_proc_product_test_t *)thiz->priv.play_info.user_data;
//     EXCEPTION_CHECK_INT(proc->ble);
//     EXCEPTION_CHECK_INT(proc->battery);
//     uint8_t mac_addr[6] = {0};
//     proc->ble->get_mac_addr(mac_addr);
    
//     aging_test_ui_aging_info_update(thiz, proc, tests_info->all_color_change);
//     aging_test_ui_mac_name_update(thiz, mac_addr, tests_info->all_color_change);
//     aging_test_ui_background_color_update(thiz, tests_info->all_color_change);
// #if IS_SUPPORT_BATTERY_PERCENT
//     aging_test_ui_battery_percent_update(thiz, proc->battery->get_battery_percent(), tests_info->all_color_change);
// #endif
//     tests_info->force_update = false;
//     thiz->priv.fps_count++;
//     if (get_timestamp() - thiz->priv.fps_start_time >= 10000) {
//         thiz->priv.fps_start_time = get_timestamp();
//         LOG_UI("===> product_tests home fps: %d", thiz->priv.fps_count / 10);
//         thiz->priv.fps_count = 0;
//         tests_info->all_color_change = !tests_info->all_color_change;
//     }
    return 0;
}
