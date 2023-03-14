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

#ifndef OHOS_HARDWARE_BT_V1_0_BTHCICALLBACKSPROXY_H
#define OHOS_HARDWARE_BT_V1_0_BTHCICALLBACKSPROXY_H

#include <iremote_proxy.h>
#include "ibt_hci_callbacks.h"

namespace ohos {
namespace hardware {
namespace bt {
namespace v1_0 {
class BtHciCallbacksProxy : public IRemoteProxy<IBtHciCallbacks> {
public:
    explicit BtHciCallbacksProxy(
        const sptr<IRemoteObject>& remote)
        : IRemoteProxy<IBtHciCallbacks>(remote)
    {}

    virtual ~BtHciCallbacksProxy() {}

    int32_t OnInited(BtStatus status) override;

    int32_t OnReceivedHciPacket(BtType type, const std::vector<uint8_t>& data) override;

private:
    static inline BrokerDelegator<BtHciCallbacksProxy> delegator_;
};
} // v1_0
} // bt
} // hardware
} // ohos

#endif // OHOS_HARDWARE_BT_V1_0_BTHCICALLBACKSPROXY_H

