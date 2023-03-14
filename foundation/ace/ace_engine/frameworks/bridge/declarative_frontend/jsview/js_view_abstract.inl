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

#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

#ifdef USE_QUICKJS_ENGINE

template<typename T>
JSValue JSViewAbstract::JsSetSingleString(JSContext* ctx, JSValueConst this_value, int32_t argc, JSValueConst* argv,
    std::function<void(T* view, std::string)> setterFunc, const char* debugInfo)
{
    if ((argv == nullptr) || (argc != 1)) {
        return JS_ThrowSyntaxError(ctx, "%s: one parameter of type string expected", debugInfo);
    }
    if (!JS_IsString(argv[0])) {
        return JS_ThrowSyntaxError(ctx, "%s: parameter of type string expected", debugInfo);
    }

    QJSContext::Scope scope(ctx);
    T* view = static_cast<T*>(UnwrapAny(this_value));
    if (!view) {
        return JS_ThrowInternalError(ctx, "%s: Failed to unwrap JS object to C++ JSView!", debugInfo);
    }

    LOGD("%s(%s)", debugInfo, ScopedString(argv[0]).get());
    setterFunc(view, ScopedString(argv[0]).str());
    return JS_DupValue(ctx, this_value);
}

#endif // USE_QUICKJS_ENGINE

} // namespace OHOS::Ace::Framework
