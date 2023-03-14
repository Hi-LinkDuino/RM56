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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_FADE_EFFECT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_FADE_EFFECT_H

#include "base/utils/macros.h"
#include "core/components/scroll/scroll_edge_effect.h"
#include "core/components/scroll/scroll_fade_controller.h"
#include "core/components/scroll/scroll_fade_painter.h"

namespace OHOS::Ace {

class ACE_EXPORT ScrollFadeEffect final : public ScrollEdgeEffect {
    DECLARE_ACE_TYPE(ScrollFadeEffect, ScrollEdgeEffect);

public:
    explicit ScrollFadeEffect(const Color& color = Color::GRAY)
        : ScrollEdgeEffect(EdgeEffect::FADE), fadeColor_(color) {}
    ~ScrollFadeEffect() override = default;

    const Color& GetFadeColor() const
    {
        return fadeColor_;
    }

    void InitialEdgeEffect() override;
    double CalculateOverScroll(double oldPosition, bool isReachMax) override;
    void Paint(RenderContext& context, const Size& viewPort, const Offset& offset) override;
    void HandleOverScroll(Axis axis, double overScroll, const Size& viewPort) override;

private:
    void ResetFadeEffect(const Color& color);
    void SetOpacityAndScale(double opacity, double scale);
    void SetPaintDirection(Axis axis, double overScroll);

    Color fadeColor_ = Color::GRAY;
    RefPtr<ScrollFadePainter> fadePainter_;
    RefPtr<ScrollFadeController> fadeController_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_FADE_EFFECT_H
