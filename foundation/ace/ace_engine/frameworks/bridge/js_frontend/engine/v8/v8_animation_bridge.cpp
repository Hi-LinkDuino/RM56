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

#include "frameworks/bridge/js_frontend/engine/v8/v8_animation_bridge.h"

#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/memory/referenced.h"
#include "base/utils/string_utils.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/tween/tween_component.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_engine.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_utils.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace::Framework {
namespace {

RefPtr<JsAcePage> GetPageById(v8::Isolate* isolate, int32_t pageId)
{
    LOGD("Enter GetPageById");
    if (isolate == nullptr) {
        LOGE("Isolate is null.");
        return nullptr;
    }
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    return (*delegate)->GetPage(pageId);
}

inline NodeId GetCurrentNodeId(const v8::Local<v8::Context>& ctx, v8::Local<v8::Object> value)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);
    NodeId id = value->Get(ctx, v8::String::NewFromUtf8(isolate, "__nodeId").ToLocalChecked())
                    .ToLocalChecked()
                    ->Int32Value(ctx)
                    .ToChecked();
    return id < 0 ? 0 : id;
}

inline int32_t GetCurrentPageId(const v8::Local<v8::Context>& ctx, v8::Local<v8::Object> value)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);
    int32_t id = value->Get(ctx, v8::String::NewFromUtf8(isolate, "__pageId").ToLocalChecked())
                     .ToLocalChecked()
                     ->Int32Value(ctx)
                     .ToChecked();
    return id < 0 ? 0 : id;
}

void HandleJsAnimationContext(
    const v8::Local<v8::Context>& ctx, int32_t pageId, int32_t nodeId, AnimationOperation operation)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    auto page = GetPageById(isolate, pageId);
    if (!page) {
        LOGE("no page found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return;
    }
    auto task = AceType::MakeRefPtr<V8AnimationBridgeTaskOperation>(operation);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    if (page->CheckPageCreated()) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        (*delegate)->TriggerPageUpdate(page->GetPageId());
    }
}

const std::vector<std::tuple<std::string, v8::FunctionCallback, v8::FunctionCallback>> V8_ANIMATION_FUNCS = {
    { "playState", V8AnimationBridgeUtils::JsAnimationPlayStateGet, V8AnimationBridgeUtils::JsAnimationPlayStateSet },
    { "startTime", V8AnimationBridgeUtils::JsAnimationStartTimeGet, V8AnimationBridgeUtils::JsAnimationStartTimeSet },
    { "pending", V8AnimationBridgeUtils::JsAnimationPendingGet, V8AnimationBridgeUtils::JsAnimationPendingSet }
};

void CallAnimationStartJs(const WeakPtr<V8AnimationBridge>& bridgeWeak, v8::Isolate* isolate)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Start Js Failed. animation bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    v8::HandleScope handleScope(isolate);
    auto animatorObject = bridge->GetJsObject();
    if (animatorObject.IsEmpty()) {
        LOGE("Animation Object is null");
        return;
    }
    auto context = bridge->GetContext();
    v8::Local<v8::Value> proto =
        animatorObject->Get(context, v8::String::NewFromUtf8(isolate, "onstart").ToLocalChecked()).ToLocalChecked();
    if (!proto->IsFunction()) {
        LOGE("cannot find 'CallAnimationStartJs' function from animation object, maybe no callback at all.");
        return;
    }
    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(proto);
    v8::Local<v8::Value> funcRes;
    v8::Local<v8::Object> global = context->Global();
    bool succ = jsFunc->Call(context, global, 0, {}).ToLocal(&funcRes);
    if (!succ) {
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::ANIMATION_START_ERROR);
        return;
    }
}

void CallAnimationFinishJs(const WeakPtr<V8AnimationBridge>& bridgeWeak, v8::Isolate* isolate,
    const RefPtr<JsAcePage>& page)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Finish Js Failed. animation bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    v8::HandleScope handleScope(isolate);
    auto animationObject = bridge->GetJsObject();
    if (animationObject.IsEmpty()) {
        LOGE("Animation Object is null");
        return;
    }
    auto context = bridge->GetContext();
    v8::Local<v8::Value> proto =
        animationObject->Get(context, v8::String::NewFromUtf8(isolate, "onfinish").ToLocalChecked()).ToLocalChecked();
    if (!proto->IsFunction()) {
        LOGE("cannot find 'CallAnimationFinishJs' function from animation object, maybe no callback at all.");
        return;
    }
    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(proto);
    v8::Local<v8::Value> funcRes;
    v8::Local<v8::Object> global = context->Global();
    bool succ = jsFunc->Call(context, global, 0, {}).ToLocal(&funcRes);
    if (!succ) {
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::ANIMATION_FINISH_ERROR, 0, page);
        return;
    }
}

