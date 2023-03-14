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

#include "core/components/container_modal/container_modal_element.h"

#include "core/components/box/box_element.h"
#include "core/components/clip/clip_element.h"
#include "core/components/clip/render_clip.h"
#include "core/components/container_modal/container_modal_constants.h"
#include "core/components/container_modal/render_container_modal.h"
#include "core/components/flex/flex_element.h"
#include "core/components/padding/render_padding.h"
#include "core/gestures/tap_gesture.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t COLUMN_CHILD_NUM = 2;
constexpr uint32_t SPLIT_BUTTON_POSITION = 2;
constexpr uint32_t BLUR_WINDOW_RADIUS = 100;
constexpr uint32_t TITLE_POPUP_TIME = 200;        // 200ms
constexpr double MOUSE_MOVE_POPUP_DISTANCE = 5.0; // 5.0px
constexpr double MOVE_POPUP_DISTANCE_X = 10.0;    // 10.0px
constexpr double MOVE_POPUP_DISTANCE_Y = 20.0;    // 20.0px
constexpr double TITLE_POPUP_DISTANCE = 37.0;     // 37vp height of title

} // namespace

RefPtr<StackElement> ContainerModalElement::GetStackElement() const
{
    auto containerBox = AceType::DynamicCast<BoxElement>(GetFirstChild());
    if (!containerBox) {
        LOGE("Get stack element failed, container box element is null!");
        return {};
    }

    // The first stack is not what we need.
    auto stackElement = AceType::DynamicCast<StackElement>(containerBox->GetFirstChild());
    if (!stackElement) {
        LOGE("Get stack element failed, stack element is null!");
        return {};
    }

    auto column = AceType::DynamicCast<ColumnElement>(stackElement->GetFirstChild());
    if (!column || column->GetChildren().size() != COLUMN_CHILD_NUM) {
        // column should have 2 children, title and content.
        LOGE("Get stack element failed, column is null or child size error!");
        return {};
    }

    // Get second child : content
    auto clip = AceType::DynamicCast<ClipElement>(column->GetLastChild());
    if (!clip) {
        LOGE("Get stack element failed, clip element is null!");
        return {};
    }

    auto contentBox = AceType::DynamicCast<BoxElement>(clip->GetFirstChild());
    if (!contentBox) {
        LOGE("Get stack element failed, content box element is null!");
        return {};
    }

    auto stack = contentBox->GetFirstChild();
    if (!stack || !AceType::InstanceOf<StackElement>(stack)) {
        LOGE("Get stack element failed, stack is null or type error!");
        return {};
    }

    return AceType::DynamicCast<StackElement>(stack);
}

RefPtr<OverlayElement> ContainerModalElement::GetOverlayElement() const
{
    auto stack = GetStackElement();
    if (!stack) {
        LOGE("Get overlay element failed, stack element is null");
        return {};
    }

    for (const auto& child : stack->GetChildren()) {
        if (child && AceType::InstanceOf<OverlayElement>(child)) {
            return AceType::DynamicCast<OverlayElement>(child);
        }
    }
    LOGE("Get overlay element failed, all children of stack element do not meet the requirements");
    return {};
}

RefPtr<StageElement> ContainerModalElement::GetStageElement() const
{
    auto stack = GetStackElement();
    if (!stack) {
        LOGE("Get stage element failed, stack element is null");
        return {};
    }
    for (const auto& child : stack->GetChildren()) {
        if (child && AceType::InstanceOf<StageElement>(child)) {
            return AceType::DynamicCast<StageElement>(child);
        }
    }
    LOGE("Get stage element failed, all children of stack element do not meet the requirements");
    return {};
}

