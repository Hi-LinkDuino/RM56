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

#ifndef NAPI_BLUETOOTH_AVRCP_CT_OBSERVER_H_
#define NAPI_BLUETOOTH_AVRCP_CT_OBSERVER_H_

#include <shared_mutex>
#include "bluetooth_avrcp_ct.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
static std::shared_mutex g_avrcpCtCallbackInfosMutex;
const std::string STR_BT_AVRCP_CT_CONNECTION_STATE_CHANGE = "connectionStateChange";
class NapiAvrcpControllerObserver : public AvrcpController::IObserver{
public:
    void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) override;
    void OnActionCompleted(const BluetoothRemoteDevice &device, const AvrcpCtResponse &resp) override {}
    NapiAvrcpControllerObserver() = default;
    virtual ~NapiAvrcpControllerObserver() = default;

    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> callbackInfos_ = {};
};

}  // namespace Bluetooth
}  // namespace OHOS
#endif /* NAPI_BLUETOOTH_AVRCP_CT_OBSERVER_H_ */