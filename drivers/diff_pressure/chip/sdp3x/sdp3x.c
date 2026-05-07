/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "sdp3x.h" // IWYU pragma: keep
#include "bsp_common.h"
#include "bsp_config.h"
#include "bsp_internal.h" // IWYU pragma: keep
#include "bsp_i2c.h" // IWYU pragma: keep
#include "hal_air_pressure.h"
#include "hal_diff_pressure.h"
#if SDP3X_FLOW_PIPELINE_ENABLE
#include "flow_algo.h"
#endif
#include "sdp3x_sensirion_reference.h"
#include <stdint.h>
#include <string.h>


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
static uint8_t s_dev_is_ready[DIFF_PRESSURE_NUM] = {0};
static uint16_t s_dev_abnormal_count[DIFF_PRESSURE_NUM] = {0};
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

#define PRESSURE_BUFFER_SIZE           50
#define BASELINE_SAMPLES               10

static uint8_t _i2cAddress;

#if SDP3X_FLOW_PIPELINE_ENABLE
static FlowContext s_flow_ctx __attribute__((section(".volatile_ram")));
static FlowResult s_flow_results[5] __attribute__((section(".volatile_ram")));
#endif
static float s_pressure_buffer[PRESSURE_BUFFER_SIZE] __attribute__((section(".volatile_ram")));
static uint16_t s_pressure_buffer_index = 0;
static diff_pressure_flow_status_t s_latest_flow_status;
static float s_pressure_baseline = 0.0f;
static uint8_t s_baseline_calibrated = 0;
static uint8_t s_baseline_sample_count = 0;
static float s_baseline_sum = 0.0f;

static int sdp3x_start_continuous_selected(void);
static int sdp3x_bringup_soft_reset_and_start(void);
static void sdp3x_flow_pipeline_reset_state(void);

#if SDP3X_FLOW_PIPELINE_ENABLE
static void sdp3x_feed_flow_pipeline(float pressure_sample);
#endif


