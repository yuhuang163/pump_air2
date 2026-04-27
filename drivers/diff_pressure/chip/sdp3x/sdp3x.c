/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "sdp3x.h" // IWYU pragma: keep
#include "bsp_common.h"
#include "bsp_config.h"
#include "bsp_internal.h" // IWYU pragma: keep
#include "bsp_i2c.h" // IWYU pragma: keep
#include "hal_diff_pressure.h"
#include "stdint.h"
#include "stdbool.h"


/*
* DEFINES
*****************************************************************************************
*/
#define SDP3X_I2C_ADDR                  0x21
#define SDP3X_LOG_DEBUG                 1
#define SDP3X_ABNORMAL_MAX_COUNT        10
#define SDP3X_LOG_COUNT_INTERVAL        100


/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static float s_temp[AIR_PRESSURE_NUM] = {0};
static uint8_t s_temp_is_ready[AIR_PRESSURE_NUM] = {0};
static uint8_t s_dev_is_ready[AIR_PRESSURE_NUM] = {0};
static uint16_t s_dev_abnormal_count[AIR_PRESSURE_NUM] = {0};
static uint16_t s_log_count = 0;


static uint8_t s_sdp3x_initialized = 0;
static uint8_t s_sdp3x_measurement_started = 0;
static uint8_t s_sdp3x_i2c_address = 0x21; // Current I2C address
static float s_last_diff_pressure = 0.0f;
static float s_last_temperature = 0.0f;

static int ExecuteCommand(Command cmd);
static int ReadMeasurementRawResults(int16_t* diffPressureTicks,
                                       int16_t* temperatureTicks,
                                       uint16_t* scaleFactor);
static int CheckCrc(const uint8_t data[], uint8_t size, uint8_t checksum);

static const float scaleFactorTemperature = 200;

static uint8_t _i2cAddress;


// Flow algorithm context and data buffer
// Place large arrays in .volatile_ram section (RAM) instead of NVRAM to avoid overflow
// static FlowContext s_flow_ctx __attribute__((section(".volatile_ram")));
#define PRESSURE_BUFFER_SIZE 50   // Buffer size for batch processing (优化：从100减到50，0.5秒数据)
static float s_pressure_buffer[PRESSURE_BUFFER_SIZE] __attribute__((section(".volatile_ram")));
static uint16_t s_pressure_buffer_index = 0;  // 优化：使用uint16_t
// static FlowResult s_flow_results[5] __attribute__((section(".volatile_ram")));  // Buffer for flow algorithm results (优化：从10减到5)
// static FlowResult s_latest_flow_results[5] __attribute__((section(".volatile_ram")));  // Store latest results for external access (优化：从10减到5)
static uint8_t s_latest_flow_results_count = 0;  // Number of latest results (优化：使用uint8_t)

// Pressure baseline calibration (用于去除传感器偏移，使压力围绕0波动)
static float s_pressure_baseline = 0.0f;  // 基准压力值
static uint8_t s_baseline_calibrated = 0;  // 基准值是否已校准
#define BASELINE_SAMPLES 10  // 用于计算基准值的采样数量


/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static int sdp3x_write_nbyte(uint8_t index, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int ret = 0;

    if (index >= AIR_PRESSURE_NUM) {
        LOGE("pressure sensor index is invalid: %d", index);
        return BSP_INVALID_PARAMETER;
    }

    if (s_dev_is_ready[index] == 0) {
        if (s_log_count % SDP3X_LOG_COUNT_INTERVAL == 0) {
            LOGE("pressure sensor is not ready: %d", index);
        }
        return BSP_NOT_READY;
    }
    
    i2c_master_data_t i2c_data = {
        .dev_address = SDP3X_I2C_ADDR,
        .reg_addr = &reg_addr,
        .reg_addr_len = 1,
        .data = data,
        .len = len,
    };

    for (uint8_t i = 0; i < 3; i++) {
        ret = bsp_i2c_write_byte(AIR_PRESSURE_I2C_ID1, &i2c_data);
        if (ret == BSP_OK) {
            s_dev_abnormal_count[index] = 0;
            return BSP_OK;
        }
        msleep(10);
    }

    s_dev_abnormal_count[index]++;
    if (s_dev_abnormal_count[index] > SDP3X_ABNORMAL_MAX_COUNT) {
        s_dev_is_ready[index] = 0;
    } else {
        LOG_PBUS("pressure sensor %d abnormal count: %d", index, s_dev_abnormal_count[index]);
    }

    return BSP_ERROR;
}

