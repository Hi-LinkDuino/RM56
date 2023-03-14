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

#include "frameworks/bridge/js_frontend/engine/quickjs/animation_bridge.h"

#include <utility>

#include "base/utils/string_utils.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_engine.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_group_js_bridge.h"

namespace OHOS::Ace::Framework {

namespace {

RefPtr<JsAcePage> GetPageById(QjsEngineInstance* instance, int32_t pageId)
{
    LOGD("Enter GetPageById");
    if (instance == nullptr) {
        LOGE("instance is null.");
        return nullptr;
    }
    auto delegate = instance->GetDelegate();
    if (delegate == nullptr) {
        LOGE("delegate is null.");
        return nullptr;
    }
    return delegate->GetPage(pageId);
}

inline int32_t GetJsInt32Val(JSContext* ctx, JSValueConst value)
{
    int32_t val = 0;
    if (JS_IsNumber(value) && (JS_ToInt32(ctx, &val, value)) < 0) {
        val = 0;
    }
    return val;
}

void HandleJsAnimationContext(JSContext* ctx, int32_t pageId, int32_t nodeId, AnimationOperation operation)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto page = GetPageById(instance, pageId);
    if (page == nullptr) {
        LOGE("no page found for nodeId: %{public}d", nodeId);
        return;
    }
    auto task = AceType::MakeRefPtr<AnimationBridgeTaskOperation>(operation);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    if (page->CheckPageCreated()) {
        instance->GetDelegate()->TriggerPageUpdate(page->GetPageId());
    }
}

const JSCFunctionListEntry JS_ANIMATION_FUNCS[] = {
    JS_CGETSET_DEF(
        "playState", AnimationBridgeUtils::JsAnimationPlayStateGet, AnimationBridgeUtils::JsAnimationPlayStateSet),
    JS_CGETSET_DEF(
        "startTime", AnimationBridgeUtils::JsAnimationStartTimeGet, AnimationBridgeUtils::JsAnimationStartTimeSet),
    JS_CGETSET_DEF("pending", AnimationBridgeUtils::JsAnimationPendingGet, AnimationBridgeUtils::JsAnimationPendingSet),
};

void AddListenerForEventCallback(const WeakPtr<AnimationBridge>& bridgeWeak,
    const RefPtr<Animator>& animator, JSContext* ctx)
{
    animator->AddStopListener([ctx, bridgeWeak] {
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        auto jsTaskExecutor = instance->GetDelegate()->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, instance]() mutable {
            auto bridge = bridgeWeak.Upgrade();
            if (!bridge) {
                return;
            }
            LOGI("call animation onfinish event");
            instance->CallAnimationFinishJs(bridge->GetJsObject());
        });
    });
    animator->AddIdleListener([ctx, bridgeWeak] {
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        auto jsTaskExecutor = instance->GetDelegate()->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, instance]() mutable {
            auto bridge = bridgeWeak.Upgrade();
            if (!bridge) {
                return;
            }
            LOGI("call animation oncancel event");
            instance->CallAnimationCancelJs(bridge->GetJsObject());
        });
    });
    animator->AddRepeatListener([ctx, bridgeWeak] {
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        auto jsTaskExecutor = instance->GetDelegate()->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, instance]() mutable {
            auto bridge = bridgeWeak.Upgrade();
            if (!bridge) {
                return;
            }
            LOGI("call animation onrepeat event");
            instance->CallAnimationRepeatJs(bridge->GetJsObject());
        });
    });
}

} // namespace

AnimationBridge::AnimationBridge(JSContext* ctx, JSValue animationContext, NodeId nodeId)
    : ctx_(ctx), animationContext_(JS_DupValue(ctx, animationContext)), nodeId_(nodeId)
{
    if (ctx_ == nullptr) {
        return;
    }
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx_));
    if (instance == nullptr) {
        return;
    }
}

AnimationBridge::~AnimationBridge()
{
    // when last page exit, js engine will destruct first, so do not free JSObject again.
    if (ctx_ != nullptr) {
        JS_FreeValue(ctx_, animationContext_);
        ctx_ = nullptr;
    }
}

void AnimationBridge::OnJsEngineDestroy()
{
    LOGI("Destruct OnJsEngineDestroy, nodeId: %{public}d", nodeId_);
    if (ctx_ != nullptr) {
        JS_FreeValue(ctx_, animationContext_);
        ctx_ = nullptr;
    }
}

