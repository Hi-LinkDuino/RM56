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

#include "frameworks/bridge/js_frontend/engine/v8/v8_animator_bridge.h"

#include "base/log/event_report.h"
#include "base/log/log.h"
#include "core/common/container.h"
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

inline int32_t GetCurrentBridgeId(const v8::Local<v8::Context>& ctx, v8::Local<v8::Object> value)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);
    int32_t id = value->Get(ctx, v8::String::NewFromUtf8(isolate, "__bridgeId").ToLocalChecked())
        .ToLocalChecked()->Int32Value(ctx).ToChecked();
    return id < 0 ? 0 : id;
}

inline int32_t GetCurrentPageId(const v8::Local<v8::Context>& ctx, v8::Local<v8::Object> value)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);
    int32_t id = value->Get(ctx, v8::String::NewFromUtf8(isolate, "__pageId").ToLocalChecked())
        .ToLocalChecked()->Int32Value(ctx).ToChecked();
    return id < 0 ? 0 : id;
}

void HandleJsAnimatorContext(const v8::Local<v8::Context>& ctx, int32_t pageId, int32_t bridgeId,
    AnimatorOperation operation)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    auto page = GetPageById(isolate, pageId);
    if (!page) {
        LOGE("no page found for bridgeId: %{public}d", bridgeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return;
    }
    auto task = AceType::MakeRefPtr<V8AnimatorTaskOperation>(operation);
    page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimator>(bridgeId, task));
    if (page->CheckPageCreated()) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        (*delegate)->TriggerPageUpdate(page->GetPageId());
    }
}

void CallAnimationStartJs(const WeakPtr<V8AnimatorBridge>& bridgeWeak, v8::Isolate* isolate)
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

void CallAnimationFinishJs(const WeakPtr<V8AnimatorBridge>& bridgeWeak, v8::Isolate* isolate)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Finish Js Failed. animation bridge is null.");
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
        animatorObject->Get(context, v8::String::NewFromUtf8(isolate, "onfinish").ToLocalChecked()).ToLocalChecked();
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
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::ANIMATION_FINISH_ERROR);
        return;
    }
}

void CallAnimationCancelJs(const WeakPtr<V8AnimatorBridge>& bridgeWeak, v8::Isolate* isolate)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Cancel Js Failed. animation bridge is null.");
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
        animatorObject->Get(context, v8::String::NewFromUtf8(isolate, "oncancel").ToLocalChecked()).ToLocalChecked();
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
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::ANIMATION_CANCEL_ERROR);
        return;
    }
}

void CallAnimationRepeatJs(const WeakPtr<V8AnimatorBridge>& bridgeWeak, v8::Isolate* isolate)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Repeat Js Failed. animation bridge is null.");
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
        animatorObject->Get(context, v8::String::NewFromUtf8(isolate, "onrepeat").ToLocalChecked()).ToLocalChecked();
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
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::ANIMATION_REPEAT_ERROR);
        return;
    }
}

void CallAnimationFrameJs(const WeakPtr<V8AnimatorBridge>& bridgeWeak, v8::Isolate* isolate, double value)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Repeat Js Failed. animation bridge is null.");
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
        animatorObject->Get(context, v8::String::NewFromUtf8(isolate, "onframe").ToLocalChecked()).ToLocalChecked();
    if (!proto->IsFunction()) {
        LOGE("cannot find 'CallAnimationFrameJs' function from animation object, maybe no callback at all.");
        return;
    }
    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(proto);
    v8::Local<v8::Value> funcRes;
    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::Value> args[] = {v8::Number::New(isolate, value)};
    bool succ = jsFunc->Call(context, global, 1, args).ToLocal(&funcRes);
    if (!succ) {
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::ANIMATION_FRAME_ERROR);
        return;
    }
}

void AddListenerForEventCallback(const WeakPtr<V8AnimatorBridge>& bridgeWeak, const RefPtr<Animator>& animator,
    v8::Isolate* isolate)
{
    animator->AddStartListener([isolate, bridgeWeak] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, isolate]() mutable {
            LOGI("call animation onstart event");
            CallAnimationStartJs(bridgeWeak, isolate);
        });
    });
    animator->AddStopListener([isolate, bridgeWeak] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, isolate]() mutable {
            LOGI("call animation onfinish event");
            CallAnimationFinishJs(bridgeWeak, isolate);
        });
    });
    animator->AddIdleListener([isolate, bridgeWeak] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, isolate]() mutable {
            LOGI("call animation oncancel event");
            CallAnimationCancelJs(bridgeWeak, isolate);
        });
    });
    animator->AddRepeatListener([isolate, bridgeWeak] {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, isolate]() mutable {
            LOGI("call animation onrepeat event");
            CallAnimationRepeatJs(bridgeWeak, isolate);
        });
    });
}

