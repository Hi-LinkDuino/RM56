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

#ifndef BT_HAL_H4_PROTOCOL_H
#define BT_HAL_H4_PROTOCOL_H

#include "hci_protocol.h"

namespace OHOS {
namespace HDI {
namespace BT {
namespace HCI {
class H4Protocol : public HciProtocol {
public:
    H4Protocol(int fd, HciDataCallback onAclReceive, HciDataCallback onScoReceive, HciDataCallback onEventReceive);

    ssize_t SendPacket(HciPacketType packetType, const std::vector<uint8_t> &packetData) override;
    void ReadData(int fd);

private:
    void PacketCallback();

private:
    int hciFd_ = 0;
    HciDataCallback onAclReceive_;
    HciDataCallback onScoReceive_;
    HciDataCallback onEventReceive_;

    uint8_t packetType_ = 0;
    std::vector<uint8_t> hciPacket_;
    uint32_t readLength_ = 0;
};
}  // namespace HCI
}  // namespace BT
}  // namespace HDI
}  // namespace OHOS

#endif /* BT_HAL_H4_PROTOCOL_H */