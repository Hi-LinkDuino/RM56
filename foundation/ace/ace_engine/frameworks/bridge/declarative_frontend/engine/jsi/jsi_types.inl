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

#include "frameworks/base/utils/string_utils.h"
#include "frameworks/bridge/js_frontend/engine/jsi/ark_js_runtime.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_ref.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_types.h"

namespace OHOS::Ace::Framework {

template<typename T>
JsiType<T>::JsiType()
{}

template<typename T>
JsiType<T>::JsiType(panda::Local<T> val)
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    handle_ = Global<T>(runtime->GetEcmaVm(), val);
}

template<typename T>
template<typename S>
JsiType<T>::JsiType(panda::Local<S> val)
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    handle_ = Global<T>(runtime->GetEcmaVm(), val);
}

template<typename T>
JsiType<T>::JsiType(panda::Global<T> other)
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    handle_ = Global<T>(runtime->GetEcmaVm(), other);
}

template<typename T>
JsiType<T>::JsiType(const JsiType<T>& rhs)
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    handle_ = Global<T>(runtime->GetEcmaVm(), rhs.handle_);
}

template<typename T>
JsiType<T>::JsiType(JsiType<T>&& rhs)
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    handle_ = Global<T>(runtime->GetEcmaVm(), rhs.handle_);
    rhs.handle_.FreeGlobalHandleAddr();
}

template<typename T>
JsiType<T>& JsiType<T>::operator=(const JsiType<T>& rhs)
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    handle_ = Global<T>(runtime->GetEcmaVm(), rhs.handle_);
    return *this;
}

template<typename T>
JsiType<T>& JsiType<T>::operator=(JsiType<T>&& rhs)
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    handle_ = Global<T>(runtime->GetEcmaVm(), rhs.handle_);
    rhs.handle_.FreeGlobalHandleAddr();
    return *this;
}

template<typename T>
JsiType<T>::~JsiType()
{
    handle_.FreeGlobalHandleAddr();
}

template<typename T>
void JsiType<T>::SetWeak()
{
    if (!handle_.IsEmpty()) {
        handle_.SetWeak();
    }
}

template<typename T>
panda::Local<T> JsiType<T>::GetHandle() const
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    return handle_.ToLocal(runtime->GetEcmaVm());
}

template<typename T>
bool JsiType<T>::IsEmpty() const
{
    return handle_.IsEmpty();
}

template<typename T>
bool JsiType<T>::IsWeak() const
{
     return handle_.IsWeak();
}

template<typename T>
void JsiType<T>::Reset()
{
    handle_.FreeGlobalHandleAddr();
}

template<typename T>
panda::Local<T> JsiType<T>::operator->() const
{
    return GetHandle();
}

template<typename T>
JsiType<T>::operator panda::Local<T>() const
{
    return GetHandle();
}

template<typename T>
JsiType<T> JsiType<T>::New()
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    return JsiType<T>(T::New(runtime->GetEcmaVm()));
}

template<typename T>
T JsiValue::ToNumber() const
{
    return JsiValueConvertor::fromJsiValue<T>(GetHandle());
}

template<typename T>
T* JsiObject::Unwrap() const
{
    if (GetHandle()->GetNativePointerFieldCount() < 1) {
        return nullptr;
    }
    return static_cast<T*>(GetHandle()->GetNativePointerField(INSTANCE));
}

template<typename T>
void JsiObject::Wrap(T* data) const
{
    GetHandle()->SetNativePointerField(INSTANCE, static_cast<void*>(data));
}

template<typename T>
void JsiObject::SetProperty(const char* prop, T value) const
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    auto stringRef = panda::StringRef::NewFromUtf8(runtime->GetEcmaVm(), prop);
    GetHandle()->Set(runtime->GetEcmaVm(), stringRef, JsiValueConvertor::toJsiValue<T>(value));
}

template<typename T>
void JsiCallbackInfo::SetReturnValue(T* instance) const
{
    retVal_ = instance;
}

template<typename T>
void JsiCallbackInfo::SetReturnValue(JsiRef<T> val) const
{
    retVal_ = panda::Global<panda::JSValueRef>(vm_, val.Get().GetHandle());
}

template<typename... Args>
void JsiException::Throw(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    auto vm = runtime->GetEcmaVm();
    panda::JSNApi::ThrowException(vm, panda::Exception::Error(vm, panda::StringRef::NewFromUtf8(vm, str.c_str())));
}

template<typename... Args>
void JsiException::ThrowRangeError(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    auto vm = runtime->GetEcmaVm();
    panda::JSNApi::ThrowException(vm, panda::Exception::RangeError(vm, panda::StringRef::NewFromUtf8(vm, str.c_str())));
}

template<typename... Args>
void JsiException::ThrowReferenceError(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    auto vm = runtime->GetEcmaVm();
    panda::JSNApi::ThrowException(
        vm, panda::Exception::ReferenceError(vm, panda::StringRef::NewFromUtf8(vm, str.c_str())));
}

template<typename... Args>
void JsiException::ThrowSyntaxError(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    auto vm = runtime->GetEcmaVm();
    panda::JSNApi::ThrowException(
        vm, panda::Exception::SyntaxError(vm, panda::StringRef::NewFromUtf8(vm, str.c_str())));
}

template<typename... Args>
void JsiException::ThrowTypeError(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    auto vm = runtime->GetEcmaVm();
    panda::JSNApi::ThrowException(vm, panda::Exception::TypeError(vm, panda::StringRef::NewFromUtf8(vm, str.c_str())));
}

template<typename... Args>
void JsiException::ThrowEvalError(const char* format, Args... args)
{
    const std::string str = StringUtils::FormatString(format, args...);
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    auto vm = runtime->GetEcmaVm();
    panda::JSNApi::ThrowException(vm, panda::Exception::EvalError(vm, panda::StringRef::NewFromUtf8(vm, str.c_str())));
}

} // namespace OHOS::Ace::Framework
