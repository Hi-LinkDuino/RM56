#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_dump.h"
#include "pcm_audio.h"
#include "mcu_audio.h"
#include "kfifo/kfifo.h"
#include "hal_trace.h"
#ifdef PCM_AUDIO_OUT_ENABLE
#include "alsa.h"
#endif

enum _audio_inout
{
    AUDIO_INOUT_OPEN,
    AUDIO_INOUT_START,
    AUDIO_INOUT_STOP,
    AUDIO_INOUT_CLOSE,
};

static pcm_audiodump_t pcm_audiodump = {0};
#define _MULTI_AUDIO_INOUT_NUM 6
typedef struct
{
    int idx;
    void *handle;
    enum _audio_inout state;
    enum pcm_mode mode;
    int samplerate;
    int channels;
    enum pcm_format format;
    pcm_state_callback_t cbf;
    void *user;
    void *param;
} multi_audio_inout_t;
static multi_audio_inout_t audio_inout[_MULTI_AUDIO_INOUT_NUM];

/* resample 48k to 16k cb func */
void pcm_audio_resample_48_to_16_cb(void *hdl, unsigned char *in, int in_len, int samplerate, int ch_num, unsigned char **p_out, int *p_out_len)
{
    if ((hdl == NULL) || (in == NULL)) {
        printf("%s hdl(%p) or in(%p) is NULL!\n", __FUNCTION__, hdl, in);
        return;
    }
    if ((p_out ==  NULL) || (p_out_len == NULL)) {
        printf("%s p_out(%p) or p_out_len(%p) is NULL!\n", __FUNCTION__, p_out, p_out_len);
        return;
    }
    multi_data_dump_t *_hdl = (multi_data_dump_t *)hdl;
    //printf("%s _hdl->samplerate(%d) in_len(%d) insamplerate(%d) inch_num(%d) MIC_BITRATE:%d\n", __FUNCTION__, _hdl->samplerate, in_len, samplerate, ch_num, MIC_BITRATE);
    if ((_hdl->samplerate == 16000) &&  (MIC_BITRATE == 48000)) {
        if (_hdl->samplerate < MIC_BITRATE) {
            if (_hdl->_resample_buf == NULL) {
                uint32_t len = 16 * (MIC_BITS / 8) * CHAN_NUM_CAPTURE * CAPTURE_PERIOD_MS;
                _hdl->_resample_buf = malloc(len);
                if (_hdl->_resample_buf == NULL) {
                    printf("%s malloc _resample_buf failed!!\n", __FUNCTION__);
                }
                memset(_hdl->_resample_buf, 0 , len);
                printf("%s malloc _hdl->_resample_buf:%p len:%u success!\n", __FUNCTION__, _hdl->_resample_buf, len);
            }
        }
        *p_out = _hdl->_resample_buf;
        *p_out_len = mic_resample_48_to_16(in, *p_out, in_len);
    }
}


/**
 * @brief Open pcm handle
 * @param[in] mode: specifies that the PCM is output or input
 * @param[in] samplerate: Audio pcm sample rate
 * @param[in] channels: Audio pcm channel count
 * @param[in] format: Audio pcm format
 * @return: pcm handle
 */
