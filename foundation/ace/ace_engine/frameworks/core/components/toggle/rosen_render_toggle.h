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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_ROSEN_RENDER_TOGGLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_ROSEN_RENDER_TOGGLE_H

#include "include/core/SkCanvas.h"

#include "core/components/toggle/render_toggle.h"
#include "core/components/transform/rosen_render_transform.h"

namespace OHOS::Ace {

class RosenRenderToggle : public RenderToggle {
    DECLARE_ACE_TYPE(RosenRenderToggle, RenderToggle);

public:
    RosenRenderToggle() = default;
    ~RosenRenderToggle() override = default;

    void PerformLayout() override;
    bool HasEffectiveTransform() const override;
    void Paint(RenderContext& context, const Offset& offset) override;

private:
    Size Measure() override;
    void DrawToggle(SkCanvas* canvas, const Offset& offset) const;
    void UpdateLayer();
    Color GetStatusColor() const;
    Matrix4 transformLayer_ = Matrix4::CreateIdentity();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_ROSEN_RENDER_TOGGLE_H
