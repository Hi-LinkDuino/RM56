/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_CALC_DIMENSION_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_CALC_DIMENSION_H

#include "base/geometry/dimension.h"

namespace OHOS::Ace {
class CalcDimension : public Dimension {
public:
    CalcDimension() = default;
    ~CalcDimension() = default;

    explicit CalcDimension(const std::string& value, DimensionUnit unit = DimensionUnit::CALC)
    {
        calcvalue_ = value;
        SetUnit(DimensionUnit::CALC);
    };

    CalcDimension(double value, DimensionUnit unit = DimensionUnit::PX) : Dimension(value, unit) {};
    CalcDimension(const Dimension& dimension) : Dimension(dimension) {};

    const std::string& CalcValue() const
    {
        return calcvalue_;
    }

    void SetCalcValue(const std::string& value)
    {
        calcvalue_ = value;
    }

    CalcDimension& operator=(const Dimension& newDimension)
    {
        Dimension& dimension = *this;
        dimension = newDimension;
        return *this;
    }

    CalcDimension& operator=(const CalcDimension& newDimension)
    {
        SetCalcValue(newDimension.CalcValue());
        SetValue(newDimension.Value());
        SetUnit(newDimension.Unit());
        return *this;
    }

private:
    std::string calcvalue_ = "";
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_CALC_DIMENSION_H
