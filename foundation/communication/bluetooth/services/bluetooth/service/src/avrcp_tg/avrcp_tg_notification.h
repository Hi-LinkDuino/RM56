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

#ifndef AVRCP_TG_NOTIFICATION_H
#define AVRCP_TG_NOTIFICATION_H

#include<deque>
#include "avrcp_tg_packet.h"
#include "avrcp_tg_vendor.h"
#include"packet.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the values of the "EVENT ID".
 * @see Audio/Video Remote Control 1.6.2 Section 28 Appendix H: list of defined notification events.
 */
enum AvrcTgEventId : uint32_t {
    AVRC_TG_EVENT_ID_PLAYBACK_STATUS_CHANGED = 0x01,  // Change in playback status of the current track.
    AVRC_TG_EVENT_ID_TRACK_CHANGED,                   // Change of current track.
    AVRC_TG_EVENT_ID_TRACK_REACHED_END,               // Reached end of a track.
    AVRC_TG_EVENT_ID_TRACK_REACHED_START,             // Reached start of a track.
    // Change in playback position. Returned after the specified playback notification change notification interval.
    AVRC_TG_EVENT_ID_PLAYBACK_POS_CHANGED,
    AVRC_TG_EVENT_ID_BATT_STATUS_CHANGED,                 // Change in battery status.
    AVRC_TG_EVENT_ID_SYSTEM_STATUS_CHANGED,               // Change in system status.
    AVRC_TG_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED,  // Change in player application setting.
    AVRC_TG_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED,         // The content of the Now Playing list has changed.
    AVRC_TG_EVENT_ID_AVAILABLE_PLAYERS_CHANGED,           // The available players have changed
    AVRC_TG_EVENT_ID_ADDRESSED_PLAYER_CHANGED,            // The Addressed Player has been changed.
    AVRC_TG_EVENT_ID_UIDS_CHANGED,                        // The UIDs have changed.
    AVRC_TG_EVENT_ID_VOLUME_CHANGED,                      // The volume has been changed locally on the TG.
    AVRC_TG_EVENT_ID_RESERVED = 0x0E,
};

/**
 * @brief This enumeration declares the values of the <b>NOTIFICATION</b> commands.
 */
