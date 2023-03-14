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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_animation_bridge.h"

#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/memory/referenced.h"
#include "base/utils/string_utils.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/tween/tween_component.h"
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
    LOGD("Enter GetPageById");
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

inline NodeId GetCurrentNodeId(shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& value)
{
    if (!value) {
        LOGE("Get current node id failed. value is null.");
        return 0;
    }
    shared_ptr<JsValue> jsNodeId = value->GetProperty(runtime, "__nodeId");
    if (!jsNodeId || !jsNodeId->IsInt32(runtime)) {
        LOGE("Get current node id failed. jsNodeId is null or not a integer");
        return 0;
    }

    NodeId id = jsNodeId->ToInt32(runtime);
    if (id < 0) {
        return 0;
    }
    return id;
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
    if (pageId < 0) {
        return 0;
    }
    return pageId;
}

void HandleJsAnimationContext(
    const shared_ptr<JsRuntime>& runtime, int32_t pageId, int32_t nodeId, AnimationOperation operation)
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
        LOGE("no page found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return;
    }
    auto task = AceType::MakeRefPtr<JsiAnimationBridgeTaskOperation>(operation);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    if (page->CheckPageCreated()) {
        delegate->TriggerPageUpdate(page->GetPageId());
    }
}

const std::vector<std::tuple<std::string, RegisterFunctionType, RegisterFunctionType>> JSI_ANIMATION_FUNCS = {
    { "playState", JsiAnimationBridgeUtils::JsAnimationPlayStateGet, JsiAnimationBridgeUtils::JsAnimationPlayStateSet },
    { "startTime", JsiAnimationBridgeUtils::JsAnimationStartTimeGet, JsiAnimationBridgeUtils::JsAnimationStartTimeSet },
    { "pending", JsiAnimationBridgeUtils::JsAnimationPendingGet, JsiAnimationBridgeUtils::JsAnimationPendingSet }
};

void CallAnimationFinishJs(const WeakPtr<JsiAnimationBridge>& bridgeWeak, shared_ptr<JsRuntime>&& runtime,
    const RefPtr<JsAcePage>& page)
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

void CallAnimationCancelJs(const WeakPtr<JsiAnimationBridge>& bridgeWeak, shared_ptr<JsRuntime>&& runtime,
    const RefPtr<JsAcePage>& page)
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

    LOGD("animation oncancel event call");
    jsFunc->Call(runtime, runtime->GetGlobal(), {}, 0);
}

void JsUpdatePlayState(
    shared_ptr<JsRuntime>&& runtime, const WeakPtr<JsiAnimationBridge>& bridgeWeak, const char* state)
{
    if (!runtime) {
        LOGE("Set playState failed. runtime is null.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_BRIDGE_ERR);
        return;
    }
    auto bridge = bridgeWeak.Upgrade();
    if (!bridge) {
        LOGW("Set playState failed. bridge is null.");
        return;
    }
    auto animationContext = bridge->GetJsObject();
    if (!animationContext) {
        LOGW("Set playState failed. animationContext is null.");
        return;
    }
    bool succ = animationContext->SetProperty(runtime, "__playState", runtime->NewString(state));
    if (!succ) {
        LOGW("Set playState failed.");
    }
}

void AddListenerForEventCallback(const WeakPtr<JsiAnimationBridge>& bridgeWeak, const RefPtr<Animator>& animator,
    shared_ptr<JsRuntime> runtime, const RefPtr<JsAcePage>& page)
{
    std::weak_ptr<JsRuntime> weakRuntime(runtime);
    animator->AddStopListener([weakRuntime, bridgeWeak, page] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle Stop listener failed, fail to get delegate");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, weakRuntime, page]() mutable {
            LOGI("call animation onfinish event");
            CallAnimationFinishJs(bridgeWeak, weakRuntime.lock(), page);
        });
    });
    animator->AddIdleListener([weakRuntime, bridgeWeak, page] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle Idle listener failed, fail to get delegate");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, weakRuntime, page]() mutable {
            LOGI("call animation oncancel event");
            CallAnimationCancelJs(bridgeWeak, weakRuntime.lock(), page);
        });
    });
}

} // namespace

