/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HISTREAMER_PIPELINE_PLUGIN_CAP_DESC_H
#define HISTREAMER_PIPELINE_PLUGIN_CAP_DESC_H
#include <tuple>
#include "plugin/common/plugin_source_tags.h"
#include "plugin/common/plugin_video_tags.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
const Plugin::ValueType g_emptyString = std::string();
const Plugin::ValueType g_u8Def = (uint8_t)0;
const Plugin::ValueType g_u32Def = (uint32_t)0;
const Plugin::ValueType g_d32Def = (int32_t)0;
const Plugin::ValueType g_d64Def = (int64_t)0;
const Plugin::ValueType g_u64Def = (uint64_t)0;
const Plugin::ValueType g_doubleDef = (double)0.0;
const Plugin::ValueType g_seekableDef = Plugin::Seekable::INVALID;
const Plugin::ValueType g_srcInputTypedef = Plugin::SrcInputType::UNKNOWN;
const Plugin::ValueType g_unknown = nullptr;
const Plugin::ValueType g_vecBufDef = std::vector<uint8_t>();
const Plugin::ValueType g_channelLayoutDef = Plugin::AudioChannelLayout::MONO;
const Plugin::ValueType g_auSampleFmtDef = Plugin::AudioSampleFormat::U8;
const Plugin::ValueType g_aacProfileDef = Plugin::AudioAacProfile::LC;
const Plugin::ValueType g_aacStFmtDef = Plugin::AudioAacStreamFormat::RAW;
const Plugin::ValueType g_vdPixelFmtDef = Plugin::VideoPixelFormat::UNKNOWN;
const Plugin::ValueType g_vdH264ProfileDef = Plugin::VideoH264Profile::BASELINE;

