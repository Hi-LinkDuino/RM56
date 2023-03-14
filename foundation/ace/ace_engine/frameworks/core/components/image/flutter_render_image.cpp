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

#include "core/components/image/flutter_render_image.h"

#include "flutter/common/task_runners.h"
#include "third_party/skia/include/core/SkClipOp.h"
#include "third_party/skia/include/core/SkColorFilter.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/include/core/SkShader.h"

#include "base/utils/utils.h"

#include "core/common/frontend.h"
#include "core/components/align/render_align.h"
#include "core/components/common/properties/radius.h"
#include "core/components/image/image_component.h"
#include "core/components/image/image_event.h"
#include "core/image/flutter_image_cache.h"
#include "core/image/image_object.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {
namespace {
// The [GRAY_COLOR_MATRIX] is of dimension [4 x 5], which transforms a RGB source color (R, G, B, A) to the
// destination color (R', G', B', A').
//
// A classic color image to grayscale conversion formula is [Gray = R * 0.3 + G * 0.59 + B * 0.11].
// Hence we get the following conversion:
//
// | M11 M12 M13 M14 M15 |   | R |   | R' |
// | M21 M22 M23 M24 M25 |   | G |   | G' |
// | M31 M32 M33 M34 M35 | x | B | = | B' |
// | M41 M42 M43 M44 M45 |   | A |   | A' |
//                           | 1 |
const float GRAY_COLOR_MATRIX[20] = { 0.30f, 0.59f, 0.11f, 0,    0,  // red
                                      0.30f, 0.59f, 0.11f, 0,    0,  // green
                                      0.30f, 0.59f, 0.11f, 0,    0,  // blue
                                      0,     0,     0,     1.0f, 0}; // alpha transparency
}

union SkColorEx {
    struct {
        SkColor color : 32;
        bool valid : 1;
        uint32_t reserved : 31; // reserved
    };
    uint64_t value = 0;
};

FlutterRenderImage::FlutterRenderImage()
{
    auto currentDartState = flutter::UIDartState::Current();
    if (!currentDartState) {
        return;
    }

    renderTaskHolder_ = MakeRefPtr<FlutterRenderTaskHolder>(
        currentDartState->GetSkiaUnrefQueue(),
        currentDartState->GetIOManager(),
        currentDartState->GetTaskRunners().GetIOTaskRunner());
    InitializeCallbacks();
}

FlutterRenderImage::~FlutterRenderImage()
{
    CancelBackgroundTasks();
}

void FlutterRenderImage::InitializeCallbacks()
{
    imageObjSuccessCallback_ = [weak = AceType::WeakClaim(this)](
        ImageSourceInfo info, const RefPtr<ImageObject>& imageObj) {
        LOGD("success info : %{public}s", info.ToString().c_str());
        auto renderImage = weak.Upgrade();
        if (!renderImage) {
            LOGE("renderImage upgrade fail when image object is ready. callback image source info: %{private}s",
                info.ToString().c_str());
            return;
        }
        if (renderImage->sourceInfo_ == info) {
            LOGD("image obj ready info : %{public}s", info.ToString().c_str());
            renderImage->ImageObjReady(imageObj);
            return;
        }
        LOGW("imageObjSuccessCallback: image source info verify fail. sourceInfo: %{private}s, "
             "callback source info: %{private}s",
            renderImage->sourceInfo_.ToString().c_str(), info.ToString().c_str());
    };

    failedCallback_ = [weak = AceType::WeakClaim(this)](ImageSourceInfo info) {
        auto renderImage = weak.Upgrade();
        if (!renderImage) {
            LOGE("renderImage upgrade fail when image load fail. callback image source info: %{private}s",
                info.ToString().c_str());
            return;
        }
        if (info != renderImage->sourceInfo_) {
            LOGW("image source not matched now source: %{private}s vs callback source: %{private}s.",
                renderImage->sourceInfo_.ToString().c_str(), info.ToString().c_str());
            return;
        }
        auto context = renderImage->GetContext().Upgrade();
        if (!context) {
            LOGE("context is null when handle image load fail callback. sourceInfo: %{private}s",
                renderImage->sourceInfo_.ToString().c_str());
            return;
        }
        auto isDeclarative = context->GetIsDeclarative();
        if (!isDeclarative && !renderImage->syncMode_ && renderImage->RetryLoading()) {
            LOGI("retry loading. sourceInfo: %{private}s", renderImage->sourceInfo_.ToString().c_str());
            return;
        }
        renderImage->ImageObjFailed();
    };

    uploadSuccessCallback_ = [weak = AceType::WeakClaim(this)] (
        ImageSourceInfo sourceInfo, const fml::RefPtr<flutter::CanvasImage>& image) {
        LOGD("paint success info %{public}s", sourceInfo.ToString().c_str());
        auto renderImage = weak.Upgrade();
        if (!renderImage) {
            LOGE("renderImage upgrade fail when image load success. callback image source info: %{private}s",
                sourceInfo.ToString().c_str());
            return;
        }
        if (renderImage->sourceInfo_ == sourceInfo) {
            renderImage->ImageDataPaintSuccess(image);
            return;
        }
        LOGW("paintDataSuccessCallback: image source info verify fail. sourceInfo: %{private}s, callback source info: "
             "%{private}s",
            renderImage->sourceInfo_.ToString().c_str(), sourceInfo.ToString().c_str());
    };

    onPostBackgroundTask_ = [weak = AceType::WeakClaim(this)] (CancelableTask task) {
        auto renderImage = weak.Upgrade();
        if (renderImage) {
            renderImage->SetFetchImageObjBackgroundTask(task);
        }
    };
}

bool FlutterRenderImage::IsRepaintBoundary() const
{
    return (imageObj_ && imageObj_->GetFrameCount() > 1);
}

