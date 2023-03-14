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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_DECLARATIVE_GROUP_JS_BRIDGE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_DECLARATIVE_GROUP_JS_BRIDGE_H

#include <map>
#include <string>

#include "third_party/v8/include/libplatform/libplatform.h"
#include "third_party/v8/include/v8.h"

#include "base/memory/ace_type.h"
#include "base/utils/singleton.h"
#include "frameworks/bridge/codec/standard_function_codec.h"
#include "frameworks/bridge/js_frontend/engine/common/group_js_bridge.h"
#include "native_engine/native_engine.h"

namespace OHOS::Ace::Framework {

struct PromiseCallback final {
    v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> resolveCallback;
    v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> rejectCallback;
};

using EventCallbackMap = std::map<int32_t, v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>>>;
using ModuleCallbackMap = std::map<int32_t, PromiseCallback>;
using RequestIdCallbackIdMap = std::map<int32_t, int32_t>;
using CallbackIdIsolateMap = std::map<int32_t, v8::Isolate*>;
using IsolateNativeEngineMap = std::map<v8::Isolate*, NativeEngine*>;

enum class ParseJsDataResult {
    PARSE_JS_SUCCESS = 0,
    PARSE_JS_ERR_UNSUPPORTED_TYPE = 101,
    PARSE_JS_ERR_TOO_MANY_PARAM = 102,
};

class V8DeclarativeGroupJsBridge : public GroupJsBridge {
    DECLARE_ACE_TYPE(V8DeclarativeGroupJsBridge, GroupJsBridge)

public:
    int32_t InitializeGroupJsBridge(v8::Local<v8::Context> context);

    void TriggerModuleJsCallback(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) override;

    void TriggerModulePluginGetErrorCallback(
        int32_t callbackId, int32_t errorCode, std::string&& errorMessage) override;

    void TriggerEventJsCallback(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) override;

    void LoadPluginJsCode(std::string&& jsCode) override;

    void LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) override;

    void Destroy() override {};

    void Destroy(v8::Isolate* isolate, bool isWorker = false);

    // load the js source code work on v8 engine
    int32_t CallEvalBuf(v8::Isolate* isolate, v8::Local<v8::String> src);

    const std::string& GetJsCode() const
    {
        return jsCode_;
    }

    static void NativeAsyncExecuteCallback(NativeEngine* engine, void* data);
    static void NativeAsyncCompleteCallback(NativeEngine* engine, int status, void* data);

    void AddIsolateNativeEngineRelation(v8::Isolate* isolate, NativeEngine* nativeEngine);

    bool ForwardToWorker(int32_t callbackId) override;

private:
    int32_t GetPendingCallbackIdAndIncrement()
    {
        return pendingCallbackId_++;
    }

    // JS bridge functions are used for the mapping between the JS and CPP functions.
    int32_t LoadJsBridgeFunction(v8::Isolate* isolate);

    bool SetModuleGroupCallbackFuncs(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t resolveCallbackIndex,
        int32_t rejectCallbackIndex, int32_t callbackId);
    bool SetEventGroupCallBackFuncs(
        v8::Isolate* isolate, v8::Local<v8::Value> localEventCallbackFunc, int32_t callbackId, int32_t requestId);
    void RemoveEventGroupCallBackFuncs(int32_t callbackId);
    void AddRequestIdCallbackIdRelation(int32_t callbackId, int32_t requestId);
    void RemoveRequestIdCallbackIdRelation(int32_t requestId, bool removeEventCallback);
    void AddCallbackIdIsolateRelation(int32_t callbackId, v8::Isolate* isolate);

    void CallModuleJsCallback(int32_t callbackId, int32_t code, v8::Local<v8::Value> callBackResult,
        v8::Isolate* isolate);
    void CallEventJsCallback(int32_t callbackId, std::vector<uint8_t>&& data);

    ParseJsDataResult ParseJsPara(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index, int32_t requestId,
        std::vector<CodecData>& arguments);

    // process when parse data from js engine failed
    static void ProcessParseJsError(ParseJsDataResult errorType, v8::Isolate* isolate, int32_t callbackId);

    static std::string SerializationObjectToString(v8::Local<v8::Context> context, v8::Local<v8::Value> val);

    // ProcessJsRequest is mapped to the JS function[SendGroupMessage]
    // contains three parameters: groupName, message, callbackfunction
    static void ProcessJsRequest(const v8::FunctionCallbackInfo<v8::Value>& args);

    // ProcessJsRequestSync is mapped to the JS function[sendGroupMessageSync]
    // contains three parameters: groupName, message, params
    static void ProcessJsRequestSync(const v8::FunctionCallbackInfo<v8::Value>& args);

    void TriggerModuleJsCallback(int32_t callbackId, int32_t code, std::string result, v8::Isolate* isolate);

    void GetCallbackId(v8::Isolate* isolate, std::set<int32_t>& callbackIdSet);
    void DestroyModuleCallbackMap(v8::Isolate* isolate);
    void DestroyCallbackIdIsolateMap(v8::Isolate* isolate);
    void DestroyIsolateNativeEngineMap(v8::Isolate* isolate);

    EventCallbackMap eventCallBackFuncs_;
    ModuleCallbackMap moduleCallBackFuncs_;
    RequestIdCallbackIdMap requestIdCallbackIdMap_;
    CallbackIdIsolateMap callbackIdIsolateMap_;
    IsolateNativeEngineMap isolateNativeEngineMap_;
    std::atomic_int pendingCallbackId_ = 1;

    v8::Isolate* isolate_ = nullptr;
    v8::Persistent<v8::Context> context_;
    std::string jsCode_;
    mutable std::mutex moduleCallbackMapMutex_;
    mutable std::mutex requestIdCallbackIdMapMutex_;
    mutable std::mutex callbackIdIsolateMapMutex_;
    mutable std::mutex isolateNativeEngineMapMutex_;
};

struct JsCallbackData final {
    explicit JsCallbackData(int32_t callbackId, int32_t code, std::string result,
        V8DeclarativeGroupJsBridge* groupJsBridge, v8::Isolate* isolate)
        : callbackId(callbackId), code(code), result(result), groupJsBridge(groupJsBridge), isolate(isolate) {}
    int32_t callbackId = -1;
    int32_t code = -1;
    std::string result;
    V8DeclarativeGroupJsBridge* groupJsBridge = nullptr;
    v8::Isolate* isolate = nullptr;
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_DECLARATIVE_GROUP_JS_BRIDGE_H
