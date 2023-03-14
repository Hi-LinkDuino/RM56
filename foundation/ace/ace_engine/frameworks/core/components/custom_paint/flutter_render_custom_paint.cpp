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

#include "core/components/custom_paint/flutter_render_custom_paint.h"

#include <cmath>

#include "flutter/lib/ui/text/font_collection.h"
#include "flutter/third_party/txt/src/txt/paragraph_builder.h"
#include "flutter/third_party/txt/src/txt/paragraph_style.h"
#include "third_party/skia/include/core/SkBlendMode.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkMaskFilter.h"
#include "third_party/skia/include/core/SkPoint.h"
#include "third_party/skia/include/effects/SkDashPathEffect.h"
#include "third_party/skia/include/effects/SkGradientShader.h"
#include "third_party/skia/include/encode/SkJpegEncoder.h"
#include "third_party/skia/include/encode/SkPngEncoder.h"
#include "third_party/skia/include/encode/SkWebpEncoder.h"
#include "third_party/skia/include/utils/SkBase64.h"
#include "third_party/skia/include/utils/SkParsePath.h"

#include "base/i18n/localization.h"
#include "base/json/json_util.h"
#include "base/utils/linear_map.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"
#include "core/components/calendar/flutter_render_calendar.h"
#include "core/components/common/painter/flutter_decoration_painter.h"
#include "core/components/font/constants_converter.h"
#include "core/components/font/flutter_font_collection.h"
#include "core/image/image_provider.h"
#include "core/pipeline/base/flutter_render_context.h"
#include "utils/native/base/include/securec.h"

namespace OHOS::Ace {
namespace {

constexpr double HANGING_PERCENT = 0.8;
constexpr double HALF_CIRCLE_ANGLE = 180.0;
constexpr double FULL_CIRCLE_ANGLE = 360.0;

constexpr double DEFAULT_QUALITY = 0.92;
constexpr int32_t MAX_LENGTH = 2048 * 2048;
const std::string UNSUPPORTED = "data:image/png";
const std::string URL_PREFIX = "data:";
const std::string URL_SYMBOL = ";base64,";
const std::string IMAGE_PNG = "image/png";
const std::string IMAGE_JPEG = "image/jpeg";
const std::string IMAGE_WEBP = "image/webp";

// If args is empty or invalid format, use default: image/png
std::string GetMimeType(const std::string& args)
{
    // Args example: ["image/png"]
    std::vector<std::string> values;
    StringUtils::StringSpliter(args, '"', values);
    if (values.size() < 3) {
        return IMAGE_PNG;
    } else {
        // Convert to lowercase string.
        for (size_t i = 0; i < values[1].size(); ++i) {
            values[1][i] = static_cast<uint8_t>(tolower(values[1][i]));
        }
        return values[1];
    }
}

// Quality need between 0.0 and 1.0 for MimeType jpeg and webp
double GetQuality(const std::string& args)
{
    // Args example: ["image/jpeg", 0.8]
    std::vector<std::string> values;
    StringUtils::StringSpliter(args, ',', values);
    if (values.size() < 2) {
        return DEFAULT_QUALITY;
    }
    auto mimeType = GetMimeType(args);
    if (mimeType != IMAGE_JPEG && mimeType != IMAGE_WEBP) {
        return DEFAULT_QUALITY;
    }
    double quality = StringUtils::StringToDouble(values[1]);
    if (quality < 0.0 || quality > 1.0) {
        return DEFAULT_QUALITY;
    }
    return quality;
}

template<typename T, typename N>
N ConvertEnumToSkEnum(T key, const LinearEnumMapNode<T, N>* map, size_t length, N defaultValue)
{
    int64_t index = BinarySearchFindIndex(map, length, key);
    return index != -1 ? map[index].value : defaultValue;
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

RenderLayer FlutterRenderCustomPaint::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::ClipLayer>(
            0, GetLayoutSize().Width(), 0, GetLayoutSize().Height(), Flutter::Clip::HARD_EDGE);
    }
    return AceType::RawPtr(layer_);
}

FlutterRenderCustomPaint::FlutterRenderCustomPaint()
{
    auto currentDartState = flutter::UIDartState::Current();
    if (!currentDartState) {
        return;
    }

    renderTaskHolder_ = MakeRefPtr<FlutterRenderTaskHolder>(
        currentDartState->GetSkiaUnrefQueue(),
        currentDartState->GetIOManager(),
        currentDartState->GetTaskRunners().GetIOTaskRunner());
}

void FlutterRenderCustomPaint::Paint(RenderContext& context, const Offset& offset)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        return;
    }
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return;
    }
    // use physical pixel to store bitmap
    double viewScale = pipeline->GetViewScale();
    if (tasks_.empty()) {
        if (canvasCache_.readyToDraw()) {
            canvas->canvas()->scale(1.0 / viewScale, 1.0 / viewScale);
            canvas->canvas()->drawBitmap(canvasCache_, 0.0f, 0.0f);
        }
        return;
    }
    if (!canvasCache_.readyToDraw()) {
        auto imageInfo = SkImageInfo::Make(GetLayoutSize().Width() * viewScale, GetLayoutSize().Height() * viewScale,
            SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);
        canvasCache_.allocPixels(imageInfo);
        cacheBitmap_.allocPixels(imageInfo);
        canvasCache_.eraseColor(SK_ColorTRANSPARENT);
        cacheBitmap_.eraseColor(SK_ColorTRANSPARENT);
        skCanvas_ = std::make_unique<SkCanvas>(canvasCache_);
        cacheCanvas_ = std::make_unique<SkCanvas>(cacheBitmap_);
    }
    skCanvas_->scale(viewScale, viewScale);
    // paint tasks
    for (const auto& task : tasks_) {
        task(*this, offset);
    }
    canvas->canvas()->scale(1.0 / viewScale, 1.0 / viewScale);
    canvas->canvas()->drawBitmap(canvasCache_, 0.0f, 0.0f);
    skCanvas_->scale(1.0 / viewScale, 1.0 / viewScale);
    tasks_.clear();
}

