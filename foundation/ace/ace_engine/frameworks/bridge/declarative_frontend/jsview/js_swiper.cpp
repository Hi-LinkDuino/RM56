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

#include "frameworks/bridge/declarative_frontend/jsview/js_swiper.h"

#include <algorithm>
#include <iterator>

#include "bridge/common/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/swiper/swiper_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t DEFAULT_SWIPER_CACHED_COUNT = 1;

JSRef<JSVal> SwiperChangeEventToJSValue(const SwiperChangeEvent& eventInfo)
{
    return JSRef<JSVal>::Make(ToJSValue(eventInfo.GetIndex()));
}

} // namespace

void JSSwiper::Create(const JSCallbackInfo& info)
{
    std::list<RefPtr<OHOS::Ace::Component>> componentChildren;
    RefPtr<OHOS::Ace::SwiperComponent> component = AceType::MakeRefPtr<OHOS::Ace::SwiperComponent>(componentChildren);
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSSwiperController* jsController = JSRef<JSObject>::Cast(info[0])->Unwrap<JSSwiperController>();
        if (jsController) {
            jsController->SetController(component->GetSwiperController());
        }
    }
    component->SetIndicator(InitIndicatorStyle());
    component->SetMainSwiperSize(MainSwiperSize::MIN);
    component->SetCachedSize(DEFAULT_SWIPER_CACHED_COUNT);
    component->SetCurve(Curves::LINEAR);
    ViewStackProcessor::GetInstance()->Push(component);
    JSInteractableView::SetFocusNode(true);
}

void JSSwiper::JsRemoteMessage(const JSCallbackInfo& info)
{
    EventMarker remoteMessageEventId;
    JSInteractableView::JsRemoteMessage(info, remoteMessageEventId);
    auto stack = ViewStackProcessor::GetInstance();
    auto swiperComponent = AceType::DynamicCast<SwiperComponent>(stack->GetMainComponent());
    if (!swiperComponent) {
        LOGE("swiperComponent is null");
        return;
    }
    swiperComponent->SetRemoteMessageEventId(remoteMessageEventId);
}

void JSSwiper::JSBind(BindingTarget globalObj)
{
    JSClass<JSSwiper>::Declare("Swiper");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSSwiper>::StaticMethod("create", &JSSwiper::Create, opt);
    JSClass<JSSwiper>::StaticMethod("autoPlay", &JSSwiper::SetAutoplay, opt);
    JSClass<JSSwiper>::StaticMethod("digital", &JSSwiper::SetDigital, opt);
    JSClass<JSSwiper>::StaticMethod("duration", &JSSwiper::SetDuration, opt);
    JSClass<JSSwiper>::StaticMethod("index", &JSSwiper::SetIndex, opt);
    JSClass<JSSwiper>::StaticMethod("interval", &JSSwiper::SetInterval, opt);
    JSClass<JSSwiper>::StaticMethod("loop", &JSSwiper::SetLoop, opt);
    JSClass<JSSwiper>::StaticMethod("vertical", &JSSwiper::SetVertical, opt);
    JSClass<JSSwiper>::StaticMethod("indicator", &JSSwiper::SetIndicator, opt);
    JSClass<JSSwiper>::StaticMethod("cancelSwipeOnOtherAxis", &JSSwiper::SetCancelSwipeOnOtherAxis, opt);
    JSClass<JSSwiper>::StaticMethod("displayMode", &JSSwiper::SetDisplayMode);
    JSClass<JSSwiper>::StaticMethod("effectMode", &JSSwiper::SetEffectMode);
    JSClass<JSSwiper>::StaticMethod("displayCount", &JSSwiper::SetDisplayCount);
    JSClass<JSSwiper>::StaticMethod("itemSpace", &JSSwiper::SetItemSpace);
    JSClass<JSSwiper>::StaticMethod("cachedCount", &JSSwiper::SetCachedCount);
    JSClass<JSSwiper>::StaticMethod("curve", &JSSwiper::SetCurve);
    JSClass<JSSwiper>::StaticMethod("onChange", &JSSwiper::SetOnChange);
    JSClass<JSSwiper>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSSwiper>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSSwiper>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSSwiper>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSSwiper>::StaticMethod("remoteMessage", &JSSwiper::JsRemoteMessage);
    JSClass<JSSwiper>::StaticMethod("onClick", &JSSwiper::SetOnClick);
    JSClass<JSSwiper>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSSwiper>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSSwiper>::StaticMethod("indicatorStyle", &JSSwiper::SetIndicatorStyle);
    JSClass<JSSwiper>::StaticMethod("enabled", &JSSwiper::SetEnabled);
    JSClass<JSSwiper>::StaticMethod("disableSwipe", &JSSwiper::SetDisableSwipe);
    JSClass<JSSwiper>::StaticMethod("height", &JSSwiper::SetHeight);
    JSClass<JSSwiper>::StaticMethod("width", &JSSwiper::SetWidth);
    JSClass<JSSwiper>::StaticMethod("size", &JSSwiper::SetSize);
    JSClass<JSSwiper>::Inherit<JSContainerBase>();
    JSClass<JSSwiper>::Inherit<JSViewAbstract>();
    JSClass<JSSwiper>::Bind<>(globalObj);
}

