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

#ifndef HISTREAMER_FFMPEG_TRACK_META_H
#define HISTREAMER_FFMPEG_TRACK_META_H

#include <memory>
#include <vector>
#include "plugin/common/plugin_tags.h"
#include "plugin/common/tag_map.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
void ConvertRawAudioStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                     const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);

void ConvertMP1StreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);

void ConvertMP2StreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);

void ConvertMP3StreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);

void ConvertAACStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);

void ConvertAACLatmStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                    const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);

void ConvertVorbisStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                   const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);

void ConvertFLACStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                 const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);

void ConvertAPEStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);

#ifdef AVS3DA_SUPPORT
void ConvertAVS3DAStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                   const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);
#endif

#ifdef VIDEO_SUPPORT
void ConvertAVCStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                                const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);
#endif

void ConvertAVStreamToMetaInfo(const AVStream& avStream, const std::shared_ptr<AVFormatContext>& avFormatContext,
                               const std::shared_ptr<AVCodecContext>& avCodecContext, TagMap& meta);
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FFMPEG_TRACK_META_H
