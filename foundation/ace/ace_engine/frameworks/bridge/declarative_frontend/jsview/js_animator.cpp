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

#include "bridge/declarative_frontend/jsview/js_animator.h"

#include "bridge/common/utils/utils.h"
#include "bridge/declarative_frontend/engine/functions/js_animator_function.h"
#include "core/common/container.h"

#ifdef USE_V8_ENGINE
#include "bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"
#elif USE_QUICKJS_ENGINE
#include "bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine_instance.h"
#elif USE_ARK_ENGINE
#include "bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"
#endif

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t FRICITION_MOTION_LENGTH = 3;
constexpr int32_t SPRING_MOTION_LENGTH = 4;
constexpr int32_t SCROLL_MOTION_LENGTH = 5;

RefPtr<JsAcePage> GetCurrentPage()
{
#ifdef USE_V8_ENGINE
    auto isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    auto page = V8DeclarativeEngineInstance::GetStagingPage(isolate);
    return page;

#elif USE_QUICKJS_ENGINE
    auto context = QJSContext::Current();
    auto page = QJSDeclarativeEngineInstance::GetRunningPage(context);
    return page;
#elif USE_ARK_ENGINE
    auto page = JsiDeclarativeEngineInstance::GetStagingPage(Container::CurrentId());
    return page;
#endif
    return nullptr;
}

RefPtr<AnimatorInfo> GetAnimatorInfo(const std::string& animatorId, const RefPtr<JsAcePage>& page)
{
    if (!page) {
        LOGE("page is nullptr");
        return nullptr;
    }
    auto animatorInfo = page->GetAnimatorInfo(animatorId);
    return animatorInfo;
}

RefPtr<PipelineContext> GetCurrentContext()
{
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return nullptr;
    }
    auto pipelineContext = container->GetPipelineContext();
    if (!pipelineContext) {
        LOGE("pipelineContext is null!");
        return nullptr;
    }
    return pipelineContext;
}

void AddEventListener(const EventMarker& event, EventOperation operation, const std::string& animatorId,
    const JSCallbackInfo& info)
{
    auto page = GetCurrentPage();
    auto animatorInfo = GetAnimatorInfo(animatorId, page);
    if (!animatorInfo) {
        LOGE("animatorInfo is nullptr");
        return;
    }
    auto animator = animatorInfo->GetAnimator();
    if (!animator) {
        LOGE("animator is nullptr");
        return;
    }
    auto pipelineContext = GetCurrentContext();
    if (!pipelineContext) {
        LOGE("context is nullptr");
        return;
    }
    WeakPtr<PipelineContext> context = WeakPtr<PipelineContext>(pipelineContext);
    switch (operation) {
        case EventOperation::START:
            animator->ClearStartListeners();
            if (!event.IsEmpty()) {
                animator->AddStartListener([event, weakContext = context] {
                    AceAsyncEvent<void()>::Create(event, weakContext)();
                });
            }
            break;
        case EventOperation::PAUSE:
            animator->ClearPauseListeners();
            if (!event.IsEmpty()) {
                animator->AddPauseListener([event, weakContext = context] {
                    AceAsyncEvent<void()>::Create(event, weakContext)();
                });
            }
            break;
        case EventOperation::REPEAT:
            animator->ClearRepeatListeners();
            if (!event.IsEmpty()) {
                animator->AddRepeatListener([event, weakContext = context] {
                    AceAsyncEvent<void()>::Create(event, weakContext)();
                });
            }
            break;
        case EventOperation::CANCEL:
            animator->ClearIdleListeners();
            if (!event.IsEmpty()) {
                animator->AddIdleListener([event, weakContext = context] {
                    AceAsyncEvent<void()>::Create(event, weakContext)();
                });
            }
            break;
        case EventOperation::FINISH:
            animator->ClearStopListeners();
            if (!event.IsEmpty()) {
                animator->AddStopListener([event, weakContext = context] {
                    AceAsyncEvent<void()>::Create(event, weakContext)();
                });
            }
            break;
        case EventOperation::NONE:
        default:
            break;
    }
}

