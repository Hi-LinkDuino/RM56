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

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_group_js_bridge.h"

#include "base/json/json_util.h"
#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "core/common/ace_page.h"
#include "core/common/js_message_dispatcher.h"
#include "frameworks/bridge/codec/function_call.h"
#include "frameworks/bridge/declarative_frontend/engine/js_execution_scope_defines.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_module_manager.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_types.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_utils.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"

namespace OHOS::Ace::Framework {
namespace {

const int32_t PLUGIN_REQUEST_MIN_ARGC_NUM = 4;
const int32_t PLUGIN_REQUEST_ARG_RESOLVE_INDEX = 0;
const int32_t PLUGIN_REQUEST_ARG_REJECT_INDEX = 1;
const int32_t PLUGIN_REQUEST_ARG_GROUP_NAME_INDEX = 2;
const int32_t PLUGIN_REQUEST_ARG_FUNCTION_NAME_INDEX = 3;
const int32_t PLUGIN_REQUEST_ARG_APP_PARAMS_INDEX = 4;

const int32_t PLUGIN_REQUEST_MIN_ARGC_NUM_SYNC = 2;
const int32_t PLUGIN_REQUEST_ARG_GROUP_NAME_INDEX_SYNC = 0;
const int32_t PLUGIN_REQUEST_ARG_FUNCTION_NAME_INDEX_SYNC = 1;
const int32_t PLUGIN_REQUEST_ARG_APP_PARAMS_INDEX_SYNC = 2;

} // namespace

int32_t V8DeclarativeGroupJsBridge::InitializeGroupJsBridge(v8::Local<v8::Context> context)
{
    LOGI("Enter InitializeGroupJsBridge");
    if (context.IsEmpty()) {
        LOGE("group module init, context is null");
        EventReport::SendAPIChannelException(APIChannelExcepType::JS_BRIDGE_INIT_ERR);
        return JS_CALL_FAIL;
    }

    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    if (isolate_ == nullptr) {
        isolate_ = isolate;
        context_.Reset(isolate_, context);
    }

    if (LoadJsBridgeFunction(isolate) != JS_CALL_SUCCESS) {
        LOGE("group module init, load bridge function failed!");
        EventReport::SendAPIChannelException(APIChannelExcepType::JS_BRIDGE_INIT_ERR);
        return JS_CALL_FAIL;
    }

    return JS_CALL_SUCCESS;
}

int32_t V8DeclarativeGroupJsBridge::LoadJsBridgeFunction(v8::Isolate* isolate)
{
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::Object> group = v8::Object::New(isolate);

    bool succ = group->Set(context, v8::String::NewFromUtf8(isolate, "sendGroupMessage").ToLocalChecked(),
        v8::Function::New(context, ProcessJsRequest).ToLocalChecked()).ToChecked();
    if (!succ) {
        LOGE("bridge function, set group message sending function mapping failed!");
        EventReport::SendAPIChannelException(APIChannelExcepType::SET_FUNCTION_ERR);
        return JS_CALL_FAIL;
    }

    succ = group->Set(context, v8::String::NewFromUtf8(isolate, "sendGroupMessageSync").ToLocalChecked(),
        v8::Function::New(context, ProcessJsRequestSync).ToLocalChecked()).ToChecked();
    if (!succ) {
        LOGE("bridge function, set sync group message sending function mapping failed!");
        EventReport::SendAPIChannelException(APIChannelExcepType::SET_FUNCTION_ERR);
        return JS_CALL_FAIL;
    }

    succ = global->Set(context, v8::String::NewFromUtf8(isolate, "group").ToLocalChecked(), group).ToChecked();
    if (!succ) {
        LOGE("bridge function, set root node failed!");
        EventReport::SendAPIChannelException(APIChannelExcepType::SET_FUNCTION_ERR);
        return JS_CALL_FAIL;
    }

    return JS_CALL_SUCCESS;
}

int32_t V8DeclarativeGroupJsBridge::CallEvalBuf(v8::Isolate* isolate, v8::Local<v8::String> src)
{
    if (isolate == nullptr) {
        LOGE("isolate can't be null!");
        return JS_CALL_FAIL;
    }

    v8::HandleScope handleScope(isolate);
    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    v8::Local<v8::Script> script;
    if (!v8::Script::Compile(context, src).ToLocal(&script)) {
        LOGE("compile js code failed!");
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch);
        return JS_CALL_FAIL;
    }

    v8::Local<v8::Value> res;
    if (!script->Run(context).ToLocal(&res)) {
        LOGE("run js code failed!");
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch);
        return JS_CALL_FAIL;
    }
    return JS_CALL_SUCCESS;
}

