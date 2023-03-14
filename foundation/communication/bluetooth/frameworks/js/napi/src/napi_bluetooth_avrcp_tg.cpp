/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#include "bluetooth_avrcp_tg.h"
#include "napi_bluetooth_avrcp_tg.h"
#include "napi_bluetooth_profile.h"

namespace OHOS {
namespace Bluetooth {
using namespace std;

NapiAvrcpTargetObserver NapiAvrcpTarget::observer_;
bool NapiAvrcpTarget::isRegistered_ = false;

void NapiAvrcpTarget::DefineAvrcpTargetJSClass(napi_env env)
{
    napi_value constructor;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("connect", Connect),       
        DECLARE_NAPI_FUNCTION("disconnect", Disconnect),
        DECLARE_NAPI_FUNCTION("on", On),       
        DECLARE_NAPI_FUNCTION("off", Off),
        DECLARE_NAPI_FUNCTION("getConnectionDevices", GetConnectionDevices),
        DECLARE_NAPI_FUNCTION("getDeviceState", GetDeviceState),
    };

    napi_define_class(env, "AvrcpTarget", NAPI_AUTO_LENGTH, AvrcpTargetConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    napi_value napiProfile;
    napi_new_instance(env, constructor, 0, nullptr, &napiProfile);
    NapiProfile::SetProfile(ProfileId::PROFILE_AVRCP_TG, napiProfile);
    HILOGI("finished");
}

napi_value NapiAvrcpTarget::AvrcpTargetConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

napi_value NapiAvrcpTarget::On(napi_env env, napi_callback_info info) {
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

    if (!isRegistered_) {
        AvrcpTarget *profile = AvrcpTarget::GetProfile();
        profile->RegisterObserver(&observer_);
        isRegistered_ = true;
    }
    HILOGI("%{public}s is registered", type.c_str());
    return ret;
}

napi_value NapiAvrcpTarget::Off(napi_env env, napi_callback_info info)
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
    observer_.callbackInfos_[type] = nullptr;
    HILOGI("%{public}s is unregistered", type.c_str());
    return ret;
}

napi_value NapiAvrcpTarget::GetConnectionDevices(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    napi_value ret = nullptr;
    napi_create_array(env, &ret);
    AvrcpTarget *profile = AvrcpTarget::GetProfile();
    vector<int> states;
    states.push_back(1);
    vector<BluetoothRemoteDevice> devices = profile->GetDevicesByStates(states);
    vector<string> deviceVector;
    for (auto &device: devices) {
        deviceVector.push_back(device.GetDeviceAddr());
    }
    ConvertStringVectorToJS(env, ret, deviceVector);
    return ret;
}

napi_value NapiAvrcpTarget::GetDeviceState(napi_env env, napi_callback_info info)
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

    AvrcpTarget *profile = AvrcpTarget::GetProfile();
    BluetoothRemoteDevice device(deviceId, 1);
    int state = profile->GetDeviceState(device);
    napi_value result = nullptr;
    int status = GetProfileConnectionState(state);
    napi_create_int32(env, status, &result);
    HILOGI("status: %{public}d", status);
    return result;
}

napi_value NapiAvrcpTarget::Connect(napi_env env, napi_callback_info info)
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

    AvrcpTarget *profile = AvrcpTarget::GetProfile();
    BluetoothRemoteDevice device(deviceId, 1);
    bool res = profile->Connect(device);

    napi_value result = nullptr;
    napi_get_boolean(env, res, &result);
    HILOGI("res: %{public}d", res);
    return result;
}

napi_value NapiAvrcpTarget::Disconnect(napi_env env, napi_callback_info info)
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

    AvrcpTarget *profile = AvrcpTarget::GetProfile();
    BluetoothRemoteDevice device(deviceId, 1);
    bool res = profile->Disconnect(device);

    napi_value result = nullptr;
    napi_get_boolean(env, res, &result);
    HILOGI("res: %{public}d", res);
    return result;
}

} // namespace Bluetooth
} // namespace OHOS
