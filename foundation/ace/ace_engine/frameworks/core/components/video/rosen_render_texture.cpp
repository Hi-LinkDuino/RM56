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

#include "core/components/video/rosen_render_texture.h"

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

namespace {

constexpr int32_t GAUSSIAN_DURATION = 1000;
constexpr Color GAUSSIAN_COLOR = Color(0xffc7bcb9);

} // namespace

void RosenRenderTexture::Update(const RefPtr<Component>& component)
{
    RenderTexture::Update(component);

    if (imageFit_ != ImageFit::FILL && GetRSNode()) {
        GetRSNode()->SetBackgroundColor(Color::BLACK.GetValue());
    }
}

void RosenRenderTexture::Paint(RenderContext& context, const Offset& offset)
{
    SyncGeometryProperties();
#ifdef OHOS_PLATFORM
    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    auto rsNode = std::static_pointer_cast<OHOS::Rosen::RSSurfaceNode>(renderContext->GetRSNode());

    if (IsAddGaussianFuzzy()) {
        AddGaussianFuzzy(context, offset);
    }
#endif
    RenderNode::Paint(context, offset);
}

void RosenRenderTexture::SyncGeometryProperties()
{
    if (!IsTailRenderNode()) {
        return;
    }
    auto rsNode = GetRSNode();
    if (!rsNode) {
        return;
    }
    Offset paintOffset = GetPaintOffset();
    rsNode->SetBounds(paintOffset.GetX() + alignmentX_, paintOffset.GetY() + alignmentY_,
        drawSize_.Width(), drawSize_.Height() - controlsHeight_);
}

void RosenRenderTexture::InitGaussianFuzzyParas()
{
    gaussianFuzzySize_ = GetPaintRect();
    colorValue_ = GAUSSIAN_COLOR;
}

void RosenRenderTexture::SetIsAddGaussianFuzzy(bool isAddGaussianFuzzy)
{
    RenderTexture::SetIsAddGaussianFuzzy(isAddGaussianFuzzy);
    if (isAddGaussianFuzzy) {
        if (!controller_) {
            controller_ = AceType::MakeRefPtr<Animator>(GetContext());
        } else if (controller_->IsRunning()) {
            controller_->Finish();
        }
        controller_->ClearInterpolators();
        controller_->ClearAllListeners();

        controller_->AddStartListener([weak = AceType::WeakClaim(this)]() {
            auto texture = weak.Upgrade();
            if (texture) {
                texture->InitGaussianFuzzyParas();
            }
        });

        controller_->AddStopListener([weak = AceType::WeakClaim(this)]() {
            auto texture = weak.Upgrade();
            if (texture) {
                texture->SetIsAddGaussianFuzzy(false);
            }
        });

        moveAnimation_ = AceType::MakeRefPtr<CurveAnimation<uint8_t>>(0, 1, Curves::LINEAR);
        moveAnimation_->AddListener(Animation<uint8_t>::ValueCallback([weak = AceType::WeakClaim(this)](uint8_t value) {
            auto texture = weak.Upgrade();
            if (texture) {
                texture->MarkNeedRender();
            }
        }));

        controller_->SetDuration(GAUSSIAN_DURATION);
        controller_->AddInterpolator(moveAnimation_);
        controller_->Play();
    } else {
        if (controller_ && controller_->IsRunning()) {
            controller_->Stop();
        }
    }
}

void RosenRenderTexture::AddGaussianFuzzy(RenderContext& context, const Offset& offset)
{
    const auto rosenContext = static_cast<RosenRenderContext*>(&context);
    auto rsNode = rosenContext->GetRSNode();
    if (rsNode == nullptr) {
        LOGE("Paint canvas is null.");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
}

void RosenRenderTexture::UpdateOpacity(uint8_t opacity)
{
    RenderNode::UpdateOpacity(opacity);
}

void RosenRenderTexture::DumpTree(int32_t depth)
{
    auto children = GetChildren();

    if (DumpLog::GetInstance().GetDumpFile() > 0) {
        DumpLog::GetInstance().AddDesc("textureId:", textureId_);
        DumpLog::GetInstance().AddDesc("drawSize:", "width = ", drawSize_.Width(), " height = ", drawSize_.Height());
        DumpLog::GetInstance().AddDesc("sourceSize:", "width = ", drawSize_.Width(), " height = ", drawSize_.Height());
        DumpLog::GetInstance().AddDesc("alignmentX:", alignmentX_);
        DumpLog::GetInstance().AddDesc("alignmentY:", alignmentY_);
        DumpLog::GetInstance().Print(depth, AceType::TypeName(this), children.size());
    }

    for (const auto& item : children) {
        item->DumpTree(depth + 1);
    }
}

std::shared_ptr<RSNode> RosenRenderTexture::CreateRSNode() const
{
    struct Rosen::RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "RosenRenderTexture"};
    return OHOS::Rosen::RSSurfaceNode::Create(surfaceNodeConfig, false);
}

OHOS::sptr<OHOS::Surface> RosenRenderTexture::GetSurface()
{
    auto surfaceNode = OHOS::Rosen::RSBaseNode::ReinterpretCast<OHOS::Rosen::RSSurfaceNode>(GetRSNode());
    if (surfaceNode) {
        auto surface = surfaceNode->GetSurface();
        return surface;
    }
    return nullptr;
}

} // namespace OHOS::Ace
