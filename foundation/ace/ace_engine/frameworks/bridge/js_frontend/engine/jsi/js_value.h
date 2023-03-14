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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_ENGINE_JSI_JS_VALUE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_ENGINE_JSI_JS_VALUE_H

#include <functional>
#include <string>
#include <memory>

// NOLINTNEXTLINE(readability-identifier-naming)
namespace OHOS::Ace::Framework {
using std::shared_ptr;
class JsRuntime;
/* A class that represent all types of js value. Since we don't have separate class for each type, please check it's
 * type before calling a type specific method such as GetProperty.
 */
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
class JsValue {
public:
    JsValue() = default;
    virtual ~JsValue() = default;

    virtual int32_t ToInt32(shared_ptr<JsRuntime> runtime) = 0;
    virtual double ToDouble(shared_ptr<JsRuntime> runtime) = 0;
    virtual std::string ToString(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool ToBoolean(shared_ptr<JsRuntime> runtime) = 0;

    virtual bool IsObject(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool IsArray(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool IsUndefined(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool IsNull(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool IsNumber(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool IsInt32(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool WithinInt32(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool IsBoolean(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool IsString(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool IsFunction(shared_ptr<JsRuntime> runtime) = 0;
    virtual bool IsException(shared_ptr<JsRuntime> runtime) = 0;

    // NOLINTNEXTLINE(google-runtime-references)
    virtual bool GetPropertyNames(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> &propertyNames, int32_t &len) = 0;
    virtual bool GetEnumerablePropertyNames(shared_ptr<JsRuntime> runtime,
                                            shared_ptr<JsValue> &propertyNames, int32_t &len) = 0;

    // Get object property by name or index, returns an nullptr if failed to get the specified property.
    virtual shared_ptr<JsValue> GetProperty(shared_ptr<JsRuntime> runtime, int32_t idx) = 0;
    virtual shared_ptr<JsValue> GetProperty(shared_ptr<JsRuntime> runtime, const std::string &name) = 0;
    virtual shared_ptr<JsValue> GetProperty(shared_ptr<JsRuntime> runtime, const shared_ptr<JsValue> &name) = 0;

    // Set a property with the given name and value, returns true if success.
    virtual bool SetProperty(shared_ptr<JsRuntime> runtime, const std::string &name,
                             const shared_ptr<JsValue> &value) = 0;
    virtual bool SetProperty(shared_ptr<JsRuntime> runtime, const shared_ptr<JsValue> &name,
                             const shared_ptr<JsValue> &value) = 0;
    virtual bool SetAccessorProperty(shared_ptr<JsRuntime> runtime, const std::string &name,
                                     const shared_ptr<JsValue> &getter, const shared_ptr<JsValue> &setter) = 0;
    virtual bool SetAccessorProperty(shared_ptr<JsRuntime> runtime, const shared_ptr<JsValue> &name,
                                     const shared_ptr<JsValue> &getter, const shared_ptr<JsValue> &setter) = 0;

    // Get an array's length. Return -1 if fails.
    virtual int32_t GetArrayLength(shared_ptr<JsRuntime> runtime) = 0;
    // Get an array element by index, Return nullptr if fails.
    virtual shared_ptr<JsValue> GetElement(shared_ptr<JsRuntime> runtime, int32_t idx) = 0;

    virtual shared_ptr<JsValue> Call(shared_ptr<JsRuntime> runtime, shared_ptr<JsValue> thisObj,
                                     std::vector<shared_ptr<JsValue>> argv, int32_t argc) = 0;
    virtual std::string GetJsonString(const shared_ptr<JsRuntime>& runtime) = 0;
};
}  // namespace OHOS::Ace::Framework
#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_ENGINE_JSI_JS_VALUE_H
