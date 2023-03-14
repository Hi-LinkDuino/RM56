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

#define HST_LOG_TAG "CodecFilterFactory"

#include "codec_filter_factory.h"
#include "async_mode.h"
#include "codec_filter_base.h"
#include "codec/audio_decoder/audio_decoder_filter.h"
#include "codec/audio_encoder/audio_encoder_filter.h"
#include "codec/video_decoder/video_decoder_filter.h"
#include "factory/filter_factory.h"
#include "pipeline/core/filter_codec_mode.h"
#include "sync_mode.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
#ifdef OHOS_LITE
static AutoRegisterFilter<AudioDecoderFilter> g_registerAudioDecoderFilter("builtin.player.audiodecoder",
    [](const std::string& name) { return CreateCodecFilter(name, FilterCodecMode::AUDIO_SYNC_DECODER); });
#ifdef VIDEO_SUPPORT
static AutoRegisterFilter<VideoDecoderFilter> g_registerVideoDecoderFilter("builtin.player.videodecoder",
    [](const std::string& name) { return CreateCodecFilter(name, FilterCodecMode::VIDEO_ASYNC_DECODER); });
#endif
#else
static AutoRegisterFilter<AudioDecoderFilter> g_registerAudioDecoderFilter("builtin.player.audiodecoder",
    [](const std::string& name) { return CreateCodecFilter(name, FilterCodecMode::AUDIO_ASYNC_DECODER); });
#ifdef VIDEO_SUPPORT
static AutoRegisterFilter<VideoDecoderFilter> g_registerVideoDecoderFilter("builtin.player.videodecoder",
    [](const std::string& name) { return CreateCodecFilter(name, FilterCodecMode::VIDEO_ASYNC_DECODER); });
#endif
#endif

std::shared_ptr<CodecFilterBase> CreateCodecFilter(const std::string& name, FilterCodecMode type)
{
    std::shared_ptr<CodecMode> codecMode {nullptr};
    std::shared_ptr<CodecFilterBase> filter {nullptr};
    switch (type) {
        case FilterCodecMode::AUDIO_SYNC_DECODER:
            codecMode = std::make_shared<SyncMode>("audioDec");
            filter = std::make_shared<AudioDecoderFilter>(name, codecMode);
            break;
        case FilterCodecMode::AUDIO_ASYNC_DECODER:
            codecMode = std::make_shared<AsyncMode>("audioDec");
            filter = std::make_shared<AudioDecoderFilter>(name, codecMode);
            break;
#ifdef VIDEO_SUPPORT
        case FilterCodecMode::VIDEO_SYNC_DECODER:
            codecMode = std::make_shared<SyncMode>("videoDec");
            filter = std::make_shared<VideoDecoderFilter>(name, codecMode);
            break;
        case FilterCodecMode::VIDEO_ASYNC_DECODER:
            codecMode = std::make_shared<AsyncMode>("videoDec");
            filter = std::make_shared<VideoDecoderFilter>(name, codecMode);
            break;
#endif
        default:
            break;
    }
    return filter;
}
} // Pipeline
} // Media
} // OHOS