SkPaint FlutterRenderCustomPaint::GetStrokePaint()
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

std::string FlutterRenderCustomPaint::ToDataURL(const std::string& args)
{
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return UNSUPPORTED;
    }

    std::string mimeType = GetMimeType(args);
    double quality = GetQuality(args);
    double width = GetLayoutSize().Width();
    double height = GetLayoutSize().Height();
    SkBitmap tempCache;
    tempCache.allocPixels(SkImageInfo::Make(width, height, SkColorType::kBGRA_8888_SkColorType,
        (mimeType == IMAGE_JPEG) ? SkAlphaType::kOpaque_SkAlphaType : SkAlphaType::kUnpremul_SkAlphaType));
    SkCanvas tempCanvas(tempCache);
    double viewScale = pipeline->GetViewScale();
    tempCanvas.clear(SK_ColorTRANSPARENT);
    tempCanvas.scale(1.0 / viewScale, 1.0 / viewScale);
    tempCanvas.drawBitmap(canvasCache_, 0.0f, 0.0f);
    SkPixmap src;
    bool success = tempCache.peekPixels(&src);
    if (!success) {
        LOGE("PeekPixels failed when ToDataURL.");
        return UNSUPPORTED;
    }
    SkDynamicMemoryWStream dst;
    if (mimeType == IMAGE_JPEG) {
        SkJpegEncoder::Options options;
        options.fQuality = quality * 100;
        success = SkJpegEncoder::Encode(&dst, src, options);
    } else if (mimeType == IMAGE_WEBP) {
        SkWebpEncoder::Options options;
        options.fQuality = quality * 100.0;
        success = SkWebpEncoder::Encode(&dst, src, options);
    } else {
        mimeType = IMAGE_PNG;
        SkPngEncoder::Options options;
        success = SkPngEncoder::Encode(&dst, src, options);
    }
    if (!success) {
        LOGE("Encode failed when ToDataURL.");
        return UNSUPPORTED;
    }
    auto result = dst.detachAsData();
    if (result == nullptr) {
        LOGE("DetachAsData failed when ToDataURL.");
        return UNSUPPORTED;
    }
    size_t len = SkBase64::Encode(result->data(), result->size(), nullptr);
    if (len > MAX_LENGTH) {
        LOGE("ToDataURL failed, image too large.");
        return UNSUPPORTED;
    }
    SkString info(len);
    SkBase64::Encode(result->data(), result->size(), info.writable_str());
    return std::string(URL_PREFIX).append(mimeType).append(URL_SYMBOL).append(info.c_str());
}

void FlutterRenderCustomPaint::SetAntiAlias(bool isEnabled)
{
    antiAlias_ = isEnabled;
}

void FlutterRenderCustomPaint::TransferFromImageBitmap(const RefPtr<OffscreenCanvas>& offscreenCanvas)
{
    std::unique_ptr<ImageData> imageData = offscreenCanvas->GetImageData(0, 0,
        offscreenCanvas->GetWidth(), offscreenCanvas->GetHeight());
    if (imageData != nullptr) {
        ImageData* imageDataPtr = imageData.get();
        PutImageData(Offset(0, 0), *imageDataPtr);
    }
}

