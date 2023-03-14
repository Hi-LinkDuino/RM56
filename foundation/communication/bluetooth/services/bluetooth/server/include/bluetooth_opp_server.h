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

#ifndef OHOS_BLUETOOTH_STANDARD_OPP_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_OPP_SERVER_H

#include <map>

#include "bt_def.h"
#include "bluetooth_opp_stub.h"
#include "bluetooth_types.h"
#include "i_bluetooth_opp.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "interface_adapter_manager.h"
#include "interface_profile_manager.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothOppServer : public BluetoothOppStub {
public:
    BluetoothOppServer();
    ~BluetoothOppServer() override;

    ErrCode SendFile(
        std::string &device,
        std::vector<std::string> &filePaths,
        std::vector<std::string> &mimeTypes,
        bool& result) override;

    ErrCode SetIncomingFileConfirmation(
        bool &accept,
        bool &result) override;

    ErrCode GetCurrentTransferInformation(
        BluetoothIOppTransferInformation &transferInformation) override;

    ErrCode CancelTransfer(
        bool &result) override;

    ErrCode RegisterObserver(
        const sptr<IBluetoothOppObserver> observer) override;

    ErrCode DeregisterObserver(
        const sptr<IBluetoothOppObserver> observer) override;

    ErrCode GetDeviceState(
        const BluetoothRawAddress &device,
        int& result) override;

    ErrCode GetDevicesByStates(
        const std::vector<int32_t> &states,
        std::vector<BluetoothRawAddress>& result) override;

private:
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothOppServer);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_OPP_SERVER_H