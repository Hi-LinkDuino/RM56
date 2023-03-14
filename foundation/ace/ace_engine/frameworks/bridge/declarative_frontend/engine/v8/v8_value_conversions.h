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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_VALUE_CONVERSIONS_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_VALUE_CONVERSIONS_H

#include "third_party/v8/include/v8.h"

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_utils.h"

namespace __detail__ {

template<typename T>
static constexpr bool is_signed_int_v =
    std::is_integral<T>::value&& std::is_signed<T>::value && !std::is_same<T, bool>::value;

} // namespace __detail__

namespace OHOS::Ace::Framework {
namespace V8ValueConvertor {

template<typename T>
bool Validate(v8::Local<v8::Value> val)
{
    if constexpr (__detail__::is_signed_int_v<T>) {
        return val->IsInt32();
    } else if constexpr (std::is_unsigned_v<T>) {
        return val->IsUint32();
    } else if constexpr (std::is_floating_point_v<T>) {
        return val->IsNumber();
    } else if constexpr (std::is_same_v<T, std::string>) {
        return val->IsString();
    } else {
        LOGD("Unhandled type: %s!", V8Utils::ValueTypeAsString(val).c_str());
        return false;
    }
}

template<typename T>
T fromV8Value(v8::Local<v8::Value> val)
{
    static_assert(!std::is_const_v<T> && !std::is_reference_v<T>, //
        "Cannot convert values to reference or cv-qualified types!");

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    if constexpr (__detail__::is_signed_int_v<T>) {
        int64_t res = val->IntegerValue(ctx).ToChecked();
        return res;
    } else if constexpr (std::is_unsigned_v<T>) {
        uint32_t res = val->Uint32Value(ctx).ToChecked();
        return res;
    } else if constexpr (std::is_floating_point_v<T>) {
        double res = val->NumberValue(ctx).ToChecked();
        return res;
    } else if constexpr (std::is_same_v<T, std::string>) {
        v8::String::Utf8Value str(isolate, val);
        if (*str) {
            return *str;
        }
    } else {
        LOGW("Conversion failure for javascript type %s", V8Utils::ValueTypeAsString(val).c_str());
    }

    return T();
}

template<typename T>
v8::Local<v8::Value> toV8Value(T val)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
        return v8::Number::New(isolate, val);
    } else if constexpr (__detail__::is_signed_int_v<T>) {
        return v8::Integer::New(isolate, val);
    } else if constexpr (std::is_unsigned_v<T>) {
        return v8::Integer::NewFromUnsigned(isolate, val);
    } else if constexpr (std::is_floating_point_v<T>) {
        return v8::Number::New(isolate, val);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return v8::String::NewFromUtf8(isolate, val.c_str()).ToLocalChecked();
    } else if constexpr (std::is_same_v<T, const char*>) {
        return v8::String::NewFromUtf8(isolate, val).ToLocalChecked();
    } else if constexpr (std::is_same_v<T, bool>) {
        return v8::Boolean::New(isolate, val).ToLocalChecked();
    }

    return V8Utils::ThrowException("toV8Value: Conversion failure, unhandled type!");
}

} // namespace V8ValueConvertor
} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_VALUE_CONVERSIONS_H
