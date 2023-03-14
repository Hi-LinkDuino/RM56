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

#include "avrcp_ct_unit_info.h"

namespace OHOS {
namespace bluetooth {
AvrcCtUnitPacket::AvrcCtUnitPacket(void)
{
    HILOGI("enter");
}

AvrcCtUnitPacket::AvrcCtUnitPacket(Packet *pkt)
{
    HILOGI("enter");

    DisassemblePacket(pkt);
}

AvrcCtUnitPacket::~AvrcCtUnitPacket(void)
{
    HILOGI("enter");

    if (pkt_ != nullptr) {
        PacketFree(pkt_);
        pkt_ = nullptr;
    }
}

const Packet *AvrcCtUnitPacket::AssemblePacket(void)
{
    HILOGI("enter");

    pkt_ = PacketMalloc(0x00, 0x00, AVRC_CT_UNIT_COMMAND_SIZE);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x00;
    offset += PushOctets1((buffer + offset), crCode_);
    offset += PushOctets1((buffer + offset), (subunitType_ << AVRC_CT_UNIT_MOVE_BIT_3) | subunitId_);
    offset += PushOctets1((buffer + offset), opCode_);
    offset += PushOctets1((buffer + offset), AVRC_CT_UNIT_OCTET_3);
    offset += PushOctets1((buffer + offset), AVRC_CT_UNIT_OCTET_3);
    offset += PushOctets1((buffer + offset), AVRC_CT_UNIT_OCTET_3);
    offset += PushOctets1((buffer + offset), AVRC_CT_UNIT_OCTET_3);
    PushOctets1((buffer + offset), AVRC_CT_UNIT_OCTET_3);

    return pkt_;
}

bool AvrcCtUnitPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    isValid_ = false;
    size_t size = PacketPayloadSize(pkt);
    if (size >= AVRC_CT_UNIT_RESPONSE_SIZE) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

        uint16_t offset = AVRC_CT_AVC_COMMON_CTYPE_OFFSET;
        uint64_t payload = 0x00;
        PopOctets1((buffer + offset), payload);
        crCode_ = static_cast<uint8_t>(payload) & 0b00001111;

        offset = AVRC_CT_UNIT_COMPANY_ID_OFFSET;
        PopOctets3((buffer + offset), payload);
        companyId_ = static_cast<uint32_t>(payload);
        isValid_ = true;
    } else {
        crCode_ = AVRC_CT_RSP_CODE_REJECTED;
        HILOGI("The size of the packet is invalid!");
    }

    return isValid_;
}
}  // namespace bluetooth
}  // namespace OHOS
