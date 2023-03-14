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

#ifndef HISTREAMER_PIPELINE_CORE_PARAMETER_H
#define HISTREAMER_PIPELINE_CORE_PARAMETER_H

#include <string>

namespace OHOS {
namespace Media {
namespace Pipeline {

const int32_t KEY_PARAMETER_START = 0x00000000; // NOLINT: global variable
// source parameter start from 0x00010000
const int32_t KEY_SOURCE_PARAMETER_START = 0x00010000; // NOLINT: global variable
// demuxer parameter start from 0x00020000
const int32_t KEY_DEMUXER_PARAMETER_START = 0x00020000; // NOLINT: global variable
// codec parameter start from 0x000300000
const int32_t KEY_CODEC_PARAMETER_START = 0x00030000; // NOLINT: global variable

// sink parameter start from 0x00400000
const int32_t KEY_SINK_PARAMETER_START = 0x00040000; // NOLINT: global variable
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_PIPELINE_CORE_PARAMETER_H