void JSSwiper::SetAutoplay(bool autoPlay)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetAutoPlay(autoPlay);
    }
}

void JSSwiper::SetEnabled(const JSCallbackInfo& info)
{
    JSViewAbstract::JsEnabled(info);
    if (info.Length() < 1) {
        LOGE("The info is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    if (!info[0]->IsBoolean()) {
        LOGE("info is not bool.");
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetDisabledStatus(!(info[0]->ToBoolean()));
    }
}

void JSSwiper::SetDisableSwipe(bool disable)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->DisableSwipe(disable);
    }
}

void JSSwiper::SetEffectMode(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The info is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (!swiper) {
        return;
    }

    if (!info[0]->IsNumber()) {
        LOGE("info is not a  number ");
        return;
    }
    auto swiperMode = static_cast<EdgeEffect>(info[0]->ToNumber<int32_t>());
    if (swiperMode == EdgeEffect::SPRING) {
        swiper->SetEdgeEffect(EdgeEffect::SPRING);
    } else if (swiperMode == EdgeEffect::FADE) {
        swiper->SetEdgeEffect(EdgeEffect::FADE);
    } else {
        swiper->SetEdgeEffect(EdgeEffect::NONE);
    }
}

void JSSwiper::SetDisplayCount(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The info is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (!swiper) {
        return;
    }

    if (info[0]->ToString() == "auto") {
        swiper->SetDisplayMode(SwiperDisplayMode::AUTO_LINEAR);
    }

    if (info[0]->IsNumber()) {
        swiper->SetDisplayCount(info[0]->ToNumber<int32_t>());
    }
}

void JSSwiper::SetDigital(bool digitalIndicator)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetDigitalIndicator(digitalIndicator);
    }
}

void JSSwiper::SetDuration(double duration)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetDuration(duration);
    }
}

void JSSwiper::SetIndex(uint32_t index)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetIndex(index);
    }
}

void JSSwiper::SetInterval(double interval)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetAutoPlayInterval(interval);
    }
}

void JSSwiper::SetLoop(bool loop)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetLoop(loop);
    }
}

void JSSwiper::SetVertical(bool isVertical)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetAxis(isVertical ? Axis::VERTICAL : Axis::HORIZONTAL);
    }
}

void JSSwiper::SetIndicator(bool showIndicator)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetShowIndicator(showIndicator);
        if (!showIndicator) {
            swiper->SetIndicator(nullptr);
        }
    }
}

void JSSwiper::SetCancelSwipeOnOtherAxis(bool cancel)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
}

void JSSwiper::SetIndicatorStyle(const JSCallbackInfo& info)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        auto indictor = swiper->GetIndicator();
        if (!indictor) {
            return;
        }

        if (info[0]->IsObject()) {
            JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
            JSRef<JSVal> leftValue = obj->GetProperty("left");
            JSRef<JSVal> topValue = obj->GetProperty("top");
            JSRef<JSVal> rightValue = obj->GetProperty("right");
            JSRef<JSVal> bottomValue = obj->GetProperty("bottom");
            JSRef<JSVal> sizeValue = obj->GetProperty("size");
            JSRef<JSVal> maskValue = obj->GetProperty("mask");
            JSRef<JSVal> colorValue = obj->GetProperty("color");
            JSRef<JSVal> selectedColorValue = obj->GetProperty("selectedColor");

            Dimension dimLeft;
            if (ParseJsDimensionPx(leftValue, dimLeft)) {
                indictor->SetLeft(dimLeft);
            }
            Dimension dimTop;
            if (ParseJsDimensionPx(topValue, dimTop)) {
                indictor->SetTop(dimTop);
            }
            Dimension dimRight;
            if (ParseJsDimensionPx(rightValue, dimRight)) {
                indictor->SetRight(dimRight);
            }
            Dimension dimBottom;
            if (ParseJsDimensionPx(bottomValue, dimBottom)) {
                indictor->SetBottom(dimBottom);
            }
            Dimension dimSize;
            if (ParseJsDimensionPx(sizeValue, dimSize)) {
                indictor->SetSize(dimSize);
            }
            if (maskValue->IsBoolean()) {
                auto mask = maskValue->ToBoolean();
                indictor->SetIndicatorMask(mask);
            }
            Color colorVal;
            if (ParseJsColor(colorValue, colorVal)) {
                indictor->SetColor(colorVal);
            }
            Color selectedColorVal;
            if (ParseJsColor(selectedColorValue, selectedColorVal)) {
                indictor->SetSelectedColor(selectedColorVal);
            }
        }
    }
    info.ReturnSelf();
}

