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

#ifndef HISTREAMER_FFMPEG_CODEC_MAP_H
#define HISTREAMER_FFMPEG_CODEC_MAP_H

#include "plugin/common/plugin_caps.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace FFCodecMap {
bool CodecId2Cap(AVCodecID codecId, bool encoder, Capability& cap);
bool FormatName2Cap(const std::string& fmtName, CapabilitySet& outCap);
bool Mime2CodecId(const std::string& mime, AVCodecID& codecId);
} // FFCodecMap
} // Plugin
} // Media
} // OHOS
#endif // HISTREAMER_FFMPEG_CODEC_MAP_H
