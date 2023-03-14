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

#include "adapter/ohos/entrance/pa_engine/engine/quickjs/qjs_pa_engine.h"

#include <algorithm>
#include <dlfcn.h>
#include <string>
#include <unistd.h>
#include <unordered_map>

#include "form_provider_info.h"
#include "napi/native_node_api.h"
#include "napi_common_want.h"
#include "napi_remote_object.h"
#include "third_party/quickjs/message_server.h"

#include "base/i18n/localization.h"
#include "base/json/json_util.h"
#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/utils/linear_map.h"
#include "base/utils/system_properties.h"
#include "base/utils/time_util.h"
#include "base/utils/utils.h"
#include "core/common/ace_application_info.h"
#include "core/common/backend.h"
#include "core/common/container_scope.h"
#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/media_query/media_query_info.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/engine/common/runtime_constants.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"
#include "js_backend_timer_module.h"

#include "napi_common_ability.h"

namespace OHOS::Ace {

FromBindingData::FromBindingData(JSContext* ctx, JSValue value)
{
    ctx_ = ctx;
    valueFromJs_ = value;
}

void FromBindingData::Constructor(const std::string& param)
{
    JSValue paFunc = Framework::QJSUtils::GetPropertyStr(ctx_, valueFromJs_, "constructor");
    if (!JS_IsFunction(ctx_, paFunc)) {
        LOGE("Constructor is not found");
        return;
    }

    JSValue jsParam = Framework::QJSUtils::NewString(ctx_, param.c_str());
    JSValueConst argv[] = { jsParam };
    JS_Call(ctx_, paFunc, JS_UNDEFINED, countof(argv), argv);
    js_std_loop(ctx_);
    return;
}

void FromBindingData::UpdateData(const std::string& data)
{
    JSValue paFunc = Framework::QJSUtils::GetPropertyStr(ctx_, valueFromJs_, "updateData");
    if (!JS_IsFunction(ctx_, paFunc)) {
        LOGE("UpdateData is not found");
        return;
    }

    JSValue param = Framework::QJSUtils::NewString(ctx_, data.c_str());
    JSValueConst argv[] = { param };
    JS_Call(ctx_, paFunc, JS_UNDEFINED, countof(argv), argv);
    js_std_loop(ctx_);
    return;
}

std::string FromBindingData::GetDataString() const
{
    JSValue paFunc = Framework::QJSUtils::GetPropertyStr(ctx_, valueFromJs_, "getDataString");
    if (!JS_IsFunction(ctx_, paFunc)) {
        LOGE("Constructor is not found");
        return "";
    }

    JSValueConst argv[] = {};
    JS_Call(ctx_, paFunc, JS_UNDEFINED, countof(argv), argv);
    js_std_loop(ctx_);
    return "";
}

void FromBindingData::AddImageData(const std::string& name, void* buffer)
{
    JSValue paFunc = Framework::QJSUtils::GetPropertyStr(ctx_, valueFromJs_, "addImageData");
    if (!JS_IsFunction(ctx_, paFunc)) {
        LOGE("AddImageData is not found");
        return;
    }

    JSValue param = Framework::QJSUtils::NewString(ctx_, name.c_str());
    JSValueConst argv[] = { param };
    JS_Call(ctx_, paFunc, JS_UNDEFINED, countof(argv), argv);
    js_std_loop(ctx_);
    return;
}

void FromBindingData::RemoveImageData(const std::string& name)
{
    JSValue paFunc = Framework::QJSUtils::GetPropertyStr(ctx_, valueFromJs_, "removeImageData");
    if (!JS_IsFunction(ctx_, paFunc)) {
        LOGE("RemoveImageData is not found");
        return;
    }

    JSValue param = Framework::QJSUtils::NewString(ctx_, name.c_str());
    JSValueConst argv[] = { param };
    JS_Call(ctx_, paFunc, JS_UNDEFINED, countof(argv), argv);
    js_std_loop(ctx_);
    return;
}

namespace {
#define JS_CFUNC_DEF_CPP(name, length, func)                            \
    {                                                                   \
        name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_CFUNC, 0, \
        {                                                               \
            {                                                           \
                length, JS_CFUNC_generic,                               \
                {                                                       \
                    func                                                \
                }                                                       \
            }                                                           \
        }                                                               \
    }

bool UnwrapRawImageDataMap(NativeEngine* engine, NativeValue* argv, std::map<std::string, int>& rawImageDataMap)
{
    LOGI("%{public}s called.", __func__);
    auto env = reinterpret_cast<napi_env>(engine);
    auto param = reinterpret_cast<napi_value>(argv);

    if (!AppExecFwk::IsTypeForNapiValue(env, param, napi_object)) {
        LOGW("%{public}s failed, param is not napi_object.", __func__);
        return false;
    }

    napi_valuetype jsValueType = napi_undefined;
    napi_value jsProNameList = nullptr;
    uint32_t jsProCount = 0;

    NAPI_CALL_BASE(env, napi_get_property_names(env, param, &jsProNameList), false);
    NAPI_CALL_BASE(env, napi_get_array_length(env, jsProNameList, &jsProCount), false);
    LOGI("%{public}s called. Property size=%{public}d.", __func__, jsProCount);

    napi_value jsProName = nullptr;
    napi_value jsProValue = nullptr;
    for (uint32_t index = 0; index < jsProCount; index++) {
        NAPI_CALL_BASE(env, napi_get_element(env, jsProNameList, index, &jsProName), false);
        std::string strProName = AppExecFwk::UnwrapStringFromJS(env, jsProName);
        LOGI("%{public}s called. Property name=%{public}s.", __func__, strProName.c_str());
        NAPI_CALL_BASE(env, napi_get_named_property(env, param, strProName.c_str(), &jsProValue), false);
        NAPI_CALL_BASE(env, napi_typeof(env, jsProValue, &jsValueType), false);
        int natValue = AppExecFwk::UnwrapInt32FromJS(env, jsProValue);
        rawImageDataMap.emplace(strProName, natValue);
        LOGI("%{public}s called. Property value=%{public}d.", __func__, natValue);
    }
    return true;
}

JSValue JsOnCreateFinish(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsOnCreateFinish");
    return JS_NULL;
}

const JSCFunctionListEntry JS_PA_FUNCS[] = {
    JS_CFUNC_DEF_CPP("onCreateFinish", 0, JsOnCreateFinish),
};

JSValue AppLogPrint(JSContext* ctx, Framework::JsLogLevel level, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ACE_SCOPED_TRACE("AppLogPrint(level=%d)", static_cast<int32_t>(level));

    if ((argv == nullptr) || (argc == 0)) {
        LOGE("the arg is error");
        return JS_EXCEPTION;
    }
    Framework::ScopedString printLog(ctx, argv[0]);

    switch (level) {
        case Framework::JsLogLevel::DEBUG:
            APP_LOGD("app Log: %{public}s", printLog.get());
            break;
        case Framework::JsLogLevel::INFO:
            APP_LOGI("app Log: %{public}s", printLog.get());
            break;
        case Framework::JsLogLevel::WARNING:
            APP_LOGW("app Log: %{public}s", printLog.get());
            break;
        case Framework::JsLogLevel::ERROR:
            APP_LOGE("app Log: %{public}s", printLog.get());
            break;
        default:
            break;
    }

    return JS_NULL;
}

JSValue AppLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, Framework::JsLogLevel::DEBUG, value, argc, argv);
}

