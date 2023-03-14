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

#ifndef NAPI_BLUETOOTH_OPP_OBSERVER_H
#define NAPI_BLUETOOTH_OPP_OBSERVER_H

#include "bluetooth_opp.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
const std::string STR_BT_OPP_OBSERVER_RECEIVE_INCOMING_FILE = "receiveIncomingFile";
const std::string STR_BT_OPP_OBSERVER_TRANSFER_STATE_CHANGE = "transferStateChange";

class NapiBluetoothOppObserver : public OppObserver {
public:
    NapiBluetoothOppObserver() = default;
    virtual ~NapiBluetoothOppObserver() = default;

    void OnReceiveIncomingFileChanged(const BluetoothOppTransferInformation &transferInformation) override;
    void OnTransferStateChanged(const BluetoothOppTransferInformation &transferInformation) override;

    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> callbackInfos_ = {};
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // NAPI_BLUETOOTH_OPP_OBSERVER_H