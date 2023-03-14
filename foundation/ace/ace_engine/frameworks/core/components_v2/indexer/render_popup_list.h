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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_RENDER_POPUP_LIST_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_RENDER_POPUP_LIST_H

#include "core/components/scroll/scroll_edge_effect.h"
#include "core/components/scroll/scrollable.h"
#include "core/components_v2/indexer/indexer_event_info.h"
#include "core/components_v2/indexer/popup_list_component.h"
#include "core/components_v2/indexer/render_popup_list_item.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace::V2 {
inline constexpr size_t INVALID_POPUP_INDEX = std::numeric_limits<size_t>::max();
inline constexpr int32_t INVALID_POPUP_SELECTED = -1;
inline constexpr size_t POPUP_ITEM_VIEW_MAX_COUNT = 5;                // not support scroll now.
inline constexpr int32_t POPUP_ITEM_VIEW_LAST_POS = POPUP_ITEM_VIEW_MAX_COUNT - 1;
inline constexpr int32_t POPUP_ITEM_VIEW_CONTENT_MAX_COUNT = 7;
inline constexpr int32_t POPUP_ITEM_DATA_MAX_COUNT = 50;               // the part which over 50 abandoned immediate

class RenderPopupList : public RenderNode {
    DECLARE_ACE_TYPE(V2::RenderPopupList, RenderNode);

public:
    using OnPopupSelectedFunc = std::function<void(std::shared_ptr<IndexerEventInfo>&)>;
    static RefPtr<RenderNode> Create();

    RenderPopupList() = default;
    ~RenderPopupList() = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    void OnRequestPopupDataSelected(std::vector<std::string>& data);
    void OnPopupSelected(int32_t selected) const;

protected:
    std::list<RefPtr<RenderPopupListItem>> items_;
    RefPtr<RenderBox> renderBox_;

private:
    void CalTouchPoint(const Point& globalPoint, int32_t& selected);
    double ApplyLayoutParam();
    double LayoutOrRecycleCurrentItems(const LayoutParam& layoutParam);
    RefPtr<RenderPopupListItem> RequestAndLayoutNewItem(size_t index, const LayoutParam& layoutParam);
    void CalculateMainScrollExtent(double curMainPos);
    void SetItemsPosition();
    double GetCurrentPosition() const;
    bool IsOutOfBoundary() const;
    
    // receive callback of srcoll and update position
    void AdjustOffset(Offset& delta, int32_t source);
    bool UpdateScrollPosition(double offset, int32_t source);

    // scroll effect
    void ResetEdgeEffect();
    void SetEdgeEffectAttribute();

    size_t startIndex_ = 0;
    int32_t popupSelected_ = INVALID_POPUP_SELECTED;
    bool reachStart_ = false;
    bool reachEnd_ = false;
    bool isOutOfBoundary_ = false;
    double viewPortHeight_ = 0.0;
    double viewPortWidth_ = 0.0;
    double startMainPos_ = 0.0;
    double endMainPos_ = 0.0;
    double currentOffset_ = 0.0;
    double mainScrollExtent_ = 0.0;
    RefPtr<Scrollable> scrollable_;
    RefPtr<ScrollEdgeEffect> scrollEffect_;
    RefPtr<PopupListComponent> component_;
    RefPtr<BoxComponent> boxComponent_;
    std::vector<std::string> datas_;

    OnPopupSelectedFunc popupSelectedEventFun_;

    ACE_DISALLOW_COPY_AND_MOVE(RenderPopupList);
};
} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_RENDER_POPUP_LIST_H
