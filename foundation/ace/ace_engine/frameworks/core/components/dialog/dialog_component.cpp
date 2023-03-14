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

#include "core/components/dialog/dialog_component.h"

#include <atomic>

#include "base/geometry/dimension.h"
#include "base/i18n/localization.h"
#include "base/log/log.h"
#include "base/resource/internal_resource.h"
#include "base/utils/system_properties.h"
#include "core/animation/curve_animation.h"
#include "core/animation/curves.h"
#include "core/animation/keyframe.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/box/box_component.h"
#include "core/components/button/button_theme.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/dialog/action_sheet/action_sheet_component.h"
#include "core/components/dialog/alert_dialog_component.h"
#include "core/components/dialog/dialog_element.h"
#include "core/components/dialog/render_dialog.h"
#include "core/components/drag_bar/drag_bar_component.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/focusable/focusable_component.h"
#include "core/components/image/image_component.h"
#include "core/components/scroll/scroll_component.h"
#include "core/components/theme/theme_manager.h"
#include "core/components/wrap/wrap_component.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {
namespace {

constexpr double PHONE_ENTER_CURVE_X0 = 0.38;
constexpr double PHONE_ENTER_CURVE_Y0 = 1.33;
constexpr double PHONE_ENTER_CURVE_X1 = 0.60;
constexpr double PHONE_ENTER_CURVE_Y1 = 1.0;
constexpr double PHONE_OPACITY_MIDDLE_IN = 0.375;
constexpr Dimension CAR_TITLE_MIN_HEIGHT = 64.0_vp;

} // namespace

static std::atomic<int32_t> g_dialogId(0);

const char CALLBACK_SUCCESS[] = "success";
const char CALLBACK_CANCEL[] = "cancel";
const char CALLBACK_COMPLETE[] = "complete";
const char DIALOG_TWEEN_NAME[] = "tween";
const int32_t DIALOG_BUTTONS_COUNT_WATCH = 2;
const char DIALOG_OK[] = "common.ok";
const char DIALOG_CANCEL[] = "common.cancel";
const char SEPARATE[] = " ";

DialogComponent::DialogComponent()
{
    dialogId_ = GenerateDialogId();
}

RefPtr<Element> DialogComponent::CreateElement()
{
    return AceType::MakeRefPtr<DialogElement>();
}

RefPtr<RenderNode> DialogComponent::CreateRenderNode()
{
    return RenderDialog::Create();
}

int32_t DialogComponent::GenerateDialogId()
{
    return g_dialogId.fetch_add(1, std::memory_order_relaxed);
}

void DialogComponent::BuildChild(const RefPtr<ThemeManager>& themeManager)
{
    if (!themeManager) {
        return;
    }
    dialogTheme_ = AceType::DynamicCast<DialogTheme>(themeManager->GetTheme(DialogTheme::TypeId()));
    if (!dialogTheme_) {
        return;
    }
    if (!isDeviceTypeSet_) {
        deviceType_ = SystemProperties::GetDeviceType();
    }
    bool isLimit = true;
    auto box = BuildBox(isLimit);
    auto transition = BuildAnimation(box);
    BuildDialogTween(transition, isLimit, margin_);

    auto focusCollaboration = AceType::MakeRefPtr<FocusCollaborationComponent>();
    if (!HasCustomChild()) {
        std::list<RefPtr<Component>> columnChildren;
        auto column = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, columnChildren);
        column->SetMainAxisSize(MainAxisSize::MIN);
        BuildTitle(column);
        BuildContent(column);
        if (isMenu_) {
            BuildMenu(column);
        } else {
            BuildActions(themeManager, column);
        }
        BuildFocusChild(column, focusCollaboration);
    } else {
        // build custom child
        BuildFocusChild(customComponent_, focusCollaboration);
        if (IsDragable()) {
            BuildDragBar(focusCollaboration);
        }
    }
    if (deviceType_ == DeviceType::WATCH) {
        auto scroll = AceType::MakeRefPtr<ScrollComponent>(focusCollaboration);
        box->SetChild(scroll);
    } else {
        box->SetChild(focusCollaboration);
    }
    box->SetTextDirection(GetTextDirection());
}

RefPtr<BoxComponent> DialogComponent::BuildBox(bool& isLimit)
{
    auto box = AceType::MakeRefPtr<BoxComponent>();
    // If use custom style, don't set default style.
    if (properties_.customStyle) {
        isLimit = false;
        return box;
    }

    auto backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetBackgroundColor(backgroundColor_);
    Border border;
    border.SetBorderRadius(dialogTheme_->GetRadius());
    backDecoration->SetBorder(border);

    if (deviceType_ == DeviceType::WATCH) {
        box->SetFlex(BoxFlex::FLEX_XY);
    } else {
        box->SetFlex(BoxFlex::FLEX_X);
    }
    box->SetBackDecoration(backDecoration);
    if (height_.IsValid()) {
        box->SetHeight(height_.Value(), height_.Unit());
        isLimit = false;
    }
    if (width_.IsValid()) {
        box->SetWidth(width_.Value(), width_.Unit());
        isLimit = false;
    }
    if (isSetMargin_) {
        box->SetMargin(margin_);
    }
    return box;
}

