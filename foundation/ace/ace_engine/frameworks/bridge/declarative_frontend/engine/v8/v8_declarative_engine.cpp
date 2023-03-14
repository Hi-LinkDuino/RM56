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

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"

#include <dirent.h>
#include <dlfcn.h>
#include <fstream>
#include <streambuf>
#include <unistd.h>

#include "native_engine/impl/v8/v8_native_engine.h"

#include "base/i18n/localization.h"
#include "base/json/json_util.h"
#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/utils/system_properties.h"
#include "base/utils/time_util.h"
#include "core/common/thread_checker.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/common/utils/v8/v8_helper.h"
#include "frameworks/bridge/declarative_frontend/engine/bindings_implementation.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/engine/js_types.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_bindings.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_group_js_bridge.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_function_destroy_helper.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_module_manager.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_types.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_register.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_xcomponent.h"
#include "frameworks/bridge/js_frontend/engine/common/js_api_perf.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/engine/common/runtime_constants.h"
#include "frameworks/core/common/ace_view.h"
#include "frameworks/core/common/container.h"

extern const char _binary_stateMgmt_js_start[];
extern const char _binary_stateMgmt_js_end[];
extern const char _binary_jsEnumStyle_js_start[];
extern const char _binary_jsEnumStyle_js_end[];

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t V8_MAX_STACK_SIZE = 1 * 1024 * 1024;
void* g_debugger = nullptr;
using StartDebug = void (*)(
    const std::unique_ptr<v8::Platform>& platform, const v8::Local<v8::Context>& context, std::string componentName,
    const bool isDebugMode, const int32_t instanceId);
using WaitingForIde = void (*)();
using StopDebug = void (*)();

bool CallEvalBuf(v8::Isolate* isolate, const char* src, int32_t length = -1, const char* filename = nullptr)
{
    CHECK_RUN_ON(JS);
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("CallEvalBuf, CurrentContext is empty!");
        return false;
    }
    v8::TryCatch tryCatch(isolate);
    if (src == nullptr || src[0] == '\0') {
        LOGE("src is null");
        return false;
    }

    const char* origin = filename;
    if (!origin) {
        origin = "<anonymous>";
    }

    v8::ScriptOrigin scriptOrigin(v8::String::NewFromUtf8(isolate, origin).ToLocalChecked());

    v8::Local<v8::String> source =
        v8::String::NewFromUtf8(isolate, src, v8::NewStringType::kNormal, length).ToLocalChecked();
    v8::Local<v8::Script> script;
    if (!v8::Script::Compile(context, source, &scriptOrigin).ToLocal(&script)) {
        LOGE("Compilation failed");
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch);
        return false;
    }

    v8::Local<v8::Value> res;
    if (!script->Run(context).ToLocal(&res)) {
        LOGE("Run failed");
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch);
        return false;
    }
    return true;
}

std::string ParseLogContent(const std::vector<std::string>& params)
{
    std::string ret;
    if (params.empty()) {
        return ret;
    }
    std::string formatStr = params[0];
    int32_t size = params.size();
    int32_t len = formatStr.size();
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

void GetLogLevelAndContent(const v8::FunctionCallbackInfo<v8::Value>& args, JsLogLevel& logLevel, std::string& content)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty() || args.Length() == 0) {
        LOGE("Get LogLevel and Content, context is empty");
        return;
    }
    if (args.Length() == 1) {
        v8::Local<v8::Value> str;
        if (args[0]->ToString(context).ToLocal(&str)) {
            v8::String::Utf8Value utf8(isolate, str);
            if (*utf8) {
                content += *utf8;
            }
        } else {
            content += "Error converting argument 1";
        }
    } else {
        std::vector<std::string> params;
        for (int32_t i = 0; i < args.Length(); i++) {
            v8::Local<v8::Value> str;
            if (args[i]->ToString(context).ToLocal(&str)) {
                v8::String::Utf8Value utf8(isolate, str);
                if (*utf8) {
                    params.emplace_back(*utf8);
                }
            }
        }
        content = ParseLogContent(params);
    }
    logLevel = static_cast<JsLogLevel>(args.Data()->Int32Value(context).ToChecked());
}

void AppLogPrint(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    JsLogLevel logLevel = JsLogLevel::INFO;
    std::string fullString;
    GetLogLevelAndContent(args, logLevel, fullString);
    switch (logLevel) {
        case JsLogLevel::DEBUG:
            APP_LOGD("app Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::INFO:
            APP_LOGI("app Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::WARNING:
            APP_LOGW("app Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::ERROR:
            APP_LOGE("app Log: %{public}s", fullString.c_str());
            break;
    }
}

void JsLogPrint(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    JsLogLevel logLevel = JsLogLevel::INFO;
    std::string fullString;
    GetLogLevelAndContent(args, logLevel, fullString);
    switch (logLevel) {
        case JsLogLevel::DEBUG:
            LOGD("ace Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::INFO:
            LOGI("ace Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::WARNING:
            LOGW("ace Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::ERROR:
            LOGE("ace Log: %{public}s", fullString.c_str());
            break;
    }
}

void JsPerfEnd(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("Enter JsPerfEnd");
    if (args.Length() != PERF_ARGS_LEN) {
        LOGE("argc error, argc = %{private}d", args.Length());
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsPerfEnd, CurrentContext is empty!");
        return;
    }

    v8::String::Utf8Value jsMethodName(isolate, args[PERF_ARGS_METHOD_IDX]);
    if (!(*jsMethodName)) {
        return;
    }
    std::string methodName(*jsMethodName);

    int64_t currentTime = GetMicroTickCount();
    JsApiPerf::GetInstance().InsertJsEndLog(methodName, currentTime);
}

void JsPerfBegin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("Enter JsPerfEnd");
    if (args.Length() != PERF_ARGS_LEN) {
        LOGE("argc error, argc = %{private}d", args.Length());
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsPerfBegin, CurrentContext is empty!");
        return;
    }

    v8::String::Utf8Value jsMethodName(isolate, args[PERF_ARGS_METHOD_IDX]);
    if (!(*jsMethodName)) {
        return;
    }
    std::string methodName(*jsMethodName);

    int64_t currentTime = GetMicroTickCount();
    JsApiPerf::GetInstance().InsertJsBeginLog(methodName, currentTime);
}

void JsPerfSleep(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("Enter JsPerfEnd");
    if (args.Length() != 1) {
        LOGE("argc error, argc = %{private}d", args.Length());
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("CurrentContext is empty!");
        return;
    }

    int32_t valInt = args[0]->Int32Value(context).ToChecked();
    usleep(valInt);
}

void JsPerfPrint(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);

    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsPerfPrint, CurrentContext is empty!");
        return;
    }

    // return the result
    std::string ret = JsApiPerf::GetInstance().PrintToLogs();
    v8::Local<v8::String> dataValue = v8::String::NewFromUtf8(isolate, ret.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(dataValue);
}

} // namespace

thread_local IsolateWrapper V8DeclarativeEngineInstance::isolate_;

RefPtr<JsAcePage> V8DeclarativeEngineInstance::GetCurrentPage()
{
    LOGD("Enter GetRunningPage");
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ACE_DCHECK(isolate);
    auto stagingPage = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8DeclarativeEngineInstance::STAGING_PAGE));
    return *stagingPage;
}

RefPtr<JsAcePage> V8DeclarativeEngineInstance::GetRunningPage(v8::Isolate* isolate)
{
    LOGD("Enter GetRunningPage");
    ACE_DCHECK(isolate);
    auto runningPage = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8DeclarativeEngineInstance::RUNNING_PAGE));
    return *runningPage;
}

