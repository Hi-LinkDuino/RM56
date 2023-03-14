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

#include "avrcp_tg_sub_unit_info.h"

namespace OHOS {
namespace bluetooth {
AvrcTgSubUnitPacket::AvrcTgSubUnitPacket(void) : AvrcTgUnitPacket()
{
    HILOGI("enter");

    subunitType_ = AVRC_TG_SUB_UNIT_SUBUNIT_TYPE_UNIT;
    subunitId_ = AVRC_TG_SUB_UNIT_SUBUNIT_ID_IGNORE;
    opCode_ = AVRC_TG_OP_CODE_SUB_UNIT_INFO;
}

AvrcTgSubUnitPacket::AvrcTgSubUnitPacket(Packet *pkt, uint8_t label) : AvrcTgUnitPacket()
{
    HILOGI("label:%{public}d", label);

    subunitType_ = AVRC_TG_SUB_UNIT_SUBUNIT_TYPE_UNIT;
    subunitId_ = AVRC_TG_SUB_UNIT_SUBUNIT_ID_IGNORE;
    opCode_ = AVRC_TG_OP_CODE_SUB_UNIT_INFO;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgSubUnitPacket::~AvrcTgSubUnitPacket(void)
{
    HILOGI("enter");

    if (pkt_ != nullptr) {
        PacketFree(pkt_);
        pkt_ = nullptr;
    }
}

const Packet *AvrcTgSubUnitPacket::AssemblePacket(void)
{
    HILOGI("enter");

    pkt_ = PacketMalloc(0x00, 0x00, AVRC_TG_SUB_UNIT_RESPONSE_SIZE);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x00;
    offset += PushOctets1((buffer + offset), crCode_);
    offset += PushOctets1((buffer + offset), (subunitType_ << AVRC_TG_OFFSET_THREE_BITS) | subunitId_);
    offset += PushOctets1((buffer + offset), opCode_);
    offset += PushOctets1(
        (buffer + offset), ((page_ & 0b00000111) << AVRC_TG_OFFSET_FOUR_BITS) | (extentionCode_ & 0b00000111));
    offset += PushOctets1((buffer + offset), AVRC_TG_SUB_UNIT_OCTET_4);
    offset += PushOctets1((buffer + offset), AVRC_TG_SUB_UNIT_OCTET_4);
    offset += PushOctets1((buffer + offset), AVRC_TG_SUB_UNIT_OCTET_4);
    PushOctets1((buffer + offset), AVRC_TG_SUB_UNIT_OCTET_4);

    return pkt_;
}

bool AvrcTgSubUnitPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    bool isValid = false;
    size_t size = PacketPayloadSize(pkt);
    if (size >= AVRC_TG_SUB_UNIT_COMMAND_SIZE) {
        isValid = true;
    } else {
        crCode_ = AVRC_TG_RSP_CODE_REJECTED;
        HILOGI("The size of the packet is invalid! actual size: %{public}zu valid min size: %{public}u",
            size, AVRC_TG_SUB_UNIT_COMMAND_SIZE);
    }

    return isValid;
}
}  // namespace bluetooth
}  // namespace OHOS
