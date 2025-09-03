/**
 * @brief Bluetooth Service Low Energy Audio Unicast Client
 *
 * @copyright Copyright (c) 2015-20223 BES Technic.
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
 */
#ifndef __BT_SVC_LEA_BC_SOURCE_H__
#define __BT_SVC_LEA_BC_SOURCE_H__

/*****************************header include********************************/

/******************************macro defination*****************************/

/******************************type defination******************************/
enum VOLUME_SOURCE
{
    APP_BIS_LOCAL_SOURCE,
    APP_BIS_REMOTE_SOURCE,
};

typedef enum
{
    BT_SVC_LEA_BC_SRC_CODEC_OK,
    BT_SVC_LEA_BC_SRC_CODEC_ERROR,
} BT_SVC_LEA_BC_SRC_CODEC_RETURN_E;

typedef enum
{
    BT_SVC_LEA_BC_SRC_ENCODER,
    BT_SVC_LEA_BC_SRC_DECODER,
} BT_SVC_LEA_BC_SRC_CODEC_TYPE_E;

typedef enum
{
    BT_SVC_LEA_BC_SRC_STREAM_0 = 0,
    BT_SVC_LEA_BC_SRC_STREAM_1,
    BT_SVC_LEA_BC_SRC_STREAM_2,
    BT_SVC_LEA_BC_SRC_STREAM_3,
    BT_SVC_LEA_BC_SRC_STREAM_4,
    BT_SVC_LEA_BC_SRC_STREAM_MAX,
} BT_SVC_LEA_BC_SRC_AUDIO_STREAM_IDX_E;

typedef struct
{
    uint8_t  codec_type:1;
    uint8_t  pcm_interlaced:1;
    uint8_t  bits_depth;
    uint8_t  channel;
    uint32_t sample_rate;
    uint32_t frame_duration_ms;
    uint32_t frame_size;
} bt_svc_lea_bc_src_codec_param_t;

typedef struct
{
    uint8_t   pack_num;
    uint16_t  pack_size;
    void      *list_info;
} bt_svc_lea_bc_src_buff_list_t;

typedef struct
{
    int (*open)(uint8_t input_type);
    int (*close)(void);
    int (*stream_set)(bool operate);
} bt_svc_lea_bc_src_input_func_t;

typedef struct
{
    // pcm sampling param
    bool     external_trig;
    uint8_t  bits_depth;
    uint8_t  channel_num;
    uint16_t volume_level;
    uint32_t sample_rate;
    uint32_t duration_ms;
    void     (*triggered_cb)(void);
    uint32_t (*play_read_data_cb)(uint8_t *data, uint32_t data_len);
    uint32_t (*play_prompt_data_cb)(uint8_t *data, uint32_t data_len);
} bt_svc_lea_bc_src_media_param_t;

typedef struct
{
    // Stream idx see@BT_SVC_LEA_BC_SRC_AUDIO_STREAM_IDX_E
    uint8_t stream_idx;
    // Select playback device, see@BT_SVC_LEA_BC_SRC_AUDIO_PLAYBACK_DEVICE_E
    uint8_t playback_dev;
    // Pcm sampling param
    uint8_t channel_num;
    // Channel selection bit
    uint32_t channel_bf;
    // Audio sampling bit depth
    uint8_t  bits_depth;
    // Audio sampling rate
    uint32_t sample_rate;
    // The size of one frame of audio data
    uint32_t frame_samples;
} bt_svc_lea_bc_src_stream_param_t;

typedef struct
{
    // Data transmission mode, see@APP_BIS_TRAN_DATA_TANS_MODE_E
    uint8_t data_tran_mode;
    // Stream number
    uint8_t stream_num;
    // Start playing volume
    uint8_t volume;
    // Stream parameter
    bt_svc_lea_bc_src_stream_param_t *stream_param;
    /// Read data callback, data_tran_mode=1
    uint32_t (*read_data_cb)(uint8_t stream_idx, uint8_t *data, uint16_t data_len);
    /// Need to input source cache data
    void (*start_cache_cb)(uint8_t cache_num);
} bt_svc_lea_bc_src_start_param_t;

/****************************function declearation**************************/

//***************************
// Stream function
//***************************
/**
 ****************************************************************************************
 * @brief Bis source transport stream start
 * @param[in]  : param, bis transport stream start param
 * @param[out] : none
 * @return: error code, see@BT_SVC_ERROR_CODE_E
 ****************************************************************************************
 */
int bt_svc_lea_bc_src_stream_start(bt_svc_lea_bc_src_start_param_t *param);

/**
 ****************************************************************************************
 * @brief Bis source transport stream stop
 * @param[in]  : none
 * @param[out] : none
 * @return: error code, see@BT_SVC_ERROR_CODE_E
 ****************************************************************************************
 */
int bt_svc_lea_bc_src_stream_stop(void);