void FlutterRenderImage::ImageObjReady(const RefPtr<ImageObject>& imageObj)
{
    LOGD("image obj ready info : %{public}s", sourceInfo_.ToString().c_str());
    imageObj_ = imageObj;
    auto imageSize = imageObj_->GetImageSize();
    bool canStartUploadImageObj = !autoResize_ && (imageObj_->GetFrameCount() == 1);
    if (canStartUploadImageObj) {
        previousResizeTarget_ = imageSize;
        resizeTarget_ = imageSize;
        imageLoadingStatus_ = ImageLoadingStatus::LOADING;
        resizeScale_ = Size(1.0, 1.0);
    }
    if (!imageObj_->IsSvg()) {
        if (sourceInfo_.IsSourceDimensionValid()) {
            rawImageSize_ = sourceInfo_.GetSourceSize();
            forceResize_ = true;
        } else {
            rawImageSize_ = imageSize;
            forceResize_ = false;
        }
        imageSizeForEvent_ = imageSize;
        rawImageSizeUpdated_ = true;
        if (!background_) {
            currentDstRect_ = dstRect_;
        } else if (imageUpdateFunc_) {
            imageUpdateFunc_();
        }
    } else {
        LOGI("image is vectorgraph(svg) without absolute size, set svgDom_");
        image_ = nullptr;
        if (useSkiaSvg_) {
            skiaDom_ = AceType::DynamicCast<SvgSkiaImageObject>(imageObj_)->GetSkiaDom();
        } else {
            if (directPaint_) {
                loadSvgOnPaint_ = true;
            } else {
                loadSvgAfterLayout_ = true;
            }
        }
        imageSizeForEvent_ = Measure();
        UpdateLoadSuccessState();
    }
    // If image component size is finally decided, only need to layout itself.
    bool layoutSizeNotChanged = (previousLayoutSize_ == GetLayoutSize());
    bool selfOnly =
        (imageComponentSize_.IsValid() && !imageComponentSize_.IsInfinite() && layoutSizeNotChanged) ||
        imageObj_->IsSvg();
    MarkNeedLayout(selfOnly);
}

void FlutterRenderImage::ImageObjFailed()
{
    LOGW("image load failed, sourceInfo : %{private}s", sourceInfo_.ToString().c_str());
    currentDstRectList_.clear();
    imageSizeForEvent_ = Size();
    image_ = nullptr;
    imageObj_ = nullptr;
    curSourceInfo_ = sourceInfo_;
    skiaDom_ = nullptr;
    svgDom_ = nullptr;
    proceedPreviousLoading_ = false;
    imageLoadingStatus_ = ImageLoadingStatus::LOAD_FAIL;
    retryCnt_ = 0;
    FireLoadEvent(imageSizeForEvent_);
    MarkNeedLayout();
}

void FlutterRenderImage::ImageDataPaintSuccess(const fml::RefPtr<flutter::CanvasImage>& image)
{
    LOGD("image data paint success.");
    static constexpr double precision = 0.5;
    int32_t dstWidth = static_cast<int32_t>(previousResizeTarget_.Width() + precision);
    int32_t dstHeight = static_cast<int32_t>(previousResizeTarget_.Height() + precision);
    bool isTargetSource = ((dstWidth == image->width()) && (dstHeight == image->height()));
    if (!isTargetSource && (imageObj_->GetFrameCount() <= 1) && !background_) {
        LOGW("The size of returned image is not as expected, rejecting it. imageSrc: %{public}s,"
            "expected: [%{public}d x %{public}d], get [%{public}d x %{public}d]",
            imageObj_->GetSourceInfo().ToString().c_str(), dstWidth, dstHeight, image->width(), image->height());
        return;
    }
    UpdateLoadSuccessState();
    image_ = image;
    skiaDom_ = nullptr;
    svgDom_ = nullptr;
    if (imageDataNotReady_) {
        LOGI("Paint image is ready, imageSrc is %{private}s", imageObj_->GetSourceInfo().ToString().c_str());
        imageDataNotReady_ = false;
    }
    if (background_) {
        currentDstRectList_ = rectList_;
        if (imageUpdateFunc_) {
            imageUpdateFunc_();
        }
    }

    if (GetHidden() && imageObj_->GetFrameCount() > 1) {
        imageObj_->Pause();
    }
    if (imageObj_->GetFrameCount() == 1) {
        imageObj_->ClearData();
    }
    CacheImageObject();
}

void FlutterRenderImage::CacheImageObject()
{
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("pipeline context is null!");
        return;
    }
    auto imageCache = context->GetImageCache();
    if (imageCache && imageObj_->GetFrameCount() == 1) {
        imageCache->CacheImgObj(imageObj_->GetSourceInfo().ToString(), imageObj_);
    }
}

void FlutterRenderImage::UpdatePixmap(const RefPtr<PixelMap>& pixmap)
{
    LOGD("update pixmap");
    imageObj_ = MakeRefPtr<PixelMapImageObject>(pixmap);
    image_ = nullptr;
    curSourceInfo_.Reset();
    rawImageSize_ = imageObj_->GetImageSize();
    rawImageSizeUpdated_ = true;
    imageLoadingStatus_ = ImageLoadingStatus::LOAD_SUCCESS;
    MarkNeedLayout();
}

void FlutterRenderImage::Update(const RefPtr<Component>& component)
{
    RenderImage::Update(component);
    // curImageSrc represents the picture currently shown and imageSrc represents next picture to be shown
    imageLoadingStatus_ = (sourceInfo_ != curSourceInfo_) ? ImageLoadingStatus::UPDATING : imageLoadingStatus_;
    UpdateRenderAltImage(component);
    if (proceedPreviousLoading_ && !sourceInfo_.IsSvg() && sourceInfo_.GetSrcType() != SrcType::MEMORY) {
        LOGI("Proceed previous loading, imageSrc is %{private}s, image loading status: %{public}d",
            sourceInfo_.ToString().c_str(), imageLoadingStatus_);
        return;
    }
    if (sourceInfo_ != curSourceInfo_ && curSourceInfo_.IsValid()) {
        rawImageSize_ = Size();
    } else if (curSourceInfo_.IsValid()) {
        rawImageSize_ = formerRawImageSize_;
    }
    FetchImageObject();
}

void FlutterRenderImage::FetchImageObject()
{
    LOGD("fetch obj : %{public}s", sourceInfo_.ToString().c_str());
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("pipeline context is null!");
        return;
    }
    auto frontend = context->GetFrontend();
    if (!frontend) {
        LOGE("frontend is null!");
        return;
    }
    if (!sourceInfo_.IsValid()) {
        LOGW("Invalid image source. sourceInfo_ is %{private}s", sourceInfo_.ToString().c_str());
        if (context->GetIsDeclarative()) {
            ImageObjFailed();
        }
        return;
    }
    rawImageSizeUpdated_ = false;
    SrcType srcType = sourceInfo_.GetSrcType();
    switch (srcType) {
        case SrcType::PIXMAP: {
            UpdatePixmap(sourceInfo_.GetPixmap());
            break;
        }
        case SrcType::MEMORY: {
            UpdateSharedMemoryImage(context);
            break;
        }
        default: {
            bool syncMode = (context->IsBuildingFirstPage() &&
                             frontend->GetType() == FrontendType::JS_CARD &&
                             sourceInfo_.GetSrcType() != SrcType::NETWORK) || syncMode_;
            ImageProvider::FetchImageObject(
                sourceInfo_,
                imageObjSuccessCallback_,
                uploadSuccessCallback_,
                failedCallback_,
                GetContext(),
                syncMode,
                useSkiaSvg_,
                autoResize_,
                renderTaskHolder_,
                onPostBackgroundTask_);
            break;
        }
    }
}

