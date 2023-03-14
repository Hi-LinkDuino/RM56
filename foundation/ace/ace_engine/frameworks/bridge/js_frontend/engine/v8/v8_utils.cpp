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

#include "frameworks/bridge/js_frontend/engine/v8/v8_utils.h"

#include <string>
#include <sys/types.h>
#include <unistd.h>

#include "base/log/event_report.h"
#include "base/log/log.h"
#include "core/common/ace_application_info.h"
#include "core/common/ace_engine.h"

namespace OHOS::Ace::Framework {

namespace {

const char JS_CRASH[] = "Js Crash";
const char COMPILE_FAILED[] = "Compile failed when eval buffer";
const char RUN_FAILED[] = "Run the scripts failed when eval buffer";
const char DESTROY_APP_ERROR[] = "Destroy application failed";
const char DESTROY_PAGE_FAILED[] = "Destroy page instance failed";
const char LOAD_JS_BUNDLE_FAILED[] = "JS framework load js bundle failed";
const char FIRE_EVENT_FAILED[] = "Fire event failed";
const char ANIMATION_FINISH_FAILED[] = "Animation finish failed";
const char ANIMATION_CANCEL_FAILED[] = "Animation cancel failed";
const char STRINGIFY_FAILED[] = "Deserialize to string failed";
const char CALLBACK_PROCESS_FAILED[] = "Callback function process failed";

std::string GetReason(JsErrorType errorType)
{
    std::string reasonStr;
    switch (errorType) {
        case OHOS::Ace::Framework::JsErrorType::JS_CRASH:
            reasonStr = JS_CRASH;
            break;
        case OHOS::Ace::Framework::JsErrorType::COMPILE_ERROR:
            reasonStr = COMPILE_FAILED;
            break;
        case OHOS::Ace::Framework::JsErrorType::RUNTIME_ERROR:
            reasonStr = RUN_FAILED;
            break;
        case OHOS::Ace::Framework::JsErrorType::DESTROY_APP_ERROR:
            reasonStr = DESTROY_APP_ERROR;
            break;
        case OHOS::Ace::Framework::JsErrorType::DESTROY_PAGE_ERROR:
            reasonStr = DESTROY_PAGE_FAILED;
            break;
        case OHOS::Ace::Framework::JsErrorType::LOAD_JS_BUNDLE_ERROR:
            reasonStr = LOAD_JS_BUNDLE_FAILED;
            break;
        case OHOS::Ace::Framework::JsErrorType::FIRE_EVENT_ERROR:
            reasonStr = FIRE_EVENT_FAILED;
            break;
        case OHOS::Ace::Framework::JsErrorType::ANIMATION_FINISH_ERROR:
            reasonStr = ANIMATION_FINISH_FAILED;
            break;
        case OHOS::Ace::Framework::JsErrorType::ANIMATION_CANCEL_ERROR:
            reasonStr = ANIMATION_CANCEL_FAILED;
            break;
        case OHOS::Ace::Framework::JsErrorType::STRINGIFY_ERROR:
            reasonStr = STRINGIFY_FAILED;
            break;
        case OHOS::Ace::Framework::JsErrorType::CALLBACK_PROCESS_ERROR:
            reasonStr = CALLBACK_PROCESS_FAILED;
            break;
        default:
            reasonStr = JS_CRASH;
            break;
    }

    return reasonStr;
}

std::string GenerateSummaryBody(const std::string& stackTrace, int32_t instanceId, const char* pageUrl)
{
    std::string abilityName;
    if (AceEngine::Get().GetContainer(instanceId) != nullptr) {
        abilityName = AceEngine::Get().GetContainer(instanceId)->GetHostClassName();
    }

    std::string summaryBody;
    summaryBody.append("Lifetime: ")
        .append(std::to_string(OHOS::Ace::AceApplicationInfo::GetInstance().GetLifeTime()))
        .append("s")
        .append("\n");

    if (!abilityName.empty()) {
        summaryBody.append("Ability: ").append(abilityName).append("\n");
    }

    if (pageUrl != nullptr) {
        summaryBody.append("page: ").append(pageUrl).append("\n");
    }

    summaryBody.append("Js-Engine: v8");

    summaryBody.append("\n").append("Stacktrace: ").append(stackTrace);

    return summaryBody;
}

} // namespace

bool GetParamsWithCallbackId(const v8::Local<v8::Context>& context, const v8::Local<v8::Array>& args,
    std::map<std::string, std::string>& params, std::string& callbackId)
{
    if (context.IsEmpty() || args.IsEmpty()) {
        LOGE("fail to get params due to context or args is empty");
        return false;
    }
    static uint32_t paramsLength = 2;
    if (args->Length() < paramsLength) {
        LOGE("fail to get params due to length is illegal, %{public}d", args->Length());
        return false;
    }
    v8::Isolate* isolate = context->GetIsolate();
    if (isolate == nullptr) {
        LOGE("fail to get params due to isolate is nullptr");
        return false;
    }
    v8::HandleScope handleScope(isolate);
    auto jsObjectValue = args->Get(context, 0).ToLocalChecked();
    if (!GetParams(context, jsObjectValue, params)) {
        return false;
    }
    v8::Local<v8::Value> jsCallBackId = args->Get(context, 1).ToLocalChecked();
    v8::String::Utf8Value jsCallBackIdStr(isolate, jsCallBackId);
    if (*jsCallBackIdStr == nullptr) {
        LOGE("fail to get callback id");
        return false;
    }
    callbackId = *jsCallBackIdStr;
    return true;
}

bool GetParams(
    const v8::Local<v8::Context>& context, const v8::Local<v8::Value>& args, std::map<std::string, std::string>& params)
{
    if (context.IsEmpty() || args.IsEmpty() || !args->IsObject()) {
        LOGE("fail to get params due to context or args is empty");
        return false;
    }
    v8::Isolate* isolate = context->GetIsolate();
    if (isolate == nullptr) {
        LOGE("fail to get params due to isolate is nullptr");
        return false;
    }
    v8::HandleScope handleScope(isolate);
    auto jsObject = args->ToObject(context).ToLocalChecked();
    v8::Local<v8::Array> valObjProperties = jsObject->GetOwnPropertyNames(context).ToLocalChecked();
    if (valObjProperties.IsEmpty()) {
        LOGE("fail to get params due to valObjProperties is illegal");
        return false;
    }
    for (uint32_t i = 0; i < valObjProperties->Length(); ++i) {
        auto valObjKey = valObjProperties->Get(context, i).ToLocalChecked();
        auto valObj = jsObject->Get(context, valObjKey).ToLocalChecked();
        v8::String::Utf8Value valObjKeyStr(isolate, valObjKey);
        v8::String::Utf8Value valObjStr(isolate, valObj);
        if ((*valObjKeyStr != nullptr) && (*valObjStr != nullptr)) {
            params.try_emplace(*valObjKeyStr, *valObjStr);
        }
    }
    return true;
}

void V8Utils::JsStdDumpErrorAce(v8::Isolate* isolate, v8::TryCatch* tryCatch, JsErrorType errorType,
    int32_t instanceId, const RefPtr<JsAcePage>& page)
{
    if (isolate == nullptr) {
        LOGE("[DUMP] isolate can't be null!");
        return;
    }

    v8::HandleScope handle_scope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsStdDumpErrorAce, CurrentContext is empty!");
        return;
    }

