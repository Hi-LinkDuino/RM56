#ifndef __CODEC_TMSG_H__
#define __CODEC_TMSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define PCM_PLAY_RSP_OK  0
#define PCM_PLAY_RSP_FAIL 1

typedef enum
{
    PCM_PLAY_START = 1,
    PCM_PLAY_ING,
    PCM_PLAY_STOP,
    PCM_PLAY_CMD_MAX
} PCM_PLAY_CMD_T;

typedef struct
{
    uint8_t cmd;
    void *data;
    int len;
}PCM_PLAY_MSG_T;

int pcm_play_init(void);

/* mcu api */
void pcm_play_open(void);
void pcm_play_start(void);
void pcm_play_process(uint8_t *inbuf, uint32_t inlen);
void pcm_play_stop(void);


#ifdef __cplusplus
}
#endif
#endif /* __CODEC_TMSG_H__ */