static int sdp3x_read_nbyte(uint8_t index, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int ret = 0;
    
    if (index >= AIR_PRESSURE_NUM) {
        LOGE("pressure sensor index is invalid: %d", index);
        return BSP_INVALID_PARAMETER;
    }
    
    if (s_dev_is_ready[index] == 0) {
        if (s_log_count % SDP3X_LOG_COUNT_INTERVAL == 0) {
            LOGE("pressure sensor is not ready: %d", index);
        }
        return BSP_NOT_READY;
    }

    i2c_master_data_t i2c_data = {
        .dev_address = SDP3X_I2C_ADDR,
        .reg_addr = &reg_addr,
        .reg_addr_len = 0,
        .data = data,
        .len = len,
    };

    for (uint8_t i = 0; i < 3; i++) {
        ret = bsp_i2c_read_byte(AIR_PRESSURE_I2C_ID1, &i2c_data);
        if (ret == BSP_OK) {
            s_dev_abnormal_count[index] = 0;
            return BSP_OK;
        }
        msleep(10);
    }

    s_dev_abnormal_count[index]++;
    if (s_dev_abnormal_count[index] > SDP3X_ABNORMAL_MAX_COUNT) {
        s_dev_is_ready[index] = 0;
    } else {
        LOG_PBUS("pressure sensor %d abnormal count: %d", index, s_dev_abnormal_count[index]);
    }

    return BSP_ERROR;
}

static int sdp3x_read_nbyte_no_retry(uint8_t index, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int ret = 0;
    if (index >= AIR_PRESSURE_NUM) {
        LOGE("pressure sensor index is invalid: %d", index);
        return BSP_INVALID_PARAMETER;
    }

    i2c_master_data_t i2c_data = {
        .dev_address = SDP3X_I2C_ADDR,
        .reg_addr = &reg_addr,
        .reg_addr_len = 0,
        .data = data,
        .len = len,
    };

    ret = bsp_i2c_read_byte(AIR_PRESSURE_I2C_ID1, &i2c_data);
    if (ret == BSP_OK) {
        s_dev_abnormal_count[index] = 0;
        return BSP_OK;
    }

    s_dev_abnormal_count[index]++;
    if (s_dev_abnormal_count[index] > SDP3X_ABNORMAL_MAX_COUNT) {
        s_dev_is_ready[index] = 0;
    } else {
        LOG_PBUS("pressure sensor %d abnormal count: %d", index, s_dev_abnormal_count[index]);
    }

    return BSP_ERROR;
}

//-----------------------------------------------------------------------------
int app_sdp3x_init(uint8_t i2cAddress)
{
    if (s_sdp3x_initialized) {
        LOG_PBUS("SDP3x already initialized\n");
        return 0;
    }

    // Initialize I2C first
    bsp_i2c_init(i2cAddress);
    msleep(10); // 10ms delay after I2C init

    Sdp3x_Init(i2cAddress);
    s_sdp3x_i2c_address = i2cAddress;

    int ret = Sdp3x_ReadMeasurementResults(&s_last_diff_pressure, &s_last_temperature);
    LOG_PBUS("SDP3x initialized successfully, pressure: %f, temperature: %f", s_last_diff_pressure, s_last_temperature);
    if (ret != BSP_OK) {
        LOGE("SDP3x initialized failed!!!");
        return BSP_ERROR;
    }

    // Initialize flow algorithm context
    // flow_algo_init(&s_flow_ctx);
    s_pressure_buffer_index = 0;
    
    // Reset baseline calibration
    s_pressure_baseline = 0.0f;
    s_baseline_calibrated = 0;

    s_sdp3x_initialized = 1;

    return 0;
}

