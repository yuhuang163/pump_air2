#ifndef __INPUT_KEY_CODE_H__
#define __INPUT_KEY_CODE_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/

/// 按键类型
typedef enum {
    KEY_CODE_POWER = (1 << 0),   /// 支持唤醒
    KEY_CODE_START = (1 << 1),   /// 启动键
    KEY_CODE_MODE = (1 << 2),   /// 模式键
    KEY_CODE_FREQUENCY = (1 << 3),   /// 频率键
    KEY_CODE_BREASTFEEDING = (1 << 4),   /// 追奶键
    KEY_CODE_LEFT_CONTROL = (1 << 5),   /// 左控制键
    KEY_CODE_RIGHT_CONTROL = (1 << 6),   /// 右控制键
} input_key_code_e;

#ifdef __cplusplus
}
#endif

#endif 