void JSSwiper::SetItemSpace(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetItemSpace(value);
    }
}

void JSSwiper::SetDisplayMode(int32_t index)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetDisplayMode(static_cast<SwiperDisplayMode>(index));
    }
}

void JSSwiper::SetCachedCount(int32_t cachedCount)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        swiper->SetCachedSize(cachedCount);
    }
}

void JSSwiper::SetCurve(const std::string& curveStr)
{
    RefPtr<Curve> curve = CreateCurve(curveStr);

    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    swiper->SetCurve(curve);
}

void JSSwiper::SetOnChange(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto changeHandler = AceType::MakeRefPtr<JsEventFunction<SwiperChangeEvent, 1>>(
            JSRef<JSFunc>::Cast(args[0]), SwiperChangeEventToJSValue);
        auto onChange = EventMarker([executionContext = args.GetExecutionContext(), func = std::move(changeHandler)](
                                        const BaseEventInfo* info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(executionContext);
            auto swiperInfo = TypeInfoHelper::DynamicCast<SwiperChangeEvent>(info);
            if (!swiperInfo) {
                LOGE("HandleChangeEvent swiperInfo == nullptr");
                return;
            }
            ACE_SCORING_EVENT("Swiper.OnChange");
            func->Execute(*swiperInfo);
        });
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
        if (swiper) {
            swiper->SetChangeEventId(onChange);
        }
    }
    args.ReturnSelf();
}

void JSSwiper::SetOnClick(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
        if (swiper) {
            swiper->SetClickEventId(JSInteractableView::GetClickEventMarker(args));
        }
    }
    args.SetReturnValue(args.This());
}

RefPtr<OHOS::Ace::SwiperIndicator> JSSwiper::InitIndicatorStyle()
{
    auto indicator = AceType::MakeRefPtr<OHOS::Ace::SwiperIndicator>();
    auto indicatorTheme = GetTheme<SwiperIndicatorTheme>();
    if (indicatorTheme) {
        indicator->InitStyle(indicatorTheme);
    }
    return indicator;
}

void JSSwiper::SetWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    SetWidth(info[0]);
}

void JSSwiper::SetWidth(const JSRef<JSVal>& jsValue)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        JSViewAbstract::JsWidth(jsValue);
        if (swiper->GetMainSwiperSize() == MainSwiperSize::MAX ||
            swiper->GetMainSwiperSize() == MainSwiperSize::MAX_Y) {
            swiper->SetMainSwiperSize(MainSwiperSize::MAX);
        } else {
            swiper->SetMainSwiperSize(MainSwiperSize::MAX_X);
        }
    }
}

void JSSwiper::SetHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    SetHeight(info[0]);
}

void JSSwiper::SetHeight(const JSRef<JSVal>& jsValue)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto swiper = AceType::DynamicCast<OHOS::Ace::SwiperComponent>(component);
    if (swiper) {
        JSViewAbstract::JsHeight(jsValue);
        if (swiper->GetMainSwiperSize() == MainSwiperSize::MAX ||
            swiper->GetMainSwiperSize() == MainSwiperSize::MAX_X) {
            swiper->SetMainSwiperSize(MainSwiperSize::MAX);
        } else {
            swiper->SetMainSwiperSize(MainSwiperSize::MAX_Y);
        }
    }
}

void JSSwiper::SetSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    if (!info[0]->IsObject()) {
        LOGE("arg is not Object or String.");
        return;
    }

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    SetWidth(sizeObj->GetProperty("width"));
    SetHeight(sizeObj->GetProperty("height"));
}

void JSSwiperController::JSBind(BindingTarget globalObj)
{
    JSClass<JSSwiperController>::Declare("SwiperController");
    JSClass<JSSwiperController>::CustomMethod("showNext", &JSSwiperController::ShowNext);
    JSClass<JSSwiperController>::CustomMethod("finishAnimation", &JSSwiperController::FinishAnimation);
    JSClass<JSSwiperController>::CustomMethod("showPrevious", &JSSwiperController::ShowPrevious);
    JSClass<JSSwiperController>::Bind(globalObj, JSSwiperController::Constructor, JSSwiperController::Destructor);
}

void JSSwiperController::Constructor(const JSCallbackInfo& args)
{
    auto scroller = Referenced::MakeRefPtr<JSSwiperController>();
    scroller->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(scroller));
}

void JSSwiperController::Destructor(JSSwiperController* scroller)
{
    if (scroller != nullptr) {
        scroller->DecRefCount();
    }
}

void JSSwiperController::FinishAnimation(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(args[0]));
        auto eventMarker = EventMarker([execCtx = args.GetExecutionContext(), func = std::move(jsFunc)]() {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("Swiper.finishAnimation");
            func->Execute();
        });
        if (controller_) {
            controller_->SetFinishCallback(eventMarker);
        }
    }
    if (controller_) {
        controller_->FinishAnimation();
    }
    args.ReturnSelf();
}

} // namespace OHOS::Ace::Framework
