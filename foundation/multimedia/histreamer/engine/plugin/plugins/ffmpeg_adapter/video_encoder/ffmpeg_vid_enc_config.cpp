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

#if defined(RECORDER_SUPPORT) && defined(VIDEO_SUPPORT)

#define HST_LOG_TAG "Fmpeg_Vid_Enc_Config"
#include "ffmpeg_vid_enc_config.h"

#include <functional>
#include "common/plugin_video_tags.h"
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

const size_t DEFAULT_BITRATE = 12004000;
const size_t DEFAULT_FRAMERATE = 60;
const size_t DEFAULT_GOP_SIZE = 10;
const size_t DEFAULT_BIT_PER_CODED_SAMPLE = 24;

template <typename T>
const T* FindTagInMap(Tag tag, const std::map<Tag, ValueType>& tagStore)
{
    auto ite = tagStore.find(tag);
    if (ite != tagStore.end() && ite->second.SameTypeWith(typeid(T))) {
        return AnyCast<T>(&ite->second);
    } else {
        MEDIA_LOG_D("parameter " PUBLIC_LOG_D32 " is not found or type mismatch", static_cast<int32_t>(tag));
        return nullptr;
    }
}

Status SetVideoResolution(AVCodecContext& codecContext, const std::map<Tag, ValueType>& tagStore)
{
    uint32_t width = 0;
    uint32_t height = 0;
    ASSIGN_IF_NOT_NULL(FindTagInMap<uint32_t>(Tag::VIDEO_WIDTH, tagStore), width);
    ASSIGN_IF_NOT_NULL(FindTagInMap<uint32_t>(Tag::VIDEO_HEIGHT, tagStore), height);
    if (width == 0 || height == 0) {
        MEDIA_LOG_E("width or height is invalid");
        return Status::ERROR_INVALID_PARAMETER;
    }
    codecContext.width = width;
    codecContext.height = height;
    return Status::OK;
}

void SetVideoFrameRateAndTimeBase(AVCodecContext& codecContext, const std::map<Tag, ValueType>& tagStore)
{
    uint32_t frameRate = 0;
    ASSIGN_IF_NOT_NULL(FindTagInMap<uint32_t>(Tag::VIDEO_FRAME_RATE, tagStore), frameRate);
    if (frameRate > 0) {
        codecContext.framerate.num = static_cast<int32_t>(frameRate);
        codecContext.framerate.den = 1;
        codecContext.time_base.num = 1;
        codecContext.time_base.den = static_cast<int32_t>(frameRate);
    } else {
        codecContext.framerate.num = DEFAULT_FRAMERATE;
        codecContext.framerate.den = 1;
        codecContext.time_base.num = 1;
        codecContext.time_base.den = DEFAULT_FRAMERATE;
    }
}

Status SetVideoPixelFormat(AVCodecContext& codecContext, const std::map<Tag, ValueType>& tagStore)
{
    VideoPixelFormat pixelFormat = VideoPixelFormat::UNKNOWN;
    ASSIGN_IF_NOT_NULL(FindTagInMap<VideoPixelFormat>(Tag::VIDEO_PIXEL_FORMAT, tagStore), pixelFormat);
    FALSE_RETURN_V_MSG_E(pixelFormat != VideoPixelFormat::UNKNOWN,
        Status::ERROR_INVALID_PARAMETER,  "pixel format is invalid");
    codecContext.pix_fmt = ConvertPixelFormatToFFmpeg(pixelFormat);
    int32_t bpp = 0;
    const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(codecContext.pix_fmt);
    if (desc) {
        for (auto i = 0; i < desc->nb_components; i++) {
            bpp += desc->comp[i].depth;
        }
    }
    codecContext.bits_per_coded_sample = (bpp > 0) ? bpp : DEFAULT_BIT_PER_CODED_SAMPLE;
    codecContext.ticks_per_frame = 1;
    if (codecContext.codec_id == AV_CODEC_ID_H264) {
        codecContext.ticks_per_frame = 2; // Default 1, e.g., H.264/MPEG-2 set it to 2.
    }
    // pixel aspect ratio
    codecContext.sample_aspect_ratio.num = 1;
    codecContext.sample_aspect_ratio.den = 1;
    return Status::OK;
}

void SetDefaultColorimetry(AVCodecContext& codecContext)
{
    if (IsYuvFormat(codecContext.pix_fmt)) {
        if (codecContext.height >= 2160) { // 2160: UHD
            codecContext.color_primaries = AVCOL_PRI_BT2020;
            codecContext.color_trc = AVCOL_TRC_BT2020_12;
            codecContext.colorspace = AVCOL_SPC_BT2020_NCL;
        } else if (codecContext.height > 576) { // 576: HD
            codecContext.color_primaries = AVCOL_PRI_BT709;
            codecContext.color_trc = AVCOL_TRC_BT709;
            codecContext.colorspace = AVCOL_SPC_BT709;
        } else { // SD
            codecContext.color_primaries = AVCOL_PRI_SMPTE170M;
            codecContext.color_trc = AVCOL_TRC_SMPTE170M;
            codecContext.colorspace = AVCOL_SPC_SMPTE170M;
        }
        codecContext.color_range = AVCOL_RANGE_MPEG;
    } else if (IsRgbFormat(codecContext.pix_fmt)) {
        codecContext.color_primaries = AVCOL_PRI_BT709;
        codecContext.color_trc = AVCOL_TRC_IEC61966_2_1;
        codecContext.colorspace = AVCOL_SPC_RGB;
        codecContext.color_range = AVCOL_RANGE_JPEG;
    } else {
        codecContext.color_primaries = AVCOL_PRI_UNSPECIFIED;
        codecContext.color_trc = AVCOL_TRC_UNSPECIFIED;
        codecContext.colorspace = AVCOL_SPC_UNSPECIFIED;
        codecContext.color_range = AVCOL_RANGE_MPEG;
    }
    // location of chroma samples
    codecContext.chroma_sample_location = AVCHROMA_LOC_UNSPECIFIED;
}

