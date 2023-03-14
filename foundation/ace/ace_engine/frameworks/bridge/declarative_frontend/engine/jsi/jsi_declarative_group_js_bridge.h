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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_GROUP_JS_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_GROUP_JS_BRIDGE_H

#include <map>
#include <string>

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
#include "adapter/preview/osal/request_data.h"
#include "adapter/preview/osal/response_data.h"
#endif
#include "base/memory/ace_type.h"
#include "base/utils/singleton.h"
#include "frameworks/bridge/codec/standard_function_codec.h"
#include "frameworks/bridge/js_frontend/engine/common/group_js_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_runtime.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_value.h"

namespace OHOS::Ace::Framework {
using std::shared_ptr;

struct PromiseCallback final {
    shared_ptr<JsValue> resolveCallback;
    shared_ptr<JsValue> rejectCallback;
};

using EventCallbackMap = std::map<int32_t, shared_ptr<JsValue>>;
using ModuleCallbackMap = std::map<int32_t, PromiseCallback>;
using RequestIdCallbackIdMap = std::map<int32_t, int32_t>;

enum class ParseJsDataResult {
    PARSE_JS_SUCCESS = 0,
    PARSE_JS_ERR_UNSUPPORTED_TYPE = 101,
    PARSE_JS_ERR_TOO_MANY_PARAM = 102,
};

class JsiDeclarativeGroupJsBridge : public GroupJsBridge {
    DECLARE_ACE_TYPE(JsiDeclarativeGroupJsBridge, GroupJsBridge)

    enum GroupType {
        MODULE_GROUP = 0,
        EVENT_GROUP = 1,
    };

public:
    JsiDeclarativeGroupJsBridge() = default;
    ~JsiDeclarativeGroupJsBridge() override = default;

    int32_t InitializeGroupJsBridge(const shared_ptr<JsRuntime>& runtime);

    void TriggerModuleJsCallback(int32_t callbackId, int32_t code, std::vector<uint8_t>&& messageData) override;

    void TriggerModulePluginGetErrorCallback(
        int32_t callbackId, int32_t errorCode, std::string&& errorMessage) override;

    void TriggerEventJsCallback(int32_t callbackId, int32_t code, std::vector<uint8_t>&& eventData) override;

    void LoadPluginJsCode(std::string&& jsCode) override;

    void LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) override;

    void Destroy() override;

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void TriggerModuleJsCallbackPreview(int32_t callbackId, int32_t code, ResponseData responseData) override;
#endif

private:
    int32_t GetPendingCallbackIdAndIncrement()
    {
        return pendingCallbackId_++;
    }

    // Js bridge functions are used for the mapping between the Js and CPP functions.
    int32_t LoadJsBridgeFunction();

    bool SetModuleGroupCallbackFuncs(const std::vector<shared_ptr<JsValue>>& argv, int32_t resolveCallbackIndex,
        int32_t rejectCallbackIndex, int32_t callbackId);
    bool SetEventGroupCallBackFuncs(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& localEventCallbackFunc, int32_t callbackId, int32_t requestId);

    void RemoveEventGroupCallBackFuncs(int32_t callbackId);
    void AddRequestIdCallbackIdRelation(int32_t callbackId, int32_t requestId);
    void RemoveRequestIdCallbackIdRelation(int32_t requestId, bool removeEventCallback);

    void CallModuleJsCallback(int32_t callbackId, int32_t code, const shared_ptr<JsValue>& callBackResult);
    void CallEventJsCallback(int32_t callbackId, std::vector<uint8_t>&& data);

    ParseJsDataResult ParseJsPara(const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
        int32_t beginIndex, int32_t requestId, std::vector<CodecData>& arguments);

    // process when parse data from js engine failed
    static void ProcessParseJsError(
        ParseJsDataResult errorType, const shared_ptr<JsRuntime>& runtime, int32_t callbackId);

    static std::string SerializationObjectToString(
        const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& val);

    // JsSendGroupMessage is mapped to the Js function[SendGroupMessage]
    // contains three parameters: groupName, message, callback function
    static shared_ptr<JsValue> ProcessJsRequest(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);

    // JsSendGroupMessage is mapped to the Js function[SendGroupMessageSync]
    // contains three parameters: groupName, message, params
    static shared_ptr<JsValue> ProcessJsRequestSync(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& thisObj, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);

    EventCallbackMap eventCallBackFuncs_;
    ModuleCallbackMap moduleCallBackFuncs_;
    RequestIdCallbackIdMap requestIdCallbackIdMap_;
    std::atomic_int pendingCallbackId_;

    shared_ptr<JsRuntime> runtime_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_GROUP_JS_BRIDGE_H