void FlutterRenderImage::UpdateSharedMemoryImage(const RefPtr<PipelineContext>& context)
{
    auto sharedImageManager = context->GetSharedImageManager();
    if (!sharedImageManager) {
        LOGE("sharedImageManager is null when image try loading memory image, sourceInfo_: %{private}s",
            sourceInfo_.ToString().c_str());
        return;
    }
    auto nameOfSharedImage = ImageLoader::RemovePathHead(sourceInfo_.GetSrc());
    if (sharedImageManager->IsResourceToReload(nameOfSharedImage)) {
        // This case means that the image to load is a memory image and its data is not ready.
        // Add [this] to [providerMapToReload_] so that it will be notified to start loading image.
        // When the data is ready, [SharedImageManager] will call [UpdateData] in [AddImageData].
        sharedImageManager->AddProviderToReloadMap(nameOfSharedImage, AceType::WeakClaim(this));
        return;
    }
    // this is when current picName is not found in [ProviderMapToReload], indicating that image data of this
    // image may have been written to [SharedImageMap], so start loading
    if (sharedImageManager->FindImageInSharedImageMap(nameOfSharedImage, AceType::WeakClaim(this))) {
        return;
    }
}

void FlutterRenderImage::PerformLayoutPixmap()
{
    ProcessPixmapForPaint();
    MarkNeedRender();
}

void FlutterRenderImage::ProcessPixmapForPaint()
{
    LOGD("convert pixmap to skimage");
    auto pixmap = AceType::DynamicCast<PixelMapImageObject>(imageObj_)->GetPixmap();
    if (!pixmap) {
        LOGI("pixmap ref is null, may be released, paint directly");
        return;
    }
    // Step1: Create SkPixmap
    auto imageInfo = MakeSkImageInfoFromPixelMap(pixmap);
    SkPixmap imagePixmap(
        imageInfo, reinterpret_cast<const void*>(pixmap->GetPixels()), pixmap->GetRowBytes());

    // Step2: Create SkImage and draw it, using gpu or cpu
    sk_sp<SkImage> skImage;
    if (!renderTaskHolder_->ioManager) {
        skImage = SkImage::MakeFromRaster(imagePixmap, nullptr, nullptr);
    } else {
#ifndef GPU_DISABLED
        skImage = SkImage::MakeCrossContextFromPixmap(
            renderTaskHolder_->ioManager->GetResourceContext().get(),
            imagePixmap,
            true,
            imagePixmap.colorSpace(),
            true);
#endif
    }
    auto canvasImage = flutter::CanvasImage::Create();
    canvasImage->set_image(flutter::SkiaGPUObject<SkImage>(skImage, renderTaskHolder_->unrefQueue));
    image_ = canvasImage;
    if (!VerifySkImageDataFromPixmap(pixmap)) {
        LOGE("pixmap paint failed due to SkImage data verification fail. rawImageSize: %{public}s",
            rawImageSize_.ToString().c_str());
        imageLoadingStatus_ = ImageLoadingStatus::LOAD_FAIL;
        FireLoadEvent(Size());
        image_ = nullptr;
        imageObj_->ClearData();
        return;
    }
    FireLoadEvent(rawImageSize_);
    renderAltImage_ = nullptr;
}

void FlutterRenderImage::PerformLayoutSvgImage()
{
    if (loadSvgAfterLayout_) {
        if (!GetLayoutSize().IsEmpty()) {
            // if layout is empty, wait for next layout
            loadSvgAfterLayout_ = false;
        }

        auto context = context_.Upgrade();
        if (!context) {
            return;
        }
        context->GetTaskExecutor()->PostTask([weak = WeakClaim(this)] {
            auto image = weak.Upgrade();
            if (image) {
                image->PerformLayoutSvgCustom();
                image->MarkNeedRender();
            }
        }, TaskExecutor::TaskType::UI);
    }
}

void FlutterRenderImage::LayoutImageObject()
{
    if (imageObj_) {
        imageObj_->PerformLayoutImageObject(AceType::Claim(this));
    }
}

void FlutterRenderImage::PerformLayoutSvgCustom()
{
    if (imageObj_ && imageObj_->IsSvg()) {
        auto currentSvgDom = AceType::DynamicCast<SvgImageObject>(imageObj_)->GetSvgDom();
        if (currentSvgDom) {
            svgDom_ = currentSvgDom;
        } else {
            LOGD("svg dom is nullptr");
        }
    }
    if (svgDom_) {
        svgDom_->SetFinishEvent(svgAnimatorFinishEvent_);
        svgDom_->SetContainerSize(GetLayoutSize());
        SvgRadius svgRadius = { topLeftRadius_, topRightRadius_, bottomLeftRadius_, bottomRightRadius_ };
        svgDom_->CreateRenderNode(imageFit_, svgRadius, !directPaint_);
        if (svgDom_->GetRootRenderNode() && !directPaint_) {
            ClearChildren();
            AddChild(svgDom_->GetRootRenderNode());
        }
    }
}

