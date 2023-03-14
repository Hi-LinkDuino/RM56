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
#ifndef OHOS_BLUETOOTH_BLUETOOTHHIDHOSTOBSERVERPROXY_H
#define OHOS_BLUETOOTH_BLUETOOTHHIDHOSTOBSERVERPROXY_H

#include <iremote_proxy.h>
#include "i_bluetooth_hid_host_observer.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;

class BluetoothHidHostObserverProxy : public IRemoteProxy<IBluetoothHidHostObserver> {
public:
    explicit BluetoothHidHostObserverProxy(
        const sptr<IRemoteObject>& remote) : IRemoteProxy<IBluetoothHidHostObserver>(remote)
    {}
    ~BluetoothHidHostObserverProxy()
    {}
    ErrCode OnConnectionStateChanged(const BluetoothRawAddress &device, int state) override;

private:
    static inline BrokerDelegator<BluetoothHidHostObserverProxy> delegator_;
};
} // Bluetooth
} // OHOS

#endif // OHOS_BLUETOOTH_BLUETOOTHHIDHOSTOBSERVERPROXY_H

