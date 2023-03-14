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

#include "core/components/image/rosen_render_image.h"

#include "flutter/common/task_runners.h"
#include "render_service_client/core/ui/rs_node.h"
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
#include "core/pipeline/base/rosen_render_context.h"

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
} // namespace

union SkColorEx {
    struct {
        SkColor color : 32;
        bool valid : 1;
        uint32_t reserved : 31; // reserved
    };
    uint64_t value = 0;
};

RosenRenderImage::RosenRenderImage()
{
    auto currentDartState = flutter::UIDartState::Current();
    if (!currentDartState) {
        return;
    }

    renderTaskHolder_ = MakeRefPtr<FlutterRenderTaskHolder>(currentDartState->GetSkiaUnrefQueue(),
        currentDartState->GetIOManager(), currentDartState->GetTaskRunners().GetIOTaskRunner());
    InitializeCallbacks();
}

RosenRenderImage::~RosenRenderImage()
{
    CancelBackgroundTasks();
}

void RosenRenderImage::InitializeCallbacks()
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

    uploadSuccessCallback_ = [weak = AceType::WeakClaim(this)](
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

    onPostBackgroundTask_ = [weak = AceType::WeakClaim(this)](CancelableTask task) {
        auto renderImage = weak.Upgrade();
        if (renderImage) {
            renderImage->SetFetchImageObjBackgroundTask(task);
        }
    };
}

void RosenRenderImage::ImageObjReady(const RefPtr<ImageObject>& imageObj)
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
    bool selfOnly = (imageComponentSize_.IsValid() && !imageComponentSize_.IsInfinite() && layoutSizeNotChanged) ||
        imageObj_->IsSvg();
    MarkNeedLayout(selfOnly);
}

void RosenRenderImage::ImageObjFailed()
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

void RosenRenderImage::ImageDataPaintSuccess(const fml::RefPtr<flutter::CanvasImage>& image)
{
    LOGD("image data paint success.");
    static constexpr double precision = 0.5;
    int32_t dstWidth = static_cast<int32_t>(previousResizeTarget_.Width() + precision);
    int32_t dstHeight = static_cast<int32_t>(previousResizeTarget_.Height() + precision);
    bool isTargetSource = ((dstWidth == image->width()) && (dstHeight == image->height()));
    if (!isTargetSource && (imageObj_->GetFrameCount() <= 1) && !background_) {
        LOGW("The size of returned image is not as expected, rejecting it. imageSrc: %{private}s,"
             "expected: [%{private}d x %{private}d], get [%{private}d x %{private}d]",
            imageObj_->GetSourceInfo().ToString().c_str(), dstWidth, dstHeight, image->width(), image->height());
        return;
    }
    UpdateLoadSuccessState();
    image_ = image->image();
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
    if (imageObj_->GetFrameCount() == 1 && imageObj_->GetSourceInfo().GetSrcType() != SrcType::MEMORY) {
        imageObj_->ClearData();
    }
    CacheImageObject();
}

void RosenRenderImage::CacheImageObject()
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

void RosenRenderImage::UpdatePixmap(const RefPtr<PixelMap>& pixmap)
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

void RosenRenderImage::Update(const RefPtr<Component>& component)
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

void RosenRenderImage::FetchImageObject()
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

void RosenRenderImage::UpdateSharedMemoryImage(const RefPtr<PipelineContext>& context)
{
    auto sharedImageManager = context->GetSharedImageManager();
    if (sharedImageManager) {
        if (sharedImageManager->IsResourceToReload(ImageLoader::RemovePathHead(sourceInfo_.GetSrc()))) {
            // This case means that the imageSrc to load is a memory image and its data is not ready.
            // If run [GetImageSize] here, there will be an unexpected [OnLoadFail] callback from [ImageProvider].
            // When the data is ready, [SharedImageManager] done [AddImageData], [GetImageSize] will be run.
            return;
        }
        auto nameOfSharedImage = ImageLoader::RemovePathHead(sourceInfo_.GetSrc());
        if (sharedImageManager->AddProviderToReloadMap(nameOfSharedImage, AceType::WeakClaim(this))) {
            return;
        }
        // this is when current picName is not found in [ProviderMapToReload], indicating that image data of this
        // image may have been written to [SharedImageMap], so return the [MemoryImageProvider] and start loading
        if (sharedImageManager->FindImageInSharedImageMap(nameOfSharedImage, AceType::WeakClaim(this))) {
            return;
        }
    }
}

