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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_FLUTTER_RENDER_TRANSFORM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_FLUTTER_RENDER_TRANSFORM_H

#include "core/components/transform/render_transform.h"
#include "core/pipeline/base/flutter_render_context.h"
#include "core/pipeline/layers/transform_layer.h"

namespace OHOS::Ace {

class FlutterRenderTransform final : public RenderTransform {
    DECLARE_ACE_TYPE(FlutterRenderTransform, RenderTransform);

public:
    FlutterRenderTransform() = default;
    ~FlutterRenderTransform() override = default;
    RenderLayer GetRenderLayer() override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void OnGlobalPositionChanged() override;

    bool IsRepaintBoundary() const override
    {
        return true;
    }

    bool HasEffectiveTransform() const override;

    Point GetTransformPoint(const Point& point) override;
    Rect GetTransformRect(const Rect& rect) override;

    void UpdateTransformLayer() override;

    void Mirror(const Offset& center, const Offset& global) override;

private:
    Matrix4 GetEffectiveTransform(const Offset& offset);
    void UpdateTransformByGlobalOffset();
    bool CheckNeedPaint() const;

    RefPtr<Flutter::TransformLayer> layer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_FLUTTER_RENDER_TRANSFORM_H
