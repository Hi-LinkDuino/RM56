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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_VALUE_CONVERSIONS_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_VALUE_CONVERSIONS_H

#include <string>

#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace __detail__ {

template<typename T>
struct _is_signed_int_ {
    static constexpr bool value =
        std::is_integral<T>::value && std::is_signed<T>::value && !std::is_same<T, bool>::value;
};

template<typename T>
static constexpr bool is_signed_int_v = _is_signed_int_<T>::value;

template<typename T>
T fromJSValue(JSValueConst val)
{
    static_assert(!std::is_const_v<T> && !std::is_reference_v<T>, //
        "Cannot convert values to reference or cv-qualified types!");

    JSContext* ctx = OHOS::Ace::Framework::QJSContext::Current();
    if constexpr (is_signed_int_v<T>) {
        int64_t res;
        JS_ToInt64(ctx, &res, val);
        return res;
    } else if constexpr (std::is_unsigned_v<T>) {
        uint32_t res;
        JS_ToUint32(ctx, &res, val);
        return res;
    } else if constexpr (std::is_floating_point_v<T>) {
        double res;
        JS_ToFloat64(ctx, &res, val);
        return res;
    } else if constexpr (std::is_same_v<T, std::string>) {
        OHOS::Ace::Framework::ScopedString str(val);
        return str.str();
    }

    return T();
}

template<typename T>
JSValue toJSValue(T val)
{
    JSContext* ctx = OHOS::Ace::Framework::QJSContext::Current();
    if constexpr (is_signed_int_v<T>) {
        return JS_NewInt64(ctx, val);
    } else if constexpr (std::is_unsigned_v<T>) {
        return JS_NewInt64(ctx, val);
    } else if constexpr (std::is_floating_point_v<T>) {
        return JS_NewFloat64(ctx, val);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return JS_NewStringLen(ctx, val.c_str(), val.size());
    } else if constexpr (std::is_same_v<T, const char*>) {
        return JS_NewStringLen(ctx, val, strlen(val));
    }

    return JS_ThrowInternalError(ctx, "Conversion failure...");
}

template<typename... Types>
struct TupleConverter {
    std::tuple<Types...> operator()(JSValueConst* argv)
    {
        int index = 0;
        return {
            __detail__::fromJSValue<Types>(argv[index++])...,
        };
    }
};

}; // namespace __detail__

namespace OHOS::Ace::Framework::QJSValueConvertor {

template<typename T>
inline JSValue toQJSValue(T&& val)
{
    return __detail__::toJSValue(std::forward<T>(val));
}

} // namespace OHOS::Ace::Framework::QJSValueConvertor

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_VALUE_CONVERSIONS_H
