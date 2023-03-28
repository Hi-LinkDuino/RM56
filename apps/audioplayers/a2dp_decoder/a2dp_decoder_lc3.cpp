/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
// Standard C Included Files
#include "cmsis.h"
#include "plat_types.h"
#include "string_s.h"
#include "heap_api.h"
#include "hal_location.h"
#include "a2dp_decoder_internal.h"
#include "lc3_process.h"
#include "btapp.h"

#ifdef LC3_IN_A7_DSP_CODEC
#include "dsp_codec.h"
#endif

#define LC3_READBUF_SIZE (800)
#define LC3_MTU_LIMITER (30)
#define LC3_OUTPUT_FRAME_SAMPLES (480)
#define A2DP_LC3_OUTPUT_FRAME_SAMPLES   (480)

static A2DP_AUDIO_CONTEXT_T *a2dp_audio_context_p = NULL;
static A2DP_AUDIO_DECODER_LASTFRAME_INFO_T a2dp_audio_lc3_lastframe_info;
static A2DP_AUDIO_OUTPUT_CONFIG_T a2dp_audio_lc3_output_config;

typedef struct {
    uint16_t sequenceNumber;
    uint32_t timestamp;
    uint16_t curSubSequenceNumber;
    uint16_t totalSubSequenceNumber;
    uint8_t *buffer;
    uint32_t buffer_len;
} a2dp_audio_lc3_decoder_frame_t;

LC3_Dec_Info* lc3_dec_info = NULL;
short* lc3_noninterleave_pcm_buf_left = NULL;
short* lc3_noninterleave_pcm_buf_right = NULL;
uint32_t lc3_pcm_frame_len = 0;

int a2dp_audio_lc3_decoder_init(uint32_t sampleRate, uint8_t channelNum, uint8_t bits)
{
    int sRet = 0;
    LC3_API_PlcMode plcMeth = LC3_API_PLC_ADVANCED;
    //float frame_ms = ((float)bt_get_lc3_frame_dms())/10;
    float frame_ms = ((float)100)/10;	
    //int delay = 0;
    //int dc = 1;
    //uint32_t nSamples =0;
    uint32_t epmode = 0;
    uint32_t temp = 0;
    //LC3_Error err     = LC3_OK;

    TRACE(3, "lc3_dec_init sample Rate=%d, channel_mode = %d, bits = %d\n", sampleRate, channelNum, bits);

#ifndef LC3_IN_A7_DSP_CODEC
    lc3_dec_info = (LC3_Dec_Info*)a2dp_audio_heap_malloc(sizeof(LC3_Dec_Info));
    if(lc3_dec_info == NULL)
        return -1;
    sRet = memset_s((void*)lc3_dec_info, sizeof(LC3_Dec_Info), 0x0, sizeof(LC3_Dec_Info));
    if(sRet != 0)
    {
        TRACE(1,"func-s line:%d sRet:%d %s ", __LINE__, sRet, __func__);
    }
    lc3_dec_info->samplerate = sampleRate;
    lc3_dec_info->channels = channelNum;
    lc3_dec_info->bitwidth = bits;
    lc3_dec_info->frame_ms = frame_ms;
    lc3_dec_info->plcMeth = plcMeth;
    lc3_dec_info->epmode= (LC3_API_EpMode)epmode;
    lc3_dec_info->dc = 1;
    lc3_dec_info->decode_size = lc3_api_dec_get_size(lc3_dec_info->samplerate, lc3_dec_info->channels, lc3_dec_info->plcMeth);       
    lc3_dec_info->decode = (void*)a2dp_audio_heap_malloc(lc3_dec_info->decode_size);
    lc3_api_decoder_init(lc3_dec_info);
    lc3_dec_info->scratch_size = lc3_api_dec_get_scratch_size(lc3_dec_info->decode);
    lc3_dec_info->scratch = (void*)a2dp_audio_heap_malloc(lc3_dec_info->scratch_size);
#else
    mcu_codec_request(DSP_CODEC_CMD_LC3_CFG, &bits, sizeof(uint32_t) , NULL, NULL);
#endif

    if(sampleRate == 44100)
        temp = 48000;
    else
        temp = sampleRate;
    //lc3_pcm_frame_len = temp*bt_get_lc3_frame_dms()*channelNum*bits/8/1000/10;
    lc3_pcm_frame_len = temp*100*channelNum*bits/8/1000/10;
    //lc3_pcm_frame_len = temp*100*channelNum*bits/8/1000/10;

    lc3_noninterleave_pcm_buf_left = (short*)a2dp_audio_heap_malloc(lc3_pcm_frame_len/channelNum);
    if(lc3_noninterleave_pcm_buf_left == NULL)
        return -1;

    lc3_noninterleave_pcm_buf_right = (short*)a2dp_audio_heap_malloc(lc3_pcm_frame_len/channelNum);
    if(lc3_noninterleave_pcm_buf_right == NULL)
        return -1;
    sRet = memset_s((void*)(lc3_noninterleave_pcm_buf_left), lc3_pcm_frame_len/channelNum, 0x0, lc3_pcm_frame_len/channelNum);
    if(sRet != 0)
    {
        TRACE(1,"func-s line:%d sRet:%d %s ", __LINE__, sRet, __func__);
    }
    sRet = memset_s((void*)(lc3_noninterleave_pcm_buf_right), lc3_pcm_frame_len/channelNum, 0x0, lc3_pcm_frame_len/channelNum);
    if(sRet != 0)
    {
        TRACE(1,"func-s line:%d sRet:%d %s ", __LINE__, sRet, __func__);
    }
    /* Print info */
    TRACE(1, "Decoder size:     %i", lc3_dec_info->decode_size);
    TRACE(1, "Scratch size:     %i", lc3_dec_info->scratch_size);
    TRACE(1, "Sample rate:      %i", sampleRate);
    TRACE(1, "Channels:         %i", channelNum);
    TRACE(1, "Frame length:     %i", lc3_dec_info->nSamples);
    TRACE(1, "PLC mode:         %i", plcMeth);

    return 0;
}

