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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIEW_COMMON_DEF_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIEW_COMMON_DEF_H

#include <functional>
#include <vector>

#include "base/memory/ace_type.h"
#include "base/utils/noncopyable.h"
#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "bridge/declarative_frontend/engine/js_types.h"

#define ACE_DEFINE_DEFAULT_FUNCTIONS(name)                        \
public:                                                           \
    JsEventCallback() = delete;                                   \
    ~JsEventCallback() = default;                                 \
    JsEventCallback(const JsEventCallback&) = default;            \
    JsEventCallback& operator=(const JsEventCallback&) = default; \
    JsEventCallback(JsEventCallback&&) = default;                 \
    JsEventCallback& operator=(JsEventCallback&&) = default

namespace OHOS::Ace::Framework {

template<class T>
JSRef<JSVal> ConvertToJSValue(T&& value);

template<class... Args>
std::vector<JSRef<JSVal>> ConvertToJSValues(Args... args);

template<class T>
bool ConvertFromJSValue(const JSRef<JSVal>& jsValue, T& result);

template<class T, size_t N>
bool ConvertFromJSValue(const JSRef<JSVal>& jsValue, const T (&enumValues)[N], T& result);

template<class T>
T FastConvertFromJSValue(const JSRef<JSVal>& jsValue);

template<class F>
class JsEventCallback;

template<class R, class... Args>
class JsEventCallback<R(Args...)> final {
    ACE_DEFINE_DEFAULT_FUNCTIONS(JsEventCallback);

public:
    using ParamCollector = std::function<std::vector<JSRef<JSVal>>(Args...)>;
    using ResultConvertor = std::function<R(const JSRef<JSVal>&)>;

    JsEventCallback(const JSExecutionContext& context, const JSRef<JSFunc>& jsFunction,
        ParamCollector paramCollector = &ConvertToJSValues<Args...>,
        ResultConvertor resultConvertor = &FastConvertFromJSValue<R>)
        : context_(context), function_(jsFunction), paramCollector_(std::move(paramCollector)),
          resultConvertor_(std::move(resultConvertor))
    {}

    R operator()(Args... args)
    {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(context_, NULL);
        std::vector<JSRef<JSVal>> params = paramCollector_(args...);
        return resultConvertor_(function_->Call(JSRef<JSObject>(), params.size(), params.data()));
    }

private:
    JSExecutionContext context_;
    JSRef<JSFunc> function_;
    ParamCollector paramCollector_;
    ResultConvertor resultConvertor_;
};

template<class R>
class JsEventCallback<R()> final {
    ACE_DEFINE_DEFAULT_FUNCTIONS(JsEventCallback);

public:
    using ResultConvertor = std::function<R(const JSRef<JSVal>&)>;

    JsEventCallback(const JSExecutionContext& context, const JSRef<JSFunc>& jsFunction,
        ResultConvertor resultConvertor = &FastConvertFromJSValue<R>)
        : context_(context), function_(jsFunction), resultConvertor_(std::move(resultConvertor))
    {}

    R operator()()
    {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(context_, NULL);
        return resultConvertor_(function_->Call(JSRef<JSObject>()));
    }

private:
    JSExecutionContext context_;
    JSRef<JSFunc> function_;
    ResultConvertor resultConvertor_;
};

template<class... Args>
class JsEventCallback<void(Args...)> final {
    ACE_DEFINE_DEFAULT_FUNCTIONS(JsEventCallback);

public:
    using ParamCollector = std::function<std::vector<JSRef<JSVal>>(Args...)>;

    JsEventCallback(const JSExecutionContext& context, const JSRef<JSFunc>& jsFunction,
        ParamCollector paramCollector = &ConvertToJSValues<Args...>)
        : context_(context), function_(jsFunction), paramCollector_(std::move(paramCollector))
    {}

    void operator()(Args... args)
    {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(context_);
        std::vector<JSRef<JSVal>> params = paramCollector_(args...);
        function_->Call(JSRef<JSObject>(), params.size(), params.data());
    }

private:
    JSExecutionContext context_;
    JSRef<JSFunc> function_;
    ParamCollector paramCollector_;
};

template<>
class JsEventCallback<void()> final {
    ACE_DEFINE_DEFAULT_FUNCTIONS(JsEventCallback);

public:
    JsEventCallback(const JSExecutionContext& context, const JSRef<JSFunc>& jsFunction)
        : context_(context), function_(jsFunction)
    {}

    void operator()()
    {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(context_);
        function_->Call(JSRef<JSObject>());
    }

private:
    JSExecutionContext context_;
    JSRef<JSFunc> function_;
};

template<class C, class V, class T, size_t N>
void JSViewSetProperty(void (C::*setMethod)(V), int32_t param, const T (&enumValues)[N], T defValue);

template<class C, class V, class T>
void JSViewSetProperty(void (C::*setMethod)(V), T&& param);

template<class C, class F>
bool JSViewBindEvent(
    void (C::*setMethod)(std::function<F>&&), const JSExecutionContext& context, const JSRef<JSVal>& jsValue);

template<class C, class F>
bool JSViewBindEvent(void (C::*setMethod)(std::function<F>&&), const JSCallbackInfo& args);

} // namespace OHOS::Ace::Framework

#include "js_view_common_def.inl"

#undef ACE_DEFINE_DEFAULT_FUNCTIONS

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIEW_COMMON_DEF_H