void DialogComponent::BuildDialogTween(const RefPtr<TransitionComponent>& transition, bool isLimit, Edge margin)
{
    auto dialogTween = AceType::MakeRefPtr<DialogTweenComponent>();
    auto controller = AceType::MakeRefPtr<Animator>(context_);
    dialogTween->SetAnimator(controller);
    if (animator_) {
        animator_->AddProxyController(controller);
    }
    dialogTween->SetParentAnimator(animator_);
    dialogTween->SetAutoCancel(autoCancel_);
    dialogTween->SetChild(transition);
    dialogTween->SetTextDirection(GetTextDirection());
    dialogTween->SetOnSuccessId(onSuccessId_);
    dialogTween->SetOnCancelId(onCancelId_);
    dialogTween->SetOnCompleteId(onCompleteId_);
    dialogTween->SetOnStatusChanged(properties_.onStatusChanged);
    dialogTween->SetDialogId(dialogId_);
    if (isMenu_) {
        dialogTween->SetIsMenu(true);
        dialogTween->SetMenuSuccessId(menuSuccessId_);
    }
    dialogTween->SetOnPositiveSuccessId(onPositiveSuccessId_);
    dialogTween->SetOnNegativeSuccessId(onNegativeSuccessId_);
    dialogTween->SetOnNeutralSuccessId(onNeutralSuccessId_);
    dialogTween->SetData(data_);
    dialogTween->SetDialogLimit(isLimit);
    dialogTween->SetDragable(dragable_);
    if (isSetMargin_) {
        dialogTween->SetMargin(margin);
    }
    dialogTween->SetAlignment(properties_.alignment);
    dialogTween->SetOffset(properties_.offset);
    dialogTween->SetGridCount(properties_.gridCount);
    if (dialogTweenBox_) {
        const auto& dialogComposed = GenerateComposed("dialog", dialogTween, false);
        dialogTween->SetComposedId(dialogTweenComposedId_);
        dialogTween->SetCustomDialogId(customDialogId_);
        dialogTweenBox_->SetChild(dialogComposed);
    }
}

void DialogComponent::BuildFocusChild(
    const RefPtr<Component>& child, const RefPtr<FocusCollaborationComponent>& collaboration)
{
    if (HasCustomChild()) {
        // for custom child
        collaboration->InsertChild(0, child);
    } else if (actions_.empty()) {
        auto focusable = AceType::MakeRefPtr<FocusableComponent>(child);
        focusable->SetFocusable(true);
        focusable->SetFocusNode(true);
        collaboration->InsertChild(0, focusable);
    } else {
        collaboration->InsertChild(0, child);
    }
}

void DialogComponent::BuildDragBar(const RefPtr<FocusCollaborationComponent>& collaboration)
{
    auto dragBar = AceType::MakeRefPtr<DragBarComponent>();
    auto boxForContent = AceType::MakeRefPtr<BoxComponent>();
    boxForContent->SetFlex(BoxFlex::FLEX_X);
    auto column =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    column->SetCrossAxisSize(CrossAxisSize::MAX);
    auto mode = PanelMode::HALF;
    dragBar->SetPanelMode(mode);
    dragBar->SetHasDragBar(dragable_);
    column->AppendChild(dragBar);
    collaboration->InsertChild(1, column);
}

void DialogComponent::BuildTitle(const RefPtr<ColumnComponent>& column)
{
    if (!title_) {
        return;
    }
    auto titlePadding = AceType::MakeRefPtr<PaddingComponent>();
    if (titlePadding_ == Edge::NONE) {
        titlePadding_ = (!content_ && actions_.empty()) ? dialogTheme_->GetTitleDefaultPadding()
                                                        : dialogTheme_->GetTitleAdjustPadding();
    }
    auto textBox = AceType::MakeRefPtr<BoxComponent>();
    textBox->SetDeliverMinToChild(false);
    textBox->SetChild(title_);
    titlePadding->SetPadding(std::move(titlePadding_));
    titlePadding->SetChild(textBox);
    std::list<RefPtr<Component>> rowChildren;
    RefPtr<RowComponent> row;
    if (deviceType_ == DeviceType::PHONE && !isMenu_) {
        row = AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, rowChildren);
    } else {
        row = AceType::MakeRefPtr<RowComponent>(FlexAlign::CENTER, FlexAlign::CENTER, rowChildren);
    }
    row->SetStretchToParent(true);
    if (SystemProperties::GetDeviceType() == DeviceType::CAR) {
        auto box = AceType::MakeRefPtr<BoxComponent>();
        box->SetMinHeight(CAR_TITLE_MIN_HEIGHT);
        box->SetChild(titlePadding);
        row->AppendChild(box);
    } else {
        row->AppendChild(titlePadding);
    }

    auto titleFlex = AceType::MakeRefPtr<FlexItemComponent>(0, 0, 0.0, row);
    column->AppendChild(GenerateComposed("dialogTitle", titleFlex, true));
}

