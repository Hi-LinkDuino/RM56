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
#ifndef NAPI_BLUETOOTH_SPP_CLIENT_H_
#define NAPI_BLUETOOTH_SPP_CLIENT_H_

#include <thread>
#include <map>
#include "bluetooth_socket.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
const std::string STR_BT_SPP_READ = "sppRead";

struct NapiSppClient {
    static napi_value SppConnect(napi_env env, napi_callback_info info);
    static napi_value SppCloseClientSocket(napi_env env, napi_callback_info info);
    static napi_value SppWrite(napi_env env, napi_callback_info info);
    static void On(napi_env env, napi_callback_info info);
    static void Off(napi_env env, napi_callback_info info);
    static void SppRead(int id);

    static std::map<int, std::shared_ptr<NapiSppClient>> clientMap;
    static int count;

    bool sppReadFlag = false;
    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> callbackInfos_ = {};

    std::shared_ptr<SppClientSocket> client_ = nullptr;
    std::shared_ptr<BluetoothRemoteDevice> device_ = nullptr;
    std::shared_ptr<std::thread> thread_;
    int id_ = -1;
};
} // namespace Bluetooth
} // namespace OHOS
#endif /* NAPI_BLUETOOTH_SPP_CLIENT_H_ */