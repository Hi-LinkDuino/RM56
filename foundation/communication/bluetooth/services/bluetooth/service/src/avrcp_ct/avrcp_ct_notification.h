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

#ifndef AVRCP_CT_NOTIFICATION_H
#define AVRCP_CT_NOTIFICATION_H

#include "avrcp_ct_packet.h"
#include "avrcp_ct_vendor.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the values of the "EVENT ID".
 * @see Audio/Video Remote Control 1.6.2 Section 28 Appendix H: list of defined notification events.
 */
enum AvrcCtEventId : uint32_t {
    AVRC_CT_EVENT_ID_PLAYBACK_STATUS_CHANGED = 0x01,  // Change in playback status of the current track.
    AVRC_CT_EVENT_ID_TRACK_CHANGED,                   // Change of current track.
    AVRC_CT_EVENT_ID_TRACK_REACHED_END,               // Reached end of a track.
    AVRC_CT_EVENT_ID_TRACK_REACHED_START,             // Reached start of a track.
    // Change in playback position. Returned after the specified playback notification change notification interval.
    AVRC_CT_EVENT_ID_PLAYBACK_POS_CHANGED,
    AVRC_CT_EVENT_ID_BATT_STATUS_CHANGED,                 // Change in battery status.
    AVRC_CT_EVENT_ID_SYSTEM_STATUS_CHANGED,               // Change in system status.
    AVRC_CT_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED,  // Change in player application setting.
    AVRC_CT_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED,         // The content of the Now Playing list has changed.
    AVRC_CT_EVENT_ID_AVAILABLE_PLAYERS_CHANGED,           // The available players have changed
    AVRC_CT_EVENT_ID_ADDRESSED_PLAYER_CHANGED,            // The Addressed Player has been changed.
    AVRC_CT_EVENT_ID_UIDS_CHANGED,                        // The UIDs have changed.
    AVRC_CT_EVENT_ID_VOLUME_CHANGED,                      // The volume has been changed locally on the TG.
    AVRC_CT_EVENT_ID_RESERVED = 0x0E,
};

/**
 * @brief This enumeration declares the values of the <b>NOTIFICATION</b> commands.
 */
enum AvrcCtNotify {
    AVRC_CT_NOTIFY_PARAMETER_LENGTH = 0x0005,       // The length of the "Parameter Length".
    AVRC_CT_NOTIFY_PLAYER_ID = 0xFFFF,              // The value of the "Player Id".
    AVRC_CT_NOTIFY_PLAYBACK_POSITION = 0xFFFFFFFF,  // The value of the "Playback position".
    AVRC_CT_NOTIFY_EVENT_ID_OFFSET = 0x0A,          // The offset of the "Event ID".
    AVRC_CT_NOTIFY_EVENT_ID_SIZE = 0x01,            // The size of the "Event ID".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the <b>NOTIFICATION</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.7.2 RegisterNotification.
 */
class AvrcCtNotifyPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtNotifyPacket</b> instance.
     */
    explicit AvrcCtNotifyPacket(uint8_t eventId, uint32_t interval = AVRC_PLAYBACK_INTERVAL_1_SEC);

    /**
     * @brief A constructor used to create an <b>AvrcCtNotifyPacket</b> instance.
     */
    explicit AvrcCtNotifyPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtNotifyPacket</b> instance.
     */
    virtual ~AvrcCtNotifyPacket();

    /**
     * @brief Assembles the operands behind the "Packet Type" of the frame.
     *
     * @details Command frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     ctype                                               4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     EventID                                             1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Playback interval                                   4 octets<br>
     * @param[in] pkt The frame packet.
     * @return The frame packet.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     EventID                                             1 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the "Packet Type".
     *
     * @return The value of the "Packet Type".
     */
    uint8_t GetPacketType(void) const
    {
        return packetType_;
    }

    /**
     * @brief Clears the "PlayerApplicationSettingAttributeID".
     *
     * @return The values of the "PlayerApplicationSettingValueID".
     */
    void ClearAttributes(void)
    {
        attributes_.clear();
    }

