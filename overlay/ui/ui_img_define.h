#ifndef __UI_IMG_DEFINE_H__
#define __UI_IMG_DEFINE_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep

/// 图片ID类型
typedef enum {
    UI_IMG_ID_LOGO = 1,  /// 开机LOGO
    UI_IMG_ID_BOOT_QRCODE = 2,   /// 开机二维码
    UI_IMG_ID_CHARGE = 3,   /// 充电中
    UI_IMG_ID_UNLOCK_TIP = 4,  /// 解锁提示
    UI_ID_PUMP_MODE_TIP = 5,     /// 泵模式提示
    UI_IMG_ID_PUMP_COMPLETE_TIP = 6, /// 吸奶完成提示
    UI_IMG_ID_LEFT_LEVEL_ADJUST_DISPLAY = 7,    /// 左档位调节显示
    UI_IMG_ID_RIGHT_LEVEL_ADJUST_DISPLAY = 8,   /// 右档位调节显示
    UI_IMG_ID_SETTINGS_SUBMENU = 9,   /// 追奶模式提示
    UI_IMG_ID_HOME_SCREEN = 10,   /// 首页
} ui_img_id_type_e;

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif 


