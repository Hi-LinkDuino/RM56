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

#include "core/components/box/rosen_render_box.h"

#include <cmath>

#include "animation/rs_animation_timing_protocol.h"
#include "flutter/common/task_runners.h"
#include "render_service_client/core/ui/rs_node.h"
#include "third_party/skia/include/effects/SkGradientShader.h"
#include "third_party/skia/include/utils/SkParsePath.h"

#include "core/common/frontend.h"
#include "core/components/box/rosen_mask_painter.h"
#include "core/components/common/painter/rosen_decoration_painter.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/border_edge.h"
#include "core/components/common/properties/color.h"
#include "core/components/flex/render_flex.h"
#include "core/components/image/image_component.h"
#include "core/components/image/rosen_render_image.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t DOUBLE_WIDTH = 2;
constexpr int32_t HOVER_ANIMATION_DURATION = 250;
constexpr float SCALE_DEFAULT = 1.0f;
constexpr float SCALE_CHANGED = 1.05f;
const Color BOARD_CHANGED = Color::FromRGBO(0, 0, 0, 0.05);
const Rosen::RSAnimationTimingCurve SCALE_ANIMATION_TIMING_CURVE =
    Rosen::RSAnimationTimingCurve::CreateCubicCurve(0.2f, 0.0f, 0.2f, 1.0f);
constexpr uint32_t ACCESSIBILITY_FOCUS_COLOR = 0xbf39b500;
constexpr double ACCESSIBILITY_FOCUS_WIDTH = 4.0;
constexpr double ACCESSIBILITY_FOCUS_RADIUS_X = 2.0;
constexpr double ACCESSIBILITY_FOCUS_RADIUS_Y = 2.0;
} // namespace

RosenRenderBox::RosenRenderBox()
{
    auto currentDartState = flutter::UIDartState::Current();
    if (!currentDartState) {
        return;
    }
    renderTaskHolder_ = MakeRefPtr<FlutterRenderTaskHolder>(currentDartState->GetSkiaUnrefQueue(),
        currentDartState->GetIOManager(), currentDartState->GetTaskRunners().GetIOTaskRunner());

    uploadSuccessCallback_ = [weak = AceType::WeakClaim(this)](
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

    onPostBackgroundTask_ = [weak = AceType::WeakClaim(this)](CancelableTask task) {
        auto renderBox = weak.Upgrade();
        if (renderBox) {
            renderBox->SetFetchImageObjBackgroundTask(task);
        }
    };
}

void RosenRenderBox::Update(const RefPtr<Component>& component)
{
    RenderBox::Update(component);
    SyncDecorationToRSNode();

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

void RosenRenderBox::UpdateBorderImageProvider(const RefPtr<BorderImage>& bImage)
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

void RosenRenderBox::FetchImageData()
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
        ImageSourceInfo inComingSource(borderSrc_, Dimension(-1), Dimension(-1), InternalResource::ResourceId::NO_ID);
        ImageProvider::FetchImageObject(inComingSource, imageObjSuccessCallback_, uploadSuccessCallback_,
            failedCallback_, GetContext(), false, false, true, renderTaskHolder_, onPostBackgroundTask_);
    }
}

void RosenRenderBox::ImageObjReady(const RefPtr<ImageObject>& imageObj)
{
    imageObj_ = imageObj;
    if (imageObj_) {
        imageObj_->UploadToGpuForRender(
            GetContext(), renderTaskHolder_, uploadSuccessCallback_, failedCallback_, Size(0, 0), false, false);
    }
}

void RosenRenderBox::ImageObjFailed()
{
    image_ = nullptr;
    imageObj_ = nullptr;
    MarkNeedLayout();
}

void RosenRenderBox::ImageDataPaintSuccess(const fml::RefPtr<flutter::CanvasImage>& image)
{
    image_ = image->image();
    MarkNeedLayout();
}

void RosenRenderBox::UpdateBackgroundImage(const RefPtr<BackgroundImage>& image)
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

