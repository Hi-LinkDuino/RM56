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

#include "core/components/web/rosen_render_web.h"

#include "render_service_client/core/ui/rs_surface_node.h"

#include "base/log/log.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

void RosenRenderWeb::Update(const RefPtr<Component>& component)
{
    RenderWeb::Update(component);

    if (GetRSNode()) {
        GetRSNode()->SetBackgroundColor(Color::WHITE.GetValue());
    }
}

void RosenRenderWeb::PerformLayout()
{
    RenderWeb::PerformLayout();
}

void RosenRenderWeb::DumpTree(int32_t depth) {}

#ifdef OHOS_STANDARD_SYSTEM
void RosenRenderWeb::OnAttachContext()
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("OnAttachContext context null");
        return;
    }
    if (delegate_) {
        auto surface = GetSurface();
        delegate_->InitOHOSWeb(context_, surface);
    }
}

void RosenRenderWeb::Paint(RenderContext& context, const Offset& offset)
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("OnAttachContext context null");
        return;
    }
    if (pipelineContext->GetIsDragStart()) {
        drawSize_ = Size(1.0, 1.0);
    }
    if (drawSize_.Width() == Size::INFINITE_SIZE || drawSize_.Height() == Size::INFINITE_SIZE) {
        LOGE("Web drawSize height or width is invalid");
        return;
    }
    if (delegate_) {
        LOGI("Web paint drawSize width = %{public}f, height = %{public}f", drawSize_.Width(), drawSize_.Height());
        delegate_->Resize(drawSize_.Width(), drawSize_.Height());
        if (!isUrlLoaded_) {
            delegate_->LoadUrl();
            isUrlLoaded_ = true;
        }
    }
    SyncGeometryProperties();
    RenderNode::Paint(context, offset);
}

void RosenRenderWeb::SyncGeometryProperties()
{
    if (!IsTailRenderNode()) {
        return;
    }
    auto rsNode = GetRSNode();
    if (rsNode == nullptr) {
        return;
    }
    Offset paintOffset = GetPaintOffset();
    rsNode->SetBounds(paintOffset.GetX(), paintOffset.GetY(), drawSize_.Width(), drawSize_.Height());
}

std::shared_ptr<RSNode> RosenRenderWeb::CreateRSNode() const
{
    struct OHOS::Rosen::RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "RosenRenderWeb"};
    return OHOS::Rosen::RSSurfaceNode::Create(surfaceNodeConfig, false);
}

OHOS::sptr<OHOS::Surface> RosenRenderWeb::GetSurface()
{
    auto surfaceNode = OHOS::Rosen::RSBaseNode::ReinterpretCast<OHOS::Rosen::RSSurfaceNode>(GetRSNode());
    if (surfaceNode != nullptr) {
        return surfaceNode->GetSurface();
    }
    return nullptr;
}
#endif
} // namespace OHOS::Ace
