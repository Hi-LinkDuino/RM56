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

#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine_instance.h"

#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <streambuf>
#include <sys/stat.h>
#include <unistd.h>

#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/log/log.h"
#include "frameworks/bridge/common/utils/engine_helper.h"
#include "frameworks/bridge/declarative_frontend/engine/bindings_implementation.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/modules/qjs_module_manager.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_helpers.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_register.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/engine/common/runtime_constants.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_group_js_bridge.h"
#include "frameworks/core/common/ace_application_info.h"
#include "frameworks/core/image/image_cache.h"

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#include "native_engine/impl/quickjs/quickjs_native_engine.h"
#endif

extern const char _binary_stateMgmt_js_start[];
extern const char _binary_jsEnumStyle_js_start[];
extern const char _binary_jsMockSystemPlugin_js_start[];

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
extern const char* _binary_stateMgmt_js_end;
extern const char* _binary_jsEnumStyle_js_end;
extern const char* _binary_jsMockSystemPlugin_js_end;
#else
extern const char _binary_stateMgmt_js_end[];
extern const char _binary_jsEnumStyle_js_end[];
extern const char _binary_jsMockSystemPlugin_js_end[];
#endif

namespace OHOS::Ace::Framework {

thread_local JSRuntime* QJSDeclarativeEngineInstance::runtime_ = nullptr;

QJSDeclarativeEngineInstance* QJSDeclarativeEngineInstance::UnWrapEngineInstance(JSContext* ctx)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not unwrap ctx and obtain QJSDeclarativeEngineInstance object.");
    }
    return instance;
}

void QJSDeclarativeEngineInstance::FlushCommandBuffer(void* context, const std::string& command)
{
    return;
}

void QJSDeclarativeEngineInstance::SetRunningPage(const RefPtr<JsAcePage>& page)
{
    std::lock_guard<std::mutex> lock(mutex_);
    LOGD("new running page is %d.", (int)page->GetPageId());
    runningPage_ = page;
}

void QJSDeclarativeEngineInstance::SetStagingPage(const RefPtr<JsAcePage>& page)
{
    std::lock_guard<std::mutex> lock(mutex_);
    LOGD("new staging page is %d.", (int)page->GetPageId());
    stagingPage_ = page;
}

void QJSDeclarativeEngineInstance::ResetStagingPage(const RefPtr<JsAcePage>& page)
{
    std::lock_guard<std::mutex> lock(mutex_);
    LOGD("new staging page is %d.", (int)page->GetPageId());
    stagingPage_ = page;
}

RefPtr<JsAcePage> QJSDeclarativeEngineInstance::GetRunningPage(JSContext* ctx)
{
    auto* instance = QJSDeclarativeEngineInstance::UnWrapEngineInstance(ctx);
    if (instance != nullptr) {
        return instance->GetRunningPage();
    } else {
        LOGE("QJS context has no ref to engine instance. Failed!");
        return nullptr;
    }
}

RefPtr<JsAcePage> QJSDeclarativeEngineInstance::GetStagingPage(JSContext* ctx)
{
    auto* instance = QJSDeclarativeEngineInstance::UnWrapEngineInstance(ctx);
    if (instance != nullptr) {
        return instance->GetStagingPage();
    } else {
        LOGE("QJS context has no ref to engine instance. Failed!");
        return nullptr;
    }
}

JSContext* QJSDeclarativeEngineInstance::GetCurrentContext()
{
    auto engine = EngineHelper::GetCurrentEngine();
    auto qjsEngine = AceType::DynamicCast<QJSDeclarativeEngine>(engine);
    if (!qjsEngine) {
        LOGE("qjsEngine is null");
        return nullptr;
    }
    auto engineInstance = qjsEngine->GetEngineInstance();
    if (engineInstance == nullptr) {
        LOGE("engineInstance is nullptr");
        return nullptr;
    }
    return engineInstance->GetQJSContext();
}

void QJSDeclarativeEngineInstance::PushJSCommand(const RefPtr<JsCommand>& jsCommand, bool forcePush) const
{
    auto page = GetRunningPage();
    if (page == nullptr) {
        LOGE("Internal error: running page is null.");
        return;
    }

    ACE_SCOPED_TRACE("PushJSCommand");
    page->PushCommand(jsCommand);

    if (!page->CheckPageCreated() && (forcePush || (page->GetCommandSize() > (FRAGMENT_SIZE + 4)))) {
        page->FlushCommands();
    }
}

void QJSDeclarativeEngineInstance::PushJSCommand(JSContext* ctx, const RefPtr<JsCommand>& jsCommand, bool forceFlush)
{
    auto* instance = QJSDeclarativeEngineInstance::UnWrapEngineInstance(ctx);
    if (instance != nullptr) {
        instance->PushJSCommand(jsCommand, forceFlush);
    } else {
        LOGE("QJS context has no ref to engine instance. Failed!");
        return;
    }
}

void QJSDeclarativeEngineInstance::PostJsTask(JSContext* ctx, std::function<void()>&& task)
{
    auto* instance = QJSDeclarativeEngineInstance::UnWrapEngineInstance(ctx);
    if (instance != nullptr) {
        instance->GetDelegate()->PostJsTask(std::move(task));
    }
}

void QJSDeclarativeEngineInstance::TriggerPageUpdate(JSContext* ctx)
{
    auto* instance = QJSDeclarativeEngineInstance::UnWrapEngineInstance(ctx);
    if (instance != nullptr) {
        instance->GetDelegate()->TriggerPageUpdate(instance->GetRunningPage()->GetPageId());
        return;
    }
    LOGE("QJS context has no ref to instance. Failed!");
}

RefPtr<PipelineContext> QJSDeclarativeEngineInstance::GetPipelineContext(JSContext* ctx)
{
    auto* instance = QJSDeclarativeEngineInstance::UnWrapEngineInstance(ctx);
    if (instance != nullptr) {
        return instance->GetDelegate()->GetPipelineContext();
    }
    return nullptr;
}

int QJSDeclarativeEngineInstance::EvalBuf(
    JSContext* ctx, const char* buf, size_t bufLen, const char* filename, int evalFlags)
{
    JSValue val = JS_Eval(ctx, buf, bufLen, filename, evalFlags);
    int32_t ret = JS_CALL_SUCCESS;
    if (JS_IsException(val)) {
        LOGE("[Qjs Native] EvalBuf failed!");
        QJSUtils::JsStdDumpErrorAce(ctx);
        ret = JS_CALL_FAIL;
    }
    JS_FreeValue(ctx, val);
    js_std_loop(ctx);
    return ret;
}

void QJSDeclarativeEngineInstance::output_object_code(JSContext* ctx, int fho, JSValueConst obj)
{
    uint8_t* out_buf;
    size_t out_buf_len;
    out_buf = JS_WriteObject(ctx, &out_buf_len, obj, JS_WRITE_OBJ_BYTECODE);
    if (!out_buf) {
        js_std_dump_error(ctx);
        return;
    }

    int count __attribute__((unused));
    count = write(fho, out_buf, out_buf_len);
    LOGD("Bytes written to file: %d vs. %zu", count, out_buf_len);

    js_free(ctx, out_buf);
}

JSValue QJSDeclarativeEngineInstance::eval_binary_buf(JSContext* ctx, const uint8_t* buf, size_t buf_len)
{
    JSValue obj;
    obj = JS_ReadObject(ctx, buf, buf_len, JS_READ_OBJ_BYTECODE);
    if (JS_IsException(obj)) {
        LOGE("eval_binary_buf (ReadObj) failed");
        js_std_dump_error(ctx);
        return obj;
    }
    js_std_loop(ctx);
    return JS_EvalFunction(ctx, obj);
}

JSValue QJSDeclarativeEngineInstance::CompileSource(
    std::string instanceName, std::string url, const char* buf, size_t bufSize)
{
    LOGD("Compiling file url %s", url.c_str());

    ACE_SCOPED_TRACE("Compile JS");
    JSContext* ctx = GetQJSContext();

    // temporary use image cache path to store the snapshot
    std::string separator = "/";
#if defined(WINDOWS_PLATFORM)
    separator = "\\";
#endif
    std::size_t h1 = std::hash<std::string> {}(instanceName + separator + url);
    std::string filename = ImageCache::GetImageCacheFilePath() + separator;
    filename.append(std::to_string(h1));

    struct stat statbuf;
    int statres = stat(filename.c_str(), &statbuf);

    JSValue retVal = JS_NULL;
    int fhi;
    if (statres == 0 && (fhi = open(filename.c_str(), O_RDONLY)) != -1) {
        uint8_t* in_buf = (uint8_t*)malloc(statbuf.st_size + 5);
        if (!in_buf) {
            close(fhi);
            return JS_NULL;
        }
        read(fhi, in_buf, statbuf.st_size);
        close(fhi);
        retVal = eval_binary_buf(GetQJSContext(), in_buf, statbuf.st_size);
        free(in_buf);
    } else {
        // Evaluate and write to the file
        LOGD("cache file does not exist, compiling source file");
        retVal = JS_Eval(GetQJSContext(), buf, bufSize, url.c_str(), JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_COMPILE_ONLY);
        js_std_loop(ctx);
        if (JS_IsException(retVal)) {
            LOGE("Failed reading (source) JS file %s into QuickJS!", url.c_str());
            QJSUtils::JsStdDumpErrorAce(ctx);
            return JS_UNDEFINED;
        }

        int fho = creat(filename.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        if (fho != -1) {
            LOGD("Compiled cache file opened for writing %s", filename.c_str());
            output_object_code(GetQJSContext(), fho, retVal);
            close(fho);
        } else {
            LOGE("Compiled cache file writing failed to %s", filename.c_str());
        }
    }
    return retVal;
}

void QJSDeclarativeEngineInstance::CallAnimationFinishJs(JSValue animationContext)
{
    JSContext* ctx = GetQJSContext();
    QJSHandleScope handleScope(ctx);
    auto proto = QJSUtils::GetPropertyStr(ctx, animationContext, "onfinish");
    if (!JS_IsFunction(ctx, proto)) {
        LOGE("cannot find 'CallAnimationFinishJs' function from global object, this should not happen!");
        return;
    }
    LOGD("animation onfinish event call");
    JSValue retVal = JS_Call(ctx, proto, animationContext, 0, {});
    JS_FreeValue(ctx, retVal);
    js_std_loop(ctx);
}

void QJSDeclarativeEngineInstance::CallAnimationCancelJs(JSValue animationContext)
{
    JSContext* ctx = GetQJSContext();
    QJSHandleScope handleScope(ctx);
    auto proto = QJSUtils::GetPropertyStr(ctx, animationContext, "oncancel");
    if (!JS_IsFunction(ctx, proto)) {
        return;
    }

    LOGD("animation oncancel event call");
    JSValue retVal = JS_Call(ctx, proto, animationContext, 0, {});
    JS_FreeValue(ctx, retVal);
    js_std_loop(ctx);
}

bool QJSDeclarativeEngineInstance::ExecuteDocumentJS(JSValue jsCode)
{
    LOGD("Executing JS ....");
    ACE_SCOPED_TRACE("Execute JS");

    JSContext* ctx = GetQJSContext();

    JSValue result = JS_EvalFunction(ctx, jsCode);
    if (JS_IsException(result)) {
        LOGD("Failed executing JS!");
        QJSUtils::JsStdDumpErrorAce(ctx);
        return false;
    }
    js_std_loop(ctx);
    LOGD("Executing JS -- DONE OK!");
    return true;
}

const JSCFunctionListEntry moduleFunctions[] = {};

int InitializeModules(JSContext* ctx, JSModuleDef* m)
{
    QJSContext::Scope scope(ctx);
    return JS_SetModuleExportList(ctx, m, moduleFunctions, countof(moduleFunctions));
}

JSModuleDef* JsInitModule(JSContext* ctx)
{
    LOGD("registering ace module methods\n");

    JSModuleDef* m = JS_NewCModule(ctx, "ace", InitializeModules);
    if (!m)
        return nullptr;

    JS_AddModuleExportList(ctx, m, moduleFunctions, countof(moduleFunctions));
    return m;
}

JSContext* InitJSContext(JSContext* ctx1, size_t maxStackSize,
    const std::unordered_map<std::string, void*>& extraNativeObject)
{
    LOGD("QJS Creating new JS context and loading HBS module");

    ACE_SCOPED_TRACE("Init JS Context");
    if (!ctx1) {
        LOGD("QJS cannot allocate JS context");
        return nullptr;
    }

    /*
     * Note: default 260k stack is not enough, lets use a value that should be enough
     *        for five cards 512k is too small, use 1MB
     */
    JS_SetMaxStackSize(ctx1, maxStackSize);

    js_std_add_helpers(ctx1, 0, NULL);

    InitConsole(ctx1);

    /* inject hbs native functions module */
    QJSContext::Scope scope(ctx1);
    JSValue globalObj = JS_GetGlobalObject(ctx1);
    JsRegisterViews(globalObj);

    /* make 'std' and 'os' visible to non module code */
    /* dito for hbs */
    const char* str = "// import ACE own functions in sce module \n"
                      "// QJS rel 1 Sept no longer defines global but globalThis object, fix it \n"
                      "var global = globalThis;\n";

    if (JS_CALL_FAIL ==
        QJSDeclarativeEngineInstance::EvalBuf(ctx1, str, strlen(str), "JS Context initialize", JS_EVAL_TYPE_GLOBAL)) {
        LOGE("QJS created JS context but failed to init hbs, os, or std module.!");
    }

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    for (const auto& [key, value] : extraNativeObject) {
        auto nativeObjectInfo = std::make_unique<NativeObjectInfo>();
        nativeObjectInfo->nativeObject = value;
        JSValue abilityValue = JS_NewExternal(ctx1, nativeObjectInfo.release(),
            [](JSContext* ctx, void *data, void *hint) {
                std::unique_ptr<NativeObjectInfo> info(static_cast<NativeObjectInfo*>(data));
            }, nullptr);
        JS_SetPropertyStr(ctx1, globalObj, key.c_str(), abilityValue);
    }
#endif
    JS_FreeValue(ctx1, globalObj);
    return ctx1;
}

std::map<std::string, std::string> QJSDeclarativeEngineInstance::mediaResourceFileMap_;

std::unique_ptr<JsonValue> QJSDeclarativeEngineInstance::currentConfigResourceData_;

bool QJSDeclarativeEngineInstance::InitJSEnv(JSRuntime* runtime, JSContext* context,
    const std::unordered_map<std::string, void*>& extraNativeObject)
{
    ACE_SCOPED_TRACE("Init JS Env");
    if (runtime == nullptr) {
        runtime = JS_NewRuntime();
    }
    if (runtime_ != nullptr) {
        JS_FreeRuntime(runtime_);
    }
    runtime_ = runtime;
    if (runtime_ == nullptr) {
        LOGE("Qjs cannot allocate JS runtime");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
        return false;
    }

    if (context == nullptr) {
        context = JS_NewContext(runtime_);
    }
    if (context_ != nullptr) {
        JS_FreeContext(context_);
    }
    context_ = context;
    bool initRet = InitJSContext(context_, MAX_STACK_SIZE, extraNativeObject);
    if (!initRet) {
        LOGE("QJS cannot allocate JS context");
        JS_FreeRuntime(runtime_);
        return false;
    }

    auto groupJsBridge = AceType::DynamicCast<QuickJsGroupJsBridge>(frontendDelegate_->GetGroupJsBridge());
    if (!groupJsBridge || JS_CALL_FAIL == groupJsBridge->InitializeGroupJsBridge(context_)) {
        LOGE("QJSDeclarative Initialize GroupJsBridge failed!");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
        return false;
    }

    // make jsProxy end of '\0'
    std::string jsProxy(_binary_stateMgmt_js_start, _binary_stateMgmt_js_end - _binary_stateMgmt_js_start);
    std::string jsEnum(_binary_jsEnumStyle_js_start, _binary_jsEnumStyle_js_end - _binary_jsEnumStyle_js_start);
    if (!InitAceModules(jsProxy.c_str(), jsProxy.length(), "stateMgmt.js")
        || !InitAceModules(jsEnum.c_str(), jsEnum.length(), "jsEnumStyle.js")) {
        return false;
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    std::string jsMockSystemPlugin(_binary_jsMockSystemPlugin_js_start,
                                   _binary_jsMockSystemPlugin_js_end - _binary_jsMockSystemPlugin_js_start);
    if (!InitAceModules(jsMockSystemPlugin.c_str(), jsMockSystemPlugin.length(), "jsMockSystemPlugin.js")) {
        return false;
    }
#endif
    InitJsNativeModuleObject(context_);
    InitJsExportsUtilObject(context_);

    currentConfigResourceData_ = JsonUtil::CreateArray(true);
    frontendDelegate_->LoadResourceConfiguration(mediaResourceFileMap_, currentConfigResourceData_);

    return true;
}

void QJSDeclarativeEngineInstance::FreeGroupJsBridge()
{
    // free JSValue reference of channel bridge and animation bridge
    if (!frontendDelegate_) {
        LOGE("frontend delegate is null.");
        return;
    }

    auto groupJsBridge = AceType::DynamicCast<QuickJsGroupJsBridge>(frontendDelegate_->GetGroupJsBridge());
    if (!groupJsBridge) {
        LOGE("group js bridge is null.");
        return;
    }

    groupJsBridge->Uninitialize();
}

bool QJSDeclarativeEngineInstance::InitAceModules(const char* start, size_t length, const char* fileName)
{
    QJSContext::Scope scp(context_);
    if (start == nullptr || length == 0 || fileName == nullptr) {
        LOGE("Failed to init Ace modules, start, end or fileName can not be null!");
        return false;
    }
    auto result = EvalBuf(context_, start, length, fileName, JS_EVAL_TYPE_GLOBAL);
    if (result == -1) {
        EventInfo eventInfo;
        eventInfo.eventType = EXCEPTION_JS;
        eventInfo.errorType = static_cast<int32_t>(JsExcepType::JS_CONTEXT_INIT_ERR);
        EventReport::SendEvent(eventInfo);
        LOGE("JS Engine created JS context but failed to init Ace modules!");
        return false;
    }
    return true;
}

JSValue RequireNativeModule(JSContext* ctx, JSValueConst new_target, int argc, JSValueConst* argv)
{
    if ((argc != 1) || !JS_IsString(argv[0])) {
        LOGE("RequireNativeModule expects a single string as parameter");
        return JS_NULL;
    }
    ScopedString moduleName(ctx, argv[0]);

    // has already init module object
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue moduleObject = JS_GetPropertyStr(ctx, globalObj, moduleName.get());
    if (JS_IsObject(moduleObject)) {
        LOGD("has already init moduleObject %s", moduleName.get());
        JS_FreeValue(ctx, globalObj);
        return JS_DupValue(ctx, moduleObject);
    }

    // init module object first time
    JSValue jsModuleObject = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, globalObj, moduleName.get(), jsModuleObject);
    JS_FreeValue(ctx, globalObj);
    if (ModuleManager::GetInstance()->InitModule(ctx, moduleName, jsModuleObject)) {
        return JS_DupValue(ctx, jsModuleObject);
    } else {
        LOGE("init moduleObject %s failed", moduleName.get());
        return JS_NULL;
    }
    return JS_NULL;
}

void QJSDeclarativeEngineInstance::InitJsNativeModuleObject(JSContext* ctx)
{
    QJSUtils::DefineGlobalFunction(ctx, RequireNativeModule, "requireNativeModule", 1);
    ModuleManager::GetInstance()->InitTimerModule(ctx);
    ModuleManager::GetInstance()->InitSyscapModule(ctx);
}

void QJSDeclarativeEngineInstance::InitJsExportsUtilObject(JSContext* ctx)
{
    JSValue exportsUtilObj = JS_NewObject(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, globalObj, "exports", exportsUtilObj);
    JS_FreeValue(ctx, globalObj);
}

QJSDeclarativeEngineInstance::~QJSDeclarativeEngineInstance()
{
    FreeGroupJsBridge();
    if (context_) {
        JS_FreeContext(context_);
    }
    if (runtime_) {
        JS_FreeRuntime(runtime_);
    }
}

void QJSDeclarativeEngineInstance::FireAsyncEvent(const std::string& eventId, const std::string& param)
{
    LOGW("QJSDeclarativeEngineInstance FireAsyncEvent is unusable");
}

std::unique_ptr<JsonValue> QJSDeclarativeEngineInstance::GetI18nStringResource(const std::string& targetStringKey,
    const std::string& targetStringValue)
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

std::string QJSDeclarativeEngineInstance::GetMeidaResource(const std::string& targetMediaFileName)
{
    auto iter = mediaResourceFileMap_.find(targetMediaFileName);

    if (iter != mediaResourceFileMap_.end()) {
        return iter->second;
    }

    return std::string();
}


void QJSDeclarativeEngineInstance::RunGarbageCollection()
{
    if (runtime_) {
        JS_RunGC(runtime_);
    }
}

} // namespace OHOS::Ace::Framework
