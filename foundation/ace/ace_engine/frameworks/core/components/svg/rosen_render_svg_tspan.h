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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_TSPAN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_TSPAN_H

#include "frameworks/core/components/svg/rosen_render_svg_text.h"

namespace OHOS::Ace {

class RosenRenderSvgTspan : public RosenRenderSvgText {
    DECLARE_ACE_TYPE(RosenRenderSvgTspan, RenderSvgText);

public:
    void Paint(RenderContext& context, const Offset& offset) override;

    void DrawTextPath(RenderContext& context, PathOffset& pathOffset);

    void MeasureTextPathBounds(PathOffset& pathOffset, Rect& bounds);

protected:
    void OnNotifyRender() override;

private:
    void UpdateDrawOffset(DrawOffset& drawOffset) override;
    void UpdatePathOffset(PathOffset& pathOffset);
    double OnDrawTextPath(RenderContext& context, const PathOffset& pathOffset);
    double OnMeasureTextPathBounds(const PathOffset& pathOffset, Rect& bounds);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_TSPAN_H