void RosenRenderImage::PerformLayoutPixmap()
{
    ProcessPixmapForPaint();
    MarkNeedRender();
}
void RosenRenderImage::ProcessPixmapForPaint()
{
    LOGD("pixmap begin paint");
    auto pixmap = AceType::DynamicCast<PixelMapImageObject>(imageObj_)->GetPixmap();
    if (!pixmap) {
        LOGI("pixmap ref is null, may be released, paint directly");
        return;
    }
    // Step1: Create SkPixmap
    auto imageInfo = MakeSkImageInfoFromPixelMap(pixmap);
    SkPixmap imagePixmap(imageInfo, reinterpret_cast<const void*>(pixmap->GetPixels()), pixmap->GetRowBytes());

    // Step2: Create SkImage and draw it, using gpu or cpu
    sk_sp<SkImage> skImage;
    if (!renderTaskHolder_->ioManager) {
        skImage = SkImage::MakeFromRaster(imagePixmap, nullptr, nullptr);
    } else {
#ifndef GPU_DISABLED
        skImage = SkImage::MakeCrossContextFromPixmap(renderTaskHolder_->ioManager->GetResourceContext().get(),
            imagePixmap, true, imagePixmap.colorSpace(), true);
#else
        skImage = SkImage::MakeFromRaster(imagePixmap, nullptr, nullptr);
#endif
    }
    image_ = std::move(skImage);
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

void RosenRenderImage::PerformLayoutSvgImage()
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
        context->GetTaskExecutor()->PostTask(
            [weak = WeakClaim(this)] {
                auto image = weak.Upgrade();
                if (image) {
                    image->PerformLayoutSvgCustom();
                    image->MarkNeedRender();
                }
            },
            TaskExecutor::TaskType::UI);
    }
}

void RosenRenderImage::LayoutImageObject()
{
    if (imageObj_) {
        imageObj_->PerformLayoutImageObject(AceType::Claim(this));
    }
}

void RosenRenderImage::PerformLayoutSvgCustom()
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

void RosenRenderImage::Paint(RenderContext& context, const Offset& offset)
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
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    SkAutoCanvasRestore acr(canvas, true);
    if (!NearZero(rotate_)) {
        Offset center =
            offset + Offset(GetLayoutSize().Width() * SK_ScalarHalf, GetLayoutSize().Height() * SK_ScalarHalf);
        if (canvas) {
            canvas->rotate(rotate_, center.GetX(), center.GetY());
        }
    }

    SkPaint paint;
    Rect paintRect = ((imageLoadingStatus_ == ImageLoadingStatus::LOADING) && !resizeCallLoadImage_)
                             ? currentDstRect_
                             : dstRect_;
    ApplyBorderRadius(offset, paintRect, canvas);
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
        paint.setColor(ALT_COLOR_GREY);
#ifdef OHOS_PLATFORM
        auto recordingCanvas = static_cast<Rosen::RSRecordingCanvas*>(canvas);
        if (GetBackgroundImageFlag()) {
            recordingCanvas->drawRect({ offset.GetX(), offset.GetY(), GetLayoutSize().Width() + offset.GetX(),
                                          GetLayoutSize().Height() + offset.GetY() },
                paint);
        } else {
            recordingCanvas->DrawAdaptiveRRect(0, paint);
        }
#else
        canvas->drawRect({ offset.GetX(), offset.GetY(), GetLayoutSize().Width() + offset.GetX(),
                             GetLayoutSize().Height() + offset.GetY() },
            paint);
#endif
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
        colorSpace = image_->refColorSpace();
    }
#ifdef USE_SYSTEM_SKIA
    paint.setColor4f(paint.getColor4f(), colorSpace.get());
#else
    paint.setColor(paint.getColor4f(), colorSpace.get());
#endif
    if (GetBackgroundImageFlag()) {
        PaintBgImage(static_cast<RosenRenderContext*>(&context)->GetRSNode());
        return;
    }
    CanvasDrawImageRect(paint, offset, canvas, paintRect);
}

