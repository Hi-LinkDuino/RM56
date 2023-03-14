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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_MODAL_RENDER_DIALOG_MODAL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_MODAL_RENDER_DIALOG_MODAL_H

#include "core/animation/animator.h"
#include "core/components/clip/render_clip.h"
#include "core/components/dialog_modal/dialog_modal_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderDialogModal : public RenderNode {
    DECLARE_ACE_TYPE(RenderDialogModal, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    void UpdateSystemBarHeight(double statusBar, double navigationBar);

    void AnimateTo(double childHeight, bool reverse);

    RefPtr<RenderClip> GetRenderClip() const;

    using LayoutNotify = std::function<void()>;
    void SetLayoutNotify(LayoutNotify&& layoutNotify)
    {
        notify_ = std::move(layoutNotify);
    }

private:
    void PerformClip();
    double PerformClipAnimation(const RefPtr<RenderClip>& clip);
    bool CanRouterPage() const;
    Size GetTopPageLayoutSize() const;
    bool IsAnimating() const
    {
        return controller_->IsRunning() || lastAnimateFrame_;
    }
    void MovePage(double delta);

    RefPtr<Animator> controller_;
    bool lastAnimateFrame_ = false;
    double animatingPageHeight_ = 0.0;
    double animateTargetHeight_ = 0.0;
    bool animatingPush_ = true;
    double statusBarHeight_ = 0.0;
    double navigationBarHeight_ = 0.0;
    LayoutNotify notify_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_MODAL_RENDER_DIALOG_MODAL_H
