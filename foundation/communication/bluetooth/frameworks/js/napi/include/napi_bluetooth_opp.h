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
#ifndef NAPI_BLUETOOTH_OPP_H
#define NAPI_BLUETOOTH_OPP_H

#include "bluetooth_opp.h"
#include "napi_bluetooth_opp_observer.h"

namespace OHOS {
namespace Bluetooth {
class NapiBluetoothOpp {
public:
    static void DefineOppJSClass(napi_env env);
    static napi_value OppConstructor(napi_env env, napi_callback_info info);

    static napi_value On(napi_env env, napi_callback_info info);
    static napi_value Off(napi_env env, napi_callback_info info);

    static napi_value SendFile(napi_env env, napi_callback_info info);
    static napi_value SetIncomingFileConfirmation(napi_env env, napi_callback_info info);
    static napi_value GetCurrentTransferInformation(napi_env env, napi_callback_info info);
    static napi_value CancelTransfer(napi_env env, napi_callback_info info);
    static napi_value GetConnectionDevices(napi_env env, napi_callback_info info);
    static napi_value GetDeviceState(napi_env env, napi_callback_info info);

    static NapiBluetoothOppObserver observer_;
    static bool isRegistered_;
};
} // namespace Bluetooth
} // namespace OHOS
#endif  // NAPI_BLUETOOTH_OPP_H