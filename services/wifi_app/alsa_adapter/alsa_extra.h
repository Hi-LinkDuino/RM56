#ifndef __ALSA_EXTRA_H__
#define __ALSA_EXTRA_H__

/**
 * NOTICE:
 * The main interface in alsa.h.
 * These interface is extra.
 * These interface is used for specific feature, not universal.
 * These interface maybe not enabled in source.
 * So NEED to confirm these interface is usable.
 * **/

#include <stdint.h>
#include <stdbool.h>

#include "alsa.h"

typedef enum
{
    ALSA_AUDIO_PROCESS_RENDER_USER_NOT,
    ALSA_AUDIO_PROCESS_RENDER_USER_WIFI,
    ALSA_AUDIO_PROCESS_RENDER_USER_BT_A2DP,
    ALSA_AUDIO_PROCESS_RENDER_USER_BT_SCO,
    ALSA_AUDIO_PROCESS_RENDER_USER_BT_BIS,

    ALSA_AUDIO_PROCESS_RENDER_USER_UNKNOWN,
} alsa_audio_process_render_user_t;

typedef enum
{
    ALSA_STATE_HOOK_PCM_FORMAT_S16 = 0,
    ALSA_STATE_HOOK_PCM_FORMAT_S24 = 1,
    ALSA_STATE_HOOK_PCM_FORMAT_S32 = 2,
    ALSA_STATE_HOOK_PCM_FORMAT_CNT,
} alsa_state_hook_pcm_format_t;

typedef enum
{
    ALSA_STATE_HOOK_ERROR = -2,
    ALSA_STATE_HOOK_FAIL = -1,
    ALSA_STATE_HOOK_OK = 0,
} alsa_state_hook_status_t;

/* type: 0 bt, 1 wifi/local ... */
typedef int (*alsa_state_open_hook)(int type, void *user_arg, int sample_rate, int channels, int format);
typedef int (*alsa_state_start_hook)(void *user_arg);
typedef int (*alsa_state_process_hook)(void *user_arg, const uint8_t *input, uint32_t len);
typedef int (*alsa_state_stop_hook)(void *user_arg);
typedef int (*alsa_state_flush_hook)(void *user_arg);
typedef int (*alsa_state_close_hook)(void *user_arg);

struct alsa_state_hook_t
{
    alsa_state_open_hook open;
    alsa_state_start_hook start;
    alsa_state_process_hook process;
    alsa_state_stop_hook stop;
    alsa_state_flush_hook flush;
    alsa_state_close_hook close;
    void *user_arg;
};

typedef void (*alsa_resample_user_cb_t)(uint8_t state, void *user_arg);

typedef void (*alsa_audio_process_render_dump_callback)(const uint8_t *data, uint32_t length);

typedef struct
{
    alsa_audio_process_render_dump_callback before_process_cb;
    alsa_audio_process_render_dump_callback after_process_cb;
} alsa_audio_process_render_dump_cb_t;

typedef struct alsa_resample alsa_resample_t;

typedef int (*alsa_trace_thirdparty_func_t)(const char *str);

typedef enum
{
    ALSA_TRIGGER_LEVEL_LOW = -1,
    ALSA_TRIGGER_LEVEL_NORMAL = 0,
    ALSA_TRIGGER_LEVEL_MEDIUM = 1,
    ALSA_TRIGGER_LEVEL_HIGH = 2,
} alsa_trigger_level_t;

#define ALSA_THIRDPARTY_TRACE alsa_trace_thirdparty

