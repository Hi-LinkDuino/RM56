

#ifndef __LC3_API_PROCESS_H__
#define __LC3_API_PROCESS_H__
#include <stdint.h>

#ifndef EXTERNC
#ifdef __cplusplus
#define EXTERNC extern "C" 
#else
#define EXTERNC
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*! Decoder packet loss concealment mode */
typedef enum
{
    LC3_API_PLC_STANDARD = 0, /*!< Less complex than advanced method */
    LC3_API_PLC_ADVANCED = 1  /*!< Enhanced concealment method */
} LC3_API_PlcMode;

/*! Error protection mode. LC3_EP_ZERO differs to LC3_EP_OFF in that
 *  errors can be detected but not corrected. */
typedef enum
{
    LC3_API_EP_OFF = 0, /*!< Error protection is disabled */
    LC3_API_EP_ZERO = 1, /*!< Error protection with 0 bit correction */
    LC3_API_EP_LOW = 2, /*!< Error protection correcting one symbol per codeword */
    LC3_API_EP_MEDIUM = 3, /*!< Error protection correcting two symbols per codeword */
    LC3_API_EP_HIGH = 4  /*!< Error protection correcting three symbols per codeword */
} LC3_API_EpMode;

/*! Error protection mode request. On the encoder sidem, LC3_EPMR_ZERO to LC3_EPMR_HIGH
 *  can be set. The decoder returns mode requests with different confidences. */
typedef enum
{
    LC3_API_EPMR_ZERO = 0,  /*!< Request no error correction. High confidence if returned by decoder. */
    LC3_API_EPMR_LOW = 1,  /*!< Request low error correction. High confidence if returned by decoder. */
    LC3_API_EPMR_MEDIUM = 2,  /*!< Request medium error correction. High confidence if returned by decoder. */
    LC3_API_EPMR_HIGH = 3,  /*!< Request high error correction. High confidence if returned by decoder. */
    LC3_API_EPMR_ZERO_MC = 4,  /*!< No error correction requested, medium confidence. */
    LC3_API_EPMR_LOW_MC = 5,  /*!< Low error correction requested, medium confidence. */
    LC3_API_EPMR_MEDIUM_MC = 6,  /*!< Medium error correction requested, medium confidence. */
    LC3_API_EPMR_HIGH_MC = 7,  /*!< High error correction requested, medium confidence. */
    LC3_API_EPMR_ZERO_NC = 8,  /*!< No error correction requested, unvalidated. */
    LC3_API_EPMR_LOW_NC = 9,  /*!< Low error correction requested, unvalidated. */
    LC3_API_EPMR_MEDIUM_NC = 10, /*!< Medium error correction requested, unvalidated. */
    LC3_API_EPMR_HIGH_NC = 11  /*!< High error correction requested, unvalidated. */
} LC3_API_EpModeRequest;

typedef enum
{
    LC3_API_OK                  = 0,  /*!< No error occurred */
    LC3_API_ERROR               = -1,  /*!< Function call failed */
} LC3_API_Error;

typedef struct LC3_Enc_Info
{
    void* encode;
    int encode_size;
    void* scratch;
    int scratch_size;
    int32_t samplerate;
    int32_t channels;
    int32_t bitwidth;
    float frame_ms;
    LC3_API_EpMode epmode;
    LC3_API_PlcMode plcMeth;
    int32_t bitrate;
    int32_t delay;
    uint32_t  nSamples;
    int32_t real_bitrate;
    int32_t dc;
    int32_t bytes_per_channel;
}LC3_Enc_Info;

typedef struct LC3_Dec_Info
{
    void* decode;
    int decode_size;
    void* scratch;
    int scratch_size;
    int32_t samplerate;
    int32_t channels;
    int32_t bitwidth;
    int32_t bitrate;
    float frame_ms;
    LC3_API_EpMode epmode;
    LC3_API_PlcMode plcMeth;
    int32_t delay;
    uint32_t  nSamples;
    int32_t dc;
}LC3_Dec_Info;

int32_t lc3_api_encoder_init(LC3_Enc_Info* lc3_enc_info);

int32_t lc3_api_enc16(void *encoder, void *scratch, int16_t **input_samples, void *output_bytes, int32_t *num_bytes);

int32_t lc3_api_enc24(void *encoder, void *scratch, int32_t **input_samples, void *output_bytes, int32_t *num_bytes);

int32_t lc3_api_enc_get_size(int32_t samplerate, int32_t channels, int bitsdepth);

int32_t lc3_api_enc_get_scratch_size(const void *encoder);
//

int32_t lc3_api_decoder_init(struct LC3_Dec_Info* lc3_dec_info);

int32_t lc3_api_dec16(void *decoder, void *scratch, void *input_bytes, int32_t num_bytes, int16_t **output_samples, int32_t bfi_ext);

int32_t lc3_api_dec24(void *decoder, void *scratch, void *input_bytes, int32_t num_bytes, int32_t **output_samples, int32_t bfi_ext);

int32_t lc3_api_dec_get_size(int32_t samplerate, int32_t channels, int plc_mode);

int32_t lc3_api_dec_get_scratch_size(const void *decoder);

//
int32_t lc3_api_enc_get_size_max();
int32_t lc3_api_enc_get_scratch_size_max();
int32_t lc3_api_dec_get_size_max();
int32_t lc3_api_dec_get_scratch_size_max();
int32_t lc3_api_get_max_bytes();
int32_t lc3_api_get_max_samples();


#ifdef __cplusplus
}
#endif
/*! \} */
#endif

