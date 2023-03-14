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

#include "qjs_types.h"

#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_unwrap_any.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_value_conversions.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace OHOS::Ace::Framework {
template<typename T>
void QJSObject::SetProperty(const char* prop, T value) const
{
    JS_SetPropertyStr(QJSContext::Current(), GetHandle(), prop, __detail__::toJSValue<T>(value));
}

template<typename U>
U* QJSObject::Unwrap() const
{
    return static_cast<U*>(UnwrapAny(GetHandle()));
}

template<typename U>
void QJSObject::Wrap(U* data) const
{
    // add registered class check
    JS_SetOpaque(GetHandle(), static_cast<void*>(data));
}

template<typename T>
T QJSValue::ToNumber() const
{
    return __detail__::fromJSValue<T>(GetHandle());
}

template<typename T>
void QJSCallbackInfo::SetReturnValue(T* instance) const
{
    retVal_ = instance;
}

template<typename T>
void QJSCallbackInfo::SetReturnValue(QJSRef<T> val) const
{
    retVal_ = JS_DupValue(nullptr, val.Get().GetHandle());
}

template<typename T>
void QJSGCMarkCallbackInfo::Mark(const QJSRef<T>& val) const
{
    JSValue jsval = val->GetHandle();
    JS_MarkValue(rt_, jsval, markFunc_);
}

template<typename... Args>
void QJSException::Throw(const char* format, Args... args)
{
    JS_ThrowInternalError(QJSContext::Current(), format, args...);
}

template<typename... Args>
void QJSException::ThrowRangeError(const char* format, Args... args)
{
    JS_ThrowRangeError(QJSContext::Current(), format, args...);
}

template<typename... Args>
void QJSException::ThrowReferenceError(const char* format, Args... args)
{
    JS_ThrowReferenceError(QJSContext::Current(), format, args...);
}

template<typename... Args>
void QJSException::ThrowSyntaxError(const char* format, Args... args)
{
    JS_ThrowSyntaxError(QJSContext::Current(), format, args...);
}

template<typename... Args>
void QJSException::ThrowTypeError(const char* format, Args... args)
{
    JS_ThrowTypeError(QJSContext::Current(), format, args...);
}
}; // namespace OHOS::Ace::Framework
