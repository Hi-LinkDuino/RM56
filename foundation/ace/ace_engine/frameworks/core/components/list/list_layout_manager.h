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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_LAYOUT_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_LAYOUT_MANAGER_H

#include <functional>
#include <vector>

#include "core/animation/bilateral_spring_adapter.h"
#include "core/animation/simple_spring_chain.h"
#include "core/components/common/layout/constants.h"
#include "core/components/list/layout_manager.h"
#include "core/components/list/render_list.h"
#include "core/components/scroll/scrollable.h"

namespace OHOS::Ace {

class ListLayoutManager : public LayoutManager {
    DECLARE_ACE_TYPE(ListLayoutManager, LayoutManager);

public:
    explicit ListLayoutManager(RenderList& renderList);

    ~ListLayoutManager() override = default;

    void Update() override;

    void PerformLayout() override;

    void RefreshLayout() override;

    void MoveItemToViewPort(double position) override;

    void MoveItemGroupToViewPort(double position, double size) override;

    int32_t focusMove(KeyDirection direction) override;

    void CalculateFocusIndexPosition() override;

    void LayoutToItem(int32_t toIndex) override;

    void LayoutToPosition(double position) override;

    void LayoutMore(double incDistance) override;

    void NotifyNeedRefresh() override;

    virtual double AdjustLayoutSize(double size)
    {
        return size;
    }

    double FlushChainAnimation() override;

    double GetItemAnimationValue(int32_t index) const override;

protected:
    virtual void SetChildPosition(const RefPtr<RenderNode>& child, int32_t index, double mainSize);
    RenderList& renderList_;

    LayoutParam MakeInnerLayoutParam(FlexAlign selfAlign) const;
    LayoutParam AdjustLayoutParam(const RefPtr<RenderNode> child, LayoutParam param);
    void RequestMoreItemsIfNeeded(int32_t viewBegin, int32_t viewEnd) override;
    void UpdateItemGroupAttr(RefPtr<RenderNode>& itemChild);
    void ShowItemFocusAnimation();
    void AnimationForItemUpdate();

private:
    void CalculateCachedRange(int32_t viewBegin, int32_t viewEnd, int32_t cachedCount, int32_t& cachedBegin,
        int32_t& cachedEnd) override;
    bool CalculateStartPosition(double& start);
    bool CheckNeedAnimation();
    void InitChainAnimation(int32_t nodeCount);
    double GetChainDelta(int32_t index) const;

    int32_t cachedCount_ = 1;
    int32_t maxCount_ = 0;
    double mainOffset_ = -1.0;
    double controlValue_ = 0.0;
    bool chainOverScroll_ = false;
    double chainOffset_ = 0.0;
    RefPtr<BilateralSpringAdapter> chainAdapter_;
    RefPtr<SimpleSpringChain> chain_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_LAYOUT_MANAGER_H
