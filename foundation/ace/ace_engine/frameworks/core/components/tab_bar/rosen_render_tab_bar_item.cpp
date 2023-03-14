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

#include "core/components/tab_bar/rosen_render_tab_bar_item.h"

#include "render_service_client/core/ui/rs_node.h"

#include "core/components/common/painter/rosen_decoration_painter.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

namespace {

constexpr double CLICKED_RADIUS = 8.0;

} // namespace

void RosenRenderTabBarItem::Paint(RenderContext& context, const Offset& offset)
{
    auto rsNode = static_cast<RosenRenderContext*>(&context)->GetRSNode();
    if (!rsNode) {
        return;
    }
    rsNode->SetClipToFrame(true);
    RenderNode::Paint(context, offset);

    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }

    pipelineContext->AddDirtyLayoutNode(AceType::Claim(this));

    double dipScale = pipelineContext->GetDipScale();

    rsNode->SetForegroundColor(Color::BLACK.BlendOpacity(hoverOpacity_).GetValue());
    rsNode->SetCornerRadius(Dimension(CLICKED_RADIUS, DimensionUnit::VP).ConvertToPx(dipScale));
}

} // namespace OHOS::Ace
