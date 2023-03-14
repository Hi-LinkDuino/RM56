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
#include "hilog_wrapper.h"
#include "js_plugin_callback.h"
#include "js_plugin_callback_mgr.h"
#include "js_plugin_util.h"
#include "js_plugin_want.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "core/components/plugin/plugin_component_manager.h"

namespace OHOS::Ace::Napi {
constexpr int ACE_ARGS_MAX_COUNT = 10;
constexpr int ACE_ARGS_TWO = 2;
constexpr int ACE_ARGS_THREE = 3;
constexpr int ACE_PARAM0 = 0;
constexpr int ACE_PARAM1 = 1;
constexpr int ACE_PARAM2 = 2;

bool UnwrapStageWantFromJS(napi_env env, napi_value param, ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    if (!AceUnwrapWant(env, param, asyncCallbackInfo->wantStage)) {
        HILOG_INFO("%{public}s called, Params(want stage) is invalid.", __func__);
        return false;
    }
    return true;
}

bool UnwrapOwnerWantFromJS(napi_env env, napi_value param, ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    napi_value jsValue = AceGetPropertyValueByPropertyName(env, param, "owner", napi_object);
    if (jsValue == nullptr) {
        HILOG_INFO("%{public}s called, Params(want) is not object.", __func__);
        return false;
    }

    if (!AceUnwrapWant(env, jsValue, asyncCallbackInfo->wantStage)) {
        HILOG_INFO("%{public}s called, Params(want) is invalid.", __func__);
        return false;
    }
    return true;
}

bool UnwrapWantFromJS(napi_env env, napi_value param, ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    napi_value jsValue = AceGetPropertyValueByPropertyName(env, param, "want", napi_object);
    if (jsValue == nullptr) {
        HILOG_INFO("%{public}s called, Params(want) is not object.", __func__);
        return false;
    }

    if (!AceUnwrapWant(env, jsValue, asyncCallbackInfo->jsParamList.want)) {
        HILOG_INFO("%{public}s called, Params(want) is invalid.", __func__);
        return false;
    }
    return true;
}

bool UnwrapNameFromJS(napi_env env, napi_value param, const std::string& key,
    ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    napi_value jsValue = AceGetPropertyValueByPropertyName(env, param, key.c_str(), napi_string);
    if (jsValue == nullptr) {
        HILOG_INFO("%{public}s called, Params(%{public}s) is not string.", __func__, key.c_str());
        return false;
    }

    std::string strValue = AceUnwrapStringFromJS(env, jsValue);
    asyncCallbackInfo->jsParamList.paramList.PutStringValue(key, strValue);
    return true;
}

bool UnwrapKVObjectFromJS(napi_env env, napi_value param, const std::string& key,
    ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    napi_value jsValue = AceGetPropertyValueByPropertyName(env, param, key.c_str(), napi_object);
    if (jsValue == nullptr) {
        HILOG_INFO("%{public}s called, Params(%{public}s) is not object.", __func__, key.c_str());
        return false;
    }

    std::string jsonString("{}");
    bool ret = AceKVObjectToString(env, jsValue, jsonString);
    if (ret) {
        asyncCallbackInfo->jsParamList.paramList.PutStringValue(key, jsonString);
    }
    return ret;
}

bool UnwrapPushParameters(napi_env env, napi_value param, ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    if (!AceIsTypeForNapiValue(env, param, napi_object)) {
        HILOG_INFO("%{public}s called, Params is invalid.", __func__);
        return false;
    }

    bool isStageModelAPI = (asyncCallbackInfo->ability == nullptr);
    if (isStageModelAPI) {
        if (!UnwrapOwnerWantFromJS(env, param, asyncCallbackInfo)) {
            HILOG_INFO("%{public}s called, Params(ownerWant) is invalid.", __func__);
            return false;
        }
    }

    if (!UnwrapWantFromJS(env, param, asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, Params(want) is invalid.", __func__);
        return false;
    }
    if (!UnwrapNameFromJS(env, param, "name", asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, Params(name) is invalid.", __func__);
        return false;
    }
    if (!UnwrapKVObjectFromJS(env, param, "data", asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, Params(data) is invalid.", __func__);
        return false;
    }
    if (!UnwrapKVObjectFromJS(env, param, "extraData", asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, Params(data) is invalid.", __func__);
        return false;
    }

    UnwrapNameFromJS(env, param, "jsonPath", asyncCallbackInfo);
    return true;
}

bool UnwrapParamForPush(napi_env env, size_t argc, napi_value* argv,
    ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    HILOG_INFO("%{public}s called, argc=%{public}zu", __func__, argc);
    const size_t argcMax = ACE_ARGS_TWO;
    if (argc != argcMax) {
        HILOG_INFO("%{public}s called, Params is invalid.", __func__);
        return false;
    }

    if (!UnwrapPushParameters(env, argv[ACE_PARAM0], asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, the first parameter is invalid.", __func__);
        return false;
    }

    if (!AceCreateAsyncCallback(env, argv[ACE_PARAM1], asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, the last parameter is invalid.", __func__);
        return false;
    }
    return true;
}

void JSPushWork(napi_env env, void* data)
{
    HILOG_INFO("%{public}s called.", __func__);
}

void JSPushCompleteAsyncCallbackWork(napi_env env, napi_status status, void* data)
{
    HILOG_INFO("%{public}s called.", __func__);
    ACEAsyncJSCallbackInfo* asyncCallbackInfo = (ACEAsyncJSCallbackInfo*)data;
    if (asyncCallbackInfo == nullptr) {
        HILOG_INFO("%{public}s called. asyncCallbackInfo is null", __func__);
        return;
    }
    PluginComponentManager::GetInstance()->Push(asyncCallbackInfo->jsParamList.want,
        asyncCallbackInfo->jsParamList.paramList.GetStringValue("name"),
        asyncCallbackInfo->jsParamList.paramList.GetStringValue("jsonPath"),
        asyncCallbackInfo->jsParamList.paramList.GetStringValue("data"),
        asyncCallbackInfo->jsParamList.paramList.GetStringValue("extraData"));

    asyncCallbackInfo->error_code = 0;
    AceCompleteAsyncCallbackWork(env, status, data);
}

napi_value NAPI_JSPushWrap(napi_env env, napi_callback_info info, ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    HILOG_INFO("%{public}s called.", __func__);
    size_t argc = ACE_ARGS_MAX_COUNT;
    napi_value args[ACE_ARGS_MAX_COUNT] = {nullptr};
    napi_value jsthis = 0;
    void* data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, &data));
    if (!UnwrapParamForPush(env, argc, args, asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called. Invoke UnwrapParamForPush fail", __func__);
        return nullptr;
    }
    ACEAsyncParamEx asyncParamEx;
    if (asyncCallbackInfo->cbInfo.callback != nullptr) {
        HILOG_INFO("%{public}s called. asyncCallback.", __func__);
        asyncParamEx.resource = "NAPI_JSPushCallback";
        asyncParamEx.execute = JSPushWork;
        asyncParamEx.complete = JSPushCompleteAsyncCallbackWork;

        return AceExecuteAsyncCallbackWork(env, asyncCallbackInfo, &asyncParamEx);
    } else {
        HILOG_INFO("%{public}s called. promise.", __func__);
        asyncParamEx.resource = "NAPI_JSPushPromise";
        return nullptr;
    }
}

static napi_value JSPush(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s called.", __func__);

    ACEAsyncJSCallbackInfo* asyncCallbackInfo = AceCreateAsyncJSCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return AceWrapVoidToJS(env);
    }

    napi_value rev = NAPI_JSPushWrap(env, info, asyncCallbackInfo);
    if (rev == nullptr) {
        AceFreeAsyncJSCallbackInfo(&asyncCallbackInfo);
        rev = AceWrapVoidToJS(env);
    }
    return rev;
}

bool UnwrapRequestParameters(napi_env env, napi_value param, ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    if (!AceIsTypeForNapiValue(env, param, napi_object)) {
        HILOG_INFO("%{public}s called, Params is invalid.", __func__);
        return false;
    }

    bool isStageModelAPI = (asyncCallbackInfo->ability == nullptr);
    if (isStageModelAPI) {
        if (!UnwrapOwnerWantFromJS(env, param, asyncCallbackInfo)) {
            HILOG_INFO("%{public}s called, Params(ownerWant) is invalid.", __func__);
            return false;
        }
    }

    if (!UnwrapWantFromJS(env, param, asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, Params(want) is invalid.", __func__);
        return false;
    }
    if (!UnwrapNameFromJS(env, param, "name", asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, Params(name) is invalid.", __func__);
        return false;
    }
    if (!UnwrapKVObjectFromJS(env, param, "data", asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, Params(data) is invalid.", __func__);
        return false;
    }

    UnwrapNameFromJS(env, param, "jsonPath", asyncCallbackInfo);
    return true;
}

bool UnwrapParamForRequest(napi_env env, size_t argc, napi_value* argv,
    ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    HILOG_INFO("%{public}s called, argc=%{public}zu", __func__, argc);
    if (argc != ACE_ARGS_TWO) {
        HILOG_INFO("%{public}s called, Params is invalid.", __func__);
        return false;
    }

    if (!UnwrapRequestParameters(env, argv[ACE_PARAM0], asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, the first parameter is invalid.", __func__);
        return false;
    }

    if (!AceCreateAsyncCallback(env, argv[ACE_PARAM1], asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called, the last parameter is invalid.", __func__);
        return false;
    }
    return true;
}

void JSRequestWork(napi_env env, void* data)
{
    HILOG_INFO("%{public}s called.", __func__);
}

void AceRequestCompleteAsyncCallbackWork(napi_env env, napi_status status, void* data)
{
    HILOG_INFO("%{public}s called.", __func__);

    ACEAsyncJSCallbackInfo* asyncCallbackInfo = (ACEAsyncJSCallbackInfo*)data;
    if (asyncCallbackInfo == nullptr) {
        HILOG_INFO("%{public}s called, asyncCallbackInfo is null", __func__);
        return;
    }

    if (asyncCallbackInfo->ability != nullptr) {
        std::shared_ptr<AAFwk::Want> pWant = asyncCallbackInfo->ability->GetWant();
        asyncCallbackInfo->wantStage = *pWant;
    }

    std::shared_ptr<AceJSPluginRequestParam> param = std::make_shared<AceJSPluginRequestParam>(
        asyncCallbackInfo->jsParamList.want,
        asyncCallbackInfo->jsParamList.paramList.GetStringValue("name"),
        asyncCallbackInfo->jsParamList.paramList.GetStringValue("data"),
        asyncCallbackInfo->jsParamList.paramList.GetStringValue("jsonPath")
    );
    if (param != nullptr) {
        HILOG_INFO("%{public}s called, pWant = %{public}s:%{public}s",
            __func__, asyncCallbackInfo->wantStage.GetElement().GetBundleName().c_str(),
            asyncCallbackInfo->wantStage.GetElement().GetAbilityName().c_str());
        bool ret = JSPluginCallbackMgr::Instance().RegisterRequestEvent(env, asyncCallbackInfo->wantStage,
            asyncCallbackInfo->cbInfo, param);
        if (ret) {
            asyncCallbackInfo->cbInfo.callback = nullptr;
        }
    }

    PluginComponentManager::GetInstance()->Request(asyncCallbackInfo->jsParamList.want,
        asyncCallbackInfo->jsParamList.paramList.GetStringValue("name"),
        asyncCallbackInfo->jsParamList.paramList.GetStringValue("jsonPath"),
        asyncCallbackInfo->jsParamList.paramList.GetStringValue("data"));

    napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
    AceFreeAsyncJSCallbackInfo(&asyncCallbackInfo);
}

napi_value NAPI_JSRequestWrap(napi_env env, napi_callback_info info, ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    HILOG_INFO("%{public}s called.", __func__);
    size_t argc = ACE_ARGS_MAX_COUNT;
    napi_value args[ACE_ARGS_MAX_COUNT] = {nullptr};
    napi_value jsthis = 0;
    void* data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, &data));

    if (!UnwrapParamForRequest(env, argc, args, asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called. Invoke UnwrapParamForRequest fail", __func__);
        return nullptr;
    }

    ACEAsyncParamEx asyncParamEx;
    if (asyncCallbackInfo->cbInfo.callback != nullptr) {
        HILOG_INFO("%{public}s called. asyncCallback.", __func__);
        asyncParamEx.resource = "NAPI_JSRequestCallback";
        asyncParamEx.execute = JSRequestWork;
        asyncParamEx.complete = AceRequestCompleteAsyncCallbackWork;
        return AceExecuteAsyncCallbackWork(env, asyncCallbackInfo, &asyncParamEx);
    } else {
        HILOG_INFO("%{public}s called. promise.", __func__);
        asyncParamEx.resource = "NAPI_JSRequestPromise";
        return nullptr;
    }
}

static napi_value JSRequest(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s called.", __func__);

    ACEAsyncJSCallbackInfo* asyncCallbackInfo = AceCreateAsyncJSCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return AceWrapVoidToJS(env);
    }

    napi_value rev = NAPI_JSRequestWrap(env, info, asyncCallbackInfo);
    if (rev == nullptr) {
        AceFreeAsyncJSCallbackInfo(&asyncCallbackInfo);
        rev = AceWrapVoidToJS(env);
    }
    return rev;
}

bool UnwrapParamForOn(napi_env env, size_t argc, napi_value* argv,
    ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    HILOG_INFO("%{public}s called, argc=%{public}zu", __func__, argc);

    const size_t argcMax = ACE_ARGS_THREE;
    if (argc > argcMax || argc < ACE_ARGS_TWO) {
        HILOG_INFO("%{public}s called, Params is invalid.", __func__);
        return false;
    }

    if (asyncCallbackInfo->ability == nullptr && argc != ACE_ARGS_THREE) {
        HILOG_INFO("%{public}s called, Params is invalid.", __func__);
        return false;
    }

    if (argc == ACE_ARGS_THREE || asyncCallbackInfo->ability == nullptr) {
        if (!UnwrapStageWantFromJS(env, argv[ACE_PARAM0], asyncCallbackInfo)) {
            HILOG_INFO("%{public}s called, the owner want parameter is invalid.", __func__);
            return false;
        }

        std::string eventName = AceUnwrapStringFromJS(env, argv[ACE_PARAM1]);
        if (eventName == "push" || eventName == "request") {
            asyncCallbackInfo->jsParamList.paramList.PutStringValue("eventName", eventName);
        } else {
            HILOG_INFO("%{public}s called, the first parameter is invalid.", __func__);
            return false;
        }

        if (!AceCreateAsyncCallback(env, argv[ACE_PARAM2], asyncCallbackInfo)) {
            HILOG_INFO("%{public}s called, the second parameter is invalid.", __func__);
            return false;
        }
    } else {
        std::string eventName = AceUnwrapStringFromJS(env, argv[ACE_PARAM0]);
        if (eventName == "push" || eventName == "request") {
            asyncCallbackInfo->jsParamList.paramList.PutStringValue("eventName", eventName);
        } else {
            HILOG_INFO("%{public}s called, the first parameter is invalid.", __func__);
            return false;
        }

        if (!AceCreateAsyncCallback(env, argv[ACE_PARAM1], asyncCallbackInfo)) {
            HILOG_INFO("%{public}s called, the second parameter is invalid.", __func__);
            return false;
        }
    }
    return true;
}

void JSOnWork(napi_env env, void* data)
{
    HILOG_INFO("%{public}s called.", __func__);
}

void AceOnCompleteAsyncCallbackWork(napi_env env, napi_status status, void* data)
{
    HILOG_INFO("%{public}s called.", __func__);

    ACEAsyncJSCallbackInfo* asyncCallbackInfo = (ACEAsyncJSCallbackInfo*)data;
    if (asyncCallbackInfo == nullptr) {
        HILOG_INFO("%{public}s called, asyncCallbackInfo is null", __func__);
        return;
    }

    std::string eventName = asyncCallbackInfo->jsParamList.paramList.GetStringValue("eventName");
    CallBackType eventCallbackType = CallBackType::PushEvent;
    if (eventName == "request") {
        eventCallbackType = CallBackType::RequestEvent;
    }

    if (asyncCallbackInfo->ability != nullptr) {
        std::shared_ptr<AAFwk::Want> pWant = asyncCallbackInfo->ability->GetWant();
        asyncCallbackInfo->wantStage = *pWant;
    }

    bool ret = JSPluginCallbackMgr::Instance().RegisterOnEvent(env, eventCallbackType,
        asyncCallbackInfo->wantStage, asyncCallbackInfo->cbInfo);
    if (ret) {
        asyncCallbackInfo->cbInfo.callback = nullptr;
    }
    napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
    AceFreeAsyncJSCallbackInfo(&asyncCallbackInfo);
}

napi_value NAPI_JSOnWrap(napi_env env, napi_callback_info info, ACEAsyncJSCallbackInfo* asyncCallbackInfo)
{
    HILOG_INFO("%{public}s called.", __func__);
    size_t argc = ACE_ARGS_MAX_COUNT;
    napi_value args[ACE_ARGS_MAX_COUNT] = {nullptr};
    napi_value jsthis = 0;
    void* data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, &data));

    if (!UnwrapParamForOn(env, argc, args, asyncCallbackInfo)) {
        HILOG_INFO("%{public}s called. Invoke UnwrapParamForOn fail", __func__);
        return nullptr;
    }

    ACEAsyncParamEx asyncParamEx;
    if (asyncCallbackInfo->cbInfo.callback != nullptr) {
        HILOG_INFO("%{public}s called. asyncCallback.", __func__);
        asyncParamEx.resource = "NAPI_JSOCallback";
        asyncParamEx.execute = JSOnWork;
        asyncParamEx.complete = AceOnCompleteAsyncCallbackWork;

        return AceExecuteAsyncCallbackWork(env, asyncCallbackInfo, &asyncParamEx);
    } else {
        HILOG_INFO("%{public}s called. promise.", __func__);
        asyncParamEx.resource = "NAPI_JSOnPromise";
        return nullptr;
    }
}

static napi_value JSOn(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s called.", __func__);

    ACEAsyncJSCallbackInfo* asyncCallbackInfo = AceCreateAsyncJSCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return AceWrapVoidToJS(env);
    }

    napi_value rev = NAPI_JSOnWrap(env, info, asyncCallbackInfo);
    if (rev == nullptr) {
        AceFreeAsyncJSCallbackInfo(&asyncCallbackInfo);
        rev = AceWrapVoidToJS(env);
    }
    return rev;
}

static napi_value PluginComponentExport(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("push", JSPush),
        DECLARE_NAPI_FUNCTION("request", JSRequest),
        DECLARE_NAPI_FUNCTION("on", JSOn),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
}

static napi_module PlugInComonentModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = PluginComponentExport,
    .nm_modname = "plugincomponent",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterPluginComponent()
{
    napi_module_register(&PlugInComonentModule);
}
}  // namespace OHOS::Ace::Napi
