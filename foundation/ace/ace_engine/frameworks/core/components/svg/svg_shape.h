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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SVG_SHAPE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SVG_SHAPE_H

#include "frameworks/core/components/common/properties/svg_paint_state.h"

namespace OHOS::Ace {

class ACE_EXPORT SvgShape : public AceType {
    DECLARE_ACE_TYPE(SvgShape, AceType);

public:
    void SetOpacity(double opacity)
    {
        opacity_ = opacity;
    }

    double GetOpacity() const
    {
        return opacity_;
    }

    void SetFillState(const FillState& fillState)
    {
        fillState_ = fillState;
    }

    const FillState& GetFillState() const
    {
        return fillState_;
    }

    void SetStrokeState(const StrokeState& strokeState)
    {
        strokeState_ = strokeState;
    }

    const StrokeState& GetStrokeState() const
    {
        return strokeState_;
    }

    void SetTextStyle(const SvgTextStyle& textStyle)
    {
        textStyle_ = textStyle;
    }

    const SvgTextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    void SetTransform(const std::string& transform)
    {
        transform_ = transform;
    }

    const std::string& GetTransform() const
    {
        return transform_;
    }

protected:
    double opacity_ = 1.0;
    std::string transform_;
    FillState fillState_;
    StrokeState strokeState_;
    SvgTextStyle textStyle_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SVG_SHAPE_H