JSValue AppDebugLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, Framework::JsLogLevel::DEBUG, value, argc, argv);
}

JSValue AppInfoLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, Framework::JsLogLevel::INFO, value, argc, argv);
}

JSValue AppWarnLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, Framework::JsLogLevel::WARNING, value, argc, argv);
}

JSValue AppErrorLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, Framework::JsLogLevel::ERROR, value, argc, argv);
}

void InitJsConsoleObject(JSContext* ctx, const JSValue& globalObj)
{
    JSValue console;
    // app log method
    console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, AppLogPrint, "log", 1));
    JS_SetPropertyStr(ctx, console, "debug", JS_NewCFunction(ctx, AppDebugLogPrint, "debug", 1));
    JS_SetPropertyStr(ctx, console, "info", JS_NewCFunction(ctx, AppInfoLogPrint, "info", 1));
    JS_SetPropertyStr(ctx, console, "warn", JS_NewCFunction(ctx, AppWarnLogPrint, "warn", 1));
    JS_SetPropertyStr(ctx, console, "error", JS_NewCFunction(ctx, AppErrorLogPrint, "error", 1));
    JS_SetPropertyStr(ctx, globalObj, "console", console);
}

int32_t JsPaInit(JSContext* ctx, JSModuleDef* moduleDef)
{
    return JS_SetModuleExportList(ctx, moduleDef, JS_PA_FUNCS, countof(JS_PA_FUNCS));
}

JSModuleDef* InitPaModules(JSContext* ctx)
{
    LOGD("QjsPaEngine: InitPaModules");
    JSModuleDef* jsModule = JS_NewCModule(ctx, "Particle", JsPaInit);
    if (jsModule == nullptr) {
        return nullptr;
    }
    JS_AddModuleExportList(ctx, jsModule, JS_PA_FUNCS, countof(JS_PA_FUNCS));
    return jsModule;
}

bool InitJsContext(JSContext* ctx, size_t maxStackSize, int32_t instanceId,
    const QjsPaEngineInstance* qjsPaEngineInstance, const std::unordered_map<std::string, void*>& extraNativeObject)
{
    LOGI("QjsPaEngine: InitJsContext");
    if (ctx == nullptr) {
        LOGE("Qjs cannot allocate JS context");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
        return false;
    }

    // Note: default 256KB is not enough
    JS_SetMaxStackSize(ctx, maxStackSize);

    // Inject pa native functions module
    InitPaModules(ctx);

    JSValue globalObj, perfUtil;
    globalObj = JS_GetGlobalObject(ctx);
    perfUtil = JS_NewObject(ctx);

    InitJsConsoleObject(ctx, globalObj);

    for (const auto& [key, value] : extraNativeObject) {
        auto nativeObjectInfo = std::make_unique<NativeObjectInfo>();
        nativeObjectInfo->nativeObject = value;
        JSValue abilityValue = JS_NewExternal(
            ctx, nativeObjectInfo.release(),
            [](JSContext* ctx, void* data, void* hint) {
                std::unique_ptr<NativeObjectInfo> info(static_cast<NativeObjectInfo*>(data));
            },
            nullptr);
        JS_SetPropertyStr(ctx, globalObj, key.c_str(), abilityValue);
    }

    JS_FreeValue(ctx, globalObj);
    return true;
}
} // namespace
// -----------------------
// Start QjsPaEngineInstance
// -----------------------
JSValue QjsPaEngineInstance::FireJsEvent(const std::string& eventId, const std::string& param)
{
    LOGI("FireJsEvent");
    JSContext* ctx = GetQjsContext();
    ACE_DCHECK(ctx);
    Framework::QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue callJsFunc = Framework::QJSUtils::GetPropertyStr(ctx, globalObj, "callJS");
    if (!JS_IsFunction(ctx, callJsFunc)) {
        LOGE("cannot find 'callJS' function from global object, this should not happen!");
        JS_FreeValue(ctx, globalObj);
        return JS_UNDEFINED;
    }

    JSValueConst argv[] = {
        Framework::QJSUtils::NewString(ctx, std::to_string(0).c_str()),
        Framework::QJSUtils::ParseJSON(ctx, param.c_str(), param.size(), nullptr),
    };

    JSValue retVal = JS_Call(ctx, callJsFunc, JS_UNDEFINED, countof(argv), argv);
    js_std_loop(ctx);
    JS_FreeValue(ctx, globalObj);

    // It is up to the caller to check this value. No exception checks here.
    return retVal;
}

bool QjsPaEngineInstance::InitJsEnv(
    JSRuntime* runtime, JSContext* context, const std::unordered_map<std::string, void*>& extraNativeObject)
{
    LOGI("InitJsEnv");
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
    if (!InitJsContext(context_, Framework::MAX_STACK_SIZE, instanceId_, this, extraNativeObject)) {
        LOGE("Qjs cannot allocate JS context");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
        JS_FreeRuntime(runtime_);
        return false;
    }

    const char* str = "\n"
                      "var global = globalThis;\n"
                      "globalThis.pa = {default: {}};\n"
                      "function $app_define$(page, packageName, parseContent) {\n"
                      "    const module = {exports: {}};\n"
                      "    parseContent({}, module.exports, module);\n"
                      "    globalThis.pa = module.exports;\n"
                      "}\n"
                      "function $app_bootstrap$() {}\n";

    JSValue CppToJSRet = JS_Eval(context_, str, strlen(str), "<evalScript>", JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(CppToJSRet)) {
        LOGE("eval framework error");
    }

    JS_FreeValue(context_, CppToJSRet);
    js_std_loop(context_);
    return true;
}

