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

#include "core/components/track/rosen_render_arc_track.h"

#include "flutter/lib/ui/text/paragraph_builder.h"
#include "flutter/third_party/txt/src/txt/paragraph_txt.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkClipOp.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPath.h"

#include "core/components/font/rosen_font_collection.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {

constexpr double EDGE = 15.0;
constexpr double HEIGHT_OFFSET = 5.0;
constexpr double INDICATOR_STROKE_WIDTH = 5.0;
constexpr int8_t UNSELECT_ALPHA = 125;

void DrawGauge(RenderContext& context, const RenderRingInfo& trackInfo)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    double thickness = trackInfo.thickness;
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(trackInfo.color.GetValue());
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setStrokeWidth(thickness);
    paint.setStrokeCap(SkPaint::kRound_Cap);

    canvas->drawArc({ trackInfo.center.GetX() + (thickness / 2) - trackInfo.radius,
                        trackInfo.center.GetY() + (thickness / 2) - trackInfo.radius,
                        trackInfo.center.GetX() + trackInfo.radius - (thickness / 2),
                        trackInfo.center.GetY() + trackInfo.radius - (thickness / 2) },
        trackInfo.startDegree - 90, trackInfo.sweepDegree, false, paint);
}

bool ShouldHighLight(const double start, const double interval, const double percent)
{
    if (LessOrEqual(percent, start + interval) && GreatOrEqual(percent, start)) {
        return true;
    }
    return false;
}

void SetTextStyle(SkCanvas* canvas, const RenderRingInfo& trackInfo, const std::string& markedText,
    const Color markedColor, const Rect dataRegion)
{
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    auto fontCollection = RosenFontCollection::GetInstance().GetFontCollection();
    if (!fontCollection) {
        LOGW("PaintText: fontCollection is null");
        return;
    }
    double pathStartVertexX = trackInfo.center.GetX();
    double pathStartVertexY = trackInfo.center.GetY() - trackInfo.radius + (trackInfo.thickness / 2);
    txt::ParagraphStyle style;
    txt::TextStyle txtStyle;
    txtStyle.font_size = 80;
    txtStyle.font_weight = txt::FontWeight::w400;
    txtStyle.color = markedColor.GetValue();
    std::unique_ptr<txt::ParagraphBuilder> builder;
    style.max_lines = 1;
    builder = txt::ParagraphBuilder::CreateTxtBuilder(style, fontCollection);
    builder->PushStyle(txtStyle);
    builder->AddText(StringUtils::Str8ToStr16(markedText));
    auto paragraph = builder->Build();
    paragraph->Layout(dataRegion.Width());
    paragraph->Paint(canvas, pathStartVertexX - txtStyle.font_size, pathStartVertexY + EDGE + HEIGHT_OFFSET * 2);
}

void DrawIndicator(RenderContext& context, const RenderRingInfo& trackInfo, const std::string& markedText,
    const Color markedColor, const Rect dataRegion)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    canvas->save();

    SkPath path;
    double pathStartVertexX = trackInfo.center.GetX();
    double pathStartVertexY = trackInfo.center.GetY() - trackInfo.radius + (trackInfo.thickness / 2);
    path.moveTo(pathStartVertexX, pathStartVertexY);
    path.lineTo(pathStartVertexX - EDGE, pathStartVertexY + EDGE);
    path.lineTo(pathStartVertexX - EDGE, pathStartVertexY + EDGE + HEIGHT_OFFSET);
    path.lineTo(pathStartVertexX + EDGE, pathStartVertexY + EDGE + HEIGHT_OFFSET);
    path.lineTo(pathStartVertexX + EDGE, pathStartVertexY + EDGE);
    path.lineTo(pathStartVertexX, pathStartVertexY);

    canvas->rotate(trackInfo.startDegree + trackInfo.sweepDegree, trackInfo.center.GetX(), trackInfo.center.GetY());
    SkPaint paint;
    paint.setStyle(SkPaint::kFill_Style);
    paint.setColor(Color::WHITE.GetValue());
    paint.setStrokeCap(SkPaint::kSquare_Cap);
    paint.setStrokeWidth(INDICATOR_STROKE_WIDTH);
    canvas->drawPath(path, paint);
    SetTextStyle(canvas, trackInfo, markedText, markedColor, dataRegion);

    paint.setStyle(SkPaint::kStroke_Style);
    paint.setColor(Color::BLACK.GetValue());
    canvas->drawPath(path, paint);
    canvas->restore();
}
} // namespace

void RosenRenderArcTrack::Paint(RenderContext& context, const Offset& offset)
{
    RenderRingInfo data = paintData_;
    Rect dataRegion_;
    dataRegion_ =
        Rect(offset.GetX() + 20, offset.GetY() + 2, GetLayoutSize().Width() - 40, GetLayoutSize().Height() - 4);
    // now depend on box to clip
    if (data.center.GetX() < 0.0 || data.center.GetY() < 0.0) {
        data.center = Offset(GetLayoutSize().Width() / 2, GetLayoutSize().Height() / 2);
    }
    if (data.radius < 0.0) {
        data.radius = GetLayoutSize().Width() / 2;
    }
    data.center += offset;
    if (!showIndicator_) {
        data.startDegree = leftToRight_ ? paintData_.startDegree : paintData_.startDegree + paintData_.sweepDegree;
        data.sweepDegree = leftToRight_ ? paintData_.sweepDegree : -1 * paintData_.sweepDegree;
        // draw background
        data.color = GetBackgroundColor();
        DrawGauge(context, data);
        data.color = GetSelectColor();
        data.sweepDegree = data.sweepDegree * GetTotalRatio();
        // draw content
        DrawGauge(context, data);
    } else {
        if (colors_.size() == 0 || colors_.size() != weights_.size()) {
            return;
        }
        double totalWeight = 0.0;
        for (const auto& weight : weights_) {
            totalWeight += weight;
        }
        if (NearEqual(totalWeight, 0.0)) {
            LOGE("total weight is 0.0");
            return;
        }
        double currentStart = 0.0;
        double highLightStart = 0.0;
        size_t highLightIndex = 0;
        double ratio = leftToRight_ ? GetTotalRatio() : 1 - GetTotalRatio();
        for (int32_t index = static_cast<int32_t>(colors_.size()) - 1; index >= 0; --index) {
            data.color = colors_[index];
            data.color.ChangeAlpha(UNSELECT_ALPHA);
            currentStart += weights_[index];
            if (ShouldHighLight(totalWeight - currentStart, weights_[index], ratio * totalWeight)) {
                highLightIndex = static_cast<size_t>(index);
                highLightStart = totalWeight - currentStart;
            }
            data.startDegree = paintData_.startDegree + (1 - currentStart / totalWeight) * paintData_.sweepDegree;
            data.sweepDegree = (weights_[index] / totalWeight) * paintData_.sweepDegree;
            DrawGauge(context, data);
        }
        // draw highlight part
        data.color = colors_[highLightIndex];
        data.startDegree = paintData_.startDegree + (highLightStart / totalWeight) * paintData_.sweepDegree;
        data.sweepDegree = (weights_[highLightIndex] / totalWeight) * paintData_.sweepDegree;
        DrawGauge(context, data);
        data.startDegree = paintData_.startDegree;
        data.sweepDegree = paintData_.sweepDegree * ratio;
        DrawIndicator(context, data, markedText_, markedTextColor_, dataRegion_);
    }
}

} // namespace OHOS::Ace
