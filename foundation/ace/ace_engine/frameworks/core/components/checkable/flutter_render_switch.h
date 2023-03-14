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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_FLUTTER_RENDER_SWITCH_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_FLUTTER_RENDER_SWITCH_H

#include "core/components/checkable/render_checkable.h"
#include "core/components/checkable/render_switch.h"
#include "core/components/common/painter/flutter_checkable_painter.h"
#include "core/components/text/render_text.h"
#include "core/pipeline/base/flutter_render_context.h"
#include "core/pipeline/base/scoped_canvas_state.h"
#include "core/pipeline/layers/opacity_layer.h"

namespace OHOS::Ace {

class FlutterRenderSwitch : public RenderSwitch, public FlutterCheckablePainter {
    DECLARE_ACE_TYPE(FlutterRenderSwitch, RenderSwitch);

public:
    FlutterRenderSwitch() = default;
    ~FlutterRenderSwitch() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void SetPaintStyle(
        double& originX, double& originY, uint32_t& trackColor, uint32_t& pointColor, flutter::Paint& trackPaint);
    Size CalculateTextSize(const std::string& text, RefPtr<RenderText>& renderText) override;

    RenderLayer GetRenderLayer() override
    {
        return ConfigureOpacityLayer(disabled_);
    }

private:
    void DrawTrackAnimation(const Offset& paintOffset, ScopedCanvas& canvas, flutter::PaintData& paintData,
        flutter::Paint& trackPaint) const;
    void DrawTrackOffAndOn(const Offset& paintOffset, double trackRadius, ScopedCanvas& canvas,
        flutter::PaintData& paintData, flutter::Paint& trackPaint) const;
    void PaintCenterPoint(const ScopedCanvas& canvas, flutter::Paint& paint, double pointOriginX, double pointOriginY,
        uint32_t pointColor) const;
    void PaintTrack(const ScopedCanvas& canvas, flutter::Paint& trackPaint, double originX, double originY,
        uint32_t trackColor) const;

    Size paintTrackSize_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_FLUTTER_RENDER_SWITCH_H