void ContainerModalElement::ShowTitle(bool isShow)
{
    auto containerBox = AceType::DynamicCast<BoxElement>(GetFirstChild());
    if (!containerBox) {
        LOGE("ContainerModalElement showTitle failed, container box element is null!");
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("ContainerModalElement showTitle failed, context is null.");
        return;
    }
    windowMode_ = context->FireWindowGetModeCallBack();

    // full screen need to hide border and padding.
    auto containerRenderBox = AceType::DynamicCast<RenderBox>(containerBox->GetRenderNode());
    if (containerRenderBox) {
        auto containerDecoration = AceType::MakeRefPtr<Decoration>();
        Edge padding = Edge();
        if (isShow) {
            Border outerBorder;
            outerBorder.SetBorderRadius(Radius(CONTAINER_OUTER_RADIUS));
            outerBorder.SetColor(CONTAINER_BORDER_COLOR);
            outerBorder.SetWidth(CONTAINER_BORDER_WIDTH);
            containerDecoration->SetBackgroundColor(CONTAINER_BACKGROUND_COLOR);
            containerDecoration->SetBorder(outerBorder);
            padding = Edge(CONTENT_PADDING, Dimension(0.0), CONTENT_PADDING, CONTENT_PADDING);
        }
        containerRenderBox->SetBackDecoration(containerDecoration);
        containerRenderBox->SetPadding(padding);
    }

    auto stackElement = AceType::DynamicCast<StackElement>(containerBox->GetFirstChild());
    if (!stackElement) {
        LOGE("ContainerModalElement showTitle failed, stack element is null!");
        return;
    }

    auto column = AceType::DynamicCast<ColumnElement>(stackElement->GetFirstChild());
    if (!column || column->GetChildren().size() != COLUMN_CHILD_NUM) {
        // column should have 2 children, title and content.
        LOGE("ContainerModalElement showTitle failed, column  element is null or children size error!");
        return;
    }

    // full screen need to hide content border radius.
    auto clip = AceType::DynamicCast<ClipElement>(column->GetLastChild());
    if (!clip) {
        LOGE("ContainerModalElement showTitle failed, clip element is null!");
        return;
    }
    auto renderClip = AceType::DynamicCast<RenderClip>(clip->GetRenderNode());
    if (renderClip) {
        isShow ? renderClip->SetClipRadius(Radius(CONTAINER_INNER_RADIUS)) : renderClip->SetClipRadius(Radius(0.0));
    }

    // Get first child : title
    auto display = AceType::DynamicCast<DisplayElement>(column->GetFirstChild());
    if (!display) {
        LOGE("ContainerModalElement showTitle failed,, display element is null.");
        return;
    }
    auto renderDisplay = AceType::DynamicCast<RenderDisplay>(display->GetRenderNode());
    if (renderDisplay) {
        renderDisplay->UpdateVisibleType(isShow ? VisibleType::VISIBLE : VisibleType::GONE);
    }
    ChangeFloatingTitleIcon();

    // hide floating title anyway.
    if (floatingTitleDisplay_) {
        floatingTitleDisplay_->UpdateVisibleType(VisibleType::GONE);
    }
}