void CallAnimationCancelJs(const WeakPtr<V8AnimationBridge>& bridgeWeak, v8::Isolate* isolate,
    const RefPtr<JsAcePage>& page)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Cancel Js Failed. animation bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    v8::HandleScope handleScope(isolate);
    auto animationObject = bridge->GetJsObject();
    if (animationObject.IsEmpty()) {
        LOGE("Animation Object is null");
        return;
    }
    auto context = bridge->GetContext();
    v8::Local<v8::Value> proto =
        animationObject->Get(context, v8::String::NewFromUtf8(isolate, "oncancel").ToLocalChecked()).ToLocalChecked();
    if (!proto->IsFunction()) {
        LOGE("cannot find 'CallAnimationCancelJs' function from animation object, maybe no callback at all.");
        return;
    }
    LOGD("animation oncancel event call");
    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(proto);
    v8::Local<v8::Value> funcRes;
    v8::Local<v8::Object> global = context->Global();
    bool succ = jsFunc->Call(context, global, 0, {}).ToLocal(&funcRes);
    if (!succ) {
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::ANIMATION_CANCEL_ERROR, 0, page);
        return;
    }
}

void CallAnimationRepeatJs(const WeakPtr<V8AnimationBridge>& bridgeWeak, v8::Isolate* isolate,
    const RefPtr<JsAcePage>& page)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Repeat Js Failed. animation bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    v8::HandleScope handleScope(isolate);
    auto animationObject = bridge->GetJsObject();
    if (animationObject.IsEmpty()) {
        LOGE("Animation Object is null");
        return;
    }
    auto context = bridge->GetContext();
    v8::Local<v8::Value> proto =
        animationObject->Get(context, v8::String::NewFromUtf8(isolate, "onrepeat").ToLocalChecked()).ToLocalChecked();
    if (!proto->IsFunction()) {
        LOGE("cannot find 'CallAnimationRepeatJs' function from animation object, maybe no callback at all.");
        return;
    }
    LOGD("animation onrepeat event call");
    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(proto);
    v8::Local<v8::Value> funcRes;
    v8::Local<v8::Object> global = context->Global();
    bool succ = jsFunc->Call(context, global, 0, {}).ToLocal(&funcRes);
    if (!succ) {
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::ANIMATION_REPEAT_ERROR, 0, page);
        return;
    }
}

void JsUpdatePlayState(v8::Isolate* isolate, const WeakPtr<V8AnimationBridge>& bridgeWeak, const char* state)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge || (isolate == nullptr)) {
        LOGW("Set playState to Stop failed. bridge or isolate is null.");
        return;
    }
    v8::HandleScope handleScope(isolate);
    auto ctx = bridge->GetContext();
    v8::Context::Scope contextScope(ctx);
    auto animationContext = bridge->GetJsObject();
    animationContext
        ->Set(ctx, v8::String::NewFromUtf8(isolate, "__playState").ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, state).ToLocalChecked())
        .ToChecked();
}

void AddListenerForEventCallback(const WeakPtr<V8AnimationBridge>& bridgeWeak,
    const RefPtr<Animator>& animator, v8::Isolate* isolate, const RefPtr<JsAcePage>& page)
{
    animator->AddStopListener([isolate, bridgeWeak, page] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, isolate, page]() mutable {
            LOGI("call animation onfinish event");
            CallAnimationFinishJs(bridgeWeak, isolate, page);
        });
    });
    animator->AddIdleListener([isolate, bridgeWeak, page] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, isolate, page]() mutable {
            LOGI("call animation oncancel event");
            CallAnimationCancelJs(bridgeWeak, isolate, page);
        });
    });
    animator->AddRepeatListener([isolate, bridgeWeak, page] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, isolate, page]() mutable {
            LOGI("call animation onrepeat event");
            CallAnimationRepeatJs(bridgeWeak, isolate, page);
        });
    });
}

} // namespace

V8AnimationBridge::V8AnimationBridge(
    const v8::Local<v8::Context>& ctx, v8::Isolate* instance, v8::Local<v8::Object> animationContext, NodeId nodeId)
    : instance_(instance), nodeId_(nodeId) // ??? how to dup animationContext
{
    animationObject_.Reset(instance_, animationContext);
    ctx_.Reset(instance_, ctx);
}

void V8AnimationBridgeUtils::JsAnimationStartTimeGet(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    int32_t nodeId = GetCurrentNodeId(context, info.Holder());
    int32_t pageId = GetCurrentPageId(context, info.Holder());
    auto page = GetPageById(isolate, pageId);
    if (!page) {
        LOGE("JsAnimationStartTimeGet: no page found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return;
    }
    auto domDocument = page->GetDomDocument();
    if (!domDocument) {
        LOGE("JsAnimationStartTimeGet failed, DomDocument is null.");
        return;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (!domNode) {
        LOGE("JsAnimationStartTimeGet failed, DomNode is null.");
        return;
    }
    auto tweenComponent = domNode->GetTweenComponent();
    if (tweenComponent) {
        auto option = tweenComponent->GetCustomTweenOption();
        auto startTime = option.GetDelay();
        info.Holder()
            ->Set(context, v8::String::NewFromUtf8(isolate, "__startTime").ToLocalChecked(),
                v8::Int32::New(isolate, startTime))
            .ToChecked();
    }
    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__startTime").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8AnimationBridgeUtils::JsAnimationStartTimeSet(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() != 1) {
        LOGE("Not valid Length for info. length: %{public}d", info.Length());
        return;
    }
    v8::Local<v8::Value> value = info[0];
    v8::Isolate* isolate = info.GetIsolate();
    v8::String::Utf8Value jsStartTime(isolate, value);
    if (!(*jsStartTime)) {
        return;
    }
    std::string startTime(*jsStartTime);

    auto ctx = isolate->GetCurrentContext();
    int32_t nodeId = GetCurrentNodeId(ctx, info.Holder());
    int32_t pageId = GetCurrentPageId(ctx, info.Holder());
    auto page = GetPageById(isolate, pageId);
    if (!page) {
        LOGE("JsAnimationStartTimeSet: no page found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return;
    }
    auto task = AceType::MakeRefPtr<V8AnimationBridgeTaskStartTime>(startTime);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimation>(nodeId, task));
}

void V8AnimationBridgeUtils::JsAnimationPendingGet(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    int32_t nodeId = GetCurrentNodeId(context, info.Holder());
    int32_t pageId = GetCurrentPageId(context, info.Holder());
    auto page = GetPageById(isolate, pageId);
    if (!page) {
        LOGE("JsAnimationPendingGet: no page found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return;
    }
    auto domDocument = page->GetDomDocument();
    if (!domDocument) {
        LOGE("JsAnimationPendingGet failed, DomDocument is null.");
        return;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (!domNode) {
        LOGE("JsAnimationPendingGet failed, DomNode is null.");
        return;
    }
    auto tweenComponent = domNode->GetTweenComponent();
    if (tweenComponent) {
        auto controller = tweenComponent->GetAnimator();
        if (controller) {
            info.Holder()
                ->Set(context, v8::String::NewFromUtf8(isolate, "__pending").ToLocalChecked(),
                    v8::Boolean::New(isolate, controller->IsPending()))
                .ToChecked();
        }
    }
    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__pending").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8AnimationBridgeUtils::JsAnimationPendingSet(const v8::FunctionCallbackInfo<v8::Value>& info) {}

void V8AnimationBridgeUtils::JsAnimationPlayStateGet(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__playState").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8AnimationBridgeUtils::JsAnimationPlayStateSet(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() != 1) {
        LOGE("Not valid Length for info. length: %{public}d", info.Length());
        return;
    }
    v8::Local<v8::Value> value = info[0];
    v8::Isolate* isolate = info.GetIsolate();
    v8::String::Utf8Value jsPlayState(isolate, value);
    if (!(*jsPlayState)) {
        return;
    }
    std::string playState(*jsPlayState);
    AnimationOperation operation = StringToAnimationOperation(playState);
    auto ctx = isolate->GetCurrentContext();
    int32_t nodeId = GetCurrentNodeId(ctx, info.Holder());
    int32_t pageId = GetCurrentPageId(ctx, info.Holder());
    auto page = GetPageById(isolate, pageId);
    if (!page) {
        LOGE("no page found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return;
    }
    auto task = AceType::MakeRefPtr<V8AnimationBridgeTaskOperation>(operation);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimation>(nodeId, task));
}

void V8AnimationBridge::JsCreateAnimation(const RefPtr<JsAcePage>& page, const std::string& param)
{
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    std::unordered_map<std::string, double> animationDoubleOptions;
    std::unordered_map<std::string, std::string> animationStringOptions;
    int32_t iterations = 0;

    BaseAnimationBridgeUtils::JsParseAnimationFrames(param, animationFrames);
    BaseAnimationBridgeUtils::JsParseAnimationOptions(
        param, iterations, animationDoubleOptions, animationStringOptions);
    auto tweenOption = TweenOption();
    auto iterEasing = animationStringOptions.find(DOM_ANIMATION_EASING);
    if (iterEasing != animationStringOptions.end()) {
        tweenOption.SetCurve(CreateCurve(iterEasing->second));
    }
    std::vector<Dimension> transformOrigin = BaseAnimationBridgeUtils::HandleTransformOrigin(animationFrames);
    if (transformOrigin.size() == BaseAnimationBridgeUtils::TRANSFORM_ORIGIN_DEFAULT_SIZE) {
        tweenOption.SetTransformOrigin(transformOrigin.front(), transformOrigin.back());
    }
    auto iterDuration = animationDoubleOptions.find(DOM_ANIMATION_DURATION_API);
    if (iterDuration != animationDoubleOptions.end()) {
        tweenOption.SetDuration(iterDuration->second);
    }
    auto iterFill = animationStringOptions.find(DOM_ANIMATION_FILL);
    if (iterFill != animationStringOptions.end()) {
        tweenOption.SetFillMode(StringToFillMode(iterFill->second));
    }
    auto iterDirection = animationStringOptions.find(DOM_ANIMATION_DIRECTION_API);
    if (iterDirection != animationStringOptions.end()) {
        tweenOption.SetAnimationDirection(StringToAnimationDirection(iterDirection->second));
    }
    auto iterDelay = animationDoubleOptions.find(DOM_ANIMATION_DELAY_API);
    if (iterDelay != animationDoubleOptions.end()) {
        tweenOption.SetDelay(iterDelay->second);
    }
    tweenOption.SetIteration(iterations);
    if (SystemProperties::GetRosenBackendEnabled()) {
        tweenOption.SetAllowRunningAsynchronously(true);
    }
    if (!page) {
        LOGE("JsCreateAnimation failed, Page is null.");
        return;
    }
    auto domDocument = page->GetDomDocument();
    if (!domDocument) {
        LOGE("JsCreateAnimation failed, DomDocument is null.");
        return;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId_);
    if (!domNode) {
        LOGE("JsCreateAnimation failed, DomNode is null.");
        return;
    }
    domNode->ParseAnimationStyle(animationFrames);
    domNode->TweenOptionSetKeyframes(tweenOption);
    if (tweenOption.IsValid()) {
        domNode->SetCustomAnimationStyleUpdate(true);
    }
    RefPtr<Animator> animator = AceType::MakeRefPtr<Animator>();
    auto tweenComponent = domNode->GetTweenComponent();
    if (!tweenComponent) {
        tweenComponent = AceType::MakeRefPtr<TweenComponent>(
            BaseAnimationBridgeUtils::COMPONENT_PREFIX + std::to_string(nodeId_), domNode->GetTag());
        domNode->SetTweenComponent(tweenComponent);
    }
    LOGD("parse animate parameters for nodeId: %d", nodeId_);
    tweenComponent->SetAnimator(animator);
    BaseAnimationBridgeUtils::SetTweenComponentParams(nullptr, animationFrames, tweenComponent, tweenOption);
    AddListenerForEventCallback(AceType::WeakClaim(this), animator, instance_, page);
    domNode->GenerateComponentNode();
    page->PushDirtyNode(nodeId_);
}

void V8AnimationBridge::SetPlayStateCallbacksWithListenerId(RefPtr<Animator>& animator)
{
    WeakPtr<V8AnimationBridge> bridgeWeak = AceType::WeakClaim(this);
    animator->RemoveStopListener(finishListenerId_);
    finishListenerId_ = animator->AddStopListener([bridgeWeak, instance = instance_] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(instance->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([instance, bridgeWeak]() mutable {
            JsUpdatePlayState(instance, bridgeWeak, DOM_ANIMATION_PLAY_STATE_FINISHED);
        });
    });
    animator->RemoveIdleListener(idleListenerId_);
    idleListenerId_ = animator->AddIdleListener([bridgeWeak, instance = instance_] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(instance->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([instance, bridgeWeak]() mutable {
            JsUpdatePlayState(instance, bridgeWeak, DOM_ANIMATION_PLAY_STATE_IDLE);
        });
    });
}

void V8AnimationBridge::SetPlayStateCallbacks(RefPtr<Animator>& animator)
{
    if (!animator) {
        LOGE("Set PlayState callbacks failed. animator is null.");
        return;
    }
    WeakPtr<V8AnimationBridge> bridgeWeak = AceType::WeakClaim(this);
    SetPlayStateCallbacksWithListenerId(animator);
    animator->ClearPauseListeners();
    animator->AddPauseListener([bridgeWeak, instance = instance_] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(instance->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([instance, bridgeWeak]() mutable {
            JsUpdatePlayState(instance, bridgeWeak, DOM_ANIMATION_PLAY_STATE_PAUSED);
        });
    });
    animator->ClearStartListeners();
    animator->AddStartListener([bridgeWeak, instance = instance_] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(instance->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([instance, bridgeWeak]() mutable {
            CallAnimationStartJs(bridgeWeak, instance);
            JsUpdatePlayState(instance, bridgeWeak, DOM_ANIMATION_PLAY_STATE_RUNNING);
        });
    });
    animator->ClearResumeListeners();
    animator->AddResumeListener([bridgeWeak, instance = instance_] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(instance->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([instance, bridgeWeak]() mutable {
            JsUpdatePlayState(instance, bridgeWeak, DOM_ANIMATION_PLAY_STATE_RUNNING);
        });
    });
}

void V8AnimationBridgeUtils::JsAnimationPlay(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 0) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    int32_t nodeId = GetCurrentNodeId(ctx, args.Holder());
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    HandleJsAnimationContext(ctx, pageId, nodeId, AnimationOperation::PLAY);
}

void V8AnimationBridgeUtils::JsAnimationFinish(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 0) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    int32_t nodeId = GetCurrentNodeId(ctx, args.Holder());
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    HandleJsAnimationContext(ctx, pageId, nodeId, AnimationOperation::FINISH);
}

void V8AnimationBridgeUtils::JsAnimationPause(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 0) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    int32_t nodeId = GetCurrentNodeId(ctx, args.Holder());
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    HandleJsAnimationContext(ctx, pageId, nodeId, AnimationOperation::PAUSE);
}

