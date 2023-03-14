/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "distributed_bundle_mgr.h"

#include <string>

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "distributed_bms_interface.h"
#include "distributed_bms_proxy.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "securec.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
constexpr size_t ARGS_SIZE_ONE = 1;
constexpr size_t ARGS_SIZE_THREE = 3;
constexpr int32_t PARAM0 = 0;
constexpr int32_t PARAM1 = 1;
constexpr int32_t PARAM2 = 2;
constexpr int32_t NAPI_RETURN_ZERO = 0;
constexpr int32_t NAPI_RETURN_ONE = 1;
constexpr int32_t GET_REMOTE_ABILITY_INFO_MAX_SIZE = 10;
enum GetRemoteAbilityInfoErrorCode : int32_t {
    SUCCESS = 0,
    ERR_INNER_ERROR,
    ERR_INVALID_PARAM,
    ERR_PARAMETERS_MORE_THAN_MAX,
    ERR_RPC_ERROR,
};
}

AsyncWorkData::AsyncWorkData(napi_env napiEnv)
{
    env = napiEnv;
}

AsyncWorkData::~AsyncWorkData()
{
    if (callbackRef) {
        APP_LOGD("AsyncWorkData::~AsyncWorkData delete callbackRef");
        napi_delete_reference(env, callbackRef);
        callbackRef = nullptr;
    }
    if (asyncWork) {
        APP_LOGD("AsyncWorkData::~AsyncWorkData delete asyncWork");
        napi_delete_async_work(env, asyncWork);
        asyncWork = nullptr;
    }
}

static OHOS::sptr<OHOS::AppExecFwk::IDistributedBms> GetDistributedBundleMgr()
{
    APP_LOGI("GetDistributedBundleMgr");
    auto samgr = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    // OHOS::sptr<OHOS::IRemoteObject> remoteObject;
    auto remoteObject = samgr->GetSystemAbility(OHOS::DISTRIBUTED_BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    return OHOS::iface_cast<IDistributedBms>(remoteObject);
}

static std::string GetStringFromNAPI(napi_env env, napi_value value)
{
    std::string result;
    size_t size = 0;

    if (napi_get_value_string_utf8(env, value, nullptr, NAPI_RETURN_ZERO, &size) != napi_ok) {
        APP_LOGE("can not get string size");
        return "";
    }
    result.reserve(size + NAPI_RETURN_ONE);
    result.resize(size);
    if (napi_get_value_string_utf8(env, value, result.data(), (size + NAPI_RETURN_ONE), &size) != napi_ok) {
        APP_LOGE("can not get string value");
        return "";
    }
    return result;
}

static napi_value ParseString(napi_env env, std::string &param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, args, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    param = GetStringFromNAPI(env, args);
    APP_LOGD("param=%{public}s.", param.c_str());
    napi_value result;
    status = napi_create_int32(env, NAPI_RETURN_ONE, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}

static int32_t ConvertResultCode(int32_t code)
{
    APP_LOGD("ConvertResultCode resultCode:%{public}d", code);
    switch (code) {
        case SUCCESS:
            return SUCCESS;
        case ERR_APPEXECFWK_FAILED_GET_REMOTE_PROXY:
            return ERR_RPC_ERROR;
        default:
            break;
    }
    return ERR_INNER_ERROR;
}

static void ConvertElementName(napi_env env, napi_value objElementName, const ElementName &elementName)
{
    napi_value nDeviceId;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetDeviceID().c_str(), NAPI_AUTO_LENGTH, &nDeviceId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objElementName, "deviceId", nDeviceId));

    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetBundleName().c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objElementName, "bundleName", nBundleName));

    napi_value nAbilityName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetAbilityName().c_str(), NAPI_AUTO_LENGTH, &nAbilityName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objElementName, "abilityName", nAbilityName));
}

static void ConvertRemoteAbilityInfo(
    napi_env env, napi_value objRemoteAbilityInfo, const RemoteAbilityInfo &remoteAbilityInfo)
{
    napi_value objElementName;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objElementName));
    ConvertElementName(env, objElementName, remoteAbilityInfo.elementName);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRemoteAbilityInfo, "elementName", objElementName));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, remoteAbilityInfo.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRemoteAbilityInfo, "label", nLabel));

    napi_value nIcon;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, remoteAbilityInfo.icon.c_str(), NAPI_AUTO_LENGTH, &nIcon));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRemoteAbilityInfo, "icon", nIcon));
}

