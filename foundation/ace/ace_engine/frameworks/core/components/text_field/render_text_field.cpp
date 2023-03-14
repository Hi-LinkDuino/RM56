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

#include "core/components/text_field/render_text_field.h"

#include "base/i18n/localization.h"
#include "base/json/json_util.h"
#include "base/utils/string_utils.h"
#include "core/animation/curve_animation.h"
#include "core/common/clipboard/clipboard_proxy.h"
#include "core/common/font_manager.h"
#include "core/components/stack/stack_element.h"
#include "core/components/text/text_utils.h"
#include "core/components/text_overlay/text_overlay_component.h"
#include "core/components/text_overlay/text_overlay_element.h"
#include "core/event/ace_event_helper.h"

#if defined(ENABLE_STANDARD_INPUT)
#include "core/components/text_field/on_text_changed_listener_impl.h"
#endif

namespace OHOS::Ace {
namespace {

constexpr uint32_t TWINKLING_INTERVAL_MS = 500;
// Tick count indicate how long should the naked character should be displayed while obscure_ == true.
constexpr uint32_t OBSCURE_SHOW_TICKS = 3;
constexpr double HANDLE_HOT_ZONE = 10.0;

constexpr char16_t OBSCURING_CHARACTER = u'•';
constexpr char16_t OBSCURING_CHARACTER_FOR_AR = u'*';

constexpr int32_t DEFAULT_SELECT_INDEX = 0;
constexpr int32_t SHOW_HANDLE_DURATION = 250;
constexpr int32_t DOUBLE_CLICK_FINGERS = 1;
constexpr int32_t DOUBLE_CLICK_COUNTS = 2;
constexpr double FIFTY_PERCENT = 0.5;

constexpr Dimension OFFSET_FOCUS = 4.0_vp;
constexpr Dimension DEFLATE_RADIUS_FOCUS = 3.0_vp;

} // namespace

#if defined(ENABLE_STANDARD_INPUT)
void RenderTextField::UpdateConfiguration()
{
    MiscServices::Configuration configuration;
    configuration.SetEnterKeyType(static_cast<MiscServices::EnterKeyType>((int32_t)action_));
    configuration.SetTextInputType(static_cast<MiscServices::TextInputType>((int32_t)keyboard_));
    MiscServices::InputMethodController::GetInstance()->OnConfigurationChange(configuration);
}
#endif

RenderTextField::RenderTextField()
    : twinklingInterval(TWINKLING_INTERVAL_MS), controller_(AceType::MakeRefPtr<TextEditController>())
{}

RenderTextField::~RenderTextField()
{
    LOGI("Destruction text field.");
    if (controller_) {
        controller_->Clear();
        controller_->RemoveObserver(WeakClaim(this));
    }
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }
    PopTextOverlay();
    pipelineContext->RemoveFontNode(AceType::WeakClaim(this));
    auto fontManager = pipelineContext->GetFontManager();
    if (fontManager) {
        fontManager->UnRegisterCallback(AceType::WeakClaim(this));
        fontManager->RemoveVariationNode(WeakClaim(this));
    }

    // If soft keyboard is still exist, close it.
    if (HasConnection()) {
#if defined(ENABLE_STANDARD_INPUT)
        LOGI("Destruction text field, close input method.");
        MiscServices::InputMethodController::GetInstance()->Close();
#else
        connection_->Close(GetInstanceId());
        connection_ = nullptr;
#endif
    }
}

void RenderTextField::Update(const RefPtr<Component>& component)
{
    const RefPtr<TextFieldComponent> textField = AceType::DynamicCast<TextFieldComponent>(component);
    if (!textField) {
        return;
    }

    // Clear children to avoid children increase.
    ClearChildren();

    if (textField->IsTextLengthLimited()) {
        maxLength_ = textField->GetMaxLength();
    }

    selection_ = textField->GetSelection();
    placeholder_ = textField->GetPlaceholder();
    inputFilter_ = textField->GetInputFilter();
    inactivePlaceholderColor_ = textField->GetPlaceholderColor();
    focusPlaceholderColor_ = textField->GetFocusPlaceholderColor();
    focusBgColor_ = textField->GetFocusBgColor();
    focusTextColor_ = textField->GetFocusTextColor();
    selectedColor_ = textField->GetSelectedColor();
    pressColor_ = textField->GetPressColor();
    decoration_ = textField->GetDecoration();
    inactiveBgColor_ = textField->GetBgColor();
    if (decoration_ && (decoration_->GetImage() || decoration_->GetGradient().IsValid())) {
        inactiveBgColor_ = Color::TRANSPARENT;
        focusBgColor_ = Color::TRANSPARENT;
    }
    originBorder_ = textField->GetOriginBorder();
    style_ = textField->GetTextStyle();
    placeHoldStyle_ = textField->GetPlaceHoldStyle();
    editingStyle_ = textField->GetEditingStyle();
    fontSize_ = style_.GetFontSize();
    errorTextStyle_ = textField->GetErrorTextStyle();
    errorSpacingInDimension_ = textField->GetErrorSpacing();
    errorIsInner_ = textField->GetErrorIsInner();
    errorBorderWidth_ = textField->GetErrorBorderWidth();
    errorBorderColor_ = textField->GetErrorBorderColor();
    needFade_ = textField->NeedFade();
    inactiveTextColor_ = style_.GetTextColor();
    maxLines_ = textField->GetTextMaxLines();
    onTextChangeEvent_ = AceAsyncEvent<void(const std::string&)>::Create(textField->GetOnTextChange(), context_);
    onError_ = textField->GetOnError();
    onValueChangeEvent_ = textField->GetOnTextChange().GetUiStrFunction();
    if (textField->GetOnChange()) {
        onChange_ = *textField->GetOnChange();
    }
    if (textField->GetOnEditChanged()) {
        onEditChanged_ = *textField->GetOnEditChanged();
    }
    if (textField->GetOnSubmit()) {
        onSubmit_ = *textField->GetOnSubmit();
    }
    if (textField->GetOnClick()) {
        onClick_ = *textField->GetOnClick();
    }
    onSelectChangeEvent_ = AceAsyncEvent<void(const std::string&)>::Create(textField->GetOnSelectChange(), context_);
    onFinishInputEvent_ = AceAsyncEvent<void(const std::string&)>::Create(textField->GetOnFinishInput(), context_);
    onTapEvent_ = AceAsyncEvent<void()>::Create(textField->GetOnTap(), context_);
    catchMode_ = textField->GetOnTap().IsEmpty() || textField->GetOnTap().GetCatchMode();
    static const int32_t bubbleModeVersion = 6;
    auto pipeline = context_.Upgrade();
    if (!catchMode_) {
        if (pipeline && pipeline->GetMinPlatformVersion() >= bubbleModeVersion) {
            catchMode_ = false;
        } else {
            catchMode_ = true;
        }
    }
    onLongPressEvent_ = AceAsyncEvent<void()>::Create(textField->GetOnLongPress(), context_);
    textAlign_ = textField->GetTextAlign();
    textDirection_ = textField->GetTextDirection();
    realTextDirection_ = textDirection_;
    showCursor_ = textField->ShowCursor();
    UpdateObscure(textField);
    enabled_ = textField->IsEnabled();
    widthReserved_ = textField->GetWidthReserved();
    blockRightShade_ = textField->GetBlockRightShade();
    isVisible_ = textField->IsVisible();
    showPasswordIcon_ = textField->ShowPasswordIcon();
    if (textField->HasSetResetToStart() && textField->GetUpdateType() == UpdateType::ALL) {
        resetToStart_ = textField->GetResetToStart();
    }
    if (keyboard_ != textField->GetTextInputType()) {
        auto context = context_.Upgrade();
        if (context && context->GetIsDeclarative()) {
            ClearEditingValue();
        } else {
            if (keyboard_ == TextInputType::VISIBLE_PASSWORD) {
                ClearEditingValue();
            }
        }
        keyboard_ = textField->GetTextInputType();
        CloseKeyboard();
    }

    if (action_ != textField->GetAction()) {
        auto context = context_.Upgrade();
        if (context && context->GetIsDeclarative()) {
            CloseKeyboard();
            action_ = textField->GetAction();
        } else {
            action_ = textField->GetAction();
        }
    }

    actionLabel_ = textField->GetActionLabel();
    height_ = textField->GetHeight();
    if (textField->IsCursorColorSet()) {
        cursorColorIsSet_ = true;
        cursorColor_ = textField->GetCursorColor();
    }
    cursorRadius_ = textField->GetCursorRadius();
    textFieldController_ = textField->GetTextFieldController();
    if (textFieldController_) {
        auto weak = AceType::WeakClaim(this);
        textFieldController_->SetCaretPosition([weak](int32_t caretPosition) {
            auto textField = weak.Upgrade();
            if (textField) {
                textField->UpdateSelection(caretPosition);
                textField->cursorPositionType_ = CursorPositionType::NORMAL;
                textField->MarkNeedLayout();
            }
        });
    }
    if (textField->GetTextEditController() && controller_ != textField->GetTextEditController()) {
        if (controller_) {
            controller_->RemoveObserver(WeakClaim(this));
        }
        controller_ = textField->GetTextEditController();
    }
    if (controller_) {
        controller_->RemoveObserver(WeakClaim(this));
        controller_->AddObserver(WeakClaim(this));
        controller_->SetHint(placeholder_);
        if (textField->IsValueUpdated()) {
            controller_->SetText(textField->GetValue(), false);
        }
    }
    ApplyRestoreInfo();
    extend_ = textField->IsExtend();
    softKeyboardEnabled_ = textField->IsSoftKeyboardEnabled();
    text_ = textField->GetValue();
    showEllipsis_ = textField->ShowEllipsis();
    auto context = context_.Upgrade();
    if (!clipboard_ && context) {
        clipboard_ = ClipboardProxy::GetInstance()->GetClipboard(context->GetTaskExecutor());
    }

    if ((style_.IsAllowScale() || style_.GetFontSize().Unit() == DimensionUnit::FP) && context) {
        context->AddFontNode(AceType::WeakClaim(this));
    }

    showCounter_ = textField->ShowCounter();
    countTextStyle_ = textField->GetCountTextStyle();
    overCountStyle_ = textField->GetOverCountStyle();
    countTextStyleOuter_ = textField->GetCountTextStyleOuter();
    overCountStyleOuter_ = textField->GetOverCountStyleOuter();

    inputOptions_ = textField->GetInputOptions();
    onOptionsClick_ = textField->GetOnOptionsClick();
    onTranslate_ = textField->GetOnTranslate();
    onShare_ = textField->GetOnShare();
    onSearch_ = textField->GetOnSearch();

#if defined(ENABLE_STANDARD_INPUT)
    UpdateConfiguration();
#endif
    SetCallback(textField);
    UpdateFormatters();
    UpdateFocusStyles();
    UpdateIcon(textField);
    RegisterFontCallbacks();
    MarkNeedLayout();
    UpdateAccessibilityAttr();
}

