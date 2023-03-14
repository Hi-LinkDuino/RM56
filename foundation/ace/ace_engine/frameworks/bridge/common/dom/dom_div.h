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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_DIV_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_DIV_H

#include "base/utils/macros.h"
#include "core/components/flex/flex_component.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/grid_layout_item_component.h"
#include "core/components/scroll/scroll_component.h"
#include "core/components/theme/card_theme.h"
#include "core/components/wrap/wrap_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMDiv : public DOMNode {
    DECLARE_ACE_TYPE(DOMDiv, DOMNode);

public:
    DOMDiv(NodeId nodeId, const std::string& nodeName);
    ~DOMDiv() override = default;

    FlexDirection GetFlexDirection() const
    {
        return flexDirection_;
    };

    const RefPtr<ScrollComponent>& GetScrollComponent() const override
    {
        return scroll_;
    }

    RefPtr<Component> GetSpecializedComponent() override;
    void AdjustSpecialParamInLiteMode() override;

protected:
    void OnMounted(const RefPtr<DOMNode>& parentNode) override;
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    void PrepareSpecializedComponent() override;
    void CompositeComponents() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;

    RefPtr<FlexComponent> flexChild_;
    RefPtr<WrapComponent> wrapChild_;
    std::string direction_ { DOM_FLEX_ROW };

private:
    enum class AxisAlign {
        START = 0,
        CENTER = 1,
        END = 2,
    };

    static Alignment ComputeFlexAlign(FlexAlign flexMainAlign, FlexAlign flexCrossAlign, bool isColumn, bool isRtl);
    void CreateOrUpdateGrid();
    void CreateOrUpdateGridItem();
    void CreateOrUpdateFlex();
    void CreateOrUpdateWrap();
    void SetBoxWidthFlex(bool isHorizontal) const;
    void SetRootBoxHeight() const;
    void SetFlexHeight(FlexAlign flexMainAlign);
    void SetCardThemeAttrs();
    void InitScrollBar();
    void SetSpecializedOverflow();

    bool isFlexWrap_ = false;
    std::string justifyContent_ { DOM_JUSTIFY_CONTENT_START };
    std::string alignItems_ { DOM_ALIGN_ITEMS_STRETCH };
    std::string alignContent_ { DOM_ALIGN_CONTENT_START };

    // Wrap's variables
    double spacing_ = 0.0;
    double contentSpacing_ = 0.0;

    // flex properties.
    FlexDirection flexDirection_ = FlexDirection::ROW;
    TextDirection textDirection_ = TextDirection::LTR;

    // for grid layout
    std::string columnsArgs_;
    std::string rowsArgs_;
    Dimension columnGap_ = 0.0_px;
    Dimension rowGap_ = 0.0_px;

    // for grid layout item
    int32_t columnStart_ = -1;
    int32_t columnEnd_ = -1;
    int32_t rowStart_ = -1;
    int32_t rowEnd_ = -1;

    // for scroll reaching the edge event
    EventMarker onReachStart_;
    EventMarker onReachEnd_;
    EventMarker onReachTop_;
    EventMarker onReachBottom_;

    RefPtr<ScrollComponent> scroll_;
    RefPtr<GridLayoutComponent> grid_;
    RefPtr<GridLayoutItemComponent> gridItem_;
    RefPtr<CardTheme> cardTheme_;
    bool isCardBlur_ = false;
    bool isCard_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_DIV_H