JSValue AnimationBridgeUtils::JsAnimationStartTimeGet(JSContext* ctx, JSValueConst value)
{
    QJSHandleScope handleScope(ctx);
    int32_t nodeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__nodeId"));
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("QjsEngineInstance is null. nodeId is: %{public}d", nodeId);
        return JS_NULL;
    }
    auto page = GetPageById(instance, pageId);
    if (page == nullptr) {
        LOGE("no page found for nodeId: %{public}d", nodeId);
        return JS_NULL;
    }
    auto domDocument = page->GetDomDocument();
    if (domDocument == nullptr) {
        LOGE("JsAnimationStartTimeGet failed, DomDocument is null.");
        return JS_NULL;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (domNode == nullptr) {
        LOGE("JsAnimationStartTimeGet failed, DomNode is null.");
        return JS_NULL;
    }
    auto tweenComponent = domNode->GetTweenComponent();
    if (tweenComponent) {
        auto option = tweenComponent->GetCustomTweenOption();
        auto startTime = option.GetDelay();
        JS_SetPropertyStr(ctx, value, "__startTime", JS_NewInt32(ctx, startTime));
    }
    return JS_GetPropertyStr(ctx, value, "__startTime");
}

JSValue AnimationBridgeUtils::JsAnimationStartTimeSet(JSContext* ctx, JSValueConst value, JSValueConst startTime)
{
    QJSHandleScope handleScope(ctx);
    int32_t startDelay = 0;
    JS_ToInt32(ctx, &startDelay, startTime);
    int32_t nodeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__nodeId"));
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("QjsEngineInstance is null. nodeID is: %{public}d", nodeId);
        return JS_NULL;
    }
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    auto page = GetPageById(instance, pageId);
    if (page == nullptr) {
        LOGE("no page found for nodeId: %{public}d", nodeId);
        return JS_NULL;
    }
    auto task = AceType::MakeRefPtr<AnimationBridgeTaskStartTime>(startDelay);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    return JS_NULL;
}

JSValue AnimationBridgeUtils::JsAnimationPendingGet(JSContext* ctx, JSValueConst value)
{
    QJSHandleScope handleScope(ctx);
    int32_t nodeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__nodeId"));
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("QjsEngineInstance is null. nodeID is: %{public}d", nodeId);
        return JS_NULL;
    }
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    auto page = GetPageById(instance, pageId);
    if (page == nullptr) {
        LOGE("no page found for nodeId: %{public}d", nodeId);
        return JS_NULL;
    }
    auto domDocument = page->GetDomDocument();
    if (domDocument == nullptr) {
        LOGE("JsAnimationPendingGet failed, DomDocument is null.");
        return JS_NULL;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (domNode == nullptr) {
        LOGE("JsAnimationPendingGet failed, DomNode is null.");
        return JS_NULL;
    }
    auto tweenComponent = domNode->GetTweenComponent();
    RefPtr<Animator> animator;
    if (tweenComponent) {
        animator = tweenComponent->GetAnimator();
    }
    if (animator) {
        JS_SetPropertyStr(ctx, value, "__pending", JS_NewBool(ctx, animator->IsPending()));
    }
    return JS_GetPropertyStr(ctx, value, "__pending");
}

JSValue AnimationBridgeUtils::JsAnimationPendingSet(JSContext* ctx, JSValueConst value, JSValueConst pending)
{
    return JS_NULL;
}

JSValue AnimationBridgeUtils::JsAnimationPlayStateGet(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__playState");
}

JSValue AnimationBridgeUtils::JsAnimationPlayStateSet(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString playState(ctx, proto);
    AnimationOperation operation = AnimationOperation::NONE;
    if (std::strcmp(playState.get(), DOM_ANIMATION_PLAY_STATE_IDLE) == 0) {
        operation = AnimationOperation::CANCEL;
    } else if (strlen(playState.get()) >= strlen(DOM_ANIMATION_PLAY_STATE_RUNNING) &&
               std::strcmp(playState.get(), DOM_ANIMATION_PLAY_STATE_RUNNING) == 0) {
        operation = AnimationOperation::PLAY;
    } else if (strlen(playState.get()) >= strlen(DOM_ANIMATION_PLAY_STATE_PAUSED) &&
               std::strcmp(playState.get(), DOM_ANIMATION_PLAY_STATE_PAUSED) == 0) {
        operation = AnimationOperation::PAUSE;
    } else if (strlen(playState.get()) >= strlen(DOM_ANIMATION_PLAY_STATE_FINISHED) &&
               std::strcmp(playState.get(), DOM_ANIMATION_PLAY_STATE_FINISHED) == 0) {
        operation = AnimationOperation::FINISH;
    } else {
        operation = AnimationOperation::NONE;
    }

    QJSHandleScope handleScope(ctx);
    int32_t nodeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__nodeId"));
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    auto page = GetPageById(instance, pageId);
    if (page == nullptr) {
        LOGE("no page found for nodeId: %{public}d", nodeId);
        return JS_NULL;
    }
    auto task = AceType::MakeRefPtr<AnimationBridgeTaskOperation>(operation);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    return JS_NULL;
}

