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

#ifndef HISTREAMER_PIPELINE_FILTER_AVBUFFER_CALIBRATION_H
#define HISTREAMER_PIPELINE_FILTER_AVBUFFER_CALIBRATION_H
#include <memory>
#include "pts_calibration_strategy.h"
#include "plugin/common/plugin_tags.h"
#include "pipeline/core/type_define.h"
namespace OHOS {
namespace Media {
namespace Pipeline {
class BufferCalibration {
public:
    BufferCalibration();
    virtual ~BufferCalibration() = default;

    void SetStrategy(std::unique_ptr<PtsCalibrationStrategy> ptr)
    {
        ptsCalibrationStrategy_ = std::move(ptr);
    }

    virtual void SetParam(Plugin::Tag tag, const Plugin::Any& param) = 0;

    void Enable()
    {
        enable_ = true;
        EnableImpl();
    }

    void Disable()
    {
        enable_ = false;
        DisableImpl();
    }

    void Reset()
    {
        Disable();
        ResetImpl();
        ptsCalibrationStrategy_->Reset();
    }

    void CorrectBuffer(const AVBufferPtr& bufferPtr)
    {
        if (!enable_ || bufferPtr == nullptr ||bufferPtr->IsEmpty()) {
            return;
        }
        CorrectBufferImpl(bufferPtr);
    }

protected:
    virtual void EnableImpl() {}
    virtual void DisableImpl() {}
    virtual void ResetImpl() {}
    virtual void CorrectBufferImpl(const AVBufferPtr& bufferPtr) {}
    bool enable_ {false};
    std::unique_ptr<PtsCalibrationStrategy> ptsCalibrationStrategy_;
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_PIPELINE_FILTER_AVBUFFER_CALIBRATION_H