void FlutterRenderImage::Paint(RenderContext& context, const Offset& offset)
{
    if (imageObj_ && imageObj_->IsSvg() && !useSkiaSvg_ && !directPaint_) {
        DrawSVGImageCustom(context, offset);
        return;
    }
    bool sourceDataEmpty = !image_ && !svgDom_ && !skiaDom_;
    if (renderAltImage_ && sourceDataEmpty) {
        renderAltImage_->SetDirectPaint(directPaint_);
        renderAltImage_->RenderWithContext(context, offset);
    }
    if (sourceInfo_.GetSrcType() != SrcType::PIXMAP) {
        UpLoadImageDataForPaint();
    }
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null, sourceInfo: %{private}s", sourceInfo_.ToString().c_str());
        return;
    }
    if (!NearZero(rotate_)) {
        Offset center =
            offset + Offset(GetLayoutSize().Width() * SK_ScalarHalf, GetLayoutSize().Height() * SK_ScalarHalf);
        SkCanvas* skCanvas = canvas.GetSkCanvas();
        if (skCanvas) {
            skCanvas->rotate(rotate_, center.GetX(), center.GetY());
        }
    }

    flutter::Paint paint;
    flutter::PaintData paint_data;
    if (opacity_ != UINT8_MAX) {
        paint.paint()->setAlpha(opacity_);
    }
    Rect paintRect = ((imageLoadingStatus_ == ImageLoadingStatus::LOADING) && !resizeCallLoadImage_)
                             ? currentDstRect_
                             : dstRect_;
    ApplyBorderRadius(offset, canvas, paintRect);
    if (imageLoadingStatus_ == ImageLoadingStatus::LOAD_FAIL) {
        if (renderAltImage_) {
            return;
        }
        auto pipelineContext = GetContext().Upgrade();
        if (pipelineContext && pipelineContext->GetIsDeclarative()) {
            // for declarative front end, do not draw anything for developers who use image component, we just inform
            // them via load fail event and the developers then decide how to react when image loads fail.
            return;
        }
        // to ensure forward compatibility, draw alt color only when it fails to load image
        paint.paint()->setColor(ALT_COLOR_GREY);
        canvas->drawRect(offset.GetX(), offset.GetY(), GetLayoutSize().Width() + offset.GetX(),
            GetLayoutSize().Height() + offset.GetY(), paint, paint_data);
        return;
    }
    if (sourceInfo_.IsSvg()) {
        if (loadSvgOnPaint_) {
            loadSvgOnPaint_ = false;
            // only loud svg render tree without box and will not bind to image node as child.
            PerformLayoutSvgCustom();
        }

        if (svgDom_) {
            canvas->translate(static_cast<float>(offset.GetX()), static_cast<float>(offset.GetY()));
            svgDom_->PaintDirectly(context, offset);
            return;
        }
        DrawSVGImage(offset, canvas);
        return;
    }
    ApplyColorFilter(paint);
    ApplyInterpolation(paint);
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
    if (image_) {
        colorSpace = image_->image()->refColorSpace();
    }
#ifdef USE_SYSTEM_SKIA
    paint.paint()->setColor4f(paint.paint()->getColor4f(), colorSpace.get());
#else
    paint.paint()->setColor(paint.paint()->getColor4f(), colorSpace.get());
#endif
    CanvasDrawImageRect(paint, offset, canvas, paintRect);
}

void FlutterRenderImage::ApplyBorderRadius(
    const Offset& offset, const ScopedCanvas& canvas, const Rect& paintRect)
{
    if (GetBackgroundImageFlag()) {
        return;
    }
    SetClipRadius();
    // There are three situations in which we apply border radius to the whole image component:
    // 1. when the image source is a SVG;
    // 2. when image loads fail;
    // 3. when there is a repeat to do;
    bool clipLayoutSize = sourceInfo_.IsSvg() || (imageRepeat_ != ImageRepeat::NOREPEAT) ||
        (imageLoadingStatus_ == ImageLoadingStatus::LOAD_FAIL);
    Rect clipRect = clipLayoutSize ? Rect(offset, GetLayoutSize()) : paintRect + offset;

    flutter::RRect rrect;
    rrect.sk_rrect.setRectRadii(
        SkRect::MakeXYWH(clipRect.Left() - imageRenderPosition_.GetX(), clipRect.Top() - imageRenderPosition_.GetY(),
            clipRect.Width(), clipRect.Height()),
        radii_);
    canvas->clipRRect(rrect, true);
}

void FlutterRenderImage::ApplyColorFilter(flutter::Paint& paint)
{
#ifdef USE_SYSTEM_SKIA
    if (imageRenderMode_ == ImageRenderMode::TEMPLATE) {
        paint.paint()->setColorFilter(SkColorFilter::MakeMatrixFilterRowMajor255(GRAY_COLOR_MATRIX));
        return;
    }
    if (!color_.has_value()) {
        return;
    }
    Color color = color_.value();
    paint.paint()->setColorFilter(SkColorFilter::MakeModeFilter(
        SkColorSetARGB(color.GetAlpha(), color.GetRed(), color.GetGreen(), color.GetBlue()), SkBlendMode::kPlus));
#else
    if (imageRenderMode_ == ImageRenderMode::TEMPLATE) {
        paint.paint()->setColorFilter(SkColorFilters::Matrix(GRAY_COLOR_MATRIX));
        return;
    }
    if (!color_.has_value()) {
        return;
    }
    Color color = color_.value();
    paint.paint()->setColorFilter(SkColorFilters::Blend(
        SkColorSetARGB(color.GetAlpha(), color.GetRed(), color.GetGreen(), color.GetBlue()), SkBlendMode::kPlus));
#endif
}

void FlutterRenderImage::ApplyInterpolation(flutter::Paint& paint)
{
    auto skFilterQuality = SkFilterQuality::kNone_SkFilterQuality;
    switch (imageInterpolation_) {
        case ImageInterpolation::LOW:
            skFilterQuality = SkFilterQuality::kLow_SkFilterQuality;
            break;
        case ImageInterpolation::MEDIUM:
            skFilterQuality = SkFilterQuality::kMedium_SkFilterQuality;
            break;
        case ImageInterpolation::HIGH:
            skFilterQuality = SkFilterQuality::kHigh_SkFilterQuality;
            break;
        case ImageInterpolation::NONE:
        default:
            break;
    }
    paint.paint()->setFilterQuality(skFilterQuality);
}

