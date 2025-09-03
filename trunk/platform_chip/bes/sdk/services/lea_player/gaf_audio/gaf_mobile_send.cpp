/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifdef USB_BLE_AUDIO_HW_TIMER_TRIGGER
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "plat_types.h"
#include "app_audio.h"
#include "gaf_dbg.h"
#include "bt_drv_interface.h"
#include "gaf_media_common.h"
#include "app_usb_hw_timer.h"
#include "hwtimer_list.h"
#include "bt_service_thread.h"

extern void bt_svc_lea_uc_cli_stream_data_tx_read(uint8_t* data, uint32_t len);
extern void bt_svc_lea_uc_cli_stream_data_rx_write(uint8_t* data, uint32_t len);
extern void gaf_mobile_usb_feed_data(uint8_t* ptrBuf, uint32_t length);
extern void gaf_mobile_usb_processing_received_data(uint8_t* ptrBuf, uint32_t length);

/************************private macro defination***************************/
#define HW_TIMER_PORCESS_PLAYBACK               (1 << 0)
#define HW_TIMER_PORCESS_CAPTURE                (1 << 1)

/************************private strcuture defination****************************/
typedef struct
{
    osThreadId thread_id;

    uint8_t stream_flg;
    uint8_t playback_count;
    uint8_t capture_count;

    HWTIMER_ID playback_timer_id;
    HWTIMER_ID capture_timer_id;

    GAF_AUDIO_STREAM_ENV_T* playbackStreamEnv ;
    GAF_AUDIO_STREAM_ENV_T* captureStreamEnv;
} gaf_mobile_send_env_t;

/************************private variable defination************************/
static gaf_mobile_send_env_t gaf_mobile_send_env = {0};

/**********************private function declaration*************************/
static void gaf_mobile_send_start(void);
static void gaf_mobile_send_stop(void);

/****************************function defination****************************/

static uint32_t gaf_usb_capture_get_trigger_offset(void *_pStreamEnv)
{
    GAF_AUDIO_STREAM_ENV_T *pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)_pStreamEnv;

    uint32_t current_bt_time = bt_syn_ble_bt_time_to_bts(btdrv_syn_get_curr_ticks(), 0);
    uint32_t latest_iso_bt_time = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);

    int32_t gapUs = (int32_t)(latest_iso_bt_time - current_bt_time);

    while (gapUs < 1000)
    {
        latest_iso_bt_time += pStreamEnv->stream_info.captureInfo.isoIntervalUs;
        gapUs = (int32_t)(latest_iso_bt_time - current_bt_time);
    }

    if (gapUs <= (int32_t)(pStreamEnv->stream_info.captureInfo.isoIntervalUs / 2) ||
        gapUs >= (int32_t)(pStreamEnv->stream_info.captureInfo.isoIntervalUs * 1.5))
    {
        return pStreamEnv->stream_info.captureInfo.isoIntervalUs;
    }
    return gapUs;
}

static uint32_t gaf_usb_playback_get_trigger_offset(void *_pStreamEnv)
{
    GAF_AUDIO_STREAM_ENV_T *pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)_pStreamEnv;

    uint32_t current_bt_time = bt_syn_ble_bt_time_to_bts(btdrv_syn_get_curr_ticks(), 0);
    uint32_t latest_iso_bt_time = gaf_media_common_get_latest_rx_iso_evt_timestamp(pStreamEnv);
    int32_t gapUs = (int32_t)(latest_iso_bt_time - current_bt_time);

    while (gapUs < 1000)
    {
        latest_iso_bt_time += pStreamEnv->stream_info.playbackInfo.isoIntervalUs;
        gapUs = (int32_t)(latest_iso_bt_time - current_bt_time);
    }

    if (gapUs <= (int32_t)(pStreamEnv->stream_info.playbackInfo.isoIntervalUs / 2) ||
        gapUs >= (int32_t)(pStreamEnv->stream_info.playbackInfo.isoIntervalUs * 1.5))
    {
        return pStreamEnv->stream_info.playbackInfo.isoIntervalUs;
    }
    return gapUs;
}

static void gaf_usb_playback_hw_timer_handler(void *_pStreamEnv)
{
    //osSignalSet(hw_timer_stream_processor_thread_id, HW_TIMER_PORCESS_PLAYBACK);
    gaf_mobile_send_env.playback_count++;
    bt_svc_base_thread_send_signal();
}