void RenderTextField::SetCallback(const RefPtr<TextFieldComponent>& textField)
{
    if (textField->GetOnCopy()) {
        onCopy_ = *textField->GetOnCopy();
    }
    if (textField->GetOnCut()) {
        onCut_ = *textField->GetOnCut();
    }
    if (textField->GetOnPaste()) {
        onPaste_ = *textField->GetOnPaste();
    }
}

void RenderTextField::OnPaintFinish()
{
    UpdateFocusAnimation();
    UpdateOverlay();
    InitAccessibilityEventListener();
    UpdateAccessibilityPosition();
}

void RenderTextField::PerformLayout()
{
    if (!lastLayoutParam_.has_value()) {
        lastLayoutParam_ = std::make_optional(GetLayoutParam());
    }

    if (GetEditingValue().text.empty()) {
        cursorPositionType_ = CursorPositionType::END;
    }

    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        const auto& currentText = controller_->GetValue().text;
        showPlaceholder_ = currentText.empty();
        if (showPlaceholder_) {
            SetTextStyle(placeHoldStyle_);
        } else {
            SetTextStyle(editingStyle_);
        }
    }

    auto pipelineContext = GetContext().Upgrade();
    if ((style_.IsAllowScale() || style_.GetFontSize().Unit() == DimensionUnit::FP) && pipelineContext &&
        !NearEqual(fontScale_, pipelineContext->GetFontScale())) {
        fontScale_ = pipelineContext->GetFontScale();
        style_.SetFontSize(fontSize_ * fontScale_);
    }

    iconSize_ = NormalizeToPx(iconSizeInDimension_);
    iconHotZoneSize_ = NormalizeToPx(iconHotZoneSizeInDimension_);
    errorSpacing_ = NormalizeToPx(errorSpacingInDimension_);
    if (!GetChildren().empty()) {
        auto innerLayout = GetLayoutParam();
        innerLayout.SetMinSize(Size());
        const auto& child = GetChildren().front();
        child->Layout(innerLayout);
    }
    SetLayoutSize(GetLayoutParam().Constrain(Measure()));
    UpdateFocusAnimation();

    LayoutParam layoutParam = GetLayoutParam();
    layoutParam.SetMinSize(Size());
    if (iconImage_) {
        iconImage_->Layout(layoutParam);
    }
    if (renderShowIcon_) {
        renderShowIcon_->Layout(layoutParam);
    }
    if (renderHideIcon_) {
        renderHideIcon_->Layout(layoutParam);
    }
    if (needNotifyChangeEvent_ && (onTextChangeEvent_ || onValueChangeEvent_ || onChange_)) {
        needNotifyChangeEvent_ = false;
        if (onChange_) {
            onChange_(GetEditingValue().text);
        }
        if (onValueChangeEvent_) {
            onValueChangeEvent_(GetEditingValue().text);
        }
        if (onTextChangeEvent_) {
            auto jsonResult = JsonUtil::Create(true);
            jsonResult->Put("text", GetEditingValue().text.c_str());
            jsonResult->Put("value", GetEditingValue().text.c_str());
            jsonResult->Put("lines", textLines_);
            jsonResult->Put("height", textHeight_);
            onTextChangeEvent_(std::string(R"("change",)").append(jsonResult->ToString()));
        }
    }

    HandleDeviceOrientationChange();
}

bool RenderTextField::HandleMouseEvent(const MouseEvent& event)
{
    if (event.button == MouseButton::LEFT_BUTTON) {
        if (event.action == MouseAction::PRESS) {
            UpdateStartSelection(DEFAULT_SELECT_INDEX, event.GetOffset(), true, false);
        } else if (event.action == MouseAction::MOVE) {
            int32_t start = GetEditingValue().selection.baseOffset;
            int32_t end = GetCursorPositionForClick(event.GetOffset());
            UpdateSelection(start, end);
            StopTwinkling();
            MarkNeedRender();
        } else {
            LOGD("on left button release");
        }
    }

    if (event.button == MouseButton::RIGHT_BUTTON && event.action == MouseAction::PRESS) {
        Offset rightClickOffset = event.GetOffset();
        ShowTextOverlay(rightClickOffset, false);
    }

    return true;
}

void RenderTextField::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!enabled_) {
        return;
    }
    if (!clickRecognizer_) {
        clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickRecognizer_->SetUseCatchMode(catchMode_);
        auto weak = WeakClaim(this);
        clickRecognizer_->SetOnClick([weak](const ClickInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->OnClick(info);
            }
        });
        clickRecognizer_->SetPriority(GesturePriority::Low);
    }
    clickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(clickRecognizer_);

    if (!doubleClickRecognizer_) {
        doubleClickRecognizer_ =
            AceType::MakeRefPtr<ClickRecognizer>(context_, DOUBLE_CLICK_FINGERS, DOUBLE_CLICK_COUNTS);
        doubleClickRecognizer_->SetUseCatchMode(catchMode_);
        auto weak = WeakClaim(this);
        doubleClickRecognizer_->SetOnClick([weak](const ClickInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->OnDoubleClick(info);
            }
        });
        doubleClickRecognizer_->SetPriority(GesturePriority::High);
    }
    doubleClickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(doubleClickRecognizer_);

    if (!longPressRecognizer_) {
        longPressRecognizer_ = AceType::MakeRefPtr<LongPressRecognizer>(context_);
        auto weak = WeakClaim(this);
        longPressRecognizer_->SetOnLongPress([weak = WeakClaim(this)](const LongPressInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->OnLongPress(info);
            }
        });
        longPressRecognizer_->SetPriority(GesturePriority::High);
    }
    longPressRecognizer_->SetCoordinateOffset(coordinateOffset);
    longPressRecognizer_->SetTouchRestrict(touchRestrict);
    result.emplace_back(longPressRecognizer_);

    if (!rawRecognizer_) {
        rawRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
        auto weak = WeakClaim(this);
        rawRecognizer_->SetOnTouchDown([weak = WeakClaim(this)](const TouchEventInfo& info) {
            auto textField = weak.Upgrade();
            if (textField) {
                textField->StartPressAnimation(true);
            }
        });

        rawRecognizer_->SetOnTouchUp([weak = WeakClaim(this)](const TouchEventInfo& info) {
            auto textField = weak.Upgrade();
            if (textField) {
                textField->StartPressAnimation(false);
            }
        });

        rawRecognizer_->SetOnTouchCancel([weak = WeakClaim(this)](const TouchEventInfo& info) {
            auto textField = weak.Upgrade();
            if (textField) {
                textField->StartPressAnimation(false);
            }
        });
    }
    rawRecognizer_->SetTouchRestrict(touchRestrict);
    rawRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(rawRecognizer_);
}

void RenderTextField::StartPressAnimation(bool pressDown)
{
    if (!pressController_) {
        pressController_ = AceType::MakeRefPtr<Animator>(context_);
    }
    if (pressController_->IsRunning()) {
        pressController_->Stop();
    }
    pressController_->ClearInterpolators();
    RefPtr<KeyframeAnimation<Color>> animation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    if (pressDown) {
        CreateMouseAnimation(animation, GetEventEffectColor(), pressColor_);
    } else {
        CreateMouseAnimation(animation, GetEventEffectColor(), Color::TRANSPARENT);
    }
    pressController_->AddInterpolator(animation);
    pressController_->SetDuration(PRESS_DURATION);
    pressController_->SetFillMode(FillMode::FORWARDS);
    pressController_->Forward();
}