void DialogComponent::BuildContent(const RefPtr<ColumnComponent>& column)
{
    if (!content_) {
        return;
    }
    auto contentPadding = AceType::MakeRefPtr<PaddingComponent>();
    if (contentPadding_ == Edge::NONE) {
        if (!title_) {
            contentPadding_ = actions_.empty() ? dialogTheme_->GetDefaultPadding() : dialogTheme_->GetAdjustPadding();
        } else {
            contentPadding_ =
                actions_.empty() ? dialogTheme_->GetContentDefaultPadding() : dialogTheme_->GetContentAdjustPadding();
        }
    }
    contentPadding->SetPadding(std::move(contentPadding_));
    RefPtr<FlexItemComponent> contentFlex;
    if (deviceType_ == DeviceType::WATCH) {
        contentPadding->SetChild(content_);
        contentFlex = AceType::MakeRefPtr<FlexItemComponent>(0, 0, 0.0, contentPadding);
    } else {
        auto scroll = AceType::MakeRefPtr<ScrollComponent>(content_);
        contentPadding->SetChild(scroll);
        contentFlex = AceType::MakeRefPtr<FlexItemComponent>(0, 1, 0.0, contentPadding);
    }
    column->AppendChild(GenerateComposed("dialogContent", contentFlex, true));
}

void DialogComponent::BuildMenu(const RefPtr<ColumnComponent>& column)
{
    if (actions_.empty()) {
        LOGW("the action is empty");
        return;
    }

    std::list<RefPtr<Component>> columnChildren;
    auto actionIter = actions_.begin();
    for (size_t index = 0; index < actions_.size(); ++index) {
        std::list<RefPtr<Component>> rowChildren;
        auto buttonRow = AceType::MakeRefPtr<RowComponent>(FlexAlign::CENTER, FlexAlign::CENTER, rowChildren);
        auto rowItem = AceType::MakeRefPtr<FlexItemComponent>(
            1, 1, 0.0, BuildButton(*actionIter, menuSuccessId_[index], Edge::NONE, false));
        buttonRow->AppendChild(rowItem);
        auto columnItem = AceType::MakeRefPtr<FlexItemComponent>(1, 1, 0.0, buttonRow);
        column->AppendChild(columnItem);
        ++actionIter;
    }
}

void DialogComponent::BuildActions(const RefPtr<ThemeManager>& themeManager, const RefPtr<ColumnComponent>& column)
{
    if (deviceType_ == DeviceType::WATCH) {
        BuildActionsForWatch(column);
        return;
    }

    if (actions_.empty()) {
        LOGW("the action is empty");
        return;
    }

    auto actionsPadding = AceType::MakeRefPtr<PaddingComponent>();
    actionsPadding->SetPadding(dialogTheme_->GetActionsPadding());
    if (actions_.size() == 1) { // the button in dialog is one.
        std::list<RefPtr<Component>> rowChildren;
        auto row = AceType::MakeRefPtr<RowComponent>(FlexAlign::SPACE_AROUND, FlexAlign::FLEX_START, rowChildren);
        row->SetStretchToParent(true);
        row->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(
            1, 1, 0.0, BuildButton(actions_.front(), onPositiveSuccessId_, Edge::NONE, true)));
        actionsPadding->SetChild(row);
    } else if (actions_.size() == 2) { // the button in dialog is two.
        std::list<RefPtr<Component>> rowChildren;
        auto row = AceType::MakeRefPtr<RowComponent>(FlexAlign::SPACE_AROUND, FlexAlign::CENTER, rowChildren);
        row->SetStretchToParent(true);
        row->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(
            1, 1, 0.0, BuildButton(actions_.front(), onPositiveSuccessId_, Edge::NONE)));
        row->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(0, 0, 0.0, BuildDivider(themeManager)));
        row->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(
            1, 1, 0.0, BuildButton(actions_.back(), onNegativeSuccessId_, Edge::NONE, true)));
        actionsPadding->SetChild(row);
    } else { // the button in dialog is more than two.
        std::list<RefPtr<Component>> wrapChildren;
        auto wrap = AceType::MakeRefPtr<WrapComponent>(wrapChildren);
        wrap->SetDialogStretch(true);
        wrap->SetMainAlignment(WrapAlignment::CENTER);
        wrap->SetSpacing(dialogTheme_->GetButtonSpacingHorizontal());
        wrap->SetContentSpacing(dialogTheme_->GetButtonSpacingVertical());
        int32_t num = 0;
        for (const auto& action : actions_) {
            ++num;
            if (num == 1) {
                wrap->AppendChild(BuildButton(action, onPositiveSuccessId_, Edge::NONE));
            } else if (num == 2) {
                wrap->AppendChild(BuildButton(action, onNegativeSuccessId_, Edge::NONE, true));
            } else if (num == 3) {
                wrap->AppendChild(BuildButton(action, onNeutralSuccessId_, Edge::NONE));
            } else {
                break;
            }
        }
        actionsPadding->SetChild(wrap);
    }
    auto actionsFlex = AceType::MakeRefPtr<FlexItemComponent>(0, 0, 0.0, actionsPadding);
    column->AppendChild(actionsFlex);
}