void FlutterRenderCustomPaint::Mesh(SkBitmap& bitmap, int column, int row,
    const float* vertices, const int* colors, const SkPaint* paint)
{
    const int vertCounts = (column + 1) * (row + 1);
    int32_t size = 6;
    const int indexCount = column * row * size;
    uint32_t flags = SkVertices::kHasTexCoords_BuilderFlag;
    if (colors) {
        flags |= SkVertices::kHasColors_BuilderFlag;
    }
    SkVertices::Builder builder(SkVertices::kTriangles_VertexMode, vertCounts, indexCount, flags);
    if (memcpy_s(builder.positions(), vertCounts * sizeof(SkPoint), vertices, vertCounts * sizeof(SkPoint)) != 0) {
        return;
    }
    if (colors) {
        if (memcpy_s(builder.colors(), vertCounts * sizeof(SkColor), colors, vertCounts * sizeof(SkColor)) != 0) {
            return;
        }
    }
    SkPoint* texsPoint = builder.texCoords();
    uint16_t* indices = builder.indices();
    const SkScalar height = SkIntToScalar(bitmap.height());
    const SkScalar width = SkIntToScalar(bitmap.width());

    if (row == 0) {
        LOGE("row is zero");
        return;
    }
    if (column == 0) {
        LOGE("column is zero");
        return;
    }
    const SkScalar dy = height / row;
    const SkScalar dx = width / column;

    SkPoint* texsPit = texsPoint;
    SkScalar y = 0;
    for (int i = 0; i <= row; i++) {
        if (i == row) {
            y = height;  // to ensure numerically we hit h exactly
        }
        SkScalar x = 0;
        for (int j = 0; j < column; j++) {
            texsPit->set(x, y);
            texsPit += 1;
            x += dx;
        }
        texsPit->set(width, y);
        texsPit += 1;
        y += dy;
    }

    uint16_t* dex = indices;
    int index = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            *dex++ = index;
            *dex++ = index + column + 1;
            *dex++ = index + column + 2;

            *dex++ = index;
            *dex++ = index + column + 2;
            *dex++ = index + 1;

            index += 1;
        }
        index += 1;
    }

    SkPaint tempPaint;
    if (paint) {
        tempPaint = *paint;
    }
    sk_sp<SkColorFilter> colorFter;
    sk_sp<SkShader> shader;
    sk_sp<SkImage> image = SkImage::MakeFromBitmap(bitmap);
#ifdef USE_SYSTEM_SKIA
    shader = image->makeShader(SkShader::kClamp_TileMode, SkShader::kClamp_TileMode);
#else
    shader = image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp);
#endif
    if (colorFter) {
        shader = shader->makeWithColorFilter(colorFter);
    }
    tempPaint.setShader(shader);
    skCanvas_->drawVertices(builder.detach(), SkBlendMode::kModulate, tempPaint);
}


void FlutterRenderCustomPaint::DrawBitmapMesh(const RefPtr<OffscreenCanvas>& offscreenCanvas,
    const std::vector<double>& mesh, int32_t column, int32_t row)
{
    std::unique_ptr<ImageData> imageData = offscreenCanvas->GetImageData(0, 0,
        offscreenCanvas->GetWidth(), offscreenCanvas->GetHeight());
    if (imageData != nullptr) {

        if (imageData->data.empty()) {
            LOGE("PutImageData failed, image data is empty.");
            return;
        }
        uint32_t* data = new (std::nothrow)uint32_t[imageData->data.size()];
        if (data == nullptr) {
            LOGE("PutImageData failed, new data is null.");
            return;
        }

        for (uint32_t i = 0; i < imageData->data.size(); ++i) {
            data[i] = imageData->data[i].GetValue();
        }
        SkBitmap skBitmap;
        auto imageInfo = SkImageInfo::Make(imageData->dirtyWidth, imageData->dirtyHeight,
            SkColorType::kBGRA_8888_SkColorType, SkAlphaType::kOpaque_SkAlphaType);
        skBitmap.allocPixels(imageInfo);
        skBitmap.setPixels(data);

        uint32_t size = mesh.size();
        float verts[size];
        for (uint32_t i = 0; i < size; i++) {
            verts[i] = mesh[i];
        }

        Mesh(skBitmap, column, row, verts, 0, nullptr);

        delete[] data;
    }

}