void RenderTextField::OnClick(const ClickInfo& clickInfo)
{
    // Handle click on password icon when password icon is valid, switch between show and hide icon.
    Point clickPoint = Point(clickInfo.GetLocalLocation().GetX(), clickInfo.GetLocalLocation().GetY());
    if (showPasswordIcon_ && passwordIconRect_.IsInRegion(clickPoint)) {
        obscure_ = !obscure_;
        passwordRecord_ = obscure_;
        PopTextOverlay();
        MarkNeedLayout();
        return;
    }

    isValueFromRemote_ = false;
    auto globalPosition = clickInfo.GetGlobalLocation();
    auto globalOffset = GetGlobalOffset();

    if (SearchAction(globalPosition, globalOffset)) {
        return;
    }
    if (tapCallback_) {
        if (!tapCallback_()) {
            return;
        }
    }
    if (onTapEvent_) {
        onTapEvent_();
    }
    if (onClick_) {
        onClick_(clickInfo);
    }
    CursorMoveOnClick(globalPosition);
    ShowError("", false);
    UpdateStartSelection(DEFAULT_SELECT_INDEX, globalPosition, true, false);
    if (clickInfo.GetSourceDevice() == SourceType::MOUSE) {
        StartTwinkling();
    } else {
        StartTwinkling();
        ShowTextOverlay(globalPosition, true);
    }
    auto context = GetContext().Upgrade();
    if (context) {
        context->SetClickPosition(GetGlobalOffset() + Size(0, GetLayoutSize().Height()));
    }
}

bool RenderTextField::SearchAction(const Offset& globalPosition, const Offset& globalOffset)
{
    double widthReserved = NormalizeToPx(widthReserved_);
    if (widthReserved > 0) {
        if (textDirection_ == TextDirection::RTL) {
            if ((globalPosition.GetX() - globalOffset.GetX()) < widthReserved) {
                controller_->SetText("");
                return true;
            } else if ((globalPosition.GetX() - globalOffset.GetX()) > (GetLayoutSize().Width() - iconHotZoneSize_) &&
                       iconImage_ && action_ == TextInputAction::SEARCH) {
                PerformAction(action_, true);
                return true;
            }
        } else {
            if ((globalPosition.GetX() - globalOffset.GetX()) >= (GetLayoutSize().Width() - widthReserved)) {
                controller_->SetText("");
                return true;
            } else if ((globalPosition.GetX() - globalOffset.GetX()) < iconHotZoneSize_ && iconImage_ &&
                       action_ == TextInputAction::SEARCH) {
                PerformAction(action_, true);
                return true;
            }
        }
    }
    return false;
}

void RenderTextField::OnDoubleClick(const ClickInfo& clickInfo)
{
    auto clickPosition = GetCursorPositionForClick(clickInfo.GetGlobalLocation());
    auto selection = TextUtils::GetRangeOfSameType(GetEditingValue().text, clickPosition - 1);
    UpdateSelection(selection.GetStart(), selection.GetEnd());
    LOGI("text field accept double click, position: %{public}d, selection: %{public}s", clickPosition,
        selection.ToString().c_str());
    MarkNeedRender();
}

void RenderTextField::OnLongPress(const LongPressInfo& longPressInfo)
{
    if (tapCallback_ && !isOverlayShowed_) {
        if (!tapCallback_()) {
            return;
        }
    }

    if (onLongPressEvent_) {
        onLongPressEvent_();
    }

    ShowError("", false);

    if (longPressInfo.GetSourceDevice() == SourceType::MOUSE) {
        return;
    }

    Offset longPressPosition = longPressInfo.GetGlobalLocation();
    bool isTextEnd =
        (static_cast<size_t>(GetCursorPositionForClick(longPressPosition)) == GetEditingValue().GetWideText().length());
    bool singleHandle = isTextEnd || GetEditingValue().text.empty();
    bool isPassword = (keyboard_ == TextInputType::VISIBLE_PASSWORD);
    UpdateStartSelection(DEFAULT_SELECT_INDEX, longPressPosition, singleHandle || isPassword, true);
    ShowTextOverlay(longPressPosition, false);
}

void RenderTextField::ShowTextOverlay(const Offset& showOffset, bool isSingleHandle)
{
    if (!isVisible_) {
        return;
    }

    if (!IsSelectiveDevice()) {
        StartTwinkling();
        return;
    }

    isSingleHandle_ = isSingleHandle;

    auto selStart = GetEditingValue().selection.GetStart();
    auto selEnd = GetEditingValue().selection.GetEnd();

    Offset startHandleOffset = GetHandleOffset(selStart);
    Offset endHandleOffset = isSingleHandle ? startHandleOffset : GetHandleOffset(selEnd);

    if (isOverlayShowed_ && updateHandlePosition_) {
        Rect caretStart;
        bool visible = GetCaretRect(selStart, caretStart) ? IsVisible(caretStart + textOffsetForShowCaret_) : false;
        OverlayShowOption option { .showMenu = isOverlayShowed_,
            .showStartHandle = visible,
            .showEndHandle = visible,
            .isSingleHandle = isSingleHandle,
            .updateOverlayType = isSingleHandle ? UpdateOverlayType::CLICK : UpdateOverlayType::LONG_PRESS,
            .startHandleOffset = startHandleOffset,
            .endHandleOffset = endHandleOffset };
        if (!isSingleHandle_ || startHandleOffset != endHandleOffset) {
            isOverlayFocus_ = true;
        }
        updateHandlePosition_(option);

        // When the textOverlay is showed, restart the animation
        if (!animator_) {
            LOGE("Show textOverlay error, animator is nullptr");
            return;
        }
        if (!animator_->IsStopped()) {
            animator_->Stop();
        }
        animator_->Play();
        return;
    }

    // Pop text overlay before push.
    PopTextOverlay();

    textOverlay_ =
        AceType::MakeRefPtr<TextOverlayComponent>(GetThemeManager(), context_.Upgrade()->GetAccessibilityManager());
    textOverlay_->SetWeakTextField(WeakClaim(this));
    textOverlay_->SetIsSingleHandle(isSingleHandle || (keyboard_ == TextInputType::VISIBLE_PASSWORD));
    textOverlay_->SetLineHeight(selectHeight_);
    textOverlay_->SetClipRect(
        innerRect_ + Size(HANDLE_HOT_ZONE, HANDLE_HOT_ZONE) + GetOffsetToPage() - Offset(HANDLE_HOT_ZONE / 2.0, 0.0));
    textOverlay_->SetTextDirection(textDirection_);
    textOverlay_->SetRealTextDirection(existStrongDirectionLetter_ ? realTextDirection_ : TextDirection::LTR);
    textOverlay_->SetIsPassword(keyboard_ == TextInputType::VISIBLE_PASSWORD);
    textOverlay_->SetStartHandleOffset(startHandleOffset);
    textOverlay_->SetEndHandleOffset(endHandleOffset);
    textOverlay_->SetImageFill(imageFill_);
    textOverlay_->SetOptions(inputOptions_);
    textOverlay_->SetOptionsClickMarker(onOptionsClick_);
    textOverlay_->SetTranslateButtonMarker(onTranslate_);
    textOverlay_->SetShareButtonMarker(onShare_);
    textOverlay_->SetSearchButtonMarker(onSearch_);
    textOverlay_->SetContext(context_);
    // Add the Animation
    InitAnimation();

    if (!isSingleHandle_ || startHandleOffset != endHandleOffset) {
        isOverlayFocus_ = true;
    }
    RegisterCallbacksToOverlay();
}

void RenderTextField::InitAnimation()
{
    if (!textOverlay_) {
        LOGE("InitAnimation error, textOverlay is nullptr");
        return;
    }

    // Get the handleDiameter in theme, textoverlay is not nullptr
    double initHandleDiameter = textOverlay_->GetHandleDiameter().Value();
    double initHandleDiameterInner = textOverlay_->GetHandleDiameterInner().Value();

    // Add the animation for handleDiameter
    auto diameterAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(
        initHandleDiameter * FIFTY_PERCENT, initHandleDiameter, Curves::ELASTICS);
    diameterAnimation->AddListener([text = AceType::WeakClaim(this)](double value) {
        auto textField = text.Upgrade();
        if (textField && textField->updateHandleDiameter_) {
            textField->updateHandleDiameter_(value);
        }
    });

    // Add the animation for handleDiameterinner
    auto diameterInnerAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(
        initHandleDiameterInner * FIFTY_PERCENT, initHandleDiameterInner, Curves::ELASTICS);
    diameterInnerAnimation->AddListener([text = AceType::WeakClaim(this)](double value) {
        auto textField = text.Upgrade();
        if (textField && textField->updateHandleDiameterInner_) {
            textField->updateHandleDiameterInner_(value);
        }
    });

    // Add the animation
    LOGD("Add animation to animator");
    animator_ = AceType::MakeRefPtr<Animator>(context_);
    animator_->AddInterpolator(diameterAnimation);
    animator_->AddInterpolator(diameterInnerAnimation);
    animator_->SetDuration(SHOW_HANDLE_DURATION);
    animator_->Play();
}