//-----------------------------------------------------------------------------
int app_sdp3x_start_measurement(Sdp3xTempComp tempComp, Sdp3xAveraging averaging)
{
    if (!s_sdp3x_initialized) {
        LOGE("pressure sensor not initialized");
        return BSP_ERROR;
    }

    return Sdp3x_StartContinousMeasurement(tempComp, averaging);
}

//-----------------------------------------------------------------------------
int app_sdp3x_stop_measurement(void)
{
    if (!s_sdp3x_initialized) {
        LOGE("pressure sensor not initialized");
        return BSP_ERROR;
    }

    return Sdp3x_StopContinousMeasurement();
}

//-----------------------------------------------------------------------------
int app_sdp3x_read_results(float* diffPressure, float* temperature)
{
    if (!s_sdp3x_initialized) {
        LOGE("pressure sensor not initialized");
        return BSP_ERROR;
    }

    if (diffPressure == NULL || temperature == NULL) {
        return BSP_INVALID_PARAMETER;
    }

    return Sdp3x_ReadMeasurementResults(diffPressure, temperature);
}

//-----------------------------------------------------------------------------
/// \brief  Start continuous measurement (called once after initialization)
/// \return Error code (ERROR_NONE on success)
int app_sdp3x_start_continuous_measurement(void)
{
    if (!s_sdp3x_initialized) {
        LOGE("pressure sensor not initialized");
        return BSP_ERROR;
    }

    if (s_sdp3x_measurement_started) {
        return BSP_ERROR; // Already started
    }

    // Try starting measurement without reset first (sensor might already be ready)
    LOG_PBUS("Attempting to start measurement without reset...\n");
    int ret = Sdp3x_StartContinousMeasurement(SDP3X_TEMPCOMP_MASS_FLOW,
                                                  SDP3X_AVERAGING_TILL_READ);
    if (ret == BSP_OK) {
        s_sdp3x_measurement_started = 1;
        LOG_PBUS("SDP3x continuous measurement started successfully (no reset needed)\n");
        return BSP_OK;
    }

    // If failed, skip reset and try multiple times with delays
    // (Reset might put sensor in a state where it doesn't respond to address)
    LOG_PBUS("Start failed without reset, trying with delays...\n");

    // Try multiple times with increasing delays (without reset)
    uint8_t retry_count = 5;
    for (uint8_t retry = 0; retry < retry_count; retry++) {
        if (retry > 0) {
            LOG_PBUS("Retry %d/%d (waiting %dms)...\n", retry + 1, retry_count, (retry + 1) * 50);
            msleep((retry + 1) * 50); // Increasing delay: 50ms, 100ms, 150ms, etc.
        }

        ret = Sdp3x_StartContinousMeasurement(SDP3X_TEMPCOMP_MASS_FLOW,
                                              SDP3X_AVERAGING_TILL_READ);

        if (ret == BSP_OK) {
            break; // Success, exit retry loop
        }
    }

    // If still failed, try with reset as last resort
    if (ret != BSP_OK) {
        LOG_PBUS("All retries failed, trying with reset...\n");
        LOG_PBUS("Resetting SDP3x sensor...\n");
        int reset_err = Sdp3x_SoftReset();
        if (reset_err != BSP_OK) {
            LOG_PBUS("SDP3x reset failed: %d\n", reset_err);
        } else {
            LOG_PBUS("SDP3x reset successful\n");
        }

        // Wait longer after reset
        msleep(200); // 200ms delay after reset

        // Try one more time after reset
        ret = Sdp3x_StartContinousMeasurement(SDP3X_TEMPCOMP_MASS_FLOW,
                                              SDP3X_AVERAGING_TILL_READ);
    }
    if (ret == BSP_OK) {
        s_sdp3x_measurement_started = 1;
        LOG_PBUS("SDP3x continuous measurement started successfully\n");
    } else {
        LOG_PBUS("SDP3x start measurement failed: %d", ret);
        LOG_PBUS(" (ERROR_ACK - No response from sensor)");
        LOG_PBUS("Current I2C address: 0x%02X\n", s_sdp3x_i2c_address);
        LOG_PBUS("Possible causes:");
        LOG_PBUS("  1) I2C address mismatch (current: 0x%02X, try: 0x%02X)",
                        s_sdp3x_i2c_address, (s_sdp3x_i2c_address == 0x21) ? 0x25 : 0x21);
        LOG_PBUS("  2) I2C wiring issue (SDA/SCL on PA08/PA07)");
        LOG_PBUS("  3) Sensor not powered (need 3.3V)");
        LOG_PBUS("  4) Sensor needs more time after reset");
    }

    return ret;
}
#if 0
//-----------------------------------------------------------------------------
/// \brief  Read measurement results periodically (called every 100ms)
/// \return Error code (ERROR_NONE on success)
int app_sdp3x_periodic_read(void)
{
    if (!s_sdp3x_initialized) {
        return ERROR_IVALID_PARAMETER;
    }

    if (!s_sdp3x_measurement_started) {
        // Try to start measurement if not started
        int err = app_sdp3x_start_continuous_measurement();
        if (err != ERROR_NONE) {
            return err;
        }
    }

    int err = Sdp3x_ReadMeasurementResults(&s_last_diff_pressure, &s_last_temperature);
    if (err == ERROR_NONE) {
        // Calibrate baseline pressure (使用前几个采样值计算基准值)
        static uint8_t baseline_sample_count = 0;
        static float baseline_sum = 0.0f;
        if (!s_baseline_calibrated && baseline_sample_count < BASELINE_SAMPLES) {
            baseline_sum += s_last_diff_pressure;
            baseline_sample_count++;
            if (baseline_sample_count >= BASELINE_SAMPLES) {
                s_pressure_baseline = baseline_sum / BASELINE_SAMPLES;
                s_baseline_calibrated = 1;
                #if (DEBUG_SW == 1)
                int32_t baseline_int = (int32_t)(s_pressure_baseline * 100.0f);
                LOG_PBUS("Pressure baseline calibrated: %d.%02d Pa\n",
                             baseline_int / 100, baseline_int % 100);
                #endif
            }
        }
        
        // Calculate relative pressure (减去基准值，使压力围绕0波动)
        float relative_pressure = s_last_diff_pressure - s_pressure_baseline;
        
        // Successfully read measurement results
        // Add pressure data to buffer (repeat value 5 times to match 10ms sampling interval)
        // Sensor reads every 100ms, algorithm expects 10ms interval, so we repeat each value 5 times
        // 优化：从10次重复减到5次，减少缓冲区填充速度
        // 使用相对压力而不是绝对压力，这样压力可以围绕0波动，产生正负值
        for (int i = 0; i < 5 && s_pressure_buffer_index < PRESSURE_BUFFER_SIZE; i++) {
            s_pressure_buffer[s_pressure_buffer_index++] = relative_pressure;
        }

        // When buffer is full (50 points = 0.5 second of data), process with flow algorithm
        // 算法会固定每秒输出一次结果，不依赖过零点检测
        if (s_pressure_buffer_index >= PRESSURE_BUFFER_SIZE) {
            #if (DEBUG_SW == 1)
            // Debug: Print pressure data range before processing
            float min_p = s_pressure_buffer[0], max_p = s_pressure_buffer[0];
            int positive_count = 0, negative_count = 0, zero_crossings = 0;
            for (int i = 0; i < PRESSURE_BUFFER_SIZE; i++) {
                if (s_pressure_buffer[i] < min_p) min_p = s_pressure_buffer[i];
                if (s_pressure_buffer[i] > max_p) max_p = s_pressure_buffer[i];
                if (s_pressure_buffer[i] >= 0) positive_count++;
                else negative_count++;
                // Count potential zero crossings (sign changes)
                if (i > 0 && ((s_pressure_buffer[i-1] >= 0 && s_pressure_buffer[i] < 0) ||
                              (s_pressure_buffer[i-1] < 0 && s_pressure_buffer[i] >= 0))) {
                    zero_crossings++;
                }
            }
            int32_t min_p_int = (int32_t)(min_p * 100.0f);
            int32_t max_p_int = (int32_t)(max_p * 100.0f);
            LOG_PBUS("Pressure buffer: min=%d.%02d, max=%d.%02d Pa, pos=%d, neg=%d, zc=%d\n",
                         min_p_int / 100, min_p_int % 100,
                         max_p_int / 100, max_p_int % 100,
                         positive_count, negative_count, zero_crossings);
            #endif
            
            // Process the batch of pressure data
            int result_count = process_pressure_batch(&s_flow_ctx,
                                                      s_pressure_buffer,
                                                      PRESSURE_BUFFER_SIZE,
                                                      s_flow_results,
                                                      sizeof(s_flow_results) / sizeof(s_flow_results[0]));

            // Reset buffer index for next batch
            s_pressure_buffer_index = 0;

            // Store latest results for external access
            s_latest_flow_results_count = result_count;
            int max_copy = (result_count < 5) ? result_count : 5;  // 优化：使用5而不是硬编码10
            for (int i = 0; i < max_copy; i++) {
                s_latest_flow_results[i] = s_flow_results[i];
            }

            // Handle algorithm results (e.g., log or store them)
            #if (DEBUG_SW == 1)
            // Convert float to integer for display
            int32_t ctx_time_int = (int32_t)(s_flow_ctx.current_time * 100.0f);
            int32_t cumsum_int = (int32_t)(s_flow_ctx.current_cumsum * 100.0f);
            LOG_PBUS("Flow algorithm: result_count=%d, ctx_time=%d.%02ds, cumsum=%d.%02d\n",
                         result_count,
                         ctx_time_int / 100, ctx_time_int % 100,
                         cumsum_int / 100, cumsum_int % 100);
            for (int i = 0; i < result_count; i++) {
                // Convert float to integer for display (timestamp: s*100, volume: mL*100, rate: mL/min*100)
                int32_t time_int = (int32_t)(s_flow_results[i].timestamp * 100.0f);
                int32_t vol_int = (int32_t)(s_flow_results[i].volume * 100.0f);
                int32_t rate_int = (int32_t)(s_flow_results[i].flow_rate * 100.0f);
                LOG_PBUS("Flow Result[%d]: t=%d.%02ds, vol=%d.%02dmL, rate=%d.%02dmL/min\n",
                             i,
                             time_int / 100, time_int % 100,
                             vol_int / 100, vol_int % 100,
                             rate_int / 100, rate_int % 100);
            }
            #endif
            
            // Send flow algorithm results via Bluetooth
            if (result_count > 0) {
                app_sdp3x_ble_report_flow_data(s_flow_results, result_count);
            }
        }

        #if (DEBUG_SW == 1)
        // Convert float to integer for display (pressure: Pa*100, temperature: C*10)
        int32_t pressure_int = (int32_t)(s_last_diff_pressure * 100.0f);
        int32_t temp_int = (int32_t)(s_last_temperature * 10.0f);
        LOG_PBUS("Pressure: %d.%02d Pa, Temperature: %d.%d C\n",
                     pressure_int / 100, pressure_int % 100,
                     temp_int / 10, temp_int % 10);
        #endif
    } else if (err == ERROR_CHECKSUM) {
        LOG_PBUS("SDP3x checksum error\n");
    } else if (err == ERROR_ACK) {
        LOG_PBUS("SDP3x ACK error, restarting measurement\n");
        s_sdp3x_measurement_started = 0; // Reset flag to restart measurement
    }

    return err;
}
#endif


