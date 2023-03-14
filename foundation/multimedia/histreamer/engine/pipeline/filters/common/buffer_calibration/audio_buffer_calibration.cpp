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

#define HST_LOG_TAG "AudioBufferCalibration"

#include "audio_buffer_calibration.h"
#include <cmath>
#include "foundation/log.h"
#include "pipeline/filters/common/plugin_utils.h"
#include "plugin/common/plugin_time.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
template<class T>
bool ExtractTag(T& out, const Plugin::Any& val)
{
    if (!val.SameTypeWith(typeid(T))) {
        return false;
    }
    out = *(Plugin::AnyCast<T>(&val));
    return true;
}
void AudioBufferCalibration::SetParam(Tag tag, const Plugin::Any& param)
{
    switch (tag) {
        case Plugin::Tag::AUDIO_SAMPLE_FORMAT:
            ExtractTag(sampleFormat_, param);
            bytesPerSample_ = Pipeline::GetBytesPerSample(sampleFormat_);
            break;
        case Plugin::Tag::AUDIO_CHANNELS:
            ExtractTag(channels_, param);
            break;
        case Plugin::Tag::AUDIO_SAMPLE_RATE:
            ExtractTag(sampleRate_, param);
            break;
        case Plugin::Tag::MEDIA_START_TIME:
            ExtractTag(startTime_, param);
            ptsCalibrationStrategy_->SetStartTime(startTime_);
            break;
        default:
            break;
    }
}

void AudioBufferCalibration::ResetImpl()
{
    sampleFormat_ = Plugin::AudioSampleFormat::NONE;
    bytesPerSample_ = 0;
    channels_ = 0;
    sampleRate_ = 0;
}

void AudioBufferCalibration::CorrectBufferImpl(const AVBufferPtr& bufferPtr)
{
    auto tmpMeta = bufferPtr->GetBufferMeta();
    if (tmpMeta!= nullptr && tmpMeta->GetType() != Plugin::BufferMetaType::AUDIO) {
        MEDIA_LOG_DD("only concerns audio buffers, ignore others");
    }
    // correction must keep in order
    CorrectDuration(bufferPtr);
    CorrectPts(bufferPtr);
}

void AudioBufferCalibration::CorrectDuration(const AVBufferPtr& bufferPtr) const
{
    if (bytesPerSample_ == 0 || channels_ == 0 || sampleRate_ == 0) {
        return;
    }
    auto samples = bufferPtr->GetMemory()->GetSize() / bytesPerSample_ / channels_;
    auto duration = samples * HST_SECOND / sampleRate_;
    int64_t delta = duration - bufferPtr->duration;
    if (std::abs(delta) > HST_MSECOND) {
        bufferPtr->duration = duration;
    }
}

void AudioBufferCalibration::CorrectPts(const AVBufferPtr &bufferPtr) const
{
    ptsCalibrationStrategy_->CorrectPts(bufferPtr);
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS