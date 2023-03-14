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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_ANIMATOR_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_ANIMATOR_H

#include "frameworks/base/memory/ace_type.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "frameworks/core/animation/keyframe_animation.h"

namespace OHOS::Ace::Framework {

enum class AnimationStatus {
    INITIAL,
    RUNNING,
    PAUSED,
    STOPPED,
};

enum class EventOperation {
    NONE,
    START,
    PAUSE,
    REPEAT,
    CANCEL,
    FINISH,
};

class JSAnimator : public AceType {
    DECLARE_ACE_TYPE(JSAnimator, AceType);

public:
    JSAnimator() = default;
    ~JSAnimator() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);

    static void SetState(int32_t state);
    static void SetDuration(int32_t duration);
    static void SetCurve(const JSCallbackInfo& info);
    static void SetDelay(int32_t delay);
    static void SetFillMode(int32_t fillMode);
    static void SetIteration(int32_t iteration);
    static void SetPlayMode(int32_t playMode);
    static void SetMotion(const JSCallbackInfo& info);

    static void OnStart(const JSCallbackInfo& info);
    static void OnPause(const JSCallbackInfo& info);
    static void OnRepeat(const JSCallbackInfo& info);
    static void OnCancel(const JSCallbackInfo& info);
    static void OnFinish(const JSCallbackInfo& info);
    static void OnFrame(const JSCallbackInfo& info);

private:
    static EventMarker GetEventMarker(const JSCallbackInfo& info);
    static std::string animatorId_;

};

class JSSpringProp : public AceType {
    DECLARE_ACE_TYPE(JSSpringProp, AceType);

public:
    JSSpringProp() = default;
    ~JSSpringProp() override = default;

    static void ConstructorCallback(const JSCallbackInfo& info);
    static void DestructorCallback(JSSpringProp* obj);

    void SetSpringProp(const RefPtr<SpringProperty>& springProp)
    {
        springProp_ = springProp;
    }

    const RefPtr<SpringProperty>& GetSpringProp()
    {
        return springProp_;
    }

private:
    RefPtr<SpringProperty> springProp_;

};

class JSMotion : public AceType {
    DECLARE_ACE_TYPE(JSMotion, AceType);

public:
    JSMotion() = default;
    ~JSMotion() override = default;

    static void ConstructorCallback(const JSCallbackInfo& info);
    static void DestructorCallback(JSMotion* obj);

    void SetMotion(const RefPtr<Motion>& motion)
    {
        motion_ = motion;
    }

    const RefPtr<Motion>& GetMotion()
    {
        return motion_;
    }

private:
    RefPtr<Motion> motion_;

};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_ANIMATOR_H