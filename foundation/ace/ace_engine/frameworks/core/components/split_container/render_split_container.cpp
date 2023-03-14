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

#include "core/components/split_container/render_split_container.h"

#include "core/components/flex/render_flex.h"
#include "core/components/split_container/split_container_component.h"
#include "core/pipeline/base/position_layout_utils.h"

namespace OHOS::Ace {

void RenderSplitContainer::Update(const RefPtr<Component>& component)
{
    ResetComponentMember();
    UpdateComponentAttr(component);
    InitializeRecognizer();
    MarkNeedLayout();
}

void RenderSplitContainer::ResetComponentMember()
{
    dragDetector_.Reset();
    resizeable_ = false;
    layoutWidth_ = 0.0;
    layoutHeight_ = 0.0;
}

void RenderSplitContainer::UpdateComponentAttr(const RefPtr<Component>& component)
{
    auto splitComponent = AceType::DynamicCast<SplitContainerComponent>(component);
    resizeable_ = splitComponent->GetResizeable();
}

void RenderSplitContainer::PerformLayout()
{
    layoutHeight_ = 0.0;
    layoutWidth_ = 0.0;
    LayoutChildren();
    double maxHeight = GetLayoutParam().GetMaxSize().Height();
    double maxWidth = GetLayoutParam().GetMaxSize().Width();
    layoutHeight_ = LessNotEqual(layoutHeight_, maxHeight) ? layoutHeight_ : maxHeight;
    layoutWidth_ = LessNotEqual(layoutWidth_, maxWidth) ? layoutWidth_ : maxWidth;
    SetLayoutSize(Size { layoutWidth_, layoutHeight_ });
}

void RenderSplitContainer::InitializeRecognizer()
{
    if (!resizeable_) {
        return;
    }
    if (!dragDetector_) {
        dragDetector_ = AceType::MakeRefPtr<VerticalDragRecognizer>();
        dragDetector_->SetOnDragStart([weak = WeakClaim(this)](const DragStartInfo& startInfo) {
            auto splitContainer = weak.Upgrade();
            if (splitContainer) {
                splitContainer->HandleDragStart(startInfo.GetLocalLocation());
            }
        });
        dragDetector_->SetOnDragUpdate([weakDrag = AceType::WeakClaim(this)](const DragUpdateInfo& info) {
            auto splitContainer = weakDrag.Upgrade();
            if (splitContainer) {
                splitContainer->HandleDragUpdate(info.GetLocalLocation());
            }
        });
        dragDetector_->SetOnDragEnd([weakDrag = AceType::WeakClaim(this)](const DragEndInfo& info) {
            auto splitContainer = weakDrag.Upgrade();
            if (splitContainer) {
                splitContainer->HandleDragEnd(info.GetLocalLocation(), info.GetMainVelocity());
            }
        });
    }
}

void RenderSplitContainer::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (dragDetector_) {
        dragDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragDetector_);
    }
}

} // namespace OHOS::Ace