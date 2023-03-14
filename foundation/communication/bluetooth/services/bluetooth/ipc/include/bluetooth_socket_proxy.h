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

#ifndef OHOS_BLUETOOTH_STANDARD_SOCKET_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_SOCKET_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_socket.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothSocketProxy : public IRemoteProxy<IBluetoothSocket> {
public:
    explicit BluetoothSocketProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBluetoothSocket>(impl)
    {}
    ~BluetoothSocketProxy()
    {}

    int Connect(std::string &addr, bluetooth::Uuid &uuid, int securityFlag, int type) override;
    int Listen(std::string &name, bluetooth::Uuid &uuid, int securityFlag, int type) override;

private:
    static inline BrokerDelegator<BluetoothSocketProxy> delegator_;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_SOCKET_PROXY_H