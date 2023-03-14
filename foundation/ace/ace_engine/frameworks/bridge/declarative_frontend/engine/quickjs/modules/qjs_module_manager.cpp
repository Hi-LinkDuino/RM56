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

#include "frameworks/bridge/declarative_frontend/engine/quickjs/modules/qjs_module_manager.h"

#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/modules/qjs_curves_module.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/modules/qjs_matrix4_module.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/modules/qjs_router_module.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace OHOS::Ace::Framework {
namespace {
constexpr int32_t CUSTOM_FULL_WINDOW_LENGTH = 3;
constexpr int32_t ARGS_FULL_WINDOW_LENGTH = 2;
} // namespace

ModuleManager* ModuleManager::GetInstance()
{
    static ModuleManager instance;
    return &instance;
}

JSValue AppGetInfo(JSContext* ctx, JSValue value, int32_t argc, JSValueConst* argv)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    auto delegate = instance->GetDelegate();
    if (!delegate) {
        LOGE("get frontend delegate failed");
        return JS_NULL;
    }
    JSValue appInfo = JS_NewObject(ctx);
    auto appId = delegate->GetAppID();
    auto appName = delegate->GetAppName();
    auto versionName = delegate->GetVersionName();
    auto versionCode = delegate->GetVersionCode();
    JS_SetPropertyStr(ctx, appInfo, "appID", JS_NewString(ctx, appId.c_str()));
    JS_SetPropertyStr(ctx, appInfo, "appName", JS_NewString(ctx, appName.c_str()));
    JS_SetPropertyStr(ctx, appInfo, "versionName", JS_NewString(ctx, versionName.c_str()));
    JS_SetPropertyStr(ctx, appInfo, "versionCode", JS_NewInt32(ctx, versionCode));
    return appInfo;
}

JSValue AppTerminate(JSContext* ctx, JSValue value, int32_t argc, JSValueConst* argv)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    auto delegate = instance->GetDelegate();
    if (!delegate) {
        LOGE("get frontend delegate failed");
        return JS_NULL;
    }
    auto pipelineContext = instance->GetPipelineContext(ctx);
    if (!pipelineContext) {
        LOGE("get frontend pipelineContext failed");
        return JS_NULL;
    }
    auto uiTaskExecutor = delegate->GetUiTask();
    WeakPtr<PipelineContext> pipelineContextWeak(pipelineContext);
    uiTaskExecutor.PostTask([pipelineContextWeak]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->Finish();
        }
    });
    return JS_NULL;
}

void ParseFullWindowParams(JSContext* ctx, JSValue params, std::string& duration)
{
    JSPropertyEnum* tab = nullptr;
    uint32_t paramLen = 0;
    if (JS_IsObject(params)) {
        JS_GetOwnPropertyNames(ctx, &tab, &paramLen, params, JS_GPN_STRING_MASK);
        const char* jsDurationKey = JS_AtomToCString(ctx, tab[0].atom);
        if (jsDurationKey == nullptr) {
            JS_FreeAtom(ctx, tab[0].atom);
            js_free(ctx, tab);
            LOGE("jsDurationKey is null.");
            return;
        }
        if (std::strcmp(jsDurationKey, "duration") == 0) {
            JSValue valObject = JS_GetProperty(ctx, params, tab[0].atom);
            if (JS_IsString(valObject) || JS_IsNumber(valObject)) {
                ScopedString styleVal(ctx, valObject);
                const char* valDuration = styleVal.get();
                duration = valDuration;
            }
            JS_FreeValue(ctx, valObject);
        }
        JS_FreeAtom(ctx, tab[0].atom);
        JS_FreeCString(ctx, jsDurationKey);
    }
    js_free(ctx, tab);
}

JSValue AppRequestFullWindow(JSContext* ctx, JSValue value, int32_t argc, JSValueConst* argv)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    int32_t duration = -1;
    if (JS_IsObject(value) && JS_IsArray(ctx, value)) {
        JS_GetOwnPropertyNames(ctx, &pTab, &len, value, JS_GPN_STRING_MASK);
        if (len < ARGS_FULL_WINDOW_LENGTH) {
            LOGW("RequestFullWindow: invalid callback value");
            js_free(ctx, pTab);
            return JS_NULL;
        }
        if (len == CUSTOM_FULL_WINDOW_LENGTH) {
            JSValue jsDuration = JS_GetProperty(ctx, value, pTab[0].atom);
            std::string valDuration;
            ParseFullWindowParams(ctx, jsDuration, valDuration);
            if (!valDuration.empty()) {
                duration = StringToInt(valDuration);
            }
            if (duration < 0) {
                duration = -1;
            }
            JS_FreeValue(ctx, jsDuration);
        }
        js_free(ctx, pTab);
    }
    auto instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    WeakPtr<PipelineContext> pipelineContextWeak = instance->GetDelegate()->GetPipelineContext();
    auto uiTaskExecutor = instance->GetDelegate()->GetUiTask();
    uiTaskExecutor.PostTask([pipelineContextWeak, duration]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->RequestFullWindow(duration);
        }
    });
    return JS_NULL;
}

