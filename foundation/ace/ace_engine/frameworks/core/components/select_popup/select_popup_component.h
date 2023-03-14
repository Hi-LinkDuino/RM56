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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_SELECT_POPUP_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_SELECT_POPUP_COMPONENT_H

#include "core/components/box/box_component.h"
#include "core/components/common/properties/tween_option.h"
#include "core/components/option/option_component.h"
#include "core/components/scroll/scroll_component.h"
#include "core/components/select/select_theme.h"
#include "core/components/stack/stack_element.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

using RefreshAnimationCallback = std::function<void(const TweenOption&, bool)>;

class ACE_EXPORT SelectPopupComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(SelectPopupComponent, SoleChildComponent);

public:
    SelectPopupComponent();
    ~SelectPopupComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void InitTheme(const RefPtr<ThemeManager>& themeManager);

    std::size_t GetOptionSize() const
    {
        return optionSize_;
    }

    void SetOptionSize(std::size_t optionSize)
    {
        optionSize_ = optionSize;
    }

    void GetPopupLayout(Offset& leftTop, Offset& rightBottom) const
    {
        leftTop = selectLeftTop_;
        rightBottom = selectRightBottom_;
    }

    const Dimension& GetPopupRRectSize() const
    {
        return theme_->GetPopupRRectSize();
    }

    const Dimension& GetNormalPadding() const
    {
        return theme_->GetNormalPadding();
    }

    const Dimension& GetPopupMinWidth() const
    {
        return theme_->GetPopupMinWidth();
    }

    const Dimension& GetBorderWidth() const
    {
        return theme_->GetPopupBorderWidth();
    }

    const Dimension& GetShadowWidth() const
    {
        return theme_->GetPopupShadowWidth();
    }

    std::size_t GetSelectOptionCount() const
    {
        return options_.size();
    }

    RefPtr<OptionComponent> GetSelectOption(std::size_t index) const;

    std::vector<RefPtr<OptionComponent>> GetSelectOptions() const
    {
        return options_;
    }

    void ClearAllOptions()
    {
        options_.clear();
    }

    void AppendSelectOption(const RefPtr<OptionComponent>& selectOption);

    void RemoveSelectOption(const RefPtr<OptionComponent>& selectOption);

    void InsertSelectOption(const RefPtr<OptionComponent>& selectOption, uint32_t index);

    void SetOptionClickedCallback(const std::function<void(std::size_t)>& clickedCallback)
    {
        optionClickedCallback_ = clickedCallback;
    }

    void SetPopupCanceledCallback(const std::function<void()>& canceledCallback)
    {
        popupCanceledCallback_ = canceledCallback;
    }

    void SetOptionModifiedCallback(const std::function<void(std::size_t)>& value)
    {
        optionModifiedCallback_ = value;
    }

    const std::function<void()>& GetPopupCanceledCallback() const
    {
        return popupCanceledCallback_;
    }

    bool GetDialogShowed() const
    {
        return dialogShowed_;
    }

    const std::string& GetTitle() const
    {
        return title_;
    }

    void SetTitle(const std::string& title)
    {
        title_ = title;
    }

    const TextStyle& GetTitleStyle() const
    {
        return theme_->GetTitleStyle();
    }

    void SetTitleStyle(const TextStyle& style)
    {
        theme_->SetTitleStyle(style);
    }

    bool IsTV() const
    {
        return theme_->IsTV();
    }

    bool IsMenu() const
    {
        return isMenu_;
    }

    bool IsContextMenu() const
    {
        return isContextMenu_;
    }

    void SetIsContextMenu(bool isContextMenu)
    {
        isContextMenu_ = isContextMenu;
    }

    const Dimension& GetHorizontalSpacing() const
    {
        return theme_->GetHorizontalSpacing();
    }

    const Dimension& GetVerticalSpacing() const
    {
        return theme_->GetVerticalSpacing();
    }

    const Dimension& GetContentSpacing() const
    {
        return theme_->GetContentSpacing();
    }

    const TweenOption& GetShowOption() const
    {
        return showOption_;
    }
    void SetShowOption(const TweenOption& value)
    {
        showOption_ = value;
        if (refreshAnimationCallback_) {
            refreshAnimationCallback_(showOption_, true);
        } else {
            LOGE("refresh animation callback is null.");
        }
    }

    void SetRefreshAnimationCallback(const RefreshAnimationCallback& value)
    {
        refreshAnimationCallback_ = value;
    }
    const RefreshAnimationCallback& GetRefreshAnimationCallback() const
    {
        return refreshAnimationCallback_;
    }

    const TweenOption& GetHideOption() const
    {
        return hideOption_;
    }
    void SetHideOption(const TweenOption& value)
    {
        hideOption_ = value;
    }

    const RefPtr<Animator>& GetAnimationController() const
    {
        return animationController_;
    }
    void SetAnimationController(const RefPtr<Animator>& value)
    {
        animationController_ = value;
    }

    bool IsFullScreen() const
    {
        return isFullScreen_;
    }
    void SetIsFullScreen(bool isFullScreen)
    {
        isFullScreen_ = isFullScreen;
    }

    void SetTheme(const RefPtr<SelectTheme>& theme)
    {
        theme_ = theme->clone();
    }

    const RefPtr<SelectTheme>& GetTheme() const
    {
        return theme_;
    }

    void HideDialog(uint32_t index);
    void ShowDialog(
        const RefPtr<StackElement>& stackElement, const Offset& leftTop, const Offset& rightBottom, bool isMenu);
    void ShowContextMenu(const Offset& offset);
    void CloseContextMenu();

    bool Initialize(const RefPtr<AccessibilityManager>& manager);

    const RefPtr<AccessibilityNode> GetNode() const
    {
        return node_;
    }

    RefPtr<StackElement> GetStackElement() const
    {
        return stackElement_;
    }

    void SetNode(const RefPtr<AccessibilityNode>& value)
    {
        node_ = value;
    }

    // used for inspector node in PC preview
    void SetSelectPopupId(const int32_t nodeId)
    {
        nodeId_ = nodeId;
    }

    // used for inspector node in PC preview
    int32_t GetSelectPopupId() const
    {
        return nodeId_;
    }

    void SetDefaultSelecting();

    bool GetPopupShowed()
    {
        return dialogShowed_;
    }

private:
    RefPtr<BoxComponent> InitializeInnerBox(const RefPtr<ScrollComponent>& scroll);
    void HandleOptionClick(std::size_t index);
    void HandleOptionModify(std::size_t index);
    void InnerHideDialog(uint32_t index);

    bool isFullScreen_ = true;
    std::size_t optionSize_ { SELECT_DEFAULT_OPTION_SIZE };
    Offset selectLeftTop_;
    Offset selectRightBottom_;
    bool isMenu_ = false;
    bool isContextMenu_ = false;
    std::vector<RefPtr<OptionComponent>> options_;
    std::string title_;
    RefPtr<SelectTheme> theme_;
    bool dialogShowed_ = false;
    // used for inspector node in PC preview
    int32_t nodeId_ = -1;
    std::function<void(std::size_t)> optionClickedCallback_;
    std::function<void(std::size_t)> optionModifiedCallback_;
    std::function<void()> popupCanceledCallback_;
    RefPtr<StackElement> stackElement_;

    TweenOption showOption_;
    TweenOption hideOption_;
    RefreshAnimationCallback refreshAnimationCallback_;
    RefPtr<Animator> animationController_;

    RefPtr<AccessibilityNode> node_;
    WeakPtr<AccessibilityManager> manager_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_SELECT_POPUP_COMPONENT_H
