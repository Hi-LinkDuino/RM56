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

#include "frameworks/bridge/declarative_frontend/jsview/js_page_transition.h"

#include "frameworks/bridge/declarative_frontend/engine/functions/js_page_transition_function.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSPageTransition::JSBind(BindingTarget globalObj)
{
    JSClass<JSPageTransition>::Declare("PageTransition");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSPageTransition>::StaticMethod("create", &JSPageTransition::Create, opt);
    JSClass<JSPageTransition>::StaticMethod("pop", &JSPageTransition::Pop);
    JSClass<JSPageTransition>::Bind<>(globalObj);

    JSClass<JSPageTransitionEnter>::Declare("PageTransitionEnter");
    JSClass<JSPageTransitionEnter>::StaticMethod("create", &JSPageTransitionEnter::Create, opt);
    JSClass<JSPageTransitionEnter>::StaticMethod("slide", &JSPageTransition::Slide);
    JSClass<JSPageTransitionEnter>::StaticMethod("translate", &JSPageTransition::Translate);
    JSClass<JSPageTransitionEnter>::StaticMethod("scale", &JSPageTransition::Scale);
    JSClass<JSPageTransitionEnter>::StaticMethod("opacity", &JSPageTransition::Opacity);
    JSClass<JSPageTransitionEnter>::StaticMethod("onEnter", &JSPageTransition::JsHandlerOnEnter);
    JSClass<JSPageTransitionEnter>::Bind<>(globalObj);

    JSClass<JSPageTransitionExit>::Declare("PageTransitionExit");
    JSClass<JSPageTransitionExit>::StaticMethod("create", &JSPageTransitionExit::Create, opt);
    JSClass<JSPageTransitionExit>::StaticMethod("slide", &JSPageTransition::Slide);
    JSClass<JSPageTransitionExit>::StaticMethod("translate", &JSPageTransition::Translate);
    JSClass<JSPageTransitionExit>::StaticMethod("scale", &JSPageTransition::Scale);
    JSClass<JSPageTransitionExit>::StaticMethod("opacity", &JSPageTransition::Opacity);
    JSClass<JSPageTransitionExit>::StaticMethod("onExit", &JSPageTransition::JsHandlerOnExit);
    JSClass<JSPageTransitionExit>::Bind<>(globalObj);
}

void JSPageTransition::Slide(const JSCallbackInfo& info)
{
    LOGD("JSPageTransition::Slide");
    if (info.Length() > 0 && info[0]->IsNumber()) {
        auto effect = info[0]->ToNumber<int32_t>();

        if (effect >= static_cast<int32_t>(SlideEffect::LEFT) && effect <= static_cast<int32_t>(SlideEffect::BOTTOM)) {
            const auto& pageTransition = GetPageTransition();
            if (pageTransition) {
                pageTransition->SetEffect(static_cast<SlideEffect>(effect));
            }
        }
    }
}

void JSPageTransition::Translate(const JSCallbackInfo& info)
{
    LOGD("JSPageTransitionTranslate");
    if (info.Length() > 0 && info[0]->IsObject()) {
        auto args = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!args || args->IsNull()) {
            LOGE("JSTransition Translate failed json value is nullptr or json object is null");
            return;
        }

        auto translateX = Dimension(0.0);
        auto translateY = Dimension(0.0);
        auto translateZ = Dimension(0.0);

        Dimension length;
        if (JSViewAbstract::ParseJsonDimensionVp(args->GetValue("x"), length)) {
            translateX = length;
        }
        if (JSViewAbstract::ParseJsonDimensionVp(args->GetValue("y"), length)) {
            translateY = length;
        }
        if (JSViewAbstract::ParseJsonDimensionVp(args->GetValue("z"), length)) {
            translateZ = length;
        }

        const auto& pageTransition = GetPageTransition();
        if (pageTransition) {
            pageTransition->AddTranslateAnimation(translateX, translateY, translateZ);
        }
    }
}

void JSPageTransition::Scale(const JSCallbackInfo& info)
{
    LOGD("JSPageTransition::Scale");
    if (info.Length() > 0 && info[0]->IsObject()) {
        auto args = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!args || args->IsNull()) {
            LOGE("JSTransition Scale failed, json value is nullptr or json object is null");
            return;
        }

        // default: x, y, z (1.0, 1.0, 1.0)
        double scaleX = 1.0;
        double scaleY = 1.0;
        double scaleZ = 1.0;
        JSViewAbstract::ParseJsonDouble(args->GetValue("x"), scaleX);
        JSViewAbstract::ParseJsonDouble(args->GetValue("y"), scaleY);
        JSViewAbstract::ParseJsonDouble(args->GetValue("z"), scaleZ);
        // default centerX, centerY 50% 50%;
        Dimension centerX = 0.5_pct;
        Dimension centerY = 0.5_pct;

        // if specify centerX
        Dimension length;
        if (JSViewAbstract::ParseJsonDimensionVp(args->GetValue("centerX"), length)) {
            centerX = length;
        }
        // if specify centerY
        if (JSViewAbstract::ParseJsonDimensionVp(args->GetValue("centerY"), length)) {
            centerY = length;
        }

        const auto& pageTransition = GetPageTransition();
        if (pageTransition) {
            pageTransition->AddScaleAnimation(scaleX, scaleY, scaleZ, centerX, centerY);
        }
    }
}

void JSPageTransition::Opacity(const JSCallbackInfo& info)
{
    LOGD("JSPageTransition::Opacity");
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    double opacity = 0.0;
    if (!JSViewAbstract::ParseJsDouble(info[0], opacity)) {
        return;
    }
    const auto& pageTransition = GetPageTransition();
    if (pageTransition) {
        pageTransition->AddOpacityAnimation(opacity);
    }
}