void AddFrameListener(const RefPtr<AnimatorInfo>& animatorInfo, const RefPtr<KeyframeAnimation<double>>& animation)
{
    if (!animatorInfo || !animation) {
        LOGE("animatorInfo or animation is nullptr");
        return;
    }
    auto frameEvent = animatorInfo->GetFrameEvent();
    if (frameEvent) {
        animation->AddListener([frameEvent](const float& progress) {
            frameEvent(progress);
        });
    }
}

void HandleAnimatorInfo(const RefPtr<AnimatorInfo>& animatorInfo, const RefPtr<Animator>& animator)
{
    if (!animatorInfo || !animator) {
        LOGE("animatorInfo or animator is nullptr");
        return;
    }
    int32_t duration = animatorInfo->GetDuration();
    int32_t delay = animatorInfo->GetDelay();
    FillMode fillMode = animatorInfo->GetFillMode();
    int32_t iteration = animatorInfo->GetIteration();
    AnimationDirection playMode = animatorInfo->GetPlayMode();
    animator->SetDuration(duration);
    animator->SetStartDelay(delay);
    animator->SetFillMode(fillMode);
    animator->SetIteration(iteration);
    animator->SetAnimationDirection(playMode);
}

bool CreateAnimation(const RefPtr<AnimatorInfo>& animatorInfo, const RefPtr<Animator>& animator,
    AnimationStatus operation)
{
    if (!animatorInfo || !animator) {
        LOGE("animatorInfo or animator is nullptr");
        return false;
    }
    auto motion = animatorInfo->GetAnimatorMotion();
    if (motion) {
        auto frameEvent = animatorInfo->GetFrameEvent();
        if (frameEvent) {
            motion->AddListener([frameEvent](const float& progress) {
                frameEvent(progress);
            });
        }
        animator->ClearPauseListeners();
        animator->ClearRepeatListeners();
        animator->ClearIdleListeners();
        if (operation == AnimationStatus::RUNNING && animator->GetStatus() != Animator::Status::RUNNING) {
            animator->PlayMotion(motion);
        } else if (operation == AnimationStatus::STOPPED) {
            animator->Finish();
        } else {
            LOGD("animator don't need to do motion");
        }
        return false;
    } else {
        animator->ClearInterpolators();
        auto keyframeBegin = AceType::MakeRefPtr<Keyframe<double>>(0.0, 0.0);
        auto keyframeEnd = AceType::MakeRefPtr<Keyframe<double>>(1.0, 1.0);
        auto keyframeAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
        auto curve = animatorInfo->GetCurve();
        if (curve) {
            keyframeAnimation->SetCurve(curve);
        }
        keyframeAnimation->AddKeyframe(keyframeBegin);
        keyframeAnimation->AddKeyframe(keyframeEnd);
        AddFrameListener(animatorInfo, keyframeAnimation);
        animator->AddInterpolator(keyframeAnimation);
        return true;
    }
}

} // namespace

std::string JSAnimator::animatorId_;

