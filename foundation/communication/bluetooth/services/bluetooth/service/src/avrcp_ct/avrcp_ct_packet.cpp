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

/**
 * @file avrcp_ct_packet.cpp
 *
 * @brief Defines the class of the basic frame packet, including attributes and methods.
 */

#include "avrcp_ct_packet.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
uint16_t AvrcCtPacket::PushOctets1(uint8_t *pkt, uint8_t payload) const
{
    HILOGI("payload: %{public}d", payload);

    return PushOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_CT_UNIT8_NUM_1, payload);
}

uint16_t AvrcCtPacket::PushOctets2(uint8_t *pkt, uint16_t payload) const
{
    HILOGI("payload: %{public}d", payload);

    return PushOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_CT_UNIT8_NUM_2, payload);
}

uint16_t AvrcCtPacket::PushOctets3(uint8_t *pkt, uint32_t payload) const
{
    HILOGI("payload: %{public}u", payload);

    return PushOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_CT_UNIT8_NUM_3, payload);
}

uint16_t AvrcCtPacket::PushOctets4(uint8_t *pkt, uint32_t payload) const
{
    HILOGI("payload: %{public}u", payload);

    return PushOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_CT_UNIT8_NUM_4, payload);
}

uint16_t AvrcCtPacket::PushOctets8(uint8_t *pkt, uint64_t payload) const
{
    HILOGI("payload: %{public}llu", static_cast<unsigned long long>(payload));

    return PushOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_CT_UNIT8_NUM_8, payload);
}

uint16_t AvrcCtPacket::PushOctets(uint8_t *pkt, uint8_t *playload, uint16_t length)
{
    HILOGI("length: %{public}d", length);

    (void)memcpy_s(pkt, length, playload, length);

    return length;
}

uint16_t AvrcCtPacket::PopOctets1(uint8_t *pkt, uint64_t &payload) const
{
    HILOGI("enter");

    uint64_t tmpPayload = 0x00;
    uint16_t octets = PopOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_CT_UNIT8_NUM_1, tmpPayload);
    payload = tmpPayload;

    return octets;
}

uint16_t AvrcCtPacket::PopOctets2(uint8_t *pkt, uint64_t &payload) const
{
    HILOGI("enter");

    uint64_t tmpPayload = 0x00;
    uint16_t octets = PopOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_CT_UNIT8_NUM_2, tmpPayload);
    payload = tmpPayload;

    return octets;
}

uint16_t AvrcCtPacket::PopOctets3(uint8_t *pkt, uint64_t &payload) const
{
    HILOGI("enter");

    uint64_t tmpPayload = 0x00;
    uint16_t octets = PopOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_CT_UNIT8_NUM_3, tmpPayload);
    payload = tmpPayload;

    return octets;
}

uint16_t AvrcCtPacket::PopOctets4(uint8_t *pkt, uint64_t &payload) const
{
    HILOGI("enter");

    uint64_t tmpPayload = 0x00;
    uint16_t octets = PopOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_CT_UNIT8_NUM_4, tmpPayload);
    payload = tmpPayload;

    return octets;
}

uint16_t AvrcCtPacket::PopOctets8(uint8_t *pkt, uint64_t &payload) const
{
    HILOGI("enter");

    uint64_t tmpPayload = 0x00;
    uint16_t octets = PopOctetsUpto8(pkt, sizeof(uint8_t) * AVRC_CT_UNIT8_NUM_8, tmpPayload);
    payload = tmpPayload;

    return octets;
}

uint16_t AvrcCtPacket::PopOctets(uint8_t *pkt, uint8_t *playload, uint16_t length)
{
    HILOGI("length: %{public}d", length);

    (void)memcpy_s(playload, length, pkt, length);

    return length;
}

uint8_t AvrcCtPacket::GetOpCode(Packet *pkt)
{
    HILOGI("enter");

    uint8_t opCode = AVRC_CT_OP_CODE_INVALID;
    if (PacketPayloadRead(pkt, &opCode, AVRC_CT_AVC_COMMON_OPCODE_OFFSET, 1) != 1) {
        opCode = AVRC_CT_OP_CODE_INVALID;
    }

    return opCode;
}

uint8_t AvrcCtPacket::GetVendorPdu(Packet *pkt)
{
    HILOGI("enter");

    uint8_t pduId = AVRC_CT_PDU_ID_INVALID;
    if (PacketPayloadRead(pkt, &pduId, AVRC_CT_AVC_COMMON_VENDOR_PDU_ID_OFFSET, 1) != 1) {
        pduId = AVRC_CT_PDU_ID_INVALID;
    }

    return pduId;
}

uint8_t AvrcCtPacket::GetBrowsePdu(Packet *pkt)
{
    HILOGI("enter");

    uint8_t pduId = AVRC_CT_PDU_ID_INVALID;
    if (PacketPayloadRead(pkt, &pduId, AVRC_CT_AVC_COMMON_BROWSE_PDU_ID_OFFSET, 1) != 1) {
        pduId = AVRC_CT_PDU_ID_INVALID;
    }

    return pduId;
}

bool AvrcCtPacket::IsValidOpCode(uint8_t code)
{
    HILOGI("code: %{public}d", code);

    bool rtnSts = true;

    switch (code) {
        case AVRC_CT_OP_CODE_UNIT_INFO:
        case AVRC_CT_OP_CODE_SUB_UNIT_INFO:
        case AVRC_CT_OP_CODE_PASS_THROUGH:
        case AVRC_CT_OP_CODE_VENDOR:
            break;
        default:
            rtnSts = false;
    }

    return rtnSts;
}

uint16_t AvrcCtPacket::PushOctetsUpto8(uint8_t *pkt, size_t size, uint64_t payload)
{
    HILOGI("payload: %{public}llu", static_cast<unsigned long long>(payload));

    if (size <= sizeof(uint64_t)) {
        for (size_t i = 0; i < size; i++) {
            pkt[i] = (payload >> (sizeof(uint64_t) * (size - (i + 1)))) & 0xFF;
        }
    }

    return static_cast<uint16_t>(size);
}

uint16_t AvrcCtPacket::PopOctetsUpto8(uint8_t *pkt, size_t size, uint64_t &payload)
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
