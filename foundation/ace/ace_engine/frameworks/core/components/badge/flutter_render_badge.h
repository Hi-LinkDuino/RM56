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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_FLUTTER_RENDER_BADGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_FLUTTER_RENDER_BADGE_H

#include "flutter/lib/ui/painting/canvas.h"

#include "core/components/badge/render_badge.h"

namespace OHOS::Ace {

class FlutterRenderBadge : public RenderBadge {
    DECLARE_ACE_TYPE(FlutterRenderBadge, RenderBadge);

public:
    FlutterRenderBadge() = default;
    ~FlutterRenderBadge() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;

private:
    void DrawBadge(RenderContext& context, const Offset& offset);
    Size CalculateTextSize(const std::string& text, const TextStyle& textStyle,
        RefPtr<RenderText>& renderText) override;
    void PaintText(const Offset& textOffset, RenderContext& context) const;
    void DrawCircleBadge(flutter::Canvas& canvas, const Offset& offset);
    void DrawNumericalBadge(flutter::Canvas& canvas, const Offset& offset);

    double badgeCircleDiameter_ = 0.0;
    double badgeCircleRadius_ = 0.0;
    double badgeWidth_ = 0.0;
    double badgeHeight_ = 0.0;
    Size textSize_;
    Offset textInitialOffset_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_FLUTTER_RENDER_BADGE_H
