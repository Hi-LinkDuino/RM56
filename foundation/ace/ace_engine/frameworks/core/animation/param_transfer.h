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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PARAM_TRANSFER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PARAM_TRANSFER_H

#include "base/memory/ace_type.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {

class ParamTransfer : public AceType {
    DECLARE_ACE_TYPE(ParamTransfer, AceType);

public:
    explicit ParamTransfer(double value) : value_(value) {}
    ~ParamTransfer() override = default;

    static constexpr double CURVE_COEFFICIENT = 0.18;

    // value control value
    // delta the distance between current index and control index
    virtual double Transfer(double value, int32_t delta) = 0;

    double GetParam() const
    {
        return value_;
    }

    void SetParam(double value)
    {
        value_ = value;
    }

protected:
    double value_ = 0.0;
};

// Linear Transfer
class LinearParamTransfer : public ParamTransfer {
    DECLARE_ACE_TYPE(LinearParamTransfer, ParamTransfer);

public:
    explicit LinearParamTransfer(double value) : ParamTransfer(value) {}
    ~LinearParamTransfer() override = default;

    double Transfer(double value, int32_t delta) override
    {
        return value - value_ * delta;
    }
};

// Exponential Transfer
class ExpParamTransfer : public ParamTransfer {
    DECLARE_ACE_TYPE(ExpParamTransfer, ParamTransfer);

public:
    explicit ExpParamTransfer(double value) : ParamTransfer(value) {}
    ~ExpParamTransfer() override = default;

    double Transfer(double value, int32_t delta) override
    {
        return std::pow(delta + 1, -value_ * CURVE_COEFFICIENT) * value;
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PARAM_TRANSFER_H
