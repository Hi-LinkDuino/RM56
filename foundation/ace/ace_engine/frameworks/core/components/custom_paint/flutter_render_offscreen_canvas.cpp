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

#include "core/components/custom_paint/flutter_render_offscreen_canvas.h"

#include <cmath>
#include <sstream>

#include "flutter/third_party/txt/src/txt/paragraph_builder.h"
#include "flutter/third_party/txt/src/txt/paragraph_style.h"
#include "third_party/skia/include/core/SkBlendMode.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkMaskFilter.h"
#include "third_party/skia/include/core/SkPoint.h"
#include "third_party/skia/include/effects/SkBlurImageFilter.h"
#include "third_party/skia/include/effects/SkDashPathEffect.h"
#include "third_party/skia/include/effects/SkGradientShader.h"
#include "third_party/skia/include/encode/SkJpegEncoder.h"
#include "third_party/skia/include/encode/SkPngEncoder.h"
#include "third_party/skia/include/encode/SkWebpEncoder.h"
#include "third_party/skia/include/utils/SkBase64.h"
#include "third_party/skia/include/utils/SkParsePath.h"

#include "base/i18n/localization.h"
#include "base/log/log.h"
#include "base/utils/string_utils.h"
#include "core/components/common/painter/flutter_decoration_painter.h"
#include "core/components/font/constants_converter.h"
#include "core/components/font/flutter_font_collection.h"
#include "core/image/image_provider.h"

namespace OHOS::Ace {
namespace {
constexpr double HANGING_PERCENT = 0.8;
template<typename T, typename N>
    N ConvertEnumToSkEnum(T key, const LinearEnumMapNode<T, N>* map, size_t length, N defaultValue)
    {
        int64_t index = BinarySearchFindIndex(map, length, key);
        return index != -1 ? map[index].value : defaultValue;
    }

constexpr double DEFAULT_QUALITY = 0.92;
constexpr int32_t MAX_LENGTH = 2048 * 2048;
const std::string UNSUPPORTED = "data:image/png";
const std::string URL_PREFIX = "data:";
const std::string URL_SYMBOL = ";base64,";
const std::string IMAGE_PNG = "image/png";
const std::string IMAGE_JPEG = "image/jpeg";
const std::string IMAGE_WEBP = "image/webp";
constexpr double HALF_CIRCLE_ANGLE = 180.0;
constexpr double FULL_CIRCLE_ANGLE = 360.0;

// If args is empty or invalid format, use default: image/png
std::string GetMimeType(const std::string& args)
{
    std::string type = args;
    for (size_t i = 0; i < type.size(); ++i) {
        type[i] = static_cast<uint8_t>(tolower(type[i]));
    }
    return type;
}

// Quality need between 0.0 and 1.0 for MimeType jpeg and webp
double GetQuality(const std::string& args, const double quality)
{
    std::string type = args;
    auto mimeType = GetMimeType(type);
    if (mimeType != IMAGE_JPEG && mimeType != IMAGE_WEBP) {
        return DEFAULT_QUALITY;
    }
    if (quality < 0.0 || quality > 1.0) {
        return DEFAULT_QUALITY;
    }
    return quality;
}

const LinearEnumMapNode<CompositeOperation, SkBlendMode> SK_BLEND_MODE_TABLE[] = {
    { CompositeOperation::SOURCE_OVER, SkBlendMode::kSrcOver },
    { CompositeOperation::SOURCE_ATOP, SkBlendMode::kSrcATop },
    { CompositeOperation::SOURCE_IN, SkBlendMode::kSrcIn },
    { CompositeOperation::SOURCE_OUT, SkBlendMode::kSrcOut },
    { CompositeOperation::DESTINATION_OVER, SkBlendMode::kDstOver },
    { CompositeOperation::DESTINATION_ATOP, SkBlendMode::kDstATop },
    { CompositeOperation::DESTINATION_IN, SkBlendMode::kDstIn },
    { CompositeOperation::DESTINATION_OUT, SkBlendMode::kDstOut },
    { CompositeOperation::LIGHTER, SkBlendMode::kLighten },
    { CompositeOperation::COPY, SkBlendMode::kSrc },
    { CompositeOperation::XOR, SkBlendMode::kXor },
};
constexpr size_t BLEND_MODE_SIZE = ArraySize(SK_BLEND_MODE_TABLE);
} // namespace

FlutterRenderOffscreenCanvas::FlutterRenderOffscreenCanvas(const WeakPtr<PipelineContext>& context,
    int32_t width, int32_t height)
{
    pipelineContext_ = context;
    width_ = width;
    height_ = height;
    auto imageInfo = SkImageInfo::Make(width, height, SkColorType::kRGBA_8888_SkColorType,
        SkAlphaType::kOpaque_SkAlphaType);
    skBitmap_.allocPixels(imageInfo);
    cacheBitmap_.allocPixels(imageInfo);
    skBitmap_.eraseColor(SK_ColorTRANSPARENT);
    cacheBitmap_.eraseColor(SK_ColorTRANSPARENT);
    skCanvas_ = std::make_unique<SkCanvas>(skBitmap_);
    cacheCanvas_ = std::make_unique<SkCanvas>(cacheBitmap_);

    auto currentDartState = flutter::UIDartState::Current();
    if (!currentDartState) {
        return;
    }

    renderTaskHolder_ = MakeRefPtr<FlutterRenderTaskHolder>(
        currentDartState->GetSkiaUnrefQueue(),
        currentDartState->GetIOManager(),
        currentDartState->GetTaskRunners().GetIOTaskRunner());

    InitFliterFunc();
}
void FlutterRenderOffscreenCanvas::AddRect(const Rect& rect)
{
    SkRect skRect = SkRect::MakeLTRB(rect.Left(), rect.Top(),
        rect.Right(), rect.Bottom());
    skPath_.addRect(skRect);
}

void FlutterRenderOffscreenCanvas::Fill()
{
    SkPaint paint;
    paint.setAntiAlias(antiAlias_);
    paint.setColor(fillState_.GetColor().GetValue());
    paint.setStyle(SkPaint::Style::kFill_Style);
    if (HasShadow()) {
        FlutterDecorationPainter::PaintShadow(skPath_, shadow_, skCanvas_.get());
    }
    if (fillState_.GetGradient().IsValid()) {
        UpdatePaintShader(paint, fillState_.GetGradient());
    }
    if (fillState_.GetPattern().IsValid()) {
        UpdatePaintShader(fillState_.GetPattern(), paint);
    }
    if (globalState_.HasGlobalAlpha()) {
        paint.setAlphaf(globalState_.GetAlpha());
    }
    if (globalState_.GetType() == CompositeOperation::SOURCE_OVER) {
        skCanvas_->drawPath(skPath_, paint);
    } else {
        InitCachePaint();
        cacheCanvas_->drawPath(skPath_, paint);
        skCanvas_->drawBitmap(cacheBitmap_, 0, 0, &cachePaint_);
        cacheBitmap_.eraseColor(0);
    }
}

void FlutterRenderOffscreenCanvas::Clip()
{
    skCanvas_->clipPath(skPath_);
}

void FlutterRenderOffscreenCanvas::FillRect(Rect rect)
{
    SkPaint paint;
    paint.setAntiAlias(antiAlias_);
    paint.setColor(fillState_.GetColor().GetValue());
    paint.setStyle(SkPaint::Style::kFill_Style);
    SkRect skRect = SkRect::MakeLTRB(rect.Left(), rect.Top(), rect.Right(), rect.Bottom());
    if (HasShadow()) {
        SkPath path;
        path.addRect(skRect);
        FlutterDecorationPainter::PaintShadow(path, shadow_, skCanvas_.get());
    }
    if (fillState_.GetGradient().IsValid()) {
        UpdatePaintShader(paint, fillState_.GetGradient());
    }
    if (fillState_.GetPattern().IsValid()) {
        UpdatePaintShader(fillState_.GetPattern(), paint);
    }
    if (globalState_.HasGlobalAlpha()) {
        paint.setAlphaf(globalState_.GetAlpha()); // update the global alpha after setting the color
    }
    if (globalState_.GetType() == CompositeOperation::SOURCE_OVER) {
        skCanvas_->drawRect(skRect, paint);
    } else {
        InitCachePaint();
        cacheCanvas_->drawRect(skRect, paint);
        skCanvas_->drawBitmap(cacheBitmap_, 0, 0, &cachePaint_);
        cacheBitmap_.eraseColor(0);
    }
}

void FlutterRenderOffscreenCanvas::PutImageData(const ImageData& imageData)
{
    if (imageData.data.empty()) {
        return;
    }
    uint32_t* data = new (std::nothrow)uint32_t[imageData.data.size()];
    if (data == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < imageData.data.size(); ++i) {
        data[i] = imageData.data[i].GetValue();
    }
    SkBitmap skBitmap;
    auto imageInfo = SkImageInfo::Make(imageData.dirtyWidth, imageData.dirtyHeight, SkColorType::kBGRA_8888_SkColorType,
        SkAlphaType::kOpaque_SkAlphaType);
    skBitmap.allocPixels(imageInfo);
    skBitmap.setPixels(data);
    skCanvas_->drawBitmap(skBitmap, imageData.x, imageData.y);
    delete[] data;
}

void FlutterRenderOffscreenCanvas::SetPaintImage()
{
    float matrix[20] = {0};
    matrix[0] = matrix[6] = matrix[12] = matrix[18] = 1.0f;
#ifdef USE_SYSTEM_SKIA
    imagePaint_.setColorFilter(SkColorFilter::MakeMatrixFilterRowMajor255(matrix));
#else
    imagePaint_.setColorFilter(SkColorFilters::Matrix(matrix));
#endif

    imagePaint_.setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kNormal_SkBlurStyle, 0));
    imagePaint_.setImageFilter(SkBlurImageFilter::Make(0, 0, nullptr));

    SetDropShadowFilter("0px 0px 0px black");
    std::string filterType, filterParam;
    if (!GetFilterType(filterType, filterParam)) {
        return;
    }
    if (filterFunc_.find(filterType) != filterFunc_.end()) {
        filterFunc_[filterType](filterParam);
    }
}

