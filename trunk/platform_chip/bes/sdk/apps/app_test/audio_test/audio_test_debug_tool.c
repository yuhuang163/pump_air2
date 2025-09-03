 /***************************************************************************
 *
 * Copyright 2015-2025 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include <string.h>
#include "hal_trace.h"
#include "cmsis_os.h"
#include "audio_test_tool.h"

static bool g_cmd_send_rsp = false;

extern void app_debug_tool_printf(const char *fmt, ...);

bool audio_test_tool_check_rsp_flag(void)
{
    return g_cmd_send_rsp;
}

void audio_test_tool_set_rsp_flag(bool val)
{
    g_cmd_send_rsp = val;
}

static void audio_test_get_frame_size(const char *cmd)
{
    APP_TEST_TRACE(0, "[%s] cmd len: %d", __func__, strlen(cmd));

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] frame_size: 256");
}

#ifdef PSAP_APP
#include "psap_process.h"
static struct_psap_cfg hw_psap_cfg;
#endif
static void audio_test_hw_psap_cfg(const char *cmd)
{
#ifdef PSAP_APP
    uint32_t len = *((uint32_t *)cmd);

    if (len != sizeof(aud_item_psap)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(aud_item_psap));
        return;
    }

    memcpy(&hw_psap_cfg.psap_cfg_l, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] hw_psap_cfg: %d, %d, %d", __func__,
        hw_psap_cfg.psap_cfg_l.psap_total_gain,
        hw_psap_cfg.psap_cfg_l.psap_band_num,
        hw_psap_cfg.psap_cfg_l.psap_type);

    psap_set_cfg_coef(&hw_psap_cfg);
#else
    APP_TEST_TRACE(0, "[%s] Need to enable PSAP_APP", __func__);
#endif
}

#include "iir_process.h"
#include "audio_process.h"
static void audio_test_get_audio_eq_bands(const char *cmd)
{
    APP_TEST_TRACE(0, "[%s] cmd len: %d", __func__, strlen(cmd));

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] audio_eq_bands: %d", IIR_PARAM_NUM);
}

static void audio_test_get_psap_band_gain_Q(const char *cmd)
{
    APP_TEST_TRACE(0, "[%s] cmd len: %d", __func__, strlen(cmd));

    g_cmd_send_rsp = true;
#if defined(CHIP_BEST1501) || defined(CHIP_BEST1501P) || defined(CHIP_BEST1502)
    app_debug_tool_printf("[CMD] psap_band_gain_Q: %d", false);
#else
    app_debug_tool_printf("[CMD] psap_band_gain_Q: %d", true);
#endif
}

static void audio_test_get_aud_item_ver(const char *cmd)
{
    APP_TEST_TRACE(0, "[%s] cmd len: %d", __func__, strlen(cmd));

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] aud_item_ver: %d", AUD_SECTION_STRUCT_VERSION);
}

static void audio_test_audio_eq_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static IIR_CFG_T audio_eq_cfg;

    if (len != sizeof(IIR_CFG_T)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(IIR_CFG_T));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: EQ len is invalid: %d, %d;", len, sizeof(IIR_CFG_T));
        return;
    }

    memcpy(&audio_eq_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] audio_eq_cfg: %d, %d, %d", __func__,
        (int32_t)(audio_eq_cfg.gain0 * 100),
        (int32_t)(audio_eq_cfg.gain1 * 100),
        audio_eq_cfg.num);

#if defined(__HW_DAC_IIR_EQ_PROCESS__)
    audio_eq_set_cfg(NULL, &audio_eq_cfg, AUDIO_EQ_TYPE_HW_DAC_IIR);
#elif defined(__SW_IIR_EQ_PROCESS__)
    audio_eq_set_cfg(NULL, &audio_eq_cfg, AUDIO_EQ_TYPE_SW_IIR);
#else
    APP_TEST_TRACE(0, "[%s] Need to enable EQ MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable EQ MACRO;");
#endif
}

static void audio_test_dac_iir_eq(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    IIR_CFG_T audio_eq_cfg;

    if (len != sizeof(IIR_CFG_T)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(IIR_CFG_T));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: DAC IIR len is invalid: %d, %d;", len, sizeof(IIR_CFG_T));
        return;
    }

    memcpy(&audio_eq_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] audio_eq_cfg: %d, %d, %d", __func__,
        (int32_t)(audio_eq_cfg.gain0 * 100),
        (int32_t)(audio_eq_cfg.gain1 * 100),
        audio_eq_cfg.num);

    if (audio_process_is_opened() == false) {
        APP_TEST_TRACE(0, "[%s] Failed: audio_process is closed", __func__);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Music is stopped;");
        return;
    }

#if defined(__HW_DAC_IIR_EQ_PROCESS__)
    audio_eq_set_cfg(NULL, &audio_eq_cfg, AUDIO_EQ_TYPE_HW_DAC_IIR);
#else
    APP_TEST_TRACE(0, "[%s] Need to enable HW_DAC_IIR_EQ_PROCESS", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable HW_DAC_IIR_EQ_PROCESS;");
#endif
}

#include "audio_cfg.h"
static void audio_test_burn_audio_eq_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static IIR_CFG_T audio_eq_cfg;

    if (len != sizeof(IIR_CFG_T)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(IIR_CFG_T));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: EQ len is invalid: %d, %d;", len, sizeof(IIR_CFG_T));
        return;
    }

    memcpy(&audio_eq_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] audio_eq_cfg: %d, %d, %d", __func__,
        (int32_t)(audio_eq_cfg.gain0 * 100),
        (int32_t)(audio_eq_cfg.gain1 * 100),
        audio_eq_cfg.num);

#if defined(__HW_DAC_IIR_EQ_PROCESS__) || defined(__SW_IIR_EQ_PROCESS__)
    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_IIR_EQ, (uint8_t *)&audio_eq_cfg);
#else
    APP_TEST_TRACE(0, "[%s] Need to enable EQ MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable EQ MACRO;");
#endif
}

#include "drc.h"
static void audio_test_burn_drc_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static DrcConfig drc_cfg;

    if (len != sizeof(DrcConfig)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(DrcConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: DRC len is invalid: %d, %d;", len, sizeof(DrcConfig));
        return;
    }

    memcpy(&drc_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] knee = %d, band_num = %d, look_ahead_time = %d", __func__,
        drc_cfg.knee, \
        drc_cfg.band_num, \
        drc_cfg.look_ahead_time);

#if defined(__AUDIO_DRC__)
    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DRC, (uint8_t *)&drc_cfg);
#else
    APP_TEST_TRACE(0, "[%s] Need to enable DRC MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable DRC MACRO;");
#endif
}

#include "limiter.h"
static void audio_test_burn_limiter_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static LimiterConfig limiter_cfg;

    if (len != sizeof(LimiterConfig)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(LimiterConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Limiter len is invalid: %d, %d;", len, sizeof(LimiterConfig));
        return;
    }

    memcpy(&limiter_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] limiter_cfg: keen=%d, look_ahead_time=%d, release_time=%d", __func__,
        limiter_cfg.knee,
        limiter_cfg.look_ahead_time,
        limiter_cfg.release_time);

#if defined(__AUDIO_LIMITER__)
    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_LIMITER, (uint8_t *)&limiter_cfg);
#else
    APP_TEST_TRACE(0, "[%s] Need to enable Limiter MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable Limiter MACRO;");
#endif
}

#include "dynamic_eq.h"
static void audio_test_burn_dynamic_eq_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static DynamicEqConfig dynamic_eq_cfg;

    if (len != sizeof(DynamicEqConfig)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(DynamicEqConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Dynamic eq len is invalid: %d, %d;", len, sizeof(DynamicEqConfig));
        return;
    }

    memcpy(&dynamic_eq_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] switch_on = %d, debug = %d, offset = %d, gain = %d, eq_num = %d", __func__, dynamic_eq_cfg.switch_on, \
                                                                                dynamic_eq_cfg.debug, \
                                                                                (uint32_t)(10*dynamic_eq_cfg.offset), \
                                                                                (uint16_t)(10*dynamic_eq_cfg.gain), \
                                                                                dynamic_eq_cfg.eq_num);

#if defined(__AUDIO_DYNAMIC_EQ__)
    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DYNAMIC_EQ, (uint8_t *)&dynamic_eq_cfg);
#else
    APP_TEST_TRACE(0, "[%s] Need to enable Dynamic eq MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable Dynamic eq MACRO;");
#endif
}

#if 1
#include "audio_cfg.h"
#include "drc.h"
#include "limiter.h"
#include "dynamic_eq.h"
#include "iir_process.h"
extern const IIR_CFG_T audio_eq_anc_hw_dac_iir_cfg;
extern const DrcConfig audio_drc_cfg;
extern const LimiterConfig audio_limiter_cfg;
extern const DynamicEqConfig audio_dynamic_eq_cfg;

static void audio_test_aud_burn_test_store(const char *cmd)
{
    APP_TEST_TRACE(0, "------store eq cfg----------, num = %d", audio_eq_anc_hw_dac_iir_cfg.num);
    for (int i = 0; i < audio_eq_anc_hw_dac_iir_cfg.num; i++) {
        APP_TEST_TRACE(0, "type = %d, gain = %d, freq = %d, Q = %d, ", (uint32_t)(audio_eq_anc_hw_dac_iir_cfg.param[0].type*10), \
                                                                                (uint32_t)(audio_eq_anc_hw_dac_iir_cfg.param[0].gain*10), \
                                                                                (uint32_t)(audio_eq_anc_hw_dac_iir_cfg.param[0].fc*10), \
                                                                                (uint32_t)(audio_eq_anc_hw_dac_iir_cfg.param[0].Q*10));
    }
    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_IIR_EQ, (uint8_t *)&audio_eq_anc_hw_dac_iir_cfg);
    osDelay(1000);

    APP_TEST_TRACE(0, "------store drc cfg----------");
    APP_TEST_TRACE(0, "knee = %d, band_num = %d, look_ahead_time = %d, ", audio_drc_cfg.knee, \
                                                                                audio_drc_cfg.band_num, \
                                                                                audio_drc_cfg.look_ahead_time);
    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DRC, (uint8_t *)&audio_drc_cfg);
    osDelay(1000);

    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_LIMITER, (uint8_t *)&audio_limiter_cfg);
    osDelay(1000);

    APP_TEST_TRACE(0, "------store dynamic eq cfg----------");
    APP_TEST_TRACE(0, "switch_on = %d, debug = %d, offset = %d, gain = %d, eq_num = %d", audio_dynamic_eq_cfg.switch_on, \
                                                                                audio_dynamic_eq_cfg.debug, \
                                                                                (uint32_t)(10*audio_dynamic_eq_cfg.offset), \
                                                                                (uint16_t)(10*audio_dynamic_eq_cfg.gain), \
                                                                                audio_dynamic_eq_cfg.eq_num);
    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DYNAMIC_EQ, (uint8_t *)&audio_dynamic_eq_cfg);
    osDelay(1000);
}

static void audio_test_aud_burn_test_lord(const char *cmd)
{
    IIR_CFG_T *eq_cfg = (IIR_CFG_T *)load_audio_cfg_from_audio_section(AUDIO_PROCESS_TYPE_IIR_EQ, 0);
    APP_TEST_TRACE(0, "------lord eq cfg----------, num = %d", eq_cfg->num);
    for (int i = 0; i < eq_cfg->num; i++) {
        APP_TEST_TRACE(0, "type = %d, gain = %d, freq = %d, Q = %d, ", (uint32_t)(eq_cfg->param[0].type*10), \
                                                                                (uint32_t)(eq_cfg->param[0].gain*10), \
                                                                                (uint32_t)(eq_cfg->param[0].fc*10), \
                                                                                (uint32_t)(eq_cfg->param[0].Q*10));
    }

    osDelay(1000);
    DrcConfig *drc_cfg = load_audio_cfg_from_audio_section(AUDIO_PROCESS_TYPE_DRC, 0);
    if (drc_cfg) {
        APP_TEST_TRACE(0, "------lord drc cfg----------");
        APP_TEST_TRACE(0, "knee = %d, band_num = %d, look_ahead_time = %d, ", drc_cfg->knee, \
                                                                                    drc_cfg->band_num, \
                                                                                    drc_cfg->look_ahead_time);
    }

    osDelay(1000);
    load_audio_cfg_from_audio_section(AUDIO_PROCESS_TYPE_LIMITER, 0);

    osDelay(1000);
    DynamicEqConfig *dy_eq = load_audio_cfg_from_audio_section(AUDIO_PROCESS_TYPE_DYNAMIC_EQ, 0);
    APP_TEST_TRACE(0, "------lord dynamic eq cfg----------");
    APP_TEST_TRACE(0, "switch_on = %d, debug = %d, offset = %d, gain = %d, eq_num = %d", dy_eq->switch_on, \
                                                                                dy_eq->debug, \
                                                                                (uint32_t)(10*dy_eq->offset), \
                                                                                (uint16_t)(10*dy_eq->gain), \
                                                                                dy_eq->eq_num);
}
#endif

const audio_test_func_t audio_test_tool_func[]= {
    {"hw_psap_cfg",         audio_test_hw_psap_cfg},
    {"audio_eq_cfg",        audio_test_audio_eq_cfg},
    {"dac_iir_eq",          audio_test_dac_iir_eq},
    {"get_frame_size",      audio_test_get_frame_size},
    {"get_audio_eq_bands",  audio_test_get_audio_eq_bands},
    {"get_psap_band_gain_Q",audio_test_get_psap_band_gain_Q},
    {"get_aud_item_ver",    audio_test_get_aud_item_ver},
    {"aud_burn_eq",         audio_test_burn_audio_eq_cfg},
    {"aud_burn_drc",        audio_test_burn_drc_cfg},
    {"aud_burn_limiter",    audio_test_burn_limiter_cfg},
    {"aud_burn_dy_eq",      audio_test_burn_dynamic_eq_cfg},
#if 1
    {"aud_burn_store",       audio_test_aud_burn_test_store},
    {"aud_burn_lord",       audio_test_aud_burn_test_lord},
#endif
};

void audio_test_tool_get_func(const audio_test_func_t **func, uint32_t *size)
{
    *func = audio_test_tool_func;
    *size = ARRAY_SIZE(audio_test_tool_func);
}
