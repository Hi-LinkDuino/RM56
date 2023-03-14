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

#include "audio_type_translate.h"

#include <map>
#include <utility>

#include "audio_errors.h"
#include "errors.h"

namespace {
using namespace OHOS;
const std::pair<AudioStandard::AudioSamplingRate, uint32_t> g_auSampleRateMap[] = {
    {AudioStandard::SAMPLE_RATE_8000, 8000},
    {AudioStandard::SAMPLE_RATE_11025, 11025},
    {AudioStandard::SAMPLE_RATE_12000, 12000},
    {AudioStandard::SAMPLE_RATE_16000, 16000},
    {AudioStandard::SAMPLE_RATE_22050, 22050},
    {AudioStandard::SAMPLE_RATE_24000, 24000},
    {AudioStandard::SAMPLE_RATE_32000, 32000},
    {AudioStandard::SAMPLE_RATE_44100, 44100},
    {AudioStandard::SAMPLE_RATE_48000, 48000},
    {AudioStandard::SAMPLE_RATE_64000, 64000},
    {AudioStandard::SAMPLE_RATE_96000, 96000},
};
const std::pair<AudioStandard::AudioSampleFormat, Media::Plugin::AudioSampleFormat> g_aduFmtMap[] = {
    {AudioStandard::SAMPLE_U8, Media::Plugin::AudioSampleFormat::U8},
    {AudioStandard::SAMPLE_S16LE, Media::Plugin::AudioSampleFormat::S16},
    {AudioStandard::SAMPLE_S24LE, Media::Plugin::AudioSampleFormat::S24},
    {AudioStandard::SAMPLE_S32LE, Media::Plugin::AudioSampleFormat::S32}
};
const std::pair<AudioStandard::AudioChannel, uint32_t> g_auChannelsMap[] = {
    {AudioStandard::MONO, 1},
    {AudioStandard::STEREO, 2},
};
}

namespace OHOS {
namespace Media {
namespace Plugin {
namespace AuCapturePlugin {
bool SampleRateEnum2Num (OHOS::AudioStandard::AudioSamplingRate enumVal, uint32_t& numVal)
{
    for (const auto& item : g_auSampleRateMap) {
        if (item.first == enumVal) {
            numVal = item.second;
            return true;
        }
    }
    numVal = 0;
    return false;
}
bool SampleRateNum2Enum (uint32_t numVal, OHOS::AudioStandard::AudioSamplingRate& enumVal)
{
    for (const auto& item : g_auSampleRateMap) {
        if (item.second == numVal) {
            enumVal = item.first;
            return true;
        }
    }
    return false;
}

bool SampleFmt2PluginFmt(OHOS::AudioStandard::AudioSampleFormat aFmt, AudioSampleFormat& pFmt)
{
    for (const auto& item : g_aduFmtMap) {
        if (item.first == aFmt) {
            pFmt = item.second;
            return true;
        }
    }
    return false;
}
bool PluginFmt2SampleFmt(AudioSampleFormat pFmt, OHOS::AudioStandard::AudioSampleFormat& aFmt)
{
    for (const auto& item : g_aduFmtMap) {
        if (item.second == pFmt) {
            aFmt = item.first;
            return true;
        }
    }
    return false;
}

bool ChannelNumEnum2Num(OHOS::AudioStandard::AudioChannel enumVal, uint32_t& numVal)
{
    for (const auto& item : g_auChannelsMap) {
        if (item.first == enumVal) {
            numVal = item.second;
            return true;
        }
    }
    return false;
}
bool ChannelNumNum2Enum(uint32_t numVal, OHOS::AudioStandard::AudioChannel& enumVal)
{
    for (const auto& item : g_auChannelsMap) {
        if (item.second == numVal) {
            enumVal = item.first;
            return true;
        }
    }
    return false;
}
Status Error2Status(int32_t err)
{
    const static std::map<int32_t, Status> transMap = {
        {OHOS::AudioStandard::SUCCESS, Status::OK},
        {OHOS::ERR_OK, Status::OK},
        {OHOS::ERR_INVALID_OPERATION, Status::ERROR_WRONG_STATE},
        {OHOS::ERR_NO_MEMORY, Status::ERROR_NO_MEMORY},
        {OHOS::ERR_INVALID_VALUE, Status::ERROR_INVALID_PARAMETER},
        {OHOS::ERR_NAME_NOT_FOUND, Status::ERROR_NOT_EXISTED},
        {OHOS::ERR_PERMISSION_DENIED, Status::ERROR_PERMISSION_DENIED},
        {OHOS::ERR_ENOUGH_DATA, Status::ERROR_NOT_ENOUGH_DATA},
        {OHOS::ERR_WOULD_BLOCK, Status::ERROR_AGAIN},
        {OHOS::ERR_TIMED_OUT, Status::ERROR_TIMED_OUT},
        {OHOS::ERR_ALREADY_EXISTS, Status::ERROR_UNKNOWN},
        {OHOS::ERR_DEAD_OBJECT, Status::ERROR_UNKNOWN},
        {OHOS::ERR_NO_INIT, Status::ERROR_UNKNOWN},
        {OHOS::ERR_OVERFLOW, Status::ERROR_UNKNOWN},
    };
    if (transMap.count(err) != 0) {
        return transMap.at(err);
    }
    return Status::ERROR_UNKNOWN;
}
} // namespace AuCapturePlugin
} // namespace Plugin
} // namespace Media
} // namespace OHOS