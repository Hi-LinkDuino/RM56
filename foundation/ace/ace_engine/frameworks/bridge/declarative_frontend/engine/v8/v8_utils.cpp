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

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_utils.h"

#include "base/log/log.h"

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"

namespace OHOS::Ace::Framework {
namespace V8Utils {
std::vector<std::string> GetObjectKeys(v8::Local<v8::Object> obj)
{
    auto isolate = v8::Isolate::GetCurrent();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Array> arr = obj->GetPropertyNames(context, v8::KeyCollectionMode::kIncludePrototypes,
                                      v8::PropertyFilter::ALL_PROPERTIES, v8::IndexFilter::kSkipIndices)
                                   .ToLocalChecked();

    std::vector<std::string> res;
    for (uint32_t i = 0; i < arr->Length(); ++i) {
        v8::Local<v8::Value> key = arr->Get(context, i).ToLocalChecked();
        v8::String::Utf8Value s(isolate, key);
        if (*s) {
            res.push_back(*s);
        }
    }

    return res;
}

void JsStdDumpErrorAce(v8::Isolate* isolate, const v8::TryCatch* tryCatch)
{
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto stagingPage = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8DeclarativeEngineInstance::STAGING_PAGE));

    // dump exception message
    v8::String::Utf8Value exception(isolate, tryCatch->Exception());
    LOGE("[DUMP] %{private}s", *exception);

    v8::Local<v8::Message> message = tryCatch->Message();
    v8::Maybe<int> lineNumberLocal = message->GetLineNumber(context);
    v8::MaybeLocal<v8::String> line = message->GetSourceLine(context);
    int columnNumberStart = message->GetStartColumn();
    int columnNumberEnd = message->GetEndColumn();

    int lineNumber = -1;
    std::string source;
    std::string column;
    if (!lineNumberLocal.IsNothing()) {
        lineNumber = lineNumberLocal.ToChecked();
    }

    if (!line.IsEmpty()) {
        v8::String::Utf8Value lineStrUtf8(isolate, line.ToLocalChecked());
        source = *lineStrUtf8;
        source += "\t\t";
    }

    v8::String::Utf8Value script(isolate, message->GetScriptResourceName());
    if (!(*script)) {
        LOGE("%{private}s", source.c_str());
        return;
    }

    source += "(" + std::string(*script);
    if (lineNumber != -1 && columnNumberStart != -1 && columnNumberEnd != -1) {
        column.append(columnNumberStart, ' ');
        column.append("^");
        if ((columnNumberEnd - columnNumberStart) > 1) {
            column.append(columnNumberEnd - columnNumberStart - 1, '~');
        }
        column.append("^");

        source += ":" + std::to_string(lineNumber) + ":" + std::to_string(columnNumberStart) + "-" +
                  std::to_string(columnNumberEnd);
    } else {
        source += ")";
    }

    LOGE("%s", source.c_str());
    if (!line.IsEmpty()) {
        LOGE("%s", column.c_str());
    }

    RefPtr<RevSourceMap> pageMap;
    RefPtr<RevSourceMap> appMap;
    if (stagingPage && (*stagingPage)) {
        pageMap = (*stagingPage)->GetPageMap();
        appMap = (*stagingPage)->GetAppMap();
    }

    if (!tryCatch->StackTrace(context).IsEmpty()) {
        v8::String::Utf8Value stackTrace(isolate, tryCatch->StackTrace(context).ToLocalChecked());
        if (pageMap || appMap) {
            std::string tempStack = JsStdDumpSourceFile(*stackTrace, pageMap, appMap);
            LOGE("%{public}s", tempStack.c_str());
        } else {
            LOGE("%{public}s", *stackTrace);
        }
    }
}

std::string ValueTypeAsString(v8::Local<v8::Value> val)
{
    auto isolate = v8::Isolate::GetCurrent();
    v8::String::Utf8Value valType(isolate, val->TypeOf(isolate));
    if (*valType) {
        return *valType;
    } else {
        return "";
    }
}

std::string JsStdDumpSourceFile(const std::string& stackTrace, const RefPtr<RevSourceMap>& pageMap,
    const RefPtr<RevSourceMap>& appMap)
{

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
        std::string line = "";
        std::string column = "";
        GetPosInfo(temp, line, column);
        if (line == "" || column == "") {
            LOGI("the stack without line info");
            break;
        }

        const std::string sourceInfo = GetSourceInfo(line, column, pageMap, appMap, isAppPage);
        if (sourceInfo == "") {
            break;
        }
        temp = "at " + sourceInfo;
        ans = ans + temp + "\n";
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
    res.push_back(tempStr);
}

void GetPosInfo(const std::string& temp, std::string& line, std::string& column)
{
    // 0 for colum, 1 for row
    int32_t flag = 0;
    // find line, column
    for (int32_t i = temp.length() - 1; i > 0; i--) {
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

std::string GetSourceInfo(const std::string& line, const std::string& column, const RefPtr<RevSourceMap>& pageMap,
    const RefPtr<RevSourceMap>& appMap, bool isAppPage)
{
    std::string sourceInfo;
    MappingInfo mapInfo;
    if (isAppPage) {
        mapInfo = appMap->Find(StringToInt(line), StringToInt(column));
    } else {
        mapInfo = pageMap->Find(StringToInt(line), StringToInt(column));
    }
    if (mapInfo.row == 0 || mapInfo.col == 0) {
        return "";
    }
    sourceInfo = "(" + mapInfo.sources + ":" + std::to_string(mapInfo.row) + ":" + std::to_string(mapInfo.col) + ")";
    return sourceInfo;
}

} // namespace V8Utils
} // namespace OHOS::Ace::Framework