static void a2dp_audio_lc3_decoder_deinit(void)
{
    if(lc3_dec_info == NULL)
        return ;

    if (lc3_dec_info->decode) {
        a2dp_audio_heap_free(lc3_dec_info->decode);
        lc3_dec_info->decode = NULL;
    }
    if (lc3_dec_info->scratch) {
        a2dp_audio_heap_free(lc3_dec_info->scratch);
        lc3_dec_info->scratch = NULL;
    }
    if (lc3_dec_info) {
        a2dp_audio_heap_free(lc3_dec_info);
        lc3_dec_info = NULL;
    }
    if (lc3_noninterleave_pcm_buf_left) {
        a2dp_audio_heap_free(lc3_noninterleave_pcm_buf_left);
        lc3_noninterleave_pcm_buf_left = NULL;
    }
    if (lc3_noninterleave_pcm_buf_right) {
        a2dp_audio_heap_free(lc3_noninterleave_pcm_buf_right);
        lc3_noninterleave_pcm_buf_right = NULL;
    }

	
}

static void *a2dp_audio_lc3_frame_malloc(uint32_t packet_len)
{
    a2dp_audio_lc3_decoder_frame_t *decoder_frame_p = NULL;
    uint8_t *buffer = NULL;

    buffer = (uint8_t *)a2dp_audio_heap_malloc(packet_len);
    decoder_frame_p = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_heap_malloc(sizeof(a2dp_audio_lc3_decoder_frame_t));
    decoder_frame_p->buffer = buffer;
    decoder_frame_p->buffer_len = packet_len;
    return (void *)decoder_frame_p;
}


static int a2dp_audio_lc3_list_checker(void)
{
    list_t *list = a2dp_audio_context_p->audio_datapath.input_raw_packet_list;
    if(list == NULL)
        return -1;
    list_node_t *node = NULL;
    a2dp_audio_lc3_decoder_frame_t *lc3_decoder_frame_p = NULL;
    int cnt = 0;

    do {
        lc3_decoder_frame_p = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_lc3_frame_malloc(LC3_READBUF_SIZE);
        if (lc3_decoder_frame_p){
            a2dp_audio_list_append(list, lc3_decoder_frame_p);
        }
        cnt++;
    }while(lc3_decoder_frame_p && cnt < LC3_MTU_LIMITER);

    do {
        node = a2dp_audio_list_begin(list);
        if (node){
            lc3_decoder_frame_p = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_list_node(node);
            a2dp_audio_list_remove(list, lc3_decoder_frame_p);
        }
    }while(node);

    TRACE(3,"%s cnt:%d list:%d", __func__, cnt, a2dp_audio_list_length(list));

    return 0;
}

//int16_t lc3_noninterleave_buf[960] = {0};