void V8AnimationBridgeUtils::JsAnimationCancel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 0) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    int32_t nodeId = GetCurrentNodeId(ctx, args.Holder());
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    HandleJsAnimationContext(ctx, pageId, nodeId, AnimationOperation::CANCEL);
}

void V8AnimationBridgeUtils::JsAnimationReverse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 0) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    int32_t nodeId = GetCurrentNodeId(ctx, args.Holder());
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    HandleJsAnimationContext(ctx, pageId, nodeId, AnimationOperation::REVERSE);
}

v8::Local<v8::Object> V8AnimationBridgeUtils::CreateAnimationContext(
    v8::Local<v8::Context>& ctx, int32_t pageId, NodeId nodeId)
{
    const std::unordered_map<const char*, v8::Local<v8::Function>> contextTable = {
        { "play", v8::Function::New(ctx, JsAnimationPlay, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "finish", v8::Function::New(ctx, JsAnimationFinish, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "pause", v8::Function::New(ctx, JsAnimationPause, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "cancel", v8::Function::New(ctx, JsAnimationCancel, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "reverse", v8::Function::New(ctx, JsAnimationReverse, v8::Local<v8::Value>(), 0).ToLocalChecked() },
    };

    auto animationContext = v8::Object::New(ctx->GetIsolate());
    for (const auto& iter : contextTable) {
        animationContext->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), iter.first).ToLocalChecked(), iter.second)
            .ToChecked();
    }
    animationContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "__nodeId").ToLocalChecked(),
            v8::Int32::New(ctx->GetIsolate(), nodeId))
        .ToChecked();
    animationContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "__pageId").ToLocalChecked(),
            v8::Int32::New(ctx->GetIsolate(), pageId))
        .ToChecked();
    animationContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "__playState").ToLocalChecked(),
        v8::String::NewFromUtf8(ctx->GetIsolate(), DOM_ANIMATION_PLAY_STATE_IDLE).ToLocalChecked())
        .ToChecked();
    animationContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "finished").ToLocalChecked(),
            v8::Boolean::New(ctx->GetIsolate(), 0))
        .ToChecked();
    for (const auto& item : V8_ANIMATION_FUNCS) {
        auto getterTempl = v8::FunctionTemplate::New(ctx->GetIsolate(), std::get<1>(item));
        auto setterTempl = v8::FunctionTemplate::New(ctx->GetIsolate(), std::get<2>(item));
        animationContext->SetAccessorProperty(
            v8::String::NewFromUtf8(ctx->GetIsolate(), std::get<0>(item).c_str()).ToLocalChecked(),
            getterTempl->GetFunction(ctx).ToLocalChecked(), setterTempl->GetFunction(ctx).ToLocalChecked());
    }
    return animationContext;
}