QjsPaEngineInstance::~QjsPaEngineInstance()
{
    if (context_) {
        JS_FreeContext(context_);
    }
    if (runtime_) {
        JS_FreeRuntime(runtime_);
    }
}
// -----------------------
// Start QjsPaEngine
// -----------------------

bool QjsPaEngine::Initialize(const RefPtr<BackendDelegate>& delegate)
{
    ACE_SCOPED_TRACE("QjsPaEngine::Initialize");
    LOGI("Initialize");

    JSRuntime* runtime = nullptr;
    JSContext* context = nullptr;

    // put JS_NewContext as early as possible to make stack_top in context
    // closer to the top stack frame pointer of JS thread.
    runtime = JS_NewRuntime();
    if (runtime != nullptr) {
        context = JS_NewContext(runtime);
    }
    LOGD("QjsPaEngine initialize");
#ifdef ENABLE_JS_DEBUG
    LOGI("QjsPaEngine debug mode");
    std::string instanceName = GetInstanceName();
    if (instanceName.empty()) {
        LOGE("GetInstanceName fail, %s", instanceName.c_str());
        return false;
    }
    constexpr int32_t COMPONENT_NAME_MAX_LEN = 100;
    char componentName[COMPONENT_NAME_MAX_LEN];
    if (!DBG_CopyComponentNameFromAce(instanceName.c_str(), componentName, COMPONENT_NAME_MAX_LEN)) {
        LOGE("GetInstanceName strcpy_s fail");
        return false;
    }
    DBG_SetComponentName(componentName, strlen(componentName));
#endif
    LoadLibrary();
    nativeEngine_ = new QuickJSNativeEngine(runtime, context, static_cast<void*>(this));
    ACE_DCHECK(delegate);
    delegate->AddTaskObserver([nativeEngine = nativeEngine_, id = instanceId_]() {
        ContainerScope scope(id);
        nativeEngine->Loop(LOOP_NOWAIT);
    });

    engineInstance_ = AceType::MakeRefPtr<QjsPaEngineInstance>(delegate, instanceId_);
    bool ret = engineInstance_->InitJsEnv(runtime, context, GetExtraNativeObject());
    JsBackendTimerModule::GetInstance()->InitTimerModule(nativeEngine_, delegate);
    SetPostTask(nativeEngine_);
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    nativeEngine_->CheckUVLoop();
#endif
    if (delegate && delegate->GetAssetManager()) {
        std::string packagePath = delegate->GetAssetManager()->GetLibPath();
        if (!packagePath.empty()) {
            auto qjsNativeEngine = static_cast<QuickJSNativeEngine*>(nativeEngine_);
            qjsNativeEngine->SetPackagePath(packagePath);
        }
    }
    RegisterWorker();
    return ret;
}

void QjsPaEngine::SetPostTask(NativeEngine* nativeEngine)
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
            ContainerScope scope(id);
            if (nativeEngine == nullptr) {
                return;
            }
            nativeEngine->Loop(LOOP_NOWAIT, needSync);
        });
    };
    nativeEngine_->SetPostTask(postTask);
}

void QjsPaEngine::RegisterInitWorkerFunc()
{
    auto&& initWorkerFunc = [weak = AceType::WeakClaim(this)](NativeEngine* nativeEngine) {
        LOGI("WorkerCore RegisterInitWorkerFunc called");
        auto paEngine = weak.Upgrade();
        if (nativeEngine == nullptr) {
            LOGE("nativeEngine is nullptr");
            return;
        }
        auto qjsNativeEngine = static_cast<QuickJSNativeEngine*>(nativeEngine);
        if (qjsNativeEngine == nullptr) {
            LOGE("qjsNativeEngine is nullptr");
            return;
        }

        JSContext* ctx = qjsNativeEngine->GetContext();
        if (ctx == nullptr) {
            LOGE("ctx is nullptr");
            return;
        }

        // Create a stack-allocated handle scope.
        Framework::QJSHandleScope handleScope(ctx);

        // Note: default 256KB is not enough
        JS_SetMaxStackSize(ctx, Framework::MAX_STACK_SIZE);

        JSValue globalObj = JS_GetGlobalObject(ctx);
        InitJsConsoleObject(ctx, globalObj);
        JS_FreeValue(ctx, globalObj);

        for (const auto& [key, value] : paEngine->GetExtraNativeObject()) {
            auto nativeObjectInfo = std::make_unique<NativeObjectInfo>();
            nativeObjectInfo->nativeObject = value;
            JSValue abilityValue = JS_NewExternal(
                ctx, nativeObjectInfo.release(),
                [](JSContext* ctx, void* data, void* hint) {
                    std::unique_ptr<NativeObjectInfo> info(static_cast<NativeObjectInfo*>(data));
                },
                nullptr);
            JS_SetPropertyStr(ctx, globalObj, key.c_str(), abilityValue);
        }
    };
    nativeEngine_->SetInitWorkerFunc(initWorkerFunc);
}

void QjsPaEngine::RegisterAssetFunc()
{
    auto weakDelegate = AceType::WeakClaim(AceType::RawPtr(engineInstance_->GetDelegate()));
    auto&& assetFunc = [weakDelegate](const std::string& uri, std::vector<uint8_t>& content, std::string& ami) {
        LOGI("WorkerCore RegisterAssetFunc called");
        auto delegate = weakDelegate.Upgrade();
        if (delegate == nullptr) {
            LOGE("delegate is nullptr");
            return;
        }
        delegate->GetResourceData(uri, content, ami);
    };
    nativeEngine_->SetGetAssetFunc(assetFunc);
}

void QjsPaEngine::RegisterWorker()
{
    RegisterInitWorkerFunc();
    RegisterAssetFunc();
}

QjsPaEngine::~QjsPaEngine()
{
    UnloadLibrary();
    engineInstance_->GetDelegate()->RemoveTaskObserver();
    if (nativeEngine_ != nullptr) {
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
        nativeEngine_->CancelCheckUVLoop();
#endif
        delete nativeEngine_;
    }
    if (engineInstance_ && engineInstance_->GetQjsRuntime()) {
        JS_RunGC(engineInstance_->GetQjsRuntime());
    }
}

inline int32_t GetJsInt32Val(JSContext* ctx, JSValueConst value)
{
    int32_t val = 0;
    if (JS_IsNumber(value) && (JS_ToInt32(ctx, &val, value)) < 0) {
        val = 0;
    }
    return val;
}