JsiAnimationBridge::JsiAnimationBridge(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& animationContext, NodeId nodeId)
    : animationObject_(animationContext), nodeId_(nodeId)
{
    runtime_ = runtime;
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationStartTimeGet(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (!thisObj) {
        LOGE("JsAnimationStartTimeGet failed. thisObj is null.");
        return runtime->NewInt32(0);
    }
    int32_t nodeId = GetCurrentNodeId(runtime, thisObj);
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    auto page = GetPageById(runtime, pageId);
    if (!page) {
        LOGE("JsAnimationStartTimeGet: no page found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return runtime->NewInt32(0);
    }
    auto domDocument = page->GetDomDocument();
    if (!domDocument) {
        LOGE("JsAnimationStartTimeGet failed, DomDocument is null.");
        return runtime->NewInt32(0);
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (!domNode) {
        LOGE("JsAnimationStartTimeGet failed, DomNode is null.");
        return runtime->NewInt32(0);
    }
    auto tweenComponent = domNode->GetTweenComponent();
    if (tweenComponent) {
        auto option = tweenComponent->GetCustomTweenOption();
        auto startTime = option.GetDelay();
        thisObj->SetProperty(runtime, "__startTime", runtime->NewInt32(startTime));
    }
    shared_ptr<JsValue> jsDelay = thisObj->GetProperty(runtime, "__startTime");
    int32_t delay = jsDelay->IsInt32(runtime) ? jsDelay->ToInt32(runtime) : 0;
    shared_ptr<JsValue> jsResult = runtime->NewInt32(delay);
    return jsResult;
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationStartTimeSet(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (!thisObj) {
        LOGE("JsAnimationStartTimeSet failed. thisObj is null.");
        return runtime->NewInt32(0);
    }
    if (argv.size() != 1) {
        LOGE("Not valid Length for info. length: %{public}u", (uint32_t)argv.size());
        return runtime->NewUndefined();
    }
    shared_ptr<JsValue> jsStartTime = argv[0];
    if (!jsStartTime) {
        return runtime->NewUndefined();
    }
    std::string startTime = jsStartTime->ToString(runtime);
    int32_t nodeId = GetCurrentNodeId(runtime, thisObj);
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    auto page = GetPageById(runtime, pageId);
    if (!page) {
        LOGE("JsAnimationStartTimeSet: no page found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return runtime->NewUndefined();
    }
    auto task = AceType::MakeRefPtr<JsiAnimationBridgeTaskStartTime>(startTime);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationPendingGet(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (!thisObj) {
        LOGE("JsAnimationPendingGet failed. thisObj is null.");
        return runtime->NewBoolean(true);
    }
    int32_t nodeId = GetCurrentNodeId(runtime, thisObj);
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    auto page = GetPageById(runtime, pageId);
    if (!page) {
        LOGE("JsAnimationPendingGet: no page found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return runtime->NewBoolean(true);
    }
    auto domDocument = page->GetDomDocument();
    if (!domDocument) {
        LOGE("JsAnimationPendingGet failed, DomDocument is null.");
        return runtime->NewBoolean(true);
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (!domNode) {
        LOGE("JsAnimationPendingGet failed, DomNode is null.");
        return runtime->NewBoolean(true);
    }
    auto tweenComponent = domNode->GetTweenComponent();
    if (tweenComponent) {
        auto controller = tweenComponent->GetAnimator();
        if (controller) {
            thisObj->SetProperty(runtime, "__pending", runtime->NewBoolean(controller->IsPending()));
        }
    }
    shared_ptr<JsValue> jsPending = thisObj->GetProperty(runtime, "__pending");
    bool pending = false;
    if (jsPending) {
        pending = jsPending->IsBoolean(runtime) ? jsPending->ToBoolean(runtime) : false;
    }
    return runtime->NewBoolean(pending);
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationPendingSet(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationPlayStateGet(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (!thisObj) {
        LOGE("JsAnimationPlayStateGet failed. thisObj is null.");
        return runtime->NewUndefined();
    }
    shared_ptr<JsValue> jsPending = thisObj->GetProperty(runtime, "__playState");
    if (!jsPending) {
        LOGE("JsAnimationPlayStateGet: no pending find.");
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return runtime->NewUndefined();
    }
    return runtime->NewString(jsPending->ToString(runtime));
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationPlayStateSet(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.size() != 1) {
        LOGE("Not valid Length for info. length: %{public}u", (uint32_t)argv.size());
        return runtime->NewUndefined();
    }
    shared_ptr<JsValue> jsPlayState = argv[0];
    if (!jsPlayState || !jsPlayState->IsString(runtime)) {
        LOGE("Not valid type for value.");
        return runtime->NewUndefined();
    }
    std::string playState = jsPlayState->ToString(runtime);
    AnimationOperation operation = StringToAnimationOperation(playState);
    int32_t nodeId = GetCurrentNodeId(runtime, thisObj);
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    auto page = GetPageById(runtime, pageId);
    if (!page) {
        LOGE("no page found for nodeId: %{public}d", nodeId);
        EventReport::SendAnimationException(AnimationExcepType::ANIMATION_PAGE_ERR);
        return runtime->NewUndefined();
    }
    auto task = AceType::MakeRefPtr<JsiAnimationBridgeTaskOperation>(operation);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    return runtime->NewUndefined();
}

void JsiAnimationBridge::JsCreateAnimation(const RefPtr<JsAcePage>& page, const std::string& param)
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
    auto iterDelay = animationDoubleOptions.find(DOM_ANIMATION_DELAY_API);
    if (iterDelay != animationDoubleOptions.end()) {
        tweenOption.SetDelay(iterDelay->second);
    }
    tweenOption.SetIteration(iterations);
    if (SystemProperties::GetRosenBackendEnabled()) {
        tweenOption.SetAllowRunningAsynchronously(true);
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
    LOGD("parse animate parameters for nodeId: %{public}d", nodeId_);
    tweenComponent->SetAnimator(animator);
    BaseAnimationBridgeUtils::SetTweenComponentParams(nullptr, animationFrames, tweenComponent, tweenOption);
    AddListenerForEventCallback(AceType::WeakClaim(this), animator, runtime_, page);
    domNode->GenerateComponentNode();
    page->PushDirtyNode(nodeId_);
}

void JsiAnimationBridge::SetPlayStateCallbacksWithListenerId(RefPtr<Animator>& animator)
{
    WeakPtr<JsiAnimationBridge> bridgeWeak = AceType::WeakClaim(this);
    animator->RemoveStopListener(finishListenerId_);
    std::weak_ptr<JsRuntime> weakRuntime(runtime_);
    finishListenerId_ = animator->AddStopListener([bridgeWeak, weakRuntime] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle stop callback failed. delegate is null.");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([weakRuntime, bridgeWeak]() mutable {
            JsUpdatePlayState(weakRuntime.lock(), bridgeWeak, DOM_ANIMATION_PLAY_STATE_FINISHED);
        });
    });
    animator->RemoveIdleListener(idleListenerId_);
    idleListenerId_ = animator->AddIdleListener([bridgeWeak, weakRuntime] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle idle callback failed. delegate is null.");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([weakRuntime, bridgeWeak]() mutable {
            JsUpdatePlayState(weakRuntime.lock(), bridgeWeak, DOM_ANIMATION_PLAY_STATE_IDLE);
        });
    });
}

void JsiAnimationBridge::SetPlayStateCallbacks(RefPtr<Animator>& animator)
{
    if (!animator) {
        LOGE("Set PlayState callbacks failed. simulation controller is null.");
        return;
    }
    WeakPtr<JsiAnimationBridge> bridgeWeak = AceType::WeakClaim(this);
    SetPlayStateCallbacksWithListenerId(animator);
    animator->ClearPauseListeners();
    std::weak_ptr<JsRuntime> weakRuntime(runtime_);
    animator->AddPauseListener([bridgeWeak, weakRuntime] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle pause callback failed. delegate is null.");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([weakRuntime, bridgeWeak]() mutable {
            JsUpdatePlayState(weakRuntime.lock(), bridgeWeak, DOM_ANIMATION_PLAY_STATE_PAUSED);
        });
    });
    animator->ClearStartListeners();
    animator->AddStartListener([bridgeWeak, weakRuntime] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle start callback failed. delegate is null.");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([weakRuntime, bridgeWeak]() mutable {
            JsUpdatePlayState(weakRuntime.lock(), bridgeWeak, DOM_ANIMATION_PLAY_STATE_RUNNING);
        });
    });
    animator->ClearResumeListeners();
    animator->AddResumeListener([bridgeWeak, weakRuntime] {
        auto delegate = GetFrontendDelegate(weakRuntime);
        if (!delegate) {
            LOGE("Handle resume callback failed. delegate is null.");
            return;
        }
        auto jsTaskExecutor = delegate->GetAnimationJsTask();
        jsTaskExecutor.PostTask([weakRuntime, bridgeWeak]() mutable {
            JsUpdatePlayState(weakRuntime.lock(), bridgeWeak, DOM_ANIMATION_PLAY_STATE_RUNNING);
        });
    });
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationPlay(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t nodeId = GetCurrentNodeId(runtime, thisObj);
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    HandleJsAnimationContext(runtime, pageId, nodeId, AnimationOperation::PLAY);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationFinish(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t nodeId = GetCurrentNodeId(runtime, thisObj);
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    HandleJsAnimationContext(runtime, pageId, nodeId, AnimationOperation::FINISH);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationPause(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t nodeId = GetCurrentNodeId(runtime, thisObj);
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    HandleJsAnimationContext(runtime, pageId, nodeId, AnimationOperation::PAUSE);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationCancel(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t nodeId = GetCurrentNodeId(runtime, thisObj);
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    HandleJsAnimationContext(runtime, pageId, nodeId, AnimationOperation::CANCEL);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::JsAnimationReverse(shared_ptr<JsRuntime> runtime,
    shared_ptr<JsValue> thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t nodeId = GetCurrentNodeId(runtime, thisObj);
    int32_t pageId = GetCurrentPageId(runtime, thisObj);
    HandleJsAnimationContext(runtime, pageId, nodeId, AnimationOperation::REVERSE);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiAnimationBridgeUtils::CreateAnimationContext(
    shared_ptr<JsRuntime> runtime, int32_t pageId, NodeId nodeId)
{
    const std::unordered_map<const char*, RegisterFunctionType> contextTable = {
        { "play", JsAnimationPlay },
        { "finish", JsAnimationFinish },
        { "pause", JsAnimationPause },
        { "cancel", JsAnimationCancel },
        { "reverse", JsAnimationReverse },
    };

    auto animationContext = runtime->NewObject();
    for (const auto& iter : contextTable) {
        animationContext->SetProperty(runtime, iter.first, runtime->NewFunction(iter.second));
    }
    animationContext->SetProperty(runtime, "__nodeId", runtime->NewInt32(nodeId));
    animationContext->SetProperty(runtime, "__pageId", runtime->NewInt32(pageId));
    animationContext->SetProperty(runtime, "__playState", runtime->NewString(DOM_ANIMATION_PLAY_STATE_IDLE));
    animationContext->SetProperty(runtime, "finished", runtime->NewBoolean(false));
    for (const auto& item : JSI_ANIMATION_FUNCS) {
        auto getterTempl = runtime->NewFunction(std::get<1>(item));
        auto setterTempl = runtime->NewFunction(std::get<2>(item));
        bool ret = animationContext->SetAccessorProperty(runtime, std::get<0>(item), getterTempl, setterTempl);
        if (!ret) {
            LOGE("Animation set accessor property failed., name: %{public}s", std::get<0>(item).c_str());
        }
    }
    return animationContext;
}

JsiAnimationBridgeTaskCreate::JsiAnimationBridgeTaskCreate(
    shared_ptr<JsRuntime> runtime, const RefPtr<JsiAnimationBridge>& bridge, std::string param)
    : bridge_(bridge), runtime_(runtime), param_(std::move(param))
{}

void JsiAnimationBridgeTaskCreate::AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId)
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
    auto bridgeFree = AceType::DynamicCast<JsiAnimationBridge>(page->GetAnimationBridge(nodeId));
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
    bridge_->JsCreateAnimation(page, param_);
    page->AddAnimationBridge(nodeId, bridge_);
}

void JsiAnimationBridgeTaskOperation::AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId)
{
    auto animationBridge = AceType::DynamicCast<JsiAnimationBridge>(page->GetAnimationBridge(nodeId));
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

void JsiAnimationBridgeTaskStartTime::AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId)
{
    if (!page) {
        LOGE("JsiAnimationBridgeTaskStartTime: Get page is error");
        return;
    }
    auto domDocument = page->GetDomDocument();
    if (!domDocument) {
        LOGE("AnimationBridgeTaskStartTime failed, DomDocument is null.");
        return;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (!domNode) {
        LOGE("AnimationBridgeTaskStartTime failed, DomNode is null.");
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
