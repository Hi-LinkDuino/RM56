/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_PROGRESS_DATA_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_PROGRESS_DATA_H

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

class ProgressData : public AceType {
    DECLARE_ACE_TYPE(ProgressData, AceType);

public:
    ProgressData() = default;
    ~ProgressData() override = default;

    void SetMinValue(double minValue)
    {
        minValue_ = minValue;
    }

    void SetMaxValue(double maxValue)
    {
        maxValue_ = maxValue;
    }

    void SetValue(double value)
    {
        value_ = value;
    }

    void SetStepValue(double stepValue)
    {
        stepValue_ = stepValue;
    }

    void SetCachedValue(double cachedValue)
    {
        cachedValue_ = cachedValue;
    }

    // Change the value by the number of step. Positive number will move forwards and negative number will move
    // backwards.
    double MoveSteps(int32_t num)
    {
        double temp = value_ + (stepValue_ * num);
        if (temp < minValue_) {
            value_ = minValue_;
        } else if (temp > maxValue_) {
            value_ = maxValue_;
        } else {
            value_ = temp;
        }
        return value_;
    }

    double GetMinValue() const
    {
        return minValue_;
    }

    double GetMaxValue() const
    {
        return maxValue_;
    }

    double GetValue() const
    {
        return value_;
    }

    double GetStepValue() const
    {
        return stepValue_;
    }

    double GetCachedValue() const
    {
        return cachedValue_;
    }

private:
    double minValue_ { 0.0 };
    double maxValue_ { 1.0 };
    double value_ { 0.0 };
    double stepValue_ { 0.01 };
    double cachedValue_ { 0.0 };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_PROGRESS_DATA_H
