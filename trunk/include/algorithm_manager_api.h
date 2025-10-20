#ifndef __ALGO_MANAGER_API_H__
#define __ALGO_MANAGER_API_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep
#include "broadcast_manager_api.h"
#include "app_algo.h" // IWYU pragma: keep

struct breast_pump_context;
struct input_dispatcher_context;
struct storage_manager_context;

/// 吸奶数据ID
typedef enum{
    ALGO_BREAST_PUMP_DATA_PUMPING,  /// 吸奶中的信息 
    ALGO_BREAST_PUMP_DATA_HISTORY,   /// 上次有效的泵奶信息 >30秒
    ALGO_BREAST_PUMP_DATA_MAX,
} algo_breast_pump_data_id_e;

/// 算法接口.
typedef struct algo_manager_context {
/// private
    struct __algo_private {
        broadcast_obj_t m_bcd;
        pump_result_info_t algo_breast_pump[ALGO_BREAST_PUMP_DATA_MAX];  
        int algo_inited;
    } priv;

/// public
    int (*get_version)(void);
    pump_result_info_t *(*get_breast_pump_info)(int index);
    struct __algo_fusion_hook {
        int (*on_sensor_data_cb_route)(struct algo_manager_context *ctx, uint8_t *data, int data_len);
        int (*add_static_calib_cmpl_cb)(void (*cb)(void));
        void (*imu_static_calib_start)(void);
        void (*imu_static_calib_stop)(void);
    } *hook;

} algo_manager_context_t;

#ifdef __cplusplus
}
#endif

#endif 