JSValue AppSetImageCacheCount(JSContext* ctx, JSValue value, int32_t argc, JSValueConst* argv)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    auto delegate = instance->GetDelegate();
    if (!delegate) {
        LOGE("get frontend delegate failed");
        return JS_NULL;
    }
    auto pipelineContext = instance->GetPipelineContext(ctx);
    if (!pipelineContext) {
        LOGE("get frontend pipelineContext failed");
        return JS_NULL;
    }
    auto uiTaskExecutor = delegate->GetUiTask();
    WeakPtr<PipelineContext> pipelineContextWeak(pipelineContext);
    int32_t size;
    JS_ToInt32(ctx, &size, argv[0]);
    size = size > 0 ? size : 0;
    uiTaskExecutor.PostTask([pipelineContextWeak, size]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            auto imageCache = pipelineContext->GetImageCache();
            if (imageCache) {
                imageCache->SetCapacity(size);
            } else {
                LOGW("image cache is null");
            }
        }
    });
    return JS_NULL;
}

JSValue AppSetImageRawDataCacheSize(JSContext* ctx, JSValue value, int32_t argc, JSValueConst* argv)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    auto delegate = instance->GetDelegate();
    if (!delegate) {
        LOGE("get frontend delegate failed");
        return JS_NULL;
    }
    auto pipelineContext = instance->GetPipelineContext(ctx);
    if (!pipelineContext) {
        LOGE("get frontend pipelineContext failed");
        return JS_NULL;
    }
    auto uiTaskExecutor = delegate->GetUiTask();
    WeakPtr<PipelineContext> pipelineContextWeak(pipelineContext);
    int32_t size;
    JS_ToInt32(ctx, &size, argv[0]);
    size = size > 0 ? size : 0;
    uiTaskExecutor.PostTask([pipelineContextWeak, size]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            auto imageCache = pipelineContext->GetImageCache();
            if (imageCache) {
                imageCache->SetCapacity(size);
            } else {
                LOGW("image cache is null");
            }
        }
    });
    return JS_NULL;
}

JSValue AppSetImageFileCacheSize(JSContext* ctx, JSValue value, int32_t argc, JSValueConst* argv)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    auto delegate = instance->GetDelegate();
    if (!delegate) {
        LOGE("get frontend delegate failed");
        return JS_NULL;
    }
    auto pipelineContext = instance->GetPipelineContext(ctx);
    if (!pipelineContext) {
        LOGE("get frontend pipelineContext failed");
        return JS_NULL;
    }
    auto uiTaskExecutor = delegate->GetUiTask();
    WeakPtr<PipelineContext> pipelineContextWeak(pipelineContext);
    int32_t size;
    JS_ToInt32(ctx, &size, argv[0]);
    size = size > 0 ? size : 0;
    uiTaskExecutor.PostTask([pipelineContextWeak, size]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            auto imageCache = pipelineContext->GetImageCache();
            if (imageCache) {
                imageCache->SetCacheFileLimit(size);
            } else {
                LOGW("image cache is null");
            }
        }
    });
    return JS_NULL;
}

void InitAppModule(JSContext* ctx, JSValue& moduleObj)
{
    JS_SetPropertyStr(ctx, moduleObj, APP_GET_INFO, JS_NewCFunction(ctx, AppGetInfo, APP_GET_INFO, 0));
    JS_SetPropertyStr(ctx, moduleObj, APP_TERMINATE, JS_NewCFunction(ctx, AppTerminate, APP_TERMINATE, 0));
    JS_SetPropertyStr(ctx, moduleObj, APP_REQUEST_FULL_WINDOW,
        JS_NewCFunction(ctx, AppRequestFullWindow, APP_REQUEST_FULL_WINDOW, 1));
    JS_SetPropertyStr(ctx, moduleObj, APP_SET_IMAGE_CACHE_COUNT,
        JS_NewCFunction(ctx, AppSetImageCacheCount, APP_SET_IMAGE_CACHE_COUNT, 1));
    JS_SetPropertyStr(ctx, moduleObj, APP_SET_IMAGE_RAWDATA_CACHE_SIZE,
        JS_NewCFunction(ctx, AppSetImageRawDataCacheSize, APP_SET_IMAGE_RAWDATA_CACHE_SIZE, 1));
    JS_SetPropertyStr(ctx, moduleObj, APP_SET_IMAGE_FILE_CACHE_SIZE,
        JS_NewCFunction(ctx, AppSetImageFileCacheSize, APP_SET_IMAGE_FILE_CACHE_SIZE, 1));
}

