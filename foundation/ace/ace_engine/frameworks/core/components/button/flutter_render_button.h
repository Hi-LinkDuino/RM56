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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_FLUTTER_RENDER_BUTTON_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_FLUTTER_RENDER_BUTTON_H

#include "flutter/lib/ui/painting/canvas.h"

#include "core/components/button/render_button.h"
#include "core/pipeline/layers/opacity_layer.h"
#include "core/pipeline/layers/transform_layer.h"

namespace OHOS::Ace {

class FlutterRenderButton : public RenderButton {
    DECLARE_ACE_TYPE(FlutterRenderButton, RenderButton);

public:
    FlutterRenderButton() = default;
    ~FlutterRenderButton() override = default;

    RenderLayer GetRenderLayer() override;
    bool HasEffectiveTransform() const override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void OnGlobalPositionChanged() override;

private:
    Size Measure() override;

    void UpdateLayer();
    void PaintButtonAnimation();
    void MeasureButtonSize();
    void MeasureCircle();
    void MeasureCapsule();
    void ResetBoxRadius();
    void DrawShape(flutter::Canvas& canvas, const Offset& offset, bool isStroke = false);
    void DrawArc(flutter::Canvas& canvas, const Offset& offset);
    void DrawLineProgress(flutter::Canvas& canvas, const Offset& offset);
    void DrawLineProgressAnimation(flutter::Canvas& canvas, const Offset& offset);
    void DrawCircleProgress(flutter::Canvas& canvas, const Offset& offset);
    void DrawDownloadButton(flutter::Canvas& canvas, const Offset& offset);
    void DrawButton(flutter::Canvas& canvas, const Offset& offset);
    void ConvertToSkVector(const std::array<Radius, 4>& radii, SkVector* skRadii);
    uint32_t GetStateColor();
    bool NeedClickedColor(const Color& backgroundColor);

    SkVector radii_[4] = { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
    RefPtr<Flutter::TransformLayer> transformLayer_;
    RefPtr<Flutter::OpacityLayer> opacityLayer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_FLUTTER_RENDER_BUTTON_H
