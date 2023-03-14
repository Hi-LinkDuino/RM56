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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_ROSEN_RENDER_TEXTURE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_ROSEN_RENDER_TEXTURE_H

#ifdef OHOS_STANDARD_SYSTEM
#include "surface.h"
#endif

#include "core/components/video/render_texture.h"

namespace OHOS::Ace {

class RosenRenderTexture final : public RenderTexture {
    DECLARE_ACE_TYPE(RosenRenderTexture, RenderTexture);

public:
    RosenRenderTexture() = default;
    ~RosenRenderTexture() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;

    void SyncGeometryProperties() override;

    std::shared_ptr<RSNode> CreateRSNode() const override;

    void Update(const RefPtr<Component>& component) override;
    void UpdateOpacity(uint8_t opacity) override;
    void DumpTree(int32_t depth) override;
    void SetIsAddGaussianFuzzy(bool isAddGaussianFuzzy) override;

#ifdef OHOS_STANDARD_SYSTEM
    OHOS::sptr<OHOS::Surface> GetSurface();
#endif

private:
    void AddGaussianFuzzy(RenderContext& context, const Offset& offset);
    void InitGaussianFuzzyParas();

    RefPtr<Animator> controller_;
    RefPtr<CurveAnimation<uint8_t>> moveAnimation_;
    Color colorValue_;
    Rect gaussianFuzzySize_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_ROSEN_RENDER_TEXTURE_H
