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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_ENGINE_JSI_ARK_JS_VALUE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_ENGINE_JSI_ARK_JS_VALUE_H

#include "frameworks/bridge/js_frontend/engine/jsi/ark_js_runtime.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_value.h"

// NOLINTNEXTLINE(readability-identifier-naming)
namespace OHOS::Ace::Framework {
using panda::ArrayRef;
using panda::EscapeLocalScope;
using panda::FunctionRef;
using panda::Global;
using panda::JSON;
using panda::JSValueRef;
using panda::Local;
using panda::LocalScope;
using panda::NumberRef;
using panda::ObjectRef;
using panda::StringRef;
class PandaFunctionData;

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
class ArkJSValue final : public JsValue {
public:
    ArkJSValue(const shared_ptr<ArkJSRuntime> &runtime, Local<JSValueRef> value) : value_(runtime->GetEcmaVm(), value)
    {
    }
    ~ArkJSValue() override
    {
        value_.FreeGlobalHandleAddr();
    }

    int32_t ToInt32(shared_ptr<JsRuntime> runtime) override;
    double ToDouble(shared_ptr<JsRuntime> runtime) override;
    std::string ToString(shared_ptr<JsRuntime> runtime) override;
    bool ToBoolean(shared_ptr<JsRuntime> runtime) override;

    bool IsUndefined(shared_ptr<JsRuntime> runtime) override;
    bool IsNull(shared_ptr<JsRuntime> runtime) override;
    bool IsBoolean(shared_ptr<JsRuntime> runtime) override;
    bool IsInt32(shared_ptr<JsRuntime> runtime) override;
    bool WithinInt32(shared_ptr<JsRuntime> runtime) override;
    bool IsString(shared_ptr<JsRuntime> runtime) override;
    bool IsNumber(shared_ptr<JsRuntime> runtime) override;
    bool IsObject(shared_ptr<JsRuntime> runtime) override;
    bool IsArray(shared_ptr<JsRuntime> runtime) override;
    bool IsFunction(shared_ptr<JsRuntime> runtime) override;
    bool IsException(shared_ptr<JsRuntime> runtime) override;

    shared_ptr<JsValue> Call(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
                             std::vector<shared_ptr<JsValue>> argv, int32_t argc) override;

    bool GetPropertyNames(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> &propName, int32_t &len) override;
    bool GetEnumerablePropertyNames(shared_ptr<JsRuntime> runtime,
                                    shared_ptr<JsValue> &propName, int32_t &len) override;
    shared_ptr<JsValue> GetProperty(shared_ptr<JsRuntime> runtime, int32_t idx) override;
    shared_ptr<JsValue> GetProperty(shared_ptr<JsRuntime> runtime, const std::string &name) override;
    shared_ptr<JsValue> GetProperty(shared_ptr<JsRuntime> runtime, const shared_ptr<JsValue> &name) override;

    bool SetProperty(shared_ptr<JsRuntime> runtime, const std::string &name, const shared_ptr<JsValue> &value) override;
    bool SetProperty(shared_ptr<JsRuntime> runtime, const shared_ptr<JsValue> &name,
                     const shared_ptr<JsValue> &value) override;
    bool SetAccessorProperty(shared_ptr<JsRuntime> runtime, const std::string &name, const shared_ptr<JsValue> &getter,
                             const shared_ptr<JsValue> &setter) override;
    bool SetAccessorProperty(shared_ptr<JsRuntime> runtime, const shared_ptr<JsValue> &name,
                             const shared_ptr<JsValue> &getter, const shared_ptr<JsValue> &setter) override;

    int32_t GetArrayLength(shared_ptr<JsRuntime> runtime) override;
    shared_ptr<JsValue> GetElement(shared_ptr<JsRuntime> runtime, int32_t idx) override;
    std::string GetJsonString(const shared_ptr<JsRuntime>& runtime) override;

    Local<JSValueRef> GetValue(const shared_ptr<ArkJSRuntime> &runtime)
    {
        return value_.ToLocal(runtime->GetEcmaVm());
    }

    void SetValue(const shared_ptr<ArkJSRuntime> &runtime, const Local<JSValueRef> &value)
    {
        value_ = Global<JSValueRef>(runtime->GetEcmaVm(), value);
    }

private:
    Global<JSValueRef> value_ {};
};
}  // namespace OHOS::Ace::Framework
#endif  // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_ENGINE_JSI_ARK_JS_VALUE_H
