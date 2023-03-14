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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_RENDER_COLLAPSING_NAVIGATION_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_RENDER_COLLAPSING_NAVIGATION_BAR_H

#include "core/animation/animation.h"
#include "core/animation/animator.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/related_node.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

using ValueChangedCallback = std::function<void(double)>;
class RenderCollapsingNavigationBar : public RenderNode, public RelatedChild {
    DECLARE_ACE_TYPE(RenderCollapsingNavigationBar, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    double GetPositionY() const
    {
        return positionY_.value;
    }
    void OnRelatedStart();
    void OnRelatedPreScroll(const Offset& delta, Offset& consumed);
    void OnRelatedScroll(const Offset& delta, Offset& consumed);
    void OnRelatedEnd();

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

private:
    bool IsNotSiblingAddRecognizerToResult() override
    {
        return false;
    }
    void Initialize(const WeakPtr<PipelineContext>& context);
    void AttachToContainer();
    void ScrollBy(double dy, double maxPosition);
    void HandleDragStart(const DragStartInfo& info);
    void HandleDragUpdate(const DragUpdateInfo& info);
    void HandleDragEnd();
    void PrepareTitleSizeTranslate(double expand, double collapse);
    void PrepareSubtitleSizeTranslate(double expand, double collapse);
    void PreparePositionTranslate(double expand, double collapse);

    bool NeedHidden(double dy)
    {
        return !NearZero(dy) && LessNotEqual(dy, 0.0) && LessNotEqual(-positionY_.value, scrollableHeight_);
    }

    bool NeedShow(double dy)
    {
        bool canExpand = GreatNotEqual(dy, 0.0) && LessNotEqual(positionY_.value, positionY_.bigger);
        bool needRestore = LessNotEqual(dy, 0.0) && GreatNotEqual(positionY_.value, positionY_.expand);
        return !NearZero(dy) && (canExpand || needRestore);
    }

    bool relateEvent_ = false;
    double dipScale_ = 0.0;
    bool lastUpScroll_ = true;
    Dimension minHeight_;
    double scrollableHeight_ = 0.0;
    double lastTitleScale_ = 1.0;
    bool barIsMini_ = false;
    std::function<void(const std::shared_ptr<BaseEventInfo>&)> changeEvent_;

    struct ChangedKeyframe {
        ChangedKeyframe() = default;
        ChangedKeyframe(double collapse, double expand, double bigger)
            : collapse(collapse), expand(expand), bigger(bigger), value(expand), expandDis(expand - collapse),
              biggerDis(bigger - expand)
        {}
        ~ChangedKeyframe() = default;
        double collapse = 0.0;
        double expand = 0.0;
        double bigger = 0.0;
        double value = 0.0;
        double expandDis = 0.0;
        double biggerDis = 0.0;
        void Update(double collapseValue, double expandValue, double biggerValue = 0.0)
        {
            collapse = collapseValue;
            expand = expandValue;
            bigger = biggerValue;
            expandDis = expand - collapse;
            biggerDis = bigger - expand;
        }
    };
    ChangedKeyframe titleSize_;
    ChangedKeyframe subtitleOpacity_;
    ChangedKeyframe positionY_;
    ChangedKeyframe titlePositionY_;
    RefPtr<DragRecognizer> dragRecognizer_;
    RefPtr<Animator> controller_;
    RefPtr<Animation<double>> positionTranslate_;
    RefPtr<Animation<double>> titleSizeTranslate_;
    RefPtr<Animation<double>> subtitleSizeTranslate_;
    ValueChangedCallback titleChangedCallback_;
    ValueChangedCallback subtitleChangedCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_RENDER_COLLAPSING_NAVIGATION_BAR_H
