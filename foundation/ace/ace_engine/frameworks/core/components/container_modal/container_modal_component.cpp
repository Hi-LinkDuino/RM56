/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components/container_modal/container_modal_component.h"

#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/clip/clip_component.h"
#include "core/components/container_modal/container_modal_constants.h"
#include "core/components/container_modal/container_modal_element.h"
#include "core/components/container_modal/render_container_modal.h"
#include "core/components/image/image_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/text/text_component.h"
#include "core/components/tween/tween_component.h"

namespace OHOS::Ace {

RefPtr<Component> ContainerModalComponent::Create(
    const WeakPtr<PipelineContext>& context, const RefPtr<Component>& child)
{
    auto component = AceType::MakeRefPtr<ContainerModalComponent>(context);
    component->SetChild(child);
    component->BuildInnerChild();
    return component;
}

RefPtr<Element> ContainerModalComponent::CreateElement()
{
    return AceType::MakeRefPtr<ContainerModalElement>();
}

RefPtr<RenderNode> ContainerModalComponent::CreateRenderNode()
{
    return RenderContainerModal::Create();
}

RefPtr<Component> ContainerModalComponent::BuildTitle()
{
    // build title box
    auto titleBox = AceType::MakeRefPtr<BoxComponent>();
    titleBox->SetHeight(CONTAINER_TITLE_HEIGHT);
    auto titleChildrenRow =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, BuildTitleChildren(false));

    // handle mouse move
    titleBox->SetOnMouseId([contextWptr = context_](MouseInfo& info) {
        auto context = contextWptr.Upgrade();
        if (context && info.GetButton() == MouseButton::LEFT_BUTTON && info.GetAction() == MouseAction::PRESS) {
            context->FireWindowStartMoveCallBack();
        }
    });

    // handle touch move
    titleBox->SetOnTouchMoveId([contextWptr = context_](const TouchEventInfo&) {
        auto context = contextWptr.Upgrade();
        if (context) {
            context->FireWindowStartMoveCallBack();
        }
    });
    titleBox->SetChild(titleChildrenRow);
    auto display = AceType::MakeRefPtr<DisplayComponent>(titleBox);
    return display;
}

RefPtr<Component> ContainerModalComponent::BuildFloatingTitle()
{
    // build floating title box
    auto titleDecoration = AceType::MakeRefPtr<Decoration>();
    titleDecoration->SetBackgroundColor(CONTAINER_BACKGROUND_COLOR);

    auto titleBox = AceType::MakeRefPtr<BoxComponent>();
    titleBox->SetHeight(CONTAINER_TITLE_HEIGHT);
    titleBox->SetBackDecoration(titleDecoration);

    auto floatingTitleChildrenRow =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, BuildTitleChildren(true));
    titleBox->SetChild(floatingTitleChildrenRow);
    auto tween = AceType::MakeRefPtr<TweenComponent>("ContainerModal", titleBox);
    return tween;
}

RefPtr<Component> ContainerModalComponent::BuildContent()
{
    auto contentBox = AceType::MakeRefPtr<BoxComponent>();
    contentBox->SetChild(GetChild());
    auto contentDecoration = AceType::MakeRefPtr<Decoration>();
    contentDecoration->SetBackgroundColor(CONTENT_BACKGROUND_COLOR);
    contentBox->SetBackDecoration(contentDecoration);

    auto clip = AceType::MakeRefPtr<ClipComponent>(contentBox);
    clip->SetClipRadius(Radius(CONTAINER_INNER_RADIUS));
    clip->SetFlexWeight(1.0);
    return clip;
}

RefPtr<ButtonComponent> ContainerModalComponent::BuildControlButton(
    InternalResource::ResourceId icon, std::function<void()>&& clickCallback, bool isFocus)
{
    auto image = AceType::MakeRefPtr<ImageComponent>(icon);
    image->SetWidth(TITLE_ICON_SIZE);
    image->SetHeight(TITLE_ICON_SIZE);
    std::list<RefPtr<Component>> btnChildren;
    btnChildren.emplace_back(image);

    auto button = AceType::MakeRefPtr<ButtonComponent>(btnChildren);
    button->SetWidth(TITLE_BUTTON_SIZE);
    button->SetHeight(TITLE_BUTTON_SIZE);
    button->SetType(ButtonType::CIRCLE);
    button->SetBackgroundColor(isFocus ? TITLE_BUTTON_BACKGROUND_COLOR : TITLE_BUTTON_BACKGROUND_COLOR_LOST_FOCUS);
    button->SetClickedColor(TITLE_BUTTON_CLICKED_COLOR);
    button->SetClickFunction(std::move(clickCallback));
    return button;
}

RefPtr<Component> ContainerModalComponent::SetPadding(
    const RefPtr<Component>& component, const Dimension& leftPadding, const Dimension& rightPadding)
{
    auto paddingComponent = AceType::MakeRefPtr<PaddingComponent>();
    paddingComponent->SetPaddingLeft(leftPadding);
    paddingComponent->SetPaddingRight(rightPadding);
    paddingComponent->SetPaddingTop((CONTAINER_TITLE_HEIGHT - TITLE_BUTTON_SIZE) / 2);
    paddingComponent->SetPaddingBottom((CONTAINER_TITLE_HEIGHT - TITLE_BUTTON_SIZE) / 2);
    paddingComponent->SetChild(component);
    return paddingComponent;
}

