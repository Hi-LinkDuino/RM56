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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_ANIMATION_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_ANIMATION_BRIDGE_H

#include <map>
#include <string>

#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/js_frontend/engine/common/base_animation_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"
#include "frameworks/bridge/js_frontend/js_command.h"

namespace OHOS::Ace::Framework {
using std::shared_ptr;

class JsiAnimationBridgeUtils {
public:
    static shared_ptr<JsValue> CreateAnimationContext(shared_ptr<JsRuntime> runtime, int32_t pageId, NodeId nodeId);
    static shared_ptr<JsValue> JsAnimationPlay(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimationFinish(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimationPause(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimationCancel(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimationReverse(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimationPlayStateGet(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimationStartTimeGet(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimationPendingGet(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimationPlayStateSet(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimationStartTimeSet(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimationPendingSet(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
};

class JsiAnimationBridge : public BaseAnimationBridge {
    DECLARE_ACE_TYPE(JsiAnimationBridge, BaseAnimationBridge)

public:
    JsiAnimationBridge(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& animationContext,
                       NodeId nodeId);
    ~JsiAnimationBridge() override = default;

    void OnJsEngineDestroy() override
    {
        animationObject_.reset();
    }

    shared_ptr<JsValue> GetJsObject() const
    {
        return animationObject_;
    }

    shared_ptr<JsRuntime> GetRuntime() const
    {
        return runtime_;
    }

    void JsCreateAnimation(const RefPtr<JsAcePage>& page, const std::string& param);
    void SetPlayStateCallbacks(RefPtr<Animator>& animator);
    void SetPlayStateCallbacksWithListenerId(RefPtr<Animator>& animator);

private:
    shared_ptr<JsRuntime> runtime_;
    shared_ptr<JsValue> animationObject_;
    NodeId nodeId_ = -1;
    IdType finishListenerId_ = 0;
    IdType idleListenerId_ = 0;
};

class JsiAnimationBridgeTaskCreate : public AnimationBridgeTask {
    DECLARE_ACE_TYPE(JsiAnimationBridgeTaskCreate, AnimationBridgeTask)
public:
    JsiAnimationBridgeTaskCreate(
        shared_ptr<JsRuntime> runtime, const RefPtr<JsiAnimationBridge>& bridge, std::string param);
    ~JsiAnimationBridgeTaskCreate() override = default;
    void AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId) override;

private:
    RefPtr<JsiAnimationBridge> bridge_;
    shared_ptr<JsRuntime> runtime_;
    std::string param_;
};

class JsiAnimationBridgeTaskOperation : public AnimationBridgeTask {
    DECLARE_ACE_TYPE(JsiAnimationBridgeTaskOperation, AnimationBridgeTask)
public:
    explicit JsiAnimationBridgeTaskOperation(AnimationOperation operation) : operation_(operation) {}
    ~JsiAnimationBridgeTaskOperation() override = default;
    void AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId) override;

private:
    AnimationOperation operation_ = AnimationOperation::NONE;
};

class JsiAnimationBridgeTaskStartTime : public AnimationBridgeTask {
    DECLARE_ACE_TYPE(JsiAnimationBridgeTaskStartTime, AnimationBridgeTask)
public:
    explicit JsiAnimationBridgeTaskStartTime(std::string startTime) : startTime_(std::move(startTime)) {}
    ~JsiAnimationBridgeTaskStartTime() override = default;
    void AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId) override;

private:
    std::string startTime_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_ANIMATION_BRIDGE_H