void RosenRenderImage::ApplyBorderRadius(
    const Offset& offset, const Rect& paintRect, SkCanvas* canvas)
{
    if (GetBackgroundImageFlag()) {
        return;
    }
    SetClipRadius();

#ifdef OHOS_PLATFORM
    auto recordingCanvas = static_cast<Rosen::RSRecordingCanvas*>(canvas);
    recordingCanvas->ClipAdaptiveRRect(radii_[0].x());
#else
    // There are three situations in which we apply border radius to the whole image component:
    // 1. when the image source is a SVG;
    // 2. when image loads fail;
    // 3. when there is a repeat to do;
    bool clipLayoutSize = sourceInfo_.IsSvg() || (imageRepeat_ != ImageRepeat::NOREPEAT) ||
        (imageLoadingStatus_ == ImageLoadingStatus::LOAD_FAIL);
    Rect clipRect = clipLayoutSize ? Rect(offset, GetLayoutSize()) : paintRect + offset;

    SkRRect rrect;
    rrect.setRectRadii(
        SkRect::MakeXYWH(clipRect.Left() - imageRenderPosition_.GetX(), clipRect.Top() - imageRenderPosition_.GetY(),
            clipRect.Width(), clipRect.Height()),
        radii_);
    canvas->clipRRect(rrect, true);
#endif
}

void RosenRenderImage::ApplyColorFilter(SkPaint& paint)
{
#ifdef USE_SYSTEM_SKIA
    if (imageRenderMode_ == ImageRenderMode::TEMPLATE) {
        paint.setColorFilter(SkColorFilter::MakeMatrixFilterRowMajor255(GRAY_COLOR_MATRIX));
        return;
    }
    if (!color_.has_value()) {
        return;
    }
    Color color = color_.value();
    paint.setColorFilter(SkColorFilter::MakeModeFilter(
        SkColorSetARGB(color.GetAlpha(), color.GetRed(), color.GetGreen(), color.GetBlue()), SkBlendMode::kPlus));
#else
    if (imageRenderMode_ == ImageRenderMode::TEMPLATE) {
        paint.setColorFilter(SkColorFilters::Matrix(GRAY_COLOR_MATRIX));
        return;
    }
    if (!color_.has_value()) {
        return;
    }
    Color color = color_.value();
    paint.setColorFilter(SkColorFilters::Blend(
        SkColorSetARGB(color.GetAlpha(), color.GetRed(), color.GetGreen(), color.GetBlue()), SkBlendMode::kPlus));
#endif
}

void RosenRenderImage::ApplyInterpolation(SkPaint& paint)
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
    paint.setFilterQuality(skFilterQuality);
}

void RosenRenderImage::CanvasDrawImageRect(
    SkPaint& paint, const Offset& offset, SkCanvas* canvas, const Rect& paintRect)
{
    if (GetBackgroundImageFlag()) {
        return;
    }
    if (!image_) {
        imageDataNotReady_ = true;
        LOGI("image data is not ready, rawImageSize_: %{public}s, image source: %{private}s",
            rawImageSize_.ToString().c_str(), sourceInfo_.ToString().c_str());
        return;
    }
#ifdef OHOS_PLATFORM
    float radius = radii_[0].x();
    int fitNum = static_cast<int>(imageFit_);
    int repeatNum = static_cast<int>(imageRepeat_);
    auto recordingCanvas = static_cast<Rosen::RSRecordingCanvas*>(canvas);
    if (GetAdaptiveFrameRectFlag()) {
        recordingCanvas->DrawImageWithParm(image_, fitNum, repeatNum, radius, paint);
        return;
    }
    bool isLoading = ((imageLoadingStatus_ == ImageLoadingStatus::LOADING) ||
                    (imageLoadingStatus_ == ImageLoadingStatus::UPDATING));
    Rect scaledSrcRect = isLoading ? currentSrcRect_ : srcRect_;

    if (sourceInfo_.IsValid() && imageObj_ && (imageObj_->GetFrameCount() == 1)) {
        Size sourceSize = (image_ ? Size(image_->width(), image_->height()) : Size());
        // calculate srcRect that matches the real image source size
        // note that gif doesn't do resize, so gif does not need to recalculate
        scaledSrcRect = RecalculateSrcRect(sourceSize);
        scaledSrcRect.ApplyScaleAndRound(currentResizeScale_);
    }

    Rect realDstRect = paintRect + offset;

    if (imageRepeat_ != ImageRepeat::NOREPEAT) {
        DrawImageOnCanvas(scaledSrcRect, realDstRect, paint, canvas);
        return;
    }
    auto skSrcRect = SkRect::MakeXYWH(
        scaledSrcRect.Left(), scaledSrcRect.Top(), scaledSrcRect.Width(), scaledSrcRect.Height());
    auto skDstRect =
        SkRect::MakeXYWH(realDstRect.Left() - imageRenderPosition_.GetX(),
                         realDstRect.Top() - imageRenderPosition_.GetY(),
                         realDstRect.Width(), realDstRect.Height());
    canvas->drawImageRect(image_, skSrcRect, skDstRect, &paint);
    LOGD("dstRect params: %{public}s", realDstRect.ToString().c_str());
    LOGD("scaledSrcRect params: %{public}s", scaledSrcRect.ToString().c_str());
#endif
}