void DialogComponent::BuildActionsForWatch(const OHOS::Ace::RefPtr<OHOS::Ace::ColumnComponent>& column)
{
    if (actions_.empty() || actions_.size() != DIALOG_BUTTONS_COUNT_WATCH) {
        return;
    }

    std::list<RefPtr<Component>> rowChildren;
    auto row = AceType::MakeRefPtr<RowComponent>(FlexAlign::SPACE_BETWEEN, FlexAlign::FLEX_START, rowChildren);
    row->SetStretchToParent(true);
    row->AppendChild(BuildButton(actions_.front(), onPositiveSuccessId_, dialogTheme_->GetButtonPaddingRight()));
    row->AppendChild(BuildButton(actions_.back(), onNegativeSuccessId_, dialogTheme_->GetButtonPaddingLeft(), true));
    auto actionsPadding = AceType::MakeRefPtr<PaddingComponent>();
    actionsPadding->SetPadding(dialogTheme_->GetDefaultPadding());
    actionsPadding->SetChild(row);
    auto actionsFlex = AceType::MakeRefPtr<FlexItemComponent>(0, 0, 0.0, actionsPadding);
    column->AppendChild(actionsFlex);
}

RefPtr<Component> DialogComponent::BuildButton(
    const RefPtr<ButtonComponent>& button, const EventMarker& callbackId, const Edge& edge, bool isAutoFocus)
{
    button->SetClickedEventId(callbackId);
    button->SetAutoFocusState(isAutoFocus);
    auto buttonPadding = AceType::MakeRefPtr<PaddingComponent>();
    buttonPadding->SetPadding(edge);
    buttonPadding->SetChild(button);
    return GenerateComposed("dialogButton", buttonPadding, true);
}

RefPtr<TransitionComponent> DialogComponent::BuildAnimation(const RefPtr<BoxComponent>& child)
{
    if (deviceType_ == DeviceType::PHONE) {
        return BuildAnimationForPhone(child);
    }
    // Build scale animation for in.
    auto scaleFrameStart =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameStart(), dialogTheme_->GetScaleStart());
    auto scaleFrameEnd = AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameEnd(), dialogTheme_->GetScaleEnd());
    auto scaleAnimationIn = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    scaleAnimationIn->AddKeyframe(scaleFrameStart);
    scaleAnimationIn->AddKeyframe(scaleFrameEnd);
    scaleAnimationIn->SetCurve(Curves::FRICTION);
    // Build opacity animation for in.
    auto opacityKeyframeStart =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameStart(), dialogTheme_->GetOpacityStart());
    auto opacityKeyframeEnd =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameEnd(), dialogTheme_->GetOpacityEnd());
    auto opacityAnimationIn = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimationIn->AddKeyframe(opacityKeyframeStart);
    opacityAnimationIn->AddKeyframe(opacityKeyframeEnd);
    opacityAnimationIn->SetCurve(Curves::FRICTION);
    // Build tween option for in
    TweenOption tweenOptionIn;
    tweenOptionIn.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimationIn);
    tweenOptionIn.SetOpacityAnimation(opacityAnimationIn);
    tweenOptionIn.SetDuration(dialogTheme_->GetAnimationDurationIn());
    tweenOptionIn.SetFillMode(FillMode::FORWARDS);

    // Build scale animation for out.
    auto scaleFrameStartOut =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameStart(), dialogTheme_->GetScaleEnd());
    auto scaleFrameEndOut =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameEnd(), dialogTheme_->GetScaleStart());
    auto scaleAnimationOut = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    scaleAnimationOut->AddKeyframe(scaleFrameStartOut);
    scaleAnimationOut->AddKeyframe(scaleFrameEndOut);
    scaleAnimationOut->SetCurve(Curves::SMOOTH);
    // Build opacity animation for out.
    auto opacityKeyframeStartOut =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameStart(), dialogTheme_->GetOpacityEnd());
    auto opacityKeyframeEndOut =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameEnd(), dialogTheme_->GetOpacityStart());
    auto opacityAnimationOut = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimationOut->AddKeyframe(opacityKeyframeStartOut);
    opacityAnimationOut->AddKeyframe(opacityKeyframeEndOut);
    opacityAnimationOut->SetCurve(Curves::SMOOTH);
    // Build tween option for out
    TweenOption tweenOptionOut;
    tweenOptionOut.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimationOut);
    tweenOptionOut.SetOpacityAnimation(opacityAnimationOut);
    tweenOptionOut.SetDuration(dialogTheme_->GetAnimationDurationOut());
    tweenOptionOut.SetFillMode(FillMode::FORWARDS);

    // Build transition
    auto transition =
        AceType::MakeRefPtr<TransitionComponent>(TweenComponent::AllocTweenComponentId(), DIALOG_TWEEN_NAME, child);
    transition->SetIsFirstFrameShow(false);
    transition->SetTransitionOption(tweenOptionIn, tweenOptionOut);
    return transition;
}

