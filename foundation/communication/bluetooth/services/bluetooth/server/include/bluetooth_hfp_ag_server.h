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

#ifndef OHOS_BLUETOOTH_STANDARD_HFP_AG_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_HFP_AG_SERVER_H

#include <string>
#include <vector>

#include "bluetooth_types.h"
#include "bluetooth_hfp_ag_stub.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothHfpAgServer : public BluetoothHfpAgStub{ 
public:
    explicit BluetoothHfpAgServer();
    ~BluetoothHfpAgServer() override;

    int GetConnectDevices(std::vector<BluetoothRawAddress> &devices) override;
    int GetDevicesByStates(const std::vector<int> &states, std::vector<BluetoothRawAddress> &devices) override;
    int GetDeviceState(const BluetoothRawAddress &device) override;
    int Connect(const BluetoothRawAddress &device) override;
    int Disconnect(const BluetoothRawAddress &device) override;
    int GetScoState(const BluetoothRawAddress &device) override;
    bool ConnectSco() override;
    bool DisconnectSco() override;
    void PhoneStateChanged(int numActive, int numHeld, int callState, const std::string &number, int type, 
        const std::string &name) override;
    void ClccResponse(int index, int direction, int status, int mode, bool mpty, const std::string &number, 
        int type) override;
    bool OpenVoiceRecognition(const BluetoothRawAddress &device) override;
    bool CloseVoiceRecognition(const BluetoothRawAddress &device) override;
    bool SetActiveDevice(const BluetoothRawAddress &device) override;
    std::string GetActiveDevice() override;
    bool IntoMock(const BluetoothRawAddress &device, int state) override;
    bool SendNoCarrier(const BluetoothRawAddress &device) override;
    void RegisterObserver(const sptr<IBluetoothHfpAgObserver> &observer) override;
    void DeregisterObserver(const sptr<IBluetoothHfpAgObserver> &observer) override;

private:
    BLUETOOTH_DECLARE_IMPL();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothHfpAgServer);
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif // OHOS_BLUETOOTH_STANDARD_HFP_AG_SERVER_H