RefPtr<JsAcePage> V8DeclarativeEngineInstance::GetStagingPage(v8::Isolate* isolate)
{
    LOGD("Enter GetStagingPage");
    ACE_DCHECK(isolate);
    auto stagingPage = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8DeclarativeEngineInstance::STAGING_PAGE));
    return *stagingPage;
}

void V8DeclarativeEngineInstance::TriggerPageUpdate()
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ACE_DCHECK(isolate);
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    auto pipelineContext = (*delegate)->GetPipelineContext();
    pipelineContext->AddPageUpdateTask([] {});
}

void V8DeclarativeEngineInstance::PostJsTask(std::function<void()>&& task)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ACE_DCHECK(isolate);
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    (*delegate)->PostJsTask(std::move(task));
}

RefPtr<PipelineContext> V8DeclarativeEngineInstance::GetPipelineContext()
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ACE_DCHECK(isolate);
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    return (*delegate)->GetPipelineContext();
}

void V8DeclarativeEngineInstance::FlushCommandBuffer(void* context, const std::string& command)
{
    LOGI("flush command buffer");
    auto v8Context = *(static_cast<v8::Local<v8::Context>*>(context));
    v8::Isolate* isolate = v8Context->GetIsolate();
    ACE_DCHECK(isolate);
    bool result = CallEvalBuf(isolate, command.c_str());
    if (!result) {
        LOGE("failed to flush command");
    }
}
void V8DeclarativeEngineInstance::CallJs(v8::Isolate* isolate,
    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pcontext, const std::string& callbackId,
    const std::string& args, bool keepAlive, bool isGlobal)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter CallJs");
    std::string keepAliveStr = keepAlive ? "true" : "false";
    std::string callBuff = std::string("[{\"args\": [\"")
                               .append(callbackId)
                               .append("\",")
                               .append(args)
                               .append(",")
                               .append(keepAliveStr)
                               .append("], \"method\":\"callback\"}]");
    LOGD("CallJs string: %{private}s", callBuff.c_str());

    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = pcontext.Get(isolate);
    if (context.IsEmpty()) {
        LOGE("CallJs, context Is Empty");
        return;
    }

    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::String> funcName = v8::String::NewFromUtf8(isolate, "callJS").ToLocalChecked();
    v8::Local<v8::Value> jsFuncVal;
    bool succ = global->Get(context, funcName).ToLocal(&jsFuncVal);
    if (!succ) {
        LOGE("cannot find 'callJS' function from global object, this should not happen!");
        return;
    }

    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(jsFuncVal);

    auto runningPage = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8DeclarativeEngineInstance::RUNNING_PAGE));
    int32_t instanceId = isGlobal ? DEFAULT_APP_ID : (*runningPage)->GetPageId();

    v8::Local<v8::String> callBufStr = v8::String::NewFromUtf8(isolate, callBuff.c_str()).ToLocalChecked();
    v8::Local<v8::Value> jsonBuf;
    if (!v8::JSON::Parse(context, callBufStr).ToLocal(&jsonBuf)) {
        LOGE("Cannot parse callBuf to json format correctly, CallJs stop!");
        return;
    }
    v8::Local<v8::Value> argv[] = { v8::Integer::New(isolate, instanceId), jsonBuf };
    int32_t len = sizeof(argv) / sizeof(argv[0]);
    v8::Local<v8::Value> res;
    succ = jsFunc->Call(context, global, len, argv).ToLocal(&res);
    if (succ) {
        LOGD("func callJs success!");
    } else {
        LOGD("func callJs fail!");
    }

    while (v8::platform::PumpMessageLoop(V8DeclarativeEngine::GetPlatform().get(), isolate)) {
        continue;
    }
}

std::map<std::string, std::string> V8DeclarativeEngineInstance::mediaResourceFileMap_;

std::unique_ptr<JsonValue> V8DeclarativeEngineInstance::currentConfigResourceData_;

bool V8DeclarativeEngineInstance::InitJSEnv()
{
    CHECK_RUN_ON(JS);
    LOGI("Enter InitJSEnv");

    // create Isolate
    create_params_.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate_.Wrap(v8::Isolate::New(create_params_));
    if (isolate_.IsEmpty()) {
        EventInfo eventInfo;
        eventInfo.eventType = EXCEPTION_JS;
        eventInfo.errorType = static_cast<int32_t>(JsExcepType::JS_RUNTIME_OBJ_ERR);
        EventReport::SendEvent(eventInfo);
        LOGE("JS Engine cannot allocate JS runtime");
        return false;
    }

    isolate_->SetStackLimit(V8_MAX_STACK_SIZE);
    isolate_->SetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE, static_cast<void*>(&frontendDelegate_));
    isolate_->SetData(V8DeclarativeEngineInstance::RUNNING_PAGE, static_cast<void*>(&runningPage_));
    isolate_->SetData(V8DeclarativeEngineInstance::STAGING_PAGE, static_cast<void*>(&stagingPage_));
    isolate_->SetData(V8DeclarativeEngineInstance::DISPATCHER, static_cast<void*>(&dispatcher_));
    isolate_->SetCaptureStackTraceForUncaughtExceptions(true);
    // create context
    InitJSContext();
    return true;
}

void V8DeclarativeEngineInstance::InitJSContext()
{
    CHECK_RUN_ON(JS);
    LOGI("Enter InitJSContext");
    v8::Isolate::Scope isolateScope(isolate_.Get());
    v8::HandleScope handleScope(isolate_.Get());

    InitGlobalObjectTemplate();

    v8::Local<v8::Context> localContext =
        v8::Context::New(isolate_.Get(), nullptr, globalObjectTemplate_.Get(isolate_.Get()));

    v8::Context::Scope contextScope(localContext);

    InitJsConsoleObject(localContext, isolate_.Get());
    InitJsPerfUtilObject(localContext);
    InitJsNativeModuleObject(localContext);
    InitJsExportsUtilObject(localContext);
    InitAceModules(_binary_stateMgmt_js_start, _binary_stateMgmt_js_end, isolate_.Get());
    InitAceModules(_binary_jsEnumStyle_js_start, _binary_jsEnumStyle_js_end, isolate_.Get());

    auto groupJsBridge = DynamicCast<V8DeclarativeGroupJsBridge>(frontendDelegate_->GetGroupJsBridge());
    if (groupJsBridge == nullptr || groupJsBridge->InitializeGroupJsBridge(localContext) == JS_CALL_FAIL) {
        LOGE("JS Engine Initialize GroupJsBridge failed!");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
    }

    context_.Reset(isolate_.Get(), localContext);

    // load resourceConfig
    currentConfigResourceData_ = JsonUtil::CreateArray(true);
    frontendDelegate_->LoadResourceConfiguration(mediaResourceFileMap_, currentConfigResourceData_);
}