static void ConvertRemoteAbilityInfos(
    napi_env env, napi_value objRemoteAbilityInfos, const std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    if (remoteAbilityInfos.size() == 0) {
        APP_LOGE("ConvertRemoteAbilityInfos remoteAbilityInfos is empty");
        return;
    }
    size_t index = 0;
    for (const auto &remoteAbilityInfo : remoteAbilityInfos) {
        APP_LOGD("remoteAbilityInfo bundleName:%{public}s, abilityName:%{public}s, label:%{public}s",
                 remoteAbilityInfo.elementName.GetBundleName().c_str(),
                 remoteAbilityInfo.elementName.GetAbilityName().c_str(),
                 remoteAbilityInfo.label.c_str());
        napi_value objRemoteAbilityInfo = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objRemoteAbilityInfo));
        ConvertRemoteAbilityInfo(env, objRemoteAbilityInfo, remoteAbilityInfo);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, objRemoteAbilityInfos, index, objRemoteAbilityInfo));
        index++;
    }
}

static bool ParseElementName(napi_env env, OHOS::AppExecFwk::ElementName &elementName, napi_value args)
{
    APP_LOGD("begin to parse ElementName");
    napi_status status;
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    if (valueType != napi_object) {
        APP_LOGE("args not object type");
        return false;
    }
    napi_value prop = nullptr;
    status = napi_get_named_property(env, args, "deviceId", &prop);
    napi_typeof(env, prop, &valueType);
    if (status == napi_ok && valueType == napi_string) {
        elementName.SetDeviceID(GetStringFromNAPI(env, prop));
    } else {
        APP_LOGE("begin to parse ElementName deviceId failed");
        return false;
    }
    prop = nullptr;
    status = napi_get_named_property(env, args, "bundleName", &prop);
    napi_typeof(env, prop, &valueType);
    if (status == napi_ok && valueType == napi_string) {
        elementName.SetBundleName(GetStringFromNAPI(env, prop));
    } else {
        APP_LOGE("begin to parse ElementName bundleName failed");
        return false;
    }
    prop = nullptr;
    status = napi_get_named_property(env, args, "moduleName", &prop);
    napi_typeof(env, prop, &valueType);
    if (status == napi_ok && valueType == napi_string) {
        std::string moduleName = GetStringFromNAPI(env, prop);
        if (moduleName.empty()) {
            APP_LOGE("err ElementName moduleName is empty");
            return false;
        }
        elementName.SetModuleName(moduleName);
    }
    prop = nullptr;
    status = napi_get_named_property(env, args, "abilityName", &prop);
    napi_typeof(env, prop, &valueType);
    if (status == napi_ok && valueType == napi_string) {
        elementName.SetAbilityName(GetStringFromNAPI(env, prop));
    } else {
        APP_LOGE("begin to parse ElementName abilityName failed");
        return false;
    }
    APP_LOGD("parse ElementName end");
    return true;
}

static bool ParseElementNames(napi_env env, std::vector<ElementName> &elementNames, napi_value args)
{
    APP_LOGD("begin to parse ElementNames");
    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, args, &isArray));
    if (!isArray) {
        APP_LOGE("parseElementNames args not array");
        return false;
    }
    uint32_t arrayLength = 0;
    NAPI_CALL(env, napi_get_array_length(env, args, &arrayLength));
    APP_LOGD("arrayLength:%{public}d", arrayLength);
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value value = nullptr;
        NAPI_CALL(env, napi_get_element(env, args, i, &value));
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, value, &valueType));
        if (valueType != napi_object) {
            APP_LOGE("array inside not object type");
            elementNames.clear();
            return false;
        }
        ElementName elementName;
        if (ParseElementName(env, elementName, value)) {
            elementNames.push_back(elementName);
        } else {
            APP_LOGE("elementNames parse elementName failed");
            return false;
        }
    }
    return true;
}

static int32_t InnerGetRemoteAbilityInfo(
    const OHOS::AppExecFwk::ElementName &elementName, const std::string &locale, RemoteAbilityInfo &remoteAbilityInfo)
{
    auto iDistBundleMgr = GetDistributedBundleMgr();
    if (!iDistBundleMgr) {
        APP_LOGE("can not get iDistBundleMgr");
        return ERR_INNER_ERROR;
    }
    int32_t result = iDistBundleMgr->GetRemoteAbilityInfo(elementName, locale, remoteAbilityInfo);
    if (result != 0) {
        APP_LOGE("InnerGetRemoteAbilityInfo failed");
    }
    return ConvertResultCode(result);
}

static int32_t InnerGetRemoteAbilityInfos(const std::vector<ElementName> &elementNames, const std::string locale,
    std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    if (elementNames.size() == 0) {
        APP_LOGE("InnerGetRemoteAbilityInfos elementNames is empty");
        return ERR_INVALID_PARAM;
    }
    auto iDistBundleMgr = GetDistributedBundleMgr();
    if (!iDistBundleMgr) {
        APP_LOGE("can not get iDistBundleMgr");
        return ERR_INNER_ERROR;
    }
    int32_t result = iDistBundleMgr->GetRemoteAbilityInfos(elementNames, locale, remoteAbilityInfos);
    if (result != 0) {
        APP_LOGE("InnerGetRemoteAbilityInfo failed");
    }
    return ConvertResultCode(result);
}

napi_value GetRemoteAbilityInfo(napi_env env, napi_callback_info info)
{
    size_t requireArgc = ARGS_SIZE_ONE;
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = { 0 };
    napi_value thisArg = nullptr;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 1 parameter");

    ElementNameInfo *asyncCallbackInfo = new (std::nothrow) ElementNameInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<ElementNameInfo> callbackPtr {asyncCallbackInfo};
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valueType));
        if ((i == PARAM0) && (valueType == napi_object)) {
            if (!ParseElementName(env, asyncCallbackInfo->elementName, argv[i])) {
                asyncCallbackInfo->errCode = ERR_INVALID_PARAM;
            }
        } else if ((i == PARAM1) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->locale, argv[i]);
        } else if ((i == PARAM1 || i == PARAM2) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callbackRef));
            break;
        } else {
            asyncCallbackInfo->errCode = ERR_INVALID_PARAM;
            asyncCallbackInfo->errMssage = "type misMatch";
        }
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callbackRef == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "getRemoteAbilityInfo", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            ElementNameInfo* asyncCallbackInfo = (ElementNameInfo*)data;
            if (!asyncCallbackInfo->errCode) {
                asyncCallbackInfo->errCode =
                    InnerGetRemoteAbilityInfo(asyncCallbackInfo->elementName,
                                              asyncCallbackInfo->locale,
                                              asyncCallbackInfo->remoteAbilityInfo);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            ElementNameInfo* asyncCallbackInfo = (ElementNameInfo*)data;
            std::unique_ptr<ElementNameInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->errCode) {
                napi_create_int32(env, asyncCallbackInfo->errCode, &result[0]);
            } else {
                napi_create_uint32(env, 0, &result[0]);
                NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[1]));
                ConvertRemoteAbilityInfo(env, result[1], asyncCallbackInfo->remoteAbilityInfo);
            }
            if (asyncCallbackInfo->callbackRef) {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callbackRef, &callback));
                napi_call_function(env, nullptr, callback, sizeof(result) / sizeof(result[0]), result, &placeHolder);
            } else {
                if (asyncCallbackInfo->errCode) {
                    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
                }
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

napi_value GetRemoteAbilityInfos(napi_env env, napi_callback_info info)
{
    size_t requireArgc = ARGS_SIZE_ONE;
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = { 0 };
    napi_value thisArg = nullptr;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 1 parameter");

    ElementNameInfos *asyncCallbackInfo = new (std::nothrow) ElementNameInfos(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<ElementNameInfos> callbackPtr {asyncCallbackInfo};
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if (i == PARAM0) {
            if (!ParseElementNames(env, asyncCallbackInfo->elementNames, argv[i])) {
                asyncCallbackInfo->errCode = ERR_INVALID_PARAM;
            }
            if (static_cast<int32_t>(asyncCallbackInfo->elementNames.size()) > GET_REMOTE_ABILITY_INFO_MAX_SIZE) {
                APP_LOGE("InnerGetRemoteAbilityInfos elementNames more than max");
                asyncCallbackInfo->errCode = ERR_PARAMETERS_MORE_THAN_MAX;
            }
        } else if ((i == PARAM1) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->locale, argv[i]);
        } else if ((i == PARAM1 || i == PARAM2) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callbackRef));
            break;
        } else {
            asyncCallbackInfo->errCode = ERR_INVALID_PARAM;
            asyncCallbackInfo->errMssage = "type misMatch";
        }
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callbackRef == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "getRemoteAbilityInfos", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            ElementNameInfos* asyncCallbackInfo = (ElementNameInfos*)data;
            if (!asyncCallbackInfo->errCode) {
                asyncCallbackInfo->errCode =
                    InnerGetRemoteAbilityInfos(asyncCallbackInfo->elementNames,
                                               asyncCallbackInfo->locale,
                                               asyncCallbackInfo->remoteAbilityInfos);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            ElementNameInfos* asyncCallbackInfo = (ElementNameInfos*)data;
            std::unique_ptr<ElementNameInfos> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->errCode) {
                napi_create_int32(env, asyncCallbackInfo->errCode, &result[0]);
            } else {
                napi_create_uint32(env, 0, &result[0]);
                NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result[1]));
                ConvertRemoteAbilityInfos(env, result[1], asyncCallbackInfo->remoteAbilityInfos);
            }
            if (asyncCallbackInfo->callbackRef) {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callbackRef, &callback));
                napi_call_function(env, nullptr, callback, sizeof(result) / sizeof(result[0]), result, &placeHolder);
            } else {
                if (asyncCallbackInfo->errCode) {
                    napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]);
                } else {
                    napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]);
                }
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}
}
}