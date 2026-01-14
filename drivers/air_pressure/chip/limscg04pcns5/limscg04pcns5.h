#ifndef __LIMS_H__
#define __LIMS_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "bsp_common.h" // IWYU pragma: keep

/**@brief Firmware information define*/
#define LIMS_CONVERSION_MODE_REG            0x30
#define LIMS_PRESSURE_MSB_REG               0x55
#define LIMS_PRESSURE_CSB_REG               0x56
#define LIMS_PRESSURE_LSB_REG               0x57

#define LIMS_TEMP_MSB_REG                   0x58
#define LIMS_TEMP_LSB_REG                   0x59

#define LIMS_STATUS_REG                     0x54
#define LIMS_RESET_REG                      0x50

/// 转换方式
typedef enum {
    LIMS_SINGLE_CONVERSION = 0x0A,  /// 单次转换
    LIMS_CONTINUE_CONVERSION = 0x0B,  /// 持续转换
    LIMS_62P5MS_PERIOD_CONVERSION = 0x1B, /// 62.5ms周期转换
    LIMS_125MS_PERIOD_CONVERSION = 0x2B,  /// 125ms周期转换
    LIMS_1S_PERIOD_CONVERSION = 0xFB,   /// 1s周期转换
} limscg04pcns5_conversion_mode_e;

/// 转换状态
#define LIMS_CONVERSION_COMPLETION          1  /// 转换完成

/// 复位
#define LIMS_RESET_VALUE                    (1 << 5)



/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif 
