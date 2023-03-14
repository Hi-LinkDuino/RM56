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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_UTILS_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_UTILS_H

#include <sstream>

#include "third_party/v8/include/v8.h"

#include "base/log/log.h"
#include "frameworks/bridge/common/utils/source_map.h"

namespace OHOS::Ace::Framework {
namespace V8Utils {

class ScopedString {
public:
    explicit ScopedString(v8::Local<v8::Value> str)
    {
        auto isolate = v8::Isolate::GetCurrent();
        v8::String::Utf8Value s(isolate, str);
        str_ = *s;
    }

    ~ScopedString() = default;

    const char* get()
    {
        return str_.c_str();
    }

    std::string str()
    {
        return str_;
    }

    operator std::string()
    {
        return str_;
    }

    operator const char*()
    {
        return str_.c_str();
    }

private:
    std::string str_;
};

std::vector<std::string> GetObjectKeys(v8::Local<v8::Object> obj);
void JsStdDumpErrorAce(v8::Isolate* isolate, const v8::TryCatch* tryCatch);
std::string JsStdDumpSourceFile(const std::string& tempStack, const RefPtr<RevSourceMap>& pageMap,
    const RefPtr<RevSourceMap>& appMap);

void ExtractEachInfo(const std::string& tempStack, std::vector<std::string>& res);
void GetPosInfo(const std::string& temp, std::string& line, std::string& column);
std::string GetSourceInfo(const std::string& line, const std::string& column, const RefPtr<RevSourceMap>& pageMap,
    const RefPtr<RevSourceMap>& appMap, bool isAppPage);

// snprintf without a string literal (-Wformat-security)
template<typename... Args>
v8::Local<v8::Value> ThrowException(Args... args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    std::stringstream ss;
    (ss << ... << args);
    return isolate->ThrowException(v8::String::NewFromUtf8(isolate, ss.str().c_str()).ToLocalChecked());
}

std::string ValueTypeAsString(v8::Local<v8::Value> val);

} // namespace V8Utils
} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_UTILS_H
