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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_FADE_PAINTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_FADE_PAINTER_H

#include "base/geometry/offset.h"
#include "base/geometry/size.h"
#include "base/memory/ace_type.h"
#include "core/components/common/properties/color.h"
#include "core/pipeline/base/render_context.h"

class SkCanvas;

namespace OHOS::Ace {

enum class OverScrollDirection {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
};

class ScrollFadePainter : public AceType {
    DECLARE_ACE_TYPE(ScrollFadePainter, AceType);

public:
    static RefPtr<ScrollFadePainter> CreatePainter();

    virtual void PaintSide(RenderContext& context, const Size& size, const Offset& offset) = 0;

    Color GetColor() const
    {
        return color_;
    }

    void SetColor(const Color& color)
    {
        color_ = color;
    }

    double GetOpacity() const
    {
        return opacity_;
    }

    void SetOpacity(double opacity)
    {
        opacity_ = opacity;
    }

    double GetScaleFactor() const
    {
        return scaleFactor_;
    }

    void SetScaleFactor(double scaleFactor)
    {
        scaleFactor_ = scaleFactor;
    }

    void SetDirection(OverScrollDirection direction)
    {
        direction_ = direction;
    }

protected:
    OverScrollDirection direction_ = OverScrollDirection::UP;
    Color color_ = Color::GRAY;
    double opacity_ = 0.0;
    double scaleFactor_ = 0.0;
};

} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_FADE_PAINTER_H
