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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_base_utils.h"

#include "base/log/event_report.h"
#include "core/common/ace_application_info.h"
#include "core/common/ace_engine.h"

namespace OHOS::Ace::Framework {
std::string JsiBaseUtils::GenerateSummaryBody(std::shared_ptr<JsValue> error, std::shared_ptr<JsRuntime> runtime)
{
    std::string summaryBody;
    summaryBody.append("Lifetime: ")
        .append(std::to_string(OHOS::Ace::AceApplicationInfo::GetInstance().GetLifeTime()))
        .append("s")
        .append("\n");

    summaryBody.append("Js-Engine: ark\n");

    if (!error) {
        summaryBody.append("error uncaught: error is null");
        return summaryBody;
    }

    const AceType *data = static_cast<AceType *>(runtime->GetEmbedderData());
    auto runningPage = GetRunningPage(data);

    RefPtr<RevSourceMap> pageMap;
    RefPtr<RevSourceMap> appMap;
    if (runningPage) {
        auto pageUrl = runningPage->GetUrl();
        summaryBody.append("page: ").append(pageUrl).append("\n");

        pageMap = runningPage->GetPageMap();
        appMap = runningPage->GetAppMap();
    }

    if (!error->IsObject(runtime) || error->IsNull(runtime)) {
        std::string errorInfo = error->ToString(runtime);
        summaryBody.append(errorInfo).append("\n");
    }
    shared_ptr<JsValue> message = error->GetProperty(runtime, "message");
    std::string messageStr = message->ToString(runtime);
    summaryBody.append("Error message: ");
    summaryBody.append(messageStr).append("\n");

    shared_ptr<JsValue> stack = error->GetProperty(runtime, "stack");
    std::string stackStr = stack->ToString(runtime);
    summaryBody.append("Stacktrace:\n");

    if (pageMap || appMap) {
        std::string tempStack = JsiDumpSourceFile(stackStr, pageMap, appMap, data);
        summaryBody.append(tempStack);
    } else {
        summaryBody.append("Cannot get SourceMap info, dump raw stack:\n");
        summaryBody.append(stackStr);
    }

    return summaryBody;
}

std::string JsiBaseUtils::TransSourceStack(RefPtr<JsAcePage> runningPage, const std::string& rawStack)
{
    if (!runningPage) {
        return rawStack;
    }
    std::string stacktrace = "Stacktrace:\n";
    stacktrace.append(rawStack).append("\n");
    std::string summaryBody;
    RefPtr<RevSourceMap> pageMap;
    RefPtr<RevSourceMap> appMap;
    if (runningPage) {
        auto pageUrl = runningPage->GetUrl();
        summaryBody.append(" Page: ").append(pageUrl).append("\n");
        pageMap = runningPage->GetPageMap();
        appMap = runningPage->GetAppMap();
    }

    if (pageMap || appMap) {
        std::string tempStack = JsiBaseUtils::JsiDumpSourceFile(stacktrace, pageMap, appMap);
        summaryBody.append(tempStack);
    } else {
        summaryBody.append("Cannot get SourceMap info, dump raw stack:\n");
        summaryBody.append(stacktrace);
    }

    return summaryBody;
}

std::string JsiBaseUtils::JsiDumpSourceFile(const std::string& stackStr, const RefPtr<RevSourceMap>& pageMap,
    const RefPtr<RevSourceMap>& appMap, const AceType *data)
{
    std::string ans = "";
    std::string tempStack = stackStr;
    int32_t appFlag = static_cast<int32_t>(tempStack.find("app_.js"));
    bool isAppPage = appFlag > 0 && appMap;

    // find per line of stack
    std::vector<std::string> res;
    ExtractEachInfo(tempStack, res);

    // collect error info first
    ans = ans + res[0] + "\n";
    for (uint32_t i = 1; i < res.size(); i++) {
        std::string temp = res[i];
        std::string line = "";
        std::string column = "";
        GetPosInfo(temp, line, column);
        if (line.empty() || column.empty()) {
            LOGI("the stack without line info");
            break;
        }

        const std::string sourceInfo = GetSourceInfo(line, column, pageMap, appMap, isAppPage, data);
        if (sourceInfo.empty()) {
            break;
        }
        temp = "at " + sourceInfo;
        ans = ans + temp + "\n";
    }
    if (ans.empty()) {
        return tempStack;
    }
    return ans;
}

void JsiBaseUtils::ExtractEachInfo(const std::string& tempStack, std::vector<std::string>& res)
{
    std::string tempStr = "";
    for (uint32_t i = 0; i < tempStack.length(); i++) {
        if (tempStack[i] == '\n') {
            res.push_back(tempStr);
            tempStr = "";
        } else {
            tempStr += tempStack[i];
        }
    }
    res.push_back(tempStr);
}

void JsiBaseUtils::GetPosInfo(const std::string& temp, std::string& line, std::string& column)
{
    // 0 for colum, 1 for row
    int32_t flag = 0;
    // find line, column
    for (int32_t i = static_cast<int32_t>(temp.length()) - 1; i > 0; i--) {
        if (temp[i] == ':') {
            flag += 1;
            continue;
        }
        // some stack line may end with ")"
        if (flag == 0) {
            if (temp[i] >= '0' && temp[i] <= '9') {
                column = temp[i] + column;
            }
        } else if (flag == 1) {
            line = temp[i] + line;
        } else {
            break;
        }
    }
}

std::string JsiBaseUtils::GetSourceInfo(const std::string& line, const std::string& column,
    const RefPtr<RevSourceMap>& pageMap, const RefPtr<RevSourceMap>& appMap, bool isAppPage, const AceType *data)
{
    int32_t offSet = GetLineOffset(data);
    std::string sourceInfo;
    MappingInfo mapInfo;
    if (isAppPage) {
        mapInfo = appMap->Find(StringToInt(line) - offSet, StringToInt(column));
    } else {
        mapInfo = pageMap->Find(StringToInt(line) - offSet, StringToInt(column));
    }
    if (mapInfo.row == 0 || mapInfo.col == 0) {
        return "";
    }

    std::string sources = GetRelativePath(mapInfo.sources);
    sourceInfo = "(" + sources + ":" + std::to_string(mapInfo.row) + ":" + std::to_string(mapInfo.col) + ")";
    return sourceInfo;
}

std::string JsiBaseUtils::GetRelativePath(const std::string& sources)
{
    std::size_t pos = sources.find_last_of("/\\");
    if (pos != std::string::npos) {
        std::size_t splitPos = sources.substr(0, pos - 1).find_last_of("/\\");
        if (splitPos != std::string::npos) {
            std::string shortUrl = "pages" + sources.substr(splitPos);
            return shortUrl;
        }
    }
    LOGI("The stack path error!");
    return sources;
}

void JsiBaseUtils::ReportJsErrorEvent(std::shared_ptr<JsValue> error, std::shared_ptr<JsRuntime> runtime)
{
    if (!runtime) {
        LOGI("ReportJsErrorEvent: jsi engine has been destroyed");
        return;
    }
    LOGI("ReportJsErrorEvent");
    std::string summaryBody = GenerateSummaryBody(error, runtime);
    LOGE("summaryBody: \n%{public}s", summaryBody.c_str());
    EventReport::JsErrReport(AceApplicationInfo::GetInstance().GetPackageName(), "", summaryBody);
}

std::string ParseLogContent(const std::vector<std::string>& params)
{
    std::string ret;
    if (params.empty()) {
        return ret;
    }
    std::string formatStr = params[0];
    int32_t size = static_cast<int32_t>(params.size());
    int32_t len = static_cast<int32_t>(formatStr.size());
    int32_t pos = 0;
    int32_t count = 1;
    for (; pos < len; ++pos) {
        if (count >= size) {
            break;
        }
        if (formatStr[pos] == '%') {
            if (pos + 1 >= len) {
                break;
            }
            switch (formatStr[pos + 1]) {
                case 's':
                case 'j':
                case 'd':
                case 'O':
                case 'o':
                case 'i':
                case 'f':
                case 'c':
                    ret += params[count++];
                    ++pos;
                    break;
                case '%':
                    ret += formatStr[pos];
                    ++pos;
                    break;
                default:
                    ret += formatStr[pos];
                    break;
            }
        } else {
            ret += formatStr[pos];
        }
    }
    if (pos < len) {
        ret += formatStr.substr(pos, len - pos);
    }
    return ret;
}

std::string GetLogContent(
    const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc == 1) {
        return argv[0]->ToString(runtime);
    }
    std::vector<std::string> params;
    for (int32_t i = 0; i < argc; ++i) {
        params.emplace_back(argv[i]->ToString(runtime));
    }
    return ParseLogContent(params);
}