void RosenRenderBox::PerformLayout()
{
    RenderBox::PerformLayout();

    // calculate repeatParam.
    CalculateRepeatParam();

    // in case margin changed, have to call these function even if size/pos not changed.
    MarkNeedSyncGeometryProperties();

    auto flex = AceType::DynamicCast<RenderFlex>(GetFirstChild());
    bool childOverflow = flex ? isChildOverflow_ || flex->IsChildOverflow() : isChildOverflow_;
    if ((overflow_ != Overflow::CLIP || !childOverflow) && mask_ == nullptr && !boxClipFlag_ &&
        overflow_ != Overflow::FORCE_CLIP && (clipPath_ == nullptr || !clipPath_->NeedClip())) {
        LOGD("do not need to clip.");
        return;
    }

    auto rsNode = GetRSNode();
    if (!rsNode) {
        return;
    }

    rsNode->SetClipToBounds(true);
#ifdef OHOS_PLATFORM
    if (mask_ && mask_->IsValid() && mask_->IsPath()) {
        SkPath skPath;
        bool ret =
            CreateSkPath(mask_->GetMaskPath()->GetBasicShape(), mask_->GetMaskPath()->GetGeometryBoxType(), &skPath);
        if (!ret) {
            return;
        }
        // border should have be the same shape as mask
        rsNode->SetClipBounds(Rosen::RSPath::CreateRSPath(skPath));
    } else if (clipPath_ && clipPath_->NeedClip()) {
        SkPath skPath;
        bool ret = CreateSkPath(clipPath_->GetBasicShape(), clipPath_->GetGeometryBoxType(), &skPath);
        if (!ret) {
            return;
        }
        rsNode->SetClipBounds(Rosen::RSPath::CreateRSPath(skPath));
    }
#endif
}

void RosenRenderBox::Paint(RenderContext& context, const Offset& offset)
{
    if (backDecoration_ && backDecoration_->GetImage() && renderImage_ &&
        backDecoration_->GetBackgroundColor() == Color::TRANSPARENT) {
        // draw background image on current context/canvas
        renderImage_->RenderWithContext(context, offset);
    }
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return;
    }
    SyncDecorationToRSNode();
    Rect paintSize = Rect(offset + margin_.GetOffsetInPx(pipeline->GetDipScale()), paintSize_);
    if (useLiteStyle_) {
        Size maxSize;
        maxSize.SetWidth(paintSize_.Width() > GetPaintRect().Width() ? paintSize_.Width() : GetPaintRect().Width());
        maxSize.SetHeight(
            paintSize_.Height() > GetPaintRect().Height() ? paintSize_.Height() : GetPaintRect().Height());
        paintSize.SetSize(maxSize);
    }
    SkRRect outerRRect =
        SkRRect::MakeRect(SkRect::MakeLTRB(paintSize.Left(), paintSize.Top(), paintSize.Right(), paintSize.Bottom()));
        SkRect focusRect = SkRect::MakeLTRB(paintSize.Left(), paintSize.Top(), paintSize.Right(), paintSize.Bottom());
    Color bgColor = pipeline->GetRootBgColor();
    if (backDecoration_) {
        auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
        if (canvas == nullptr) {
            LOGE("Paint canvas is null.");
            return;
        }
        auto context = context_.Upgrade();
        if (context->GetIsDeclarative()) {
            RosenDecorationPainter::PaintGrayScale(outerRRect, canvas, backDecoration_->GetGrayScale(), bgColor);
            RosenDecorationPainter::PaintBrightness(outerRRect, canvas, backDecoration_->GetBrightness(), bgColor);
            RosenDecorationPainter::PaintContrast(outerRRect, canvas, backDecoration_->GetContrast(), bgColor);
            RosenDecorationPainter::PaintSaturate(outerRRect, canvas, backDecoration_->GetSaturate(), bgColor);
            RosenDecorationPainter::PaintInvert(outerRRect, canvas, backDecoration_->GetInvert(), bgColor);
            RosenDecorationPainter::PaintSepia(outerRRect, canvas, backDecoration_->GetSepia(), bgColor);
            RosenDecorationPainter::PaintHueRotate(outerRRect, canvas, backDecoration_->GetHueRotate(), bgColor);
            RosenDecorationPainter::PaintColorBlend(outerRRect, canvas, backDecoration_->GetColorBlend(), bgColor);
        }
    }
    RenderNode::Paint(context, offset);
    if (frontDecoration_) {
        auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
        if (canvas == nullptr) {
            LOGE("Paint canvas is null.");
            return;
        }
        auto context = context_.Upgrade();
        if (context->GetIsDeclarative()) {
            RosenDecorationPainter::PaintGrayScale(outerRRect, canvas, frontDecoration_->GetGrayScale(), bgColor);
            RosenDecorationPainter::PaintBrightness(outerRRect, canvas, frontDecoration_->GetBrightness(), bgColor);
            RosenDecorationPainter::PaintContrast(outerRRect, canvas, frontDecoration_->GetContrast(), bgColor);
            RosenDecorationPainter::PaintSaturate(outerRRect, canvas, frontDecoration_->GetSaturate(), bgColor);
            RosenDecorationPainter::PaintInvert(outerRRect, canvas, frontDecoration_->GetInvert(), bgColor);
            RosenDecorationPainter::PaintSepia(outerRRect, canvas, frontDecoration_->GetSepia(), bgColor);
            RosenDecorationPainter::PaintHueRotate(outerRRect, canvas, frontDecoration_->GetHueRotate(), bgColor);
            RosenDecorationPainter::PaintColorBlend(outerRRect, canvas, frontDecoration_->GetColorBlend(), bgColor);
        }
    }

    if (isAccessibilityFocus_) {
        PaintAccessibilityFocus(focusRect, context);
    }

    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    auto rsNode = renderContext->GetRSNode();
    if (rsNode == nullptr) {
        return;
    }

