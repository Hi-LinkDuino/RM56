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

#include "avrcp_ct_notification.h"

namespace OHOS {
namespace bluetooth {
AvrcCtNotifyPacket::AvrcCtNotifyPacket(uint8_t eventId, uint32_t interval)
    : AvrcCtVendorPacket(),
      interval_(AVRC_PLAYBACK_INTERVAL_1_SEC),
      playStatus_(AVRC_PLAY_STATUS_ERROR),
      volume_(AVRC_ABSOLUTE_VOLUME_INVALID)
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_NOTIFY;
    pduId_ = AVRC_CT_PDU_ID_REGISTER_NOTIFICATION;
    parameterLength_ = AVRC_CT_NOTIFY_PARAMETER_LENGTH;
    eventId_ = eventId;
    interval_ = interval;
}

AvrcCtNotifyPacket::AvrcCtNotifyPacket(Packet *pkt)
    : AvrcCtVendorPacket(),
      interval_(AVRC_PLAYBACK_INTERVAL_1_SEC),
      playStatus_(AVRC_PLAY_STATUS_ERROR),
      volume_(AVRC_ABSOLUTE_VOLUME_INVALID)
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_NOTIFY;
    pduId_ = AVRC_CT_PDU_ID_REGISTER_NOTIFICATION;

    DisassemblePacket(pkt);
}

AvrcCtNotifyPacket::~AvrcCtNotifyPacket()
{
    HILOGI("enter");

    attributes_.clear();
    values_.clear();
}

Packet *AvrcCtNotifyPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_NOTIFY_PARAMETER_LENGTH;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PushOctets1((bufferPtr + offset), eventId_);
    HILOGI("eventId_: %{public}x", eventId_);

    PushOctets4((bufferPtr + offset), interval_);
    HILOGI("interval_: %{public}d", interval_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);

    return pkt;
}

bool AvrcCtNotifyPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    payload = 0x00;
    PopOctets1((buffer + offset), payload);
    eventId_ = static_cast<uint8_t>(payload);
    HILOGI("eventId_: %{public}x", eventId_);

    switch (eventId_) {
        case AVRC_CT_EVENT_ID_PLAYBACK_STATUS_CHANGED:
            isValid_ = DisassemblePlaybackStatus(buffer);
            break;
        case AVRC_CT_EVENT_ID_TRACK_CHANGED:
            isValid_ = DisassembleTrackChanged(buffer);
            break;
        case AVRC_CT_EVENT_ID_PLAYBACK_POS_CHANGED:
            isValid_ = DisassemblePlaybackPosChanged(buffer);
            break;
        case AVRC_CT_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED:
            isValid_ = DisassemblePlayerApplicationSettingChanged(buffer);
            break;
        case AVRC_CT_EVENT_ID_ADDRESSED_PLAYER_CHANGED:
            isValid_ = DisassembleAddressedPlayerChanged(buffer);
            break;
        case AVRC_CT_EVENT_ID_UIDS_CHANGED:
            isValid_ = DisassembleUidsChanged(buffer);
            break;
        case AVRC_CT_EVENT_ID_VOLUME_CHANGED:
            isValid_ = DisassembleVolumeChanged(buffer);
            break;
        case AVRC_CT_EVENT_ID_TRACK_REACHED_END:
        case AVRC_CT_EVENT_ID_TRACK_REACHED_START:
        case AVRC_CT_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED:
        case AVRC_CT_EVENT_ID_AVAILABLE_PLAYERS_CHANGED:
            /// FALL THROUGH
        default:
            /// Do nothing!
            isValid_ = true;
            break;
    }

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

bool AvrcCtNotifyPacket::DisassemblePlaybackStatus(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_NOTIFY_EVENT_ID_OFFSET + AVRC_CT_NOTIFY_EVENT_ID_SIZE;
    uint64_t payload = 0x00;
    PopOctets1((buffer + offset), payload);
    playStatus_ = static_cast<uint8_t>(payload);
    HILOGI("playStatus_: %{public}x", playStatus_);

    isValid_ = true;

    return isValid_;
}

bool AvrcCtNotifyPacket::DisassembleTrackChanged(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_NOTIFY_EVENT_ID_OFFSET + AVRC_CT_NOTIFY_EVENT_ID_SIZE;
    uint64_t payload = 0x00;
    PopOctets8((buffer + offset), payload);
    uid_ = payload;
    HILOGI("uid_: %{public}jx", uid_);

    isValid_ = true;

    return isValid_;
}

bool AvrcCtNotifyPacket::DisassemblePlaybackPosChanged(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_NOTIFY_EVENT_ID_OFFSET + AVRC_CT_NOTIFY_EVENT_ID_SIZE;
    uint64_t payload = 0x00;
    PopOctets4((buffer + offset), payload);
    playbackPos_ = static_cast<uint32_t>(payload);
    HILOGI("playbackPos_: %{public}d", playbackPos_);

    isValid_ = true;

    return isValid_;
}

bool AvrcCtNotifyPacket::DisassemblePlayerApplicationSettingChanged(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    receivedFragments_++;
    HILOGI("receivedFragments_: %{public}d", receivedFragments_);

    uint16_t offset = AVRC_CT_NOTIFY_EVENT_ID_OFFSET + AVRC_CT_NOTIFY_EVENT_ID_SIZE;
    uint64_t payload = 0x00;
    offset += PopOctets1((buffer + offset), payload);
    auto numOfAttributes = static_cast<uint8_t>(payload);
    HILOGI("numOfAttributes: %{public}d", numOfAttributes);

    for (int i = 0; i < numOfAttributes; i++) {
        payload = 0x00;
        offset += PopOctets1((buffer + offset), payload);
        attributes_.push_back(static_cast<uint8_t>(payload));
        HILOGI("attribute: %{public}x", attributes_.back());

        offset += PopOctets1((buffer + offset), payload);
        values_.push_back(static_cast<uint8_t>(payload));
        HILOGI("value: %{public}x", values_.back());
    }

    return isValid_;
}

bool AvrcCtNotifyPacket::DisassembleAddressedPlayerChanged(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_NOTIFY_EVENT_ID_OFFSET + AVRC_CT_NOTIFY_EVENT_ID_SIZE;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    playerId_ = static_cast<uint16_t>(payload);
    HILOGI("playerId_: %{public}x", playerId_);

    payload = 0x00;
    PopOctets2((buffer + offset), payload);
    uidCounter_ = static_cast<uint16_t>(payload);
    HILOGI("uidCounter_: %{public}x", uidCounter_);

    isValid_ = true;

    return isValid_;
}

bool AvrcCtNotifyPacket::DisassembleUidsChanged(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_NOTIFY_EVENT_ID_OFFSET + AVRC_CT_NOTIFY_EVENT_ID_SIZE;
    uint64_t payload = 0x00;
    PopOctets2((buffer + offset), payload);
    uidCounter_ = static_cast<uint16_t>(payload);
    HILOGI("uidCounter_: %{public}x", uidCounter_);

    isValid_ = true;

    return isValid_;
}

bool AvrcCtNotifyPacket::DisassembleVolumeChanged(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_NOTIFY_EVENT_ID_OFFSET + AVRC_CT_NOTIFY_EVENT_ID_SIZE;
    uint64_t payload = 0x00;
    PopOctets1((buffer + offset), payload);
    volume_ = static_cast<uint8_t>(payload) & 0b01111111;
    HILOGI("volume_: %{public}d", volume_);

    isValid_ = true;

    return isValid_;
}
}  // namespace bluetooth
}  // namespace OHOS