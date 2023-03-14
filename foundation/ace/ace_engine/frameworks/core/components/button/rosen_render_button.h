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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_ROSEN_RENDER_BUTTON_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_ROSEN_RENDER_BUTTON_H

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPoint.h"

#include "core/components/button/render_button.h"

namespace OHOS::Ace {

class RosenRenderButton : public RenderButton {
    DECLARE_ACE_TYPE(RosenRenderButton, RenderButton);

public:
    RosenRenderButton() = default;
    ~RosenRenderButton() override = default;

    bool HasEffectiveTransform() const override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void PerformLayout() override;

private:
    Size Measure() override;

    void UpdateLayer();
    void PaintButtonAnimation();
    void MeasureButtonSize();
    void MeasureCircle();
    void MeasureCapsule();
    void ResetBoxRadius();
    void PaintLayer(SkCanvas* canvas);
    void DrawShape(SkCanvas* canvas, const Offset& offset, bool isStroke = false);
    void DrawArc(SkCanvas* canvas, const Offset& offset);
    void DrawLineProgress(SkCanvas* canvas, const Offset& offset);
    void DrawLineProgressAnimation(SkCanvas* canvas, const Offset& offset);
    void DrawCircleProgress(SkCanvas* canvas, const Offset& offset);
    void DrawDownloadButton(SkCanvas* canvas, const Offset& offset);
    void DrawButton(SkCanvas* canvas, const Offset& offset);
    void ConvertToSkVector(const std::array<Radius, 4>& radii, SkVector* skRadii);
    uint32_t GetStateColor();
    bool NeedClickedColor(const Color& backgroundColor);

    SkVector radii_[4] = { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
    Matrix4 transformLayer_ = Matrix4::CreateIdentity();
    float opacityLayer_ = 1.0f;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_ROSEN_RENDER_BUTTON_H
