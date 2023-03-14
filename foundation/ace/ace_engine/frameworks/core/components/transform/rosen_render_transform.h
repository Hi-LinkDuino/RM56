/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_ROSEN_RENDER_TRANSFORM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_ROSEN_RENDER_TRANSFORM_H

#include "core/components/transform/render_transform.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

class RosenRenderTransform final : public RenderTransform {
    DECLARE_ACE_TYPE(RosenRenderTransform, RenderTransform);

public:
    RosenRenderTransform() = default;
    ~RosenRenderTransform() override = default;

    void Update(const RefPtr<Component>& component) override;
    static void SyncTransformToRsNode(const std::shared_ptr<RSNode>& rsNode, const Matrix4& transformMatrix);
    void SyncOriginToRsNode(const std::shared_ptr<RSNode>& rsNode);

    bool HasGeometryProperties() const override
    {
        return true;
    }
    void SyncGeometryProperties() override;

    void OnAttachContext() override;
    bool HasEffectiveTransform() const override;

    Point GetTransformPoint(const Point& point) override;
    Rect GetTransformRect(const Rect& rect) override;

    void UpdateTransformLayer() override;

    void Mirror(const Offset& center, const Offset& global) override;

    void PerformLayout() override;

    void SetPendingAppearingTransition() override
    {
        pendingAppearingTransition_ = true;
    }

protected:
    void OnRSTransition(TransitionType type) override;

private:
    Matrix4 GetEffectiveTransform(const Offset& offset);
    bool CheckNeedPaint() const;
    double ConvertDimensionToScaleBySize(const Dimension& dimension, double size);

    Matrix4 previousTransformMatrix_;

    bool pendingAppearingTransition_ = false;
    bool pendingUpdateTransformLayer_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_FLUTTER_RENDER_TRANSFORM_H
