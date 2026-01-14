#ifndef __ADAPTIVE_DIMMING_H__
#define __ADAPTIVE_DIMMING_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep

#define ADAPTIVE_DIMMING_DATA_NUM                 2   /// 自适应背光数据个数
#define ADAPTIVE_DIMMING_DATA_SIZE                10  /// 自适应背光数据大小

typedef struct {
    uint8_t is_acquire_complete; /// 采集完成标志
    uint8_t index; /// 采集索引
    uint16_t buff[ADAPTIVE_DIMMING_DATA_SIZE];
} adaptive_dimming_capture_data_t;

typedef struct {
    uint8_t is_adaptive_dimming; /// 自适应背光开关
    uint16_t lux_level; /// 当前亮度等级
    uint16_t last_lux_level; /// 上次亮度等级
    adaptive_dimming_capture_data_t capture_data;
} adaptive_dimming_info_t;

/*
* INTERNAL FUNCTION DECLARATION
*****************************************************************************************
*/
void set_adaptive_dimming_state(bool state);
bool get_adaptive_dimming_state(void);
void adaptive_dimming_init(void);

#ifdef __cplusplus
}
#endif

#endif 







