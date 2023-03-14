/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#ifndef __AUDIO_FORMAT_TYPE_H__
#define __AUDIO_FORMAT_TYPE_H__

#include <stdint.h>
#include <stdbool.h>

#define URL_LEN 4096

#define FORMAT_LANGUAGE_LEN 4

typedef enum audio_key_type audio_key_type_t;
enum audio_key_type
{
    AUDIO_KEY_TYPE_MIME = 0x6d696d65,
    AUDIO_KEY_TYPE_HTTP_HEADERS = 0x68706864,
    AUDIO_KEY_TYPE_LANGUAGE = 0x6c616e67,
    AUDIO_KEY_TYPE_PRE_CACHE = 0x70726361,
    AUDIO_KEY_TYPE_AOUT_WATERLINE = 0x57414C49,
    AUDIO_KEY_TYPE_PCM_PARAMETER = 0x50434D50,
};

typedef enum player_type player_type_t;
enum player_type
{
    LOCAL_PLAYER,
    TTS_PLAYER,
    WIFI_PLAYER,
    USB_PLAYER,
    BLUETOOTH_PLAYER,
    DLNA_PLAYER,
    HIPLAY_PLAYER,
    ALARM_PLAYER,
    NOTIFICATION_PLAYER,
    TALK_PLAYER,
    VOIP_TYPE,
    VOIP_LOCAL_TYPE,
    STORE_LOCAL_TYPE,
    USBTALK,

    PLAYER_TYPE_BUT,
};

typedef struct stream_param stream_paramter_t;
struct stream_param
{
    int32_t channel_num;
    int32_t sample_rate;
    int32_t bit_num;
};

typedef struct audio_parameter_item audio_parameter_item_t;
struct audio_parameter_item
{
    enum audio_key_type key;
    int32_t size;
    union
    {
        int32_t s32Value;
        uint32_t u32Value;
        int64_t s64Value;
        uint64_t u64Value;
        float fValue;
        double dValue;
        const void *pValue;
    } value;
};

typedef enum audio_seek_mode audio_seek_mode_t;
enum audio_seek_mode
{
    AUDIO_SEEK_MODE_FORWARD,
    AUDIO_SEEK_MODE_BACKWARD,
    AUDIO_SEEK_MODE_CLOSEST,
    AUDIO_SEEK_MODE_BUT,
};

typedef enum audio_demuxer_type audio_demuxer_type_t;
#define AUDIO_DEMUXER_UNSUPPORT AUDIO_DEMUXER_MAX_COUNT
enum audio_demuxer_type
{
    AUDIO_DEMUXER_UNKONWN = 0,
    AUDIO_DEMUXER_MP3,
    AUDIO_DEMUXER_WAV,
    AUDIO_DEMUXER_MP4,
    AUDIO_DEMUXER_AAC,
    AUDIO_DEMUXER_PCM,
    AUDIO_DEMUXER_FLAC,
    // AUDIO_DEMUXER_fMP4,
    // AUDIO_DEMUXER_TS,
    // AUDIO_DEMUXER_M3U8, /* maybe not konw ts or fmp4 */
    // AUDIO_FORMAT_OGG,
    AUDIO_DEMUXER_MAX_COUNT,
};

typedef enum audio_decoder_type audio_decoder_type_t;
#define AUDIO_DECODER_UNSUPPORT AUDIO_DECODER_MAX_COUNT
enum audio_decoder_type
{
    AUDIO_DECODER_UNKONWN = 0,
    AUDIO_DECODER_MP3,
    AUDIO_DECODER_AAC_ADTS,
    AUDIO_DECODER_AAC_MP4,
    AUDIO_DECODER_PCM,
    // AUDIO_DECODER_OGG_VORBIS,
    // AUDIO_DECODER_OPUS,
    AUDIO_DECODER_MAX_COUNT,
};

typedef enum audio_sink_type audio_sink_type_t;
#define AUDIO_SINK_UNSUPPORT AUDIO_SINK_MAX_COUNT
enum audio_sink_type
{
    AUDIO_SINK_UNKONWN = 0,
    AUDIO_SINK_ALSA,
    AUDIO_SINK_LE_AUDIO,
    AUDIO_SINK_BT_AUDIO,
    AUDIO_SINK_MAX_COUNT,
};

typedef enum audio_sample_foramt audio_sample_foramt_t;
enum audio_sample_foramt
{
    AUDIO_PCM_SAMPLE_FORMAT_S8 = 0,
    AUDIO_PCM_SAMPLE_FORMAT_S16,
    AUDIO_PCM_SAMPLE_FORMAT_S24,
    AUDIO_PCM_SAMPLE_FORMAT_S32,
    AUDIO_PCM_SAMPLE_FORMAT_FLOAT,
};

typedef struct audio_format_buffer_setting audio_format_buffer_setting_t;
struct audio_format_buffer_setting
{
    int64_t maxSize;
    int64_t maxDurationMs;
};

typedef struct audio_format_frame audio_format_frame_t;
struct audio_format_frame
{
    uint32_t trackId;
    bool isKeyFrame;
    int64_t timestampUs;
    int64_t durationUs;
    uint8_t *data;
    uint32_t len;
    uint32_t frameIndex;
    int64_t position;
    int32_t itemCnt;
    struct audio_parameter_item *item;
};

typedef struct audio_track_info audio_track_info_t;
struct audio_track_info
{
    enum audio_decoder_type decoder_type;
    enum audio_demuxer_type format_type;
    uint32_t profile;
    uint32_t sampleRate;
    enum audio_sample_foramt sampleFmt;
    uint16_t channels;
    int32_t subStreamID;
    uint32_t bitrate;
    char language[FORMAT_LANGUAGE_LEN];
    int64_t durationMs;
};

