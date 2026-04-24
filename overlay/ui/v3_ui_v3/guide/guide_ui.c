/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep
#include "window_screen_guide.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_GUIDE_COMP_BG                                (0)
#define GUIDE_PAGE_IMG_NUM                                      (6)
#define GUIDE_QRCODE_BG_COLOR                                   0xFFFFFF
#define GUIDE_QRCODE_FG_COLOR                                   0x000000
#define GUIDE_QRCODE_BORDER_COLOR                               (0xFFEAE6)  // 0xA6A6A6
#if 0
#define GUIDE_QRCODE_SIZE                                       (124 - 4)
#define GUIDE_QRCODE_ALIGN_Y                                    (68)
#else
#define GUIDE_QRCODE_SIZE                                       (124)
#define GUIDE_QRCODE_ALIGN_Y                                    (68)
#endif
#define GUIDE_TEXT_NUM                                          (4)
#define GUIDE_ARROW_TEXT_X_OFFSET                               (0)
#define GUIDE_ARROW_TEXT_Y_OFFSET                               (316)
#define GUIDE_TEXT_COLOR                                        (0xB25962)
#define GUIDE_TITLE_TEXT_COLOR                                  (0xB25962)
#define GUIDE_DESC_TEXT_COLOR                                   (0xC5757C)
#define GUIDE_EXIT_TEXT_COLOR                                   (0xDCA7A5)
#define GUIDE_BIND_TEXT_COLOR                                   (0x6A5A5B)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
typedef struct {
    // lv_obj_t * root;
    // lv_obj_t * qrcode;
    // lv_obj_t * text[GUIDE_TEXT_NUM];
    // lv_obj_t * page_img;
    // res_img_info_t img_info;
    // lv_draw_buf_t *page_decoded_buf;
    uint8_t force_update;
    uint8_t page_index;
    char qr_str[120];
} guide_display_info_t;

typedef struct {
    // const char *text;
    // const lv_font_t *font;
    // uint16_t text_x;
    // uint16_t text_y;
    // uint8_t line_space; /// 行间距
    uint32_t color;
} guide_text_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static guide_display_info_t *guide_info;

const static char *bind_qr_str[] = {
    "https://qr.momcozy.com/apps/?v=1&type=device&model=V3&sn=",
};

const static uint16_t guide_page_img_ids[GUIDE_PAGE_IMG_NUM] = {
    // RESOURCE_ID_GUIDE_PAGE1_IMG,
    // RESOURCE_ID_GUIDE_PAGE2_IMG,
    // RESOURCE_ID_GUIDE_PAGE3_IMG,
    // RESOURCE_ID_GUIDE_PAGE4_IMG,
    // RESOURCE_ID_GUIDE_PAGE5_IMG,
    // RESOURCE_ID_GUIDE_PAGE6_IMG,
};

/// 首页
const static guide_text_info_t home_text_infos[GUIDE_TEXT_NUM] = {

};

/// 开始泵奶
const static guide_text_info_t start_pump_text_infos[GUIDE_TEXT_NUM] = {
    // {"Press to enter/exit\nthe function", 
    //     &lv_font_oppo_sans_20_medium, 0, 51, 4, GUIDE_TITLE_TEXT_COLOR},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, 4, GUIDE_DESC_TEXT_COLOR},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, 4, GUIDE_TEXT_COLOR},
    // {"Press down to exit", &lv_font_oppo_sans_14_medium, 0, 
    //     GUIDE_ARROW_TEXT_Y_OFFSET, 4, GUIDE_EXIT_TEXT_COLOR}
};

/// 设置泵奶
const static guide_text_info_t setting_pump_text_infos[GUIDE_TEXT_NUM] = {
    // {"Turn to adjust the options\nPress down to confirm", 
    //     &lv_font_oppo_sans_20_medium, 0, 51, 4, GUIDE_TITLE_TEXT_COLOR},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, 4, GUIDE_DESC_TEXT_COLOR},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, 4, GUIDE_TEXT_COLOR},
    // {"Press down to exit", &lv_font_oppo_sans_14_medium, 0,
    //     GUIDE_ARROW_TEXT_Y_OFFSET, 4, GUIDE_EXIT_TEXT_COLOR}
};

/// 停止泵奶
const static guide_text_info_t stop_pump_text_infos[GUIDE_TEXT_NUM] = {
    // {"Press", &lv_font_oppo_sans_20_medium, 78, 51, 4, GUIDE_TITLE_TEXT_COLOR},
    // {"quick start", &lv_font_oppo_sans_20_medium, 180, 51, 4, GUIDE_TITLE_TEXT_COLOR},
    // {"default program", &lv_font_oppo_sans_20_medium, 0, 75, 4, GUIDE_TITLE_TEXT_COLOR},
    // {"Press down to exit", &lv_font_oppo_sans_14_medium, 0, 
    //     GUIDE_ARROW_TEXT_Y_OFFSET, 4, GUIDE_EXIT_TEXT_COLOR}
};

