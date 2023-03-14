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

#include "frameworks/core/components/svg/flutter_render_svg_filter.h"

#include "frameworks/core/components/svg/flutter_render_svg_fe_colormatrix.h"
#include "frameworks/core/components/svg/flutter_render_svg_fe_composite.h"
#include "frameworks/core/components/svg/flutter_render_svg_fe_gaussianblur.h"
#include "frameworks/core/components/svg/flutter_render_svg_fe_offset.h"

#include "third_party/skia/include/effects/SkColorFilterImageFilter.h"
#include "third_party/skia/include/effects/SkColorMatrix.h"

namespace OHOS::Ace {

void FlutterRenderSvgFilter::Paint(RenderContext& context, const Offset& offset)
{
    return;
}

const SkPaint FlutterRenderSvgFilter::OnAsPaint()
{
    SkPaint skPaint;
    skPaint.setAntiAlias(true);
    sk_sp<SkImageFilter> imageFilter = nullptr;
    ColorInterpolationType currentColor = ColorInterpolationType::SRGB;
    for (const auto& item : GetChildren()) {
        GetImageFilter(AceType::DynamicCast<RenderSvgFe>(item), imageFilter, currentColor);
    }
    ConverImageFilterColor(imageFilter, currentColor, ColorInterpolationType::SRGB);
    skPaint.setImageFilter(imageFilter);
    return skPaint;
}

void FlutterRenderSvgFilter::GetImageFilter(
    const RefPtr<RenderSvgFe>& fe, sk_sp<SkImageFilter>& imageFilter, ColorInterpolationType& currentColor)
{
    if (!fe) {
        return;
    }
    ColorInterpolationType srcColor = currentColor;
    InitFilterColor(fe, currentColor);

    auto feComposite = AceType::DynamicCast<FlutterRenderSvgFeComposite>(fe);
    if (feComposite) {
        auto foreImageFilter = MakeImageFilter(feComposite->GetInType(), imageFilter);
        auto backImageFilter = MakeImageFilter(feComposite->GetIn2Type(), imageFilter);
        ConverImageFilterColor(foreImageFilter, srcColor, currentColor);
        ConverImageFilterColor(backImageFilter, srcColor, currentColor);
        feComposite->OnAsImageFilter(backImageFilter, foreImageFilter, imageFilter);
        ConverImageFilterColor(imageFilter, srcColor, currentColor);
        return;
    }

    imageFilter = MakeImageFilter(fe->GetInType(), imageFilter);
    auto feOffset = AceType::DynamicCast<FlutterRenderSvgFeOffset>(fe);
    if (feOffset) {
        feOffset->OnAsImageFilter(imageFilter);
        ConverImageFilterColor(imageFilter, srcColor, currentColor);
        return;
    }

    auto feColorMatrix = AceType::DynamicCast<FlutterRenderSvgFeColorMatrix>(fe);
    if (feColorMatrix) {
        feColorMatrix->OnAsImageFilter(imageFilter);
        ConverImageFilterColor(imageFilter, srcColor, currentColor);
        return;
    }

    auto feGaussianBlur = AceType::DynamicCast<FlutterRenderSvgFeGaussianBlur>(fe);
    if (feGaussianBlur) {
        feGaussianBlur->OnAsImageFilter(imageFilter);
        ConverImageFilterColor(imageFilter, srcColor, currentColor);
        return;
    }

    currentColor = srcColor;
}

void FlutterRenderSvgFilter::InitFilterColor(const RefPtr<RenderSvgFe>& fe, ColorInterpolationType& currentColor)
{
    if (!fe) {
        return;
    }

    if (fe->GetInType() == FeInType::SOURCE_GRAPHIC) {
        currentColor = ColorInterpolationType::SRGB;
    } else {
        currentColor = fe->GetColorType();
    }
}

sk_sp<SkImageFilter> FlutterRenderSvgFilter::MakeImageFilter(const FeInType& in, sk_sp<SkImageFilter>& imageFilter)
{
    switch (in) {
        case FeInType::SOURCE_GRAPHIC:
            return nullptr;
        case FeInType::SOURCE_ALPHA:
            SkColorMatrix m;
            m.setScale(0, 0, 0, 1.0f);
#ifdef USE_SYSTEM_SKIA
            return SkColorFilterImageFilter::Make(SkColorFilter::MakeMatrixFilterRowMajor255(m.fMat), nullptr);
#else
            return SkColorFilterImageFilter::Make(SkColorFilters::Matrix(m), nullptr);
#endif
        case FeInType::BACKGROUND_IMAGE:
            break;
        case FeInType::BACKGROUND_ALPHA:
            break;
        case FeInType::FILL_PAINT:
            break;
        case FeInType::STROKE_PAINT:
            break;
        case FeInType::PRIMITIVE:
            break;
        default:
            break;
    }
    return imageFilter;
}

void FlutterRenderSvgFilter::ConverImageFilterColor(
    sk_sp<SkImageFilter>& imageFilter, const ColorInterpolationType& src, const ColorInterpolationType& dst)
{
    if (dst == ColorInterpolationType::LINEAR_RGB && src == ColorInterpolationType::SRGB) {
#ifdef USE_SYSTEM_SKIA
        imageFilter = SkColorFilterImageFilter::Make(SkColorFilter::MakeSRGBToLinearGamma(), imageFilter);
#else
        imageFilter = SkColorFilterImageFilter::Make(SkColorFilters::SRGBToLinearGamma(), imageFilter);
#endif
    } else if (dst == ColorInterpolationType::SRGB && src == ColorInterpolationType::LINEAR_RGB) {
#ifdef USE_SYSTEM_SKIA
        imageFilter = SkColorFilterImageFilter::Make(SkColorFilter::MakeLinearToSRGBGamma(), imageFilter);
#else
        imageFilter = SkColorFilterImageFilter::Make(SkColorFilters::LinearToSRGBGamma(), imageFilter);
#endif
    }
}

} // namespace OHOS::Ace