RefPtr<TransitionComponent> DialogComponent::BuildAnimationForPhone(const RefPtr<Component>& child)
{
    // Build scale animation for in.
    auto scaleFrameStart =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameStart(), dialogTheme_->GetScaleStart());
    auto scaleFrameEnd = AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameEnd(), dialogTheme_->GetScaleEnd());
    auto scaleAnimationIn = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    scaleAnimationIn->AddKeyframe(scaleFrameStart);
    scaleAnimationIn->AddKeyframe(scaleFrameEnd);
    auto dialogCurve = AceType::MakeRefPtr<CubicCurve>(
        PHONE_ENTER_CURVE_X0, PHONE_ENTER_CURVE_Y0, PHONE_ENTER_CURVE_X1, PHONE_ENTER_CURVE_Y1);
    scaleAnimationIn->SetCurve(dialogCurve);
    // Build opacity animation for in.
    auto opacityKeyframeStart =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameStart(), dialogTheme_->GetOpacityStart());
    auto opacityKeyframeMiddle =
        AceType::MakeRefPtr<Keyframe<float>>(PHONE_OPACITY_MIDDLE_IN, dialogTheme_->GetOpacityEnd());
    auto opacityKeyframeEnd =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameEnd(), dialogTheme_->GetOpacityEnd());
    auto opacityAnimationIn = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimationIn->AddKeyframe(opacityKeyframeStart);
    opacityAnimationIn->AddKeyframe(opacityKeyframeMiddle);
    opacityAnimationIn->AddKeyframe(opacityKeyframeEnd);
    opacityAnimationIn->SetCurve(Curves::SHARP);
    // Build tween option for in
    TweenOption tweenOptionIn;
    tweenOptionIn.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimationIn);
    tweenOptionIn.SetOpacityAnimation(opacityAnimationIn);
    tweenOptionIn.SetDuration(dialogTheme_->GetAnimationDurationIn());
    tweenOptionIn.SetFillMode(FillMode::FORWARDS);
    // Build scale animation for out.
    auto scaleFrameStartOut =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameStart(), dialogTheme_->GetScaleEnd());
    auto scaleFrameEndOut =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameEnd(), dialogTheme_->GetScaleStart());
    auto scaleAnimationOut = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    scaleAnimationOut->AddKeyframe(scaleFrameStartOut);
    scaleAnimationOut->AddKeyframe(scaleFrameEndOut);
    scaleAnimationOut->SetCurve(Curves::SMOOTH);
    // Build opacity animation for out.
    auto opacityKeyframeStartOut =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameStart(), dialogTheme_->GetOpacityEnd());
    auto opacityKeyframeEndOut =
        AceType::MakeRefPtr<Keyframe<float>>(dialogTheme_->GetFrameEnd(), dialogTheme_->GetOpacityStart());
    auto opacityAnimationOut = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimationOut->AddKeyframe(opacityKeyframeStartOut);
    opacityAnimationOut->AddKeyframe(opacityKeyframeEndOut);
    opacityAnimationOut->SetCurve(Curves::SMOOTH);
    // Build tween option for out
    TweenOption tweenOptionOut;
    tweenOptionOut.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimationOut);
    tweenOptionOut.SetOpacityAnimation(opacityAnimationOut);
    tweenOptionOut.SetDuration(dialogTheme_->GetAnimationDurationOut());
    tweenOptionOut.SetFillMode(FillMode::FORWARDS);
    auto transition =
        AceType::MakeRefPtr<TransitionComponent>(TweenComponent::AllocTweenComponentId(), DIALOG_TWEEN_NAME, child);
    transition->SetIsFirstFrameShow(false);
    transition->SetTransitionOption(tweenOptionIn, tweenOptionOut);
    return transition;
}

RefPtr<Component> DialogComponent::GenerateComposed(
    const std::string& name, const RefPtr<Component>& child, bool isDialogTweenChild)
{
    const auto& pipelineContext = context_.Upgrade();
    if (pipelineContext) {
        const auto& accessibilityManager = pipelineContext->GetAccessibilityManager();
        if (accessibilityManager) {
            // use accessibility node already created with dom node in JS app
            int32_t composedId = customDialogId_;
            if (composedId == -1) {
                composedId = accessibilityManager->GenerateNextAccessibilityId();
            }
            const auto& composed = AceType::MakeRefPtr<ComposedComponent>(std::to_string(composedId), name, child);
            if (isDialogTweenChild) {
                accessibilityManager->CreateSpecializedNode(name, composedId, dialogTweenComposedId_);
            } else {
                dialogTweenComposedId_ = composedId;
            }
            return composed;
        }
    }
    return child;
}