#ifdef OHOS_PLATFORM
    RefPtr<RosenMaskPainter> mask = AceType::DynamicCast<RosenMaskPainter>(mask_);
    if (mask && mask->HasReady()) {
        SkPath skPath;
        if (mask_->IsPath()) {
            if (!CreateSkPath(mask_->GetMaskPath()->GetBasicShape(),
                mask_->GetMaskPath()->GetGeometryBoxType(), &skPath)) {
                LOGE("CreateSkPath is failed.");
                return;
            }
        }
        rsNode->SetMask(mask->GetRSMask(GetPaintRect(), skPath));
    }
#endif

    if ((!backDecoration_) || backDecoration_->GetImage() ||
        (backDecoration_->GetBackgroundColor() != Color::TRANSPARENT) || !(backDecoration_->GetGradient().IsValid())) {
        // no need to paint gradient
        return;
    }

    auto size = SkSize::Make(GetLayoutSize().Width(), GetLayoutSize().Height());
    auto shader = RosenDecorationPainter::CreateGradientShader(backDecoration_->GetGradient(), size, dipScale_);
#ifdef OHOS_PLATFORM
    rsNode->SetBackgroundShader(Rosen::RSShader::CreateRSShader(shader));
#endif
}

SkColorType ConvertToSkColorType(PixelFormat pixelFormat);

SkAlphaType ConvertToSkAlphaType(AlphaType alphaType);

void RosenRenderBox::PaintAccessibilityFocus(const SkRect& focusRect, RenderContext& context)
{
    auto canvas = static_cast<RosenRenderContext&>(context).GetCanvas();
    if (canvas == nullptr) {
        LOGE("Canvas is null, save failed.");
        return;
    }
    canvas->save();

    SkPaint paint;
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setColor(ACCESSIBILITY_FOCUS_COLOR);
    paint.setStrokeWidth(ACCESSIBILITY_FOCUS_WIDTH);

    SkRRect rRect = SkRRect::MakeRectXY(focusRect, ACCESSIBILITY_FOCUS_RADIUS_X, ACCESSIBILITY_FOCUS_RADIUS_Y);
    canvas->drawRRect(rRect, paint);
    canvas->restore();
}

