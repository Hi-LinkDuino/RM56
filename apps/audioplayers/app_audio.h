/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef __APP_AUDIO_H__
#define __APP_AUDIO_H__

#include "app_bt_stream.h"
#include "heap_api.h"

//should solve this problem
//#include "./../../utils/cqueue/cqueue.h"
//#include "cqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

//This buff is used as queue
#ifndef APP_AUDIO_BUFFER_SIZE
#if (RAM_SIZE >= 0x48000)

#if defined(AUDIO_ANC_FB_MC) && defined(ANC_APP) && !defined(__AUDIO_RESAMPLE__)

#if defined(A2DP_EQ_24BIT)
#define APP_AUDIO_BUFFER_SIZE (1024 * 210)
#else
#define APP_AUDIO_BUFFER_SIZE (1024 * 140)
#endif

#else
#define APP_AUDIO_BUFFER_SIZE (1024 * 128)
#endif

#elif (RAM_SIZE >= 0x40000)

#ifdef NEW_NV_RECORD_ENABLED
#define APP_AUDIO_BUFFER_SIZE (1024 * 62)
#else
#define APP_AUDIO_BUFFER_SIZE (1024 * 59)
#endif

#else // RAM_SIZE < 0x40000

#ifdef APP_TEST_AUDIO
#define APP_AUDIO_BUFFER_SIZE (1024 * 10)
#elif defined(SPEECH_RX_NS)
#define APP_AUDIO_BUFFER_SIZE (1024 * 37)
#else
#define APP_AUDIO_BUFFER_SIZE (1024 * 32)
#endif

#endif // RAM_SIZE < 0x40000
#endif

/// buffer size for hotword engine
#if defined(__ALEXA_WWE)
#define HW_ENGINE_BUF_SIZE      (67 * 1024)
#else
#define HW_ENGINE_BUF_SIZE      (0)
#endif

#if defined(VOICE_DETECTOR_SENS_EN)
#undef HW_ENGINE_BUF_SIZE
#define HW_ENGINE_BUF_SIZE  (0)
#endif

/// buffer size for encode
#if defined(VOC_ENCODE_ENABLE)
#if defined(__BIXBY)
#define ENCODE_BUF_SIZE         (10 * 1024)
#elif defined(AI_32KBPS_VOICE) || defined(KNOWLES_UART_DATA) || defined(__AMA_VOICE__)
#define ENCODE_BUF_SIZE         (9 * 1024)
#elif defined(DUAL_MIC_RECORDING)
#define ENCODE_BUF_SIZE         (5 * 1024)
#else
#define ENCODE_BUF_SIZE         (5 * 1024)
#endif
#else
#define ENCODE_BUF_SIZE         (0)
#endif

/// encoded data cache buffer size
#if defined(DUAL_MIC_RECORDING)
#define ENCODED_DATA_BUF_SIZE   (50 * 1024)
#else
#define ENCODED_DATA_BUF_SIZE   (0)
#endif

/// buffer size used for encode codec
/// use opus to encode
#if defined(VOC_ENCODE_OPUS)
#define ENCODE_CODEC_BUF_SIZE   (54 * 1024)
#else //!< use codec except for opus
#define ENCODE_CODEC_BUF_SIZE   (0)
#endif

/// buffer size for PCM data cache queue
#ifdef __AI_VOICE__
#ifdef __BIXBY
#define PCM_DATA_CACHE_BUF_SIZE     (8000)
#else
#define PCM_DATA_CACHE_BUF_SIZE     (64 * 1024)
#endif
#else
#define PCM_DATA_CACHE_BUF_SIZE     (0)
#endif

/// buffer size for audio flinger cache PCM data
#ifdef __AI_VOICE__
#define PCM_AUDIO_FLINGER_BUF_SIZE  (4 * 1024)
#else
#define PCM_AUDIO_FLINGER_BUF_SIZE  (0)
#endif

/// buffer sizr for VAD
#if defined(VOICE_DETECTOR_EN)
#define VAD_BUF_SIZE            (32 * 1024)
#else
#define VAD_BUF_SIZE            (0)
#endif

// FIXME: distinguish between ANC ASSIST with PSAP
#if (defined(ANC_ASSIST_ENABLED) || defined(PSAP_APP)) && defined(DUAL_MIC_RECORDING)
#define ANC_ASSIST_BUF_SIZE     (25 * 1024)
#else
#define ANC_ASSIST_BUF_SIZE     (0)
#endif

#if defined(STEREO_RECORD_PROCESS) && defined(DUAL_MIC_RECORDING)
#define STEREO_RECORD_PROCESS_BUF_SIZE  (10 * 1024)
#else
#define STEREO_RECORD_PROCESS_BUF_SIZE  (0)
#endif

