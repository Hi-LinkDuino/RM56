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

#include "core/components/progress/rosen_render_progress.h"

#include "render_service_client/core/ui/rs_node.h"
#include "third_party/skia/include/effects/SkGradientShader.h"

#include "core/components/progress/progress_component.h"
#include "core/components/progress/render_progress.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

void RosenRenderProgress::Update(const RefPtr<Component>& component)
{
    const RefPtr<ProgressComponent> progress = AceType::DynamicCast<ProgressComponent>(component);

    if (initialUpdate_) {
        track_ = progress->GetTrack()->CreateRenderNode();
        AddChild(track_);
        track_->Attach(GetContext());
        initialUpdate_ = false;
    } else if (type_ != progress->GetType()) {
        RemoveChild(track_);
        track_ = progress->GetTrack()->CreateRenderNode();
        AddChild(track_);
        track_->Attach(GetContext());
    }
    RenderProgress::Update(component);
    track_->Update(progress->GetTrack());
    const RefPtr<RenderTrack> track = AceType::DynamicCast<RenderTrack>(track_);
    track->SetAnimationPlay(progress->GetAnimationPlay());
    track->SetTotalRatio(totalRatio_);
    track->SetCachedRatio(cachedRatio_);
    MarkNeedLayout();

    if (auto rsNode = GetRSNode()) {
        rsNode->SetClipToFrame(true);
    }
}

void RosenRenderProgress::PerformLayout()
{
    RenderProgress::PerformLayout();
    Offset trackPosition;
    const RefPtr<RenderTrack> track = AceType::DynamicCast<RenderTrack>(track_);
    if (!track) {
        LOGE("convert to render track error. skip performlayout");
        return;
    }
    track->SetPosition(trackPosition);
}

} // namespace OHOS::Ace
