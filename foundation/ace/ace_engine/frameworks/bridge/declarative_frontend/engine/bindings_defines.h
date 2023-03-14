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

#ifndef FOUNDATION_ACE_FRAMEWORKS_DECLARATIVE_FRONTEND_ENGINE_BINDINGS_DEFINES_H
#define FOUNDATION_ACE_FRAMEWORKS_DECLARATIVE_FRONTEND_ENGINE_BINDINGS_DEFINES_H

#include "frameworks/bridge/declarative_frontend/engine/js_types.h"

enum class JavascriptEngine { NONE, QUICKJS, V8, ARK };

#ifdef USE_QUICKJS_ENGINE

#ifdef __cplusplus
extern "C" {
#endif
#include "third_party/quickjs/cutils.h"
#include "third_party/quickjs/quickjs-libc.h"

void* JS_GetOpaqueA(JSValueConst obj, JSClassID* classIds, size_t classIdLen);

JSValueConst JS_NewGlobalCConstructor(
    JSContext* ctx, const char* name, JSCFunction* func, int length, JSValueConst proto);

#ifdef __cplusplus
}
#endif

using BindingTarget = JSValue;
using FunctionCallback = JSValue (*)(JSContext*, JSValueConst, int, JSValueConst*);
using FunctionGetCallback = JSValue (*)(JSContext*, JSValueConst);
using FunctionSetCallback = JSValue (*)(JSContext*, JSValueConst, JSValueConst);
template<typename T>
using MemberFunctionCallback = JSValue (T::*)(JSContext*, JSValueConst, int, JSValueConst*);
template<typename T>
using MemberFunctionGetCallback = JSValue (T::*)(JSContext*, JSValueConst);
template<typename T>
using MemberFunctionSetCallback = JSValue (T::*)(JSContext*, JSValueConst, JSValueConst);
using ExoticGetterCallback = JSValue (*)(JSContext* ctx, JSValueConst obj, JSAtom atom, JSValueConst receiver);
using ExoticSetterCallback = int (*)(
    JSContext* ctx, JSValueConst obj, JSAtom atom, JSValueConst value, JSValueConst receiver, int flags);
using ExoticHasPropertyCallback = int (*)(JSContext* ctx, JSValueConst obj, JSAtom atom);
using ExoticIsArrayCallback = int (*)(JSContext* ctx, JSValueConst obj);

/* return < 0 if exception or TRUE/FALSE */

constexpr const JavascriptEngine cCurrentJSEngine = JavascriptEngine::QUICKJS;

#elif USE_V8_ENGINE

#include "third_party/v8/include/v8.h"

using BindingTarget = v8::Local<v8::ObjectTemplate>;
using FunctionCallback = void (*)(const v8::FunctionCallbackInfo<v8::Value>&);
using FunctionGetCallback = void (*)(const v8::FunctionCallbackInfo<v8::Value>&);
using FunctionSetCallback = void (*)(const v8::FunctionCallbackInfo<v8::Value>&);
template<typename T>
using MemberFunctionCallback = void (T::*)(const v8::FunctionCallbackInfo<v8::Value>&);
template<typename T>
using MemberFunctionGetCallback = void (T::*)(const v8::FunctionCallbackInfo<v8::Value>&);
template<typename T>
using MemberFunctionSetCallback = void (T::*)(const v8::FunctionCallbackInfo<v8::Value>&);
using ExoticGetterCallback = int;
using ExoticSetterCallback = int;
using ExoticHasPropertyCallback = int;
using ExoticIsArrayCallback = int;

constexpr const JavascriptEngine cCurrentJSEngine = JavascriptEngine::V8;

#elif USE_ARK_ENGINE

#include "ecmascript/napi/include/jsnapi.h"

using BindingTarget = panda::Local<panda::ObjectRef>;
using FunctionCallback = panda::Local<panda::JSValueRef>(*)(
    panda::EcmaVM*, panda::Local<panda::JSValueRef>, const panda::Local<panda::JSValueRef> [], int32_t, void*);
using FunctionGetCallback = panda::Local<panda::JSValueRef>(*)(
        panda::EcmaVM*, panda::Local<panda::JSValueRef>, const panda::Local<panda::JSValueRef> [], int32_t, void*);
using FunctionSetCallback = panda::Local<panda::JSValueRef>(*)(
        panda::EcmaVM*, panda::Local<panda::JSValueRef>, const panda::Local<panda::JSValueRef> [], int32_t, void*);
template<typename T>
using MemberFunctionCallback = panda::Local<panda::JSValueRef>(T::*)(
    panda::EcmaVM*, panda::Local<panda::JSValueRef>, const panda::Local<panda::JSValueRef> [], int32_t, void*);
template<typename T>
using MemberFunctionGetCallback = panda::Local<panda::JSValueRef>(T::*)(
    panda::EcmaVM*, panda::Local<panda::JSValueRef>, const panda::Local<panda::JSValueRef> [], int32_t, void*);
template<typename T>
using MemberFunctionSetCallback = panda::Local<panda::JSValueRef>(T::*)(
    panda::EcmaVM*, panda::Local<panda::JSValueRef>, const panda::Local<panda::JSValueRef> [], int32_t, void*);
using ExoticGetterCallback = int;
using ExoticSetterCallback = int;
using ExoticHasPropertyCallback = int;
using ExoticIsArrayCallback = int;

constexpr const JavascriptEngine cCurrentJSEngine = JavascriptEngine::ARK;

#else
#error "No engine selected"
#endif

using JSFunctionCallback = void (*)(const OHOS::Ace::Framework::JSCallbackInfo&);
template<typename T>
using JSMemberFunctionCallback = void (T::*)(const OHOS::Ace::Framework::JSCallbackInfo&);
template<typename T>
using JSDestructorCallback = void (*)(T* instance);
template<typename T>
using JSGCMarkCallback = void (*)(T* instance, const OHOS::Ace::Framework::JSGCMarkCallbackInfo&);

#endif
