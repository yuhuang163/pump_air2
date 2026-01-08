/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "algo_port.h"
#include "stddef.h"
#include "v3_wear_detection.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static algo_port_context_t s_algo_port_context;

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
    uint8_t msg = 0;
    aglo_thread_context_t *thd = (aglo_thread_context_t *)args;
    CREATE_MSG_TYPE_QUEUE(&thd->msg_id, AIGO_WORK_DISPATCH_MESQ_NUM, 1);
    do {
        MSG_QUEUE_RECV(thd->msg_id, &msg, OS_WAIT_FOREVER);
        /// 进度单元.
       if (s_algo_port_context.algo_run_control == 1) {
        /// 采集压力数据,队列中获取压力数据
        /// 处理
        //V3WearDetectionMain(pressure_data);
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
    s_algo_port_context.result_cb = result_cb;
    AIGO_RUN_NEW_THREAD(&s_algo_port_context.thread_context, "ALGO_THREAD", algo_port_process_pressure_data, 
        RT_PRIO_ALGO_THREAD, (ALGO_THREAD_STACKSIZE));
    return 0;
}

int algo_port_deinit(void *user_data)
{
    AIGO_EXIT_THREAD(&s_algo_port_context.thread_context);
    return 0;
}

int algo_port_start(void *user_data) {
    s_algo_port_context.algo_run_control = 1;
    return 0;
}

int algo_port_stop(void *user_data) {
    s_algo_port_context.algo_run_control = 0;
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