void AnimationBridge::JsCreateAnimation(const RefPtr<JsAcePage>& page, const std::string& param)
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
    if (page == nullptr) {
        LOGE("JsCreateAnimation failed, Page is null.");
        return;
    }
    auto domDocument = page->GetDomDocument();
    if (domDocument == nullptr) {
        LOGE("JsCreateAnimation failed, DomDocument is null.");
        return;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId_);
    if (domNode == nullptr) {
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
    AddListenerForEventCallback(AceType::WeakClaim(this), animator, ctx_);
    domNode->GenerateComponentNode();
    page->PushDirtyNode(nodeId_);
}

void AnimationBridge::SetPlayStateCallbacksWithListenerId(RefPtr<Animator>& animator)
{
    if (animator == nullptr) {
        LOGE("Set PlayState callbacks failed. animator is null.");
        return;
    }
    WeakPtr<AnimationBridge> bridgeWeak = AceType::WeakClaim(this);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx_));
    animator->RemoveStopListener(finishListenerId_);
    finishListenerId_ = animator->AddStopListener([ctx = ctx_, bridgeWeak, instance] {
        auto jsTaskExecutor = instance->GetDelegate()->GetAnimationJsTask();
        jsTaskExecutor.PostTask([ctx, bridgeWeak, instance]() mutable {
            auto bridge = bridgeWeak.Upgrade();
            if (!bridge) {
                return;
            }
            JS_SetPropertyStr(ctx, bridge->GetJsObject(), "__playState",
                JS_NewString(ctx, DOM_ANIMATION_PLAY_STATE_FINISHED));
        });
    });
    animator->RemoveIdleListener(idleListenerId_);
    idleListenerId_ = animator->AddIdleListener([ctx = ctx_, bridgeWeak, instance] {
        auto jsTaskExecutor = instance->GetDelegate()->GetAnimationJsTask();
        jsTaskExecutor.PostTask([ctx, bridgeWeak, instance]() mutable {
            auto bridge = bridgeWeak.Upgrade();
            if (!bridge) {
                return;
            }
            JS_SetPropertyStr(
                ctx, bridge->GetJsObject(), "__playState", JS_NewString(ctx, DOM_ANIMATION_PLAY_STATE_IDLE));
        });
    });
}

void AnimationBridge::SetPlayStateCallbacks(RefPtr<Animator>& animator)
{
    if (animator == nullptr) {
        LOGE("Set PlayState callbacks failed. animator is null.");
        return;
    }
    WeakPtr<AnimationBridge> bridgeWeak = AceType::WeakClaim(this);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx_));
    SetPlayStateCallbacksWithListenerId(animator);
    animator->ClearPauseListeners();
    animator->AddPauseListener([ctx = ctx_, bridgeWeak, instance] {
        auto jsTaskExecutor = instance->GetDelegate()->GetAnimationJsTask();
        jsTaskExecutor.PostTask([ctx, bridgeWeak, instance]() mutable {
            auto bridge = bridgeWeak.Upgrade();
            if (!bridge) {
                return;
            }
            JS_SetPropertyStr(ctx, bridge->GetJsObject(), "__playState",
                JS_NewString(ctx, DOM_ANIMATION_PLAY_STATE_PAUSED));
        });
    });
    animator->ClearStartListeners();
    animator->AddStartListener([ctx = ctx_, bridgeWeak, instance] {
        auto jsTaskExecutor = instance->GetDelegate()->GetAnimationJsTask();
        jsTaskExecutor.PostTask([ctx, bridgeWeak, instance]() mutable {
            auto bridge = bridgeWeak.Upgrade();
            if (!bridge) {
                return;
            }
            instance->CallAnimationStartJs(bridge->GetJsObject());
            JS_SetPropertyStr(ctx, bridge->GetJsObject(), "__playState",
                JS_NewString(ctx, DOM_ANIMATION_PLAY_STATE_RUNNING));
        });
    });
    animator->ClearResumeListeners();
    animator->AddResumeListener([ctx = ctx_, bridgeWeak, instance] {
        auto jsTaskExecutor = instance->GetDelegate()->GetAnimationJsTask();
        jsTaskExecutor.PostTask([ctx, bridgeWeak, instance]() mutable {
            auto bridge = bridgeWeak.Upgrade();
            if (!bridge) {
                return;
            }
            JS_SetPropertyStr(ctx, bridge->GetJsObject(), "__playState",
                JS_NewString(ctx, DOM_ANIMATION_PLAY_STATE_RUNNING));
        });
    });
}

JSValue AnimationBridgeUtils::JsAnimationPlay(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }

    QJSHandleScope handleScope(ctx);
    int32_t nodeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__nodeId"));
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    HandleJsAnimationContext(ctx, pageId, nodeId, AnimationOperation::PLAY);
    return JS_NULL;
}