void RosenRenderImage::DrawImageOnCanvas(const Rect& srcRect, const Rect& dstRect, const SkPaint& paint,
    SkCanvas* canvas) const
{
#ifdef OHOS_PLATFORM
    auto recordingCanvas = static_cast<Rosen::RSRecordingCanvas*>(canvas);
    auto skSrcRect = SkIRect::MakeXYWH(
        Round(srcRect.Left()), Round(srcRect.Top()),
        Round(srcRect.Width()), Round(srcRect.Height()));
    // only transform one time, set skDstRect.top and skDstRect.left to 0.
    auto skDstRect = SkRect::MakeXYWH(
        0, 0,
        dstRect.Width(), dstRect.Height());

    // initialize a transform matrix
    SkScalar scaleX = skDstRect.width() / skSrcRect.width();
    SkScalar scaleY = skDstRect.height() / skSrcRect.height();
    SkScalar transX = dstRect.Left() - imageRenderPosition_.GetX();
    SkScalar transY = dstRect.Top() - imageRenderPosition_.GetY();
    if (matchTextDirection_ && GetTextDirection() == TextDirection::RTL) {
        // flip the image algin x direction.
        scaleX = -1 * scaleX;
        transX = skDstRect.left() + skDstRect.width();
    }
    SkScalar skewX = 0;
    SkScalar skewY = 0;
    SkScalar pers0 = 0;
    SkScalar pers1 = 0;
    SkScalar pers2 = 1;
    auto sampleMatrix = SkMatrix::MakeAll(
        scaleX, skewX, transX,
        skewY, scaleY, transY,
        pers0, pers1, pers2);

    recordingCanvas->save();
    recordingCanvas->concat(sampleMatrix);
    recordingCanvas->drawImageRect(image_, skSrcRect, skDstRect, &paint, SkCanvas::kFast_SrcRectConstraint);
    recordingCanvas->restore();
#endif
}