// function callback for groupObj's function: sendGroupMessage
void V8DeclarativeGroupJsBridge::ProcessJsRequest(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    if (delegate == nullptr) {
        LOGE("send message para check, fail to get front-end delegate");
        return;
    }

    auto groupJsBridge = AceType::DynamicCast<V8DeclarativeGroupJsBridge>((*delegate)->GetGroupJsBridge());
    if (groupJsBridge == nullptr) {
        LOGE("send message para check, fail to get group-js-bridge");
        return;
    }

    // Should have at least 4 parameters
    if (args.Length() < PLUGIN_REQUEST_MIN_ARGC_NUM) {
        LOGE("send message para check, invalid args number:%{public}d", args.Length());
        return;
    }

    int32_t callbackId = groupJsBridge->GetPendingCallbackIdAndIncrement();
    groupJsBridge->AddCallbackIdIsolateRelation(callbackId, isolate);
    if (!groupJsBridge->SetModuleGroupCallbackFuncs(
        args, PLUGIN_REQUEST_ARG_RESOLVE_INDEX, PLUGIN_REQUEST_ARG_REJECT_INDEX, callbackId)) {
        LOGE("send message, set module callback function failed!");
        return;
    }

    v8::String::Utf8Value jsGroupName(isolate, args[PLUGIN_REQUEST_ARG_GROUP_NAME_INDEX]);
    const char* groupName = *jsGroupName;
    if (groupName == nullptr) {
        groupJsBridge->TriggerModulePluginGetErrorCallback(
            callbackId, PLUGIN_REQUEST_FAIL, "plugin name can't be null");
        LOGE("plugin name is null");
        return;
    }

    LOGI("send message, groupName: %{private}s, callbackId: %{private}d", groupName, callbackId);
    std::string strGroupName(groupName);

    v8::String::Utf8Value jsFunctionName(isolate, args[PLUGIN_REQUEST_ARG_FUNCTION_NAME_INDEX]);
    const char* functionName = *jsFunctionName;
    std::string strFunctionName(functionName);

    std::vector<CodecData> arguments;
    ParseJsDataResult parseJsResult =
        groupJsBridge->ParseJsPara(args, PLUGIN_REQUEST_ARG_APP_PARAMS_INDEX, callbackId, arguments);
    if (parseJsResult != ParseJsDataResult::PARSE_JS_SUCCESS) {
        ProcessParseJsError(parseJsResult, isolate, callbackId);
        return;
    }

    FunctionCall functionCall(strFunctionName, arguments);
    StandardFunctionCodec codec;
    std::vector<uint8_t> encodeBuf;
    if (!codec.EncodeFunctionCall(functionCall, encodeBuf)) {
        groupJsBridge->TriggerModulePluginGetErrorCallback(
            callbackId, PLUGIN_REQUEST_FAIL, "encode request message failed");
        return;
    }

    // CallPlatformFunction
    auto dispatcher =
        static_cast<WeakPtr<JsMessageDispatcher>*>(isolate->GetData(V8DeclarativeEngineInstance::DISPATCHER));
    if (dispatcher == nullptr) {
        LOGE("dispatcher is nullptr");
        return;
    }
    auto dispatcherUpgrade = (*dispatcher).Upgrade();
    if (dispatcherUpgrade != nullptr) {
        dispatcherUpgrade->Dispatch(strGroupName, std::move(encodeBuf), callbackId);
    } else {
        LOGW("Dispatcher Upgrade fail when dispatch request mesaage to platform");
        groupJsBridge->TriggerModulePluginGetErrorCallback(callbackId, PLUGIN_REQUEST_FAIL, "send message failed");
    }
}

