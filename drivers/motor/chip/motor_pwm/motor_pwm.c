/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "hal_motor.h"
#include "hal_pwm.h"
#include "hal_iomux.h"
#include "rtx_lib.h"  // IWYU pragma: keep
#include "hal_timer.h"
#include "bsp_common.h" 
#include "bsp_motor_internal.h"
#include "motor_pwm.h"
/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_PUMP_RUN_TIME_TEST          (1)
#define IS_SUPPORT_PUMP_DEBUG_LOG              (0)

#define IS_SUPPORT_PUMP_PWM_GRADUAL_CHANGE     (0)
#define PUMP_PWM_GRADUAL_CHANGE_INIT_DUTY      (10) /// 初始占空比
#define PUMP_PWM_GRADUAL_CHANGE_PERIOD         (2) /// 变化周期 2ms
#define PUMP_PWM_GRADUAL_CHANGE_TIME_SCALE     (1) /// 变化时间比例系数：change_time = target_duty * SCALE (ms)

typedef struct {
    enum HAL_PWM_ID_T id;
    void (*iomux_set)(void);
} motor_pwm_cfg_t;

#if IS_SUPPORT_PUMP_PWM_GRADUAL_CHANGE
static void bsp_motor_pump_1_timer_timeout_handler(void *param);
static void bsp_motor_pump_2_timer_timeout_handler(void *param);
#endif
/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static const motor_pwm_cfg_t motor_pwm_cfg[] = {
    {HAL_PWM_ID_1,  hal_iomux_set_pwm1},
    {HAL_PWM1_ID_3, hal_iomux_set_pwm7}, // P37
    {HAL_PWM_ID_0,  hal_iomux_set_pwm0},
    {HAL_PWM1_ID_2, hal_iomux_set_pwm6}, // P36
};

static uint8_t motor_pwm_status[MOTOR_PWM_ID_MAX];
#if IS_SUPPORT_PUMP_RUN_TIME_TEST
static uint8_t count = 0;
static uint32_t start_time = 0;
static uint32_t run_time[20] = {0};
#endif

#if IS_SUPPORT_PUMP_PWM_GRADUAL_CHANGE
static motor_pwm_timer_cfg_t timer_info[MOTOR_PWM_PUMP_2 + 1] = {
    [MOTOR_PWM_PUMP_1] =
        {
            .time_id = NULL,
            .time_handler = bsp_motor_pump_1_timer_timeout_handler,
            .time_period = PUMP_PWM_GRADUAL_CHANGE_PERIOD, //2ms
            .pwm_cfg =
                {
                    .freq = 0,
                    .ratio = 0,
                    .inv = false,
                    .sleep_on = false,
                },
            .init_duty = PUMP_PWM_GRADUAL_CHANGE_INIT_DUTY,
            .target_duty = 0,
            .duty_step = 0,
            .current_duty = PUMP_PWM_GRADUAL_CHANGE_INIT_DUTY,
        },
    [MOTOR_PWM_PUMP_2] = {
        .time_id = NULL,
        .time_handler = bsp_motor_pump_2_timer_timeout_handler,
        .time_period = PUMP_PWM_GRADUAL_CHANGE_PERIOD, //2ms
        .pwm_cfg =
            {
                .freq = 0,
                .ratio = 0,
                .inv = false,
                .sleep_on = false,
            },
        .init_duty = PUMP_PWM_GRADUAL_CHANGE_INIT_DUTY,
        .target_duty = 0,
        .duty_step = 0,
        .current_duty = PUMP_PWM_GRADUAL_CHANGE_INIT_DUTY,
    }};
#endif
/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_PUMP_PWM_GRADUAL_CHANGE
/**
 * @brief PWM步进更新函数
 * @param pwm_id PWM ID
 */