void JSAnimator::JSBind(BindingTarget globalObj)
{
    JSClass<JSAnimator>::Declare("Animator");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSAnimator>::StaticMethod("create", &JSAnimator::Create, opt);
    JSClass<JSAnimator>::StaticMethod("state", &JSAnimator::SetState, opt);
    JSClass<JSAnimator>::StaticMethod("duration", &JSAnimator::SetDuration, opt);
    JSClass<JSAnimator>::StaticMethod("curve", &JSAnimator::SetCurve, opt);
    JSClass<JSAnimator>::StaticMethod("delay", &JSAnimator::SetDelay, opt);
    JSClass<JSAnimator>::StaticMethod("fillMode", &JSAnimator::SetFillMode, opt);
    JSClass<JSAnimator>::StaticMethod("iterations", &JSAnimator::SetIteration, opt);
    JSClass<JSAnimator>::StaticMethod("playMode", &JSAnimator::SetPlayMode, opt);
    JSClass<JSAnimator>::StaticMethod("motion", &JSAnimator::SetMotion, opt);

    JSClass<JSAnimator>::StaticMethod("onStart", &JSAnimator::OnStart, opt);
    JSClass<JSAnimator>::StaticMethod("onPause", &JSAnimator::OnPause, opt);
    JSClass<JSAnimator>::StaticMethod("onRepeat", &JSAnimator::OnRepeat, opt);
    JSClass<JSAnimator>::StaticMethod("onCancel", &JSAnimator::OnCancel, opt);
    JSClass<JSAnimator>::StaticMethod("onFinish", &JSAnimator::OnFinish, opt);
    JSClass<JSAnimator>::StaticMethod("onFrame", &JSAnimator::OnFrame, opt);

    JSClass<JSAnimator>::Bind<>(globalObj);

    JSClass<JSSpringProp>::Declare("SpringProp");
    JSClass<JSSpringProp>::Bind(globalObj, JSSpringProp::ConstructorCallback, JSSpringProp::DestructorCallback);

    JSClass<JSMotion>::Declare("SpringMotion");
    JSClass<JSMotion>::Bind(globalObj, JSMotion::ConstructorCallback, JSMotion::DestructorCallback);

    JSClass<JSMotion>::Declare("FrictionMotion");
    JSClass<JSMotion>::Bind(globalObj, JSMotion::ConstructorCallback, JSMotion::DestructorCallback);

    JSClass<JSMotion>::Declare("ScrollMotion");
    JSClass<JSMotion>::Bind(globalObj, JSMotion::ConstructorCallback, JSMotion::DestructorCallback);
}