/**
 ****************************************************************************************
 * @brief Bis source transport write pcm data, only bt_svc_lea_bc_src_stream_start
          function param.data_tran_mode = BT_SVC_LEA_BC_SRC_USE_SEND use this function
 * @param[in]  : stream_idx, audio channels index, see@BT_SVC_LEA_BC_SRC_AUDIO_CHAN_IDX_E
 * @param[in]  : data, write data buffer pointer
 * @param[in]  : data_len, write data length
 * @param[out] : none
 * @return: error code, see@BT_SVC_ERROR_CODE_E
 ****************************************************************************************
 */
int bt_svc_lea_bc_src_write_pcm_data(uint8_t stream_idx, uint8_t *data, uint16_t data_len);

/**
 ****************************************************************************************
 * @brief User notifies new data can be read , only bt_svc_lea_bc_src_stream_start
          function param.data_tran_mode = BT_SVC_LEA_BC_SRC_SVC_READ use this function
 * @param[in]  : stream_idx, audio channels index, see@BT_SVC_LEA_BC_SRC_AUDIO_CHAN_IDX_E
 * @param[out] : none
 * @return: error code, see@BT_SVC_ERROR_CODE_E
 ****************************************************************************************
 */
int bt_svc_lea_bc_src_produce_data_ntf(uint8_t stream_idx);

/**
 ****************************************************************************************
 * @brief Set local and remote device paly volume
 * @param[in]  : type, volume type, see@BT_SVC_LEA_BC_SRC_VOLUME_E
 * @param[in]  : volume_level, volume level, see@BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_E
 * @param[out] : none
 * @return: error code, see@BT_SVC_ERROR_CODE_E
 ****************************************************************************************
 */
int bt_svc_lea_bc_src_set_volume(uint8_t type, uint8_t volume_level);


//***************************
// input source
//***************************
int bts_lea_bc_src_input_a2dp_func_get(bt_svc_lea_bc_src_input_func_t * func);

int bts_lea_bc_src_input_usb_func_get(bt_svc_lea_bc_src_input_func_t * func);

int bts_lea_bc_src_input_wifi_func_get(bt_svc_lea_bc_src_input_func_t * func);

int bts_lea_bc_src_input_af_func_get(bt_svc_lea_bc_src_input_func_t * func);

int bts_lea_bc_src_input_smf_func_get(bt_svc_lea_bc_src_input_func_t * func);

//***************************
// codec
//***************************
void *bt_svc_lea_bc_src_codec_creat(bt_svc_lea_bc_src_codec_param_t *param);

bool bt_svc_lea_bc_src_codec_delete(void *codec);

int bt_svc_lea_bc_src_codec_decode(void *codec, uint8_t *input, uint32_t input_len,
                                                    uint8_t *output, uint32_t *output_len);

int bt_svc_lea_bc_src_codec_encode(void *codec, uint8_t *input, uint32_t input_len,
                                                    uint8_t *output, uint32_t *output_len);

//***************************
// buffer
//***************************
void bt_svc_lea_bc_src_buffer_init();

void bt_svc_lea_bc_src_buffer_deinit();

void bt_svc_lea_bc_src_buffer_free(void *ptr);

uint8_t *bt_svc_lea_bc_src_buffer_malloc(uint32_t size);

void bt_svc_lea_bc_src_buf_list_creat(bt_svc_lea_bc_src_buff_list_t *list);

void bt_svc_lea_bc_src_buf_list_destroy(    bt_svc_lea_bc_src_buff_list_t      *list);

uint8_t *bt_svc_lea_bc_src_buf_list_get_data_packet(bt_svc_lea_bc_src_buff_list_t *list, uint16_t *data_len);

uint8_t *bt_svc_lea_bc_src_buf_list_get_free_packet(bt_svc_lea_bc_src_buff_list_t *list);

void bt_svc_lea_bc_src_buf_list_push_data_packet(bt_svc_lea_bc_src_buff_list_t *list, uint8_t *data, uint16_t data_len);

void bt_svc_lea_bc_src_buf_list_push_free_packet(bt_svc_lea_bc_src_buff_list_t *list, uint8_t* data);

uint8_t bt_svc_lea_bc_src_buf_list_get_data_pkt_num(bt_svc_lea_bc_src_buff_list_t *list);

//***************************
// media
//***************************
void bt_svc_lea_bc_src_media_start(bt_svc_lea_bc_src_media_param_t *audio_info);

void bt_svc_lea_bc_src_media_stop();

void bt_svc_lea_bc_src_media_audio_trigger(uint32_t expected_play_time);

void bt_svc_lea_bc_src_media_audio_set_volume(bool source, uint8_t volume);

void bt_svc_lea_bc_src_media_audio_volume_clean(void);

uint8_t bt_svc_lea_bc_src_media_audio_get_volume(void);

uint32_t bt_svc_lea_bc_src_media_audio_get_start_play_time();

uint8_t bt_svc_lea_bc_src_media_player_play(void (start_cb)(uint8_t device_id, uint32_t context_type),
        void (stop_cb)(uint8_t device_id, uint32_t context_type));

uint8_t bt_svc_lea_bc_src_media_player_stop(void);

//***************************
// prompt
//***************************
uint8_t bt_svc_lea_bc_src_prompt_play(uint8_t id);

uint32_t bt_svc_lea_bc_src_prompt_data_get(uint8_t *buf, uint32_t buf_len);


#endif