//-----------------------------------------------------------------------------
/// \brief  Get last read differential pressure
/// \return Last differential pressure value in Pa
float app_sdp3x_get_last_pressure(void)
{
    return s_last_diff_pressure;
}

//-----------------------------------------------------------------------------
/// \brief  Get last read temperature
/// \return Last temperature value in Celsius
float app_sdp3x_get_last_temperature(void)
{
    return s_last_temperature;
}
#if 0
//-----------------------------------------------------------------------------
/// \brief  Get flow algorithm results (volume and flow rate)
/// \param  results  Pointer to array to store results
/// \param  max_results  Maximum number of results to retrieve
/// \return Number of results retrieved
int app_sdp3x_get_flow_results(FlowResult* results, int max_results)
{
    if (!results || max_results <= 0) {
        return 0;
    }

    int count = (s_latest_flow_results_count < max_results) ?
                s_latest_flow_results_count : max_results;

    // for (int i = 0; i < count; i++) {
    //     results[i] = s_latest_flow_results[i];
    // }

    return count;
}

//-----------------------------------------------------------------------------
/// \brief  Send flow algorithm results via Bluetooth
/// \param  results  Pointer to flow results array
/// \param  count    Number of results to send
/// \note   Data format: [count(1B)][timestamp(4B)][volume(4B)][flow_rate(4B)] for each result
///         Values are multiplied by 1000 and sent as uint32_t (little-endian)
void app_sdp3x_ble_report_flow_data(FlowResult* results, int count)
{
    if (!results || count <= 0) {
        return;
    }
    
    // Limit count to avoid buffer overflow (each result is 12 bytes: 3 floats)
    // BLE_SEND_MAX_LEN = 120, so max 9 results per packet (120/13 = 9, with 1 byte for count)
    int max_results_per_packet = 9;
    if (count > max_results_per_packet) {
        count = max_results_per_packet;
    }
    
    // Prepare data buffer: [count(1B)][timestamp(4B)][volume(4B)][flow_rate(4B)] for each result
    uint8_t data_buf[BLE_SEND_MAX_LEN];
    uint8_t data_len = 0;
    
    // First byte: result count
    data_buf[data_len++] = count;
    
    for (int i = 0; i < count; i++) {
        // Convert float to uint32_t (multiply by 1000 for 3 decimal places)
        uint32_t timestamp = (uint32_t)(results[i].timestamp * 1000.0f);
        uint32_t volume = (uint32_t)(results[i].volume * 1000.0f);
        uint32_t flow_rate = (uint32_t)(results[i].flow_rate * 1000.0f);
        
        // Pack data in little-endian format
        data_buf[data_len++] = timestamp & 0xFF;
        data_buf[data_len++] = (timestamp >> 8) & 0xFF;
        data_buf[data_len++] = (timestamp >> 16) & 0xFF;
        data_buf[data_len++] = (timestamp >> 24) & 0xFF;
        
        data_buf[data_len++] = volume & 0xFF;
        data_buf[data_len++] = (volume >> 8) & 0xFF;
        data_buf[data_len++] = (volume >> 16) & 0xFF;
        data_buf[data_len++] = (volume >> 24) & 0xFF;
        
        data_buf[data_len++] = flow_rate & 0xFF;
        data_buf[data_len++] = (flow_rate >> 8) & 0xFF;
        data_buf[data_len++] = (flow_rate >> 16) & 0xFF;
        data_buf[data_len++] = (flow_rate >> 24) & 0xFF;
    }
    
    // Send via Bluetooth using NOTIFY type
    // Note: ble_cmd_send is defined in app_ble_protocol.c
    extern void ble_cmd_send(uint8_t type, uint8_t cmd, uint8_t len, uint8_t *data);
    ble_cmd_send(BLE_CMD_NOTIFY, BLE_CMD_FLOW_DATA_REPORT, data_len, data_buf);
}
#endif

