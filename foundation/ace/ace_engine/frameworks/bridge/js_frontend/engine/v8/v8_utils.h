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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_UTILS_H

#include "third_party/v8/include/v8.h"

#include "frameworks/bridge/js_frontend/engine/v8/v8_engine.h"

namespace OHOS::Ace::Framework {

bool GetParamsWithCallbackId(const v8::Local<v8::Context>& context, const v8::Local<v8::Array>& args,
    std::map<std::string, std::string>& params, std::string& callbackId);

bool GetParams(const v8::Local<v8::Context>& context, const v8::Local<v8::Value>& args,
    std::map<std::string, std::string>& params);

void ExtractEachInfo(const std::string& tempStack, std::vector<std::string>& res);

void GetPosInfo(const std::string& temp, int32_t start, std::string& line, std::string& column);

std::string GetSourceInfo(const std::string& line, const std::string& column, const RefPtr<RevSourceMap>& pageMap,
    const RefPtr<RevSourceMap>& appMap, bool isAppPage);

enum class JsErrorType {
    JS_CRASH = 0,
    COMPILE_ERROR,
    RUNTIME_ERROR,
    FIRE_EVENT_ERROR,
    DESTROY_APP_ERROR,
    DESTROY_PAGE_ERROR,
    LOAD_JS_BUNDLE_ERROR,
    ANIMATION_START_ERROR,
    ANIMATION_FINISH_ERROR,
    ANIMATION_CANCEL_ERROR,
    ANIMATION_REPEAT_ERROR,
    ANIMATION_FRAME_ERROR,
    STRINGIFY_ERROR,
    CALLBACK_PROCESS_ERROR,
};

class V8Utils {
public:
    static void JsStdDumpErrorAce(v8::Isolate* isolate, v8::TryCatch* tryCatch,
        JsErrorType errorType = JsErrorType::JS_CRASH, int32_t instanceId = 0, const RefPtr<JsAcePage>& page = nullptr);
    static std::string JsStdDumpSourceFile(const std::string& tempStack, const RefPtr<RevSourceMap>& pageMap,
        const RefPtr<RevSourceMap>& appMap);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_UTILS_H