static void bsp_motor_pwm_step_update_pump1(motor_pwm_control_id_e pwm_id)
{
    if (pwm_id != MOTOR_PWM_PUMP_1) {
        //LOGE("pwm_id is not MOTOR_PWM_PUMP_1");
        return;
    }
    
    // 保存当前占空比（用于日志）
    uint8_t old_duty = timer_info[pwm_id].current_duty;
    
    // 计算新的占空比
    uint8_t new_duty = timer_info[pwm_id].current_duty + timer_info[pwm_id].duty_step;
    
    // 防止超过目标值
    if (new_duty > timer_info[pwm_id].target_duty) {
        new_duty = timer_info[pwm_id].target_duty;
    }
    
    // 更新当前占空比
    timer_info[pwm_id].current_duty = new_duty;
    
    // 更新PWM配置
    struct HAL_PWM_CFG_T pwm_cfg = {
        .freq = timer_info[pwm_id].pwm_cfg.freq,
        .ratio = new_duty,
        .inv = timer_info[pwm_id].pwm_cfg.inv,
        .sleep_on = timer_info[pwm_id].pwm_cfg.sleep_on,
    };

    hal_pwm_enable(motor_pwm_cfg[pwm_id].id, &pwm_cfg);
    if (new_duty < timer_info[pwm_id].target_duty && timer_info[pwm_id].time_status == 1) {
        if (timer_info[pwm_id].time_id != NULL) {
            hwtimer_start(timer_info[pwm_id].time_id, MS_TO_HWTICKS(timer_info[pwm_id].time_period));
        }
    }
}

static void bsp_motor_pwm_step_update_pump2(motor_pwm_control_id_e pwm_id)
{
    if (pwm_id != MOTOR_PWM_PUMP_2) {
        //LOGE("pwm_id is not MOTOR_PWM_PUMP_2");
        return;
    }
    
    // 保存当前占空比（用于日志）
    uint8_t old_duty = timer_info[pwm_id].current_duty;
    
    // 计算新的占空比
    uint8_t new_duty = timer_info[pwm_id].current_duty + timer_info[pwm_id].duty_step;
    
    // 防止超过目标值
    if (new_duty > timer_info[pwm_id].target_duty) {
        new_duty = timer_info[pwm_id].target_duty;
    }
    
    // 更新当前占空比
    timer_info[pwm_id].current_duty = new_duty;
    
    // 更新PWM配置
    struct HAL_PWM_CFG_T pwm_cfg = {
        .freq = timer_info[pwm_id].pwm_cfg.freq,
        .ratio = new_duty,
        .inv = timer_info[pwm_id].pwm_cfg.inv,
        .sleep_on = timer_info[pwm_id].pwm_cfg.sleep_on,
    };

    hal_pwm_enable(motor_pwm_cfg[pwm_id].id, &pwm_cfg);
    if (new_duty < timer_info[pwm_id].target_duty && timer_info[pwm_id].time_status == 1) {
        if (timer_info[pwm_id].time_id != NULL) {
            hwtimer_start(timer_info[pwm_id].time_id, MS_TO_HWTICKS(timer_info[pwm_id].time_period));
        }
    }
}

static void bsp_motor_pump_1_timer_timeout_handler(void *param)
{
    bsp_motor_pwm_step_update_pump1(MOTOR_PWM_PUMP_1);
}

static void bsp_motor_pump_2_timer_timeout_handler(void *param)
{
    bsp_motor_pwm_step_update_pump2(MOTOR_PWM_PUMP_2);
}

static void bsp_motor_start_periodic_task(motor_pwm_control_id_e pwm_id)
{
    if (timer_info[MOTOR_PWM_PUMP_1].time_id == NULL ||
        timer_info[MOTOR_PWM_PUMP_2].time_id == NULL) {
        LOGE("Failed to start periodic task");
        return;
    }
    LOGI("Start periodic task: %d", pwm_id);
    // 将毫秒转换为tick
    uint32_t ticks = MS_TO_HWTICKS(timer_info[pwm_id].time_period);
    
    if (hwtimer_active(timer_info[pwm_id].time_id)) {
        hwtimer_stop(timer_info[pwm_id].time_id);
    }
    // 启动定时器
    hwtimer_start(timer_info[pwm_id].time_id, ticks);
    timer_info[pwm_id].time_status = 1;
}

