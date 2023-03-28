#ifndef __BAP_LC3_H__
#define __BAP_LC3_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int32_t        bitrates;
    uint32_t       maxSdu;
    uint32_t       sduInterval;
    uint32_t       sendFrameNum;
    uint32_t       audioPlayInterval;
    uint32_t       sinkTransDelay;
} bis_config_para_t;

int bap_lc3_encoder_init(void);
int bap_lc3_decoder_init(void);
int bap_lc3_encode(void * input, int inlen, void * output, int *outlen);
int bap_lc3_decode(void * input, int inlen, void * output, int *outlen);
int bap_lc3_config(void * input, int len);

#ifdef __cplusplus
}
#endif
#endif /* __BAP_LC3_H__ */

