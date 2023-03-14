/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "bluetooth_opp.h"
#include "bluetooth_log.h"
#include "napi_bluetooth_opp_observer.h"
#include "napi_bluetooth_utils.h"
#include "napi_bluetooth_profile.h"
#include "napi_bluetooth_opp.h"

namespace OHOS {
namespace Bluetooth {
using namespace std;
NapiBluetoothOppObserver NapiBluetoothOpp::observer_;
bool NapiBluetoothOpp::isRegistered_ = false;

void NapiBluetoothOpp::DefineOppJSClass(napi_env env)
{
    HILOGI("NapiBluetoothOpp::DefineOppJSClass called");
    napi_value constructor;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("on", NapiBluetoothOpp::On),
        DECLARE_NAPI_FUNCTION("off", NapiBluetoothOpp::Off),
        DECLARE_NAPI_FUNCTION("sendFile", NapiBluetoothOpp::SendFile),
        DECLARE_NAPI_FUNCTION("setIncomingFileConfirmation", NapiBluetoothOpp::SetIncomingFileConfirmation),
        DECLARE_NAPI_FUNCTION("getCurrentTransferInformation", NapiBluetoothOpp::GetCurrentTransferInformation),
        DECLARE_NAPI_FUNCTION("cancelTransfer", NapiBluetoothOpp::CancelTransfer),
        DECLARE_NAPI_FUNCTION("getConnectionDevices", GetConnectionDevices),
        DECLARE_NAPI_FUNCTION("getDeviceState", GetDeviceState),
    };

    napi_define_class(env, "NapiBluetoothOpp", NAPI_AUTO_LENGTH, NapiBluetoothOpp::OppConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);

    napi_value napiProfile;
    napi_new_instance(env, constructor, 0, nullptr, &napiProfile);
    NapiProfile::SetProfile(ProfileId::PROFILE_OPP, napiProfile);
    Opp *profile = Opp::GetProfile();
    profile->RegisterObserver(&observer_);
    HILOGI("DefineOppJSClass finished");
}

napi_value NapiBluetoothOpp::OppConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

napi_value NapiBluetoothOpp::On(napi_env env, napi_callback_info info)
{
    HILOGI("On called");
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 2 argument.");
        return ret;
    }
    string type;
    if (!ParseString(env, type, argv[PARAM0])) {
        HILOGE("string expected.");
        return ret;
    }
    std::shared_ptr<TransforInformationCallbackInfo> callbackInfo = std::make_shared<TransforInformationCallbackInfo>();
    callbackInfo->env_ = env;

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[PARAM1], &valueType);
    if (valueType != napi_function) {
        HILOGE("Wrong argument type. Function expected.");
        return ret;
    }
    napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
    observer_.callbackInfos_[type] = callbackInfo;
    HILOGI("%{public}s is registered", type.c_str());
    return ret;
}

napi_value NapiBluetoothOpp::Off(napi_env env, napi_callback_info info)
{
    HILOGI("Off called");
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }
    string type;
    if (!ParseString(env, type, argv[PARAM0])) {
        HILOGE("string expected.");
        return ret;
    }

    if (observer_.callbackInfos_[type] != nullptr) {
        std::shared_ptr<BluetoothCallbackInfo> callbackInfo = observer_.callbackInfos_[type];
        napi_delete_reference(env, callbackInfo->callback_);
    }
    observer_.callbackInfos_[type] = nullptr;
    HILOGI("%{public}s is unregistered", type.c_str());
    return ret;
}

static bool ParseStringArrayParameters(const napi_env &env, napi_value &args, std::vector<std::string> &params)
{
    HILOGI("%{public}s, enter", __func__);
    if (args == nullptr) {
        return NapiGetNull(env);
    }

    bool isArray = false;
    std::string fileParam;
    napi_is_array(env, args, &isArray);
    if (isArray) {
        uint32_t length = 0;
        napi_get_array_length(env, args, &length);
        for (size_t i = 0; i < length; i++) {
            napi_value result;
            napi_get_element(env, args, i, &result);
            if (!ParseString(env, fileParam, result)) {
                return false;
            }
            params.push_back(fileParam);
        }
    } else {
        return false;
    }
    return true;
}

