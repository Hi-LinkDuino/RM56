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

#ifndef NAPI_BLUETOOTH_HOST_H_
#define NAPI_BLUETOOTH_HOST_H_

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Bluetooth {
napi_value BluetoothHostInit(napi_env env, napi_value exports);
napi_value EnableBluetooth(napi_env env, napi_callback_info info);
napi_value DisableBluetooth(napi_env env, napi_callback_info info);
napi_value SetLocalName(napi_env env, napi_callback_info info);
napi_value GetLocalName(napi_env env, napi_callback_info info);
napi_value GetRemoteDeviceName(napi_env env, napi_callback_info info);
napi_value GetRemoteDeviceClass(napi_env env, napi_callback_info info);

napi_value SetBluetoothScanMode(napi_env env, napi_callback_info info);
napi_value GetBluetoothScanMode(napi_env env, napi_callback_info info);
napi_value StartBluetoothDiscovery(napi_env env, napi_callback_info info);
napi_value StopBluetoothDiscovery(napi_env env, napi_callback_info info);
napi_value GetState(napi_env env, napi_callback_info info);
napi_value GetBtConnectionState(napi_env env, napi_callback_info info);
napi_value PairDevice(napi_env env, napi_callback_info info);
napi_value CancelPairedDevice(napi_env env, napi_callback_info info);
napi_value GetPairedDevices(napi_env env, napi_callback_info info);
napi_value SetDevicePairingConfirmation(napi_env env, napi_callback_info info);


napi_value GetDeviceName(napi_env env, napi_callback_info info);
napi_value GetRssiValue(napi_env env, napi_callback_info info);

napi_value PropertyValueInit(napi_env env, napi_value exports);
napi_value ScanDutyInit(napi_env env);
napi_value MatchModeInit(napi_env env);
napi_value StateChangeInit(napi_env env);
napi_value ProfileStateInit(napi_env env);
napi_value ScanModeInit(napi_env env);
napi_value BondStateInit(napi_env env);
napi_value MajorClassOfDeviceInit(napi_env env);
napi_value MajorMinorClassOfDeviceInit(napi_env env);
napi_value NapiGetRemoteDeviceAddr(napi_env env, napi_callback_info info, napi_value addrArgv);

void RegisterObserverToHost();
napi_value GetProfileConnState(napi_env env, napi_callback_info info);
}  // namespace Bluetooth
}  // namespace OHOS
#endif /* NAPI_BLUETOOTH_H_ */