void RosenRenderBox::DrawOnPixelMap()
{
    if (pixelMap_ == nullptr) {
        LOGE("pixelMap_ or clipLayer_ is nullptr.");
        return;
    }
    auto width = paintSize_.Width();
    auto height = paintSize_.Height();
    if (LessOrEqual(width, 0.0) || LessOrEqual(height, 0.0)) {
        LOGE("invalidate size.");
        return;
    }
    auto imageInfo =
        SkImageInfo::Make(width, height, SkColorType::kBGRA_8888_SkColorType, SkAlphaType::kOpaque_SkAlphaType);
    SkBitmap tempCache;
    tempCache.allocPixels(imageInfo);
    SkCanvas tempCanvas(tempCache);

    uint8_t* dstPixels = const_cast<uint8_t*>(pixelMap_->GetPixels());
    if (dstPixels == nullptr) {
        LOGE("dstPixels is nullptr.");
        return;
    }
    SkImageInfo dstImageInfo = SkImageInfo::Make(pixelMap_->GetWidth(), pixelMap_->GetHeight(),
        ConvertToSkColorType(pixelMap_->GetPixelFormat()), ConvertToSkAlphaType(pixelMap_->GetAlphaType()));
    SkPixmap dstPixmap(dstImageInfo, dstPixels, pixelMap_->GetRowBytes());
    tempCache.readPixels(dstPixmap);
}

void RosenRenderBox::CalculateRepeatParam()
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

SkVector RosenRenderBox::GetSkRadii(const Radius& radius, double shrinkFactor, double borderWidth)
{
    SkVector fRadii;
    fRadii.set(SkDoubleToScalar(std::max(NormalizeToPx(radius.GetX()) - shrinkFactor * borderWidth, 0.0)),
        SkDoubleToScalar(std::max(NormalizeToPx(radius.GetY()) - shrinkFactor * borderWidth, 0.0)));
    return fRadii;
}

bool RosenRenderBox::CheckBorderEdgeForRRect(const Border& border)
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

void RosenRenderBox::SyncGeometryProperties()
{
    auto rsNode = GetRSNode();
    if (!rsNode) {
        return;
    }
    Offset boundsOffset = GetPaintOffset() + margin_.GetOffset();
    Size boundsSize = paintSize_;
    rsNode->SetBounds(boundsOffset.GetX(), boundsOffset.GetY(), boundsSize.Width(), boundsSize.Height());
    if (IsTailRenderNode()) {
        Offset frameOffset = GetPaintOffset();
        Size frameSize = GetLayoutSize();
        rsNode->SetFrame(frameOffset.GetX(), frameOffset.GetY(), frameSize.Width(), frameSize.Height());
    }
}

SkRRect RosenRenderBox::GetBoxRRect(const Offset& offset, const Border& border, double shrinkFactor, bool isRound)
{
    SkRRect rRect;
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
    rRect.setRectRadii(skRect, fRadii);
    return rRect;
}

bool RosenRenderBox::MaybeRelease()
{
    auto context = GetContext().Upgrade();
    if (context && context->GetRenderFactory() && context->GetRenderFactory()->GetRenderBoxFactory()->Recycle(this)) {
        ClearRenderObject();
        return false;
    }
    return true;
}

void RosenRenderBox::UpdateBlurRRect(const SkRRect& rRect, const Offset& offset)
{
    SkVector radius = rRect.radii(SkRRect::kUpperLeft_Corner);
    const SkRect& rect = rRect.rect();
    windowBlurRRect_.SetRectWithSimpleRadius(
        Rect(rect.left(), rect.top(), rect.width(), rect.height()), radius.fX, radius.fY);
    // this is relative offset
    Rect innerRect = windowBlurRRect_.GetRect();
    innerRect -= offset;
    windowBlurRRect_.SetRect(innerRect);
}

bool RosenRenderBox::CreateSkPath(const RefPtr<BasicShape>& basicShape, GeometryBoxType geometryBoxType, SkPath* skPath)
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

