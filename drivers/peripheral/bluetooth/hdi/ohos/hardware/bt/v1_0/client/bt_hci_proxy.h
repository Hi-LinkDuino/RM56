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

#ifndef OHOS_HARDWARE_BT_V1_0_BTHCIPROXY_H
#define OHOS_HARDWARE_BT_V1_0_BTHCIPROXY_H

#include <iremote_proxy.h>
#include "ibt_hci.h"

namespace ohos {
namespace hardware {
namespace bt {
namespace v1_0 {
class BtHciProxy : public IRemoteProxy<IBtHci> {
public:
    explicit BtHciProxy(const sptr<IRemoteObject> &remote) : IRemoteProxy<IBtHci>(remote)
    {}

    virtual ~BtHciProxy()
    {}

    int32_t Init(const sptr<IBtHciCallbacks> &callbacks) override;

    int32_t SendHciPacket(BtType type, const std::vector<uint8_t> &data) override;

    int32_t Close() override;

private:
    static inline BrokerDelegator<BtHciProxy> delegator_;
};
}  // namespace v1_0
}  // namespace bt
}  // namespace hardware
}  // namespace ohos

#endif  // OHOS_HARDWARE_BT_V1_0_BTHCIPROXY_H
