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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_RENDER_LIST_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_RENDER_LIST_ITEM_H

#include "base/geometry/dimension.h"
#include "core/components_v2/list/list_item_component.h"
#include "core/gestures/long_press_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace::V2 {

class RenderListItem : public RenderNode {
    DECLARE_ACE_TYPE(V2::RenderListItem, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    RenderListItem() = default;
    ~RenderListItem() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void UpdateTouchRect() override;

    void OnChildAdded(const RefPtr<RenderNode>& child) override;
    void OnChildRemoved(const RefPtr<RenderNode>& child) override;

    bool IsDeletable() const
    {
        return component_ ? (component_->GetEditMode() & EditMode::DELETABLE) != 0 : false;
    }

    bool IsMovable() const
    {
        return component_ ? (component_->GetEditMode() & EditMode::MOVABLE) != 0 : false;
    }

    StickyMode GetSticky() const
    {
        return component_ ? component_->GetSticky() : StickyMode::NONE;
    }

    int32_t GetZIndex() const
    {
        return component_ ? component_->GetZIndex() : 0;
    }

    void SetEditMode(bool editMode);

    ACE_DEFINE_COMPONENT_EVENT(OnDeleteClick, void(RefPtr<RenderListItem>));
    ACE_DEFINE_COMPONENT_EVENT(OnSelect, void(RefPtr<RenderListItem>));

    RefPtr<Component> GetComponent() override
    {
        return component_;
    }

    int32_t GetEditMode() const
    {
        return component_->GetEditMode();
    }

    bool GetSelectable() const
    {
        return selectable_;
    }

    OnSelectFunc GetOnSelectId() const
    {
        return onSelectId_;
    }

    void MarkIsSelected(bool isSelected)
    {
        isSelected_ = isSelected;
    }

    bool IsSelected() const
    {
        return isSelected_;
    }

    Dimension GetBorderRadius() const
    {
        return borderRadius_;
    }

private:
    void CreateDeleteButton();

    RefPtr<ListItemComponent> component_;

    RefPtr<RenderNode> child_;
    RefPtr<RenderNode> button_;
    bool editMode_ = false;

    RefPtr<LongPressRecognizer> longPressRecognizer_;
    OnSelectFunc onSelectId_;
    bool selectable_ = true;
    bool isSelected_ = false;
    Dimension borderRadius_;

    ACE_DISALLOW_COPY_AND_MOVE(RenderListItem);
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_RENDER_LIST_ITEM_H
