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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_ROSEN_RENDER_SWITCH_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_ROSEN_RENDER_SWITCH_H

#include "core/components/checkable/render_checkable.h"
#include "core/components/checkable/render_switch.h"
#include "core/components/common/painter/rosen_checkable_painter.h"
#include "core/components/text/render_text.h"

namespace OHOS::Ace {

class RosenRenderSwitch : public RenderSwitch, public RosenCheckablePainter {
    DECLARE_ACE_TYPE(RosenRenderSwitch, RenderSwitch);

public:
    RosenRenderSwitch() = default;
    ~RosenRenderSwitch() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void SetPaintStyle(
        double& originX, double& originY, uint32_t& trackColor, uint32_t& pointColor, SkPaint& trackPaint);
    Size CalculateTextSize(const std::string& text, RefPtr<RenderText>& renderText) override;

private:
    void DrawTrackAnimation(const Offset& paintOffset, SkCanvas* canvas, SkPaint& trackPaint) const;
    void DrawTrackOffAndOn(const Offset& paintOffset, double trackRadius, SkCanvas* canvas, SkPaint& trackPaint) const;
    void PaintCenterPoint(
        SkCanvas* canvas, SkPaint& paint, double pointOriginX, double pointOriginY, uint32_t pointColor) const;
    void PaintTrack(SkCanvas* canvas, SkPaint& trackPaint, double originX, double originY, uint32_t trackColor) const;

    Size paintTrackSize_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_ROSEN_RENDER_SWITCH_H