// function callback for groupObj's function: sendGroupMessageSync
void V8DeclarativeGroupJsBridge::ProcessJsRequestSync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    if (delegate == nullptr) {
        LOGE("send message para check, fail to get front-end delegate");
        return;
    }

    auto groupJsBridge = AceType::DynamicCast<V8DeclarativeGroupJsBridge>((*delegate)->GetGroupJsBridge());
    if (groupJsBridge == nullptr) {
        LOGE("send message para check, fail to get group-js-bridge");
        return;
    }

    // Should have at least 2 parameters
    if (args.Length() < PLUGIN_REQUEST_MIN_ARGC_NUM_SYNC) {
        LOGE("send message para check, invalid args number:%{public}d", args.Length());
        return;
    }

    v8::String::Utf8Value jsGroupName(isolate, args[PLUGIN_REQUEST_ARG_GROUP_NAME_INDEX_SYNC]);
    const char* groupName  = *jsGroupName;
    if (groupName == nullptr) {
        LOGE("plugin name is null");
        return;
    }
    std::string strGroupName(groupName);

    v8::String::Utf8Value jsFunctionName(isolate, args[PLUGIN_REQUEST_ARG_FUNCTION_NAME_INDEX_SYNC]);
    const char* functionName = *jsFunctionName;
    std::string strFunctionName(functionName);
    LOGI("send message, groupName: %{private}s, functionName: %{private}s", groupName, functionName);

    std::vector<CodecData> arguments;
    ParseJsDataResult parseJsResult =
        groupJsBridge->ParseJsPara(args, PLUGIN_REQUEST_ARG_APP_PARAMS_INDEX_SYNC, 0, arguments);
    if (parseJsResult != ParseJsDataResult::PARSE_JS_SUCCESS) {
        LOGE("parse js data error");
        return;
    }

    FunctionCall functionCall(strFunctionName, arguments);
    StandardFunctionCodec codec;
    std::vector<uint8_t> encodeBuf;
    if (!codec.EncodeFunctionCall(functionCall, encodeBuf)) {
        LOGE("encode arguments fail");
        return;
    }

    // CallPlatformFunction
    auto dispatcher =
        static_cast<WeakPtr<JsMessageDispatcher>*>(isolate->GetData(V8DeclarativeEngineInstance::DISPATCHER));
    if (dispatcher == nullptr) {
        LOGE("dispatcher is nullptr");
        return;
    }
    auto dispatcherUpgrade = (*dispatcher).Upgrade();

    uint8_t* resData = nullptr;
    int64 position = 0;

    if (dispatcherUpgrade != nullptr) {
        dispatcherUpgrade->DispatchSync(strGroupName, std::move(encodeBuf), &resData, position);
    } else {
        LOGW("Dispatcher Upgrade fail when dispatch request mesaage to platform");
        return;
    }

    std::vector<uint8_t> messageData = std::vector<uint8_t>(resData, resData + position);

    v8::Local<v8::Value> callBackResult;
    CodecData codecResult;
    if (codec.DecodePlatformMessage(messageData, codecResult)) {
        std::string resultString = codecResult.GetStringValue();
        LOGI("sync resultString = %{private}s", resultString.c_str());
        if (resultString.empty()) {
            callBackResult = v8::Null(isolate);
        } else {
            callBackResult = v8::String::NewFromUtf8(isolate, resultString.c_str()).ToLocalChecked();
        }
        args.GetReturnValue().Set(callBackResult);
    }
}

bool V8DeclarativeGroupJsBridge::SetEventGroupCallBackFuncs(
    v8::Isolate* isolate, v8::Local<v8::Value> localEventCallbackFunc, int32_t callbackId, int32_t requestId)
{
    if (localEventCallbackFunc->IsNull() || !localEventCallbackFunc->IsFunction()) {
        LOGE("callback function is invalid!");
        return false;
    }

    LOGI("record event callback, requestId:%{private}d, callbackId:%{private}d", requestId, callbackId);
    v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> eventCallbackFunc;
    eventCallbackFunc.Reset(isolate, localEventCallbackFunc);
    auto result = eventCallBackFuncs_.try_emplace(callbackId, eventCallbackFunc);
    if (!result.second) {
        result.first->second.Reset();
        result.first->second.Reset(isolate, localEventCallbackFunc);
    }

    AddRequestIdCallbackIdRelation(callbackId, requestId);

    return true;
}

void V8DeclarativeGroupJsBridge::RemoveEventGroupCallBackFuncs(int32_t callbackId)
{
    LOGI("remove event callback, callbackId:%{private}d", callbackId);
    auto itFunc = eventCallBackFuncs_.find(callbackId);
    if (itFunc != eventCallBackFuncs_.end()) {
        itFunc->second.Reset();
        eventCallBackFuncs_.erase(callbackId);
    }
}

void V8DeclarativeGroupJsBridge::AddRequestIdCallbackIdRelation(int32_t eventId, int32_t requestId)
{
    std::lock_guard<std::mutex> lock(requestIdCallbackIdMapMutex_);
    auto result = requestIdCallbackIdMap_.try_emplace(requestId, eventId);
    if (!result.second) {
        result.first->second = eventId;
    }
}

void V8DeclarativeGroupJsBridge::RemoveRequestIdCallbackIdRelation(int32_t requestId, bool removeEventCallback)
{
    requestIdCallbackIdMapMutex_.lock();
    auto eventId = requestIdCallbackIdMap_.find(requestId);
    requestIdCallbackIdMapMutex_.unlock();
    if (eventId != requestIdCallbackIdMap_.end()) {
        if (removeEventCallback) {
            RemoveEventGroupCallBackFuncs(eventId->second);
        }
        requestIdCallbackIdMapMutex_.lock();
        requestIdCallbackIdMap_.erase(requestId);
        requestIdCallbackIdMapMutex_.unlock();
    }
}

