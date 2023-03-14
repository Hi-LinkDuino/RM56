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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_XCOMPONENT_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_XCOMPONENT_H

#include "core/components/xcomponent/xcomponent_component.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_utils.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_xcomponent_controller.h"

namespace OHOS::Ace::Framework {
class XComponentClient {
public:
    using GetCallback = std::function<bool(RefPtr<XComponentComponent> &component)>;
    using GetJSValCallback = std::function<bool(JSRef<JSVal> &param)>;
    XComponentClient &operator = (const XComponentClient &) = delete;
    XComponentClient(const XComponentClient &) = delete;
    ~XComponentClient() = default;

    static XComponentClient& GetInstance()
    {
        static XComponentClient instance;
        return instance;
    }

    void RegisterCallback(GetCallback &&callback)
    {
        getCallback_ = callback;
    }

    void RegisterJSValCallback(GetJSValCallback &&callback)
    {
        getJSValCallback_ = callback;
    }

    void SetRegisterCallbackToNull()
    {
        getCallback_ = nullptr;
    }

    bool GetXComponent(RefPtr<XComponentComponent> &component)
    {
        if (getCallback_) {
            return getCallback_(component);
        } else {
            return false;
        }
    }

    void SetJSValCallToNull()
    {
        getJSValCallback_ = nullptr;
    }

    bool GetJSVal(JSRef<JSVal> &param)
    {
        if (getJSValCallback_) {
            return getJSValCallback_(param);
        } else {
            return false;
        }
    }

private:
    XComponentClient() = default;
    GetCallback getCallback_ = nullptr;
    GetJSValCallback getJSValCallback_ = nullptr;
};

class ACE_EXPORT JSXComponent : public JSContainerBase {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);
    static void JsOnLoad(const JSCallbackInfo& args);
    static void JsOnDestroy(const JSCallbackInfo& args);

private:
    static EventMarker GetEventMarker(const JSCallbackInfo& info, const std::vector<std::string>& keys);
    static RefPtr<JSXComponentController> jsXComponentController_;
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_XCOMPONENT_H