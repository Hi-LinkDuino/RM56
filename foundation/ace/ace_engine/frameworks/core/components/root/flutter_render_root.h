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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_FLUTTER_RENDER_ROOT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_FLUTTER_RENDER_ROOT_H

#include "flutter/lib/ui/compositing/scene.h"
#include "flutter/lib/ui/compositing/scene_builder.h"

#include "base/geometry/matrix4.h"
#include "core/components/root/render_root.h"
#include "core/pipeline/layers/transform_layer.h"

namespace OHOS::Ace {

class FlutterRenderRoot : public RenderRoot {
    DECLARE_ACE_TYPE(FlutterRenderRoot, RenderRoot);

public:
    FlutterRenderRoot() = default;
    ~FlutterRenderRoot() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void FinishRender(const std::unique_ptr<DrawDelegate>& delegate, const Rect& dirty) override;
    RenderLayer GetRenderLayer() override;

    BridgeType GetBridgeType() const override
    {
        return BridgeType::FLUTTER;
    }

    void DumpLayerTree() override;

private:
    Matrix4 matrix_;
    RefPtr<Flutter::Layer> layer_;
    fml::RefPtr<flutter::SceneBuilder> sceneBuilder_;
    fml::RefPtr<flutter::Scene> scene_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_FLUTTER_RENDER_ROOT_H