static void bsp_motor_stop_periodic_task(motor_pwm_control_id_e pwm_id)
{
    LOGI("Stop periodic task: %d", pwm_id);
    if (timer_info[pwm_id].time_id != NULL) {
        hwtimer_stop(timer_info[pwm_id].time_id);
        timer_info[pwm_id].time_status = 0;
    }
}

static int8_t bsp_motor_init_timer_system(void)
{
    // 分配定时器
    timer_info[MOTOR_PWM_PUMP_1].time_id = hwtimer_alloc(bsp_motor_pump_1_timer_timeout_handler, NULL);
    if (timer_info[MOTOR_PWM_PUMP_1].time_id == NULL) {
        LOGE("Failed to allocate timer");
        return -1;
    }
    timer_info[MOTOR_PWM_PUMP_2].time_id = hwtimer_alloc(bsp_motor_pump_2_timer_timeout_handler, NULL);
    if (timer_info[MOTOR_PWM_PUMP_2].time_id == NULL) {
        LOGE("Failed to allocate timer");
        return -1;
    }
    return 0;
}

static void bsp_motor_deinit_timer_system(void)
{
    bsp_motor_stop_periodic_task(MOTOR_PWM_PUMP_1);
    bsp_motor_stop_periodic_task(MOTOR_PWM_PUMP_2);
    if (timer_info[MOTOR_PWM_PUMP_1].time_id != NULL) {
        hwtimer_free(timer_info[MOTOR_PWM_PUMP_1].time_id);
        timer_info[MOTOR_PWM_PUMP_1].time_id = NULL;
    }
    if (timer_info[MOTOR_PWM_PUMP_2].time_id != NULL) {
        hwtimer_free(timer_info[MOTOR_PWM_PUMP_2].time_id);
        timer_info[MOTOR_PWM_PUMP_2].time_id = NULL;
    }
}