static int a2dp_audio_lc3_mcu_decode_frame(uint8_t * buffer, uint32_t buffer_bytes)
{
    list_t *list = a2dp_audio_context_p->audio_datapath.input_raw_packet_list;
    if(list == NULL)
        return -1;
    list_node_t *node = NULL;
    a2dp_audio_lc3_decoder_frame_t* lc3_decoder_frame_p = NULL;
    int16_t *output16[] = {lc3_noninterleave_pcm_buf_left, lc3_noninterleave_pcm_buf_right};
    //int16_t *output16[] = {lc3_noninterleave_buf, lc3_noninterleave_buf + 480};
	
    int16_t *pcm_short_buf = (int16_t*)buffer;
    bool cache_underflow = false;
    int output_byte = 0;
    int err     = LC3_API_OK;
    int16_t channelnum = 2;

    node = a2dp_audio_list_begin(list);    
    if (!node){
        TRACE(0,"lc3_decode cache underflow");
        cache_underflow = true;
        goto exit;
    }else{
        lc3_decoder_frame_p = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_list_node(node);
        do {
#ifndef LC3_IN_A7_DSP_CODEC
            //memset((void*)(lc3_decoder_frame_p->buffer), 0x0, lc3_decoder_frame_p->buffer_len);			
            err = lc3_api_dec16(lc3_dec_info->decode, lc3_dec_info->scratch, lc3_decoder_frame_p->buffer, lc3_decoder_frame_p->buffer_len, output16, 0);
#else
            uint32_t output_len;
            err = (LC3_API_Error) mcu_codec_request(DSP_CODEC_CMD_LC3_DEC, lc3_decoder_frame_p->buffer, lc3_decoder_frame_p->buffer_len, (uint8_t *)output16[0], &output_len);
#endif
            //TRACE(3,"%s, %d, %d", __func__, err, output_byte);	
            if (err == LC3_API_OK){
                for(uint32_t i = 0; i < lc3_pcm_frame_len/2/channelnum; i++)
                {
                     pcm_short_buf[2*i] = lc3_noninterleave_pcm_buf_left[i];
                     pcm_short_buf[2*i +1] = lc3_noninterleave_pcm_buf_right[i];
                }

                output_byte += lc3_pcm_frame_len;
            }else{
                TRACE(1, "lc3 decode err %d", err);
                break;
            }

            a2dp_audio_lc3_lastframe_info.sequenceNumber = lc3_decoder_frame_p->sequenceNumber;
            a2dp_audio_lc3_lastframe_info.timestamp = lc3_decoder_frame_p->timestamp;
            a2dp_audio_lc3_lastframe_info.curSubSequenceNumber = lc3_decoder_frame_p->curSubSequenceNumber;
            a2dp_audio_lc3_lastframe_info.totalSubSequenceNumber = lc3_decoder_frame_p->totalSubSequenceNumber;
            a2dp_audio_lc3_lastframe_info.frame_samples = A2DP_LC3_OUTPUT_FRAME_SAMPLES;
            a2dp_audio_lc3_lastframe_info.decoded_frames++;
            a2dp_audio_lc3_lastframe_info.undecode_frames = a2dp_audio_list_length(list)-1;
            a2dp_audio_decoder_internal_lastframe_info_set(&a2dp_audio_lc3_lastframe_info);  
            a2dp_audio_list_remove(list, lc3_decoder_frame_p);
        }while(output_byte < (int)buffer_bytes);
    }
exit:
    if(cache_underflow){
        a2dp_audio_lc3_lastframe_info.undecode_frames = 0;
        a2dp_audio_decoder_internal_lastframe_info_set(&a2dp_audio_lc3_lastframe_info);        
        output_byte = A2DP_DECODER_CACHE_UNDERFLOW_ERROR;
    }
    return output_byte;
}

int a2dp_audio_lc3_decode_frame(uint8_t *buffer, uint32_t buffer_bytes)
{
    return a2dp_audio_lc3_mcu_decode_frame(buffer, buffer_bytes);
}

