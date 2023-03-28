#include <string.h>
#include "plat_types.h"
#include "hal_trace.h"
#include "audio_dump.h"
#include "bt_sco_codec.h"
#include "bt_drv.h"
#include "speech_cfg.h"
// Customer Codec Header File
#include "codec_sbc.h"

/**
 * NOTE:
 *  1. This example shows MSBC encoder, decoder and PLC 
 **/ 

#define CODEC_SAMPLE_RATE           (16000)
#define CODEC_PACKET_SIZE           (57)
#define SCO_PACKET_SIZE             (60)
#define CODEC_PCM_FRAME_SIZE        (240)

#define SCO_PACKET_HEADER               (0x01)
#define SCO_PACKET_SEQ_NUM              (4)
#define SCO_PACKET_SEQ_INDEX_INVALID    (0xff)
static const uint8_t g_sco_packet_seq[SCO_PACKET_SEQ_NUM] = {0x08, 0x38, 0xc8, 0xf8};
#define SCO_PACKET_PADDING              (0x00)

typedef struct {
    uint8_t     header;
    uint8_t     seq;
    uint8_t     payload[CODEC_PACKET_SIZE];
    uint8_t     padding;
} sco_xxxx_packet_t;

typedef struct {
    uint32_t    total;
    uint32_t    bt_mute;
    uint32_t    parser;
    uint32_t    seq;
    uint32_t    repair;
} xxxx_plc_histogram_t;

static uint8_t g_encoder_seq_index = 0;
static uint8_t g_decoder_seq_index = SCO_PACKET_SEQ_INDEX_INVALID;

static xxxx_plc_histogram_t g_xxxx_plc_histogram;

// Encoder and Decoder State
static btif_sbc_encoder_t *g_enc_state = NULL;
static btif_sbc_decoder_t *g_dec_state = NULL;
static float dec_eq_band_gain[8]= {0,0,0,0,0,0,0,0};

// PLC State
#include "lpc_plc_api.h"
static LpcPlcState *plc_st = NULL;

static uint32_t get_packet_index(uint8_t seq)
{
    for (uint32_t index = 0; index < SCO_PACKET_SEQ_NUM; index++) {
        if (g_sco_packet_seq[index] == seq) {
            return index;
        }
    }

    return SCO_PACKET_SEQ_INDEX_INVALID;
}

static bool check_controller_mute_pattern(uint8_t *pkt, uint8_t pattern)
{
    // do not check padding byte as it maybe useless when msbc_offset is 1
    for (int i = 0; i < SCO_PACKET_SIZE - 1; i++)
        if (pkt[i] != pattern)
            return false;

    return true;
}    

static void generate_xxxx_plc_histogram(void)
{
    uint32_t packet_loss_num =  g_xxxx_plc_histogram.bt_mute + 
                                g_xxxx_plc_histogram.parser + 
                                g_xxxx_plc_histogram.seq;

    TRACE(0, "------------------------ XXXX PLC Histogram ------------------------");
    TRACE(0, "* BT Mute:  %d", g_xxxx_plc_histogram.bt_mute);
    TRACE(0, "* Parser:   %d", g_xxxx_plc_histogram.parser);
    TRACE(0, "* Seq:      %d", g_xxxx_plc_histogram.seq);
    TRACE(0, "* Repair:   %d", g_xxxx_plc_histogram.repair);
    TRACE(0, "* Packet Loss Percent:    %d/10000(%d/%d)", 
        (int32_t)(10000.f * packet_loss_num / g_xxxx_plc_histogram.total),
        packet_loss_num, g_xxxx_plc_histogram.total);
    TRACE(0, "* Repair Packet Percent:  %d/10000(%d/%d)", 
        (int32_t)(10000.f * g_xxxx_plc_histogram.repair / g_xxxx_plc_histogram.total),
        g_xxxx_plc_histogram.repair, g_xxxx_plc_histogram.total);
    TRACE(0, "-------------------------------- End --------------------------------");
}

