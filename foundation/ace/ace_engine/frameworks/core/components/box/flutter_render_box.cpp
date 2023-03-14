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

#include "core/components/box/flutter_render_box.h"

#include <cmath>

#include "flutter/common/task_runners.h"
#include "flutter/lib/ui/ui_dart_state.h"

#include "third_party/skia/include/core/SkGraphics.h"
#include "third_party/skia/include/core/SkMaskFilter.h"
#include "third_party/skia/include/effects/Sk1DPathEffect.h"
#include "third_party/skia/include/effects/SkDashPathEffect.h"
#include "third_party/skia/include/effects/SkGradientShader.h"
#include "third_party/skia/include/utils/SkParsePath.h"

#include "core/common/frontend.h"
#include "core/components/box/flutter_mask_painter.h"
#include "core/components/common/painter/flutter_decoration_painter.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/border_edge.h"
#include "core/components/common/properties/color.h"
#include "core/components/flex/render_flex.h"
#include "core/components/image/image_component.h"
#include "core/components/transform/flutter_render_transform.h"
#include "core/pipeline/base/flutter_render_context.h"
#include "core/pipeline/base/scoped_canvas_state.h"
#include "core/pipeline/layers/flutter_scene_builder.h"
#include "core/pipeline/layers/picture_layer.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t DOUBLE_WIDTH = 2;
constexpr uint32_t ACCESSIBILITY_FOCUS_COLOR = 0xbf39b500;
constexpr double ACCESSIBILITY_FOCUS_WIDTH = 4.0;
constexpr double ACCESSIBILITY_FOCUS_RADIUS_X = 2.0;
constexpr double ACCESSIBILITY_FOCUS_RADIUS_Y = 2.0;

} // namespace

using namespace Flutter;

FlutterRenderBox::FlutterRenderBox()
{
    auto currentDartState = flutter::UIDartState::Current();
    if (!currentDartState) {
        return;
    }
    renderTaskHolder_ = MakeRefPtr<FlutterRenderTaskHolder>(
        currentDartState->GetSkiaUnrefQueue(),
        currentDartState->GetIOManager(),
        currentDartState->GetTaskRunners().GetIOTaskRunner());

    uploadSuccessCallback_ = [weak = AceType::WeakClaim(this)] (
        ImageSourceInfo sourceInfo, const fml::RefPtr<flutter::CanvasImage>& image) {
        auto renderBox = weak.Upgrade();
        if (!renderBox) {
            LOGE("renderBox upgrade fail when image load success. callback image source info: %{private}s",
                sourceInfo.ToString().c_str());
            return;
        }
        renderBox->ImageDataPaintSuccess(image);
    };

    imageObjSuccessCallback_ = [weak = AceType::WeakClaim(this)](
        ImageSourceInfo info, const RefPtr<ImageObject>& imageObj) {
        auto renderBox = weak.Upgrade();
        if (!renderBox) {
            LOGE("renderBox upgrade fail when image object is ready. callback image source info: %{private}s",
                info.ToString().c_str());
            return;
        }
        renderBox->ImageObjReady(imageObj);
    };

    failedCallback_ = [weak = AceType::WeakClaim(this)](ImageSourceInfo info) {
        auto renderBox = weak.Upgrade();
        if (renderBox) {
            renderBox->ImageObjFailed();
        }
    };

    onPostBackgroundTask_ = [weak = AceType::WeakClaim(this)] (CancelableTask task) {
        auto renderBox = weak.Upgrade();
        if (renderBox) {
            renderBox->SetFetchImageObjBackgroundTask(task);
        }
    };
}

void FlutterRenderBox::Update(const RefPtr<Component>& component)
{
    RenderBox::Update(component);

    // use render image to render background image
    if (backDecoration_) {
        RefPtr<BackgroundImage> backgroundImage = backDecoration_->GetImage();
        UpdateBackgroundImage(backgroundImage);

        RefPtr<BorderImage> borderImage = backDecoration_->GetBorderImage();
        UpdateBorderImageProvider(borderImage);
    }

    if (mask_) {
        mask_->LoadMask(GetContext(), AceType::Claim(this));
    }
}

void FlutterRenderBox::UpdateBorderImageProvider(const RefPtr<BorderImage>& bImage)
{
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("pipeline context is null!");
        return;
    }
    if (!bImage) {
        return;
    }
    borderSrc_ = bImage->GetSrc();
    if (borderSrc_.empty()) {
        LOGE("borderImageSrc is null!");
        return;
    }
    image_ = nullptr;
    imageObj_ = nullptr;
}

void FlutterRenderBox::FetchImageData()
{
    if (backDecoration_) {
        RefPtr<BorderImage> borderImage = backDecoration_->GetBorderImage();
        if (!borderImage) {
            return;
        }
        borderSrc_ = borderImage->GetSrc();
        if (borderSrc_.empty()) {
            LOGE("borderSrc_ is null!");
            return;
        }
        if (image_) {
            return;
        }
        ImageSourceInfo inComingSource(
            borderSrc_,
            Dimension(-1),
            Dimension(-1),
            InternalResource::ResourceId::NO_ID
        );
        ImageProvider::FetchImageObject(
            inComingSource,
            imageObjSuccessCallback_,
            uploadSuccessCallback_,
            failedCallback_,
            GetContext(),
            false,
            false,
            true,
            renderTaskHolder_,
            onPostBackgroundTask_);
    }
}

