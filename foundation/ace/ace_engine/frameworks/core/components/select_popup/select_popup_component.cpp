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

#include "core/components/select_popup/select_popup_component.h"

#include "base/subwindow/subwindow_manager.h"
#include "base/utils/string_utils.h"
#include "core/components/clip/clip_component.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/gesture_listener/gesture_listener_component.h"
#include "core/components/menu/menu_component.h"
#include "core/components/positioned/positioned_component.h"
#include "core/components/select_popup/render_select_popup.h"
#include "core/components/select_popup/select_popup_element.h"
#include "core/components/tween/tween_component.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t TITLE_TEXT_MAX_LINES = 1;
const Dimension ROUND_RADIUS_PHONE = 12.0_vp;
const Dimension IN_OUT_BOX_INTERVAL = 4.0_vp;

} // namespace

SelectPopupComponent::SelectPopupComponent() : SoleChildComponent() {}

void SelectPopupComponent::InitTheme(const RefPtr<ThemeManager>& themeManager)
{
    if (!themeManager) {
        return;
    }
    auto selectTheme = themeManager->GetTheme<SelectTheme>();
    if (!selectTheme) {
        return;
    }
    theme_ = selectTheme->clone();
}

RefPtr<RenderNode> SelectPopupComponent::CreateRenderNode()
{
    return RenderSelectPopup::Create();
}

RefPtr<Element> SelectPopupComponent::CreateElement()
{
    return AceType::MakeRefPtr<SelectPopupElement>();
}

RefPtr<OptionComponent> SelectPopupComponent::GetSelectOption(std::size_t index) const
{
    if (index >= options_.size()) {
        LOGE("select: input index is too large.");
        return nullptr;
    }

    return options_[index];
}