/*
* LOCAL FUNCTION IMPLEMENTATIONS (I2C primitives)
*****************************************************************************************
*/
static int sdp3x_write_nbyte(uint8_t index, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int ret = 0;

    if (index >= DIFF_PRESSURE_NUM) {
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
        .dev_address = _i2cAddress,
        .reg_addr = &reg_addr,
        .reg_addr_len = 1,
        .data = data,
        .len = len,
    };

    for (uint8_t i = 0; i < 3; i++) {
        ret = bsp_i2c_write_byte(DIFF_PRESSURE_I2C_ID, &i2c_data);
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
    
    if (index >= DIFF_PRESSURE_NUM) {
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
        .dev_address = _i2cAddress,
        .reg_addr = &reg_addr,
        .reg_addr_len = 0,
        .data = data,
        .len = len,
    };

    for (uint8_t i = 0; i < 3; i++) {
        ret = bsp_i2c_read_byte(DIFF_PRESSURE_I2C_ID, &i2c_data);
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
    if (index >= DIFF_PRESSURE_NUM) {
        LOGE("pressure sensor index is invalid: %d", index);
        return BSP_INVALID_PARAMETER;
    }

    i2c_master_data_t i2c_data = {
        .dev_address = _i2cAddress,
        .reg_addr = &reg_addr,
        .reg_addr_len = 0,
        .data = data,
        .len = len,
    };

    ret = bsp_i2c_read_byte(DIFF_PRESSURE_I2C_ID, &i2c_data);
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
/// Sensirion 示例：连续测量前先 SoftReset（main.c 外层循环）。
static int sdp3x_start_continuous_selected(void)
{
#if SDP3X_MEASUREMENT_USE_MASS_FLOW_REFERENCE
    return Sdp3x_StartContinousMeasurement(SDP3X_TEMPCOMP_MASS_FLOW, SDP3X_AVERAGING_TILL_READ);
#else
    return Sdp3x_StartContinousMeasurement(SDP3X_TEMPCOMP_DIFFERNTIAL_PRESSURE,
                                           SDP3X_AVERAGING_TILL_READ);
#endif
}

static void sdp3x_ref_delay_ms(uint32_t ms)
{
    msleep(ms);
}

static int sdp3x_ref_soft_reset(void)
{
    return (int)Sdp3x_SoftReset();
}

static int sdp3x_ref_start_continuous_measurement(void)
{
    return (int)sdp3x_start_continuous_selected();
}

static const sdp3x_sensirion_reference_ops_t s_sdp3x_sens_ref_ops = {
    .soft_reset = sdp3x_ref_soft_reset,
    .start_continuous_measurement = sdp3x_ref_start_continuous_measurement,
    .delay_ms = sdp3x_ref_delay_ms,
};

static int sdp3x_bringup_soft_reset_and_start(void)
{
    /* 与 BSP：成功为 0（BSP_OK）；失败为 ops 返回的 bsp_status_e 或 bringup 的 -1 */
    return sdp3x_sensirion_reference_bringup(&s_sdp3x_sens_ref_ops,
                                              SDP3X_SENSIRION_MS_AFTER_SOFT_RESET,
                                              SDP3X_SENSIRION_MS_AFTER_START_CONTINUOUS);
}

static void sdp3x_flow_pipeline_reset_state(void)
{
    s_pressure_buffer_index = 0;
    s_pressure_baseline = 0.0f;
    s_baseline_calibrated = 0;
    s_baseline_sample_count = 0;
    s_baseline_sum = 0.0f;
    memset(&s_latest_flow_status, 0, sizeof(s_latest_flow_status));
#if SDP3X_FLOW_PIPELINE_ENABLE
    flow_algo_init(&s_flow_ctx);
#endif
}

#if SDP3X_FLOW_PIPELINE_ENABLE
static void sdp3x_feed_flow_pipeline(float pressure_sample)
{
    if (!s_baseline_calibrated && s_baseline_sample_count < BASELINE_SAMPLES) {
        s_baseline_sum += pressure_sample;
        s_baseline_sample_count++;
        if (s_baseline_sample_count >= BASELINE_SAMPLES) {
            s_pressure_baseline = s_baseline_sum / (float)BASELINE_SAMPLES;
            s_baseline_calibrated = 1;
        }
    }

    float rel = pressure_sample;
    if (s_baseline_calibrated) {
        rel = pressure_sample - s_pressure_baseline;
    }

    int repeats = (int)SDP3X_FLOW_VIRTUAL_10MS_SAMPLES_PER_READ;
    if (repeats < 1) {
        repeats = 1;
    }
    for (int r = 0; r < repeats; r++) {
        if (s_pressure_buffer_index < PRESSURE_BUFFER_SIZE) {
            s_pressure_buffer[s_pressure_buffer_index++] = rel;
        }
    }

    if (s_pressure_buffer_index >= PRESSURE_BUFFER_SIZE) {
        int n = process_pressure_batch(&s_flow_ctx, s_pressure_buffer, PRESSURE_BUFFER_SIZE, s_flow_results,
                                       (int)(sizeof(s_flow_results) / sizeof(s_flow_results[0])));
        s_pressure_buffer_index = 0;
        if (n > 0) {
            FlowResult *last = &s_flow_results[n - 1];
            s_latest_flow_status.valid = 1;
            s_latest_flow_status.volume_ml = last->volume;
            s_latest_flow_status.flow_rate_ml_min = last->flow_rate;
            s_latest_flow_status.algo_time_s = last->timestamp;
        }
    }
}
#endif


//-----------------------------------------------------------------------------
int app_sdp3x_init(uint8_t i2cAddress)
{
    if (s_sdp3x_initialized) {
        LOG_PBUS("SDP3x already initialized\n");
        return BSP_OK;
    }

    for (uint8_t i = 0; i < DIFF_PRESSURE_NUM; i++) {
        s_dev_is_ready[i] = 1;
        s_dev_abnormal_count[i] = 0;
    }

    Sdp3x_Init(i2cAddress);
    s_sdp3x_i2c_address = i2cAddress;

    sdp3x_flow_pipeline_reset_state();

    int ret = sdp3x_bringup_soft_reset_and_start();
    if (ret != BSP_OK) {
        LOGE("SDP3x bring-up (SoftReset + start continuous) failed: %d", ret);
        for (uint8_t i = 0; i < DIFF_PRESSURE_NUM; i++) {
            s_dev_is_ready[i] = 0;
        }
        return BSP_ERROR;
    }

#if SDP3X_READ_PAUSE_MS_BEFORE_SAMPLE > 0
    sdp3x_sensirion_reference_delay_before_read(
        &s_sdp3x_sens_ref_ops, (uint32_t)SDP3X_READ_PAUSE_MS_BEFORE_SAMPLE);
#endif

    ret = Sdp3x_ReadMeasurementResults(&s_last_diff_pressure, &s_last_temperature);
    if (ret != BSP_OK) {
        LOG_PBUS("SDP3x first read failed, retry after bring-up");
        ret = sdp3x_bringup_soft_reset_and_start();
        if (ret == BSP_OK) {
#if SDP3X_READ_PAUSE_MS_BEFORE_SAMPLE > 0
            sdp3x_sensirion_reference_delay_before_read(
                &s_sdp3x_sens_ref_ops, (uint32_t)SDP3X_READ_PAUSE_MS_BEFORE_SAMPLE);
#endif
            ret = Sdp3x_ReadMeasurementResults(&s_last_diff_pressure, &s_last_temperature);
        }
    }

    if (ret != BSP_OK) {
        LOGE("SDP3x initialized failed (read)!!! ret=%d", ret);
        (void)Sdp3x_StopContinousMeasurement();
        for (uint8_t i = 0; i < DIFF_PRESSURE_NUM; i++) {
            s_dev_is_ready[i] = 0;
        }
        return BSP_ERROR;
    }

#if SDP3X_MEASUREMENT_USE_MASS_FLOW_REFERENCE
    LOG_PBUS("SDP3x mode: MASS_FLOW + AVERAGE_TILL_READ (Sensirion demo compatible)");
#else
    LOG_PBUS("SDP3x mode: DIFFERENTIAL_PRESSURE + AVERAGE_TILL_READ");
#endif
    LOG_PBUS("SDP3x initialized OK, signal: %f, temperature: %f",
             s_last_diff_pressure, s_last_temperature);

#if SDP3X_FLOW_PIPELINE_ENABLE
    sdp3x_feed_flow_pipeline(s_last_diff_pressure);
#endif

    s_sdp3x_measurement_started = 1;
    s_sdp3x_initialized = 1;

    return BSP_OK;
}

//-----------------------------------------------------------------------------
void app_sdp3x_deinit(void)
{
    if (!s_sdp3x_initialized) {
        return;
    }
    (void)Sdp3x_StopContinousMeasurement();
    s_sdp3x_measurement_started = 0;
    s_sdp3x_initialized = 0;
    for (uint8_t i = 0; i < DIFF_PRESSURE_NUM; i++) {
        s_dev_is_ready[i] = 0;
        s_dev_abnormal_count[i] = 0;
    }
    sdp3x_flow_pipeline_reset_state();
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
    int ret = sdp3x_start_continuous_selected();
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

        ret = sdp3x_start_continuous_selected();

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

        msleep(200);
        ret = sdp3x_start_continuous_selected();
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
        ret = CheckCrc(&read_data[i * 3], 2, read_data[(i * 3) + 2]);
        if (ret != BSP_OK) {
            return ret;
        }
    }

    *diffPressureTicks = (int16_t)(((uint16_t)read_data[0] << 8) | read_data[1]);
    *temperatureTicks = (int16_t)(((uint16_t)read_data[3] << 8) | read_data[4]);
    *scaleFactor = (uint16_t)(((uint16_t)read_data[6] << 8) | read_data[7]);
    if (*scaleFactor == 0) {
        return BSP_ERROR;
    }

    return BSP_OK;
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




int bsp_diff_pressure_init(void)
{
    return app_sdp3x_init(SDP3X_I2C_ADDR);
}

int bsp_diff_pressure_deinit(void)
{
    app_sdp3x_deinit();
    memset(s_dev_is_ready, 0, sizeof(s_dev_is_ready));
    memset(s_dev_abnormal_count, 0, sizeof(s_dev_abnormal_count));
    s_log_count = 0;
    return BSP_OK;
}

int bsp_read_diff_pressure_status(void *args)
{
    air_pressue_status_t *out_status = (air_pressue_status_t *)args;

    if (out_status == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    if (!s_sdp3x_initialized || !s_sdp3x_measurement_started) {
        return BSP_NOT_READY;
    }

#if SDP3X_LOG_DEBUG
    uint32_t start_time = get_timestamp();
#endif

    for (uint8_t i = 0; i < DIFF_PRESSURE_NUM; i++) {
        if (s_dev_is_ready[i] == 0) {
            return BSP_NOT_READY;
        }
    }

    memset(out_status, 0, sizeof(air_pressue_status_t) * DIFF_PRESSURE_NUM);

#if SDP3X_READ_PAUSE_MS_BEFORE_SAMPLE > 0
    sdp3x_sensirion_reference_delay_before_read(
        &s_sdp3x_sens_ref_ops, (uint32_t)SDP3X_READ_PAUSE_MS_BEFORE_SAMPLE);
#endif

    int ret = Sdp3x_ReadMeasurementResults(&s_last_diff_pressure, &s_last_temperature);
    if (ret != BSP_OK) {
        LOG_PBUS("SDP3x read failed: %d, recover (SoftReset+start)", ret);
        int br = sdp3x_bringup_soft_reset_and_start();
        if (br == BSP_OK) {
#if SDP3X_READ_PAUSE_MS_BEFORE_SAMPLE > 0
            sdp3x_sensirion_reference_delay_before_read(
                &s_sdp3x_sens_ref_ops, (uint32_t)SDP3X_READ_PAUSE_MS_BEFORE_SAMPLE);
#endif
            ret = Sdp3x_ReadMeasurementResults(&s_last_diff_pressure, &s_last_temperature);
        }
        if (ret != BSP_OK) {
            s_sdp3x_measurement_started = 0;
            return BSP_ERROR;
        }
        s_sdp3x_measurement_started = 1;
    }

    for (uint8_t i = 0; i < DIFF_PRESSURE_NUM; i++) {
        out_status[i].pressure = s_last_diff_pressure;
        out_status[i].temp = s_last_temperature;
        float rd = s_last_diff_pressure * 1000.0f;
        if (rd > 2147483647.0f) {
            rd = 2147483647.0f;
        } else if (rd < -2147483648.0f) {
            rd = -2147483648.0f;
        }
        out_status[i].raw_data = (uint32_t)(int32_t)rd;
    }

#if SDP3X_FLOW_PIPELINE_ENABLE
    sdp3x_feed_flow_pipeline(s_last_diff_pressure);
#endif

#if SDP3X_LOG_DEBUG
    BSP_LOG_PASS("sdp3x rd time: %d, Pa: %f, T: %f\r\n",
        get_timestamp_ms(start_time),
        out_status[0].pressure,
        out_status[0].temp);
#endif
    s_log_count++;
    return BSP_OK;
}

int bsp_diff_pressure_get_flow_status(void *args)
{
#if !SDP3X_FLOW_PIPELINE_ENABLE
    (void)args;
    return BSP_ERROR;
#else
    diff_pressure_flow_status_t *out = (diff_pressure_flow_status_t *)args;

    if (out == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    *out = s_latest_flow_status;
    return BSP_OK;
#endif
}

int bsp_diff_pressure_is_ready(void *args)
{
    uint8_t *pressure_is_ready_status = (uint8_t *)args;

    if (pressure_is_ready_status == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    for (uint8_t i = 0; i < DIFF_PRESSURE_NUM; i++) {
        pressure_is_ready_status[i] = (uint8_t)(s_dev_is_ready[i] & 1U);
    }
    return BSP_OK;
}