enum AvrcTgNotify {
    AVRC_TG_NOTIFY_NUM_OF_PACKETS = 0x01,                 // The number of the packets.
    AVRC_TG_NOTIFY_PARAMETER_LENGTH = 0x0000,             // The length of the "Parameter Length".
    AVRC_TG_NOTIFY_PLAYER_ID = 0xFFFF,                    // The value of the "Player Id".
    AVRC_TG_NOTIFY_PLAYBACK_POSITION = 0xFFFFFFFF,        // The value of the "Playback position".
    AVRC_TG_NOTIFY_EVENT_ID_OFFSET = 0x0A,                // The offset of the "Event ID".
    AVRC_TG_NOTIFY_EVENT_ID_SIZE = 0x01,                  // The size of the "Event ID".
    AVRC_TG_NOTIFY_EVENT_ID_PLAYBACK_STATUS_SIZE = 0x01,  // The size of the "PlayStatus".
    AVRC_TG_NOTIFY_EVENT_UID_SIZE = 0x08,                 // The size of the "Identifier".
    AVRC_TG_NOTIFY_EVENT_PLAYBACK_POSITION_SIZE = 0x04,   // The size of the "Playback position".
    AVRC_TG_NOTIFY_EVENT_PLAYER_ID_SIZE = 0x02,           // The size of the "Player Id".
    AVRC_TG_NOTIFY_EVENT_UID_COUNTER_SIZE = 0x02,         // The size of the "UID Counter".
    AVRC_TG_NOTIFY_EVENT_ID_VOLUME_SIZE = 0x01,           // The size of the "Absolute Volume".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the <b>NOTIFICATION</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.7.2 RegisterNotification.
 */
class AvrcTgNotifyPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtNotifyPacket</b> instance.
     */
    AvrcTgNotifyPacket(uint8_t eventId, uint8_t crCode, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcCtNotifyPacket</b> instance.
     */
    AvrcTgNotifyPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcCtNotifyPacket</b> instance.
     */
    virtual ~AvrcTgNotifyPacket();

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
     * @param[in] pkt The frame packet.
     * @return The frame packet.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
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
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the number of the packets.
     *
     * @return The number of the packets.
     */
    uint16_t GetNumberOfPackets(void) override;

    /**
     * @brief Gets the maximum number of values for the "PlayerApplicationSettingValueID".
     *
     * @return The maximum number of values for the "PlayerApplicationSettingValueID".
     */
    uint16_t GetAvailableParameterSize(void) const;

    /**
     * @brief Initializes for <b>EVENT_PLAYBACK_STATUS_CHANGED</b>
     *
     * @param[in] playStatus Indicates the current status of playback.
     */
    void InitPlaybackStatusChanged(uint8_t playStatus)
    {
        playStatus_ = playStatus;
    }

    /**
     * @brief Initializes for <b>EVENT_TRACK_CHANGED</b>
     *
     * @param[in] Identifier Unique Identifier to identify an element on TG, as is used for GetElementAttribute command
     * in case Browsing is not supported and GetItemAttribute command in case Browsing is supported.
     */
    void InitTrackChanged(uint64_t identifier)
    {
        uid_ = identifier;
    }

    /**
     * @brief Initializes for <b>EVENT_PLAYBACK_POS_CHANGED</b>
     *
     * @param[in] playbackPos Current playback position in millisecond.
     */
    void InitPlaybackPosChanged(uint32_t playbackPos)
    {
        playbackPos_ = playbackPos;
    }

    /**
     * @brief Initializes for <b>EVENT_PLAYER_APPLICATION_SETTING_CHANGED</b>
     *
     * @param[in] attributes Player application setting attribute ID for which the value is returned.
     * @param[in] values     Currently set Player Application Setting value on the TG for the above attributes.
     */
    void InitPlayerApplicationSettingChanged(const std::deque<uint8_t> &attributes, const std::deque<uint8_t> &values)
    {
        attributes_ = attributes;
        values_ = values;
    }

    /**
     * @brief Initializes for <b>EVENT_ADDRESSED_PLAYER_CHANGED</b>
     *
     * @param[in] playerId   The player id.
     * @param[in] uidCounter The uid counter.
     */
    void InitAddressedPlayerChanged(uint16_t playerId, uint16_t uidCounter)
    {
        playerId_ = playerId;
        uidCounter_ = uidCounter;
    }

    /**
     * @brief Initializes for <b>EVENT_UIDS_CHANGED</b>
     *
     * @param[in] uidCounter The uid counter.
     */
    void InitUidsChanged(uint16_t uidCounter)
    {
        uidCounter_ = uidCounter;
    }

    /**
     * @brief Initializes for <b>EVENT_VOLUME_CHANGED</b>
     *
     * @param[in] volume The absolute volume.
     */
    void InitVolumeChanged(uint8_t volume)
    {
        volume_ = volume;
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
     * @brief Gets the "Playback interval".
     *
     * @return The value of the "Playback interval".
     */
    uint8_t GetPlaybackInterval(void) const
    {
        return interval_;
    }

    /**
     * @brief Checks the event id is valid or not.
     *
     * @return The result of the method execution.
     * @retval true  The event id is valid.
     * @retval false The event id is invalid.
     */
    bool IsValidEventId(void) const;

    /**
     * @brief Checks the event id is supported or not.
     *
     * @return The result of the method execution.
     * @retval true  The event id is supported.
     * @retval false The event id is unsupported.
     */
    bool IsSupportedEventId(void) const;

private:
    uint8_t eventId_ {AVRC_TG_EVENT_ID_RESERVED};              // The value of the "EventID".
    uint32_t interval_ {AVRC_PLAYBACK_INTERVAL_1_SEC};         // The value of the "Playback interval".
    uint8_t playStatus_ {AVRC_PLAY_STATUS_ERROR};              // The value of the "PlayStatus".
    uint64_t uid_ {AVRC_TG_VENDOR_UID};                        // The value of the "Identifier".
    uint32_t playbackPos_ {AVRC_TG_NOTIFY_PLAYBACK_POSITION};  // The value of the "Playback position".
    std::deque<uint8_t> attributes_ {};
    // The value of the "PlayerApplicationSettingAttributeID".
    std::deque<uint8_t> values_ {};                            // The value of the "PlayerApplicationSettingValueID".
    uint16_t playerId_ {AVRC_TG_NOTIFY_PLAYER_ID};             // The value of the "Player Id".
    uint16_t uidCounter_ {AVRC_TG_VENDOR_UID_COUNTER};         // The value of the "UID Counter".
    uint8_t volume_ {AVRC_ABSOLUTE_VOLUME_INVALID};            // The value of the "Absolute Volume".

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     EventID                                             1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayStatus                                          1 octets<br>
     * @param[in] pkt The frame packet.
     */
    void AssemblePlaybackStatusChanged(Packet *pkt);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     EventID                                             1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Identifier                                          8 octets<br>
     * @param[in] pkt The frame packet.
     */
    void AssembleTrackChanged(Packet *pkt);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     EventID                                             1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Playback position                                   4 octets<br>
     * @param[in] pkt The frame packet.
     */
    void AssemblePlaybackPosChanged(Packet *pkt);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     EventID                                             1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingAttributeID              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID 1               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID 1                   1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID n               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID n                   1 octets<br>
     * @param[in] pkt The frame packet.
     */
    void AssemblePlayerApplicationSettingChanged(Packet *pkt);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     EventID                                             1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Player Id                                           2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     * @param[in] pkt The frame packet.
     */
    void AssembleAddressedPlayerChanged(Packet *pkt);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     EventID                                             1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     * @param[in] pkt The frame packet.
     */
    void AssembleUidsChanged(Packet *pkt);

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     EventID                                             1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Absolute Volume                                     1 octets<br>
     * @param[in] pkt The frame packet.
     */
    void AssembleVolumeChanged(Packet *pkt);

    void AssembleCommonChanged(Packet *pkt);
    /**
     * @brief A constructor used to create an <b>AvrcCtNotifyPacket</b> instance.
     */
    AvrcTgNotifyPacket() = delete;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_NOTIFICATION_H