static int8_t bsp_motor_pwm_gradual_change_start(motor_para_t *motor_para, motor_pwm_control_id_e pwm_id)
{
    if (pwm_id >= MOTOR_PWM_ID_MAX) {
        LOGE("pwm_id is out of range: %d", pwm_id);
        return -1;
    }
    //不需要渐变，设置后直接退出函数，并返回错误码
    if (pwm_id == MOTOR_PWM_VALVE_1 || pwm_id == MOTOR_PWM_VALVE_2) {
        motor_para_t *motor = (motor_para_t *)motor_para;
        struct HAL_PWM_CFG_T pwm_cfg = {
            .freq = motor->un.pwm.freq,
            .ratio = motor->un.pwm.duty,
            .inv = false,
            .sleep_on = false,
        };
        return hal_pwm_enable(motor_pwm_cfg[pwm_id].id, &pwm_cfg);
    } else { ///需要渐变，设置渐变参数
        //先以初始占空比启动PWM，然后根据目标占空比和变化时间计算出步长，
        // 然后启动定时器，定时器到期后，将占空比增加步长，直到达到目标占空比
        motor_para_t *motor = (motor_para_t *)motor_para;
        struct HAL_PWM_CFG_T pwm_cfg = {
            .freq = motor->un.pwm.freq,
            .ratio = PUMP_PWM_GRADUAL_CHANGE_INIT_DUTY,
            .inv = false,
            .sleep_on = false,
        };
        int ret = hal_pwm_enable(motor_pwm_cfg[pwm_id].id, &pwm_cfg);
        if (ret != 0) {
            LOGE("Failed to enable PWM: %d", pwm_id);
            return ret;
        }

        timer_info[pwm_id].pwm_cfg.freq = motor_para->un.pwm.freq;
        timer_info[pwm_id].pwm_cfg.ratio = motor_para->un.pwm.duty;
        timer_info[pwm_id].pwm_cfg.inv = false;
        timer_info[pwm_id].pwm_cfg.sleep_on = false;
        timer_info[pwm_id].init_duty = PUMP_PWM_GRADUAL_CHANGE_INIT_DUTY;
        timer_info[pwm_id].target_duty = motor_para->un.pwm.duty;
        timer_info[pwm_id].current_duty = PUMP_PWM_GRADUAL_CHANGE_INIT_DUTY; // 初始化当前占空比
        timer_info[pwm_id].duty_step = 0; // 初始化步长
        timer_info[pwm_id].time_period = PUMP_PWM_GRADUAL_CHANGE_PERIOD; // 初始化定时器周期
        
        // 计算步长和定时器周期
        // 变化时间与目标占空比成比例：change_time = target_duty * SCALE
        // 步数 = 总变化时间 / 定时器周期
        uint16_t change_time = timer_info[pwm_id].target_duty * PUMP_PWM_GRADUAL_CHANGE_TIME_SCALE; // 总变化时间（ms），与占空比成正比
        uint16_t time_period = PUMP_PWM_GRADUAL_CHANGE_PERIOD; // 定时器周期（ms）
        int16_t duty_diff = (int16_t)timer_info[pwm_id].target_duty - (int16_t)timer_info[pwm_id].init_duty;
        
        // 计算步数（向上取整，确保能覆盖整个变化范围）
        uint16_t step_count = (change_time + time_period - 1) / time_period; // 向上取整
        if (step_count == 0) {
            step_count = 1; // 至少1步
        }
        
        // 计算步长（无符号，总是增加）
        // 默认目标占空比不会小于初始占空比
        if (duty_diff > 0) {
            // 向上取整：(duty_diff + step_count - 1) / step_count
            timer_info[pwm_id].duty_step = (duty_diff + step_count - 1) / step_count;
        } else if (duty_diff == 0) {
            timer_info[pwm_id].duty_step = 0; // 无需变化
        } else {
            // 目标占空比小于初始占空比（异常情况），默认以初始占空比启动PWM
            LOGE("target_duty (%d) < init_duty (%d), this should not happen!", 
                 timer_info[pwm_id].target_duty, timer_info[pwm_id].init_duty);
            timer_info[pwm_id].duty_step = 0;
            return 0;
        }
        
        // 设置定时器周期
        timer_info[pwm_id].time_period = time_period;
        
        // 计算定时器最大可能运行时间
        // 注意：定时器会在 current_duty >= target_duty 时提前停止
        // 所以实际运行时间可能小于 step_count * time_period
        // total_time 表示最大可能的运行时间（如果所有步都执行完）
        uint16_t total_time_max = step_count * time_period;
        
        // 估算实际运行时间（基于步长计算）
        // 实际需要的步数 = ceil(duty_diff / duty_step)
        uint16_t actual_steps_needed = 0;
        if (duty_diff > 0 && timer_info[pwm_id].duty_step > 0) {
            actual_steps_needed = (duty_diff + timer_info[pwm_id].duty_step - 1) / timer_info[pwm_id].duty_step;
        }
        uint16_t total_time_estimated = actual_steps_needed * time_period;
        
        LOGI("PWM gradual change: init=%d, target=%d, diff=%d, change_time=%dms, step_count=%d, step=%d, period=%dms",
             timer_info[pwm_id].init_duty, timer_info[pwm_id].target_duty, duty_diff,
             change_time, step_count, timer_info[pwm_id].duty_step, timer_info[pwm_id].time_period);
        LOGI("  Time estimate: max=%dms, estimated=%dms (actual_steps_needed=%d)",
             total_time_max, total_time_estimated, actual_steps_needed);
        
        bsp_motor_start_periodic_task(pwm_id);
        return 0; // 成功
    }
}
#endif
/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int bsp_motor_init(void)
{
    for (uint8_t i = 0; i < LENGTH_OF_ARRAY(motor_pwm_cfg); i++) {
        motor_pwm_cfg[i].iomux_set();
    }
    memset(motor_pwm_status, 0, sizeof(motor_pwm_status));
#if IS_SUPPORT_PUMP_PWM_GRADUAL_CHANGE
    if (bsp_motor_init_timer_system() != 0) {
        LOGE("Failed to initialize timer system");
        return BSP_ERROR;
    }
#endif
    return BSP_OK;
}