// Build ContainerModal FA structure
void ContainerModalComponent::BuildInnerChild()
{
    Border outerBorder;
    outerBorder.SetBorderRadius(Radius(CONTAINER_OUTER_RADIUS));
    outerBorder.SetColor(CONTAINER_BORDER_COLOR);
    outerBorder.SetWidth(CONTAINER_BORDER_WIDTH);
    auto containerDecoration = AceType::MakeRefPtr<Decoration>();
    containerDecoration->SetBackgroundColor(CONTAINER_BACKGROUND_COLOR);
    containerDecoration->SetBorder(outerBorder);

    auto column =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    column->AppendChild(BuildTitle());
    column->AppendChild(BuildContent());
    std::list<RefPtr<Component>> stackChildren;
    stackChildren.emplace_back(column);
    stackChildren.emplace_back(BuildFloatingTitle());
    auto stackComponent = AceType::MakeRefPtr<StackComponent>(
        Alignment::TOP_LEFT, StackFit::INHERIT, Overflow::OBSERVABLE, stackChildren);

    auto containerBox = AceType::MakeRefPtr<BoxComponent>();
    containerBox->SetBackDecoration(containerDecoration);
    containerBox->SetFlex(BoxFlex::FLEX_X);
    containerBox->SetAlignment(Alignment::CENTER);

    Edge padding = Edge(CONTENT_PADDING, Dimension(0.0), CONTENT_PADDING, CONTENT_PADDING);
    containerBox->SetPadding(padding);
    containerBox->SetChild(stackComponent);
    SetChild(containerBox);
}

std::list<RefPtr<Component>> ContainerModalComponent::BuildTitleChildren(bool isFloating, bool isFocus)
{
    // title icon
    if (!titleIcon_) {
        titleIcon_ = AceType::MakeRefPtr<ImageComponent>();
        titleIcon_->SetWidth(TITLE_ICON_SIZE);
        titleIcon_->SetHeight(TITLE_ICON_SIZE);
    }

    // title text
    if (!titleLabel_) {
        titleLabel_ = AceType::MakeRefPtr<TextComponent>("");
    }
    TextStyle style;
    style.SetFontSize(TITLE_TEXT_FONT_SIZE);
    style.SetTextColor(isFocus ? TITLE_TEXT_COLOR : TITLE_TEXT_COLOR_LOST_FOCUS);
    style.SetFontWeight(FontWeight::W500);
    style.SetAllowScale(false);
    style.SetTextOverflow(TextOverflow::ELLIPSIS);
    titleLabel_->SetTextStyle(style);
    titleLabel_->SetFlexWeight(1.0);

    // title control button
    auto contextWptr = context_;
    auto leftSplitButton = isFocus ? InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_SPLIT_LEFT
                                   : InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_SPLIT_LEFT;
    auto titleLeftSplitButton = BuildControlButton(leftSplitButton, [contextWptr]() {
            LOGI("left split button clicked");
            auto context = contextWptr.Upgrade();
            if (context) {
                context->FireWindowSplitCallBack();
            }
        }, isFocus);
    auto maxRecoverButton = isFloating ? InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_RECOVER
                                       : InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_MAXIMIZE;
    if (!isFocus) {
        maxRecoverButton = isFloating ? InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_RECOVER
                                      : InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_MAXIMIZE;
    }
    auto titleMaximizeRecoverButton = BuildControlButton(maxRecoverButton, [contextWptr]() {
            auto context = contextWptr.Upgrade();
            if (context) {
                auto mode = context->FireWindowGetModeCallBack();
                if (mode == WindowMode::WINDOW_MODE_FULLSCREEN) {
                    LOGI("recover button clicked");
                    context->FireWindowRecoverCallBack();
                } else {
                    LOGI("maximize button clicked");
                    context->FireWindowMaximizeCallBack();
                }
            }
        }, isFocus);
    auto minimizeButton = isFocus ? InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_MINIMIZE
                                  : InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_MINIMIZE;
    auto titleMinimizeButton = BuildControlButton(minimizeButton, [contextWptr]() {
            auto context = contextWptr.Upgrade();
            if (context) {
                LOGI("minimize button clicked");
                context->FireWindowMinimizeCallBack();
            }
        }, isFocus);
    auto closeButton = isFocus ? InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_CLOSE
                               : InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_CLOSE;
    auto titleCloseButton = BuildControlButton(closeButton, [contextWptr]() {
            auto context = contextWptr.Upgrade();
            if (context) {
                LOGI("close button clicked");
                context->FireWindowCloseCallBack();
            }
        }, isFocus);
    std::list<RefPtr<Component>> titleChildren;
    titleChildren.emplace_back(SetPadding(titleIcon_, TITLE_PADDING_START, TITLE_ELEMENT_MARGIN_HORIZONTAL));
    titleChildren.emplace_back(titleLabel_);
    titleChildren.emplace_back(SetPadding(titleLeftSplitButton, ZERO_PADDING, TITLE_ELEMENT_MARGIN_HORIZONTAL));
    titleChildren.emplace_back(SetPadding(titleMaximizeRecoverButton, ZERO_PADDING, TITLE_ELEMENT_MARGIN_HORIZONTAL));
    titleChildren.emplace_back(SetPadding(titleMinimizeButton, ZERO_PADDING, TITLE_ELEMENT_MARGIN_HORIZONTAL));
    titleChildren.emplace_back(SetPadding(titleCloseButton, ZERO_PADDING, TITLE_PADDING_END));
    return titleChildren;
}

} // namespace OHOS::Ace