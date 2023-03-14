#ifndef _MCU_AUDIO_H_
#define _MCU_AUDIO_H_

#include "a7_dump.h"
#include "aud_param.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAPTURE_SIZE_RATIO 2
#define ALIGNED4                        ALIGNED(4)

#define NON_EXP_ALIGN(val, exp)         (((val) + ((exp) - 1)) / (exp) * (exp))

#define AF_DMA_LIST_CNT                 4

#ifndef MIC_SAMPLERATE
#define MIC_SAMPLERATE                  16000//AUD_SAMPRATE_16000
#endif
#ifndef MIC_PERIOD_MS
#define MIC_PERIOD_MS                   64
#endif
#ifndef MIC_BITS
#define MIC_BITS                        16
#endif

#ifndef CHAN_NUM_CAPTURE
#ifdef HW_AUD_LOOPBACK_MODE
#define CHAN_NUM_CAPTURE                4
#else
#define CHAN_NUM_CAPTURE                3//2
#endif /* HW_AUD_LOOPBACK_MODE */
#define CHAN_CAPTURE_MIC1               1
#define CHAN_CAPTURE_MIC2               2
#define CHAN_CAPTURE_REF1               0
#define CHAN_CAPTURE_NULL               -1
#endif

#define CHAN_SEP_BUF_CAPTURE            false//true

#define STREAM_DEVICE                   AUD_STREAM_USE_INT_CODEC

#define TX_BUFF_ALIGN                   (4 * AF_DMA_LIST_CNT)
#define RX_BURST_NUM                    4
#define RX_ALL_CH_DMA_BURST_SIZE        (RX_BURST_NUM * 2 * CHAN_NUM_CAPTURE)
#define RX_BUFF_ALIGN                   (RX_ALL_CH_DMA_BURST_SIZE * AF_DMA_LIST_CNT)

#define PLAYBACK_FRAME_SIZE             ((MIC_SAMPLERATE/1000) * 2 * 1) //16bit, 1ch per ms
#define CAPTURE_FRAME_SIZE              ((MIC_SAMPLERATE/1000) * 2 * CHAN_NUM_CAPTURE) //16bit, CHAN_NUM_CAPTURE per ms

#define PLAYBACK_SIZE                   NON_EXP_ALIGN(PLAYBACK_FRAME_SIZE * MIC_PERIOD_MS, TX_BUFF_ALIGN)
#define _CAPTURE_SIZE                   NON_EXP_ALIGN(CAPTURE_FRAME_SIZE * MIC_PERIOD_MS, RX_BUFF_ALIGN)

#define KFIFO_CAPTURE_SIZE              (128*1024)


#define _MULTI_RECORD_SUPPORT // external algorithm, so just keep recording
/* 1:before m33->a7, 2:a7 received */
#ifdef RTOS
#define _MULTI_RECORD_DUMP_POINT        1
#endif

#if !defined(DSP_RAM_SHARE_SIZE)
#define A7_ALLOC_TRANSQ_BUF
#endif

#ifdef __ARM_ARCH_ISA_ARM
#ifdef RTOS
#define A7_RECORD_DIRECT_CB // mic irq to cb directly
#endif
#endif

typedef enum {
    eANALOG_MIC,
    eDIGTAL_MIC,
} e_MIC_TYPE_T;

extern unsigned char *_a7_dsp_heap_alloc(char *func, int line, int size);
#define a7_dsp_heap_alloc(size) _a7_dsp_heap_alloc((char *)__FUNCTION__, __LINE__, size)

void aud_record_subscribe(aud_dump_cb_t cbf, void *param);
void aud_record_unsubscribe(aud_dump_cb_t cbf, void *param);

void mcu_cmd_send(A7_CMD_T *cmd);

typedef void (*mcu_audiodump_cb_t)(aud_dump_cb_t cbf, void *param);
void mcu_audiodump_register(char *pattern, mcu_audiodump_cb_t register_cb, mcu_audiodump_cb_t unregister_cb, int channels, int buf_KB, int timeout);
void mcu_audiodump_unregister(char *pattern);

void mcu_audio_task_init(void);
void mcu_audio_task_deinit(void);

typedef void (*mcu_audio_common_cb)(void);
typedef void (*mcu_audio_kws_put_data_cb)(uint8_t *buf, uint32_t len, uint8_t ch_num);
void mcu_af_capture_onoff(int onoff);
void mcu_audio_dsp_clock_onoff(uint8_t onoff);
#ifdef APP_SLEEP_ENABLE
void mcu_audio_dsp_clock(uint8_t onoff);
#endif
void mcu_audio_vad_timer_start(int ms);
void mcu_audio_vad_timer_stop(void);
void mcu_audio_vad_init(void *thread_id);
void mcu_audio_vad_loop(void);
void mcu_audio_vad_register(mcu_audio_common_cb pre_cb, mcu_audio_common_cb post_cb);
void mcu_audio_vad_pre_lp(void);
void mcu_audio_vad_post_lp(void);

void mcu_audio_kws_register(mcu_audio_common_cb start_cb, mcu_audio_common_cb stop_cb, mcu_audio_kws_put_data_cb put_data_cb);
void mcu_audio_kws_set_process(uint8_t onoff);
uint8_t mcu_audio_kws_get_process(void);
void mcu_audio_kws_set_wakeup(uint8_t onoff);
uint8_t mcu_audio_kws_wakeup_check(void);
void mcu_audio_kws_start(void);
void mcu_audio_kws_stop(void);
void mcu_audio_kws_signal(void);
void mcu_audio_kws_wait(void);
void mcu_audio_kws_put_data(uint8_t *buf, uint32_t len, uint8_t ch_num);

void mic_set_type(e_MIC_TYPE_T type);
e_MIC_TYPE_T mic_get_type(void);
void mic_set_samplerate(int mic_samplerate);
int mic_get_samplerate();
#define MIC_BITRATE mic_get_samplerate()
void mic_set_period(int mic_period_ms);
int mic_get_period();
#define CAPTURE_PERIOD_MS mic_get_period()
#define CAPTURE_SIZE NON_EXP_ALIGN(((MIC_BITRATE/1000) * 2 * CHAN_NUM_CAPTURE) * CAPTURE_PERIOD_MS, RX_BUFF_ALIGN)
#define RECORD_CHAN_SIZE (CAPTURE_SIZE>>1)

void audio_dump_a7_cb_register(aud_dump_cb_t cbf, void *param);
void audio_dump_a7_cb_unregister(aud_dump_cb_t cbf, void *param);

typedef void (*mcu_record_pre_handler)(uint8_t *buf, uint32_t len);
void mcu_record_pre_handler_set(mcu_record_pre_handler handler);

void a7_to_m33_aud_subscribe(aud_dump_cb_t cbf, void *param);
void a7_to_m33_aud_unsubscribe(aud_dump_cb_t cbf, void *param);
int record_data_get_mic1_mic2(uint16_t *record_in, uint32_t len_in, uint16_t *buf_out);

void a7_to_m33_algo_subscribe(aud_dump_cb_t cbf, void *param);
void a7_to_m33_algo_unsubscribe(aud_dump_cb_t cbf, void *param);

void mcu_audio_task_init_hook(void);

void mcu_dsp_disable(void);
void mcu_dsp_enable(void);

#ifdef __cplusplus
}
#endif

#endif /* _MCU_AUDIO_H_ */