bool ModuleManager::InitModule(JSContext* ctx, const std::string& moduleName, JSValue& jsObject)
{
    static const std::unordered_map<std::string, void (*)(JSContext * ctx, JSValue & jsObject)> MODULE_LIST = {
        { "system.router", [](JSContext* ctx, JSValue& jsObject) { InitRouterModule(ctx, jsObject); } },
        { "ohos.router", [](JSContext* ctx, JSValue& jsObject) { InitRouterModule(ctx, jsObject); } },
        { "system.curves", [](JSContext* ctx, JSValue& jsObject) { InitCurvesModule(ctx, jsObject); } },
        { "ohos.curves", [](JSContext* ctx, JSValue& jsObject) { InitCurvesModule(ctx, jsObject); } },
        { "system.matrix4", [](JSContext* ctx, JSValue& jsObject) { InitMatrix4Module(ctx, jsObject); } },
        { "ohos.matrix4", [](JSContext* ctx, JSValue& jsObject) { InitMatrix4Module(ctx, jsObject); } },
        { "system.app", [](JSContext* ctx, JSValue& jsObject) { InitAppModule(ctx, jsObject); } },
        { "ohos.app", [](JSContext* ctx, JSValue& jsObject) { InitAppModule(ctx, jsObject); } },
    };
    auto iter = MODULE_LIST.find(moduleName);
    if (iter != MODULE_LIST.end()) {
        iter->second(ctx, jsObject);
        LOGD("ModuleManager InitModule is %{private}s", moduleName.c_str());
        return true;
    } else {
        LOGE("register module is not found");
        return false;
    }
}

JSValue SetTimeout(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return ModuleManager::GetInstance()->SetWaitTimer(ctx, argc, argv, false);
}

JSValue SetInterval(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return ModuleManager::GetInstance()->SetWaitTimer(ctx, argc, argv, true);
}

JSValue ClearTimeout(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return ModuleManager::GetInstance()->ClearWaitTimer(ctx, argc, argv, false);
}

JSValue ClearInterval(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return ModuleManager::GetInstance()->ClearWaitTimer(ctx, argc, argv, true);
}

JSValue CanIUse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanIUse argc is %d", argc);

    if (argc != 1) {
        LOGE("CanIUse: invalid value");
        return JS_NULL;
    }

    QJSContext::Scope scp(ctx);
    if (!JS_IsString(argv[0])) {
        LOGE("CanIUse: invalid input");
        return JS_NULL;
    }

    ScopedString targetString(ctx, argv[0]);
    std::string syscapString = targetString.get();

    bool ret = Ace::SystemProperties::IsSyscapExist(syscapString.c_str());

    return JS_NewBool(ctx, ret);
}

JSValue ModuleManager::SetWaitTimer(JSContext* ctx, int32_t argc, JSValueConst* argv, bool isInterval)
{
    LOGD("SetWaitTimer argc is %d", argc);

    if (argc < 1) {
        LOGE("JsSetTimer: invalid callback value");
        return JS_NULL;
    }

    JSValue jsFunc = JS_DupValue(ctx, argv[0]);
    if (!JS_IsFunction(ctx, jsFunc)) {
        LOGE("argv[0] is not function");
        return JS_NULL;
    }

    int index = 1;
    uint32_t delay = 0;
    if (JS_IsNumber(argv[1])) {
        JS_ToUint32(ctx, &delay, argv[1]);
        index = 2;
    }

    std::vector<JSValue> callbackArray;
    while (index < argc) {
        callbackArray.emplace_back(JS_DupValue(ctx, argv[index]));
        ++index;
    }

    uint32_t callbackId = ModuleManager::GetInstance()->AddCallback(jsFunc, callbackArray, isInterval);

    auto instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    instance->GetDelegate()->WaitTimer(std::to_string(callbackId), std::to_string(delay), isInterval, true);

    return JS_NewInt32(ctx, callbackId);
}