void V8DeclarativeGroupJsBridge::AddCallbackIdIsolateRelation(int32_t callbackId, v8::Isolate* isolate)
{
    std::lock_guard<std::mutex> lock(callbackIdIsolateMapMutex_);
    auto result = callbackIdIsolateMap_.try_emplace(callbackId, isolate);
    if (!result.second) {
        result.first->second = isolate;
    }
    LOGD("callbackIdIsolateMap size = %{private}zu", callbackIdIsolateMap_.size());
}

void V8DeclarativeGroupJsBridge::AddIsolateNativeEngineRelation(v8::Isolate* isolate, NativeEngine* nativeEngine)
{
    if (isolate == nullptr || nativeEngine == nullptr) {
        LOGE("isolate or nativeEngine is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(isolateNativeEngineMapMutex_);
    auto result = isolateNativeEngineMap_.try_emplace(isolate, nativeEngine);
    if (!result.second) {
        result.first->second = nativeEngine;
    }
    LOGD("isolateNativeEngineMap size = %{private}zu", isolateNativeEngineMap_.size());
}

void V8DeclarativeGroupJsBridge::ProcessParseJsError(
    ParseJsDataResult errorType, v8::Isolate* isolate, int32_t callbackId)
{
    // PluginErrorCallback
    auto dispatcher =
        static_cast<WeakPtr<JsMessageDispatcher>*>(isolate->GetData(V8DeclarativeEngineInstance::DISPATCHER));
    if (dispatcher == nullptr) {
        LOGE("dispatcher is nullptr");
        return;
    }
    auto dispatcherUpgrade = (*dispatcher).Upgrade();
    if (dispatcherUpgrade == nullptr) {
        LOGW("Dispatcher Upgrade fail at ProcessParseJsError");
        return;
    }
    std::string errMessage;
    switch (errorType) {
        case ParseJsDataResult::PARSE_JS_ERR_UNSUPPORTED_TYPE:
            errMessage = "unsupported js parameter types";
            dispatcherUpgrade->DispatchPluginError(callbackId,
                static_cast<int32_t>(ParseJsDataResult::PARSE_JS_ERR_UNSUPPORTED_TYPE), std::move(errMessage));
            break;
        case ParseJsDataResult::PARSE_JS_ERR_TOO_MANY_PARAM:
            errMessage = "the number of parameters exceeds 255";
            dispatcherUpgrade->DispatchPluginError(callbackId,
                static_cast<int32_t>(ParseJsDataResult::PARSE_JS_ERR_TOO_MANY_PARAM), std::move(errMessage));
            break;
        default:
            break;
    }
}

bool V8DeclarativeGroupJsBridge::SetModuleGroupCallbackFuncs(const v8::FunctionCallbackInfo<v8::Value>& args,
    int32_t resolveCallbackIndex, int32_t rejectCallbackIndex, int32_t callbackId)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    LOGD("Enter SetModuleGroupCallbackFuncs");
    if (args[resolveCallbackIndex]->IsNull() || !args[resolveCallbackIndex]->IsFunction() ||
        args[rejectCallbackIndex]->IsNull() || !args[rejectCallbackIndex]->IsFunction()) {
        LOGE("resolve or reject callback function is invalid");
        return false;
    }

    PromiseCallback promiseCallJsFunc;

    promiseCallJsFunc.resolveCallback.Reset(isolate, args[resolveCallbackIndex]);
    promiseCallJsFunc.rejectCallback.Reset(isolate, args[rejectCallbackIndex]);

    moduleCallbackMapMutex_.lock();
    auto result = moduleCallBackFuncs_.try_emplace(callbackId, promiseCallJsFunc);
    moduleCallbackMapMutex_.unlock();
    if (!result.second) {
        LOGE("module callback function has been existed!");
        return false;
    }
    return true;
}

std::string V8DeclarativeGroupJsBridge::SerializationObjectToString(
    v8::Local<v8::Context> context, v8::Local<v8::Value> val)
{
    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::Value> strValue;
    v8::Local<v8::Value> global;
    v8::Local<v8::Value> json;
    v8::Local<v8::Value> func;

    global = context->Global();
    if (!global->IsObject()) {
        LOGE("SerializationObjectToString error: fail to get Global Object");
        return "";
    }
    v8::Local<v8::Object> globalObj = global->ToObject(context).ToLocalChecked();
    json = globalObj->Get(context, v8::String::NewFromUtf8(isolate, "JSON").ToLocalChecked()).ToLocalChecked();
    if (!json->IsObject()) {
        LOGE("SerializationObjectToString error: global has no attribute JSON");
        return "";
    }
    v8::Local<v8::Object> jsonObj = json->ToObject(context).ToLocalChecked();
    func = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "stringify").ToLocalChecked()).ToLocalChecked();
    if (!func->IsFunction()) {
        LOGE("SerializationObjectToString error: JSON has no attribute stringify");
        return "";
    }
    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(func);
    v8::Local<v8::Value> argv[] = { val };
    bool succ = function->Call(context, globalObj, 1, argv).ToLocal(&strValue);
    if (!succ) {
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch);
        return "";
    }

    v8::String::Utf8Value resStr(isolate, strValue);
    std::string res;
    if (*resStr) {
        res = *resStr;
    }
    return res;
}