bool RosenRenderBox::CreateInset(
    const RefPtr<BasicShape>& basicShape, const Size& size, const Offset& position, SkPath* skPath)
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
    const SkVector fRadii[4] = { { topLeftRadiusX, topLeftRadiusY }, { topRightRadiusX, topRightRadiusY },
        { bottomRightRadiusX, bottomRightRadiusY }, { bottomLeftRadiusX, bottomLeftRadiusY } };
    SkRRect roundRect;
    roundRect.setRectRadii(rect, fRadii);
    skPath->addRRect(roundRect);
    return true;
}

bool RosenRenderBox::CreateCircle(
    const RefPtr<BasicShape>& basicShape, const Size& size, const Offset& position, SkPath* skPath)
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

bool RosenRenderBox::CreateEllipse(
    const RefPtr<BasicShape>& basicShape, const Size& size, const Offset& position, SkPath* skPath)
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

bool RosenRenderBox::CreatePolygon(
    const RefPtr<BasicShape>& basicShape, const Size& size, const Offset& position, SkPath* skPath)
{
    const auto& polygon = AceType::DynamicCast<Polygon>(basicShape);
    if (!polygon) {
        LOGE("polygon is null");
        return false;
    }
    std::vector<SkPoint> skPoints;
    for (auto [x, y] : polygon->GetPoints()) {
        skPoints.emplace_back(SkPoint::Make(DimensionToPx(x, size, LengthMode::HORIZONTAL) + position.GetX(),
            DimensionToPx(y, size, LengthMode::VERTICAL) + position.GetX()));
    }
    if (skPoints.empty()) {
        LOGW("points is null");
        return false;
    }
    skPath->addPoly(&skPoints[0], skPoints.size(), true);
    return true;
}

bool RosenRenderBox::CreatePath(
    const RefPtr<BasicShape>& basicShape, const Size& size, const Offset& position, SkPath* skPath)
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

bool RosenRenderBox::CreateRect(
    const RefPtr<BasicShape>& basicShape, const Size& size, const Offset& position, SkPath* skPath)
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
    const SkVector fRadii[4] = { { topLeftRadiusX, topLeftRadiusY }, { topRightRadiusX, topRightRadiusY },
        { bottomRightRadiusX, bottomRightRadiusY }, { bottomLeftRadiusX, bottomLeftRadiusY } };
    SkRRect roundRect;
    roundRect.setRectRadii(skRect, fRadii);
    skPath->addRRect(roundRect);
    return true;
}

float RosenRenderBox::GetFloatRadiusValue(
    const Dimension& src, const Dimension& dest, const Size& size, LengthMode type)
{
    if (src.Value() < 0.0 && dest.Value() > 0.0) {
        return DimensionToPx(dest, size, type);
    }
    return DimensionToPx(src, size, type);
}

void RosenRenderBox::GetSizeAndPosition(GeometryBoxType geometryBoxType, Size& size, Offset& position)
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

float RosenRenderBox::DimensionToPx(const Dimension& value, const Size& size, LengthMode type) const
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

void RosenRenderBox::CalculateAlignDeclaration()
{
    RenderBox::CalculateAlignDeclaration();
    auto rsNode = GetRSNode();
    if (rsNode && isUseAlign_) {
        rsNode->SetTranslate({ alignOffset_.GetX(), alignOffset_.GetY() });
    }
}

void RosenRenderBox::SetWidth(double width)
{
    RenderBoxBase::SetWidth(width);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set width to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetBoundsWidth(GetBoundsWidth());
}

void RosenRenderBox::SetHeight(double height)
{
    RenderBoxBase::SetHeight(height);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set height to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetBoundsHeight(GetBoundsHeight());
}

void RosenRenderBox::SetWidth(const Dimension& width)
{
    RenderBoxBase::SetWidth(width);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set dimension width to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetBoundsWidth(GetBoundsWidth());
}

void RosenRenderBox::SetHeight(const Dimension& height)
{
    RenderBoxBase::SetHeight(height);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set dimension height to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetBoundsHeight(GetBoundsHeight());
}

void RosenRenderBox::SetColor(const Color& color, bool isBackground)
{
    RenderBox::SetColor(color, isBackground);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set color to rsNode failed, rsNode is null");
        return;
    }
    if (isBackground) {
        rsNode->SetBackgroundColor(color.GetValue());
    } else {
        rsNode->SetForegroundColor(color.GetValue());
    }
}

