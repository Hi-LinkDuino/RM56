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

#ifndef HISTREAMER_AV_PTS_CALIBRATION_STRATEGY_H
#define HISTREAMER_AV_PTS_CALIBRATION_STRATEGY_H
#include "pipeline/core/type_define.h"
#include "plugin/common/plugin_time.h"
namespace OHOS {
namespace Media {
namespace Pipeline {
class PtsCalibrationStrategy {
public:
    PtsCalibrationStrategy() = default;
    virtual ~PtsCalibrationStrategy() = default;
    void Reset()
    {
        startTime_ = HST_TIME_NONE;
        ResetImpl();
    }
    virtual void CorrectPts(const AVBufferPtr& ptr) = 0;

    void SetStartTimeDiffThreshold(int64_t delta)
    {
        startTimeDiffThreshold_ = delta;
    }

    void SetPtsDiffThreshold(int64_t delta)
    {
        ptsDiffThreshold_ = delta;
    }

    void SetStartTime(int64_t startTime)
    {
        startTime_ = startTime;
    }
protected:
    virtual void ResetImpl() {}
    int64_t startTime_ {HST_TIME_NONE};
    int64_t startTimeDiffThreshold_ {5 * HST_MSECOND};
    int64_t ptsDiffThreshold_ {10 * HST_MSECOND};
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_AV_PTS_CALIBRATION_STRATEGY_H
