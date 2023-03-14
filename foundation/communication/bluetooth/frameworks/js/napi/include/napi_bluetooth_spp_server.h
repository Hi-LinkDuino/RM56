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
#ifndef NAPI_BLUETOOTH_SPP_SERVER_H_
#define NAPI_BLUETOOTH_SPP_SERVER_H_

#include <map>
#include "bluetooth_socket.h"
#include "napi_bluetooth_spp_client.h"

namespace OHOS {
namespace Bluetooth {
void DefineSppFunctions(napi_env env, napi_value exports);

struct NapiSppServer {
    static napi_value SppListen(napi_env env, napi_callback_info info);
    static napi_value SppAccept(napi_env env, napi_callback_info info);
    static napi_value SppConnect(napi_env env, napi_callback_info info);
    static napi_value SppCloseServerSocket(napi_env env, napi_callback_info info);

    static std::map<int, std::shared_ptr<NapiSppServer>> serverMap;
    static int count;

    std::shared_ptr<SppServerSocket> server_ = nullptr;
    int id_ = -1;
};
} // namespace Bluetooth
} // namespace OHOS
#endif /* NAPI_BLUETOOTH_SPP_SERVER_H_ */