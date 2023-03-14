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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_SCROLL_VIEW_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_SCROLL_VIEW_BRIDGE_H

#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_engine.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace OHOS::Ace::Framework {

class ComponentApiBridge : virtual public AceType {
    DECLARE_ACE_TYPE(ComponentApiBridge, AceType)

public:
    static JSValue JsGetScrollOffset(JSContext* ctx, NodeId nodeId);
    static JSValue JsGetBoundingRect(JSContext* ctx, NodeId nodeId);
    static JSValue JsGetInspector(JSContext* ctx, NodeId nodeId);
    static void JsScrollTo(JSContext* ctx, const std::string& args, NodeId nodeId);
    static JSValue JsCreateObserver(JSContext* ctx, const std::string& args, NodeId nodeId);

    static JSValue JsObserverOn(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsObserverOff(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
};

class QJSVisibleListenerCallback : virtual public AceType {
    DECLARE_ACE_TYPE(QJSVisibleListenerCallback, AceType)

public:
    QJSVisibleListenerCallback(JSContext* ctx, JSValue callback, NodeId nodeId);

    ~QJSVisibleListenerCallback() override;

    JSValue GetJsObject()
    {
        return listenCallback_;
    }

    JSContext* GetContext() const
    {
        return ctx_;
    }

    NodeId GetNodeId() const
    {
        return nodeId_;
    }

private:
    JSContext* ctx_ = nullptr;
    JSValue listenCallback_;
    NodeId nodeId_ = -1;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_SCROLL_VIEW_BRIDGE_H
