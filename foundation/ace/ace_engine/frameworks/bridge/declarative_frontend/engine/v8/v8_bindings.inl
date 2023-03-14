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

#include <sstream>
#include <string>
#include <type_traits>

#include "v8_bindings.h"

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_function_destroy_helper.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_wrapper.h"

namespace OHOS::Ace::Framework {

template<typename C, bool = std::is_base_of_v<Referenced, C>>
struct MaybeReferenced;

template<typename C>
struct MaybeReferenced<C, false>
{
    static C* IncRef(C* ptr)
    {
        return ptr;
    }
    static void DecRef(C* ptr)
    {
        delete ptr;
    }
};

template<typename C>
struct MaybeReferenced<C, true>
{
    static C* IncRef(C* ptr)
    {
        if (ptr != nullptr) {
            ptr->IncRefCount();
        }
        return ptr;
    }
    static void DecRef(C* ptr)
    {
        if (ptr != nullptr) {
            ptr->DecRefCount();
        }
    }
};

template<typename C>
thread_local std::unordered_map<v8::Isolate*, v8::Persistent<v8::FunctionTemplate>> V8Class<C>::functionTemplates_;

template<typename C>
thread_local std::mutex V8Class<C>::mutex_;

template<typename C>
thread_local FunctionCallback V8Class<C>::constructor_ = nullptr;
template<typename C>
thread_local std::unordered_map<std::string, v8::Local<v8::FunctionTemplate>> V8Class<C>::staticPropertyNames_;

template<typename C>
thread_local JSFunctionCallback V8Class<C>::jsConstructor_ = nullptr;

template<typename C>
thread_local JSDestructorCallback<C> V8Class<C>::jsDestructor_ = nullptr;

template<typename C>
thread_local JSGCMarkCallback<C> V8Class<C>::gcMark_ = nullptr;

template<typename C>
void V8Class<C>::Declare(const char* name)
{
    staticPropertyNames_.clear();
    constructor_ = nullptr;
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    v8::Local<v8::FunctionTemplate> funcTemplate = v8::FunctionTemplate::New(isolate);
    funcTemplate->InstanceTemplate()->SetInternalFieldCount(1);
    auto destroyCallback = [](v8::Isolate* isolate) {
        std::lock_guard<std::mutex> lock(mutex_);
        functionTemplates_.erase(isolate);
    };
    V8FunctionDestroyCallbackHelper::AddDestroyCallback(destroyCallback, isolate);
    std::lock_guard<std::mutex> lock(mutex_);
    functionTemplates_[isolate].Empty();
    functionTemplates_[isolate].Reset(isolate, funcTemplate);
}

template<typename C>
template<typename Base, typename R, typename... Args>
void V8Class<C>::Method(const char* name, R (Base::*func)(Args...), int id)
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    std::lock_guard<std::mutex> lock(mutex_);
    auto& functionTemplate = functionTemplates_[isolate];
    functionTemplate.Get(isolate)->PrototypeTemplate()->Set(isolate, name,
        v8::FunctionTemplate::New(isolate, MethodCallback<Base, R, Args...>, v8::Integer::New(isolate, id)));
}

template<typename C>
template<typename T>
void V8Class<C>::CustomMethod(const char* name, MemberFunctionCallback<T> callback, int id)
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    std::lock_guard<std::mutex> lock(mutex_);
    auto& functionTemplate = functionTemplates_[isolate];
    functionTemplate.Get(isolate)->PrototypeTemplate()->Set(isolate, name,
        v8::FunctionTemplate::New(isolate,
            InternalMemberFunctionCallback<T, const v8::FunctionCallbackInfo<v8::Value>&>,
            v8::Integer::New(isolate, id)));
}

template<typename C>
void V8Class<C>::CustomMethod(const char* name, FunctionCallback callback)
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    std::lock_guard<std::mutex> lock(mutex_);
    auto& functionTemplate = functionTemplates_[isolate];
    functionTemplate.Get(isolate)->PrototypeTemplate()->Set(
        isolate, name, v8::FunctionTemplate::New(isolate, callback));
}

template<typename C>
template<typename T>
void V8Class<C>::CustomMethod(const char* name, JSMemberFunctionCallback<T> callback, int id)
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    std::lock_guard<std::mutex> lock(mutex_);
    auto& functionTemplate = functionTemplates_[isolate];
    functionTemplate.Get(isolate)->PrototypeTemplate()->Set(isolate, name,
        v8::FunctionTemplate::New(
            isolate, InternalMemberFunctionCallback<T, const JSCallbackInfo&>, v8::Integer::New(isolate, id)));
}