ParseJsDataResult V8DeclarativeGroupJsBridge::ParseJsPara(const v8::FunctionCallbackInfo<v8::Value>& args,
    int32_t beginIndex, int32_t requestId, std::vector<CodecData>& arguments)
{
    if (args.Length() < beginIndex) { // no others params
        return ParseJsDataResult::PARSE_JS_SUCCESS;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    for (int32_t i = beginIndex; i < args.Length(); i++) {
        v8::Local<v8::Value> val = args[i];
        if (val->IsString()) {
            v8::String::Utf8Value jsPara(isolate, val);
            if (*jsPara) {
                std::string para(*jsPara);
                CodecData arg(para);
                arguments.push_back(arg);
            }
        } else if (val->IsNumber()) {
            if (val->IsInt32()) {
                int32_t valInt = val->Int32Value(context).ToChecked();
                CodecData arg(valInt);
                arguments.push_back(arg);
            } else {
                double valDouble = val->NumberValue(context).ToChecked();
                CodecData arg(valDouble);
                arguments.push_back(arg);
            }
        } else if (val->IsBoolean()) {
            bool valBool = val->BooleanValue(isolate);
            CodecData arg(valBool);
            arguments.push_back(arg);
        } else if (val->IsNull()) {
            CodecData argNull;
            arguments.push_back(argNull);
        } else if (val->IsFunction()) {
            int32_t functionId = GetPendingCallbackIdAndIncrement();
            CodecData arg(functionId, BufferDataType::TYPE_FUNCTION);
            arguments.push_back(arg);
            SetEventGroupCallBackFuncs(isolate, val, functionId, requestId);
        } else if (val->IsArray() || val->IsObject()) {
            std::string objStr = SerializationObjectToString(context, val);
            CodecData arg(objStr, BufferDataType::TYPE_OBJECT);
            arguments.push_back(arg);
        } else if (val->IsUndefined()) {
            LOGD("Process callNative para type:undefined");
        } else {
            LOGE("Process callNative para type: unsupported type");
            return ParseJsDataResult::PARSE_JS_ERR_UNSUPPORTED_TYPE;
        }
    }
    return ParseJsDataResult::PARSE_JS_SUCCESS;
}

void V8DeclarativeGroupJsBridge::TriggerModuleJsCallback(
    int32_t callbackId, int32_t code, std::vector<uint8_t>&& messageData)
{
    LOGD("TriggerModuleJsCallback in");
    CodecData codecResult;
    StandardFunctionCodec codec;
    std::string resultString;
    if (codec.DecodePlatformMessage(messageData, codecResult)) {
        resultString = codecResult.GetStringValue();
    } else {
        LOGE("trigger JS result function error, decode message fail, callbackId:%{private}d", callbackId);
        code = PLUGIN_REQUEST_FAIL;
        resultString = std::string("{\"code\":")
                            .append(std::to_string(code))
                            .append(",")
                            .append("\"data\":\"invalid reponse data\"}");
    }
    messageData.clear();

    v8::Isolate* isolate = isolate_;
    callbackIdIsolateMapMutex_.lock();
    auto result = callbackIdIsolateMap_.find(callbackId);
    callbackIdIsolateMapMutex_.unlock();
    if (result != callbackIdIsolateMap_.end()) {
        isolate = result->second;
        callbackIdIsolateMapMutex_.lock();
        callbackIdIsolateMap_.erase(callbackId);
        callbackIdIsolateMapMutex_.unlock();
        isolateNativeEngineMapMutex_.lock();
        auto nativeEngineResult = isolateNativeEngineMap_.find(isolate);
        isolateNativeEngineMapMutex_.unlock();
        if (nativeEngineResult != isolateNativeEngineMap_.end()) {
            // Trigger worker js callback
            NativeEngine* nativeEngine = nativeEngineResult->second;
            if (nativeEngine == nullptr) {
                LOGE("nativeEngine is nullptr");
                return;
            }
            JsCallbackData* callbackData = new JsCallbackData(callbackId, code, resultString, this, isolate);
            nativeEngine->SendAsyncWork(callbackData);
            return;
        }
    } else {
        LOGE("callbackIdIsolateMap find failed");
    }

    TriggerModuleJsCallback(callbackId, code, resultString, isolate);
}

void V8DeclarativeGroupJsBridge::TriggerModuleJsCallback(int32_t callbackId, int32_t code,
    std::string result, v8::Isolate* isolate)
{
    v8::HandleScope handleScope(isolate);
    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::Value> callBackResult;
    if (result.empty()) {
        callBackResult = v8::Null(isolate);
    } else {
        callBackResult = v8::String::NewFromUtf8(isolate, result.c_str()).ToLocalChecked();
    }
    CallModuleJsCallback(callbackId, code, callBackResult, isolate);
}

void V8DeclarativeGroupJsBridge::NativeAsyncExecuteCallback(NativeEngine* engine, void* data)
{
    LOGD("NativeAsyncExecuteCallback in");
    auto callbackData = static_cast<JsCallbackData*>(data);
    if (callbackData == nullptr) {
        LOGE("callbackData is nullptr");
        return;
    }
    if (callbackData->groupJsBridge == nullptr) {
        LOGE("callbackData groupJsBridge is nullptr");
        return;
    }
    callbackData->groupJsBridge->TriggerModuleJsCallback(callbackData->callbackId, callbackData->code,
        callbackData->result, callbackData->isolate);
}

void V8DeclarativeGroupJsBridge::NativeAsyncCompleteCallback(NativeEngine* engine, int status, void* data)
{
    LOGD("NativeAsyncCompleteCallback in");
    auto callbackData = static_cast<JsCallbackData*>(data);
    if (callbackData == nullptr) {
        LOGE("callbackData is nullptr in CompleteCallback");
        return;
    }
    delete callbackData;
    callbackData = nullptr;
}

void V8DeclarativeGroupJsBridge::CallModuleJsCallback(
    int32_t callbackId, int32_t code, v8::Local<v8::Value> callBackResult, v8::Isolate* isolate)
{
    LOGD("CallModuleJsCallback in");
    RemoveRequestIdCallbackIdRelation(callbackId, code != PLUGIN_REQUEST_SUCCESS);

    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> global = context->Global();

    v8::TryCatch tryCatch(isolate);

    moduleCallbackMapMutex_.lock();
    auto itFunc = moduleCallBackFuncs_.find(callbackId);
    moduleCallbackMapMutex_.unlock();
    if (itFunc != moduleCallBackFuncs_.end()) {
        v8::Local<v8::Value> jsFunc = (code == PLUGIN_REQUEST_SUCCESS ? itFunc->second.resolveCallback.Get(isolate)
                                                                      : itFunc->second.rejectCallback.Get(isolate));
        if (!jsFunc->IsFunction() || jsFunc->IsNull()) {
            LOGE("trigger JS result function error, it is not a function, callbackId:%{private}d", callbackId);
            return;
        }

        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(jsFunc);
        v8::Local<v8::Value> argv[] = { callBackResult };
        // Pass only 1 parameter, call promise resolve call back.
        v8::Local<v8::Value> res;
        v8::Persistent<v8::Context> persistentContext;
        persistentContext.Reset(isolate, context);
        auto executionContext = V8ExecutionContext { isolate, persistentContext };
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK_THREAD(executionContext);
        bool succ = func->Call(context, global, 1, argv).ToLocal(&res);
        if (succ) {
            LOGI("trigger JS result function success, callbackId:%{private}d, code:%{private}d", callbackId, code);
        } else {
            LOGW("trigger JS result function fail, callbackId:%{private}d, code:%{private}d", callbackId, code);
            V8Utils::JsStdDumpErrorAce(isolate, &tryCatch);
        }

        while (v8::platform::PumpMessageLoop(V8DeclarativeEngine::GetPlatform().get(), isolate)) {
            continue;
        }

        itFunc->second.rejectCallback.Reset();
        itFunc->second.resolveCallback.Reset();
        moduleCallbackMapMutex_.lock();
        moduleCallBackFuncs_.erase(itFunc);
        moduleCallbackMapMutex_.unlock();
    } else {
        LOGE("trigger JS result function is not exists, callbackId:%{private}d, code:%{private}d", callbackId, code);
    }
}

void V8DeclarativeGroupJsBridge::TriggerModulePluginGetErrorCallback(
    int32_t callbackId, int32_t errorCode, std::string&& errorMessage)
{
    LOGD("TriggerModulePluginGetErrorCallback in");
    RemoveRequestIdCallbackIdRelation(callbackId, true);

    auto resultJson = JsonUtil::Create(true);
    resultJson->Put(std::string("code").c_str(), errorCode);
    resultJson->Put(std::string("data").c_str(), errorMessage.c_str());

    v8::Isolate* isolate = isolate_;
    callbackIdIsolateMapMutex_.lock();
    auto result = callbackIdIsolateMap_.find(callbackId);
    callbackIdIsolateMapMutex_.unlock();
    if (result != callbackIdIsolateMap_.end()) {
        isolate = result->second;
        callbackIdIsolateMapMutex_.lock();
        callbackIdIsolateMap_.erase(callbackId);
        callbackIdIsolateMapMutex_.unlock();
        isolateNativeEngineMapMutex_.lock();
        auto nativeEngineResult = isolateNativeEngineMap_.find(isolate);
        isolateNativeEngineMapMutex_.unlock();
        if (nativeEngineResult != isolateNativeEngineMap_.end()) {
            // Trigger worker js callback
            NativeEngine* nativeEngine = nativeEngineResult->second;
            if (nativeEngine == nullptr) {
                LOGE("nativeEngine is nullptr");
                return;
            }
            JsCallbackData* callbackData = new JsCallbackData(callbackId, errorCode,
                resultJson->ToString(), this, isolate);
            nativeEngine->SendAsyncWork(callbackData);
            return;
        }
    } else {
        LOGE("callbackIdIsolateMap find failed");
    }

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> global = context->Global();

    moduleCallbackMapMutex_.lock();
    auto itFunc = moduleCallBackFuncs_.find(callbackId);
    moduleCallbackMapMutex_.unlock();
    if (itFunc != moduleCallBackFuncs_.end()) {
        v8::Local<v8::Value> jsFunc = itFunc->second.rejectCallback.Get(isolate);
        if (!jsFunc->IsFunction() || jsFunc->IsNull()) {
            LOGE("trigger JS result function error, reject is not a function, callbackId:%{private}d", callbackId);
            return;
        }

        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(jsFunc);
        v8::Local<v8::String> emptyReplyCallback =
            v8::String::NewFromUtf8(isolate, resultJson->ToString().c_str()).ToLocalChecked();
        v8::Local<v8::Value> argv[] = { emptyReplyCallback };
        // Pass only 1 parameter, call promise reject call back for error get in plugin.
        v8::Local<v8::Value> res;
        v8::Persistent<v8::Context> persistentContext;
        persistentContext.Reset(isolate, context);
        auto executionContext = V8ExecutionContext { isolate, persistentContext };
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK_THREAD(executionContext);
        bool succ = func->Call(context, global, 1, argv).ToLocal(&res);
        if (succ) {
            LOGI("trigger JS result function success, callbackId:%{private}d", callbackId);
        } else {
            LOGW("trigger JS result function fail, callbackId:%{private}d", callbackId);
        }

        while (v8::platform::PumpMessageLoop(V8DeclarativeEngine::GetPlatform().get(), isolate)) {
            continue;
        }

        itFunc->second.rejectCallback.Reset();
        itFunc->second.resolveCallback.Reset();
        moduleCallbackMapMutex_.lock();
        moduleCallBackFuncs_.erase(itFunc);
        moduleCallbackMapMutex_.unlock();
    } else {
        LOGE("trigger JS result function is not exists, callbackId:%{private}d", callbackId);
    }
}

void V8DeclarativeGroupJsBridge::CallEventJsCallback(int32_t callbackId, std::vector<uint8_t>&& eventData)
{
    v8::Isolate* isolate = isolate_;
    callbackIdIsolateMapMutex_.lock();
    auto result = callbackIdIsolateMap_.find(callbackId);
    callbackIdIsolateMapMutex_.unlock();
    if (result != callbackIdIsolateMap_.end()) {
        isolate = result->second;
    } else {
        LOGE("callbackIdIsolateMap find failed");
    }

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> global = context->Global();

    v8::Local<v8::Value> callBackEvent;
    CodecData codecEvent;
    StandardFunctionCodec codec;
    if (codec.DecodePlatformMessage(eventData, codecEvent)) {
        std::string eventString = codecEvent.GetStringValue();
        if (eventString.empty()) {
            callBackEvent = v8::Null(isolate);
        } else {
            callBackEvent = v8::String::NewFromUtf8(isolate, eventString.c_str()).ToLocalChecked();
        }
    } else {
        LOGE("trigger JS callback function error, decode message fail, callbackId:%{private}d", callbackId);
        return;
    }

    auto itFunc = eventCallBackFuncs_.find(callbackId);
    if (itFunc != eventCallBackFuncs_.end()) {
        v8::Local<v8::Value> jsFunc = itFunc->second.Get(isolate);
        if (!jsFunc->IsFunction() || jsFunc->IsNull()) {
            LOGE("trigger JS callback function error, callback is not a function, callbackId:%{private}d", callbackId);
            return;
        }

        // Pass only 1 parameter
        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(jsFunc);
        v8::Local<v8::Value> argv[] = { callBackEvent };
        v8::Local<v8::Value> res;
        v8::Persistent<v8::Context> persistentContext;
        persistentContext.Reset(isolate, context);
        auto executionContext = V8ExecutionContext { isolate, persistentContext };
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK_THREAD(executionContext);
        bool succ = func->Call(context, global, 1, argv).ToLocal(&res);
        if (!succ) {
            LOGW("trigger JS callback function failed, callbackId:%{private}d", callbackId);
        } else {
            LOGW("trigger JS callback function success, callbackId:%{private}d", callbackId);
        }

        while (v8::platform::PumpMessageLoop(V8DeclarativeEngine::GetPlatform().get(), isolate)) {
            continue;
        }
    } else {
        LOGE("trigger JS callback function error, it is not exists, callbackId:%{private}d", callbackId);
    }
    eventData.clear();
}

void V8DeclarativeGroupJsBridge::TriggerEventJsCallback(
    int32_t callbackId, int32_t code, std::vector<uint8_t>&& eventData)
{
    LOGD("TriggerEventJsCallback in");
    if (code == PLUGIN_CALLBACK_DESTROY) {
        RemoveEventGroupCallBackFuncs(callbackId);
    } else {
        CallEventJsCallback(callbackId, std::move(eventData));
    }
}

void V8DeclarativeGroupJsBridge::LoadPluginJsCode(std::string&& jsCode)
{
    LOGI("Load plugin js code, code len:%{public}d", static_cast<int32_t>(jsCode.length()));
    jsCode_ = std::move(jsCode);
    v8::HandleScope handleScope(isolate_);
    v8::Local<v8::Context> context = context_.Get(isolate_);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_, jsCode_.c_str()).ToLocalChecked();
    if (CallEvalBuf(isolate_, source) != JS_CALL_SUCCESS) {
        LOGE("run plugin js code failed!");
    }
}

