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

#include "core/image/image_provider.h"

#include "experimental/svg/model/SkSVGDOM.h"
#include "third_party/skia/include/core/SkGraphics.h"
#include "third_party/skia/include/core/SkStream.h"

#include "base/thread/background_task_executor.h"
#include "core/common/container.h"
#include "core/common/container_scope.h"
#include "core/components/image/flutter_render_image.h"
#include "core/event/ace_event_helper.h"
#include "core/image/flutter_image_cache.h"
#include "core/image/image_object.h"

namespace OHOS::Ace {
namespace {

constexpr double RESIZE_MAX_PROPORTION = 0.5 * 0.5; // Cache image when resize exceeds 25%
// If a picture is a wide color gamut picture, its area value will be larger than this threshold.
constexpr double SRGB_GAMUT_AREA = 0.104149;

} // namespace

void ImageProvider::FetchImageObject(
    ImageSourceInfo imageInfo,
    ImageObjSuccessCallback successCallback,
    UploadSuccessCallback uploadSuccessCallback,
    FailedCallback failedCallback,
    const WeakPtr<PipelineContext> context,
    bool syncMode,
    bool useSkiaSvg,
    bool needAutoResize,
    RefPtr<FlutterRenderTaskHolder>& renderTaskHolder,
    OnPostBackgroundTask onBackgroundTaskPostCallback)
{
    auto task = [context, imageInfo, successCallback, failedCallback, useSkiaSvg, renderTaskHolder,
                    uploadSuccessCallback, needAutoResize, id = Container::CurrentId(), syncMode]() mutable {
        ContainerScope scope(id);
        auto pipelineContext = context.Upgrade();
        if (!pipelineContext) {
            LOGE("pipline context has been released. imageInfo: %{private}s", imageInfo.ToString().c_str());
            return;
        }
        auto taskExecutor = pipelineContext->GetTaskExecutor();
        if (!taskExecutor) {
            LOGE("task executor is null. imageInfo: %{private}s", imageInfo.ToString().c_str());
            return;
        }
        RefPtr<ImageObject> imageObj = QueryImageObjectFromCache(imageInfo, pipelineContext);
        if (!imageObj) { // if image object is not in cache, generate a new one.
            imageObj = GeneraterAceImageObject(imageInfo, pipelineContext, useSkiaSvg);
        }
        if (!imageObj) { // if it fails to generate an image object, trigger fail callback.
            if (syncMode) {
                failedCallback(imageInfo);
                return;
            }
            taskExecutor->PostTask(
                [failedCallback, imageInfo] { failedCallback(imageInfo); }, TaskExecutor::TaskType::UI);
            return;
        }
        if (syncMode) {
            successCallback(imageInfo, imageObj);
        } else {
            taskExecutor->PostTask([successCallback, imageInfo, imageObj]() { successCallback(imageInfo, imageObj); },
                TaskExecutor::TaskType::UI);
        }
        bool canStartUploadImageObj = !needAutoResize && (imageObj->GetFrameCount() == 1);
        if (canStartUploadImageObj) {
            bool forceResize = (!imageObj->IsSvg()) && (imageInfo.IsSourceDimensionValid());
            FlutterRenderImage::UploadImageObjToGpuForRender(imageObj, context, renderTaskHolder, uploadSuccessCallback,
                failedCallback, imageObj->GetImageSize(), forceResize, true);
        }
    };
    if (syncMode) {
        task();
        return;
    }
    CancelableTask cancelableTask(std::move(task));
    if (onBackgroundTaskPostCallback) {
        onBackgroundTaskPostCallback(cancelableTask);
    }
    BackgroundTaskExecutor::GetInstance().PostTask(cancelableTask);
}

RefPtr<ImageObject> ImageProvider::QueryImageObjectFromCache(
    const ImageSourceInfo& imageInfo, const RefPtr<PipelineContext>& pipelineContext)
{
    auto imageCache = pipelineContext->GetImageCache();
    if (!imageCache) {
        return nullptr;
    }
    return imageCache->GetCacheImgObj(imageInfo.ToString());
}

RefPtr<ImageObject> ImageProvider::GeneraterAceImageObject(
    const ImageSourceInfo& imageInfo,
    const RefPtr<PipelineContext> context,
    bool useSkiaSvg)
{
    auto imageData = LoadImageRawData(imageInfo, context);

    if (!imageData) {
        LOGE("load image data failed. imageInfo: %{private}s", imageInfo.ToString().c_str());
        return nullptr;
    }

    return ImageObject::BuildImageObject(imageInfo, context, imageData, useSkiaSvg);
}

sk_sp<SkData> ImageProvider::LoadImageRawData(
    const ImageSourceInfo& imageInfo,
    const RefPtr<PipelineContext> context,
    const Size& targetSize)
{
    auto imageCache = context->GetImageCache();
    if (imageCache) {
        // 1. try get data from cache.
        auto cacheData = imageCache->GetCacheImageData(imageInfo.GetSrc());
        if (cacheData) {
            LOGD("sk data from memory cache.");
            return AceType::DynamicCast<SkiaCachedImageData>(cacheData)->imageData;
        }
        // 2 try get data from file cache.
        if (targetSize.IsValid()) {
            LOGD("size valid try load from cache.");
            std::string cacheFilePath =
                ImageCache::GetImageCacheFilePath(ImageObject::GenerateCacheKey(imageInfo, targetSize));
            LOGD("cache file path: %{private}s", cacheFilePath.c_str());
            auto data = imageCache->GetDataFromCacheFile(cacheFilePath);
            if (data) {
                LOGD("cache file found : %{public}s", cacheFilePath.c_str());
                return AceType::DynamicCast<SkiaCachedImageData>(data)->imageData;
            }
        } else {
            LOGD("target size is not valid, load raw image file.");
        }
    }
    // 3. try load raw image file.
    auto imageLoader = ImageLoader::CreateImageLoader(imageInfo);
    if (!imageLoader) {
        LOGE("imageLoader create failed. imageInfo: %{private}s", imageInfo.ToString().c_str());
        return nullptr;
    }
    auto data = imageLoader->LoadImageData(imageInfo, context);
    if (data && imageCache) {
        // cache sk data.
        imageCache->CacheImageData(imageInfo.GetSrc(), AceType::MakeRefPtr<SkiaCachedImageData>(data));
    }
    return data;
}

void ImageProvider::GetSVGImageDOMAsyncFromSrc(
    const std::string& src,
    std::function<void(const sk_sp<SkSVGDOM>&)> successCallback,
    std::function<void()> failedCallback,
    const WeakPtr<PipelineContext> context,
    uint64_t svgThemeColor,
    OnPostBackgroundTask onBackgroundTaskPostCallback)
{
    auto task = [src, successCallback, failedCallback, context, svgThemeColor, id = Container::CurrentId()] {
        ContainerScope scope(id);
        auto pipelineContext = context.Upgrade();
        if (!pipelineContext) {
            LOGW("render image or pipeline has been released.");
            return;
        }
        auto taskExecutor = pipelineContext->GetTaskExecutor();
        if (!taskExecutor) {
            return;
        }
        ImageSourceInfo info(src);
        auto imageLoader = ImageLoader::CreateImageLoader(info);
        if (!imageLoader) {
            LOGE("load image failed when create image loader.");
            return;
        }
        auto imageData = imageLoader->LoadImageData(info, context);
        if (imageData) {
            const auto svgStream = std::make_unique<SkMemoryStream>(std::move(imageData));
            if (svgStream) {
                auto skiaDom = SkSVGDOM::MakeFromStream(*svgStream, svgThemeColor);
                if (skiaDom) {
                    taskExecutor->PostTask(
                        [successCallback, skiaDom] { successCallback(skiaDom); }, TaskExecutor::TaskType::UI);
                    return;
                }
            }
        }
        LOGE("svg data wrong!");
        taskExecutor->PostTask([failedCallback] { failedCallback(); }, TaskExecutor::TaskType::UI);
    };
    CancelableTask cancelableTask(std::move(task));
    if (onBackgroundTaskPostCallback) {
        onBackgroundTaskPostCallback(cancelableTask);
    }
    BackgroundTaskExecutor::GetInstance().PostTask(cancelableTask);
}

void ImageProvider::GetSVGImageDOMAsyncFromData(
    const sk_sp<SkData>& skData,
    std::function<void(const sk_sp<SkSVGDOM>&)> successCallback,
    std::function<void()> failedCallback,
    const WeakPtr<PipelineContext> context,
    uint64_t svgThemeColor,
    OnPostBackgroundTask onBackgroundTaskPostCallback)
{
    auto task = [skData, successCallback, failedCallback, context, svgThemeColor, id = Container::CurrentId()] {
        ContainerScope scope(id);
        auto pipelineContext = context.Upgrade();
        if (!pipelineContext) {
            LOGW("render image or pipeline has been released.");
            return;
        }
        auto taskExecutor = pipelineContext->GetTaskExecutor();
        if (!taskExecutor) {
            return;
        }

        const auto svgStream = std::make_unique<SkMemoryStream>(skData);
        if (svgStream) {
            auto skiaDom = SkSVGDOM::MakeFromStream(*svgStream, svgThemeColor);
            if (skiaDom) {
                taskExecutor->PostTask(
                    [successCallback, skiaDom] { successCallback(skiaDom); }, TaskExecutor::TaskType::UI);
                return;
            }
        }
        LOGE("svg data wrong!");
        taskExecutor->PostTask([failedCallback] { failedCallback(); }, TaskExecutor::TaskType::UI);
    };
    CancelableTask cancelableTask(std::move(task));
    if (onBackgroundTaskPostCallback) {
        onBackgroundTaskPostCallback(cancelableTask);
    }
    BackgroundTaskExecutor::GetInstance().PostTask(cancelableTask);
}

void ImageProvider::UploadImageToGPUForRender(
    const sk_sp<SkImage>& image,
    const std::function<void(flutter::SkiaGPUObject<SkImage>)>&& callback,
    const RefPtr<FlutterRenderTaskHolder>& renderTaskHolder)
{
    if (!renderTaskHolder) {
        LOGW("renderTaskHolder has been released.");
        return;
    }
#if defined(DUMP_DRAW_CMD) || defined(GPU_DISABLED)
    // If want to dump draw command or gpu disabled, should use CPU image.
    callback({ image, renderTaskHolder->unrefQueue });
#else
    auto rasterizedImage = image->makeRasterImage();
    if (!rasterizedImage) {
        LOGW("Rasterize image failed. callback.");
        callback({ image, renderTaskHolder->unrefQueue });
        return;
    }
    auto task = [rasterizedImage, callback, renderTaskHolder] () {
        if (!renderTaskHolder) {
            LOGW("renderTaskHolder has been released.");
            return;
        }
        // weak reference of io manager must be check and used on io thread, because io manager is created on io thread.
        if (!renderTaskHolder->ioManager) {
            // Shell is closing.
            callback({ rasterizedImage, renderTaskHolder->unrefQueue });
            return;
        }
        ACE_DCHECK(!rasterizedImage->isTextureBacked());
        auto resContext = renderTaskHolder->ioManager->GetResourceContext();
        if (!resContext) {
            callback({ rasterizedImage, renderTaskHolder->unrefQueue });
            return;
        }
        SkPixmap pixmap;
        if (!rasterizedImage->peekPixels(&pixmap)) {
            LOGW("Could not peek pixels of image for texture upload.");
            callback({ rasterizedImage, renderTaskHolder->unrefQueue });
            return;
        }
        auto textureImage =
            SkImage::MakeCrossContextFromPixmap(resContext.get(), pixmap, true, pixmap.colorSpace(), true);
        callback({ textureImage ? textureImage : rasterizedImage, renderTaskHolder->unrefQueue });

        // Trigger purge cpu bitmap resource, after image upload to gpu.
        SkGraphics::PurgeResourceCache();
    };
    renderTaskHolder->ioTaskRunner->PostTask(std::move(task));
#endif
}

sk_sp<SkImage> ImageProvider::ResizeSkImage(
    const sk_sp<SkImage>& rawImage,
    const std::string& src,
    Size imageSize,
    bool forceResize)
{
    if (!imageSize.IsValid()) {
        LOGE("not valid size!, imageSize: %{private}s, src: %{private}s", imageSize.ToString().c_str(), src.c_str());
        return rawImage;
    }
    int32_t dstWidth = static_cast<int32_t>(imageSize.Width() + 0.5);
    int32_t dstHeight = static_cast<int32_t>(imageSize.Height() + 0.5);

    bool needResize = false;

    if (!forceResize) {
        if (rawImage->width() > dstWidth) {
            needResize = true;
        } else {
            dstWidth = rawImage->width();
        }
        if (rawImage->height() > dstHeight) {
            needResize = true;
        } else {
            dstHeight = rawImage->height();
        }
    }

    if (!needResize && !forceResize) {
        return rawImage;
    }
    return ApplySizeToSkImage(
        rawImage,
        dstWidth,
        dstHeight,
        ImageObject::GenerateCacheKey(ImageSourceInfo(src), imageSize));
}

sk_sp<SkImage> ImageProvider::ApplySizeToSkImage(
    const sk_sp<SkImage>& rawImage,
    int32_t dstWidth,
    int32_t dstHeight,
    const std::string& srcKey)
{
    auto scaledImageInfo =
        SkImageInfo::Make(dstWidth, dstHeight, rawImage->colorType(), rawImage->alphaType(), rawImage->refColorSpace());
    SkBitmap scaledBitmap;
    if (!scaledBitmap.tryAllocPixels(scaledImageInfo)) {
        LOGE("Could not allocate bitmap when attempting to scale. srcKey: %{private}s, destination size: [%{public}d x"
            " %{public}d], raw image size: [%{public}d x %{public}d]",
            srcKey.c_str(), dstWidth, dstHeight, rawImage->width(), rawImage->height());
        return rawImage;
    }
    if (!rawImage->scalePixels(scaledBitmap.pixmap(), kLow_SkFilterQuality, SkImage::kDisallow_CachingHint)) {
        LOGE("Could not scale pixels srcKey: %{private}s, destination size: [%{public}d x"
            " %{public}d], raw image size: [%{public}d x %{public}d]",
            srcKey.c_str(), dstWidth, dstHeight, rawImage->width(), rawImage->height());
        return rawImage;
    }
    // Marking this as immutable makes the MakeFromBitmap call share the pixels instead of copying.
    scaledBitmap.setImmutable();
    auto scaledImage = SkImage::MakeFromBitmap(scaledBitmap);
    if (scaledImage) {
        bool needCacheResizedImageFile =
            (1.0 * dstWidth * dstHeight) / (rawImage->width() * rawImage->height()) < RESIZE_MAX_PROPORTION;
        if (needCacheResizedImageFile && !srcKey.empty()) {
            BackgroundTaskExecutor::GetInstance().PostTask(
                [srcKey, scaledImage]() {
                    LOGI("write png cache file: %{private}s", srcKey.c_str());
                    auto data = scaledImage->encodeToData(SkEncodedImageFormat::kPNG, 100);
                    if (!data) {
                        LOGI("encode cache image into cache file failed.");
                        return;
                    }
                    ImageCache::WriteCacheFile(srcKey, data->data(), data->size());
                },
                BgTaskPriority::LOW);
        }
        return scaledImage;
    }
    LOGE("Could not create a scaled image from a scaled bitmap. srcKey: %{private}s, destination size: [%{public}d x"
        " %{public}d], raw image size: [%{public}d x %{public}d]",
        srcKey.c_str(), dstWidth, dstHeight, rawImage->width(), rawImage->height());
    return rawImage;
}

sk_sp<SkImage> ImageProvider::GetSkImage(
    const std::string& src,
    const WeakPtr<PipelineContext> context,
    Size targetSize)
{
    ImageSourceInfo info(src);
    auto imageLoader = ImageLoader::CreateImageLoader(info);
    if (!imageLoader) {
        LOGE("Invalid src, src is %{public}s", src.c_str());
        return nullptr;
    }
    auto imageSkData = imageLoader->LoadImageData(info, context);
    if (!imageSkData) {
        LOGE("fetch data failed. src: %{private}s", src.c_str());
        return nullptr;
    }
    auto rawImage = SkImage::MakeFromEncoded(imageSkData);
    if (!rawImage) {
        LOGE("MakeFromEncoded failed! src: %{private}s", src.c_str());
        return nullptr;
    }
    auto image = ResizeSkImage(rawImage, src, targetSize);
    return image;
}

void ImageProvider::TryLoadImageInfo(const RefPtr<PipelineContext>& context, const std::string& src,
    std::function<void(bool, int32_t, int32_t)>&& loadCallback)
{
    BackgroundTaskExecutor::GetInstance().PostTask(
        [src, callback = std::move(loadCallback), context, id = Container::CurrentId()]() {
            ContainerScope scope(id);
            auto taskExecutor = context->GetTaskExecutor();
            if (!taskExecutor) {
                return;
            }
            auto image = ImageProvider::GetSkImage(src, context);
            if (image) {
                callback(true, image->width(), image->height());
                return;
            }
            callback(false, 0, 0);
        });
}

bool ImageProvider::IsWideGamut(const sk_sp<SkColorSpace>& colorSpace)
{
    skcms_ICCProfile encodedProfile;
    colorSpace->toProfile(&encodedProfile);
    if (!encodedProfile.has_toXYZD50) {
        LOGI("This profile's gamut can not be represented by a 3x3 transform to XYZD50");
        return false;
    }
    // Normalize gamut by 1.
    // rgb[3] represents the point of Red, Green and Blue coordinate in color space diagram.
    Point rgb[3];
    auto xyzGamut = encodedProfile.toXYZD50;
    for (int32_t i = 0; i < 3; i++) {
        auto sum = xyzGamut.vals[i][0] + xyzGamut.vals[i][1] + xyzGamut.vals[i][2];
        rgb[i].SetX(xyzGamut.vals[i][0] / sum);
        rgb[i].SetY(xyzGamut.vals[i][1] / sum);
    }
    // Calculate the area enclosed by the coordinates of the three RGB points
    Point red = rgb[0];
    Point green = rgb[1];
    Point blue = rgb[2];
    // Assuming there is a triangle enclosed by three points: A(x1, y1), B(x2, y2), C(x3, y3),
    // the formula for calculating the area of triangle ABC is as follows:
    // S = (x1 * y2 + x2 * y3 + x3 * y1 - x1 * y3 - x2 * y1 - x3 * y2) / 2.0
    auto areaOfPoint = std::fabs(red.GetX() * green.GetY() + green.GetX() * blue.GetY() + blue.GetX() * green.GetY() -
        red.GetX() * blue.GetY() - blue.GetX() * green.GetY() - green.GetX() * red.GetY()) / 2.0;
    return GreatNotEqual(areaOfPoint, SRGB_GAMUT_AREA);
}

SkImageInfo ImageProvider::MakeSkImageInfoFromPixelMap(const RefPtr<PixelMap>& pixmap)
{
    SkColorType ct = PixelFormatToSkColorType(pixmap);
    SkAlphaType at = AlphaTypeToSkAlphaType(pixmap);
    sk_sp<SkColorSpace> cs = ColorSpaceToSkColorSpace(pixmap);
    return SkImageInfo::Make(pixmap->GetWidth(), pixmap->GetHeight(), ct, at, cs);
}

sk_sp<SkColorSpace> ImageProvider::ColorSpaceToSkColorSpace(const RefPtr<PixelMap>& pixmap)
{
    return SkColorSpace::MakeSRGB(); // Media::PixelMap has not support wide gamut yet.
}

SkAlphaType ImageProvider::AlphaTypeToSkAlphaType(const RefPtr<PixelMap>& pixmap)
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

SkColorType ImageProvider::PixelFormatToSkColorType(const RefPtr<PixelMap>& pixmap)
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

} // namespace OHOS::Ace