void RenderTextField::RegisterCallbacksToOverlay()
{
    if (!textOverlay_) {
        return;
    }
    textOverlay_->SetOnCut([weak = AceType::WeakClaim(this)] {
        auto textfield = weak.Upgrade();
        if (textfield) {
            textfield->HandleOnCut();
        }
    });

    textOverlay_->SetOnCopy([weak = AceType::WeakClaim(this)] {
        auto textfield = weak.Upgrade();
        if (textfield) {
            textfield->HandleOnCopy();
        }
    });

    textOverlay_->SetOnCopyAll(
        [weak = AceType::WeakClaim(this)](const std::function<void(const Offset&, const Offset&)>& callback) {
            auto textfield = weak.Upgrade();
            if (textfield) {
                textfield->HandleOnCopyAll(callback);
            }
        });

    textOverlay_->SetOnStartHandleMove(
        [weak = AceType::WeakClaim(this)](int32_t end, const Offset& startHandleOffset,
            const std::function<void(const Offset&)>& startCallback, bool isSingleHandle) {
            auto textfield = weak.Upgrade();
            if (textfield) {
                textfield->HandleOnStartHandleMove(end, startHandleOffset, startCallback, isSingleHandle);
            }
        });

    textOverlay_->SetOnEndHandleMove([weak = AceType::WeakClaim(this)](int32_t start, const Offset& endHandleOffset,
                                         const std::function<void(const Offset&)>& endCallback) {
        auto textfield = weak.Upgrade();
        if (textfield) {
            textfield->HandleOnEndHandleMove(start, endHandleOffset, endCallback);
        }
    });

    auto callback = [weakTextField = WeakClaim(this)](const std::string& data) {
        auto textfield = weakTextField.Upgrade();
        if (textfield) {
            auto textOverlay = textfield->textOverlay_;
            if (textOverlay && !data.empty()) {
                textOverlay->SetOnPaste([weakTextField] {
                    auto textfield = weakTextField.Upgrade();
                    if (textfield) {
                        textfield->HandleOnPaste();
                    }
                });
            }
            textfield->PushTextOverlayToStack();
            textfield->UpdateOverlay();
        }
    };
    if (clipboard_) {
        clipboard_->GetData(callback);
    }

    auto onFocusChange = [weak = WeakClaim(this)](bool isFocus, bool needCloseKeyboard) {
        auto textField = weak.Upgrade();
        if (textField) {
            textField->OnOverlayFocusChange(isFocus, needCloseKeyboard);
        }
    };
    textOverlay_->SetOnFocusChange(onFocusChange);
}

void RenderTextField::PushTextOverlayToStack()
{
    if (!textOverlay_) {
        LOGE("TextOverlay is null");
        return;
    }
    hasTextOverlayPushed_ = true;
    auto lastStack = GetLastStack();
    if (!lastStack) {
        LOGE("LastStack is null");
        return;
    }
    isOverlayShowed_ = true;
    lastStack->PushComponent(textOverlay_, false);
    stackElement_ = WeakClaim(RawPtr(lastStack));
    MarkNeedRender();
}

bool RenderTextField::RequestKeyboard(bool isFocusViewChanged, bool needStartTwinkling)
{
    if (!enabled_) {
        LOGD("TextField is not enabled.");
        return false;
    }

    instanceId_ = ContainerScope::CurrentId();

    if (softKeyboardEnabled_) {
        LOGI("RenderTextField::CloseKeyboard: Request open soft keyboard");
#if defined(ENABLE_STANDARD_INPUT)
        if (textChangeListener_ == nullptr) {
            textChangeListener_ = new OnTextChangedListenerImpl(WeakClaim(this));
        }
        MiscServices::InputMethodController::GetInstance()->Attach(textChangeListener_);
#else
        if (!HasConnection()) {
            AttachIme();
            if (!HasConnection()) {
                LOGE("Get TextInput connection error");
                return false;
            }
            connection_->SetEditingState(GetEditingValue(), GetInstanceId());
        }
        connection_->Show(isFocusViewChanged, GetInstanceId());
#endif
    }

    if (keyboard_ != TextInputType::MULTILINE) {
        resetToStart_ = false;
        MarkNeedLayout();
    }
    if (needStartTwinkling) {
        StartTwinkling();
    }
    if (onEditChanged_) {
        onEditChanged_(softKeyboardEnabled_);
    }
    return true;
}

bool RenderTextField::CloseKeyboard(bool forceClose)
{
    if (!isOverlayShowed_ || !isOverlayFocus_ || forceClose) {
        if (!textFieldController_) {
            StopTwinkling();
        }
        if (HasConnection()) {
            LOGI("RenderTextField::CloseKeyboard: Request close soft keyboard");
#if defined(ENABLE_STANDARD_INPUT)
            MiscServices::InputMethodController::GetInstance()->HideTextInput();
#else
            connection_->Close(GetInstanceId());
            connection_ = nullptr;
#endif
        }

        if (onKeyboardClose_) {
            onKeyboardClose_(forceClose);
            onKeyboardClose_ = nullptr;
            UpdateSelection(GetEditingValue().selection.GetEnd());
            MarkNeedLayout();
        }

        if (keyboard_ != TextInputType::MULTILINE) {
            resetToStart_ = true;
            MarkNeedLayout();
        }
        if (onEditChanged_) {
            onEditChanged_(forceClose);
        }
        return true;
    }
    return false;
}

void RenderTextField::AttachIme()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("No context exists, failed to request keyboard.");
        return;
    }

    TextInputConfiguration config;
    config.type = keyboard_;
    config.action = action_;
    config.actionLabel = actionLabel_;
    config.obscureText = obscure_;
    LOGD("Request keyboard configuration: type=%{private}d action=%{private}d actionLabel=%{private}s "
         "obscureText=%{private}d",
        keyboard_, action_, actionLabel_.c_str(), obscure_);
    connection_ =
        TextInputProxy::GetInstance().Attach(WeakClaim(this), config, context->GetTaskExecutor(), GetInstanceId());
}

void RenderTextField::StartTwinkling()
{
    // Ignore the result because all ops are called on this same thread (ACE UI).
    // The only reason failed is that the task has finished.
    cursorTwinklingTask_.Cancel();

    // Show cursor right now.
    cursorVisibility_ = true;
    // Does not matter call more than one times.
    MarkNeedRender();

    ScheduleCursorTwinkling();
}

void RenderTextField::StopTwinkling()
{
    obscureTickPendings_ = 0;
    cursorTwinklingTask_.Cancel();

    if (cursorVisibility_) {
        // Repaint only if cursor is visible for now.
        cursorVisibility_ = false;
        MarkNeedRender();
    }
}

const TextEditingValue& RenderTextField::GetEditingValue() const
{
    return controller_->GetValue();
}

const TextEditingValue& RenderTextField::GetPreEditingValue() const
{
    return controller_->GetPreValue();
}

void RenderTextField::SetEditingValue(TextEditingValue&& newValue, bool needFireChangeEvent)
{
    if (newValue.text != GetEditingValue().text && needFireChangeEvent) {
        needNotifyChangeEvent_ = true;
    }
    ChangeCounterStyle(newValue);
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        if (GetEditingValue().text.empty()) {
            Dimension fontSize_ = placeHoldStyle_.GetFontSize();
            if (fontSize_.Value() <= 0) {
                Dimension fontSize_ { 14, DimensionUnit::FP };
                placeHoldStyle_.SetFontSize(fontSize_);
            }
            SetTextStyle(placeHoldStyle_);
        }
    }

    if (context && context->GetIsDeclarative()) {
        if (inputFilter_.empty() || newValue.text.empty()) {
            controller_->SetValue(newValue, needFireChangeEvent);
        } else {
            std::regex rw(inputFilter_);
            if (regex_match(newValue.text.c_str(), rw)) {
                inputCallBackStrSize_ = static_cast<int32_t>(newValue.text.length());
                controller_->SetValue(newValue, needFireChangeEvent);
            } else {
                inputCallBackStr_ = newValue.text.substr(inputCallBackStrSize_);
                if (onError_) {
                    onError_(inputCallBackStr_);
                }
            }
        }
    } else {
        controller_->SetValue(newValue, needFireChangeEvent);
    }
    UpdateAccessibilityAttr();
}

void RenderTextField::ClearEditingValue()
{
    TextEditingValue emptyValue;
    SetEditingValue(std::move(emptyValue));
}

std::u16string RenderTextField::GetTextForDisplay(const std::string& text) const
{
    std::u16string txtContent = StringUtils::Str8ToStr16(text);
    auto len = txtContent.length();
    if (!obscure_ || len == 0 || (obscureTickPendings_ > 0 && len == 1)) {
        return txtContent;
    }

    std::u16string obscured;
    if (Localization::GetInstance()->GetLanguage() == "ar") { // ar is the abbreviation of Arabic.
        obscured = std::u16string(len, OBSCURING_CHARACTER_FOR_AR);
    } else {
        obscured = std::u16string(len, OBSCURING_CHARACTER);
    }
    int32_t posBeforeCursor = GetEditingValue().selection.extentOffset - 1;
    if (obscureTickPendings_ > 0 && posBeforeCursor >= 0 && static_cast<size_t>(posBeforeCursor) < obscured.length()) {
        // Let the last commit character naked.
        obscured[posBeforeCursor] = txtContent[posBeforeCursor];
    }

    return obscured;
}