// tuple is <tagName, default_val, typeName> default_val is used for type compare
const std::map<Plugin::Tag, std::tuple<const char*, const Plugin::ValueType&, const char*>> g_tagInfoMap = {
    {Plugin::Tag::MIME, {"mime",                               g_emptyString,      "string"}},
    {Plugin::Tag::TRACK_ID, {"track_id",                       g_u32Def,           "uint32_t"}},
    {Plugin::Tag::REQUIRED_OUT_BUFFER_CNT, {"req_out_buf_cnt", g_u32Def,           "uint32_t"}},
    {Plugin::Tag::BUFFER_ALLOCATOR, {"buf_allocator",          g_unknown,          "shared_ptr<Allocator>"}},
    {Plugin::Tag::BUFFERING_SIZE, {"bufing_size",              g_u32Def,           "uint32_t"}},
    {Plugin::Tag::WATERLINE_HIGH, {"waterline_h",              g_u32Def,           "uint32_t"}},
    {Plugin::Tag::WATERLINE_LOW, {"waterline_l",               g_u32Def,           "uint32_t"}},
    {Plugin::Tag::SRC_INPUT_TYPE, {"src_input_typ",            g_srcInputTypedef,  "SrcInputType"}},
    {Plugin::Tag::MEDIA_TITLE, {"title",                       g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_ARTIST, {"artist",                     g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_LYRICIST, {"lyricist",                 g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_ALBUM, {"album",                       g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_ALBUM_ARTIST, {"album_artist",         g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_DATE, {"date",                         g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_COMMENT, {"comment",                   g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_GENRE, {"genre",                       g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_COPYRIGHT, {"copyright",               g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_LANGUAGE, {"lang",                     g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_DESCRIPTION, {"media_desc",            g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_LYRICS, {"lyrics",                     g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_DURATION, {"duration",                 g_d64Def,           "int64_t"}},
    {Plugin::Tag::MEDIA_FILE_SIZE, {"file_size",               g_u64Def,           "uint64_t"}},
    {Plugin::Tag::MEDIA_SEEKABLE, {"media_seekable",           g_seekableDef,      "Seekable"}},
    {Plugin::Tag::MEDIA_BITRATE, {"bit_rate",                  g_d64Def,           "int64_t"}},
    {Plugin::Tag::MEDIA_FILE_EXTENSION, {"file_ext",           g_emptyString,      "string"}},
    {Plugin::Tag::MEDIA_CODEC_CONFIG, {"codec_config",         g_vecBufDef,        "std::vector<uint8_t>"}},
    {Plugin::Tag::MEDIA_POSITION, {"position",                 g_u64Def,           "uint64_t"}},
    {Plugin::Tag::AUDIO_CHANNELS, {"channels",                 g_u32Def,           "uint32_t"}},
    {Plugin::Tag::AUDIO_CHANNEL_LAYOUT, {"channel_layout",     g_channelLayoutDef, "AudioChannelLayout"}},
    {Plugin::Tag::AUDIO_SAMPLE_RATE, {"sample_rate",           g_u32Def,           "uint32_t"}},
    {Plugin::Tag::AUDIO_SAMPLE_FORMAT, {"sample_fmt",          g_auSampleFmtDef,   "AudioSampleFormat"}},
    {Plugin::Tag::AUDIO_SAMPLE_PER_FRAME, {"sample_per_frame", g_u32Def,           "uint32_t"}},
    {Plugin::Tag::AUDIO_OUTPUT_CHANNELS, {"output_channels",   g_u32Def,           "uint32_t"}},
    {Plugin::Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT, {"output_channel_layout", g_channelLayoutDef, "AudioChannelLayout"}},
    {Plugin::Tag::AUDIO_MPEG_VERSION, {"ad_mpeg_ver",          g_u32Def,           "uint32_t"}},
    {Plugin::Tag::AUDIO_MPEG_LAYER, {"ad_mpeg_layer",          g_u32Def,           "uint32_t"}},
    {Plugin::Tag::AUDIO_AAC_PROFILE, {"aac_profile",           g_aacProfileDef,    "AudioAacProfile"}},
    {Plugin::Tag::AUDIO_AAC_LEVEL, {"aac_level",               g_u32Def,           "uint32_t"}},
    {Plugin::Tag::AUDIO_AAC_STREAM_FORMAT, {"aac_stm_fmt",     g_aacStFmtDef,      "AudioAacStreamFormat"}},
    {Plugin::Tag::VIDEO_WIDTH, {"vd_w",                        g_u32Def,           "uint32_t"}},
    {Plugin::Tag::VIDEO_HEIGHT, {"vd_h",                       g_u32Def,           "uint32_t"}},
    {Plugin::Tag::VIDEO_PIXEL_FORMAT, {"pixel_fmt",            g_vdPixelFmtDef,    "VideoPixelFormat"}},
    {Plugin::Tag::VIDEO_FRAME_RATE, {"frm_rate",               g_u32Def,           "uint32_t"}},
    {Plugin::Tag::VIDEO_SURFACE, {"surface",                   g_unknown,          "Surface"}},
    {Plugin::Tag::VIDEO_MAX_SURFACE_NUM, {"surface_num",       g_u32Def,           "uint32_t"}},
    {Plugin::Tag::VIDEO_CAPTURE_RATE, {"capture_rate",         g_doubleDef,        "double"}},
    {Plugin::Tag::BITS_PER_CODED_SAMPLE, {"bits_per_coded_sample", g_u32Def,       "uint32_t"}},
    {Plugin::Tag::MEDIA_START_TIME, {"med_start_time",         g_d64Def,           "int64_t"}},
    {Plugin::Tag::VIDEO_H264_PROFILE, {"h264_profile",         g_vdH264ProfileDef, "VideoH264Profile"}},
    {Plugin::Tag::VIDEO_H264_LEVEL, {"vd_level",               g_u32Def,           "uint32_t"}},
    {Plugin::Tag::APP_TOKEN_ID, {"apptoken_id",                g_u32Def,           "uint32_t"}},
    {Plugin::Tag::APP_UID, {"app_uid",                         g_d32Def,           "int32_t"}},
};

const std::map<Plugin::AudioSampleFormat, const char*> g_auSampleFmtStrMap = {
    {Plugin::AudioSampleFormat::S8, "S8"},
    {Plugin::AudioSampleFormat::U8, "U8"},
    {Plugin::AudioSampleFormat::S8P, "S8P"},
    {Plugin::AudioSampleFormat::U8P, "U8P"},
    {Plugin::AudioSampleFormat::S16, "S16"},
    {Plugin::AudioSampleFormat::U16, "U16"},
    {Plugin::AudioSampleFormat::S16P, "S16P"},
    {Plugin::AudioSampleFormat::U16P, "U16P"},
    {Plugin::AudioSampleFormat::S24, "S24"},
    {Plugin::AudioSampleFormat::U24, "U24"},
    {Plugin::AudioSampleFormat::S24P, "S24P"},
    {Plugin::AudioSampleFormat::U24P, "U24P"},
    {Plugin::AudioSampleFormat::S32, "S32"},
    {Plugin::AudioSampleFormat::U32, "U32"},
    {Plugin::AudioSampleFormat::S32P, "S32P"},
    {Plugin::AudioSampleFormat::U32P, "U32P"},
    {Plugin::AudioSampleFormat::S64, "S64"},
    {Plugin::AudioSampleFormat::U64, "U64"},
    {Plugin::AudioSampleFormat::S64P, "S64P"},
    {Plugin::AudioSampleFormat::U64P, "U64P"},
    {Plugin::AudioSampleFormat::F32, "F32"},
    {Plugin::AudioSampleFormat::F32P, "F32P"},
    {Plugin::AudioSampleFormat::F64, "F64"},
    {Plugin::AudioSampleFormat::F64P, "F64P"},
};

const std::map<Plugin::AudioChannelLayout, const char*> g_auChannelLayoutStrMap = {
    {Plugin::AudioChannelLayout::UNKNOWN, "UNKNOWN"},
    {Plugin::AudioChannelLayout::MONO, "MONO"},
    {Plugin::AudioChannelLayout::STEREO, "STEREO"},
    {Plugin::AudioChannelLayout::CH_2POINT1, "CH_2POINT1"},
    {Plugin::AudioChannelLayout::CH_2_1, "CH_2_1"},
    {Plugin::AudioChannelLayout::SURROUND, "SURROUND"},
    {Plugin::AudioChannelLayout::CH_3POINT1, "CH_3POINT1"},
    {Plugin::AudioChannelLayout::CH_4POINT0, "CH_4POINT0"},
    {Plugin::AudioChannelLayout::CH_4POINT1, "CH_4POINT1"},
    {Plugin::AudioChannelLayout::CH_2_2, "CH_2_2"},
    {Plugin::AudioChannelLayout::QUAD, "QUAD"},
    {Plugin::AudioChannelLayout::CH_5POINT0, "CH_5POINT0"},
    {Plugin::AudioChannelLayout::CH_5POINT1, "CH_5POINT1"},
    {Plugin::AudioChannelLayout::CH_5POINT0_BACK, "CH_5POINT0_BACK"},
    {Plugin::AudioChannelLayout::CH_5POINT1_BACK, "CH_5POINT1_BACK"},
    {Plugin::AudioChannelLayout::CH_6POINT0, "CH_6POINT0"},
    {Plugin::AudioChannelLayout::CH_6POINT0_FRONT, "CH_6POINT0_FRONT"},
    {Plugin::AudioChannelLayout::HEXAGONAL, "HEXAGONAL"},
    {Plugin::AudioChannelLayout::CH_6POINT1, "CH_6POINT1"},
    {Plugin::AudioChannelLayout::CH_6POINT1_BACK, "CH_6POINT1_BACK"},
    {Plugin::AudioChannelLayout::CH_6POINT1_FRONT, "CH_6POINT1_FRONT"},
    {Plugin::AudioChannelLayout::CH_7POINT0, "CH_7POINT0"},
    {Plugin::AudioChannelLayout::CH_7POINT0_FRONT, "CH_7POINT0_FRONT"},
    {Plugin::AudioChannelLayout::CH_7POINT1, "CH_7POINT1"},
    {Plugin::AudioChannelLayout::CH_7POINT1_WIDE, "CH_7POINT1_WIDE"},
    {Plugin::AudioChannelLayout::CH_7POINT1_WIDE_BACK, "CH_7POINT1_WIDE_BACK"},
#ifdef AVS3DA_SUPPORT
    {Plugin::AudioChannelLayout::CH_5POINT1POINT2, "CH_5POINT1POINT2"},
    {Plugin::AudioChannelLayout::CH_5POINT1POINT4, "CH_5POINT1POINT4"},
    {Plugin::AudioChannelLayout::CH_7POINT1POINT2, "CH_7POINT1POINT2"},
    {Plugin::AudioChannelLayout::CH_7POINT1POINT4, "CH_7POINT1POINT4"},
    {Plugin::AudioChannelLayout::CH_9POINT1POINT4, "CH_9POINT1POINT4"},
    {Plugin::AudioChannelLayout::CH_9POINT1POINT6, "CH_9POINT1POINT6"},
    {Plugin::AudioChannelLayout::CH_10POINT2, "CH_10POINT2"},
    {Plugin::AudioChannelLayout::CH_22POINT2, "CH_22POINT2"},
#endif
    {Plugin::AudioChannelLayout::OCTAGONAL, "OCTAGONAL"},
    {Plugin::AudioChannelLayout::HEXADECAGONAL, "HEXADECAGONAL"},
    {Plugin::AudioChannelLayout::STEREO_DOWNMIX, "STEREO_DOWNMIX"},
};

const std::map<Plugin::VideoPixelFormat, const char*> g_videoPixelFormatStrMap = {
    {Plugin::VideoPixelFormat::UNKNOWN, "UNKNOWN"},
    {Plugin::VideoPixelFormat::YUV410P, "YUV410P"},
    {Plugin::VideoPixelFormat::YUV411P, "YUV411P"},
    {Plugin::VideoPixelFormat::YUV420P, "YUV420P"},
    {Plugin::VideoPixelFormat::NV12, "NV12"},
    {Plugin::VideoPixelFormat::NV21, "NV21"},
    {Plugin::VideoPixelFormat::YUYV422, "YUYV422"},
    {Plugin::VideoPixelFormat::YUV422P, "YUV422P"},
    {Plugin::VideoPixelFormat::YUV444P, "YUV444P"},
    {Plugin::VideoPixelFormat::RGBA, "RGBA"},
    {Plugin::VideoPixelFormat::ARGB, "ARGB"},
    {Plugin::VideoPixelFormat::ABGR, "ABGR"},
    {Plugin::VideoPixelFormat::BGRA, "BGRA"},
    {Plugin::VideoPixelFormat::RGB24, "RGB24"},
    {Plugin::VideoPixelFormat::BGR24, "BGR24"},
    {Plugin::VideoPixelFormat::PAL8, "PAL8"},
    {Plugin::VideoPixelFormat::GRAY8, "GRAY8"},
    {Plugin::VideoPixelFormat::MONOWHITE, "MONOWHITE"},
    {Plugin::VideoPixelFormat::MONOBLACK, "MONOBLACK"},
    {Plugin::VideoPixelFormat::YUVJ420P, "YUVJ420P"},
    {Plugin::VideoPixelFormat::YUVJ422P, "YUVJ422P"},
    {Plugin::VideoPixelFormat::YUVJ444P, "YUVJ444P"},
};

const std::map<Plugin::AudioAacProfile, const char*> g_auAacProfileNameStrMap = {
    {Plugin::AudioAacProfile::NONE, "NONE"},
    {Plugin::AudioAacProfile::MAIN, "MAIN"},
    {Plugin::AudioAacProfile::LC, "LC"},
    {Plugin::AudioAacProfile::SSR, "SSR"},
    {Plugin::AudioAacProfile::LTP, "LTP"},
    {Plugin::AudioAacProfile::HE, "HE"},
    {Plugin::AudioAacProfile::SCALABLE, "SCALABLE"},
    {Plugin::AudioAacProfile::ERLC, "ERLC"},
    {Plugin::AudioAacProfile::ER_SCALABLE, "ER_SCALABLE"},
    {Plugin::AudioAacProfile::LD, "LD"},
    {Plugin::AudioAacProfile::HE_PS, "HE_PS"},
    {Plugin::AudioAacProfile::ELD, "ELD"},
    {Plugin::AudioAacProfile::XHE, "XHE"},
};

const std::map<Plugin::AudioAacStreamFormat, const char*> g_auAacStreamFormatNameStrMap = {
    {Plugin::AudioAacStreamFormat::MP2ADTS, "MP2ADTS"},
    {Plugin::AudioAacStreamFormat::MP4ADTS, "MP4ADTS"},
    {Plugin::AudioAacStreamFormat::MP4LOAS, "MP4LOAS"},
    {Plugin::AudioAacStreamFormat::MP4LATM, "MP4LATM"},
    {Plugin::AudioAacStreamFormat::ADIF, "ADIF"},
    {Plugin::AudioAacStreamFormat::MP4FF, "MP4FF"},
    {Plugin::AudioAacStreamFormat::RAW, "RAW"},
};

inline bool HasTagInfo(Plugin::Tag tag)
{
    return g_tagInfoMap.count(tag) != 0;
}

inline const char* GetTagStrName(Plugin::Tag tag)
{
    if (!HasTagInfo(tag)) {
        return "null";
    }
    return std::get<0>(g_tagInfoMap.at(tag));
}

inline const char* GetTagTypeStrName(Plugin::Tag tag)
{
    if (!HasTagInfo(tag)) {
        return "null";
    }
    return std::get<2>(g_tagInfoMap.at(tag)); // secondary parameter 2
}

inline const Plugin::ValueType* GetTagDefValue(Plugin::Tag tag)
{
    if (!HasTagInfo(tag)) {
        return nullptr;
    }
    return &std::get<1>(g_tagInfoMap.at(tag));
}

inline bool HasAudSampleFmtInfo(Plugin::AudioSampleFormat fmt)
{
    return g_auSampleFmtStrMap.count(fmt) != 0;
}

inline const char* GetAudSampleFmtNameStr(Plugin::AudioSampleFormat fmt)
{
    if (!HasAudSampleFmtInfo(fmt)) {
        return "null";
    }
    return g_auSampleFmtStrMap.at(fmt);
}

inline bool HasAudChanLyInfo(Plugin::AudioChannelLayout layout)
{
    return g_auChannelLayoutStrMap.count(layout) != 0;
}

inline const char* GetAudChanLyNameStr(Plugin::AudioChannelLayout layout)
{
    if (!HasAudChanLyInfo(layout)) {
        return "null";
    }
    return g_auChannelLayoutStrMap.at(layout);
}

inline bool HasVideoPixelFormatNameStr(Plugin::VideoPixelFormat pixelFormat)
{
    return g_videoPixelFormatStrMap.count(pixelFormat) != 0;
}

inline const char* GetVideoPixelFormatNameStr(Plugin::VideoPixelFormat pixelFormat)
{
    if (!HasVideoPixelFormatNameStr(pixelFormat)) {
        return "null";
    }
    return g_videoPixelFormatStrMap.at(pixelFormat);
}

inline bool HasAuAacProfileNameStr(Plugin::AudioAacProfile aacProfile)
{
    return g_auAacProfileNameStrMap.count(aacProfile) != 0;
}

inline const char* GetAuAacProfileNameStr(Plugin::AudioAacProfile aacProfile)
{
    if (!HasAuAacProfileNameStr(aacProfile)) {
        return "null";
    }
    return g_auAacProfileNameStrMap.at(aacProfile);
}

inline bool HasAacStreamFormatNameStr(Plugin::AudioAacStreamFormat aacStreamFormat)
{
    return g_auAacStreamFormatNameStrMap.count(aacStreamFormat) != 0;
}

inline const char* GetAuAacStreamFormatNameStr(Plugin::AudioAacStreamFormat aacStreamFormat)
{
    if (!HasAacStreamFormatNameStr(aacStreamFormat)) {
        return "null";
    }
    return g_auAacStreamFormatNameStrMap.at(aacStreamFormat);
}

inline const char* Tag2String(const Plugin::Tag tag)
{
    auto mapIte = g_tagInfoMap.find(tag);
    if (mapIte == g_tagInfoMap.end()) {
        return "NULL";
    }
    return std::get<0>(mapIte->second);
}
} // Pipeline
} // Media
} // OHOS
#endif // HISTREAMER_PIPELINE_PLUGIN_CAP_DESC_H