int a2dp_audio_lc3_preparse_packet(btif_media_header_t * header, uint8_t *buffer, uint32_t buffer_bytes)
{
    if (header == NULL) {
        return A2DP_DECODER_DECODE_ERROR;
    }
    a2dp_audio_lc3_lastframe_info.sequenceNumber = header->sequenceNumber;
    a2dp_audio_lc3_lastframe_info.timestamp = header->timestamp;
    a2dp_audio_lc3_lastframe_info.curSubSequenceNumber = 0;
    a2dp_audio_lc3_lastframe_info.totalSubSequenceNumber = 0;
    a2dp_audio_lc3_lastframe_info.frame_samples = A2DP_LC3_OUTPUT_FRAME_SAMPLES;
    a2dp_audio_lc3_lastframe_info.list_samples = A2DP_LC3_OUTPUT_FRAME_SAMPLES;
    a2dp_audio_lc3_lastframe_info.decoded_frames = 0;
    a2dp_audio_lc3_lastframe_info.undecode_frames = 0;
    a2dp_audio_decoder_internal_lastframe_info_set(&a2dp_audio_lc3_lastframe_info);

    TRACE(3,"%s seq:%d timestamp:%08x", __func__, header->sequenceNumber, header->timestamp);

    return A2DP_DECODER_NO_ERROR;
}

void a2dp_audio_lc3_free(void *packet)
{
    if (packet == NULL) {
        return;
    }
    a2dp_audio_lc3_decoder_frame_t *decoder_frame_p = (a2dp_audio_lc3_decoder_frame_t *)packet; 
    a2dp_audio_heap_free(decoder_frame_p->buffer);
    a2dp_audio_heap_free(decoder_frame_p);
}

int a2dp_audio_lc3_store_packet(btif_media_header_t * header, uint8_t *buffer, uint32_t buffer_bytes)
{
    int sRet = 0;
    list_t *list = a2dp_audio_context_p->audio_datapath.input_raw_packet_list;
    if(list == NULL)
        return -1;
    int nRet = A2DP_DECODER_NO_ERROR;
    uint16_t frame_len = 0;

    frame_len = (buffer[0]) | (buffer[1] << 8);
    //TRACE(2,"%s, frame_len:%d %d, %d", __func__, frame_len, buffer_bytes, a2dp_audio_list_length(list));
    if (a2dp_audio_list_length(list) < LC3_MTU_LIMITER) {	
        a2dp_audio_lc3_decoder_frame_t *decoder_frame_p = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_lc3_frame_malloc(frame_len);
        //TRACE(3,"%s seq:%d len:%d", __func__, header->sequenceNumber, frame_len);
        decoder_frame_p->sequenceNumber = header->sequenceNumber;
        decoder_frame_p->timestamp = header->timestamp;
        sRet = memcpy_s(decoder_frame_p->buffer, frame_len, buffer+2, frame_len);
        if(sRet != 0)
        {
            TRACE(1,"func-s line:%d sRet:%d %s ", __LINE__, sRet, __func__);
        }
        decoder_frame_p->buffer_len = frame_len;
        a2dp_audio_list_append(list, decoder_frame_p);       
        return A2DP_DECODER_NO_ERROR;
    }else{
        TRACE(3,"%s list full current list_len:%d buff_len:%d", __func__, a2dp_audio_list_length(list), frame_len);
        nRet = A2DP_DECODER_MTU_LIMTER_ERROR;
    }

    return nRet;
}

int a2dp_audio_lc3_discards_packet(uint32_t packets)
{
    int nRet = A2DP_DECODER_MEMORY_ERROR;
    list_t *list = a2dp_audio_context_p->audio_datapath.input_raw_packet_list;
    if(list == NULL)
        return -1;
    list_node_t *node = NULL;
    a2dp_audio_lc3_decoder_frame_t* lc3_decoder_frame_p = NULL;

    if (packets <= a2dp_audio_list_length(list))
    {
        for (uint8_t i=0; i<packets; i++)
        {
            node = a2dp_audio_list_begin(list);
            lc3_decoder_frame_p = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_list_node(node);
            a2dp_audio_list_remove(list, lc3_decoder_frame_p);
        }
        nRet = A2DP_DECODER_NO_ERROR;
    }
    TRACE(3,"%s packets:%d nRet:%d", __func__, packets, nRet);
    return nRet;

}

