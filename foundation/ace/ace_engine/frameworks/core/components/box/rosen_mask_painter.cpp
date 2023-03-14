/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "include/core/SkPath.h"
#include "include/core/SkPaint.h"

#include "core/components/box/rosen_mask_painter.h"
#include "core/components/common/painter/rosen_decoration_painter.h"
#include "core/components/declaration/common/declaration.h"
#include "render_service_client/core/ui/rs_node.h"

namespace OHOS::Ace {
namespace {
constexpr int32_t SIZE_PERCENT = 100;
} // namespace

void RosenMaskPainter::LoadMask(const WeakPtr<PipelineContext>& context, const RefPtr<RenderNode>& parent)
{
    if (!IsValid()) {
        return;
    }

    if (!parent) {
        return;
    }
    parent_ = parent;

    if (loadStatus_ == LoadStatus::LOADSUCCESS) {
        loadStatus_ = LoadStatus::UPDATING;
    }

    if (IsSvgImage()) {
        LoadSVGImage(context);
    } else if (IsColorGradient()) {
        LoadGradient(context);
    } else if (IsPath()) {
        LoadPath(context);
    }
}

void RosenMaskPainter::LoadSVGImage(const WeakPtr<PipelineContext>& context)
{
    if (file_.empty()) {
        LOGW("mask image is empty.");
        loadStatus_ = LoadStatus::UNLOADED;
        return;
    }

    if (loadStatus_ == LoadStatus::UPDATING && file_ == lastFile_) {
        // File don't need reload when it is same.
        loadStatus_ = LoadStatus::LOADSUCCESS;
        return;
    }

    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        LOGE("pipeline context or render node is null!");
        loadStatus_ = LoadStatus::UNLOADED;
        return;
    }
    auto successCallback = [rsPainter = Claim(this)](const sk_sp<SkSVGDOM>& svgDom) {
        if (svgDom) {
            rsPainter->skiaDom_ = svgDom;
            rsPainter->loadStatus_ = LoadStatus::LOADSUCCESS;
            if (rsPainter->parent_) {
                rsPainter->parent_->MarkNeedLayout(true);
            }
        }
    };
    auto failedCallback = [rsPainter = Claim(this)]() {
        rsPainter->loadStatus_ = LoadStatus::LOADFAIL;
        LOGE("Create imageProvider fail! mask image is %{private}s", rsPainter->maskImage_.c_str());
        if (rsPainter->parent_) {
            rsPainter->parent_->MarkNeedLayout();
        }
    };

    auto onPostBackgroundTaskCallback = [weakPainter = AceType::WeakClaim(this)] (CancelableTask task) {
        auto rsPainter = weakPainter.Upgrade();
        if (rsPainter) {
            rsPainter->SetFetchImageObjBackgroundTask(task);
        }
    };

    ImageProvider::GetSVGImageDOMAsyncFromSrc(file_, successCallback, failedCallback, context, 0,
        onPostBackgroundTaskCallback);
}

void RosenMaskPainter::LoadGradient(const WeakPtr<PipelineContext>& context)
{
    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        LOGE("pipeline context or render node is null!");
        return;
    }
    dipScale_ = pipelineContext->GetDipScale();

    Declaration declaration;
    declaration.Init();
    Declaration::SetMaskGradient(maskImage_, declaration);
    auto& backgroundStyle =
        declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }
    decoration_ = AceType::MakeRefPtr<Decoration>();
    if (decoration_) {
        decoration_->SetGradient(backgroundStyle.gradient);
    }

    loadStatus_ = LoadStatus::LOADSUCCESS;
    parent_->MarkNeedRender();
}

void RosenMaskPainter::LoadPath(const WeakPtr<PipelineContext>& context)
{
    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        LOGE("pipeline context or render node is null!");
        loadStatus_ = LoadStatus::UNLOADED;
        return;
    }

    loadStatus_ = LoadStatus::LOADSUCCESS;
    parent_->MarkNeedRender();
}

void RosenMaskPainter::SetFetchImageObjBackgroundTask(CancelableTask task)
{
    if (fetchSvgImageTask_) {
        fetchSvgImageTask_.Cancel(false);
    }
    fetchSvgImageTask_ = task;
}

bool RosenMaskPainter::GetPathPaint(SkPaint& paint)
{
    if (maskPath_ == nullptr) {
        LOGE("maskPath_ is null.");
        return false;
    }

    const auto& basicShape = maskPath_->GetBasicShape();
    if (basicShape == nullptr) {
        LOGE("BasicShape is null.");
        return false;
    }

    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setColor(basicShape->GetColor().GetValue());
    return true;
}

bool RosenMaskPainter::GetGradientPaint(const Rect& paintRect, SkPaint& paint)
{
    paint.setAntiAlias(true);
    auto decorationPainter = AceType::MakeRefPtr<RosenDecorationPainter>(
        decoration_, paintRect, Size(paintRect.Width(), paintRect.Height()), dipScale_);
    if (decorationPainter && decorationPainter->GetGradientPaint(paint)) {
        lastMaskImageType_ = maskImageType_;
        return true;
    }
    return false;
}