V8AnimationBridgeTaskCreate::V8AnimationBridgeTaskCreate(
    const RefPtr<FrontendDelegate>& delegate, const RefPtr<V8AnimationBridge>& bridge, std::string param)
    : bridge_(bridge), delegate_(delegate), param_(std::move(param))
{}

void V8AnimationBridgeTaskCreate::AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId)
{
    if (!bridge_ || !page) {
        LOGE("Create Animation Bridge failed. bridge or page is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    auto bridgeFree = AceType::DynamicCast<V8AnimationBridge>(page->GetAnimationBridge(nodeId));
    auto sp = delegate_.Upgrade();
    if (sp) {
        auto jsTaskExecutor = sp->GetAnimationJsTask();
        if (bridgeFree) {
            auto weakBridge = AceType::WeakClaim(AceType::RawPtr(bridgeFree));
            jsTaskExecutor.PostTask([weakBridge]() mutable {
                auto bridgeFree = weakBridge.Upgrade();
                if (bridgeFree != nullptr) {
                    bridgeFree.Reset();
                }
            });
        }
    }
    bridge_->JsCreateAnimation(page, param_);
    page->AddAnimationBridge(nodeId, bridge_);
}

void V8AnimationBridgeTaskOperation::AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId)
{
    if (!page) {
        LOGE("AnimationBridgeTaskFunc failed, page is nullptr.");
        return;
    }
    auto animationBridge = AceType::DynamicCast<V8AnimationBridge>(page->GetAnimationBridge(nodeId));
    if (!animationBridge) {
        LOGE("no animation bridge found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    auto domDocument = page->GetDomDocument();
    if (!domDocument) {
        LOGE("Animation operation failed, DomDocument is null.");
        return;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (!domNode) {
        LOGE("Animation operation failed, DomNode is null.");
        return;
    }
    auto tweenComponent = domNode->GetTweenComponent();
    if (tweenComponent) {
        tweenComponent->SetCustomAnimationOperation(operation_);
    }

    RefPtr<Animator> animator;
    if (tweenComponent) {
        animator = tweenComponent->GetAnimator();
    }
    if (animator) {
        animationBridge->SetPlayStateCallbacks(animator);
    }
    domNode->GenerateComponentNode();
    page->PushDirtyNode(nodeId);
}

void V8AnimationBridgeTaskStartTime::AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId)
{
    if (!page) {
        LOGE("V8AnimationBridgeTaskStartTime: Get page is error");
        return;
    }
    auto domDocument = page->GetDomDocument();
    if (!domDocument) {
        LOGE("V8AnimationBridgeTaskStartTime failed, DomDocument is null.");
        return;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (!domNode) {
        LOGE("V8AnimationBridgeTaskStartTime failed, DomNode is null.");
        return;
    }
    auto tweenComponent = domNode->GetTweenComponent();
    if (tweenComponent) {
        auto option = tweenComponent->GetCustomTweenOption();
        option.SetDelay(StringToInt(startTime_));
        tweenComponent->SetCustomTweenOption(option);
    }
}

} // namespace OHOS::Ace::Framework