uint32_t ModuleManager::AddCallback(JSValue callbackFunc, std::vector<JSValue> callbackArray, bool isInterval)
{
    if (isInterval) {
        return AddCallback(intervalCallbackFuncMap_, intervalCallbackArrayMap_, callbackFunc, callbackArray);
    } else {
        return AddCallback(callbackFuncMap_, callbackArrayMap_, callbackFunc, callbackArray);
    }
}

uint32_t ModuleManager::AddCallback(std::unordered_map<uint32_t, JSValue>& callbackFuncMap,
    std::unordered_map<uint32_t, std::vector<JSValue>>& callbackArrayMap, JSValue callbackFunc,
    std::vector<JSValue> callbackArray)
{
    ++callbackId_;
    callbackFuncMap[callbackId_] = callbackFunc;
    if (!callbackArray.empty()) {
        callbackArrayMap[callbackId_] = callbackArray;
    }
    return callbackId_;
}

JSValue ModuleManager::GetCallbackFunc(uint32_t callbackId, bool isInterval)
{
    if (isInterval) {
        return intervalCallbackFuncMap_[callbackId];
    } else {
        return callbackFuncMap_[callbackId];
    }
}

std::vector<JSValue> ModuleManager::GetCallbackArray(uint32_t callbackId, bool isInterval)
{
    if (isInterval) {
        return GetCallbackArray(intervalCallbackArrayMap_, callbackId);
    } else {
        return GetCallbackArray(callbackArrayMap_, callbackId);
    }
}

std::vector<JSValue> ModuleManager::GetCallbackArray(
    std::unordered_map<uint32_t, std::vector<JSValue>>& callbackArrayMap, uint32_t callbackId)
{
    if (callbackArrayMap.find(callbackId) != callbackArrayMap.end()) {
        return callbackArrayMap[callbackId];
    }
    std::vector<JSValue> emptyRet;
    return emptyRet;
}

JSValue ModuleManager::ClearWaitTimer(JSContext* ctx, int32_t argc, JSValueConst* argv, bool isInterval)
{
    if (argc < 0 || !JS_IsNumber(argv[0])) {
        LOGE("args[0] is not number");
        return JS_NULL;
    }

    uint32_t callbackId;
    JS_ToUint32(ctx, &callbackId, argv[0]);
    ModuleManager::GetInstance()->RemoveCallbackFunc(ctx, callbackId, isInterval);
    auto instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    instance->GetDelegate()->ClearTimer(std::to_string(callbackId));
    return JS_NULL;
}

void ModuleManager::RemoveCallbackFunc(JSContext* ctx, uint32_t callbackId, bool isInterval)
{
    if (isInterval) {
        RemoveCallbackFunc(ctx, intervalCallbackFuncMap_, intervalCallbackArrayMap_, callbackId);
    } else {
        RemoveCallbackFunc(ctx, callbackFuncMap_, callbackArrayMap_, callbackId);
    }
}

void ModuleManager::RemoveCallbackFunc(JSContext* ctx, std::unordered_map<uint32_t, JSValue>& callbackFuncMap,
    std::unordered_map<uint32_t, std::vector<JSValue>>& callbackArrayMap, uint32_t callbackId)
{
    JS_FreeValue(ctx, callbackFuncMap[callbackId]);
    callbackFuncMap.erase(callbackId);

    if (callbackArrayMap.find(callbackId) != callbackArrayMap.end()) {
        uint32_t index = 0;
        while (index < callbackArrayMap[callbackId].size()) {
            JS_FreeValue(ctx, callbackArrayMap[callbackId][index]);
            ++index;
        }
        callbackArrayMap.erase(callbackId);
    }
}

void ModuleManager::InitTimerModule(JSContext* ctx)
{
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, globalObj, SET_TIMEOUT, JS_NewCFunction(ctx, SetTimeout, SET_TIMEOUT, 2));
    JS_SetPropertyStr(ctx, globalObj, SET_INTERVAL, JS_NewCFunction(ctx, SetInterval, SET_INTERVAL, 2));
    JS_SetPropertyStr(ctx, globalObj, CLEAR_TIMEOUT, JS_NewCFunction(ctx, ClearTimeout, CLEAR_TIMEOUT, 1));
    JS_SetPropertyStr(ctx, globalObj, CLEAR_INTERVAL, JS_NewCFunction(ctx, ClearInterval, CLEAR_INTERVAL, 1));
    JS_FreeValue(ctx, globalObj);
}

void ModuleManager::InitSyscapModule(JSContext* ctx)
{
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, globalObj, CAN_IUSE, JS_NewCFunction(ctx, CanIUse, CAN_IUSE, 1));
    JS_FreeValue(ctx, globalObj);
}

} // namespace OHOS::Ace::Framework
