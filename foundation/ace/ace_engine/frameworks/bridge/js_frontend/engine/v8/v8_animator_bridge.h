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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ANIMATOR_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ANIMATOR_BRIDGE_H

#include <map>
#include <string>
#include <v8.h>

#include "core/animation/keyframe_animation.h"
#include "frameworks/bridge/js_frontend/engine/common/base_animation_bridge.h"
#include "frameworks/bridge/js_frontend/js_command.h"

namespace OHOS::Ace::Framework {

class V8AnimatorBridgeUtils {
public:
    static v8::Local<v8::Object> CreateAnimatorContext(v8::Local<v8::Context>& ctx, int32_t pageId, int32_t bridgeId);
    static void JsAnimatorPlay(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsAnimatorFinish(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsAnimatorPause(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsAnimatorCancel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsAnimatorReverse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsAnimatorUpdate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static int32_t JsCreateBridgeId();
};

class V8AnimatorBridge : public BaseAnimationBridge {
    DECLARE_ACE_TYPE(V8AnimatorBridge, BaseAnimationBridge)
public:
    V8AnimatorBridge(const v8::Local<v8::Context>& ctx, v8::Isolate* instance, v8::Local<v8::Object> animatorContext);
    ~V8AnimatorBridge() override;

    void JsCreateAnimation(const std::string& param);
    RefPtr<Animator> JsGetAnimator() override
    {
        return animator_;
    }

    v8::Local<v8::Object> GetJsObject()
    {
        return animatorObject_.Get(instance_);
    }

    v8::Local<v8::Context> GetContext() const
    {
        return ctx_.Get(instance_);
    }

private:
    RefPtr<KeyframeAnimation<double>> CreateDoubleAnimation(
        std::unordered_map<std::string, double>& animationParams, const RefPtr<Curve>& curve);

    v8::Persistent<v8::Context> ctx_;
    v8::Isolate* instance_ = nullptr;
    v8::Persistent<v8::Object> animatorObject_;
    RefPtr<Animator> animator_;
};

class V8AnimatorTaskCreate : public AnimatorBridgeTask {
    DECLARE_ACE_TYPE(V8AnimatorTaskCreate, AnimatorBridgeTask)
public:
    V8AnimatorTaskCreate(v8::Isolate* isolate, const RefPtr<V8AnimatorBridge>& bridge, const std::string& param);
    ~V8AnimatorTaskCreate() override = default;
    void AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId) override;

private:
    RefPtr<V8AnimatorBridge> bridge_;
    v8::Isolate* isolate_ = nullptr;
    std::string param_;
};

class V8AnimatorTaskOperation : public AnimatorBridgeTask {
    DECLARE_ACE_TYPE(V8AnimatorTaskOperation, AnimatorBridgeTask)
public:
    explicit V8AnimatorTaskOperation(AnimatorOperation operation) : operation_(operation) {};
    ~V8AnimatorTaskOperation() override = default;
    void AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId) override;

private:
    AnimatorOperation operation_ = AnimatorOperation::NONE;
};

class V8AnimatorTaskUpdate : public AnimatorBridgeTask {
    DECLARE_ACE_TYPE(V8AnimatorTaskUpdate, AnimatorBridgeTask)
public:
    V8AnimatorTaskUpdate(v8::Isolate* isolate, const std::unordered_map<std::string, std::string>& params)
    : isolate_(isolate), params_(params) {};
    ~V8AnimatorTaskUpdate() override = default;
    void AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId) override;

private:
    void UpdateAnimator(const RefPtr<Animator>& animator, const RefPtr<V8AnimatorBridge>& bridge,
        v8::Isolate* isolate, const std::unordered_map<std::string, std::string>& params);
    RefPtr<KeyframeAnimation<double>> CreateDoubleAnimation(double begin, double end, const RefPtr<Curve>& curve);

    v8::Isolate* isolate_ = nullptr;
    std::unordered_map<std::string, std::string> params_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ANIMATOR_BRIDGE_H
