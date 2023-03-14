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

#ifndef HISTREAMER_STRICTMONOTONICPTSCAL_H
#define HISTREAMER_STRICTMONOTONICPTSCAL_H
#include "pts_calibration_strategy.h"
namespace OHOS {
namespace Media {
namespace Pipeline {
class StrictMonotonicPtsCal : public PtsCalibrationStrategy {
public:
    StrictMonotonicPtsCal() = default;
    ~StrictMonotonicPtsCal() override = default;
    void CorrectPts(const AVBufferPtr& ptr) override
    {
        if (firstSeenBuffer_) {
            if (startTime_ != HST_TIME_NONE) {
                if (std::abs(ptr->pts - startTime_) > startTimeDiffThreshold_ || ptr->pts < 0) {
                    ptr->pts = startTime_;
                }
            }
            firstSeenBuffer_ = false;
        } else if (lastPts_ > 0) {
            auto predictedPts = lastPts_ + lastDuration_;

            if (std::abs(predictedPts - ptr->pts) < ptsDiffThreshold_ && ptr->pts > 0) {
                // do not need to changed
            } else if (std::abs(predictedPts - ptr->dts) < ptsDiffThreshold_ && ptr->dts > 0) {
                ptr->pts = ptr->dts;
            } else {
                ptr->pts = predictedPts;
            }
        }
        lastPts_ = ptr->pts;
        lastDuration_ = ptr->duration;
    }

protected:
    void ResetImpl() override
    {
        lastPts_ = HST_TIME_NONE;
        lastDuration_ = 0;
        firstSeenBuffer_ = true;
    }

private:
    bool firstSeenBuffer_ {true};
    int64_t lastPts_ {HST_TIME_NONE};
    int64_t lastDuration_ {0};
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_STRICTMONOTONICPTSCAL_H