int32_t bt_sco_codec_xxxx_init(uint32_t sample_rate)
{
    // ASSERT(0, "[%s] Please implement this function! Then remove this ASSERT!", __func__);
    TRACE(0, "[%s] ...", __func__);

    ASSERT(SCO_PACKET_SIZE == sizeof(sco_xxxx_packet_t),
        "[%s] SCO_PACKET_SIZE != sizeof(sco_xxxx_packet_t)", __func__);

    g_encoder_seq_index = 0;
    g_decoder_seq_index = SCO_PACKET_SEQ_INDEX_INVALID;

    memset(&g_xxxx_plc_histogram, 0, sizeof(g_xxxx_plc_histogram));

    g_enc_state = (btif_sbc_encoder_t *)speech_calloc(1, sizeof(btif_sbc_encoder_t));  // 2k Bytes
    g_dec_state = (btif_sbc_decoder_t *)speech_calloc(1, sizeof(btif_sbc_decoder_t));  // 3k Bytes

    // Init Encoder
    btif_sbc_init_encoder(g_enc_state);
    g_enc_state->streamInfo.mSbcFlag = 1;
    g_enc_state->streamInfo.numChannels = 1;
    g_enc_state->streamInfo.channelMode = BTIF_SBC_CHNL_MODE_MONO;
    g_enc_state->streamInfo.bitPool     = 26;
    g_enc_state->streamInfo.sampleFreq  = BTIF_SBC_CHNL_SAMPLE_FREQ_16;
    g_enc_state->streamInfo.allocMethod = BTIF_SBC_ALLOC_METHOD_LOUDNESS;
    g_enc_state->streamInfo.numBlocks   = BTIF_MSBC_BLOCKS;
    g_enc_state->streamInfo.numSubBands = 8;

    // Init Decoder
    btif_sbc_init_decoder(g_dec_state);

    g_dec_state->streamInfo.mSbcFlag = 1;
    g_dec_state->streamInfo.bitPool = 26;
    g_dec_state->streamInfo.sampleFreq = BTIF_SBC_CHNL_SAMPLE_FREQ_16;
    g_dec_state->streamInfo.channelMode = BTIF_SBC_CHNL_MODE_MONO;
    g_dec_state->streamInfo.allocMethod = BTIF_SBC_ALLOC_METHOD_LOUDNESS;
    g_dec_state->streamInfo.numBlocks = BTIF_MSBC_BLOCKS;
    g_dec_state->streamInfo.numSubBands = 8;
    g_dec_state->streamInfo.numChannels = 1;

    for (uint8_t i=0; i<sizeof(dec_eq_band_gain)/sizeof(float); i++) {
        dec_eq_band_gain[i] = 1.0;
    }

    // Init PLC
    plc_st = lpc_plc_create(sample_rate);

    return 0;
}

int32_t bt_sco_codec_xxxx_deinit(void)
{
    // ASSERT(0, "[%s] Please implement this function! Then remove this ASSERT!", __func__);
    TRACE(0, "[%s] ...", __func__);

    generate_xxxx_plc_histogram();

    // Deinit PLC
    lpc_plc_destroy(plc_st);

    // Free Encoder and Decoder State
    speech_free(g_enc_state);
    speech_free(g_dec_state);

    return 0;
}

// xxxx_size = 120, pcm_size = 480
int32_t bt_sco_codec_xxxx_encoder(uint8_t *xxxx_buf, uint32_t xxxx_size, uint8_t *pcm_buf, uint32_t pcm_size)
{
    // ASSERT(0, "[%s] Please implement this function! Then remove this ASSERT!", __func__);

    ASSERT(xxxx_size / SCO_PACKET_SIZE == pcm_size / CODEC_PCM_FRAME_SIZE,
        "[%s] xxxx_size = %d, pcm_size = %d", __func__, xxxx_size, pcm_size);

    uint16_t bytes_encoded = 0;
    uint16_t msbc_packet_size = CODEC_PACKET_SIZE;
    btif_sbc_pcm_data_t enc_pcm_data;

    uint32_t loop_cnt = xxxx_size / SCO_PACKET_SIZE;
    sco_xxxx_packet_t *sco_packet_ptr = (sco_xxxx_packet_t *)xxxx_buf;

    // TRACE(0, "[%s] xxxx_size = %d, pcm_size = %d", __func__, xxxx_size, pcm_size);

    for (uint32_t i=0; i<loop_cnt; i++) {
        sco_packet_ptr->header  = SCO_PACKET_HEADER;
        sco_packet_ptr->padding = SCO_PACKET_PADDING;
        sco_packet_ptr->seq     = g_sco_packet_seq[g_encoder_seq_index];

        // Encoder
        enc_pcm_data.data = pcm_buf;
        enc_pcm_data.dataLen = CODEC_PCM_FRAME_SIZE;

        btif_sbc_encode_frames(g_enc_state, &enc_pcm_data, &bytes_encoded, sco_packet_ptr->payload, (uint16_t *)&msbc_packet_size, 0xFFFF);

        sco_packet_ptr++;
        pcm_buf += CODEC_PCM_FRAME_SIZE;
        g_encoder_seq_index = (g_encoder_seq_index + 1) % SCO_PACKET_SEQ_NUM;
    }

    return 0;
}