napi_value NapiBluetoothOpp::SendFile(napi_env env, napi_callback_info info)
{
    HILOGI("SendFile called");

    size_t expectedArgsCount = ARGS_SIZE_THREE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_THREE] = {};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 3 argument.");
        return ret;
    }
    std::string device;
    if (!ParseString(env, device, argv[PARAM0])) {
        HILOGE("string expected.");
        return ret;
    }
    std::vector<std::string> filePaths;
    if (argv[PARAM1] != nullptr) {
        if (!ParseStringArrayParameters(env, argv[PARAM1], filePaths)) {
            return ret;
        }
    }
    std::vector<std::string> mimeTypes;
    if (argv[PARAM2] != nullptr) {
        if (!ParseStringArrayParameters(env, argv[PARAM2], mimeTypes)) {
            return ret;
        }
    }
    Opp *profile = Opp::GetProfile();
    bool isOK = profile->SendFile(device, filePaths, mimeTypes);
    napi_value result = nullptr;
    napi_get_boolean(env, isOK, &result);
    return result;
}

napi_value NapiBluetoothOpp::SetIncomingFileConfirmation(napi_env env, napi_callback_info info)
{
    HILOGI("SetIncomingFileConfirmation called");

    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }
    bool accept;
    if (!ParseBool(env, accept, argv[PARAM0])) {
        HILOGE("Bool expected.");
        return ret;
    }
    Opp *profile = Opp::GetProfile();
    bool isOk = profile->SetIncomingFileConfirmation(accept);
    napi_value result = nullptr;
    napi_get_boolean(env, isOk, &result);
    return result;
}

napi_value NapiBluetoothOpp::GetCurrentTransferInformation(napi_env env, napi_callback_info info)
{
    HILOGI("GetCurrentTransferInformation called");

    size_t expectedArgsCount = ARGS_SIZE_ZERO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ZERO] = {};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("No Requires argument.");
        return ret;
    }
    Opp *profile = Opp::GetProfile();
    BluetoothOppTransferInformation information = profile->GetCurrentTransferInformation();
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    ConvertOppTransferInformationToJS(env, obj, information);
    return obj;
}

napi_value NapiBluetoothOpp::CancelTransfer(napi_env env, napi_callback_info info)
{
    HILOGI("CancelTransfer called");

    size_t expectedArgsCount = ARGS_SIZE_ZERO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ZERO] = {};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("No Requires argument.");
        return ret;
    }
    Opp *profile = Opp::GetProfile();
    bool isOk = profile->CancelTransfer();
    napi_value result = nullptr;
    napi_get_boolean(env, isOk, &result);
    return result;
}

napi_value NapiBluetoothOpp::GetConnectionDevices(napi_env env, napi_callback_info info)
{
    HILOGI("GetConnectionDevices called");

    size_t expectedArgsCount = ARGS_SIZE_ZERO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ZERO] = {};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("No Requires argument.");
        return ret;
    }
    napi_create_array(env, &ret);
    Opp *profile = Opp::GetProfile();
    vector<int> states = { static_cast<int>(BTConnectState::CONNECTED) };
    vector<BluetoothRemoteDevice> devices = profile->GetDevicesByStates(states);
    vector<string> deviceVector;
    for (auto &device : devices) {
        deviceVector.push_back(device.GetDeviceAddr());
    }
    ConvertStringVectorToJS(env, ret, deviceVector);
    return ret;
}

napi_value NapiBluetoothOpp::GetDeviceState(napi_env env, napi_callback_info info)
{
    HILOGI("GetDeviceState called");
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }
    string deviceId;
    if (!ParseString(env, deviceId, argv[PARAM0])) {
        HILOGE("string expected.");
        return ret;
    }

    Opp *profile = Opp::GetProfile();
    BluetoothRemoteDevice device(deviceId, 1);
    int state = profile->GetDeviceState(device);
    int profileState = GetProfileConnectionState(state);
    napi_value result = nullptr;
    napi_create_int32(env, profileState, &result);
    return result;
}
}  // namespace Bluetooth
}  // namespace OHOS