int a2dp_audio_lc3_headframe_info_get(A2DP_AUDIO_HEADFRAME_INFO_T* headframe_info)
{
    int sRet = 0;
    list_t *list = a2dp_audio_context_p->audio_datapath.input_raw_packet_list;
    if(list == NULL)
        return -1;
    list_node_t *node = NULL;
    a2dp_audio_lc3_decoder_frame_t *decoder_frame_p = NULL;
    if (headframe_info == NULL) {
        return A2DP_DECODER_DECODE_ERROR;
    }

    if (a2dp_audio_list_length(list)){
        node = a2dp_audio_list_begin(list);                
        decoder_frame_p = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_list_node(node);
        headframe_info->sequenceNumber = decoder_frame_p->sequenceNumber;
        headframe_info->timestamp = decoder_frame_p->timestamp;
        headframe_info->curSubSequenceNumber = 0;
        headframe_info->totalSubSequenceNumber = 0;
    }else{
        sRet = memset_s(headframe_info, sizeof(A2DP_AUDIO_HEADFRAME_INFO_T), 0, sizeof(A2DP_AUDIO_HEADFRAME_INFO_T));
        if(sRet != 0)
        {
            TRACE(1,"func-s line:%d sRet:%d %s ", __LINE__, sRet, __func__);
        }
    }

    return A2DP_DECODER_NO_ERROR;
}

int a2dp_audio_lc3_info_get(void *info)
{
    return A2DP_DECODER_NO_ERROR;
}

int a2dp_audio_lc3_init(A2DP_AUDIO_OUTPUT_CONFIG_T *config, void *context)
{
    int sRet = 0;

    if (config == NULL) {
        return A2DP_DECODER_DECODE_ERROR;
    } 
    TRACE(4,"%s ch:%d freq:%d bits:%d", __func__, config->num_channels, config->sample_rate, config->bits_depth);

    a2dp_audio_context_p = (A2DP_AUDIO_CONTEXT_T *)context;

    sRet = memset_s(&a2dp_audio_lc3_lastframe_info, sizeof(A2DP_AUDIO_DECODER_LASTFRAME_INFO_T), 0, sizeof(A2DP_AUDIO_DECODER_LASTFRAME_INFO_T));
    if(sRet != 0)
    {
        TRACE(1,"func-s line:%d sRet:%d %s ", __LINE__, sRet, __func__);
    }
    
    sRet = memcpy_s(&a2dp_audio_lc3_output_config, sizeof(A2DP_AUDIO_OUTPUT_CONFIG_T), config, sizeof(A2DP_AUDIO_OUTPUT_CONFIG_T));
    if(sRet != 0)
    {
        TRACE(1,"func-s line:%d sRet:%d %s ", __LINE__, sRet, __func__);
    }
    a2dp_audio_lc3_lastframe_info.stream_info = a2dp_audio_lc3_output_config;
    a2dp_audio_lc3_lastframe_info.frame_samples = LC3_OUTPUT_FRAME_SAMPLES;
    a2dp_audio_lc3_lastframe_info.list_samples = LC3_OUTPUT_FRAME_SAMPLES;
    a2dp_audio_decoder_internal_lastframe_info_set(&a2dp_audio_lc3_lastframe_info);

    a2dp_audio_lc3_decoder_init(config->sample_rate, config->num_channels, config->bits_depth);

    a2dp_audio_lc3_list_checker();

    return A2DP_DECODER_NO_ERROR;
}

int a2dp_audio_lc3_deinit(void)
{

    a2dp_audio_lc3_decoder_deinit();

    return A2DP_DECODER_NO_ERROR;
}

int  a2dp_audio_lc3_synchronize_packet(A2DP_AUDIO_SYNCFRAME_INFO_T *sync_info, uint32_t mask)
{
    int nRet = A2DP_DECODER_SYNC_ERROR;
    list_t *list = a2dp_audio_context_p->audio_datapath.input_raw_packet_list;
    if(list == NULL)
        return -1;
    list_node_t *node = NULL;
    int list_len;
    a2dp_audio_lc3_decoder_frame_t* lc3_decoder_frame = NULL;

    list_len = a2dp_audio_list_length(list);

    for (uint16_t i=0; i<list_len; i++){
        node = a2dp_audio_list_begin(list);
        lc3_decoder_frame = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_list_node(node);
        if  (A2DP_AUDIO_SYNCFRAME_CHK(lc3_decoder_frame->sequenceNumber         == sync_info->sequenceNumber,        A2DP_AUDIO_SYNCFRAME_MASK_SEQ,        mask)&&
             A2DP_AUDIO_SYNCFRAME_CHK(lc3_decoder_frame->curSubSequenceNumber   == sync_info->curSubSequenceNumber,  A2DP_AUDIO_SYNCFRAME_MASK_CURRSUBSEQ, mask)&&
             A2DP_AUDIO_SYNCFRAME_CHK(lc3_decoder_frame->totalSubSequenceNumber == sync_info->totalSubSequenceNumber,A2DP_AUDIO_SYNCFRAME_MASK_TOTALSUBSEQ,mask)){
            nRet = A2DP_DECODER_NO_ERROR;
            break;
        }
        a2dp_audio_list_remove(list, lc3_decoder_frame);
    }

    node = a2dp_audio_list_begin(list);
    if (node){
        lc3_decoder_frame = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_list_node(node);
        TRACE(6,"%s nRet:%d SEQ:%d timestamp:%d %d/%d", __func__, nRet, lc3_decoder_frame->sequenceNumber, lc3_decoder_frame->timestamp,
                                                      lc3_decoder_frame->curSubSequenceNumber, lc3_decoder_frame->totalSubSequenceNumber);
    }else{
        TRACE(1,",%s nRet:%d", __func__, nRet);
    }

    return nRet;
}