template<typename C>
template<typename T>
void V8Class<C>::CustomProperty(const char* name, MemberFunctionGetCallback<T> callback,
    int getterId, int setterId)
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    std::lock_guard<std::mutex> lock(mutex_);
    auto& functionTemplate = functionTemplates_[isolate];

    functionTemplate.Get(isolate)->PrototypeTemplate()->SetAccessorProperty(
        v8::Local<v8::Name>::Cast(V8ValueConvertor::toV8Value<std::string>(name)),
        v8::FunctionTemplate::New(isolate,
            InternalMemberFunctionCallback<T, const v8::FunctionCallbackInfo<v8::Value>&>,
            v8::Integer::New(isolate, getterId)),
        v8::FunctionTemplate::New(isolate,
            InternalMemberFunctionCallback<T, const v8::FunctionCallbackInfo<v8::Value>&>,
            v8::Integer::New(isolate, setterId)));
}

template<typename C>
void V8Class<C>::CustomProperty(const char* name, FunctionGetCallback getter, FunctionSetCallback setter)
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    std::lock_guard<std::mutex> lock(mutex_);
    auto& functionTemplate = functionTemplates_[isolate];
    functionTemplate.Get(isolate)->PrototypeTemplate()->SetAccessorProperty(
        v8::Local<v8::Name>::Cast(V8ValueConvertor::toV8Value<std::string>(name)),
        v8::FunctionTemplate::New(isolate, getter),
        v8::FunctionTemplate::New(isolate, setter));
}

template<typename C>
template<typename T>
void V8Class<C>::CustomProperty(const char* name, JSMemberFunctionCallback<T> callback,
    int getterId, int setterId)
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    std::lock_guard<std::mutex> lock(mutex_);
    auto& functionTemplate = functionTemplates_[isolate];
    functionTemplate.Get(isolate)->PrototypeTemplate()->SetAccessorProperty(
        v8::Local<v8::Name>::Cast(V8ValueConvertor::toV8Value<std::string>(name)),
        v8::FunctionTemplate::New(
            isolate, InternalMemberFunctionCallback<T, const JSCallbackInfo&>, v8::Integer::New(isolate, getterId)),
        v8::FunctionTemplate::New(
            isolate, InternalMemberFunctionCallback<T, const JSCallbackInfo&>, v8::Integer::New(isolate, setterId)));
}

template<typename C>
template<typename R, typename... Args>
void V8Class<C>::StaticMethod(const char* name, R (*func)(Args...), int id)
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    v8::Local<v8::FunctionTemplate> functiontemplate =
        v8::FunctionTemplate::New(isolate, StaticMethodCallback<R, Args...>, v8::Integer::New(isolate, id));
    staticPropertyNames_.emplace(name, functiontemplate);
    std::lock_guard<std::mutex> lock(mutex_);
    functionTemplates_[isolate].Get(isolate)->Set(isolate, name, functiontemplate);
}

template<typename C>
void V8Class<C>::StaticMethod(const char* name, JSFunctionCallback func, int id)
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    v8::Local<v8::FunctionTemplate> functiontemplate =
        v8::FunctionTemplate::New(isolate, JSStaticMethodCallback, v8::Integer::New(isolate, id));
    staticPropertyNames_.emplace(name, functiontemplate);
    std::lock_guard<std::mutex> lock(mutex_);
    functionTemplates_[isolate].Get(isolate)->Set(isolate, name, functiontemplate);
}

template<typename C>
void V8Class<C>::CustomStaticMethod(const char* name, FunctionCallback callback)
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    v8::Local<v8::FunctionTemplate> functiontemplate = v8::FunctionTemplate::New(isolate, callback);
    staticPropertyNames_.emplace(name, functiontemplate);
    std::lock_guard<std::mutex> lock(mutex_);
    functionTemplates_[isolate].Get(isolate)->Set(isolate, name, functiontemplate);
}

template<typename C>
template<typename T>
void V8Class<C>::StaticConstant(const char* name, T val)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    std::lock_guard<std::mutex> lock(mutex_);
    functionTemplates_[isolate].Get(isolate)->Set(
        v8::Local<v8::Name>::Cast(V8ValueConvertor::toV8Value<std::string>(name)),
        V8ValueConvertor::toV8Value(val), v8::PropertyAttribute::ReadOnly);
}