void RenderTextField::UpdateObscure(const RefPtr<TextFieldComponent>& textField)
{
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        if (!passwordRecord_) {
            if (keyboard_ != textField->GetTextInputType()) {
                passwordRecord_ = true;
                obscure_ = textField->NeedObscure();
            } else {
                obscure_ = !textField->NeedObscure();
            }
        } else {
            obscure_ = textField->NeedObscure();
        }
    } else {
        obscure_ = textField->NeedObscure();
    }
}

void RenderTextField::UpdateFormatters()
{
    textInputFormatters_.clear();

    if (maxLength_ < std::numeric_limits<uint32_t>::max()) {
        textInputFormatters_.emplace_back(std::make_unique<LengthLimitingFormatter>(maxLength_));
    }

    if (maxLines_ == 1) {
        textInputFormatters_.emplace_back(std::make_unique<SingleLineFormatter>());
    }

    switch (keyboard_) {
        case TextInputType::NUMBER: {
            textInputFormatters_.emplace_back(std::make_unique<NumberFormatter>());
            break;
        }
        case TextInputType::PHONE: {
            textInputFormatters_.emplace_back(std::make_unique<PhoneNumberFormatter>());
            break;
        }
        case TextInputType::EMAIL_ADDRESS: {
            textInputFormatters_.emplace_back(std::make_unique<EmailFormatter>());
            break;
        }
        case TextInputType::URL: {
            textInputFormatters_.emplace_back(std::make_unique<UriFormatter>());
            break;
        }
        default: {
            // No need limit.
        }
    }

    TextEditingValue temp = GetEditingValue();
    for (const auto& formatter : textInputFormatters_) {
        if (formatter) {
            formatter->Format(GetEditingValue(), temp);
        }
    }
    SetEditingValue(std::move(temp));
}

void RenderTextField::UpdateEditingValue(const std::shared_ptr<TextEditingValue>& value, bool needFireChangeEvent)
{
    if (!value) {
        LOGE("the value is nullptr");
        return;
    }

    lastKnownRemoteEditingValue_ = value;
    lastKnownRemoteEditingValue_->hint = placeholder_;
    TextEditingValue temp = *lastKnownRemoteEditingValue_;
    if (cursorPositionType_ != CursorPositionType::END ||
        (temp.selection.baseOffset == temp.selection.extentOffset &&
            temp.selection.baseOffset != static_cast<int32_t>(temp.GetWideText().length()))) {
        cursorPositionType_ = CursorPositionType::NORMAL;
        isValueFromRemote_ = true;
    }
    ChangeCounterStyle(temp);
    for (const auto& formatter : textInputFormatters_) {
        // GetEditingValue() is the old value, and lastKnownRemoteEditingValue_ is the newer.
        if (formatter) {
            formatter->Format(GetEditingValue(), temp);
        }
    }

    if (obscure_ && (temp.text.length() == GetEditingValue().text.length() + 1)) {
        // Reset pending.
        obscureTickPendings_ = OBSCURE_SHOW_TICKS;
    }

    if (temp.text != GetEditingValue().text && needFireChangeEvent) {
        needNotifyChangeEvent_ = true;
    }

    auto editingText = GetEditingValue().text;
    SetEditingValue(std::move(temp), needFireChangeEvent);
    UpdateRemoteEditingIfNeeded(needFireChangeEvent);

    MarkNeedLayout();

    // If input or delete text when overlay is showing, pop overlay from stack.
    if (lastKnownRemoteEditingValue_ && (lastKnownRemoteEditingValue_->text != editingText)) {
        if (onValueChange_) {
            onValueChange_();
        }
    }
}

void RenderTextField::PerformDefaultAction()
{
    PerformAction(action_);
}

void RenderTextField::PerformAction(TextInputAction action, bool forceCloseKeyboard)
{
    LOGD("PerformAction  %{public}d", static_cast<int32_t>(action));
    if (action == TextInputAction::NEXT && moveNextFocusEvent_) {
        moveNextFocusEvent_();
    } else {
        CloseKeyboard(forceCloseKeyboard);
    }
    if (onFinishInputEvent_) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("value", static_cast<int32_t>(action));
        onFinishInputEvent_(std::string(R"("enterkeyclick",)").append(jsonResult->ToString()));
    }
    if (onSubmitEvent_ && controller_) {
        onSubmitEvent_(controller_->GetValue().text);
    }
    if (onSubmit_) {
        onSubmit_(static_cast<int32_t>(action_));
    }
}

Size RenderTextField::Measure()
{
    return Size();
}

void RenderTextField::ScheduleCursorTwinkling()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("No context exists.");
        return;
    }

    if (!context->GetTaskExecutor()) {
        LOGW("context has no task executor.");
        return;
    }

    auto weak = WeakClaim(this);
    cursorTwinklingTask_.Reset([weak] {
        auto client = weak.Upgrade();
        if (client) {
            client->OnCursorTwinkling();
        }
    });
    auto taskExecutor = context->GetTaskExecutor();
    if (taskExecutor) {
        taskExecutor->PostDelayedTask(cursorTwinklingTask_, TaskExecutor::TaskType::UI, twinklingInterval);
    } else {
        LOGE("the task executor is nullptr");
    }
}

void RenderTextField::OnCursorTwinkling()
{
    // When glyph changes from visible to invisible, layout is needed.
    obscureTickPendings_ == 1 ? MarkNeedLayout() : MarkNeedRender();
    if (obscureTickPendings_ > 0) {
        --obscureTickPendings_;
    }
    cursorVisibility_ = !cursorVisibility_;
    ScheduleCursorTwinkling();
}

bool RenderTextField::OnKeyEvent(const KeyEvent& event)
{
    if (!enabled_) {
        return false;
    }

    // If back or escape is clicked and overlay is showing, pop overlay firstly.
    if (event.action == KeyAction::UP && (event.code == KeyCode::KEY_BACK || event.code == KeyCode::KEY_ESCAPE)) {
        if (isOverlayShowed_) {
            PopTextOverlay();
            return false;
        }
    }
    if (event.action == KeyAction::UP &&
        ((event.code == KeyCode::KEY_SHIFT_LEFT || event.code == KeyCode::KEY_SHIFT_RIGHT) ||
            (event.code == KeyCode::KEY_CTRL_LEFT || event.code == KeyCode::KEY_CTRL_RIGHT))) {
        return HandleKeyEvent(event);
    }

    if (event.action == KeyAction::DOWN) {
        cursorPositionType_ = CursorPositionType::NONE;
        bool moved = true;
        if (event.code == KeyCode::TV_CONTROL_LEFT) {
            CursorMoveLeft();
        } else if (event.code == KeyCode::TV_CONTROL_RIGHT) {
            CursorMoveRight();
        } else if (event.code == KeyCode::TV_CONTROL_UP) {
            CursorMoveUp();
        } else if (event.code == KeyCode::TV_CONTROL_DOWN) {
            CursorMoveDown();
        } else {
            moved = HandleKeyEvent(event);
        }
        if (moved) {
            // Obscure all glyphs immediately after cursor moved.
            obscureTickPendings_ = 0;
        }
        return moved;
    }

    return false;
}

void RenderTextField::UpdateFocusStyles()
{
    if (hasFocus_) {
        style_.SetTextColor(focusTextColor_);
        placeholderColor_ = focusPlaceholderColor_;
        if (decoration_) {
            decoration_->SetBackgroundColor(focusBgColor_);
        }
    } else {
        style_.SetTextColor(inactiveTextColor_);
        placeholderColor_ = inactivePlaceholderColor_;
        if (decoration_) {
            decoration_->SetBackgroundColor(inactiveBgColor_);
        }
    }
}

void RenderTextField::UpdateFocusAnimation()
{
    if (hasFocus_) {
        auto context = context_.Upgrade();
        if (!context) {
            return;
        }
        Offset offset;
        Size size;
        Radius deflateRadius;
        if (IsSelectiveDevice()) {
            double focusOffset = NormalizeToPx(OFFSET_FOCUS);
            offset = Offset(focusOffset, focusOffset);
            size = Size(focusOffset * 2.0, focusOffset * 2.0);
            deflateRadius = Radius(DEFLATE_RADIUS_FOCUS, DEFLATE_RADIUS_FOCUS);
        }
        RRect rrect = RRect::MakeRect(
            Rect(GetPosition() + offset, GetLayoutSize() - ComputeDeflateSizeOfErrorAndCountText() - size));
        if (decoration_) {
            const auto& border = decoration_->GetBorder();
            rrect.SetCorner({ border.TopLeftRadius() - deflateRadius, border.TopRightRadius() - deflateRadius,
                border.BottomRightRadius() - deflateRadius, border.BottomLeftRadius() - deflateRadius });
        }
        context->ShowFocusAnimation(rrect, focusBgColor_, GetGlobalOffset() + offset);
    }
}

