/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BT_HAL_VENDOR_INTERFACE_H
#define BT_HAL_VENDOR_INTERFACE_H

#include <functional>
#include <vector>

#include "singleton.h"

#include "hci_internal.h"
#include "hci_protocol.h"
#include "hci_watcher.h"
#include "ohos_bt_vendor_lib.h"

namespace OHOS {
namespace HDI {
namespace BT {
namespace V1_0 {
class VendorInterface : public DelayedSingleton<VendorInterface> {
public:
    using InitializeCompleteCallback = std::function<void(bool isSuccess)>;
    using ReceiveDataCallback = HCI::HciProtocol::HciDataCallback;
    struct ReceiveCallback {
        ReceiveDataCallback onAclReceive;
        ReceiveDataCallback onScoReceive;
        ReceiveDataCallback onEventReceive;
    };

    bool Initialize(InitializeCompleteCallback initializeCompleteCallback, const ReceiveCallback &receiveCallback);
    void CleanUp();
    size_t SendPacket(HCI::HciPacketType type, const std::vector<uint8_t> &packet);

private:
    static void OnInitCallback(bt_op_result_t result);
    static void* OnMallocCallback(int size);
    static void OnFreeCallback(void* buf);
    static size_t OnCmdXmitCallback(uint16_t opcode, void* buf);

    void OnEventReceived(const std::vector<uint8_t> &data);
    bool WatchHciChannel(const ReceiveCallback &receiveCallback);
    void WatcherTimeout();

private:
    InitializeCompleteCallback initializeCompleteCallback_;
    ReceiveDataCallback eventDataCallback_;
    void* vendorHandle_ = nullptr;
    bt_vendor_interface_t *vendorInterface_ = nullptr;
    static bt_vendor_callbacks_t vendorCallbacks_;
    HciWatcher watcher_;
    std::shared_ptr<HCI::HciProtocol> hci_ = nullptr;
    uint16_t vendorSentOpcode_ = 0;
    uint32_t lpmTimer_ = 0;
    std::mutex wakeupMutex_;
    bool wakeupLock_ = false;
    bool activity_ = false;

    DECLARE_DELAYED_SINGLETON(VendorInterface);
    DISALLOW_COPY_AND_MOVE(VendorInterface);
};
}  // namespace V1_0
}  // namespace BT
}  // namespace HDI
}  // namespace OHOS
#endif /* BT_HAL_VENDOR_INTERFACE_H */