void FlutterRenderOffscreenCanvas::InitImagePaint()
{
    if (smoothingEnabled_) {
        if (smoothingQuality_ == "low") {
            imagePaint_.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
        } else if (smoothingQuality_ == "medium") {
            imagePaint_.setFilterQuality(SkFilterQuality::kMedium_SkFilterQuality);
        } else if (smoothingQuality_ == "high") {
            imagePaint_.setFilterQuality(SkFilterQuality::kHigh_SkFilterQuality);
        } else {
            LOGE("Unsupported Quality type:%{public}s", smoothingQuality_.c_str());
        }
    } else {
        imagePaint_.setFilterQuality(SkFilterQuality::kNone_SkFilterQuality);
    }
    SetPaintImage();
}

void FlutterRenderOffscreenCanvas::DrawImage(const CanvasImage& canvasImage, double width, double height)
{
    if (!flutter::UIDartState::Current()) {
        return;
    }

    auto context = pipelineContext_.Upgrade();
    if (!context) {
        return;
    }

    auto image = GreatOrEqual(width, 0) && GreatOrEqual(height, 0)
                        ? ImageProvider::GetSkImage(canvasImage.src, context, Size(width, height))
                        : ImageProvider::GetSkImage(canvasImage.src, context);
    if (!image) {
        LOGE("image is null");
        return;
    }
    InitCachePaint();
    const auto skCanvas =
        globalState_.GetType() == CompositeOperation::SOURCE_OVER ? skCanvas_.get() : cacheCanvas_.get();
    InitImagePaint();
    if (HasImageShadow()) {
        SkRect skRect = SkRect::MakeXYWH(canvasImage.dx, canvasImage.dy, canvasImage.dWidth, canvasImage.dHeight);
        SkPath path;
        path.addRect(skRect);
        FlutterDecorationPainter::PaintShadow(path, imageShadow_, skCanvas);
    }
    switch (canvasImage.flag) {
        case 0:
            skCanvas->drawImage(image, canvasImage.dx, canvasImage.dy);
            break;
        case 1: {
            SkRect rect = SkRect::MakeXYWH(canvasImage.dx, canvasImage.dy, canvasImage.dWidth, canvasImage.dHeight);
            skCanvas->drawImageRect(image, rect, &imagePaint_);
            break;
        }
        case 2: {
            SkRect dstRect =
                SkRect::MakeXYWH(canvasImage.dx, canvasImage.dy, canvasImage.dWidth, canvasImage.dHeight);
            SkRect srcRect =
                SkRect::MakeXYWH(canvasImage.sx, canvasImage.sy, canvasImage.sWidth, canvasImage.sHeight);
            skCanvas->drawImageRect(image, srcRect, dstRect, &imagePaint_);
            break;
        }
        default:
            break;
    }
    if (globalState_.GetType() != CompositeOperation::SOURCE_OVER) {
        skCanvas_->drawBitmap(cacheBitmap_, 0, 0, &cachePaint_);
        cacheBitmap_.eraseColor(0);
    }
}

void FlutterRenderOffscreenCanvas::DrawPixelMap(RefPtr<PixelMap> pixelMap, const CanvasImage& canvasImage)
{
    if (!flutter::UIDartState::Current()) {
        return;
    }

    auto context = pipelineContext_.Upgrade();
    if (!context) {
        return;
    }

    // get skImage form pixelMap
    auto imageInfo = ImageProvider::MakeSkImageInfoFromPixelMap(pixelMap);
    SkPixmap imagePixmap(imageInfo, reinterpret_cast<const void*>(pixelMap->GetPixels()), pixelMap->GetRowBytes());

    // Step2: Create SkImage and draw it, using gpu or cpu
    sk_sp<SkImage> image;
    if (!renderTaskHolder_->ioManager) {
        image = SkImage::MakeFromRaster(imagePixmap, nullptr, nullptr);
    } else {
#ifndef GPU_DISABLED
        image = SkImage::MakeCrossContextFromPixmap(renderTaskHolder_->ioManager->GetResourceContext().get(),
            imagePixmap, true, imagePixmap.colorSpace(), true);
#else
        image = SkImage::MakeFromRaster(imagePixmap, nullptr, nullptr);
#endif
    }
    if (!image) {
        LOGE("image is null");
        return;
    }

    InitCachePaint();
    const auto skCanvas =
        globalState_.GetType() == CompositeOperation::SOURCE_OVER ? skCanvas_.get() : cacheCanvas_.get();
    InitImagePaint();
    switch (canvasImage.flag) {
        case 0:
            skCanvas->drawImage(image, canvasImage.dx, canvasImage.dy);
            break;
        case 1: {
            SkRect rect = SkRect::MakeXYWH(canvasImage.dx, canvasImage.dy, canvasImage.dWidth, canvasImage.dHeight);
            skCanvas->drawImageRect(image, rect, &imagePaint_);
            break;
        }
        case 2: {
            SkRect dstRect =
                SkRect::MakeXYWH(canvasImage.dx, canvasImage.dy, canvasImage.dWidth, canvasImage.dHeight);
            SkRect srcRect =
                SkRect::MakeXYWH(canvasImage.sx, canvasImage.sy, canvasImage.sWidth, canvasImage.sHeight);
            skCanvas->drawImageRect(image, srcRect, dstRect, &imagePaint_);
            break;
        }
        default:
            break;
    }
    if (globalState_.GetType() != CompositeOperation::SOURCE_OVER) {
        skCanvas_->drawBitmap(cacheBitmap_, 0, 0, &cachePaint_);
        cacheBitmap_.eraseColor(0);
    }
}

std::unique_ptr<ImageData> FlutterRenderOffscreenCanvas::GetImageData(double left, double top,
    double width, double height)
{
    auto imageInfo =
        SkImageInfo::Make(width, height, SkColorType::kBGRA_8888_SkColorType, SkAlphaType::kOpaque_SkAlphaType);
    double dirtyWidth = width >= 0 ? width : 0;
    double dirtyHeight = height >= 0 ? height : 0;
    int32_t size = dirtyWidth * dirtyHeight;
    // write color
    std::unique_ptr<uint8_t[]> pixels = std::make_unique<uint8_t[]>(size * 4);
    skCanvas_->readPixels(imageInfo, pixels.get(), dirtyWidth * imageInfo.bytesPerPixel(), 0, 0);
    std::unique_ptr<ImageData> imageData = std::make_unique<ImageData>();
    imageData->dirtyWidth = dirtyWidth;
    imageData->dirtyHeight = dirtyHeight;
    // a pixel include 4 data blue, green, red, alpha
    for (int i = 0; i < size * 4; i += 4) {
        auto blue = pixels[i];
        auto green = pixels[i + 1];
        auto red = pixels[i + 2];
        auto alpha = pixels[i + 3];
        imageData->data.emplace_back(Color::FromARGB(alpha, red, green, blue));
    }
    return imageData;
}

void FlutterRenderOffscreenCanvas::Save()
{
    SaveStates();
    skCanvas_->save();
}

void FlutterRenderOffscreenCanvas::Restore()
{
    RestoreStates();
    skCanvas_->restore();
}

