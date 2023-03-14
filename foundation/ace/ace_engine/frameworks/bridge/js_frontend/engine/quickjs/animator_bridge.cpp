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

#include "frameworks/bridge/js_frontend/engine/quickjs/animator_bridge.h"

#include <utility>

#include "base/log/log.h"
#include "base/utils/string_utils.h"
#include "core/common/container.h"
#include "core/common/thread_checker.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_engine.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_group_js_bridge.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

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
    if (!delegate) {
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

void HandleJsAnimatorContext(JSContext* ctx, JSValueConst value, AnimatorOperation operation)
{
    QJSHandleScope handleScope(ctx);
    int32_t bridgeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__bridgeId"));
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto page = GetPageById(instance, pageId);
    if (!page) {
        LOGE("no page found for bridgeId: %{public}d", bridgeId);
        return;
    }
    auto task = AceType::MakeRefPtr<AnimatorTaskOperation>(operation);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimator>(bridgeId, task));
    if (page->CheckPageCreated()) {
        auto delegate = instance->GetDelegate();
        if (!delegate) {
            LOGE("delegate is nullptr");
            return;
        }
        delegate->TriggerPageUpdate(page->GetPageId());
    }
}

void AddListenerForEventCallback(const WeakPtr<AnimatorBridge>& bridgeWeak,
    const RefPtr<Animator>& animator, JSContext* ctx)
{
    animator->AddStartListener([ctx, bridgeWeak] {
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        auto jsTaskExecutor = instance->GetDelegate()->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, instance]() mutable {
            auto bridge = bridgeWeak.Upgrade();
            if (!bridge) {
                return;
            }
            LOGI("call animation onstart event");
            instance->CallAnimationStartJs(bridge->GetJsObject());
        });
    });
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

void AddFrameListener(const WeakPtr<AnimatorBridge>& bridgeWeak, const RefPtr<KeyframeAnimation<double>>& animation,
    JSContext* ctx)
{
    animation->AddListener([ctx, bridgeWeak](double value) {
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        auto jsTaskExecutor = instance->GetDelegate()->GetAnimationJsTask();
        jsTaskExecutor.PostTask([bridgeWeak, instance, value]() mutable {
            auto bridge = bridgeWeak.Upgrade();
            if (!bridge) {
                return;
            }
            std::string arg = std::to_string(value);
            instance->CallAnimationFrameJs(bridge->GetJsObject(), arg.c_str());
        });
    });
}
}

JSValue AnimatorBridgeUtils::CreateAnimatorContext(JSContext* ctx, int32_t pageId, int32_t bridgeId)
{
    auto animatorContext = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, animatorContext, "play", JS_NewCFunction(ctx, JsAnimatorPlay, "play", 0));
    JS_SetPropertyStr(ctx, animatorContext, "finish", JS_NewCFunction(ctx, JsAnimatorFinish, "finish", 0));
    JS_SetPropertyStr(ctx, animatorContext, "pause", JS_NewCFunction(ctx, JsAnimatorPause, "pause", 0));
    JS_SetPropertyStr(ctx, animatorContext, "cancel", JS_NewCFunction(ctx, JsAnimatorCancel, "cancel", 0));
    JS_SetPropertyStr(ctx, animatorContext, "reverse", JS_NewCFunction(ctx, JsAnimatorReverse, "reverse", 0));
    JS_SetPropertyStr(ctx, animatorContext, "update", JS_NewCFunction(ctx, JsAnimatorUpdate, "updateOption", 1));
    JS_SetPropertyStr(ctx, animatorContext, "__pageId", JS_NewInt32(ctx, pageId));
    JS_SetPropertyStr(ctx, animatorContext, "__bridgeId", JS_NewInt32(ctx, bridgeId));
    return animatorContext;
}

JSValue AnimatorBridgeUtils::JsAnimatorPlay(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    HandleJsAnimatorContext(ctx, value, AnimatorOperation::PLAY);
    return JS_NULL;
}

JSValue AnimatorBridgeUtils::JsAnimatorFinish(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    HandleJsAnimatorContext(ctx, value, AnimatorOperation::FINISH);
    return JS_NULL;
}

JSValue AnimatorBridgeUtils::JsAnimatorPause(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    HandleJsAnimatorContext(ctx, value, AnimatorOperation::PAUSE);
    return JS_NULL;
}

JSValue AnimatorBridgeUtils::JsAnimatorCancel(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    HandleJsAnimatorContext(ctx, value, AnimatorOperation::CANCEL);
    return JS_NULL;
}

JSValue AnimatorBridgeUtils::JsAnimatorReverse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    HandleJsAnimatorContext(ctx, value, AnimatorOperation::REVERSE);
    return JS_NULL;
}

JSValue AnimatorBridgeUtils::JsAnimatorUpdate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 1)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, argv[0], &pTab, &len)) {
        return JS_NULL;
    }
    std::unordered_map<std::string, std::string> params;
    for (uint32_t i = 0; i < len; ++i) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue valItem = JS_GetProperty(ctx, argv[0], pTab[i].atom);
        if (JS_IsString(valItem) || JS_IsNumber(valItem)) {
            ScopedString styleVal(ctx, valItem);
            const char* valStr = styleVal.get();
            params[key] = valStr;
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
    int32_t pageId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    int32_t bridgeId = GetJsInt32Val(ctx, QJSUtils::GetPropertyStr(ctx, value, "__bridgeId"));
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto page = GetPageById(instance, pageId);
    if (!page) {
        LOGE("no page found for bridgeId: %{public}d", bridgeId);
        return JS_NULL;
    }
    auto task = AceType::MakeRefPtr<AnimatorTaskUpdate>(params);
    page->PushCommand(AceType::MakeRefPtr<JsCommandAnimator>(bridgeId, task));
    return JS_NULL;
}