void FlutterRenderImage::CanvasDrawImageRect(
    const flutter::Paint& paint,
    const Offset& offset,
    const ScopedCanvas& canvas,
    const Rect& paintRect)
{
    if (GetBackgroundImageFlag()) {
        PaintBgImage(paint, offset, canvas);
        return;
    }
    if (!image_ || !image_->image()) {
        imageDataNotReady_ = true;
        LOGI("image data is not ready, rawImageSize_: %{public}s, image source: %{private}s",
            rawImageSize_.ToString().c_str(), sourceInfo_.ToString().c_str());
        return;
    }
    bool isLoading = ((imageLoadingStatus_ == ImageLoadingStatus::LOADING) ||
                      (imageLoadingStatus_ == ImageLoadingStatus::UPDATING));
    Rect scaledSrcRect = isLoading ? currentSrcRect_ : srcRect_;
    if (sourceInfo_.IsValid() &&
        imageObj_ && (imageObj_->GetFrameCount() == 1) &&
        sourceInfo_.GetSrcType() != SrcType::PIXMAP) {
        Size sourceSize = (image_ ? Size(image_->width(), image_->height()) : Size());
        // calculate srcRect that matches the real image source size
        // note that gif doesn't do resize, so gif does not need to recalculate
        scaledSrcRect = RecalculateSrcRect(sourceSize);
        scaledSrcRect.ApplyScaleAndRound(currentResizeScale_);
    }
    Rect realDstRect = paintRect + offset;
    if (!scaledSrcRect.IsValid() || !realDstRect.IsValid()) {
        if (imageLoadingStatus_ == ImageLoadingStatus::LOAD_SUCCESS) {
            LOGW("image %{private}s src rect or dst rect is not valid.", sourceInfo_.ToString().c_str());
            LOGW("src rect is %{public}s", scaledSrcRect.ToString().c_str());
            LOGW("dst rect is %{public}s", realDstRect.ToString().c_str());
        }
        return;
    }
    if (imageRepeat_ != ImageRepeat::NOREPEAT) {
        DrawImageOnCanvas(scaledSrcRect, realDstRect, canvas, paint, imageRenderPosition_);
        return;
    }
    auto skSrcRect = SkRect::MakeXYWH(
        scaledSrcRect.Left(), scaledSrcRect.Top(), scaledSrcRect.Width(), scaledSrcRect.Height());
    auto skDstRect =
        SkRect::MakeXYWH(realDstRect.Left() - imageRenderPosition_.GetX(),
                         realDstRect.Top() - imageRenderPosition_.GetY(),
                         realDstRect.Width(), realDstRect.Height());
    canvas->canvas()->drawImageRect(image_->image(), skSrcRect, skDstRect, paint.paint());
    LOGD("dstRect params: %{public}s", realDstRect.ToString().c_str());
    LOGD("scaledSrcRect params: %{public}s", scaledSrcRect.ToString().c_str());
}

void FlutterRenderImage::DrawImageOnCanvas(
    const Rect& srcRect,
    const Rect& dstRect,
    const ScopedCanvas& canvas,
    const flutter::Paint& paint,
    const Offset& dstOffset) const
{
    auto skSrcRect = SkIRect::MakeXYWH(
        Round(srcRect.Left()), Round(srcRect.Top()), Round(srcRect.Width()), Round(srcRect.Height()));
    auto skDstRect = SkRect::MakeXYWH(
        dstRect.Left() - dstOffset.GetX(),
        dstRect.Top() - dstOffset.GetY(),
        dstRect.Width(), dstRect.Height());

    // initialize a transform matrix
    SkScalar scaleX =  skDstRect.width() / skSrcRect.width();
    SkScalar skewX = 0;
    SkScalar transX = skDstRect.left();
    SkScalar skewY = 0;
    SkScalar scaleY = skDstRect.height() / skSrcRect.height();
    SkScalar transY = skDstRect.top();
    SkScalar pers0 = 0;
    SkScalar pers1 = 0;
    SkScalar pers2 = 1;

    if (matchTextDirection_ && GetTextDirection() == TextDirection::RTL) {
        // flip the image algin x direction.
        scaleX = -1 * scaleX;
        transX = skDstRect.left() + skDstRect.width();
    }
    auto sampleMatrix = SkMatrix::MakeAll(
        scaleX, skewX, transX,
        skewY, scaleY, transY,
        pers0, pers1, pers2);
#if defined(USE_SYSTEM_SKIA)
    SkShader::TileMode xTileMode = SkShader::TileMode::kDecal_TileMode;
    SkShader::TileMode yTileMode = SkShader::TileMode::kDecal_TileMode;
    switch (imageRepeat_) {
        case ImageRepeat::REPEAT:
            xTileMode = SkShader::TileMode::kRepeat_TileMode;
            yTileMode = SkShader::TileMode::kRepeat_TileMode;
            break;
        case ImageRepeat::REPEATX:
            xTileMode = SkShader::TileMode::kRepeat_TileMode;
            break;
        case ImageRepeat::REPEATY:
            yTileMode = SkShader::TileMode::kRepeat_TileMode;
            break;
        default:
            break;
    }
#else
    SkTileMode xTileMode = SkTileMode::kDecal;
    SkTileMode yTileMode = SkTileMode::kDecal;
    switch (imageRepeat_) {
        case ImageRepeat::REPEAT:
            xTileMode = SkTileMode::kRepeat;
            yTileMode = SkTileMode::kRepeat;
            break;
        case ImageRepeat::REPEATX:
            xTileMode = SkTileMode::kRepeat;
            break;
        case ImageRepeat::REPEATY:
            yTileMode = SkTileMode::kRepeat;
            break;
        default:
            break;
    }
#endif
    auto imgShader = (skSrcRect.width() < image_->width() || skSrcRect.height() < image_->height()) ?
                        image_->image()->makeSubset(skSrcRect)->makeShader(xTileMode, yTileMode, &sampleMatrix) :
                        image_->image()->makeShader(xTileMode, yTileMode, &sampleMatrix);
    SkPaint skPaint = *paint.paint();
    skPaint.setShader(imgShader);
    canvas->canvas()->drawPaint(skPaint);
}

bool FlutterRenderImage::VerifySkImageDataFromPixmap(const RefPtr<PixelMap>& pixmap) const
{
    if (!image_ || !image_->image()) {
        LOGE("image data made from pixmap is null");
        return false;
    }
    if ((image_->width() <= 0 || image_->height() <= 0)) {
        LOGE("image data made from pixmap is invalid, image data size: [%{public}d x %{public}d], pixmap size:"
             " [%{public}d x %{public}d]",
            image_->width(), image_->height(), pixmap->GetWidth(), pixmap->GetHeight());
        return false;
    }
    return true;
}