void SelectPopupComponent::InnerHideDialog(uint32_t index)
{
    if (!dialogShowed_) {
        return;
    }

    if (!stackElement_) {
        LOGE("stored stack element is null.");
        return;
    }

    stackElement_->PopMenu();
    dialogShowed_ = false;
    stackElement_ = nullptr;

    if (index == SELECT_INVALID_INDEX) {
        if (popupCanceledCallback_) {
            popupCanceledCallback_();
        } else {
            LOGW("popup cancel callback is null.");
        }
    } else {
        if (optionClickedCallback_) {
            optionClickedCallback_(index);
        } else {
            LOGW("option clicked callback is null.");
        }
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    if (node_) {
        auto parentNode = node_->GetParentNode();
        if (parentNode) {
            parentNode->SetLeft(0);
            parentNode->SetTop(0);
            parentNode->SetWidth(0);
            parentNode->SetHeight(0);
        }
    }
#endif
    auto manager = manager_.Upgrade();
    if (manager) {
        for (const auto& option : options_) {
            option->SetNode(nullptr);
        }
        manager->RemoveAccessibilityNodes(node_);
        SetNode(nullptr);
    }
}

void SelectPopupComponent::HideDialog(uint32_t index)
{
    if (!dialogShowed_) {
        return;
    }

    if (refreshAnimationCallback_ && animationController_) {
        hideOption_.ClearListeners();
        refreshAnimationCallback_(hideOption_, false);
        animationController_->AddStopListener([weak = WeakClaim(this), index]() {
            auto refPtr = weak.Upgrade();
            if (!refPtr) {
                return;
            }
            refPtr->InnerHideDialog(index);
        });
        animationController_->Play();
    } else {
        InnerHideDialog(index);
    }
}

void SelectPopupComponent::ShowDialog(
    const RefPtr<StackElement>& stackElement, const Offset& leftTop, const Offset& rightBottom, bool isMenu)
{
    if (dialogShowed_) {
        return;
    }

    RefPtr<PositionedComponent> positioned = AceType::DynamicCast<PositionedComponent>(GetChild());
    if (positioned) {
        positioned->SetAutoFocus(true);
    }
    if (!IsTV() && isMenu) {
        // do use center point reference for phone menu.
        Offset center(leftTop.GetX() / 2 + rightBottom.GetX() / 2, leftTop.GetY() / 2 + rightBottom.GetY() / 2);
        selectLeftTop_ = center;
        selectRightBottom_ = center;
    } else {
        selectLeftTop_ = leftTop;
        selectRightBottom_ = rightBottom;
    }

    stackElement->PushComponent(AceType::Claim(this));
    dialogShowed_ = true;
    stackElement_ = stackElement;
    isMenu_ = isMenu;
}

void SelectPopupComponent::ShowContextMenu(const Offset& offset)
{
    LOGI("Show contextMenu, position is %{public}s", offset.ToString().c_str());
    RefPtr<PositionedComponent> positioned = AceType::DynamicCast<PositionedComponent>(GetChild());
    if (positioned) {
        positioned->SetAutoFocus(true);
    }
    selectLeftTop_ = offset;
    selectRightBottom_ = offset;
    SubwindowManager::GetInstance()->ShowMenu(AceType::Claim(this));
}

void SelectPopupComponent::CloseContextMenu()
{
    LOGI("Close Contextmenu.");
    if (refreshAnimationCallback_ && animationController_) {
        hideOption_.ClearListeners();
        refreshAnimationCallback_(hideOption_, false);
        animationController_->AddStopListener([]() {
            SubwindowManager::GetInstance()->ClearMenu();
        });
        animationController_->Play();
    } else {
        SubwindowManager::GetInstance()->ClearMenu();
    }
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    auto parentNode = node_->GetParentNode();
    if (parentNode) {
        parentNode->SetLeft(0);
        parentNode->SetTop(0);
        parentNode->SetWidth(0);
        parentNode->SetHeight(0);
    }
#endif
    auto manager = manager_.Upgrade();
    if (manager) {
        for (const auto& option : options_) {
            option->SetNode(nullptr);
        }
        manager->RemoveAccessibilityNodes(node_);
        SetNode(nullptr);
    }
}

RefPtr<BoxComponent> SelectPopupComponent::InitializeInnerBox(const RefPtr<ScrollComponent>& scroll)
{
    RefPtr<BoxComponent> innerBox = AceType::MakeRefPtr<BoxComponent>();
    innerBox->SetDeliverMinToChild(false);
    if (title_.empty()) {
        innerBox->SetChild(scroll);
    } else {
        RefPtr<GestureListenerComponent> titleGesture = AceType::MakeRefPtr<GestureListenerComponent>();
        EventMarker mark("-1");
        titleGesture->SetOnClickId(mark);
        RefPtr<BoxComponent> titleBox = AceType::MakeRefPtr<BoxComponent>();
        titleBox->SetDeliverMinToChild(false);
        titleBox->SetPadding(Edge(theme_->GetTitleLeftPadding().Value(), theme_->GetTitleTopPadding().Value(),
            theme_->GetTitleRightPadding().Value(), theme_->GetTitleBottomPadding().Value(),
            theme_->GetTitleBottomPadding().Unit()));
        auto title = AceType::MakeRefPtr<TextComponent>(title_);
        auto textStyle = GetTitleStyle();
        auto isRtl = GetTextDirection() == TextDirection::RTL;
        if (isRtl) {
            textStyle.SetTextAlign(TextAlign::RIGHT);
        }
        textStyle.SetMaxLines(TITLE_TEXT_MAX_LINES);
        textStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
        title->SetTextStyle(textStyle);
        title->SetFocusColor(GetTitleStyle().GetTextColor());
        titleGesture->SetChild(title);
        titleBox->SetChild(titleGesture);

        std::list<RefPtr<Component>> childList;
        childList.emplace_back(titleBox);
        childList.emplace_back(scroll);

        RefPtr<ColumnComponent> outColumn =
            AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_START, childList);
        innerBox->SetChild(outColumn);
        if (isRtl) {
            titleBox->SetAlignment(Alignment::CENTER_RIGHT);
            outColumn->SetMainAxisAlign(FlexAlign::FLEX_END);
            outColumn->SetCrossAxisAlign(FlexAlign::FLEX_END);
        }
    }
    return innerBox;
}

void SelectPopupComponent::SetDefaultSelecting()
{
    if (options_.empty()) {
        return;
    }

    bool hasSelecting = false;
    for (const auto& option : options_) {
        if (option->GetSelected()) {
            hasSelecting = true;
            break;
        }
    }

    if (!hasSelecting) {
        options_[0]->SetSelected(true);
    }
}

