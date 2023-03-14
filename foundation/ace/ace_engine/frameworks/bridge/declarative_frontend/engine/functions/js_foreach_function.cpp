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

#include "frameworks/bridge/declarative_frontend/engine/functions/js_foreach_function.h"

#include "frameworks/bridge/declarative_frontend/jsview/js_view.h"

namespace OHOS::Ace::Framework {

std::vector<std::string> JsForEachFunction::ExecuteIdentityMapper()
{
    std::vector<std::string> result;

    JSRef<JSObject> jsKeys;
    if (!jsIdentityMapperFunc_.IsEmpty()) {
        JSRef<JSVal> argv[] = { jsIdentityMapperFunc_.Lock() };
        jsKeys = JSRef<JSObject>::Cast(JsFunction::ExecuteJS(1, argv));

        if (jsKeys.IsEmpty()) {
            return result;
        }
    }

    JSRef<JSArray> jsKeysArr;
    if (!jsKeys.IsEmpty()) {
        jsKeysArr = JSRef<JSArray>::Cast(jsKeys);
    } else {
        jsKeysArr = JSRef<JSArray>::Cast(jsThis_.Lock());
    }
    int length = static_cast<int>(jsKeysArr->Length());

    for (int i = 0; i < length; i++) {
        if (!jsKeys.IsEmpty()) {
            JSRef<JSVal> jsKey = jsKeysArr->GetValueAt(i);

            if (!jsKey->IsString() && !jsKey->IsNumber()) {
                LOGE("ForEach Item with invalid identifier.........");
                continue;
            }

            std::string key(jsKey->ToString());
            result.emplace_back(key.c_str());
        } else {
            result.emplace_back(std::to_string(i));
        }
    }

    return result;
}

void JsForEachFunction::ExecuteBuilderForIndex(int32_t index)
{
    LOGD("ExecuteBuilderForIndex: start, index %{public}d", index);
    // indexed item
    JSRef<JSArray> jsArray = JSRef<JSArray>::Cast(jsThis_.Lock());
    JSRef<JSVal> params[2];
    params[0] = jsArray->GetValueAt(index);
    params[1] = JSRef<JSVal>::Make(ToJSValue(index));
    jsViewMapperFunc_.Lock()->Call(jsThis_.Lock(), 2, params);
}

} // namespace OHOS::Ace::Framework