inline std::string GetJsStringVal(JSContext* ctx, JSValueConst value)
{
    std::string val;
    if (!JS_IsString(value)) {
        return val;
    }

    auto cstr = JS_ToCString(ctx, value);
    if (cstr == nullptr) {
        return val;
    }
    val = cstr;
    JS_FreeCString(ctx, cstr);

    return val;
}

inline std::string ToJSONStringInt(std::string sKey, std::string sValue)
{
    char szDoubleQuotes[] = "\"";
    char szColon[] = ":";
    std::string strResult;
    strResult.append(szDoubleQuotes);
    strResult.append(sKey);
    strResult.append(szDoubleQuotes);

    strResult.append(szColon);
    strResult.append(sValue);
    return strResult;
}

inline std::string ToJSONString(std::string sKey, std::string sValue)
{
    char szDoubleQuotes[] = "\"";
    char szColon[] = ":";
    std::string strResult;
    strResult.append(szDoubleQuotes);
    strResult.append(sKey);
    strResult.append(szDoubleQuotes);

    strResult.append(szColon);
    strResult.append(szDoubleQuotes);
    strResult.append(sValue);
    strResult.append(szDoubleQuotes);
    return strResult;
}

void QjsPaEngine::LoadJs(const std::string& url, const OHOS::AAFwk::Want& want)
{
    LOGI("QjsPaEngine LoadJs: %{private}s", url.c_str());
    ACE_SCOPED_TRACE("QjsPaEngine::LoadJs");
    ACE_DCHECK(engineInstance_);

    JSContext* ctx = engineInstance_->GetQjsContext();

    // Create a stack-allocated handle scope.
    Framework::QJSHandleScope handleScope(ctx);

    // Memorize the context that this JSContext is working with.
    JS_SetContextOpaque(ctx, reinterpret_cast<void*>(AceType::RawPtr(engineInstance_)));

    JSValue globalObj = JS_GetGlobalObject(ctx);

    std::string jsContent;
    if (!engineInstance_->GetDelegate()->GetAssetContent(url, jsContent)) {
        LOGE("js file load failed!");
        JS_FreeValue(ctx, globalObj);
        return;
    }

    JSValue CppToJSRet = JS_Eval(ctx, jsContent.c_str(), jsContent.size(), url.c_str(), JS_EVAL_TYPE_GLOBAL);

    JS_FreeValue(ctx, CppToJSRet);

    // call onStart
    JSValue paObj;
    paObj = Framework::QJSUtils::GetPropertyStr(ctx, globalObj, "pa");
    if (!JS_IsObject(paObj)) {
        LOGE("get paObj error");
    }

    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!Framework::CheckAndGetJsProperty(ctx, paObj, &pTab, &len)) {
        LOGE("PA: Framework::CheckAndGetJsProperty error");
    }
    LOGI("PA: QjsPaEngine pa propey num %{public}u", len);

    BackendType type = engineInstance_->GetDelegate()->GetType();
    JSValue paStartFunc = JS_NULL;
    if (type == BackendType::SERVICE) {
        paStartFunc = Framework::QJSUtils::GetPropertyStr(ctx, paObj, "onStart");
    } else if (type == BackendType::DATA) {
        paStartFunc = Framework::QJSUtils::GetPropertyStr(ctx, paObj, "onInitialized");
    } else if (type == BackendType::FORM) {
        LOGI("Form Ability LoadJS finish.");
        JS_FreeValue(ctx, globalObj);
        return;
    } else {
        LOGE("backend type not support");
    }

    if (!JS_IsFunction(ctx, paStartFunc)) {
        LOGE("paStartFunc is not found");
        JS_FreeValue(ctx, globalObj);
        return;
    }

    JSValue retVal = JS_UNDEFINED;
    if (type == BackendType::SERVICE) {
        JSValueConst argv[] = {};
        retVal = Framework::QJSUtils::Call(ctx, paStartFunc, paObj, countof(argv), argv);
    } else if (type == BackendType::DATA) {
        const auto& nativeObjects = GetExtraNativeObject();
        auto it = nativeObjects.find("ability");
        if (it == nativeObjects.end() || it->second == nullptr) {
            LOGE("Can't find ability object");
            return;
        }
        const std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo =
            reinterpret_cast<Ability*>(it->second)->GetAbilityInfo();
        const AppExecFwk::AbilityInfo abilityInfoInstance = *(abilityInfo.get());
        napi_value abilityInfoNapi =
            AppExecFwk::ConvertAbilityInfo(reinterpret_cast<napi_env>(nativeEngine_), abilityInfoInstance);

        NativeValue* abilityInfoNative = reinterpret_cast<NativeValue*>(abilityInfoNapi);
        JSValue abilityInfoJS = static_cast<JSValue>(*abilityInfoNative);
        JSValueConst argv[] = { abilityInfoJS };
        retVal = Framework::QJSUtils::Call(ctx, paStartFunc, paObj, countof(argv), argv);
    } else {
        LOGE("PA: QjsPaEngine backend type not support");
    }

    if (JS_IsException(retVal)) {
        LOGE("PA: QjsPaEngine QJSUtils::Call IsException");
    } else {
        LOGI("PA: QjsPaEngine QJSUtils::Call ok");
    }

    js_std_loop(ctx);
    JS_FreeValue(ctx, retVal);
    JS_FreeValue(ctx, globalObj);
}

void QjsPaEngine::SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher)
{
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetJsMessageDispatcher(dispatcher);
}

void QjsPaEngine::FireAsyncEvent(const std::string& eventId, const std::string& param)
{
    std::string callBuf = std::string("[{\"args\": [\"")
                              .append(eventId)
                              .append("\",")
                              .append(param)
                              .append("], \"method\":\"fireEvent\"}]");
    LOGD("FireAsyncEvent string: %{private}s", callBuf.c_str());

    ACE_DCHECK(engineInstance_);
    JSValue cppToJsRet = engineInstance_->FireJsEvent(eventId, callBuf);
    if (JS_IsException(cppToJsRet)) {
        LOGE("Qjs FireAsyncEvent FAILED !! jsCall: %{private}s", callBuf.c_str());
    }
    JS_FreeValue(engineInstance_->GetQjsContext(), cppToJsRet);
}

