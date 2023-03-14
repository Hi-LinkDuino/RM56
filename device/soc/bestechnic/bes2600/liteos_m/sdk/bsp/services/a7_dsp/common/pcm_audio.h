/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#ifndef __PCM_AUDIO_H__
#define __PCM_AUDIO_H__

#include <stdint.h>
#include "data_dump.h"

//#define PCM_AUDIO_OUT_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

#define PCM_AUDIO_KFIFO_CAPTURE_SIZE              (64*1024)

enum mic_ch_map {
    MIC_1 = 0,
    MIC_2 = 1,
    REF_1 = 2,
    REF_2 = 3,
    CH_MAX = 4,

    CH_ALL = 0xF,
};

/** A flag that specifies that the PCM is output or input.
 * Used in @ref mic_pcm_open.
 */
enum pcm_mode {
    PCM_OUT = 0,
    PCM_IN = 1,
    PCM_ULTRASONIC = (1 << 1),
};

/** Audio sample format of a PCM.
 * The first letter specifiers whether the sample is signed or unsigned.
 * The letter 'S' means signed. The letter 'U' means unsigned.
 * The following number is the amount of bits that the sample occupies in memory.
 * Following the underscore, specifiers whether the sample is big endian or little endian.
 * The letters 'LE' mean little endian.
 * The letters 'BE' mean big endian.
 * This enumeration is used in the @ref pcm_config structure.
 */
enum pcm_format {
    /** Signed, 8-bit */
    PCM_FORMAT_S8 = 1,
    /** Signed 16-bit, little endian */
    PCM_FORMAT_S16_LE = 0,
    /** Signed, 16-bit, big endian */
    PCM_FORMAT_S16_BE = 2,
    /** Signed, 24-bit (32-bit in memory), little endian */
    PCM_FORMAT_S24_LE,
    /** Signed, 24-bit (32-bit in memory), big endian */
    PCM_FORMAT_S24_BE,
    /** Signed, 24-bit, little endian */
    PCM_FORMAT_S24_3LE,
    /** Signed, 24-bit, big endian */
    PCM_FORMAT_S24_3BE,
    /** Signed, 32-bit, little endian */
    PCM_FORMAT_S32_LE,
    /** Signed, 32-bit, big endian */
    PCM_FORMAT_S32_BE,
    /** Max of the enumeration list, not an actual format. */
    PCM_FORMAT_MAX
};

enum pcm_state {
    PCM_STATE_UNDERRUN,
    PCM_STATE_UNDERRUN_EXIT,
    PCM_STATE_OVERRUN,
    PCM_STATE_OVERRUN_EXIT,
    PCM_STATE_MAX
};

typedef void (*pcm_state_callback_t)(enum pcm_state state, void *user, void *info);

typedef void *pcm_handle_t;


/* resample 48k to 16k cb func */
void pcm_audio_resample_48_to_16_cb(void *hdl, unsigned char *in, int in_len, int samplerate, int ch_num, unsigned char **p_out, int *p_out_len);

/**
 * @brief Open pcm handle
 * @param[in] mode: specifies that the PCM is output or input
 * @param[in] samplerate: Audio pcm sample rate
 * @param[in] channels: Audio pcm channel count
 * @param[in] format: Audio pcm format
 * @param[in] cb: Audio pcm cb
 * @return: pcm handle
 */
pcm_handle_t mic_pcm_open(enum pcm_mode mode, int samplerate, int channels, enum pcm_format format,data_dump_input_cb_t cb);

/**
* @brief Open pcm handle
* @param[in] samplerate: Audio pcm sample rate
* @param[in] channels: Audio pcm channel count
* @param[in] cb: call back function
* @return: pcm handle
*/
pcm_handle_t pcm_open(int samplerate, int channelst, data_dump_input_cb_t cb);

int pcm_change_sample_rate(pcm_handle_t pcm, int samplerate);

/**
 * @brief Start pcm device
 * @param[in] pcm: Audio pcm handle
 * @return: - 0: success; - other: fail
 */
int pcm_start(pcm_handle_t pcm);

/**
 * @brief Write data to pcm device that opened as PCM_OUT mode
 * @param[in] pcm: Audio pcm handle
 * @param[in] buf: PCM data buffer
 * @param[in] size: buffer size in bytes, not samples
 * @return: bytes written, <0 means fail
 */
int pcm_write(pcm_handle_t pcm, void *buf, unsigned int size);

/**
 * @brief Read data from pcm device that opened as PCM_IN mode
 * @param[in] pcm: Audio pcm handle
 * @param[in] buf: PCM data buffer
 * @param[in] size: buffer size in bytes, not samples
 * @return: bytes read, <0 means fail
 */
int pcm_read(pcm_handle_t pcm, void *buf, unsigned int size);

/**
 * @brief Stop pcm device
 * @param[in] pcm: Audio pcm handle
 * @return: - 0 : success; - other: fail
 */
int pcm_stop(pcm_handle_t pcm);

/**
 * @brief Close pcm handle
 * @param[in] pcm: Audio pcm handle
 */
void pcm_close(pcm_handle_t pcm);


typedef void (*pcm_audiodump_cb_t)(uint8_t * data, uint32_t len, void * param);
typedef struct
{
    pcm_audiodump_cb_t cb;
    void *arg;
} pcm_audiodump_t;

/**
 * @brief Set state listener
 * @param[in] pcm: Audio pcm handle
 * @param[in] cbf: state listener handle
 * @param[in] user: listener private data
 */
void pcm_audio_set_state_listener(pcm_handle_t pcm, pcm_state_callback_t cbf, const void *user);
void pcm_audiodump_cb_register(pcm_audiodump_cb_t cbf, void *param);
void pcm_audiodump_cb_unregister(void);

#ifdef __cplusplus
}
#endif

#endif /* __PCM_AUDIO_H__ */