//-----------------------------------------------------------------------------
void Sdp3x_Init(uint8_t i2cAddress)
{
    _i2cAddress = i2cAddress;
}
  
//-----------------------------------------------------------------------------
int Sdp3x_StartContinousMeasurement(Sdp3xTempComp  tempComp,
                                    Sdp3xAveraging averaging)
{
    int ret;
    Command command = COMMAND_UNDEFINED;

    // determine command code
    switch(tempComp) {
        case SDP3X_TEMPCOMP_MASS_FLOW:
        switch(averaging) {
            case SDP3X_AVERAGING_TILL_READ:
            command = COMMAND_START_MEASURMENT_MF_AVERAGE;
            break;
            case SDP3X_AVERAGING_NONE:
            command = COMMAND_START_MEASURMENT_MF_NONE;
            break;
        }
        break;
        case SDP3X_TEMPCOMP_DIFFERNTIAL_PRESSURE:
        switch(averaging) {
            case SDP3X_AVERAGING_TILL_READ:
            command = COMMAND_START_MEASURMENT_DP_AVERAGE;
            break;
            case SDP3X_AVERAGING_NONE:
            command = COMMAND_START_MEASURMENT_DP_NONE;
            break;
        }
        break;
    }

    if(COMMAND_UNDEFINED != command) {
        ret = ExecuteCommand(command);
        // wait 10 ms for startup
        msleep(10);
    } else {
        ret = BSP_INVALID_PARAMETER;
    }

    return ret;
}

