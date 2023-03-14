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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_QJS_GROUP_JS_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_QJS_GROUP_JS_BRIDGE_H

#include <map>
#include <string>

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
#include "adapter/preview/osal/request_data.h"
#include "adapter/preview/osal/response_data.h"
#endif
#include "base/memory/ace_type.h"
#include "base/utils/singleton.h"
#include "frameworks/bridge/codec/codec_data.h"
#include "frameworks/bridge/js_frontend/engine/common/group_js_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace OHOS::Ace::Framework {

struct PromiseCallback final {
    JSValue resolveCallback;
    JSValue rejectCallback;
};

using EventCallbackMap = std::map<int32_t, JSValue>;
using ModuleCallbackMap = std::map<int32_t, PromiseCallback>;
using RequestIdCallbackIdMap = std::map<int32_t, int32_t>;

// for parse js data
enum class ParseJsDataResult {
    PARSE_JS_SUCCESS = 0,
    PARSE_JS_ERR_UNSUPPORTED_TYPE = 101,
    PARSE_JS_ERR_TOO_MANY_PARAM = 102,
};

class QuickJsGroupJsBridge : public GroupJsBridge {
    DECLARE_ACE_TYPE(QuickJsGroupJsBridge, GroupJsBridge)

public:
    QuickJsGroupJsBridge() = default;
    ~QuickJsGroupJsBridge() override = default;

    int32_t InitializeGroupJsBridge(JSContext* jsContext);
    void Uninitialize();

    void TriggerModuleJsCallback(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) override;
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void GetRequestData(JSContext* ctx, JSValueConst valObject, OHOS::Ace::RequestData& requestData);

    ParseJsDataResult ParseRequestData(
        JSContext* ctx, int32_t argc, JSValueConst* argv, OHOS::Ace::RequestData& requestData, int32_t requestId);

    void TriggerModuleJsCallbackPreview(
        int32_t callbackId, int32_t code, OHOS::Ace::ResponseData responseData) override;
#endif
    void TriggerModulePluginGetErrorCallback(
        int32_t callbackId, int32_t errorCode, std::string&& errorMessage) override;

    void TriggerEventJsCallback(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) override;

    void LoadPluginJsCode(std::string&& jsCode) override;

    void Destroy() override {}

    void LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) override;

private:
    int32_t GetPendingCallbackIdAndIncrement()
    {
        return pendingCallbackId_++;
    }

    // JS bridge functions are used for the mapping between the JS and CPP functions.
    int32_t LoadJsBridgeFunction();

    // load the js source code work on qjs engine
    int32_t CallEvalBuf(JSContext* ctx, const char* buf, size_t bufLen, const char* filename, int32_t evalFlags);

    bool SetModuleGroupCallbackFuncs(JSContext* ctx, const JSValue& resolveCallback,
        const JSValue& rejectCallback, int32_t callbackId);
    bool SetEventGroupCallBackFuncs(JSContext* ctx, const JSValue& eventCallbackFunc, int32_t callbackId,
        int32_t requestId);
    void RemoveEventGroupCallBackFuncs(int32_t callbackId);
    void AddRequestIdCallbackIdRelation(int32_t callbackId, int32_t requestId);
    void RemoveRequestIdCallbackIdRelation(int32_t requestId, bool removeEventCallback);

    void CallModuleJsCallback(int32_t callbackId, int32_t code, JSValue callBackResult);
    void CallEventJsCallback(int32_t callbackId, std::vector<uint8_t>&& data);

    // process when parse data from js engine failed
    static void ProcessParseJsError(ParseJsDataResult errorType, JSContext* ctx, int32_t callbackId);
    ParseJsDataResult ParseJsPara(JSContext* ctx, int32_t argc, JSValueConst* argv, std::vector<CodecData>& args,
        int32_t requestId, int32_t beginIndex);

    static void QuickJsPrintException(JSContext* context, int32_t expLine, int32_t line);
    static bool QuickJsCheckException(JSContext* context, JSValueConst object, int32_t line);
    static int32_t QuickJsGetLengthIfObjectIsArray(JSContext* ctx, JSValueConst obj);
    static std::string SerializationObjectToString(JSContext* ctx, JSValue& val);

    // JsSendGroupMessage is mapped to the JS function[SendGroupMessage]
    // contains three parameters: groupName, message, callbackfunction
    static JSValue ProcessJsRequest(JSContext* ctx, JSValueConst thisVal, int32_t argc, JSValueConst* argv);
    // JsSendGroupMessage is mapped to the JS function[SendGroupMessage]
    // contains three parameters: groupName, message, callbackfunction
    static JSValue ProcessJsRequestSync(JSContext* ctx, JSValueConst thisVal, int32_t argc, JSValueConst* argv);

    // JsSetEventMsgHandler is mapped to the JS function[SetEventMsgHandler]
    // contains two parameters: groupName, eventCallbackFunction
    static JSValue JsSetEventMessageHandler(JSContext* ctx, JSValueConst thisVal, int32_t argc, JSValueConst* argv);

    EventCallbackMap eventCallBackFuncs_;
    ModuleCallbackMap moduleCallBackFuncs_;
    RequestIdCallbackIdMap requestIdCallbackIdMap_;

    JSContext* context_ = nullptr;
    std::atomic_int pendingCallbackId_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_QJS_GROUP_JS_BRIDGE_H