void ContainerModalElement::PerformBuild()
{
    SoleChildElement::PerformBuild();
    if (!controller_) {
        controller_ = AceType::MakeRefPtr<Animator>(context_);
        controller_->SetDuration(TITLE_POPUP_TIME);
        controller_->SetFillMode(FillMode::FORWARDS);
        auto translateY = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
            DimensionOffset(Dimension(), Dimension(-TITLE_POPUP_DISTANCE * density_)),
            DimensionOffset(Dimension(), Dimension()), Curves::FRICTION);
        TweenOption option;
        option.SetTranslateAnimations(AnimationType::TRANSLATE_Y, translateY);
        auto containerBox = AceType::DynamicCast<BoxElement>(GetFirstChild());
        if (!containerBox) {
            LOGE("ContainerModalElement PerformBuild failed, container box element is null!");
            return;
        }

        auto stackElement = AceType::DynamicCast<StackElement>(containerBox->GetFirstChild());
        if (!stackElement) {
            LOGE("ContainerModalElement PerformBuild failed, stack element is null!");
            return;
        }
        auto column = AceType::DynamicCast<ColumnElement>(stackElement->GetFirstChild());
        if (!column || column->GetChildren().size() != COLUMN_CHILD_NUM) {
            // column should have 2 children, title and content.
            LOGE("ContainerModalElement PerformBuild failed, column  element is null or children size error!");
            return;
        }

        auto titleDisplay = AceType::DynamicCast<DisplayElement>(column->GetFirstChild());
        if (titleDisplay) {
            titleBox_ = AceType::DynamicCast<BoxElement>(titleDisplay->GetFirstChild());
        }

        auto tween = AceType::DynamicCast<TweenElement>(stackElement->GetLastChild());
        if (!tween) {
            LOGE("ContainerModalElement PerformBuild failed, tween element is null.");
            return;
        }
        floatingTitleBox_ = AceType::DynamicCast<BoxElement>(tween->GetContentElement());
        auto display = AceType::DynamicCast<DisplayElement>(tween->GetFirstChild());
        if (display && !floatingTitleDisplay_) {
            floatingTitleDisplay_ = AceType::DynamicCast<RenderDisplay>(display->GetRenderNode());
            if (floatingTitleDisplay_) {
                floatingTitleDisplay_->UpdateVisibleType(VisibleType::GONE);
            }
        }
        tween->SetController(controller_);
        tween->SetOption(option);
        tween->ApplyKeyframes();
    }

    // The first time it starts up, it needs to hide title if mode as follows.
    windowMode_ = context_.Upgrade()->FireWindowGetModeCallBack();
    if (windowMode_ == WindowMode::WINDOW_MODE_FULLSCREEN || windowMode_ == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
        windowMode_ == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        ShowTitle(false);
    }
}

void ContainerModalElement::Update()
{
    RenderElement::Update();

    containerModalComponent_ = AceType::DynamicCast<ContainerModalComponent>(component_);
    if (!containerModalComponent_) {
        LOGE("ContainerModalElement update failed, container modal component is null.");
        return;
    }
    auto containerBox = AceType::DynamicCast<BoxComponent>(containerModalComponent_->GetChild());
    if (!containerBox) {
        LOGE("ContainerModalElement update failed, container box component is null.");
        return;
    }
    auto context = context_.Upgrade();
    if (context) {
        density_ = (float)context->GetDensity();
    }

    containerBox->SetOnTouchDownId([week = WeakClaim(this)](const TouchEventInfo& info) {
        auto containerElement = week.Upgrade();
        if (containerElement && info.GetChangedTouches().begin()->GetGlobalLocation().GetY() <=
                                    (TITLE_POPUP_DISTANCE * containerElement->density_)) {
            containerElement->moveX_ = (float)info.GetChangedTouches().begin()->GetGlobalLocation().GetX();
            containerElement->moveY_ = (float)info.GetChangedTouches().begin()->GetGlobalLocation().GetY();
        }
    });

    // touch top to pop-up title bar.
    containerBox->SetOnTouchMoveId([week = WeakClaim(this)](const TouchEventInfo& info) {
        auto containerElement = week.Upgrade();
        if (!containerElement || !containerElement->CanShowFloatingTitle()) {
            return;
        }
        if (info.GetChangedTouches().begin()->GetGlobalLocation().GetY() >
            (TITLE_POPUP_DISTANCE * containerElement->density_)) {
            return;
        }
        auto deltaMoveX = fabs(info.GetChangedTouches().begin()->GetGlobalLocation().GetX() - containerElement->moveX_);
        auto deltaMoveY = info.GetChangedTouches().begin()->GetGlobalLocation().GetY() - containerElement->moveY_;
        if (deltaMoveX <= MOVE_POPUP_DISTANCE_X && deltaMoveY >= MOVE_POPUP_DISTANCE_Y) {
            containerElement->floatingTitleDisplay_->UpdateVisibleType(VisibleType::VISIBLE);
            containerElement->controller_->ClearStopListeners();
            containerElement->controller_->Forward();
        }
    });

    // mouse move top to pop up title bar and move other area to hide title bar.
    containerBox->SetOnMouseId([weak = WeakClaim(this)](MouseInfo& info) {
        auto containerElement = weak.Upgrade();
        if (!containerElement || info.GetAction() != MouseAction::MOVE) {
            return;
        }
        if (info.GetLocalLocation().GetY() <= MOUSE_MOVE_POPUP_DISTANCE && containerElement->CanShowFloatingTitle()) {
            containerElement->floatingTitleDisplay_->UpdateVisibleType(VisibleType::VISIBLE);
            containerElement->controller_->ClearStopListeners();
            containerElement->controller_->Forward();
        }
        if (info.GetLocalLocation().GetY() > (TITLE_POPUP_DISTANCE * containerElement->density_) &&
            containerElement->CanHideFloatingTitle()) {
            containerElement->controller_->AddStopListener([weak] {
                auto container = weak.Upgrade();
                container->floatingTitleDisplay_->UpdateVisibleType(VisibleType::GONE);
            });
            containerElement->controller_->Backward();
        }
    });

    // touch other area of screen to hide title bar.
    auto tapGesture = AceType::MakeRefPtr<TapGesture>();
    tapGesture->SetOnActionId([weak = WeakClaim(this)](GestureEvent& info) {
        auto containerElement = weak.Upgrade();
        if (!containerElement || !containerElement->CanHideFloatingTitle()) {
            return;
        }
        containerElement->controller_->AddStopListener([weak] {
            auto container = weak.Upgrade();
            container->floatingTitleDisplay_->UpdateVisibleType(VisibleType::GONE);
        });
        containerElement->controller_->Backward();
    });
    containerBox->SetOnClick(tapGesture);
}

