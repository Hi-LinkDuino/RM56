#ifndef __DSP_AUDIO_PROCESS_H__
#define __DSP_AUDIO_PROCESS_H__

#include <stdlib.h>
#include "cmsis_os2.h"

typedef enum
{
    DSP_PCM_M33_TO_A7 = 100,
    DSP_PCM_A7_TO_M33,
    DSP_OPEN_RESULT_M33_TO_A7,
    DSP_OPEN_RESULT_A7_TO_M33,

    DSP_SET_CFG_PRESET_M33_TO_A7,
    DSP_SET_CFG_PRESET_A7_TO_M33,
    DSP_SET_CFG_PARAM_M33_TO_A7,
    DSP_SET_CFG_PARAM_A7_TO_M33,
    DSP_SET_TYPE_M33_TO_A7,
    DSP_SET_TYPE_A7_TO_M33,
    DSP_SET_VOL_M33_TO_A7,
    DSP_SET_VOL_A7_TO_M33,
    DSP_SET_BASS_TUNING_M33_TO_A7,
    DSP_SET_BASS_TUNING_A7_TO_M33,
    DSP_RESET,
} dsp_audio_process_msg_type_t;

// should not modify.
#define DSP_AUDIO_PROCESS_DATA_LENGTH_MAX (24576)

typedef struct
{
    dsp_audio_process_msg_type_t msg_type;
    uint32_t index;
    uint8_t data[DSP_AUDIO_PROCESS_DATA_LENGTH_MAX];
    uint32_t data_len;
} dsp_audio_process_msg_t;

// if retrun value not 0 will bypass all process as a error state.
typedef int (*dsp_audio_process_adapter_open_t)(void);

// input  48k 32bit 2ch
// output 48k 32bit 2ch
typedef int (*dsp_audio_process_adapter_process_t)(uint8_t *in_out_buf, uint32_t *in_out_lenth);

typedef int (*dsp_audio_process_adapter_set_preset_t)(uint8_t *arg, uint32_t arg_size);
typedef int (*dsp_audio_process_adapter_set_config_t)(uint8_t *arg, uint32_t arg_size);
typedef int (*dsp_audio_process_adapter_set_type_t)(uint8_t *arg, uint32_t arg_size);
typedef int (*dsp_audio_process_adapter_set_bass_tuning_t)(uint8_t *arg, uint32_t arg_size);
typedef int (*dsp_audio_process_adapter_set_vol_t)(uint8_t *arg, uint32_t arg_size);
typedef int (*dsp_audio_process_adapter_reset_t)(uint8_t *arg, uint32_t arg_size);

typedef struct
{
    // The follow is mandatory.
    dsp_audio_process_adapter_open_t open;
    dsp_audio_process_adapter_process_t process;

    // The follow is optional. if not use , assign NULL.
    dsp_audio_process_adapter_set_preset_t set_preset;
    dsp_audio_process_adapter_set_config_t set_config;
    dsp_audio_process_adapter_set_type_t set_type;
    dsp_audio_process_adapter_set_bass_tuning_t set_bass_tuning;
    dsp_audio_process_adapter_set_vol_t set_vol;
    dsp_audio_process_adapter_reset_t reset;
} dsp_audio_process_adapter_t;

#ifdef __cplusplus
extern "C"
{
#endif

    // call in A core setup. a7_dsp_main().
    // user should set adapter pointer.
    int dsp_audio_process_open(dsp_audio_process_adapter_t *adapter);

#ifdef __cplusplus
}
#endif

#endif