int a2dp_audio_lc3_synchronize_dest_packet_mut(uint16_t packet_mut)
{
    list_node_t *node = NULL;
    uint32_t list_len = 0;
    list_t *list = a2dp_audio_context_p->audio_datapath.input_raw_packet_list;
    if(list == NULL)
        return -1;
    a2dp_audio_lc3_decoder_frame_t *lc3_decoder_frame_p = NULL;

    list_len = a2dp_audio_list_length(list);
    if (list_len > packet_mut){
        do{        
            node = a2dp_audio_list_begin(list);            
            lc3_decoder_frame_p = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_list_node(node);
            a2dp_audio_list_remove(list, lc3_decoder_frame_p);
        }while(a2dp_audio_list_length(list) > packet_mut);
    }

    TRACE(2,"%s list:%d", __func__, a2dp_audio_list_length(list));

    return A2DP_DECODER_NO_ERROR;
}


int a2dp_audio_lc3_convert_list_to_samples(uint32_t *samples)
{
    uint32_t list_len = 0;
    list_t *list = a2dp_audio_context_p->audio_datapath.input_raw_packet_list;
    if(list == NULL)
        return -1;
    if (samples == NULL) {
        return A2DP_DECODER_DECODE_ERROR;
    }
    list_len = a2dp_audio_list_length(list);
    *samples = LC3_OUTPUT_FRAME_SAMPLES*list_len;

    TRACE(3, "%s list:%d samples:%d", __func__, list_len, *samples);

    return A2DP_DECODER_NO_ERROR;
}

int a2dp_audio_lc3_discards_samples(uint32_t samples)
{
    int nRet = A2DP_DECODER_SYNC_ERROR;
    list_t *list = a2dp_audio_context_p->audio_datapath.input_raw_packet_list;
    if(list == NULL)
        return -1;
    a2dp_audio_lc3_decoder_frame_t *lc3_decoder_frame_p = NULL;
    list_node_t *node = NULL;
    int need_remove_list = 0;
    uint32_t list_samples = 0;
    ASSERT(!(samples%LC3_OUTPUT_FRAME_SAMPLES), "%s samples err:%d", __func__, samples);

    a2dp_audio_lc3_convert_list_to_samples(&list_samples);
    if (list_samples >= samples){
        need_remove_list = samples/LC3_OUTPUT_FRAME_SAMPLES;
        for (int i=0; i<need_remove_list; i++){
            node = a2dp_audio_list_begin(list);
            lc3_decoder_frame_p = (a2dp_audio_lc3_decoder_frame_t *)a2dp_audio_list_node(node);
            a2dp_audio_list_remove(list, lc3_decoder_frame_p);
        }
        nRet = A2DP_DECODER_NO_ERROR;
    }
    return nRet;
}

A2DP_AUDIO_DECODER_T a2dp_audio_lc3_decoder_config = {
                                                        {44100, 2, 16},
                                                        1,
                                                        a2dp_audio_lc3_init,
                                                        a2dp_audio_lc3_deinit,
                                                        a2dp_audio_lc3_decode_frame,
                                                        a2dp_audio_lc3_preparse_packet,
                                                        a2dp_audio_lc3_store_packet,
                                                        a2dp_audio_lc3_discards_packet,
                                                        a2dp_audio_lc3_synchronize_packet,
                                                        a2dp_audio_lc3_synchronize_dest_packet_mut,
                                                        a2dp_audio_lc3_convert_list_to_samples,
                                                        a2dp_audio_lc3_discards_samples,
                                                        a2dp_audio_lc3_headframe_info_get,
                                                        a2dp_audio_lc3_info_get,
                                                        a2dp_audio_lc3_free,
                                                     } ;


