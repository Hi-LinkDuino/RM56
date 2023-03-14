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

#ifndef OHOS_HARDWARE_BT_V1_0_BTHCISERVICE_H
#define OHOS_HARDWARE_BT_V1_0_BTHCISERVICE_H

#include <hdf_base.h>
#include "ibt_hci.h"
#include "remote_death_recipient.h"

namespace ohos {
namespace hardware {
namespace bt {
namespace v1_0 {
class BtHciService : public IBtHci {
public:
    BtHciService();
    ~BtHciService() override;

    int32_t Init(const sptr<IBtHciCallbacks>& callbacks) override;

    int32_t SendHciPacket(BtType type, const std::vector<uint8_t>& data) override;

    int32_t Close() override;

private:
    void OnRemoteDied(const wptr<IRemoteObject> &object);

private:
    sptr<IBtHciCallbacks> callbacks_ = nullptr;
    sptr<RemoteDeathRecipient> remoteDeathRecipient_ = nullptr;
};
} // v1_0
} // bt
} // hardware
} // ohos

#endif // OHOS_HARDWARE_BT_V1_0_BTHCISERVICE_H