bool ContainerModalElement::CanShowFloatingTitle()
{
    if (!floatingTitleDisplay_ || !controller_) {
        LOGI("Show floating title failed, floatingTitleDisplay or controller is null.");
        return false;
    }
    if (windowMode_ != WindowMode::WINDOW_MODE_FULLSCREEN && windowMode_ != WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
        windowMode_ != WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        LOGI("Window is not full screen or split screen, can not show floating title.");
        return false;
    }
    if (floatingTitleDisplay_->GetVisible()) {
        LOGI("Floating tittle is visible now, no need to show again.");
        return false;
    }
    return true;
}

bool ContainerModalElement::CanHideFloatingTitle()
{
    if (!floatingTitleDisplay_ || !controller_) {
        LOGI("Hide floating title failed, floatingTitleDisplay or controller is null.");
        return false;
    }
    if (!floatingTitleDisplay_->GetVisible()) {
        LOGI("Hide floating title failed, title is not visible.");
        return false;
    }
    return true;
}

void ContainerModalElement::ChangeFloatingTitleIcon(bool isFocus)
{
    if (!floatingTitleBox_ || !containerModalComponent_) {
        LOGE("ChangeFloatingTitleIcon failed.");
        return;
    }
    auto renderFloatingTitleBox = AceType::DynamicCast<RenderBox>(floatingTitleBox_->GetRenderNode());
    if (!renderFloatingTitleBox) {
        LOGE("ChangeFloatingTitleIcon failed, render floating title box is null.");
        return;
    }
    auto backDecoration = renderFloatingTitleBox->GetBackDecoration();
    backDecoration->SetBackgroundColor(isFocus ? CONTAINER_BACKGROUND_COLOR : CONTAINER_BACKGROUND_COLOR_LOST_FOCUS);
    renderFloatingTitleBox->SetBackDecoration(backDecoration);

    auto rowElement = AceType::DynamicCast<RowElement>(floatingTitleBox_->GetFirstChild());
    if (!rowElement) {
        LOGE("ChangeFloatingTitleIcon failed, row element is null.");
        return;
    }
    auto renderRow = AceType::DynamicCast<RenderFlex>(rowElement->GetRenderNode());
    if (!renderRow) {
        LOGE("ChangeFloatingTitleIcon failed, renderRow is null.");
        return;
    }
    auto iterator = renderRow->GetChildren().begin();
    std::advance(iterator, SPLIT_BUTTON_POSITION);
    auto splitButton = AceType::DynamicCast<RenderPadding>(*iterator);

    if (windowMode_ == WindowMode::WINDOW_MODE_FULLSCREEN) {
        auto floatingTitleChildrenRow = AceType::MakeRefPtr<RowComponent>(
            FlexAlign::FLEX_START, FlexAlign::CENTER, containerModalComponent_->BuildTitleChildren(true, isFocus));
        rowElement->SetUpdateComponent(floatingTitleChildrenRow);
        splitButton->SetHidden(false);
    } else {
        auto titleChildrenRow = AceType::MakeRefPtr<RowComponent>(
            FlexAlign::FLEX_START, FlexAlign::CENTER, containerModalComponent_->BuildTitleChildren(false, isFocus));
        rowElement->SetUpdateComponent(titleChildrenRow);
        splitButton->SetHidden(true);
    }
}