bool SelectPopupComponent::Initialize(const RefPtr<AccessibilityManager>& manager)
{
    if (options_.size() == 0 || !manager) {
        LOGW("select: there is no any option or accessibility manager is null.");
        return false;
    }
    manager_ = manager;
    auto id = manager->GenerateNextAccessibilityId();
    std::list<RefPtr<Component>> children;
    for (std::size_t index = 0; index < options_.size(); index++) {
        options_[index]->SetIndex(index);
        auto customizedFunc = options_[index]->GetCustomizedCallback();
        options_[index]->SetClickedCallback(
            [weak = WeakClaim(this), customizedFunc](std::size_t index) {
                if (customizedFunc) {
                    customizedFunc();
                }
                auto refPtr = weak.Upgrade();
                if (!refPtr) {
                    return;
                }
                refPtr->HandleOptionClick(index);
            }
        );
        options_[index]->SetParentId(id);
        if (options_[index]->GetVisible()) {
            children.push_back(options_[index]);
        }
    }

    RefPtr<ColumnComponent> column =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_START, children);
    RefPtr<ScrollComponent> scroll = AceType::MakeRefPtr<ScrollComponent>(column);
    RefPtr<BoxComponent> innerBox = InitializeInnerBox(scroll);
    RefPtr<ClipComponent> innerClip = AceType::MakeRefPtr<ClipComponent>(innerBox);
    innerClip->SetTopLeftRadius(Radius(ROUND_RADIUS_PHONE));
    innerClip->SetTopRightRadius(Radius(ROUND_RADIUS_PHONE));
    innerClip->SetBottomLeftRadius(Radius(ROUND_RADIUS_PHONE));
    innerClip->SetBottomRightRadius(Radius(ROUND_RADIUS_PHONE));

    RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
    box->SetDeliverMinToChild(false);
    if (!IsTV()) {
        RefPtr<Decoration> back = AceType::MakeRefPtr<Decoration>();
        back->SetBackgroundColor(theme_->GetBackgroundColor());
        back->SetBorderRadius(Radius(theme_->GetPopupRRectSize()));
        back->AddShadow(ShadowConfig::DefaultShadowM);
        box->SetBackDecoration(back);
        box->SetPadding(Edge(IN_OUT_BOX_INTERVAL));
    }
    box->SetChild(innerClip);

    auto tweenId = TweenComponent::AllocTweenComponentId();
    RefPtr<TweenComponent> tween = AceType::MakeRefPtr<TweenComponent>(tweenId, tweenId);
    tween->SetShadow(ShadowConfig::DefaultShadowM);
    tween->SetIsFirstFrameShow(false);
    tween->SetAnimationOperation(AnimationOperation::PLAY);

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    auto popupNode = manager->CreateSpecializedNode("select-popup", id, GetSelectPopupId());
    SetNode(popupNode);
#else
    SetNode(manager->CreateSpecializedNode("select-popup", id, -1));
#endif
    if (isFullScreen_) {
        RefPtr<FocusCollaborationComponent> collaboration = AceType::MakeRefPtr<FocusCollaborationComponent>();
        collaboration->InsertChild(0, box);
        tween->SetChild(collaboration);
        RefPtr<PositionedComponent> positioned = AceType::MakeRefPtr<PositionedComponent>(tween);
        SetChild(positioned);
    } else {
        tween->SetChild(box);
        SetChild(tween);
    }
    return true;
}

void SelectPopupComponent::HandleOptionClick(std::size_t index)
{
    HideDialog(index);
}

void SelectPopupComponent::HandleOptionModify(std::size_t index)
{
    if (!optionModifiedCallback_) {
        LOGE("modify callback of select popup component is null.");
        return;
    }
    RefPtr<OptionComponent> selectedOption;
    RefPtr<OptionComponent> modifiedOption;
    for (const auto& option : options_) {
        if (option->GetSelected()) {
            selectedOption = option;
        }
        if (option->GetIndex() == index) {
            modifiedOption = option;
        }
    }
    if (!modifiedOption) {
        LOGE("modify option is null of select popup component.");
        return;
    }
    if (!(modifiedOption == selectedOption || modifiedOption->GetIndex() == 0)) {
        LOGE("no need modify callback of select popup component.");
        return;
    }
    optionModifiedCallback_(index);
}

void SelectPopupComponent::AppendSelectOption(const RefPtr<OptionComponent>& selectOption)
{
    if (selectOption) {
        selectOption->SetIndex(options_.size());
        options_.emplace_back(selectOption);
        auto weak = AceType::WeakClaim(this);
        selectOption->SetModifiedCallback([weak](std::size_t index) {
            auto refPtr = weak.Upgrade();
            if (refPtr) {
                refPtr->HandleOptionModify(index);
            }
        });
    } else {
        LOGE("select: input select option component is null.");
    }
}

void SelectPopupComponent::RemoveSelectOption(const RefPtr<OptionComponent>& selectOption)
{
    if (selectOption) {
        auto iter = std::remove(options_.begin(), options_.end(), selectOption);
        if (iter != options_.end()) {
            options_.erase(iter);
            selectOption->SetIndex(SELECT_INVALID_INDEX);
        }
    } else {
        LOGE("select: input select option component is null.");
    }
    for (uint32_t index = 0; index < options_.size(); ++index) {
        options_[index]->SetIndex(index);
    }
}

void SelectPopupComponent::InsertSelectOption(const RefPtr<OptionComponent>& selectOption, uint32_t index)
{
    if (!selectOption) {
        return;
    }
    if (index >= options_.size()) {
        AppendSelectOption(selectOption);
        return;
    }
    options_.insert(options_.begin() + index, selectOption);
    for (uint32_t index = 0; index < options_.size(); ++index) {
        options_[index]->SetIndex(index);
    }
    auto weak = AceType::WeakClaim(this);
    selectOption->SetModifiedCallback([weak](std::size_t index) {
        auto refPtr = weak.Upgrade();
        if (refPtr) {
            refPtr->HandleOptionModify(index);
        }
    });
}

} // namespace OHOS::Ace
