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

#ifndef BT_HAL_HCI_PROTOCOL_H
#define BT_HAL_HCI_PROTOCOL_H

#include <cstdio>
#include <vector>

#include "hci_internal.h"

namespace OHOS {
namespace HDI {
namespace BT {
namespace HCI {
class HciProtocol {
public:
    using HciDataCallback = std::function<void(const std::vector<uint8_t> &data)>;

    HciProtocol() = default;
    virtual ~HciProtocol() = default;

    virtual ssize_t SendPacket(HciPacketType packetType, const std::vector<uint8_t> &packetData) = 0;

    const PacketHeader& GetPacketHeaderInfo(HciPacketType packetType);

protected:
    static ssize_t Read(int fd, uint8_t *data, size_t length);
    static ssize_t Write(int fd, const uint8_t *data, size_t length);

    static const PacketHeader header_[HCI_PACKET_TYPE_MAX];
};
}  // namespace HCI
}  // namespace BT
}  // namespace HDI
}  // namespace OHOS
#endif /* BT_HAL_HCI_PROTOCOL_H */