Rect FlutterRenderImage::RecalculateSrcRect(const Size& realImageSize)
{
    if (!currentResizeScale_.IsValid() || scale_ <= 0.0) {
        return Rect();
    }
    auto realSrcSize = Size(
        realImageSize.Width() / currentResizeScale_.Width(), realImageSize.Height() / currentResizeScale_.Height());
    Rect realSrcRect(Offset(), realSrcSize * (1.0 / scale_));
    Rect realDstRect(Offset(), GetLayoutSize());
    ApplyImageFit(realSrcRect, realDstRect);
    realSrcRect.ApplyScale(scale_);
    return realSrcRect;
}

void FlutterRenderImage::PaintBgImage(
    const flutter::Paint& paint, const Offset& offset, const ScopedCanvas& canvas) const
{
    if (!GetBackgroundImageFlag()) {
        return;
    }
    if (currentDstRectList_.empty() || !image_) {
        return;
    }

    for (auto rect : currentDstRectList_) {
        auto realDstRect = rect + offset + boxMarginOffset_;
        auto skSrcRect = SkRect::MakeXYWH(0.0, 0.0, image_->width(), image_->height());
        auto skDstRect =
            SkRect::MakeXYWH(realDstRect.Left(), realDstRect.Top(), realDstRect.Width(), realDstRect.Height());
        canvas->canvas()->drawImageRect(image_->image(), skSrcRect, skDstRect, paint.paint());
    }
}

bool FlutterRenderImage::NeedUploadImageObjToGpu()
{
    bool sourceChange = sourceInfo_ != curSourceInfo_;
    bool newSourceCallLoadImage = (sourceChange && rawImageSize_.IsValid() && srcRect_.IsValid() &&
                                   (rawImageSizeUpdated_ && imageLoadingStatus_ != ImageLoadingStatus::LOADING) &&
                                   imageLoadingStatus_ != ImageLoadingStatus::LOAD_FAIL);
    if (imageLoadingStatus_ != ImageLoadingStatus::LOADING) {
        resizeCallLoadImage_ =
            !sourceChange && NeedResize() && (imageLoadingStatus_ == ImageLoadingStatus::LOAD_SUCCESS);
    }
    return (newSourceCallLoadImage && (background_ || resizeTarget_.IsValid())) ||
           (resizeCallLoadImage_ && autoResize_);
}

void FlutterRenderImage::UpLoadImageDataForPaint()
{
    if (NeedUploadImageObjToGpu()) {
        imageLoadingStatus_ = ImageLoadingStatus::LOADING;
        if (imageObj_) {
            previousResizeTarget_ = resizeTarget_;
            FlutterRenderImage::UploadImageObjToGpuForRender(imageObj_, GetContext(), renderTaskHolder_,
                uploadSuccessCallback_, failedCallback_, resizeTarget_, forceResize_, syncMode_);
        }
    }
}

void FlutterRenderImage::UploadImageObjToGpuForRender(
    const RefPtr<ImageObject>& imageObj,
    const WeakPtr<PipelineContext> context,
    RefPtr<FlutterRenderTaskHolder>& renderTaskHolder,
    UploadSuccessCallback uploadSuccessCallback,
    FailedCallback failedCallback,
    Size resizeTarget,
    bool forceResize,
    bool syncMode)
{
    if (!imageObj) {
        LOGW("image object is null when try UploadImageObjToGpuForRender.");
        return;
    }
    imageObj->UploadToGpuForRender(
        context, renderTaskHolder, uploadSuccessCallback, failedCallback, resizeTarget, forceResize, syncMode);
}

void FlutterRenderImage::UpdateData(const std::string& uri, const std::vector<uint8_t>& memData)
{
    if (uri != sourceInfo_.GetSrc()) {
        return;
    }
    auto skData = SkData::MakeWithCopy(memData.data(), memData.size());
    if (!skData) {
        LOGE("memory data is null. update data failed. uri: %{private}s", uri.c_str());
        return;
    }
    if (sourceInfo_.IsSvg()) {
        PaintSVGImage(skData, true);
        return;
    }
    auto codec = SkCodec::MakeFromData(skData);
    if (!codec) {
        LOGE("decode image failed, update memory data failed. uri: %{private}s", uri.c_str());
        return;
    }

    auto context = GetContext().Upgrade();
    if (!context) {
        return;
    }
    auto ImageObj =
        ImageObject::BuildImageObject(sourceInfo_, context, skData, useSkiaSvg_);
    ImageObjReady(ImageObj);
}

void FlutterRenderImage::SetClipRadius()
{
    SetSkRadii(topLeftRadius_, radii_[SkRRect::kUpperLeft_Corner]);
    SetSkRadii(topRightRadius_, radii_[SkRRect::kUpperRight_Corner]);
    SetSkRadii(bottomLeftRadius_, radii_[SkRRect::kLowerLeft_Corner]);
    SetSkRadii(bottomRightRadius_, radii_[SkRRect::kLowerRight_Corner]);
}

void FlutterRenderImage::SetSkRadii(const Radius& radius, SkVector& radii)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    double dipScale = context->GetDipScale();
    radii.set(SkDoubleToScalar(std::max(radius.GetX().ConvertToPx(dipScale), 0.0)),
        SkDoubleToScalar(std::max(radius.GetY().ConvertToPx(dipScale), 0.0)));
}

Size FlutterRenderImage::MeasureForPixmap()
{
    return rawImageSize_;
}

Size FlutterRenderImage::MeasureForSvgImage()
{
    return imageComponentSize_;
}

Size FlutterRenderImage::MeasureForNormalImage()
{
    switch (imageLoadingStatus_) {
        case ImageLoadingStatus::LOAD_SUCCESS:
        case ImageLoadingStatus::LOADING:
        case ImageLoadingStatus::UNLOADED:
        case ImageLoadingStatus::LOAD_FAIL:
            return rawImageSize_;
        case ImageLoadingStatus::UPDATING:
            if (rawImageSizeUpdated_) {
                return rawImageSize_;
            }
            return formerRawImageSize_;
        default:
            return Size();
    }
}

Size FlutterRenderImage::Measure()
{
    if (imageObj_) {
        return imageObj_->MeasureForImage(AceType::Claim(this));
    }
    LOGI("empty image object, measure failed, return empty size.");
    return Size();
}

void FlutterRenderImage::OnHiddenChanged(bool hidden)
{
    LOGD("On hide changed.");
    if (hidden) {
        if (imageObj_ && imageObj_->GetFrameCount() > 1) {
            LOGI("Animated image Pause");
            imageObj_->Pause();
        } else {
            CancelBackgroundTasks();
        }
    } else {
        if (imageObj_ && imageObj_->GetFrameCount() > 1) {
            LOGI("Animated image Resume");
            imageObj_->Resume();
        } else if (backgroundTaskCancled_) {
            backgroundTaskCancled_ = false;
            FetchImageObject();
        }
    }
}

