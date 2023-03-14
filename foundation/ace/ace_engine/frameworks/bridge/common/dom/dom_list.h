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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_LIST_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_LIST_H

#include "base/utils/macros.h"
#include "core/components/indexer/indexer_list_component.h"
#include "core/components/list/list_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

class ACE_EXPORT DOMList final : public DOMNode {
    DECLARE_ACE_TYPE(DOMList, DOMNode);

public:
    DOMList(NodeId nodeId, const std::string& nodeName);
    ~DOMList() override = default;

    void CallSpecializedMethod(const std::string& method, const std::string& args) override;
    void OnPageLoadFinish() override;
    void OnScrollBy(double dx, double dy, bool isSmooth) override;
    Offset GetCurrentOffset() const;
    void ScrollToMethod(int32_t index);

    RefPtr<Component> GetSpecializedComponent() override
    {
        if (indexer_) {
            return indexerComponent_;
        } else {
            return listComponent_;
        }
    }

    FlexDirection GetDirection() const
    {
        return flexDirection_;
    }

    bool GetItemScale() const
    {
        return itemScale_;
    }

    bool GetItemOpacity() const
    {
        return itemOpacity_;
    }

    bool NeedVibrate() const
    {
        return scrollVibrate_;
    }

    bool NeedRotationVibrate() const
    {
        return rotationVibrate_;
    }

    bool GetItemCenter() const
    {
        return itemCenter_;
    }

    bool NeedDivider() const
    {
        return needDivider_;
    }

    const Dimension& GetDividerHeight() const
    {
        return dividerHeight_;
    }

    const Dimension& GetDividerLength() const
    {
        return dividerLength_;
    }

    const Dimension& GetDividerOrigin() const
    {
        return dividerOrigin_;
    }

    Color GetDividerColor() const
    {
        return dividerColor_;
    }

    void AdjustSpecialParamInLiteMode() override;

    void SetOnRotate(const EventMarker& eventMarker)
    {
        listComponent_->SetOnRotateId(eventMarker);
    }

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void PrepareSpecializedComponent() override;
    void ResetInitializedStyle() override;
    void OnMounted(const RefPtr<DOMNode>& parentNode) override;

    const EventMarker& GetRotateId()
    {
        static EventMarker defaultMarker;
        auto& crownEvent = static_cast<CommonCrownEvent&>(declaration_->GetEvent(EventTag::COMMON_CROWN_EVENT));
        return crownEvent.IsValid() ? crownEvent.rotate.eventMarker : defaultMarker;
    }

private:
    void CreateOrUpdateList();

    void CreateOrUpdateIndexer();

    RefPtr<AccessibilityNode> GetAccessibilityNode();
    void UpdateAccessibilityOrder();
    void UpdateAccessibilityByVisible();

    void SetScrollBar();

    void SetChildActive();

    void ExpandGroup(const std::string& groupId, bool expand = true);
    void ScrollByMethod(double x, double y, bool isSmooth);
    void ScrollArrowMethod(bool reverse, bool isSmooth);
    void ScrollToEdgeMethod(const std::string& method, const std::string& args);
    void ScrollPageMethod(const std::string& method, const std::string& args);

    void ParseIndexer(const std::string& indexerAlphabet);
    bool SupportChainAnimation() const;

    void InitScrollBarWithSpecializedStyle();

    // List columns default value is 1
    int32_t listColumns_ = 1;
    int32_t cachedCount_ = 0;
    bool itemScale_ = true;
    bool itemOpacity_ = false;
    bool itemCenter_ = false;
    bool centerLayout_ = false;
    bool scrollVibrate_ = true;
    bool rotationVibrate_ = false;
    bool scrollPage_ = false;
    bool chainAnimation_ = false;
    SpringChainProperty chainProperty_;
    bool useDefaultOverSpringProperty_ = true;
    double overStiffness_ = 0.0;
    double overDamping_ = 0.0;
    bool updateEffect_ = false;
    double scrollDistance_ = 0.0;
    bool accessibilityDisabled_ = false;
    Dimension itemExtent_;
    std::pair<bool, Dimension> scrollbarWidth_;
    std::pair<bool, Color> scrollbarColor_;
    std::pair<bool, Dimension> scrollbarPositionX_;
    std::pair<bool, Dimension> scrollbarPositionY_;

    EventMarker onRequestItem_;

    bool indexer_ = false;

    // first:value second:tag whether user defined
    std::pair<bool, bool> circleMode_ = { false, false };
    std::pair<bool, bool> bubble_ = { true, false };
    std::pair<bool, bool> multiLanguage_ = { false, false };

    std::vector<std::string> indexerAlphabet_;
    bool needUpdateIds_ = false;

    // list divider
    bool needDivider_ = false;
    Dimension dividerHeight_ = DIVIDER_DEFAULT_HEIGHT;
    Dimension dividerLength_;
    Dimension dividerOrigin_;
    Color dividerColor_ = Color::TRANSPARENT;

    EventMarker onScroll_;
    EventMarker onScrollTop_;
    EventMarker onScrollBottom_;
    EventMarker onScrollEnd_;
    EventMarker onScrollTouchUp_;
    EventMarker onIndexerChange_;
    FlexDirection flexDirection_ { FlexDirection::COLUMN };
    FlexAlign crossAxisAlign_ { FlexAlign::STRETCH };
    EdgeEffect edgeEffect_ = EdgeEffect::SPRING;
    DisplayMode displayMode_ = DisplayMode::OFF;
    ShapeMode shapeMode_ = ShapeMode::DEFAULT;
    Color fadeColor_ = Color::GRAY;
    Color backgroundColor_ = Color::TRANSPARENT;
    std::string selectedItem_;

    RefPtr<ScrollBar> scrollBar_;

    int32_t beginIndex_ = LIST_PARAM_INVAID;
    int32_t endIndex_ = LIST_PARAM_INVAID;
    int32_t repeatedLength_ = 0;
    int32_t indexOffset_ = 0;

    int32_t initialIndex_ = 0;
    double initialOffset_ = 0.0;

    RefPtr<ListComponent> listComponent_;
    RefPtr<IndexerListComponent> indexerComponent_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_LIST_H
