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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_BINDINGS_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_BINDINGS_H

#include "ecmascript/napi/include/jsnapi.h"

#include "frameworks/bridge/declarative_frontend/engine/bindings_implementation.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_value_conversions.h"

namespace __detail__ {

template<typename... Types>
std::tuple<Types...> ToTuple(const panda::Local<panda::JSValueRef> argv[])
{
    int index = 0;
    return {
        OHOS::Ace::Framework::JsiValueConvertor::fromJsiValue<Types>(argv[index++])...,
    };
}

}; // namespace __detail__

namespace OHOS::Ace::Framework {

template<typename C>
class JsiClass {
public:
    using ThisJSClass = JSClassImpl<C, JsiClass>;

    JsiClass() = delete;

    static void Declare(const char* name);

    template<typename Base, typename R, typename... Args>
    static void Method(const char* name, R (Base::*func)(Args...), int id);

    template<typename T>
    static void CustomMethod(const char* name, MemberFunctionCallback<T> callback, int id);

    static void CustomMethod(const char* name, FunctionCallback callback);

    template<typename T>
    static void CustomMethod(const char* name, JSMemberFunctionCallback<T> callback, int id);

    template<typename T>
    static void CustomProperty(const char* name, MemberFunctionGetCallback<T> callback, int getterId, int setterId);

    static void CustomProperty(const char* name, FunctionGetCallback getter, FunctionSetCallback setter);

    template<typename T>
    static void CustomProperty(const char* name, JSMemberFunctionCallback<T> callback, int getterId, int setterId);

    template<typename R, typename... Args>
    static void StaticMethod(const char* name, R (*func)(Args...), int id);

    static void StaticMethod(const char* name, JSFunctionCallback callback, int id);

    static void CustomStaticMethod(const char* name, FunctionCallback callback);

    template<typename T>
    static void StaticConstant(const char* name, T val);

    static void Bind(BindingTarget t, FunctionCallback ctor);

    static void Bind(BindingTarget t, JSFunctionCallback ctor, JSDestructorCallback<C> dtor = nullptr,
        JSGCMarkCallback<C> gcMark = nullptr);

    template<typename... Args>
    static void Bind(BindingTarget t, JSDestructorCallback<C> dtor = nullptr, JSGCMarkCallback<C> gcMark = nullptr);

    template<typename Base>
    static void Inherit();

    static std::unordered_map<std::string, panda::Global<panda::FunctionRef>>& GetCustomFunctions();

    static std::unordered_map<std::string, panda::Global<panda::FunctionRef>>& GetStaticFunctions();

    static panda::Local<panda::JSValueRef> NewInstance();

private:
    template<typename T, typename... Args>
    static panda::Local<panda::JSValueRef> InternalMemberFunctionCallback(panda::ecmascript::EcmaVM* vm,
        panda::Local<panda::JSValueRef> thisObj, const panda::Local<panda::JSValueRef> argv[], int32_t argc,
        void* data);

    template<typename T>
    static panda::Local<panda::JSValueRef> InternalJSMemberFunctionCallback(panda::EcmaVM* vm,
        panda::Local<panda::JSValueRef> thisObj, const panda::Local<panda::JSValueRef> argv[], int32_t argc,
        void* data);

    template<typename Class, typename R, typename... Args>
    static panda::Local<panda::JSValueRef> MethodCallback(panda::ecmascript::EcmaVM* vm,
        panda::Local<panda::JSValueRef> thisObj, const panda::Local<panda::JSValueRef> argv[], int32_t argc,
        void* data);

    template<typename Class, typename R, typename... Args>
    static panda::Local<panda::JSValueRef> JSMethodCallback(panda::ecmascript::EcmaVM* vm,
        panda::Local<panda::JSValueRef> thisObj, const panda::Local<panda::JSValueRef> argv[], int32_t argc,
        void* data);

    template<typename R, typename... Args>
    static panda::Local<panda::JSValueRef> StaticMethodCallback(panda::ecmascript::EcmaVM* vm,
        panda::Local<panda::JSValueRef> thisObj, const panda::Local<panda::JSValueRef> argv[], int32_t argc,
        void* data);

    static panda::Local<panda::JSValueRef> JSStaticMethodCallback(panda::ecmascript::EcmaVM* vm,
        panda::Local<panda::JSValueRef> thisObj, const panda::Local<panda::JSValueRef> argv[], int32_t argc,
        void* data);

    template<typename... Args>
    static panda::Local<panda::JSValueRef> InternalConstructor(panda::ecmascript::EcmaVM* vm,
        panda::Local<panda::JSValueRef> thisObj, panda::Local<panda::JSValueRef> newTarget,
        const panda::Local<panda::JSValueRef> argv[], int32_t argc, void* data);

    static panda::Local<panda::JSValueRef> ConstructorInterceptor(panda::ecmascript::EcmaVM* vm,
        panda::Local<panda::JSValueRef> thisObj, panda::Local<panda::JSValueRef> newTarget,
        const panda::Local<panda::JSValueRef> argv[], int32_t argc, void* data);

    static void DestructorInterceptor(void* nativePtr, void* data);

    static panda::Local<panda::JSValueRef> JSConstructorInterceptor(panda::ecmascript::EcmaVM* vm,
        panda::Local<panda::JSValueRef> thisObj, panda::Local<panda::JSValueRef> newTarget,
        const panda::Local<panda::JSValueRef> argv[], int32_t argc, void* data);

    static bool CheckIfConstructCall(panda::ecmascript::EcmaVM* vm, panda::Local<panda::JSValueRef> thisObj,
        panda::Local<panda::JSValueRef> newTarget, const panda::Local<panda::JSValueRef> argv[], int32_t argc,
        void* data);

    static thread_local std::unordered_map<std::string, panda::Global<panda::FunctionRef>> staticFunctions_;
    static thread_local std::unordered_map<std::string, panda::Global<panda::FunctionRef>> customFunctions_;
    static thread_local std::unordered_map<std::string, panda::Global<panda::FunctionRef>> customGetFunctions_;
    static thread_local std::unordered_map<std::string, panda::Global<panda::FunctionRef>> customSetFunctions_;
    static thread_local FunctionCallback constructor_;
    static thread_local JSFunctionCallback jsConstructor_;
    static thread_local JSDestructorCallback<C> jsDestructor_;
    static thread_local JSGCMarkCallback<C> jsGcMark_;
    static thread_local std::string className_;
    static thread_local panda::Global<panda::FunctionRef> classFunction_;
    static thread_local std::vector<shared_ptr<int32_t>> functionIds_;
};

template<typename T>
using JSClass = JSClassImpl<T, JsiClass>;

}; // namespace OHOS::Ace::Framework

#include "jsi_bindings.inl"

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_BINDINGS_H
