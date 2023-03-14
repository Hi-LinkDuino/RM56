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

#include "bluetooth_pan.h"
#include "bluetooth_log.h"
#include "napi_bluetooth_pan_observer.h"
#include "napi_bluetooth_utils.h"
#include "napi_bluetooth_profile.h"
#include "napi_bluetooth_pan.h"

namespace OHOS {
namespace Bluetooth {
using namespace std;
NapiBluetoothPanObserver NapiBluetoothPan::observer_;

void NapiBluetoothPan::DefinePanJSClass(napi_env env)
{
    napi_value constructor;
    napi_property_descriptor properties [] = {
        DECLARE_NAPI_FUNCTION("on", NapiBluetoothPan::On),
        DECLARE_NAPI_FUNCTION("off", NapiBluetoothPan::Off),
        DECLARE_NAPI_FUNCTION("getConnectionDevices", NapiBluetoothPan::GetConnectionDevices),
        DECLARE_NAPI_FUNCTION("getDeviceState", NapiBluetoothPan::GetDeviceState),
        DECLARE_NAPI_FUNCTION("disconnect", NapiBluetoothPan::Disconnect),
        DECLARE_NAPI_FUNCTION("setTethering", NapiBluetoothPan::SetTethering),
        DECLARE_NAPI_FUNCTION("isTetheringOn", NapiBluetoothPan::IsTetheringOn),
    };

    napi_define_class(env, "NapiBluetoothPan", NAPI_AUTO_LENGTH, NapiBluetoothPan::PanConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    napi_value napiProfile;
    napi_new_instance(env, constructor, 0, nullptr, &napiProfile);
    NapiProfile::SetProfile(ProfileId::PROFILE_PAN_NETWORK, napiProfile);
    Pan *profile = Pan::GetProfile();
    profile->RegisterObserver(&NapiBluetoothPan::observer_);
    HILOGI("finished");
}

napi_value NapiBluetoothPan::PanConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

napi_value NapiBluetoothPan::On(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
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
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = std::make_shared<BluetoothCallbackInfo>();
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

napi_value NapiBluetoothPan::Off(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
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

napi_value NapiBluetoothPan::GetConnectionDevices(napi_env env, napi_callback_info info)
{
    HILOGI("enter");

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
    Pan *profile = Pan::GetProfile();
    vector<int> states = { static_cast<int>(BTConnectState::CONNECTED) };
    vector<BluetoothRemoteDevice> devices = profile->GetDevicesByStates(states);
    vector<string> deviceVector;
    for (auto &device : devices) {
        deviceVector.push_back(device.GetDeviceAddr());
    }
    ConvertStringVectorToJS(env, ret, deviceVector);
    return ret;
}

napi_value NapiBluetoothPan::GetDeviceState(napi_env env, napi_callback_info info)
{
    HILOGI("enter");

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

    Pan *profile = Pan::GetProfile();
    BluetoothRemoteDevice device(deviceId, 1);
    int state = profile->GetDeviceState(device);
    int profileState = GetProfileConnectionState(state);
    napi_value result = nullptr;
    napi_create_int32(env, profileState, &result);
    HILOGI("profileState: %{public}d", profileState);
    return result;
}

napi_value NapiBluetoothPan::Disconnect(napi_env env, napi_callback_info info)
{
    HILOGI("enter");

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

    Pan *profile = Pan::GetProfile();
    BluetoothRemoteDevice device(deviceId, 1);
    bool res = profile->Disconnect(device);
    napi_value result = nullptr;
    napi_get_boolean(env, res, &result);
    HILOGI("res: %{public}d", res);
    return result;
}

napi_value NapiBluetoothPan::SetTethering(napi_env env, napi_callback_info info)
{
    HILOGI("enter");

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
    bool value;
    if (!ParseBool(env, value, argv[PARAM0])) {
        HILOGE("Bool expected.");
        return ret;
    }

    Pan *profile = Pan::GetProfile();
    bool res = profile->SetTethering(value);
    napi_value result = nullptr;
    napi_get_boolean(env, res, &result);
    HILOGI("res: %{public}d", res);
    return result;
}

napi_value NapiBluetoothPan::IsTetheringOn(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
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
    Pan *profile = Pan::GetProfile();
    bool res = profile->IsTetheringOn();
    napi_value result = nullptr;
    napi_get_boolean(env, res, &result);
    HILOGI("res: %{public}d", res);
    return result;
}
}  // namespace Bluetooth
}  // namespace OHOS