int bsp_motor_deinit(void)
{
    for (int i = 0; i < ARRAY_SIZE(motor_pwm_cfg); i++) {
        hal_pwm_disable(motor_pwm_cfg[i].id);
    }
#if IS_SUPPORT_PUMP_PWM_GRADUAL_CHANGE
    bsp_motor_deinit_timer_system();
#endif
    return BSP_OK;
}

int bsp_motor_parameter_set(void *args)
{
    uint32_t *tmp = (uint32_t *)args;
    motor_pwm_control_id_e pwm_id = (motor_pwm_control_id_e )tmp[0];
    if (pwm_id >= MOTOR_PWM_ID_MAX) {
        LOGE("pwm_id is out of range: %d", pwm_id);
        return -1;
    }
    int ret = 0;
#if IS_SUPPORT_PUMP_PWM_GRADUAL_CHANGE
    ret = bsp_motor_pwm_gradual_change_start((motor_para_t *)tmp[1], pwm_id);
#else
    motor_para_t *motor = (motor_para_t *)tmp[1];
    struct HAL_PWM_CFG_T pwm_cfg = {
        .freq = motor->un.pwm.freq,
        .ratio = motor->un.pwm.duty,
        .inv = false,
        .sleep_on = false,
    };
    ret = hal_pwm_enable(motor_pwm_cfg[pwm_id].id, &pwm_cfg);
#endif
#if IS_SUPPORT_PUMP_RUN_TIME_TEST
    if (pwm_id == MOTOR_PWM_PUMP_1) {
        start_time = get_timestamp();
    }
#endif
    motor_pwm_status[pwm_id] = 1;
#if IS_SUPPORT_PUMP_DEBUG_LOG
    BSP_LOG("parameter", "pwm_id: %d, duty: %d, freq: %d, ret: %d", pwm_id, 
                motor->un.pwm.duty, motor->un.pwm.freq, ret);
#endif
    return ret;
}

int bsp_motor_stop(void *args)
{
    int ret = 0;
    motor_pwm_control_id_e pwm_id = *(motor_pwm_control_id_e *)args;
#if IS_SUPPORT_PUMP_DEBUG_LOG
    BSP_LOG("stop", "pwm_id: %d", pwm_id);
#endif
    // BSP_LOG("stop", "pwm_id: %d", pwm_id);
    if (pwm_id >= MOTOR_PWM_ID_MAX) {
#if IS_SUPPORT_PUMP_PWM_GRADUAL_CHANGE
        bsp_motor_stop_periodic_task(MOTOR_PWM_PUMP_1);
        bsp_motor_stop_periodic_task(MOTOR_PWM_PUMP_2);
#endif
        for (uint8_t i = 0; i < MOTOR_PWM_ID_MAX; i++) {
            ret |= hal_pwm_disable(motor_pwm_cfg[i].id);
            motor_pwm_status[i] = 0;
        }
        return ret;
    }
#if IS_SUPPORT_PUMP_PWM_GRADUAL_CHANGE
    bsp_motor_stop_periodic_task(pwm_id);
#endif
    ret = hal_pwm_disable(motor_pwm_cfg[pwm_id].id);
#if IS_SUPPORT_PUMP_RUN_TIME_TEST
    if (pwm_id == MOTOR_PWM_PUMP_1) {
        if (count < 20) {
            run_time[count] = get_timestamp() - start_time;
            count++;
        } else {
            PRINT_LOG("run_time %d: ", TICKS_TO_MS(run_time[0]));
            for (uint8_t i = 0; i < 20; i++) {
                PRINT_LOG("%d ", run_time[i]);
            }
            PRINT_LOG("\r\n");
            count = 0;
        }
    }
#endif
    motor_pwm_status[pwm_id] = 0;
    return ret;
}