void QjsPaEngine::FireSyncEvent(const std::string& eventId, const std::string& param)
{
    std::string callBuf = std::string("[{\"args\": [\"")
                              .append(eventId)
                              .append("\",")
                              .append(param)
                              .append("], \"method\":\"fireEventSync\"}]");
    LOGD("FireSyncEvent string: %{private}s", callBuf.c_str());

    ACE_DCHECK(engineInstance_);
    JSValue cppToJsRet = engineInstance_->FireJsEvent(eventId, callBuf.c_str());
    if (JS_IsException(cppToJsRet)) {
        LOGE("Qjs FireSyncEvent FAILED !! jsCall: %{private}s", callBuf.c_str());
    }
    JS_FreeValue(engineInstance_->GetQjsContext(), cppToJsRet);
}

void QjsPaEngine::DestroyApplication(const std::string& packageName)
{
    LOGI("DestroyApplication: destroy app instance from jsfwk");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue paObj = Framework::QJSUtils::GetPropertyStr(ctx, globalObj, "pa");
    if (!JS_IsObject(paObj)) {
        LOGE("get paObj error");
    }
    JSValue paDestroyFunc = Framework::QJSUtils::GetPropertyStr(ctx, paObj, "onStop");
    if (!JS_IsFunction(ctx, paDestroyFunc)) {
        LOGE("paDestroyFunc is not found, cannot destroy page instance!");
        JS_FreeValue(ctx, globalObj);
        return;
    }

    JSValue name = Framework::QJSUtils::NewString(ctx, packageName.c_str());
    JSValueConst argv[] = { name };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paDestroyFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paDestroyFunc FAILED!");
    }

    JS_FreeValue(ctx, globalObj);
}

void QjsPaEngine::OnCommandApplication(const std::string& intent, int startId)
{
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue paObj = Framework::QJSUtils::GetPropertyStr(ctx, globalObj, "pa");
    if (!JS_IsObject(paObj)) {
        LOGE("get paObj error");
    }
    JSValue paCommandFunc = Framework::QJSUtils::GetPropertyStr(ctx, paObj, "onCommand");
    if (!JS_IsFunction(ctx, paCommandFunc)) {
        LOGE("paCommandFunc is not found!");
        JS_FreeValue(ctx, globalObj);
        return;
    }

    JSValueConst argv[] = {
        Framework::QJSUtils::NewString(ctx, intent.c_str()),
        JS_NewInt32(ctx, startId),
    };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paCommandFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paCommandFunc FAILED!");
    }

    JS_FreeValue(ctx, globalObj);
}

JSValue QjsPaEngine::GetPaFunc(const std::string& funcName)
{
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue paObj = Framework::QJSUtils::GetPropertyStr(ctx, globalObj, "pa");
    if (!JS_IsObject(paObj)) {
        LOGE("PA: get paObj error");
        return JS_UNDEFINED;
    }
    JSValue paFunc = Framework::QJSUtils::GetPropertyStr(ctx, paObj, funcName.c_str());
    if (!JS_IsFunction(ctx, paFunc)) {
        LOGE("PA: paFunc is not found,! %{public}s", funcName.c_str());
        JS_FreeValue(ctx, globalObj);
        return JS_UNDEFINED;
    }
    LOGI("GetPaFunc: funcName success: %{public}s", funcName.c_str());
    JS_FreeValue(ctx, globalObj);
    return paFunc;
}

void QjsPaEngine::LoadLibrary()
{
#ifdef _ARM_
    const char* rdbPath = "/system/lib/module/data/librdb.z.so";
#else
    const char* rdbPath = "/system/lib64/module/data/librdb.z.so";
#endif
    if (strlen(rdbPath) == 0) {
        LOGE("module path is empty");
        return;
    }

    libRdb_ = dlopen(rdbPath, RTLD_LAZY);
    if (libRdb_ == nullptr) {
        LOGE("dlopen failed: %{public}s", dlerror());
    }

    rdbValueBucketNewInstance_ = reinterpret_cast<RdbValueBucketNewInstance>(
        dlsym(libRdb_, "NAPI_OHOS_Data_RdbJsKit_ValuesBucketProxy_NewInstance"));
    if (rdbValueBucketNewInstance_ == nullptr) {
        LOGE("symbol not found: %{public}s", dlerror());
    }

    rdbValueBucketGetNativeObject_ = reinterpret_cast<RdbValueBucketGetNativeObject>(
        dlsym(libRdb_, "NAPI_OHOS_Data_RdbJsKit_ValuesBucketProxy_GetNativeObject"));
    if (rdbValueBucketGetNativeObject_ == nullptr) {
        LOGE("symbol not found: %{public}s", dlerror());
    }

    rdbResultSetProxyNewInstance_ = reinterpret_cast<RdbResultSetProxyNewInstance>(
        dlsym(libRdb_, "NAPI_OHOS_Data_RdbJsKit_ResultSetProxy_NewInstance"));
    if (rdbResultSetProxyNewInstance_ == nullptr) {
        LOGE("symbol not found: %{public}s", dlerror());
    }

    rdbResultSetProxyGetNativeObject_ = reinterpret_cast<RdbResultSetProxyGetNativeObject>(
        dlsym(libRdb_, "NAPI_OHOS_Data_RdbJsKit_ResultSetProxy_GetNativeObject"));
    if (rdbResultSetProxyGetNativeObject_ == nullptr) {
        LOGE("symbol not found: %{public}s", dlerror());
    }

#ifdef _ARM_
    const char* dataAbilityPath = "/system/lib/module/data/libdataability.z.so";
#else
    const char* dataAbilityPath = "/system/lib64/module/data/libdataability.z.so";
#endif
    if (strlen(dataAbilityPath) == 0) {
        LOGE("module path is empty");
        return;
    }

    libDataAbility_ = dlopen(dataAbilityPath, RTLD_LAZY);
    if (libDataAbility_ == nullptr) {
        LOGE("dlopen failed: %{public}s", dlerror());
    }

    dataAbilityPredicatesNewInstance_ = reinterpret_cast<DataAbilityPredicatesNewInstance>(
        dlsym(libDataAbility_, "NAPI_OHOS_Data_DataAbilityJsKit_DataAbilityPredicatesProxy_NewInstance"));
    if (dataAbilityPredicatesNewInstance_ == nullptr) {
        LOGE("symbol not found: %{public}s", dlerror());
    }

    dataAbilityPredicatesGetNativeObject_ = reinterpret_cast<DataAbilityPredicatesGetNativeObject>(
        dlsym(libDataAbility_, "NAPI_OHOS_Data_DataAbilityJsKit_DataAbilityPredicatesProxy_GetNativeObject"));
    if (dataAbilityPredicatesGetNativeObject_ == nullptr) {
        LOGE("symbol not found: %{public}s", dlerror());
    }
}