shared_ptr<JsValue> AppLogPrint(
    const shared_ptr<JsRuntime>& runtime, JsLogLevel level, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // Should have at least 1 parameters.
    if (argc == 0) {
        LOGE("the arg is error");
        return runtime->NewUndefined();
    }
    std::string content = GetLogContent(runtime, argv, argc);
    switch (level) {
        case JsLogLevel::DEBUG:
            APP_LOGD("app Log: %{public}s", content.c_str());
            break;
        case JsLogLevel::INFO:
            APP_LOGI("app Log: %{public}s", content.c_str());
            break;
        case JsLogLevel::WARNING:
            APP_LOGW("app Log: %{public}s", content.c_str());
            break;
        case JsLogLevel::ERROR:
            APP_LOGE("app Log: %{public}s", content.c_str());
            break;
    }

    return runtime->NewUndefined();
}

// native implementation for js function: console.debug()
shared_ptr<JsValue> JsiBaseUtils::AppDebugLogPrint(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return AppLogPrint(runtime, JsLogLevel::DEBUG, argv, argc);
}

// native implementation for js function: console.info()
shared_ptr<JsValue> JsiBaseUtils::AppInfoLogPrint(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return AppLogPrint(runtime, JsLogLevel::INFO, argv, argc);
}

// native implementation for js function: console.warn()
shared_ptr<JsValue> JsiBaseUtils::AppWarnLogPrint(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return AppLogPrint(runtime, JsLogLevel::WARNING, argv, argc);
}

