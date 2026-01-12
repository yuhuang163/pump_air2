/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "algo_port.h"
#include "cmsis_os_api.h"
#include "stddef.h"
#include "v3_wear_detection.h"
#include <stdint.h>

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static algo_port_context_t s_algo_port_context ={0} ;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void algo_port_process_pressure_data(const void *args)
{
    algo_port_msg_type_queue_t msg = {0};
    algo_port_pressure_data_t pressure_data = {0};
    do {
        MSG_QUEUE_RECV(&s_algo_port_context.thread_context.msg_id, &msg, OS_WAIT_FOREVER);
        /// 进度单元.
       if (s_algo_port_context.algo_pressure_run_control == 1 && msg.type == ALGO_PORT_DATA_TYPE_PRESSURE) {
        /// 采集压力数据,队列中获取压力数据
        uint8_t pressure_data_ptr[sizeof(algo_port_pressure_data_t)];
        aglo_ring_buffer_read_multiple(&s_algo_port_context.pressure_rb.rb, pressure_data.pressure_union.pressure_data, sizeof(algo_port_pressure_data_t));
        if (msg.type == ALGO_PORT_DATA_TYPE_PRESSURE && msg.size > 0) {
            //memcpy(&pressure_data, msg.data, msg.size);
        }
        PRINT_LOG("pressure_data: %f, %f", 
                 (double)pressure_data.pressure_union.pressure[0], 
                 (double)pressure_data.pressure_union.pressure[1]);
        /// 处理结果回调
        if (s_algo_port_context.result_cb != NULL) {
            algo_port_result_t result = {
                .timestamp = 0x00,
                .wear_detection_result = {0,0}, 
            };
            s_algo_port_context.result_cb(NULL, &result);
        }
       }
    } while (1);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int  algo_port_init(algo_port_result_cb result_cb, void *user_data)
{
    if (result_cb == NULL) {
        return -1;
    }
    AIGO_PRINT_LOG("algo_port_init");
    aglo_ring_buffer_init(&s_algo_port_context.pressure_rb.rb,&s_algo_port_context.pressure_rb.pressure_data, sizeof(s_algo_port_context.pressure_rb.pressure_data), sizeof(uint8_t));
    s_algo_port_context.result_cb = result_cb;
    AIGO_RUN_NEW_THREAD(&s_algo_port_context.thread_context, "ALGO_THREAD", algo_port_process_pressure_data, 
        RT_PRIO_ALGO_THREAD, (ALGO_THREAD_STACKSIZE));
    return 0;
}

int algo_port_deinit(void *user_data)
{
    AIGO_PRINT_LOG("algo_port_deinit");
    AIGO_EXIT_THREAD(&s_algo_port_context.thread_context);
    return 0;
}

int algo_port_start(void *user_data) {
    algo_port_run_trigger_t run_trigger = {0};
    if (user_data == NULL) {
        return -1;
    }
    memcpy(&run_trigger, user_data, sizeof(algo_port_run_trigger_t));
    if (run_trigger.type == ALGO_PORT_DATA_TYPE_PRESSURE) {
        s_algo_port_context.algo_pressure_run_control = run_trigger.run_trigger;
        PRINT_LOG("algo_port_start: pressure run control: %d", s_algo_port_context.algo_pressure_run_control);
    } else {
        return -1;
    }
    return 0;
}

int algo_port_stop(void *user_data) {
    algo_port_run_trigger_t run_trigger = {0};
    if (user_data == NULL) {
        return -1;
    }
    memcpy(&run_trigger, user_data, sizeof(algo_port_run_trigger_t));
    if (run_trigger.type == ALGO_PORT_DATA_TYPE_PRESSURE) {
        s_algo_port_context.algo_pressure_run_control = run_trigger.run_trigger;
        PRINT_LOG("algo_port_stop: pressure run control: %d", s_algo_port_context.algo_pressure_run_control);
    } else {
        return -1;
    }
    return 0;
}

int algo_port_set_level_and_mode(algo_port_pumping_info_t *pumping_info, void *user_data) {
    if (pumping_info == NULL) {
        return -1;
    }
    memcpy(&s_algo_port_context.pumping_info, pumping_info, sizeof(algo_port_pumping_info_t));
    //V3WearDetectionInit(s_algo_port_context.pumping_info.level, s_algo_port_context.pumping_info.mode);
    return 0;
}

int algo_port_set_data_to_ring_buffer(algo_port_data_type_e data_type,
                                      unsigned int size, uint8_t *user_data) {
    if ( size == 0 || user_data == NULL) {
        return -1;
    }
    if (data_type == ALGO_PORT_DATA_TYPE_PRESSURE) {
        return aglo_ring_buffer_write_multiple(&s_algo_port_context.pressure_rb.rb, user_data, size);
    } else {
        return -1;
    }
    return 0;
}

int algo_port_run_trigger(void *user_data) {
    algo_port_msg_type_queue_t msg = {0};
    msg.type = ALGO_PORT_DATA_TYPE_PRESSURE;
    msg.data = user_data;
    msg.size = sizeof(user_data);
    MSG_QUEUE_SEND(&s_algo_port_context.thread_context.msg_id, &msg, OS_WAIT_FOREVER);
    return 0;
}