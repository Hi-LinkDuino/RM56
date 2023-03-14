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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_ANIMATOR_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_ANIMATOR_BRIDGE_H

#include <string>
#include <unordered_map>

#include "frameworks/bridge/js_frontend/engine/common/base_animation_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"

namespace OHOS::Ace::Framework {
using std::shared_ptr;

class JsiAnimatorBridgeUtils {
public:
    static shared_ptr<JsValue> CreateAnimatorContext(shared_ptr<JsRuntime> runtime, int32_t pageId, int32_t bridgeId);
    static shared_ptr<JsValue> JsAnimatorPlay(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimatorFinish(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimatorPause(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimatorCancel(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimatorReverse(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAnimatorUpdate(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static int32_t JsCreateBridgeId();
};

class JsiAnimatorBridge : public BaseAnimationBridge {
    DECLARE_ACE_TYPE(JsiAnimatorBridge, BaseAnimationBridge)
public:
    JsiAnimatorBridge(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& animatorContext);
    ~JsiAnimatorBridge() override;

    void JsCreateAnimation(const std::string& param);
    RefPtr<Animator> JsGetAnimator() override
    {
        return animator_;
    }

    void OnJsEngineDestroy() override
    {
        animatorObject_.reset();
    }

    shared_ptr<JsValue> GetJsObject()
    {
        return animatorObject_;
    }

    shared_ptr<JsRuntime> GetRuntime() const
    {
        return runtime_;
    }

private:
    RefPtr<KeyframeAnimation<double>> CreateDoubleAnimation(
        std::unordered_map<std::string, double>& animationParams, const RefPtr<Curve>& curve);

    shared_ptr<JsRuntime> runtime_;
    shared_ptr<JsValue> animatorObject_;
    RefPtr<Animator> animator_;
};

class JsiAnimatorTaskCreate : public AnimatorBridgeTask {
    DECLARE_ACE_TYPE(JsiAnimatorTaskCreate, AnimatorBridgeTask)
public:
    JsiAnimatorTaskCreate(shared_ptr<JsRuntime> runtime,
        const RefPtr<JsiAnimatorBridge>& bridge, const std::string& param);
    ~JsiAnimatorTaskCreate() override = default;
    void AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId) override;

private:
    RefPtr<JsiAnimatorBridge> bridge_;
    shared_ptr<JsRuntime> runtime_;
    std::string param_;
};

class JsiAnimatorTaskOperation : public AnimatorBridgeTask {
    DECLARE_ACE_TYPE(JsiAnimatorTaskOperation, AnimatorBridgeTask)
public:
    explicit JsiAnimatorTaskOperation(AnimatorOperation operation) : operation_(operation) {};
    ~JsiAnimatorTaskOperation() override = default;
    void AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId) override;

private:
    AnimatorOperation operation_ = AnimatorOperation::NONE;
};

class JsiAnimatorTaskUpdate : public AnimatorBridgeTask {
    DECLARE_ACE_TYPE(JsiAnimatorTaskUpdate, AnimatorBridgeTask)
public:
    JsiAnimatorTaskUpdate(shared_ptr<JsRuntime> runtime, const std::unordered_map<std::string, std::string>& params)
        : runtime_(runtime), params_(params) {};
    ~JsiAnimatorTaskUpdate() override = default;
    void AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId) override;

private:
    void UpdateAnimator(const RefPtr<Animator>& animator, const RefPtr<JsiAnimatorBridge>& bridge,
        shared_ptr<JsRuntime> runtime, const std::unordered_map<std::string, std::string>& params);
    RefPtr<KeyframeAnimation<double>> CreateDoubleAnimation(double begin, double end, const RefPtr<Curve>& curve);

    shared_ptr<JsRuntime> runtime_;
    std::unordered_map<std::string, std::string> params_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_ANIMATOR_BRIDGE_H