template<typename C>
void V8Class<C>::Bind(BindingTarget t, FunctionCallback ctor)
{
    constructor_ = ctor;
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    v8::HandleScope scp(isolate);
    std::lock_guard<std::mutex> lock(mutex_);
    functionTemplates_[isolate].Get(isolate)->SetCallHandler(ConstructorInterceptor);
    t->Set(v8::String::NewFromUtf8(isolate, ThisJSClass::JSName()).ToLocalChecked(),
        functionTemplates_[isolate].Get(isolate));
}

template<typename C>
void V8Class<C>::Bind(
    BindingTarget t, JSFunctionCallback ctor, JSDestructorCallback<C> dtor, JSGCMarkCallback<C> gcMark)
{
    jsConstructor_ = ctor;
    jsDestructor_ = dtor;
    gcMark_ = gcMark;
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    std::lock_guard<std::mutex> lock(mutex_);
    functionTemplates_[isolate].Get(isolate)->SetCallHandler(JSConstructorInterceptor);
    t->Set(v8::String::NewFromUtf8(isolate, ThisJSClass::JSName()).ToLocalChecked(),
        functionTemplates_[isolate].Get(isolate));
}

template<typename C>
template<typename... Args>
void V8Class<C>::Bind(BindingTarget t, JSDestructorCallback<C> dtor, JSGCMarkCallback<C> gcMark)
{
    jsDestructor_ = dtor;
    gcMark_ = gcMark;
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    v8::HandleScope scp(isolate);
    std::lock_guard<std::mutex> lock(mutex_);
    functionTemplates_[isolate].Get(isolate)->SetCallHandler(InternalConstructor<Args...>);
    t->Set(v8::String::NewFromUtf8(isolate, ThisJSClass::JSName()).ToLocalChecked(),
        functionTemplates_[isolate].Get(isolate));
}

template<typename C>
template<typename Base>
void V8Class<C>::Inherit()
{
    GetFunctionTemplate()->Inherit(V8Class<Base>::GetFunctionTemplate());

    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    auto& baseStaticPropertyNames = V8Class<Base>::GetStaticPropertyNames();
    auto& staticPropertyNames = GetStaticPropertyNames();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [property, functionTemplate] : baseStaticPropertyNames) {
        if (staticPropertyNames.find(property) != staticPropertyNames.end()) {
            continue;
        }
        GetFunctionTemplate()->Set(isolate, property.c_str(), functionTemplate);
        staticPropertyNames.emplace(property, functionTemplate);
    }
}

template<typename C>
v8::Local<v8::FunctionTemplate> V8Class<C>::GetFunctionTemplate()
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    return functionTemplates_[isolate].Get(isolate);
}

template<typename C>
std::unordered_map<std::string, v8::Local<v8::FunctionTemplate>>& V8Class<C>::GetStaticPropertyNames()
{
    return staticPropertyNames_;
}

template<typename C>
template<typename T, typename... Args>
void V8Class<C>::InternalMemberFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scp(isolate);
    v8::Context::Scope contextScope(isolate->GetCurrentContext());
    v8::Local<v8::Object> thisObj = info.This();
    C* ptr = static_cast<C*>(thisObj->GetAlignedPointerFromInternalField(0));
    T* instance = static_cast<T*>(ptr);
    int index = V8ValueConvertor::fromV8Value<int>(info.Data());
    auto binding = ThisJSClass::GetFunctionBinding(index);
    if (binding == nullptr) {
        LOGW("Can't find function in function map, index: %{public}d, JSClass: %{public}s",
            index, ThisJSClass::JSName());
        return;
    }
    LOGD("InternalMemberFunctionCallback: Calling %{public}s::%{public}s", ThisJSClass::JSName(), binding->Name());
    auto fnPtr = static_cast<FunctionBinding<T, void, Args...>*>(binding)->Get();
    (instance->*fnPtr)(info);
}