static void gaf_usb_capture_hw_timer_handler(void *_pStreamEnv)
{
    //osSignalSet(hw_timer_stream_processor_thread_id, HW_TIMER_PORCESS_CAPTURE);

    gaf_mobile_send_env.capture_count++;
    bt_svc_base_thread_send_signal();
}

void gaf_usb_playback_hw_timer_start(void *_pStreamEnv, uint32_t trig_tick)
{
    if (NULL == _pStreamEnv)
    {
         LOG_E("%s, pStreamEnv is NULL", __func__);
         return;
    }

    if ((!gaf_mobile_send_env.capture_timer_id) &&
        (!gaf_mobile_send_env.playback_timer_id))
    {
        gaf_mobile_send_start();
    }

    if (NULL == gaf_mobile_send_env.playback_timer_id)
    {
        gaf_mobile_send_env.playback_timer_id =
            hwtimer_alloc(gaf_usb_playback_hw_timer_handler, _pStreamEnv);
    }
    gaf_mobile_send_env.playbackStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)_pStreamEnv;

    app_audio_mempool_get_buff(&gaf_mobile_send_env.playbackStreamEnv->stream_info.playbackInfo.dmaBufPtr,
        gaf_mobile_send_env.playbackStreamEnv->stream_info.playbackInfo.dmaChunkSize);

    uint32_t current_bt_time = bt_syn_ble_bt_time_to_bts(btdrv_syn_get_curr_ticks(), 0);
    LOG_I("hwtimer playback trigger_tick:%u curr_time:%u", trig_tick, current_bt_time);
    hwtimer_update_then_start(gaf_mobile_send_env.playback_timer_id,
        gaf_usb_playback_hw_timer_handler, _pStreamEnv, US_TO_TICKS(trig_tick - current_bt_time));
}

void gaf_usb_capture_hw_timer_start(void *_pStreamEnv, uint32_t trig_tick)
{
    GAF_AUDIO_STREAM_COMMON_INFO_T*  captureInfo = NULL;

    if (NULL == _pStreamEnv)
    {
         LOG_E("%s please check param", __func__);
         return;
    }

    if ((!gaf_mobile_send_env.capture_timer_id) &&
        (!gaf_mobile_send_env.playback_timer_id))
    {
        gaf_mobile_send_start();
    }

    if (NULL == gaf_mobile_send_env.capture_timer_id)
    {
        gaf_mobile_send_env.capture_timer_id =
            hwtimer_alloc(gaf_usb_capture_hw_timer_handler, _pStreamEnv);
    }
    gaf_mobile_send_env.captureStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)_pStreamEnv;

    captureInfo = &gaf_mobile_send_env.captureStreamEnv->stream_info.captureInfo;
    app_audio_mempool_get_buff(&captureInfo->dmaBufPtr, captureInfo->dmaChunkSize);

    uint32_t current_bt_time = bt_syn_ble_bt_time_to_bts(btdrv_syn_get_curr_ticks(), 0);
    LOG_I("hwtimer capture trigger_tick:%u curr_time:%u", trig_tick, current_bt_time);
    hwtimer_update_then_start(gaf_mobile_send_env.capture_timer_id,
        gaf_usb_capture_hw_timer_handler, _pStreamEnv, US_TO_TICKS(trig_tick - current_bt_time));
}

void gaf_usb_hw_timer_stop(uint32_t stream)
{
    if (AUD_STREAM_PLAYBACK == stream)
    {
        if (gaf_mobile_send_env.capture_timer_id)
        {
            hwtimer_stop(gaf_mobile_send_env.capture_timer_id);
            hwtimer_free(gaf_mobile_send_env.capture_timer_id);
            gaf_mobile_send_env.capture_timer_id = NULL;
        }
    }
    else if (AUD_STREAM_CAPTURE == stream)
    {
        if (gaf_mobile_send_env.playback_timer_id)
        {
            hwtimer_stop(gaf_mobile_send_env.playback_timer_id);
            hwtimer_free(gaf_mobile_send_env.playback_timer_id);
            gaf_mobile_send_env.playback_timer_id = NULL;
        }
    }

    if ((!gaf_mobile_send_env.capture_timer_id) &&
        (!gaf_mobile_send_env.playback_timer_id))
    {
        gaf_mobile_send_stop();
    }
}