RefPtr<Component> DialogComponent::BuildDivider(const RefPtr<ThemeManager>& themeManager)
{
    if (!themeManager) {
        return nullptr;
    }
    auto padding = AceType::MakeRefPtr<PaddingComponent>();
    auto dialogTheme = AceType::DynamicCast<DialogTheme>(themeManager->GetTheme(DialogTheme::TypeId()));
    if (!dialogTheme) {
        return nullptr;
    }
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        padding->SetPadding(Edge(dialogTheme->GetButtonSpacingHorizontal(), Dimension(0.0, DimensionUnit::VP),
            Dimension(0.0, DimensionUnit::VP), Dimension(0.0, DimensionUnit::VP)));
        return padding;
    }
    padding->SetPadding(dialogTheme->GetDividerPadding());
    auto dividerBox = AceType::MakeRefPtr<BoxComponent>();
    dividerBox->SetWidth(dialogTheme->GetDividerWidth().Value(), dialogTheme->GetDividerWidth().Unit());
    dividerBox->SetHeight(dialogTheme->GetDividerHeight().Value(), dialogTheme->GetDividerHeight().Unit());
    auto backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetBackgroundColor(dialogTheme->GetDividerColor());
    dividerBox->SetBackDecoration(backDecoration);
    padding->SetChild(dividerBox);
    return padding;
}

RefPtr<DialogComponent> DialogBuilder::Build(
    const DialogProperties& dialogProperties, const WeakPtr<PipelineContext>& context)
{
    auto dialog = BuildDialogWithType(dialogProperties.type);
    dialog->SetDialogProperties(dialogProperties);
    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        return dialog;
    }
    auto themeManager = pipelineContext->GetThemeManager();
    if (!themeManager) {
        return dialog;
    }
    auto dialogTheme = AceType::DynamicCast<DialogTheme>(themeManager->GetTheme(DialogTheme::TypeId()));
    if (!dialogTheme) {
        return dialog;
    }
    std::string data;
    dialog->SetContext(context);
    dialog->SetBackgroundColor(dialogTheme->GetBackgroundColor());
    // Set title and content of dialog
    BuildTitleAndContent(dialog, dialogProperties, dialogTheme, data);
    // Set buttons of dialog
    BuildButtons(themeManager, dialog, dialogProperties.buttons, dialogTheme, data);
    // Build DialogTween
    auto controller = AceType::MakeRefPtr<Animator>(context);
    dialog->SetAnimator(controller);
    dialog->SetAutoCancel(dialogProperties.autoCancel);
    dialog->SetData(data);
    // Set eventMarker of dialog component
    if (!dialogProperties.callbacks.empty()) {
        for (const auto& callback : dialogProperties.callbacks) {
            if (callback.first == CALLBACK_SUCCESS) {
                dialog->SetOnSuccessId(callback.second);
            }
            if (callback.first == CALLBACK_CANCEL) {
                dialog->SetOnCancelId(callback.second);
            }
            if (callback.first == CALLBACK_COMPLETE) {
                dialog->SetOnCompleteId(callback.second);
            }
        }
    }
    // Set menu evenMarker
    if (dialogProperties.isMenu) {
        dialog->SetIsMenu(true);
        for (size_t index = 0; index < dialogProperties.buttons.size(); ++index) {
            dialog->GetMenuSuccessId().emplace_back(BackEndEventManager<void()>::GetInstance().GetAvailableMarker());
        }
    }
    return BuildAnimation(dialog, dialogTheme);
}

RefPtr<DialogComponent> DialogBuilder::BuildDialogWithType(DialogType type)
{
    RefPtr<DialogComponent> dialog;
    // Create different dialog according to type.
    switch (type) {
        case DialogType::ALERT_DIALOG: {
            dialog = AceType::MakeRefPtr<AlertDialogComponent>();
            dialog->SetOnSuccessId(BackEndEventManager<void(int32_t)>::GetInstance().GetAvailableMarker());
            break;
        }
        case DialogType::ACTION_SHEET: {
            dialog = AceType::MakeRefPtr<ActionSheetComponent>();
            dialog->SetIsMenu(true);
            dialog->SetOnSuccessId(BackEndEventManager<void(int32_t)>::GetInstance().GetAvailableMarker());
            break;
        }
        default:
            dialog = AceType::MakeRefPtr<DialogComponent>();
            break;
    }
    return dialog;
}

