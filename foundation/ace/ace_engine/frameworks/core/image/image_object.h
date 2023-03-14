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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_OBJECT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_OBJECT_H

#include "experimental/svg/model/SkSVGDOM.h"

#include "base/image/pixel_map.h"
#include "core/image/animated_image_player.h"
#include "core/image/image_source_info.h"
#include "frameworks/core/components/svg/parse/svg_dom.h"

namespace OHOS::Ace {

class RenderImage;
class ImageObject : public virtual AceType {
    DECLARE_ACE_TYPE(ImageObject, AceType);
public:
    static RefPtr<ImageObject> BuildImageObject(
        ImageSourceInfo source,
        const RefPtr<PipelineContext> context,
        const sk_sp<SkData>& skData,
        bool useSkiaSvg);

    ImageObject() = default;
    ImageObject(
        ImageSourceInfo source,
        const Size& imageSize,
        int32_t frameCount,
        bool isSvg = false)
        : imageSource_(source), imageSize_(imageSize), frameCount_(frameCount), isSvg_(isSvg)
    {}
    virtual ~ImageObject() = default;

    static std::string GenerateCacheKey(const ImageSourceInfo& srcInfo, Size targetSize);

    Size GetImageSize()
    {
        return imageSize_;
    }

    int32_t GetFrameCount()
    {
        return frameCount_;
    }

    ImageSourceInfo GetSourceInfo()
    {
        return imageSource_;
    }

    bool IsSvg() const
    {
        return isSvg_;
    }

    virtual void UploadToGpuForRender(
        const WeakPtr<PipelineContext> context,
        RefPtr<FlutterRenderTaskHolder>& renderTaskHolder,
        UploadSuccessCallback successCallback,
        FailedCallback failedCallback,
        Size imageSize,
        bool forceResize,
        bool syncMode = false)
    {}

    virtual void Pause() {}
    virtual void Resume() {}
    virtual void ClearData() {}

    // this will be called on ui thread when renderImage do perform layout for deferent image objects.
    virtual void PerformLayoutImageObject(RefPtr<RenderImage> image) {}

    // this will be called on ui thread when renderImage do measure for deferent image objects.
    virtual Size MeasureForImage(RefPtr<RenderImage> image);

    virtual bool CancelBackgroundTasks()
    {
        return false;
    }

protected:
    ImageSourceInfo imageSource_;
    Size imageSize_;
    int32_t frameCount_ = 1;
    bool isSvg_ = false;
};

class SvgSkiaImageObject : public ImageObject {
    DECLARE_ACE_TYPE(SvgSkiaImageObject, ImageObject);
public:
    SvgSkiaImageObject(
        ImageSourceInfo source,
        const Size& imageSize,
        int32_t frameCount,
        const sk_sp<SkSVGDOM>& skiaDom)
        : ImageObject(source, imageSize, frameCount, true), skiaDom_(skiaDom)
    {}

    ~SvgSkiaImageObject() override = default;

    const sk_sp<SkSVGDOM>& GetSkiaDom()
    {
        return skiaDom_;
    }

    void PerformLayoutImageObject(RefPtr<RenderImage> image) override;
    Size MeasureForImage(RefPtr<RenderImage> image) override;

private:
    sk_sp<SkSVGDOM> skiaDom_;
};

class SvgImageObject : public ImageObject {
    DECLARE_ACE_TYPE(SvgImageObject, ImageObject);
public:
    SvgImageObject(
        ImageSourceInfo source,
        const Size& imageSize,
        int32_t frameCount,
        const RefPtr<SvgDom>& svgDom)
        : ImageObject(source, imageSize, frameCount, true), svgDom_(svgDom)
    {}

    ~SvgImageObject() override = default;

    const RefPtr<SvgDom>& GetSvgDom()
    {
        return svgDom_;
    }

    void PerformLayoutImageObject(RefPtr<RenderImage> image) override;
    Size MeasureForImage(RefPtr<RenderImage> image) override;

private:
    RefPtr<SvgDom> svgDom_;
};

class StaticImageObject : public ImageObject {
    DECLARE_ACE_TYPE(StaticImageObject, ImageObject);
public:
    using CancelableTask = CancelableCallback<void()>;
    StaticImageObject(
        ImageSourceInfo source,
        const Size& imageSize,
        int32_t frameCount,
        const sk_sp<SkData>& data)
        : ImageObject(source, imageSize, frameCount), skData_(data)
    {}

    ~StaticImageObject() override = default;

    void UploadToGpuForRender(
        const WeakPtr<PipelineContext> context,
        RefPtr<FlutterRenderTaskHolder>& renderTaskHolder,
        UploadSuccessCallback successCallback,
        FailedCallback failedCallback,
        Size imageSize,
        bool forceResize,
        bool syncMode = false) override;

    void ClearData() override
    {
        skData_ = nullptr;
    }

    bool CancelBackgroundTasks() override;

private:
    sk_sp<SkData> skData_;
    CancelableTask uploadForPaintTask_;
};

class AnimatedImageObject : public ImageObject {
    DECLARE_ACE_TYPE(AnimatedImageObject, ImageObject);
public:
    AnimatedImageObject(
        ImageSourceInfo source,
        const Size& imageSize,
        int32_t frameCount,
        const sk_sp<SkData>& data)
        : ImageObject(source, imageSize, frameCount), skData_(data)
    {}

    ~AnimatedImageObject() override = default;

    void UploadToGpuForRender(
        const WeakPtr<PipelineContext> context,
        RefPtr<FlutterRenderTaskHolder>& renderTaskHolder,
        UploadSuccessCallback successCallback,
        FailedCallback failedCallback,
        Size imageSize,
        bool forceResize,
        bool syncMode = false) override;

    void Pause() override
    {
        if (animatedPlayer_) {
            LOGI("animatied image Paused");
            animatedPlayer_->Pause();
        }
    }

    void Resume() override
    {
        if (animatedPlayer_) {
            LOGI("animatied image Resume");
            animatedPlayer_->Resume();
        }
    }

    void ClearData() override
    {
        skData_ = nullptr;
    }

private:
    sk_sp<SkData> skData_;
    RefPtr<AnimatedImagePlayer> animatedPlayer_;
};

class PixelMapImageObject : public ImageObject {
    DECLARE_ACE_TYPE(PixelMapImageObject, ImageObject);

public:
    explicit PixelMapImageObject(const RefPtr<PixelMap>& pixmap) : pixmap_(pixmap)
    {
        imageSize_ = Size(pixmap_->GetWidth(), pixmap_->GetHeight());
    }

    ~PixelMapImageObject() override = default;

    void* GetRawPixelMapPtr()
    {
        return pixmap_->GetRawPixelMapPtr();
    }

    void PerformLayoutImageObject(RefPtr<RenderImage> image) override;
    Size MeasureForImage(RefPtr<RenderImage> image) override;

    void ClearData() override
    {
        pixmap_ = nullptr;
    }

    const RefPtr<PixelMap>& GetPixmap() const
    {
        return pixmap_;
    }

private:
    RefPtr<PixelMap> pixmap_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_OBJECT_H