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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_FLUTTER_RENDER_XCOMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_FLUTTER_RENDER_XCOMPONENT_H

#include "flutter/fml/memory/ref_counted.h"
#include "third_party/skia/include/core/SkCanvas.h"

#include "core/components/common/properties/color.h"
#include "core/components/xcomponent/render_xcomponent.h"
#include "core/pipeline/base/render_layer.h"
#include "core/pipeline/base/scoped_canvas_state.h"
#include "core/pipeline/layers/clip_layer.h"
#include "core/pipeline/layers/picture_layer.h"
#include "core/pipeline/layers/texture_layer.h"

namespace OHOS::Ace {
class FlutterRenderXComponent final : public RenderXComponent {
    DECLARE_ACE_TYPE(FlutterRenderXComponent, RenderXComponent);

public:
    void DumpTree(int32_t depth) override;
    void Paint(RenderContext& context, const Offset& offset) override;
    RenderLayer GetRenderLayer() override;

    RefPtr<Flutter::TextureLayer> textureLayer_;
    RefPtr<Flutter::ClipLayer> layer_;
    RefPtr<Flutter::PictureLayer> backgroundLayer_;

private:
    void AddTextureLayer();
    void AddBackgroundLayer();
    void DrawBackground();
    void PerformLayout() override;

    bool hasSetDefaultSize_ = false;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_FLUTTER_RENDER_XCOMPONENT_H
