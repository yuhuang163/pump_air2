/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h" // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_MODE_COMP_BG              (0)
#define IS_SUPPORT_MODE_BG_IMG               (0)
#define BF_MODE_COUNT                        (6)
#define BF_CIRCLE_SLOT_COUNT                 (3)   /// 仅保留左/中/右 3 个圆槽位
#define BF_VIEW_W                            (360)
#define BF_VIEW_H                            (360)
#define BF_CIRCLE_W                          (250)
#define BF_CIRCLE_H                          (250)
#define BF_CIRCLE_CENTER_X_GAP               (BF_CIRCLE_W + 20)  /// 圆圈中心间距
#define BF_SIDE_CIRCLE_Y_OFFSET              (24)  /// 左右两个圆都比中间圆高 24
#define BF_TEXT_COLOR                        (0xB25962)
#define BF_CENTER_CIRCLE_GRAD_TOP_COLOR      (0xEB7581)
#define BF_CENTER_CIRCLE_GRAD_BOTTOM_COLOR   (0xF0868A)
#define BF_SIDE_CIRCLE_GRAD_TOP_COLOR        (0xF7A1A8)
#define BF_SIDE_CIRCLE_GRAD_BOTTOM_COLOR     (0xF7A1A8)
#define BF_TITLE_TEXT_COLOR                  (0xFFFFFF)
#define BF_DESC_TEXT_COLOR                   (0xFFDDDF)
#define BF_CENTER_DESC_TEXT_COLOR            (0xFFEAEA)

#define BF_PAGE_TITLE_Y_OFFSET               (34)
#define BF_COMPONENT_CENTER_Y_OFFSET         (-70)
#define BF_LAST_USAGE_ICON_Y_OFFSET          (BF_COMPONENT_CENTER_Y_OFFSET - 14)
#define BF_UNCONFIRMED_ICON_Y_OFFSET         (BF_COMPONENT_CENTER_Y_OFFSET - 18)
#define BF_TITLE_BASE_Y_OFFSET               (-26)
#define BF_DESC_BASE_Y_OFFSET                (6)

#define BF_SWITCH_ANIM_TIME_MS               (260)  /// 切换动画时间，单位：毫秒

typedef enum {
    BF_SLOT_LEFT = 0,
    BF_SLOT_CENTER,
    BF_SLOT_RIGHT,
    BF_SLOT_MAX,
} bf_circle_slot_e;

typedef struct {
    breast_pump_manager_context_t *pump_ctx;
    uint32_t custom_mode_id; /// 个性化模式ID
    uint16_t slot_res_id[BF_CIRCLE_SLOT_COUNT];
    int16_t slot_mode_idx[BF_CIRCLE_SLOT_COUNT];
    uint8_t cur_mode;
    uint8_t force_update;
    uint8_t mode_obj_init_done;
    uint8_t mode_num;
    uint8_t popup_ui_index;
    uint8_t new_step_info_flag; /// 新模式新步数信息标志 true-有新步数信息 false-无新步数信息, 使用后重置
    uint8_t rsv[2];
} bf_mode_display_info_t;


/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static bf_mode_display_info_t *bf_mode_info;

static const uint16_t bf_mode_component_ids[BF_MODE_COUNT] = {
    // RESOURCE_ID_MODE_EXPERT_ICON,  
    // RESOURCE_ID_MODE_EXPERT_ICON,  
    // RESOURCE_ID_MODE_EXPERT_ICON,  
    // RESOURCE_ID_MODE_EXPERT_ICON, 
    // RESOURCE_ID_MODE_EXPERT_ICON,  
    // RESOURCE_ID_MODE_CUSTOM_ICON, 
};

static const uint16_t dots_5_img_ids[BF_MODE_COUNT - 1] = {
    // RESOURCE_ID_SELECTED_LOCATE_5_START,
    // RESOURCE_ID_SELECTED_LOCATE_5_START + 1,
    // RESOURCE_ID_SELECTED_LOCATE_5_START + 2,
    // RESOURCE_ID_SELECTED_LOCATE_5_START + 3,
    // RESOURCE_ID_SELECTED_LOCATE_5_END,
};

static const uint16_t dots_6_img_ids[BF_MODE_COUNT] = {
    // RESOURCE_ID_SELECTED_LOCATE_6_START,
    // RESOURCE_ID_SELECTED_LOCATE_6_START + 1,
    // RESOURCE_ID_SELECTED_LOCATE_6_START + 2,
    // RESOURCE_ID_SELECTED_LOCATE_6_START + 3,
    // RESOURCE_ID_SELECTED_LOCATE_6_START + 4,
    // RESOURCE_ID_SELECTED_LOCATE_6_END,
};

static const char *bf_mode_name[BF_MODE_COUNT] = {
    "Cozy Flow", /// 自然韵律模式
    "Milk Boost", /// 2+8奶量提升模式
    "Power Pumping", /// PP追奶模式
    "Milk Start", /// 开奶模式
    "Deep Emptying", /// 追加排空模式
    "Customize", /// 已使用过的个性化模式
};

