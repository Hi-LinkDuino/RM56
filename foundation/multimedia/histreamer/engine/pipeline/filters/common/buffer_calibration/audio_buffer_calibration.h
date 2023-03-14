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

#ifndef HISTREAMER_PIPELINE_AUDIO_BUFFER_CALIBRATION_H
#define HISTREAMER_PIPELINE_AUDIO_BUFFER_CALIBRATION_H
#include "buffer_calibration.h"
namespace OHOS {
namespace Media {
namespace Pipeline {
class AudioBufferCalibration : public BufferCalibration {
public:
    ~AudioBufferCalibration() override = default;

    void SetParam(Plugin::Tag tag, const Plugin::Any& param) override;

protected:
    void ResetImpl() override;
    void CorrectBufferImpl(const AVBufferPtr& bufferPtr) override;

private:
    void CorrectDuration(const AVBufferPtr& bufferPtr) const;
    void CorrectPts(const AVBufferPtr& bufferPtr) const;

    Plugin::AudioSampleFormat sampleFormat_ {Plugin::AudioSampleFormat::NONE};
    uint32_t bytesPerSample_ {0};
    uint32_t channels_ {0};
    uint32_t sampleRate_ {0};
    int64_t startTime_ {-1};
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_PIPELINE_AUDIO_BUFFER_CALIBRATION_H