void QjsPaEngine::UnloadLibrary()
{
    dlclose(libRdb_);
    dlclose(libDataAbility_);
}

int32_t QjsPaEngine::Insert(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value)
{
    LOGI("Insert");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("insert");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());

    napi_env env = reinterpret_cast<napi_env>(nativeEngine_);
    napi_value argNapiValue = rdbValueBucketNewInstance_(env, const_cast<OHOS::NativeRdb::ValuesBucket&>(value));
    NativeValue* argNapiNativeValue = reinterpret_cast<NativeValue*>(argNapiValue);
    JSValue argNapiJSValue = (JSValue)*argNapiNativeValue;
    JSValueConst argv[] = { argUriJSValue, argNapiJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return 0;
    }
    return GetJsInt32Val(ctx, retVal);
}

std::shared_ptr<OHOS::NativeRdb::AbsSharedResultSet> QjsPaEngine::Query(
    const Uri& uri, const std::vector<std::string>& columns, const OHOS::NativeRdb::DataAbilityPredicates& predicates)
{
    LOGI("Query");
    std::shared_ptr<OHOS::NativeRdb::AbsSharedResultSet> resultSet = nullptr;
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("query");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());

    napi_env env = reinterpret_cast<napi_env>(nativeEngine_);
    napi_value argColumnsNapiValue = nullptr;
    napi_create_array(env, &argColumnsNapiValue);
    bool isArray = false;
    if (napi_is_array(env, argColumnsNapiValue, &isArray) != napi_ok || !isArray) {
        LOGE("create array failed");
        return resultSet;
    }

    int32_t index = 0;
    for (auto column : columns) {
        napi_value result = nullptr;
        napi_create_string_utf8(env, column.c_str(), column.length(), &result);
        napi_set_element(env, argColumnsNapiValue, index++, result);
    }

    NativeValue* argColumnsNativeValue = reinterpret_cast<NativeValue*>(argColumnsNapiValue);
    JSValue argColumnsJSValue = (JSValue)*argColumnsNativeValue;

    OHOS::NativeRdb::DataAbilityPredicates* predicatesPtr = new OHOS::NativeRdb::DataAbilityPredicates();
    *predicatesPtr = predicates;
    napi_value argPredicatesNapiValue = dataAbilityPredicatesNewInstance_(env, predicatesPtr);
    NativeValue* argPredicatesNativeValue = reinterpret_cast<NativeValue*>(argPredicatesNapiValue);
    if (argPredicatesNativeValue == nullptr) {
        LOGE("Query argPredicatesNativeValue is nullptr");
        return resultSet;
    }
    JSValue argPredicatesJSValue = (JSValue)*argPredicatesNativeValue;

    JSValueConst argv[] = { argUriJSValue, argColumnsJSValue, argPredicatesJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal) || JS_IsUndefined(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return resultSet;
    }

    auto nativeObject = rdbResultSetProxyGetNativeObject_(
        env, reinterpret_cast<napi_value>(QuickJSNativeEngine::JSValueToNativeValue(nativeEngine_, retVal)));
    if (nativeObject == nullptr) {
        LOGE("AbsSharedResultSet from JS to Native failed");
        return resultSet;
    }

    resultSet.reset(nativeObject);
    return resultSet;
}

int32_t QjsPaEngine::Update(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value,
    const OHOS::NativeRdb::DataAbilityPredicates& predicates)
{
    LOGI("Update");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("update");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());

    napi_env env = reinterpret_cast<napi_env>(nativeEngine_);
    napi_value argNapiValue = rdbValueBucketNewInstance_(env, const_cast<OHOS::NativeRdb::ValuesBucket&>(value));
    NativeValue* argNapiNativeValue = reinterpret_cast<NativeValue*>(argNapiValue);
    JSValue argNapiJSValue = (JSValue)*argNapiNativeValue;

    OHOS::NativeRdb::DataAbilityPredicates* predicatesPtr = new OHOS::NativeRdb::DataAbilityPredicates();
    *predicatesPtr = predicates;
    napi_value argPredicatesNapiValue = dataAbilityPredicatesNewInstance_(env, predicatesPtr);
    NativeValue* argPredicatesNativeValue = reinterpret_cast<NativeValue*>(argPredicatesNapiValue);
    if (argPredicatesNativeValue == nullptr) {
        LOGE("Update argPredicatesNativeValue is nullptr");
        return 0;
    }
    JSValue argPredicatesJSValue = (JSValue)*argPredicatesNativeValue;

    JSValueConst argv[] = { argUriJSValue, argNapiJSValue, argPredicatesJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return 0;
    }
    return GetJsInt32Val(ctx, retVal);
}

int32_t QjsPaEngine::Delete(const Uri& uri, const OHOS::NativeRdb::DataAbilityPredicates& predicates)
{
    LOGI("Delete");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("delete");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());

    napi_env env = reinterpret_cast<napi_env>(nativeEngine_);
    OHOS::NativeRdb::DataAbilityPredicates* predicatesPtr = new OHOS::NativeRdb::DataAbilityPredicates();
    *predicatesPtr = predicates;
    napi_value argPredicatesNapiValue = dataAbilityPredicatesNewInstance_(env, predicatesPtr);
    NativeValue* argPredicatesNativeValue = reinterpret_cast<NativeValue*>(argPredicatesNapiValue);
    if (argPredicatesNativeValue == nullptr) {
        LOGE("Delete argPredicatesNativeValue is nullptr");
        return 0;
    }
    JSValue argPredicatesJSValue = (JSValue)*argPredicatesNativeValue;

    JSValueConst argv[] = { argUriJSValue, argPredicatesJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return 0;
    }
    return GetJsInt32Val(ctx, retVal);
}

int32_t QjsPaEngine::BatchInsert(const Uri& uri, const std::vector<OHOS::NativeRdb::ValuesBucket>& values)
{
    LOGI("BatchInsert");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("batchInsert");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());

    napi_env env = reinterpret_cast<napi_env>(nativeEngine_);
    napi_value argColumnsNapiValue = nullptr;
    napi_create_array(env, &argColumnsNapiValue);
    bool isArray = false;
    if (napi_is_array(env, argColumnsNapiValue, &isArray) != napi_ok || !isArray) {
        LOGE("create array failed");
        return 0;
    }

    int32_t index = 0;
    for (auto value : values) {
        napi_value result = rdbValueBucketNewInstance_(env, const_cast<OHOS::NativeRdb::ValuesBucket&>(value));
        napi_set_element(env, argColumnsNapiValue, index++, result);
    }

    NativeValue* argColumnsNativeValue = reinterpret_cast<NativeValue*>(argColumnsNapiValue);
    JSValue argColumnsJSValue = (JSValue)*argColumnsNativeValue;

    JSValueConst argv[] = { argUriJSValue, argColumnsJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return 0;
    }
    return GetJsInt32Val(ctx, retVal);
}

