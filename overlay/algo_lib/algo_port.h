#ifndef _ALGO_PORT_H_
#define _ALGO_PORT_H_
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "aigo_port_api.h"


typedef struct {
    uint32_t timestamp;
    uint8_t wear_detection_result[AIGO_AIR_PRESSURE_NUM]; //佩戴检测结果 0:未佩戴 1:佩戴    
} algo_port_result_t;

typedef struct {    //AIGO_BREAST_PUMP_NUM 0:左泵 1:右泵
    uint8_t level[AIGO_BREAST_PUMP_NUM]; //0-15
    uint8_t mode[AIGO_BREAST_PUMP_NUM];//0:刺激模式 1:吸乳模式 2:混合模式 3:个性化模式 4:PP追奶模式 5:2+8奶量提升模式 6:开奶模式
} algo_port_pumping_info_t;

typedef int (*algo_port_result_cb)( void *user_data, algo_port_result_t *result);

typedef struct {
    aglo_thread_context_t thread_context;
    algo_port_result_cb result_cb;
    algo_port_pumping_info_t pumping_info;
    uint8_t algo_run_control; //0:停止 1:运行
} algo_port_context_t;

int algo_port_init(algo_port_result_cb result_cb, void *user_data);
int algo_port_deinit(void *user_data);
int algo_port_start(void *user_data);
int algo_port_stop(void *user_data);
int algo_port_set_level_and_mode(algo_port_pumping_info_t *pumping_info, void *user_data);

#ifdef __cplusplus
}
#endif

#endif