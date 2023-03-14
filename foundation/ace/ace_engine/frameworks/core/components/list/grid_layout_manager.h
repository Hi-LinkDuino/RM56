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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_GRID_LAYOUT_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_GRID_LAYOUT_MANAGER_H

#include "core/components/list/layout_manager.h"
#include "core/components/list/render_list.h"
#include "core/components/scroll/scrollable.h"

namespace OHOS::Ace {

typedef std::map<KeyDirection, int32_t> RelationMap;
typedef std::map<int32_t, RelationMap> ChildRelationsMap;

class GridLayoutManager : public LayoutManager {
    DECLARE_ACE_TYPE(GridLayoutManager, LayoutManager);

public:
    explicit GridLayoutManager(RenderList& renderList);
    ~GridLayoutManager() override = default;

    void Update() override;

    void RefreshLayout() override;

    void PerformLayout() override;

    int32_t focusMove(KeyDirection direction) override;

    void MoveItemToViewPort(double position) override;

    void LayoutToItem(int32_t toIndex) override;

    void LayoutMore(double incDistance) override;

private:
    LayoutParam MakeInnerLayoutParam(int32_t columnSpan) const;

    void CalculateAxisSize();

    // Sets child position, the mainAxis does not contain the offset.
    void SetChildPosition(const RefPtr<RenderNode>& child, double mainSize, int32_t gridPos, int32_t columnSpan);

    bool CalculateStartPosition(double& start);

    void ShowItemFocusAnimation();

    void CalculateCachedRange(
        int32_t viewBegin, int32_t viewEnd, int32_t cachedCount, int32_t& cachedBegin, int32_t& cachedEnd) override;

    void RequestMoreItemsIfNeeded(int32_t viewBegin, int32_t viewEnd) override;

    int32_t cachedCount_ = 1;
    int32_t maxCount_ = 0;

    Offset offset_;
    RefPtr<Scrollable> scrollable_;

    double mainSize_ = 0.0;
    double crossSize_ = 0.0;
    double gridLen_ = 0.0;
    double gridWidth_ = -1.0;
    double gridHeight_ = -1.0;
    int32_t columnCount_ = 0;
    int32_t columnExtent_ = 0;
    bool updateFlag_ = false;

    RenderList& renderList_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_GRID_LAYOUT_MANAGER_H
