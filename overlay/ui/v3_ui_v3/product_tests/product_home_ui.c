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
#define IS_SUPPORT_SCAN_QRCODE_TIP                                   0
#if (ROOT_PRODUCT_ID == ROOT_PRODUCT_V3PRO_ID)
#define IS_SUPPORT_BATTERY_PERCENT                                   1
#else
#define IS_SUPPORT_BATTERY_PERCENT                                   0
#endif
#define PRODUCT_TESTS_HOME_CONTENT_COUNT                             1
#define PRODUCT_TESTS_HOME_CONTENT_COLOR                             0x3C3D40
#define PRODUCT_TESTS_HOME_QRCODE_BG_COLOR                           0xFFFFFF
#define PRODUCT_TESTS_HOME_QRCODE_FG_COLOR                           0x000000
#define PRODUCT_TESTS_HOME_QRCODE_BORDER_COLOR                       0xA6A6A6
#define PRODUCT_TESTS_HOME_MAC_NAME_COLOR                            0x01D65A // 0x3C3D40
#define PRODUCT_TESTS_HOME_BATTERY_PERCENT_COLOR                      0x01D65A // 0x3C3D40
#define PRODUCT_TESTS_HOME_AGING_INFO_COLOR                           0x01D65A // 老化信息颜色
#if IS_SUPPORT_SCAN_QRCODE_TIP
#define PRODUCT_TESTS_HOME_MAC_NAME_X                                (105 + 4)
#define PRODUCT_TESTS_HOME_MAC_NAME_Y                                (80)
#define PRODUCT_TESTS_HOME_QRCODE_X                                  (PRODUCT_TESTS_HOME_MAC_NAME_X)
#define PRODUCT_TESTS_HOME_QRCODE_Y                                  (PRODUCT_TESTS_HOME_MAC_NAME_Y + 20 + 5)
#define PRODUCT_TESTS_HOME_QRCODE_SIZE                               (150)
#else
#define PRODUCT_TESTS_HOME_MAC_NAME_X                                (90 + 4)
#define PRODUCT_TESTS_HOME_MAC_NAME_Y                                (80)
#define PRODUCT_TESTS_HOME_QRCODE_X                                  (PRODUCT_TESTS_HOME_MAC_NAME_X)
#define PRODUCT_TESTS_HOME_QRCODE_Y                                  (PRODUCT_TESTS_HOME_MAC_NAME_Y + 20 + 10)
#define PRODUCT_TESTS_HOME_QRCODE_SIZE                               (180)
#define PRODUCT_TESTS_HOME_BATTERY_PERCENT_X                         (PRODUCT_TESTS_HOME_MAC_NAME_X+70)
#define PRODUCT_TESTS_HOME_BATTERY_PERCENT_Y                         (PRODUCT_TESTS_HOME_MAC_NAME_Y)
#define PRODUCT_TESTS_HOME_AGING_INFO_X                              (PRODUCT_TESTS_HOME_MAC_NAME_X)
#define PRODUCT_TESTS_HOME_AGING_INFO_Y                              (PRODUCT_TESTS_HOME_BATTERY_PERCENT_Y + PRODUCT_TESTS_HOME_QRCODE_SIZE+36)
#endif

typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
} product_tests_content_info_t;

typedef struct {  
    uint8_t force_update;
    uint8_t mac_addr[6];
    uint8_t rsv[16];
} product_tests_home_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_SCAN_QRCODE_TIP
const static product_tests_content_info_t product_tests_content_infos[PRODUCT_TESTS_HOME_CONTENT_COUNT] = {
    { "Please scan the QR code\nto quickly connect the device", (63 + 6), (250 + 20)},
};  
#endif

static product_tests_home_display_info_t *tests_info = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void product_tests_ui_root_init(void)
{

}

static void product_tests_mac_name_update(ui_manager_context_t *thiz, uint8_t *mac_addr)
{

}

static void product_tests_qrcode_update(ui_manager_context_t *thiz, uint8_t *mac_addr)
{      

}

static void product_tests_aging_info_update(ui_manager_context_t *thiz, ui_proc_product_test_t *proc)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // EXCEPTION_CHECK_VOID(tests_info);
    // EXCEPTION_CHECK_VOID(tests_info->root);
    // EXCEPTION_CHECK_VOID(proc);
    // EXCEPTION_CHECK_VOID(proc->tests);
    // EXCEPTION_CHECK_VOID(proc->pump);

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_product_tests_home_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (tests_info == NULL) {
    //     tests_info = (product_tests_home_display_info_t *)u_malloz(sizeof(product_tests_home_display_info_t));
    //     EXCEPTION_CHECK_VOID(tests_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // product_tests_ui_root_init();
    // ble_ui_init(thiz);
    // tests_info->force_update = true;
}

void ui_display_product_tests_home_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // EXCEPTION_CHECK_VOID(tests_info);
    // ble_ui_deinit(thiz);
    // LVGL_OBJ_DELETE(tests_info->root);
    // u_free(tests_info);
    // tests_info = NULL;
}

int ui_display_product_tests_home_process(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_INT(thiz);
    // EXCEPTION_CHECK_INT(tests_info);
    // EXCEPTION_CHECK_INT(tests_info->root);
    // ui_proc_product_test_t *proc = (ui_proc_product_test_t *)thiz->priv.play_info.user_data;
    // EXCEPTION_CHECK_INT(proc->ble);
    // EXCEPTION_CHECK_INT(proc->battery);
    // ble_ui_update(thiz, tests_info->root, proc->ble, COMMON_BLE_UI_X, COMMON_BLE_UI_Y);
    // uint8_t mac_addr[6] = {0};
    // proc->ble->get_mac_addr(mac_addr);
    // product_tests_qrcode_update(thiz, mac_addr);
    // product_tests_aging_info_update(thiz, proc);
    // product_tests_mac_name_update(thiz, mac_addr);
    // tests_info->force_update = false;
    // thiz->priv.fps_count++;
    // if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
    //     thiz->priv.fps_start_time = get_timestamp();
    //     LOG_UI("===> product_tests home fps: %d", thiz->priv.fps_count);
    //     thiz->priv.fps_count = 0;
    // }
    return 0;
}
