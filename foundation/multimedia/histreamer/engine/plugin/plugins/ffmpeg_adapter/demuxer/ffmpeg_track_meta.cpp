/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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
#define HST_LOG_TAG "FfmpegTrackMeta"
#include "ffmpeg_track_meta.h"
#include "foundation/log.h"
#include "plugins/ffmpeg_adapter/utils/aac_audio_config_parser.h"
#include "plugins/ffmpeg_adapter/utils/ffmpeg_utils.h"
#include "utils/constants.h"

#ifdef VIDEO_SUPPORT
#include "plugins/ffmpeg_adapter/utils/avc_config_data_parser.h"
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
namespace {
using ConvertFunc = void (*)(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
        const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);

struct StreamConvertor {
    AVCodecID codecId;
    ConvertFunc convertor;
};

StreamConvertor g_streamConvertors[] = {{AV_CODEC_ID_PCM_S16LE, ConvertRawAudioStreamToMetaInfo},
                                        {AV_CODEC_ID_PCM_S16BE, ConvertRawAudioStreamToMetaInfo},
                                        {AV_CODEC_ID_PCM_U16LE, ConvertRawAudioStreamToMetaInfo},
                                        {AV_CODEC_ID_PCM_U16BE, ConvertRawAudioStreamToMetaInfo},
                                        {AV_CODEC_ID_PCM_S24LE, ConvertRawAudioStreamToMetaInfo},
                                        {AV_CODEC_ID_PCM_F32LE, ConvertRawAudioStreamToMetaInfo},
                                        {AV_CODEC_ID_PCM_S8, ConvertRawAudioStreamToMetaInfo},
                                        {AV_CODEC_ID_PCM_U8, ConvertRawAudioStreamToMetaInfo},
                                        {AV_CODEC_ID_MP1, ConvertMP1StreamToMetaInfo},
                                        {AV_CODEC_ID_MP2, ConvertMP2StreamToMetaInfo},
                                        {AV_CODEC_ID_MP3, ConvertMP3StreamToMetaInfo},
                                        {AV_CODEC_ID_AAC, ConvertAACStreamToMetaInfo},
                                        {AV_CODEC_ID_AAC_LATM, ConvertAACLatmStreamToMetaInfo},
                                        {AV_CODEC_ID_VORBIS, ConvertVorbisStreamToMetaInfo},
                                        {AV_CODEC_ID_FLAC, ConvertFLACStreamToMetaInfo},
                                        {AV_CODEC_ID_APE, ConvertAPEStreamToMetaInfo},
#ifdef AVS3DA_SUPPORT
                                        {AV_CODEC_ID_AVS3DA, ConvertAVS3DAStreamToMetaInfo},
#endif
#ifdef VIDEO_SUPPORT
                                        {AV_CODEC_ID_H264, ConvertAVCStreamToMetaInfo}
#endif
};

bool IsPcmStream(const AVStream& avStream)
{
    auto codecId = avStream.codecpar->codec_id;
    return codecId == AV_CODEC_ID_PCM_S16LE || codecId == AV_CODEC_ID_PCM_S16BE || codecId == AV_CODEC_ID_PCM_U16LE ||
           codecId == AV_CODEC_ID_PCM_U16BE || codecId == AV_CODEC_ID_PCM_S8 || codecId == AV_CODEC_ID_PCM_U8 ||
           codecId == AV_CODEC_ID_PCM_F32LE;
}

void ConvertCommonTrackToMetaInfo(const AVStream& avStream,
                                  const std::shared_ptr<AVFormatContext>& avFormatContext,
                                  const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::TRACK_ID>(avStream.index);
    meta.Insert<Tag::MEDIA_DURATION>(
            ConvertTimeFromFFmpeg(avStream.duration, avStream.time_base));
    meta.Insert<Tag::MEDIA_START_TIME>(
            ConvertTimeFromFFmpeg(avStream.start_time, avStream.time_base));
    if (avCodecContext->extradata_size > 0) {
        CodecConfig codecConfig;
        codecConfig.assign(avCodecContext->extradata, avCodecContext->extradata + avCodecContext->extradata_size);
        meta.Insert<Tag::MEDIA_CODEC_CONFIG>(std::move(codecConfig));
    }
    int64_t bitRate = avCodecContext->bit_rate;
    if (!bitRate) {
        bitRate = avFormatContext->bit_rate;
    }
    meta.Insert<Tag::MEDIA_BITRATE>(bitRate);
    meta.Insert<Tag::BITS_PER_CODED_SAMPLE>(avCodecContext->bits_per_coded_sample);
}

#ifdef VIDEO_SUPPORT
void ConvertCommonVideoTrackToMetaInfo(const AVStream& avStream,
                                       const std::shared_ptr<AVFormatContext>& avFormatContext,
                                       const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    ConvertCommonTrackToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
    meta.Insert<Tag::VIDEO_WIDTH>(avCodecContext->width);
    meta.Insert<Tag::VIDEO_HEIGHT>(avCodecContext->height);
    uint32_t frameRate = 0;
    if (avCodecContext->framerate.den != 0) {
        frameRate = avCodecContext->framerate.num / avCodecContext->framerate.den;
    }
    if (frameRate > 0) {
        meta.Insert<Tag::VIDEO_FRAME_RATE>(avCodecContext->framerate.num / avCodecContext->framerate.den);
    }
}
#endif

void ConvertCommonAudioStreamToMetaInfo(const AVStream& avStream,
                                        const std::shared_ptr<AVFormatContext>& avFormatContext,
                                        const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    ConvertCommonTrackToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
    if (avCodecContext->channels != -1) {
        meta.Insert<Tag::AUDIO_SAMPLE_RATE>(avCodecContext->sample_rate);
        meta.Insert<Tag::AUDIO_CHANNELS>(avCodecContext->channels);
        meta.Insert<Tag::AUDIO_CHANNEL_LAYOUT>(ConvertChannelLayoutFromFFmpeg(avCodecContext->channels,
                                                                              avCodecContext->channel_layout));
        // ffmpeg defaults to 1024 samples per frame for planar PCM in each buffer (one for each channel).
        uint32_t samplesPerFrame = 1024;
        if (!IsPcmStream(avStream) && avCodecContext->frame_size != 0) {
            samplesPerFrame = static_cast<uint32_t>(avCodecContext->frame_size);
        }
        meta.Insert<Tag::AUDIO_SAMPLE_PER_FRAME>(samplesPerFrame);
        if (avCodecContext->sample_fmt != AV_SAMPLE_FMT_NONE) {
            meta.Insert<Tag::AUDIO_SAMPLE_FORMAT>(ConvFf2PSampleFmt(avCodecContext->sample_fmt));
        } else {
            meta.Insert<Tag::AUDIO_SAMPLE_FORMAT>(AudioSampleFormat::S16);
        }
    }
}
} // namespace

void ConvertRawAudioStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                     const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_AUDIO_RAW);
    ConvertCommonAudioStreamToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
}

void ConvertMP1StreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_AUDIO_MPEG);
    ConvertCommonAudioStreamToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
    meta.Insert<Tag::AUDIO_MPEG_VERSION>(1);
    meta.Insert<Tag::AUDIO_MPEG_LAYER>(1);
}

void ConvertMP2StreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_AUDIO_MPEG);
    ConvertCommonAudioStreamToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
    meta.Insert<Tag::AUDIO_MPEG_VERSION>(1);
    meta.Insert<Tag::AUDIO_MPEG_LAYER>(2); // 2
}

void ConvertMP3StreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_AUDIO_MPEG);
    ConvertCommonAudioStreamToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
    meta.Insert<Tag::AUDIO_MPEG_VERSION>(1);
    meta.Insert<Tag::AUDIO_MPEG_LAYER>(3); // 3
}

void ConvertFLACStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                 const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_AUDIO_FLAC);
    ConvertCommonAudioStreamToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
}

void ConvertAPEStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_AUDIO_APE);
    ConvertCommonAudioStreamToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
}

void ConvertVorbisStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                   const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_AUDIO_VORBIS);
    ConvertCommonAudioStreamToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
}

void ConvertAACStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_AUDIO_AAC);
    ConvertCommonAudioStreamToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
    meta.Insert<Tag::AUDIO_MPEG_VERSION>(4); // 4
    meta.Insert<Tag::AUDIO_AAC_PROFILE>(AudioAacProfile::LC);
    if (avCodecContext->extradata_size > 0) {
        std::vector<uint8_t> codecConfig;
        codecConfig.assign(avCodecContext->extradata, avCodecContext->extradata + avCodecContext->extradata_size);
        meta.Insert<Tag::MEDIA_CODEC_CONFIG>(std::move(codecConfig));
        AACAudioConfigParser parser(avCodecContext->extradata, avCodecContext->extradata_size);
        if (!parser.ParseConfigs()) {
            return;
        }
        meta.Insert<Tag::AUDIO_AAC_LEVEL>(parser.GetLevel());
        auto profile = parser.GetProfile();
        if (profile != AudioAacProfile::NONE) {
            meta.Insert<Tag::AUDIO_AAC_PROFILE>(profile);
        }
    } else {
        meta.Insert<Tag::AUDIO_AAC_STREAM_FORMAT>(AudioAacStreamFormat::MP4ADTS);
    }
}

void ConvertAACLatmStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                    const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_AUDIO_AAC_LATM);
    ConvertCommonAudioStreamToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
    meta.Insert<Tag::AUDIO_MPEG_VERSION>(4); // 4
    meta.Insert<Tag::AUDIO_AAC_STREAM_FORMAT>(AudioAacStreamFormat::MP4LOAS);
    meta.Insert<Tag::BITS_PER_CODED_SAMPLE>(avCodecContext->bits_per_coded_sample);
}

#ifdef AVS3DA_SUPPORT
void ConvertAVS3DAStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                   const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_AUDIO_AVS3DA);
    ConvertCommonAudioStreamToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
}
#endif

#ifdef VIDEO_SUPPORT
void ConvertAVCStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Insert<Tag::MIME>(MEDIA_MIME_VIDEO_H264);
    ConvertCommonVideoTrackToMetaInfo(avStream, avFormatContext, avCodecContext, meta);
    if (avCodecContext->extradata_size > 0) {
        AVCConfigDataParser parser(avCodecContext->extradata, avCodecContext->extradata_size);
        if (!parser.ParseConfigData()) {
            return;
        }
        std::shared_ptr<uint8_t> cfgData = nullptr;
        size_t cfgDataSize = 0;
        if (parser.GetNewConfigData(cfgData, cfgDataSize) && (cfgData != nullptr) && (cfgDataSize != 0)) {
            std::vector<uint8_t> codecConfig;
            codecConfig.assign(cfgData.get(), cfgData.get() + cfgDataSize);
            meta.Insert<Tag::MEDIA_CODEC_CONFIG>(std::move(codecConfig));
        }
    }
}
#endif
void ConvertAVStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                               const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta)
{
    meta.Clear();
    auto codecId = avStream.codecpar->codec_id;
    for (auto& streamConvertor : g_streamConvertors) {
        if (streamConvertor.codecId == codecId) {
            streamConvertor.convertor(avStream, avFormatContext, avCodecContext, meta);
            return;
        }
    }
    MEDIA_LOG_E("unsupported codec id: " PUBLIC_LOG_D32 ", name: " PUBLIC_LOG_S, codecId, avcodec_get_name(codecId));
}
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