static const char *bf_mode_desc[BF_MODE_COUNT] = {
    "Moves from quick, gentle\nsuction to slower, deeper\nsuction to follow a baby's\nnatural nursing rhythm.",
    "Smooth, gradual suction to\nsupport milk production\nduring low-supply\nmoments.",
    "Frequent, gentle\nstimulation to encourage\nmultiple let-downs and\nsupport milk supply.",
    "Initiate, build, and maintain\na consistent milk supply.",
    "Helps encourage more\ncomplete milk removal at\nthe end of a session.",
    "For mothers who have a\nhigh demand for milk\nproduction.",
};

#if IS_SUPPORT_MODE_COMP_BG
static PSRAM_DATA_LOC uint8_t bf_mode_bg_data[] = {
    #include "comparison_png2txt/Custom3_360x360.txt"
};
#endif

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static void update_slot_content(ui_manager_context_t *thiz, bf_circle_slot_e slot, 
    uint8_t mode_idx, uint8_t last_mode_idx, bool new_step_info_flag)
{

}

static void circle_layout_apply(int8_t direction, bool anim)
{

}

static void circle_carousel_set_mode(ui_manager_context_t *thiz, uint8_t idx, bool anim)
{

}



static void bf_mode_root_init(ui_manager_context_t *thiz)
{

}

static void bf_mode_bg_update(ui_manager_context_t *thiz)
{
}

static void bf_mode_ui_update(ui_manager_context_t *thiz, uint8_t mode_idx)
{

}

static void bf_mode_text_update(ui_manager_context_t *thiz)
{
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void common_pump_bf_mode_ui_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (bf_mode_info == NULL) {
    //     bf_mode_info = (bf_mode_display_info_t *)u_malloz(sizeof(bf_mode_display_info_t));
    //     EXCEPTION_CHECK_VOID(bf_mode_info);
    // }
    // bf_mode_info->pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    // if (bf_mode_info->pump_ctx == NULL) {
    //     u_free(bf_mode_info);
    //     bf_mode_info = NULL;
    //     return;
    // }
    // bf_mode_root_init(thiz);
    // common_ui_bg_init(thiz);
    // bf_mode_info->force_update = true;
    // bf_mode_info->mode_num = BF_MODE_COUNT - 1;
    // bf_mode_info->popup_ui_index = ID_POPUP_UI_NONE;
    // bf_mode_info->custom_mode_id = 0;
    // int activate_status = 0;
    // READ_PUMP_MODE_ACTIVATE(bf_mode_info->pump_ctx, BREAST_PUMP_CUSTOM_MODE, activate_status);
    // storage_manager_context_t *st = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    // if (st && activate_status) {
    //     bf_mode_info->mode_num = BF_MODE_COUNT;    
    //     uint32_t custom_mode_id = 0;
    //     st->get_store_ram_val(STORE_ID_PUMP_MODE_STEPS_INFO_ID, BREAST_PUMP_CUSTOM_MODE, 0, &custom_mode_id);
    //     bf_mode_info->custom_mode_id = custom_mode_id;
    //     int new_step_info_flag = false; 
    //     st->get_store_ram_val(STORE_ID_MODE_NEW_STEP_INFO_FLAG, BREAST_PUMP_CUSTOM_MODE, 0, &new_step_info_flag);
    //     bf_mode_info->new_step_info_flag = new_step_info_flag > 0 ? true : false;
    //     LOG_UI("customization activated, id: %d, flag: %d", bf_mode_info->custom_mode_id, bf_mode_info->new_step_info_flag);
    // } else {
    //     bf_mode_info->new_step_info_flag = false;
    // }
}

void common_pump_bf_mode_ui_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz && bf_mode_info);
    // common_ui_img_resource_deinit(thiz, &bf_mode_info->last_usage_icon_info);
    // common_ui_img_resource_deinit(thiz, &bf_mode_info->unconfirmed_icon_info);
    // common_ui_img_resource_deinit(thiz, &bf_mode_info->dots_img_info);
    // for (uint8_t i = 0; i < BF_CIRCLE_SLOT_COUNT; i++) {
    //     common_ui_img_resource_deinit(thiz, &bf_mode_info->mode_component_info[i]);
    // }
    // common_ui_bg_deinit(thiz);
    // ui_common_operation_tip_remove(thiz);
    // LVGL_OBJ_DELETE(bf_mode_info->root);
    // u_free(bf_mode_info);
    // bf_mode_info = NULL;
}

int common_pump_bf_mode_ui_process(ui_manager_context_t *thiz, uint8_t mode_idx, uint16_t popup_ui_type)
{
    // EXCEPTION_CHECK_INT(thiz);
    // EXCEPTION_CHECK_INT(bf_mode_info && bf_mode_info->root);
    // if (bf_mode_info->mode_num > BF_MODE_COUNT || bf_mode_info->mode_num == 0) {
    //     int activate_status = 0;
    //     READ_PUMP_MODE_ACTIVATE(bf_mode_info->pump_ctx, BREAST_PUMP_CUSTOM_MODE, activate_status);
    //     if (activate_status) {
    //         bf_mode_info->mode_num = BF_MODE_COUNT;
    //     } else {
    //         bf_mode_info->mode_num = BF_MODE_COUNT - 1;
    //     }
    // }
    // bf_mode_bg_update(thiz);
    // bf_mode_ui_update(thiz, mode_idx);
    // bf_mode_text_update(thiz);
    // bf_mode_info->force_update = false;
    
    return 0;
}