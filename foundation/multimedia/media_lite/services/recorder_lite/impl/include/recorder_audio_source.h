/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef RECORDER_AUDIO_SOURCE_H
#define RECORDER_AUDIO_SOURCE_H

#include <memory>
#include <map>
#include <string>
#include "format.h"
#include "media_errors.h"
#include "media_info.h"
#include "recorder_source.h"
#include "audio_capturer_impl.h"

namespace OHOS {
namespace Media {
using namespace OHOS::Audio;

class RecorderAudioSource : public RecorderSource {
public:

    RecorderAudioSource();
    virtual ~RecorderAudioSource();

    int32_t Init(const RecorderAudioSourceConfig &sourceConfig);

    virtual int32_t Start() override;

    virtual int32_t AcquireBuffer(RecorderSourceBuffer &buffer, bool isBlocking) override;

    virtual int32_t ReleaseBuffer(RecorderSourceBuffer &buffer) override;

    virtual int32_t Stop() override;

    virtual int32_t Resume() override;

    virtual int32_t Pause() override;

    virtual int32_t Release() override;

private:
    std::unique_ptr<AudioCapturerImpl> audioCap_;
    uint32_t framesize_;
    uint8_t *buffer_;
    uint64_t frameSeq_;
};
}  // namespace Media
}  // namespace OHOS
#endif  // RECORDER_AUDIO_SOURCE_H