void RosenRenderBox::SetBackgroundSize(const BackgroundImageSize& size)
{
    RenderBox::SetBackgroundSize(size);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set background size to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetBgImageWidth(size.GetSizeValueX());
    rsNode->SetBgImageHeight(size.GetSizeValueY());
}

void RosenRenderBox::SetBackgroundPosition(const BackgroundImagePosition& position)
{
    RenderBox::SetBackgroundPosition(position);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set background position to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetBgImagePositionX(position.GetSizeValueX());
    rsNode->SetBgImagePositionY(position.GetSizeValueY());
}

void RosenRenderBox::SetShadow(const Shadow& shadow)
{
    RenderBox::SetShadow(shadow);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set shadow to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetShadowRadius(shadow.GetBlurRadius());
    rsNode->SetShadowColor(shadow.GetColor().GetValue());
    rsNode->SetShadowOffsetX(shadow.GetOffset().GetX());
    rsNode->SetShadowOffsetY(shadow.GetOffset().GetY());
}

void RosenRenderBox::SetBorderWidth(double width, const BorderEdgeHelper& helper)
{
    RenderBox::SetBorderWidth(width, helper);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set border width to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetBorderWidth(width);
}

void RosenRenderBox::SetBorderColor(const Color& color, const BorderEdgeHelper& helper)
{
    RenderBox::SetBorderColor(color, helper);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set border color to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetBorderColor(color.GetValue());
}

void RosenRenderBox::SetBorderStyle(BorderStyle borderStyle, const BorderEdgeHelper& helper)
{
    RenderBox::SetBorderStyle(borderStyle, helper);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set border style to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
}

void RosenRenderBox::SetBorderRadius(double radius, const BorderRadiusHelper& helper)
{
    RenderBox::SetBorderRadius(radius, helper);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set border radius to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetCornerRadius(radius);
}

void RosenRenderBox::SetBlurRadius(const AnimatableDimension& radius)
{
    RenderBox::SetBlurRadius(radius);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set blur radius to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetFilter(Rosen::RSFilter::CreateBlurFilter(radius.Value(), radius.Value()));
}

void RosenRenderBox::SetBackdropRadius(const AnimatableDimension& radius)
{
    RenderBox::SetBackdropRadius(radius);
    auto& rsNode = GetRSNode();
    if (rsNode == nullptr) {
        LOGE("set backdrop radius to rsNode failed, rsNode is null");
        return;
    }
    rsNode->SetBackgroundFilter(Rosen::RSFilter::CreateBlurFilter(radius.Value(), radius.Value()));
}

void RosenRenderBox::SetWindowBlurProgress(double progress)
{
    RenderBox::SetWindowBlurProgress(progress);
}

double RosenRenderBox::GetBoundsWidth()
{
    return ConvertHorizontalDimensionToPx(GetWidthDimension(), false) - GetMargin().LeftPx() - GetMargin().RightPx();
}

double RosenRenderBox::GetBoundsHeight()
{
    return ConvertVerticalDimensionToPx(GetHeightDimension(), false) - GetMargin().TopPx() - GetMargin().BottomPx();
}

