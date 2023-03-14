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

#define HST_LOG_TAG "Fmpeg_Au_Enc_Config"
#include "ffmpeg_au_enc_config.h"

#include <functional>
#include "common/plugin_audio_tags.h"
#include "plugins/ffmpeg_adapter/utils/ffmpeg_utils.h"
#include "foundation/log.h"

namespace {
#define ASSIGN_IF_NOT_NULL(exec, target) \
do { \
    auto tmp = (exec); \
    if (tmp != nullptr) { \
        (target) = *tmp; \
    } \
} while (0) \

using namespace OHOS::Media::Plugin;
using namespace Ffmpeg;
template <typename T>
const T* FindTagInMap(Tag tag, const std::map<Tag, ValueType>& tagStore)
{
    auto ite = tagStore.find(tag);
    if (ite != tagStore.end() && ite->second.SameTypeWith(typeid(T))) {
        return AnyCast<T>(&ite->second);
    } else {
        MEDIA_LOG_W("parameter " PUBLIC_LOG_D32 " is not found or type mismatch", static_cast<int32_t>(tag));
        return nullptr;
    }
}

void ConfigAudioCommonAttr(AVCodecContext& codecContext, const std::map<Tag, ValueType>& tagStore)
{
    ASSIGN_IF_NOT_NULL(FindTagInMap<uint32_t>(Tag::AUDIO_CHANNELS, tagStore), codecContext.channels);
    ASSIGN_IF_NOT_NULL(FindTagInMap<uint32_t>(Tag::AUDIO_SAMPLE_RATE, tagStore), codecContext.sample_rate);
    ASSIGN_IF_NOT_NULL(FindTagInMap<int64_t>(Tag::MEDIA_BITRATE, tagStore), codecContext.bit_rate);
    auto audioSampleFmtPtr = FindTagInMap<AudioSampleFormat>(Tag::AUDIO_SAMPLE_FORMAT, tagStore);
    if (audioSampleFmtPtr != nullptr) {
        auto ffFmt = ConvP2FfSampleFmt(*audioSampleFmtPtr);
        if (ffFmt != AV_SAMPLE_FMT_NONE) {
            codecContext.sample_fmt = ffFmt;
        }
    }
    auto layoutPtr  = FindTagInMap<AudioChannelLayout>(Tag::AUDIO_CHANNEL_LAYOUT, tagStore);
    if (layoutPtr != nullptr) {
        auto ffLayout = ConvertChannelLayoutToFFmpeg(*layoutPtr);
        codecContext.channel_layout = ffLayout;
    }
}

void ConfigAacCodec(AVCodecContext& codecContext, const std::map<Tag, ValueType>& tagStore)
{
    ASSIGN_IF_NOT_NULL(FindTagInMap<uint32_t>(Tag::AUDIO_AAC_LEVEL, tagStore), codecContext.level);
    if (codecContext.level == FF_LEVEL_UNKNOWN) {
        codecContext.level = 1; // set to default 1
    }
    auto profilePtr = FindTagInMap<AudioAacProfile>(Tag::AUDIO_AAC_PROFILE, tagStore);
    if (profilePtr != nullptr) {
        auto ffProfile = ConvAacProfileToFfmpeg(*profilePtr);
        if (ffProfile != FF_PROFILE_UNKNOWN) {
            codecContext.profile = ffProfile;
        }
    }
}
using ConfigFunc = std::function<void(AVCodecContext&, const std::map<Tag, ValueType>&)>;
std::map<AVCodecID, ConfigFunc> g_ConfigFuncMap = {
    {AV_CODEC_ID_AAC, ConfigAacCodec},
    {AV_CODEC_ID_AAC_LATM, ConfigAacCodec}
};

void GetAudioCommonAttr(const AVCodecContext& codecContext, Tag tag, ValueType& outVal)
{
    switch (tag) {
        case Tag::AUDIO_SAMPLE_PER_FRAME:
            outVal = static_cast<uint32_t>(codecContext.frame_size);
            break;
        case Tag::MEDIA_CODEC_CONFIG: {
            std::vector<uint8_t> extra(codecContext.extradata_size);
            extra.assign(codecContext.extradata, codecContext.extradata + codecContext.extradata_size);
            outVal = extra;
            break;
        }
        default:
            break;
    }
}

void GetAacAttr(const AVCodecContext& codecContext, Tag tag, ValueType& outVal)
{
    switch (tag) {
        case Tag::AUDIO_AAC_LEVEL:
            outVal = static_cast<uint32_t>(codecContext.level);
            break;
        case Tag::AUDIO_AAC_PROFILE:
            outVal = ConvAacProfileFromFfmpeg(codecContext.profile);
            break;
        default:
            break;
    }
}

using GetAttrFunc = std::function<void(const AVCodecContext&, Tag, ValueType&)>;
std::map<AVCodecID, GetAttrFunc> g_GetAttrFuncMap = {
    {AV_CODEC_ID_AAC, GetAacAttr},
    {AV_CODEC_ID_AAC_LATM, GetAacAttr}
};
}

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
void ConfigAudioEncoder(AVCodecContext& codecContext, const std::map<Tag, ValueType>& tagStore)
{
    ConfigAudioCommonAttr(codecContext, tagStore);
    if (g_ConfigFuncMap.count(codecContext.codec_id) != 0) {
        g_ConfigFuncMap.at(codecContext.codec_id)(codecContext, tagStore);
    }
}
Status GetAudioEncoderParameters(const AVCodecContext& codecContext, Tag tag, Plugin::ValueType& outVal)
{
    outVal.Reset();
    GetAudioCommonAttr(codecContext, tag, outVal);
    if (!outVal.HasValue() && g_GetAttrFuncMap.count(codecContext.codec_id) != 0) {
        g_GetAttrFuncMap.at(codecContext.codec_id)(codecContext, tag, outVal);
    }
    if (outVal.HasValue()) {
        return Status::OK;
    } else {
        return Status::ERROR_INVALID_PARAMETER;
    }
}
} // Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS