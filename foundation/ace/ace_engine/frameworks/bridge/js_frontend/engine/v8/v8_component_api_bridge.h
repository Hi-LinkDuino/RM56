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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_SCROLL_VIEW_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_SCROLL_VIEW_BRIDGE_H

#include "frameworks/bridge/js_frontend/engine/v8/v8_engine.h"

namespace OHOS::Ace::Framework {

class V8ComponentApiBridge : virtual public AceType {
    DECLARE_ACE_TYPE(V8ComponentApiBridge, AceType)

public:
    static v8::Local<v8::Object> JsGetScrollOffset(v8::Isolate* isolate, NodeId nodeId);

    static v8::Local<v8::Object> JsGetBoundingRect(v8::Isolate* isolate, NodeId nodeId);

    static void JsScrollTo(
        const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& arguments, NodeId nodeId);

    static v8::Local<v8::Object> JsAddVisibleListener(
        const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& arguments, NodeId nodeId);

    static void JsVisibleListenerOn(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsVisibleListenerOff(const v8::FunctionCallbackInfo<v8::Value>& args);
};

class VisibleListenerCallback : virtual public AceType {
    DECLARE_ACE_TYPE(VisibleListenerCallback, AceType)
public:
    VisibleListenerCallback(
        const v8::Local<v8::Context>& ctx, v8::Isolate* instance, v8::Local<v8::Function> callback, NodeId nodeId)
        : instance_(instance), nodeId_(nodeId)
    {
        listenCallback_.Reset(instance_, callback);
        ctx_.Reset(instance_, ctx);
    }

    ~VisibleListenerCallback() override = default;

    v8::Local<v8::Function> GetJsObject()
    {
        return listenCallback_.Get(instance_);
    }

    v8::Local<v8::Context> GetContext() const
    {
        return ctx_.Get(instance_);
    }

    NodeId GetNodeId() const
    {
        return nodeId_;
    }

private:
    v8::Persistent<v8::Context> ctx_;
    v8::Isolate* instance_ = nullptr;
    v8::Persistent<v8::Function> listenCallback_;
    NodeId nodeId_ = -1;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_SCROLL_VIEW_BRIDGE_H