    // dump exception message
    v8::String::Utf8Value exception(isolate, tryCatch->Exception());
    if (*exception) {
        LOGE("[DUMP] %{public}s", *exception);
    }

    RefPtr<RevSourceMap> pageMap;
    RefPtr<RevSourceMap> appMap;
    if (page) {
        pageMap = page->GetPageMap();
        appMap = page->GetAppMap();
    }

    // dump stack trace
    v8::String::Utf8Value stackTrace(isolate, tryCatch->StackTrace(context).ToLocalChecked());
    std::string stack = *stackTrace;
    if (!stack.empty()) {
        if (pageMap || appMap) {
            stack = JsStdDumpSourceFile(*stackTrace, pageMap, appMap);
        }
        LOGE("%{public}s", stack.c_str());
    }

    std::string packageName = AceApplicationInfo::GetInstance().GetPackageName();
    std::string reasonStr = GetReason(errorType);
    const char* pageUrl = (page == nullptr ? nullptr : page->GetUrl().c_str());
    std::string summaryBody = GenerateSummaryBody(stack, instanceId, pageUrl);
    EventReport::JsErrReport(packageName, reasonStr, summaryBody);
}

std::string V8Utils::JsStdDumpSourceFile(const std::string& stackTrace, const RefPtr<RevSourceMap>& pageMap,
    const RefPtr<RevSourceMap>& appMap)
{
    const std::string closeBrace = ")";
    const std::string openBrace = "(";
    const std::string suffix = ".js";
    std::string ans = "";
    std::string tempStack = stackTrace;
    int32_t appFlag = tempStack.find("app.js");
    bool isAppPage = appFlag > 0 && appMap;

    // find per line of stack
    std::vector<std::string> res;
    ExtractEachInfo(tempStack, res);

    // collect error info first
    ans = ans + res[0] + "\n";
    for (uint32_t i = 1; i < res.size(); i++) {
        std::string temp = res[i];
        int32_t closeBracePos = temp.find(closeBrace);
        int32_t openBracePos = temp.find(openBrace);

        std::string line = "";
        std::string column = "";
        GetPosInfo(temp, closeBracePos, line, column);

        if (line == "" || column == "") {
            LOGI("the stack without line info");
            break;
        }

        // if the page is end with ".js", push into stack
        if (temp.find(suffix) != suffix.npos) {
            const std::string sourceInfo = GetSourceInfo(line, column, pageMap, appMap, isAppPage);
            if (sourceInfo == "") {
                break;
            }
            temp.replace(openBracePos, closeBracePos - openBracePos + 1, sourceInfo);
            ans = ans + temp + "\n";
        } else {
            // other stack string is useless
            break;
        }
    }
    if (ans == "") {
        return tempStack;
    }
    return ans;
}

