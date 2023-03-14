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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_FLUTTER_RENDER_TEXTURE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_FLUTTER_RENDER_TEXTURE_H

#include "flutter/fml/memory/ref_counted.h"

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/common/properties/color.h"
#include "core/components/video/render_texture.h"
#include "core/pipeline/layers/clip_layer.h"
#include "core/pipeline/layers/picture_layer.h"
#include "core/pipeline/layers/texture_layer.h"

namespace OHOS::Ace {

class FlutterRenderTexture final : public RenderTexture {
    DECLARE_ACE_TYPE(FlutterRenderTexture, RenderTexture);

public:
    FlutterRenderTexture() = default;
    ~FlutterRenderTexture() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    RenderLayer GetRenderLayer() override;
    void UpdateOpacity(uint8_t opacity) override;
    void DumpTree(int32_t depth) override;
    void SetIsAddGaussianFuzzy(bool isAddGaussianFuzzy) override;

private:
    void PerformLayout() override;

    void AddTextureLayer();
    void AddBackgroundLayer();
    void DrawBackground();
    void AddGaussianFuzzy(RenderContext& context, const Offset& offset);
    void InitGaussianFuzzyParas();

    RefPtr<Flutter::TextureLayer> textureLayer_;
    RefPtr<Flutter::ClipLayer> layer_;
    RefPtr<Flutter::PictureLayer> backgroundLayer_;
    bool needDrawBackground_ = false;
    RefPtr<Animator> controller_;
    RefPtr<CurveAnimation<uint8_t>> moveAnimation_;
    Color colorValue_;
    Rect gaussianFuzzySize_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_FLUTTER_RENDER_TEXTURE_H