void JSPageTransition::JsHandlerOnEnter(const JSCallbackInfo& info)
{
    LOGD("JSPageTransition::JsHandlerOnEnter");
    if (info.Length() < 1 || !info[0]->IsFunction()) {
        LOGE("JsHandlerOnEnter info.Length < 0 or info[0] is not function");
        return;
    }

    RefPtr<JsPageTransitionFunction> function =
        AceType::MakeRefPtr<JsPageTransitionFunction>(JSRef<JSFunc>::Cast(info[0]));

    auto onEnterHandler = [execCtx = info.GetExecutionContext(), func = std::move(function)](
                              RouteType type, const float& progress) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("PageTransition.onEnter");
        func->Execute(type, progress);
    };

    const auto& pageTransition = GetPageTransition();
    if (pageTransition) {
        pageTransition->SetOnEnterHandler(std::move(onEnterHandler));
    }
}

void JSPageTransition::JsHandlerOnExit(const JSCallbackInfo& info)
{
    LOGD("JSPageTransition::JsHandlerOnExit");
    if (info.Length() < 1 || !info[0]->IsFunction()) {
        LOGE("JsHandlerOnExit info.Length < 0 or info[0] is not function");
        return;
    }

    RefPtr<JsPageTransitionFunction> function =
        AceType::MakeRefPtr<JsPageTransitionFunction>(JSRef<JSFunc>::Cast(info[0]));

    auto onExitHandler = [execCtx = info.GetExecutionContext(), func = std::move(function)](
                             RouteType type, const float& progress) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("PageTransition.onExit");
        func->Execute(type, progress);
    };

    const auto& pageTransition = GetPageTransition();
    if (pageTransition) {
        pageTransition->SetOnExitHandler(std::move(onExitHandler));
    }
}

void JSPageTransition::Create(const JSCallbackInfo& info)
{
    LOGD("JSPageTransition::JSTransition::Create");
    // create PageTransitionComponent
    auto pageTransitionComponent = ViewStackProcessor::GetInstance()->GetPageTransitionComponent();
    if (pageTransitionComponent) {
        pageTransitionComponent->ClearPageTransition();
    }
}

void JSPageTransition::Pop()
{
    LOGD("JSPageTransition::Pop");
    auto pageTransitionComponent = ViewStackProcessor::GetInstance()->GetPageTransitionComponent();
    if (pageTransitionComponent) {
        pageTransitionComponent->ClearPageTransitionStack();
    }
}

RefPtr<PageTransition> JSPageTransition::CreateTransition(
    PageTransitionType type, const std::unique_ptr<JsonValue>& transitionArgs)
{
    RouteType routeType = RouteType::NONE;
    int32_t duration = 1000;
    int32_t delay = 0;
    RefPtr<Curve> curve = AceType::MakeRefPtr<LinearCurve>();
    if (transitionArgs && !transitionArgs->IsNull()) {
        duration = transitionArgs->GetInt("duration", 1000);
        delay = transitionArgs->GetInt("delay", 0);
        auto routeTypeTmp = transitionArgs->GetInt("type", static_cast<int32_t>(RouteType::NONE));
        if (routeTypeTmp >= static_cast<int32_t>(RouteType::NONE) &&
            routeTypeTmp <= static_cast<int32_t>(RouteType::POP)) {
            routeType = static_cast<RouteType>(routeTypeTmp);
        } else {
            LOGE("CreateTransition RouteType out of range");
        }

        auto curveArgs = transitionArgs->GetValue("curve");
        if (curveArgs->IsString()) {
            curve = CreateCurve(curveArgs->GetString());
        } else if (curveArgs->IsObject()) {
            auto curveString = curveArgs->GetValue("__curveString");
            if (curveString) {
                curve = CreateCurve(curveString->GetString());
            }
        }
    }

    auto pageTransitionInfo = AceType::MakeRefPtr<PageTransition>(type);
    pageTransitionInfo->SetDuration(duration);
    pageTransitionInfo->SetDelay(delay);
    pageTransitionInfo->SetCurve(curve);
    pageTransitionInfo->SetRouteType(routeType);

    return pageTransitionInfo;
}

RefPtr<PageTransition> JSPageTransition::GetPageTransition()
{
    auto pageTransitionComponent = ViewStackProcessor::GetInstance()->GetPageTransitionComponent();
    if (pageTransitionComponent) {
        return pageTransitionComponent->GetTopPageTransition();
    }
    return nullptr;
}

void JSPageTransitionEnter::Create(const JSCallbackInfo& info)
{
    LOGD("JSPageTransitionEnter::Create");
    if (info.Length() > 0 && info[0]->IsObject()) {
        auto transitionArgs = JsonUtil::ParseJsonString(info[0]->ToString());
        auto pageTransition = CreateTransition(PageTransitionType::ENTER, transitionArgs);

        auto pageTransitionComponent = ViewStackProcessor::GetInstance()->GetPageTransitionComponent();
        if (pageTransitionComponent) {
            pageTransitionComponent->PushPageTransition(pageTransition);
        }
    }
}

void JSPageTransitionExit::Create(const JSCallbackInfo& info)
{
    LOGD("JSPageTransitionEnter::Create");
    if (info.Length() > 0 && info[0]->IsObject()) {
        auto transitionArgs = JsonUtil::ParseJsonString(info[0]->ToString());
        auto pageTransition = CreateTransition(PageTransitionType::EXIT, transitionArgs);

        auto pageTransitionComponent = ViewStackProcessor::GetInstance()->GetPageTransitionComponent();
        if (pageTransitionComponent) {
            pageTransitionComponent->PushPageTransition(pageTransition);
        }
    }
}

} // namespace OHOS::Ace::Framework