std::string FlutterRenderOffscreenCanvas::ToDataURL(const std::string& type, const double quality)
{
    std::string mimeType = GetMimeType(type);
    double qua = GetQuality(type, quality);
    SkPixmap src;
    bool success = skBitmap_.peekPixels(&src);
    if (!success) {
        return UNSUPPORTED;
    }
    SkDynamicMemoryWStream dst;
    if (mimeType == IMAGE_JPEG) {
        SkJpegEncoder::Options options;
        options.fQuality = qua;
        success = SkJpegEncoder::Encode(&dst, src, options);
    } else if (mimeType == IMAGE_WEBP) {
        SkWebpEncoder::Options options;
        options.fQuality = qua * 100.0;
        success = SkWebpEncoder::Encode(&dst, src, options);
    } else {
        mimeType = IMAGE_PNG;
        SkPngEncoder::Options options;
        success = SkPngEncoder::Encode(&dst, src, options);
    }
    if (!success) {
        return UNSUPPORTED;
    }
    auto result = dst.detachAsData();
    if (result == nullptr) {
        return UNSUPPORTED;
    }
    size_t len = SkBase64::Encode(result->data(), result->size(), nullptr);
    if (len > MAX_LENGTH) {
        return UNSUPPORTED;
    }
    SkString info(len);
    SkBase64::Encode(result->data(), result->size(), info.writable_str());
    return std::string(URL_PREFIX).append(mimeType).append(URL_SYMBOL).append(info.c_str());
}

void FlutterRenderOffscreenCanvas::UpdatePaintShader(SkPaint& paint, const Gradient& gradient)
{
    SkPoint beginPoint = SkPoint::Make(SkDoubleToScalar(gradient.GetBeginOffset().GetX()),
        SkDoubleToScalar(gradient.GetBeginOffset().GetY()));
    SkPoint endPoint = SkPoint::Make(SkDoubleToScalar(gradient.GetEndOffset().GetX()),
        SkDoubleToScalar(gradient.GetEndOffset().GetY()));
    SkPoint pts[2] = { beginPoint, endPoint };
    auto gradientColors = gradient.GetColors();
    uint32_t colorsSize = gradientColors.size();
    SkColor colors[gradientColors.size()];
    float pos[gradientColors.size()];
    for (uint32_t i = 0; i < colorsSize; ++i) {
        const auto& gradientColor = gradientColors[i];
        colors[i] = gradientColor.GetColor().GetValue();
        pos[i] = gradientColor.GetDimension().Value();
    }

#ifdef USE_SYSTEM_SKIA
    auto mode = SkShader::kClamp_TileMode;
#else
    auto mode = SkTileMode::kClamp;
#endif

    sk_sp<SkShader> skShader = nullptr;
    if (gradient.GetType() == GradientType::LINEAR) {
        skShader = SkGradientShader::MakeLinear(pts, colors, pos, gradientColors.size(), mode);
    } else {
        if (gradient.GetInnerRadius() <= 0.0 && beginPoint == endPoint) {
            skShader = SkGradientShader::MakeRadial(
                endPoint, gradient.GetOuterRadius(), colors, pos, gradientColors.size(), mode);
        } else {
            skShader = SkGradientShader::MakeTwoPointConical(beginPoint, gradient.GetInnerRadius(),
                endPoint, gradient.GetOuterRadius(), colors, pos, gradientColors.size(), mode);
        }
    }
    paint.setShader(skShader);
}

void FlutterRenderOffscreenCanvas::BeginPath()
{
    skPath_.reset();
}