void V8DeclarativeGroupJsBridge::LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen)
{
    LOGW("V8 do not support load js bytecode now.");
}

void V8DeclarativeGroupJsBridge::Destroy(v8::Isolate* isolate, bool isWorker)
{
    if (!isWorker) {
        eventCallBackFuncs_.clear();
        requestIdCallbackIdMap_.clear();
        context_.Reset();
    }

    ModuleManager::GetInstance()->ClearTimerIsolate(isolate);
    DestroyModuleCallbackMap(isolate);
    DestroyCallbackIdIsolateMap(isolate);
    DestroyIsolateNativeEngineMap(isolate);
}

void V8DeclarativeGroupJsBridge::GetCallbackId(v8::Isolate* isolate, std::set<int32_t>& callbackIdSet)
{
    std::lock_guard<std::mutex> lock(callbackIdIsolateMapMutex_);
    for (auto iter = callbackIdIsolateMap_.begin(); iter != callbackIdIsolateMap_.end(); ++iter) {
        if (iter->second == isolate) {
            callbackIdSet.emplace(iter->first);
        }
    }
}

void V8DeclarativeGroupJsBridge::DestroyModuleCallbackMap(v8::Isolate* isolate)
{
    std::set<int32_t> callbackIdSet;
    GetCallbackId(isolate, callbackIdSet);

    std::lock_guard<std::mutex> lock(moduleCallbackMapMutex_);
    for (auto iter = moduleCallBackFuncs_.begin(); iter != moduleCallBackFuncs_.end();) {
        if (callbackIdSet.find(iter->first) != callbackIdSet.end()) {
            iter = moduleCallBackFuncs_.erase(iter);
        } else {
            ++iter;
        }
    }
}

