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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RENDER_TEXTURE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RENDER_TEXTURE_H

#include "core/components/common/layout/constants.h"
#include "core/components/video/texture_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderTexture : public RenderNode {
    DECLARE_ACE_TYPE(RenderTexture, RenderNode);

public:
    using HiddenChangeEvent = std::function<void(bool)>;
    using TextureSizeChangeEvent = std::function<void(int64_t, int32_t, int32_t)>;
    using TextureOffsetChangeEvent = std::function<void(int64_t, int32_t, int32_t)>;

    ~RenderTexture() override = default;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void SetHidden(bool hidden, bool inRecursion) override;

    bool IsRepaintBoundary() const override
    {
        return true;
    }

    void SetHiddenChangeEvent(HiddenChangeEvent&& hiddenChangeEvent)
    {
        hiddenChangeEvent_ = std::move(hiddenChangeEvent);
    }

    void SetTextureSizeChange(TextureSizeChangeEvent&& textureSizeChangeEvent)
    {
        textureSizeChangeEvent_ = std::move(textureSizeChangeEvent);
    }

    void SetTextureOffsetChange(TextureOffsetChangeEvent&& textureOffsetChangeEvent)
    {
        textureOffsetChangeEvent_ = std::move(textureOffsetChangeEvent);
    }

    bool SupportOpacity() override
    {
        return true;
    }

    ImageFit GetFit() const
    {
        return imageFit_;
    }

#ifdef OHOS_STANDARD_SYSTEM
    void OnAppShow() override
    {
        LOGI("RenderTexture: Camera OnAppShow.");
        RenderNode::OnAppShow();
        if (hiddenChangeEvent_) {
            hiddenChangeEvent_(false);
        }
    }

    void OnAppHide() override
    {
        LOGI("RenderTexture: Camera OnAppHidden.");
        RenderNode::OnAppHide();
        if (hiddenChangeEvent_) {
            hiddenChangeEvent_(true);
        }
    }
#endif

    void SetIsComponentSize(bool isComponentSize)
    {
        isComponentSize_ = isComponentSize;
    }

    bool IsComponentSize() const
    {
        return isComponentSize_;
    }

    virtual void SetIsAddGaussianFuzzy(bool isAddGaussianFuzzy)
    {
        isAddGaussianFuzzy_ = isAddGaussianFuzzy;
    }

    bool IsAddGaussianFuzzy() const
    {
        return isAddGaussianFuzzy_;
    }

protected:
    RenderTexture();

    int64_t textureId_ = INVALID_TEXTURE;
    Size drawSize_;   // size of draw area
    Size sourceSize_; // size of source
#if (defined OHOS_STANDARD_SYSTEM) && (!defined ENABLE_ROSEN_BACKEND)
    ImageFit imageFit_ = ImageFit::FILL;
#else
    ImageFit imageFit_ = ImageFit::CONTAIN;
#endif
    ImageObjectPosition imagePosition_;
    double alignmentX_ = 0.0;
    double alignmentY_ = 0.0;
    double controlsHeight_ = 0.0;

private:
    void CalculateFitContain();
    void CalculateFitCover();
    void CalculateFitFill();
    void CalculateFitNone();
    void CalculateFitScaleDown();
    void ApplyObjectPosition();
    void InitControlsHeight(const RefPtr<Component>& component);
    HiddenChangeEvent hiddenChangeEvent_;
    TextureSizeChangeEvent textureSizeChangeEvent_;
    TextureOffsetChangeEvent textureOffsetChangeEvent_;
    bool isComponentSize_ = false;
    bool isAddGaussianFuzzy_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RENDER_TEXTURE_H