std::string QjsPaEngine::GetType(const Uri& uri)
{
    LOGI("GetType");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("getType");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());

    JSValueConst argv[] = { argUriJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return std::string();
    }
    return GetJsStringVal(ctx, retVal);
}

std::vector<std::string> QjsPaEngine::GetFileTypes(const Uri& uri, const std::string& mimeTypeFilter)
{
    LOGI("GetFileTypes");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    std::vector<std::string> ret;
    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("getFileTypes");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());
    JSValue argMimeTypeFilterJSValue = JS_NewString(ctx, mimeTypeFilter.c_str());

    JSValueConst argv[] = { argUriJSValue, argMimeTypeFilterJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return ret;
    }

    if (!JS_IsArray(ctx, retVal)) {
        LOGE("GetFileTypes return not array");
        return ret;
    }

    int32_t length = Framework::QJSUtils::JsGetArrayLength(ctx, retVal);
    for (int i = 0; i < length; i++) {
        JSValue itemVal = JS_GetPropertyUint32(ctx, retVal, i);
        ret.push_back(GetJsStringVal(ctx, itemVal));
    }

    return ret;
}

int32_t QjsPaEngine::OpenFile(const Uri& uri, const std::string& mode)
{
    LOGI("OpenFile");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("openFile");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());
    JSValue argModeJSValue = JS_NewString(ctx, mode.c_str());

    JSValueConst argv[] = { argUriJSValue, argModeJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return 0;
    }
    return GetJsInt32Val(ctx, retVal);
}

int32_t QjsPaEngine::OpenRawFile(const Uri& uri, const std::string& mode)
{
    LOGI("OpenRawFile");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("openRawFile");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());
    JSValue argModeJSValue = JS_NewString(ctx, mode.c_str());

    JSValueConst argv[] = { argUriJSValue, argModeJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return 0;
    }
    return GetJsInt32Val(ctx, retVal);
}

Uri QjsPaEngine::NormalizeUri(const Uri& uri)
{
    LOGI("NormalizeUri");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Uri ret("");
    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("normalizeUri");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());

    JSValueConst argv[] = { argUriJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return ret;
    }

    return Uri(GetJsStringVal(ctx, retVal));
}

Uri QjsPaEngine::DenormalizeUri(const Uri& uri)
{
    LOGI("DenormalizeUri");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Uri ret("");
    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("denormalizeUri");

    JSValue argUriJSValue = JS_NewString(ctx, uri.ToString().c_str());

    JSValueConst argv[] = { argUriJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs paFunc FAILED!");
        return ret;
    }

    return Uri(GetJsStringVal(ctx, retVal));
}

sptr<IRemoteObject> QjsPaEngine::OnConnectService(const OHOS::AAFwk::Want& want)
{
    LOGI("OnConnectService");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);
    Framework::QJSHandleScope handleScope(ctx);
    Framework::QJSContext::Scope scope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue paObj = Framework::QJSUtils::GetPropertyStr(ctx, globalObj, "pa");
    if (!JS_IsObject(paObj)) {
        LOGE("get onConnect function error");
        return nullptr;
    }
    JSValue onConnectFunc = Framework::QJSUtils::GetPropertyStr(ctx, paObj, "onConnect");
    if (!JS_IsFunction(ctx, onConnectFunc)) {
        LOGE("onConnect function is not found!");
        JS_FreeValue(ctx, globalObj);
        return nullptr;
    }
    napi_value napiWant = OHOS::AppExecFwk::WrapWant(reinterpret_cast<napi_env>(nativeEngine_), want);
    NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
    JSValue jsWant = (JSValue)*nativeWant;
    JSValueConst argv[] = { jsWant };
    JSValue retVal = Framework::QJSUtils::Call(ctx, onConnectFunc, JS_UNDEFINED, countof(argv), argv);
    js_std_loop(ctx);
    if (JS_IsException(retVal)) {
        LOGE("Qjs onConnectService FAILED!");
        JS_FreeValue(ctx, globalObj);
        JS_FreeValue(ctx, retVal);
        return nullptr;
    } else {
        NativeValue* nativeValue = QuickJSNativeEngine::JSValueToNativeValue(nativeEngine_, retVal);
        auto remoteObj = NAPI_ohos_rpc_getNativeRemoteObject(
            reinterpret_cast<napi_env>(nativeEngine_), reinterpret_cast<napi_value>(nativeValue));
        JS_FreeValue(ctx, globalObj);
        return remoteObj;
    }
}

void QjsPaEngine::OnDisconnectService(const OHOS::AAFwk::Want& want)
{
    LOGI("OnDisconnectService");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue paObj = Framework::QJSUtils::GetPropertyStr(ctx, globalObj, "pa");
    if (!JS_IsObject(paObj)) {
        LOGE("get onDisconnect function error");
        return;
    }
    JSValue onDisConnectFunc = Framework::QJSUtils::GetPropertyStr(ctx, paObj, "onDisconnect");
    if (!JS_IsFunction(ctx, onDisConnectFunc)) {
        LOGE("onDisconnect function is not found!");
        JS_FreeValue(ctx, globalObj);
        return;
    }
    napi_value napiWant = OHOS::AppExecFwk::WrapWant(reinterpret_cast<napi_env>(nativeEngine_), want);
    NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
    JSValue jsWant = (JSValue)*nativeWant;
    JSValueConst argv[] = { jsWant };
    JSValue retVal = Framework::QJSUtils::Call(ctx, onDisConnectFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs OnDisconnectService FAILED!");
    }
    JS_FreeValue(ctx, globalObj);
    JS_FreeValue(ctx, retVal);
}