void FlutterRenderBox::ImageObjReady(const RefPtr<ImageObject>& imageObj)
{
    imageObj_ = imageObj;
    if (imageObj_) {
        imageObj_->UploadToGpuForRender(GetContext(), renderTaskHolder_, uploadSuccessCallback_, failedCallback_,
            Size(0, 0), false, false);
    }
}

void FlutterRenderBox::ImageObjFailed()
{
    image_ = nullptr;
    imageObj_ = nullptr;
    MarkNeedLayout();
}

void FlutterRenderBox::ImageDataPaintSuccess(const fml::RefPtr<flutter::CanvasImage>& image)
{
    image_ = image->image();
    MarkNeedLayout();
}

void FlutterRenderBox::UpdateBackgroundImage(const RefPtr<BackgroundImage>& image)
{
    if (!image) {
        renderImage_ = nullptr;
        return;
    }

    if (!renderImage_) {
        renderImage_ = AceType::DynamicCast<RenderImage>(RenderImage::Create());
        if (!renderImage_) {
            return;
        }
        renderImage_->SetBackgroundImageFlag(true);
        renderImage_->Attach(GetContext());
        renderImage_->RegisterImageUpdateFunc([weakRenderBox = AceType::WeakClaim(this)]() {
            auto box = weakRenderBox.Upgrade();
            if (box) {
                box->MarkNeedLayout();
            }
        });
        renderImage_->RegisterImageRenderFunc([weakRenderBox = AceType::WeakClaim(this)]() {
            auto box = weakRenderBox.Upgrade();
            if (box) {
                box->MarkNeedRender();
            }
        });
    }

    auto imageComponent = AceType::MakeRefPtr<ImageComponent>(image->GetSrc());
    imageComponent->SetImageRepeat(image->GetImageRepeat());
    // set image size, x direction
    renderImage_->SetBgImageSize(image->GetImageSize().GetSizeTypeX(), image->GetImageSize().GetSizeValueX(), true);
    // set image size, y direction
    renderImage_->SetBgImageSize(image->GetImageSize().GetSizeTypeY(), image->GetImageSize().GetSizeValueY(), false);
    renderImage_->SetBgImagePosition(image->GetImagePosition());
    renderImage_->Update(imageComponent);
}

void FlutterRenderBox::PerformLayout()
{
    RenderBox::PerformLayout();

    // calculate repeatParam.
    CalculateRepeatParam();
}

void FlutterRenderBox::UpdateLayer()
{
    float translateX = GetLayoutSize().Width() / 2.0 * (1.0 - scale_);
    float translateY = GetLayoutSize().Height() / 2.0 * (1.0 - scale_);
    Matrix4 translateMatrix = Matrix4::CreateTranslate(translateX, translateY, 0.0);
    Matrix4 scaleMatrix = Matrix4::CreateScale(scale_, scale_, 1.0);
    Matrix4 transformMatrix = translateMatrix * scaleMatrix;
    transformMatrix = FlutterRenderTransform::GetTransformByOffset(transformMatrix, GetGlobalOffset());
    if (transformLayer_) {
        transformLayer_->Update(transformMatrix);
    }
}