void V8DeclarativeEngineInstance::InitAceModules(const char* start, const char* end, v8::Isolate* isolate)
{
    if (start == nullptr || end == nullptr) {
        LOGE("Failed to init Ace modules, start or end can not be null!");
        return;
    }
    bool evalAceModule = CallEvalBuf(isolate, start, end - start);
    if (!evalAceModule) {
        EventInfo eventInfo;
        eventInfo.eventType = EXCEPTION_JS;
        eventInfo.errorType = static_cast<int32_t>(JsExcepType::JS_CONTEXT_INIT_ERR);
        EventReport::SendEvent(eventInfo);
        LOGE("JS Engine created JS context but failed to init Ace modules!");
    }
}

void V8DeclarativeEngineInstance::InitJsConsoleObject(v8::Local<v8::Context>& localContext, v8::Isolate* isolate)
{
    v8::Local<v8::Object> global = localContext->Global();
    // app log method
    v8::Local<v8::Value> console =
        global->Get(localContext, v8::String::NewFromUtf8(isolate, "console").ToLocalChecked()).ToLocalChecked();
    v8::Local<v8::Object> consoleObj = console->ToObject(localContext).ToLocalChecked();
    consoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "log").ToLocalChecked(),
            v8::Function::New(
                localContext, AppLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::DEBUG)))
                .ToLocalChecked())
        .ToChecked();
    consoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "debug").ToLocalChecked(),
            v8::Function::New(
                localContext, AppLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::DEBUG)))
                .ToLocalChecked())
        .ToChecked();
    consoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "info").ToLocalChecked(),
            v8::Function::New(
                localContext, AppLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::INFO)))
                .ToLocalChecked())
        .ToChecked();
    consoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "warn").ToLocalChecked(),
            v8::Function::New(
                localContext, AppLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::WARNING)))
                .ToLocalChecked())
        .ToChecked();
    consoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "error").ToLocalChecked(),
            v8::Function::New(
                localContext, AppLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::ERROR)))
                .ToLocalChecked())
        .ToChecked();
    // js framework log method
    auto aceConsoleObj = v8::Object::New(isolate);
    global->Set(localContext, v8::String::NewFromUtf8(isolate, "aceConsole").ToLocalChecked(), aceConsoleObj)
        .ToChecked();
    aceConsoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "log").ToLocalChecked(),
            v8::Function::New(
                localContext, JsLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::DEBUG)))
                .ToLocalChecked())
        .ToChecked();
    aceConsoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "debug").ToLocalChecked(),
            v8::Function::New(
                localContext, JsLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::DEBUG)))
                .ToLocalChecked())
        .ToChecked();
    aceConsoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "info").ToLocalChecked(),
            v8::Function::New(
                localContext, JsLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::INFO)))
                .ToLocalChecked())
        .ToChecked();
    aceConsoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "warn").ToLocalChecked(),
            v8::Function::New(
                localContext, JsLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::WARNING)))
                .ToLocalChecked())
        .ToChecked();
    aceConsoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "error").ToLocalChecked(),
            v8::Function::New(
                localContext, JsLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::ERROR)))
                .ToLocalChecked())
        .ToChecked();
}

void V8DeclarativeEngineInstance::InitJsPerfUtilObject(v8::Local<v8::Context>& localContext)
{
    v8::Local<v8::Object> global = localContext->Global();

    auto perfUtilObj = v8::Object::New(isolate_.Get());
    global->Set(localContext, v8::String::NewFromUtf8(isolate_.Get(), "perfutil").ToLocalChecked(), perfUtilObj)
        .ToChecked();
    perfUtilObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate_.Get(), "printlog").ToLocalChecked(),
            v8::Function::New(localContext, JsPerfPrint).ToLocalChecked())
        .ToChecked();
    perfUtilObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate_.Get(), "sleep").ToLocalChecked(),
            v8::Function::New(localContext, JsPerfSleep).ToLocalChecked())
        .ToChecked();
    perfUtilObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate_.Get(), "begin").ToLocalChecked(),
            v8::Function::New(localContext, JsPerfBegin).ToLocalChecked())
        .ToChecked();
    perfUtilObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate_.Get(), "end").ToLocalChecked(),
            v8::Function::New(localContext, JsPerfEnd).ToLocalChecked())
        .ToChecked();
}

thread_local std::unordered_map<int32_t, CopyablePersistent<v8::Object>>
    V8DeclarativeEngineInstance::persistentRootViewMap_;

void V8DeclarativeEngineInstance::PersistRootViewHandle(v8::Isolate* isolate, v8::Local<v8::Object> obj)
{
    V8DeclarativeEngineInstance::persistentRootViewMap_.emplace(
        V8DeclarativeEngineInstance::GetStagingPage(isolate)->GetPageId(),
        CopyablePersistent<v8::Object>(isolate, obj));
}

void V8DeclarativeEngineInstance::DestroyPersistRootViewHandle(int32_t pageId)
{
    CHECK_RUN_ON(JS);
    if (V8DeclarativeEngineInstance::persistentRootViewMap_.count(pageId) != 0) {
        v8::Isolate* isolate = GetV8Isolate();
        ACE_DCHECK(isolate);
        v8::Isolate::Scope isolateScope(isolate);
        v8::HandleScope handleScope(isolate);
        CopyablePersistent<v8::Object> rootView = V8DeclarativeEngineInstance::persistentRootViewMap_[pageId];
        if (!rootView.IsEmpty()) {
            v8::Local<v8::Object> previousRootObj = rootView.Get(isolate);
            JSView* previousView = static_cast<JSView*>(previousRootObj->GetAlignedPointerFromInternalField(0));
            previousView->Destroy(nullptr);
        }
        rootView.Reset();

        V8DeclarativeEngineInstance::persistentRootViewMap_.erase(pageId);
    }
}

void V8DeclarativeEngineInstance::DestroyAllPersistRootViewHandle()
{
    CHECK_RUN_ON(JS);
    if (persistentRootViewMap_.size() > 0) {
        LOGI("DestroyAllPersistRootViewHandle release left %zu views ", persistentRootViewMap_.size());
    }
    for (const auto& pair : persistentRootViewMap_) {
        v8::Isolate* isolate = GetV8Isolate();
        ACE_DCHECK(isolate);
        v8::Isolate::Scope isolateScope(isolate);
        v8::HandleScope handleScope(isolate);
        CopyablePersistent<v8::Object> rootView = pair.second;
        if (!rootView.IsEmpty()) {
            v8::Local<v8::Object> previousRootObj = rootView.Get(isolate);
            JSView* previousView = static_cast<JSView*>(previousRootObj->GetAlignedPointerFromInternalField(0));
            previousView->Destroy(nullptr);
        }
        rootView.Reset();
    }
    persistentRootViewMap_.clear();
}

void RequireNativeModule(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto localContext = isolate->GetCurrentContext();
    if (localContext.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }
    v8::String::Utf8Value value(isolate, args[0]->ToString(localContext).ToLocalChecked());
    if (!(*value)) {
        return;
    }
    std::string moduleName(*value);

    // init method
    v8::Local<v8::Object> global = localContext->Global();
    v8::Local<v8::Value> moduleObj =
        global->Get(localContext, v8::String::NewFromUtf8(isolate, moduleName.c_str()).ToLocalChecked())
            .ToLocalChecked();
    if (!moduleObj->IsUndefined()) {
        args.GetReturnValue().Set(moduleObj);
        return;
    }

    auto newObj = v8::Object::New(isolate);
    if (ModuleManager::GetInstance()->InitModule(newObj, moduleName, isolate)) {
        global->Set(localContext, v8::String::NewFromUtf8(isolate, moduleName.c_str()).ToLocalChecked(), newObj)
            .ToChecked();
        args.GetReturnValue().Set(newObj);
    }
}

