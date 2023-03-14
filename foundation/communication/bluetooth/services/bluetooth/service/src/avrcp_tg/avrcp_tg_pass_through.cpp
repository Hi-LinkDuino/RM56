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

#include "avrcp_tg_pass_through.h"

namespace OHOS {
namespace bluetooth {
AvrcTgPassPacket::AvrcTgPassPacket()
    : stateFlag_(AVRC_KEY_STATE_INVALID), operationId_(AVRC_KEY_OPERATION_INVALID), label_(AVRC_DEFAULT_LABEL)
{
    HILOGI("enter");
}

AvrcTgPassPacket::AvrcTgPassPacket(uint8_t oper, uint8_t state, uint8_t label)
    : stateFlag_(AVRC_KEY_STATE_INVALID), operationId_(AVRC_KEY_OPERATION_INVALID), label_(AVRC_DEFAULT_LABEL)
{
    HILOGI("oper: %{public}d, state: %{public}d, label: %{public}d", oper, state, label);

    operationId_ = oper;
    stateFlag_ = state;
    label_ = label;
}

AvrcTgPassPacket::AvrcTgPassPacket(Packet *pkt, uint8_t label)
    : stateFlag_(AVRC_KEY_STATE_INVALID), operationId_(AVRC_KEY_OPERATION_INVALID), label_(AVRC_DEFAULT_LABEL)
{
    HILOGI("label: %{public}d", label);

    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgPassPacket::~AvrcTgPassPacket(void)
{
    HILOGI("enter");

    if (pkt_ != nullptr) {
        PacketFree(pkt_);
        pkt_ = nullptr;
    }
}

const Packet *AvrcTgPassPacket::AssemblePacket(void)
{
    HILOGI("enter");

    pkt_ = PacketMalloc(0x00, 0x00, AVRC_TG_PASS_RESPONSE_SIZE);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x00;
    offset += PushOctets1((buffer + offset), crCode_);
    offset += PushOctets1((buffer + offset), (subunitType_ << AVRC_TG_OFFSET_THREE_BITS) | subunitId_);
    offset += PushOctets1((buffer + offset), opCode_);
    offset += PushOctets1((buffer + offset), (stateFlag_ << AVRC_TG_OFFSET_SEVEN_BITS) | operationId_);
    PushOctets1((buffer + offset), operationDataFieldLength_);

    return pkt_;
}

bool AvrcTgPassPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    isValid_ = false;
    size_t size = PacketPayloadSize(pkt);
    if (size >= AVRC_TG_PASS_COMMAND_SIZE) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

        // Only gets the company ID, because other operands are the default values.
        uint16_t offset = AVRC_TG_PASS_OPERATION_ID_OFFSET;
        uint64_t payload = 0x00;
        PopOctets1((buffer + offset), payload);
        stateFlag_ = (static_cast<uint8_t>(payload) >> AVRC_TG_OFFSET_SEVEN_BITS);
        operationId_ = (static_cast<uint8_t>(payload) & 0b01111111);

        if (!IsSupportedKeyOperation()) {
            crCode_ = AVRC_TG_RSP_CODE_NOT_IMPLEMENTED;
        } else if (!IsValidKeyState()) {
            crCode_ = AVRC_TG_RSP_CODE_REJECTED;
        } else {
            isValid_ = true;
            crCode_ = AVRC_TG_RSP_CODE_ACCEPTED;
        }
    } else {
        crCode_ = AVRC_TG_RSP_CODE_REJECTED;
        HILOGI("The size of the packet is invalid! - actual size: %{public}zu valid min size: %{public}u",
            size, AVRC_TG_PASS_COMMAND_SIZE);
    }

    return isValid_;
}

bool AvrcTgPassPacket::IsSupportedKeyOperation(void)
{
    HILOGI("enter");

    return IsSupportedKeyOperation(operationId_);
}

bool AvrcTgPassPacket::IsValidKeyState(void)
{
    HILOGI("enter");

    return IsValidKeyState(stateFlag_);
}

bool AvrcTgPassPacket::IsSupportedKeyOperation(uint8_t key)
{
    HILOGI("key: %{public}d", key);

    bool rtnSts = true;

    switch (key) {
        case AVRC_KEY_OPERATION_VOLUME_UP:
        case AVRC_KEY_OPERATION_VOLUME_DOWN:
        case AVRC_KEY_OPERATION_MUTE:
        case AVRC_KEY_OPERATION_PLAY:
        case AVRC_KEY_OPERATION_STOP:
        case AVRC_KEY_OPERATION_PAUSE:
        case AVRC_KEY_OPERATION_REWIND:
        case AVRC_KEY_OPERATION_FAST_FORWARD:
        case AVRC_KEY_OPERATION_FORWARD:
        case AVRC_KEY_OPERATION_BACKWARD:
            break;
        default:
            rtnSts = false;
    }

    return rtnSts;
}

bool AvrcTgPassPacket::IsValidKeyState(uint8_t state)
{
    HILOGI("state: %{public}d", state);

    bool rtnSts = true;

    switch (state) {
        case AVRC_KEY_STATE_PRESS:
        case AVRC_KEY_STATE_RELEASE:
            break;
        default:
            rtnSts = false;
    }

    return rtnSts;
}
}  // namespace bluetooth
}  // namespace OHOS