void FlutterRenderImage::CancelBackgroundTasks()
{
    if (fetchImageObjTask_) {
        backgroundTaskCancled_ = fetchImageObjTask_.Cancel(false);
    }
    if (imageObj_) {
        backgroundTaskCancled_ = imageObj_->CancelBackgroundTasks();
    }
}

void FlutterRenderImage::PaintSVGImage(const sk_sp<SkData>& skData, bool onlyLayoutSelf)
{
    imageLoadingStatus_ = ImageLoadingStatus::LOADING;
    auto successCallback = [svgImageWeak = AceType::WeakClaim(this), onlyLayoutSelf](const sk_sp<SkSVGDOM>& svgDom) {
        auto svgImage = svgImageWeak.Upgrade();
        if (!svgImage) {
            return;
        }
        svgImage->skiaDom_ = svgDom;
        svgImage->image_ = nullptr;
        svgImage->imageSizeForEvent_ = svgImage->Measure();
        svgImage->UpdateLoadSuccessState();
        svgImage->MarkNeedLayout(onlyLayoutSelf);
    };
    auto failedCallback = [svgImageWeak = AceType::WeakClaim(this)]() {
        auto svgImage = svgImageWeak.Upgrade();
        if (svgImage) {
            LOGE("svg data wrong: %{private}s", svgImage->sourceInfo_.ToString().c_str());
            svgImage->ImageObjFailed(); // if Upgrade fail, just callback with nullptr
        }
    };
    SkColorEx skColor;
    auto fillColor = sourceInfo_.GetFillColor();
    if (fillColor.has_value()) {
        skColor.color = fillColor.value().GetValue();
        skColor.valid = 1;
    }
    ImageProvider::GetSVGImageDOMAsyncFromData(
        skData,
        successCallback,
        failedCallback,
        GetContext(),
        skColor.value,
        onPostBackgroundTask_);
    MarkNeedLayout();
}

void FlutterRenderImage::DrawSVGImage(const Offset& offset, ScopedCanvas& canvas)
{
    if (!skiaDom_) {
        return;
    }
    Size svgContainerSize = GetLayoutSize();
    if (svgContainerSize.IsInfinite() || !svgContainerSize.IsValid()) {
        // when layout size is invalid, try the container size of svg
        svgContainerSize = Size(skiaDom_->containerSize().width(), skiaDom_->containerSize().height());
        if (svgContainerSize.IsInfinite() || !svgContainerSize.IsValid()) {
            LOGE("Invalid layout size: %{private}s, invalid svgContainerSize: %{private}s, stop draw svg. The max size"
                 " of layout param is %{private}s", GetLayoutSize().ToString().c_str(),
                 svgContainerSize.ToString().c_str(), GetLayoutParam().GetMaxSize().ToString().c_str());
            return;
        } else {
            LOGE("Invalid layout size: %{private}s, valid svgContainerSize: %{private}s, use svg container size to draw"
                 " svg. The max size of layout param is %{private}s", GetLayoutSize().ToString().c_str(),
                 svgContainerSize.ToString().c_str(), GetLayoutParam().GetMaxSize().ToString().c_str());
        }
    }
    canvas->translate(static_cast<float>(offset.GetX()), static_cast<float>(offset.GetY()));
    double width = svgContainerSize.Width();
    double height = svgContainerSize.Height();
    if (matchTextDirection_ && GetTextDirection() == TextDirection::RTL) {
        canvas.FlipHorizontal(0.0, width);
    }
    skiaDom_->setContainerSize({ width, height });
    canvas->clipRect(0, 0, width, height, SkClipOp::kIntersect);
    skiaDom_->render(canvas.GetSkCanvas());
}

void FlutterRenderImage::DrawSVGImageCustom(RenderContext& context, const Offset& offset)
{
    if (svgDom_ && svgDom_->GetRootRenderNode()) {
        svgDom_->GetRootRenderNode()->RenderWithContext(context, offset);
    }
}

void FlutterRenderImage::UpdateLoadSuccessState()
{
    LOGD("update success state info: %{public}s", sourceInfo_.ToString().c_str());
    imageLoadingStatus_ = ImageLoadingStatus::LOAD_SUCCESS;
    auto currentFrameCount = imageObj_->GetFrameCount();
    if ((!sourceInfo_.IsSvg() && currentFrameCount == 1) ||
        (currentFrameCount > 1 && curSourceInfo_ != sourceInfo_)) {
        FireLoadEvent(imageSizeForEvent_);
    }
    if (currentFrameCount > 1 && curSourceInfo_ != sourceInfo_) {
        auto parent = GetParent().Upgrade();
        if (parent) {
            parent->MarkNeedRender();
        }
    }
    if (currentFrameCount != frameCount_) {
        frameCount_ = currentFrameCount;
    }

    currentSrcRect_ = srcRect_;
    curSourceInfo_ = sourceInfo_;
    formerRawImageSize_ = rawImageSize_;
    forceResize_ = false;
    retryCnt_ = 0;
    currentResizeScale_ = resizeScale_;
    if (renderAltImage_) {
        RemoveChild(renderAltImage_);
        renderAltImage_ = nullptr;
        MarkNeedLayout();
        return;
    }
    proceedPreviousLoading_ = false;
    rawImageSizeUpdated_ = false;
    MarkNeedRender();
}

void FlutterRenderImage::UpdateRenderAltImage(const RefPtr<Component>& component)
{
    const RefPtr<ImageComponent> image = AceType::DynamicCast<ImageComponent>(component);
    if (!image) {
        LOGE("image component is null when try update alt image, sourceInfo_: %{private}s",
            sourceInfo_.ToString().c_str());
        return;
    }
    bool imageAltValid = !imageAlt_.empty() && (imageAlt_ != IMAGE_ALT_BLANK);
    if (!imageAltValid || imageLoadingStatus_ == ImageLoadingStatus::LOAD_SUCCESS) {
        return;
    }
    // note that alt image component must be a copy
    RefPtr<ImageComponent> altImageComponent = ImageComponent::MakeFromOtherWithoutSourceAndEvent(image);
    altImageComponent->SetSrc(imageAlt_);
    altImageComponent->SetAlt("");
    if (renderAltImage_) {
        renderAltImage_->Update(altImageComponent);
        return;
    }
    renderAltImage_ = AceType::DynamicCast<RenderImage>(altImageComponent->CreateRenderNode());
    renderAltImage_->Attach(GetContext());
    renderAltImage_->Update(altImageComponent);
    AddChild(renderAltImage_);
}

