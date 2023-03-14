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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_LAYOUT_PARAM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_LAYOUT_PARAM_H

#include "base/geometry/size.h"

namespace OHOS::Ace {

class LayoutParam {
public:
    LayoutParam() = default;
    ~LayoutParam() = default;

    LayoutParam(Size max, Size min)
    {
        SetMaxSize(max);
        SetMinSize(min);
    }

    const Size& GetMaxSize() const
    {
        return maxSize_;
    }

    void SetMaxSize(const Size& size)
    {
        maxSize_ = size;
    }

    const Size& GetMinSize() const
    {
        return minSize_;
    }

    void SetMinSize(const Size& size)
    {
        minSize_ = size;
    }

    void SetFixedSize(const Size& size)
    {
        minSize_ = size;
        maxSize_ = size;
    }

    void SetHasUsedConstraints(bool flag)
    {
        hasUsedConstraints_ = flag;
    }

    bool HasUsedConstraints() const
    {
        return hasUsedConstraints_;
    }

    bool IsValid() const
    {
        return maxSize_.IsValid();
    }

    bool IsTight() const
    {
        return (minSize_.Width() >= maxSize_.Width()) && (minSize_.Height() >= maxSize_.Height());
    }

    bool IsWidthValid() const
    {
        return (maxSize_.Width() >= minSize_.Width()) && (maxSize_.Width() > 0.0);
    }

    bool IsHeightValid() const
    {
        return (maxSize_.Height() >= minSize_.Height()) && (maxSize_.Height() > 0.0);
    }

    Size Constrain(Size size) const
    {
        Size constrainSize;
        if (size.Width() < 0) {
            constrainSize.SetWidth(size.Width());
        } else {
            constrainSize.SetWidth(std::clamp(size.Width(), minSize_.Width(), maxSize_.Width()));
        }
        if (size.Height() < 0) {
            constrainSize.SetHeight(size.Height());
        } else {
            constrainSize.SetHeight(std::clamp(size.Height(), minSize_.Height(), maxSize_.Height()));
        }
        return constrainSize;
    }

    LayoutParam Enforce(const LayoutParam& layoutParam) const
    {
        Size min = layoutParam.Constrain(minSize_);
        Size max = layoutParam.Constrain(maxSize_);
        return LayoutParam(max, min);
    }

    bool operator==(const LayoutParam& layoutParam) const
    {
        return (minSize_ == layoutParam.minSize_) && (maxSize_ == layoutParam.maxSize_);
    }

    bool operator!=(const LayoutParam& layoutParam) const
    {
        return !operator==(layoutParam);
    }

    std::string ToString() const
    {
        return std::string(minSize_.ToString())
            .append(" - ")
            .append(maxSize_.ToString());
    }

    void SetMinWidth(double value)
    {
        minSize_.SetWidth(value);
    }

    void SetMaxWidth(double value)
    {
        maxSize_.SetWidth(value);
    }

private:
    Size minSize_ { 0.0f, 0.0f };
    Size maxSize_ { Size::INFINITE_SIZE, Size::INFINITE_SIZE };
    bool hasUsedConstraints_ = false;
};

enum class MeasureType {
    PARENT,       // return the max layout params from parent
    CONTENT,      // return the current component size
    DEFAULT       // user hasn't set the component size
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_LAYOUT_PARAM_H
