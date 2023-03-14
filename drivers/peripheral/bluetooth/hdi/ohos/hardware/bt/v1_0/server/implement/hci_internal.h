/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BT_HAL_HCI_INTERNAL_H
#define BT_HAL_HCI_INTERNAL_H

namespace OHOS {
namespace HDI {
namespace BT {
namespace HCI {
enum HciPacketType : uint8_t {
    HCI_PACKET_TYPE_UNKNOWN = 0,
    HCI_PACKET_TYPE_COMMAND = 1,
    HCI_PACKET_TYPE_ACL_DATA = 2,
    HCI_PACKET_TYPE_SCO_DATA = 3,
    HCI_PACKET_TYPE_EVENT = 4,
    HCI_PACKET_TYPE_MAX,
};

struct PacketHeader {
    uint8_t headerSize;
    uint8_t dataLengthOffset;
    uint8_t dataLengthSize;
};

constexpr uint8_t HCI_EVENT_CODE_COMMAND_COMPLETE = 0x0E;
constexpr uint8_t HCI_EVENT_CODE_VENDOR_SPECIFIC = 0xFF;
}  // namespace HCI
}  // namespace BT
}  // namespace HDI
}  // namespace OHOS
#endif /* BT_HAL_HCI_INTERNAL_H */