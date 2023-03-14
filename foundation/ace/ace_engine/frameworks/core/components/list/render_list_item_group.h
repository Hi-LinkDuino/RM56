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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_RENDER_LIST_ITEM_GROUP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_RENDER_LIST_ITEM_GROUP_H

#include "base/utils/system_properties.h"
#include "core/animation/animator.h"
#include "core/components/box/render_box.h"
#include "core/components/image/image_component.h"
#include "core/components/image/render_image.h"
#include "core/components/list/render_list.h"
#include "core/components/list/render_list_item.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

using GroupEventFunc = std::function<void(const std::string&)>;
using RebuildFunc = std::function<void()>;

class RenderListItemGroup : public RenderListItem {
    DECLARE_ACE_TYPE(RenderListItemGroup, RenderListItem);

public:
    static RefPtr<RenderNode> Create();
    void LayoutExpandableList(double mainSize);
    void SetAnimationStop();
    void ChangeDirection(FlexDirection direction);
    void HandleClicked();
    void UpdateExpandStatusInList();
    void ResetLayout();

    RefPtr<RenderList> GetRenderList();
    void ShowFocusAnimation();
    int32_t GetNextFocusIndex(int32_t lastFocusIndex, bool vertical, bool reverse);
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override {}
    void ItemPrimaryChange(int32_t index);

    void RegisterRebuildCallback(const RebuildFunc& rebuild)
    {
        rebuild_ = rebuild;
    }

    void SetExpand(bool expand)
    {
        if (expand_ != expand) {
            expand_ = expand;
            FireExpandEvent();
            UpdateGroupComponentStatus(expand);
        }
    }

    bool GetExpand() const
    {
        return expand_;
    }

    void SetRightToLeft(bool rightToLeft)
    {
        rightToLeft_ = rightToLeft;
    }

    double GetMainSize(const Size& size) const
    {
        return direction_ == FlexDirection::ROW ? size.Width() : size.Height();
    }

    bool IsExpanded();

protected:
    bool expand_ = false;
    GroupEventFunc onClicked_;
    GroupEventFunc onCollapse_;
    GroupEventFunc onExpand_;
    std::string groupId_;

private:
    template<typename T>
    T MakeValue(double mainValue, double crossValue) const
    {
        return direction_ == FlexDirection::ROW ? T(mainValue, crossValue) : T(crossValue, mainValue);
    }

    double GetCrossSize(const Size& size) const
    {
        return direction_ == FlexDirection::ROW ? size.Height() : size.Width();
    }

    void PerformLayout() override;
    void Update(const RefPtr<Component>& component) override;
    LayoutParam MakeInnerLayoutParam() const;
    void GetPrimaryItem();
    bool NeedRebuild() const;
    std::string MakeEventParam(const std::string& command);

    void AddArrowImage(double mainSize);
    void InitialImage();
    void AnimationPerformLayout(double crossSize, double startPosition, double endPosition);
    RefPtr<KeyframeAnimation<double>> createPositionAnimation(
        double crossSize, double startPosition, double endPosition);
    RefPtr<KeyframeAnimation<int32_t>> CreateOpacityAnimation();
    void SetChildOpacity(int32_t opacity);
    void SetChildStretch(bool isStretch);
    RefPtr<RenderBox> GetRenderBox(const RefPtr<RenderNode>& renderNode);
    RefPtr<KeyframeAnimation<double>> CreateRotateAnimation();
    void RotateArrow(double angle);
    void ResetChildVisibleState();
    void UpdateGroupComponentStatus(bool expand);
    void FireExpandEvent();
    void InitAccessibilityEventListener();

    RebuildFunc rebuild_;
    FlexDirection direction_ = FlexDirection::COLUMN;
    RefPtr<RenderNode> primary_;
    RefPtr<Animator> animator_;
    bool animating_ = false;
    bool rightToLeft_ = false;

    bool initialUpdate_ = true;
    bool isDefaultPrimary_ = false;
    Dimension imageSize_;
    RefPtr<RenderImage> arrowImage_;
    RefPtr<Component> itemGroup_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_RENDER_LIST_ITEM_GROUP_H