// native implementation for js function: console.error()
shared_ptr<JsValue> JsiBaseUtils::AppErrorLogPrint(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return AppLogPrint(runtime, JsLogLevel::ERROR, argv, argc);
}

shared_ptr<JsValue> JsLogPrint(
    const shared_ptr<JsRuntime>& runtime, JsLogLevel level, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // Should have 1 parameters.
    if (argc == 0) {
        LOGE("the arg is error");
        return runtime->NewUndefined();
    }

    std::string content = GetLogContent(runtime, argv, argc);
    switch (level) {
        case JsLogLevel::DEBUG:
            LOGD("ace Log: %{public}s", content.c_str());
            break;
        case JsLogLevel::INFO:
            LOGI("ace Log: %{public}s", content.c_str());
            break;
        case JsLogLevel::WARNING:
            LOGW("ace Log: %{public}s", content.c_str());
            break;
        case JsLogLevel::ERROR:
            LOGE("ace Log: %{public}s", content.c_str());
            break;
    }

    shared_ptr<JsValue> ret = runtime->NewUndefined();
    return ret;
}

int PrintLog(int id, int level, const char* tag, const char* fmt, const char* message)
{
    switch (JsLogLevel(level - 3)) {
        case JsLogLevel::INFO:
            LOGI("%{public}s::%{public}s", tag, message);
            break;
        case JsLogLevel::WARNING:
            LOGW("%{public}s::%{public}s", tag, message);
            break;
        case JsLogLevel::ERROR:
            LOGE("%{public}s::%{public}s", tag, message);
            break;
        case JsLogLevel::DEBUG:
            LOGD("%{public}s::%{public}s", tag, message);
            break;
        default:
            LOGF("%{public}s::%{public}s", tag, message);
            break;
    }
    return 0;
}

shared_ptr<JsValue> JsiBaseUtils::JsDebugLogPrint(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return JsLogPrint(runtime, JsLogLevel::DEBUG, std::move(argv), argc);
}

shared_ptr<JsValue> JsiBaseUtils::JsInfoLogPrint(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return JsLogPrint(runtime, JsLogLevel::INFO, std::move(argv), argc);
}

shared_ptr<JsValue> JsiBaseUtils::JsWarnLogPrint(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return JsLogPrint(runtime, JsLogLevel::WARNING, std::move(argv), argc);
}

shared_ptr<JsValue> JsiBaseUtils::JsErrorLogPrint(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return JsLogPrint(runtime, JsLogLevel::ERROR, std::move(argv), argc);
}

std::string GetLogContent(NativeEngine* nativeEngine, NativeCallbackInfo* info)
{
    std::string content;
    for (size_t i = 0; i < info->argc; ++i) {
        if (info->argv[i]->TypeOf() != NATIVE_STRING) {
            LOGE("argv is not NativeString");
            continue;
        }
        auto nativeString = reinterpret_cast<NativeString*>(info->argv[i]->GetInterface(NativeString::INTERFACE_ID));
        size_t bufferSize = nativeString->GetLength();
        size_t strLength = 0;
        char* buffer = new char[bufferSize + 1] { 0 };
        nativeString->GetCString(buffer, bufferSize + 1, &strLength);
        content.append(buffer);
        delete[] buffer;
    }
    return content;
}

NativeValue* AppLogPrint(NativeEngine* nativeEngine, NativeCallbackInfo* info, JsLogLevel level)
{
    // Should have at least 1 parameters.
    if (info->argc == 0) {
        LOGE("the arg is error");
        return nativeEngine->CreateUndefined();
    }
    std::string content = GetLogContent(nativeEngine, info);
    switch (level) {
        case JsLogLevel::DEBUG:
            APP_LOGD("app Log: %{public}s", content.c_str());
            break;
        case JsLogLevel::INFO:
            APP_LOGI("app Log: %{public}s", content.c_str());
            break;
        case JsLogLevel::WARNING:
            APP_LOGW("app Log: %{public}s", content.c_str());
            break;
        case JsLogLevel::ERROR:
            APP_LOGE("app Log: %{public}s", content.c_str());
            break;
    }

    return nativeEngine->CreateUndefined();
}

NativeValue* AppDebugLogPrint(NativeEngine* nativeEngine, NativeCallbackInfo* info)
{
    return AppLogPrint(nativeEngine, info, JsLogLevel::DEBUG);
}

NativeValue* AppInfoLogPrint(NativeEngine* nativeEngine, NativeCallbackInfo* info)
{
    return AppLogPrint(nativeEngine, info, JsLogLevel::INFO);
}

NativeValue* AppWarnLogPrint(NativeEngine* nativeEngine, NativeCallbackInfo* info)
{
    return AppLogPrint(nativeEngine, info, JsLogLevel::WARNING);
}

NativeValue* AppErrorLogPrint(NativeEngine* nativeEngine, NativeCallbackInfo* info)
{
    return AppLogPrint(nativeEngine, info, JsLogLevel::ERROR);
}
} // namespace OHOS::Ace::Framework
