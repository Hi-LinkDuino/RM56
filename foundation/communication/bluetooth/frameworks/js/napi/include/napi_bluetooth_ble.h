/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef NAPI_BLUETOOTH_BLE_H_
#define NAPI_BLUETOOTH_BLE_H_

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Bluetooth {
void DefineBLEJSObject(napi_env env, napi_value exports);
void DefineSystemBLEInterface(napi_env env, napi_value exports);
napi_value StartBLEScan(napi_env env, napi_callback_info info);
napi_value StopBLEScan(napi_env env, napi_callback_info info);
napi_value StartAdvertising(napi_env env, napi_callback_info info);
napi_value StopAdvertising(napi_env env, napi_callback_info info);
napi_value GetConnectedBLEDevices(napi_env env, napi_callback_info info);
napi_value PropertyInit(napi_env env, napi_value exports);
}  // namespace Bluetooth
}  // namespace OHOS
#endif /* NAPI_BLUETOOTH_BLE_H_ */