template<typename C>
template<typename Class, typename R, typename... Args>
void V8Class<C>::MethodCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scp(isolate);
    v8::Context::Scope contextScope(isolate->GetCurrentContext());

    v8::Local<v8::Object> thisObj = info.This();
    C* ptr = static_cast<C*>(thisObj->GetAlignedPointerFromInternalField(0));
    Class* instance = static_cast<Class*>(ptr);
    int index = V8ValueConvertor::fromV8Value<int>(info.Data());
    auto binding = ThisJSClass::GetFunctionBinding(index);
    if (binding == nullptr) {
        LOGW("Can't find function in function map, index: %{public}d, JSClass: %{public}s",
            index, ThisJSClass::JSName());
        return;
    }
    LOGD("Calling %{public}s::%{public}s", ThisJSClass::JSName(), binding->Name());
    bool strictTypeCheck = binding->Options() & MethodOptions::STRICT_TYPE_CHECK;

    if (strictTypeCheck) {
        if (!__detail__::ValidateArguments<Args...>(info, ThisJSClass::JSName(), binding->Name())) {
            return;
        }
    }

    auto fnPtr = static_cast<FunctionBinding<Class, R, Args...>*>(binding)->Get();
    auto tuple = __detail__::ToTuple<std::decay_t<Args>...>(info);

    bool returnSelf = binding->Options() & MethodOptions::RETURN_SELF;

    constexpr bool isVoid = std::is_void_v<R>;
    constexpr bool hasArguments = sizeof...(Args) != 0;

    if constexpr (isVoid && hasArguments) {
        // void C::MemberFunction(Args...)
        FunctionUtils::CallMemberFunction(instance, fnPtr, tuple);
        if (returnSelf) {
            info.GetReturnValue().Set(info.This());
        } else {
            info.GetReturnValue().SetUndefined();
        }
    } else if constexpr (isVoid && !hasArguments) {
        // void C::MemberFunction()
        (instance->*fnPtr)();
        if (returnSelf) {
            info.GetReturnValue().Set(info.This());
        } else {
            info.GetReturnValue().SetUndefined();
        }
    } else if constexpr (!isVoid && hasArguments) {
        // R C::MemberFunction(Args...)
        auto result = FunctionUtils::CallMemberFunction(instance, fnPtr, tuple);
        info.GetReturnValue().Set(V8ValueConvertor::toV8Value<R>(result));
    } else if constexpr (!isVoid && !hasArguments) {
        // R C::MemberFunction()
        auto result = (instance->*fnPtr)();
        info.GetReturnValue().Set(V8ValueConvertor::toV8Value<R>(result));
    }
}

template<typename C>
template<typename Class, typename R, typename... Args>
void V8Class<C>::JSMethodCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scp(isolate);
    v8::Context::Scope contextScope(isolate->GetCurrentContext());

    v8::Local<v8::Object> thisObj = info.This();
    C* ptr = static_cast<C*>(thisObj->GetAlignedPointerFromInternalField(0));
    Class* instance = static_cast<Class*>(ptr);
    int index = V8ValueConvertor::fromV8Value<int>(info.Data());
    auto binding = ThisJSClass::GetFunctionBinding(index);
    if (binding == nullptr) {
        LOGW("Can't find function in function map, index: %{public}d, JSClass: %{public}s",
            index, ThisJSClass::JSName());
        return;
    }
    LOGD("Calling %{public}s::%{public}s", ThisJSClass::JSName(), binding->Name());
    bool strictTypeCheck = binding->Options() & MethodOptions::STRICT_TYPE_CHECK;

    if (strictTypeCheck) {
        if (!__detail__::ValidateArguments<Args...>(info, ThisJSClass::JSName(), binding->Name())) {
            return;
        }
    }

    auto fnPtr = static_cast<FunctionBinding<Class, R, Args...>*>(binding)->Get();
    (instance->*fnPtr)(info);
}

template<typename C>
template<typename R, typename... Args>
void V8Class<C>::StaticMethodCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scp(isolate);
    v8::Context::Scope contextScope(isolate->GetCurrentContext());

    int index = V8ValueConvertor::fromV8Value<int>(info.Data());
    auto binding = ThisJSClass::GetFunctionBinding(index);
    if (binding == nullptr) {
        LOGW("Can't find function in function map, index: %{public}d, JSClass: %{public}s",
            index, ThisJSClass::JSName());
        return;
    }
    LOGD("Calling %{public}s::%{public}s", ThisJSClass::JSName(), binding->Name());
    bool strictTypeCheck = binding->Options() & MethodOptions::STRICT_TYPE_CHECK;

    if (strictTypeCheck) {
        if (!__detail__::ValidateArguments<Args...>(info, ThisJSClass::JSName(), binding->Name())) {
            return;
        }
    }

    auto fnPtr = static_cast<StaticFunctionBinding<R, Args...>*>(binding)->Get();
    auto tuple = __detail__::ToTuple<std::decay_t<Args>...>(info);

    constexpr bool isVoid = std::is_void_v<R>;
    constexpr bool hasArguments = sizeof...(Args) != 0;

    if constexpr (isVoid && hasArguments) {
        // void C::MemberFunction(Args...)
        FunctionUtils::CallStaticMemberFunction(fnPtr, tuple);
        info.GetReturnValue().SetUndefined();
    } else if constexpr (isVoid && !hasArguments) {
        // void C::MemberFunction()
        fnPtr();
        info.GetReturnValue().SetUndefined();
    } else if constexpr (!isVoid && hasArguments) {
        // R C::MemberFunction(Args...)
        auto result = FunctionUtils::CallStaticMemberFunction(fnPtr, tuple);
        info.GetReturnValue().Set(V8ValueConvertor::toV8Value<R>(result));
    } else if constexpr (!isVoid && !hasArguments) {
        // R C::MemberFunction()
        auto result = fnPtr();
        info.GetReturnValue().Set(V8ValueConvertor::toV8Value<R>(result));
    }
}

