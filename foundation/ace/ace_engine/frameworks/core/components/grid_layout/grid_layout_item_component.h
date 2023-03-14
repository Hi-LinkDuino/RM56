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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_GRID_LAYOUT_ITEM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_GRID_LAYOUT_ITEM_COMPONENT_H

#include "base/utils/macros.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

using OnSelectFunc = std::function<void(bool)>;

class ACE_EXPORT GridLayoutItemComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(GridLayoutItemComponent, SoleChildComponent);

public:
    GridLayoutItemComponent() = default;
    explicit GridLayoutItemComponent(const RefPtr<Component>& child)
        : SoleChildComponent(child) {}
    ~GridLayoutItemComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;

    RefPtr<Element> CreateElement() override;

    void SetColumnIndex(int32_t columnIndex);
    void SetRowIndex(int32_t rowIndex);
    void SetColumnSpan(int32_t columnSpan);
    void SetRowSpan(int32_t rowSpan);
    void SetForceRebuild(bool forceRebuild)
    {
        forceRebuild_ = forceRebuild;
    }

    void SetClickedEventId(const EventMarker& eventId)
    {
        clickEventId_ = eventId;
    }

    const EventMarker& GetClickedEventId() const
    {
        return clickEventId_;
    }

    int32_t GetColumnIndex() const
    {
        return columnIndex_;
    }

    int32_t GetRowIndex() const
    {
        return rowIndex_;
    }

    int32_t GetColumnSpan() const
    {
        return columnSpan_;
    }

    int32_t GetRowSpan() const
    {
        return rowSpan_;
    }

    bool ForceRebuild() const
    {
        return forceRebuild_;
    }

    bool GetSelectable() const
    {
        return selectable_;
    }

    void SetSelectable(bool selectable)
    {
        selectable_ = selectable;
    }

    OnSelectFunc GetOnSelectId() const
    {
        return onSelectId_;
    }

    void SetOnSelectId(const OnSelectFunc& onSelectId)
    {
        onSelectId_ = onSelectId;
    }

private:
    EventMarker clickEventId_;
    int32_t columnIndex_ = -1;
    int32_t rowIndex_ = -1;
    int32_t columnSpan_ = 1;
    int32_t rowSpan_ = 1;
    bool forceRebuild_ = false;
    OnSelectFunc onSelectId_;
    bool selectable_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_GRID_LAYOUT_ITEM_COMPONENT_H