void AddFrameListener(const WeakPtr<V8AnimatorBridge>& bridgeWeak, const RefPtr<KeyframeAnimation<double>>& animation,
    v8::Isolate* isolate)
{
    animation->AddListener([isolate, bridgeWeak](double value) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, isolate, value]() mutable {
            CallAnimationFrameJs(bridgeWeak, isolate, value);
        });
    });
}

} // namespace

v8::Local<v8::Object> V8AnimatorBridgeUtils::CreateAnimatorContext(v8::Local<v8::Context>& ctx, int32_t pageId,
    int32_t bridgeId)
{
    const std::unordered_map<const char*, v8::Local<v8::Function>> contextTable = {
        { "play", v8::Function::New(ctx, JsAnimatorPlay, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "finish", v8::Function::New(ctx, JsAnimatorFinish, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "pause", v8::Function::New(ctx, JsAnimatorPause, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "cancel", v8::Function::New(ctx, JsAnimatorCancel, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "reverse", v8::Function::New(ctx, JsAnimatorReverse, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "update", v8::Function::New(ctx, JsAnimatorUpdate, v8::Local<v8::Value>(), 1).ToLocalChecked() },
    };
    auto animatorContext = v8::Object::New(ctx->GetIsolate());
    for (const auto& iter : contextTable) {
        animatorContext->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), iter.first).ToLocalChecked(), iter.second)
            .ToChecked();
    }
    animatorContext->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "__pageId").ToLocalChecked(),
        v8::Int32::New(ctx->GetIsolate(), pageId)).ToChecked();
    animatorContext->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "__bridgeId").ToLocalChecked(),
        v8::Int32::New(ctx->GetIsolate(), bridgeId)).ToChecked();
    return animatorContext;
}

void V8AnimatorBridgeUtils::JsAnimatorPlay(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 0) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    int32_t bridgeId = GetCurrentBridgeId(ctx, args.Holder());
    HandleJsAnimatorContext(ctx, pageId, bridgeId, AnimatorOperation::PLAY);
}

void V8AnimatorBridgeUtils::JsAnimatorFinish(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 0) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    int32_t bridgeId = GetCurrentBridgeId(ctx, args.Holder());
    HandleJsAnimatorContext(ctx, pageId, bridgeId, AnimatorOperation::FINISH);
}

void V8AnimatorBridgeUtils::JsAnimatorPause(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 0) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    int32_t bridgeId = GetCurrentBridgeId(ctx, args.Holder());
    HandleJsAnimatorContext(ctx, pageId, bridgeId, AnimatorOperation::PAUSE);
}

void V8AnimatorBridgeUtils::JsAnimatorCancel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 0) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    int32_t bridgeId = GetCurrentBridgeId(ctx, args.Holder());
    HandleJsAnimatorContext(ctx, pageId, bridgeId, AnimatorOperation::CANCEL);
}

void V8AnimatorBridgeUtils::JsAnimatorReverse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 0) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    int32_t bridgeId = GetCurrentBridgeId(ctx, args.Holder());
    HandleJsAnimatorContext(ctx, pageId, bridgeId, AnimatorOperation::REVERSE);
}