static void hw_timer_stream_processor_thread(const void *arg)
{
    while(1)
    {
        osEvent evt;
        evt = osSignalWait(0x0, osWaitForever);

        if (osEventSignal == evt.status)
        {
            if (evt.value.signals & HW_TIMER_PORCESS_PLAYBACK)
            {
                if (gaf_stream_is_playback_stream_iso_created(gaf_mobile_send_env.playbackStreamEnv)) {
                    uint32_t trigger_offset = gaf_usb_playback_get_trigger_offset(gaf_mobile_send_env.playbackStreamEnv);
                    hwtimer_start(gaf_mobile_send_env.playback_timer_id, US_TO_TICKS(trigger_offset));
                    //send_usb_audio_buffer(AUD_STREAM_CAPTURE);
                }
                else {
                    LOG_I("playback iso stream stopped");
                }
            }
            if (evt.value.signals & HW_TIMER_PORCESS_CAPTURE)
            {
                if (gaf_stream_is_capture_stream_iso_created(gaf_mobile_send_env.captureStreamEnv)) {
                    uint32_t trigger_offset = gaf_usb_capture_get_trigger_offset(gaf_mobile_send_env.captureStreamEnv);
                    hwtimer_start(gaf_mobile_send_env.capture_timer_id, US_TO_TICKS(trigger_offset));
                    //send_usb_audio_buffer(AUD_STREAM_PLAYBACK);
                }
                else {
                    LOG_I("capture iso stream stopped");
                }
            }
        }
    }
}

void gaf_usb_hw_timer_init(void)
{
/*
    if (NULL == gaf_mobile_send_env.thread_id)
    {
        gaf_mobile_send_env.thread_id =
            osThreadCreate(osThread(hw_timer_stream_processor_thread), NULL);
    }
*/
}


static void gaf_mobile_send_thread(void)
{
    GAF_AUDIO_STREAM_COMMON_INFO_T* stream=NULL;

    if (gaf_mobile_send_env.playback_count)
    {
        stream = &gaf_mobile_send_env.playbackStreamEnv->stream_info.playbackInfo;
        if (gaf_mobile_send_env.playback_count > 1)
        {
            LOG_W("send data miss: %d", gaf_mobile_send_env.playback_count);
        }

        if (gaf_stream_is_playback_stream_iso_created(gaf_mobile_send_env.playbackStreamEnv)) {
            uint32_t trigger_offset = gaf_usb_playback_get_trigger_offset(gaf_mobile_send_env.playbackStreamEnv);
            hwtimer_start(gaf_mobile_send_env.playback_timer_id, US_TO_TICKS(trigger_offset));
            gaf_mobile_usb_feed_data(stream->dmaBufPtr, stream->dmaChunkSize);
            bt_svc_lea_uc_cli_stream_data_rx_write(stream->dmaBufPtr, stream->dmaChunkSize);
        }
        else
        {
            LOG_I("playback iso stream stopped");
        }

        gaf_mobile_send_env.playback_count = 0;
    }

    if (gaf_mobile_send_env.capture_count)
    {
        stream = &gaf_mobile_send_env.captureStreamEnv->stream_info.captureInfo;

        if (gaf_mobile_send_env.capture_count > 1)
        {
            LOG_W("send data miss: %d", gaf_mobile_send_env.playback_count);
        }

        if (gaf_stream_is_capture_stream_iso_created(gaf_mobile_send_env.captureStreamEnv)) {
            uint32_t trigger_offset = gaf_usb_capture_get_trigger_offset(gaf_mobile_send_env.captureStreamEnv);
            hwtimer_start(gaf_mobile_send_env.capture_timer_id, US_TO_TICKS(trigger_offset));
            bt_svc_lea_uc_cli_stream_data_tx_read(stream->dmaBufPtr, stream->dmaChunkSize);
            gaf_mobile_usb_processing_received_data(stream->dmaBufPtr, stream->dmaChunkSize);
        }
        else {
            LOG_I("capture iso stream stopped");
        }

        gaf_mobile_send_env.capture_count = 0;
    }
}

void gaf_mobile_send_start(void)
{
    bt_svc_base_thread_add_task(gaf_mobile_send_thread);
    bt_svc_base_thread_set_priority(osPriorityHigh);
}

void gaf_mobile_send_stop(void)
{
    bt_svc_base_thread_delete_task(gaf_mobile_send_thread);
    bt_svc_base_thread_set_priority(osPriorityNormal);
}

#endif
