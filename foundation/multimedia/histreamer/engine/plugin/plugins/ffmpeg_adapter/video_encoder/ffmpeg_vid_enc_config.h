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

#ifndef HISTREAMER_FFMPEG_VID_ENC_CONFIG_H
#define HISTREAMER_FFMPEG_VID_ENC_CONFIG_H

#if defined(RECORDER_SUPPORT) && defined(VIDEO_SUPPORT)

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
#ifdef __cplusplus
}
#endif

#include "plugin/common/plugin_tags.h"
#include "plugin/common/plugin_types.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
void ConfigVideoEncoder(AVCodecContext& codecContext, const std::map<Tag, ValueType>& source);
Status GetVideoEncoderParameters(const AVCodecContext& codecContext, Tag tag, Plugin::ValueType& outVal);
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif
#endif // HISTREAMER_FFMPEG_VID_ENC_CONFIG_H