#define APP_CAPTURE_AUDIO_BUFFER_SIZE   (HW_ENGINE_BUF_SIZE + ENCODE_BUF_SIZE + ENCODED_DATA_BUF_SIZE + \
                                        ENCODE_CODEC_BUF_SIZE + PCM_DATA_CACHE_BUF_SIZE + PCM_AUDIO_FLINGER_BUF_SIZE + \
                                        VAD_BUF_SIZE + ANC_ASSIST_BUF_SIZE + STEREO_RECORD_PROCESS_BUF_SIZE)

#if defined(A2DP_SCALABLE_ON)
#define SCALABLE_MAX_FRAME_SIZE (900)
#define SCALABLE_FRAME_SIZE     (864)
#define SCALABLE_HEAD_SIZE      (4)     /* pick something big enough to hold a bunch of frames */
#define SCALABLE_READBUF_SIZE   (500)   /* pick something big enough to hold a bunch of frames */
#define SCALABLE_DECODER_SIZE   (17456) /* pick something big enough to hold a bunch of frames */
#endif

#define MAX_AUDIO_BUF_LIST 20
enum APP_AUDIO_CACHE_T {
    APP_AUDIO_CACHE_CACHEING= 0,
    APP_AUDIO_CACHE_OK,
    APP_AUDIO_CACHE_QTY,
};

#define APP_AUDIO_SET_MESSAGE(appevt, id, status) (appevt = (((uint32_t)id&0xffff)<<16)|(status&0xffff))
#define APP_AUDIO_GET_ID(appevt, id) (id = (appevt>>16)&0xffff)
#define APP_AUDIO_GET_STATUS(appevt, status) (status = appevt&0xffff)
#define APP_AUDIO_GET_AUD_ID(appevt, aud_id) (aud_id = appevt)
#define APP_AUDIO_GET_FREQ(appevt, freq) (freq = appevt)

#define APP_AUDIO_InitCQueue(a, b, c)   InitCQueue(a, b, c)
#define APP_AUDIO_AvailableOfCQueue(a)     AvailableOfCQueue(a)
#define APP_AUDIO_LengthOfCQueue(a)     LengthOfCQueue(a)
#define APP_AUDIO_PeekCQueue(a, b, c, d, e, f)   PeekCQueue(a, b, c, d, e, f)
#define APP_AUDIO_EnCQueue(a, b, c)     EnCQueue(a, b, c)
#define APP_AUDIO_DeCQueue(a, b, c)     DeCQueue(a, b, c)

#define app_audio_mempool_init  syspool_init

#define app_audio_mempool_noclr_init syspool_init

#define app_audio_mempool_use_mempoolsection_init syspool_init

#define app_audio_mempool_free_buff_size syspool_free_size

#define app_audio_mempool_get_buff syspool_get_buff

#if defined(A2DP_LDAC_ON)
#define  app_audio_mempool_total_buf              syspool_total_size
#define  app_audio_mempool_force_set_buff_used    syspool_force_used_size
#endif


void UNLOCK_APP_AUDIO_QUEUE();

void LOCK_APP_AUDIO_QUEUE();

uint32_t app_audio_lr_balance(uint8_t *buf, uint32_t len, int8_t balance);

void app_audio_mempool_init_with_specific_size(uint32_t size);

int app_audio_pcmbuff_init(uint8_t *buff, uint16_t len);

int app_audio_pcmbuff_space(void);

int app_audio_pcmbuff_length(void);

int app_audio_pcmbuff_put(uint8_t *buff, uint16_t len);

int app_audio_pcmbuff_get(uint8_t *buff, uint16_t len);

int app_audio_pcmbuff_discard(uint16_t len);

void app_audio_memcpy_16bit(int16_t *des, int16_t *src, int len);

void app_audio_memset_16bit(int16_t *des, int16_t val, int len);


#define app_audio_sendrequest(id, status, ptr) app_audio_sendrequest_param((id), (status), (ptr), 0)

int app_audio_sendrequest_param(uint16_t id, uint8_t status, uint32_t ptr, uint32_t param);

#ifdef BT_MEDIA_SUPPORT_LPLAYER
int app_audio_sendrequest_callback(uint32_t cb, uint32_t param0, uint32_t param1, uint32_t param2);
#endif

void app_audio_open(void);

void app_audio_close(void);

bool app_audio_list_append(APP_AUDIO_STATUS* aud_status);

bool app_audio_list_playback_exist(void);

void app_audio_list_playback_clear(void);

bool app_audio_list_rmv_callback(APP_AUDIO_STATUS *status_close, APP_AUDIO_STATUS *status_next, enum APP_BT_AUDIO_Q_POS pos);

void app_audio_list_rmv_by_info(APP_AUDIO_STATUS *info);

void app_audio_list_clear();

int app_capture_audio_mempool_init(void);

uint32_t app_capture_audio_mempool_free_buff_size();

int app_capture_audio_mempool_get_buff(uint8_t **buff, uint32_t size);

int app_media_mempool_init(void);

uint32_t app_media_mempool_free_buff_size();

int app_media_mempool_get_buff(uint8_t **buf, uint32_t size);

void app_start_post_chopping_timer(void);
#ifdef __cplusplus
}
#endif

#endif//__FMDEC_H__