bool RosenMaskPainter::UpadteSVGImageDom(double& x, double& y)
{
    if (!skiaDom_) {
        LOGW("dom is not ready.");
        return false;
    }

    x = 0.0f;
    y = 0.0f;
    Size boxSize = parent_->GetLayoutSize();
    Size svgSize = Size(skiaDom_->containerSize().width(), skiaDom_->containerSize().height());
    if (boxSize.IsInfinite() || !boxSize.IsValid()) {
        if (svgSize.IsInfinite() || !svgSize.IsValid()) {
            return false;
        }
        // if layout size is invalid, use svg size
        boxSize = svgSize;
    } else {
        // update svg scale and size by image-size
        UpdateSVGScale(boxSize, svgSize);
        Size left = boxSize - svgSize;
        if (left.Width() < 0) {
            left.SetWidth(0);
        }
        if (left.Height() < 0) {
            left.SetHeight(0);
        }
        x = (position_.GetSizeTypeX() == BackgroundImagePositionType::PERCENT) ?
            (left.Width() * position_.GetSizeValueX() / SIZE_PERCENT) : position_.GetSizeValueX();
        y = (position_.GetSizeTypeY() == BackgroundImagePositionType::PERCENT) ?
            (left.Height() * position_.GetSizeValueY() / SIZE_PERCENT) : position_.GetSizeValueY();
    }

    if (svgSize.Width() <= 0 || svgSize.Height() <= 0) {
        int32_t width = static_cast<int32_t>(boxSize.Width());
        int32_t height = static_cast<int32_t>(boxSize.Height());
        skiaDom_->setContainerSize({ width, height });
    }

    lastFile_ = file_;
    lastMaskImageType_ = maskImageType_;
    return true;
}

void RosenMaskPainter::UpdateSVGScale(const Size& boxSize, Size& svgSize)
{
    scaleX_ = 1.0f;
    scaleY_ = 1.0f;
    if (!size_.IsValid()) {
        return;
    }

    auto typeX = size_.GetSizeTypeX();
    if (typeX == BackgroundImageSizeType::AUTO) {
        return;
    }
    auto typeY = size_.GetSizeTypeY();
    double valueX = size_.GetSizeValueX();
    double valueY = size_.GetSizeValueY();

    // cover use max scale
    if (typeX == BackgroundImageSizeType::COVER) {
        scaleX_ = fmax(boxSize.Width() / svgSize.Width(), boxSize.Height() / svgSize.Height());
        scaleY_ = scaleX_;
        svgSize.ApplyScale(scaleX_);
        return;
    }

    // contain use min scale
    if (typeX == BackgroundImageSizeType::CONTAIN) {
        scaleX_ = fmin(boxSize.Width() / svgSize.Width(), boxSize.Height() / svgSize.Height());
        scaleY_ = scaleX_;
        svgSize.ApplyScale(scaleX_);
        return;
    }

    // percent and length
    if (typeX == BackgroundImageSizeType::LENGTH) {
        scaleX_ = valueX / svgSize.Width();
        svgSize.SetWidth(valueX);
    } else if (typeX == BackgroundImageSizeType::PERCENT) {
        scaleX_ = valueX / SIZE_PERCENT;
        svgSize.SetWidth(svgSize.Width() * scaleX_);
    } else {
        return;
    }
    if (typeY == BackgroundImageSizeType::PERCENT) {
        scaleY_ = valueY / SIZE_PERCENT;
        svgSize.SetHeight(svgSize.Height() * scaleY_);
    } else if (typeY == BackgroundImageSizeType::LENGTH) {
        scaleY_ = valueY / svgSize.Height();
        svgSize.SetHeight(valueY);
    } else if (typeY == BackgroundImageSizeType::AUTO) {
        scaleY_ = scaleX_;
        svgSize.SetHeight(svgSize.Height() * scaleY_);
    }
}

bool RosenMaskPainter::HasReady() const
{
    if (!IsValid()) {
        return false;
    }

    if (loadStatus_ == LoadStatus::LOADSUCCESS ||
        (loadStatus_ == LoadStatus::UPDATING &&
        lastMaskImageType_ > MaskImageType::NONE && lastMaskImageType_ <= MaskImageType::COLOR)) {
        return true;
    }
    return false;
}

std::shared_ptr<Rosen::RSMask> RosenMaskPainter::GetRSMask(const Rect& paintRect, const SkPath& path)
{
    std::shared_ptr<Rosen::RSMask> rsMask = nullptr;
    if ((loadStatus_ == LoadStatus::LOADSUCCESS && IsSvgImage()) ||
        (loadStatus_ == LoadStatus::UPDATING && IsLastSvgImage())) {
        double x = 0.0f;
        double y = 0.0f;
        if (UpadteSVGImageDom(x, y)) {
            rsMask = Rosen::RSMask::CreateSVGMask(x, y, scaleX_, scaleY_, skiaDom_);
        }
    } else if (IsColorGradient()) {
        SkPaint paint;
        if (GetGradientPaint(paintRect, paint)) {
            rsMask = Rosen::RSMask::CreateGradientMask(paint);
        }
    } else if (IsPath()) {
        SkPaint paint;
        if (GetPathPaint(paint)) {
            rsMask = Rosen::RSMask::CreatePathMask(path, paint);
        }
    }
    return rsMask;
}
} // namespace OHOS::Ace