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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LAYOUT_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LAYOUT_MANAGER_H

#include <map>

#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"
#include "base/memory/ace_type.h"
#include "base/utils/system_properties.h"
#include "core/event/touch_event.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

const double DEFAULT_SCALE = 1.0;
const double TV_ITEM_SCALE = 1.05;
const double HALF_ITEM_SIZE = 0.5;
const int32_t LIST_PARAM_INVAID = -2;
const int32_t LIST_LENGTH_INFINITE = -1;
const double INFINITE_POSITION = 0.0;

// first bool mean if vertical, second bool mean if reverse
// false, false --> RIGHT
// false, true --> LEFT
// true, false --> DOWN
// true, true ---> UP
// This map will adapter the Grid FlexDirection with Key Direction.
extern const std::map<bool, std::map<FlexDirection, std::map<bool, std::map<bool, KeyDirection>>>> DIRECTION_MAP;

class LayoutManager : public virtual AceType {
    DECLARE_ACE_TYPE(LayoutManager, AceType);

public:
    LayoutManager() = default;
    ~LayoutManager() override = default;

    virtual void Update() = 0;

    virtual void PerformLayout() = 0;

    virtual void RefreshLayout() = 0;

    virtual void CalculateCachedRange(int32_t viewBegin, int32_t viewEnd, int32_t cachedCount, int32_t& cachedBegin,
        int32_t& cachedEnd) = 0;

    virtual void RequestMoreItemsIfNeeded(int32_t viewBegin, int32_t viewEnd) = 0;

    virtual int32_t focusMove(KeyDirection key) = 0;

    virtual void CalculateFocusIndexPosition() {}

    virtual void MoveItemToViewPort(double position) {}

    virtual void MoveItemGroupToViewPort(double position, double size) {}

    virtual void LayoutToItem(int32_t toIndex) {}

    virtual void LayoutToPosition(double position) {}

    virtual void LayoutMore(double incDistance) {}

    virtual void NotifyNeedRefresh() {}

    int32_t RequestNextFocus(bool vertical, bool reverse)
    {
        KeyDirection key = DIRECTION_MAP.at(rightToLeft_).at(direction_).at(vertical).at(reverse);
        int32_t index = focusMove(key);
        LOGD("Focus move from %{public}d to %{public}d", focusIndex_, index);
        if (index < 0) {
            focusMove_ = focusIndex_;
            return index;
        }
        focusMove_ = index;
        focusIndex_ = index;
        return focusMove_;
    }

    void ListItemFocused(int32_t focusIndex)
    {
        focusIndex_ = focusIndex;
        focusMove_ = focusIndex;
    }

    void MarkNeedRefresh()
    {
        needRefresh_ = true;
        NotifyNeedRefresh();
    }

    void ResetLayoutRange(double head, double tail, Offset position, Size viewPort)
    {
        head_ = head;
        tail_ = tail;
        position_ = position;
        viewPort_ = viewPort;
    }

    const Offset& GetPosition() const
    {
        return position_;
    }

    virtual double FlushChainAnimation()
    {
        return 0.0;
    }

    bool IsRowReverse() const
    {
        return rightToLeft_ && direction_ == FlexDirection::ROW;
    }

    int32_t GetItemGrid(int32_t index) const
    {
        auto iter = itemGrid_.find(index);
        if (iter == itemGrid_.end()) {
            return 0;
        }
        return iter->second;
    }

    bool CheckItemPosition(int32_t index) const
    {
        return itemPosition_.find(index) != itemPosition_.end();
    }

    virtual double GetItemAnimationValue(int32_t index) const
    {
        return 0.0;
    }

    double GetItemPosition(int32_t index) const
    {
        auto iter = itemPosition_.find(index);
        if (iter == itemPosition_.end()) {
            return 0.0;
        }
        return iter->second;
    }

    double GetChainInterval() const
    {
        return enableChain_ ? chainInterval_ : 0.0;
    }

    void GetChainItemRange(int32_t index, double& start, double& end) const
    {
        if (!enableChain_) {
            return;
        }
        start = GetItemPosition(index) - GetChainInterval() * HALF_ITEM_SIZE;
        double nextStartPosition = GetItemPosition(index + 1);
        if (!NearZero(nextStartPosition)) {
            end = nextStartPosition - GetChainInterval() * HALF_ITEM_SIZE;
        }
    }