void RenderTextField::UpdateIcon(const RefPtr<TextFieldComponent>& textField)
{
    if (!textField) {
        return;
    }
    iconSizeInDimension_ = textField->GetIconSize();
    iconHotZoneSizeInDimension_ = textField->GetIconHotZoneSize();
    UpdatePasswordIcon(textField);

    double widthReserved = NormalizeToPx(widthReserved_);
    if (textField->GetIconImage() == iconSrc_ && textField->GetImageFill() == imageFill_ && widthReserved <= 0.0) {
        return;
    }
    imageFill_ = textField->GetImageFill();
    iconSrc_ = textField->GetIconImage();
    if (!iconSrc_.empty() || widthReserved > 0.0) {
        RefPtr<ImageComponent> imageComponent;
        if (iconSrc_.empty() && widthReserved > 0.0) {
            imageComponent = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::SEARCH_SVG);
        } else {
            imageComponent = AceType::MakeRefPtr<ImageComponent>(iconSrc_);
            imageComponent->SetImageFill(imageFill_);
        }
        imageComponent->SetWidth(textField->GetIconSize());
        imageComponent->SetHeight(textField->GetIconSize());
        if (textDirection_ == TextDirection::RTL) {
            imageComponent->SetMatchTextDirection(true);
            imageComponent->SetTextDirection(TextDirection::RTL);
        }

        iconImage_ = AceType::DynamicCast<RenderImage>(imageComponent->CreateRenderNode());
        if (!iconImage_) {
            return;
        }
        iconImage_->Attach(GetContext());
        iconImage_->SetDirectPaint(true);
        iconImage_->Update(imageComponent);
        AddChild(iconImage_);
    }
}

void RenderTextField::UpdatePasswordIcon(const RefPtr<TextFieldComponent>& textField)
{
    if (!IsSelectiveDevice()) {
        return;
    }
    if (!showPasswordIcon_) {
        renderShowIcon_.Reset();
        renderHideIcon_.Reset();
        return;
    }

    showIconSrc_ = textField->GetShowIconImage();
    hideIconSrc_ = textField->GetHideIconImage();

    // update show icon.
    RefPtr<ImageComponent> showImage;
    if (showIconSrc_.empty()) {
        showImage = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::SHOW_PASSWORD_SVG);
    } else {
        showImage = AceType::MakeRefPtr<ImageComponent>(showIconSrc_);
    }
    showImage->SetWidth(textField->GetIconSize());
    showImage->SetHeight(textField->GetIconSize());

    renderShowIcon_ = AceType::DynamicCast<RenderImage>(showImage->CreateRenderNode());
    if (!renderShowIcon_) {
        return;
    }
    renderShowIcon_->Attach(GetContext());
    renderShowIcon_->SetDirectPaint(true);
    renderShowIcon_->Update(showImage);
    AddChild(renderShowIcon_);

    // update hide icon.
    RefPtr<ImageComponent> hideImage;
    if (hideIconSrc_.empty()) {
        hideImage = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::HIDE_PASSWORD_SVG);
    } else {
        hideImage = AceType::MakeRefPtr<ImageComponent>(hideIconSrc_);
    }
    hideImage->SetWidth(textField->GetIconSize());
    hideImage->SetHeight(textField->GetIconSize());

    renderHideIcon_ = AceType::DynamicCast<RenderImage>(hideImage->CreateRenderNode());
    if (!renderHideIcon_) {
        return;
    }
    renderHideIcon_->Attach(GetContext());
    renderHideIcon_->SetDirectPaint(true);
    renderHideIcon_->Update(hideImage);
    AddChild(renderHideIcon_);
}

void RenderTextField::UpdateOverlay()
{
    // When textfield PerformLayout, update overlay.
    if (isOverlayShowed_ && updateHandlePosition_) {
        auto selStart = GetEditingValue().selection.GetStart();
        auto selEnd = GetEditingValue().selection.GetEnd();
        Rect caretStart;
        Rect caretEnd;
        bool startHandleVisible =
            GetCaretRect(selStart, caretStart) ? IsVisible(caretStart + textOffsetForShowCaret_) : false;
        bool endHandleVisible =
            (selStart == selEnd)
                ? startHandleVisible
                : (GetCaretRect(selEnd, caretEnd) ? IsVisible(caretEnd + textOffsetForShowCaret_) : false);

        OverlayShowOption option { .showMenu = isOverlayShowed_,
            .showStartHandle = startHandleVisible,
            .showEndHandle = endHandleVisible,
            .isSingleHandle = isSingleHandle_,
            .updateOverlayType = UpdateOverlayType::SCROLL,
            .startHandleOffset = GetPositionForExtend(selStart, isSingleHandle_),
            .endHandleOffset = GetPositionForExtend(selEnd, isSingleHandle_) };
        updateHandlePosition_(option);
        if (onClipRectChanged_) {
            onClipRectChanged_(innerRect_ + Size(HANDLE_HOT_ZONE, HANDLE_HOT_ZONE) + GetOffsetToPage() -
                               Offset(HANDLE_HOT_ZONE / 2.0, 0.0));
        }
    }
}

void RenderTextField::RegisterFontCallbacks()
{
    // Register callback for fonts.
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }
    auto callback = [textfield = AceType::WeakClaim(this)] {
        auto refPtr = textfield.Upgrade();
        if (refPtr) {
            refPtr->isCallbackCalled_ = true;
            refPtr->MarkNeedLayout();
        }
    };
    auto fontManager = pipelineContext->GetFontManager();
    if (fontManager) {
        for (const auto& familyName : style_.GetFontFamilies()) {
            fontManager->RegisterCallback(AceType::WeakClaim(this), familyName, callback);
        }
        fontManager->AddVariationNode(WeakClaim(this));
    }
}

void RenderTextField::OnStatusChanged(OHOS::Ace::RenderStatus renderStatus)
{
    hasFocus_ = renderStatus == RenderStatus::FOCUS;
    UpdateFocusStyles();
    MarkNeedLayout();

    if (!hasFocus_) {
        auto context = context_.Upgrade();
        if (!context) {
            return;
        }
        // Don't call cancel focus animation when next frame comes because then focus is switched, next node will
        // show focus immediately, we shouldn't cancel focus animation that time.
        context->CancelFocusAnimation();
    }
}

void RenderTextField::OnValueChanged(bool needFireChangeEvent, bool needFireSelectChangeEvent)
{
    isValueFromFront_ = !needFireChangeEvent;
    TextEditingValue temp = GetEditingValue();
    for (const auto& formatter : textInputFormatters_) {
        if (formatter) {
            formatter->Format(GetEditingValue(), temp);
        }
    }
    if (cursorPositionType_ == CursorPositionType::NORMAL && temp.selection.GetStart() == temp.selection.GetEnd()) {
        temp.selection.Update(AdjustCursorAndSelection(temp.selection.GetEnd()));
    }
    FireSelectChangeIfNeeded(temp, needFireSelectChangeEvent);
    SetEditingValue(std::move(temp), needFireChangeEvent);
    UpdateRemoteEditingIfNeeded(needFireChangeEvent);
    MarkNeedLayout();
}

void RenderTextField::FireSelectChangeIfNeeded(const TextEditingValue& newValue, bool needFireSelectChangeEvent) const
{
    if (needFireSelectChangeEvent && onSelectChangeEvent_ && newValue.selection != GetPreEditingValue().selection) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("start", newValue.selection.GetStart());
        jsonResult->Put("end", newValue.selection.GetEnd());
        onSelectChangeEvent_(std::string(R"("selectchange",)").append(jsonResult->ToString()));
    }
}

void RenderTextField::CursorMoveLeft(CursorMoveSkip skip)
{
    if (skip != CursorMoveSkip::CHARACTER) {
        // Not support yet.
        LOGE("move skip not support character yet");
        return;
    }
    if (GetEditingValue().selection.extentOffset > 0) {
        isValueFromRemote_ = false;
        auto value = GetEditingValue();
        value.MoveLeft();
        SetEditingValue(std::move(value));
    }
    cursorPositionType_ = CursorPositionType::NONE;
    MarkNeedLayout();
}

void RenderTextField::CursorMoveRight(CursorMoveSkip skip)
{
    if (skip != CursorMoveSkip::CHARACTER) {
        // Not support yet.
        LOGE("move skip not support character yet");
        return;
    }

    auto text = GetTextForDisplay(GetEditingValue().text);
    if (text.length() > static_cast<size_t>(GetEditingValue().selection.extentOffset)) {
        isValueFromRemote_ = false;
        auto value = GetEditingValue();
        value.MoveRight();
        SetEditingValue(std::move(value));
    }
    cursorPositionType_ = CursorPositionType::NONE;
    MarkNeedLayout();
}

void RenderTextField::CursorMoveUp()
{
    if (keyboard_ != TextInputType::MULTILINE) {
        return;
    }
    isValueFromRemote_ = false;
    auto value = GetEditingValue();
    value.MoveToPosition(GetCursorPositionForMoveUp());
    SetEditingValue(std::move(value));
    cursorPositionType_ = CursorPositionType::NONE;
    MarkNeedLayout();
}