//-----------------------------------------------------------------------------
int Sdp3x_StopContinousMeasurement(void)
{
    return ExecuteCommand(COMMAND_STOP_CONTINOUS_MEASUREMENT);
}

//-----------------------------------------------------------------------------
int Sdp3x_ReadMeasurementResults(float* diffPressure, float* temperature)
{
    int  ret;
    int16_t  diffPressureTicks;
    int16_t  temperatureTicks;
    uint16_t scaleFactorDiffPressure;

    ret = ReadMeasurementRawResults(&diffPressureTicks, &temperatureTicks,
                                        &scaleFactorDiffPressure);

    if (BSP_OK == ret) {
        *diffPressure = (float)diffPressureTicks / (float)scaleFactorDiffPressure;
        *temperature  = (float)temperatureTicks / scaleFactorTemperature;
    }

    return ret;
}

//-----------------------------------------------------------------------------
int Sdp3x_SoftReset(void)
{
    uint8_t reg_addr = 0x00;
    uint8_t write_data = 0x06;
    int ret = sdp3x_write_nbyte(0, reg_addr, &write_data, 1);
    // wait 20 ms
    msleep(20);
    
    return ret;
}

//-----------------------------------------------------------------------------
static int ReadMeasurementRawResults(int16_t*  diffPressureTicks,
                                        int16_t*  temperatureTicks,
                                        uint16_t* scaleFactor)
{
    int ret = BSP_ERROR;
    uint8_t read_data[10] = {0};
    ret = sdp3x_read_nbyte(0, 0x00, read_data, 9);
    if (ret != BSP_OK) {
        return ret;
    }
    for (int i = 0; i < 3; i++) {
        ret = CheckCrc(read_data[i*3], 2, read_data[(i*3) + 2]);
        if (ret != BSP_OK) {
            return ret;
        }
    }

    return ret;
}

