/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_MEASURABLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_MEASURABLE_H

#include "base/memory/ace_type.h"
#include "frameworks/base/geometry/animatable_dimension.h"

namespace OHOS::Ace {

class Measurable : public virtual AceType {
    DECLARE_ACE_TYPE(Measurable, AceType);

public:
    virtual const AnimatableDimension& GetWidth() const
    {
        return width_;
    }

    virtual void SetWidth(const Dimension& dimension, const AnimationOption& option = AnimationOption())
    {
        width_ = AnimatableDimension(dimension, option);
    }

    virtual void SetWidth(const CalcDimension& dimension, const AnimationOption& option = AnimationOption())
    {
        width_ = AnimatableDimension(dimension, option);
    }

    virtual void SetWidth(double width, DimensionUnit unit = DimensionUnit::PX)
    {
        width_ = AnimatableDimension(width, unit);
    }

    virtual void SetWidth(std::string width, DimensionUnit unit = DimensionUnit::CALC)
    {
        width_ = AnimatableDimension(width, unit);
    }

    virtual void SetWidth(const AnimatableDimension& dimension)
    {
        width_ = dimension;
    }

    virtual const AnimatableDimension& GetHeight() const
    {
        return height_;
    }

    virtual void SetHeight(const Dimension& dimension, const AnimationOption& option = AnimationOption())
    {
        height_ = AnimatableDimension(dimension, option);
    }

    virtual void SetHeight(const CalcDimension& dimension, const AnimationOption& option = AnimationOption())
    {
        height_ = AnimatableDimension(dimension, option);
    }

    virtual void SetHeight(double height, DimensionUnit unit = DimensionUnit::PX)
    {
        height_ = AnimatableDimension(height, unit);
    }

    virtual void SetHeight(std::string height, DimensionUnit unit = DimensionUnit::CALC)
    {
        height_ = AnimatableDimension(height, unit);
    }

    virtual void SetHeight(const AnimatableDimension& dimension)
    {
        height_ = dimension;
    }

protected:
    AnimatableDimension width_ {-1.0, DimensionUnit::PX};
    AnimatableDimension height_ {-1.0, DimensionUnit::PX};
};

}  // namespace OHOS::Ace

#endif  // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_MEASURABLE_H