void ExtractEachInfo(const std::string& tempStack, std::vector<std::string>& res)
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
    // that's for only one line in the error stack
    res.push_back(tempStr);
}

void GetPosInfo(const std::string& temp, int32_t start, std::string& line, std::string& column)
{
    // 0 for colum, 1 for row
    int32_t flag = 0;
    // find line, column
    for (int32_t i = start - 1; i > 0; i--) {
        if (temp[i] == ':') {
            flag += 1;
            continue;
        }
        if (flag == 0) {
            column = temp[i] + column;
        } else if (flag == 1) {
            line = temp[i] + line;
        } else {
            break;
        }
    }
}

std::string GetSourceInfo(const std::string& line, const std::string& column, const RefPtr<RevSourceMap>& pageMap,
    const RefPtr<RevSourceMap>& appMap, bool isAppPage)
{
    std::string sourceInfo;
    MappingInfo mapInfo;
    int32_t offSet = 4;
    if (isAppPage) {
        mapInfo = appMap->Find(StringToInt(line) - offSet, StringToInt(column));
    } else {
        mapInfo = pageMap->Find(StringToInt(line) - offSet, StringToInt(column));
    }
    if (mapInfo.row == 0 || mapInfo.col == 0) {
        return "";
    }
    sourceInfo = "(" + mapInfo.sources + ":" + std::to_string(mapInfo.row) + ":" + std::to_string(mapInfo.col) + ")";
    return sourceInfo;
}

} // namespace OHOS::Ace::Framework
