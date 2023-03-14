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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_BINDINGS
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_BINDINGS

#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "frameworks/bridge/common/utils/class_utils.h"
#include "frameworks/bridge/common/utils/function_traits.h"
#include "frameworks/bridge/declarative_frontend/engine/bindings_implementation.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_value_conversions.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

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

namespace OHOS::Ace::Framework {

extern std::vector<JSClassID> g_classIds;

void* UnwrapAny(JSValueConst val);

template<typename T>
JSValue Wrap(JSValueConst newTarget, T* thisPtr);

template<typename T>
JSValue Wrap(T* thisPtr);

template<typename T>
T* Unwrap(JSValueConst val);

template<typename C>
class QJSKlass {
    using ThisJSClass = JSClassImpl<C, QJSKlass>;

public:
    QJSKlass() = delete;
    ~QJSKlass() = default;

    static void Declare(const char* name);

    template<typename R, typename... Args>
    static void Method(const char* name, R (C::*func)(Args...), int id);

    template<typename Base, typename R, typename... Args>
    static void Method(const char* name, R (Base::*func)(Args...), int id);

    static void CustomMethod(const char* name, FunctionCallback cb);

    template<typename T>
    static void CustomMethod(const char* name, MemberFunctionCallback<T> cb, int id);

    template<typename T>
    static void CustomMethod(const char* name, JSMemberFunctionCallback<T> callback, int id);

    static void CustomProperty(const char* name, FunctionGetCallback getter, FunctionSetCallback setter);

    template<typename T>
    static void CustomProperty(const char* name, MemberFunctionGetCallback<T> cb, int getterId, int setterId);

    template<typename T>
    static void CustomProperty(const char* name, JSMemberFunctionCallback<T> callback, int getterId, int setterId);

    template<typename R, typename... Args>
    static void StaticMethod(const char* name, R (*func)(Args...), int id);

    static void StaticMethod(const char* name, JSFunctionCallback func, int id);

    static void CustomStaticMethod(const char* name, FunctionCallback cb);

    static void ExoticGetter(ExoticGetterCallback callback);
    static void ExoticSetter(ExoticSetterCallback callback);
    static void ExoticHasProperty(ExoticHasPropertyCallback callback);
    static void ExoticIsArray(ExoticIsArrayCallback callback);

    template<typename... Args>
    static void Bind(
        BindingTarget target, JSDestructorCallback<C> dtor = nullptr, JSGCMarkCallback<C> gcMark = nullptr);

    static void Bind(BindingTarget target, FunctionCallback ctor);
    static void Bind(
        BindingTarget target, JSFunctionCallback ctor, JSDestructorCallback<C> dtor, JSGCMarkCallback<C> gcMark);

    template<typename Base>
    static void Inherit();

    static C* Unwrap(JSValueConst val);

    static JSValue NewObjectProtoClass(JSValue proto);

    template<typename... Args>
    static JSValue NewInstance(Args... args);
    static JSValue GetProto();

    static int NumberOfInstances();

private:
    DEFINE_HAS_METHOD(QjsDestructor)
    DEFINE_HAS_METHOD(QjsGcMark)

    static void BindInternal(BindingTarget target, FunctionCallback ctor, size_t length);

    template<typename T>
    static JSValue InternalMemberFunctionCallback(
        JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv, int magic);
    template<typename T>
    static JSValue InternalMemberFunctionGetCallback(
            JSContext* ctx, JSValueConst thisObj, int magic);
    template<typename T>
    static JSValue InternalMemberFunctionSetCallback(
            JSContext* ctx, JSValueConst thisObj, JSValueConst argv, int magic);
    template<typename T>
    static JSValue InternalJSMemberFunctionCallback(
        JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv, int magic);
    template<typename T>
    static JSValue InternalJSMemberFunctionGetCallback(
            JSContext* ctx, JSValueConst thisObj, int magic);
    template<typename T>
    static JSValue InternalJSMemberFunctionSetCallback(
            JSContext* ctx, JSValueConst thisObj, JSValueConst argv, int magic);

    template<typename Class, typename R, typename... Args>
    static JSValue MethodCallback(JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv, int magic);

    template<typename Class, typename R, typename... Args>
    static JSValue StaticMethodCallback(JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv, int magic);

    static JSValue JSStaticMethodCallback(
        JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv, int magic);

    template<typename... Args>
    static JSValue InternalConstructor(JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv);

    static JSValue ConstructorInterceptor(JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv);
    static JSValue JSConstructorInterceptor(JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv);

    static void Finalizer(JSRuntime* rt, JSValue val);
    static void GcMark(JSRuntime* rt, JSValueConst val, JS_MarkFunc* markFunc);

    static thread_local JSValue proto_;
    static thread_local std::unordered_map<std::string, JSCFunctionListEntry*> functions_;
    static thread_local std::unordered_map<std::string, JSCFunctionListEntry*> staticFunctions_;
    static thread_local JSClassID classId_;
    static thread_local JSClassExoticMethods exoticMethods_;
    static thread_local JSClassDef classDefinitions_;
    static thread_local FunctionCallback constructor_;
    static thread_local JSFunctionCallback jsConstructor_;
    static thread_local JSDestructorCallback<C> jsDestructor_;
    static thread_local JSGCMarkCallback<C> gcMark_;

    template<typename U>
    friend class QJSKlass;
}; // namespace OHOS::Ace::Framework

template<typename T>
using JSClass = JSClassImpl<T, QJSKlass>;

} // namespace OHOS::Ace::Framework

#include "qjs_bindings.inl"

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_BINDINGS