void FlutterRenderBox::Paint(RenderContext& context, const Offset& offset)
{
    const auto renderContext = static_cast<FlutterRenderContext*>(&context);

    if (opacity_ == 0) {
        RenderNode::Paint(context, offset);
        return;
    }
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return;
    }
    FetchImageData();
    // global offset and paint size exclude margin
    Rect paintSize = Rect(offset + margin_.GetOffsetInPx(pipeline->GetDipScale()), paintSize_);
    if (useLiteStyle_) {
        Size maxSize;
        maxSize.SetWidth(paintSize_.Width() > GetPaintRect().Width() ? paintSize_.Width() : GetPaintRect().Width());
        maxSize.SetHeight(paintSize_.Height() > GetPaintRect().Height() ?
            paintSize_.Height() : GetPaintRect().Height());
        paintSize.SetSize(maxSize);
    }
    flutter::RRect outerRRect;
    outerRRect.sk_rrect =
        SkRRect::MakeRect(SkRect::MakeLTRB(paintSize.Left(), paintSize.Top(), paintSize.Right(), paintSize.Bottom()));
    SkRect focusRect = SkRect::MakeLTRB(paintSize.Left(), paintSize.Top(), paintSize.Right(), paintSize.Bottom());
    UpdateBlurRRect(outerRRect, offset);

    RefPtr<FlutterDecorationPainter> decorationPainter;
    if (backDecoration_ || frontDecoration_) {
        if (!useLiteStyle_) {
            // consider the effect of padding and margin, we get the adjustedSize
            Size adjustedSize = GetPaintRect().GetSize() - (GetLayoutSize() - paintSize_);
            decorationPainter = AceType::MakeRefPtr<FlutterDecorationPainter>(
                backDecoration_, GetPaintRect(), adjustedSize, pipeline->GetDipScale());
        } else {
            decorationPainter = AceType::MakeRefPtr<FlutterDecorationPainter>(
                backDecoration_, paintSize, paintSize_, pipeline->GetDipScale());
        }
        decorationPainter->SetAlpha(opacity_);
        decorationPainter->SetMargin(margin_);
        decorationPainter->SetRenderImage(renderImage_);
    }

    Color bgColor = pipeline->GetRootBgColor();
    if (backDecoration_) {
        flutter::Canvas* canvas = renderContext->GetCanvas();
        if (canvas == nullptr) {
            LOGE("Paint canvas is null.");
            return;
        }
        outerRRect = decorationPainter->GetBoxOuterRRect(offset);
        UpdateBlurRRect(outerRRect, offset);

        if (GreatNotEqual(backDecoration_->GetWindowBlurProgress(), 0.0)) {
            UpdateWindowBlurProgress(backDecoration_->GetWindowBlurProgress());
            SetWindowBlurStyle(backDecoration_->GetWindowBlurStyle());
            MarkNeedWindowBlur(true);
        }
        decorationPainter->PaintDecoration(offset, canvas->canvas(), context, image_);
        decorationPainter->PaintBlur(outerRRect, canvas->canvas(), backDecoration_->GetBlurRadius(), bgColor);
        auto context = context_.Upgrade();
        if (context->GetIsDeclarative()) {
            decorationPainter->PaintGrayScale(outerRRect, canvas->canvas(), backDecoration_->GetGrayScale(), bgColor);
            decorationPainter->PaintBrightness(outerRRect, canvas->canvas(), backDecoration_->GetBrightness(), bgColor);
            decorationPainter->PaintContrast(outerRRect, canvas->canvas(), backDecoration_->GetContrast(), bgColor);
            decorationPainter->PaintSaturate(outerRRect, canvas->canvas(), backDecoration_->GetSaturate(), bgColor);
            decorationPainter->PaintInvert(outerRRect, canvas->canvas(), backDecoration_->GetInvert(), bgColor);
            decorationPainter->PaintSepia(outerRRect, canvas->canvas(), backDecoration_->GetSepia(), bgColor);
            decorationPainter->PaintHueRotate(outerRRect, canvas->canvas(), backDecoration_->GetHueRotate(), bgColor);
            decorationPainter->PaintColorBlend(outerRRect, canvas->canvas(), backDecoration_->GetColorBlend(), bgColor);
        }
    }

    if (isZoom) {
        UpdateLayer();
    }
    RenderNode::Paint(context, offset);
    if (frontDecoration_) {
        flutter::Canvas* canvas = renderContext->GetCanvas();
        if (canvas == nullptr) {
            LOGE("Paint canvas is null.");
            return;
        }
        decorationPainter->SetDecoration(frontDecoration_);
        decorationPainter->PaintDecoration(offset, canvas->canvas(), context);
        decorationPainter->PaintBlur(outerRRect, canvas->canvas(), frontDecoration_->GetBlurRadius(), bgColor);
        auto context = context_.Upgrade();
        if (context->GetIsDeclarative()) {
            decorationPainter->PaintGrayScale(outerRRect, canvas->canvas(), frontDecoration_->GetGrayScale(), bgColor);
            decorationPainter->PaintBrightness(
                outerRRect, canvas->canvas(), frontDecoration_->GetBrightness(), bgColor);
            decorationPainter->PaintContrast(outerRRect, canvas->canvas(), frontDecoration_->GetContrast(), bgColor);
            decorationPainter->PaintSaturate(outerRRect, canvas->canvas(), frontDecoration_->GetSaturate(), bgColor);
            decorationPainter->PaintInvert(outerRRect, canvas->canvas(), frontDecoration_->GetInvert(), bgColor);
            decorationPainter->PaintSepia(outerRRect, canvas->canvas(), frontDecoration_->GetSepia(), bgColor);
            decorationPainter->PaintHueRotate(outerRRect, canvas->canvas(), frontDecoration_->GetHueRotate(), bgColor);
            decorationPainter->PaintColorBlend(
                outerRRect, canvas->canvas(), frontDecoration_->GetColorBlend(), bgColor);
        }
    }
    if (isAccessibilityFocus_) {
        PaintAccessibilityFocus(focusRect, context);
    }
}

void FlutterRenderBox::PaintAccessibilityFocus(const SkRect& focusRect, RenderContext& context)
{
    flutter::Canvas* canvas = static_cast<FlutterRenderContext&>(context).GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }

    flutter::Paint paint;
    flutter::RRect rRect;
    rRect.sk_rrect.setRectXY(focusRect, ACCESSIBILITY_FOCUS_RADIUS_X, ACCESSIBILITY_FOCUS_RADIUS_Y);
    paint.paint()->setStyle(SkPaint::Style::kStroke_Style);
    paint.paint()->setColor(ACCESSIBILITY_FOCUS_COLOR);
    paint.paint()->setStrokeWidth(ACCESSIBILITY_FOCUS_WIDTH);
    flutter::PaintData paintData;

    canvas->drawRRect(rRect, paint, paintData);
}

SkColorType ConvertToSkColorType(PixelFormat pixelFormat)
{
    SkColorType colorType = kUnknown_SkColorType;
    switch (pixelFormat) {
        case PixelFormat::ALPHA_8: {
            colorType = kAlpha_8_SkColorType;
            break;
        }
        case PixelFormat::RGB_565: {
            colorType = kRGB_565_SkColorType;
            break;
        }
        case PixelFormat::RGBA_F16: {
            colorType = kRGBA_F16_SkColorType;
            break;
        }
        case PixelFormat::RGBA_8888:
        case PixelFormat::BGRA_8888: {
            colorType = kN32_SkColorType;
            break;
        }
        default: {
            LOGE("pixel format not supported.");
            break;
        }
    }

    return colorType;
}