void FlutterRenderCustomPaint::FillRect(const Offset& offset, const Rect& rect)
{
    SkPaint paint;
    paint.setAntiAlias(antiAlias_);
    paint.setColor(fillState_.GetColor().GetValue());
    paint.setStyle(SkPaint::Style::kFill_Style);
    SkRect skRect = SkRect::MakeLTRB(rect.Left() + offset.GetX(), rect.Top() + offset.GetY(),
        rect.Right() + offset.GetX(), offset.GetY() + rect.Bottom());
    if (HasShadow()) {
        SkPath path;
        path.addRect(skRect);
        FlutterDecorationPainter::PaintShadow(path, shadow_, skCanvas_.get());
    }
    if (fillState_.GetGradient().IsValid()) {
        UpdatePaintShader(offset, paint, fillState_.GetGradient());
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

void FlutterRenderCustomPaint::StrokeRect(const Offset& offset, const Rect& rect)
{
    SkPaint paint = GetStrokePaint();
    paint.setAntiAlias(antiAlias_);
    SkRect skRect = SkRect::MakeLTRB(rect.Left() + offset.GetX(), rect.Top() + offset.GetY(),
        rect.Right() + offset.GetX(), offset.GetY() + rect.Bottom());
    if (HasShadow()) {
        SkPath path;
        path.addRect(skRect);
        FlutterDecorationPainter::PaintShadow(path, shadow_, skCanvas_.get());
    }
    if (strokeState_.GetGradient().IsValid()) {
        UpdatePaintShader(offset, paint, strokeState_.GetGradient());
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

void FlutterRenderCustomPaint::ClearRect(const Offset& offset, const Rect& rect)
{
    SkPaint paint;
    paint.setAntiAlias(antiAlias_);
    paint.setBlendMode(SkBlendMode::kClear);
    auto skRect = SkRect::MakeLTRB(rect.Left() + offset.GetX(), rect.Top() + offset.GetY(),
        rect.Right() + offset.GetX(), rect.Bottom() + offset.GetY());
    skCanvas_->drawRect(skRect, paint);
}

void FlutterRenderCustomPaint::FillText(const Offset& offset, const std::string& text, double x, double y)
{
    if (!UpdateParagraph(offset, text, false)) {
        return;
    }
    PaintText(offset, x, y, false);
}

void FlutterRenderCustomPaint::StrokeText(const Offset& offset, const std::string& text, double x, double y)
{
    if (HasShadow()) {
        if (!UpdateParagraph(offset, text, true, true)) {
            return;
        }
        PaintText(offset, x, y, true);
    }

    if (!UpdateParagraph(offset, text, true)) {
        return;
    }
    PaintText(offset, x, y, true);
}

double FlutterRenderCustomPaint::MeasureText(const std::string& text, const PaintState& state)
{
    using namespace Constants;
    txt::ParagraphStyle style;
    style.text_align = ConvertTxtTextAlign(state.GetTextAlign());
    auto fontCollection = FlutterFontCollection::GetInstance().GetFontCollection();
    if (!fontCollection) {
        LOGW("MeasureText: fontCollection is null");
        return 0.0;
    }
    std::unique_ptr<txt::ParagraphBuilder> builder = txt::ParagraphBuilder::CreateTxtBuilder(style, fontCollection);
    txt::TextStyle txtStyle;
    ConvertTxtStyle(state.GetTextStyle(), context_, txtStyle);
    txtStyle.font_size = state.GetTextStyle().GetFontSize().Value();
    builder->PushStyle(txtStyle);
    builder->AddText(StringUtils::Str8ToStr16(text));
    auto paragraph = builder->Build();
    paragraph->Layout(Size::INFINITE_SIZE);
    return paragraph->GetMaxIntrinsicWidth();
}

double FlutterRenderCustomPaint::MeasureTextHeight(const std::string& text, const PaintState& state)
{
    using namespace Constants;
    txt::ParagraphStyle style;
    style.text_align = ConvertTxtTextAlign(state.GetTextAlign());
    auto fontCollection = FlutterFontCollection::GetInstance().GetFontCollection();
    if (!fontCollection) {
        LOGW("MeasureText: fontCollection is null");
        return 0.0;
    }
    std::unique_ptr<txt::ParagraphBuilder> builder = txt::ParagraphBuilder::CreateTxtBuilder(style, fontCollection);
    txt::TextStyle txtStyle;
    ConvertTxtStyle(state.GetTextStyle(), context_, txtStyle);
    txtStyle.font_size = state.GetTextStyle().GetFontSize().Value();
    builder->PushStyle(txtStyle);
    builder->AddText(StringUtils::Str8ToStr16(text));
    auto paragraph = builder->Build();
    paragraph->Layout(Size::INFINITE_SIZE);
    return paragraph->GetHeight();
}

void FlutterRenderCustomPaint::PaintText(const Offset& offset, double x, double y, bool isStroke, bool hasShadow)
{
    paragraph_->Layout(GetLayoutSize().Width());
    if (GetLayoutSize().Width() > paragraph_->GetMaxIntrinsicWidth()) {
        paragraph_->Layout(std::ceil(paragraph_->GetMaxIntrinsicWidth()));
    }
    auto align = isStroke ? strokeState_.GetTextAlign() : fillState_.GetTextAlign();
    double dx = offset.GetX() + x + GetAlignOffset(align);
    auto baseline =
        isStroke ? strokeState_.GetTextStyle().GetTextBaseline() : fillState_.GetTextStyle().GetTextBaseline();
    double dy = offset.GetY() + y + GetBaselineOffset(baseline);

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

double FlutterRenderCustomPaint::GetAlignOffset(TextAlign align)
{
    double x = 0.0;
    switch (align) {
        case TextAlign::LEFT:
            x = 0.0;
            break;
        case TextAlign::START:
            x = (GetTextDirection() == TextDirection::LTR) ? 0.0 : -paragraph_->GetMaxIntrinsicWidth();
            break;
        case TextAlign::RIGHT:
            x = -paragraph_->GetMaxIntrinsicWidth();
            break;
        case TextAlign::END:
            x = (GetTextDirection() == TextDirection::LTR) ? -paragraph_->GetMaxIntrinsicWidth() : 0.0;
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

double FlutterRenderCustomPaint::GetBaselineOffset(TextBaseline baseline)
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

void FlutterRenderCustomPaint::MoveTo(const Offset& offset, double x, double y)
{
    skPath_.moveTo(SkDoubleToScalar(x + offset.GetX()), SkDoubleToScalar(y + offset.GetY()));
}

void FlutterRenderCustomPaint::LineTo(const Offset& offset, double x, double y)
{
    skPath_.lineTo(SkDoubleToScalar(x + offset.GetX()), SkDoubleToScalar(y + offset.GetY()));
}

void FlutterRenderCustomPaint::BezierCurveTo(const Offset& offset, const BezierCurveParam& param)
{
    skPath_.cubicTo(SkDoubleToScalar(param.cp1x + offset.GetX()), SkDoubleToScalar(param.cp1y + offset.GetY()),
        SkDoubleToScalar(param.cp2x + offset.GetX()), SkDoubleToScalar(param.cp2y + offset.GetY()),
        SkDoubleToScalar(param.x + offset.GetX()), SkDoubleToScalar(param.y + offset.GetY()));
}

void FlutterRenderCustomPaint::QuadraticCurveTo(const Offset& offset, const QuadraticCurveParam& param)
{
    skPath_.quadTo(SkDoubleToScalar(param.cpx + offset.GetX()), SkDoubleToScalar(param.cpy + offset.GetY()),
        SkDoubleToScalar(param.x + offset.GetX()), SkDoubleToScalar(param.y + offset.GetY()));
}

void FlutterRenderCustomPaint::Arc(const Offset& offset, const ArcParam& param)
{
    double left = param.x - param.radius + offset.GetX();
    double top = param.y - param.radius + offset.GetY();
    double right = param.x + param.radius + offset.GetX();
    double bottom = param.y + param.radius + offset.GetY();
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

void FlutterRenderCustomPaint::ArcTo(const Offset& offset, const ArcToParam& param)
{
    double x1 = param.x1 + offset.GetX();
    double y1 = param.y1 + offset.GetY();
    double x2 = param.x2 + offset.GetX();
    double y2 = param.y2 + offset.GetY();
    double radius = param.radius;
    skPath_.arcTo(SkDoubleToScalar(x1), SkDoubleToScalar(y1), SkDoubleToScalar(x2), SkDoubleToScalar(y2),
        SkDoubleToScalar(radius));
}

void FlutterRenderCustomPaint::Ellipse(const Offset& offset, const EllipseParam& param)
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
    double left = param.x - param.radiusX + offset.GetX();
    double top = param.y - param.radiusY + offset.GetY();
    double right = param.x + param.radiusX + offset.GetX();
    double bottom = param.y + param.radiusY + offset.GetY();
    auto rect = SkRect::MakeLTRB(left, top, right, bottom);
    if (!NearZero(rotation)) {
        SkMatrix matrix;
        matrix.setRotate(-rotation, param.x + offset.GetX(), param.y + offset.GetY());
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
        matrix.setRotate(rotation, param.x + offset.GetX(), param.y + offset.GetY());
        skPath_.transform(matrix);
    }
}

void FlutterRenderCustomPaint::AddRect(const Offset& offset, const Rect& rect)
{
    SkRect skRect = SkRect::MakeLTRB(rect.Left() + offset.GetX(), rect.Top() + offset.GetY(),
        rect.Right() + offset.GetX(), offset.GetY() + rect.Bottom());
    skPath_.addRect(skRect);
}

void FlutterRenderCustomPaint::Fill(const Offset& offset)
{
    SkPaint paint;
    paint.setAntiAlias(antiAlias_);
    paint.setColor(fillState_.GetColor().GetValue());
    paint.setStyle(SkPaint::Style::kFill_Style);
    if (HasShadow()) {
        FlutterDecorationPainter::PaintShadow(skPath_, shadow_, skCanvas_.get());
    }
    if (fillState_.GetGradient().IsValid()) {
        UpdatePaintShader(offset, paint, fillState_.GetGradient());
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

void FlutterRenderCustomPaint::Stroke(const Offset& offset)
{
    SkPaint paint = GetStrokePaint();
    paint.setAntiAlias(antiAlias_);
    if (HasShadow()) {
        FlutterDecorationPainter::PaintShadow(skPath_, shadow_, skCanvas_.get());
    }
    if (strokeState_.GetGradient().IsValid()) {
        UpdatePaintShader(offset, paint, strokeState_.GetGradient());
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

void FlutterRenderCustomPaint::Stroke(const Offset& offset, const RefPtr<CanvasPath2D>& path)
{
    if (path == nullptr) {
        LOGE("Stroke failed, target path is null.");
        return;
    }
    for (const auto& [cmd, args] : path->GetCaches()) {
        switch (cmd) {
            case PathCmd::CMDS: {
                Path2DAddPath(offset, args);
                break;
            }
            case PathCmd::TRANSFORM: {
                Path2DSetTransform(offset, args);
                break;
            }
            case PathCmd::MOVE_TO: {
                Path2DMoveTo(offset, args);
                break;
            }
            case PathCmd::LINE_TO: {
                Path2DLineTo(offset, args);
                break;
            }
            case PathCmd::ARC: {
                Path2DArc(offset, args);
                break;
            }
            case PathCmd::ARC_TO: {
                Path2DArcTo(offset, args);
                break;
            }
            case PathCmd::QUADRATIC_CURVE_TO: {
                Path2DQuadraticCurveTo(offset, args);
                break;
            }
            case PathCmd::BEZIER_CURVE_TO: {
                Path2DBezierCurveTo(offset, args);
                break;
            }
            case PathCmd::ELLIPSE: {
                Path2DEllipse(offset, args);
                break;
            }
            case PathCmd::RECT: {
                Path2DRect(offset, args);
                break;
            }
            case PathCmd::CLOSE_PATH: {
                Path2DClosePath(offset, args);
                break;
            }
            default: {
                break;
            }
        }
    }
    Path2DStroke(offset);
    strokePath_.reset();
}

void FlutterRenderCustomPaint::Path2DAddPath(const Offset& offset, const PathArgs& args)
{
    SkPath out;
    SkParsePath::FromSVGString(args.cmds.c_str(), &out);
    strokePath_.addPath(out);
}

void FlutterRenderCustomPaint::Path2DSetTransform(const Offset& offset, const PathArgs& args)
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

void FlutterRenderCustomPaint::Path2DMoveTo(const Offset& offset, const PathArgs& args)
{
    double x = args.para1 + offset.GetX();
    double y = args.para2 + offset.GetY();
    strokePath_.moveTo(x, y);
}

void FlutterRenderCustomPaint::Path2DLineTo(const Offset& offset, const PathArgs& args)
{
    double x = args.para1 + offset.GetX();
    double y = args.para2 + offset.GetY();
    strokePath_.lineTo(x, y);
}

void FlutterRenderCustomPaint::Path2DArc(const Offset& offset, const PathArgs& args)
{
    double x = args.para1;
    double y = args.para2;
    double r = args.para3;
    auto rect = SkRect::MakeLTRB(x - r + offset.GetX(), y - r + offset.GetY(),
                                 x + r + offset.GetX(), y + r + offset.GetY());
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

void FlutterRenderCustomPaint::Path2DArcTo(const Offset& offset, const PathArgs& args)
{
    double x1 = args.para1 + offset.GetX();
    double y1 = args.para2 + offset.GetY();
    double x2 = args.para3 + offset.GetX();
    double y2 = args.para4 + offset.GetY();
    double r = args.para5;
    strokePath_.arcTo(x1, y1, x2, y2, r);
}

void FlutterRenderCustomPaint::Path2DQuadraticCurveTo(const Offset& offset, const PathArgs& args)
{
    double cpx = args.para1 + offset.GetX();
    double cpy = args.para2 + offset.GetY();
    double x = args.para3 + offset.GetX();
    double y = args.para4 + offset.GetY();
    strokePath_.quadTo(cpx, cpy, x, y);
}

void FlutterRenderCustomPaint::Path2DBezierCurveTo(const Offset& offset, const PathArgs& args)
{
    double cp1x = args.para1 + offset.GetX();
    double cp1y = args.para2 + offset.GetY();
    double cp2x = args.para3 + offset.GetX();
    double cp2y = args.para4 + offset.GetY();
    double x = args.para5 + offset.GetX();
    double y = args.para6 + offset.GetY();
    strokePath_.cubicTo(cp1x, cp1y, cp2x, cp2y, x, y);
}

void FlutterRenderCustomPaint::Path2DEllipse(const Offset& offset, const PathArgs& args)
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
    auto rect = SkRect::MakeLTRB(x - rx + offset.GetX(), y - ry + offset.GetY(),
                                 x + rx + offset.GetX(), y + ry + offset.GetY());

    if (!NearZero(rotation)) {
        SkMatrix matrix;
        matrix.setRotate(-rotation, x + offset.GetX(), y + offset.GetY());
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
        matrix.setRotate(rotation, x + offset.GetX(), y + offset.GetY());
        strokePath_.transform(matrix);
    }
}

void FlutterRenderCustomPaint::Path2DRect(const Offset& offset, const PathArgs& args)
{
    double left = args.para1 + offset.GetX();
    double top = args.para2 + offset.GetY();
    double right = args.para3 + offset.GetX();
    double bottom = args.para4 + offset.GetY();
    strokePath_.addRect(SkRect::MakeLTRB(left, top, right, bottom));
}

void FlutterRenderCustomPaint::Path2DClosePath(const Offset& offset, const PathArgs& args)
{
    strokePath_.close();
}

void FlutterRenderCustomPaint::Path2DStroke(const Offset& offset)
{
    SkPaint paint = GetStrokePaint();
    paint.setAntiAlias(antiAlias_);
    if (HasShadow()) {
        FlutterDecorationPainter::PaintShadow(strokePath_, shadow_, skCanvas_.get());
    }
    if (strokeState_.GetGradient().IsValid()) {
        UpdatePaintShader(offset, paint, strokeState_.GetGradient());
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

void FlutterRenderCustomPaint::Clip()
{
    skCanvas_->clipPath(skPath_);
}

void FlutterRenderCustomPaint::BeginPath()
{
    skPath_.reset();
}

void FlutterRenderCustomPaint::ClosePath()
{
    skPath_.close();
}

void FlutterRenderCustomPaint::Save()
{
    SaveStates();
    skCanvas_->save();
}

void FlutterRenderCustomPaint::Restore()
{
    RestoreStates();
    skCanvas_->restore();
}

void FlutterRenderCustomPaint::InitImagePaint()
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
}

void FlutterRenderCustomPaint::InitCachePaint()
{
    cachePaint_.setBlendMode(
        ConvertEnumToSkEnum(globalState_.GetType(), SK_BLEND_MODE_TABLE, BLEND_MODE_SIZE, SkBlendMode::kSrcOver));
}

bool FlutterRenderCustomPaint::UpdateParagraph(
    const Offset& offset, const std::string& text, bool isStroke, bool hasShadow)
{
    using namespace Constants;
    txt::ParagraphStyle style;
    if (isStroke) {
        style.text_align = ConvertTxtTextAlign(strokeState_.GetTextAlign());
    } else {
        style.text_align = ConvertTxtTextAlign(fillState_.GetTextAlign());
    }
    auto fontCollection = FlutterFontCollection::GetInstance().GetFontCollection();
    if (!fontCollection) {
        LOGW("UpdateParagraph: fontCollection is null");
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
    UpdateTextStyleForeground(offset, isStroke, txtStyle, hasShadow);
    builder->PushStyle(txtStyle);
    builder->AddText(StringUtils::Str8ToStr16(text));
    paragraph_ = builder->Build();
    return true;
}

void FlutterRenderCustomPaint::UpdateTextStyleForeground(
    const Offset& offset, bool isStroke, txt::TextStyle& txtStyle, bool hasShadow)
{
    using namespace Constants;
    if (!isStroke) {
        txtStyle.color = ConvertSkColor(fillState_.GetColor());
        txtStyle.font_size = fillState_.GetTextStyle().GetFontSize().Value();
        ConvertTxtStyle(fillState_.GetTextStyle(), context_, txtStyle);
        if (fillState_.GetGradient().IsValid()) {
            SkPaint paint;
            paint.setStyle(SkPaint::Style::kFill_Style);
            UpdatePaintShader(offset, paint, fillState_.GetGradient());
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
        ConvertTxtStyle(strokeState_.GetTextStyle(), context_, txtStyle);
        txtStyle.font_size = strokeState_.GetTextStyle().GetFontSize().Value();
        if (strokeState_.GetGradient().IsValid()) {
            UpdatePaintShader(offset, paint, strokeState_.GetGradient());
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

bool FlutterRenderCustomPaint::HasShadow() const
{
    return !(NearZero(shadow_.GetOffset().GetX()) && NearZero(shadow_.GetOffset().GetY()) &&
             NearZero(shadow_.GetBlurRadius()));
}

void FlutterRenderCustomPaint::UpdatePaintShader(const Offset& offset, SkPaint& paint, const Gradient& gradient)
{
    SkPoint beginPoint = SkPoint::Make(SkDoubleToScalar(gradient.GetBeginOffset().GetX() + offset.GetX()),
        SkDoubleToScalar(gradient.GetBeginOffset().GetY() + offset.GetY()));
    SkPoint endPoint = SkPoint::Make(SkDoubleToScalar(gradient.GetEndOffset().GetX() + offset.GetX()),
        SkDoubleToScalar(gradient.GetEndOffset().GetY() + offset.GetY()));
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

void FlutterRenderCustomPaint::Rotate(double angle)
{
    skCanvas_->rotate(angle * 180 / M_PI);
}

void FlutterRenderCustomPaint::Scale(double x, double y)
{
    skCanvas_->scale(x, y);
}

void FlutterRenderCustomPaint::SetTransform(const TransformParam& param)
{
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return;
    }
    // use physical pixel to store bitmap
    double viewScale = pipeline->GetViewScale();
    SkMatrix skMatrix;
    skMatrix.setAll(param.scaleX * viewScale, param.skewY * viewScale, param.translateX * viewScale,
        param.skewX * viewScale, param.scaleY * viewScale, param.translateY * viewScale, 0, 0, 1);
    skCanvas_->setMatrix(skMatrix);
}

void FlutterRenderCustomPaint::Transform(const TransformParam& param)
{
    SkMatrix skMatrix;
    skMatrix.setAll(param.scaleX, param.skewY, param.translateX, param.skewX, param.scaleY, param.translateY, 0, 0, 1);
    skCanvas_->concat(skMatrix);
}

void FlutterRenderCustomPaint::Translate(double x, double y)
{
    skCanvas_->translate(x, y);
}

void FlutterRenderCustomPaint::UpdateLineDash(SkPaint& paint)
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

void FlutterRenderCustomPaint::DrawImage(
    const Offset& offset, const CanvasImage& canvasImage, double width, double height)
{
    if (!flutter::UIDartState::Current()) {
        return;
    }

    auto context = GetContext().Upgrade();
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

void FlutterRenderCustomPaint::DrawPixelMap(RefPtr<PixelMap> pixelMap, const CanvasImage& canvasImage)
{
    if (!flutter::UIDartState::Current()) {
        return;
    }

    auto context = GetContext().Upgrade();
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

void FlutterRenderCustomPaint::UpdatePaintShader(const Pattern& pattern, SkPaint& paint)
{
    if (!flutter::UIDartState::Current()) {
        return;
    }

    auto context = GetContext().Upgrade();
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

void FlutterRenderCustomPaint::PutImageData(const Offset& offset, const ImageData& imageData)
{
    if (imageData.data.empty()) {
        LOGE("PutImageData failed, image data is empty.");
        return;
    }
    uint32_t* data = new (std::nothrow)uint32_t[imageData.data.size()];
    if (data == nullptr) {
        LOGE("PutImageData failed, new data is null.");
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

std::unique_ptr<ImageData> FlutterRenderCustomPaint::GetImageData(double left, double top, double width, double height)
{
    double viewScale = 1.0;
    auto pipeline = context_.Upgrade();
    if (pipeline) {
        viewScale = pipeline->GetViewScale();
    }
    // copy the bitmap to tempCanvas
    auto imageInfo =
        SkImageInfo::Make(width, height, SkColorType::kBGRA_8888_SkColorType, SkAlphaType::kOpaque_SkAlphaType);
    SkBitmap tempCache;
    double scaledLeft = left * viewScale;
    double scaledTop = top * viewScale;
    double dirtyWidth = width >= 0 ? width : 0;
    double dirtyHeight = height >= 0 ? height : 0;
    tempCache.allocPixels(imageInfo);
    SkCanvas tempCanvas(tempCache);
    auto srcRect = SkRect::MakeXYWH(scaledLeft, scaledTop, width * viewScale, height * viewScale);
    auto dstRect = SkRect::MakeXYWH(0.0, 0.0, dirtyWidth, dirtyHeight);
    tempCanvas.drawBitmapRect(canvasCache_, srcRect, dstRect, nullptr);
    int32_t size = dirtyWidth * dirtyHeight;
    // write color
    std::unique_ptr<uint8_t[]> pixels = std::make_unique<uint8_t[]>(size * 4);
    tempCache.readPixels(imageInfo, pixels.get(), dirtyWidth * imageInfo.bytesPerPixel(), 0, 0);
    std::unique_ptr<ImageData> imageData = std::make_unique<ImageData>();
    imageData->dirtyWidth = dirtyWidth;
    imageData->dirtyHeight = dirtyHeight;
    // a pixel include 4 data(blue, green, red, alpha)
    for (int i = 0; i < size * 4; i += 4) {
        auto blue = pixels[i];
        auto green = pixels[i + 1];
        auto red = pixels[i + 2];
        auto alpha = pixels[i + 3];
        imageData->data.emplace_back(Color::FromARGB(alpha, red, green, blue));
    }
    return imageData;
}

std::string FlutterRenderCustomPaint::GetJsonData(const std::string& path)
{
    AssetImageLoader imageLoader;
    return imageLoader.LoadJsonData(path, context_.Upgrade());
}

void FlutterRenderCustomPaint::WebGLInit(CanvasRenderContextBase* context)
{
    webGLContext_ = context;
    if (webGLContext_) {
        auto pipeline = context_.Upgrade();
        if (!pipeline) {
            return;
        }
        double viewScale = pipeline->GetViewScale();
        if (!webglBitmap_.readyToDraw()) {
            auto imageInfo =
                SkImageInfo::Make(GetLayoutSize().Width() * viewScale, GetLayoutSize().Height() * viewScale,
                    SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kOpaque_SkAlphaType);
            webglBitmap_.allocPixels(imageInfo);
#ifdef USE_SYSTEM_SKIA
            webglBitmap_.eraseColor(SK_ColorTRANSPARENT);
#endif
        }
        webGLContext_->SetBitMapPtr(reinterpret_cast<char*>(webglBitmap_.getPixels()),
            webglBitmap_.width(), webglBitmap_.height());
    }
}

void FlutterRenderCustomPaint::WebGLUpdate()
{
    LOGD("FlutterRenderCustomPaint::WebGLUpdate");
    if (skCanvas_ && webglBitmap_.readyToDraw()) {
        skCanvas_->save();
        /* Do mirror flip */
        skCanvas_->setMatrix(SkMatrix::MakeScale(1.0, -1.0));
        skCanvas_->drawBitmap(webglBitmap_, 0, -webglBitmap_.height());
        skCanvas_->restore();
    }
}

} // namespace OHOS::Ace