void DialogBuilder::BuildTitleAndContent(const RefPtr<DialogComponent>& dialog,
    const DialogProperties& dialogProperties, const RefPtr<DialogTheme>& dialogTheme, std::string& data)
{
    auto deviceType = SystemProperties::GetDeviceType();
    if ((deviceType != DeviceType::WATCH) && (!dialogProperties.title.empty())) {
        auto titleComponent = AceType::MakeRefPtr<TextComponent>(dialogProperties.title);
        auto style = dialogTheme->GetTitleTextStyle();
        style.SetMaxLines(dialogTheme->GetTitleMaxLines());
        style.SetTextOverflow(TextOverflow::ELLIPSIS);
        style.SetAdaptTextSize(style.GetFontSize(), dialogTheme->GetTitleMinFontSize());
        titleComponent->SetTextStyle(style);
        titleComponent->SetFocusColor(style.GetTextColor());
        dialog->SetTitle(titleComponent);
        data += dialogProperties.title + SEPARATE;
    }
    if (!dialogProperties.content.empty()) {
        auto contentComponent = AceType::MakeRefPtr<TextComponent>(dialogProperties.content);
        auto contentStyle = dialogTheme->GetContentTextStyle();
        if (deviceType == DeviceType::WATCH) {
            std::vector<TextSizeGroup> preferTextSizeGroups;
            preferTextSizeGroups.push_back({ contentStyle.GetFontSize(), 1 });
            preferTextSizeGroups.push_back({ dialogTheme->GetContentMinFontSize(), UINT32_MAX, TextOverflow::NONE });
            contentStyle.SetPreferTextSizeGroups(preferTextSizeGroups);
            contentStyle.SetTextAlign(TextAlign::CENTER);
        }
        contentComponent->SetTextStyle(contentStyle);
        contentComponent->SetFocusColor(dialogTheme->GetContentTextStyle().GetTextColor());
        dialog->SetContent(contentComponent);
        data += dialogProperties.content + SEPARATE;
    }
}

void DialogBuilder::BuildButtons(const RefPtr<ThemeManager>& themeManager, const RefPtr<DialogComponent>& dialog,
    const std::vector<ButtonInfo>& buttons, const RefPtr<DialogTheme>& dialogTheme,
    std::string& data)
{
    if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
        BuildButtonsForWatch(themeManager, dialog, data);
        return;
    }
    if (buttons.empty()) {
        return;
    }
    auto buttonTheme = AceType::DynamicCast<ButtonTheme>(themeManager->GetTheme(ButtonTheme::TypeId()));
    if (!buttonTheme) {
        return;
    }
    int32_t buttonIndex = 0;
    std::list<RefPtr<ButtonComponent>> buttonComponents;
    for (const auto& button : buttons) {
        if (button.text.empty()) {
            continue;
        }
        data += button.text + SEPARATE;

        // Init text style in button.
        TextStyle buttonTextStyle = buttonTheme->GetTextStyle();
        const Color TEXT_COLOR = Color::FromString("#0a59f4");
        buttonTextStyle.SetTextColor(TEXT_COLOR);
        buttonTextStyle.SetAdaptTextSize(buttonTheme->GetMaxFontSize(), buttonTheme->GetMinFontSize());
        buttonTextStyle.SetMaxLines(1);
        buttonTextStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
        if (SystemProperties::GetDeviceType() == DeviceType::CAR) {
            buttonTextStyle.SetAdaptTextSize(dialogTheme->GetButtonTextSize(), dialogTheme->GetMinButtonTextSize());
            buttonTextStyle.SetFontWeight(FontWeight::MEDIUM);
            buttonTextStyle.SetFontSize(dialogTheme->GetButtonTextSize());
            buttonTextStyle.SetAdaptTextSize(dialogTheme->GetButtonTextSize(), dialogTheme->GetMinButtonTextSize());
            if (buttonIndex != static_cast<int32_t>(buttons.size()) - 1) {
                buttonTextStyle.SetTextColor(dialogTheme->GetCommonButtonTextColor());
            } else {
                buttonTextStyle.SetTextColor(dialogTheme->GetEmphasizeButtonTextColor());
            }
        }

        RefPtr<ButtonComponent> buttonComponent;
        if (!button.textColor.empty()) {
            buttonTextStyle.SetTextColor(Color::FromString(button.textColor));
            buttonComponent = ButtonBuilder::Build(
                themeManager, button.text, buttonTextStyle, Color::FromString(button.textColor), true);
        } else {
            buttonComponent =
                ButtonBuilder::Build(themeManager, button.text, buttonTextStyle, buttonTextStyle.GetTextColor(), true);
        }
        buttonComponent->SetBackgroundColor(dialogTheme->GetButtonBackgroundColor());
        if (SystemProperties::GetDeviceType() == DeviceType::CAR) {
            buttonComponent->SetHeight(dialogTheme->GetButtonHeight());
            buttonComponent->SetRectRadius(dialogTheme->GetButtonHeight() / 2.0);
            if (buttonIndex != static_cast<int32_t>(buttons.size()) - 1) {
                buttonComponent->SetBackgroundColor(dialogTheme->GetCommonButtonBgColor());
            } else {
                buttonComponent->SetBackgroundColor(dialogTheme->GetEmphasizeButtonBgColor());
            }
        }
        buttonComponent->SetHoverColor(Color::FromString("#0C000000"));
        buttonComponent->SetClickedColor(dialogTheme->GetButtonClickedColor());
        // If background color of button is setted by developer, use it.
        if (button.isBgColorSetted) {
            buttonComponent->SetBackgroundColor(button.bgColor);
            buttonComponent->SetHoverColor(button.bgColor.BlendColorWithAlpha(dialogTheme->GetButtonClickedColor()));
            buttonComponent->SetClickedColor(button.bgColor.BlendColorWithAlpha(dialogTheme->GetButtonClickedColor()));
        }
        buttonComponent->SetType(ButtonType::TEXT);
        buttonComponents.emplace_back(buttonComponent);
        ++buttonIndex;
    }
    dialog->SetActions(buttonComponents);
}