void RenderTextField::CursorMoveDown()
{
    if (keyboard_ != TextInputType::MULTILINE) {
        return;
    }
    isValueFromRemote_ = false;
    auto value = GetEditingValue();
    value.MoveToPosition(GetCursorPositionForMoveDown());
    SetEditingValue(std::move(value));
    cursorPositionType_ = CursorPositionType::NONE;
    MarkNeedLayout();
}

void RenderTextField::CursorMoveOnClick(const Offset& offset)
{
    auto value = GetEditingValue();
    auto position = GetCursorPositionForClick(offset);
    value.MoveToPosition(position);
    SetEditingValue(std::move(value));

    if (!GetEditingValue().text.empty() && position == GetEditingValue().selection.GetEnd()) {
        OnValueChanged(true, false);
    }
}

void RenderTextField::UpdateSelection(int32_t both)
{
    UpdateSelection(both, both);
}

void RenderTextField::UpdateSelection(int32_t start, int32_t end)
{
    auto value = GetEditingValue();
    value.UpdateSelection(start, end);
    SetEditingValue(std::move(value));
    auto refPtr = accessibilityNode_.Upgrade();
    if (refPtr) {
        refPtr->SetTextSelectionStart(start);
        refPtr->SetTextSelectionEnd(end);
    }
}

void RenderTextField::UpdateRemoteEditing(bool needFireChangeEvent)
{
#if defined(ENABLE_STANDARD_INPUT)
    auto value = GetEditingValue();
    MiscServices::InputMethodController::GetInstance()->OnSelectionChange(
        StringUtils::Str8ToStr16(value.text), value.selection.GetStart(), value.selection.GetEnd());
#else
    if (!HasConnection()) {
        return;
    }
    connection_->SetEditingState(GetEditingValue(), GetInstanceId(), needFireChangeEvent);
#endif
}

void RenderTextField::UpdateRemoteEditingIfNeeded(bool needFireChangeEvent)
{
    if (!enabled_) {
        return;
    }
#if defined(ENABLE_STANDARD_INPUT)
    UpdateRemoteEditing(needFireChangeEvent);
#else
    if (!lastKnownRemoteEditingValue_ || GetEditingValue() != *lastKnownRemoteEditingValue_) {
        lastKnownRemoteEditingValue_ = std::make_shared<TextEditingValue>(GetEditingValue());
        UpdateRemoteEditing(needFireChangeEvent);
    }
#endif
}

void RenderTextField::ShowError(const std::string& errorText, bool resetToStart)
{
    errorText_ = errorText;
    if (!errorText.empty()) {
        auto refPtr = accessibilityNode_.Upgrade();
        if (refPtr) {
            refPtr->SetErrorText(errorText);
        }
    }

    if (!errorText.empty()) {
        ChangeBorderToErrorStyle();
    } else {
        if (decoration_) {
            decoration_->SetBorder(originBorder_);
        }
    }
    MarkNeedLayout();
}

void RenderTextField::SetOnValueChange(const std::function<void()>& onValueChange)
{
    onValueChange_ = onValueChange;
}

const std::function<void()>& RenderTextField::GetOnValueChange() const
{
    return onValueChange_;
}

void RenderTextField::SetOnKeyboardClose(const std::function<void(bool)>& onKeyboardClose)
{
    onKeyboardClose_ = onKeyboardClose;
}

void RenderTextField::SetOnClipRectChanged(const std::function<void(const Rect&)>& onClipRectChanged)
{
    onClipRectChanged_ = onClipRectChanged;
}

void RenderTextField::SetUpdateHandlePosition(const std::function<void(const OverlayShowOption&)>& updateHandlePosition)
{
    updateHandlePosition_ = updateHandlePosition;
}

void RenderTextField::SetUpdateHandleDiameter(const std::function<void(const double&)>& updateHandleDiameter)
{
    updateHandleDiameter_ = updateHandleDiameter;
}

void RenderTextField::SetUpdateHandleDiameterInner(const std::function<void(const double&)>& updateHandleDiameterInner)
{
    updateHandleDiameterInner_ = updateHandleDiameterInner;
}

void RenderTextField::SetIsOverlayShowed(bool isOverlayShowed, bool needStartTwinkling)
{
    isOverlayShowed_ = isOverlayShowed;
    // When pop overlay, reset selection and clear selected style.
    if (GetEditingValue().selection.GetStart() != GetEditingValue().selection.GetEnd()) {
        UpdateSelection(GetEditingValue().selection.GetEnd());
    }
    if (!isOverlayShowed_ && hasFocus_ && needStartTwinkling) {
        StartTwinkling();
    }
}

void RenderTextField::HandleOnCut()
{
    if (!clipboard_) {
        return;
    }
    clipboard_->SetData(GetEditingValue().GetSelectedText());
    if (onCut_) {
        onCut_(GetEditingValue().GetSelectedText());
    }
    auto value = GetEditingValue();
    value.text = value.GetBeforeSelection() + value.GetAfterSelection();
    value.UpdateSelection(GetEditingValue().selection.GetStart());
    SetEditingValue(std::move(value));
}

void RenderTextField::HandleOnCopy()
{
    if (!clipboard_) {
        return;
    }
    clipboard_->SetData(GetEditingValue().GetSelectedText());
    if (onCopy_) {
        onCopy_(GetEditingValue().GetSelectedText());
    }
    UpdateSelection(GetEditingValue().selection.GetEnd());
}

void RenderTextField::HandleOnPaste()
{
    if (!clipboard_) {
        return;
    }
    auto textSelection = GetEditingValue().selection;
    auto pasteCallback = [weak = WeakClaim(this), textSelection](const std::string& data) {
        auto textfield = weak.Upgrade();
        if (textfield) {
            auto value = textfield->GetEditingValue();
            value.selection = textSelection;
            value.text = value.GetBeforeSelection() + data + value.GetAfterSelection();
            value.UpdateSelection(textSelection.GetStart() + StringUtils::Str8ToStr16(data).length());
            textfield->SetEditingValue(std::move(value));
            if (textfield->onPaste_) {
                textfield->onPaste_(data);
            }
        }
    };
    clipboard_->GetData(pasteCallback);
}

void RenderTextField::HandleOnCopyAll(const std::function<void(const Offset&, const Offset&)>& callback)
{
    isSingleHandle_ = false;
    cursorPositionType_ = CursorPositionType::NORMAL;
    auto textSize = GetEditingValue().GetWideText().length();
    if (textSize == 0) {
        isSingleHandle_ = true;
    }
    UpdateSelection(0, textSize);
    if (callback) {
        callback(GetPositionForExtend(0, isSingleHandle_),
            GetPositionForExtend(GetEditingValue().GetWideText().length(), isSingleHandle_));
    }
}

void RenderTextField::HandleOnStartHandleMove(int32_t end, const Offset& startHandleOffset,
    const std::function<void(const Offset&)>& startCallback, bool isSingleHandle)
{
    Offset realOffset = startHandleOffset;
    if (startCallback) {
        UpdateStartSelection(end, realOffset, isSingleHandle, false);
        startCallback(GetHandleOffset(GetEditingValue().selection.GetStart()));
    }
}

void RenderTextField::HandleOnEndHandleMove(
    int32_t start, const Offset& endHandleOffset, const std::function<void(const Offset&)>& endCallback)
{
    Offset realOffset = endHandleOffset;
    if (endCallback) {
        UpdateEndSelection(start, realOffset);
        endCallback(GetHandleOffset(GetEditingValue().selection.GetEnd()));
    }
}

RefPtr<StackElement> RenderTextField::GetLastStack() const
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Context is nullptr");
        return nullptr;
    }
    return context->GetLastStack();
}

bool RenderTextField::HandleKeyEvent(const KeyEvent& event)
{
    std::string appendElement;
    if (event.action == KeyAction::DOWN) {
        if (event.IsNumberKey()) {
            appendElement = event.ConvertCodeToString();
        } else if (event.IsLetterKey()) {
            if (event.IsKey({ KeyCode::KEY_CTRL_LEFT, KeyCode::KEY_A }) ||
                event.IsKey({ KeyCode::KEY_CTRL_RIGHT, KeyCode::KEY_A })) {
                HandleOnCopyAll(nullptr);
            } else if (event.IsKey({ KeyCode::KEY_CTRL_LEFT, KeyCode::KEY_C }) ||
                       event.IsKey({ KeyCode::KEY_CTRL_RIGHT, KeyCode::KEY_C })) {
                HandleOnCopy();
            } else if (event.IsKey({ KeyCode::KEY_CTRL_LEFT, KeyCode::KEY_V }) ||
                       event.IsKey({ KeyCode::KEY_CTRL_RIGHT, KeyCode::KEY_V })) {
                HandleOnPaste();
            } else if (event.IsKey({ KeyCode::KEY_CTRL_LEFT, KeyCode::KEY_X }) ||
                       event.IsKey({ KeyCode::KEY_CTRL_RIGHT, KeyCode::KEY_X })) {
                HandleOnCut();
            } else {
                appendElement = event.ConvertCodeToString();
            }
        }
        MarkNeedLayout();
    }
    if (appendElement.empty()) {
        return false;
    }
    auto value = GetEditingValue();
    value.text = value.GetBeforeSelection() + appendElement + value.GetAfterSelection();
    value.UpdateSelection(
        std::max(GetEditingValue().selection.GetEnd(), 0) + StringUtils::Str8ToStr16(appendElement).length());
    SetEditingValue(std::move(value));
    MarkNeedLayout();
    return true;
}

