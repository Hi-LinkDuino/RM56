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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_ENGINE_JSI_JS_RUNTIME_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_ENGINE_JSI_JS_RUNTIME_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

// NOLINTNEXTLINE(readability-identifier-naming)
namespace OHOS::Ace::Framework {
class JsValue;
class JsRuntime;

using std::shared_ptr;
using RegisterFunctionType = std::function<shared_ptr<JsValue>(shared_ptr<JsRuntime>, shared_ptr<JsValue>,
                                                               const std::vector<shared_ptr<JsValue>> &, int32_t)>;
using LOG_PRINT = int (*)(int id, int level, const char *tag, const char *fmt, const char *message);
using UncaughtExceptionCallback = std::function<void(shared_ptr<JsValue>, std::shared_ptr<JsRuntime>)>;

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
class JsRuntime {
public:
    virtual ~JsRuntime() = default;

    // Prepare js environment, returns true if success.
    virtual bool Initialize(const std::string &libraryPath, bool isDebugMode) = 0;
    virtual void Reset() = 0;
    virtual void SetLogPrint(LOG_PRINT out) = 0;

    // Evaluate a piece of js code, returns true if success.
    // If exception occurs during execution, it is handled inside this interface.
    virtual shared_ptr<JsValue> EvaluateJsCode(const std::string &src) = 0;
    virtual bool EvaluateJsCode(const uint8_t *buffer, int32_t size) = 0;

    virtual bool ExecuteJsBin([[maybe_unused]] const std::string &fileName)
    {
        return true;
    }

    // Get the global object.
    virtual shared_ptr<JsValue> GetGlobal() = 0;
    virtual void RunGC() = 0;

    virtual shared_ptr<JsValue> NewNumber(double d) = 0;
    virtual shared_ptr<JsValue> NewInt32(int32_t value) = 0;
    virtual shared_ptr<JsValue> NewBoolean(bool value) = 0;
    virtual shared_ptr<JsValue> NewNull() = 0;
    virtual shared_ptr<JsValue> NewUndefined() = 0;
    virtual shared_ptr<JsValue> NewString(const std::string &str) = 0;
    virtual shared_ptr<JsValue> ParseJson(const std::string &str) = 0;
    virtual shared_ptr<JsValue> NewObject() = 0;
    virtual shared_ptr<JsValue> NewArray() = 0;
    virtual shared_ptr<JsValue> NewFunction(RegisterFunctionType func) = 0;
    virtual shared_ptr<JsValue> NewNativePointer(void *ptr) = 0;
    virtual void RegisterUncaughtExceptionHandler(UncaughtExceptionCallback callback) = 0;
    virtual void HandleUncaughtException() = 0;
    virtual void ExecutePendingJob() = 0;

    // Set c++ data to js environment.
    void SetEmbedderData(void *data)
    {
        embedderData_ = data;
    }
    // Get c++ data from js environment.
    void *GetEmbedderData() const
    {
        return embedderData_;
    }

private:
    void *embedderData_ = nullptr;
};
}  // namespace OHOS::Ace::Framework
#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_ENGINE_JSI_JS_RUNTIME_H
