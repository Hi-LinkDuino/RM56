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

#include "core/components/select/select_element.h"

#include "core/components/box/render_box.h"
#include "core/components/positioned/positioned_component.h"
#include "core/components/root/render_root.h"
#include "core/components/select/render_select.h"
#include "core/components/select/select_component.h"
#include "core/components/select/select_theme.h"
#include "core/components/select_popup/select_popup_component.h"
#include "core/components/stack/stack_element.h"
#include "core/components/text/render_text.h"
#include "core/event/ace_event_helper.h"
#include "core/gestures/click_recognizer.h"

namespace OHOS::Ace {

void SelectElement::PerformBuild()
{
    LOGD("SelectElement::PerformBuild");
    RefPtr<ClickRecognizer> clickRecognizer = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer->SetOnClick([weak = AceType::WeakClaim(this)](const ClickInfo&) {
        auto element = weak.Upgrade();
        if (element) {
            element->HandleClickedEvent();
        }
    });
    RefPtr<RawRecognizer> rawRecognizer = AceType::MakeRefPtr<RawRecognizer>();
    rawRecognizer->SetOnTouchDown([weak = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto element = weak.Upgrade();
        if (element) {
            element->HandleTouchEvent(true);
        }
    });
    rawRecognizer->SetOnTouchUp([weak = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto element = weak.Upgrade();
        if (element) {
            element->HandleTouchEvent(false);
        }
    });
    rawRecognizer->SetOnTouchCancel([weak = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto element = weak.Upgrade();
        if (element) {
            element->HandleTouchEvent(false);
        }
    });

    RefPtr<RenderSelect> render = AceType::DynamicCast<RenderSelect>(renderNode_);
    if (render) {
        render->SetClickRecognizer(clickRecognizer);
        render->SetRawRecognizer(rawRecognizer);
    } else {
        LOGE("select: can not get render node of select by dynamic cast failed.");
        return;
    }

    RefPtr<SelectComponent> component = AceType::DynamicCast<SelectComponent>(component_);
    if (!component || !component->Initialize()) {
        LOGE("select: can not get component of select by dynamic cast failed or initialize failed.");
        return;
    }

    normalPadding_ = component->GetNormalPadding();
    auto weak = AceType::WeakClaim(this);
    component->SetOptionClickedCallback([weak](std::size_t index) {
        auto refPtr = weak.Upgrade();
        if (refPtr) {
            refPtr->HandleOptionClickedEvent(index);
        }
    });
    component->SetOptionModifiedCallback([weak](std::size_t index) {
        auto refPtr = weak.Upgrade();
        if (refPtr) {
            refPtr->HandleOptionModifiedEvent(index);
        }
    });
    render->SetFocusCallback([weak] {
        auto ref = weak.Upgrade();
        if (ref && ref->IsCurrentFocus()) {
            ref->OnFocus();
        }
    });

    if (context_.Invalid() || component->GetOnChanged().IsEmpty()) {
        LOGE("select: can not set callback of onchange for it is null.");
    } else {
        onChangeCallback_ = AceAsyncEvent<void(const std::string&)>::Create(component->GetOnChanged(), context_);
    }

    if (component->GetOnSelected()) {
        onSelected_ = *component->GetOnSelected();
    }

    dataComponent_ = component;

    auto focusNode = AceType::DynamicCast<FocusNode>(this);
    if (!focusNode) {
        LOGE("select: can not dynamicCast to focusNode.");
        return;
    }
    focusNode->SetFocusable(!component->GetDisabled());

    SoleChildElement::PerformBuild();
}

void SelectElement::HandleClickedEvent()
{
    const auto pipeline = context_.Upgrade();
    if (!pipeline) {
        LOGE("select: can not show dialog, inner pipeline is null.");
        return;
    }

    auto stackElement = pipeline->GetLastStack();
    if (!stackElement) {
        LOGE("select: can not get last stack from pipeline context.");
        return;
    }

    RefPtr<SelectComponent> select = AceType::DynamicCast<SelectComponent>(dataComponent_);
    if (!select || select->GetDisabled()) {
        LOGE("select: the component of select is null or disabled now.");
        return;
    }

    RefPtr<SelectPopupComponent> selectPopup = AceType::DynamicCast<SelectPopupComponent>(select->GetPopup());
    if (!selectPopup) {
        LOGE("select: the type of popup component is not SelectPopupComponent.");
        return;
    }

    RefPtr<RenderSelect> render = AceType::DynamicCast<RenderSelect>(renderNode_);
    if (!render) {
        LOGE("select: can not get render node of select by dynamic cast failed.");
        return;
    }

    if (!RequestFocusImmediately()) {
        LOGE("OnClick can not request SelectElement's focus successfully");
    }

    LOGD("SelectElement::HandleClickedEvent");
    if (selectPopup->GetDialogShowed()) {
        // hide
        selectPopup->HideDialog(SELECT_INVALID_INDEX);
    } else {
        // show
        Offset leftTop = render->GetOffsetToStage();
        Offset rightBottom = leftTop + render->GetLayoutSize();
        selectPopup->SetDefaultSelecting();
        selectPopup->ShowDialog(stackElement, leftTop, rightBottom, false);
    }
}

void SelectElement::HandleTouchEvent(bool isDown)
{
    LOGD("SelectElement::HandleTouchEvent");
    auto component = AceType::DynamicCast<SelectComponent>(dataComponent_);
    if (!component) {
        LOGE("select: the component of select is null now.");
        return;
    }
    if (component->GetDisabled()) {
        LOGW("select: the component of select is disabled now.");
        return;
    }
    auto endColor = isDown ? component->GetClickedColor() : Color::TRANSPARENT;
    PlayEventEffectAnimation(isDown, endColor);
}

void SelectElement::HandleOptionModifiedEvent(std::size_t index)
{
    RefPtr<SelectComponent> component = AceType::DynamicCast<SelectComponent>(dataComponent_);
    if (!component) {
        LOGE("select: the component of select is null now.");
        return;
    }

    auto option = component->GetSelectOption(index);
    if (!option) {
        LOGE("select: can not get option of index[%{public}u].", static_cast<int32_t>(index));
        return;
    }

    auto tagText = component->GetTipText();
    if (!tagText) {
        LOGE("select: can not get current text.");
        return;
    }

    auto selText = option->GetText();
    if (!selText) {
        LOGE("select: can not get select text.");
        return;
    }

    tagText->SetData(selText->GetData());

    RefPtr<RenderNode> render = GetRenderText();
    if (!render) {
        LOGE("select:can not get render text now.");
        return;
    }

    render->Update(tagText);
}

void SelectElement::FlushRefresh()
{
    if (children_.empty()) {
        return;
    }
    RefPtr<SelectComponent> component = AceType::DynamicCast<SelectComponent>(dataComponent_);
    if (!component) {
        LOGE("select: the component of select is null now.");
        return;
    }
    component->Initialize();
    const auto& child = children_.front();
    RemoveChild(child);
    InflateComponent(dataComponent_, DEFAULT_ELEMENT_SLOT, DEFAULT_RENDER_SLOT);
}

void SelectElement::HandleOptionClickedEvent(std::size_t index)
{
    HandleOptionModifiedEvent(index);

    RefPtr<SelectComponent> component = AceType::DynamicCast<SelectComponent>(dataComponent_);
    if (!component) {
        LOGE("select: the component of select is null now.");
        return;
    }

    auto option = component->GetSelectOption(index);
    if (!option) {
        LOGE("select: can not get option of index[%{public}u].", static_cast<int32_t>(index));
        return;
    }

    if (onChangeCallback_) {
        std::string param = std::string("\"change\",{\"newValue\":\"").append(option->GetValue().append("\"},null"));
        onChangeCallback_(param);
    }

    std::string value = option->GetValue();
    if (onSelected_) {
        onSelected_(index, value);
    }
}

RefPtr<RenderNode> SelectElement::GetRenderText() const
{
    auto box = GetRenderBox();
    if (!box) {
        LOGE("select: can not get render node of box by function[GetRenderBox].");
        return nullptr;
    }

    if (box->GetChildren().empty()) {
        LOGE("select: there has no child in box.");
        return nullptr;
    }

    auto inner = box->GetChildren().front();
    if (!inner) {
        LOGE("inner component is null");
        return nullptr;
    }

    if (inner->GetChildren().empty()) {
        LOGE("inner component is empty");
        return nullptr;
    }

    auto row = inner->GetChildren().front();
    if (!row) {
        LOGE("select: can not get render node of row by first child.");
        return nullptr;
    }

    if (row->GetChildren().empty()) {
        LOGE("select: there has no child in box.");
        return nullptr;
    }

    for (const auto& child : row->GetChildren()) {
        auto textItem = AceType::DynamicCast<RenderFlexItem>(child);
        if (textItem) {
            for (const auto& text : textItem->GetChildren()) {
                auto renderText = AceType::DynamicCast<RenderText>(text);
                if (renderText) {
                    return renderText;
                }
            }
        }
    }

    LOGE("select: there has no child in row's all children.");
    return nullptr;
}

RefPtr<RenderNode> SelectElement::GetRenderBox() const
{
    RefPtr<RenderSelect> select = AceType::DynamicCast<RenderSelect>(renderNode_);
    if (!select) {
        LOGE("select: can not get render node of select by dynamic cast failed.");
        return nullptr;
    }

    if (select->GetChildren().empty()) {
        LOGE("select: there has no child in render select.");
        return nullptr;
    }

    auto box = select->GetChildren().front();
    if (!box) {
        LOGE("select: can not get render node of box by first child.");
        return nullptr;
    }

    auto renderBox = AceType::DynamicCast<RenderBox>(box);
    if (!renderBox) {
        LOGE("select: can not get render node of box by dynamic cast.");
        return nullptr;
    }

    return renderBox;
}

void SelectElement::OnClick()
{
    HandleClickedEvent();
}

Corner SelectElement::GetCorner() const
{
    RefPtr<SelectComponent> component = AceType::DynamicCast<SelectComponent>(dataComponent_);
    Corner corner;
    if (!component) {
        LOGE("select: the component of select is null now.");
        return corner;
    }

    const auto& border = component->GetInnerBorder();
    corner.topLeftRadius = border.TopLeftRadius();
    corner.topRightRadius = border.TopRightRadius();
    corner.bottomLeftRadius = border.BottomLeftRadius();
    corner.bottomRightRadius = border.BottomRightRadius();
    return corner;
}

void SelectElement::OnFocus()
{
    auto render = GetRenderBox();
    if (!render) {
        return;
    }
    auto pipe = context_.Upgrade();
    if (!pipe) {
        return;
    }
    RefPtr<SelectComponent> data = AceType::DynamicCast<SelectComponent>(dataComponent_);
    if (!data) {
        return;
    }
    Size size = render->GetLayoutSize();
    Rect rect(0.0, 0.0, size.Width(), size.Height());
    RRect rrect;
    rrect.SetRect(rect);
    auto corner = GetCorner();
    rrect.SetCorner(corner);
    Offset offset = render->GetGlobalOffset();
    pipe->ShowFocusAnimation(rrect, data->GetClickedColor(), offset, true);
}

void SelectElement::OnBlur()
{
    HandleTouchEvent(false);
}

void SelectElement::SetBackgroundColor(bool isDown, const Color& color)
{
    auto component = AceType::DynamicCast<SelectComponent>(dataComponent_);
    if (!component) {
        LOGE("select: the component of select is null now.");
        return;
    }
    if (component->GetDisabled()) {
        return;
    }
    component->SetClicked(isDown, color);
    auto boxComponent = component->GetBoxComponent();
    if (!boxComponent) {
        LOGE("select: can not get box component of select.");
        return;
    }
    auto boxRender = GetRenderBox();
    if (!boxRender) {
        LOGE("select: can not get box render by function[GetRenderBox].");
        return;
    }
    // Change background color of box of select.
    boxRender->Update(boxComponent);
}

void SelectElement::CreateColorAnimation(RefPtr<KeyframeAnimation<Color>>& animation, const Color& from,
    const Color& to, bool isDown)
{
    if (!animation) {
        return;
    }
    auto start = AceType::MakeRefPtr<Keyframe<Color>>(0.0f, from);
    auto end = AceType::MakeRefPtr<Keyframe<Color>>(1.0f, to);
    end->SetCurve(Curves::SHARP);
    animation->AddKeyframe(start);
    animation->AddKeyframe(end);
    animation->AddListener([weak = AceType::WeakClaim(this), isDown](const Color& value) {
        auto select = weak.Upgrade();
        if (select) {
            select->eventEffectColor_ = value;
            select->SetBackgroundColor(isDown, value);
        }
    });
}

void SelectElement::PlayEventEffectAnimation(bool isDown, const Color& endColor)
{
    if (!eventEffectController_) {
        eventEffectController_ = AceType::MakeRefPtr<Animator>(context_);
    }
    if (!eventEffectController_->IsStopped()) {
        eventEffectController_->Stop();
    }
    auto colorAnimation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    CreateColorAnimation(colorAnimation, eventEffectColor_, endColor, isDown);
    eventEffectController_->ClearInterpolators();
    eventEffectController_->ClearStopListeners();
    eventEffectController_->AddInterpolator(colorAnimation);
    eventEffectController_->SetDuration(PRESS_DURATION);
    eventEffectController_->SetFillMode(FillMode::FORWARDS);
    eventEffectController_->Forward();
}

} // namespace OHOS::Ace
