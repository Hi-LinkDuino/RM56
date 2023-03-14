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

#include "core/components/box/flutter_mask_painter.h"

#include <cmath>

#include "core/components/common/painter/flutter_decoration_painter.h"
#include "core/components/declaration/common/declaration.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

void FlutterMaskPainter::LoadMask(const WeakPtr<PipelineContext>& context, const RefPtr<RenderNode>& parent)
{
    if (!IsValid()) {
        LOGW("valid mask image type");
        return;
    }

    if (!parent) {
        LOGE("pipeline context or render node is null!");
        return;
    }
    parent_ = parent;

    if (maskLoadStatus_ == MaskLoadingStatus::LOAD_SUCCESS) {
        maskLoadStatus_ = MaskLoadingStatus::UPDATING;
    }

    if (IsSvgImage()) {
        LoadSVGImage(context);
    } else if (IsColorGradient()) {
        LoadGradient(context);
    } else if (IsPath()) {
        LoadPath(context);
    }
}

void FlutterMaskPainter::LoadSVGImage(const WeakPtr<PipelineContext>& context)
{
    if (file_.empty()) {
        LOGW("mask image is empty.");
        maskLoadStatus_ = MaskLoadingStatus::UNLOADED;
        return;
    }

    if (maskLoadStatus_ == MaskLoadingStatus::UPDATING && file_ == lastFile_) {
        // File don't need reload when it is same.
        maskLoadStatus_ = MaskLoadingStatus::LOAD_SUCCESS;
        return;
    }

    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        LOGE("pipeline context or render node is null!");
        maskLoadStatus_ = MaskLoadingStatus::UNLOADED;
        return;
    }
    auto successCallback = [svgImage = Claim(this)](const sk_sp<SkSVGDOM>& svgDom) {
        if (svgDom) {
            svgImage->skiaDom_ = svgDom;
            svgImage->maskLoadStatus_ = MaskLoadingStatus::LOAD_SUCCESS;
            if (svgImage->parent_) {
                svgImage->parent_->MarkNeedLayout(true);
            }
        }
    };
    auto failedCallback = [svgImage = Claim(this)]() {
        svgImage->maskLoadStatus_ = MaskLoadingStatus::LOAD_FAIL;
        LOGE("Create imageProvider fail! mask image is %{private}s", svgImage->maskImage_.c_str());
        if (svgImage->parent_) {
            svgImage->parent_->MarkNeedLayout();
        }
    };

    auto onPostBackgroundTaskCallback = [weakImage = AceType::WeakClaim(this)] (CancelableTask task) {
        auto svgImage = weakImage.Upgrade();
        if (svgImage) {
            svgImage->SetFetchImageObjBackgroundTask(task);
        }
    };

    ImageProvider::GetSVGImageDOMAsyncFromSrc(
        file_,
        successCallback,
        failedCallback,
        context,
        0,
        onPostBackgroundTaskCallback);
}

void FlutterMaskPainter::LoadGradient(const WeakPtr<PipelineContext>& context)
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
    maskLoadStatus_ = MaskLoadingStatus::LOAD_SUCCESS;
    parent_->MarkNeedRender();
}

void FlutterMaskPainter::LoadPath(const WeakPtr<PipelineContext>& context)
{
    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        LOGE("pipeline context or render node is null!");
        maskLoadStatus_ = MaskLoadingStatus::UNLOADED;
        return;
    }

    maskLoadStatus_ = MaskLoadingStatus::LOAD_SUCCESS;
    parent_->MarkNeedRender();

}

bool FlutterMaskPainter::GetGradientPaint(const Rect& paintRect, RefPtr<Flutter::ClipLayer>& clipLayer)
{
    if (!clipLayer) {
        return false;
    }

    SkPaint maskPaint;
    maskPaint.setAntiAlias(true);
    auto decorationPainter = AceType::MakeRefPtr<FlutterDecorationPainter>(
        decoration_, paintRect, Size(paintRect.Width(), paintRect.Height()), dipScale_);
    if (decorationPainter && decorationPainter->GetGradientPaint(maskPaint)) {
        clipLayer->SetColorMask(maskPaint);
        lastMaskImageType_ = maskImageType_;
        return true;
    }

    return false;
}

