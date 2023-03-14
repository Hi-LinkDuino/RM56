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

/**
 * @file avrcp_tg_unit_info.cpp
 *
 * @brief Defines the class of the UNIT INFO command, including attributes and methods.
 */

#include "avrcp_tg_unit_info.h"

namespace OHOS {
namespace bluetooth {
AvrcTgUnitPacket::AvrcTgUnitPacket() : label_(AVRC_DEFAULT_LABEL)
{
    HILOGI("enter");
}

AvrcTgUnitPacket::AvrcTgUnitPacket(Packet *pkt, uint8_t label) : label_(label)
{
    HILOGI("label:%{public}d", label);

    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgUnitPacket::~AvrcTgUnitPacket(void)
{
    HILOGI("enter");

    if (pkt_ != nullptr) {
        PacketFree(pkt_);
        pkt_ = nullptr;
    }
}

const Packet *AvrcTgUnitPacket::AssemblePacket(void)
{
    HILOGI("enter");

    pkt_ = PacketMalloc(0x00, 0x00, AVRC_TG_UNIT_RESPONSE_SIZE);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));
    ASSERT(buffer != nullptr);
    uint16_t offset = 0x00;
    offset += PushOctets1((buffer + offset), crCode_);
    offset += PushOctets1((buffer + offset), (subunitType_ << AVRC_TG_OFFSET_THREE_BITS) | subunitId_);
    offset += PushOctets1((buffer + offset), opCode_);
    offset += PushOctets1((buffer + offset), AVRC_TG_UNIT_OCTET_3);
    offset += PushOctets1((buffer + offset), (unit_type_ << AVRC_TG_OFFSET_THREE_BITS) | unit_);
    PushOctets3((buffer + offset), companyId_);

    return pkt_;
}

bool AvrcTgUnitPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    isValid_ = false;
    size_t size = PacketPayloadSize(pkt);
    if (size >= AVRC_TG_UNIT_COMMAND_SIZE) {
        isValid_ = true;
    } else {
        crCode_ = AVRC_TG_RSP_CODE_REJECTED;
        HILOGI("The size of the packet is invalid! actual size: %{public}zu valid min size: %{public}u",
            size, AVRC_TG_UNIT_COMMAND_SIZE);
    }

    return isValid_;
}
}  // namespace bluetooth
}  // namespace OHOS