pcm_handle_t mic_pcm_open(enum pcm_mode mode, int samplerate, int channels, enum pcm_format format,data_dump_input_cb_t cb)
{
    multi_audio_inout_t *_hdl = NULL;

    for (int i = 0; i < _MULTI_AUDIO_INOUT_NUM; i++)
    {
        if (!audio_inout[i].handle)
        {
            _hdl = &audio_inout[i];
            _hdl->idx = i + 1;
            _hdl->handle = (void *)0x12345678;
            break;
        }
    }
    if (!_hdl)
    {
        TRACE(0, "%s fail since no more handles! [mode=%d, samp=%d, ch=%d, fmt=%d]\n",
               __FUNCTION__, mode, samplerate, channels, format);
        return NULL;
    }
    _hdl->mode = mode;
    _hdl->samplerate = samplerate & 0xFFFFFF;
    _hdl->channels = channels;
    _hdl->format = format;
    _hdl->state = AUDIO_INOUT_OPEN;

    switch (mode & 0x01)
    {
    case PCM_OUT:
    {
#ifdef PCM_AUDIO_OUT_ENABLE
        alsa_mode_t alsa_mode = (alsa_mode_t)mode;
        alsa_pcm_format_t alsa_pcm_format = (alsa_pcm_format_t)format;
        alsa_handle_t *h = alsa_open(alsa_mode, samplerate, channels, alsa_pcm_format);
        _hdl->handle = h;
#endif
    }
    break;
    case PCM_IN:
    {
        int large_buf = (samplerate & 0xFF000000) >> 24;
        int _kfifo_len = PCM_AUDIO_KFIFO_CAPTURE_SIZE;
        if (large_buf)
        {
            _kfifo_len *= large_buf;
        }
        TRACE(0, "%s [kfifo len: %dk]", __FUNCTION__, (_kfifo_len / 1024));
        if(cb)
            _hdl->param = data_dump_open_withcb(aud_record_subscribe, cb, _kfifo_len / 1024, _hdl->samplerate, channels, 0);
        else
            _hdl->param = data_dump_open(aud_record_subscribe, _kfifo_len / 1024, _hdl->samplerate, channels);
    }
    break;
    default:
        break;
    }

    TRACE(0, "%s handle=%p idx=%d param=%p [mode=%d, samp=%d, ch=%d, fmt=%d]\n",
           __FUNCTION__, _hdl, _hdl->idx, _hdl->param, _hdl->mode, _hdl->samplerate, _hdl->channels, _hdl->format);
    return (pcm_handle_t)_hdl;
}

/**
* @brief Open pcm handle
* @param[in] samplerate: Audio pcm sample rate
* @param[in] channels: Audio pcm channel count
* @param[in] cb: call back function
* @return: pcm handle
*/
pcm_handle_t pcm_open(int samplerate, int channelst, data_dump_input_cb_t cb)
{
    return mic_pcm_open(PCM_IN,samplerate,channelst,PCM_FORMAT_S16_LE,cb);
}

/**
 * @brief Start pcm device
 * @param[in] pcm: Audio pcm handle
 * @return: - 0: success; - other: fail
 */
int pcm_start(pcm_handle_t pcm)
{
    if (pcm == NULL) {
        TRACE(0, "%s pcm is NULL!", __FUNCTION__);
        return -1;
    }

    multi_audio_inout_t *_hdl = (multi_audio_inout_t *)pcm;
    switch (_hdl->mode & 0x01)
    {
    case PCM_OUT:
        break;
    case PCM_IN:
        if (_hdl->param != NULL) {
            data_dump_start(_hdl->param);
        }
        break;
    default:
        break;
    }
    _hdl->state = AUDIO_INOUT_START;
    return 0;
}

/**
 * @brief Write data to pcm device that opened as PCM_OUT mode
 * @param[in] pcm: Audio pcm handle
 * @param[in] buf: PCM data buffer
 * @param[in] size: buffer size in bytes, not samples
 * @return: bytes written, <0 means fail
 */
int pcm_write(pcm_handle_t pcm, void *buf, unsigned int size)
{
    if (pcm == NULL || buf == NULL) {
        TRACE(0, "%s pcm(%p) or buf is NULL!", __FUNCTION__, pcm);
        return -1;
    }
    int ret = size;
    multi_audio_inout_t *_hdl = (multi_audio_inout_t *)pcm;
    switch (_hdl->mode)
    {
    case PCM_OUT:
    {
#ifdef PCM_AUDIO_OUT_ENABLE
        if (pcm_audiodump.cb)
        {
            pcm_audiodump.cb(buf, size, pcm_audiodump.arg);
        }
        alsa_handle_t *h = (alsa_handle_t *)_hdl->handle;
        ret = alsa_write(h, (uint8_t *)buf, size);
#endif
    }
    break;
    case PCM_IN:
    default:
        break;
    }
    return ret;
}

/**
 * @brief Read data from pcm device that opened as PCM_IN mode
 * @param[in] pcm: Audio pcm handle
 * @param[in] buf: PCM data buffer
 * @param[in] size: buffer size in bytes, not samples
 * @return: bytes read, <0 means fail
 */
