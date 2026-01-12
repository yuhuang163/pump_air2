/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "algo_port.h"
#include "cmsis_os_api.h"
#include "stddef.h"
#include "v3_wear_detection.h"
#include <stdint.h>
#include "v3_light_ctrl.h"
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
    aglo_thread_context_t *thd = (aglo_thread_context_t *)args;
    algo_port_msg_type_queue_t msg = {0};
    algo_port_pressure_data_t pressure_data = {0};
    algo_port_light_data_t light_data = {0};
    
    /// 在线程函数内部创建消息队列（标准模式）
    AIGO_CREATE_MSG_TYPE_QUEUE(&thd->msg_id, 10, sizeof(algo_port_msg_type_queue_t));
    AIGO_PRINT_LOG("algo_port_process_pressure_data: msg queue created in thread \n\r");
    
    do {
        AIGO_MSG_QUEUE_RECV(thd->msg_id, &msg, AIGO_OS_WAIT_FOREVER);
        
        /// 进度单元.
        if (s_algo_port_context.algo_pressure_run_control == 1 && msg.type == ALGO_PORT_DATA_TYPE_PRESSURE) {
            /// 采集压力数据,队列中获取压力数据
            unsigned int read_count = aglo_ring_buffer_read_multiple(&s_algo_port_context.pressure_rb.rb, 
                                                                      pressure_data.pressure_union.pressure_data, 
                                                                      sizeof(algo_port_pressure_data_t));
            //AIGO_PRINT_LOG("algo_port_process_pressure_data: ring buffer read_count=%d \n\r", read_count);
            
            if (read_count > 0) {
                AIGO_PRINT_LOG("pressure_data: %f, %f \n\r", 
                         (double)pressure_data.pressure_union.pressure[0], 
                         (double)pressure_data.pressure_union.pressure[1]);
            } else {
                AIGO_PRINT_LOG("pressure_data: ring buffer is empty, no data to read \n\r");
            }
            
            /// 处理结果回调
            if (s_algo_port_context.result_cb != NULL) {
                algo_port_result_t result = {
                    .data_type = ALGO_PORT_DATA_TYPE_PRESSURE,
                    .data_union.pressure_result.timestamp = 0x00,
                    .data_union.pressure_result.wear_detection_result = {0,0}, 
                };
                s_algo_port_context.result_cb(NULL, &result);
            }
        } 
        
        else if (s_algo_port_context.algo_light_run_control == 1 && msg.type == ALGO_PORT_DATA_TYPE_LIGHT) {
            /// 采集光传感器数据,队列中获取光传感器数据
            unsigned int read_count = aglo_ring_buffer_read_multiple(&s_algo_port_context.light_rb.rb, 
                                                                      light_data.light_union.light_data, 
                                                                      sizeof(algo_port_light_data_t));
            AIGO_PRINT_LOG("algo_port_process_light_data: ring buffer "
                           "read_count=%d,lux_level=%d \n\r",
                           read_count, light_data.light_union.light);
            // 数据处理，计算pwm_duty和setting_level
            algo_port_result_t result = {
                .data_type = ALGO_PORT_DATA_TYPE_LIGHT,
                .data_union.light_result.timestamp = 0x00,
                .data_union.light_result.pwm_duty = 0,
                .data_union.light_result.setting_level = 0,
            };
            algo_light_handle(light_data.light_union.light, &result.data_union.light_result);
            s_algo_port_context.result_cb(NULL, &result);
        }
        else {
            AIGO_PRINT_LOG("algo_port_process_pressure_data: skip processing, run_control=%d, msg.type=%d \n\r", 
                     s_algo_port_context.algo_pressure_run_control, msg.type);
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
    AIGO_PRINT_LOG("algo_port_init \n\r");
    /// 初始化气压传感器环形缓冲区
    aglo_ring_buffer_init(&s_algo_port_context.pressure_rb.rb,&s_algo_port_context.pressure_rb.pressure_data, sizeof(s_algo_port_context.pressure_rb.pressure_data), sizeof(uint8_t));
    /// 设置结果回调函数
    s_algo_port_context.result_cb = result_cb;
    /// 初始化光传感器环形缓冲区
    aglo_ring_buffer_init(&s_algo_port_context.light_rb.rb,&s_algo_port_context.light_rb.light_data, sizeof(s_algo_port_context.light_rb.light_data), sizeof(uint8_t));
    /// 消息队列在线程函数内部创建（标准模式）
    AIGO_RUN_NEW_THREAD(&s_algo_port_context.thread_context, "ALGO_THREAD", algo_port_process_pressure_data, 
        RT_PRIO_ALGO_THREAD, (ALGO_THREAD_STACKSIZE));
    return 0;
}

int algo_port_deinit(void *user_data)
{
    AIGO_PRINT_LOG("algo_port_deinit \n\r");
    AIGO_EXIT_THREAD(&s_algo_port_context.thread_context);
    AIGO_DELETE_MSG_QUEUE(s_algo_port_context.thread_context.msg_id);
    aglo_ring_buffer_reset(&s_algo_port_context.pressure_rb.rb);
    aglo_ring_buffer_reset(&s_algo_port_context.light_rb.rb);
    s_algo_port_context.result_cb = NULL;
    s_algo_port_context.pumping_info = (algo_port_pumping_info_t){0};
    s_algo_port_context.algo_pressure_run_control = 0;
    s_algo_port_context.algo_light_run_control = 0;
    memset(&s_algo_port_context, 0, sizeof(algo_port_context_t));
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
        AIGO_PRINT_LOG("algo_port_start: pressure run control: %d \n\r", s_algo_port_context.algo_pressure_run_control);
    } 
    else if (run_trigger.type == ALGO_PORT_DATA_TYPE_LIGHT) {
        s_algo_port_context.algo_light_run_control = run_trigger.run_trigger;
        AIGO_PRINT_LOG("algo_port_start: light run control: %d \n\r", s_algo_port_context.algo_light_run_control);
    }
    else {
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
        AIGO_PRINT_LOG("algo_port_stop: pressure run control: %d \n\r", s_algo_port_context.algo_pressure_run_control);
    } 
    else if (run_trigger.type == ALGO_PORT_DATA_TYPE_LIGHT) {
        s_algo_port_context.algo_light_run_control = run_trigger.run_trigger;
        AIGO_PRINT_LOG("algo_port_stop: light run control: %d \n\r", s_algo_port_context.algo_light_run_control);
    }
    else {
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
    } 
    else if (data_type == ALGO_PORT_DATA_TYPE_LIGHT) {
        return aglo_ring_buffer_write_multiple(&s_algo_port_context.light_rb.rb, user_data, size);
    }
    else {
        return -1;
    }
    return 0;
}

int algo_port_run_trigger(void *user_data)
{
    algo_port_msg_type_queue_t *msg = (algo_port_msg_type_queue_t *)user_data;
    if (msg == NULL) {
        AIGO_PRINT_LOG("algo_port_run_trigger: user_data is NULL \n\r");
        return -1;
    }
    /// 检查消息队列是否已创建
    if (s_algo_port_context.thread_context.msg_id == NULL) {
        AIGO_PRINT_LOG("algo_port_run_trigger: msg queue not created yet \n\r");
        return -1;
    }
    //AIGO_PRINT_LOG("algo_port_run_trigger: sending msg, type=%d, size=%d, msg_id=%p \n\r", 
    //               msg->type, msg->size, s_algo_port_context.thread_context.msg_id);
    if (s_algo_port_context.algo_pressure_run_control == 1 ||
        s_algo_port_context.algo_light_run_control == 1) {
      int ret = AIGO_MSG_QUEUE_SEND(s_algo_port_context.thread_context.msg_id,
                                    msg, 0);
      if (ret != 0) {
        AIGO_PRINT_LOG(
            "algo_port_run_trigger: MSG_QUEUE_SEND failed, ret=%d \n\r", ret);
      }
      return ret;
    }
    return  -1;
}
