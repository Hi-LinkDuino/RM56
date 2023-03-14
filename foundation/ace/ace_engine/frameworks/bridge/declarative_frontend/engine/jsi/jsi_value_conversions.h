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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_VALUE_CONVERSIONS_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_VALUE_CONVERSIONS_H

#include "ecmascript/napi/include/jsnapi.h"

#include "base/log/log.h"
#include "frameworks/bridge/js_frontend/engine/jsi/ark_js_runtime.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"

namespace OHOS::Ace::Framework {
namespace JsiValueConvertor {

template<typename T>
bool Validate(panda::Local<panda::JSValueRef> val)
{
    if constexpr (std::is_integral<T>::value && std::is_signed<T>::value && !std::is_same<T, bool>::value) {
        return val->IsInt();
    } else if constexpr (std::is_unsigned_v<T>) {
        return val->WithinInt32();
    } else if constexpr (std::is_floating_point_v<T>) {
        return val->IsNumber();
    } else if constexpr (std::is_same_v<T, std::string>) {
        return val->IsString();
    } else {
        LOGD("Unhandled type");
        return false;
    }
}

template<typename T>
T fromJsiValue(panda::Local<panda::JSValueRef> val)
{
    static_assert(!std::is_const_v<T> && !std::is_reference_v<T>, //
        "Cannot convert values to reference or cv-qualified types!");

    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    auto vm = runtime->GetEcmaVm();
    if constexpr (std::is_integral<T>::value && std::is_signed<T>::value && !std::is_same<T, bool>::value) {
        return val->Int32Value(vm);
    } else if constexpr (std::is_unsigned_v<T>) {
        return val->Uint32Value(vm);
    } else if constexpr (std::is_floating_point_v<T>) {
        return val->ToNumber(vm)->Value();
    } else if constexpr (std::is_same_v<T, std::string>) {
        return val->ToString(vm)->ToString();
    } else {
        LOGW("Conversion failure for javascript type.");
    }

    return T();
}

template<typename T>
panda::Local<panda::JSValueRef> toJsiValue(T val)
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    auto vm = runtime->GetEcmaVm();
    if constexpr (std::is_integral<T>::value && std::is_signed<T>::value && !std::is_same<T, bool>::value) {
        return panda::IntegerRef::New(vm, val);
    } else if constexpr (std::is_unsigned_v<T>) {
        return panda::IntegerRef::NewFromUnsigned(vm, val);
    } else if constexpr (std::is_floating_point_v<T>) {
        return panda::NumberRef::New(vm, val);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return panda::StringRef::NewFromUtf8(vm, val.c_str());
    } else if constexpr (std::is_same_v<T, const char*>) {
        return panda::StringRef::NewFromUtf8(vm, val);
    } else if constexpr (std::is_same_v<T, bool>) {
        return panda::BooleanRef::New(vm, val);
    }

    return panda::JSValueRef::Exception(vm);
}

} // namespace JsiValueConvertor
} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_VALUE_CONVERSIONS_H
