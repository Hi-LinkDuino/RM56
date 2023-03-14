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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_FOREACH_FUNCTION_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_FOREACH_FUNCTION_H

#include <map>

#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JsForEachFunction : public JsFunction {
    DECLARE_ACE_TYPE(JsForEachFunction, JsFunction)

private:
    JSWeak<JSFunc> jsIdentityMapperFunc_;
    JSWeak<JSFunc> jsViewMapperFunc_;

public:
    JsForEachFunction(JSRef<JSObject> jsArray, JSRef<JSFunc> jsIdentityMapperFunc, JSRef<JSFunc> jsViewMapperFunc)
        : JsFunction(jsArray, JSRef<JSFunc>::Cast(jsArray->GetProperty("map")))
    {
        if (!jsIdentityMapperFunc.IsEmpty()) {
            jsIdentityMapperFunc_ = jsIdentityMapperFunc;
        }
        jsViewMapperFunc_ = jsViewMapperFunc;
    }

    JsForEachFunction(JSRef<JSObject> jsArray, JSRef<JSFunc> jsViewMapperFunc)
        : JsFunction(jsArray, JSRef<JSFunc>::Cast(jsArray->GetProperty("map")))
    {
        jsViewMapperFunc_ = jsViewMapperFunc;
    }

    ~JsForEachFunction() override
    {
        LOGD("Destroy: JsForEachFunction");
    }

    // This executes the IdentifierFunction on all items  in a array
    // returns the vector of keys/ids in the same order.
    std::vector<std::string> ExecuteIdentityMapper();

    // This executes the BuilderFunction on a specific index
    // returns the native JsView for the item in index.
    void ExecuteBuilderForIndex(int32_t index);
}; // JsForEachFunction

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_FOREACH_FUNCTION_H
