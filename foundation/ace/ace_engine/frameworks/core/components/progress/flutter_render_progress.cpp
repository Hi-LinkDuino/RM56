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

#include "core/components/progress/flutter_render_progress.h"

#include "third_party/skia/include/effects/SkGradientShader.h"

#include "core/components/progress/progress_component.h"
#include "core/components/progress/render_progress.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {

void FlutterRenderProgress::Update(const RefPtr<Component>& component)
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
}

void FlutterRenderProgress::PerformLayout()
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

void FlutterRenderProgress::Paint(RenderContext& context, const Offset& offset)
{
    if (GetErrorBit()) {
        LOGE("progress size error. height needs to larger than 2 * thickness");
    }
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("paint canvas is null");
        return;
    }
    canvas->clipRect(offset.GetX(), offset.GetY(), GetLayoutSize().Width() + offset.GetX(),
        GetLayoutSize().Height() + offset.GetY(), SkClipOp::kIntersect, true);
    canvas->save();
    RenderNode::Paint(context, offset);
    canvas->restore();
}

} // namespace OHOS::Ace
