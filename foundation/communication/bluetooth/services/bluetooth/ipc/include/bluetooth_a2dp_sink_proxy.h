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

#ifndef OHOS_BLUETOOTH_STANDARD_A2DP_SINK_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_A2DP_SINK_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_a2dp_sink.h"
#include "raw_address.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
class BluetoothA2dpSinkProxy : public IRemoteProxy<IBluetoothA2dpSink> {
public:
    explicit BluetoothA2dpSinkProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBluetoothA2dpSink>(impl)
    {}
    ~BluetoothA2dpSinkProxy()
    {}

    int Connect(const RawAddress &device) override;
    int Disconnect(const RawAddress &device) override;
    void RegisterObserver(const sptr<IBluetoothA2dpSinkObserver> &observer) override;
    void DeregisterObserver(const sptr<IBluetoothA2dpSinkObserver> &observer) override;
    std::vector<RawAddress> GetDevicesByStates(const std::vector<int32_t> &states) override;
    int GetDeviceState(const RawAddress &device) override;
    int GetPlayingState(const RawAddress &device) override;
    int SetConnectStrategy(const RawAddress &device, int32_t strategy) override;
    int GetConnectStrategy(const RawAddress &device) override;
    int SendDelay(const RawAddress &device, int32_t delayValue) override;

private:
    static inline BrokerDelegator<BluetoothA2dpSinkProxy> delegator_;
    /**
     * @brief Write the serializable data
     * @param parcelableVector The communication data of IPC
     * @param reply Serializable data
     * @return true: Write the serializable data successfully; otherwise is not.
     */
    bool WriteParcelableInt32Vector(const std::vector<int32_t> &parcelableVector, Parcel &reply);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_A2DP_SINK_PROXY_H