typedef struct audio_program_track_info audio_program_track_info_t;
struct audio_program_track_info
{
    int32_t trackId;
    struct audio_track_info audTrack;
    int32_t itemCnt;
    struct audio_parameter_item *item;
};

typedef struct audio_program_info audio_program_info_t;
struct audio_program_info
{
    int32_t programId;
    int64_t durationMs;
    uint32_t trackNum;
    struct audio_program_track_info *track;
};

typedef struct audio_file_info audio_file_info_t;
struct audio_file_info
{
    char *formatName;
    uint32_t bitrate;
    uint32_t programNum;
    struct audio_program_info *programInfo;
    uint8_t is_chunk;
};

typedef enum audio_data_flag audio_data_flag_t;
enum audio_data_flag
{
    AUDIO_DATA_FLAG_NONE = 0,
    AUDIO_DATA_FLAG_SYNCFRAME = 1,
    AUDIO_DATA_FLAG_CODECCONFIG = 2,
    AUDIO_DATA_FLAG_EOS = 4,
    AUDIO_DATA_FLAG_PARTIAL_FRAME = 8,
    AUDIO_DATA_FLAG_ENDOFFRAME = 16,
    AUDIO_DATA_FLAG_MUXER_DATA = 32,
};

typedef struct audio_buffer_stream audio_buffer_stream_t;
typedef int32_t (*audio_buffer_stream_read_data_t)(void *, uint8_t *, int32_t, int32_t, audio_data_flag_t *);
typedef int32_t (*audio_buffer_stream_get_readable_size_t)(const void *);
struct audio_buffer_stream
{
    void *handle;
    audio_buffer_stream_read_data_t read_data;
    audio_buffer_stream_get_readable_size_t get_readable_size;
};

typedef enum audio_format_source_type audio_format_source_type_t;
enum audio_format_source_type
{
    AUDIO_SOURCE_TYPE_FD = 0,
    AUDIO_SOURCE_TYPE_URI,
    AUDIO_SOURCE_TYPE_STREAM,
    AUDIO_SOURCE_TYPE_BUFFER,
    AUDIO_SOURCE_TYPE_BUT
};

typedef struct audio_format_source audio_format_source_t;
struct audio_format_source
{
    enum audio_format_source_type type;
    union
    {
        int fd;
        char url[URL_LEN];
        const void *buffer_attr;
        audio_buffer_stream_t stream;
    };
};

typedef enum audio_format_info_type audio_format_info_type_t;
enum audio_format_info_type
{
    AUDIO_FORMAT_INFO_DEMUXER_PREPARED = 10000,
    AUDIO_FORMAT_INFO_DEMUXER_SEEK_COMPLETE,
    AUDIO_FORMAT_INFO_DEMUXER_NETWORK_DISCONNECTED,
    AUDIO_FORMAT_INFO_DEMUXER_NETWORK_RECONNECTED,
};

typedef enum audio_format_error audio_format_error_t;
enum audio_format_error
{
    AUDIO_FORMAT_ERROR_UNKNOWN = 0,
};

typedef void *audio_format_callback_handle_t;
typedef struct audio_format_callback
{
    audio_format_callback_handle_t privateDataHandle;
    int32_t (*OnError)(audio_format_callback_handle_t privateDataHandle, int32_t errorType, int32_t errorCode);
    int32_t (*OnInfo)(audio_format_callback_handle_t privateDataHandle, int32_t type, int32_t extra);
} audio_format_callback_t;

typedef struct audio_codec_buffer_info audio_codec_buffer_info_t;
struct audio_codec_buffer_info
{
    uint8_t *addr;
    uint32_t offset;
    uint32_t length;
    uint32_t size;
};

typedef struct audio_codec_input_info audio_codec_input_info_t;
struct audio_codec_input_info
{
    uint32_t bufferCnt;
    audio_codec_buffer_info_t *buffers;
    int64_t pts;
    int32_t flag;
};

typedef struct audio_codec_output_info audio_codec_output_info_t;
struct audio_codec_output_info
{
    uint32_t bufferCnt;
    audio_codec_buffer_info_t *buffers;
    int64_t timeStamp;
    uint32_t sequence;
    uint32_t flag;
    void *vendorPrivate;
};

typedef enum audio_codec_event audio_codec_event_t;
enum audio_codec_event
{
    AUDIO_CODEC_EVENT_ERROR,
    AUDIO_CODEC_EVENT_FLUSH_COMPLETE,
    AUDIO_CODEC_EVENT_STOP_COMPLETE,
    AUDIO_CODEC_EVENT_OUTPUT_FORMAT_CHANGED,
    AUDIO_CODEC_EVENT_MAX = 0x1FFFFFFF,
};

typedef int (*audio_codec_event_callback_t)(void *comp, void *app_data, audio_codec_event_t event,
                                            uint32_t data1, uint32_t data2, void *event_data);

#define AUDIO_DECODER_OUTPUT_QUEUE_ITEM_COUNT_MAX 15
#define AUDIO_DECODER_INPUT_QUEUE_ITEM_COUNT_MAX (AUDIO_DECODER_OUTPUT_QUEUE_ITEM_COUNT_MAX)
#define AUDIO_DEMUXER_OUTPUT_QUEUE_ITEM_COUNT_MAX (AUDIO_DECODER_OUTPUT_QUEUE_ITEM_COUNT_MAX)

#define AUDIO_DECODER_OUTPUT_QUEUE_ITEM_SIZE_MAX 5120
#define AUDIO_DEMUXER_OUTPUT_QUEUE_ITEM_SIZE_MAX 4096
#define AUDIO_DECODER_INPUT_QUEUE_ITEM_SIZE_MAX AUDIO_DEMUXER_OUTPUT_QUEUE_ITEM_SIZE_MAX

#endif