void V8DeclarativeGroupJsBridge::DestroyCallbackIdIsolateMap(v8::Isolate* isolate)
{
    std::lock_guard<std::mutex> lock(callbackIdIsolateMapMutex_);
    for (auto iter = callbackIdIsolateMap_.begin(); iter != callbackIdIsolateMap_.end();) {
        if (iter->second == isolate) {
            iter = callbackIdIsolateMap_.erase(iter);
        } else {
            ++iter;
        }
    }
}

void V8DeclarativeGroupJsBridge::DestroyIsolateNativeEngineMap(v8::Isolate* isolate)
{
    std::lock_guard<std::mutex> lock(isolateNativeEngineMapMutex_);
    auto result = isolateNativeEngineMap_.find(isolate);
    if (result != isolateNativeEngineMap_.end()) {
        LOGD("remove worker isolate, isolate:%{private}p", isolate);
        isolateNativeEngineMap_.erase(isolate);
    }
}

bool V8DeclarativeGroupJsBridge::ForwardToWorker(int32_t callbackId)
{
    v8::Isolate* isolate = isolate_;
    callbackIdIsolateMapMutex_.lock();
    auto result = callbackIdIsolateMap_.find(callbackId);
    callbackIdIsolateMapMutex_.unlock();
    if (result != callbackIdIsolateMap_.end()) {
        isolate = result->second;
        isolateNativeEngineMapMutex_.lock();
        auto nativeEngineResult = isolateNativeEngineMap_.find(isolate);
        isolateNativeEngineMapMutex_.unlock();
        if (nativeEngineResult != isolateNativeEngineMap_.end()) {
            return true;
        }
    }
    return false;
}

} // namespace OHOS::Ace::Framework