SkAlphaType ConvertToSkAlphaType(AlphaType alphaType)
{
    SkAlphaType skAlphaType = kUnknown_SkAlphaType;
    switch (alphaType) {
        case AlphaType::IMAGE_ALPHA_TYPE_OPAQUE: {
            skAlphaType = kOpaque_SkAlphaType;
            break;
        }
        case AlphaType::IMAGE_ALPHA_TYPE_PREMUL: {
            skAlphaType = kPremul_SkAlphaType;
            break;
        }
        case AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL: {
            skAlphaType = kUnpremul_SkAlphaType;
            break;
        }
        default: {
            LOGE("alpha type not supported.");
            break;
        }
    }
    return skAlphaType;
}

void FlutterRenderBox::DrawOnPixelMap()
{
    if (pixelMap_ == nullptr || clipLayer_ == nullptr) {
        LOGE("pixelMap_ or clipLayer_ is nullptr.");
        return;
    }
    auto width = paintSize_.Width();
    auto height = paintSize_.Height();
    if (LessOrEqual(width, 0.0) || LessOrEqual(height, 0.0)) {
        LOGE("invalidate size.");
        return;
    }
    auto imageInfo = SkImageInfo::Make(
        width, height, SkColorType::kBGRA_8888_SkColorType, SkAlphaType::kOpaque_SkAlphaType);
    SkBitmap tempCache;
    tempCache.allocPixels(imageInfo);
    SkCanvas tempCanvas(tempCache);

    RefPtr<Flutter::FlutterSceneBuilder> flutterSceneBuilder = AceType::MakeRefPtr<Flutter::FlutterSceneBuilder>();
    Offset saveOffset = clipLayer_->GetOffset();
    Offset saveStaticOffset = clipLayer_->GetStaticOffset();
    clipLayer_->SetOffset(0, 0);
    clipLayer_->SetStaticOffset(0, 0);
    clipLayer_->AddToScene(*flutterSceneBuilder, 0.0, 0.0);
    auto scene = flutterSceneBuilder->Build();
    if (!scene) {
        LOGE("scene is nullptr.");
        return;
    }
    auto layerTree = scene->takeLayerTree();
    if (!layerTree) {
        LOGE("layerTree is nullptr.");
        return;
    }
    auto picture = layerTree->Flatten(SkRect::MakeWH(width, height));
    if (!picture) {
        LOGE("picture is nullptr.");
        return;
    }
    clipLayer_->SetOffset(saveOffset.GetX(), saveOffset.GetY());
    clipLayer_->SetStaticOffset(saveStaticOffset.GetX(), saveStaticOffset.GetY());
    tempCanvas.clear(SK_ColorTRANSPARENT);
    tempCanvas.drawPicture(picture.get());

    uint8_t* dstPixels = const_cast<uint8_t*>(pixelMap_->GetPixels());
    if (dstPixels == nullptr) {
        LOGE("dstPixels is nullptr.");
        return;
    }
    SkImageInfo dstImageInfo = SkImageInfo::Make(
        pixelMap_->GetWidth(),
        pixelMap_->GetHeight(),
        ConvertToSkColorType(pixelMap_->GetPixelFormat()),
        ConvertToSkAlphaType(pixelMap_->GetAlphaType()));
    SkPixmap dstPixmap(dstImageInfo, dstPixels, pixelMap_->GetRowBytes());
    tempCache.readPixels(dstPixmap);
}

void FlutterRenderBox::CalculateRepeatParam()
{
    if (backDecoration_) {
        RefPtr<BackgroundImage> backgroundImage = backDecoration_->GetImage();
        if (backgroundImage && renderImage_) {
            renderImage_->SetBgImageBoxPaintSize(paintSize_);
            renderImage_->SetBgImageBoxMarginOffset(margin_.GetOffset());
            LayoutParam param;
            param.SetFixedSize(paintSize_);
            renderImage_->Layout(param);
        }
    }
}

void FlutterRenderBox::DrawLayerForBlur(SkCanvas* canvas, Flutter::ContainerLayer* containerLayer)
{
    if (!canvas || !containerLayer) {
        return;
    }
    for (const auto& layer : containerLayer->GetChildren()) {
        if (!layer) {
            continue;
        }
        auto pictureLayer = AceType::DynamicCast<Flutter::PictureLayer>(layer);
        if (pictureLayer) {
            auto picture = pictureLayer->GetPicture();
            auto offset = pictureLayer->GetOffset();
            if (offset.GetX() >= 0.0 && picture && picture->picture() && picture->picture().get()) {
                canvas->save();
                canvas->translate(offset.GetX(), offset.GetY());
                canvas->drawPicture(picture->picture().get());
                canvas->restore();
                pictureLayer->SetNeedAddToScene(false);
            }
            continue;
        }
        auto container = AceType::DynamicCast<Flutter::ContainerLayer>(layer);
        if (container) {
            DrawLayerForBlur(canvas, AceType::RawPtr(container));
        }
    }
}

SkVector FlutterRenderBox::GetSkRadii(const Radius& radius, double shrinkFactor, double borderWidth)
{
    SkVector fRadii;
    fRadii.set(SkDoubleToScalar(std::max(NormalizeToPx(radius.GetX()) - shrinkFactor * borderWidth, 0.0)),
        SkDoubleToScalar(std::max(NormalizeToPx(radius.GetY()) - shrinkFactor * borderWidth, 0.0)));
    return fRadii;
}