JSValue AnimationBridgeUtils::JsAnimationFinish(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }

    QJSHandleScope handleScope(ctx);
    int32_t nodeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__nodeId"));
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    HandleJsAnimationContext(ctx, pageId, nodeId, AnimationOperation::FINISH);
    return JS_NULL;
}

JSValue AnimationBridgeUtils::JsAnimationPause(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }

    QJSHandleScope handleScope(ctx);
    int32_t nodeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__nodeId"));
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    HandleJsAnimationContext(ctx, pageId, nodeId, AnimationOperation::PAUSE);
    return JS_NULL;
}

JSValue AnimationBridgeUtils::JsAnimationCancel(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }

    QJSHandleScope handleScope(ctx);
    int32_t nodeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__nodeId"));
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    HandleJsAnimationContext(ctx, pageId, nodeId, AnimationOperation::CANCEL);
    return JS_NULL;
}

JSValue AnimationBridgeUtils::JsAnimationReverse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }

    QJSHandleScope handleScope(ctx);
    int32_t nodeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__nodeId"));
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    HandleJsAnimationContext(ctx, pageId, nodeId, AnimationOperation::REVERSE);
    return JS_NULL;
}

JSValue AnimationBridgeUtils::CreateAnimationContext(JSContext* ctx, int32_t pageId, NodeId nodeId)
{
    auto animationContext = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, animationContext, "play", JS_NewCFunction(ctx, JsAnimationPlay, "play", 0));
    JS_SetPropertyStr(ctx, animationContext, "finish", JS_NewCFunction(ctx, JsAnimationFinish, "finish", 0));
    JS_SetPropertyStr(ctx, animationContext, "pause", JS_NewCFunction(ctx, JsAnimationPause, "pause", 0));
    JS_SetPropertyStr(ctx, animationContext, "cancel", JS_NewCFunction(ctx, JsAnimationCancel, "cancel", 0));
    JS_SetPropertyStr(ctx, animationContext, "reverse", JS_NewCFunction(ctx, JsAnimationReverse, "reverse", 0));
    JS_SetPropertyStr(ctx, animationContext, "__pageId", JS_NewInt32(ctx, pageId));
    JS_SetPropertyStr(ctx, animationContext, "__nodeId", JS_NewInt32(ctx, nodeId));
    JS_SetPropertyStr(
        ctx, animationContext, "__playState", JS_NewString(ctx, DOM_ANIMATION_PLAY_STATE_IDLE));
    JS_SetPropertyStr(ctx, animationContext, "finished", JS_NewBool(ctx, 0));
    JS_SetPropertyFunctionList(ctx, animationContext, JS_ANIMATION_FUNCS, countof(JS_ANIMATION_FUNCS));
    return animationContext;
}

AnimationBridgeTaskCreate::AnimationBridgeTaskCreate(const RefPtr<AnimationBridge>& animationBridge, std::string param)
    : animationBridge_(animationBridge), param_(std::move(param))
{}

void AnimationBridgeTaskCreate::AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId)
{
    if (page == nullptr) {
        LOGE("the page is nullptr");
        return;
    }
    page->RemoveAnimationBridge(nodeId);
    animationBridge_->JsCreateAnimation(page, param_);
    page->AddAnimationBridge(nodeId, animationBridge_);
}

void AnimationBridgeTaskOperation::AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId)
{
    if (page == nullptr) {
        LOGE("the page is nullptr");
        return;
    }
    auto animationBridge = AceType::DynamicCast<AnimationBridge>(page->GetAnimationBridge(nodeId));
    if (!animationBridge) {
        LOGE("no animation bridge found for nodeId: %{public}d", nodeId);
        return;
    }
    auto domDocument = page->GetDomDocument();
    if (domDocument == nullptr) {
        LOGE("Animation operation failed, DomDocument is null.");
        return;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (domNode == nullptr) {
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

void AnimationBridgeTaskStartTime::AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId)
{
    if (page == nullptr) {
        LOGE("Get page is error");
        return;
    }
    auto domDocument = page->GetDomDocument();
    if (domDocument == nullptr) {
        LOGE("AnimationBridgeTaskStartTime failed, DomDocument is null.");
        return;
    }
    auto domNode = domDocument->GetDOMNodeById(nodeId);
    if (domNode == nullptr) {
        LOGE("AnimationBridgeTaskStartTime failed, DomNode is null.");
        return;
    }
    auto tweenComponent = domNode->GetTweenComponent();
    if (tweenComponent) {
        auto option = tweenComponent->GetCustomTweenOption();
        option.SetDelay(startTime_);
        tweenComponent->SetCustomTweenOption(option);
    }
}

} // namespace OHOS::Ace::Framework
