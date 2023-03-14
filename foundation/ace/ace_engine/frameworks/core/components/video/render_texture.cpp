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

#include "core/components/video/render_texture.h"

#include "core/components/theme/theme_manager.h"
#include "core/components/video/video_component.h"
#include "core/components/video/video_theme.h"

namespace OHOS::Ace {

RenderTexture::RenderTexture() : RenderNode(true) {}

void RenderTexture::Update(const RefPtr<Component>& component)
{
    const RefPtr<TextureComponent> texture = AceType::DynamicCast<TextureComponent>(component);
    if (!texture) {
        return;
    }

    textureId_ = texture->GetTextureId();
    auto sourceSize = Size(static_cast<double>(texture->GetSrcWidth()), static_cast<double>(texture->GetSrcHeight()));
    if (sourceSize.IsValid()) {
        sourceSize_ = sourceSize;
    }
#if (!defined OHOS_STANDARD_SYSTEM) || (defined ENABLE_ROSEN_BACKEND)
    imageFit_ = texture->GetFit();
#endif
    imagePosition_ = texture->GetImagePosition();
    InitControlsHeight(component);

    MarkNeedLayout();
}

void RenderTexture::InitControlsHeight(const RefPtr<Component>& component)
{
    controlsHeight_ = 0.0;
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is null");
        return;
    }

    auto themeManager = context->GetThemeManager();
    if (!themeManager) {
        LOGE("theme manager is null");
        return;
    }

    auto videoTheme = themeManager->GetTheme<VideoTheme>();
    if (!videoTheme) {
        LOGE("video theme is null");
        return;
    }

    auto videoComponent = AceType::DynamicCast<VideoComponent>(component);
    if (!videoComponent) {
        LOGI("component is not video");
        return;
    }

    if (videoComponent->NeedControls()) {
        controlsHeight_ += NormalizeToPx(Dimension(videoTheme->GetBtnSize().Height(), DimensionUnit::VP));
        controlsHeight_ += NormalizeToPx(videoTheme->GetBtnEdge().Top());
        controlsHeight_ += NormalizeToPx(videoTheme->GetBtnEdge().Bottom());
    }
}

void RenderTexture::SetHidden(bool hidden, bool inRecursion)
{
    RenderNode::SetHidden(hidden, inRecursion);
    if (hiddenChangeEvent_) {
        hiddenChangeEvent_(hidden);
    }
}

void RenderTexture::PerformLayout()
{
    if (!NeedLayout()) {
        return;
    }
    double width = GetLayoutParam().GetMinSize().Width();
    double height = GetLayoutParam().GetMinSize().Height();
    for (const auto& item : GetChildren()) {
        item->Layout(GetLayoutParam());
        width = std::max(width, item->GetLayoutSize().Width());
        height = std::max(height, item->GetLayoutSize().Height());
    }

    if (!GetLayoutParam().GetMaxSize().IsInfinite()) {
        SetLayoutSize(GetLayoutParam().GetMaxSize());
    } else {
        SetLayoutSize(Size(width, height));
    }

    switch (imageFit_) {
        case ImageFit::CONTAIN:
            CalculateFitContain();
            break;
        case ImageFit::FILL:
            CalculateFitFill();
            break;
        case ImageFit::COVER:
            CalculateFitCover();
            break;
        case ImageFit::NONE:
            CalculateFitNone();
            break;
        case ImageFit::SCALEDOWN:
            CalculateFitScaleDown();
            break;
        default:
            CalculateFitContain();
            break;
    }

    ApplyObjectPosition();

    SetNeedLayout(false);
    if (textureSizeChangeEvent_) {
        textureSizeChangeEvent_(textureId_, drawSize_.Width(), drawSize_.Height());
    }
    if (textureOffsetChangeEvent_) {
        textureOffsetChangeEvent_(textureId_,
            (int32_t)(alignmentX_ + GetGlobalOffset().GetX()), (int32_t)(alignmentY_ + GetGlobalOffset().GetY()));
    }
    MarkNeedRender();
}

void RenderTexture::ApplyObjectPosition()
{
    const Size& layoutSize = GetLayoutSize();
    if (imagePosition_.GetSizeTypeX() == BackgroundImagePositionType::PX) {
        alignmentX_ = imagePosition_.GetSizeValueX();
    } else {
        alignmentX_ = imagePosition_.GetSizeValueX() * (layoutSize.Width() - drawSize_.Width()) / PERCENT_TRANSLATE;
    }

    if (imagePosition_.GetSizeTypeY() == BackgroundImagePositionType::PX) {
        alignmentY_ = imagePosition_.GetSizeValueY();
    } else {
        alignmentY_ = imagePosition_.GetSizeValueY() * (layoutSize.Height() - drawSize_.Height()) / PERCENT_TRANSLATE;
    }
}

void RenderTexture::CalculateFitContain()
{
    const Size& layoutSize = GetLayoutSize();
    double layoutRatio = NearZero(layoutSize.Height()) ? 0.0 : layoutSize.Width() / layoutSize.Height();
    double sourceRatio = NearZero(sourceSize_.Height()) ? layoutRatio : sourceSize_.Width() / sourceSize_.Height();

    if (NearZero(layoutRatio) || NearZero(sourceRatio)) {
        drawSize_ = layoutSize;
    } else if (sourceRatio < layoutRatio) {
        drawSize_ = Size(sourceRatio * layoutSize.Height(), layoutSize.Height());
    } else {
        drawSize_ = Size(layoutSize.Width(), layoutSize.Width() / sourceRatio);
    }
}

void RenderTexture::CalculateFitCover()
{
    const Size& layoutSize = GetLayoutSize();
    double layoutRatio = NearZero(layoutSize.Height()) ? 0.0 : layoutSize.Width() / layoutSize.Height();
    double sourceRatio = NearZero(sourceSize_.Height()) ? layoutRatio : sourceSize_.Width() / sourceSize_.Height();

    if (NearZero(layoutRatio) || NearZero(sourceRatio)) {
        drawSize_ = layoutSize;
    } else if (sourceRatio < layoutRatio) {
        drawSize_ = Size(layoutSize.Width(), layoutSize.Width() / sourceRatio);
    } else {
        drawSize_ = Size(layoutSize.Height() * sourceRatio, layoutSize.Height());
    }
}

void RenderTexture::CalculateFitFill()
{
    drawSize_ = GetLayoutSize();
}

void RenderTexture::CalculateFitNone()
{
    drawSize_ = sourceSize_;
}

void RenderTexture::CalculateFitScaleDown()
{
    const Size& LayoutSize = GetLayoutSize();

    if (LayoutSize.Width() > sourceSize_.Width()) {
        CalculateFitNone();
    } else {
        CalculateFitContain();
    }
}

} // namespace OHOS::Ace