//-----------------------------------------------------------------------------
int ExecuteCommand(Command cmd)
{
    uint8_t reg_addr = cmd >> 8;
    uint8_t write_data = cmd & 0xFF;
    int ret = sdp3x_write_nbyte(0, reg_addr, &write_data, 1);
    if (ret != BSP_OK) {
        return BSP_ERROR;
    }
    return BSP_OK;
}

//-----------------------------------------------------------------------------
static int CheckCrc(const uint8_t data[], uint8_t size, uint8_t checksum)
{
    uint8_t crc = 0xFF;

    // calculates 8-Bit checksum with given polynomial 0x31 (x^8 + x^5 + x^4 + 1)
    for(int i = 0; i < size; i++) {
        crc ^= (data[i]);
        for(uint8_t bit = 8; bit > 0; --bit) {
        if(crc & 0x80) crc = (crc << 1) ^ 0x31;
        else           crc = (crc << 1);
        }
    }

    // verify checksum
    return (crc == checksum) ? BSP_OK : BSP_ERROR;
}


/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int bsp_diff_pressure_init(void)
{
    return app_sdp3x_init(SDP3X_I2C_ADDR);

    return BSP_OK;
}

int bsp_diff_pressure_deinit(void)
{
    memset(s_temp, 0, sizeof(s_temp));
    memset(s_temp_is_ready, 0, sizeof(s_temp_is_ready));
    memset(s_dev_is_ready, 0, sizeof(s_dev_is_ready));
    memset(s_dev_abnormal_count, 0, sizeof(s_dev_abnormal_count));
    s_log_count = 0;
    return BSP_OK;
}