bool FlutterRenderImage::MaybeRelease()
{
    auto context = GetContext().Upgrade();
    if (context && context->GetRenderFactory() && context->GetRenderFactory()->GetRenderImageFactory()->Recycle(this)) {
        ClearRenderObject();
        return false;
    }
    return true;
}

void FlutterRenderImage::ClearRenderObject()
{
    LOGD("Clear obj %{public}s", curSourceInfo_.ToString().c_str());
    RenderImage::ClearRenderObject();
    CancelBackgroundTasks();
    curSourceInfo_.Reset();
    image_ = nullptr;
    layer_ = nullptr;
    formerRawImageSize_ = { 0.0, 0.0 };
    imageObj_ = nullptr;
    skiaDom_ = nullptr;
    svgDom_ = nullptr;
}

bool FlutterRenderImage::IsSourceWideGamut() const
{
    if (sourceInfo_.IsSvg() || !image_ || !image_->image()) {
        return false;
    }
    return ImageProvider::IsWideGamut(image_->image()->refColorSpace());
}

bool FlutterRenderImage::RetryLoading()
{
    if (!sourceInfo_.IsValid()) {
        LOGW("sourceInfo is invalid, no need retry loading. sourceInfo: %{private}s. retry loading time: %{public}d",
            sourceInfo_.ToString().c_str(), retryCnt_);
        return false;
    }
    if (retryCnt_++ > 5) { // retry loading 5 times at most
        LOGW("Retry time has reached 5, stop retry loading, please check fail reason. imageSrc: %{private}s",
            sourceInfo_.ToString().c_str());
        return false;
    }

    if (rawImageSizeUpdated_ && imageObj_) { // case when image size is ready, only have to do loading again
        imageObj_->UploadToGpuForRender(
            GetContext(), renderTaskHolder_, uploadSuccessCallback_, failedCallback_, resizeTarget_, forceResize_);
        LOGW("Retry loading time: %{public}d, trigger by LoadImage fail, imageSrc: %{private}s", retryCnt_,
            sourceInfo_.ToString().c_str());
        return true;
    }
    // case when the fail event is triggered by GetImageSize, do GetImageSize again
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("pipeline context is null while trying to get image size again. imageSrc: %{private}s",
            sourceInfo_.ToString().c_str());
        return false;
    }
    auto frontend = context->GetFrontend();
    if (!frontend) {
        LOGE("frontend is null while trying to get image size again. imageSrc: %{private}s",
            sourceInfo_.ToString().c_str());
        return false;
    }
    bool syncMode = context->IsBuildingFirstPage() &&
                    frontend->GetType() == FrontendType::JS_CARD &&
                    sourceInfo_.GetSrcType() != SrcType::NETWORK;
    ImageProvider::FetchImageObject(
        sourceInfo_,
        imageObjSuccessCallback_,
        uploadSuccessCallback_,
        failedCallback_,
        GetContext(),
        syncMode,
        useSkiaSvg_,
        autoResize_,
        renderTaskHolder_,
        onPostBackgroundTask_);
    LOGW("Retry loading time: %{public}d, triggered by GetImageSize fail, imageSrc: %{private}s", retryCnt_,
        sourceInfo_.ToString().c_str());
    return true;
}

SkImageInfo FlutterRenderImage::MakeSkImageInfoFromPixelMap(const RefPtr<PixelMap>& pixmap)
{
    SkColorType ct = PixelFormatToSkColorType(pixmap);
    SkAlphaType at = AlphaTypeToSkAlphaType(pixmap);
    sk_sp<SkColorSpace> cs = ColorSpaceToSkColorSpace(pixmap);
    return SkImageInfo::Make(pixmap->GetWidth(), pixmap->GetHeight(), ct, at, cs);
}

sk_sp<SkColorSpace> FlutterRenderImage::ColorSpaceToSkColorSpace(const RefPtr<PixelMap>& pixmap)
{
    return SkColorSpace::MakeSRGB(); // Media::PixelMap has not support wide gamut yet.
}

SkAlphaType FlutterRenderImage::AlphaTypeToSkAlphaType(const RefPtr<PixelMap>& pixmap)
{
    switch (pixmap->GetAlphaType()) {
        case AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return SkAlphaType::kUnknown_SkAlphaType;
        case AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return SkAlphaType::kOpaque_SkAlphaType;
        case AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return SkAlphaType::kPremul_SkAlphaType;
        case AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return SkAlphaType::kUnpremul_SkAlphaType;
        default:
            return SkAlphaType::kUnknown_SkAlphaType;
    }
}

SkColorType FlutterRenderImage::PixelFormatToSkColorType(const RefPtr<PixelMap>& pixmap)
{
    switch (pixmap->GetPixelFormat()) {
        case PixelFormat::RGB_565:
            return SkColorType::kRGB_565_SkColorType;
        case PixelFormat::RGBA_8888:
            return SkColorType::kRGBA_8888_SkColorType;
        case PixelFormat::BGRA_8888:
            return SkColorType::kBGRA_8888_SkColorType;
        case PixelFormat::ALPHA_8:
            return SkColorType::kAlpha_8_SkColorType;
        case PixelFormat::RGBA_F16:
            return SkColorType::kRGBA_F16_SkColorType;
        case PixelFormat::UNKNOWN:
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGB_888:
        case PixelFormat::NV21:
        case PixelFormat::NV12:
        case PixelFormat::CMYK:
        default:
            return SkColorType::kUnknown_SkColorType;
    }
}

void FlutterRenderImage::OnAppHide()
{
    isAppOnShow_ = false;
    if (imageObj_) {
        imageObj_->Pause();
    }
}

void FlutterRenderImage::OnAppShow()
{
    isAppOnShow_ = true;
    if (imageObj_) {
        imageObj_->Resume();
    }
}

void FlutterRenderImage::OnVisibleChanged()
{
    if (imageObj_ && GetVisible()) {
        imageObj_->Resume();
    } else if (imageObj_ && !GetVisible()) {
        imageObj_->Pause();
    }
}

} // namespace OHOS::Ace