bool RosenRenderImage::VerifySkImageDataFromPixmap(const RefPtr<PixelMap>& pixmap) const
{
    if (!image_) {
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

Rect RosenRenderImage::RecalculateSrcRect(const Size& realImageSize)
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

void RosenRenderImage::PaintBgImage(const std::shared_ptr<RSNode>& rsNode)
{
    if (!GetBackgroundImageFlag()) {
        return;
    }
    if (currentDstRectList_.empty() || !image_) {
        return;
    }

    if (!rsNode) {
        return;
    }
#ifdef OHOS_PLATFORM
    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetImage(image_);
    rosenImage->SetImageRepeat(static_cast<int>(imageRepeat_));
    rsNode->SetBgImageWidth(imageRenderSize_.Width());
    rsNode->SetBgImageHeight(imageRenderSize_.Height());
    rsNode->SetBgImagePositionX(imageRenderPosition_.GetX());
    rsNode->SetBgImagePositionY(imageRenderPosition_.GetY());
    rsNode->SetBgImage(rosenImage);
#endif
}

bool RosenRenderImage::NeedUploadImageObjToGpu()
{
    if (sourceInfo_.GetSrcType() == SrcType::MEMORY && imageObj_ != nullptr) {
        return true;
    }
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

void RosenRenderImage::UpLoadImageDataForPaint()
{
    if (NeedUploadImageObjToGpu()) {
        imageLoadingStatus_ = ImageLoadingStatus::LOADING;
        if (imageObj_) {
            previousResizeTarget_ = resizeTarget_;
            RosenRenderImage::UploadImageObjToGpuForRender(imageObj_, GetContext(), renderTaskHolder_,
                uploadSuccessCallback_, failedCallback_, resizeTarget_, forceResize_, syncMode_);
        }
    }
}

void RosenRenderImage::UploadImageObjToGpuForRender(const RefPtr<ImageObject>& imageObj,
    const WeakPtr<PipelineContext> context, RefPtr<FlutterRenderTaskHolder>& renderTaskHolder,
    UploadSuccessCallback uploadSuccessCallback, FailedCallback failedCallback, Size resizeTarget, bool forceResize,
    bool syncMode)
{
    if (!imageObj) {
        LOGW("image object is null when try UploadImageObjToGpuForRender.");
        return;
    }
    imageObj->UploadToGpuForRender(
        context, renderTaskHolder, uploadSuccessCallback, failedCallback, resizeTarget, forceResize, syncMode);
}

void RosenRenderImage::UpdateData(const std::string& uri, const std::vector<uint8_t>& memData)
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
    if (!ImageObj) {
        LOGW("image object is null");
        return;
    }
    ImageObjReady(ImageObj);
}

void RosenRenderImage::SetClipRadius()
{
    SetSkRadii(topLeftRadius_, radii_[SkRRect::kUpperLeft_Corner]);
    SetSkRadii(topRightRadius_, radii_[SkRRect::kUpperRight_Corner]);
    SetSkRadii(bottomLeftRadius_, radii_[SkRRect::kLowerLeft_Corner]);
    SetSkRadii(bottomRightRadius_, radii_[SkRRect::kLowerRight_Corner]);
}

void RosenRenderImage::SetSkRadii(const Radius& radius, SkVector& radii)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    double dipScale = context->GetDipScale();
    radii.set(SkDoubleToScalar(std::max(radius.GetX().ConvertToPx(dipScale), 0.0)),
        SkDoubleToScalar(std::max(radius.GetY().ConvertToPx(dipScale), 0.0)));
}

Size RosenRenderImage::MeasureForPixmap()
{
    return rawImageSize_;
}

Size RosenRenderImage::MeasureForSvgImage()
{
    return imageComponentSize_;
}

Size RosenRenderImage::MeasureForNormalImage()
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

Size RosenRenderImage::Measure()
{
    if (imageObj_) {
        return imageObj_->MeasureForImage(AceType::Claim(this));
    }
    LOGI("empty image object, measure failed, return empty size.");
    return Size();
}

void RosenRenderImage::OnHiddenChanged(bool hidden)
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

void RosenRenderImage::CancelBackgroundTasks()
{
    if (fetchImageObjTask_) {
        backgroundTaskCancled_ = fetchImageObjTask_.Cancel(false);
    }
    if (imageObj_) {
        backgroundTaskCancled_ = imageObj_->CancelBackgroundTasks();
    }
}

void RosenRenderImage::PaintSVGImage(const sk_sp<SkData>& skData, bool onlyLayoutSelf)
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

void RosenRenderImage::DrawSVGImage(const Offset& offset, SkCanvas* canvas)
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
                 " of layout param is %{private}s",
                GetLayoutSize().ToString().c_str(), svgContainerSize.ToString().c_str(),
                GetLayoutParam().GetMaxSize().ToString().c_str());
            return;
        } else {
            LOGE("Invalid layout size: %{private}s, valid svgContainerSize: %{private}s, use svg container size to draw"
                 " svg. The max size of layout param is %{private}s",
                GetLayoutSize().ToString().c_str(), svgContainerSize.ToString().c_str(),
                GetLayoutParam().GetMaxSize().ToString().c_str());
        }
    }
    canvas->translate(static_cast<float>(offset.GetX()), static_cast<float>(offset.GetY()));
    double width = svgContainerSize.Width();
    double height = svgContainerSize.Height();
    if (matchTextDirection_ && GetTextDirection() == TextDirection::RTL) {
        canvas->translate(width, 0);
        canvas->scale(-1, 1);
    }
    skiaDom_->setContainerSize({ width, height });
    canvas->clipRect({ 0, 0, width, height }, SkClipOp::kIntersect, true);
    skiaDom_->render(canvas);
}

