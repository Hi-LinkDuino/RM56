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

#include "core/image/image_object.h"

#include "base/thread/background_task_executor.h"
#include "core/common/container.h"
#include "core/common/container_scope.h"
#include "core/components/image/render_image.h"
#include "core/image/flutter_image_cache.h"

namespace OHOS::Ace {

std::string ImageObject::GenerateCacheKey(const ImageSourceInfo& srcInfo, Size targetImageSize)
{
    return srcInfo.GetCacheKey() + std::to_string(static_cast<int32_t>(targetImageSize.Width())) +
           std::to_string(static_cast<int32_t>(targetImageSize.Height()));
}

RefPtr<ImageObject> ImageObject::BuildImageObject(
    ImageSourceInfo source,
    const RefPtr<PipelineContext> context,
    const sk_sp<SkData>& skData,
    bool useSkiaSvg)
{
    // build svg image object.
    if (source.IsSvg()) {
        const auto svgStream = std::make_unique<SkMemoryStream>(skData);
        if (!svgStream) {
            return nullptr;
        }
        auto color = source.GetFillColor();
        if (!useSkiaSvg) {
            auto svgDom = SvgDom::CreateSvgDom(*svgStream, context, color);
            return svgDom ? MakeRefPtr<SvgImageObject>(source, Size(), 1, svgDom) : nullptr;
        } else {
            uint64_t colorValue = 0;
            if (color.has_value()) {
                colorValue = color.value().GetValue();
                // skia svg relies on the 32th bit to determine whether or not to use the color we set.
                colorValue = colorValue | (static_cast<int64_t>(0b1) << 32);
            }
            auto skiaDom = SkSVGDOM::MakeFromStream(*svgStream, colorValue);
            return skiaDom ? MakeRefPtr<SvgSkiaImageObject>(source, Size(), 1, skiaDom) : nullptr;
        }
    }
    // build normal pixel image object.
    auto codec = SkCodec::MakeFromData(skData);
    int32_t totalFrames = 1;
    Size imageSize;
    if (codec) {
        totalFrames = codec->getFrameCount();
        switch (codec->getOrigin()) {
            case SkEncodedOrigin::kLeftTop_SkEncodedOrigin:
            case SkEncodedOrigin::kRightTop_SkEncodedOrigin:
            case SkEncodedOrigin::kRightBottom_SkEncodedOrigin:
            case SkEncodedOrigin::kLeftBottom_SkEncodedOrigin:
                imageSize.SetSize(Size(codec->dimensions().fHeight, codec->dimensions().fWidth));
                break;
            default:
                imageSize.SetSize(Size(codec->dimensions().fWidth, codec->dimensions().fHeight));
        }
    }
    if (totalFrames == 1) {
        return MakeRefPtr<StaticImageObject>(source, imageSize, totalFrames, skData);
    } else {
        return MakeRefPtr<AnimatedImageObject>(source, imageSize, totalFrames, skData);
    }
}

Size ImageObject::MeasureForImage(RefPtr<RenderImage> image)
{
    return image->MeasureForNormalImage();
}

void SvgImageObject::PerformLayoutImageObject(RefPtr<RenderImage> image)
{
    image->PerformLayoutSvgImage();
}

Size SvgImageObject::MeasureForImage(RefPtr<RenderImage> image)
{
    return image->MeasureForSvgImage();
}

void SvgSkiaImageObject::PerformLayoutImageObject(RefPtr<RenderImage> image) {}

Size SvgSkiaImageObject::MeasureForImage(RefPtr<RenderImage> image)
{
    return image->MeasureForSvgImage();
}

void StaticImageObject::UploadToGpuForRender(
    const WeakPtr<PipelineContext> context,
    RefPtr<FlutterRenderTaskHolder>& renderTaskHolder,
    UploadSuccessCallback successCallback,
    FailedCallback failedCallback,
    Size imageSize,
    bool forceResize,
    bool syncMode)
{
    auto task = [context, renderTaskHolder, successCallback, failedCallback, imageSize, forceResize, skData = skData_,
                    imageSource = imageSource_, id = Container::CurrentId()]() mutable {
        ContainerScope scope(id);
        auto pipelineContext = context.Upgrade();
        if (!pipelineContext) {
            LOGE("pipline context has been released.");
            return;
        }
        auto taskExecutor = pipelineContext->GetTaskExecutor();
        if (!taskExecutor) {
            LOGE("task executor is null.");
            return;
        }
        fml::RefPtr<flutter::CanvasImage> cachedFlutterImage;
        auto imageCache = pipelineContext->GetImageCache();
        if (imageCache) {
            auto cachedImage = imageCache->GetCacheImage(GenerateCacheKey(imageSource, imageSize));
            LOGD("image cache valid");
            if (cachedImage) {
                LOGD("cached image found.");
                cachedFlutterImage = cachedImage->imagePtr;
            }
        }
        if (cachedFlutterImage) {
            LOGD("get cached image success: %{public}s", GenerateCacheKey(imageSource, imageSize).c_str());
            taskExecutor->PostTask([successCallback, imageSource,
                                       cachedFlutterImage] { successCallback(imageSource, cachedFlutterImage); },
                TaskExecutor::TaskType::UI);
            return;
        }

        if (!skData) {
            LOGD("reload sk data");
            skData = ImageProvider::LoadImageRawData(imageSource, pipelineContext, imageSize);
            if (!skData) {
                LOGE("reload image data failed. imageSource: %{private}s", imageSource.ToString().c_str());
                taskExecutor->PostTask(
                    [failedCallback, imageSource] { failedCallback(imageSource); }, TaskExecutor::TaskType::UI);
                return;
            }
        }
        auto rawImage = SkImage::MakeFromEncoded(skData);
        if (!rawImage) {
            LOGE("static image MakeFromEncoded fail! imageSource: %{private}s", imageSource.ToString().c_str());
            taskExecutor->PostTask(
                [failedCallback, imageSource] { failedCallback(imageSource); }, TaskExecutor::TaskType::UI);
            return;
        }
        auto image = ImageProvider::ResizeSkImage(rawImage, imageSource.GetSrc(), imageSize, forceResize);
        auto callback =
            [successCallback, imageSource, taskExecutor, imageCache, imageSize](flutter::SkiaGPUObject<SkImage> image) {
                auto canvasImage = flutter::CanvasImage::Create();
                canvasImage->set_image(std::move(image));
                if (imageCache) {
                    LOGD("cache image key: %{public}s", GenerateCacheKey(imageSource, imageSize).c_str());
                    imageCache->CacheImage(
                        GenerateCacheKey(imageSource, imageSize),
                        std::make_shared<CachedImage>(canvasImage));
                }
                taskExecutor->PostTask(
                    [ successCallback, imageSource, canvasImage ] {
                        successCallback(imageSource, canvasImage);
                    },
                    TaskExecutor::TaskType::UI);
        };
        ImageProvider::UploadImageToGPUForRender(image, callback, renderTaskHolder);
    };
    if (syncMode) {
        task();
        return;
    }
    uploadForPaintTask_ = CancelableTask(std::move(task));
    BackgroundTaskExecutor::GetInstance().PostTask(uploadForPaintTask_);
}

bool StaticImageObject::CancelBackgroundTasks()
{
    return uploadForPaintTask_ ? uploadForPaintTask_.Cancel(false) : false;
}

void AnimatedImageObject::UploadToGpuForRender(
    const WeakPtr<PipelineContext> context,
    RefPtr<FlutterRenderTaskHolder>& renderTaskHolder,
    UploadSuccessCallback successCallback,
    FailedCallback failedCallback,
    Size imageSize,
    bool forceResize,
    bool syncMode)
{
    if (!animatedPlayer_ && skData_) {
        auto codec = SkCodec::MakeFromData(skData_);
        int32_t dstWidth = -1;
        int32_t dstHeight = -1;
        if (forceResize) {
            dstWidth = static_cast<int32_t>(imageSize.Width() + 0.5);
            dstHeight = static_cast<int32_t>(imageSize.Height() + 0.5);
        }
        animatedPlayer_ = MakeRefPtr<AnimatedImagePlayer>(
            imageSource_,
            successCallback,
            context,
            renderTaskHolder->ioManager,
            renderTaskHolder->unrefQueue,
            std::move(codec),
            dstWidth,
            dstHeight);
        ClearData();
    } else if (animatedPlayer_ && forceResize && imageSize.IsValid()) {
        LOGI("animated player has been construced, forceResize: %{public}s", imageSize.ToString().c_str());
        int32_t dstWidth = static_cast<int32_t>(imageSize.Width() + 0.5);
        int32_t dstHeight = static_cast<int32_t>(imageSize.Height() + 0.5);
        animatedPlayer_->SetTargetSize(dstWidth, dstHeight);
    } else if (!animatedPlayer_ && !skData_) {
        LOGE("animated player is not constructed and image data is null, can not construct animated player!");
    } else if (animatedPlayer_ && !forceResize) {
        LOGI("animated player has been construced, do nothing!");
    }
}

void PixelMapImageObject::PerformLayoutImageObject(RefPtr<RenderImage> image)
{
    image->PerformLayoutPixmap();
}

Size PixelMapImageObject::MeasureForImage(RefPtr<RenderImage> image)
{
    return image->MeasureForPixmap();
}

} // namespace OHOS::Ace