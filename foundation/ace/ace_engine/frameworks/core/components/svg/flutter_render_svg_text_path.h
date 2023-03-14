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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_FLUTTER_RENDER_SVG_TEXT_PATH_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_FLUTTER_RENDER_SVG_TEXT_PATH_H

#include "frameworks/core/components/svg/flutter_render_svg_tspan.h"
#include "frameworks/core/components/svg/render_svg_text_path.h"
#include "frameworks/core/pipeline/layers/transform_layer.h"

namespace OHOS::Ace {

class FlutterRenderSvgTextPath : public RenderSvgTextPath {
    DECLARE_ACE_TYPE(FlutterRenderSvgTextPath, RenderSvgTextPath);

public:
    static RefPtr<RenderNode> Create();
    RenderLayer GetRenderLayer() override;
    void Paint(RenderContext& context, const Offset& offset) override;
    Offset PaintTextPath(RenderContext& context, const Offset& offset);
    Offset MeasureTextPathBounds(const Offset& offset, Rect& bounds);

    Rect GetPaintBounds(const Offset& offset) override;

    bool IsRepaintBoundary() const override
    {
        return true;
    }
    void DrawTextPath(RenderContext& context, PathOffset& pathOffset);

protected:
    void OnNotifyRender() override;

private:
    double OnDrawTextPath(RenderContext& context, const PathOffset& pathOffset);
    double OnMeasureTextPathBounds(const PathOffset& pathOffset, Rect& bounds);

private:
    RefPtr<Flutter::TransformLayer> transformLayer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_FLUTTER_RENDER_SVG_TEXT_PATH_H