int bsp_motor_start(void *args)
{
    bsp_motor_parameter_set(args);
    return BSP_OK;
}

int bsp_motor_is_running(void *args)
{
    motor_pwm_control_id_e pwm_id = *(motor_pwm_control_id_e *)args;
    if (pwm_id >= MOTOR_PWM_ID_MAX) {
        return -1;
    }
    return motor_pwm_status[pwm_id] > 0 ? 1 : 0;
}

#if IS_SUPPORT_PUMP_PWM_GRADUAL_CHANGE
/**
 * @brief 测试所有可能的步数组合
 * 系统地测试各种 step_count 值（1-128），确保计算逻辑正确
 */
void bsp_motor_pwm_test_all_step_counts(void)
{
    LOGI("========== PWM Step Count Comprehensive Test ==========");
    LOGI("Config: INIT_DUTY=%d, PERIOD=%dms, SCALE=%d", 
         PUMP_PWM_GRADUAL_CHANGE_INIT_DUTY, 
         PUMP_PWM_GRADUAL_CHANGE_PERIOD,
         PUMP_PWM_GRADUAL_CHANGE_TIME_SCALE);
    LOGI("Formula: step_count = (target_duty * SCALE + PERIOD - 1) / PERIOD");
    LOGI("Testing all step_count values from 1 to 128");
    LOGI("------------------------------------------------------");
    
    uint8_t init_duty = PUMP_PWM_GRADUAL_CHANGE_INIT_DUTY;
    uint16_t time_period = PUMP_PWM_GRADUAL_CHANGE_PERIOD;
    uint16_t scale = PUMP_PWM_GRADUAL_CHANGE_TIME_SCALE;
    
    int pass_count = 0;
    int fail_count = 0;
    
    // 测试所有可能的 step_count 值（1-128）
    for (uint16_t step_count = 1; step_count <= 128; step_count++) {
        // 根据 step_count 反推 target_duty
        // change_time = step_count * time_period (最小值，向下取整)
        // target_duty = change_time / scale
        // 为了得到特定的 step_count，需要满足：
        // (target_duty * scale + time_period - 1) / time_period == step_count
        // 即：step_count * time_period - time_period + 1 <= target_duty * scale < step_count * time_period + time_period
        // 所以：target_duty >= (step_count * time_period - time_period + 1) / scale
        //      target_duty < (step_count * time_period + time_period) / scale
        
        uint16_t min_change_time = step_count * time_period - time_period + 1;
        uint16_t max_change_time = step_count * time_period + time_period - 1;
        uint8_t min_target_duty = (min_change_time + scale - 1) / scale; // 向上取整
        uint8_t max_target_duty = max_change_time / scale; // 向下取整
        
        // 选择一个代表性的 target_duty（取中间值）
        uint8_t target_duty = (min_target_duty + max_target_duty) / 2;
        if (target_duty < init_duty) {
            target_duty = init_duty + 1;
        }
        if (target_duty > 255) {
            target_duty = 255;
        }
        
        // 验证计算
        uint16_t change_time = target_duty * scale;
        uint16_t calculated_step_count = (change_time + time_period - 1) / time_period;
        if (calculated_step_count == 0) {
            calculated_step_count = 1;
        }
        
        int16_t duty_diff = (int16_t)target_duty - (int16_t)init_duty;
        uint8_t duty_step = 0;
        if (duty_diff > 0) {
            duty_step = (duty_diff + calculated_step_count - 1) / calculated_step_count;
        }
        
        // 模拟执行验证
        uint8_t final_duty = init_duty;
        uint16_t actual_steps = 0;
        for (uint16_t j = 0; j < calculated_step_count && final_duty < target_duty; j++) {
            uint8_t new_duty = final_duty + duty_step;
            if (new_duty > target_duty) {
                new_duty = target_duty;
            }
            final_duty = new_duty;
            actual_steps++;
            if (final_duty >= target_duty) {
                break; // 达到目标，提前停止（模拟定时器行为）
            }
        }
        
        // 计算实际运行时间（定时器会在达到目标时提前停止）
        // total_time = actual_steps * time_period（实际执行的步数 * 周期）
        uint16_t total_time = actual_steps * time_period;
        uint16_t total_time_max = calculated_step_count * time_period; // 最大可能时间
        
        bool can_reach_target = (final_duty >= target_duty);
        bool step_count_match = (calculated_step_count == step_count);
        
        if (step_count_match && can_reach_target) {
            pass_count++;
            // 只输出部分结果，避免日志过多
            if (step_count <= 20 || step_count % 10 == 0 || step_count == 128) {
                LOGI("step_count=%3d: target_duty=%3d, change_time=%3dms, duty_diff=%3d, duty_step=%2d, final=%3d", 
                     step_count, target_duty, change_time, duty_diff, duty_step, final_duty);
                LOGI("  actual_steps=%3d, total_time=%3dms (max=%3dms) [PASS]", 
                     actual_steps, total_time, total_time_max);
            }
        } else {
            fail_count++;
            LOGI("step_count=%3d: target_duty=%3d, calculated_step_count=%3d, can_reach=%d [FAIL]",
                 step_count, target_duty, calculated_step_count, can_reach_target ? 1 : 0);
        }
    }
    
    LOGI("------------------------------------------------------");
    LOGI("========== Test Summary ==========");
    LOGI("Total tested: 128 (step_count from 1 to 128)");
    LOGI("Passed: %d", pass_count);
    LOGI("Failed: %d", fail_count);
    LOGI("Success rate: %.2f%%", (float)pass_count * 100.0f / 128);
    LOGI("==================================");
    
    // 输出一些关键步数的详细信息
    LOGI("------------------------------------------------------");
    LOGI("Key step_count values detail:");
    uint16_t key_steps[] = {1, 2, 5, 10, 20, 25, 50, 75, 100, 128};
    for (uint32_t i = 0; i < sizeof(key_steps) / sizeof(key_steps[0]); i++) {
        uint16_t step = key_steps[i];
        if (step > 128) continue;
        
        // 计算对应的 target_duty
        uint16_t min_change_time = step * time_period - time_period + 1;
        uint8_t target_duty = (min_change_time + scale - 1) / scale;
        if (target_duty < init_duty) target_duty = init_duty + 1;
        if (target_duty > 255) target_duty = 255;
        
        uint16_t change_time = target_duty * scale;
        uint16_t calculated_step_count = (change_time + time_period - 1) / time_period;
        if (calculated_step_count == 0) calculated_step_count = 1;
        int16_t duty_diff = (int16_t)target_duty - (int16_t)init_duty;
        uint8_t duty_step = 0;
        if (duty_diff > 0) {
            duty_step = (duty_diff + calculated_step_count - 1) / calculated_step_count;
        }
        
        // 计算实际需要的步数（定时器会在达到目标时提前停止）
        uint16_t actual_steps_needed = 0;
        if (duty_diff > 0 && duty_step > 0) {
            actual_steps_needed = (duty_diff + duty_step - 1) / duty_step;
        }
        uint16_t total_time = actual_steps_needed * time_period; // 实际运行时间
        uint16_t total_time_max = calculated_step_count * time_period; // 最大可能时间
        
        LOGI("  step_count=%3d: target_duty=%3d, change_time=%3dms, duty_diff=%3d, duty_step=%2d",
             step, target_duty, change_time, duty_diff, duty_step);
        LOGI("    actual_steps_needed=%3d, total_time=%3dms (max=%3dms)",
             actual_steps_needed, total_time, total_time_max);
    }
    LOGI("==================================");
}
#endif