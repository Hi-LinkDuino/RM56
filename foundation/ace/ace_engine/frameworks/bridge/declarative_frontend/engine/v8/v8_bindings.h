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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_BINDINGS_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_BINDINGS_H

#include "third_party/v8/include/v8.h"
#include "v8_value_conversions.h"

#include <mutex>

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_utils.h"

namespace __detail__ {

template<typename... Types>
std::tuple<Types...> ToTuple(const v8::FunctionCallbackInfo<v8::Value>& argv)
{
    int index = 0;
    return {
        OHOS::Ace::Framework::V8ValueConvertor::fromV8Value<Types>(argv[index++])...,
    };
}

template<typename T>
bool ValidateArgument(const v8::FunctionCallbackInfo<v8::Value>& argv, const std::string& className,
    const std::string& methodName, int index)
{
    std::string errorSuffix;
    if constexpr (is_signed_int_v<T>) {
        if (argv[index]->IsInt32()) {
            return true;
        } else {
            errorSuffix = " is not an integer!";
        }
    } else if constexpr (std::is_unsigned_v<T>) {
        if (argv[index]->IsUint32()) {
            return true;
        } else {
            errorSuffix = " is not an unsigned integer!";
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        if (argv[index]->IsNumber()) {
            return true;
        } else {
            errorSuffix = " is not a floating point number!";
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (argv[index]->IsString()) {
            return true;
        } else {
            errorSuffix = " is not a string!";
        }
    } else {
        errorSuffix = " has unhandled type! ";
    }

    std::string errorMessage = className + "::" + methodName + " Argument " + std::to_string(index + 1) + errorSuffix;

    argv.GetReturnValue().Set(OHOS::Ace::Framework::V8Utils::ThrowException("%s (provided argument is %s)",
        errorMessage.c_str(), OHOS::Ace::Framework::V8Utils::ValueTypeAsString(argv[index]).c_str()));
    return false;
}

template<typename... Types>
bool ValidateArguments(const v8::FunctionCallbackInfo<v8::Value>& info, const char* className, const char* methodName)
{
    if ((size_t)info.Length() < sizeof...(Types)) {
        info.GetReturnValue().Set(OHOS::Ace::Framework::V8Utils::ThrowException(className, "::", methodName,
            " requires ", sizeof...(Types), " arguments but ", info.Length(), " were provided!"));
        return false;
    }
    int index = 0;
    return (ValidateArgument<Types>(info, className, methodName, index++) && ...);
}

}; // namespace __detail__

namespace OHOS::Ace::Framework {

template<typename C>
class V8Class {
public:
    using ThisJSClass = JSClassImpl<C, V8Class>;

    V8Class() = delete;

    static void Declare(const char* name);

    template<typename Base, typename R, typename... Args>
    static void Method(const char* name, R (Base::*func)(Args...), int id);
    template<typename T>
    static void CustomMethod(const char* name, MemberFunctionCallback<T> callback, int id);
    static void CustomMethod(const char* name, FunctionCallback callback);
    template<typename T>
    static void CustomMethod(const char* name, JSMemberFunctionCallback<T> callback, int id);

    template<typename T>
    static void CustomProperty(const char* name, MemberFunctionGetCallback<T> callback,
        int getterId, int setterId);
    static void CustomProperty(const char* name, FunctionGetCallback getter, FunctionSetCallback setter);
    template<typename T>
    static void CustomProperty(const char* name, JSMemberFunctionCallback<T> callback,
        int getterId, int setterId);

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

    static v8::Local<v8::FunctionTemplate> GetFunctionTemplate();

    static std::unordered_map<std::string, v8::Local<v8::FunctionTemplate>>& GetStaticPropertyNames();

    static v8::Local<v8::Object> NewInstance();

private:
    template<typename T, typename... Args>
    static void InternalMemberFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info);

    template<typename Class, typename R, typename... Args>
    static void MethodCallback(const v8::FunctionCallbackInfo<v8::Value>& info);

    template<typename Class, typename R, typename... Args>
    static void JSMethodCallback(const v8::FunctionCallbackInfo<v8::Value>& info);

    template<typename R, typename... Args>
    static void StaticMethodCallback(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void JSStaticMethodCallback(const v8::FunctionCallbackInfo<v8::Value>& info);

    template<typename... Args>
    static void InternalConstructor(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void ConstructorInterceptor(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void JSConstructorInterceptor(const v8::FunctionCallbackInfo<v8::Value>& info);
    static bool CheckIfConstructCall(const v8::FunctionCallbackInfo<v8::Value>& info);

    // functionTemplate corresponding to isolate
    static thread_local std::unordered_map<v8::Isolate*, v8::Persistent<v8::FunctionTemplate>> functionTemplates_;
    static thread_local std::unordered_map<std::string, v8::Local<v8::FunctionTemplate>> staticPropertyNames_;
    static thread_local std::mutex mutex_;
    static thread_local FunctionCallback constructor_;
    static thread_local JSFunctionCallback jsConstructor_;
    static thread_local JSDestructorCallback<C> jsDestructor_;
    static thread_local JSGCMarkCallback<C> gcMark_;

    friend class V8Wrapper;
};

template<typename T>
using JSClass = JSClassImpl<T, V8Class>;

}; // namespace OHOS::Ace::Framework

#include "v8_bindings.inl"

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_BINDINGS_H
