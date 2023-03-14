#ifndef __ALSA_H__
#define __ALSA_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    ALSA_MODE_UNKNOWN = -1,

    /* PLAYBACK */
    ALSA_MODE_OUT = (1 << 0),

    /* CAPTURE */
    ALSA_MODE_IN = (1 << 1),

    /**
     * user register a callback to
     * pull data (for capture) or
     * push data (for playback).
     * NOT invoke read data or write data.
     * **/
    ALSA_MODE_DATA_PULL_PUSH = (1 << 2),

    ALSA_MODE_ULTRASONIC = (1 << 3),

} alsa_mode_t;

typedef enum
{
    /** Signed, 8-bit */
    // ALSA_PCM_FORMAT_S8 = 1,

    /** Signed 16-bit, little endian */
    ALSA_PCM_FORMAT_S16_LE = 0,

    /** Signed, 16-bit, big endian */
    // ALSA_PCM_FORMAT_S16_BE = 2,

    /** Signed, 24-bit (32-bit in memory), little endian */
    ALSA_PCM_FORMAT_S24_LE,

    /** Signed, 24-bit (32-bit in memory), big endian */
    // ALSA_PCM_FORMAT_S24_BE,

    /** Signed, 24-bit, little endian */
    // ALSA_PCM_FORMAT_S24_3LE,

    /** Signed, 24-bit, big endian */
    // ALSA_PCM_FORMAT_S24_3BE,

    /** Signed, 32-bit, little endian */
    ALSA_PCM_FORMAT_S32_LE,

    /** Signed, 32-bit, big endian */
    // ALSA_PCM_FORMAT_S32_BE,

    /** Max of the enumeration list, not an actual format. */
    ALSA_PCM_FORMAT_MAX
} alsa_pcm_format_t;

typedef enum
{
    ALSA_STATE_ERROR = -1,
    ALSA_STATE_CLOSE = 0,
    ALSA_STATE_OPEN,
    ALSA_STATE_START,
    ALSA_STATE_STOP,
} alsa_state_t;

typedef enum
{
    ALSA_PCM_STATE_UNKNOWN = 0,
    ALSA_PCM_STATE_UNDERRUN,
    ALSA_PCM_STATE_UNDERRUN_EXIT,
    ALSA_PCM_STATE_OVERRUN,
    ALSA_PCM_STATE_OVERRUN_EXIT,
} alsa_pcm_state_t;

typedef enum
{
    ALSA_USER_TYPE_UNKNOWN = 0,
    ALSA_USER_TYPE_LOCAL,
    ALSA_USER_TYPE_TTS,
    ALSA_USER_TYPE_WIFI,
    ALSA_USER_TYPE_USB,
    ALSA_USER_TYPE_BT,
    ALSA_USER_TYPE_BUT,
} alsa_user_type_t;

typedef void (*alsa_pcm_state_callback_t)(alsa_pcm_state_t state, void *user, void *info);

typedef int (*alsa_data_pull_push_cb_t)(uint8_t *data, uint32_t length);

typedef struct alsa_handle alsa_handle_t;

typedef int (*alsa_dump_callback_open_t)(void *arg);
typedef int (*alsa_dump_callback_write_t)(void *arg, uint8_t *data, uint32_t data_len);
typedef int (*alsa_dump_callback_close_t)(void *arg);
typedef struct
{
    alsa_dump_callback_open_t open;
    alsa_dump_callback_write_t write;
    alsa_dump_callback_close_t close;
} alsa_dump_callback_t;

typedef enum
{
    // alsa internal point
    ALSA_DUMP_POINT_AFTER_INSTANCE_WRITE,
    ALSA_DUMP_POINT_AFTER_SW_GAIN,

    ALSA_DUMP_POINT_BEFORE_AUDIO_PROCESS,
    ALSA_DUMP_POINT_AFTER_AUDIO_PROCESS,

    // alsa external point eg. player demuxer and decoder.
    ALSA_DUMP_POINT_USER_AFTER_DEMUXER,
    ALSA_DUMP_POINT_USER_AFTER_DECODER,

    //
    ALSA_DUMP_POINT_MAX,
} alsa_dump_point_type_t;

typedef struct alsa_dump_handle alsa_dump_t;

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * system invoke it, user should not invoke it
     * **/
    void alsa_init(void);

    /**
     * user interface
     * **/

    alsa_handle_t *alsa_open(alsa_mode_t mode, int sample_rate, int channels, alsa_pcm_format_t format);

    /**
     * if (mode & ALSA_MODE_DATA_PULL_PUSH) need use this interface register cb to pull(for capture) or push(for playback) data.
     * **/
    int alsa_set_data_pull_psuh_callback(alsa_handle_t *h, alsa_data_pull_push_cb_t cb);

    int alsa_start(alsa_handle_t *h);

    int alsa_pause(alsa_handle_t *h);

    int alsa_stop(alsa_handle_t *h);

    int alsa_flush(alsa_handle_t *h);

    int alsa_close(alsa_handle_t *h);

    int alsa_write(alsa_handle_t *h, uint8_t *buf, uint32_t length);

    int alsa_read(alsa_handle_t *h, uint8_t *buf, uint32_t length);

    int alsa_register_pcm_state_callback(alsa_handle_t *h, alsa_pcm_state_callback_t cb, void *arg);

    int alsa_set_user_type(alsa_handle_t *h, alsa_user_type_t type);

    /**
     * volume ctrl interface
     * **/

    void alsa_mute_set(void);

    void alsa_mute_cancel(void);

    void alsa_volume_set(uint8_t vol_dac);

    uint8_t alsa_volume_get(void);

    int alsa_volume_update(void);

    int alsa_dac_vol_level_to_db(uint8_t dac_vol);

    /**
     * alsa internal buffer
     * The following interfaces only work on playback mode(ALSA_MODE_OUT)
     * **/
    int alsa_internal_buffer_get_total_length(alsa_handle_t *h, uint32_t *length);
    int alsa_internal_buffer_get_readable_length(alsa_handle_t *h, uint32_t *length);
    int alsa_internal_buffer_get_writeable_length(alsa_handle_t *h, uint32_t *length);
    int alsa_internal_buffer_get_readable_length_with_dma(alsa_handle_t *h, uint32_t *length);
    int alsa_dma_buffer_get_total_length(uint32_t *length);
    int alsa_dma_buffer_get_readable_length(uint32_t *length);

#ifdef __cplusplus
}
#endif

#endif