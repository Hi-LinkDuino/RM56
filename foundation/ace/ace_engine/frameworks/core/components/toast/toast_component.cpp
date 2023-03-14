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

#include "core/components/toast/toast_component.h"

#include <atomic>

#include "base/utils/string_utils.h"
#include "base/utils/system_properties.h"
#include "core/components/align/align_component.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/flex/flex_component.h"
#include "core/components/positioned/positioned_component.h"
#include "core/components/stage/stage_element.h"
#include "core/components/tween/tween_component.h"

namespace OHOS::Ace {
namespace {

constexpr double START_FRAME_TIME = 0.0;
constexpr double END_FRAME_TIME = 0.9;
constexpr double START_FRAME_OPACITY = 0.0;
constexpr double MID_FRAME_OPACITY = 1.0;
constexpr double END_FRAME_OPACITY = 0.0;
constexpr float TOAST_ANIMATION_TIME = 100.0f;
constexpr char TOAST_TWEEN_NAME[] = "toast";

} // namespace

ToastComponent::ToastComponent() = default;
ToastComponent::~ToastComponent() = default;

static std::atomic<int32_t> g_toastId(1);

int32_t ToastComponent::GenerateNextToastId()
{
    return g_toastId.fetch_add(1, std::memory_order_relaxed);
}

void ToastComponent::InitToastAnimation()
{
    if (NearZero(toastDurationTime_)) {
        return;
    }
    float midFrameTime = TOAST_ANIMATION_TIME / toastDurationTime_;
    float stopFrameTime = END_FRAME_TIME - TOAST_ANIMATION_TIME / toastDurationTime_;
    auto opacityKeyframeStart = AceType::MakeRefPtr<Keyframe<float>>(START_FRAME_TIME, START_FRAME_OPACITY);
    auto opacityKeyframeMid = AceType::MakeRefPtr<Keyframe<float>>(midFrameTime, MID_FRAME_OPACITY);
    opacityKeyframeMid->SetCurve(Curves::FRICTION);
    auto opacityKeyframeStop = AceType::MakeRefPtr<Keyframe<float>>(stopFrameTime, MID_FRAME_OPACITY);
    opacityKeyframeStop->SetCurve(Curves::LINEAR);
    auto opacityKeyframeEnd = AceType::MakeRefPtr<Keyframe<float>>(END_FRAME_TIME, END_FRAME_OPACITY);
    opacityKeyframeEnd->SetCurve(Curves::FRICTION);
    auto opacityAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimation->AddKeyframe(opacityKeyframeStart);
    opacityAnimation->AddKeyframe(opacityKeyframeMid);
    opacityAnimation->AddKeyframe(opacityKeyframeStop);
    opacityAnimation->AddKeyframe(opacityKeyframeEnd);
    tweenOption_.SetOpacityAnimation(opacityAnimation);
    tweenOption_.SetDuration(toastDurationTime_);
    tweenOption_.SetFillMode(FillMode::FORWARDS);
}

void ToastComponent::BuildToastContent(const RefPtr<TextComponent>& text, const RefPtr<ToastTheme>& toastTheme)
{
    if (!text || !toastTheme) {
        return;
    }
    TextStyle toastTextStyle = toastTheme->GetTextStyle();
    auto deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::WATCH) {
        toastTextStyle.SetAdaptTextSize(toastTextStyle.GetFontSize(), toastTheme->GetMinFontSize());
        toastTextStyle.SetMaxLines(toastTheme->GetTextMaxLines());
        toastTextStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
        toastTextStyle.SetTextAlign(TextAlign::CENTER);
    }
    text->SetTextStyle(toastTextStyle);
}

