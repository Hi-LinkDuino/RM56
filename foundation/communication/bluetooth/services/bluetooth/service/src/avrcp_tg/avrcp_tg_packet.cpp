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

#include "avrcp_tg_packet.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
uint16_t AvrcTgPacket::PushOctets1(uint8_t *pkt, uint8_t payload)
{
    HILOGI("payload:%{public}d", payload);

    return PushOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_TG_OFFSET_ONE_BIT, payload);
}

uint16_t AvrcTgPacket::PushOctets2(uint8_t *pkt, uint16_t payload)
{
    HILOGI("payload:%{public}d", payload);

    return PushOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_TG_OFFSET_TWO_BITS, payload);
}

uint16_t AvrcTgPacket::PushOctets3(uint8_t *pkt, uint32_t payload)
{
    HILOGI("payload:%{public}u", payload);

    return PushOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_TG_OFFSET_THREE_BITS, payload);
}

uint16_t AvrcTgPacket::PushOctets4(uint8_t *pkt, uint32_t payload)
{
    HILOGI("payload:%{public}u", payload);

    return PushOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_TG_OFFSET_FOUR_BITS, payload);
}

uint16_t AvrcTgPacket::PushOctets8(uint8_t *pkt, uint64_t payload)
{
    HILOGI("payload:%{public}llu", static_cast<unsigned long long>(payload));

    return PushOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_TG_OFFSET_EIGHT_BITS, payload);
}

uint16_t AvrcTgPacket::PushOctets(uint8_t *pkt, uint8_t *playload, uint16_t length)
{
    HILOGI("length:%{public}d", length);

    (void)memcpy_s(pkt, length, playload, length);

    return length;
}

uint16_t AvrcTgPacket::PopOctets1(uint8_t *pkt, uint64_t &payload)
{
    HILOGI("enter");

    uint64_t tmpPayload = 0x00;
    uint16_t octets = PopOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_TG_OFFSET_ONE_BIT, tmpPayload);
    payload = tmpPayload;

    return octets;
}

uint16_t AvrcTgPacket::PopOctets2(uint8_t *pkt, uint64_t &payload)
{
    HILOGI("enter");

    uint64_t tmpPayload = 0x00;
    uint16_t octets = PopOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_TG_OFFSET_TWO_BITS, tmpPayload);
    payload = tmpPayload;

    return octets;
}

uint16_t AvrcTgPacket::PopOctets3(uint8_t *pkt, uint64_t &payload)
{
    HILOGI("enter");

    uint64_t tmpPayload = 0x00;
    uint16_t octets = PopOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_TG_OFFSET_THREE_BITS, tmpPayload);
    payload = tmpPayload;

    return octets;
}

uint16_t AvrcTgPacket::PopOctets4(uint8_t *pkt, uint64_t &payload)
{
    HILOGI("enter");

    uint64_t tmpPayload = 0x00;
    uint16_t octets = PopOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_TG_OFFSET_FOUR_BITS, tmpPayload);
    payload = tmpPayload;

    return octets;
}

uint16_t AvrcTgPacket::PopOctets8(uint8_t *pkt, uint64_t &payload)
{
    HILOGI("enter");

    uint64_t tmpPayload = 0x00;
    uint16_t octets = PopOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_TG_OFFSET_EIGHT_BITS, tmpPayload);
    payload = tmpPayload;

    return octets;
}

uint16_t AvrcTgPacket::PopOctets(uint8_t *pkt, uint8_t *playload, uint16_t length)
{
    HILOGI("length:%{public}d", length);

    (void)memcpy_s(playload, length, pkt, length);

    return length;
}

uint8_t AvrcTgPacket::GetOpCode(Packet *pkt)
{
    HILOGI("enter");

    uint8_t opCode = AVRC_TG_OP_CODE_INVALID;
    if (PacketPayloadRead(pkt, &opCode, AVRC_TG_AVC_COMMON_OPCODE_OFFSET, 1) != 1) {
        opCode = AVRC_TG_OP_CODE_INVALID;
    }

    return opCode;
}

uint8_t AvrcTgPacket::GetVendorPdu(Packet *pkt)
{
    HILOGI("enter");

    uint8_t pduId = AVRC_TG_PDU_ID_INVALID;
    if (PacketPayloadRead(pkt, &pduId, AVRC_TG_AVC_COMMON_VENDOR_PDU_ID_OFFSET, 1) != 1) {
        pduId = AVRC_TG_PDU_ID_INVALID;
    }

    return pduId;
}

uint8_t AvrcTgPacket::GetBrowsePdu(Packet *pkt)
{
    HILOGI("enter");

    uint8_t pduId = AVRC_TG_PDU_ID_INVALID;
    if (PacketPayloadRead(pkt, &pduId, AVRC_TG_AVC_COMMON_BROWSE_PDU_ID_OFFSET, 1) != 1) {
        pduId = AVRC_TG_PDU_ID_INVALID;
    }

    return pduId;
}

bool AvrcTgPacket::IsValidOpCode(uint8_t code)
{
    HILOGI("code:%{public}d", code);

    bool rtnSts = true;

    switch (code) {
        case AVRC_TG_OP_CODE_UNIT_INFO:
        case AVRC_TG_OP_CODE_SUB_UNIT_INFO:
        case AVRC_TG_OP_CODE_PASS_THROUGH:
        case AVRC_TG_OP_CODE_VENDOR:
            break;
        default:
            rtnSts = false;
    }

    return rtnSts;
}

uint16_t AvrcTgPacket::PushOctetsUpto8(uint8_t *pkt, size_t size, uint64_t payload)
{
    HILOGI("payload:%{public}llu", static_cast<unsigned long long>(payload));

    if (size <= sizeof(uint64_t)) {
        for (size_t i = 0; i < size; i++) {
            pkt[i] = payload >> ((sizeof(uint64_t) * (size - (i + 1))) & 0xFF);
        }
    }

    return static_cast<uint16_t>(size);
}

uint16_t AvrcTgPacket::PopOctetsUpto8(uint8_t *pkt, size_t size, uint64_t &payload)
{
    HILOGI("enter");

    if (size <= sizeof(uint64_t)) {
        for (size_t i = 0; i < size; i++) {
            uint64_t tmpPayload = pkt[i] & 0xFF;
            payload |= (tmpPayload << (sizeof(uint64_t) * (size - (i + 1))));
        }
    }

    return static_cast<uint16_t>(size);
}
}  // namespace bluetooth
}  // namespace OHOS