void ContainerModalElement::ChangeTitleIcon(bool isFocus)
{
    if (!titleBox_ || !containerModalComponent_) {
        LOGE("ChangeTitleIcon failed.");
        return;
    }
    auto rowElement = AceType::DynamicCast<RowElement>(titleBox_->GetFirstChild());
    if (!rowElement) {
        LOGE("ChangeTitleIcon failed, row element is null.");
        return;
    }
    auto titleChildrenRow = AceType::MakeRefPtr<RowComponent>(
        FlexAlign::FLEX_START, FlexAlign::CENTER, containerModalComponent_->BuildTitleChildren(false, isFocus));
    rowElement->SetUpdateComponent(titleChildrenRow);
}

void ContainerModalElement::BlurWindow(bool isBlur)
{
    auto containerBox = AceType::DynamicCast<BoxElement>(GetFirstChild());
    if (!containerBox) {
        LOGE("ContainerModalElement BlurWindow failed, container box element is null!");
        return;
    }
    auto containerRenderBox = AceType::DynamicCast<RenderBox>(containerBox->GetRenderNode());
    if (!containerRenderBox) {
        LOGE("ContainerModalElement BlurWindow failed, container box render is null!");
        return;
    }
    if (isBlur) {
        auto frontDecoration = AceType::MakeRefPtr<Decoration>();
        frontDecoration->SetBlurRadius(Dimension(BLUR_WINDOW_RADIUS));
        containerRenderBox->SetFrontDecoration(frontDecoration);
    } else {
        containerRenderBox->SetFrontDecoration(nullptr);
    }
}

void ContainerModalElement::WindowFocus(bool isFocus)
{
    auto containerBox = AceType::DynamicCast<BoxElement>(GetFirstChild());
    if (!containerBox) {
        LOGE("ContainerModalElement WindowFocus failed, container box element is null!");
        return;
    }

    auto containerRenderBox = AceType::DynamicCast<RenderBox>(containerBox->GetRenderNode());
    if (containerRenderBox) {
        auto containerDecoration = containerRenderBox->GetBackDecoration();
        containerDecoration->SetBackgroundColor(
            isFocus ? CONTAINER_BACKGROUND_COLOR : CONTAINER_BACKGROUND_COLOR_LOST_FOCUS);
        auto border = containerDecoration->GetBorder();
        border.SetColor(isFocus ? CONTAINER_BORDER_COLOR : CONTAINER_BORDER_COLOR_LOST_FOCUS);
        containerDecoration->SetBorder(border);
        containerRenderBox->SetBackDecoration(containerDecoration);
    }
    if (windowMode_ == WindowMode::WINDOW_MODE_FULLSCREEN || windowMode_ == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
        windowMode_ == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        ChangeFloatingTitleIcon(isFocus);
    } else {
        ChangeTitleIcon(isFocus);
    }
}

} // namespace OHOS::Ace