void V8DeclarativeEngineInstance::InitJsNativeModuleObject(v8::Local<v8::Context>& localContext)
{
    v8::Local<v8::Object> global = localContext->Global();

    global
        ->Set(localContext, v8::String::NewFromUtf8(isolate_.Get(), "requireNativeModule").ToLocalChecked(),
            v8::Function::New(localContext, RequireNativeModule).ToLocalChecked())
        .ToChecked();

    ModuleManager::GetInstance()->InitTimerModule(localContext);
}

void V8DeclarativeEngineInstance::InitJsExportsUtilObject(v8::Local<v8::Context>& localContext)
{
    v8::Local<v8::Object> global = localContext->Global();

    auto exportsUtilObj = v8::Object::New(isolate_.Get());
    global->Set(localContext, v8::String::NewFromUtf8(isolate_.Get(), "exports").ToLocalChecked(), exportsUtilObj)
        .ToChecked();
}

void V8DeclarativeEngineInstance::InitGlobalObjectTemplate()
{
    v8::Isolate::Scope isolateScope(isolate_.Get());
    v8::HandleScope handleScope(isolate_.Get());
    v8::Local<v8::ObjectTemplate> globalObj = v8::ObjectTemplate::New(isolate_.Get());
    JsRegisterViews(globalObj);

    globalObjectTemplate_.Reset(isolate_.Get(), globalObj);
}

bool V8DeclarativeEngineInstance::FireJsEvent(const std::string& param)
{
    return true;
}

std::unique_ptr<JsonValue> V8DeclarativeEngineInstance::GetI18nStringResource(
    const std::string& targetStringKey, const std::string& targetStringValue)
{
    auto resourceI18nFileNum = currentConfigResourceData_->GetArraySize();
    for (int i = 0; i < resourceI18nFileNum; i++) {
        auto priorResource = currentConfigResourceData_->GetArrayItem(i);
        if ((priorResource->Contains(targetStringKey))) {
            auto valuePair = priorResource->GetValue(targetStringKey);
            if (valuePair->Contains(targetStringValue)) {
                return valuePair->GetValue(targetStringValue);
            }
        }
    }

    return JsonUtil::Create(true);
}

std::string V8DeclarativeEngineInstance::GetMediaResource(const std::string& targetFileName)
{
    auto iter = mediaResourceFileMap_.find(targetFileName);

    if (iter != mediaResourceFileMap_.end()) {
        return iter->second;
    }

    return std::string();
}

V8DeclarativeEngineInstance::~V8DeclarativeEngineInstance()
{
    CHECK_RUN_ON(JS);
    LOG_DESTROY();
    DestroyAllPersistRootViewHandle();
    globalObjectTemplate_.Reset();
    delete create_params_.array_buffer_allocator;

    // Destroy group bridge
    auto groupJsBridge = DynamicCast<V8DeclarativeGroupJsBridge>(frontendDelegate_->GetGroupJsBridge());
    if (groupJsBridge != nullptr) {
        groupJsBridge->Destroy(GetV8Isolate());
    }
    // Force GC before dispose v8 isolate
    auto* isolate = isolate_.Get();
    if (isolate != nullptr) {
        v8::Isolate::Scope isolateScope(isolate);
        isolate->LowMemoryNotification();
    }
    // delete function template
    V8FunctionDestroyCallbackHelper::DeleteFunctionTemplate(isolate_.Get());
    // Just invalidate it, isolate_ will dispose when js-thread finished.
    ModuleManager::GetInstance()->ClearTimerIsolate(isolate_.Get());
    isolate_.Invalidate();
}

void LoadDebuggerSo()
{
    LOGI("LoadDebuggerSo");
    const std::string soDir = "libv8_debugger.z.so";
    g_debugger = dlopen(soDir.c_str(), RTLD_LAZY);
    if (g_debugger == nullptr) {
        LOGE("cannot find debugger so");
    }
}

void StartDebuggerAgent(
    const std::unique_ptr<v8::Platform>& platform, const v8::Local<v8::Context>& context, std::string componentName,
    const bool isDebugMode, const int32_t instanceId)
{
    LOGI("StartAgent");
    if (g_debugger == nullptr) {
        LOGE("g_debugger is null");
        return;
    }
    StartDebug startDebug = (StartDebug)dlsym(g_debugger, "StartDebug");
    if (startDebug == nullptr) {
        LOGE("StartDebug=NULL, dlerror=%s", dlerror());
        return;
    }
    startDebug(platform, context, componentName, isDebugMode, instanceId);
}

// -----------------------
// Start V8DeclarativeEngine
// -----------------------

std::unique_ptr<v8::Platform>& V8DeclarativeEngine::GetPlatform()
{
    return V8Helper::GetPlatform();
}

bool V8DeclarativeEngine::Initialize(const RefPtr<FrontendDelegate>& delegate)
{
    CHECK_RUN_ON(JS);
    ACE_SCOPED_TRACE("V8DeclarativeEngine::Initialize");
    LOGI("V8DeclarativeEngine initialize");
    ACE_DCHECK(delegate);

    // Initialize V8 engine
    GetPlatform();

    // if load debugger.so successfully, debug mode
    if (IsDebugVersion()) {
        if (NeedDebugBreakPoint()) {
            LOGI("NeedDebugBreakPoint = TRUE");
        } else {
            LOGI("NeedDebugBreakPoint = FALSE");
        }
        LoadDebuggerSo();
        LOGI("debug mode in V8DeclarativeEngine");
    }
    // Initialize engine instance
    engineInstance_ = AceType::MakeRefPtr<V8DeclarativeEngineInstance>(delegate);
    bool res = engineInstance_->InitJSEnv();
    if (!res) {
        LOGE("V8DeclarativeEngine initialize failed: %{public}d", instanceId_);
        return false;
    }

    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    {
        v8::Isolate::Scope isolateScope(isolate);
        v8::HandleScope handleScope(isolate);
        auto context = engineInstance_->GetV8Context();
        if (context.IsEmpty()) {
            LOGE("Initialize, context Is Empty");
            return false;
        }
        // debug mode
        if (g_debugger != nullptr) {
            std::string instanceName = GetInstanceName();
            if (instanceName.empty()) {
                LOGE("GetInstanceName fail, %s", instanceName.c_str());
                return false;
            }
            StartDebuggerAgent(GetPlatform(), context, instanceName, NeedDebugBreakPoint(), instanceId_);
        }
    }
    nativeEngine_ =
        new V8NativeEngine(GetPlatform().get(), isolate, engineInstance_->GetContext(), static_cast<void*>(this));
    engineInstance_->SetNativeEngine(nativeEngine_);
    SetPostTask(nativeEngine_);
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    nativeEngine_->CheckUVLoop();
#endif
    if (delegate && delegate->GetAssetManager()) {
        std::string packagePath = delegate->GetAssetManager()->GetLibPath();
        if (!packagePath.empty()) {
            auto v8NativeEngine = static_cast<V8NativeEngine*>(nativeEngine_);
            v8NativeEngine->SetPackagePath(packagePath);
        }
    }
    RegisterWorker();

    return true;
}

void V8DeclarativeEngine::SetPostTask(NativeEngine* nativeEngine)
{
    LOGI("SetPostTask");
    auto weakDelegate = AceType::WeakClaim(AceType::RawPtr(engineInstance_->GetDelegate()));
    auto&& postTask = [weakDelegate, nativeEngine = nativeEngine_, id = instanceId_](bool needSync) {
        auto delegate = weakDelegate.Upgrade();
        if (delegate == nullptr) {
            LOGE("delegate is nullptr");
            return;
        }
        delegate->PostJsTask([nativeEngine, needSync, id]() {
            if (nativeEngine == nullptr) {
                return;
            }
            ContainerScope scope(id);
            nativeEngine->Loop(LOOP_NOWAIT, needSync);
        });
    };
    nativeEngine_->SetPostTask(postTask);
}

void V8DeclarativeEngine::RegisterInitWorkerFunc()
{
    WeakPtr<GroupJsBridge> weakJsBridge = engineInstance_->GetDelegate()->GetGroupJsBridge();
    auto&& initWorkerFunc = [weakJsBridge, delegate = engineInstance_->GetDelegateForV8Data(),
        dispatch = engineInstance_->GetJsMessageDispatcherForV8Data()](NativeEngine* nativeEngine) {
        LOGI("WorkerCore RegisterInitWorkerFunc called");
        if (nativeEngine == nullptr) {
            LOGE("nativeEngine is nullptr");
            return;
        }
        auto v8NativeEngine = static_cast<V8NativeEngine*>(nativeEngine);
        if (v8NativeEngine == nullptr) {
            LOGE("v8NativeEngine is nullptr");
            return;
        }
        auto isolate = v8NativeEngine->GetIsolate();
        if (isolate == nullptr) {
            LOGE("isolate is nullptr");
            return;
        }

        v8::HandleScope handleScope(isolate);
        v8::Local<v8::Context> localContext = v8NativeEngine->GetContext();
        isolate->SetStackLimit(V8_MAX_STACK_SIZE);
        isolate->SetData(V8DeclarativeEngineInstance::DISPATCHER, dispatch);
        isolate->SetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE, delegate);
        V8DeclarativeEngineInstance::InitJsConsoleObject(localContext, isolate);
        V8DeclarativeEngineInstance::InitAceModules(_binary_jsEnumStyle_js_start, _binary_jsEnumStyle_js_end, isolate);
        ModuleManager::InitTimerModule(localContext);

        auto jsBridge = DynamicCast<V8DeclarativeGroupJsBridge>(weakJsBridge.Upgrade());
        if (jsBridge != nullptr) {
            jsBridge->AddIsolateNativeEngineRelation(isolate, nativeEngine);
            auto source = v8::String::NewFromUtf8(isolate, jsBridge->GetJsCode().c_str()).ToLocalChecked();
            if (!CallEvalBuf(isolate, "var global = globalThis;\n")
                || jsBridge->InitializeGroupJsBridge(localContext) == JS_CALL_FAIL
                || jsBridge->CallEvalBuf(isolate, source) == JS_CALL_FAIL) {
                LOGE("Worker Initialize GroupJsBridge failed!");
            }
        } else {
            LOGE("Worker Initialize GroupJsBridge failed, jsBridge is nullptr");
        }
    };
    nativeEngine_->SetInitWorkerFunc(initWorkerFunc);
}

void V8DeclarativeEngine::RegisterAssetFunc()
{
    auto weakAsset = WeakPtr<AssetManager>(engineInstance_->GetDelegate()->GetAssetManager());
    auto&& assetFunc = [weakAsset](const std::string& uri, std::vector<uint8_t>& content, std::string& ami) {
        LOGI("WorkerCore RegisterAssetFunc called");
        auto asset = weakAsset.Upgrade();
        if (!asset) {
            LOGE("delegate is nullptr");
            return;
        }
        FrontendDelegate::GetResourceData(uri, asset, content);
    };
    nativeEngine_->SetGetAssetFunc(assetFunc);
}

void V8DeclarativeEngine::RegisterOffWorkerFunc()
{
    WeakPtr<GroupJsBridge> weakJsBridge = engineInstance_->GetDelegate()->GetGroupJsBridge();
    auto&& offWorkerFunc = [weakJsBridge](NativeEngine* nativeEngine) {
        LOGI("WorkerCore RegisterOffWorkerFunc called");
        if (nativeEngine == nullptr) {
            LOGE("nativeEngine is nullptr");
            return;
        }
        auto v8NativeEngine = static_cast<V8NativeEngine*>(nativeEngine);
        if (v8NativeEngine == nullptr) {
            LOGE("v8NativeEngine is nullptr");
            return;
        }
        auto jsBridge = DynamicCast<V8DeclarativeGroupJsBridge>(weakJsBridge.Upgrade());
        if (jsBridge == nullptr) {
            LOGE("jsBridge is nullptr");
            return;
        }
        auto isolate = v8NativeEngine->GetIsolate();
        if (isolate == nullptr) {
            LOGE("isolate is nullptr");
            return;
        }

        jsBridge->Destroy(isolate, true);
    };
    nativeEngine_->SetOffWorkerFunc(offWorkerFunc);
}

void V8DeclarativeEngine::RegisterWorker()
{
    nativeEngine_->SetWorkerAsyncWorkFunc(V8DeclarativeGroupJsBridge::NativeAsyncExecuteCallback,
        V8DeclarativeGroupJsBridge::NativeAsyncCompleteCallback);
    RegisterInitWorkerFunc();
    RegisterAssetFunc();
    RegisterOffWorkerFunc();
}

V8DeclarativeEngine::~V8DeclarativeEngine()
{
    CHECK_RUN_ON(JS);
    if (g_debugger != nullptr) {
        StopDebug stopDebug = (StopDebug)dlsym(g_debugger, "StopDebug");
        if (stopDebug != nullptr) {
            stopDebug();
        }
    }
    LOG_DESTROY();
    XComponentClient::GetInstance().SetRegisterCallbackToNull();
    XComponentClient::GetInstance().SetJSValCallToNull();
    if (nativeEngine_ != nullptr) {
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
        nativeEngine_->CancelCheckUVLoop();
#endif
        delete nativeEngine_;
    }
    if (nativeXComponent_) {
        delete nativeXComponent_;
        nativeXComponent_ = nullptr;
    }
    if (g_debugger != nullptr) {
        dlclose(g_debugger);
    }
}

void V8DeclarativeEngine::CallAppFunc(
    v8::Isolate* isolate, const v8::Local<v8::Context>& context, std::string appFuncName)
{
    (void)CallAppFunc(isolate, context, appFuncName, 0, nullptr);
}