bool FlutterRenderBox::CheckBorderEdgeForRRect(const Border& border)
{
    double leftWidth = NormalizeToPx(border.Left().GetWidth());
    if (NearEqual(leftWidth, NormalizeToPx(border.Top().GetWidth())) &&
        NearEqual(leftWidth, NormalizeToPx(border.Right().GetWidth())) &&
        NearEqual(leftWidth, NormalizeToPx(border.Bottom().GetWidth()))) {
        BorderStyle leftStyle = border.Left().GetBorderStyle();
        return leftStyle == border.Top().GetBorderStyle() && leftStyle == border.Right().GetBorderStyle() &&
               leftStyle == border.Bottom().GetBorderStyle();
    }
    return false;
}

RefPtr<Flutter::ClipLayer> FlutterRenderBox::GetClipLayer()
{
    auto flex = AceType::DynamicCast<RenderFlex>(GetFirstChild());
    bool childOverflow = flex ? isChildOverflow_ || flex->IsChildOverflow() : isChildOverflow_;
    if ((overflow_ != Overflow::CLIP || !childOverflow) && mask_ == nullptr && !boxClipFlag_ &&
        overflow_ != Overflow::FORCE_CLIP && (clipPath_ == nullptr || !clipPath_->NeedClip()) &&
        (pixelMap_ == nullptr)) {
        LOGD("do not need to create clipLayer.");
        return nullptr;
    }

    RefPtr<FlutterMaskPainter> mask = AceType::DynamicCast<FlutterMaskPainter>(mask_);
    if (mask) {
        if (!clipLayer_) {
            clipLayer_ = AceType::MakeRefPtr<ClipLayer>(
                0.0, GetPaintRect().Width(), 0.0, GetPaintRect().Height(), Clip::HARD_EDGE);
        }
        if (mask->HasReady()) {
            if (mask->IsPaintSvgMask()) {
                mask->GetSVGImageDom(clipLayer_);
            } else if (mask->IsColorGradient()) {
                mask->GetGradientPaint(GetPaintRect(), clipLayer_);
            } else if (mask->IsPath() && mask->GetMaskPath()) {
                SkPath skPath;
                bool ret = CreateSkPath(mask->GetMaskPath()->GetBasicShape(),
                    mask->GetMaskPath()->GetGeometryBoxType(), &skPath);
                if (!ret) {
                    if (mask) {
                        return clipLayer_;
                    }
                    return nullptr;
                }
                mask->GetPathPaint(GetPaintRect(), skPath, clipLayer_);
            }
        } else {
            clipLayer_->CancelMask();
        }
    }

    if (clipPath_ && clipPath_->NeedClip()) {
        SkPath skPath;
        bool ret = CreateSkPath(clipPath_->GetBasicShape(), clipPath_->GetGeometryBoxType(), &skPath);
        if (!ret) {
            if (mask) {
                return clipLayer_;
            }
            return nullptr;
        }
        if (!clipLayer_) {
            clipLayer_ = AceType::MakeRefPtr<ClipLayer>(
                0.0, GetPaintRect().Width(), 0.0, GetPaintRect().Height(), Clip::ANTI_ALIAS);
        }
        fml::RefPtr<flutter::CanvasPath> canvasPath = flutter::CanvasPath::CreateFrom(skPath);
        clipLayer_->SetClipPath(canvasPath);
        return clipLayer_;
    } else {
        if (!clipLayer_) {
            clipLayer_ = AceType::MakeRefPtr<ClipLayer>(
                0.0, GetPaintRect().Width(), 0.0, GetPaintRect().Height(), Clip::ANTI_ALIAS);
        }
        Border border;
        if (backDecoration_) {
            border = backDecoration_->GetBorder();
        }
        flutter::RRect outerRRect = GetBoxRRect(margin_.GetOffset(), border, 0.0, true);
        clipLayer_->SetClipRRect(outerRRect);
        return clipLayer_;
    }
    return nullptr;
}

RenderLayer FlutterRenderBox::GetRenderLayer()
{
    renderLayer_ = GetClipLayer();

    if (isUseAlign_ && !alignOffset_.IsZero()) {
        if (!renderLayer_) {
            if (!offsetLayer_) {
                offsetLayer_ = AceType::MakeRefPtr<OffsetLayer>();
            }
            renderLayer_ = offsetLayer_;
        }
        renderLayer_->SetStaticOffset(alignOffset_.GetX(), alignOffset_.GetY());
    }
    if (isZoom) {
        if (!transformLayer_) {
            transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
        }
        return AceType::RawPtr(transformLayer_);
    } else {
        return AceType::RawPtr(renderLayer_);
    }
}