void JSAnimator::Create(const JSCallbackInfo& info)
{
    if (info.Length() != 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    if (!info[0]->IsString()) {
        LOGE("arg is not a string.");
        return;
    }
    animatorId_ = info[0]->ToString();
    auto page = GetCurrentPage();
    if (!page) {
        LOGE("page is nullptr");
        return;
    }
    auto animatorInfo = GetAnimatorInfo(animatorId_, page);
    if (!animatorInfo) {
        animatorInfo = AceType::MakeRefPtr<AnimatorInfo>();
        auto animator = AceType::MakeRefPtr<Animator>();
        animatorInfo->SetAnimator(animator);
        page->AddAnimatorInfo(animatorId_, animatorInfo);
    }
}

void JSAnimator::SetState(int32_t state)
{
    auto page = GetCurrentPage();
    auto animatorInfo = GetAnimatorInfo(animatorId_, page);
    if (!animatorInfo) {
        LOGE("animatorInfo is nullptr");
        return;
    }
    auto animator = animatorInfo->GetAnimator();
    if (!animator) {
        LOGE("animator is nullptr");
        return;
    }
    auto operation = static_cast<AnimationStatus>(state);
    HandleAnimatorInfo(animatorInfo, animator);
    if (!CreateAnimation(animatorInfo, animator, operation)) {
        return;
    }
    switch (operation) {
        case AnimationStatus::RUNNING:
            animator->Play();
            break;
        case AnimationStatus::PAUSED:
            animator->Pause();
            break;
        case AnimationStatus::STOPPED:
            animator->Finish();
            break;
        case AnimationStatus::INITIAL:
            animator->Cancel();
            break;
        default:
            break;
    }
}

void JSAnimator::SetDuration(int32_t duration)
{
    auto page = GetCurrentPage();
    auto animatorInfo = GetAnimatorInfo(animatorId_, page);
    if (!animatorInfo) {
        LOGE("animatorInfo is nullptr");
        return;
    }
    animatorInfo->SetDuration(duration);
}

void JSAnimator::SetCurve(const JSCallbackInfo& info)
{
    if (info.Length() != 1) {
        LOGE("The arg is wrong, it is supposed to have 1 arguments");
        return;
    }

    if (!info[0]->IsString()) {
        LOGE("arg is not a string.");
        return;
    }
    auto value = info[0]->ToString();
    auto page = GetCurrentPage();
    auto animatorInfo = GetAnimatorInfo(animatorId_, page);
    if (!animatorInfo) {
        LOGE("animatorInfo is nullptr");
        return;
    }
    auto curve = CreateCurve(value);
    animatorInfo->SetCurve(curve);
}

void JSAnimator::SetDelay(int32_t delay)
{
    auto page = GetCurrentPage();
    auto animatorInfo = GetAnimatorInfo(animatorId_, page);
    if (!animatorInfo) {
        LOGE("animatorInfo is nullptr");
        return;
    }
    animatorInfo->SetDelay(delay);
}

void JSAnimator::SetFillMode(int32_t fillMode)
{
    auto page = GetCurrentPage();
    auto animatorInfo = GetAnimatorInfo(animatorId_, page);
    if (!animatorInfo) {
        LOGE("animatorInfo is nullptr");
        return;
    }
    animatorInfo->SetFillMode(static_cast<FillMode>(fillMode));
}

void JSAnimator::SetIteration(int32_t iteration)
{
    auto page = GetCurrentPage();
    auto animatorInfo = GetAnimatorInfo(animatorId_, page);
    if (!animatorInfo) {
        LOGE("animatorInfo is nullptr");
        return;
    }
    animatorInfo->SetIteration(iteration);
}

void JSAnimator::SetPlayMode(int32_t playMode)
{
    auto page = GetCurrentPage();
    auto animatorInfo = GetAnimatorInfo(animatorId_, page);
    if (!animatorInfo) {
        LOGE("animatorInfo is nullptr");
        return;
    }
    animatorInfo->SetPlayMode(static_cast<AnimationDirection>(playMode));
}

void JSAnimator::SetMotion(const JSCallbackInfo& info)
{
    if (info.Length() != 1 || !info[0]->IsObject()) {
        LOGE("The arg is wrong, it is supposed to have 1 arguments");
        return;
    }
    JSMotion* rawMotion = JSRef<JSObject>::Cast(info[0])->Unwrap<JSMotion>();
    if (!rawMotion) {
        LOGE("motion is nullptr");
        return;
    }

    RefPtr<Motion> motion = rawMotion->GetMotion();
    auto page = GetCurrentPage();
    auto animatorInfo = GetAnimatorInfo(animatorId_, page);
    if (!animatorInfo) {
        LOGE("animatorInfo is nullptr");
        return;
    }
    animatorInfo->SetAnimatorMotion(motion);
}

void JSAnimator::OnStart(const JSCallbackInfo& info)
{
    auto startEvent = GetEventMarker(info);
    AddEventListener(startEvent, EventOperation::START, animatorId_, info);
}

void JSAnimator::OnPause(const JSCallbackInfo& info)
{
    auto pauseEvent = GetEventMarker(info);
    AddEventListener(pauseEvent, EventOperation::PAUSE, animatorId_, info);
}

void JSAnimator::OnRepeat(const JSCallbackInfo& info)
{
    auto repeatEvent = GetEventMarker(info);
    AddEventListener(repeatEvent, EventOperation::REPEAT, animatorId_, info);
}

void JSAnimator::OnCancel(const JSCallbackInfo& info)
{
    auto cancelEvent = GetEventMarker(info);
    AddEventListener(cancelEvent, EventOperation::CANCEL, animatorId_, info);
}

void JSAnimator::OnFinish(const JSCallbackInfo& info)
{
    auto finishEvent = GetEventMarker(info);
    AddEventListener(finishEvent, EventOperation::FINISH, animatorId_, info);
}

void JSAnimator::OnFrame(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsFunction()) {
        LOGE("OnFrame info.Length < 0 or info[0] is not function");
        return;
    }
    RefPtr<JsAnimatorFunction> function = AceType::MakeRefPtr<JsAnimatorFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto OnFrameEvent = [execCtx = info.GetExecutionContext(), func = std::move(function)](
        const float& progress) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("Animator.onFrame");
        func->Execute(progress);
    };
    auto page = GetCurrentPage();
    auto animatorInfo = GetAnimatorInfo(animatorId_, page);
    if (!animatorInfo) {
        LOGE("animatorInfo is nullptr");
        return;
    }
    animatorInfo->SetFrameEvent(OnFrameEvent);
}