void SetDefaultEncodeParams(AVCodecContext& codecContext, const std::map<Tag, ValueType>& tagStore)
{
    int64_t bitRate = 0;
    ASSIGN_IF_NOT_NULL(FindTagInMap<int64_t>(Tag::MEDIA_BITRATE, tagStore), bitRate);
    codecContext.bit_rate = (bitRate > 0) ? bitRate : DEFAULT_BITRATE;
    codecContext.gop_size = DEFAULT_GOP_SIZE;
    codecContext.max_b_frames = 1;
    SetDefaultColorimetry(codecContext);
}

Status ConfigVideoCommonAttr(AVCodecContext& codecContext, const std::map<Tag, ValueType>& tagStore)
{
    auto ret = SetVideoResolution(codecContext, tagStore);
    FALSE_RETURN_V_MSG_E(ret == Status::OK, ret, "SetVideoResolution() fail");
    ret = SetVideoPixelFormat(codecContext, tagStore);
    FALSE_RETURN_V_MSG_E(ret == Status::OK, ret, "SetVideoPixelFormat() fail");
    SetVideoFrameRateAndTimeBase(codecContext, tagStore);
    SetDefaultEncodeParams(codecContext, tagStore);
    return ret;
}

void ConfigH264Codec(AVCodecContext& codecContext, const std::map<Tag, ValueType>& tagStore)
{
    ASSIGN_IF_NOT_NULL(FindTagInMap<uint32_t>(Tag::VIDEO_H264_LEVEL, tagStore), codecContext.level);
    if (codecContext.level == FF_LEVEL_UNKNOWN) {
        codecContext.level = 41; // 41
    }
    auto profilePtr = FindTagInMap<VideoH264Profile>(Tag::VIDEO_H264_PROFILE, tagStore);
    if (profilePtr != nullptr) {
        auto profile = ConvH264ProfileToFfmpeg(*profilePtr);
        if (profile != FF_PROFILE_UNKNOWN) {
            codecContext.profile = profile;
        }
    }
    if (codecContext.profile == FF_PROFILE_UNKNOWN) {
        codecContext.profile = FF_PROFILE_H264_BASELINE;
    }
    MEDIA_LOG_D("profile: " PUBLIC_LOG_D32, codecContext.profile);
    av_opt_set(codecContext.priv_data, "preset", "slow", 0);
    av_opt_set(codecContext.priv_data, "tune", "zerolatency", 0);
    codecContext.flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}
using ConfigFunc = std::function<void(AVCodecContext&, const std::map<Tag, ValueType>&)>;
std::map<AVCodecID, ConfigFunc> g_videoConfigFuncMap = {
    {AV_CODEC_ID_H264, ConfigH264Codec},
};

void GetVideoCommonAttr(const AVCodecContext& codecContext, Tag tag, ValueType& outVal)
{
    switch (tag) {
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

void GetH264Attr(const AVCodecContext& codecContext, Tag tag, ValueType& outVal)
{
    switch (tag) {
        case Tag::VIDEO_H264_PROFILE:
            outVal = ConvH264ProfileFromFfmpeg(codecContext.profile);
            break;
        case Tag::VIDEO_H264_LEVEL:
            outVal = static_cast<uint32_t>(codecContext.level);
            break;
        default:
            break;
    }
}

using GetAttrFunc = std::function<void(const AVCodecContext&, Tag, ValueType&)>;
std::map<AVCodecID, GetAttrFunc> g_getVideoAttrFuncMap = {
    {AV_CODEC_ID_H264, GetH264Attr},
};
}

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
void ConfigVideoEncoder(AVCodecContext& codecContext, const std::map<Tag, ValueType>& tagStore)
{
    ConfigVideoCommonAttr(codecContext, tagStore);
    if (g_videoConfigFuncMap.count(codecContext.codec_id) != 0) {
        g_videoConfigFuncMap.at(codecContext.codec_id)(codecContext, tagStore);
    }
}

Status GetVideoEncoderParameters(const AVCodecContext& codecContext, Tag tag, Plugin::ValueType& outVal)
{
    outVal.Reset();
    GetVideoCommonAttr(codecContext, tag, outVal);
    if (!outVal.HasValue() && g_getVideoAttrFuncMap.count(codecContext.codec_id) != 0) {
        g_getVideoAttrFuncMap.at(codecContext.codec_id)(codecContext, tag, outVal);
    }
    if (outVal.HasValue()) {
        return Status::OK;
    } else {
        return Status::ERROR_INVALID_PARAMETER;
    }
}
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif