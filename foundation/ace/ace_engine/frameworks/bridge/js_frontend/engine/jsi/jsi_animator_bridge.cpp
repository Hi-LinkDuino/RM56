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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_animator_bridge.h"

#include "base/log/event_report.h"
#include "base/log/log.h"
#include "core/common/container.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace::Framework {
namespace {

RefPtr<FrontendDelegate> GetFrontendDelegate(shared_ptr<JsRuntime>&& runtime)
{
    if (!runtime) {
        LOGE("Get front delegate failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (engine == nullptr) {
        LOGE("Get front delegate failed. engin is null");
        return nullptr;
    }
    return engine->GetFrontendDelegate();
}

RefPtr<FrontendDelegate> GetFrontendDelegate(std::weak_ptr<JsRuntime> weakRuntime)
{
    return GetFrontendDelegate(weakRuntime.lock());
}

RefPtr<JsAcePage> GetPageById(const shared_ptr<JsRuntime>& runtime, int32_t pageId)
{
    if (!runtime) {
        LOGE("Get page by id failed, runtime is null");
        return nullptr;
    }
    auto delegate = GetFrontendDelegate(runtime);
    if (!delegate) {
        LOGE("Get page by id failed, delegate is null");
        return nullptr;
    }
    return delegate->GetPage(pageId);
}

inline int32_t GetCurrentBridgeId(shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& value)
{
    if (!value) {
        LOGE("Get current bridge id failed. value is null.");
        return 0;
    }
    shared_ptr<JsValue> jsBridgeId = value->GetProperty(runtime, "__bridgeId");
    if (!jsBridgeId || !jsBridgeId->IsInt32(runtime)) {
        LOGE("Get current bridge id failed. jsBridgeId is null or not a integer");
        return 0;
    }
    int32_t bridgeId = jsBridgeId->ToInt32(runtime);
    return bridgeId < 0 ? 0 : bridgeId;
}

inline int32_t GetCurrentPageId(shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& value)
{
    if (!value) {
        LOGE("Get current page id failed. value is null.");
        return 0;
    }
    shared_ptr<JsValue> jsPageId = value->GetProperty(runtime, "__pageId");
    if (!jsPageId || !jsPageId->IsInt32(runtime)) {
        LOGE("Get current page id failed. jsPageId is null or not a integer");
        return 0;
    }
    int32_t pageId = jsPageId->ToInt32(runtime);
    return pageId < 0 ? 0 : pageId;
}

void HandleJsAnimatorContext(const shared_ptr<JsRuntime>& runtime, int32_t pageId, int32_t bridgeId,
    AnimatorOperation operation)
{
    if (!runtime) {
        LOGE("Handle JsAnimationContext failed, runtime is null.");
        return;
    }
    auto delegate = GetFrontendDelegate(runtime);
    if (!delegate) {
        LOGE("Handle JsAnimationContext failed, delegate is null.");
        return;
    }
    auto page = GetPageById(runtime, pageId);
    if (!page) {
        LOGE("no page found for pageId: %{public}d", pageId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return;
    }
    auto task = AceType::MakeRefPtr<JsiAnimatorTaskOperation>(operation);
    page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimator>(bridgeId, task));
    if (page->CheckPageCreated()) {
        delegate->TriggerPageUpdate(page->GetPageId());
    }
}

void CallAnimationStartJs(const WeakPtr<JsiAnimatorBridge>& bridgeWeak, shared_ptr<JsRuntime>&& runtime)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Start Js Failed. animation bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    if (!runtime) {
        LOGE("Call Animation Start Js Failed. runtime is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    auto animationObject = bridge->GetJsObject();
    if (!animationObject || animationObject->IsNull(runtime)) {
        LOGE("Animation Object is null");
        return;
    }
    shared_ptr<JsValue> jsFunc = animationObject->GetProperty(runtime, "onstart");
    if (!jsFunc || !jsFunc->IsFunction(runtime)) {
        LOGD("cannot find 'CallAnimationStartJs' function from animation object, maybe no callback at all.");
        return;
    }
    jsFunc->Call(runtime, runtime->GetGlobal(), {}, 0);
}

void CallAnimationFinishJs(const WeakPtr<JsiAnimatorBridge>& bridgeWeak, shared_ptr<JsRuntime>&& runtime)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Finish Js Failed. animation bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    if (!runtime) {
        LOGE("Call Animation Finish Js Failed. runtime is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    auto animationObject = bridge->GetJsObject();
    if (!animationObject || animationObject->IsNull(runtime)) {
        LOGE("Animation Object is null");
        return;
    }
    shared_ptr<JsValue> jsFunc = animationObject->GetProperty(runtime, "onfinish");
    if (!jsFunc || !jsFunc->IsFunction(runtime)) {
        LOGD("cannot find 'CallAnimationFinishJs' function from animation object, maybe no callback at all.");
        return;
    }
    jsFunc->Call(runtime, runtime->GetGlobal(), {}, 0);
}

void CallAnimationCancelJs(const WeakPtr<JsiAnimatorBridge>& bridgeWeak, shared_ptr<JsRuntime>&& runtime)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Cancel Js Failed. animation bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    if (!runtime) {
        LOGE("Call Animation Cancel Js Failed. runtime is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    auto animationObject = bridge->GetJsObject();
    if (!animationObject || animationObject->IsNull(runtime)) {
        LOGE("Animation Object is null");
        return;
    }
    shared_ptr<JsValue> jsFunc = animationObject->GetProperty(runtime, "oncancel");
    if (!jsFunc || !jsFunc->IsFunction(runtime)) {
        LOGD("cannot find 'CallAnimationCancelJs' function from animation object, maybe no callback at all.");
        return;
    }
    jsFunc->Call(runtime, runtime->GetGlobal(), {}, 0);
}

void CallAnimationRepeatJs(const WeakPtr<JsiAnimatorBridge>& bridgeWeak, shared_ptr<JsRuntime>&& runtime)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Repeat Js Failed. animation bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    if (!runtime) {
        LOGE("Call Animation Repeat Js Failed. runtime is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    auto animationObject = bridge->GetJsObject();
    if (!animationObject || animationObject->IsNull(runtime)) {
        LOGE("Animation Object is null");
        return;
    }
    shared_ptr<JsValue> jsFunc = animationObject->GetProperty(runtime, "onrepeat");
    if (!jsFunc || !jsFunc->IsFunction(runtime)) {
        LOGD("cannot find 'CallAnimationRepeatJs' function from animation object, maybe no callback at all.");
        return;
    }
    jsFunc->Call(runtime, runtime->GetGlobal(), {}, 0);
}

void CallAnimationFrameJs(const WeakPtr<JsiAnimatorBridge>& bridgeWeak, shared_ptr<JsRuntime>&& runtime, double value)
{
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGE("Call Animation Frame Js Failed. animation bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    if (!runtime) {
        LOGE("Call Animation Frame Js Failed. runtime is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    auto animationObject = bridge->GetJsObject();
    if (!animationObject || animationObject->IsNull(runtime)) {
        LOGE("Animation Object is null");
        return;
    }
    shared_ptr<JsValue> jsFunc = animationObject->GetProperty(runtime, "onframe");
    if (!jsFunc || !jsFunc->IsFunction(runtime)) {
        LOGD("cannot find 'CallAnimationFrameJs' function from animation object, maybe no callback at all.");
        return;
    }
    std::vector<shared_ptr<JsValue>> argv = { runtime->NewNumber(value) };
    jsFunc->Call(runtime, runtime->GetGlobal(), argv, argv.size());
}

void AddListenerForEventCallback(const WeakPtr<JsiAnimatorBridge>& bridgeWeak, const RefPtr<Animator>& animator,
    shared_ptr<JsRuntime> runtime)
{
    std::weak_ptr<JsRuntime> weakRuntime(runtime);
    animator->AddStartListener([weakRuntime, bridgeWeak] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle Start listener failed, fail to get delegate");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, weakRuntime]() mutable {
            LOGI("call animation onstart event");
            CallAnimationStartJs(bridgeWeak, weakRuntime.lock());
        });
    });
    animator->AddStopListener([weakRuntime, bridgeWeak] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle Stop listener failed, fail to get delegate");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, weakRuntime]() mutable {
            LOGI("call animation onfinish event");
            CallAnimationFinishJs(bridgeWeak, weakRuntime.lock());
        });
    });
    animator->AddIdleListener([weakRuntime, bridgeWeak] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle Idle listener failed, fail to get delegate");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, weakRuntime]() mutable {
            LOGI("call animation oncancel event");
            CallAnimationCancelJs(bridgeWeak, weakRuntime.lock());
        });
    });
    animator->AddRepeatListener([weakRuntime, bridgeWeak] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle Repeat listener failed, fail to get delegate");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, weakRuntime]() mutable {
            LOGI("call animation onrepeat event");
            CallAnimationRepeatJs(bridgeWeak, weakRuntime.lock());
        });
    });
}

void AddFrameListener(const WeakPtr<JsiAnimatorBridge>& bridgeWeak, const RefPtr<KeyframeAnimation<double>>& animator,
    shared_ptr<JsRuntime> runtime)
{
    std::weak_ptr<JsRuntime> weakRuntime(runtime);
    animator->AddListener([weakRuntime, bridgeWeak](double value) {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle Frame listener failed, fail to get delegate");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, weakRuntime, value]() mutable {
            LOGI("call animation onframe event");
            CallAnimationFrameJs(bridgeWeak, weakRuntime.lock(), value);
        });
    });
}

} // namespace

shared_ptr<JsValue> JsiAnimatorBridgeUtils::CreateAnimatorContext(
    shared_ptr<JsRuntime> runtime, int32_t pageId, int32_t bridgeId)
{
    const std::unordered_map<const char*, RegisterFunctionType> contextTable = {
        { "play", JsAnimatorPlay },
        { "finish", JsAnimatorFinish },
        { "pause", JsAnimatorPause },
        { "cancel", JsAnimatorCancel },
        { "reverse", JsAnimatorReverse },
        { "update", JsAnimatorUpdate },
    };
    auto animatorContext = runtime->NewObject();
    for (const auto& iter : contextTable) {
        animatorContext->SetProperty(runtime, iter.first, runtime->NewFunction(iter.second));
    }
    animatorContext->SetProperty(runtime, "__pageId", runtime->NewInt32(pageId));
    animatorContext->SetProperty(runtime, "__bridgeId", runtime->NewInt32(bridgeId));
    return animatorContext;
}

shared_ptr<JsValue> JsiAnimatorBridgeUtils::JsAnimatorPlay(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    int32_t bridgeId = GetCurrentBridgeId(runtime, thisObj);
    HandleJsAnimatorContext(runtime, pageId, bridgeId, AnimatorOperation::PLAY);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimatorBridgeUtils::JsAnimatorFinish(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    int32_t bridgeId = GetCurrentBridgeId(runtime, thisObj);
    HandleJsAnimatorContext(runtime, pageId, bridgeId, AnimatorOperation::FINISH);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimatorBridgeUtils::JsAnimatorPause(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    int32_t bridgeId = GetCurrentBridgeId(runtime, thisObj);
    HandleJsAnimatorContext(runtime, pageId, bridgeId, AnimatorOperation::PAUSE);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimatorBridgeUtils::JsAnimatorCancel(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    int32_t bridgeId = GetCurrentBridgeId(runtime, thisObj);
    HandleJsAnimatorContext(runtime, pageId, bridgeId, AnimatorOperation::CANCEL);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimatorBridgeUtils::JsAnimatorReverse(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    int32_t bridgeId = GetCurrentBridgeId(runtime, thisObj);
    HandleJsAnimatorContext(runtime, pageId, bridgeId, AnimatorOperation::REVERSE);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimatorBridgeUtils::JsAnimatorUpdate(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty()) {
        return runtime->NewUndefined();
    }
    shared_ptr<JsValue> paramObj = argv[0]->GetElement(runtime, 0);
    if (!paramObj || !paramObj->IsObject(runtime)) {
        LOGE("JsAnimatorUpdate failed, first argument is not an object!");
        return runtime->NewUndefined();
    }
    int32_t len = 0;
    shared_ptr<JsValue> properties;
    if (!paramObj->GetPropertyNames(runtime, properties, len)) {
        LOGE("JsAnimatorUpdate failed, fail to get object property list!");
        return runtime->NewUndefined();
    }
    std::unordered_map<std::string, std::string> params;
    for (int32_t i = 0; i < len; ++i) {
        shared_ptr<JsValue> key = properties->GetElement(runtime, i);
        shared_ptr<JsValue> val = paramObj->GetProperty(runtime, key);
        std::string keyStr = key->ToString(runtime);
        std::string valStr = val->ToString(runtime);
        params[keyStr] = valStr;
    }
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    int32_t bridgeId = GetCurrentBridgeId(runtime, thisObj);
    auto page = GetPageById(runtime, pageId);
    if (!page) {
        LOGE("no page found for pageId: %{public}d", pageId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return runtime->NewUndefined();
    }
    auto task = AceType::MakeRefPtr<JsiAnimatorTaskUpdate>(runtime, params);
    page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimator>(bridgeId, task));
    return runtime->NewUndefined();
}

int32_t JsiAnimatorBridgeUtils::JsCreateBridgeId()
{
    static int32_t bridgeId = 0;
    return bridgeId++;
}

JsiAnimatorBridge::JsiAnimatorBridge(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& animatorContext)
    : animatorObject_(animatorContext)
{
    runtime_ = runtime;
}

JsiAnimatorBridge::~JsiAnimatorBridge()
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

void JsiAnimatorBridge::JsCreateAnimation(const std::string& param)
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
    if (!runtime_) {
        LOGE("runtime is null");
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
    AddListenerForEventCallback(AceType::WeakClaim(this), animator_, runtime_);
}

RefPtr<KeyframeAnimation<double>> JsiAnimatorBridge::CreateDoubleAnimation(
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
    AddFrameListener(AceType::WeakClaim(this), keyframeAnimation, runtime_);
    return keyframeAnimation;
}

JsiAnimatorTaskCreate::JsiAnimatorTaskCreate(
    shared_ptr<JsRuntime> runtime, const RefPtr<JsiAnimatorBridge>& bridge, const std::string& param)
    : bridge_(bridge), runtime_(runtime), param_(std::move(param))
{}

void JsiAnimatorTaskCreate::AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId)
{
    if (!page) {
        LOGE("Create Animation Bridge failed. page is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    if (!bridge_) {
        LOGE("Create Animation Bridge failed. bridge is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    auto bridgeFree = AceType::DynamicCast<JsiAnimatorBridge>(page->GetAnimatorBridge(bridgeId));
    auto delegate = GetFrontendDelegate(runtime_);
    if (!delegate) {
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        LOGE("Create Animation Bridge failed. delegate is null.");
        return;
    }
    auto jsTaskExecutor = delegate->GetAnimationJsTask();
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

void JsiAnimatorTaskOperation::AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId)
{
    if (!page) {
        LOGE("AnimatorBridgeTaskFunc failed. page is null");
        return;
    }
    auto animatorBridge = AceType::DynamicCast<JsiAnimatorBridge>(page->GetAnimatorBridge(bridgeId));
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

void JsiAnimatorTaskUpdate::AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId)
{
    if (!page) {
        LOGE("AnimatorBridgeTaskFunc failed. page is null");
        return;
    }
    auto animatorBridge = AceType::DynamicCast<JsiAnimatorBridge>(page->GetAnimatorBridge(bridgeId));
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
    if (!runtime_) {
        LOGE("runtime is null");
        return;
    }
    animator->ClearInterpolators();
    UpdateAnimator(animator, animatorBridge, runtime_, params_);
}

void JsiAnimatorTaskUpdate::UpdateAnimator(const RefPtr<Animator>& animator, const RefPtr<JsiAnimatorBridge>& bridge,
    shared_ptr<JsRuntime> runtime, const std::unordered_map<std::string, std::string>& params)
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
    AddFrameListener(AceType::WeakClaim(RawPtr(bridge)), keyframeAnimation, runtime);
    animator->SetDuration(duration);
    animator->SetIteration(iterations);
    animator->SetStartDelay(delay);
    animator->SetFillMode(StringToFillMode(fillString));
    animator->AddInterpolator(keyframeAnimation);
}

RefPtr<KeyframeAnimation<double>> JsiAnimatorTaskUpdate::CreateDoubleAnimation(
    double begin, double end, const RefPtr<Curve>& curve)
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
