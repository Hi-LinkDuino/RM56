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

#ifndef NAPI_BLUETOOTH_GATT_SERVER_H_
#define NAPI_BLUETOOTH_GATT_SERVER_H_

#include <vector>
#include "bluetooth_gatt_server.h"
#include "bluetooth_log.h"
#include "napi_bluetooth_gatt_server_callback.h"

namespace OHOS {
namespace Bluetooth {
class NapiGattServer {

public:
    static napi_value CreateGattServer(napi_env env, napi_callback_info info);
    static void DefineGattServerJSClass(napi_env env);
    static napi_value GattServerConstructor(napi_env env, napi_callback_info info);

    static napi_value On(napi_env env, napi_callback_info info);
    static napi_value Off(napi_env env, napi_callback_info info);

    static napi_value AddService(napi_env env, napi_callback_info info);
    static napi_value Close(napi_env env, napi_callback_info info);
    static napi_value RemoveGattService(napi_env env, napi_callback_info info);
    static napi_value SendResponse(napi_env env, napi_callback_info info);
    static napi_value NotifyCharacteristicChanged(napi_env env, napi_callback_info info);

    std::shared_ptr<GattServer> &GetServer()
    {
        return server_;
    }
    NapiGattServerCallback &GetCallback()
    {
        return callback_;
    }
    static std::vector<std::string> deviceList;

    NapiGattServer()
    {
        HILOGI("enter");
        server_ = std::make_shared<GattServer>(callback_);
    }
    ~NapiGattServer() = default;

    static thread_local napi_ref consRef_;

private:
    std::shared_ptr<GattServer> server_ = nullptr;
    NapiGattServerCallback callback_;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif /* NAPI_BLUETOOTH_GATT_SERVER_H_ */