    /**
     * @brief Gets the "EventID".
     *
     * @return The value of the "EventID".
     */
    uint8_t GetEventId(void) const
    {
        return eventId_;
    }

    /**
     * @brief Gets the "PlayStatus".
     *
     * @return The value of the "PlayStatus".
     */
    uint8_t GetPlayStatus(void) const
    {
        return playStatus_;
    }

    /**
     * @brief Gets the "Identifier".
     *
     * @return The value of the "Identifier".
     */
    uint64_t GetUid(void) const
    {
        return uid_;
    }

    /**
     * @brief Gets the "Playback position".
     *
     * @return The value of the "Playback position".
     */
    uint32_t GetPlaybackPosition(void) const
    {
        return playbackPos_;
    }

    /**
     * @brief Gets the "PlayerApplicationSettingAttributeID".
     *
     * @return The value of the "PlayerApplicationSettingAttributeID".
     */
    const std::vector<uint8_t> &GetAttributes(void) const
    {
        return attributes_;
    }

    /**
     * @brief Gets the "PlayerApplicationSettingValueID".
     *
     * @return The value of the "PlayerApplicationSettingValueID".
     */
    const std::vector<uint8_t> &GetValues(void) const
    {
        return values_;
    }

    /**
     * @brief Gets the "Player Id".
     *
     * @return The value of the "Player Id".
     */
    uint16_t GetPlayerId(void) const
    {
        return playerId_;
    }

    /**
     * @brief Gets the "UID Counter".
     *
     * @return The value of the "UID Counter".
     */
    uint16_t GetUidCounter(void) const
    {
        return uidCounter_;
    }

    /**
     * @brief Gets the absolute volume.
     *
     * @return The value of the "Absolute Volume".
     */
    uint8_t GetAbsoluteVolume(void) const
    {
        return volume_;
    }

private:
    uint8_t eventId_ {AVRC_CT_EVENT_ID_RESERVED};              // The value of the "EventID".
    uint32_t interval_ {AVRC_PLAYBACK_INTERVAL_1_SEC};         // The value of the "Playback interval".
    uint8_t playStatus_ {AVRC_PLAY_STATUS_ERROR};              // The value of the "PlayStatus".
    uint64_t uid_ {AVRC_CT_VENDOR_UID};                        // The value of the "Identifier".
    uint32_t playbackPos_ {AVRC_CT_NOTIFY_PLAYBACK_POSITION};  // The value of the "Playback position".
    std::vector<uint8_t> attributes_ {};
    // The value of the "PlayerApplicationSettingAttributeID".
    std::vector<uint8_t> values_ {};                           // The value of the "PlayerApplicationSettingValueID".
    uint16_t playerId_ {AVRC_CT_NOTIFY_PLAYER_ID};             // The value of the "Player Id".
    uint16_t uidCounter_ {AVRC_CT_VENDOR_UID_COUNTER};         // The value of the "UID Counter".
    uint8_t volume_ {AVRC_ABSOLUTE_VOLUME_INVALID};            // The value of the "Absolute Volume".

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     PlayStatus                                          1 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePlaybackStatus(uint8_t *buffer);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Identifier                                          8 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleTrackChanged(uint8_t *buffer);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Playback position                                   4 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePlaybackPosChanged(uint8_t *buffer);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingAttributeID              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID 1               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID 1                   1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID n               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID n                   1 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePlayerApplicationSettingChanged(uint8_t *buffer);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Player Id                                           2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleAddressedPlayerChanged(uint8_t *buffer);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleUidsChanged(uint8_t *buffer);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Absolute Volume                                     1 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleVolumeChanged(uint8_t *buffer);

    /**
     * @brief A constructor used to create an <b>AvrcCtNotifyPacket</b> instance.
     */
    AvrcCtNotifyPacket() = delete;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_CT_NOTIFICATION_H