    int32_t GetIndexByPosition(double position) const
    {
        int32_t index = 0;
        double curPosition = 0.0;
        for (const auto& item : itemPosition_) {
            double correctedPosition =  item.second + GetItemAnimationValue(item.first);
            if (NearEqual(correctedPosition, position)) {
                return item.first;
            } else {
                if (NearEqual(curPosition, correctedPosition)) {
                    continue;
                }
                curPosition = correctedPosition;
                if (correctedPosition < position) {
                    index = item.first;
                } else {
                    return index;
                }
            }
        }
        return index;
    }

    int32_t CalculateRepeatedIndex(int32_t cachedIndex)
    {
        int32_t repeatedIndex;
        if (repeatedLength_ == LIST_LENGTH_INFINITE) {
            repeatedIndex = ((cachedIndex - indexOffset_) > 0 ? (cachedIndex - indexOffset_) : 0);
        } else {
            if (cachedIndex < indexOffset_) {
                repeatedIndex = 0;
            } else {
                if (cachedIndex > indexOffset_ + repeatedLength_) {
                    repeatedIndex = repeatedLength_;
                } else {
                    repeatedIndex = cachedIndex - indexOffset_;
                }
            }
        }
        return repeatedIndex;
    }

    bool GetExpandStatus(int32_t groupIndex) const
    {
        auto iter = itemGroupsExpand_.find(groupIndex);
        if (iter != itemGroupsExpand_.end()) {
            return iter->second;
        } else {
            return expandAll_;
        }
    }

    int32_t GetItemGroupFocusIndex(int32_t groupIndex) const
    {
        auto iter = itemGroupsFocusIndex_.find(groupIndex);
        if (iter != itemGroupsFocusIndex_.end()) {
            return iter->second;
        }
        return 0;
    }

    void SetExpandAll(bool expandAll)
    {
        expandAll_ = expandAll;
    }

    void ClearItemGroupsExpand()
    {
        itemGroupsExpand_.clear();
    }

    void ClearItemPosition()
    {
        itemPosition_.clear();
    }

    void AddItemGroupExpand(int32_t index, bool expand)
    {
        itemGroupsExpand_[index] = expand;
    }

    void AddItemGroupFocusIndex(int32_t index, int32_t groupFocusIndex)
    {
        itemGroupsFocusIndex_[index] = groupFocusIndex;
    }

    double GetFriction() const
    {
        return friction_;
    }

    void SetFriction(double friction)
    {
        friction_ = friction;
    }

    double GetSlipFactor() const
    {
        return slipFactor_;
    }

protected:
    double head_ = 0.0;
    double tail_ = 0.0;
    double lastHead_ = 0.0;
    double lastTail_ = 0.0;
    int32_t focusMove_ = 0; // Record the focus move path.
    int32_t focusIndex_ = 0; // Record the index of focus item.
    Offset position_;
    Size viewPort_;
    bool needRefresh_ = false;
    bool firstLayout_ = true;
    Dimension itemExtent_;

    bool rightToLeft_ = false;
    bool isVertical_ = false;
    FlexDirection direction_ { FlexDirection::COLUMN };
    FlexAlign crossAxisAlign_ { FlexAlign::STRETCH };
    int32_t beginIndex_ = LIST_PARAM_INVAID;
    int32_t endIndex_ = LIST_PARAM_INVAID;
    int32_t length_ = 0;
    int32_t repeatedLength_ = 0;
    int32_t indexOffset_ = 0;

    std::map<int32_t, int32_t> itemGrid_;
    std::map<int32_t, double> itemPosition_;
    std::map<int32_t, bool> itemGroupsExpand_;
    std::map<int32_t, int32_t> itemGroupsFocusIndex_;

    RefPtr<Animator> controller_;
    std::vector<int32_t> needRemoveItems_;
    std::map<int32_t, RefPtr<RenderNode>> newItems_;
    bool isAnimating_ = false;
    double friction_ = 1.0;
    bool expandAll_ = false;
    bool enableChain_ = false; // whether enables spring chain effect.
    double chainInterval_ = 0.0;
    int32_t itemCountOfPage_ = 0;
    double slipFactor_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LAYOUT_MANAGER_H