void QjsPaEngine::OnCreate(const OHOS::AAFwk::Want& want)
{
    LOGI("PA: QjsPaEngine OnCreate");
    // call onCreate
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);
    Framework::QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue paObj = Framework::QJSUtils::GetPropertyStr(ctx, globalObj, "pa");
    if (!JS_IsObject(paObj)) {
        LOGE("OnCreate failed, get pa object error");
        return;
    }

    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!Framework::CheckAndGetJsProperty(ctx, paObj, &pTab, &len)) {
        LOGE("PA: Framework::CheckAndGetJsProperty error");
    }
    LOGI("PA: QjsPaEngine pa propey num %{public}u", len);
    JSValue paStartFunc = Framework::QJSUtils::GetPropertyStr(ctx, paObj, "onCreate");
    if (!JS_IsFunction(ctx, paStartFunc)) {
        LOGE("onCreate func is not found.");
        JS_FreeValue(ctx, globalObj);
        return;
    }

    napi_value napiWant = OHOS::AppExecFwk::WrapWant(reinterpret_cast<napi_env>(nativeEngine_), want);
    NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
    JSValue jsWant = (JSValue)*nativeWant;
    JSValueConst argv[] = { jsWant };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paStartFunc, paObj, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("PA: QjsPaEngine QJSUtils::Call IsException");
    }

    std::string jsonStr;
    JSValue formJsonData = Framework::QJSUtils::GetPropertyStr(ctx, retVal, "data");
    if (JS_IsString(formJsonData)) {
        Framework::ScopedString strValue(ctx, formJsonData);
        jsonStr = strValue.get();
        LOGI("Add FormBindingData json:%{public}s", jsonStr.c_str());
    }
    AppExecFwk::FormProviderData formData = AppExecFwk::FormProviderData(jsonStr);
    JSValue formImageData = Framework::QJSUtils::GetPropertyStr(ctx, retVal, "image");
    if (JS_IsObject(formImageData)) {
        std::map<std::string, int> rawImageDataMap;
        NativeValue* nativeValue = reinterpret_cast<NativeValue*>(&formImageData);
        UnwrapRawImageDataMap(nativeEngine_, nativeValue, rawImageDataMap);
        for (const auto& data : rawImageDataMap) {
            formData.AddImageData(data.first, data.second);
        }
    }
    SetFormData(formData);
}

void QjsPaEngine::OnDelete(const int64_t formId)
{
    LOGI("PA: QjsPaEngine OnDelete");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("onDestroy");

    JSValue argFormIdJSValue = JS_NewString(ctx, std::to_string(formId).c_str());

    JSValueConst argv[] = { argFormIdJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("PA: OnDelete FAILED!");
        return;
    }
    return;
}

void QjsPaEngine::OnTriggerEvent(const int64_t formId, const std::string& message)
{
    LOGI("PA: QjsPaEngine OnTriggerEvent");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("onEvent");

    JSValue argFormIdJSValue = JS_NewString(ctx, std::to_string(formId).c_str());
    JSValue argMessageJSValue = JS_NewString(ctx, message.c_str());

    JSValueConst argv[] = { argFormIdJSValue, argMessageJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("PA: OnTriggerEvent FAILED!");
        return;
    }
    return;
}

void QjsPaEngine::OnUpdate(const int64_t formId)
{
    LOGI("PA: QjsPaEngine OnUpdate");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("onUpdate");

    JSValue argFormIdJSValue = JS_NewString(ctx, std::to_string(formId).c_str());

    JSValueConst argv[] = { argFormIdJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("PA: OnUpdate FAILED!");
        return;
    }
    return;
}

void QjsPaEngine::OnCastTemptoNormal(const int64_t formId)
{
    LOGI("PA: QjsPaEngine OnCastTemptoNormal");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("onCastToNormal");

    JSValue argFormIdJSValue = JS_NewString(ctx, std::to_string(formId).c_str());

    JSValueConst argv[] = { argFormIdJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("PA: OnCastTemptoNormal FAILED!");
        return;
    }
    return;
}

void QjsPaEngine::OnVisibilityChanged(const std::map<int64_t, int32_t>& formEventsMap)
{
    LOGI("PA: QjsPaEngine OnVisibilityChanged");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("onVisibilityChange");

    std::string strJsonResult("{");
    for (auto item = formEventsMap.begin(); item != formEventsMap.end(); item++) {
        strJsonResult.append(ToJSONStringInt(std::to_string(item->first), std::to_string(item->second)));
        strJsonResult.append(",");
    }
    strJsonResult = strJsonResult.substr(0, strJsonResult.size() - 1);
    strJsonResult.append("}");

    LOGI("PA: QjsPaEngine strJsonResult = %{public}s", strJsonResult.c_str());
    JSValue argJSValue = Framework::QJSUtils::ParseJSON(ctx, strJsonResult.c_str(), strJsonResult.size(), nullptr);

    JSValueConst argv[] = { argJSValue };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("PA: QjsPaEngine OnVisibilityChanged FAILED!");
        return;
    }
    return;
}

void QjsPaEngine::OnAcquireState(const OHOS::AAFwk::Want& want)
{
    LOGI("PA: QjsPaEngine OnAcquireState");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue paFunc = GetPaFunc("onAcquireState");

    napi_value napiWant = OHOS::AppExecFwk::WrapWant(reinterpret_cast<napi_env>(nativeEngine_), want);
    NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
    JSValue jsWant = (JSValue)*nativeWant;
    JSValueConst argv[] = { jsWant };
    JSValue retVal = Framework::QJSUtils::Call(ctx, paFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("PA: OnAcquireState FAILED!");
        return;
    }
    return;
}

void QjsPaEngine::OnCommand(const OHOS::AAFwk::Want &want, int startId)
{
    LOGI("OnCommand");
    JSContext *ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    Framework::QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue paObj = Framework::QJSUtils::GetPropertyStr(ctx, globalObj, "pa");
    if (!JS_IsObject(paObj)) {
        LOGE("get onCommand function error");
        return;
    }
    JSValue onCommandFunc = Framework::QJSUtils::GetPropertyStr(ctx, paObj, "onCommand");
    if (!JS_IsFunction(ctx, onCommandFunc)) {
        LOGE("onCommand function is not found!");
        JS_FreeValue(ctx, globalObj);
        return;
    }
    napi_value napiWant = OHOS::AppExecFwk::WrapWant(reinterpret_cast<napi_env>(nativeEngine_), want);
    NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
    JSValue jsWant = (JSValue)*nativeWant;
    JSValueConst argv[] = {
        jsWant,
        JS_NewInt32(ctx, startId)
    };
    JSValue retVal = Framework::QJSUtils::Call(ctx, onCommandFunc, JS_UNDEFINED, countof(argv), argv);
    if (JS_IsException(retVal)) {
        LOGE("Qjs onCommand FAILED!");
    }
    JS_FreeValue(ctx, globalObj);
    JS_FreeValue(ctx, retVal);
}

} // namespace OHOS::Ace