template<typename C>
void V8Class<C>::JSStaticMethodCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scp(isolate);
    v8::Context::Scope contextScope(isolate->GetCurrentContext());
    int index = V8ValueConvertor::fromV8Value<int>(info.Data());
    auto binding = ThisJSClass::GetFunctionBinding(index);
    if (binding == nullptr) {
        LOGW("Can't find function in function map, index: %{public}d, JSClass: %{public}s",
            index, ThisJSClass::JSName());
        return;
    }
    LOGD("Calling %{public}s::%{public}s", ThisJSClass::JSName(), binding->Name());

    auto fnPtr = static_cast<StaticFunctionBinding<void, const JSCallbackInfo&>*>(binding)->Get();

    fnPtr(info);
}

template<typename C>
template<typename... Args>
void V8Class<C>::InternalConstructor(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scp(isolate);
    v8::Context::Scope contextScope(isolate->GetCurrentContext());

    if (!info.IsConstructCall()) {
        std::stringstream ss;
        ss << "Constructor called for ";
        ss << ThisJSClass::JSName();
        ss << " without 'new'!";
        isolate->ThrowException(v8::String::NewFromUtf8(isolate, ss.str().c_str()).ToLocalChecked());
        return;
    }

    auto tuple = __detail__::ToTuple<std::decay_t<Args>...>(info);
    C* instance = FunctionUtils::ConstructFromTuple<C>(tuple);

    V8Wrapper* jsObj = V8Wrapper::New(info.This(), instance);
    info.GetReturnValue().Set(jsObj->Get());
}

template<typename C>
bool V8Class<C>::CheckIfConstructCall(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (!info.IsConstructCall()) {
        std::stringstream ss;
        ss << "Constructor called for ";
        ss << ThisJSClass::JSName();
        ss << " without 'new'!";
        v8::Isolate* isolate = info.GetIsolate();
        isolate->ThrowException(v8::String::NewFromUtf8(isolate, ss.str().c_str()).ToLocalChecked());
        return false;
    }

    return true;
}

template<typename C>
void V8Class<C>::ConstructorInterceptor(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scp(isolate);
    v8::Context::Scope contextScope(isolate->GetCurrentContext());

    if (!CheckIfConstructCall(info)) {
        return;
    }

    constructor_(info);
}
template<typename C>
void V8Class<C>::JSConstructorInterceptor(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scp(isolate);
    v8::Context::Scope contextScope(isolate->GetCurrentContext());

    if (!CheckIfConstructCall(info)) {
        return;
    }

    jsConstructor_(info);
}

template<typename C>
v8::Local<v8::Object> V8Class<C>::NewInstance()
{
    v8::Isolate* isolate = V8DeclarativeEngineInstance::GetV8Isolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scp(isolate);
    v8::Context::Scope contextScope(isolate->GetCurrentContext());

    auto context = isolate->GetCurrentContext();
    std::lock_guard<std::mutex> lock(mutex_);
    v8::MaybeLocal<v8::Function> func = functionTemplates_[isolate].Get(isolate)->GetFunction(context);
    if (func.IsEmpty()) {
        return v8::Local<v8::Object>();
    }
    v8::MaybeLocal<v8::Object> val = func.ToLocalChecked()->NewInstance(context);
    if (val.IsEmpty()) {
        return v8::Local<v8::Object>();
    }
    return val.ToLocalChecked();
}

} // namespace OHOS::Ace::Framework