int32_t AnimatorBridgeUtils::JsCreateBridgeId()
{
    static int32_t bridgeId = 0;
    return bridgeId++;
}

AnimatorBridge::AnimatorBridge(JSContext* ctx, JSValue animationContext)
    : ctx_(ctx), animationContext_(JS_DupValue(ctx, animationContext))
{
    if (ctx_ == nullptr) {
        return;
    }
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx_));
    if (instance == nullptr) {
        return;
    }
}

AnimatorBridge::~AnimatorBridge()
{
    // when last page exit, js engine will destruct first, so do not free JSObject again.
    CHECK_RUN_ON(JS);
    if (ctx_ != nullptr) {
        JS_FreeValue(ctx_, animationContext_);
        ctx_ = nullptr;
    }
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

void AnimatorBridge::OnJsEngineDestroy()
{
    CHECK_RUN_ON(JS);
    if (ctx_ != nullptr) {
        JS_FreeValue(ctx_, animationContext_);
        ctx_ = nullptr;
    }
}

void AnimatorBridge::JsCreateAnimation(const std::string& param)
{
    int32_t iterations = 1;
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
    auto keyframeAnimation = CreateDoubleAnimation(animationDoubleParams, curve);
    if (!ctx_ || !keyframeAnimation) {
        LOGE("animation create failed");
        return;
    }
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
    AddFrameListener(AceType::WeakClaim(this), keyframeAnimation, ctx_);
    if (!animator_) {
        animator_ = AceType::MakeRefPtr<Animator>();
    }
    auto iterDirection = animationStringParams.find(DOM_ANIMATION_DIRECTION_API);
    if (iterDirection != animationStringParams.end()) {
        animator_->SetAnimationDirection(StringToAnimationDirection(iterDirection->second));
    }
    if (!animator_->IsStopped()) {
        animator_->Stop();
    }
    animator_->ClearInterpolators();
    animator_->SetDuration(duration);
    animator_->SetIteration(iterations);
    animator_->SetStartDelay(delay);
    animator_->SetFillMode(StringToFillMode(fillString));
    animator_->AddInterpolator(keyframeAnimation);
    AddListenerForEventCallback(AceType::WeakClaim(this), animator_, ctx_);
}

RefPtr<KeyframeAnimation<double>> AnimatorBridge::CreateDoubleAnimation(
    const std::unordered_map<std::string, double>& animationParams, const RefPtr<Curve>& curve)
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
    return keyframeAnimation;
}

AnimatorTaskCreate::AnimatorTaskCreate(const RefPtr<AnimatorBridge>& bridge, const std::string& param)
    : bridge_(bridge), param_(std::move(param))
{}

void AnimatorTaskCreate::AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId)
{
    if (!bridge_ || !page) {
        LOGE("Create Animation Bridge failed. bridge is null.");
        return;
    }
    page->RemoveAnimatorBridge(bridgeId);
    bridge_->JsCreateAnimation(param_);
    page->AddAnimatorBridge(bridgeId, bridge_);
}

void AnimatorTaskOperation::AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId)
{
    auto animatorBridge = AceType::DynamicCast<AnimatorBridge>(page->GetAnimatorBridge(bridgeId));
    if (!animatorBridge) {
        LOGE("no animation bridge found for bridgeId: %{public}d", bridgeId);
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

void AnimatorTaskUpdate::AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId)
{
    auto animatorBridge = AceType::DynamicCast<AnimatorBridge>(page->GetAnimatorBridge(bridgeId));
    if (!animatorBridge) {
        LOGE("no animation bridge found for bridgeId: %{public}d", bridgeId);
        return;
    }
    RefPtr<Animator> animator = animatorBridge->JsGetAnimator();
    if (!animator) {
        LOGE("animator is null");
        return;
    }
    JSContext* ctx = animatorBridge->GetContext();
    if (!ctx) {
        LOGE("ctx is null");
        return;
    }
    animator->ClearInterpolators();
    UpdateAnimator(animator, animatorBridge, ctx, params_);
}

void AnimatorTaskUpdate::UpdateAnimator(const RefPtr<Animator>& animator, const RefPtr<AnimatorBridge>& bridge,
    JSContext* ctx, const std::unordered_map<std::string, std::string>& params)
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
    if (!keyframeAnimation) {
        LOGE("animation create failed");
        return;
    }
    AddFrameListener(AceType::WeakClaim(RawPtr(bridge)), keyframeAnimation, ctx);
    animator->SetDuration(duration);
    animator->SetIteration(iterations);
    animator->SetStartDelay(delay);
    animator->SetFillMode(StringToFillMode(fillString));
    animator->AddInterpolator(keyframeAnimation);
}

RefPtr<KeyframeAnimation<double>> AnimatorTaskUpdate::CreateDoubleAnimation(double begin, double end,
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