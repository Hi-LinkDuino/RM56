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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_ANIMATION_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_ANIMATION_BRIDGE_H

#include <map>
#include <string>

#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/js_frontend/engine/common/base_animation_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"
#include "frameworks/bridge/js_frontend/frontend_delegate.h"
#include "frameworks/bridge/js_frontend/js_command.h"

namespace OHOS::Ace::Framework {

class JsAcePage;

class AnimationBridgeUtils {
public:
    static JSValue CreateAnimationContext(JSContext* ctx, int32_t pageId, NodeId nodeId);
    static JSValue JsAnimationPlay(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAnimationFinish(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAnimationPause(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAnimationCancel(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAnimationReverse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAnimationPlayStateGet(JSContext* ctx, JSValueConst value);
    static JSValue JsAnimationStartTimeGet(JSContext* ctx, JSValueConst value);
    static JSValue JsAnimationPendingGet(JSContext* ctx, JSValueConst value);
    static JSValue JsAnimationPlayStateSet(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsAnimationStartTimeSet(JSContext* ctx, JSValueConst value, JSValueConst startTime);
    static JSValue JsAnimationPendingSet(JSContext* ctx, JSValueConst value, JSValueConst pending);
};

class AnimationBridge : public BaseAnimationBridge {
    DECLARE_ACE_TYPE(AnimationBridge, BaseAnimationBridge)

public:
    AnimationBridge(JSContext* ctx, JSValue animationContext, NodeId nodeId);
    ~AnimationBridge() override;
    void OnJsEngineDestroy() override;
    JSValue GetJsObject()
    {
        return animationContext_;
    }

    void JsCreateAnimation(const RefPtr<JsAcePage>& page, const std::string& param);
    void SetPlayStateCallbacks(RefPtr<Animator>& animator);
    void SetPlayStateCallbacksWithListenerId(RefPtr<Animator>& animator);

private:
    JSContext* ctx_ = nullptr;
    JSValue animationContext_;
    NodeId nodeId_ = -1;
    IdType finishListenerId_ = 0;
    IdType idleListenerId_ = 0;
};

class AnimationBridgeTaskCreate : public AnimationBridgeTask {
    DECLARE_ACE_TYPE(AnimationBridgeTaskCreate, AnimationBridgeTask)
public:
    AnimationBridgeTaskCreate(const RefPtr<AnimationBridge>& animationBridge, std::string param);
    ~AnimationBridgeTaskCreate() override = default;
    void AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId) override;

private:
    RefPtr<AnimationBridge> animationBridge_;
    std::string param_;
};

class AnimationBridgeTaskOperation : public AnimationBridgeTask {
    DECLARE_ACE_TYPE(AnimationBridgeTaskOperation, AnimationBridgeTask)
public:
    explicit AnimationBridgeTaskOperation(AnimationOperation operation) : operation_(operation) {};
    ~AnimationBridgeTaskOperation() override = default;
    void AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId) override;

private:
    AnimationOperation operation_ = AnimationOperation::NONE;
};

class AnimationBridgeTaskStartTime : public AnimationBridgeTask {
    DECLARE_ACE_TYPE(AnimationBridgeTaskStartTime, AnimationBridgeTask)
public:
    explicit AnimationBridgeTaskStartTime(int32_t startTime) : startTime_(startTime) {}
    ~AnimationBridgeTaskStartTime() override = default;
    void AnimationBridgeTaskFunc(const RefPtr<JsAcePage>& page, NodeId nodeId) override;

private:
    int32_t startTime_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_ANIMATION_BRIDGE_H