#ifdef __cplusplus
extern "C"
{
#endif

    int alsa_state_hook_register(struct alsa_state_hook_t *state_hook);
    int alsa_state_hook_unregister(void);

    int alsa_stop_immediate_with_fadeout(alsa_handle_t *h);

    int alsa_audio_process_render_open(alsa_audio_process_render_user_t user, int sample_rate,
                                       int in_channels, int out_channels,
                                       int in_bits, int out_bits, uint32_t timeout);
    int alsa_audio_process_render_close(alsa_audio_process_render_user_t user);
    int alsa_audio_process_render(uint8_t *data, uint32_t *in_out_length);
    /* preset: point the memory of preset. preset_size: unit Byte. NOTICES: sizeof(short) = 2 */
    int alsa_audio_process_render_set_cfg_preset(short *preset, uint32_t preset_size);
    /* param: point the memory of param. preset_size: unit Byte. NOTICES: sizeof(short) = 2! */
    int alsa_audio_process_render_set_cfg_param(short *param, uint32_t param_size);
    int alsa_audio_process_render_set_type(int32_t process_type);
    int alsa_audio_process_render_get_type(int32_t *process_type);
    int alsa_audio_process_render_dump_cb_register(alsa_audio_process_render_dump_cb_t *cb);
    int alsa_audio_process_render_set_bypass_state(bool enable);
    bool alsa_audio_process_render_get_bypass_state(void);
    int alsa_audio_process_render_reset_enable(bool enable);
    int alsa_audio_process_render_set_vol(float vol);
    int alsa_audio_process_render_set_bass_tuning(int bass_tuning_value);
    int alsa_audio_process_render_get_bass_tuning(int32_t *bass_tuning_value);

    int alsa_audio_process_open(int sample_rate, int channel_num, int bits, uint32_t dma_data_size);
    int alsa_audio_process_close(void);
    int alsa_audio_process(uint8_t *buf, uint32_t len);
    void alsa_audio_process_enter_tuning_mode(void);
    void alsa_audio_process_leave_tuning_mode(void);
    bool alsa_audio_process_is_tuning_mode(void);
    void alsa_audio_process_bypass_eq_set(uint8_t enable);
    uint8_t alsa_audio_process_bypass_eq_get(void);
    int alsa_audio_process_append_cfg(uint8_t cfg_index);
    int alsa_audio_process_append_cfg_on_vol_changed(uint32_t vol);

    alsa_resample_t *alsa_resample_process_open(uint8_t channels, uint8_t bits, uint32_t sample_rate_src, uint32_t sample_rate_dst);
    int alsa_resample_process(alsa_resample_t *resample, uint8_t **in_out_data, uint32_t *in_out_len);
    int alsa_resample_process_close(alsa_resample_t *resample);

    int alsa_volume_factory_set(uint8_t vol_dac_factory);
    int alsa_volume_debug_set(uint8_t vol_dac_debug);

    int alsa_volume_float_set(float volume);
    int alsa_volume_float_get(float *volume);

    void alsa_trace_thirdparty_func_register(alsa_trace_thirdparty_func_t func);
    int alsa_trace_thirdparty(uint8_t attr, const char *fmt, ...);

    int alsa_ultrasound_start(void);
    int alsa_ultrasound_stop(void);

    /**
     * return new_buffer_ptr
     * **/
    uint8_t *alsa_mono_to_stereo_16bit_with_new_buffer(uint8_t **in_out_buf, uint32_t *in_out_len);
    uint8_t *alsa_mono_to_stereo_32bit_with_new_buffer(uint8_t **in_out_buf, uint32_t *in_out_len);
    int alsa_mono_to_stereo_free_new_buffer(uint8_t *new_buffer_ptr);
    int alsa_stereo_to_mono_16bit(uint8_t *buf, uint32_t *len);
    int alsa_stereo_to_mono_32bit(uint8_t *buf, uint32_t *len);
    int alsa_stereo_to_mono_24bit(uint8_t *buf, uint32_t *len);
    void alsa_convert_16bit_to_32bit(int32_t *out, int16_t *in, uint32_t *in_out_len);
    void alsa_convert_16bit_to_24bit(int32_t *out, int16_t *in, uint32_t *in_out_len);
    /**
     * return new_buffer_ptr
     * **/
    uint8_t *alsa_convert_16bit_to_24bit_with_new_buffer(uint8_t **in_out_buf, uint32_t *in_out_len);
    uint8_t *alsa_convert_16bit_to_32bit_with_new_buffer(uint8_t **in_out_buf, uint32_t *in_out_len);
    int alsa_convert_16bit_free_new_buffer(uint8_t *new_buffer_ptr);
    int alsa_convert_24bit_to_32bit(uint8_t *buf, uint32_t len);
    int alsa_convert_32bit_to_24bit(uint8_t *buf, uint32_t len);
    int alsa_convert_32bit_to_16bit(uint8_t *buf, uint32_t *in_out_len);
    int alsa_convert_24bit_to_16bit(uint8_t *buf, uint32_t *in_out_len);

    int alsa_sw_gain_bypass(alsa_handle_t *h, uint8_t bypass);
    int alsa_sw_gain_fade_inout_bypass(alsa_handle_t *h, uint8_t bypass);
    int alsa_sw_gain_fade_inout_coef(alsa_handle_t *h, float coef);
    int alsa_sw_gain_pre_compensation_enable(alsa_handle_t *h, uint8_t enable, float compensation_db);

    /**
     * dump and debug interface
     * **/

    // dmup_format = 8/16/32
    void alsa_dump_trace(uint8_t *dump_data, uint32_t dump_count, uint8_t dmup_format);

    // user register dump callback and enable or disable dump point
    int alsa_dump_enable_point(alsa_dump_point_type_t type, alsa_dump_callback_t *cb, void *user_arg);
    int alsa_dump_disable_point(alsa_dump_point_type_t type);

    // internal FTP dump function config, enable and disable.
    int alsa_dump_ftp_server_cofig(char *ip_str, int port, char *user_name, char *pwd);
    int alsa_dump_after_instance_write_config(uint8_t instance_id, uint32_t sample_rate, uint8_t bits, uint8_t channels);
    int alsa_dump_after_sw_gain_config(uint32_t sample_rate, uint8_t bits, uint8_t channels);
    int alsa_dump_before_audio_process_config(uint32_t sample_rate, uint8_t bits, uint8_t channels);
    int alsa_dump_after_audio_process_config(uint32_t sample_rate, uint8_t bits, uint8_t channels);
    int alsa_dump_after_demuxer(bool enable);
    int alsa_dump_after_instance_write(bool enable);
    int alsa_dump_after_sw_gain(bool enable);
    int alsa_dump_before_audio_process(bool enable);
    int alsa_dump_after_audio_process(bool enable);
    int alsa_dump_point(alsa_dump_point_type_t type, uint8_t *data, uint32_t lenght);

    int alsa_set_trigger_level(alsa_handle_t *h, alsa_trigger_level_t level);

    int alsa_set_threshold(alsa_handle_t *h, uint32_t threshold);

    int alsa_set_dma_pcm_buffer_size(uint32_t dma_pcm_size);

    uint8_t alsa_adc_vol_get(uint8_t mic_id);
    void alsa_adc_vol_set(uint8_t mic_id, uint8_t vol);
    int alsa_adc_vol_to_db(uint8_t adc_vol);
    void alsa_adc_gain_ctrl_set_dac_vol(uint8_t dac_vol);
    void alsa_adc_gain_ctrl_with_dac_vol(bool dac_on);

    int alsa_set_maxdelaytime(alsa_handle_t *h, int delay_ms);

#ifdef __cplusplus
}
#endif

#endif