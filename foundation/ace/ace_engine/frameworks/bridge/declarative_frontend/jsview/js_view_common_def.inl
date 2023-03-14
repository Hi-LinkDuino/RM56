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

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"

#include <string>
#include <type_traits>

#include "base/geometry/dimension.h"
#include "base/log/log.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "core/components/common/properties/color.h"

namespace OHOS::Ace::Framework {

template<class T>
JSRef<JSVal> ConvertToJSValue(T&& value)
{
    using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;
    if constexpr (std::is_arithmetic_v<ValueType> || std::is_same_v<ValueType, std::string>) {
        return JSRef<JSVal>::Make(ToJSValue(std::forward<T>(value)));
    } else if constexpr (std::is_enum_v<ValueType>) {
        return JSRef<JSVal>::Make(ToJSValue(static_cast<std::make_signed_t<ValueType>>(value)));
    } else if constexpr (std::is_same_v<ValueType, Dimension>) {
        if (value.Unit() == DimensionUnit::VP) {
            return JSRef<JSVal>::Make(ToJSValue(value.Value()));
        } else {
            LOGE("Failed to convert to JS value with dimension which it not using 'VP' unit");
            return JSRef<JSVal>();
        }
    } else {
        LOGE("Failed to convert to JS value");
        return JSRef<JSVal>();
    }
}

template<class T>
void ConvertToJSValuesImpl(std::vector<JSRef<JSVal>>& result, T&& value)
{
    result.emplace_back(ConvertToJSValue(std::forward<T>(value)));
}

template<class T, class V, class... Args>
void ConvertToJSValuesImpl(std::vector<JSRef<JSVal>>& result, T&& value, V&& nextValue, Args&&... args)
{
    result.emplace_back(ConvertToJSValue(std::forward<T>(value)));
    ConvertToJSValuesImpl(result, std::forward<V>(nextValue), std::forward<Args>(args)...);
}

template<class... Args>
std::vector<JSRef<JSVal>> ConvertToJSValues(Args... args)
{
    std::vector<JSRef<JSVal>> result;
    ConvertToJSValuesImpl(result, args...);
    return result;
}

template<class T>
bool ConvertFromJSValue(const JSRef<JSVal>& jsValue, T& result)
{
    if constexpr (std::is_same_v<T, bool>) {
        if (jsValue->IsBoolean()) {
            result = jsValue->ToBoolean();
            return true;
        }
        result = false;
    } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        double value;
        if (JSViewAbstract::ParseJsDouble(jsValue, value)) {
            result = static_cast<T>(value);
            return true;
        }
        result = 0;
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (jsValue->IsString()) {
            result = jsValue->ToString();
            return true;
        }
    } else if constexpr (std::is_same_v<T, Dimension>) {
        return JSViewAbstract::ParseJsDimensionVp(jsValue, result);
    } else if constexpr (std::is_same_v<T, Color>) {
        return JSViewAbstract::ParseJsColor(jsValue, result);
    }
    return false;
}

template<class T, size_t N>
bool ConvertFromJSValue(const JSRef<JSVal>& jsValue, const T (&enumValues)[N], T& result)
{
    int32_t value = 0;
    if (!ConvertFromJSValue(jsValue, value) || value < 0 || static_cast<size_t>(value) >= N) {
        return false;
    }
    result = enumValues[value];
    return true;
}

template<class T>
T FastConvertFromJSValue(const JSRef<JSVal>& jsValue)
{
    T result;
    if (!ConvertFromJSValue(jsValue, result)) {
        LOGE("Failed to convert from JS value");
    }
    return result;
}

template<class C, class V, class T, size_t N>
void JSViewSetProperty(void (C::*setMethod)(V), int32_t param, const T (&enumValues)[N], T defValue)
{
    auto component = AceType::DynamicCast<C>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<C>());
        return;
    }
    T value = defValue;
    if (param >= 0 && static_cast<size_t>(param) < N) {
        value = enumValues[param];
    }
    ((*component).*setMethod)(value);
}

template<class C, class V, class T>
void JSViewSetProperty(void (C::*setMethod)(V), T&& param)
{
    auto component = AceType::DynamicCast<C>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<C>());
        return;
    }
    ((*component).*setMethod)(std::forward<T>(param));
}

template<class C, class F>
bool JSViewBindEvent(
    void (C::*setMethod)(std::function<F>&&), const JSExecutionContext& context, const JSRef<JSVal>& jsValue)
{
    if (!jsValue->IsFunction()) {
        LOGW("Argument is not a function object");
        return false;
    }
    auto component = AceType::DynamicCast<C>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<C>());
        return false;
    }
    ((*component).*setMethod)(JsEventCallback<F>(context, JSRef<JSFunc>::Cast(jsValue)));
    return true;
}

template<class C, class F>
bool JSViewBindEvent(void (C::*setMethod)(std::function<F>&&), const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGW("Must contain at least 1 argument");
        return false;
    }
    return JSViewBindEvent(setMethod, args.GetExecutionContext(), args[0]);
}

} // namespace OHOS::Ace::Framework