flutter::RRect FlutterRenderBox::GetBoxRRect(
    const Offset& offset, const Border& border, double shrinkFactor, bool isRound)
{
    flutter::RRect rRect = flutter::RRect();
    SkRect skRect {};
    SkVector fRadii[4] = { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
    Size adjustedSize = GetPaintRect().GetSize() - (GetLayoutSize() - paintSize_);
    if (CheckBorderEdgeForRRect(border)) {
        BorderEdge borderEdge = border.Left();
        double borderWidth = NormalizeToPx(borderEdge.GetWidth());
        skRect.setXYWH(SkDoubleToScalar(offset.GetX() + shrinkFactor * borderWidth),
            SkDoubleToScalar(offset.GetY() + shrinkFactor * borderWidth),
            SkDoubleToScalar(adjustedSize.Width() - shrinkFactor * DOUBLE_WIDTH * borderWidth),
            SkDoubleToScalar(adjustedSize.Height() - shrinkFactor * DOUBLE_WIDTH * borderWidth));
        if (isRound) {
            fRadii[SkRRect::kUpperLeft_Corner] = GetSkRadii(border.TopLeftRadius(), shrinkFactor, borderWidth);
            fRadii[SkRRect::kUpperRight_Corner] = GetSkRadii(border.TopRightRadius(), shrinkFactor, borderWidth);
            fRadii[SkRRect::kLowerRight_Corner] = GetSkRadii(border.BottomRightRadius(), shrinkFactor, borderWidth);
            fRadii[SkRRect::kLowerLeft_Corner] = GetSkRadii(border.BottomLeftRadius(), shrinkFactor, borderWidth);
        }
    } else {
        float offsetX = SkDoubleToScalar(offset.GetX() + shrinkFactor * NormalizeToPx(border.Left().GetWidth()));
        float offsetY = SkDoubleToScalar(offset.GetY() + shrinkFactor * NormalizeToPx(border.Top().GetWidth()));
        float width = SkDoubleToScalar(
            adjustedSize.Width() - shrinkFactor * DOUBLE_WIDTH * NormalizeToPx(border.Right().GetWidth()));
        float height = SkDoubleToScalar(
            adjustedSize.Height() - shrinkFactor * DOUBLE_WIDTH * NormalizeToPx(border.Bottom().GetWidth()));
        skRect.setXYWH(offsetX, offsetY, width, height);
    }
    rRect.sk_rrect.setRectRadii(skRect, fRadii);
    return rRect;
}

bool FlutterRenderBox::MaybeRelease()
{
    auto context = GetContext().Upgrade();
    if (context && context->GetRenderFactory() && context->GetRenderFactory()->GetRenderBoxFactory()->Recycle(this)) {
        ClearRenderObject();
        return false;
    }
    return true;
}

void FlutterRenderBox::UpdateBlurRRect(const flutter::RRect& rRect, const Offset& offset)
{
    //  radius of four edge should be same
    SkVector radius = rRect.sk_rrect.radii(SkRRect::kUpperLeft_Corner);
    const SkRect& rect = rRect.sk_rrect.rect();
    windowBlurRRect_.SetRectWithSimpleRadius(
        Rect(rect.left(), rect.top(), rect.width(), rect.height()), radius.fX, radius.fY);
    // this is relative offset
    Rect innerRect = windowBlurRRect_.GetRect();
    innerRect -= offset;
    windowBlurRRect_.SetRect(innerRect);
}

bool FlutterRenderBox::CreateSkPath(const RefPtr<BasicShape>& basicShape,
                                    GeometryBoxType geometryBoxType,
                                    SkPath *skPath)
{
    Size size;
    Offset position;
    GetSizeAndPosition(geometryBoxType, size, position);
    if (basicShape == nullptr || basicShape->GetBasicShapeType() == BasicShapeType::NONE) {
        skPath->addRect(SkRect::MakeXYWH(position.GetX(), position.GetY(), size.Width(), size.Height()));
        return true;
    }
    bool ret = false;
    switch (basicShape->GetBasicShapeType()) {
        case BasicShapeType::INSET: {
            ret = CreateInset(basicShape, size, position, skPath);
            break;
        }
        case BasicShapeType::CIRCLE: {
            ret = CreateCircle(basicShape, size, position, skPath);
            break;
        }
        case BasicShapeType::ELLIPSE: {
            ret = CreateEllipse(basicShape, size, position, skPath);
            break;
        }
        case BasicShapeType::POLYGON: {
            ret = CreatePolygon(basicShape, size, position, skPath);
            break;
        }
        case BasicShapeType::PATH: {
            ret = CreatePath(basicShape, size, position, skPath);
            break;
        }
        case BasicShapeType::RECT: {
            ret = CreateRect(basicShape, size, position, skPath);
            break;
        }
        default: {
            LOGE("invalid BasicShapeType");
            ret = false;
            break;
        }
    }
    return ret;
}

bool FlutterRenderBox::CreateInset(const RefPtr<BasicShape>& basicShape,
                                   const Size& size,
                                   const Offset& position,
                                   SkPath* skPath)
{
    const auto& inset = AceType::DynamicCast<Inset>(basicShape);
    if (!inset) {
        LOGE("inset is null");
        return false;
    }
    double left = DimensionToPx(inset->GetLeft(), size, LengthMode::HORIZONTAL) + position.GetX();
    double top = DimensionToPx(inset->GetTop(), size, LengthMode::VERTICAL) + position.GetY();
    double right = size.Width() - DimensionToPx(inset->GetRight(), size, LengthMode::HORIZONTAL) + position.GetX();
    double bottom = size.Height() - DimensionToPx(inset->GetBottom(), size, LengthMode::VERTICAL) + position.GetY();
    SkRect rect = SkRect::MakeLTRB(left, top, right, bottom);
    auto radiusSize = Size(std::abs(rect.width()), std::abs(rect.height()));
    float topLeftRadiusX = DimensionToPx(inset->GetTopLeftRadius().GetX(), radiusSize, LengthMode::HORIZONTAL);
    float topLeftRadiusY = DimensionToPx(inset->GetTopLeftRadius().GetY(), radiusSize, LengthMode::VERTICAL);
    float topRightRadiusX = DimensionToPx(inset->GetTopRightRadius().GetX(), radiusSize, LengthMode::HORIZONTAL);
    float topRightRadiusY = DimensionToPx(inset->GetTopRightRadius().GetY(), radiusSize, LengthMode::VERTICAL);
    float bottomRightRadiusX = DimensionToPx(inset->GetBottomRightRadius().GetX(), radiusSize, LengthMode::HORIZONTAL);
    float bottomRightRadiusY = DimensionToPx(inset->GetBottomRightRadius().GetY(), radiusSize, LengthMode::VERTICAL);
    float bottomLeftRadiusX = DimensionToPx(inset->GetBottomLeftRadius().GetX(), radiusSize, LengthMode::HORIZONTAL);
    float bottomLeftRadiusY = DimensionToPx(inset->GetBottomLeftRadius().GetY(), radiusSize, LengthMode::VERTICAL);
    const SkVector fRadii[4] = {
        { topLeftRadiusX, topLeftRadiusY },
        { topRightRadiusX, topRightRadiusY },
        { bottomRightRadiusX, bottomRightRadiusY },
        { bottomLeftRadiusX, bottomLeftRadiusY }
    };
    SkRRect roundRect;
    roundRect.setRectRadii(rect, fRadii);
    skPath->addRRect(roundRect);
    return true;
}

bool FlutterRenderBox::CreateCircle(const RefPtr<BasicShape>& basicShape,
                                    const Size& size,
                                    const Offset& position,
                                    SkPath* skPath)
{
    const auto& circle = AceType::DynamicCast<Circle>(basicShape);
    if (!circle) {
        LOGE("circle is null");
        return false;
    }
    if (circle->GetRadius().IsValid()) {
        skPath->addCircle(DimensionToPx(circle->GetAxisX(), size, LengthMode::HORIZONTAL) + position.GetX(),
            DimensionToPx(circle->GetAxisY(), size, LengthMode::VERTICAL) + position.GetY(),
            DimensionToPx(circle->GetRadius(), size, LengthMode::OTHER));
    } else {
        float width = DimensionToPx(circle->GetWidth(), size, LengthMode::HORIZONTAL);
        float height = DimensionToPx(circle->GetHeight(), size, LengthMode::VERTICAL);
        float offsetX = DimensionToPx(circle->GetOffset().GetX(), size, LengthMode::HORIZONTAL) + position.GetX();
        float offsetY = DimensionToPx(circle->GetOffset().GetY(), size, LengthMode::VERTICAL) + position.GetY();
        skPath->addCircle(width * 0.5 + offsetX, height * 0.5 + offsetY, std::min(width, height) * 0.5);
    }

    return true;
}

bool FlutterRenderBox::CreateEllipse(const RefPtr<BasicShape>& basicShape,
                                     const Size& size,
                                     const Offset& position,
                                     SkPath* skPath)
{
    const auto& ellipse = AceType::DynamicCast<Ellipse>(basicShape);
    if (!ellipse) {
        LOGE("ellipse is null");
        return false;
    }
    if (ellipse->GetRadiusX().IsValid()) {
        float rx = DimensionToPx(ellipse->GetRadiusX(), size, LengthMode::HORIZONTAL);
        float ry = DimensionToPx(ellipse->GetRadiusY(), size, LengthMode::VERTICAL);
        double x = DimensionToPx(ellipse->GetAxisX(), size, LengthMode::HORIZONTAL) + position.GetX() - rx;
        double y = DimensionToPx(ellipse->GetAxisY(), size, LengthMode::VERTICAL) + position.GetY() - ry;
        SkRect rect = SkRect::MakeXYWH(x, y, rx + rx, ry + ry);
        skPath->addOval(rect);
    } else {
        auto width = DimensionToPx(ellipse->GetWidth(), size, LengthMode::HORIZONTAL);
        auto height = DimensionToPx(ellipse->GetHeight(), size, LengthMode::VERTICAL);
        float x = DimensionToPx(ellipse->GetOffset().GetX(), size, LengthMode::HORIZONTAL) + position.GetX();
        float y = DimensionToPx(ellipse->GetOffset().GetY(), size, LengthMode::VERTICAL) + position.GetY();
        SkRect rect = SkRect::MakeXYWH(x, y, width, height);
        skPath->addOval(rect);
    }
    return true;
}

bool FlutterRenderBox::CreatePolygon(const RefPtr<BasicShape>& basicShape,
                                     const Size& size,
                                     const Offset& position,
                                     SkPath* skPath)
{
    const auto& polygon = AceType::DynamicCast<Polygon>(basicShape);
    if (!polygon) {
        LOGE("polygon is null");
        return false;
    }
    std::vector<SkPoint> skPoints;
    for (auto [x, y] : polygon->GetPoints()) {
        skPoints.emplace_back(SkPoint::Make(
            DimensionToPx(x, size, LengthMode::HORIZONTAL) + position.GetX(),
            DimensionToPx(y, size, LengthMode::VERTICAL) + position.GetX()));
    }
    if (skPoints.empty()) {
        LOGW("points is null");
        return false;
    }
    skPath->addPoly(&skPoints[0], skPoints.size(), true);
    return true;
}

bool FlutterRenderBox::CreatePath(const RefPtr<BasicShape>& basicShape,
                                  const Size& size,
                                  const Offset& position,
                                  SkPath* skPath)
{
    const auto& path = AceType::DynamicCast<Path>(basicShape);
    if (!path) {
        LOGE("path is null");
        return false;
    }
    if (path->GetValue().empty()) {
        LOGW("path value is null");
        return false;
    }
    SkPath tmpPath;
    bool ret = SkParsePath::FromSVGString(path->GetValue().c_str(), &tmpPath);
    if (!ret) {
        LOGW("path value is invalid");
        return false;
    }
    float offsetX = DimensionToPx(path->GetOffset().GetX(), size, LengthMode::HORIZONTAL) + position.GetX();
    float offsetY = DimensionToPx(path->GetOffset().GetY(), size, LengthMode::VERTICAL) + position.GetY();
    skPath->addPath(tmpPath, offsetX, offsetY);
    return true;
}

bool FlutterRenderBox::CreateRect(const RefPtr<BasicShape>& basicShape,
                                  const Size& size,
                                  const Offset& position,
                                  SkPath* skPath)
{
    const auto& rect = AceType::DynamicCast<ShapeRect>(basicShape);
    if (!rect) {
        LOGE("rect is null");
        return false;
    }
    double left = DimensionToPx(rect->GetOffset().GetX(), size, LengthMode::HORIZONTAL) + position.GetX();
    double top = DimensionToPx(rect->GetOffset().GetY(), size, LengthMode::VERTICAL) + position.GetY();
    double width = DimensionToPx(rect->GetWidth(), size, LengthMode::HORIZONTAL);
    double height = DimensionToPx(rect->GetHeight(), size, LengthMode::VERTICAL);
    SkRect skRect = SkRect::MakeXYWH(left, top, width, height);
    auto radiusSize = Size(width, height);
    float topLeftRadiusX = GetFloatRadiusValue(
        rect->GetTopLeftRadius().GetX(), rect->GetTopLeftRadius().GetY(), radiusSize, LengthMode::HORIZONTAL);
    float topLeftRadiusY = GetFloatRadiusValue(
        rect->GetTopLeftRadius().GetY(), rect->GetTopLeftRadius().GetX(), radiusSize, LengthMode::VERTICAL);
    float topRightRadiusX = GetFloatRadiusValue(
        rect->GetTopRightRadius().GetX(), rect->GetTopRightRadius().GetY(), radiusSize, LengthMode::HORIZONTAL);
    float topRightRadiusY = GetFloatRadiusValue(
        rect->GetTopRightRadius().GetY(), rect->GetTopRightRadius().GetX(), radiusSize, LengthMode::VERTICAL);
    float bottomRightRadiusX = GetFloatRadiusValue(
        rect->GetBottomRightRadius().GetX(), rect->GetBottomRightRadius().GetY(), radiusSize, LengthMode::HORIZONTAL);
    float bottomRightRadiusY = GetFloatRadiusValue(
        rect->GetBottomRightRadius().GetY(), rect->GetBottomRightRadius().GetX(), radiusSize, LengthMode::VERTICAL);
    float bottomLeftRadiusX = GetFloatRadiusValue(
        rect->GetBottomLeftRadius().GetX(), rect->GetBottomLeftRadius().GetY(), radiusSize, LengthMode::HORIZONTAL);
    float bottomLeftRadiusY = GetFloatRadiusValue(
        rect->GetBottomLeftRadius().GetY(), rect->GetBottomLeftRadius().GetX(), radiusSize, LengthMode::VERTICAL);
    const SkVector fRadii[4] = {
        { topLeftRadiusX, topLeftRadiusY },
        { topRightRadiusX, topRightRadiusY },
        { bottomRightRadiusX, bottomRightRadiusY },
        { bottomLeftRadiusX, bottomLeftRadiusY }
    };
    SkRRect roundRect;
    roundRect.setRectRadii(skRect, fRadii);
    skPath->addRRect(roundRect);
    return true;
}

float FlutterRenderBox::GetFloatRadiusValue(
    const Dimension& src, const Dimension& dest, const Size& size, LengthMode type)
{
    if (src.Value() < 0.0 && dest.Value() > 0.0) {
        return DimensionToPx(dest, size, type);
    }
    return DimensionToPx(src, size, type);
}

void FlutterRenderBox::GetSizeAndPosition(GeometryBoxType geometryBoxType, Size& size, Offset& position)
{
    switch (geometryBoxType) {
        case GeometryBoxType::BORDER_BOX:
            size = GetPaintSize();
            position = GetPaintPosition();
            break;
        case GeometryBoxType::MARGIN_BOX:
            size = GetLayoutSize();
            position = Offset();
            break;
        case GeometryBoxType::PADDING_BOX:
            size = GetPaintSize() - GetBorderSize();
            position = GetPaintPosition() + GetBorderOffset();
            break;
        case GeometryBoxType::CONTENT_BOX:
            size = GetPaintSize() - GetBorderSize() - GetPaddingSize();
            position = GetPaintPosition() + GetBorderOffset() + padding_.GetOffset();
            break;
        default:
            size = GetPaintSize();
            position = GetPaintPosition();
            break;
    }
}

float FlutterRenderBox::DimensionToPx(const Dimension& value, const Size& size, LengthMode type) const
{
    switch (value.Unit()) {
        case DimensionUnit::PERCENT: {
            if (type == LengthMode::HORIZONTAL) {
                return SkDoubleToScalar(value.Value() * size.Width());
            }
            if (type == LengthMode::VERTICAL) {
                return SkDoubleToScalar(value.Value() * size.Height());
            }
            if (type == LengthMode::OTHER) {
                return SkDoubleToScalar(value.Value() * sqrt(size.Width() * size.Height()));
            }
            return 0.0f;
        }
        case DimensionUnit::PX:
            return SkDoubleToScalar(value.Value());
        default:
            return SkDoubleToScalar(NormalizeToPx(value));
    }
}

} // namespace OHOS::Ace