void RosenRenderImage::DrawSVGImageCustom(RenderContext& context, const Offset& offset)
{
    if (svgDom_ && svgDom_->GetRootRenderNode()) {
        PaintChild(svgDom_->GetRootRenderNode(), context, offset);
    }
}

void RosenRenderImage::UpdateLoadSuccessState()
{
    LOGD("update success state info: %{public}s", sourceInfo_.ToString().c_str());
    imageLoadingStatus_ = ImageLoadingStatus::LOAD_SUCCESS;
    auto currentFrameCount = imageObj_->GetFrameCount();
    if ((!sourceInfo_.IsSvg() && currentFrameCount == 1) || (currentFrameCount > 1 && curSourceInfo_ != sourceInfo_)) {
        FireLoadEvent(imageSizeForEvent_);
    }
    // alt_image has empty imageAlt_, then mark the parent needs render
    if ((currentFrameCount > 1 && curSourceInfo_ != sourceInfo_) || imageAlt_.empty()) {
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

void RosenRenderImage::UpdateRenderAltImage(const RefPtr<Component>& component)
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

bool RosenRenderImage::MaybeRelease()
{
    auto context = GetContext().Upgrade();
    if (context && context->GetRenderFactory() && context->GetRenderFactory()->GetRenderImageFactory()->Recycle(this)) {
        ClearRenderObject();
        return false;
    }
    return true;
}

void RosenRenderImage::ClearRenderObject()
{
    LOGD("Clear obj %{public}s", curSourceInfo_.ToString().c_str());
    RenderImage::ClearRenderObject();
    CancelBackgroundTasks();
    curSourceInfo_.Reset();
    image_ = nullptr;
    formerRawImageSize_ = { 0.0, 0.0 };
    imageObj_ = nullptr;
    skiaDom_ = nullptr;
    svgDom_ = nullptr;
}

bool RosenRenderImage::IsSourceWideGamut() const
{
    if (sourceInfo_.IsSvg() || !image_) {
        return false;
    }
    return ImageProvider::IsWideGamut(image_->refColorSpace());
}

bool RosenRenderImage::RetryLoading()
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
    ImageProvider::FetchImageObject(sourceInfo_, imageObjSuccessCallback_, uploadSuccessCallback_, failedCallback_,
        GetContext(), syncMode, useSkiaSvg_, autoResize_, renderTaskHolder_, onPostBackgroundTask_);
    LOGW("Retry loading time: %{public}d, triggered by GetImageSize fail, imageSrc: %{private}s", retryCnt_,
        sourceInfo_.ToString().c_str());
    return true;
}

SkImageInfo RosenRenderImage::MakeSkImageInfoFromPixelMap(const RefPtr<PixelMap>& pixmap)
{
    SkColorType ct = PixelFormatToSkColorType(pixmap);
    SkAlphaType at = AlphaTypeToSkAlphaType(pixmap);
    sk_sp<SkColorSpace> cs = ColorSpaceToSkColorSpace(pixmap);
    return SkImageInfo::Make(pixmap->GetWidth(), pixmap->GetHeight(), ct, at, cs);
}

sk_sp<SkColorSpace> RosenRenderImage::ColorSpaceToSkColorSpace(const RefPtr<PixelMap>& pixmap)
{
    return SkColorSpace::MakeSRGB(); // Media::PixelMap has not support wide gamut yet.
}

SkAlphaType RosenRenderImage::AlphaTypeToSkAlphaType(const RefPtr<PixelMap>& pixmap)
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

SkColorType RosenRenderImage::PixelFormatToSkColorType(const RefPtr<PixelMap>& pixmap)
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

void RosenRenderImage::OnAppHide()
{
    isAppOnShow_ = false;
    if (imageObj_) {
        imageObj_->Pause();
    }
}

void RosenRenderImage::OnAppShow()
{
    isAppOnShow_ = true;
    if (imageObj_) {
        imageObj_->Resume();
    }
}

void RosenRenderImage::OnVisibleChanged()
{
    if (imageObj_ && GetVisible()) {
        imageObj_->Resume();
    } else if (imageObj_ && !GetVisible()) {
        imageObj_->Pause();
    }
}

} // namespace OHOS::Ace
