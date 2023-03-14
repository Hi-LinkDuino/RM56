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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_SELECT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_SELECT_COMPONENT_H

#include <vector>

#include "base/geometry/dimension.h"
#include "core/components/box/box_component.h"
#include "core/components/common/properties/color.h"
#include "core/components/select/select_theme.h"
#include "core/components/select_popup/select_popup_component.h"
#include "core/components/text/text_component.h"
#include "core/components_v2/common/common_def.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class ACE_EXPORT SelectComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(SelectComponent, SoleChildComponent);

public:
    SelectComponent();
    ~SelectComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void InitTheme(const RefPtr<ThemeManager>& themeManager);

    void SetTextDirection(TextDirection direction) override
    {
        SoleChildComponent::SetTextDirection(direction);
        if (popup_) {
            popup_->SetTextDirection(direction);
        } else {
            LOGE("popup component of select is null.");
        }
    }

    bool GetDisabled() const
    {
        return disabled_;
    }
    void SetDisabled(bool disabled)
    {
        disabled_ = disabled;
    }

    bool GetClicked() const
    {
        return clicked_;
    }
    void SetClicked(bool clicked, const Color& pressColor);

    const Color& GetClickedColor() const
    {
        return theme_->GetClickedColor();
    }
    void SetClickedColor(const Color& clickedColor)
    {
        theme_->SetClickedColor(clickedColor);
    }

    const Color& GetSelectedColor() const
    {
        return theme_->GetSelectedColor();
    }
    void SetSelectedColor(const Color& selectedColor)
    {
        theme_->SetSelectedColor(selectedColor);
    }

    const Color& GetDisabledColor() const
    {
        return theme_->GetDisabledColor();
    }
    void SetDisabledColor(const Color& disabledColor)
    {
        theme_->SetDisabledColor(disabledColor);
    }

    bool IsAllowScale() const
    {
        return theme_->IsAllowScale();
    }
    void SetAllowScale(bool value)
    {
        theme_->SetAllowScale(value);
    }

    const Color& GetTextColor() const
    {
        return theme_->GetFontColor();
    }
    void SetTextColor(const Color& value)
    {
        theme_->SetFontColor(value);
    }

    FontWeight GetFontWeight() const
    {
        return theme_->GetFontWeight();
    }
    void SetFontWeight(FontWeight value)
    {
        theme_->SetFontWeight(value);
    }

    TextDecoration GetTextDecoration() const
    {
        return theme_->GetTextDecoration();
    }
    void SetTextDecoration(TextDecoration value)
    {
        theme_->SetTextDecoration(value);
    }

    const EventMarker& GetOnChanged() const
    {
        return onChanged_;
    }
    void SetOnChanged(const EventMarker& onChanged)
    {
        onChanged_ = onChanged;
    }

    const RefPtr<TextComponent>& GetTipText() const
    {
        return tipText_;
    }
    void SetTipText(const RefPtr<TextComponent>& tipText)
    {
        tipText_ = tipText;
    }

    bool IsFontSet() const
    {
        return isSelectFontSize_;
    }

    void SetIsFontSetFlag(bool flag)
    {
        isSelectFontSize_ = flag;
    }

    const Dimension& GetFontSize() const
    {
        return theme_->GetFontSize();
    }
    void SetFontSize(const Dimension& fontSize)
    {
        theme_->SetFontSize(fontSize);
    }

    const std::string& GetFontFamily() const
    {
        return theme_->GetFontFamily();
    }
    void SetFontFamily(const std::string& fontFamily)
    {
        theme_->SetFontFamily(fontFamily);
    }

    std::size_t GetOptionSize() const
    {
        return theme_->GetOptionSize();
    }
    void SetOptionSize(std::size_t optionSize)
    {
        theme_->SetOptionSize(optionSize);
    }

    const Dimension& GetRRectSize() const
    {
        return theme_->GetRRectSize();
    }
    void SetRRectSize(const Dimension& rrectSize)
    {
        theme_->SetRRectSize(rrectSize);
    }

    const Dimension& GetPopupShadowWidth() const
    {
        return theme_->GetPopupShadowWidth();
    }
    void SetPopupShadowWidth(const Dimension& popupShadowWidth)
    {
        theme_->SetPopupShadowWidth(popupShadowWidth);
    }

    const Dimension& GetPopupBorderWidth() const
    {
        return theme_->GetPopupBorderWidth();
    }
    void SetPopupBorderWidth(const Dimension& popupBorderWidth)
    {
        theme_->SetPopupBorderWidth(popupBorderWidth);
    }

    const Dimension& GetNormalPadding() const
    {
        return theme_->GetNormalPadding();
    }

    void SetOptionClickedCallback(const std::function<void(std::size_t)>& clickedCallback)
    {
        if (popup_) {
            popup_->SetOptionClickedCallback(clickedCallback);
        }
    }

    void SetOptionModifiedCallback(const std::function<void(std::size_t)>& value)
    {
        if (popup_) {
            popup_->SetOptionModifiedCallback(value);
        }
    }

    std::size_t GetSelectOptionCount() const
    {
        if (!popup_) {
            return 0;
        }

        return popup_->GetSelectOptionCount();
    }

    RefPtr<OptionComponent> GetSelectOption(std::size_t index)
    {
        if (!popup_) {
            return nullptr;
        }
        selected_ = index;
        return popup_->GetSelectOption(index);
    }

    std::size_t GetSelected() const
    {
        return selected_;
    }

    void ClearAllOptions()
    {
        if (popup_) {
            popup_->ClearAllOptions();
        }
    }

    void AppendSelectOption(const RefPtr<OptionComponent>& selectOption)
    {
        if (popup_) {
            popup_->AppendSelectOption(selectOption);
        }
    }

    void RemoveSelectOption(const RefPtr<OptionComponent>& selectOption)
    {
        if (popup_) {
            popup_->RemoveSelectOption(selectOption);
        }
    }

    void InsertSelectOption(const RefPtr<OptionComponent>& selectOption, uint32_t index)
    {
        if (popup_) {
            popup_->InsertSelectOption(selectOption, index);
        }
    }

    RefPtr<SelectPopupComponent> GetPopup() const;

    RefPtr<BoxComponent> GetBoxComponent() const;

    bool Initialize();

    void FlushRefresh()
    {
        if (flushRefreshCallback_) {
            flushRefreshCallback_();
        }
    }

    void SetBackgroundColor(const Color& value)
    {
        backgroundColor_ = value;
    }

    void SetInnerPadding(const Edge& innerPadding)
    {
        innerPadding_ = innerPadding;
    }

    void SetInnerSize(const Dimension& width, const Dimension& height)
    {
        width_ = width;
        height_ = height;
    }

    void SetInnerBorder(const Border& border)
    {
        border_ = border;
    }

    const Border& GetInnerBorder() const
    {
        return border_;
    }

    // used for inspector node in PC preview
    void SetNodeId(const int32_t nodeId)
    {
        nodeId_ = nodeId;
    }

    // used for inspector node in PC preview
    int32_t GetNodeId() const
    {
        return nodeId_;
    }

    const TextStyle& GetSelectStyle() const
    {
        return theme_->GetTitleStyle();
    }

    void SetSelectStyle(const TextStyle& style)
    {
        theme_->SetTitleStyle(style);
    }

    void SetTheme(const RefPtr<SelectTheme>& theme)
    {
        if (popup_) {
            popup_->SetTheme(theme);
        }
        theme_ = theme;
    }
    ACE_DEFINE_COMPONENT_EVENT(OnSelected, void(int32_t, std::string));

    RefPtr<SelectPopupComponent> GetSelectPopupComponent() const
    {
        return popup_;
    }

    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }

    void SetLeftPadding(const Dimension& padding)
    {
        innerPadding_.SetLeft(padding);;
    }

    void SetRightPadding(const Dimension& padding)
    {
        innerPadding_.SetRight(padding);
    }

    const Dimension& GetLeftPadding() const
    {
        return innerPadding_.Left();
    }

    const Dimension& GetRightPadding() const
    {
        return innerPadding_.Right();
    }

    void SetTopPadding(const Dimension& padding)
    {
        innerPadding_.SetTop(padding);
    }

    void SetBottomPadding(const Dimension& padding)
    {
        innerPadding_.SetBottom(padding);
    }

    const Dimension& GetTopPadding() const
    {
        return innerPadding_.Top();
    }

    const Dimension& GetBottomPadding() const
    {
        return innerPadding_.Bottom();
    }

private:
    bool disabled_ { false };
    bool clicked_ { false };
    EventMarker onChanged_;
    RefPtr<TextComponent> tipText_;
    RefPtr<SelectPopupComponent> popup_;
    RefPtr<BoxComponent> boxComponent_;
    RefPtr<SelectTheme> theme_;
    std::function<void()> flushRefreshCallback_;
    Color backgroundColor_ = Color::TRANSPARENT;
    std::size_t selected_ = 0;
    // used for inspector node in PC preview
    int32_t nodeId_ = -1;

    // used in navigation
    bool isSelectFontSize_ = false;
    Edge innerPadding_ = Edge(8.0, 9.25, 8.0, 9.25, DimensionUnit::VP);

    Dimension width_ = -1.0_px;
    Dimension height_ = -1.0_px;
    Border border_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_SELECT_COMPONENT_H
