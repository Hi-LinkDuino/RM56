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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ANIMATION_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ANIMATION_BRIDGE_H

#include <map>
#include <string>
#include <v8.h>

#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/js_frontend/engine/common/base_animation_bridge.h"
#include "frameworks/bridge/js_frontend/frontend_delegate.h"
#include "frameworks/bridge/js_frontend/js_command.h"

namespace OHOS::Ace::Framework {

class V8AnimationBridgeUtils {
public:
    static v8::Local<v8::Object> CreateAnimationContext(v8::Local<v8::Context>& ctx, int32_t pageId, NodeId nodeId);
    static void JsAnimationPlay(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsAnimationFinish(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsAnimationPause(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsAnimationCancel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsAnimationReverse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsAnimationPlayStateGet(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void JsAnimationStartTimeGet(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void JsAnimationPendingGet(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void JsAnimationPlayStateSet(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void JsAnimationStartTimeSet(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void JsAnimationPendingSet(const v8::FunctionCallbackInfo<v8::Value>& info);
};

class V8AnimationBridge : public BaseAnimationBridge {
    DECLARE_ACE_TYPE(V8AnimationBridge, BaseAnimationBridge)

public:
    V8AnimationBridge(const v8::Local<v8::Context>& ctx, v8::Isolate* instance, v8::Local<v8::Object> animationContext,
        NodeId nodeId);
    ~V8AnimationBridge() override = default;
    v8::Local<v8::Object> GetJsObject()
    {
        return animationObject_.Get(instance_);
    }

    v8::Local<v8::Context> GetContext() const
    {
        return ctx_.Get(instance_);
    }

    void JsCreateAnimation(const RefPtr<JsAcePage>& page, const std::string& param);
    void SetPlayStateCallbacks(RefPtr<Animator>& animator);
    void SetPlayStateCallbacksWithListenerId(RefPtr<Animator>& animator);

private:
    v8::Persistent<v8::Context> ctx_;
    v8::Isolate* instance_ = nullptr;
    v8::Persistent<v8::Object> animationObject_;
    NodeId nodeId_ = -1;
    IdType finishListenerId_ = 0;
    IdType idleListenerId_ = 0;
};

class V8AnimationBridgeTaskCreate : public AnimationBridgeTask {
    DECLARE_ACE_TYPE(V8AnimationBridgeTaskCreate, AnimationBridgeTask)
public:
    V8AnimationBridgeTaskCreate(const RefPtr<FrontendDelegate>& delegate,
        const RefPtr<V8AnimationBridge>& bridge, std::string param);
    ~V8AnimationBridgeTaskCreate() override = default;
    void AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId) override;

private:
    RefPtr<V8AnimationBridge> bridge_;
    WeakPtr<FrontendDelegate> delegate_;
    std::string param_;
};

class V8AnimationBridgeTaskOperation : public AnimationBridgeTask {
    DECLARE_ACE_TYPE(V8AnimationBridgeTaskOperation, AnimationBridgeTask)
public:
    explicit V8AnimationBridgeTaskOperation(AnimationOperation operation) : operation_(operation) {}
    ~V8AnimationBridgeTaskOperation() override = default;
    void AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId) override;

private:
    AnimationOperation operation_ = AnimationOperation::NONE;
};

class V8AnimationBridgeTaskStartTime : public AnimationBridgeTask {
    DECLARE_ACE_TYPE(V8AnimationBridgeTaskStartTime, AnimationBridgeTask)
public:
    explicit V8AnimationBridgeTaskStartTime(std::string startTime) : startTime_(std::move(startTime)) {}
    ~V8AnimationBridgeTaskStartTime() override = default;
    void AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId) override;

private:
    std::string startTime_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ANIMATION_BRIDGE_H
