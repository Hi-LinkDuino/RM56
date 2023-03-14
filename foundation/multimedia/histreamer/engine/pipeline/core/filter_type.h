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

#ifndef HISTREAMER_PIPELINE_CORE_FILTER_TYPE_H
#define HISTREAMER_PIPELINE_CORE_FILTER_TYPE_H

#include <cstdint>

namespace OHOS {
namespace Media {
namespace Pipeline {
enum struct FilterType : uint8_t {
    NONE = 0,
    MEDIA_SOURCE,
    CAPTURE_SOURCE,
    DEMUXER,
    MUXER,
    AUDIO_DECODER,
    VIDEO_DECODER,
    AUDIO_ENCODER,
    VIDEO_ENCODER,
    AUDIO_SINK,
    VIDEO_SINK,
    OUTPUT_SINK,
};
inline bool IsSink(FilterType type)
{
    switch (type) {
        case FilterType::AUDIO_SINK:
        case FilterType::VIDEO_SINK:
        case FilterType::OUTPUT_SINK:
            return true;
        default:
            return false;
    }
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PIPELINE_CORE_FILTER_TYPE_H
