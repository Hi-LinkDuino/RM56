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

#include "bridge/declarative_frontend/jsview/js_view_context.h"

#include <functional>

#include "base/utils/system_properties.h"
#include "bridge/common/utils/utils.h"
#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/view_stack_processor.h"

#include "core/common/container_scope.h"
#include "core/components/common/properties/animation_option.h"

#ifdef USE_V8_ENGINE
#include "bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"
#elif USE_QUICKJS_ENGINE
#include "bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine_instance.h"
#elif USE_ARK_ENGINE
#include "bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"
#endif

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t DEFAULT_DURATION = 1000; // ms

} // namespace

const AnimationOption JSViewContext::CreateAnimation(const std::unique_ptr<JsonValue>& animationArgs)
{
    AnimationOption option = AnimationOption();
    if (!animationArgs) {
        LOGW("CreateAnimation: animationArgs is null");
        return option;
    }
    // If the attribute does not exist, the default value is used.
    auto duration = animationArgs->GetInt("duration", DEFAULT_DURATION);
    auto delay = animationArgs->GetInt("delay", 0);
    auto iterations = animationArgs->GetInt("iterations", 1);
    auto tempo = animationArgs->GetDouble("tempo", 1.0);
    auto direction = StringToAnimationDirection(animationArgs->GetString("playMode", "normal"));
    RefPtr<Curve> curve;
    auto curveArgs = animationArgs->GetValue("curve");
    if (curveArgs->IsString()) {
        curve = CreateCurve(animationArgs->GetString("curve", "linear"));
    } else if (curveArgs->IsObject()) {
        auto curveString = curveArgs->GetValue("__curveString");
        if (!curveString) {
            // Default AnimationOption which is invalid.
            return option;
        }
        curve = CreateCurve(curveString->GetString());
    } else {
        curve = Curves::EASE_IN_OUT;
    }

    option.SetDuration(duration);
    option.SetDelay(delay);
    option.SetIteration(iterations);
    option.SetTempo(tempo);
    option.SetAnimationDirection(direction);
    option.SetCurve(curve);
    return option;
}

void JSViewContext::JSAnimation(const JSCallbackInfo& info)
{
    LOGD("JSAnimation");
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have 1 object argument.");
        return;
    }
    AnimationOption option = AnimationOption();
    if (info[0]->IsNull()) {
        LOGE("JSAnimation: info[0] is null.");
        ViewStackProcessor::GetInstance()->SetImplicitAnimationOption(option);
        return;
    }
    if (!info[0]->IsObject()) {
        LOGE("JSAnimation: info[0] is not object.");
        ViewStackProcessor::GetInstance()->SetImplicitAnimationOption(option);
        return;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> onFinish = obj->GetProperty("onFinish");
    std::function<void()> onFinishEvent;
    if (onFinish->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onFinish));
        onFinishEvent = [execCtx = info.GetExecutionContext(), func = std::move(jsFunc),
                            id = Container::CurrentId()]() {
            ContainerScope scope(id);
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            func->Execute();
        };
    }

    auto animationArgs = JsonUtil::ParseJsonString(info[0]->ToString());
    if (animationArgs->IsNull()) {
        LOGE("Js Parse failed. animationArgs is null.");
        ViewStackProcessor::GetInstance()->SetImplicitAnimationOption(option);
        return;
    }
    option = CreateAnimation(animationArgs);
    option.SetOnFinishEvent(onFinishEvent);
    if (SystemProperties::GetRosenBackendEnabled()) {
        option.SetAllowRunningAsynchronously(true);
    }
    ViewStackProcessor::GetInstance()->SetImplicitAnimationOption(option);
}

void JSViewContext::JSAnimateTo(const JSCallbackInfo& info)
{
    if (info.Length() < 2) {
        LOGE("The arg is wrong, it is supposed to have two arguments.");
        return;
    }
    if (!info[0]->IsObject()) {
        LOGE("1st argument is not object.");
        return;
    }
    // 2nd argument should be a closure passed to the animateTo function.
    if (!info[1]->IsFunction()) {
        LOGE("2nd argument is not a function.");
        return;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> onFinish = obj->GetProperty("onFinish");
    std::function<void()> onFinishEvent;
    if (onFinish->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onFinish));
        onFinishEvent = [execCtx = info.GetExecutionContext(), func = std::move(jsFunc),
                            id = Container::CurrentId()]() {
            ContainerScope scope(id);
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            func->Execute();
        };
    }

    auto animationArgs = JsonUtil::ParseJsonString(info[0]->ToString());
    if (animationArgs->IsNull()) {
        LOGE("Js Parse failed. animationArgs is null.");
        return;
    }

#ifdef USE_V8_ENGINE
    auto pipelineContext = V8DeclarativeEngineInstance::GetPipelineContext();
#elif USE_QUICKJS_ENGINE
    auto pipelineContext = QJSDeclarativeEngineInstance::GetPipelineContext(info[1]->GetJsContext());
#elif USE_ARK_ENGINE
    auto runtime = JsiDeclarativeEngineInstance::GetCurrentRuntime();
    auto pipelineContext = JsiDeclarativeEngineInstance::GetPipelineContext(runtime);
#endif
    if (!pipelineContext) {
        LOGE("pipelineContext is null!");
        return;
    }

    AnimationOption option = CreateAnimation(animationArgs);
    if (SystemProperties::GetRosenBackendEnabled()) {
        LOGD("RSAnimationInfo: Begin JSAnimateTo");
        pipelineContext->FlushBuild();
        pipelineContext->OpenImplicitAnimation(option, option.GetCurve(), onFinishEvent);
        // Execute the function.
        JSRef<JSFunc> jsAnimateToFunc = JSRef<JSFunc>::Cast(info[1]);
        jsAnimateToFunc->Call(info[1]);
        pipelineContext->FlushBuild();
        pipelineContext->CloseImplicitAnimation();
        LOGD("RSAnimationInfo: End JSAnimateTo");
    } else {
        pipelineContext->FlushBuild();
        pipelineContext->SaveExplicitAnimationOption(option);
        // Execute the function.
        JSRef<JSFunc> jsAnimateToFunc = JSRef<JSFunc>::Cast(info[1]);
        jsAnimateToFunc->Call(info[1]);
        pipelineContext->FlushBuild();
        pipelineContext->CreateExplicitAnimator(onFinishEvent);
        pipelineContext->ClearExplicitAnimationOption();
    }
}

void JSViewContext::JSBind(BindingTarget globalObj)
{
    JSClass<JSViewContext>::Declare("Context");
    JSClass<JSViewContext>::StaticMethod("animation", JSAnimation);
    JSClass<JSViewContext>::StaticMethod("animateTo", JSAnimateTo);
    JSClass<JSViewContext>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