void FlutterRenderOffscreenCanvas::UpdatePaintShader(const Pattern& pattern, SkPaint& paint)
{
    if (!flutter::UIDartState::Current()) {
        return;
    }

    auto context = pipelineContext_.Upgrade();
    if (!context) {
        return;
    }

    auto width = pattern.GetImageWidth();
    auto height = pattern.GetImageHeight();
    auto image = GreatOrEqual(width, 0) && GreatOrEqual(height, 0)
                        ? ImageProvider::GetSkImage(pattern.GetImgSrc(), context, Size(width, height))
                        : ImageProvider::GetSkImage(pattern.GetImgSrc(), context);
    if (!image) {
        LOGE("image is null");
        return;
    }
    static const LinearMapNode<void (*)(sk_sp<SkImage>, SkPaint&)> staticPattern[] = {
        { "no-repeat",
            [](sk_sp<SkImage> image, SkPaint& paint) {
#ifdef USE_SYSTEM_SKIA
                paint.setShader(image->makeShader(SkShader::kDecal_TileMode, SkShader::kDecal_TileMode, nullptr));
#else
                paint.setShader(image->makeShader(SkTileMode::kDecal, SkTileMode::kDecal, nullptr));
#endif
            } },
        { "repeat",
            [](sk_sp<SkImage> image, SkPaint& paint) {
#ifdef USE_SYSTEM_SKIA
                paint.setShader(image->makeShader(SkShader::kRepeat_TileMode, SkShader::kRepeat_TileMode, nullptr));
#else
                paint.setShader(image->makeShader(SkTileMode::kRepeat, SkTileMode::kRepeat, nullptr));
#endif
            } },
        { "repeat-x",
            [](sk_sp<SkImage> image, SkPaint& paint) {
#ifdef USE_SYSTEM_SKIA
                paint.setShader(image->makeShader(SkShader::kRepeat_TileMode, SkShader::kDecal_TileMode, nullptr));
#else
                paint.setShader(image->makeShader(SkTileMode::kRepeat, SkTileMode::kDecal, nullptr));
#endif
            } },
        { "repeat-y",
            [](sk_sp<SkImage> image, SkPaint& paint) {
#ifdef USE_SYSTEM_SKIA
                paint.setShader(image->makeShader(SkShader::kDecal_TileMode, SkShader::kRepeat_TileMode, nullptr));
#else
                paint.setShader(image->makeShader(SkTileMode::kDecal, SkTileMode::kRepeat, nullptr));
#endif
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(staticPattern, ArraySize(staticPattern), pattern.GetRepetition().c_str());
    if (operatorIter != -1) {
        staticPattern[operatorIter].value(image, paint);
    }
}
void FlutterRenderOffscreenCanvas::Arc(const ArcParam& param)
{
    double left = param.x - param.radius;
    double top = param.y - param.radius;
    double right = param.x + param.radius;
    double bottom = param.y + param.radius;
    double startAngle = param.startAngle * HALF_CIRCLE_ANGLE / M_PI;
    double endAngle = param.endAngle * HALF_CIRCLE_ANGLE / M_PI;
    double sweepAngle = endAngle - startAngle;
    if (param.anticlockwise) {
        sweepAngle =
            endAngle > startAngle ? (std::fmod(sweepAngle, FULL_CIRCLE_ANGLE) - FULL_CIRCLE_ANGLE) : sweepAngle;
    } else {
        sweepAngle =
            endAngle > startAngle ? sweepAngle : (std::fmod(sweepAngle, FULL_CIRCLE_ANGLE) + FULL_CIRCLE_ANGLE);
    }
    auto rect = SkRect::MakeLTRB(left, top, right, bottom);
    if (NearEqual(std::fmod(sweepAngle, FULL_CIRCLE_ANGLE), 0.0) && !NearEqual(startAngle, endAngle)) {
        // draw circle
        double half = GreatNotEqual(sweepAngle, 0.0) ? HALF_CIRCLE_ANGLE : -HALF_CIRCLE_ANGLE;
        skPath_.arcTo(rect, SkDoubleToScalar(startAngle), SkDoubleToScalar(half), false);
        skPath_.arcTo(rect, SkDoubleToScalar(half + startAngle), SkDoubleToScalar(half), false);
    } else {
        skPath_.arcTo(rect, SkDoubleToScalar(startAngle), SkDoubleToScalar(sweepAngle), false);
    }
}

void FlutterRenderOffscreenCanvas::ClearRect(Rect rect)
{
    SkPaint paint;
    paint.setAntiAlias(antiAlias_);
    paint.setBlendMode(SkBlendMode::kClear);
    auto skRect = SkRect::MakeLTRB(rect.Left(), rect.Top(), rect.Right(), rect.Bottom());
    skCanvas_->drawRect(skRect, paint);
}

void FlutterRenderOffscreenCanvas::StrokeRect(Rect rect)
{
    SkPaint paint = GetStrokePaint();
    paint.setAntiAlias(antiAlias_);
    SkRect skRect = SkRect::MakeLTRB(rect.Left(), rect.Top(),
        rect.Right(), rect.Bottom());
    if (HasShadow()) {
        SkPath path;
        path.addRect(skRect);
        FlutterDecorationPainter::PaintShadow(path, shadow_, skCanvas_.get());
    }
    if (strokeState_.GetGradient().IsValid()) {
        UpdatePaintShader(paint, strokeState_.GetGradient());
    }
    if (strokeState_.GetPattern().IsValid()) {
        UpdatePaintShader(strokeState_.GetPattern(), paint);
    }
    if (globalState_.GetType() == CompositeOperation::SOURCE_OVER) {
        skCanvas_->drawRect(skRect, paint);
    } else {
        InitCachePaint();
        cacheCanvas_->drawRect(skRect, paint);
        skCanvas_->drawBitmap(cacheBitmap_, 0, 0, &cachePaint_);
        cacheBitmap_.eraseColor(0);
    }
}

void FlutterRenderOffscreenCanvas::Stroke()
{
    SkPaint paint = GetStrokePaint();
    paint.setAntiAlias(antiAlias_);
    if (HasShadow()) {
        FlutterDecorationPainter::PaintShadow(skPath_, shadow_, skCanvas_.get());
    }
    if (strokeState_.GetGradient().IsValid()) {
        UpdatePaintShader(paint, strokeState_.GetGradient());
    }
    if (strokeState_.GetPattern().IsValid()) {
        UpdatePaintShader(strokeState_.GetPattern(), paint);
    }
    if (globalState_.GetType() == CompositeOperation::SOURCE_OVER) {
        skCanvas_->drawPath(skPath_, paint);
    } else {
        InitCachePaint();
        cacheCanvas_->drawPath(skPath_, paint);
        skCanvas_->drawBitmap(cacheBitmap_, 0, 0, &cachePaint_);
        cacheBitmap_.eraseColor(0);
    }
}

void FlutterRenderOffscreenCanvas::Stroke(const RefPtr<CanvasPath2D>& path)
{
    if (path == nullptr) {
        return;
    }
    strokePath_.reset();

    for (const auto& [cmd, args] : path->GetCaches()) {
        switch (cmd) {
        case PathCmd::CMDS: {
            Path2DAddPath(args);
            break;
        }
        case PathCmd::TRANSFORM: {
            Path2DSetTransform(args);
            break;
        }
        case PathCmd::MOVE_TO: {
            Path2DMoveTo(args);
            break;
        }
        case PathCmd::LINE_TO: {
            Path2DLineTo(args);
            break;
        }
        case PathCmd::ARC: {
            Path2DArc(args);
            break;
        }
        case PathCmd::ARC_TO: {
            Path2DArcTo(args);
            break;
        }
        case PathCmd::QUADRATIC_CURVE_TO: {
            Path2DQuadraticCurveTo(args);
            break;
        }
        case PathCmd::BEZIER_CURVE_TO: {
            Path2DBezierCurveTo(args);
            break;
        }
        case PathCmd::ELLIPSE: {
            Path2DEllipse(args);
            break;
        }
        case PathCmd::RECT: {
            Path2DRect(args);
            break;
        }
        case PathCmd::CLOSE_PATH: {
            Path2DClosePath(args);
            break;
        }
        default: {
            break;
        }
        }
    }
    Path2DStroke();
}
SkPaint FlutterRenderOffscreenCanvas::GetStrokePaint()
{
    static const LinearEnumMapNode<LineJoinStyle, SkPaint::Join> skLineJoinTable[] = {
        { LineJoinStyle::MITER, SkPaint::Join::kMiter_Join },
        { LineJoinStyle::ROUND, SkPaint::Join::kRound_Join },
        { LineJoinStyle::BEVEL, SkPaint::Join::kBevel_Join },
    };
    static const LinearEnumMapNode<LineCapStyle, SkPaint::Cap> skLineCapTable[] = {
        { LineCapStyle::BUTT, SkPaint::Cap::kButt_Cap },
        { LineCapStyle::ROUND, SkPaint::Cap::kRound_Cap },
        { LineCapStyle::SQUARE, SkPaint::Cap::kSquare_Cap },
    };
    SkPaint paint;
    paint.setColor(strokeState_.GetColor().GetValue());
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setStrokeJoin(ConvertEnumToSkEnum(
        strokeState_.GetLineJoin(), skLineJoinTable, ArraySize(skLineJoinTable), SkPaint::Join::kMiter_Join));
    paint.setStrokeCap(ConvertEnumToSkEnum(
        strokeState_.GetLineCap(), skLineCapTable, ArraySize(skLineCapTable), SkPaint::Cap::kButt_Cap));
    paint.setStrokeWidth(static_cast<SkScalar>(strokeState_.GetLineWidth()));
    paint.setStrokeMiter(static_cast<SkScalar>(strokeState_.GetMiterLimit()));

    // set line Dash
    UpdateLineDash(paint);

    // set global alpha
    if (globalState_.HasGlobalAlpha()) {
        paint.setAlphaf(globalState_.GetAlpha());
    }
    return paint;
}
void FlutterRenderOffscreenCanvas::SetAntiAlias(bool isEnabled)
{
    antiAlias_ = isEnabled;
}
bool FlutterRenderOffscreenCanvas::HasShadow() const
{
    return !(NearZero(shadow_.GetOffset().GetX()) && NearZero(shadow_.GetOffset().GetY()) &&
         NearZero(shadow_.GetBlurRadius()));
}

bool FlutterRenderOffscreenCanvas::HasImageShadow() const
{
    return !(NearZero(imageShadow_.GetOffset().GetX()) && NearZero(imageShadow_.GetOffset().GetY()) &&
         NearZero(imageShadow_.GetBlurRadius()));
}

void FlutterRenderOffscreenCanvas::Path2DAddPath(const PathArgs& args)
{
    SkPath out;
    SkParsePath::FromSVGString(args.cmds.c_str(), &out);
    strokePath_.addPath(out);
}

void FlutterRenderOffscreenCanvas::Path2DSetTransform(const PathArgs& args)
{
    SkMatrix skMatrix;
    double scaleX = args.para1;
    double skewX = args.para2;
    double skewY = args.para3;
    double scaleY = args.para4;
    double translateX = args.para5;
    double translateY = args.para6;
    skMatrix.setAll(scaleX, skewY, translateX, skewX, scaleY, translateY, 0, 0, 1);
    strokePath_.transform(skMatrix);
}

void FlutterRenderOffscreenCanvas::Path2DMoveTo(const PathArgs& args)
{
    double x = args.para1;
    double y = args.para2;
    strokePath_.moveTo(x, y);
}

void FlutterRenderOffscreenCanvas::Path2DLineTo(const PathArgs& args)
{
    double x = args.para1;
    double y = args.para2;
    strokePath_.lineTo(x, y);
}

void FlutterRenderOffscreenCanvas::Path2DArc(const PathArgs& args)
{
    double x = args.para1;
    double y = args.para2;
    double r = args.para3;
    auto rect = SkRect::MakeLTRB(x - r, y - r,
        x + r, y + r);
    double startAngle = args.para4 * HALF_CIRCLE_ANGLE / M_PI;
    double endAngle = args.para5 * HALF_CIRCLE_ANGLE / M_PI;
    double sweepAngle = endAngle - startAngle;
    if (!NearZero(args.para6)) {
        sweepAngle = endAngle > startAngle ?
            (std::fmod(sweepAngle, FULL_CIRCLE_ANGLE) - FULL_CIRCLE_ANGLE) : sweepAngle;
    } else {
        sweepAngle = endAngle > startAngle ?
            sweepAngle : (std::fmod(sweepAngle, FULL_CIRCLE_ANGLE) + FULL_CIRCLE_ANGLE);
    }
    if (NearEqual(std::fmod(sweepAngle, FULL_CIRCLE_ANGLE), 0.0) && !NearEqual(startAngle, endAngle)) {
        strokePath_.arcTo(rect, startAngle, HALF_CIRCLE_ANGLE, false);
        strokePath_.arcTo(rect, startAngle + HALF_CIRCLE_ANGLE, HALF_CIRCLE_ANGLE, false);
    } else {
        strokePath_.arcTo(rect, startAngle, sweepAngle, false);
    }
}

void FlutterRenderOffscreenCanvas::Path2DArcTo(const PathArgs& args)
{
    double x1 = args.para1;
    double y1 = args.para2;
    double x2 = args.para3;
    double y2 = args.para4;
    double r = args.para5;
    strokePath_.arcTo(x1, y1, x2, y2, r);
}

void FlutterRenderOffscreenCanvas::Path2DQuadraticCurveTo(const PathArgs& args)
{
    double cpx = args.para1;
    double cpy = args.para2;
    double x = args.para3;
    double y = args.para4;
    strokePath_.quadTo(cpx, cpy, x, y);
}

void FlutterRenderOffscreenCanvas::Path2DBezierCurveTo(const PathArgs& args)
{
    double cp1x = args.para1;
    double cp1y = args.para2;
    double cp2x = args.para3;
    double cp2y = args.para4;
    double x = args.para5;
    double y = args.para6;
    strokePath_.cubicTo(cp1x, cp1y, cp2x, cp2y, x, y);
}

void FlutterRenderOffscreenCanvas::Path2DEllipse(const PathArgs& args)
{
    if (NearEqual(args.para6, args.para7)) {
        return; // Just return when startAngle is same as endAngle.
    }

    double x = args.para1;
    double y = args.para2;
    double rx = args.para3;
    double ry = args.para4;
    double rotation = args.para5 * HALF_CIRCLE_ANGLE / M_PI;
    double startAngle = std::fmod(args.para6, M_PI * 2.0);
    double endAngle = std::fmod(args.para7, M_PI * 2.0);
    bool anticlockwise = NearZero(args.para8) ? false : true;
    startAngle = (startAngle < 0.0 ? startAngle + M_PI * 2.0 : startAngle) * HALF_CIRCLE_ANGLE / M_PI;
    endAngle = (endAngle < 0.0 ? endAngle + M_PI * 2.0 : endAngle) * HALF_CIRCLE_ANGLE / M_PI;
    double sweepAngle = endAngle - startAngle;
    if (anticlockwise) {
        if (sweepAngle > 0.0) { // Make sure the sweepAngle is negative when anticlockwise.
            sweepAngle -= FULL_CIRCLE_ANGLE;
        }
    } else {
        if (sweepAngle < 0.0) { // Make sure the sweepAngle is positive when clockwise.
            sweepAngle += FULL_CIRCLE_ANGLE;
        }
    }
    auto rect = SkRect::MakeLTRB(x - rx, y - ry,
        x + rx, y + ry);

    if (!NearZero(rotation)) {
        SkMatrix matrix;
        matrix.setRotate(-rotation, x, y);
        strokePath_.transform(matrix);
    }
    if (NearZero(sweepAngle) && !NearZero(args.para6 - args.para7)) {
        // The entire ellipse needs to be drawn with two arcTo.
        strokePath_.arcTo(rect, startAngle, HALF_CIRCLE_ANGLE, false);
        strokePath_.arcTo(rect, startAngle + HALF_CIRCLE_ANGLE, HALF_CIRCLE_ANGLE, false);
    } else {
        strokePath_.arcTo(rect, startAngle, sweepAngle, false);
    }
    if (!NearZero(rotation)) {
        SkMatrix matrix;
        matrix.setRotate(rotation, x, y);
        strokePath_.transform(matrix);
    }
}

void FlutterRenderOffscreenCanvas::Path2DRect(const PathArgs& args)
{
    double left = args.para1;
    double top = args.para2;
    double right = args.para3;
    double bottom = args.para4;
    strokePath_.addRect(SkRect::MakeLTRB(left, top, right, bottom));
}

void FlutterRenderOffscreenCanvas::Path2DClosePath(const PathArgs& args)
{
    strokePath_.close();
}
void FlutterRenderOffscreenCanvas::Path2DStroke()
{
    SkPaint paint = GetStrokePaint();
    paint.setAntiAlias(antiAlias_);
    if (HasShadow()) {
        FlutterDecorationPainter::PaintShadow(strokePath_, shadow_, skCanvas_.get());
    }
    if (strokeState_.GetGradient().IsValid()) {
        UpdatePaintShader(paint, strokeState_.GetGradient());
    }
    if (strokeState_.GetPattern().IsValid()) {
        UpdatePaintShader(strokeState_.GetPattern(), paint);
    }
    if (globalState_.GetType() == CompositeOperation::SOURCE_OVER) {
        skCanvas_->drawPath(strokePath_, paint);
    } else {
        InitCachePaint();
        cacheCanvas_->drawPath(strokePath_, paint);
        skCanvas_->drawBitmap(cacheBitmap_, 0, 0, &cachePaint_);
        cacheBitmap_.eraseColor(0);
    }
}
void FlutterRenderOffscreenCanvas::UpdateLineDash(SkPaint& paint)
{
    if (!strokeState_.GetLineDash().lineDash.empty()) {
        auto lineDashState = strokeState_.GetLineDash().lineDash;
        SkScalar intervals[lineDashState.size()];
        for (size_t i = 0; i < lineDashState.size(); ++i) {
            intervals[i] = SkDoubleToScalar(lineDashState[i]);
        }
        SkScalar phase = SkDoubleToScalar(strokeState_.GetLineDash().dashOffset);
        paint.setPathEffect(SkDashPathEffect::Make(intervals, lineDashState.size(), phase));
    }
}
void FlutterRenderOffscreenCanvas::ArcTo(const ArcToParam& param)
{
    double x1 = param.x1;
    double y1 = param.y1;
    double x2 = param.x2;
    double y2 = param.y2;
    double radius = param.radius;
    skPath_.arcTo(SkDoubleToScalar(x1), SkDoubleToScalar(y1), SkDoubleToScalar(x2), SkDoubleToScalar(y2),
        SkDoubleToScalar(radius));
}
void FlutterRenderOffscreenCanvas::MoveTo(double x, double y)
{
    skPath_.moveTo(SkDoubleToScalar(x), SkDoubleToScalar(y));
}
void FlutterRenderOffscreenCanvas::ClosePath()
{
    skPath_.close();
}

void FlutterRenderOffscreenCanvas::Rotate(double angle)
{
    skCanvas_->rotate(angle * 180 / M_PI);
}
void FlutterRenderOffscreenCanvas::Scale(double x, double y)
{
    skCanvas_->scale(x, y);
}

void FlutterRenderOffscreenCanvas::FillText(const std::string& text, double x, double y, const PaintState& state)
{
    if (!UpdateOffParagraph(text, false, state)) {
        return;
    }
    PaintText(text, x, y, false);
}

void FlutterRenderOffscreenCanvas::StrokeText(const std::string& text, double x, double y, const PaintState& state)
{
    if (HasShadow()) {
        if (!UpdateOffParagraph(text, true, state, true)) {
            return;
        }
        PaintText(text, x, y, true);
    }

    if (!UpdateOffParagraph(text, true, state)) {
        return;
    }
    PaintText(text, x, y, true);
}

double FlutterRenderOffscreenCanvas::MeasureText(const std::string& text, const PaintState& state)
{
    using namespace Constants;
    txt::ParagraphStyle style;
    style.text_align = ConvertTxtTextAlign(state.GetTextAlign());
    style.text_direction = ConvertTxtTextDirection(state.GetOffTextDirection());
    auto fontCollection = FlutterFontCollection::GetInstance().GetFontCollection();
    if (!fontCollection) {
        LOGW("MeasureText: fontCollection is null");
        return 0.0;
    }
    std::unique_ptr<txt::ParagraphBuilder> builder = txt::ParagraphBuilder::CreateTxtBuilder(style, fontCollection);
    txt::TextStyle txtStyle;
    ConvertTxtStyle(state.GetTextStyle(), pipelineContext_, txtStyle);
    txtStyle.font_size = state.GetTextStyle().GetFontSize().Value();
    builder->PushStyle(txtStyle);
    builder->AddText(StringUtils::Str8ToStr16(text));
    auto paragraph = builder->Build();
    paragraph->Layout(Size::INFINITE_SIZE);
    return paragraph->GetMaxIntrinsicWidth();
}

double FlutterRenderOffscreenCanvas::MeasureTextHeight(const std::string& text, const PaintState& state)
{
    using namespace Constants;
    txt::ParagraphStyle style;
    style.text_align = ConvertTxtTextAlign(state.GetTextAlign());
    style.text_direction = ConvertTxtTextDirection(state.GetOffTextDirection());
    auto fontCollection = FlutterFontCollection::GetInstance().GetFontCollection();
    if (!fontCollection) {
        LOGW("MeasureText: fontCollection is null");
        return 0.0;
    }
    std::unique_ptr<txt::ParagraphBuilder> builder = txt::ParagraphBuilder::CreateTxtBuilder(style, fontCollection);
    txt::TextStyle txtStyle;
    ConvertTxtStyle(state.GetTextStyle(), pipelineContext_, txtStyle);
    txtStyle.font_size = state.GetTextStyle().GetFontSize().Value();
    builder->PushStyle(txtStyle);
    builder->AddText(StringUtils::Str8ToStr16(text));
    auto paragraph = builder->Build();
    paragraph->Layout(Size::INFINITE_SIZE);
    return paragraph->GetHeight();
}

void FlutterRenderOffscreenCanvas::PaintText(const std::string& text, double x, double y, bool isStroke, bool hasShadow)
{
    paragraph_->Layout(width_);
    if (width_ > paragraph_->GetMaxIntrinsicWidth()) {
        paragraph_->Layout(std::ceil(paragraph_->GetMaxIntrinsicWidth()));
    }
    auto align = isStroke ? strokeState_.GetTextAlign() : fillState_.GetTextAlign();
    double dx = x + GetAlignOffset(text, align);
    auto baseline =
        isStroke ? strokeState_.GetTextStyle().GetTextBaseline() : fillState_.GetTextStyle().GetTextBaseline();
    double dy = y + GetBaselineOffset(baseline);

    if (hasShadow) {
        skCanvas_->save();
        auto shadowOffsetX = shadow_.GetOffset().GetX();
        auto shadowOffsetY = shadow_.GetOffset().GetY();
        paragraph_->Paint(skCanvas_.get(), dx + shadowOffsetX, dy + shadowOffsetY);
        skCanvas_->restore();
        return;
    }
    paragraph_->Paint(skCanvas_.get(), dx, dy);
}

double FlutterRenderOffscreenCanvas::GetAlignOffset(const std::string& text, TextAlign align)
{
    double x = 0.0;
    switch (align) {
        case TextAlign::LEFT:
            x = 0.0;
            break;
        case TextAlign::START:
            x = (GetTextDirection(text) == TextDirection::LTR) ? 0.0 : -paragraph_->GetMaxIntrinsicWidth();
            break;
        case TextAlign::RIGHT:
            x = -paragraph_->GetMaxIntrinsicWidth();
            break;
        case TextAlign::END:
            x = (GetTextDirection(text) == TextDirection::LTR) ? -paragraph_->GetMaxIntrinsicWidth() : 0.0;
            break;
        case TextAlign::CENTER:
            x = -paragraph_->GetMaxIntrinsicWidth() / 2;
            break;
        default:
            x = 0.0;
            break;
    }
    return x;
}

TextDirection FlutterRenderOffscreenCanvas::GetTextDirection(const std::string& text)
{
    auto wstring = StringUtils::ToWstring(text);
    // Find first strong direction char.
    for (const auto& charInStr : wstring) {
        auto direction = u_charDirection(charInStr);
        if (direction == UCharDirection::U_LEFT_TO_RIGHT) {
            return TextDirection::LTR;
        }
        if (direction == UCharDirection::U_RIGHT_TO_LEFT || direction == UCharDirection::U_RIGHT_TO_LEFT_ARABIC) {
            return TextDirection::RTL;
        }
    }
    return TextDirection::INHERIT;
}

void FlutterRenderOffscreenCanvas::InitCachePaint()
{
    cachePaint_.setBlendMode(
        ConvertEnumToSkEnum(globalState_.GetType(), SK_BLEND_MODE_TABLE, BLEND_MODE_SIZE, SkBlendMode::kSrcOver));
}

bool FlutterRenderOffscreenCanvas::UpdateOffParagraph(const std::string& text, bool isStroke,
    const PaintState& state, bool hasShadow)
{
    using namespace Constants;
    txt::ParagraphStyle style;
    if (isStroke) {
        style.text_align = ConvertTxtTextAlign(strokeState_.GetTextAlign());
    } else {
        style.text_align = ConvertTxtTextAlign(fillState_.GetTextAlign());
    }
    style.text_direction = ConvertTxtTextDirection(state.GetOffTextDirection());
    auto fontCollection = FlutterFontCollection::GetInstance().GetFontCollection();
    if (!fontCollection) {
        return false;
    }
    std::unique_ptr<txt::ParagraphBuilder> builder = txt::ParagraphBuilder::CreateTxtBuilder(style, fontCollection);
    txt::TextStyle txtStyle;
    if (!isStroke && hasShadow) {
        txt::TextShadow txtShadow;
        txtShadow.color = shadow_.GetColor().GetValue();
        txtShadow.offset.fX = shadow_.GetOffset().GetX();
        txtShadow.offset.fY = shadow_.GetOffset().GetY();
        txtShadow.blur_radius = shadow_.GetBlurRadius();
        txtStyle.text_shadows.emplace_back(txtShadow);
    }
    txtStyle.locale = Localization::GetInstance()->GetFontLocale();
    UpdateTextStyleForeground(isStroke, txtStyle, hasShadow);
    builder->PushStyle(txtStyle);
    builder->AddText(StringUtils::Str8ToStr16(text));
    paragraph_ = builder->Build();
    return true;
}

void FlutterRenderOffscreenCanvas::UpdateTextStyleForeground(
    bool isStroke, txt::TextStyle& txtStyle, bool hasShadow)
{
    using namespace Constants;
    if (!isStroke) {
        txtStyle.color = ConvertSkColor(fillState_.GetColor());
        txtStyle.font_size = fillState_.GetTextStyle().GetFontSize().Value();
        ConvertTxtStyle(fillState_.GetTextStyle(), pipelineContext_, txtStyle);
        if (fillState_.GetGradient().IsValid()) {
            SkPaint paint;
            paint.setStyle(SkPaint::Style::kFill_Style);
            UpdatePaintShader(paint, fillState_.GetGradient());
            txtStyle.foreground = paint;
            txtStyle.has_foreground = true;
        }
        if (globalState_.HasGlobalAlpha()) {
            if (txtStyle.has_foreground) {
                txtStyle.foreground.setColor(fillState_.GetColor().GetValue());
                txtStyle.foreground.setAlphaf(globalState_.GetAlpha()); // set alpha after color
            } else {
                SkPaint paint;
                paint.setColor(fillState_.GetColor().GetValue());
                paint.setAlphaf(globalState_.GetAlpha()); // set alpha after color
                txtStyle.foreground = paint;
                txtStyle.has_foreground = true;
            }
        }
    } else {
        // use foreground to draw stroke
        SkPaint paint = GetStrokePaint();
        ConvertTxtStyle(strokeState_.GetTextStyle(), pipelineContext_, txtStyle);
        txtStyle.font_size = strokeState_.GetTextStyle().GetFontSize().Value();
        if (strokeState_.GetGradient().IsValid()) {
            UpdatePaintShader(paint, strokeState_.GetGradient());
        }
        if (hasShadow) {
            paint.setColor(shadow_.GetColor().GetValue());
            paint.setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kNormal_SkBlurStyle,
                FlutterDecorationPainter::ConvertRadiusToSigma(shadow_.GetBlurRadius())));
        }
        txtStyle.foreground = paint;
        txtStyle.has_foreground = true;
    }
}

double FlutterRenderOffscreenCanvas::GetBaselineOffset(TextBaseline baseline)
{
    double y = 0.0;
    switch (baseline) {
        case TextBaseline::ALPHABETIC:
            y = -paragraph_->GetAlphabeticBaseline();
            break;
        case TextBaseline::IDEOGRAPHIC:
            y = -paragraph_->GetIdeographicBaseline();
            break;
        case TextBaseline::BOTTOM:
            y = -paragraph_->GetHeight();
            break;
        case TextBaseline::TOP:
            y = 0.0;
            break;
        case TextBaseline::MIDDLE:
            y = -paragraph_->GetHeight() / 2;
            break;
        case TextBaseline::HANGING:
            y = -HANGING_PERCENT * (paragraph_->GetHeight() - paragraph_->GetAlphabeticBaseline());
            break;
        default:
            y = -paragraph_->GetAlphabeticBaseline();
            break;
    }
    return y;
}
void FlutterRenderOffscreenCanvas::LineTo(double x, double y)
{
    skPath_.lineTo(SkDoubleToScalar(x), SkDoubleToScalar(y));
}
void FlutterRenderOffscreenCanvas::BezierCurveTo(const BezierCurveParam& param)
{
    skPath_.cubicTo(SkDoubleToScalar(param.cp1x), SkDoubleToScalar(param.cp1y),
        SkDoubleToScalar(param.cp2x), SkDoubleToScalar(param.cp2y),
        SkDoubleToScalar(param.x), SkDoubleToScalar(param.y));
}
void FlutterRenderOffscreenCanvas::QuadraticCurveTo(const QuadraticCurveParam& param)
{
    skPath_.quadTo(SkDoubleToScalar(param.cpx), SkDoubleToScalar(param.cpy),
        SkDoubleToScalar(param.x), SkDoubleToScalar(param.y));
}
void FlutterRenderOffscreenCanvas::Ellipse(const EllipseParam& param)
{
    // Init the start and end angle, then calculated the sweepAngle.
    double startAngle = std::fmod(param.startAngle, M_PI * 2.0);
    double endAngle = std::fmod(param.endAngle, M_PI * 2.0);
    startAngle = (startAngle < 0.0 ? startAngle + M_PI * 2.0 : startAngle) * HALF_CIRCLE_ANGLE / M_PI;
    endAngle = (endAngle < 0.0 ? endAngle + M_PI * 2.0 : endAngle) * HALF_CIRCLE_ANGLE / M_PI;
    if (NearEqual(param.startAngle, param.endAngle)) {
        return; // Just return when startAngle is same as endAngle.
    }
    double rotation = param.rotation * HALF_CIRCLE_ANGLE / M_PI;
    double sweepAngle = endAngle - startAngle;
    if (param.anticlockwise) {
        if (sweepAngle > 0.0) { // Make sure the sweepAngle is negative when anticlockwise.
            sweepAngle -= FULL_CIRCLE_ANGLE;
        }
    } else {
        if (sweepAngle < 0.0) { // Make sure the sweepAngle is positive when clockwise.
            sweepAngle += FULL_CIRCLE_ANGLE;
        }
    }

    // Init the oval Rect(left, top, right, bottom).
    double left = param.x - param.radiusX;
    double top = param.y - param.radiusY;
    double right = param.x + param.radiusX;
    double bottom = param.y + param.radiusY;
    auto rect = SkRect::MakeLTRB(left, top, right, bottom);
    if (!NearZero(rotation)) {
        SkMatrix matrix;
        matrix.setRotate(-rotation, param.x, param.y);
        skPath_.transform(matrix);
    }
    if (NearZero(sweepAngle) && !NearZero(param.endAngle - param.startAngle)) {
        // The entire ellipse needs to be drawn with two arcTo.
        skPath_.arcTo(rect, startAngle, HALF_CIRCLE_ANGLE, false);
        skPath_.arcTo(rect, startAngle + HALF_CIRCLE_ANGLE, HALF_CIRCLE_ANGLE, false);
    } else {
        skPath_.arcTo(rect, startAngle, sweepAngle, false);
    }
    if (!NearZero(rotation)) {
        SkMatrix matrix;
        matrix.setRotate(rotation, param.x, param.y);
        skPath_.transform(matrix);
    }
}
void FlutterRenderOffscreenCanvas::SetTransform(const TransformParam& param)
{
    auto pipeline = pipelineContext_.Upgrade();
    if (!pipeline) {
        return;
    }

    // use physical pixel to store bitmap
    double viewScale = pipeline->GetViewScale();

    SkMatrix skMatrix;
    skMatrix.setAll(param.scaleX * viewScale, param.skewY * viewScale, param.translateX, param.skewX * viewScale,
        param.scaleY * viewScale, param.translateY, 0, 0, 1);
    skCanvas_->setMatrix(skMatrix);
}
void FlutterRenderOffscreenCanvas::Transform(const TransformParam& param)
{
    SkMatrix skMatrix;
    skMatrix.setAll(param.scaleX, param.skewY, param.translateX, param.skewX, param.scaleY, param.translateY, 0, 0, 1);
    skCanvas_->concat(skMatrix);
}
void FlutterRenderOffscreenCanvas::Translate(double x, double y)
{
    skCanvas_->translate(x, y);
}

void FlutterRenderOffscreenCanvas::TranspareCmdToPath(const RefPtr<CanvasPath2D>& path)
{
    strokePath_.reset();
    for (const auto& [cmd, args] : path->GetCaches()) {
        switch (cmd) {
            case PathCmd::CMDS: {
                Path2DAddPath(args);
                break;
            }
            case PathCmd::TRANSFORM: {
                Path2DSetTransform(args);
                break;
            }
            case PathCmd::MOVE_TO: {
                Path2DMoveTo(args);
                break;
            }
            case PathCmd::LINE_TO: {
                Path2DLineTo(args);
                break;
            }
            case PathCmd::ARC: {
                Path2DArc(args);
                break;
            }
            case PathCmd::ARC_TO: {
                Path2DArcTo(args);
                break;
            }
            case PathCmd::QUADRATIC_CURVE_TO: {
                Path2DQuadraticCurveTo(args);
                break;
            }
            case PathCmd::BEZIER_CURVE_TO: {
                Path2DBezierCurveTo(args);
                break;
            }
            case PathCmd::ELLIPSE: {
                Path2DEllipse(args);
                break;
            }
            case PathCmd::RECT: {
                Path2DRect(args);
                break;
            }
            case PathCmd::CLOSE_PATH: {
                Path2DClosePath(args);
                break;
            }
            default: {
                break;
            }
        }
    }
}

bool FlutterRenderOffscreenCanvas::IsPointInPathByColor(double x, double y, SkPath& path, SkColor colorMatch)
{
    auto imageInfo = SkImageInfo::Make(width_, height_, SkColorType::kRGBA_8888_SkColorType,
        SkAlphaType::kOpaque_SkAlphaType);
    SkBitmap skBitmap;
    skBitmap.allocPixels(imageInfo);
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(skBitmap);

    SkPaint paint;
    paint.setColor(SK_ColorRED);
    paint.setStyle(SkPaint::Style::kFill_Style);
    skCanvas->drawPath(path, paint);

    paint.setColor(SK_ColorBLUE);
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setStrokeWidth(static_cast<SkScalar>(strokeState_.GetLineWidth()));
    skCanvas->drawPath(path, paint);

    SkColor color = skBitmap.getColor(x, y);
    if (color == colorMatch) {
        return true;
    }
    return false;
}

bool FlutterRenderOffscreenCanvas::IsPointInPath(double x, double y)
{
    return IsPointInPathByColor(x, y, skPath_, SK_ColorRED);
}

bool FlutterRenderOffscreenCanvas::IsPointInPath(const RefPtr<CanvasPath2D>& path, double x, double y)
{
    TranspareCmdToPath(path);
    return IsPointInPathByColor(x, y, strokePath_, SK_ColorRED);
}

bool FlutterRenderOffscreenCanvas::IsPointInStroke(double x, double y)
{
    return IsPointInPathByColor(x, y, skPath_, SK_ColorBLUE);
}

bool FlutterRenderOffscreenCanvas::IsPointInStroke(const RefPtr<CanvasPath2D>& path, double x, double y)
{
    TranspareCmdToPath(path);
    return IsPointInPathByColor(x, y, strokePath_, SK_ColorBLUE);
}

void FlutterRenderOffscreenCanvas::ResetTransform()
{
    SkMatrix skMatrix;
    skMatrix.setAll(1, 0, 0, 0, 1, 0, 0, 0, 1);
    skCanvas_->setMatrix(skMatrix);
}

void FlutterRenderOffscreenCanvas::InitFliterFunc()
{
    filterFunc_["grayscale"] = [&](const std::string& percentage) {
        SetGrayFilter(percentage);
    };
    filterFunc_["sepia"] = [&](const std::string& percentage) {
        SetSepiaFilter(percentage);
    };
    filterFunc_["invert"] = [&](const std::string& percentage) {
        SetInvertFilter(percentage);
    };
    filterFunc_["opacity"] = [&](const std::string& percentage) {
        SetOpacityFilter(percentage);
    };
    filterFunc_["brightness"] = [&](const std::string& percentage) {
        SetBrightnessFilter(percentage);
    };
    filterFunc_["contrast"] = [&](const std::string& percentage) {
        SetContrastFilter(percentage);
    };
    filterFunc_["blur"] = [&](const std::string& percentage) {
        SetBlurFilter(percentage);
    };
    filterFunc_["drop-shadow"] = [&](const std::string& percentage) {
        SetDropShadowFilter(percentage);
    };
    filterFunc_["saturate"] = [&](const std::string& percentage) {
        SetSaturateFilter(percentage);
    };
    filterFunc_["hue-rotate"] = [&](const std::string& percentage) {
        SetHueRotateFilter(percentage);
    };
}

bool FlutterRenderOffscreenCanvas::GetFilterType(std::string& filterType, std::string& filterParam)
{
    std::string paramData = filterParam_;
    size_t index = paramData.find("(");
    if (index == std::string::npos) {
        return false;
    }
    filterType = paramData.substr(0, index);
    filterParam = paramData.substr(index + 1);
    size_t endeIndex = filterParam.find(")");
    if (endeIndex  == std::string::npos) {
        return false;
    }
    filterParam.erase(endeIndex, 1);
    return true;
}

bool FlutterRenderOffscreenCanvas::IsPercentStr(std::string& percent)
{
    if (percent.find("%") != std::string::npos) {
        size_t index = percent.find("%");
        percent = percent.substr(0, index);
        return true;
    }
    return false;
}

double FlutterRenderOffscreenCanvas::PxStrToDouble(const std::string& str)
{
    double ret = 0;
    size_t index = str.find("px");
    if (index != std::string::npos) {
        std::string result = str.substr(0, index);
        std::istringstream iss(result);
        iss >> ret;
    }
    return  ret;
}

double FlutterRenderOffscreenCanvas::BlurStrToDouble(const std::string& str)
{
    double ret = 0;
    size_t index = str.find("px");
    size_t index1 = str.find("rem");
    size_t demIndex = std::string::npos;
    if (index != std::string::npos) {
        demIndex = index;
    }
    if (index1 != std::string::npos) {
        demIndex = index1;
    }
    if (demIndex != std::string::npos) {
        std::string result = str.substr(0, demIndex);
        std::istringstream iss(result);
        iss >> ret;
    }
    if (str.find("rem") != std::string::npos) {
        return  ret * 15;
    }
    return  ret;
}

void FlutterRenderOffscreenCanvas::SetGrayFilter(const std::string& percent)
{
    std::string percentage = percent;
    bool hasPercent = IsPercentStr(percentage);
    float percentNum = 0.0f;
    std::istringstream iss(percentage);
    iss >> percentNum;
    if (hasPercent) {
        percentNum = percentNum / 100;
    }
    if (percentNum > 1) {
        percentNum = 1;
    }
    float otherColor = percentNum / 3;
    float primColor = 1 - 2 * otherColor;
    float matrix[20] = {0};
    matrix[0] = matrix[6] = matrix[12] = primColor;
    matrix[1] = matrix[2] = matrix[5] = matrix[7] = matrix[10] = matrix[11] = otherColor;
    matrix[18] = 1.0f;
    SetColorFilter(matrix);
}

void FlutterRenderOffscreenCanvas::SetSepiaFilter(const std::string& percent)
{
    std::string percentage = percent;
    bool hasPercent = IsPercentStr(percentage);
    float percentNum = 0.0f;
    std::istringstream iss(percentage);
    iss >> percentNum;
    if (hasPercent) {
        percentNum = percentNum / 100;
    }
    if (percentNum > 1) {
        percentNum = 1;
    }
    float matrix[20] = {0};
    matrix[0] = 1.0f - percentNum * 0.6412f;
    matrix[1] = percentNum * 0.7044f;
    matrix[2] = percentNum * 0.1368f;
    matrix[5] = percentNum * 0.2990f;
    matrix[6] = 1.0f - percentNum * 0.4130f;
    matrix[7] = percentNum * 0.1140f;
    matrix[10] = percentNum * 0.2392f;
    matrix[11] = percentNum * 0.4696f;
    matrix[12] = 1.0f - percentNum * 0.9088f;
    matrix[18] = 1.0f;
    SetColorFilter(matrix);
}

void FlutterRenderOffscreenCanvas::SetInvertFilter(const std::string& filterParam)
{
    std::string percent = filterParam;
    bool hasPercent = IsPercentStr(percent);
    float percentage = 0.0f;
    std::istringstream iss(percent);
    iss >> percentage;
    if (hasPercent) {
        percentage = percentage / 100;
    }

    float matrix[20] = {0};
    matrix[0] = matrix[6] = matrix[12] = 1.0 - 2.0 * percentage;
    matrix[4] = matrix[9] = matrix[14] = percentage;
    matrix[18] = 1.0f;
    SetColorFilter(matrix);
}

void FlutterRenderOffscreenCanvas::SetOpacityFilter(const std::string& filterParam)
{
    std::string percent = filterParam;
    bool hasPercent = IsPercentStr(percent);
    float percentage = 0.0f;
    std::istringstream iss(percent);
    iss >> percentage;
    if (hasPercent) {
        percentage = percentage / 100;
    }

    float matrix[20] = {0};
    matrix[0] = matrix[6] = matrix[12] = 1.0f;
    matrix[18] = percentage;
    SetColorFilter(matrix);
}

void FlutterRenderOffscreenCanvas::SetBrightnessFilter(const std::string& percent)
{
    std::string perStr = percent;
    bool hasPercent = IsPercentStr(perStr);
    float percentage = 0.0f;
    std::istringstream iss(perStr);
    iss >> percentage;
    if (hasPercent) {
        percentage = percentage / 100;
    }

    if (percentage < 0) {
        return;
    }
    float matrix[20] = {0};
    matrix[0] = matrix[6] = matrix[12] = percentage;
    matrix[18] = 1.0f;
    SetColorFilter(matrix);
}

void FlutterRenderOffscreenCanvas::SetContrastFilter(const std::string& percent)
{
    std::string perStr = percent;
    float percentage = 0.0f;
    bool hasPercent = IsPercentStr(perStr);
    std::istringstream iss(perStr);
    iss >> percentage;
    if (hasPercent) {
        percentage = percentage / 100;
    }

    float matrix[20] = {0};
    matrix[0] = matrix[6] = matrix[12] = percentage;
    matrix[4] = matrix[9] = matrix[14] = 0.5f * (1 - percentage);
    matrix[18] = 1;
    SetColorFilter(matrix);
}

void FlutterRenderOffscreenCanvas::SetBlurFilter(const std::string& percent)
{
    imagePaint_.setImageFilter(SkBlurImageFilter::Make(BlurStrToDouble(percent), BlurStrToDouble(percent), nullptr));
}

void FlutterRenderOffscreenCanvas::SetDropShadowFilter(const std::string& percent)
{
    std::vector<std::string> offsets;
    StringUtils::StringSpliter(percent, ' ', offsets);
    if (offsets.empty() || offsets.size() != 4) {
        return;
    }
    imageShadow_.SetOffsetX(PxStrToDouble(offsets[0]));
    imageShadow_.SetOffsetY(PxStrToDouble(offsets[1]));
    imageShadow_.SetBlurRadius(PxStrToDouble(offsets[2]));
    imageShadow_.SetColor(Color::FromString(offsets[3]));
}

void FlutterRenderOffscreenCanvas::SetSaturateFilter(const std::string& filterParam)
{
    std::string percent = filterParam;
    bool hasPercent = IsPercentStr(percent);
    float percentage = 0.0f;
    std::istringstream iss(percent);
    iss >> percentage;
    if (hasPercent) {
        percentage = percentage / 100;
    }
    double N = percentage;
    float matrix[20] = {0};
    matrix[0] = 0.3086f * (1 - N) + N;
    matrix[1] = matrix[11] = 0.6094f * (1 - N);
    matrix[2] = matrix[7] = 0.0820f * (1 - N);
    matrix[5] = matrix[10] = 0.3086f * (1 - N);
    matrix[6] = 0.6094f * (1 - N) + N;
    matrix[12] = 0.0820f * (1 - N) + N;
    matrix[18] = 1.0f;
    SetColorFilter(matrix);
}

void FlutterRenderOffscreenCanvas::SetHueRotateFilter(const std::string& filterParam)
{
    std::string percent = filterParam;
    float degree = 0.0f;
    if (percent.find("deg") != std::string::npos) {
        size_t index = percent.find("deg");
        percent = percent.substr(0, index);
        std::istringstream iss(percent);
        iss >> degree;
    }
    if (percent.find("turn") != std::string::npos) {
        size_t index = percent.find("turn");
        percent = percent.substr(0, index);
        std::istringstream iss(percent);
        iss >> degree;
        degree = degree * 360;
    }
    if (percent.find("rad") != std::string::npos) {
        size_t index = percent.find("rad");
        percent = percent.substr(0, index);
        std::istringstream iss(percent);
        iss >> degree;
        degree = degree * 180 / 3.142f;
    }

    while (GreatOrEqual(degree, 360)) {
        degree -= 360;
    }

    float matrix[20] = {0};
    int32_t type = degree / 120;
    float N = (degree - 120 * type) / 120;
    switch (type) {
        case 0:
            // degree is in 0-120
            matrix[0] = matrix[6] = matrix[12] = 1 - N;
            matrix[2] = matrix[5] = matrix[11] = N;
            matrix[18] = 1.0f;
            break;
        case 1:
            // degree is in 120-240
            matrix[1] = matrix[7] = matrix[10] = N;
            matrix[2] = matrix[5] = matrix[11] = 1 - N;
            matrix[18] = 1.0f;
            break;
        case 2:
            // degree is in 240-360
            matrix[0] = matrix[6] = matrix[11] = N;
            matrix[1] = matrix[7] = matrix[10] = 1 - N;
            matrix[18] = 1.0f;
            break;
        default:
            break;
    }
    SetColorFilter(matrix);
}

void FlutterRenderOffscreenCanvas::SetColorFilter(float matrix[20])
{
#ifdef USE_SYSTEM_SKIA
    matrix[4] *= 255;
    matrix[9] *= 255;
    matrix[14] *= 255;
    matrix[19] *= 255;
    imagePaint_.setColorFilter(SkColorFilter::MakeMatrixFilterRowMajor255(matrix));
#else
    imagePaint_.setColorFilter(SkColorFilters::Matrix(matrix));
#endif
}
} // namespace OHOS::Ace