int bsp_read_diff_pressure_status(void *args)
{
//     air_pressue_status_t *out_status = (air_pressue_status_t *)args;
// #if SDP3X_LOG_DEBUG
//     uint32_t start_time = get_timestamp();
// #endif
//     if (s_dev_is_ready[0] == 0 || s_dev_is_ready[1] == 0) {
//         return BSP_NOT_READY;
//     }
    
//     memset(out_status, 0, sizeof(air_pressue_status_t) * AIR_PRESSURE_NUM);
//     /// 温度转换未完成，则进行单次转换
//     if (s_temp_is_ready[0] == 0 || s_temp_is_ready[1] == 0) {
//         if (temperature_single_conversion() != BSP_OK) {
//             return BSP_ERROR;
//         }
//         LOG_PBUS("start pressure conversion");
//         start_air_pressure_conversion(LIMS_PRESSURE_DATA); /// 重新开始压力转换
//         msleep(LIMS_PRESSURE_CONVERSION_TIME); /// 等待压力转换完成
//     }

//     /// 等待转换完成
//     if (wait_conversion_completed() != BSP_OK) {
//         start_air_pressure_conversion(LIMS_PRESSURE_DATA); /// 重新开始压力转换
//         return BSP_ERROR;
//     }

//     for (uint8_t i = 0; i < AIR_PRESSURE_NUM; i++) {
//         if (air_pressure_result_get(i, &out_status[i]) != BSP_OK) {
//             start_air_pressure_conversion(LIMS_PRESSURE_DATA); /// 重新开始压力转换
//             return BSP_ERROR;
//         }
//     }
//     start_air_pressure_conversion(LIMS_PRESSURE_DATA);
// #if SDP3X_LOG_DEBUG
//     BSP_LOG_PASS("4 time: %d, pressure: %f, %f, raw: %d, %d\r\n", 
//         get_timestamp_ms(start_time), out_status[0].pressure, 
//         out_status[1].pressure, out_status[0].raw_data, out_status[1].raw_data);
// #endif
//     s_log_count++;
    return BSP_OK;
}

int bsp_diff_pressure_is_ready(void *args)
{
    // uint8_t *pressure_is_ready_status = (uint8_t *)args;
    // for (uint8_t i = 0; i < AIR_PRESSURE_NUM; i++) {
    //     pressure_is_ready_status[i] = s_dev_is_ready[i];
    // }
    return BSP_OK;
}   
