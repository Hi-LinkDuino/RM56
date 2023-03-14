/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components/xcomponent/rosen_render_xcomponent.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#ifdef ENABLE_ROSEN_BACKEND
#include "render_service_client/core/ui/rs_surface_node.h"
#endif

#include "base/log/ace_trace.h"
#include "base/log/dump_log.h"
#include "base/log/log.h"
#include "base/utils/system_properties.h"
#include "core/components/display/render_display.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
void RosenRenderXComponent::Update(const RefPtr<Component>& component)
{
    RenderXComponent::Update(component);

    if (GetRSNode()) {
        GetRSNode()->SetBackgroundColor(Color::BLACK.GetValue());
    }
}

void RosenRenderXComponent::Paint(RenderContext& context, const Offset& offset)
{
    SyncGeometryProperties();
#ifdef OHOS_STANDARD_SYSTEM
    if (!hasSetDefaultSize_) {
        if (xcomponentSizeChangeEvent_ && !drawSize_.IsHeightInfinite()) {
            xcomponentSizeChangeEvent_(textureId_, drawSize_.Width(), drawSize_.Height());
        }
        hasSetDefaultSize_ = true;
    }
#endif
    RenderNode::Paint(context, offset);
}

void RosenRenderXComponent::SyncGeometryProperties()
{
    if (!IsTailRenderNode()) {
        return;
    }
    auto rsNode = GetRSNode();
    if (!rsNode) {
        return;
    }
    Offset paintOffset = GetPaintOffset();
    rsNode->SetBounds(paintOffset.GetX(), paintOffset.GetY(),
        drawSize_.Width(), drawSize_.Height());
}

void RosenRenderXComponent::UpdateOpacity(uint8_t opacity)
{
    RenderNode::UpdateOpacity(opacity);
}

void RosenRenderXComponent::DumpTree(int32_t depth)
{
    auto children = GetChildren();

    if (DumpLog::GetInstance().GetDumpFile() > 0) {
        DumpLog::GetInstance().AddDesc("textureId:", textureId_);
        DumpLog::GetInstance().AddDesc("drawSize:", "width = ", drawSize_.Width(), " height = ", drawSize_.Height());
        DumpLog::GetInstance().AddDesc("sourceSize:", "width = ", drawSize_.Width(), " height = ", drawSize_.Height());
        DumpLog::GetInstance().Print(depth, AceType::TypeName(this), children.size());
    }

    for (const auto& item : children) {
        item->DumpTree(depth + 1);
    }
}

std::shared_ptr<RSNode> RosenRenderXComponent::CreateRSNode() const
{
    struct Rosen::RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "RosenRenderXComponent"};
    return OHOS::Rosen::RSSurfaceNode::Create(surfaceNodeConfig, false);
}

#ifdef OHOS_STANDARD_SYSTEM
OHOS::sptr<OHOS::Surface> RosenRenderXComponent::GetSurface()
{
    auto surfaceNode = OHOS::Rosen::RSBaseNode::ReinterpretCast<OHOS::Rosen::RSSurfaceNode>(GetRSNode());
    if (surfaceNode) {
        auto surface = surfaceNode->GetSurface();
        return surface;
    }
    return nullptr;
}
#endif
} // namespace OHOS::Ace