void RosenRenderBox::SyncDecorationToRSNode()
{
    auto rsNode = GetRSNode();
    if (!rsNode) {
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    dipScale_ = context->GetDipScale();
    // get raw value from decoration
    uint32_t borderColor = 0; // transparent
    uint32_t borderStyle = 0;
    float borderWidth = 0;
    float cornerRadius = 0;
    float shadowRadius = 0;
    uint32_t backgroundColor = 0; // transparent
    uint32_t foregroundColor = 0; // transparent
    std::shared_ptr<Rosen::RSFilter> backFilter = nullptr;
    std::shared_ptr<Rosen::RSFilter> filter = nullptr;
    if (backDecoration_) {
        // shape
        if (backDecoration_->GetBorder().HasValue()) {
            borderColor = backDecoration_->GetBorder().Top().GetColor().GetValue();
            borderWidth = backDecoration_->GetBorder().Top().GetWidth().ConvertToPx(dipScale_);
            borderStyle = static_cast<uint32_t>(backDecoration_->GetBorder().Top().GetBorderStyle());
        }
        if (backDecoration_->GetBorder().HasRadius()) {
            cornerRadius = backDecoration_->GetBorder().TopLeftRadius().GetX().ConvertToPx(dipScale_);
        }
        // background color
        backgroundColor = backDecoration_->GetBackgroundColor().GetValue();
        // shadow
        if (!backDecoration_->GetShadows().empty()) {
            shadowRadius = backDecoration_->GetShadows().front().GetBlurRadius();
            shadowRadius = RosenDecorationPainter::ConvertRadiusToSigma(shadowRadius);
            rsNode->SetShadowColor(backDecoration_->GetShadows().front().GetColor().GetValue());
            rsNode->SetShadowOffsetX(backDecoration_->GetShadows().front().GetOffset().GetX());
            rsNode->SetShadowOffsetY(backDecoration_->GetShadows().front().GetOffset().GetY());
        }
        if (backDecoration_->GetBlurRadius().IsValid()) {
            float radius = backDecoration_->GetBlurRadius().ConvertToPx(dipScale_);
            float backblurRadius = RosenDecorationPainter::ConvertRadiusToSigma(radius);
            backFilter = Rosen::RSFilter::CreateBlurFilter(backblurRadius, backblurRadius);
        }
    }
    if (frontDecoration_) {
        // shape
        if (frontDecoration_->GetBorder().HasValue()) {
            borderColor = frontDecoration_->GetBorder().Top().GetColor().GetValue();
            borderWidth = frontDecoration_->GetBorder().Top().GetWidth().ConvertToPx(dipScale_);
            borderStyle = static_cast<uint32_t>(frontDecoration_->GetBorder().Top().GetBorderStyle());
        }
        foregroundColor = frontDecoration_->GetBackgroundColor().GetValue();
        if (frontDecoration_->GetBorder().HasRadius()) {
            cornerRadius = frontDecoration_->GetBorder().TopLeftRadius().GetX().ConvertToPx(dipScale_);
        }
        if (frontDecoration_->GetBlurRadius().IsValid()) {
            float radius = frontDecoration_->GetBlurRadius().ConvertToPx(dipScale_);
            float frontblurRadius = RosenDecorationPainter::ConvertRadiusToSigma(radius);
            filter = Rosen::RSFilter::CreateBlurFilter(frontblurRadius, frontblurRadius);
        }
    }
    // set value to rsnode
    rsNode->SetBorderColor(borderColor);
    rsNode->SetBorderWidth(borderWidth);
    rsNode->SetBorderStyle(borderStyle);
    rsNode->SetCornerRadius(cornerRadius);
    rsNode->SetBackgroundColor(backgroundColor);
    rsNode->SetForegroundColor(foregroundColor);
    rsNode->SetBackgroundFilter(backFilter);
    rsNode->SetFilter(filter);
    rsNode->SetShadowRadius(shadowRadius);
}

void RosenRenderBox::OnAttachContext()
{
    if (!backDecoration_) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    backDecoration_->SetContextAndCallback(context_, [weak = WeakClaim(this)] {
        if (auto renderBox = weak.Upgrade()) {
            renderBox->SyncDecorationToRSNode();
            renderBox->OnAnimationCallback();
        }
    });
    if (!frontDecoration_) {
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    frontDecoration_->SetContextAndCallback(context_, [weak = WeakClaim(this)] {
        auto renderBox = weak.Upgrade();
        if (renderBox) {
            renderBox->SyncDecorationToRSNode();
            renderBox->OnAnimationCallback();
        }
    });
    width_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
        if (auto renderBox = weak.Upgrade()) {
            renderBox->OnAnimationCallback();
        }
    });
    height_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
        if (auto renderBox = weak.Upgrade()) {
            renderBox->OnAnimationCallback();
        }
    });
    marginOrigin_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
        auto renderBox = weak.Upgrade();
        if (renderBox) {
            renderBox->OnAnimationCallback();
        }
    });
    paddingOrigin_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
        auto renderBox = weak.Upgrade();
        if (renderBox) {
            renderBox->OnAnimationCallback();
        }
    });
    aspectRatio_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
        auto renderBox = weak.Upgrade();
        if (renderBox) {
            renderBox->OnAnimationCallback();
        }
    });
}

