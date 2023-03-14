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

#include "frameworks/core/components/svg/rosen_render_svg_polygon.h"

#include "include/utils/SkParsePath.h"
#include "render_service_client/core/ui/rs_node.h"

#include "core/pipeline/base/rosen_render_context.h"
#include "frameworks/core/components/common/painter/rosen_svg_painter.h"
#include "frameworks/core/components/transform/rosen_render_transform.h"

namespace OHOS::Ace {

void RosenRenderSvgPolygon::Paint(RenderContext& context, const Offset& offset)
{
    if (points_.empty()) {
        return;
    }
    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    auto rsNode = renderContext->GetRSNode();
    auto canvas = renderContext->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    SkPath out;
    if (!GetPath(&out)) {
        return;
    }
    if (rsNode && NeedTransform()) {
        auto [transform, pivotX, pivotY] = GetRawTransformInfo();
        rsNode->SetPivot(pivotX, pivotY);
        RosenRenderTransform::SyncTransformToRsNode(rsNode, transform);
    }

    SkAutoCanvasRestore save(canvas, false);
    PaintMaskLayer(context, offset, offset);

    UpdateGradient(fillState_);

    RenderInfo renderInfo = { AceType::Claim(this), offset, opacity_, true };
    RosenSvgPainter::SetFillStyle(canvas, out, fillState_, renderInfo);
    RosenSvgPainter::SetStrokeStyle(canvas, out, strokeState_, renderInfo);
    RenderNode::Paint(context, offset);
}

void RosenRenderSvgPolygon::PaintDirectly(RenderContext& context, const Offset& offset)
{
    if (points_.empty()) {
        return;
    }
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    SkPath out;
    if (!GetPath(&out)) {
        return;
    }
    if (NeedTransform()) {
        canvas->save();
        canvas->concat(RosenSvgPainter::ToSkMatrix(GetTransformMatrix4()));
    }
    UpdateGradient(fillState_);
    RosenSvgPainter::SetFillStyle(canvas, out, fillState_, opacity_);
    RosenSvgPainter::SetStrokeStyle(canvas, out, strokeState_, opacity_);
    if (NeedTransform()) {
        canvas->restore();
    }
}

bool RosenRenderSvgPolygon::CreateSkPath(const std::string& pointsStr, std::vector<SkPoint>& skPoints)
{
    if (pointsStr.empty()) {
        return false;
    }
    RosenSvgPainter::StringToPoints(pointsStr.c_str(), skPoints);
    return !skPoints.empty();
}

bool RosenRenderSvgPolygon::CreateSkPaths(
    const std::string& points1, const std::string& points2, double weight, SkPath* out)
{
    SkPath begin;
    SkPath end;
    if (points1.empty() || points2.empty() || out == nullptr) {
        return false;
    }
    std::vector<SkPoint> skPoints1;
    std::vector<SkPoint> skPoints2;
    if (!CreateSkPath(points1.c_str(), skPoints1) || !CreateSkPath(points2.c_str(), skPoints2)) {
        return false;
    }
    if (skPoints1.size() != skPoints2.size()) {
        return false;
    }
    if (isBy_) {
        auto skPointIter1 = skPoints1.begin();
        auto skPointIter2 = skPoints2.begin();
        while (skPointIter1 != skPoints1.end()) {
            *skPointIter1 = *skPointIter1 + *skPointIter2;
            ++skPointIter1;
            ++skPointIter2;
        }
    }
    begin.addPoly(&skPoints1[0], skPoints1.size(), isClose_);
    end.addPoly(&skPoints2[0], skPoints2.size(), isClose_);
    begin.interpolate(end, weight, out);
    return true;
}

void RosenRenderSvgPolygon::UpdateMotion(const std::string& path, const std::string& rotate, double percent)
{
    auto rsNode = GetRSNode();
    if (!rsNode) {
        LOGE("transformLayer is null");
        return;
    }
    RosenSvgPainter::UpdateMotionMatrix(rsNode, path, rotate, percent);
}

Rect RosenRenderSvgPolygon::GetPaintBounds(const Offset& offset)
{
    SkPath path;
    if (!GetPath(&path)) {
        return GetPaintRect();
    }
    auto& bounds = path.getBounds();
    return Rect(bounds.left(), bounds.top(), bounds.width(), bounds.height());
}

bool RosenRenderSvgPolygon::GetPath(SkPath* out)
{
    if (!out) {
        LOGE("SkPath is null");
        return false;
    }
    if (pointsVector_.empty()) {
        return GetPathWithoutAnimate(out);
    } else {
        int32_t firstPart = (int)weight_;
        int32_t pathsSize = static_cast<int32_t>(pointsVector_.size());
        if (firstPart < 0 || firstPart > (pathsSize - 1)) {
            return false;
        }
        float weight = 1.0f;
        int32_t currValue = 0;
        int32_t nextValue = 0;
        if (firstPart == (pathsSize - 1)) {
            currValue = firstPart;
            nextValue = firstPart - 1;
        } else {
            weight = weight_ - firstPart;
            currValue = firstPart + 1;
            nextValue = firstPart;
        }
        if (!CreateSkPaths(pointsVector_[currValue].c_str(), pointsVector_[nextValue].c_str(), weight, out)) {
            return GetPathWithoutAnimate(out);
        }
    }
    if (fillState_.IsEvenodd()) {
        out->setFillType(SkPath::FillType::kEvenOdd_FillType);
    }
    return true;
}

bool RosenRenderSvgPolygon::GetPathWithoutAnimate(SkPath* out)
{
    if (!out) {
        LOGE("SkPath is null");
        return false;
    }
    std::vector<SkPoint> skPoints;
    if (!CreateSkPath(points_, skPoints)) {
        return false;
    }
    out->addPoly(&skPoints[0], skPoints.size(), isClose_);
    if (fillState_.IsEvenodd()) {
        out->setFillType(SkPath::FillType::kEvenOdd_FillType);
    }
    return true;
}

} // namespace OHOS::Ace