int pcm_read(pcm_handle_t pcm, void *buf, unsigned int size)
{
    if (pcm == NULL || buf == NULL) {
        TRACE(0, "%s pcm(%p) or buf is NULL!", __FUNCTION__, pcm);
        return -1;
    }
    int ret = size;
    multi_audio_inout_t *_hdl = (multi_audio_inout_t *)pcm;
    switch (_hdl->mode)
    {
    case PCM_IN:
        if (_hdl->param != NULL) {
            ret = data_dump_read(_hdl->param, buf, size);
        }
        break;
    case PCM_OUT:
    default:
        break;
    }
    return ret;
}

/**
 * @brief Stop pcm device
 * @param[in] pcm: Audio pcm handle
 * @return: - 0 : success; - other: fail
 */
int pcm_stop(pcm_handle_t pcm)
{
    if (pcm == NULL) {
        TRACE(0, "%s pcm is NULL!", __FUNCTION__);
        return -1;
    }
    int ret = 0;
    multi_audio_inout_t *_hdl = (multi_audio_inout_t *)pcm;
    switch (_hdl->mode & 0x01)
    {
    case PCM_OUT:
    {
#ifdef PCM_AUDIO_OUT_ENABLE
        alsa_handle_t *h = (alsa_handle_t *)_hdl->handle;
        ret = alsa_stop(h);
#endif
    }
    break;
    case PCM_IN:
        if (_hdl->param != NULL) {
            data_dump_stop(_hdl->param);
        }
        break;
    default:
        break;
    }
    _hdl->state = AUDIO_INOUT_STOP;
    return ret;
}

/**
 * @brief Close pcm handle
 * @param[in] pcm: Audio pcm handle
 */
void pcm_close(pcm_handle_t pcm)
{
    if (pcm == NULL) {
        TRACE(0, "%s pcm is NULL!", __FUNCTION__);
        return;
    }
    multi_audio_inout_t *_hdl = (multi_audio_inout_t *)pcm;
    TRACE(0, "%s handle=%p idx=%d [mode=%d, samp=%d, ch=%d, fmt=%d]\n",
           __FUNCTION__, _hdl, _hdl->idx, _hdl->mode, _hdl->samplerate, _hdl->channels, _hdl->format);
    _hdl->state = AUDIO_INOUT_CLOSE;
    switch (_hdl->mode & 0x01)
    {
    case PCM_OUT:
    {
#ifdef PCM_AUDIO_OUT_ENABLE
        alsa_handle_t *h = (alsa_handle_t *)_hdl->handle;
        alsa_close(h);
#endif
    }
    break;
    case PCM_IN:
    {
        data_dump_close(_hdl->param, aud_record_unsubscribe);
    }
    break;
    default:
        break;
    }
    memset(_hdl, 0, sizeof(multi_audio_inout_t));
}

/**
 * @brief Set state listener
 * @param[in] pcm: Audio pcm handle
 * @param[in] cbf: state listener handle
 * @param[in] user: listener private data
 */
void pcm_audio_set_state_listener(pcm_handle_t pcm, pcm_state_callback_t cbf, const void *user)
{
    if (pcm == NULL) {
        return;
    }
    multi_audio_inout_t *_hdl = (multi_audio_inout_t *)pcm;
    _hdl->cbf = cbf;
    _hdl->user = (void *)user;
    switch (_hdl->mode & 0x01)
    {
    case PCM_OUT:
    {
#ifdef PCM_AUDIO_OUT_ENABLE
        alsa_handle_t *h = (alsa_handle_t *)_hdl->handle;
        alsa_register_pcm_state_callback(h, (alsa_pcm_state_callback_t)cbf, user);
#endif
    }
    break;
    case PCM_IN:
        break;
    default:
        break;
    }
}

void pcm_audiodump_cb_register(pcm_audiodump_cb_t cbf, void *param)
{
    pcm_audiodump.cb = cbf;
    pcm_audiodump.arg = param;
}

void pcm_audiodump_cb_unregister(void)
{
    pcm_audiodump.cb = NULL;
    pcm_audiodump.arg = NULL;
}
