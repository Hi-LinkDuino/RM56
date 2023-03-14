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

#include "v8_types.h"

#include "frameworks/base/utils/string_utils.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_wrapper.h"

namespace OHOS::Ace::Framework {

template<typename T>
V8Type<T>::V8Type()
{}

template<typename T>
V8Type<T>::V8Type(v8::Local<T> val) : handle_(V8DeclarativeEngineInstance::GetV8Isolate(), val)
{}

template<typename T>
template<typename S>
V8Type<T>::V8Type(v8::Local<S> val) : handle_(V8DeclarativeEngineInstance::GetV8Isolate(), v8::Local<T>::Cast(val))
{}

template<typename T>
V8Type<T>::V8Type(CopyablePersistent<T> other) : handle_(other)
{}

template<typename T>
V8Type<T>::V8Type(const V8Type<T>& rhs) : handle_(rhs.handle_)
{}

template<typename T>
V8Type<T>::V8Type(V8Type<T>&& rhs) : handle_(std::move(rhs.handle_))
{
    rhs.handle_.Reset();
}

template<typename T>
V8Type<T>& V8Type<T>::operator=(const V8Type<T>& rhs)
{
    handle_ = rhs.handle_;
    return *this;
}

template<typename T>
V8Type<T>& V8Type<T>::operator=(V8Type<T>&& rhs)
{
    handle_ = std::move(rhs.handle_);
    rhs.handle_.Reset();
    return *this;
}

template<typename T>
V8Type<T>::~V8Type()
{
    handle_.Reset();
}

template<typename T>
void V8Type<T>::SetWeak()
{
    if (!handle_.IsEmpty())
        handle_.SetWeak();
}

template<typename T>
v8::Local<T> V8Type<T>::GetHandle() const
{
    return handle_.Get(V8DeclarativeEngineInstance::GetV8Isolate());
}

template<typename T>
bool V8Type<T>::IsEmpty() const
{
    return handle_.IsEmpty();
}

template<typename T>
bool V8Type<T>::IsWeak() const
{
    return handle_.IsWeak();
}

template<typename T>
void V8Type<T>::Reset()
{
    handle_.Reset();
}

template<typename T>
v8::Local<T> V8Type<T>::operator->() const
{
    return GetHandle();
}

template<typename T>
V8Type<T>::operator v8::Local<T>() const
{
    return GetHandle();
}

template<typename T>
V8Type<T> V8Type<T>::New()
{
    auto current = v8::Isolate::GetCurrent();
    return V8Type<T>(T::New(current));
}

template<typename T>
T* V8Object::Unwrap() const
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    if (GetHandle()->InternalFieldCount() < 1) {
        return nullptr;
    }
    return static_cast<T*>(GetHandle()->GetAlignedPointerFromInternalField(INSTANCE));
}

template<typename T>
void V8Object::Wrap(T* data) const
{
    GetHandle()->SetAlignedPointerInInternalField(INSTANCE, static_cast<void*>(data));
}

template<typename T>
T V8Value::ToNumber() const
{
    return V8ValueConvertor::fromV8Value<T>(GetHandle());
}

template<typename T>
void V8Object::SetProperty(const char* prop, T value) const
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    GetHandle()
        ->Set(context, V8ValueConvertor::toV8Value<std::string>(prop), V8ValueConvertor::toV8Value<T>(value))
        .ToChecked();
}

template<typename T>
void V8CallbackInfo::SetReturnValue(T* instance) const
{
    V8Wrapper* obj = V8Wrapper::New<T>(info_.This(), instance);
    info_.GetReturnValue().Set(obj->Get());
}

template<typename T>
void V8CallbackInfo::SetReturnValue(V8Ref<T> val) const
{
    info_.GetReturnValue().Set(val.Get().GetHandle());
}

template<typename... Args>
void V8Exception::Throw(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    isolate->ThrowException(v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked());
}

template<typename... Args>
void V8Exception::ThrowRangeError(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    isolate->ThrowException(v8::Exception::RangeError(v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked()));
}

template<typename... Args>
void V8Exception::ThrowReferenceError(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    isolate->ThrowException(v8::Exception::ReferenceError(v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked()));
}

template<typename... Args>
void V8Exception::ThrowSyntaxError(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    isolate->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked()));
}

template<typename... Args>
void V8Exception::ThrowTypeError(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked()));
}

} // namespace OHOS::Ace::Framework
