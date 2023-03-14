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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_FLUTTER_RENDER_IMAGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_FLUTTER_RENDER_IMAGE_H

#include "experimental/svg/model/SkSVGDOM.h"
#include "flutter/fml/memory/ref_counted.h"
#include "flutter/lib/ui/painting/image.h"

#include "core/components/image/render_image.h"
#include "core/image/image_provider.h"
#include "core/pipeline/base/scoped_canvas_state.h"
#include "core/pipeline/layers/offset_layer.h"
#include "frameworks/core/components/svg/parse/svg_dom.h"

namespace OHOS::Ace {

class FlutterRenderImage final : public RenderImage, public ImageProviderLoader {
    DECLARE_ACE_TYPE(FlutterRenderImage, RenderImage);

public:
    FlutterRenderImage();
    ~FlutterRenderImage() override;

    void Update(const RefPtr<Component>& component) override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void UpdateData(const std::string& uri, const std::vector<uint8_t>& memData) override;
    bool IsRepaintBoundary() const override;

    RenderLayer GetRenderLayer() override
    {
        if (IsRepaintBoundary()) {
            if (!layer_) {
                layer_ = AceType::MakeRefPtr<Flutter::OffsetLayer>();
            }
        } else {
            layer_ = nullptr;
        }
        return AceType::RawPtr(layer_);
    }

    void OnHiddenChanged(bool hidden) override;

    void UpLoadImageDataForPaint();

    void PaintBgImage(const flutter::Paint& paint, const Offset& offset, const ScopedCanvas& canvas) const;

    void FetchImageObject() override;

    bool SupportOpacity() override
    {
        return true;
    }

    bool IsSourceWideGamut() const override;
    bool RetryLoading() override;
    static SkColorType PixelFormatToSkColorType(const RefPtr<PixelMap>& pixmap);
    static SkAlphaType AlphaTypeToSkAlphaType(const RefPtr<PixelMap>& pixmap);
    static SkImageInfo MakeSkImageInfoFromPixelMap(const RefPtr<PixelMap>& pixmap);
    static sk_sp<SkColorSpace> ColorSpaceToSkColorSpace(const RefPtr<PixelMap>& pixmap);
    static void UploadImageObjToGpuForRender(
        const RefPtr<ImageObject>& imageObj,
        const WeakPtr<PipelineContext> context,
        RefPtr<FlutterRenderTaskHolder>& renderTaskHolder,
        UploadSuccessCallback uploadSuccessCallback,
        FailedCallback failedCallback,
        Size resizeTarget,
        bool forceResize,
        bool syncMode = false);

    void ImageDataPaintSuccess(const fml::RefPtr<flutter::CanvasImage>& image);
    void ImageObjReady(const RefPtr<ImageObject>& imageObj);
    void ImageObjFailed();
    bool NeedUploadImageObjToGpu();

    void SetFetchImageObjBackgroundTask(CancelableTask task)
    {
        if (fetchImageObjTask_) {
            fetchImageObjTask_.Cancel(false);
        }
        fetchImageObjTask_ = task;
    }

    void OnAppHide() override;
    void OnAppShow() override;
    void OnVisibleChanged() override;

    void PerformLayoutPixmap() override;
    void PerformLayoutSvgImage() override;

    Size MeasureForPixmap() override;
    Size MeasureForSvgImage() override;
    Size MeasureForNormalImage() override;

protected:
    bool MaybeRelease() override;
    void ClearRenderObject() override;
    void LayoutImageObject() override;

private:
    void InitializeCallbacks();
    Size Measure() override;
    void UpdateRenderAltImage(const RefPtr<Component>& component);
    void SetSkRadii(const Radius& radius, SkVector& radii);
    void SetClipRadius();
    void CanvasDrawImageRect(
        const flutter::Paint& paint,
        const Offset& offset,
        const ScopedCanvas& canvas,
        const Rect& paintRect);
    void DrawImageOnCanvas(
        const Rect& srcRect,
        const Rect& dstRect,
        const ScopedCanvas& canvas,
        const flutter::Paint& paint,
        const Offset& dstOffset = Offset()) const;
    void PaintSVGImage(const sk_sp<SkData>& skData, bool onlyLayoutSelf = false);
    void DrawSVGImage(const Offset& offset, ScopedCanvas& canvas);
    void DrawSVGImageCustom(RenderContext& context, const Offset& offset);
    void UpdateLoadSuccessState();
    Rect RecalculateSrcRect(const Size& realImageSize);
    void ApplyColorFilter(flutter::Paint& paint);
    void ApplyInterpolation(flutter::Paint& paint);
    void ApplyBorderRadius(const Offset& offset, const ScopedCanvas& canvas, const Rect& paintRect);
    void AddSvgChild();
    void CreateAnimatedPlayer(const RefPtr<ImageProvider>& provider, SkCodec* codecPtr, bool forceResize);
    bool VerifySkImageDataFromPixmap(const RefPtr<PixelMap>& pixmap) const;
    void PerformLayoutSvgCustom();
    void CancelBackgroundTasks();
    void CacheImageObject();
    void UpdatePixmap(const RefPtr<PixelMap>& pixmap);
    void UpdateSharedMemoryImage(const RefPtr<PipelineContext>& context);
    void ProcessPixmapForPaint();

    sk_sp<SkSVGDOM> skiaDom_;
    RefPtr<SvgDom> svgDom_;
    fml::RefPtr<flutter::CanvasImage> image_;
    bool loadSvgAfterLayout_ = false;
    bool loadSvgOnPaint_ = false; // only load svg trees without box and bind
    RefPtr<Flutter::OffsetLayer> layer_;
    SkVector radii_[4] = { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
    Size formerRawImageSize_;
    bool imageDataNotReady_ = false;

    ImageSourceInfo curSourceInfo_;
    ImageObjSuccessCallback imageObjSuccessCallback_;
    UploadSuccessCallback uploadSuccessCallback_;
    FailedCallback failedCallback_;
    OnPostBackgroundTask onPostBackgroundTask_;
    RefPtr<ImageObject> imageObj_;
    RefPtr<FlutterRenderTaskHolder> renderTaskHolder_;

    CancelableTask fetchImageObjTask_;
    bool backgroundTaskCancled_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_FLUTTER_RENDER_IMAGE_H
