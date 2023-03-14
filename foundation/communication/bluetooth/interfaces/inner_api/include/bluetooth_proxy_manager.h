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

#ifndef BLUETOOTH_PROXY_MANAGER_H
#define BLUETOOTH_PROXY_MANAGER_H

#include <mutex>

#include "iremote_broker.h"
#include "refbase.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothBleCentralManager;
class BluetoothProxyManager {
public:
    bool ProxyUid(int32_t uid, bool isProxy);
    bool ResetAllProxy();

    static BluetoothProxyManager& GetInstance();

private:
    BluetoothProxyManager() = default;
    ~BluetoothProxyManager() = default;
    BluetoothProxyManager(const BluetoothProxyManager& bleProxyManager);
    const BluetoothProxyManager &operator=(const BluetoothProxyManager& bleProxyManager);

private:
    class BleCentralManagerDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit BleCentralManagerDeathRecipient(BluetoothProxyManager &bleProxyManager);
        ~BleCentralManagerDeathRecipient();

        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    private:
        BluetoothProxyManager &bleProxyManager_;
    };

private:
    bool GetBleCentralManagerProxy();
    void ResetClient();
    sptr<IBluetoothBleCentralManager> proxy_;
    sptr<BleCentralManagerDeathRecipient> recipient_;
    std::mutex mutex_;
};
} // namespace Bluetooth
} // namespace OHOS
#endif