bool FlutterMaskPainter::GetPathPaint(const Rect& paintRect, const SkPath& path, RefPtr<Flutter::ClipLayer>& clipLayer)
{
    if (!clipLayer) {
        return false;
    }
    SkPaint maskPaint;
    maskPaint.setAntiAlias(true);
    maskPaint.setStyle(SkPaint::Style::kFill_Style);

    const auto& basicShape = maskPath_->GetBasicShape();
    if (basicShape != nullptr) {
        maskPaint.setColor(basicShape->GetColor().GetValue());
    } else {
        LOGE("basicShape is null.");
        return false;
    }

    clipLayer->SetPathMask(maskPaint, path);
    return true;
}

bool FlutterMaskPainter::GetSVGImageDom(RefPtr<Flutter::ClipLayer>& clipLayer)
{
    if (!clipLayer) {
        LOGE("clipLayer is not ready.");
        return false;
    }
    if (!skiaDom_) {
        LOGW("skia dom is not ready.");
        return false;
    }

    double x = 0.0f;
    double y = 0.0f;
    Size boxSize = parent_->GetLayoutSize();
    Size svgSize = Size(skiaDom_->containerSize().width(), skiaDom_->containerSize().height());
    if (boxSize.IsInfinite() || !boxSize.IsValid()) {
        // when layout size is invalid, try the container size of svg
        boxSize = svgSize;
        if (boxSize.IsInfinite() || !boxSize.IsValid()) {
            LOGE("Invalid layout size: %{private}s, boxSize: %{private}s, stop draw svg. max size: %{private}s",
                parent_->GetLayoutSize().ToString().c_str(), boxSize.ToString().c_str(),
                parent_->GetLayoutParam().GetMaxSize().ToString().c_str());
            return false;
        } else {
            LOGW("Invalid layout size: %{private}s, use svg size to draw: %{private}s, max size:%{private}s",
                parent_->GetLayoutSize().ToString().c_str(), boxSize.ToString().c_str(),
                parent_->GetLayoutParam().GetMaxSize().ToString().c_str());
        }
    } else {
        // update svg scale and size by image-size
        UpdateSvgScale(boxSize, svgSize);
        Size left = boxSize - svgSize;
        if (left.Width() < 0) {
            left.SetWidth(0);
        }
        if (left.Height() < 0) {
            left.SetHeight(0);
        }
        x = (position_.GetSizeTypeX() == BackgroundImagePositionType::PERCENT) ?
            (left.Width() * position_.GetSizeValueX() / 100) : position_.GetSizeValueX();
        y = (position_.GetSizeTypeY() == BackgroundImagePositionType::PERCENT) ?
            (left.Height() * position_.GetSizeValueY() / 100) : position_.GetSizeValueY();
    }

    int32_t width = static_cast<int32_t>(boxSize.Width());
    int32_t height = static_cast<int32_t>(boxSize.Height());
    if (svgSize.Width() <= 0 || svgSize.Height() <= 0) {
        skiaDom_->setContainerSize({ width, height });
    }

    lastFile_ = file_;
    lastMaskImageType_ = maskImageType_;
    clipLayer->SetSvgMask(skiaDom_, x, y, scaleX_, scaleY_);
    return true;
}

void FlutterMaskPainter::UpdateSvgScale(const Size& boxSize, Size& svgSize)
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
        scaleX_ = valueX / 100;
        svgSize.SetWidth(svgSize.Width() * scaleX_);
    } else {
        return;
    }
    if (typeY == BackgroundImageSizeType::PERCENT) {
        scaleY_ = valueY / 100;
        svgSize.SetHeight(svgSize.Height() * scaleY_);
    } else if (typeY == BackgroundImageSizeType::LENGTH) {
        scaleY_ = valueY / svgSize.Height();
        svgSize.SetHeight(valueY);
    } else if (typeY == BackgroundImageSizeType::AUTO) {
        scaleY_ = scaleX_;
        svgSize.SetHeight(svgSize.Height() * scaleY_);
    }
}

bool FlutterMaskPainter::HasReady() const
{
    if (!IsValid()) {
        return false;
    }

    if (maskLoadStatus_ == MaskLoadingStatus::LOAD_SUCCESS ||
        (maskLoadStatus_ == MaskLoadingStatus::UPDATING &&
        lastMaskImageType_ > MaskImageType::NONE && lastMaskImageType_ <= MaskImageType::COLOR)) {
        return true;
    }
    return false;
}

bool FlutterMaskPainter::IsPaintSvgMask() const
{
    if ((maskLoadStatus_ == MaskLoadingStatus::LOAD_SUCCESS && IsSvgImage()) ||
        (maskLoadStatus_ == MaskLoadingStatus::UPDATING && IsLastSvgImage())) {
        return true;
    }
    return false;
}

} // namespace OHOS::Ace
