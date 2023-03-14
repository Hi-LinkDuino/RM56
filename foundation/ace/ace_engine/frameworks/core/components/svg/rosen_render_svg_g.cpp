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

#include "frameworks/core/components/svg/rosen_render_svg_g.h"

#include "render_service_client/core/ui/rs_node.h"

#include "core/pipeline/base/rosen_render_context.h"
#include "frameworks/core/components/common/painter/rosen_svg_painter.h"
#include "frameworks/core/components/svg/render_svg_filter.h"
#include "frameworks/core/components/transform/rosen_render_transform.h"

namespace OHOS::Ace {

void RosenRenderSvgG::Paint(RenderContext& context, const Offset& offset)
{
    auto rsNode = static_cast<RosenRenderContext*>(&context)->GetRSNode();
    if (rsNode == nullptr) {
        return;
    }

    if (NeedTransform()) {
        auto [transform, pivotX, pivotY] = GetRawTransformInfo();
        rsNode->SetPivot(pivotX, pivotY);
        RosenRenderTransform::SyncTransformToRsNode(rsNode, transform);
    }
    RenderNode::Paint(context, offset);
}

void RosenRenderSvgG::PaintDirectly(RenderContext& context, const Offset& offset)
{
    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    auto canvas = renderContext->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    SkCanvas* skCanvas = canvas;
    if (!skCanvas) {
        LOGE("Paint skCanvas is null");
        return;
    }

    if (NeedTransform()) {
        skCanvas->save();
        skCanvas->concat(RosenSvgPainter::ToSkMatrix(GetTransformMatrix4()));
    }

    RenderSvgBase::PaintDirectly(context, offset);

    if (NeedTransform()) {
        skCanvas->restore();
    }
}

} // namespace OHOS::Ace
