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

#ifndef HISTREAMER_HDI_ADAPTER_AU_UTILS_H
#define HISTREAMER_HDI_ADAPTER_AU_UTILS_H

#include <vector>
#include "audio_types.h"
#include "plugin/common/plugin_audio_tags.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HosLite {
bool PluginAuFormat2HdiAttrs(OHOS::Media::Plugin::AudioSampleFormat pFormat, AudioSampleAttributes& attrs);
bool HdiAuFormat2PluginFormat(::AudioSampleFormat hdiAudioFormat, OHOS::Media::Plugin::AudioSampleFormat& pluginFormat);
bool HdiAttrs2PluginAuFormat(AudioSampleAttributes attrs, OHOS::Media::Plugin::AudioSampleFormat& pFormat);

bool PluginSampleRate2HdiRate(uint32_t pRate, AudioSampleRatesMask& mask);
std::vector<uint32_t> HdiSampleRatesMask2PluginRates(uint32_t mask);
bool HdiRate2PluginSampleRate(AudioSampleRatesMask mask, uint32_t& pRate);

bool PluginChannelLayout2HdiMask(OHOS::Media::Plugin::AudioChannelLayout layout, AudioChannelMask& mask);
bool HdiMask2PluginChannelLayout(AudioChannelMask mask, OHOS::Media::Plugin::AudioChannelLayout& layout);

int32_t GetPcmBytes(::AudioFormat format);
} // namespace HosLite
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif
