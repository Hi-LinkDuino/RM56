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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_ANIMATOR_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_ANIMATOR_BRIDGE_H

#include <map>
#include <string>

#include "core/animation/keyframe_animation.h"
#include "frameworks/bridge/js_frontend/engine/common/base_animation_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"
#include "frameworks/bridge/js_frontend/frontend_delegate.h"
#include "frameworks/bridge/js_frontend/js_command.h"

namespace OHOS::Ace::Framework {

class AnimatorBridgeUtils {
public:
    static JSValue CreateAnimatorContext(JSContext* ctx, int32_t pageId, int32_t bridgeId);
    static JSValue JsAnimatorPlay(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAnimatorFinish(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAnimatorPause(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAnimatorCancel(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAnimatorReverse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAnimatorUpdate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static int32_t JsCreateBridgeId();
};

class AnimatorBridge : public BaseAnimationBridge {
    DECLARE_ACE_TYPE(AnimatorBridge, BaseAnimationBridge)
public:
    AnimatorBridge(JSContext* ctx, JSValue animationContext);
    ~AnimatorBridge() override;
    void OnJsEngineDestroy() override;
    JSValue GetJsObject()
    {
        return animationContext_;
    }

    JSContext* GetContext()
    {
        return ctx_;
    }

    RefPtr<Animator> JsGetAnimator() override
    {
        return animator_;
    }

    void JsCreateAnimation(const std::string& param);

private:
    RefPtr<KeyframeAnimation<double>> CreateDoubleAnimation(
        const std::unordered_map<std::string, double>& animationParams, const RefPtr<Curve>& curve);

    JSContext* ctx_ = nullptr;
    JSValue animationContext_;
    RefPtr<Animator> animator_;
};

class AnimatorTaskCreate : public AnimatorBridgeTask {
    DECLARE_ACE_TYPE(AnimatorTaskCreate, AnimatorBridgeTask)
public:
    AnimatorTaskCreate(const RefPtr<AnimatorBridge>& bridge, const std::string& param);
    ~AnimatorTaskCreate() override = default;
    void AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId) override;

private:
    RefPtr<AnimatorBridge> bridge_;
    std::string param_;
};

class AnimatorTaskOperation : public AnimatorBridgeTask {
    DECLARE_ACE_TYPE(AnimatorTaskOperation, AnimatorBridgeTask)
public:
    explicit AnimatorTaskOperation(AnimatorOperation operation) : operation_(operation) {}
    ~AnimatorTaskOperation() override = default;
    void AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId) override;

private:
    AnimatorOperation operation_ = AnimatorOperation::NONE;
};

class AnimatorTaskUpdate : public AnimatorBridgeTask {
    DECLARE_ACE_TYPE(AnimatorTaskUpdate, AnimatorBridgeTask)
public:
    explicit AnimatorTaskUpdate(const std::unordered_map<std::string, std::string>& params) : params_(params) {}
    ~AnimatorTaskUpdate() override = default;
    void AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId) override;

private:
    void UpdateAnimator(const RefPtr<Animator>& animator, const RefPtr<AnimatorBridge>& bridge,
        JSContext* ctx, const std::unordered_map<std::string, std::string>& params);
    RefPtr<KeyframeAnimation<double>> CreateDoubleAnimation(double begin, double end, const RefPtr<Curve>& curve);

    std::unordered_map<std::string, std::string> params_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_ANIMATOR_BRIDGE_H