void RosenRenderBox::AnimateMouseHoverEnter()
{
    LOGD("RosenRenderBox::AnimateMouseHoverEnter in. animationType_ = %{public}d", animationType_);
    if (animationType_ == HoverAnimationType::SCALE) {
        isHoveredScale = true;
        auto rsNode = GetRSNode();
        if (!rsNode) {
            return;
        }
        float scaleBegin = SCALE_DEFAULT;
        float scaleEnd = SCALE_CHANGED;

        rsNode->SetScale(scaleBegin);
        Rosen::RSAnimationTimingProtocol protocol;
        protocol.SetDuration(HOVER_ANIMATION_DURATION);
        RSNode::Animate(
            protocol, SCALE_ANIMATION_TIMING_CURVE,
            [rsNode, scaleEnd]() {
                if (rsNode) {
                    rsNode->SetScale(scaleEnd);
                }
            },
            []() {});
    } else if (animationType_ == HoverAnimationType::BOARD) {
        ResetController(controllerExit_);
        if (!controllerEnter_) {
            controllerEnter_ = AceType::MakeRefPtr<Animator>(context_);
        }
        if (!backDecoration_) {
            backDecoration_ = AceType::MakeRefPtr<Decoration>();
        }
        if (!colorAnimationEnter_) {
            colorAnimationEnter_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
        }
        CreateColorAnimation(colorAnimationEnter_, hoverColorBegin_, BOARD_CHANGED);
        controllerEnter_->ClearInterpolators();
        controllerEnter_->AddInterpolator(colorAnimationEnter_);
        controllerEnter_->SetDuration(HOVER_ANIMATION_DURATION);
        controllerEnter_->SetFillMode(FillMode::FORWARDS);
        controllerEnter_->Play();
        isHoveredBoard = true;
    } else {
        return;
    }
}

void RosenRenderBox::AnimateMouseHoverExit()
{
    LOGI("RosenRenderBox::AnimateMouseHoverExit in. animationType_ = %{public}d", animationType_);
    if (animationType_ == HoverAnimationType::SCALE) {
        isHoveredScale = true;
        auto rsNode = GetRSNode();
        float scaleBegin = SCALE_CHANGED;
        float scaleEnd = SCALE_DEFAULT;

        rsNode->SetScale(scaleBegin);
        Rosen::RSAnimationTimingProtocol protocol;
        protocol.SetDuration(HOVER_ANIMATION_DURATION);
        RSNode::Animate(
            protocol, SCALE_ANIMATION_TIMING_CURVE,
            [rsNode, scaleEnd]() {
                if (rsNode) {
                    rsNode->SetScale(scaleEnd);
                }
            },
            []() {});
    } else if (animationType_ == HoverAnimationType::BOARD) {
        ResetController(controllerEnter_);
        if (!controllerExit_) {
            controllerExit_ = AceType::MakeRefPtr<Animator>(context_);
        }
        if (!backDecoration_) {
            backDecoration_ = AceType::MakeRefPtr<Decoration>();
        }
        if (!colorAnimationExit_) {
            colorAnimationExit_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
        }
        LOGD("AnimateMouseHoverExit hoverColor_.GetValue() = %{public}x, hoverColorBegin_.GetValue() = %{public}x",
            hoverColor_.GetValue(), hoverColorBegin_.GetValue());
        CreateColorAnimation(colorAnimationExit_, hoverColor_, hoverColorBegin_);
        controllerExit_->ClearInterpolators();
        controllerExit_->AddInterpolator(colorAnimationExit_);
        controllerExit_->SetDuration(HOVER_ANIMATION_DURATION);
        controllerExit_->Play();
        controllerExit_->SetFillMode(FillMode::FORWARDS);
        isHoveredBoard = false;
    }
}

} // namespace OHOS::Ace
