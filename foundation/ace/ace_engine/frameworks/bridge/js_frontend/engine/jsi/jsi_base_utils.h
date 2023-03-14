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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_BASE_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_BASE_UTILS_H

#include "frameworks/base/log/ace_trace.h"
#include "frameworks/base/log/event_report.h"
#include "frameworks/bridge/js_frontend/engine/common/runtime_constants.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_runtime.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_value.h"

namespace OHOS::Ace::Framework {
int32_t GetLineOffset(const AceType *data);
RefPtr<JsAcePage> GetRunningPage(const AceType *data);
NativeValue* AppDebugLogPrint(NativeEngine* nativeEngine, NativeCallbackInfo* info);
NativeValue* AppInfoLogPrint(NativeEngine* nativeEngine, NativeCallbackInfo* info);
NativeValue* AppWarnLogPrint(NativeEngine* nativeEngine, NativeCallbackInfo* info);
NativeValue* AppErrorLogPrint(NativeEngine* nativeEngine, NativeCallbackInfo* info);
int PrintLog(int id, int level, const char* tag, const char* fmt, const char* message);

class JsiBaseUtils {
public:
    static void ReportJsErrorEvent(std::shared_ptr<JsValue> error, std::shared_ptr<JsRuntime> runtime);
    static std::string TransSourceStack(RefPtr<JsAcePage> runningPage, const std::string& rawStack);
    // native implementation for js function: console.debug()
    static shared_ptr<JsValue> AppDebugLogPrint(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    // native implementation for js function: console.info()
    static shared_ptr<JsValue> AppInfoLogPrint(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    // native implementation for js function: console.warn()
    static shared_ptr<JsValue> AppWarnLogPrint(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    // native implementation for js function: console.error()
    static shared_ptr<JsValue> AppErrorLogPrint(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);

    // native implementation for js function: aceConsole.debug()
    static shared_ptr<JsValue> JsDebugLogPrint(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    // native implementation for js function: aceConsole.info()
    static shared_ptr<JsValue> JsInfoLogPrint(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    // native implementation for js function: aceConsole.warn()
    static shared_ptr<JsValue> JsWarnLogPrint(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    // native implementation for js function: aceConsole.error()
    static shared_ptr<JsValue> JsErrorLogPrint(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);

private:
    static std::string GenerateSummaryBody(std::shared_ptr<JsValue> error, std::shared_ptr<JsRuntime> runtime);
    static std::string JsiDumpSourceFile(const std::string& stackStr, const RefPtr<RevSourceMap>& pageMap,
        const RefPtr<RevSourceMap>& appMap, const AceType *data = nullptr);
    static void ExtractEachInfo(const std::string& tempStack, std::vector<std::string>& res);
    static void GetPosInfo(const std::string& temp, std::string& line, std::string& column);
    static std::string GetSourceInfo(const std::string& line, const std::string& column,
        const RefPtr<RevSourceMap>& pageMap, const RefPtr<RevSourceMap>& appMap, bool isAppPage, const AceType *data);
    static std::string GetRelativePath(const std::string& sources);
};
} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_BASE_UTILS_H
