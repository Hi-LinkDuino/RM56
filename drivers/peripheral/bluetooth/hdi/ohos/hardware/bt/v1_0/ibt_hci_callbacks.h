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

#ifndef OHOS_HARDWARE_BT_V1_0_IBTHCICALLBACKS_H
#define OHOS_HARDWARE_BT_V1_0_IBTHCICALLBACKS_H

#include <vector>
#include <string>
#include <cstdint>
#include <hdf_log.h>
#include <iservmgr_hdi.h>
#include "ohos/hardware/bt/v1_0/types.h"

namespace ohos {
namespace hardware {
namespace bt {
namespace v1_0 {
using namespace OHOS;

enum {
    CMD_ON_INITED,
    CMD_ON_RECEIVED_HCI_PACKET,
};

class IBtHciCallbacks : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.hardware.bt.v1_0.IBtHciCallbacks");

    virtual ~IBtHciCallbacks() = default;

    virtual int32_t OnInited(BtStatus status) = 0;

    virtual int32_t OnReceivedHciPacket(BtType type, const std::vector<uint8_t>& data) = 0;
};
} // v1_0
} // bt
} // hardware
} // ohos

#endif // OHOS_HARDWARE_BT_V1_0_IBTHCICALLBACKS_H

