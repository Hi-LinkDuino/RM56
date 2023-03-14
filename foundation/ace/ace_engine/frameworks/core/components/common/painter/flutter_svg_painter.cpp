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

#include "core/components/common/painter/flutter_svg_painter.h"

#include "include/core/SkColor.h"
#include "include/core/SkPathMeasure.h"
#include "include/core/SkRSXform.h"
#include "include/core/SkTextBlob.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/effects/SkLumaColorFilter.h"
#include "include/utils/SkParsePath.h"
#include "third_party/skia/include/effects/SkGradientShader.h"

#include "frameworks/core/components/svg/flutter_render_svg_pattern.h"
#include "frameworks/core/components/transform/flutter_render_transform.h"

namespace OHOS::Ace {

namespace {

constexpr int32_t COL_INDEX = 2;
constexpr int32_t ROW_INDEX = 3;
constexpr float ROTATE_VALUE = 0.0005f;
constexpr float FLAT_ANGLE = 180.0f;
const char ROTATE_TYPE_AUTO[] = "auto";
const char ROTATE_TYPE_REVERSE[] = "auto-reverse";

} // namespace

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
const char FONT_TYPE_HWCHINESE[] = "/system/fonts/DroidSansChinese.ttf";
const char FONT_TYPE_DROIDSANS[] = "/system/fonts/DroidSans.ttf";
sk_sp<SkTypeface> FlutterSvgPainter::fontTypeChinese_ = SkTypeface::MakeFromFile(FONT_TYPE_HWCHINESE);
sk_sp<SkTypeface> FlutterSvgPainter::fontTypeNormal_ = SkTypeface::MakeFromFile(FONT_TYPE_DROIDSANS);
#else
sk_sp<SkTypeface> FlutterSvgPainter::fontTypeChinese_;
sk_sp<SkTypeface> FlutterSvgPainter::fontTypeNormal_;
#endif

void FlutterSvgPainter::SetMask(SkCanvas* canvas)
{
    SkPaint mask_filter;
#ifdef USE_SYSTEM_SKIA
    auto outerFilter = SkLumaColorFilter::Make();
    auto innerFilter = SkColorFilter::MakeSRGBToLinearGamma();
    auto filter = SkColorFilter::MakeComposeFilter(outerFilter, std::move(innerFilter));
    mask_filter.setColorFilter(filter);
#else
    auto outerFilter = SkLumaColorFilter::Make();
    auto innerFilter = SkColorFilters::SRGBToLinearGamma();
    auto filter = SkColorFilters::Compose(outerFilter, std::move(innerFilter));
    mask_filter.setColorFilter(filter);
#endif
    canvas->saveLayer(nullptr, &mask_filter);
}

void FlutterSvgPainter::SetFillStyle(SkPaint& skPaint, const FillState& fillState, uint8_t opacity, bool antiAlias)
{
    double curOpacity = fillState.GetOpacity().GetValue() * opacity * (1.0f / UINT8_MAX);
    skPaint.setStyle(SkPaint::Style::kFill_Style);
    skPaint.setAntiAlias(antiAlias);
    if (fillState.GetGradient()) {
        SetGradientStyle(skPaint, fillState, curOpacity);
    } else {
        skPaint.setColor(fillState.GetColor().BlendOpacity(curOpacity).GetValue());
    }
}

void FlutterSvgPainter::SetFillStyle(
    SkCanvas* skCanvas, const SkPath& skPath, const FillState& fillState, uint8_t opacity, bool antiAlias)
{
    if (fillState.GetColor() == Color::TRANSPARENT && !fillState.GetGradient()) {
        return;
    }
    SkPaint paint;
    SetFillStyle(paint, fillState, opacity, antiAlias);
    skCanvas->drawPath(skPath, paint);
}

void FlutterSvgPainter::SetGradientStyle(SkPaint& skPaint, const FillState& fillState, double opacity)
{
    auto gradient = fillState.GetGradient();
    if (!gradient) {
        return;
    }
    auto gradientColors = gradient->GetColors();
    if (gradientColors.empty()) {
        return;
    }
    std::vector<SkScalar> pos;
    std::vector<SkColor> colors;
    for (const auto& gradientColor : gradientColors) {
        pos.push_back(gradientColor.GetDimension().Value());
        colors.push_back(
            gradientColor.GetColor().BlendOpacity(gradientColor.GetOpacity()).BlendOpacity(opacity).GetValue());
    }
    if (gradient->GetType() == GradientType::LINEAR) {
        auto info = gradient->GetLinearGradientInfo();
        SkPoint pts[2] = { SkPoint::Make(info.x1, info.y1), SkPoint::Make(info.x2, info.y2) };
#ifdef USE_SYSTEM_SKIA
        skPaint.setShader(SkGradientShader::MakeLinear(pts, &colors[0], &pos[0], gradientColors.size(),
            static_cast<SkShader::TileMode>(gradient->GetSpreadMethod()), 0, nullptr));
#else
        skPaint.setShader(SkGradientShader::MakeLinear(pts, &colors[0], &pos[0], gradientColors.size(),
            static_cast<SkTileMode>(gradient->GetSpreadMethod()), 0, nullptr));
#endif
    }
    if (gradient->GetType() == GradientType::RADIAL) {
        auto info = gradient->GetRadialGradientInfo();
        auto center = SkPoint::Make(info.cx, info.cy);
        auto focal = SkPoint::Make(info.fx, info.fx);
#ifdef USE_SYSTEM_SKIA
        return center == focal ? skPaint.setShader(SkGradientShader::MakeRadial(center, info.r, &colors[0], &pos[0],
                                     gradientColors.size(),
                                     static_cast<SkShader::TileMode>(gradient->GetSpreadMethod()), 0, nullptr))
                               : skPaint.setShader(SkGradientShader::MakeTwoPointConical(focal, 0, center, info.r,
                                     &colors[0], &pos[0], gradientColors.size(),
                                     static_cast<SkShader::TileMode>(gradient->GetSpreadMethod()), 0, nullptr));
#else
        return center == focal
                   ? skPaint.setShader(SkGradientShader::MakeRadial(center, info.r, &colors[0], &pos[0],
                         gradientColors.size(), static_cast<SkTileMode>(gradient->GetSpreadMethod()), 0, nullptr))
                   : skPaint.setShader(
                         SkGradientShader::MakeTwoPointConical(focal, 0, center, info.r, &colors[0], &pos[0],
                             gradientColors.size(), static_cast<SkTileMode>(gradient->GetSpreadMethod()), 0, nullptr));
#endif
    }
}

void FlutterSvgPainter::SetFillStyle(
    SkCanvas* canvas, const SkPath& skPath, const FillState& fillState, RenderInfo& renderInfo)
{
    const auto& fillHref = fillState.GetHref();
    if (fillHref.empty() || fillState.GetGradient() || !renderInfo.node) {
        return SetFillStyle(canvas, skPath, fillState, renderInfo.opacity, renderInfo.antiAlias);
    }

    SkPaint skPaint;
    skPaint.reset();
    auto pattern = AceType::DynamicCast<FlutterRenderSvgPattern>(renderInfo.node->GetPatternFromRoot(fillHref));
    if (!pattern) {
        return;
    }
    if (!pattern->OnAsPaint(renderInfo.offset, renderInfo.node->GetPaintBounds(renderInfo.offset), skPaint)) {
        return;
    }
    skPaint.setAlphaf(fillState.GetOpacity().GetValue() * renderInfo.opacity * (1.0f / UINT8_MAX));
    skPaint.setAntiAlias(renderInfo.antiAlias);
    canvas->drawPath(skPath, skPaint);
}

void FlutterSvgPainter::SetStrokeStyle(SkPaint& skPaint, const StrokeState& strokeState, uint8_t opacity,
    bool antiAlias)
{
    skPaint.setStyle(SkPaint::Style::kStroke_Style);
    double curOpacity = strokeState.GetOpacity().GetValue() * opacity * (1.0f / UINT8_MAX);
    skPaint.setColor(strokeState.GetColor().BlendOpacity(curOpacity).GetValue());
    if (strokeState.GetLineCap() == LineCapStyle::ROUND) {
        skPaint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    } else if (strokeState.GetLineCap() == LineCapStyle::SQUARE) {
        skPaint.setStrokeCap(SkPaint::Cap::kSquare_Cap);
    } else {
        skPaint.setStrokeCap(SkPaint::Cap::kButt_Cap);
    }
    if (strokeState.GetLineJoin() == LineJoinStyle::ROUND) {
        skPaint.setStrokeJoin(SkPaint::Join::kRound_Join);
    } else if (strokeState.GetLineJoin() == LineJoinStyle::BEVEL) {
        skPaint.setStrokeJoin(SkPaint::Join::kBevel_Join);
    } else {
        skPaint.setStrokeJoin(SkPaint::Join::kMiter_Join);
    }
    skPaint.setStrokeWidth(static_cast<SkScalar>(strokeState.GetLineWidth().Value()));
    skPaint.setStrokeMiter(static_cast<SkScalar>(strokeState.GetMiterLimit()));
    skPaint.setAntiAlias(antiAlias);
    UpdateLineDash(skPaint, strokeState);
}

void FlutterSvgPainter::SetStrokeStyle(
    SkCanvas* skCanvas, const SkPath& skPath, const StrokeState& strokeState, uint8_t opacity, bool antiAlias)
{
    if (strokeState.GetColor() == Color::TRANSPARENT) {
        return;
    }
    if (GreatNotEqual(strokeState.GetLineWidth().Value(), 0.0)) {
        SkPaint paint;
        SetStrokeStyle(paint, strokeState, opacity, antiAlias);
        skCanvas->drawPath(skPath, paint);
    }
}

void FlutterSvgPainter::SetStrokeStyle(
    SkCanvas* skCanvas, const SkPath& skPath, const StrokeState& strokeState, RenderInfo& renderInfo)
{
    const auto& strokeHref = strokeState.GetHref();
    if (strokeHref.empty() || !renderInfo.node) {
        return SetStrokeStyle(skCanvas, skPath, strokeState, renderInfo.opacity, renderInfo.antiAlias);
    }

    if (GreatNotEqual(strokeState.GetLineWidth().Value(), 0.0)) {
        SkPaint paint;
        SetStrokeStyle(paint, strokeState, renderInfo.opacity, renderInfo.antiAlias);
        auto pattern = AceType::DynamicCast<FlutterRenderSvgPattern>(renderInfo.node->GetPatternFromRoot(strokeHref));
        if (!pattern) {
            return;
        }
        if (!pattern->OnAsPaint(renderInfo.offset, renderInfo.node->GetPaintBounds(renderInfo.offset), paint)) {
            return;
        }
        skCanvas->drawPath(skPath, paint);
    }
}

void FlutterSvgPainter::UpdateLineDash(SkPaint& paint, const StrokeState& strokeState)
{
    if (!strokeState.GetLineDash().lineDash.empty()) {
        auto lineDashState = strokeState.GetLineDash().lineDash;
        SkScalar intervals[lineDashState.size()];
        for (size_t i = 0; i < lineDashState.size(); ++i) {
            intervals[i] = SkDoubleToScalar(lineDashState[i]);
        }
        SkScalar phase = SkDoubleToScalar(strokeState.GetLineDash().dashOffset);
        paint.setPathEffect(SkDashPathEffect::Make(intervals, lineDashState.size(), phase));
    }
}

void FlutterSvgPainter::CheckFontType()
{
    if (!fontTypeChinese_) {
        LOGW("can't load HwChinese-Medium.ttf");
    }
    if (!fontTypeNormal_) {
        LOGW("can't load DroidSans.ttf");
    }
}

double FlutterSvgPainter::GetPathLength(const std::string& path)
{
    SkPath skPath;
    SkParsePath::FromSVGString(path.c_str(), &skPath);
    SkPathMeasure pathMeasure(skPath, false);
    SkScalar length = pathMeasure.getLength();
    return length;
}

Offset FlutterSvgPainter::GetPathOffset(const std::string& path, double current)
{
    SkPath skPath;
    SkParsePath::FromSVGString(path.c_str(), &skPath);
    SkPathMeasure pathMeasure(skPath, false);
    SkPoint position;
    if (!pathMeasure.getPosTan(current, &position, nullptr)) {
        return Offset(0.0, 0.0);
    }
    return Offset(position.fX, position.fY);
}

bool FlutterSvgPainter::GetMotionPathPosition(const std::string& path, double percent, MotionPathPosition& result)
{
    if (path.empty()) {
        return false;
    }
    SkPath motion;
    SkParsePath::FromSVGString(path.c_str(), &motion);
    SkPathMeasure pathMeasure(motion, false);
    SkPoint position;
    SkVector tangent;
    bool ret = pathMeasure.getPosTan(pathMeasure.getLength() * percent, &position, &tangent);
    if (!ret) {
        return false;
    }
    result.rotate = SkRadiansToDegrees(std::atan2(tangent.y(), tangent.x()));
    result.offset.SetX(position.x());
    result.offset.SetY(position.y());
    return true;
}

Offset FlutterSvgPainter::UpdateText(SkCanvas* canvas, const SvgTextInfo& svgTextInfo, const TextDrawInfo& textDrawInfo)
{
    Offset offset = textDrawInfo.offset;
    if (!canvas) {
        LOGE("Paint skCanvas is null");
        return offset;
    }

    SkFont font;

    font.setSize(svgTextInfo.textStyle.GetFontSize().Value());
    font.setScaleX(1.0);
    double space = 0.0;
    SkScalar x = SkDoubleToScalar(offset.GetX());
    SkScalar y = SkDoubleToScalar(offset.GetY());
    std::wstring data = StringUtils::ToWstring(svgTextInfo.data);

    SkPaint paint;
    SkPaint strokePaint;
    FlutterSvgPainter::SetFillStyle(paint, svgTextInfo.fillState, svgTextInfo.opacity);
    FlutterSvgPainter::SetStrokeStyle(strokePaint, svgTextInfo.strokeState, svgTextInfo.opacity);

    for (int i = 0; i < (int)data.size(); i++) {
        wchar_t temp = data[i];
        if (temp >= 0x4e00 && temp <= 0x9fa5) {
            // range of chinese
            font.setTypeface(fontTypeChinese_);
        } else {
            font.setTypeface(fontTypeNormal_);
        }
        auto blob = SkTextBlob::MakeFromText(&temp, sizeof(temp), font, SkTextEncoding::kUTF16);
        auto width = font.measureText(&temp, sizeof(temp), SkTextEncoding::kUTF16);
        canvas->save();
        canvas->rotate(textDrawInfo.rotate, x, y);
        canvas->drawTextBlob(blob.get(), x, y, paint);
        if (svgTextInfo.strokeState.HasStroke() && !NearZero(svgTextInfo.strokeState.GetLineWidth().Value())) {
            canvas->drawTextBlob(blob.get(), x, y, strokePaint);
        }
        canvas->restore();
        x = x + width + space;
    }

    return Offset(x, y);
}

double FlutterSvgPainter::UpdateTextPath(
    SkCanvas* canvas, const SvgTextInfo& svgTextInfo, const PathDrawInfo& pathDrawInfo)
{
    double offset = pathDrawInfo.offset;
    if (!canvas) {
        LOGE("Paint skCanvas is null");
        return offset;
    }

    SkFont font;
    font.setSize(svgTextInfo.textStyle.GetFontSize().Value());
    font.setScaleX(1.0);
    double space = 0.0;
    std::wstring data = StringUtils::ToWstring(svgTextInfo.data);

    SkPaint paint;
    SkPaint strokePaint;
    FlutterSvgPainter::SetFillStyle(paint, svgTextInfo.fillState, svgTextInfo.opacity);
    FlutterSvgPainter::SetStrokeStyle(strokePaint, svgTextInfo.strokeState, svgTextInfo.opacity);

    SkPath path;
    SkParsePath::FromSVGString(pathDrawInfo.path.c_str(), &path);
    SkPathMeasure pathMeasure(path, false);
    SkScalar length = pathMeasure.getLength();

    for (int i = 0; i < (int)data.size(); i++) {
        wchar_t temp = data[i];
        if (temp >= 0x4e00 && temp <= 0x9fa5) {
            font.setTypeface(fontTypeChinese_);
        } else {
            font.setTypeface(fontTypeNormal_);
        }
        auto width = font.measureText(&temp, sizeof(wchar_t), SkTextEncoding::kUTF16);
        if (length < offset + width + space) {
            LOGD("path length is not enough, length:%{public}lf, next offset:%{public}lf",
                length, offset + width + space);
            break;
        }
        if (offset < 0) {
            offset += (width + space);
            continue;
        }

        SkPoint position;
        SkVector tangent;
        if (!pathMeasure.getPosTan(offset + width / 2.0, &position, &tangent)) {
            break;
        }
        if (!pathMeasure.getPosTan(offset, &position, nullptr)) {
            break;
        }
        SkRSXform rsxForm = SkRSXform::Make(tangent.fX, tangent.fY, position.fX, position.fY);
        auto blob = SkTextBlob::MakeFromRSXform(&temp, sizeof(wchar_t), &rsxForm, font, SkTextEncoding::kUTF16);

        canvas->save();
        canvas->rotate(pathDrawInfo.rotate, position.fX, position.fY);
        canvas->drawTextBlob(blob.get(), 0.0, 0.0, paint);
        if (svgTextInfo.strokeState.HasStroke() && !NearZero(svgTextInfo.strokeState.GetLineWidth().Value())) {
            canvas->drawTextBlob(blob.get(), 0.0, 0.0, strokePaint);
        }
        canvas->restore();
        offset = offset + width + space;
    }

    return offset;
}

Offset FlutterSvgPainter::MeasureTextBounds(
    const SvgTextInfo& svgTextInfo, const TextDrawInfo& textDrawInfo, Rect& bounds)
{
    Offset offset = textDrawInfo.offset;
    SkFont font;

    font.setSize(svgTextInfo.textStyle.GetFontSize().Value());
    font.setScaleX(1.0);
    double space = 0.0;
    SkScalar x = SkDoubleToScalar(offset.GetX());
    SkScalar y = SkDoubleToScalar(offset.GetY());
    std::wstring data = StringUtils::ToWstring(svgTextInfo.data);

    for (int i = 0; i < (int)data.size(); i++) {
        wchar_t temp = data[i];
        if (temp >= 0x4e00 && temp <= 0x9fa5) {
            // range of chinese
            font.setTypeface(fontTypeChinese_);
        } else {
            font.setTypeface(fontTypeNormal_);
        }
        auto width = font.measureText(&temp, sizeof(temp), SkTextEncoding::kUTF16);
        x = x + width + space;
    }
    bounds.SetWidth(fmax(x, bounds.Width()));
    bounds.SetHeight(fmax(y, bounds.Height()));

    return Offset(x, y);
}

double FlutterSvgPainter::MeasureTextPathBounds(
    const SvgTextInfo& svgTextInfo, const PathDrawInfo& pathDrawInfo, Rect& bounds)
{
    double offset = pathDrawInfo.offset;

    SkFont font;
    font.setSize(svgTextInfo.textStyle.GetFontSize().Value());
    font.setScaleX(1.0);
    double space = 0.0;
    std::wstring data = StringUtils::ToWstring(svgTextInfo.data);

    SkPath path;
    SkParsePath::FromSVGString(pathDrawInfo.path.c_str(), &path);
    SkPathMeasure pathMeasure(path, false);
    SkScalar length = pathMeasure.getLength();

    for (int i = 0; i < (int)data.size(); i++) {
        wchar_t temp = data[i];
        if (temp >= 0x4e00 && temp <= 0x9fa5) {
            font.setTypeface(fontTypeChinese_);
        } else {
            font.setTypeface(fontTypeNormal_);
        }
        auto width = font.measureText(&temp, sizeof(temp), SkTextEncoding::kUTF16);
        if (length < offset + width + space) {
            LOGD("path length is not enough, length:%{public}lf, next offset:%{public}lf",
                length, offset + width + space);
            break;
        }
        offset = offset + width + space;
    }

    auto& pathBounds = path.getBounds();
    bounds.SetWidth(fmax(pathBounds.right(), bounds.Width()));
    bounds.SetHeight(fmax(pathBounds.bottom(), bounds.Height()));
    return offset;
}

static const char* SkipSpace(const char str[])
{
    if (!str) {
        return nullptr;
    }
    while (isspace(*str)) {
        str++;
    }
    return str;
}

static const char* SkipSep(const char str[])
{
    if (!str) {
        return nullptr;
    }
    while (isspace(*str) || *str == ',') {
        str++;
    }
    return str;
}

static const char* FindDoubleValue(const char str[], double& value)
{
    str = SkipSpace(str);
    if (!str) {
        return nullptr;
    }
    char* stop = nullptr;
    float v = std::strtod(str, &stop);
    if (str == stop || errno == ERANGE) {
        return nullptr;
    }
    value = v;
    return stop;
}

void FlutterSvgPainter::StringToPoints(const char str[], std::vector<SkPoint>& points)
{
    for (;;) {
        double x = 0.0;
        str = FindDoubleValue(str, x);
        if (str == nullptr) {
            break;
        }
        str = SkipSep(str);
        double y = 0.0;
        str = FindDoubleValue(str, y);
        if (str == nullptr) {
            break;
        }
        points.emplace_back(SkPoint::Make(x, y));
    }
}

Matrix4 FlutterSvgPainter::CreateMotionMatrix(const std::string& path, const std::string& rotate,
    double percent, bool& isSuccess)
{
    if (path.empty()) {
        isSuccess = false;
        return Matrix4::CreateIdentity();
    }
    SkPath motion;
    SkParsePath::FromSVGString(path.c_str(), &motion);
    SkPathMeasure pathMeasure(motion, false);
    SkPoint position;
    SkVector tangent;
    bool ret = pathMeasure.getPosTan(pathMeasure.getLength() * percent, &position, &tangent);
    if (!ret) {
        isSuccess = false;
        return Matrix4::CreateIdentity();
    }
    float degrees = 0.0f;
    if (rotate == ROTATE_TYPE_AUTO) {
        degrees = SkRadiansToDegrees(std::atan2(tangent.y(), tangent.x()));
    } else if (rotate == ROTATE_TYPE_REVERSE) {
        degrees = SkRadiansToDegrees(std::atan2(tangent.y(), tangent.x())) + FLAT_ANGLE;
    } else {
        degrees = StringUtils::StringToDouble(rotate);
    }
    auto transform = Matrix4::CreateTranslate(position.x(), position.y(), 0.0);
    Matrix4 rotateMatrix;
    rotateMatrix.SetEntry(ROW_INDEX, COL_INDEX, ROTATE_VALUE);
    rotateMatrix.Rotate(degrees, 0.0f, 0.0f, 1.0f);
    transform = rotateMatrix * transform;
    isSuccess = true;
    return FlutterRenderTransform::GetTransformByOffset(transform, Offset(position.x(), position.y()));
}

SkMatrix FlutterSvgPainter::ToSkMatrix(const Matrix4& matrix4)
{
    // Mappings from SkMatrix-index to input-index.
    static const int32_t K_SK_MATRIX_INDEX_TO_MATRIX4_INDEX[] = {
        0,
        4,
        12,
        1,
        5,
        13,
        3,
        7,
        15,
    };

    SkMatrix skMatrix;
    for (std::size_t i = 0; i < ArraySize(K_SK_MATRIX_INDEX_TO_MATRIX4_INDEX); ++i) {
        int32_t matrixIndex = K_SK_MATRIX_INDEX_TO_MATRIX4_INDEX[i];
        if (matrixIndex < matrix4.Count())
            skMatrix[i] = matrix4[matrixIndex];
        else
            skMatrix[i] = 0.0;
    }
    return skMatrix;
}

} // namespace OHOS::Ace