/// 泵奶模式选择
const static guide_text_info_t pump_mode_select_text_infos[GUIDE_TEXT_NUM] = {
    // {"Press and hold for 3 seconds\nto stop pumping", 
    //     &lv_font_oppo_sans_20_medium, 0, 125, 4, 0xFFFFFF},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, 4, GUIDE_DESC_TEXT_COLOR},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, 4, GUIDE_TEXT_COLOR},
    // {"Press down to exit", &lv_font_oppo_sans_14_medium, 0, 
    //     GUIDE_ARROW_TEXT_Y_OFFSET, 4, 0x938383}
};

/// 绑定引导
const static guide_text_info_t bind_guide_text_infos[GUIDE_TEXT_NUM] = {
    // {"Scan to connect for tracking, remote\ncontrol, and custom modes",
    // &lv_font_oppo_sans_16_medium, 0, 212, 4, GUIDE_BIND_TEXT_COLOR},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, 4, GUIDE_BIND_TEXT_COLOR},
    // {"", &lv_font_oppo_sans_14_medium, 0, 0, 4, GUIDE_BIND_TEXT_COLOR},
    // {"Press down to exit", &lv_font_oppo_sans_14_medium, 0, 
    //     GUIDE_ARROW_TEXT_Y_OFFSET, 4, GUIDE_TITLE_TEXT_COLOR}
};

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static void ui_guide_root_init(void)
{

}

static void guide_qrcode_update(ui_manager_context_t *thiz, uint8_t page_index)
{  

}

static void guide_ui_page_img_update(ui_manager_context_t *thiz, uint8_t page_index)
{

}

static void guide_ui_text_update(ui_manager_context_t *thiz, uint8_t page_index)
{

}

static void guide_ui_update(ui_manager_context_t *thiz, uint8_t page_index)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_guide_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (guide_info == NULL) {
    //     guide_info = (guide_display_info_t *)u_malloz(sizeof(guide_display_info_t));
    //     EXCEPTION_CHECK_VOID(guide_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // ui_guide_root_init(); /// 初始化 root 容器
    // guide_info->force_update = true;
    // storage_manager_context_t *storage = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    // if (storage) {
    //     factory_data_store_t *factory_data = storage->get_factory_data();
    //     factory_dev_info_t *dev_info = &factory_data->dev_info;
    //     unsigned int sn_n = dev_info->sn_len;
    //     if (sn_n > DEVICE_UNIQUE_SN_LEN) {
    //         sn_n = DEVICE_UNIQUE_SN_LEN;
    //     }
    //     snprintf(guide_info->qr_str, sizeof(guide_info->qr_str), "%s%.*s", bind_qr_str[0],
    //              (int)sn_n, (const char *)dev_info->sn);
    //     LOG_UI("%s", guide_info->qr_str);
    // } else {
    //     snprintf(guide_info->qr_str, sizeof(guide_info->qr_str), "%s", bind_qr_str[0]);
    //     LOG_UI("%s", guide_info->qr_str);
    // }
}

void ui_display_guide_remove(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_VOID(thiz && guide_info && guide_info->root);
// #if !IS_SUPPORT_GUIDE_COMP_BG
//     if (guide_info->page_decoded_buf != NULL) {
//         if (guide_info->page_img != NULL && lv_obj_is_valid(guide_info->page_img)) {
//             lv_image_set_src(guide_info->page_img, NULL);
//         }
//         common_ui_png_decode_release(guide_info->page_decoded_buf);
//         guide_info->page_decoded_buf = NULL;
//     }
// #endif
//     common_ui_img_resource_deinit(thiz, &guide_info->img_info);
//     LVGL_OBJ_DELETE(guide_info->root);
//     u_free(guide_info);
//     guide_info = NULL;
}

int ui_display_guide_process(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_INT(thiz && guide_info && guide_info->root);
    // ui_proc_guide_t *proc = (ui_proc_guide_t *)thiz->priv.play_info.user_data;
    // EXCEPTION_CHECK_INT(proc);
    // guide_ui_update(thiz, proc->page_index);
    // guide_info->force_update = false;
    // thiz->priv.fps_count++;
    // if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
    //     thiz->priv.fps_start_time = get_timestamp();
    //     LOG_UI("===> guide fps: %d", thiz->priv.fps_count);
    //     thiz->priv.fps_count = 0;
    // }
    return 0;
}
