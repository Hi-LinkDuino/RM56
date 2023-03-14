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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_FUNCTION_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_FUNCTION_H

#include <functional>

#include "base/memory/ace_type.h"
#include "frameworks/bridge/declarative_frontend/engine/bindings.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/engine/js_execution_scope_defines.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_xcomponent_controller.h"

namespace OHOS::Ace::Framework {

class ACE_EXPORT JsFunction : public virtual AceType {
    DECLARE_ACE_TYPE(JsFunction, AceType);

public:
    explicit JsFunction(const JSRef<JSFunc>& jsFunction);
    JsFunction(const JSRef<JSObject>& jsObject, const JSRef<JSFunc>& jsFunction);
    ~JsFunction() override;
    void Execute();
    void Execute(const std::vector<std::string>& keys, const std::string& param);
    void ExecuteNew(const std::vector<std::string>& keys, const std::string& param,
        RefPtr<JSXComponentController>& jsXComponentController);
    JSRef<JSVal> ExecuteJS(int argc = 0, JSRef<JSVal>* argv = nullptr);

protected:
    JSRef<JSFunc> jsFunction_;
    JSWeak<JSVal> jsThis_;
};

template<class T, int32_t ARGC = 0>
class ACE_EXPORT JsEventFunction : public JsFunction {
    DECLARE_ACE_TYPE(JsEventFunction, JsFunction);

public:
    using ParseFunc = std::function<JSRef<JSVal>(const T&)>;
    JsEventFunction() = delete;
    JsEventFunction(const JSRef<JSFunc>& jsFunction, ParseFunc parser)
        : JsFunction(JSRef<JSObject>(), jsFunction), parser_(parser)
    {}
    ~JsEventFunction() override = default;

    void Execute()
    {
        JsFunction::ExecuteJS();
    }

    void Execute(const T& eventInfo)
    {
        JSRef<JSVal> param;
        if (parser_) {
            param = parser_(eventInfo);
        }
        JsFunction::ExecuteJS(ARGC, &param);
    }

    JSRef<JSVal> ExecuteWithValue(const T& eventInfo)
    {
        JSRef<JSVal> param;
        if (parser_) {
            param = parser_(eventInfo);
        }
        return JsFunction::ExecuteJS(ARGC, &param);
    }

private:
    ParseFunc parser_;
};

JSRef<JSObject> CreateEventTargetObject(const BaseEventInfo& info);

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_FUNCTION_H
