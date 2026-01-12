#ifndef _ALGO_PORT_H_
#define _ALGO_PORT_H_
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "aigo_port_api.h"
#include "aglo_ring_buffer.h"

typedef enum {
    ALGO_PORT_DATA_TYPE_PRESSURE = 1,
    ALGO_PORT_DATA_TYPE_LIGHT = 2,
    ALGO_PORT_DATA_TYPE_MAX
} algo_port_data_type_e;

typedef struct {
    algo_port_data_type_e type;
    uint8_t run_trigger; //0:停止 1:运行
} algo_port_run_trigger_t;

typedef struct {
    algo_port_data_type_e type;
    void *data;
    unsigned int size;
} algo_port_msg_type_queue_t;

typedef struct {
    union {
        uint8_t pressure_data[sizeof(float) * AIGO_AIR_PRESSURE_NUM];
        float pressure[AIGO_AIR_PRESSURE_NUM];
    } pressure_union;
} algo_port_pressure_data_t;


typedef struct {
    uint32_t timestamp;
    uint8_t wear_detection_result[AIGO_AIR_PRESSURE_NUM]; //佩戴检测结果 0:未佩戴 1:佩戴    
} algo_port_pressure_result_t;

typedef struct {    //AIGO_BREAST_PUMP_NUM 0:左泵 1:右泵
    uint8_t level[AIGO_BREAST_PUMP_NUM]; //0-15
    uint8_t mode[AIGO_BREAST_PUMP_NUM];//0:刺激模式 1:吸乳模式 2:混合模式 3:个性化模式 4:PP追奶模式 5:2+8奶量提升模式 6:开奶模式
} algo_port_pumping_info_t;

typedef struct {
    union {
        uint8_t light_data[4];
        uint32_t light;
    } light_union;
} algo_port_light_data_t;

typedef struct {
    uint32_t timestamp;
    uint16_t pwm_duty; //算法计算的亮度PWM占空比等级
    uint16_t setting_level; //算法计算的亮度设置等级0-10
} algo_port_light_result_t;

typedef struct {
    algo_port_data_type_e data_type;
    union {
        algo_port_pressure_result_t pressure_result;
        algo_port_light_result_t light_result;
    } data_union;
} algo_port_result_t;

typedef int (*algo_port_result_cb)( void *user_data, algo_port_result_t *result);

typedef struct {
    aglo_ring_buffer_t rb;
    uint8_t pressure_data[4*32*2]; //2个气压传感器，32个采样点，4个字节(float)一个采样点
} algo_port_pressure_ring_buffer_t;

typedef struct {
    aglo_ring_buffer_t rb;
    uint8_t light_data[4*10]; //10个采样点，4个字节(uint32_t)一个采样点
} algo_port_light_ring_buffer_t;

typedef struct {
    aglo_thread_context_t thread_context;
    algo_port_result_cb result_cb;
    algo_port_pumping_info_t pumping_info;
    uint8_t algo_pressure_run_control; //0:停止 1:运行
    uint8_t algo_light_run_control; //0:停止 1:运行
    algo_port_pressure_ring_buffer_t pressure_rb;
    algo_port_light_ring_buffer_t light_rb;
} algo_port_context_t;

int algo_port_init(algo_port_result_cb result_cb, void *user_data);
int algo_port_deinit(void *user_data);
int algo_port_start(void *user_data);
int algo_port_stop(void *user_data);
int algo_port_set_level_and_mode(algo_port_pumping_info_t *pumping_info, void *user_data);
int algo_port_run_trigger(void *user_data);
int algo_port_set_data_to_ring_buffer(algo_port_data_type_e data_type, unsigned int size, uint8_t *user_data);
#ifdef __cplusplus
}
#endif

#endif