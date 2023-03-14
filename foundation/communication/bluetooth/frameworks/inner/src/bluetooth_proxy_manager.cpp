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

#include "bluetooth_proxy_manager.h"

#include "bluetooth_ble_central_manager.h"
#include "bluetooth_def.h"
#include "bluetooth_log.h"
#include "i_bluetooth_ble_central_manager.h"
#include "i_bluetooth_host.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Bluetooth {
BluetoothProxyManager& BluetoothProxyManager::GetInstance()
{
    static BluetoothProxyManager instance;
    return instance;
}

bool BluetoothProxyManager::ProxyUid(int32_t uid, bool isProxy)
{
    HILOGI("ProxyUid start. uid:%{public}d , isProxy:%{public}d", uid, isProxy);
    if (!GetBleCentralManagerProxy()) {
        HILOGE("GetBleCentralManagerProxy failed.");
        return false;
    }
    return proxy_->ProxyUid(uid, isProxy);
}

bool BluetoothProxyManager::ResetAllProxy()
{
    HILOGI("ResetAllProxy start.");
    if (!GetBleCentralManagerProxy()) {
        HILOGE("GetBleCentralManagerProxy failed.");
        return false;
    }
    return proxy_->ResetAllProxy();
}

bool BluetoothProxyManager::GetBleCentralManagerProxy()
{
    if (proxy_) {
        return true;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOGE("samgr is null");
        return false;
    }

    sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);
    if (!hostRemote) {
        HILOGE("hostRemote is null");
        return false;
    }
    sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
    if (!hostProxy) {
        HILOGE("hostProxy is null");
        return false;
    }
    sptr<IRemoteObject> remote = hostProxy->GetBleRemote(BLE_CENTRAL_MANAGER_SERVER);
    if (!remote) {
        HILOGE("remote is null");
        return false;
    }
    proxy_ = iface_cast<IBluetoothBleCentralManager>(remote);
    if (!proxy_ || !proxy_->AsObject()) {
        HILOGE("proxy_ is null");
        return false;
    }
    recipient_ = new (std::nothrow) BleCentralManagerDeathRecipient(*this);
    if (!recipient_) {
        HILOGE("recipient_ is null");
        proxy_ = nullptr;
        return false;
    }
    proxy_->AsObject()->AddDeathRecipient(recipient_);
    return true;
}

void BluetoothProxyManager::ResetClient()
{
    if (proxy_ && proxy_->AsObject()) {
        proxy_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    proxy_ = nullptr;
}

BluetoothProxyManager::BleCentralManagerDeathRecipient::BleCentralManagerDeathRecipient(
    BluetoothProxyManager &bleProxyManager) : bleProxyManager_(bleProxyManager) {}

BluetoothProxyManager::BleCentralManagerDeathRecipient::~BleCentralManagerDeathRecipient() {}

void BluetoothProxyManager::BleCentralManagerDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    bleProxyManager_.ResetClient();
}
} // namespace Bluetooth
} // namespace OHOS