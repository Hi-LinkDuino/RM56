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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_COMPONENT_H

#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/edge.h"
#include "core/components/tab_bar/tab_bar_indicator_component.h"
#include "core/components/tab_bar/tab_bar_item_component.h"
#include "core/components/tab_bar/tab_controller.h"
#include "core/components/tab_bar/tab_theme.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

enum class TabBarIndicatorType {
    TAB,
    LABEL,
};

class ACE_EXPORT TabBarComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(TabBarComponent, ComponentGroup);

public:
    TabBarComponent(const std::list<RefPtr<Component>>& tabItems, const RefPtr<TabController>& controller,
        const RefPtr<BoxComponent>& indicator = MakeRefPtr<TabBarIndicatorComponent>());
    ~TabBarComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void InitStyle(const RefPtr<TabTheme>& theme);

    void SetVertical(bool isVertical)
    {
        vertical_ = isVertical;
    }

    bool IsVertical() const
    {
        return vertical_;
    }

    void SetIndex(int32_t index)
    {
        if (controller_) {
            auto size = static_cast<int32_t>(GetChildren().size());
            if (index < 0 || index >= size) {
                index = 0;
            }
            controller_->SetIndexWithoutChangeContent(index);
        }
    }

    void SetController(const RefPtr<TabController>& controller)
    {
        controller_ = controller;
    }

    RefPtr<TabController> GetController() const
    {
        return controller_;
    }

    const RefPtr<BoxComponent>& GetIndicator() const
    {
        return indicator_;
    }

    const RefPtr<BoxComponent>& GetFocusIndicator() const
    {
        return focusIndicator_;
    }

    TabBarMode GetMode() const
    {
        return mode_;
    }

    void SetMode(TabBarMode mode)
    {
        mode_ = mode;
    }

    TabBarIndicatorType GetIndicatorSize() const
    {
        return indicatorSize_;
    }

    void SetIndicatorSize(TabBarIndicatorType indicatorSize)
    {
        indicatorSize_ = indicatorSize;
    }

    const Edge& GetLabelPadding() const
    {
        return labelPadding_;
    }

    void SetLabelPadding(const Edge& labelPadding)
    {
        labelPadding_ = labelPadding;
    }

    const Edge& GetPadding() const
    {
        return padding_;
    }

    void SetPadding(const Edge& padding)
    {
        padding_ = padding;
    }

    const Color& GetFocusAnimationColor() const
    {
        return focusAnimationColor_;
    }

    const Dimension& GetFocusRadiusDimension() const
    {
        return focusRadiusDimension_;
    }

    const Dimension& GetGradientWidth() const
    {
        return gradientWidth_;
    }

    const Dimension& GetActiveIndicatorMinWidth() const
    {
        return activeIndicatorMinWidth_;
    }

    const EventMarker& GetDomChangeEventId() const
    {
        return domChangeEventId_;
    }

    void SetDomChangeEventId(const EventMarker& domChangeEventId)
    {
        domChangeEventId_ = domChangeEventId;
    }

    void SetBarPosition(BarPosition barPosition)
    {
        barPosition_ = barPosition;
    }
    BarPosition GetBarPosition() const
    {
        return barPosition_;
    }

    void InitNavigationBarStyle();
    void InitBottomTabStyle(const RefPtr<TabTheme>& theme);
    // Called by element on perform build process.
    void BuildItems(std::list<RefPtr<TabBarItemComponent>>& items);

    void ResetIndicator()
    {
        indicator_.Reset();
        focusIndicator_.Reset();
    }

    void SetAlignment(Alignment alignment)
    {
        itemAlignment_ = alignment;
    }

private:
    bool vertical_ { false }; // the tab is vertical or not, default value is false

    TabBarIndicatorType indicatorSize_ { TabBarIndicatorType::LABEL };
    TabBarMode mode_ { TabBarMode::FIXED };
    Edge labelPadding_;
    Edge padding_;

    Color focusAnimationColor_ = Color::WHITE;
    Dimension focusRadiusDimension_;
    Dimension gradientWidth_;
    Dimension activeIndicatorMinWidth_;

    RefPtr<BoxComponent> indicator_;
    RefPtr<BoxComponent> focusIndicator_;
    RefPtr<TabController> controller_;

    bool usingDefaultStyle_ { false };
    TextStyle activeTextStyle_;
    TextStyle inactiveTextStyle_;
    Color activeColor_;
    Color inactiveColor_;
    Alignment itemAlignment_ = Alignment::CENTER;
    EventMarker domChangeEventId_;
    BarPosition barPosition_ = BarPosition::START;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_COMPONENT_H
