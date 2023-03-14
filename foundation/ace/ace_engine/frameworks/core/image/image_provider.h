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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_PROVIDER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_PROVIDER_H

#include <string>

#include "flutter/fml/memory/ref_counted.h"
#include "flutter/lib/ui/painting/image.h"
#include "third_party/skia/include/codec/SkCodec.h"

#include "base/memory/ace_type.h"
#include "base/resource/internal_resource.h"
#include "core/components/common/layout/constants.h"
#include "core/image/image_source_info.h"
#include "core/image/image_loader.h"
#include "core/pipeline/pipeline_context.h"

class SkSVGDOM;
namespace OHOS::Ace {

class ImageObject;
struct FlutterRenderTaskHolder : public virtual AceType {
    DECLARE_ACE_TYPE(FlutterRenderTaskHolder, AceType);
public:
    FlutterRenderTaskHolder(
        fml::RefPtr<flutter::SkiaUnrefQueue> queue,
        fml::WeakPtr<flutter::IOManager> manager,
        fml::RefPtr<fml::TaskRunner> taskRunner)
        : unrefQueue(queue),
          ioManager(manager),
          ioTaskRunner(taskRunner)
        {}

    fml::RefPtr<flutter::SkiaUnrefQueue> unrefQueue;
    // weak reference of io manager must be check and used on io thread, because io manager is created on io thread.
    fml::WeakPtr<flutter::IOManager> ioManager;
    fml::RefPtr<fml::TaskRunner> ioTaskRunner;
};

class SvgDom;
using ImageObjSuccessCallback = std::function<void(ImageSourceInfo, const RefPtr<ImageObject>)>;
using UploadSuccessCallback = std::function<void(ImageSourceInfo, const fml::RefPtr<flutter::CanvasImage>&)>;
using SvgDomSuccessCallback =  std::function<void(ImageSourceInfo, const RefPtr<SvgDom>&)>;
using FailedCallback = std::function<void(ImageSourceInfo)>;
using CancelableTask = CancelableCallback<void()>;
using OnPostBackgroundTask = std::function<void(CancelableTask)>;

class FlutterRenderImage;
class ImageProvider {
public:
    static void TryLoadImageInfo(const RefPtr<PipelineContext>& context, const std::string& src,
        std::function<void(bool, int32_t, int32_t)>&& loadCallback);

    static void GetSVGImageDOMAsyncFromSrc(
        const std::string& src,
        std::function<void(const sk_sp<SkSVGDOM>&)> callback,
        std::function<void()> failedCallback,
        const WeakPtr<PipelineContext> context,
        uint64_t svgThemeColor = 0,
        OnPostBackgroundTask onBackgroundTaskPostCallback = nullptr);

    static void GetSVGImageDOMAsyncFromData(
        const sk_sp<SkData>& skData,
        std::function<void(const sk_sp<SkSVGDOM>&)> callback,
        std::function<void()> failedCallback,
        const WeakPtr<PipelineContext> context,
        uint64_t svgThemeColor = 0,
        OnPostBackgroundTask onBackgroundTaskPostCallback = nullptr);

    // upload image data to gpu context for painting asynchronously.
    static void UploadImageToGPUForRender(
        const sk_sp<SkImage>& image,
        const std::function<void(flutter::SkiaGPUObject<SkImage>)>&& callback,
        const RefPtr<FlutterRenderTaskHolder>& renderTaskHolder);

    // get out source image data asynchronously.
    static void FetchImageObject(
        ImageSourceInfo imageInfo,
        ImageObjSuccessCallback successCallback,
        UploadSuccessCallback uploadSuccessCallback,
        FailedCallback failedCallback,
        const WeakPtr<PipelineContext> context,
        bool syncMode,
        bool useSkiaSvg,
        bool needAutoResize,
        RefPtr<FlutterRenderTaskHolder>& renderTaskHolder,
        OnPostBackgroundTask onBackgroundTaskPostCallback = nullptr);

    static sk_sp<SkImage> ResizeSkImage(
        const sk_sp<SkImage>& rawImage,
        const std::string& src,
        Size imageSize,
        bool forceResize = false);

    static sk_sp<SkImage> ApplySizeToSkImage(
        const sk_sp<SkImage>& rawImage,
        int32_t dstWidth,
        int32_t dstHeight,
        const std::string& srcKey = std::string());

    static bool IsWideGamut(const sk_sp<SkColorSpace>& colorSpace);

    static bool NeedExchangeWidthAndHeight(SkEncodedOrigin origin);

    // This is a synchronization interface for getting out source image.
    static sk_sp<SkImage> GetSkImage(
        const std::string& src,
        const WeakPtr<PipelineContext> context,
        Size targetSize = Size());

    static RefPtr<ImageObject> GeneraterAceImageObject(
        const ImageSourceInfo& imageInfo,
        const RefPtr<PipelineContext> context,
        bool useSkiaSvg);

    static sk_sp<SkData> LoadImageRawData(
        const ImageSourceInfo& imageInfo,
        const RefPtr<PipelineContext> context,
        const Size& targetSize = Size());

    static RefPtr<ImageObject> QueryImageObjectFromCache(
        const ImageSourceInfo& imageInfo, const RefPtr<PipelineContext>& pipelineContext);
    static SkColorType PixelFormatToSkColorType(const RefPtr<PixelMap>& pixmap);
    static SkAlphaType AlphaTypeToSkAlphaType(const RefPtr<PixelMap>& pixmap);
    static SkImageInfo MakeSkImageInfoFromPixelMap(const RefPtr<PixelMap>& pixmap);
    static sk_sp<SkColorSpace> ColorSpaceToSkColorSpace(const RefPtr<PixelMap>& pixmap);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_PROVIDER_H
