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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_ROSEN_SVG_PAINTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_ROSEN_SVG_PAINTER_H

#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPoint.h"
#include "include/core/SkTypeface.h"

#include "frameworks/base/geometry/matrix4.h"
#include "frameworks/core/components/common/properties/motion_path_evaluator.h"
#include "frameworks/core/components/common/properties/svg_paint_state.h"
#include "frameworks/core/components/svg/render_svg_base.h"
#include "frameworks/core/pipeline/base/render_node.h"

namespace OHOS::Ace {

struct SvgTextInfo {
    FillState fillState;
    StrokeState strokeState;
    SvgTextStyle textStyle;
    std::string data;
    double opacity = 1.0;
};

struct PathDrawInfo {
    std::string path;
    double offset = 0.0;
    double rotate = 0.0;
};

struct TextDrawInfo {
    Offset offset;
    double rotate = 0.0;
};

struct RenderInfo {
    RefPtr<RenderSvgBase> node = nullptr;
    Offset offset;
    uint8_t opacity = UINT8_MAX;
    bool antiAlias = true;
};

class RosenSvgPainter {
public:
    RosenSvgPainter() = default;
    ~RosenSvgPainter() = default;

    static void CheckFontType();
    static double GetPathLength(const std::string& path);
    static Offset GetPathOffset(const std::string& path, double current);
    static bool GetMotionPathPosition(const std::string& path, double percent, MotionPathPosition& result);
    static void SetMask(SkCanvas* canvas);
    static void SetFillStyle(
        SkPaint& skPaint, const FillState& fillState, uint8_t opacity = UINT8_MAX, bool antiAlias = true);
    static void SetFillStyle(SkCanvas* canvas, const SkPath& skPath, const FillState& fillState,
        uint8_t opacity = UINT8_MAX, bool antiAlias = true);
    static void SetFillStyle(
        SkCanvas* canvas, const SkPath& skPath, const FillState& fillState, RenderInfo& renderInfo);
    static void SetStrokeStyle(
        SkPaint& skPaint, const StrokeState& strokeState, uint8_t opacity = UINT8_MAX, bool antiAlias = true);
    static void SetStrokeStyle(SkCanvas* canvas, const SkPath& skPath, const StrokeState& strokeState,
        uint8_t opacity = UINT8_MAX, bool antiAlias = true);
    static void SetStrokeStyle(
        SkCanvas* canvas, const SkPath& skPath, const StrokeState& strokeState, RenderInfo& renderInfo);
    static void UpdateLineDash(SkPaint& paint, const StrokeState& strokeState);
    static Offset UpdateText(SkCanvas* canvas, const SvgTextInfo& svgTextInfo, const TextDrawInfo& textDrawInfo);
    static double UpdateTextPath(SkCanvas* canvas, const SvgTextInfo& svgTextInfo, const PathDrawInfo& pathDrawInfo);
    static Offset MeasureTextBounds(const SvgTextInfo& svgTextInfo, const TextDrawInfo& textDrawInfo, Rect& bounds);
    static double MeasureTextPathBounds(const SvgTextInfo& svgTextInfo, const PathDrawInfo& pathDrawInfo, Rect& bounds);
    static void StringToPoints(const char str[], std::vector<SkPoint>& points);
    static void UpdateMotionMatrix(
        const std::shared_ptr<RSNode>& rsNode, const std::string& path, const std::string& rotate, double percent);
    static SkMatrix ToSkMatrix(const Matrix4& matrix4);

    static void SetGradientStyle(SkPaint& skPaint, const FillState& fillState, double opacity);

    static sk_sp<SkTypeface> fontTypeChinese_;
    static sk_sp<SkTypeface> fontTypeNormal_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_ROSEN_SVG_PAINTER_H