void ToastComponent::BuildPackageBox(const RefPtr<PipelineContext>& context, const RefPtr<BoxComponent>& box,
    const RefPtr<TextComponent>& text, const RefPtr<ToastTheme>& toastTheme)
{
    if (!context || !box || !text || !toastTheme) {
        return;
    }
    // create base box for background of toast
    RefPtr<BoxComponent> baseBox = AceType::MakeRefPtr<BoxComponent>();
    // baseBox set back decoration
    RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetBackgroundColor(toastTheme->GetBackgroundColor());
    backDecoration->AddShadow(ShadowConfig::DefaultShadowL);
    Border border;
    border.SetBorderRadius(toastTheme->GetRadius());
    backDecoration->SetBorder(border);
    baseBox->SetBackDecoration(backDecoration);
    // baseBox set padding
    baseBox->SetPadding(toastTheme->GetPadding());
    auto deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::WATCH) {
        // baseBox set constraints
        LayoutParam constraints;
        constraints.SetMinSize(Size(
            context->NormalizeToPx(toastTheme->GetMinWidth()), context->NormalizeToPx(toastTheme->GetMinHeight())));
        constraints.SetMaxSize(Size(Size::INFINITE_SIZE, Size::INFINITE_SIZE));
        baseBox->SetConstraints(constraints);
    } else {
        baseBox->SetGridLayoutInfo(GridSystemManager::GetInstance().GetInfoByType(GridColumnType::TOAST));
    }
    // baseBox set child
    if (deviceType == DeviceType::WATCH) {
        baseBox->SetMargin(toastTheme->GetMarging());
        baseBox->SetChild(text);
    } else {
        // Single line center alignment, multiple lines Left alignment.
        std::list<RefPtr<Component>> rowChildren;
        rowChildren.emplace_back(text);
        RefPtr<RowComponent> row = AceType::MakeRefPtr<RowComponent>(FlexAlign::CENTER, FlexAlign::CENTER, rowChildren);
        row->SetMainAxisSize(MainAxisSize::MIN);
        baseBox->SetAlignment(Alignment::CENTER);
        baseBox->SetChild(row);
    }

    box->SetFlex(BoxFlex::FLEX_X);
    box->SetChild(baseBox);
}

void ToastComponent::Show(const RefPtr<PipelineContext>& context, const std::string& message, int32_t duration,
    const std::string& bottom, bool isRightToLeft)
{
    if (!context) {
        LOGE("fail to show toast due to context is null");
        return;
    }
    auto stackElement = context->GetLastStack();
    if (!stackElement) {
        return;
    }

    auto themeManager = context->GetThemeManager();
    if (!themeManager) {
        return;
    }
    auto toastTheme = themeManager->GetTheme<ToastTheme>();
    if (!toastTheme) {
        return;
    }

    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>(message);
    text->SetTextDirection((isRightToLeft ? TextDirection::RTL : TextDirection::LTR));
    BuildToastContent(text, toastTheme);
    RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
    BuildPackageBox(context, box, text, toastTheme);

    int32_t toastId = GenerateNextToastId();
    auto deviceType = SystemProperties::GetDeviceType();
    // get toast animation playing time
    toastDurationTime_ = duration;
    Dimension bottomPosition = StringUtils::StringToDimension(bottom);
    RefPtr<TweenComponent> tween =
        AceType::MakeRefPtr<TweenComponent>(TweenComponent::AllocTweenComponentId(), TOAST_TWEEN_NAME, box);
    InitToastAnimation();
    tween->SetTweenOption(tweenOption_);
    tween->SetAnimationOperation(AnimationOperation::PLAY);
    // to prevent flicking when play animation
    tween->SetIsFirstFrameShow(false);

    // prevent layer from clipping shadows
    tween->SetShadow(ShadowConfig::DefaultShadowL);

    if (deviceType == DeviceType::WATCH) {
        // center alignment
        std::list<RefPtr<Component>> alignChildren;
        alignChildren.emplace_back(tween);
        RefPtr<AlignComponent> align = AceType::MakeRefPtr<AlignComponent>(alignChildren, Alignment::CENTER);
        stackElement->PushToastComponent(align, toastId);
    } else {
        RefPtr<PositionedComponent> positioned = AceType::MakeRefPtr<PositionedComponent>(tween);
        positioned->SetBottom(bottomPosition.IsValid() ? bottomPosition : toastTheme->GetBottom());
        stackElement->PushToastComponent(positioned, toastId);
    }

    WeakPtr<StackElement> weak = stackElement;
    context->GetTaskExecutor()->PostDelayedTask([weak, toastId] {
            auto ref = weak.Upgrade();
            if (ref == nullptr) {
                return;
            }
            ref->PopToastComponent(toastId);
        },
        TaskExecutor::TaskType::UI, duration);
}

} // namespace OHOS::Ace
