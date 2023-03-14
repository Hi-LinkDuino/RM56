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

#include "frameworks/core/components/svg/rosen_render_svg.h"

#include "render_service_client/core/ui/rs_node.h"

#include "core/pipeline/base/rosen_render_context.h"
#include "frameworks/core/components/transform/rosen_render_transform.h"

namespace OHOS::Ace {

void RosenRenderSvg::Paint(RenderContext& context, const Offset& offset)
{
    auto rsNode = static_cast<RosenRenderContext*>(&context)->GetRSNode();
    if (rsNode == nullptr) {
        return;
    }
    rsNode->SetAlpha(rootOpacity_ / 255.0);
    UpdateTransformByGlobalOffset(rsNode);
    RenderNode::Paint(context, offset);
}

void RosenRenderSvg::UpdateTransformByGlobalOffset(const std::shared_ptr<RSNode>& rsNode)
{
    if (rsNode == nullptr) {
        return;
    }
    if (GreatNotEqual(viewBox_.Width(), 0.0) && GreatNotEqual(viewBox_.Height(), 0.0)) {
        double scale =
            std::min(GetLayoutSize().Width() / viewBox_.Width(), GetLayoutSize().Height() / viewBox_.Height());
        double tx = GetLayoutSize().Width() * 0.5 - (viewBox_.Width() * 0.5 + viewBox_.Left()) * scale;
        double ty = GetLayoutSize().Height() * 0.5 - (viewBox_.Height() * 0.5 + viewBox_.Top()) * scale;
        rsNode->SetScale(scale);
        rsNode->SetTranslate({ tx, ty });

        double pivotX = ConvertDimensionToPx(transformOrigin_.first, LengthType::HORIZONTAL, true) / viewBox_.Width();
        double pivotY = ConvertDimensionToPx(transformOrigin_.second, LengthType::VERTICAL, true) / viewBox_.Height();
        rsNode->SetPivot(pivotX, pivotY);
    }
}

} // namespace OHOS::Ace