bool V8DeclarativeEngine::CallAppFunc(v8::Isolate* isolate, const v8::Local<v8::Context>& context,
    std::string appFuncName, int argc, v8::Local<v8::Value>* argv)
{
    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::String> exportName = v8::String::NewFromUtf8(isolate, "exports").ToLocalChecked();
    v8::Local<v8::Value> exportval;
    bool succ = global->Get(context, exportName).ToLocal(&exportval);
    if (!succ) {
        LOGD("no property named \"export\" in global object");
        return false;
    }
    if (!exportval->IsObject()) {
        LOGD("property \"exports\" is not a object");
        return false;
    }

    v8::Local<v8::Object> exportobj = exportval->ToObject(context).ToLocalChecked();
    v8::Local<v8::String> defaultName = v8::String::NewFromUtf8(isolate, "default").ToLocalChecked();
    v8::Local<v8::Value> defaultval;
    succ = exportobj->Get(context, defaultName).ToLocal(&defaultval);
    if (!succ) {
        LOGD("no property named \"default\" in export object");
        return false;
    }
    if (!defaultval->IsObject()) {
        LOGD("property \"defaultval\" is not a object");
        return false;
    }

    v8::Local<v8::Object> defaultobj = defaultval->ToObject(context).ToLocalChecked();
    v8::Local<v8::String> funcName = v8::String::NewFromUtf8(isolate, appFuncName.c_str()).ToLocalChecked();
    v8::Local<v8::Value> jsFuncVal;
    succ = defaultobj->Get(context, funcName).ToLocal(&jsFuncVal);
    if (!succ) {
        LOGD("no property named \"%{public}s\" in global object", appFuncName.c_str());
        return false;
    }
    if (!jsFuncVal->IsFunction()) {
        LOGD("property \"%{public}s\" is not a function", appFuncName.c_str());
        return false;
    }

    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(jsFuncVal);
    v8::Local<v8::Value> res;
    succ = jsFunc->Call(context, defaultobj, argc, argv).ToLocal(&res);
    if (!succ) {
        LOGD("call failed");
        return false;
    }
    return res->IsTrue();
}