void RenderTextField::UpdateAccessibilityAttr()
{
    auto refPtr = accessibilityNode_.Upgrade();
    if (!refPtr) {
        LOGW("RenderTextField accessibilityNode is null.");
        return;
    }

    refPtr->SetHintText(placeholder_);
    refPtr->SetMaxTextLength(maxLength_);
    refPtr->SetEditable(enabled_);
    refPtr->SetClickableState(true);
    refPtr->SetLongClickableState(true);
    if (maxLines_ > 1) {
        refPtr->SetIsMultiLine(true);
    }
    if (controller_) {
        refPtr->SetText(controller_->GetText());
    }
    switch (keyboard_) {
        case TextInputType::TEXT:
            refPtr->SetTextInputType(AceTextCategory::INPUT_TYPE_TEXT);
            break;
        case TextInputType::NUMBER:
            refPtr->SetTextInputType(AceTextCategory::INPUT_TYPE_NUMBER);
            break;
        case TextInputType::DATETIME:
            refPtr->SetTextInputType(AceTextCategory::INPUT_TYPE_DATE);
            break;
        case TextInputType::EMAIL_ADDRESS:
            refPtr->SetTextInputType(AceTextCategory::INPUT_TYPE_EMAIL);
            break;
        case TextInputType::VISIBLE_PASSWORD:
            refPtr->SetTextInputType(AceTextCategory::INPUT_TYPE_PASSWORD);
            break;
        default:
            break;
    }
}

void RenderTextField::InitAccessibilityEventListener()
{
    const auto& accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->AddSupportAction(AceAction::ACTION_CLICK);
    accessibilityNode->SetActionClickImpl([weakPtr = WeakClaim(this)]() {
        const auto& textField = weakPtr.Upgrade();
        if (textField) {
            textField->OnClick(ClickInfo(0));
        }
    });

    accessibilityNode->AddSupportAction(AceAction::ACTION_LONG_CLICK);
    accessibilityNode->SetActionLongClickImpl([weakPtr = WeakClaim(this)]() {
        const auto& textField = weakPtr.Upgrade();
        if (textField) {
            textField->OnLongPress(LongPressInfo(0));
        }
    });
}

void RenderTextField::UpdateDirectionStatus()
{
    directionStatus_ = static_cast<DirectionStatus>(
        (static_cast<uint8_t>(textDirection_) << 1) | static_cast<uint8_t>(realTextDirection_));
}

void RenderTextField::UpdateStartSelection(int32_t end, const Offset& pos, bool isSingleHandle, bool isLongPress)
{
    int32_t extend = GetCursorPositionForClick(pos);
    int32_t extendEnd = GetEditingValue().selection.GetEnd();
    if (isLongPress) {
        // Use custom selection if exist, otherwise select content near finger.
        if (selection_.IsValid()) {
            UpdateSelection(selection_.baseOffset, selection_.extentOffset);
        } else {
            extendEnd = extend + GetGraphemeClusterLength(extend, false);
            UpdateSelection(extend, extendEnd);
        }
        return;
    }
    if (isSingleHandle) {
        UpdateSelection(extend);
    } else {
        UpdateSelection(extend, end);
    }
}

void RenderTextField::UpdateEndSelection(int32_t start, const Offset& pos)
{
    int32_t extend = GetCursorPositionForClick(pos);
    UpdateSelection(start, extend);
}

Offset RenderTextField::GetPositionForExtend(int32_t extend, bool isSingleHandle)
{
    if (extend < 0) {
        extend = 0;
    }
    if (static_cast<size_t>(extend) > GetEditingValue().GetWideText().length()) {
        extend = static_cast<int32_t>(GetEditingValue().GetWideText().length());
    }
    return GetHandleOffset(extend);
}

int32_t RenderTextField::GetGraphemeClusterLength(int32_t extend, bool isPrefix) const
{
    auto text = GetTextForDisplay(GetEditingValue().text);
    char16_t aroundChar = 0;
    if (isPrefix) {
        if (static_cast<size_t>(extend) <= text.length()) {
            aroundChar = text[std::max(0, extend - 1)];
        }
    } else {
        if (static_cast<size_t>(extend) < (text.length())) {
            aroundChar = text[std::min(static_cast<int32_t>(text.length() - 1), extend)];
        }
    }
    return StringUtils::NotInUtf16Bmp(aroundChar) ? 2 : 1;
}

bool RenderTextField::ShowCounter() const
{
    return showCounter_ && maxLength_ < std::numeric_limits<uint32_t>::max();
}

void RenderTextField::ChangeCounterStyle(const TextEditingValue& value)
{
    if (!ShowCounter()) {
        return;
    }
    if (value.GetWideText().size() > maxLength_) {
        overCount_ = true;
        ChangeBorderToErrorStyle();
    } else if (value.GetWideText().size() < maxLength_) {
        overCount_ = false;
        if (decoration_) {
            decoration_->SetBorder(originBorder_);
        }
    }
}

void RenderTextField::ChangeBorderToErrorStyle()
{
    if (!decoration_) {
        decoration_ = AceType::MakeRefPtr<Decoration>();
    }
    const auto& border = decoration_->GetBorder();
    BorderEdge errorBorderEdge(errorBorderColor_, errorBorderWidth_, BorderStyle::SOLID);
    Border errorBorder;
    if (!border.Left().HasValue() && !border.Top().HasValue() && !border.Right().HasValue() &&
        border.Bottom().HasValue()) {
        // Change over count style for linear input.
        errorBorder = Border(BorderEdge(), BorderEdge(), BorderEdge(), errorBorderEdge);
    } else {
        errorBorder = Border(errorBorderEdge);
    }
    errorBorder.SetBorderRadius(decoration_->GetBorder().TopLeftRadius());
    decoration_->SetBorder(errorBorder);
}

void RenderTextField::HandleDeviceOrientationChange()
{
    if (deviceOrientation_ != SystemProperties::GetDevcieOrientation()) {
        deviceOrientation_ = SystemProperties::GetDevcieOrientation();
        if (isOverlayShowed_) {
            onKeyboardClose_ = nullptr;
            PopTextOverlay();
            StartTwinkling();
        }
    }
}

void RenderTextField::OnHiddenChanged(bool hidden)
{
    if (hidden) {
        CloseKeyboard();
        PopTextOverlay();
    }
}

void RenderTextField::OnAppHide()
{
    RenderNode::OnAppHide();
    OnHiddenChanged(true);
}

void RenderTextField::OnOverlayFocusChange(bool isFocus, bool needCloseKeyboard)
{
    isOverlayFocus_ = isFocus;
    if (needCloseKeyboard && onOverlayFocusChange_) {
        onOverlayFocusChange_(isFocus);
    }
}

int32_t RenderTextField::GetInstanceId() const
{
    auto context = context_.Upgrade();
    if (context) {
        return context->GetInstanceId();
    }
    return 0;
}

void RenderTextField::Insert(const std::string& text)
{
    auto context = context_.Upgrade();
    if (context) {
        context->GetTaskExecutor()->PostTask(
            [weakPtr = WeakClaim(this), text] {
                const auto& textField = weakPtr.Upgrade();
                auto value = textField->GetEditingValue();
                auto textEditingValue = std::make_shared<TextEditingValue>();
                textEditingValue->text = value.GetBeforeSelection() + text + value.GetAfterSelection();
                textEditingValue->UpdateSelection(std::max(value.selection.GetStart(), 0) + text.length());
                textField->UpdateEditingValue(textEditingValue, true);
            },
            TaskExecutor::TaskType::UI);
    }
}

void RenderTextField::Delete(int32_t start, int32_t end)
{
    auto value = GetEditingValue();
    value.Delete(start, end);
    SetEditingValue(std::move(value));
    if (onValueChange_) {
        onValueChange_();
    }
}

void RenderTextField::PopTextOverlay()
{
    const auto& stackElement = stackElement_.Upgrade();
    if (stackElement) {
        stackElement->PopTextOverlay();
    }
    isOverlayShowed_ = false;
}

std::string RenderTextField::ProvideRestoreInfo()
{
    if (!onIsCurrentFocus_ || !onIsCurrentFocus_()) {
        return "";
    }
    auto value = GetEditingValue();
    auto jsonObj = JsonUtil::Create(true);
    jsonObj->Put("start", value.selection.GetStart());
    jsonObj->Put("end", value.selection.GetEnd());
    return jsonObj->ToString();
}

void RenderTextField::ApplyRestoreInfo()
{
    if (GetRestoreInfo().empty()) {
        return;
    }
    auto info = JsonUtil::ParseJsonString(GetRestoreInfo());
    if (!info->IsValid() || !info->IsObject()) {
        LOGW("RenderTextField:: restore info is invalid");
        return;
    }
    auto jsonStart = info->GetValue("start");
    auto jsonEnd = info->GetValue("end");
    UpdateSelection(jsonStart->GetInt(), jsonEnd->GetInt());
    StartTwinkling();
    SetRestoreInfo("");
}

} // namespace OHOS::Ace
