#ifndef __CODEC_TMSG_H__
#define __CODEC_TMSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DSP_CODEC_RSP_OK  0
#define DSP_CODEC_RSP_FAIL 1

typedef enum
{
    DSP_CODEC_CMD_LC3_CFG = 0,
    DSP_CODEC_CMD_LC3_ENC,
    DSP_CODEC_CMD_LC3_DEC,
    DSP_CODEC_CMD_OPUS_CFG,
    DSP_CODEC_CMD_OPUS_ENC,
    DSP_CODEC_CMD_OPUS_ENC1,
    DSP_CODEC_CMD_OPUS_ENC2,
    DSP_CODEC_CMD_OPUS_ENC3,
    DSP_CODEC_CMD_OPUS_DEC,
    DSP_CODEC_CMD_OPUS_AUDIO_DROP,
    DSP_CODEC_CMD_MAX
} DSP_CODEC_CMD_T;

typedef struct
{
    uint8_t cmd;
    void *data;
    int len;
}DSP_CODEC_MSG_T;

int dsp_codec_process(DSP_CODEC_MSG_T* msg, uint8_t **outbuf, uint32_t *outlen);
int dsp_codec_init(void);

/* mcu api */
void mcu_codec_open(void);
int mcu_codec_request(DSP_CODEC_CMD_T cmd, uint8_t * in, uint32_t len, uint8_t *out, uint32_t *out_len);

/* a7 api */
void a7_codec_open(void);

#ifdef __cplusplus
}
#endif
#endif /* __CODEC_TMSG_H__ */