void V8AnimatorBridgeUtils::JsAnimatorUpdate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1) {
        LOGE("args length error, length: %{public}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    v8::Local<v8::Object> valObj = args[0]->ToObject(ctx).ToLocalChecked();
    v8::Local<v8::Array> valObjProperties = valObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    uint32_t objLen = valObjProperties->Length();
    std::unordered_map<std::string, std::string> params;
    for (uint32_t i = 0; i < objLen; i++) {
        v8::Local<v8::Value> valObjKey;
        bool succ = valObjProperties->Get(ctx, i).ToLocal(&valObjKey);
        if (!succ) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value valObjKeyStr(isolate, valObjKey);
        const char* keyStr = *valObjKeyStr;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> valObjVal = valObj->Get(ctx, valObjKey).ToLocalChecked();
        if (valObjVal->IsString() || valObjVal->IsNumber()) {
            v8::String::Utf8Value valObjValStr(isolate, valObjVal);
            const char* valStr = *valObjValStr;
            if (valStr == nullptr) {
                continue;
            }
            params[keyStr] = valStr;
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
    int32_t pageId = GetCurrentPageId(ctx, args.Holder());
    int32_t bridgeId = GetCurrentBridgeId(ctx, args.Holder());
    auto page = GetPageById(isolate, pageId);
    if (!page) {
        LOGE("no page found for bridgeId: %{public}d", bridgeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return;
    }
    auto task = AceType::MakeRefPtr<V8AnimatorTaskUpdate>(isolate, params);
    page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimator>(bridgeId, task));
}

int32_t V8AnimatorBridgeUtils::JsCreateBridgeId()
{
    static int32_t bridgeId = 0;
    return bridgeId++;
}

V8AnimatorBridge::V8AnimatorBridge(
    const v8::Local<v8::Context>& ctx, v8::Isolate* instance, v8::Local<v8::Object> animatorContext)
    : instance_(instance)
{
    animatorObject_.Reset(instance_, animatorContext);
    ctx_.Reset(instance_, ctx);
}

V8AnimatorBridge::~V8AnimatorBridge()
{
    RefPtr<Animator> animator;
    animator.Swap(animator_);
    auto taskExecutor = Container::CurrentTaskExecutor();
    if (taskExecutor) {
        taskExecutor->PostSyncTask(
            [&animator]() {
                LOGI("release animator on UI thread");
                animator.Reset();
            },
            TaskExecutor::TaskType::UI);
    }
}

void V8AnimatorBridge::JsCreateAnimation(const std::string& param)
{
    int32_t iterations = 0;
    double duration = 0.0;
    double delay = 0.0;
    std::unordered_map<std::string, double> animationDoubleParams;
    std::unordered_map<std::string, std::string> animationStringParams;
    BaseAnimationBridgeUtils::JsParseAnimatorParams(param, iterations, animationDoubleParams, animationStringParams);
    RefPtr<Curve> curve;
    std::string curveString;
    auto iterEasing = animationStringParams.find(DOM_ANIMATION_EASING);
    if (iterEasing != animationStringParams.end()) {
        curveString = iterEasing->second;
    }
    curve = CreateCurve(curveString);
    auto iterDuration = animationDoubleParams.find(DOM_ANIMATION_DURATION_API);
    if (iterDuration != animationDoubleParams.end()) {
        duration = iterDuration->second;
    }
    std::string fillString;
    auto iterFill = animationStringParams.find(DOM_ANIMATION_FILL);
    if (iterFill != animationStringParams.end()) {
        fillString = iterFill->second;
    }
    auto iterDelay = animationDoubleParams.find(DOM_ANIMATION_DELAY_API);
    if (iterDelay != animationDoubleParams.end()) {
        delay = iterDelay->second;
    }
    if (!instance_) {
        LOGE("instance is null");
        return;
    }
    auto keyframeAnimation = CreateDoubleAnimation(animationDoubleParams, curve);
    if (!animator_) {
        animator_ = AceType::MakeRefPtr<Animator>();
    }
    if (!animator_->IsStopped()) {
        animator_->Stop();
    }
    auto iterDirection = animationStringParams.find(DOM_ANIMATION_DIRECTION_API);
    if (iterDirection != animationStringParams.end()) {
        animator_->SetAnimationDirection(StringToAnimationDirection(iterDirection->second));
    }
    animator_->ClearInterpolators();
    animator_->SetDuration(duration);
    animator_->SetIteration(iterations);
    animator_->SetStartDelay(delay);
    animator_->SetFillMode(StringToFillMode(fillString));
    animator_->AddInterpolator(keyframeAnimation);
    AddListenerForEventCallback(AceType::WeakClaim(this), animator_, instance_);
}

RefPtr<KeyframeAnimation<double>> V8AnimatorBridge::CreateDoubleAnimation(
    std::unordered_map<std::string, double>& animationParams, const RefPtr<Curve>& curve)
{
    double begin = 0.0;
    double end = 1.0;
    auto animationBegin = animationParams.find(DOM_ANIMATION_BEGIN);
    if (animationBegin != animationParams.end()) {
        begin = animationBegin->second;
    }
    auto animationEnd = animationParams.find(DOM_ANIMATION_END);
    if (animationEnd != animationParams.end()) {
        end = animationEnd->second;
    }
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<double>>(0.0, begin);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<double>>(1.0, end);
    auto keyframeAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    keyframeAnimation->AddKeyframe(keyframeBegin);
    keyframeAnimation->AddKeyframe(keyframeEnd);
    keyframeAnimation->SetCurve(curve);
    AddFrameListener(AceType::WeakClaim(this), keyframeAnimation, instance_);
    return keyframeAnimation;
}

V8AnimatorTaskCreate::V8AnimatorTaskCreate(
    v8::Isolate* isolate, const RefPtr<V8AnimatorBridge>& bridge, const std::string& param)
    : bridge_(bridge), isolate_(isolate), param_(std::move(param))
{}

void V8AnimatorTaskCreate::AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId)
{
    if (!bridge_) {
        LOGE("Create Animation Bridge failed. bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    auto bridgeFree = AceType::DynamicCast<V8AnimatorBridge>(page->GetAnimatorBridge(bridgeId));
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate_->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    auto jsTaskExecutor = (*delegate)->GetAnimationJsTask();
    if (bridgeFree) {
        auto weakBridge = AceType::WeakClaim(AceType::RawPtr(bridgeFree));
        jsTaskExecutor.PostTask([weakBridge]() mutable {
            auto bridgeFree = weakBridge.Upgrade();
            if (bridgeFree != nullptr) {
                bridgeFree.Reset();
            }
        });
    }
    page->RemoveAnimatorBridge(bridgeId);
    bridge_->JsCreateAnimation(param_);
    page->AddAnimatorBridge(bridgeId, bridge_);
}

void V8AnimatorTaskOperation::AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId)
{
    auto animatorBridge = AceType::DynamicCast<V8AnimatorBridge>(page->GetAnimatorBridge(bridgeId));
    if (!animatorBridge) {
        LOGE("no animation bridge found for bridgeId: %{public}d", bridgeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    RefPtr<Animator> animator = animatorBridge->JsGetAnimator();
    if (!animator) {
        LOGE("animator is null");
        return;
    }
    switch (operation_) {
        case AnimatorOperation::PLAY:
            animator->Play();
            break;
        case AnimatorOperation::PAUSE:
            animator->Pause();
            break;
        case AnimatorOperation::CANCEL:
            animator->Cancel();
            break;
        case AnimatorOperation::FINISH:
            animator->Finish();
            break;
        case AnimatorOperation::REVERSE:
            animator->Reverse();
            break;
        case AnimatorOperation::NONE:
        default:
            break;
    }
}

void V8AnimatorTaskUpdate::AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId)
{
    auto animatorBridge = AceType::DynamicCast<V8AnimatorBridge>(page->GetAnimatorBridge(bridgeId));
    if (!animatorBridge) {
        LOGE("no animation bridge found for bridgeId: %{public}d", bridgeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    RefPtr<Animator> animator = animatorBridge->JsGetAnimator();
    if (!animator) {
        LOGE("animator is null");
        return;
    }
    if (!isolate_) {
        LOGE("isolate is null");
        return;
    }
    animator->ClearInterpolators();
    UpdateAnimator(animator, animatorBridge, isolate_, params_);
}

void V8AnimatorTaskUpdate::UpdateAnimator(const RefPtr<Animator>& animator, const RefPtr<V8AnimatorBridge>& bridge,
    v8::Isolate* isolate, const std::unordered_map<std::string, std::string>& params)
{
    int32_t iterations = 1;
    double duration = 0.0;
    double delay = 0.0;
    double begin = 0.0;
    double end = 1.0;
    std::string curveString;
    std::string fillString;
    RefPtr<Curve> curve;
    auto iterEasing = params_.find(DOM_ANIMATION_EASING);
    if (iterEasing != params_.end()) {
        curveString = iterEasing->second;
    }
    curve = CreateCurve(curveString);
    auto iterIterations = params_.find(DOM_ANIMATION_ITERATIONS);
    if (iterIterations != params_.end()) {
        iterations = StringToInt(iterIterations->second);
    }
    auto iterDuration = params_.find(DOM_ANIMATION_DURATION_API);
    if (iterDuration != params_.end()) {
        duration = StringToDouble(iterDuration->second);
    }
    auto iterFill = params_.find(DOM_ANIMATION_FILL);
    if (iterFill != params_.end()) {
        fillString = iterFill->second;
    }
    auto iterDelay = params_.find(DOM_ANIMATION_DELAY_API);
    if (iterDelay != params_.end()) {
        delay = StringToDouble(iterDelay->second);
    }
    auto iterDirection = params_.find(DOM_ANIMATION_DIRECTION_API);
    if (iterDirection != params_.end()) {
        animator->SetAnimationDirection(StringToAnimationDirection(iterDirection->second));
    }
    auto animationBegin = params_.find(DOM_ANIMATION_BEGIN);
    if (animationBegin != params_.end()) {
        begin = StringToDouble(animationBegin->second);
    }
    auto animationEnd = params_.find(DOM_ANIMATION_END);
    if (animationEnd != params_.end()) {
        end = StringToDouble(animationEnd->second);
    }
    auto keyframeAnimation = CreateDoubleAnimation(begin, end, curve);
    AddFrameListener(AceType::WeakClaim(RawPtr(bridge)), keyframeAnimation, isolate);
    animator->SetDuration(duration);
    animator->SetIteration(iterations);
    animator->SetStartDelay(delay);
    animator->SetFillMode(StringToFillMode(fillString));
    animator->AddInterpolator(keyframeAnimation);
}

RefPtr<KeyframeAnimation<double>> V8AnimatorTaskUpdate::CreateDoubleAnimation(double begin, double end,
    const RefPtr<Curve>& curve)
{
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<double>>(0.0, begin);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<double>>(1.0, end);
    auto keyframeAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    keyframeAnimation->AddKeyframe(keyframeBegin);
    keyframeAnimation->AddKeyframe(keyframeEnd);
    keyframeAnimation->SetCurve(curve);
    return keyframeAnimation;
}

} // namespace OHOS::Ace::Framework