// xxxx_size = 120, pcm_size = 480
int32_t bt_sco_codec_xxxx_decoder(uint8_t *xxxx_buf, uint32_t xxxx_size, uint8_t *pcm_buf, uint32_t pcm_size)
{
    // ASSERT(0, "[%s] Please implement this function! Then remove this ASSERT!", __func__);

    ASSERT(xxxx_size / SCO_PACKET_SIZE == pcm_size / CODEC_PCM_FRAME_SIZE,
        "[%s] xxxx_size = %d, pcm_size = %d", __func__, xxxx_size, pcm_size);

    btif_sbc_pcm_data_t dec_pcm_data;
    uint16_t byte_decode = 0;

    int16_t plc_flag = false;
    uint32_t loop_cnt = xxxx_size / SCO_PACKET_SIZE;
    sco_xxxx_packet_t *sco_packet_ptr = (sco_xxxx_packet_t *)xxxx_buf;

#if 0
    TRACE(0, "[%s] xxxx_size = %d, pcm_size = %d", __func__, xxxx_size, pcm_size);
    TRACE(0, "Decoder: 0x%2x, 0x%2x, 0x%2x, 0x%2x", xxxx_buf[0], 
                                                    xxxx_buf[1], 
                                                    xxxx_buf[2], 
                                                    xxxx_buf[3]);
#endif

    for (uint32_t i=0; i<loop_cnt; i++) {
        // TRACE(0, "[%s] index: %d; seq: 0x%2x, 0x%2x", __func__,
        //     g_decoder_seq_index,
        //     g_sco_packet_seq[g_decoder_seq_index],
        //     sco_packet_ptr->seq);

        // Check PLC Status
        if (check_controller_mute_pattern((uint8_t *)sco_packet_ptr, MSBC_MUTE_PATTERN)) { // Check PLC
            TRACE(0, "[%s] Decoder PLC: Mute pattern!", __func__);
            g_xxxx_plc_histogram.bt_mute++;
            plc_flag = true;
        } else if ( (sco_packet_ptr->header != SCO_PACKET_HEADER) || 
                    (sco_packet_ptr->padding != SCO_PACKET_PADDING)) { // Check header and padding
            TRACE(0, "[%s] Decoder PLC: Parser packet error: 0x%x, 0x%x!", __func__, sco_packet_ptr->header, sco_packet_ptr->padding);
            g_xxxx_plc_histogram.parser++;
            plc_flag = true;
        } else { // Check index
            uint32_t index = get_packet_index(sco_packet_ptr->seq);
            if (g_decoder_seq_index == SCO_PACKET_SEQ_INDEX_INVALID) { // First frame
                if (index == SCO_PACKET_SEQ_INDEX_INVALID) { // Can not match valid seq
                    TRACE(0, "[%s] Decoder PLC: First seq error!", __func__);
                    g_xxxx_plc_histogram.seq++;
                    plc_flag = true;
                } else {
                    TRACE(0, "[%s] Decoder: First seq OK!", __func__);
                    g_decoder_seq_index = index;
                    plc_flag = false;
                }
            } else if (g_decoder_seq_index != index) {
                TRACE(0, "[%s] Decoder PLC seq error!", __func__);
                g_xxxx_plc_histogram.seq++;
                if (index < SCO_PACKET_SEQ_NUM) {
                    TRACE(0, "[%s] Decoder PLC: Fix seq!", __func__);
                    g_decoder_seq_index = index;
                }
                plc_flag = true;
            } else {
                plc_flag = false;
            }
        }

        if (plc_flag == false) {
            dec_pcm_data.sampleFreq = BTIF_SBC_CHNL_SAMPLE_FREQ_16;
            dec_pcm_data.numChannels = 1;
            dec_pcm_data.dataLen = 0;
            dec_pcm_data.data = pcm_buf;
            bt_status_t ret = btif_sbc_decode_frames(g_dec_state,
                    sco_packet_ptr->payload,
                    SCO_PACKET_SIZE, &byte_decode,
                    &dec_pcm_data, CODEC_PCM_FRAME_SIZE,
                    dec_eq_band_gain);

            if (ret != BT_STS_SUCCESS) {
                TRACE(0, "[%s] ERROR: Decoder failed!", __func__);
            }
            // PLC: Save good data
            lpc_plc_save(plc_st, (int16_t *)pcm_buf);
        } else {
            // PLC: Generate data
            lpc_plc_generate(plc_st, (int16_t *)pcm_buf, NULL);
        }


        g_xxxx_plc_histogram.total++;

        if (g_decoder_seq_index != SCO_PACKET_SEQ_INDEX_INVALID) {
            g_decoder_seq_index = (g_decoder_seq_index + 1) % SCO_PACKET_SEQ_NUM;
        }
        
        sco_packet_ptr++;
        pcm_buf += CODEC_PCM_FRAME_SIZE;
    }

    return 0;
}

bt_sco_codec_t bt_sco_codec_xxxx = {
    .init       = bt_sco_codec_xxxx_init,
    .deinit     = bt_sco_codec_xxxx_deinit,
    .encoder    = bt_sco_codec_xxxx_encoder,
    .decoder    = bt_sco_codec_xxxx_decoder
};