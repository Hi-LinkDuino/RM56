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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHART_FLUTTER_RENDER_CHART_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHART_FLUTTER_RENDER_CHART_H

#include "flutter/lib/ui/painting/path.h"
#include "flutter/lib/ui/text/font_collection.h"
#include "flutter/lib/ui/text/paragraph_builder.h"

#include "base/geometry/offset.h"
#include "base/geometry/rect.h"
#include "core/components/chart/render_chart.h"
#include "core/pipeline/base/scoped_canvas_state.h"
#include "core/pipeline/layers/clip_layer.h"

namespace OHOS::Ace {

class FlutterRenderChart : public RenderChart {
    DECLARE_ACE_TYPE(FlutterRenderChart, RenderChart);

public:
    FlutterRenderChart() = default;

    ~FlutterRenderChart() override = default;

    RenderLayer GetRenderLayer() override;

    bool IsRepaintBoundary() const override
    {
        return true;
    }

private:
    void Update(const RefPtr<Component>& component) override;

    void Paint(RenderContext& context, const Offset& offset) override;

    void PaintDatas(RenderContext& context, const Rect& paintRect);

    void PaintHorizontalAxis(RenderContext& context, const Rect& paintRect);

    void PaintVerticalAxis(RenderContext& context, const Offset& offset, const Rect& paintRect);

    Offset ConvertDataToPosition(const Rect& paintRegion, const PointInfo& point);

    void SetEdgeStyle(const PointInfo& point, flutter::Paint& paint) const;

    void PaintStylePoints(const ScopedCanvas& canvas, const Rect& paintRegion, const MainChart& chartData);

    void PaintPoint(const ScopedCanvas& canvas, const Offset& offset, flutter::Paint paint,
        flutter::PaintData paintData, const PointInfo& point);

    void PaintText(const ScopedCanvas& canvas, const Rect& paintRegion, const MainChart& chartData);

    void PaintLinearGraph(const ScopedCanvas& context, const Rect& paintRect);

    void PaintLineEdge(const ScopedCanvas& canvas, fml::RefPtr<flutter::CanvasPath> path, const SegmentInfo segmentInfo,
        double thickness, bool drawGradient = false);

    void UpdateLineGradientPoint(const std::vector<LineInfo>& pointInfo, const MainChart& line, const Rect& paintRect);

    void PaintLineGradient(const ScopedCanvas& canvas, fml::RefPtr<flutter::CanvasPath> path, const Rect& paintRect,
        Color fillColor, const PointInfo& peekPoint);

    int32_t PaintLine(uint32_t index, const std::vector<LineInfo>& line, fml::RefPtr<flutter::CanvasPath> path,
        const MainChart& data, const Rect& paintRect);

    void AddCubicPath(fml::RefPtr<flutter::CanvasPath>& path, const Rect& paintRect, const std::vector<LineInfo>& line,
        uint32_t index, bool isEnd);

    sk_sp<SkShader> CreateFillGradientShader(const Rect& paintRect, const Color& fillColor, double top);

    void PaintBar(const ScopedCanvas& canvas, flutter::Paint& paint, const std::vector<LineInfo>& barGroupData,
        const Rect& paintRect, int32_t barGroupNum, int32_t barsAreaNum, int32_t barGroupIndex);

    Rect GetBarsAreaPaintRect(const Rect& paintRect, int32_t barsAreaIndex);

    Rect GetBarAreaPaintRect(const Rect& barsAreaPaintRect, int32_t barGroupIndex, int32_t barGroupNumber,
        double barInterval);

    Offset CalculateControlA(const Offset& prev, const Offset& cur, const Offset& next);

    Offset CalculateControlB(const Offset& prev, const Offset& cur, const Offset& next);

    double tickOffset_ = 0.0;
    double tickHorizontalOffset_ = 0.0;

    bool drawLine_ = false;
    uint32_t startIndex_ = 0;

    RefPtr<Flutter::ClipLayer> layer_;

    Offset startGradientPoint_;
    Offset endGradientPoint_;
    Size lastLayoutSize_;
    size_t startGradientIndex_ = 0;
    size_t endGradientIndex_ = 0;
    bool gradientOfLine_ = false;
    Offset startOffset_;
    double paintWidth_ = 0;
    Color targetColor_;
    bool wholeLineGradient_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHART_FLUTTER_RENDER_CHART_H