void V8DeclarativeEngine::LoadJs(const std::string& url, const RefPtr<JsAcePage>& page, bool isMainPage)
{
    LOGI("Enter LoadJs");
    ACE_SCOPED_TRACE("V8DeclarativeEngine::LoadJS");
    ACE_DCHECK(engineInstance_);

    if (g_debugger != nullptr) {
        WaitingForIde waitingForIde = (WaitingForIde)dlsym(g_debugger, "WaitingForIde");
        if (waitingForIde != nullptr) {
            waitingForIde();
        }
    }

    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = engineInstance_->GetV8Context();
    v8::Context::Scope contextScope(context);

    engineInstance_->SetStagingPage(page);
    if (isMainPage) {
        ACE_DCHECK(!engineInstance_->GetRunningPage());
        engineInstance_->SetRunningPage(page);
    }

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    v8::TryCatch tryCatch(isolate);

    if (isMainPage) {
        std::string commonsJsContent;
        if ((*delegate)->GetAssetContent("commons.js", commonsJsContent)) {
            bool commonsJsResult = CallEvalBuf(isolate, commonsJsContent.c_str(), -1, "commons.js");
            if (!commonsJsResult) {
                LOGE("fail to excute load commonsjs script");
                return;
            }
        }
        std::string vendorsJsContent;
        if ((*delegate)->GetAssetContent("vendors.js", vendorsJsContent)) {
            bool vendorsJsResult = CallEvalBuf(isolate, vendorsJsContent.c_str(), -1, "vendors.js");
            if (!vendorsJsResult) {
                LOGE("fail to excute load vendorsjs script");
                return;
            }
        }
        std::string appjsContent;
        if (!(*delegate)->GetAssetContent("app.js", appjsContent)) {
            LOGE("js file load failed!");
            return;
        }
        std::string appMap;
        if ((*delegate)->GetAssetContent("app.js.map", appMap)) {
            page->SetAppMap(appMap);
        } else {
            LOGI("app map is missing!");
        }
        bool result = CallEvalBuf(isolate, appjsContent.c_str(), -1, "app.js");
        if (!result) {
            LOGE("failed to execute Loadjs script");
            return;
        }
        auto container = Container::Current();
        if (container && container->IsMainWindow()) {
            CallAppFunc(isolate, context, "onCreate");
        }
    }

    if (g_debugger != nullptr) {
        WaitingForIde waitingForIde = (WaitingForIde)dlsym(g_debugger, "WaitingForIde");
        if (waitingForIde != nullptr) {
            waitingForIde();
        }
    }

    std::string jsContent;
    LOGI("Enter LoadJs file:%s", url.c_str());
    if (!(*delegate)->GetAssetContent(url, jsContent)) {
        LOGE("js file load failed!");
        return;
    }

    std::string jsSourceMap;
    if ((*delegate)->GetAssetContent(url + ".map", jsSourceMap)) {
        page->SetPageMap(jsSourceMap);
    } else {
        LOGI("js source map load failed!");
    }
    bool result = CallEvalBuf(isolate, jsContent.c_str(), -1, url.c_str());
    if (!result) {
        LOGE("failed to execute Loadjs script");
        return;
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::UpdateRunningPage(const RefPtr<JsAcePage>& page)
{
    ACE_DCHECK(engineInstance_);
    LOGD("Enter UpdateRunningPage %d", page->GetPageId());
    engineInstance_->SetRunningPage(page);
}

void V8DeclarativeEngine::UpdateStagingPage(const RefPtr<JsAcePage>& page)
{
    LOGD("Enter UpdateStagingPage %d", page->GetPageId());
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetStagingPage(page);
}

void V8DeclarativeEngine::ResetStagingPage()
{
    ACE_DCHECK(engineInstance_);
    LOGD("Enter ResetStagingPage");
    auto runningPage = engineInstance_->GetRunningPage();
    engineInstance_->ResetStagingPage(runningPage);
}

void V8DeclarativeEngine::DestroyPageInstance(int32_t pageId)
{
    LOGI("Enter DestroyPageInstance %d", pageId);
    ACE_DCHECK(engineInstance_);

    engineInstance_->DestroyPersistRootViewHandle(pageId);
}

void V8DeclarativeEngine::DestroyApplication(const std::string& packageName)
{
    LOGI("Enter DestroyApplication: destroy, packageName %{public}s", packageName.c_str());
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("Destroy Application, context Is Empty");
        return;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    auto container = Container::Current();
    if (container && container->IsMainWindow()) {
        CallAppFunc(isolate, context, "onDestroy");
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::UpdateApplicationState(const std::string& packageName, Frontend::State state)
{
    LOGD("Enter UpdateApplicationState: destroy, packageName %{public}s", packageName.c_str());
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("Destroy Application, context Is Empty");
        return;
    }

    if (state == Frontend::State::ON_SHOW) {
        CallAppFunc(isolate, context, "onShow");
    } else if (state == Frontend::State::ON_HIDE) {
        CallAppFunc(isolate, context, "onHide");
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::OnWindowDisplayModeChanged(bool isShownInMultiWindow, const std::string& data)
{
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("Destroy Application, context Is Empty");
        return;
    }

    v8::Local<v8::Boolean> v8Bool = v8::Boolean::New(isolate, isShownInMultiWindow);
    v8::Local<v8::String> v8Data = v8::String::NewFromUtf8(isolate, data.c_str()).ToLocalChecked();
    auto jsonValue = v8::JSON::Parse(context, v8Data);
    if (jsonValue.IsEmpty()) {
        LOGE("jsonValue is empty");
        return;
    }
    v8::Local<v8::Value> v8Config = jsonValue.ToLocalChecked();
    v8::Local<v8::Value> argv[] = { v8Bool, v8Config };
    int32_t argc = 2;
    (void)CallAppFunc(isolate, context, "onWindowDisplayModeChanged", argc, argv);
    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::OnNewWant(const std::string& data)
{
    LOGD("V8DeclarativeEngine::OnNewWant");
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility Sava date, context Is Empty");
        return;
    }
    v8::Context::Scope contextScope(context);
    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::String> v8Data = v8::String::NewFromUtf8(isolate, data.c_str()).ToLocalChecked();
    auto jsonValue = v8::JSON::Parse(context, v8Data);
    if (jsonValue.IsEmpty()) {
        v8::Local<v8::Object> tmpData = v8::Object::New(isolate);
        v8::Local<v8::Value> argv[] = { tmpData };
        int len = 1;
        if (!CallAppFunc(isolate, context, "onNewWant", len, argv)) {
            return;
        }
    } else {
        v8::Local<v8::Value> dataValue = jsonValue.ToLocalChecked();
        v8::Local<v8::Value> argv[] = { dataValue };
        int32_t len = 1;
        if (!CallAppFunc(isolate, context, "onNewWant", len, argv)) {
            return;
        }
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::OnSaveAbilityState(std::string& saveData)
{
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility Sava data, context Is Empty");
        return;
    }

    v8::Local<v8::Object> data = v8::Object::New(isolate);
    v8::Local<v8::Value> argv[] = { data };
    int len = 1;
    CallAppFunc(isolate, context, "onSaveAbilityState", len, argv);
    v8::Local<v8::String> propertyEntries = v8::JSON::Stringify(context, data).ToLocalChecked();
    v8::String::Utf8Value utf8Value(isolate, propertyEntries);
    if (*utf8Value) {
        saveData = *utf8Value;
    }
    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::OnRestoreAbilityState(const std::string& data)
{
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility Restore data, context Is Empty");
        return;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::String> v8Data = v8::String::NewFromUtf8(isolate, data.c_str()).ToLocalChecked();
    auto jsonValue = v8::JSON::Parse(context, v8Data);
    if (jsonValue.IsEmpty()) {
        LOGE("AceAbility RestoreAbility, data Is Empty");
        return;
    }
    v8::Local<v8::Value> dataValue = v8::JSON::Parse(context, v8Data).ToLocalChecked();
    v8::Local<v8::Value> argv[] = { dataValue };
    int32_t len = 1;

    if (!CallAppFunc(isolate, context, "onRestoreAbilityState", len, argv)) {
        return;
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::OnActive()
{
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility OnActive, context Is Empty");
        return;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    CallAppFunc(isolate, context, "onActive");
    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::OnInactive()
{
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility OnInactive, context Is Empty");
        return;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    CallAppFunc(isolate, context, "onInactive");
    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::OnMemoryLevel(const int32_t level)
{
    LOGD("AceAbility Enter OnMemoryLevel");
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility OnMemoryLevel, context Is Empty");
        return;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::Integer> v8Integer = v8::Uint32::NewFromUnsigned(isolate, level);
    v8::Local<v8::Value> argv[] = { v8Integer };
    int32_t len = 1;

    if (!CallAppFunc(isolate, context, "onMemoryLevel", len, argv)) {
        return;
    }
    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::OnConfigurationUpdated(const std::string& data)
{
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("Destroy Application, context Is Empty");
        return;
    }

    v8::Local<v8::String> v8Data = v8::String::NewFromUtf8(isolate, data.c_str()).ToLocalChecked();
    auto jsonValue = v8::JSON::Parse(context, v8Data);
    if (jsonValue.IsEmpty()) {
        LOGE("jsonValue is empty");
        return;
    }
    v8::Local<v8::Value> ResourceConfiguration = jsonValue.ToLocalChecked();
    v8::Local<v8::Value> argv[] = { ResourceConfiguration };
    int32_t argc = 1;
    (void)CallAppFunc(isolate, context, "onConfigurationUpdated", argc, argv);
    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::OnRemoteTerminated()
{
    LOGD("AceAbility Enter OnRemoteTerminated");
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility OnRemoteTerminated, context Is Empty");
        return;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    CallAppFunc(isolate, context, "onRemoteTerminated");
    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::OnCompleteContinuation(const int32_t code)
{
    LOGD("AceAbility Enter OnCompleteContinuation");
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility OnCompleteContinuation, context Is Empty");
        return;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::Integer> v8Integer = v8::Uint32::NewFromUnsigned(isolate, code);
    v8::Local<v8::Value> argv[] = { v8Integer };
    int32_t len = 1;

    (void)CallAppFunc(isolate, context, "onCompleteContinuation", len, argv);
    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}


bool V8DeclarativeEngine::OnRestoreData(const std::string& data)
{
    LOGD("AceAbility Enter OnRestoreData");
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility OnRestoreData, context Is Empty");
        return false;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::String> v8Data = v8::String::NewFromUtf8(isolate, data.c_str()).ToLocalChecked();
    auto jsonValue = v8::JSON::Parse(context, v8Data);
    if (jsonValue.IsEmpty()) {
        LOGE("jsonValue is empty");
        return false;
    }
    v8::Local<v8::Value> dataValue = jsonValue.ToLocalChecked();
    v8::Local<v8::Value> argv[] = { dataValue };
    int32_t len = 1;

    if (!CallAppFunc(isolate, context, "onRestoreData", len, argv)) {
        return false;
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }

    return true;
}

bool V8DeclarativeEngine::OnStartContinuation()
{
    LOGE("AceAbility Enter OnStartContinuation");
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility OnStartContinuation, context Is Empty");
        return false;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    int len = 0;
    auto argv = nullptr;
    if (!CallAppFunc(isolate, context, "onStartContinuation", len, argv)) {
        return false;
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }

    return true;
}

void V8DeclarativeEngine::OnSaveData(std::string& saveData)
{
    LOGE("AceAbility Enter save data");
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("AceAbility Sava date, context Is Empty");
        return;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::Object> data = v8::Object::New(isolate);
    v8::Local<v8::Value> argv[] = { data };
    int len = 1;

    if (CallAppFunc(isolate, context, "onSaveData", len, argv)) {
        v8::Local<v8::String> propertyEntries = v8::JSON::Stringify(context, data).ToLocalChecked();
        v8::String::Utf8Value utf8Value(isolate, propertyEntries);
        saveData = *utf8Value;
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8DeclarativeEngine::TimerCallback(const std::string& callbackId, const std::string& delay, bool isInterval)
{
    LOGD("Enter TimerCallback");
    ACE_DCHECK(engineInstance_);
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = engineInstance_->GetV8Context();
    v8::Context::Scope contextScope(context);

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    if (isInterval) {
        TimerCallJs(callbackId, isInterval);
        (*delegate)->WaitTimer(callbackId, delay, isInterval, false);
    } else {
        TimerCallJs(callbackId, isInterval);
        ModuleManager::GetInstance()->RemoveCallbackFunc(std::stoi(callbackId), isInterval);
        (*delegate)->ClearTimer(callbackId);
    }
}

void V8DeclarativeEngine::TimerCallJs(const std::string& callbackId, bool isInterval)
{
    ACE_DCHECK(engineInstance_);
    v8::Isolate* isolate = ModuleManager::GetInstance()->GetCallbackIsolate(std::stoi(callbackId), isInterval);
    ACE_DCHECK(isolate);
    if (isolate == nullptr) {
        LOGW("CallbackIsolate is null when TimerCallJs is called.");
        return;
    }
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::Function> jsFunc =
        ModuleManager::GetInstance()->GetCallbackFunc(std::stoi(callbackId), isInterval).Get(isolate);
    if (!jsFunc->IsFunction()) {
        LOGE("timer jsFunc callbackId = %s is not a function", callbackId.c_str());
        return;
    }
    std::vector<CopyablePersistent<v8::Value>> jsargv =
        ModuleManager::GetInstance()->GetCallbackArray(std::stoi(callbackId), isInterval);
    v8::Local<v8::Value> res;
    if (jsargv.empty()) {
        LOGI("jsargv is empty");
        v8::Local<v8::Value> argv[] = {};
        bool succ = jsFunc->Call(context, context->Global(), 0, argv).ToLocal(&res);
        if (succ) {
            LOGI("call timer callback success");
        } else {
            V8Utils::JsStdDumpErrorAce(isolate, &tryCatch);
            LOGE("call timer callback fail");
        }
    } else {
        LOGI("jsargv's size is %{private}zu", jsargv.size());
        v8::Local<v8::Value>* argv = new v8::Local<v8::Value>[jsargv.size()];
        uint32_t index = 0;
        while (index < jsargv.size()) {
            argv[index] = jsargv[index].Get(isolate);
            ++index;
        }

        bool succ = jsFunc->Call(context, context->Global(), jsargv.size(), argv).ToLocal(&res);
        if (succ) {
            LOGI("call timer callback success");
        } else {
            V8Utils::JsStdDumpErrorAce(isolate, &tryCatch);
            LOGE("call timer callback fail");
        }
        delete[] argv;
    }
}

void V8DeclarativeEngine::MediaQueryCallback(const std::string& callbackId, const std::string& args)
{
    JsEngine::MediaQueryCallback(callbackId, args);
}

void V8DeclarativeEngine::RequestAnimationCallback(const std::string& callbackId, uint64_t timeStamp)
{
    LOGD("Enter RequestAnimationCallback");
}

void V8DeclarativeEngine::JsCallback(const std::string& callbackId, const std::string& args)
{
    LOGD("Enter JSCallback");
}

void V8DeclarativeEngine::FireAsyncEvent(const std::string& eventId, const std::string& param)
{
    LOGW("V8DeclarativeEngine FireAsyncEvent is unusable");
}

void V8DeclarativeEngine::FireSyncEvent(const std::string& eventId, const std::string& param)
{
    LOGW("V8DeclarativeEngine FireAsyncEvent is unusable");
}

void V8DeclarativeEngine::InitXComponent()
{
    ACE_DCHECK(engineInstance_);
    isolateXComp_ = engineInstance_->GetV8Isolate();
    if (!isolateXComp_) {
        LOGE("InitXComponent isolateXComp_ is null.");
        return;
    }
    ACE_DCHECK(isolateXComp_);
    v8::Isolate::Scope isolateScope(isolateXComp_);
    v8::HandleScope handleScope(isolateXComp_);
    auto context = isolateXComp_->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("InitXComponent context is empty");
        return;
    }
    ctxXComp_.Reset(isolateXComp_, context);
    nativeXComponentImpl_ = AceType::MakeRefPtr<NativeXComponentImpl>();
    nativeXComponent_ = new OH_NativeXComponent(AceType::RawPtr(nativeXComponentImpl_));
}

void V8DeclarativeEngine::FireExternalEvent(const std::string& componentId, const uint32_t nodeId)
{
    CHECK_RUN_ON(JS);
    InitXComponent();
    RefPtr<XComponentComponent> xcomponent;
    OHOS::Ace::Framework::XComponentClient::GetInstance().GetXComponent(xcomponent);
    if (!xcomponent) {
        LOGE("FireExternalEvent xcomponent is null.");
        return;
    }
    auto textureId = static_cast<int64_t>(xcomponent->GetTextureId());
    auto container = Container::Current();
    if (!container) {
        LOGE("FireExternalEvent Current container null");
        return;
    }
    auto nativeView = static_cast<AceView*>(container->GetView());
    if (!nativeView) {
        LOGE("FireExternalEvent nativeView null");
        return;
    }
    auto nativeWindow = const_cast<void*>(nativeView->GetNativeWindowById(textureId));
    if (!nativeWindow) {
        LOGE("FireExternalEvent nativeWindow invalid");
        return;
    }

    nativeXComponentImpl_->SetSurface(nativeWindow);
    nativeXComponentImpl_->SetXComponentId(xcomponent->GetId());
    auto v8NativeEngine = static_cast<V8NativeEngine*>(nativeEngine_);
    if (v8NativeEngine == nullptr) {
        LOGE("FireExternalEvent v8NativeEngine is nullptr");
        return;
    }
    std::string arguments;
    v8::Local<v8::Object> renderContext = v8NativeEngine->LoadModuleByName(xcomponent->GetLibraryName(), true,
                                                                           arguments, OH_NATIVE_XCOMPONENT_OBJ,
                                                                           reinterpret_cast<void*>(nativeXComponent_));
    renderContextXComp_.Reset(isolateXComp_, renderContext);
    auto objContext = V8Object(renderContext);
    JSRef<JSObject> obj = JSRef<JSObject>::Make(objContext);
    auto getJSValCallback = [obj](JSRef<JSVal> &jsVal) {
        jsVal = obj;
        return true;
    };
    XComponentClient::GetInstance().RegisterJSValCallback(getJSValCallback);
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolateXComp_->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    v8::TryCatch tryCatch(isolateXComp_);
    auto task = [weak = WeakClaim(this), xcomponent]() {
        auto pool = xcomponent->GetTaskPool();
        if (!pool) {
            return;
        }
        auto bridge = weak.Upgrade();
        if (bridge) {
            pool->NativeXComponentInit(
                bridge->nativeXComponent_, AceType::WeakClaim(AceType::RawPtr(bridge->nativeXComponentImpl_)));
        }
    };
    if (*delegate == nullptr) {
        LOGE("FireExternalEvent delegate is null.");
        return;
    }
    (*delegate)->PostSyncTaskToPage(task);
}

void V8DeclarativeEngine::SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher)
{
    ACE_DCHECK(engineInstance_);
    LOGD("Enter SetJsMessageDispatcher");
    engineInstance_->SetJsMessageDispatcher(dispatcher);
}

void V8DeclarativeEngine::RunGarbageCollection()
{
    if (engineInstance_) {
        v8::Isolate* isolate = engineInstance_->GetV8Isolate();
        if (isolate != nullptr) {
            v8::Isolate::Scope isolateScope(isolate);
            LOGD("Sending LowMemoryNotification to V8");
            isolate->LowMemoryNotification();
        }
    }
}

RefPtr<GroupJsBridge> V8DeclarativeEngine::GetGroupJsBridge()
{
    return AceType::MakeRefPtr<V8DeclarativeGroupJsBridge>();
}

void V8DeclarativeEngine::NotifyAppStorage(const std::string& key, const std::string& value)
{
    if (engineInstance_) {
        v8::Isolate* isolate = engineInstance_->GetV8Isolate();
        if (isolate != nullptr) {
            v8::Isolate::Scope isolateScope(isolate);
            std::string str = "notifyAppStorageChange('" + key + "', '" + value + "');\n";
            bool result = CallEvalBuf(isolate, str.c_str());
            if (!result) {
                LOGE("failed to notify AppStorage");
            }
        }
    }
}

} // namespace OHOS::Ace::Framework
