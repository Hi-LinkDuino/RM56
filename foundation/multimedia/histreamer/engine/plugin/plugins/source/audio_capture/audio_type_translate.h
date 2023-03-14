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

#ifndef HISTREAMER_AUDIO_CAPTURE_TYPE_TRANSLATE_H
#define HISTREAMER_AUDIO_CAPTURE_TYPE_TRANSLATE_H

#include <cstdint>
#include "audio_info.h"
#include "plugin/common/plugin_audio_tags.h"
#include "plugin/common/plugin_types.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace AuCapturePlugin {
bool SampleRateEnum2Num (OHOS::AudioStandard::AudioSamplingRate enumVal, uint32_t& numVal);
bool SampleRateNum2Enum (uint32_t numVal, OHOS::AudioStandard::AudioSamplingRate& enumVal);

bool SampleFmt2PluginFmt(OHOS::AudioStandard::AudioSampleFormat aFmt, AudioSampleFormat& pFmt);
bool PluginFmt2SampleFmt(AudioSampleFormat pFmt, OHOS::AudioStandard::AudioSampleFormat& aFmt);

bool ChannelNumEnum2Num(OHOS::AudioStandard::AudioChannel enumVal, uint32_t& numVal);
bool ChannelNumNum2Enum(uint32_t numVal, OHOS::AudioStandard::AudioChannel& enumVal);
Status Error2Status(int32_t err);
} // namespace AuCapturePlugin
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_AUDIO_CAPTURE_TYPE_TRANSLATE_H
