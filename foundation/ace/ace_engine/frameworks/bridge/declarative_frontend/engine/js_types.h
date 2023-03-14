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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JS_TYPES_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JS_TYPES_H

#ifdef USE_V8_ENGINE
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_types.h"
#elif USE_QUICKJS_ENGINE
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_types.h"
#elif USE_ARK_ENGINE
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_types.h"
#endif

namespace OHOS::Ace::Framework {

#ifdef USE_V8_ENGINE
// QJS already has JSValue so we can't use that.
// Put QJS APIs within a namespace called "qjs"
// This way we will make sure there won't be any name clashes
using JSVal = V8Value;
using JSObject = V8Object;
// QJS has JSFunction typedef
using JSFunc = V8Funktion;
using JSArray = V8Array;
using JSCallbackInfo = V8CallbackInfo;
using JSGCMarkCallbackInfo = V8GCMarkCallbackInfo;
using JSException = V8Exception;
using JSExecutionContext = V8ExecutionContext;
using JSObjTemplate = V8ObjTemplate;

template<class T>
inline auto ToJSValue(T&& val)
{
    return V8ValueConvertor::toV8Value(std::forward<T>(val));
};

void JsStopPropagation(const v8::FunctionCallbackInfo<v8::Value>& info);

#elif USE_QUICKJS_ENGINE

using JSVal = QJSValue;
using JSObject = QJSObject;
using JSFunc = QJSFunction;
using JSArray = QJSArray;
using JSCallbackInfo = QJSCallbackInfo;
using JSGCMarkCallbackInfo = QJSGCMarkCallbackInfo;
using JSException = QJSException;
using JSExecutionContext = QJSExecutionContext;
using JSObjTemplate = QJSObjTemplate;

template<class T>
inline auto ToJSValue(T&& val)
{
    return QJSValueConvertor::toQJSValue(std::forward<T>(val));
};

JSValue JsStopPropagation(JSContext* ctx, JSValueConst new_target, int argc, JSValueConst* arg);

#elif USE_ARK_ENGINE

using JSVal = JsiValue;
using JSObject = JsiObject;
using JSFunc = JsiFunction;
using JSArray = JsiArray;
using JSCallbackInfo = JsiCallbackInfo;
using JSGCMarkCallbackInfo = JsiGCMarkCallbackInfo;
using JSException = JsiException;
using JSExecutionContext = JsiExecutionContext;
using JSObjTemplate = JsiObjTemplate;

template<class T>
inline auto ToJSValue(T&& val)
{
    return JsiValueConvertor::toJsiValue(std::forward<T>(val));
};

Local<JSValueRef> JsStopPropagation(EcmaVM*, Local<JSValueRef>, const Local<JSValueRef> [], int32_t, void*);

#endif

}; // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JS_TYPES_H