EventMarker JSAnimator::GetEventMarker(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("info[0] is not a function.");
        return EventMarker();
    }
    RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
    auto eventMarker = EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)]() {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("Animator.onClick");
        func->Execute();
    });
    return eventMarker;
}

void JSSpringProp::ConstructorCallback(const JSCallbackInfo& info)
{
    if (info.Length() != 3 || !info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber()) {
        LOGE("The arg is wrong, it is supposed to have 3 arguments of number");
        return;
    }
    auto obj = AceType::MakeRefPtr<JSSpringProp>();
    double mass = info[0]->ToNumber<double>();
    double stiffness = info[1]->ToNumber<double>();
    double damping = info[2]->ToNumber<double>();
    auto springProp = AceType::MakeRefPtr<SpringProperty>(mass, stiffness, damping);
    obj->SetSpringProp(springProp);
    obj->IncRefCount();
    info.SetReturnValue(AceType::RawPtr(obj));
}

void JSSpringProp::DestructorCallback(JSSpringProp* obj)
{
    if (obj != nullptr) {
        obj->DecRefCount();
    }
}

void JSMotion::ConstructorCallback(const JSCallbackInfo& info)
{
    int32_t len = info.Length();
    if (len != FRICITION_MOTION_LENGTH && len != SPRING_MOTION_LENGTH && len != SCROLL_MOTION_LENGTH) {
        LOGE("The arg is wrong, it is supposed to have 3 or 4 or 5 arguments");
        return;
    }
    auto obj = AceType::MakeRefPtr<JSMotion>();
    if (len == FRICITION_MOTION_LENGTH) {
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber()) {
            LOGE("The friction args is wrong");
            return;
        }
        double friction = info[0]->ToNumber<double>();
        double position = info[1]->ToNumber<double>();
        double velocity = info[2]->ToNumber<double>();
        RefPtr<FrictionMotion> frictionMotion = AceType::MakeRefPtr<FrictionMotion>(friction, position, velocity);
        obj->SetMotion(frictionMotion);
    } else if (len == SPRING_MOTION_LENGTH) {
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsObject()) {
            LOGE("The spring args is wrong");
            return;
        }
        double start = info[0]->ToNumber<double>();
        double end = info[1]->ToNumber<double>();
        double velocity = info[2]->ToNumber<double>();
        JSSpringProp* prop = JSRef<JSObject>::Cast(info[3])->Unwrap<JSSpringProp>();
        if (!prop) {
            LOGE("SpringProp is nullptr");
            return;
        }
        RefPtr<SpringProperty> springProperty = prop->GetSpringProp();
        auto springMotion = AceType::MakeRefPtr<SpringMotion>(start, end, velocity, springProperty);
        obj->SetMotion(springMotion);
    } else {
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber()
            || !info[4]->IsObject()) {
            LOGE("The scroll args is wrong");
            return;
        }
        double position = info[0]->ToNumber<double>();
        double velocity = info[1]->ToNumber<double>();
        double min = info[2]->ToNumber<double>();
        double max = info[3]->ToNumber<double>();
        JSSpringProp* prop = JSRef<JSObject>::Cast(info[4])->Unwrap<JSSpringProp>();
        if (!prop) {
            LOGE("SpringProp is nullptr");
            return;
        }
        RefPtr<SpringProperty> springProperty = prop->GetSpringProp();
        RefPtr<ScrollMotion> scrollMotion = AceType::MakeRefPtr<ScrollMotion>(position, velocity, ExtentPair(min, min),
            ExtentPair(max, max), springProperty);
        obj->SetMotion(scrollMotion);
    }
    obj->IncRefCount();
    info.SetReturnValue(AceType::RawPtr(obj));
}

void JSMotion::DestructorCallback(JSMotion* obj)
{
    if (obj != nullptr) {
        obj->DecRefCount();
    }
}

} // namespace OHOS::Ace::Framework