void DialogBuilder::BuildButtonsForWatch(
    const RefPtr<ThemeManager>& themeManager, const RefPtr<DialogComponent>& dialog, std::string& data)
{
    auto buttonTheme = AceType::DynamicCast<ButtonTheme>(themeManager->GetTheme(ButtonTheme::TypeId()));
    auto dialogTheme = AceType::DynamicCast<DialogTheme>(themeManager->GetTheme(DialogTheme::TypeId()));
    if (!buttonTheme || !dialogTheme) {
        return;
    }
    std::string buttonText;
    std::list<RefPtr<ButtonComponent>> buttonComponents;
    for (int32_t i = 1; i <= DIALOG_BUTTONS_COUNT_WATCH; ++i) {
        auto buttonPadding = AceType::MakeRefPtr<PaddingComponent>();
        buttonPadding->SetPadding(buttonTheme->GetMinCircleButtonPadding());
        RefPtr<ImageComponent> buttonIcon;
        if (i == 1) {
            buttonText = Localization::GetInstance()->GetEntryLetters(DIALOG_CANCEL);
            buttonIcon = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::WRONG_SVG);
        } else {
            buttonText = Localization::GetInstance()->GetEntryLetters(DIALOG_OK);
            buttonIcon = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::CORRECT_SVG);
        }
        data += buttonText + SEPARATE;
        buttonIcon->SetWidth(buttonTheme->GetMinCircleButtonIcon());
        buttonIcon->SetHeight(buttonTheme->GetMinCircleButtonIcon());
        buttonPadding->SetChild(buttonIcon);
        std::list<RefPtr<Component>> buttonChildren;
        buttonChildren.emplace_back(buttonPadding);
        auto buttonComponent = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
        buttonComponent->SetWidth(buttonTheme->GetMinCircleButtonDiameter());
        buttonComponent->SetHeight(buttonTheme->GetMinCircleButtonDiameter());
        buttonComponent->SetRectRadius(buttonTheme->GetMinCircleButtonDiameter() / 2.0);
        buttonComponent->SetBackgroundColor(buttonTheme->GetBgColor());
        buttonComponent->SetClickedColor(buttonTheme->GetClickedColor());
        if (i == 2) {
            buttonComponent->SetBackgroundColor(dialogTheme->GetButtonBackgroundColor());
            buttonComponent->SetClickedColor(dialogTheme->GetButtonClickedColor());
        }
        buttonComponent->SetFocusColor(buttonTheme->GetBgFocusColor());
        buttonComponent->SetFocusAnimationColor(buttonTheme->GetBgFocusColor());
        buttonComponent->SetAccessibilityText(buttonText);
        buttonComponents.emplace_back(buttonComponent);
    }
    dialog->SetActions(buttonComponents);
}

RefPtr<DialogComponent> DialogBuilder::BuildAnimation(
    const RefPtr<DialogComponent>& dialogChild, const RefPtr<DialogTheme>& dialogTheme)
{
    auto tweenBox = AceType::MakeRefPtr<BoxComponent>();
    auto decoration = AceType::MakeRefPtr<Decoration>();
    decoration->SetBackgroundColor(Color(dialogTheme->GetMaskColorEnd()));
    tweenBox->SetBackDecoration(decoration);
    const auto& colorAnimation = AceType::MakeRefPtr<CurveAnimation<Color>>(
        dialogTheme->GetMaskColorStart(), dialogTheme->GetMaskColorEnd(), Curves::LINEAR);
    // Build tween option of in
    TweenOption tweenOptionIn;
    tweenOptionIn.SetColorAnimation(colorAnimation);
    tweenOptionIn.SetDuration(dialogTheme->GetAnimationDurationIn());
    tweenOptionIn.SetFillMode(FillMode::FORWARDS);
    // Build tween option of out
    const auto& colorAnimationOut = AceType::MakeRefPtr<CurveAnimation<Color>>(
        dialogTheme->GetMaskColorEnd(), dialogTheme->GetMaskColorStart(), Curves::LINEAR);
    TweenOption tweenOptionOut;
    tweenOptionOut.SetColorAnimation(colorAnimationOut);
    tweenOptionOut.SetDuration(dialogTheme->GetAnimationDurationOut());
    tweenOptionOut.SetFillMode(FillMode::FORWARDS);
    // Build transition
    auto transition =
        AceType::MakeRefPtr<TransitionComponent>(TweenComponent::AllocTweenComponentId(), "transition", tweenBox);
    transition->SetIsFirstFrameShow(false);
    transition->SetTransitionOption(tweenOptionIn, tweenOptionOut);

    dialogChild->SetChild(transition);
    dialogChild->SetDialogTweenBox(tweenBox);
    return dialogChild;
}

} // namespace OHOS::Ace