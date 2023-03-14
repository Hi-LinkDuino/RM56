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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_FLUTTER_RENDER_TOGGLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_FLUTTER_RENDER_TOGGLE_H

#include "flutter/lib/ui/painting/canvas.h"

#include "core/components/toggle/render_toggle.h"
#include "core/pipeline/layers/transform_layer.h"

namespace OHOS::Ace {

class FlutterRenderToggle : public RenderToggle {
    DECLARE_ACE_TYPE(FlutterRenderToggle, RenderToggle);

public:
    FlutterRenderToggle() = default;
    ~FlutterRenderToggle() override = default;

    RenderLayer GetRenderLayer() override;
    bool HasEffectiveTransform() const override;
    void Paint(RenderContext& context, const Offset& offset) override;

private:
    Size Measure() override;
    void DrawToggle(flutter::Canvas& canvas, const Offset& offset) const;
    void UpdateLayer();
    Color GetStatusColor() const;

    RefPtr<Flutter::TransformLayer> transformLayer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_FLUTTER_RENDER_TOGGLE_H
