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

#ifndef AVRCP_TG_BROWSE_H
#define AVRCP_TG_BROWSE_H

#include "avrcp_media.h"

#include "avrcp_tg_packet.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the values of the <b>BROWSING</b> commands.
 */
enum AvrcTgBrowse {
    AVRC_TG_BROWSE_MIN_SIZE = 0x03,                  // The size of the "PDU ID" + The size of the "Parameter Length".
    AVRC_TG_BROWSE_PARAMETER_LENGTH = 0x0000,        // The default value of the "Parameter Length".
    AVRC_TG_BROWSE_UID = 0x0000000000000000,         // The default value of the "UID".
    AVRC_TG_BROWSE_UID_COUNTER = 0x0000,             // The default value of the "UID Counter".
    AVRC_TG_BROWSE_FOLDER_UID = 0x0000000000000000,  // The default value of the "Folder UID".
    AVRC_TG_BROWSE_PARAMETER_LENGTH_OFFSET = 0x01,   // The offset of the "Parameter Length".
    AVRC_TG_BROWSE_PDU_ID_SIZE = 0x01,               // The size of the "PDU ID".
    AVRC_TG_BROWSE_PARAMETER_LENGTH_SIZE = 0x02,     // The size of the "Parameter Length".
    AVRC_TG_BROWSE_STATUS_SIZE = 0x01,               // The size of the "Status".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the <b>BROWSING</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 5.1.2 Browsing commands.
 */
class AvrcTgBrowsePacket : public AvrcTgPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgBrowsePacket</b> instance.
     */
    AvrcTgBrowsePacket();

    /**
     * @brief A constructor used to create an <b>AvrcTgBrowsePacket</b> instance.
     */
    AvrcTgBrowsePacket(uint8_t pduId, uint8_t status, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgBrowsePacket</b> instance.
     */
    virtual ~AvrcTgBrowsePacket();

    /**
     * @brief Assembles the frame packet.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Other operands                                      n octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the frame packet.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Other operands                                      n octets<br>
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the "PDU ID".
     *
     * @return The value of the "PDU ID".
     */
    uint8_t GetPduId(void) const
    {
        return pduId_;
    }

    /**
     * @brief Gets the label.
     *
     * @return The value of the label.
     */
    uint8_t GetLabel(void) const
    {
        return label_;
    }

    /**
     * @brief Checks the frame packet is valid or not.
     *
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool IsValid(void) const
    {
        return isValid_;
    }

protected:
    uint8_t pduId_ {AVRC_TG_PDU_ID_INVALID};                      // The value of the "PDU ID".
    uint16_t parameterLength_ {AVRC_TG_BROWSE_PARAMETER_LENGTH};  // The value of the "Parameter Length".
    uint8_t status_ {AVRC_ES_CODE_NO_ERROR};                      // The value of the "Status".
    uint16_t mtu_ {AVRC_TG_DEFAULT_BROWSE_MTU_SIZE};              // The value of the maximum transmission unit.
    Packet *pkt_ {nullptr};                                       // The frame packet.
    uint8_t label_ {AVRC_INVALID_LABEL};  // The label which is used to distinguish different call.
    bool isValid_ {false};                // The packet is valid or not.

    /**
     * @brief Checks the "Parameter Length" is valid or not.
     *
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The "Parameter Length" is valid.
     * @retval false The "Parameter Length" is invalid.
     */
    bool IsValidParameterLength(Packet *pkt);

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcTgBrowsePacket);
};

/******************************************************************
 * SetBrowsedPlayer                                               *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>SetBrowsedPlayer</b> command.
 */
enum AvrcTgSbp {
    AVRC_TG_SBP_FIXED_CMD_PARAMETER_LENGTH = 0x0002,  // The fixed "Parameter Length" of the command frame.
    AVRC_TG_SBP_FIXED_RSP_PARAMETER_LENGTH = 0x000A,  // The fixed "Parameter Length" of the response frame.
    AVRC_TG_SBP_PLAYER_ID = 0xFFFF,                   // The default value of the "Player Id".
    AVRC_TG_SBP_NUMBER_OF_ITEMS = 0x0000,             // The default value of the "Number of Items".
    AVRC_TG_SBP_FOLDER_DEPTH = 0x03,                  // The default value of the "Folder Depth".
    AVRC_TG_SBP_FIXED_CMD_FRAME_SIZE = 0x05,          // The fixed size of the command frame.
    AVRC_TG_SBP_MIN_RSP_FRAME_SIZE = 0x0D,       // The min size of the response frame.(exclude behind "Folder Depth")
    AVRC_TG_SBP_FOLDER_NAME_LENGTH_SIZE = 0x02,  // The size of the "Folder Name Length".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>SetBrowsedPlayer</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.9.3 SetBrowsedPlayer.
 */
class AvrcTgSbpPacket : public AvrcTgBrowsePacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgSbpPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [mtu], [status], [uidCounter],
     * [numOfItems], [folderNames] and [label].
     */
    AvrcTgSbpPacket(uint16_t mtu, uint8_t status, uint16_t uidCounter, uint32_t numOfItems,
        const std::vector<std::string> &folderNames, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgSbpPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgSbpPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgSbpPacket</b> instance.
     */
    ~AvrcTgSbpPacket();

    /**
     * @brief Assembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Number of Items                                     4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder Depth                                        1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder Name Length 1                                2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder Name 1                                       n octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder Name Length n                                2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder Name n                                       n octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Player Id                                           2 octets<br>
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    virtual bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the unique media player id.
     *
     * @return The value of the "Player Id".
     */
    uint16_t GetPlayerId(void) const
    {
        return playerId_;
    }

private:
    uint16_t playerId_ {AVRC_TG_SBP_PLAYER_ID};          // The unique media player id.
    uint16_t uidCounter_ {AVRC_TG_BROWSE_UID_COUNTER};   // The value of the "UID Counter".
    uint32_t numOfItems_ {AVRC_TG_SBP_NUMBER_OF_ITEMS};  // The value of the "Number of items".
    // The value of the "Folder Depth".The root folder has no name and its folder depth is 0.
    uint8_t folderDepth_ {AVRC_TG_SBP_FOLDER_DEPTH};
    std::vector<std::string> folderNames_ {};  // The value of the "Folder Name".

    /**
     * @brief A constructor used to create an <b>AvrcTgSbpPacket</b> instance.
     */
    AvrcTgSbpPacket() = delete;
};

/******************************************************************
 * ChangePath                                                     *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>ChangePath</b> command.
 */
enum AvrcTgCp {
    AVRC_TG_CP_FIXED_CMD_PARAMETER_LENGTH = 0x000B,  // The fixed "Parameter Length" of the command frame.
    AVRC_TG_CP_FIXED_RSP_PARAMETER_LENGTH = 0x0005,  // The fixed "Parameter Length" of the response frame.
    AVRC_TG_CP_NUMBER_OF_ITEMS = 0x0000,             // The default value of the "Number of Items".
    AVRC_TG_CP_NUMBER_OF_ITEMS_SIZE = 0x04,          // The size of the "Number of Items".
    AVRC_TG_CP_FIXED_CMD_FRAME_SIZE = 0x0E,          // The fixed size of the command frame.
    AVRC_TG_CP_FIXED_RSP_FRAME_SIZE = 0x08,          // The fixed size of the response frame.
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>ChangePath</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.4.1 ChangePath.
 */
class AvrcTgCpPacket : public AvrcTgBrowsePacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgCpPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [status], [numOfItems] and
     * [label].
     */
    AvrcTgCpPacket(uint8_t status, uint32_t numOfItems, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgCpPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgCpPacket(Packet *pkt, uint32_t uidCounter, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgCpPacket</b> instance.
     */
    ~AvrcTgCpPacket();

    /**
     * @brief Assembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Number of Items                                     4 octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Direction                                           1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder UID                                          8 octets<br>
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    virtual bool DisassemblePacket(Packet *pkt) override;

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
     * @brief Gets the "Direction".
     *
     * @return The value of the "Direction".
     */
    uint8_t GetDirection(void) const
    {
        return direction_;
    }

    /**
     * @brief Gets the "Folder UID".
     *
     * @return The value of the "Folder UID".
     */
    uint64_t GetFolderUid(void) const
    {
        return folderUid_;
    }

private:
    uint16_t uidCounter_ {AVRC_TG_BROWSE_UID_COUNTER};   // The value of the "UID Counter".
    uint8_t direction_ {AVRC_FOLDER_DIRECTION_INVALID};  // The value of the "Direction".
    uint64_t folderUid_ {AVRC_TG_BROWSE_FOLDER_UID};     // The value of the "Folder UID".
    uint32_t numOfItems_ {AVRC_TG_CP_NUMBER_OF_ITEMS};   // The value of the "Number of items".

    /**
     * @brief A constructor used to create an <b>AvrcTgCpPacket</b> instance.
     */
    AvrcTgCpPacket() = delete;
};

/******************************************************************
 * GetFolderItems                                                 *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>GetFolderItems</b> command.
 */
enum AvrcTgGfi {
    AVRC_TG_GFI_VALID_FEATURE_OCTETS = 0x10,          // The valid octets of the "Feature Bit Mask".
    AVRC_TG_GFI_FIXED_CMD_PARAMETER_LENGTH = 0x000A,  // The fixed "Parameter Length" of the command frame.
    AVRC_TG_GFI_FIXED_RSP_PARAMETER_LENGTH = 0x0005,  // The fixed "Parameter Length" of the response frame.
    AVRC_TG_GFI_START_ITEM = 0x00000000,              // The default value of the "Start Item".
    AVRC_TG_GFI_END_ITEM = 0x00000000,                // The default value of the "End Item".
    AVRC_TG_GFI_ATTRIBUTE_COUNT = 0x00,               // The default value of the "AttributeCount".
    AVRC_TG_GFI_NUMBER_OF_ITEMS = 0x00,               // The default value of the "Number of Items".
    AVRC_TG_GFI_ITEM_TYPE_SIZE = 0x01,                // The size of the "Item Type".
    AVRC_TG_GFI_ITEM_LENGTH_SIZE = 0x02,              // The size of the "Item Length".
    AVRC_TG_GFI_PLAYER_ID_SIZE = 0x02,                // The size of the "Player Id".
    AVRC_TG_GFI_MAJOR_PLAYER_TYPE_SIZE = 0x01,        // The size of the "Major Player Type".
    AVRC_TG_GFI_PLAYER_SUB_TYPE_SIZE = 0x04,          // The size of the "Player Sub Type".
    AVRC_TG_GFI_PLAY_STATUS_SIZE = 0x01,              // The size of the "Play Status".
    AVRC_TG_GFI_FEATURE_BIT_MASK = 0x10,              // The size of the "Feature Bit Mask".
    AVRC_TG_GFI_UID_SIZE = 0x08,                      // The size of the "Folder UID" and the "Media Element UID".
    AVRC_TG_GFI_TYPE_SIZE = 0x01,                     // The size of the "Folder Type" and the "Media Type".
    AVRC_TG_GFI_IS_PLAYABLE_SIZE = 0x01,              // The size of the "Is Playable".
    AVRC_TG_GFI_NUMBER_OF_ATTRIBUTES_SIZE = 0x01,     // The size of the "Number of Attributes".
    AVRC_TG_GFI_ATTRIBUTE_ID_SIZE = 0x04,             // The size of the "Attribute ID".
    AVRC_TG_GFI_ATTRIBUTE_VALUE_LENGTH_SIZE = 0x02,   // The size of the "Attribute Value Length".
    AVRC_TG_GFI_CHARACTER_SET_ID_SIZE = 0x02,         // The size of the "Character Set Id".
    AVRC_TG_GFI_NAME_LENGTH_SIZE = 0x02,              // The size of the "Displayable Name Length".
    AVRC_TG_GFI_MIN_CMD_FRAME_SIZE = 0x0D,            // The fixed size of the command frame.
    AVRC_TG_GFI_MIN_RSP_FRAME_SIZE = 0x03,  // The minimum size of the response frame.(exclude operand behind "Status")
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>GetFolderItems</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.4.2 GetFolderItems.
 */
class AvrcTgGfiPacket : public AvrcTgBrowsePacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgGfiPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [mtu], [status], [uidCounter],
     * [status], [items] and [label].
     */
    AvrcTgGfiPacket(
        uint16_t mtu, uint8_t status, uint32_t uidCounter, const std::vector<AvrcMpItem> &items, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgGfiPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [mtu], [status], [uidCounter],
     * [status], [items] and [label].
     */
    AvrcTgGfiPacket(
        uint16_t mtu, uint8_t status, uint32_t uidCounter, const std::vector<AvrcMeItem> &items, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgGfiPacket</b> instance.
     */
    AvrcTgGfiPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgGfiPacket</b> instance.
     */
    ~AvrcTgGfiPacket();

    /**
     * @brief Assembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Number of Items                                     4 octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Direction                                           1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder UID                                          8 octets<br>
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    virtual bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the "Scope".
     *
     * @return The value of the "Scope".
     */
    uint8_t GetScope(void) const
    {
        return scope_;
    }

    /**
     * @brief Gets the "Start Item".
     *
     * @return The value of the "Start Item".
     */
    uint32_t GetStartItem(void) const
    {
        return startItem_;
    }

    /**
     * @brief Gets the "End Item".
     *
     * @return The value of the "End Item".
     */
    uint32_t GetEndItem(void) const
    {
        return endItem_;
    }

    /**
     * @brief Gets the "Attribute ID".
     *
     * @return The value of the "Attribute ID".
     */
    const std::vector<uint32_t> &GetAttributes(void) const
    {
        return attributes_;
    }

private:
    uint8_t scope_ {AVRC_MEDIA_SCOPE_INVALID};              // The value of the "Scope".
    uint32_t startItem_ {AVRC_TG_GFI_START_ITEM};           // The value of the "Start Item".
    uint32_t endItem_ {AVRC_TG_GFI_END_ITEM};               // The value of the "End Item".
    uint8_t attributeCount_ {AVRC_TG_GFI_ATTRIBUTE_COUNT};  // The value of the "AttributeCount".
    std::vector<uint32_t> attributes_ {};                   // The value of the "Attribute ID".
    uint16_t uidCounter_ {AVRC_TG_BROWSE_UID_COUNTER};      // The value of the "UID Counter".
    uint16_t numOfItems_ {AVRC_TG_GFI_NUMBER_OF_ITEMS};     // The value of the "Number of Items".
    std::vector<AvrcMpItem> mpItems_ {};                    // The list of the <b>AvrcCtMpItem</b> class.
    std::vector<AvrcMeItem> meItems_ {};                    // The list of the <b>AvrcCtMeItem</b> class.

    /**
     * @brief A constructor used to create an <b>AvrcTgGfiPacket</b> instance.
     */
    AvrcTgGfiPacket() = delete;

    uint16_t CountMpFrameSize(void);

    /**
     * @brief Assembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Number of Items                                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Item Type (Media Element Item)                      1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Item Length                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder UID                                          8 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder Type                                         1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Is Playable                                         1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name Length                             2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name                                    n octets<br>
     * @return The frame packet.
     */
    Packet *AssembleMpPacket(void);

    /**
     * @brief Assembles informations of the media player into the packet.
     */
    void AssembleMpItem(uint8_t *buffer, uint16_t offset);

    uint16_t CountMeFrameSize(void);

    /**
     * @brief Assembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *
     *                        0 0 0 0 | 0 0 0 0     Item Type (Media Element Item)                      1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Item Length                                         2 octets<br>
     *
     *                        0 0 0 0 | 0 0 0 0     Folder UID                                          8 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder Type                                         1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Is Playable                                         1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name Length                             2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name                                    n octets<br>
     *
     *                        0 0 0 0 | 0 0 0 0     Number of Items                                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Media Element UID                                   8 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Media Type                                          1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name Length                             2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name                                    n octets<br>
     *                        0 0 0 0 | 0 0 0 0     Number of Attributes                                1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute ID 1                                      4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute Value Length                              2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute Value                                     n octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute ID n                                      n octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    n octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute Value Length                              n octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute Value                                     n octets<br>
     * @return The frame packet.
     */
    Packet *AssembleMePacket(void);

    /**
     * @brief Assembles informations of the media Item into the packet.
     */
    void AssembleMeItem(uint8_t *buffer, uint16_t offset);

    void DisassemblePacketAttributes(Packet *pkt, uint8_t offset);
};

/******************************************************************
 * GetItemAttributes                                              *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>GetItemAttributes</b> command.
 */
enum AvrcTgGia {
    AVRC_TG_GIA_FIXED_CMD_PARAMETER_LENGTH = 0x000C,  // The fixed "Parameter Length" of the command frame.
    AVRC_TG_GIA_FIXED_RSP_PARAMETER_LENGTH = 0x0002,  // The fixed "Parameter Length" of the response frame.
    AVRC_TG_GIA_NUMBER_OF_ATTRIBUTES = 0x00,          // The default value of the "Number of Attributes".
    AVRC_TG_GIA_NUMBER_OF_ATTRIBUTES_SIZE = 0x01,     // The size of the "Number of Attributes".
    AVRC_TG_GIA_ATTRIBUTE_ID_SIZE = 0x04,             // The size of the "Attribute ID".
    AVRC_TG_GIA_CHARACTER_SET_ID_SIZE = 0x02,         // The size of the "Character Set Id".
    AVRC_TG_GIA_ATTRIBUTE_VALUE_LENGTH_SIZE = 0x02,   // The size of the "Attribute Value Length".
    AVRC_TG_GIA_MIN_CMD_FRAME_SIZE = 0x0F,            // The fixed size of the command frame.
    AVRC_TG_GIA_MIN_RSP_FRAME_SIZE = 0x03,  // The minimum size of the response frame.(exclude operand behind "Status")
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>GetItemAttributes</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.4.3 GetItemAttributes.
 */
class AvrcTgGiaPacket : public AvrcTgBrowsePacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgGtnoiPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [uidCounter], [status],
     * [numOfItems] and [label].
     */
    AvrcTgGiaPacket(uint16_t mtu, uint8_t status, const std::vector<uint32_t> &attributes,
        const std::vector<std::string> &values, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgGtnoiPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgGiaPacket(Packet *pkt, uint32_t uidCounter, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgGtnoiPacket</b> instance.
     */
    ~AvrcTgGiaPacket();

    /**
     * @brief Assembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Number of Attributes                                1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute ID 1                                      4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute Value Length                              2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute Value                                     n octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute ID n                                      n octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    n octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute Value Length                              n octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute Value                                     n octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Scope                                               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID                                                 8 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Number of Attributes                                1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute ID 1                                      4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute ID n                                      n octets<br>
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    virtual bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the "Scope".
     *
     * @return The value of the "Scope".
     */
    uint8_t GetScope(void) const
    {
        return scope_;
    }

    /**
     * @brief Gets the "UID".
     *
     * @return The value of the "UID".
     */
    uint16_t GetUid(void) const
    {
        return uid_;
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
     * @brief Gets the "Attribute ID".
     *
     * @return The value of the "Attribute ID".
     */
    const std::vector<uint32_t> &GetAttributes(void) const
    {
        return attributes_;
    }

private:
    uint8_t scope_ {AVRC_MEDIA_SCOPE_INVALID};                    // The value of the "Scope".
    uint64_t uid_ {AVRC_TG_BROWSE_UID};                           // The value of the "UID".
    uint16_t uidCounter_ {AVRC_TG_BROWSE_UID_COUNTER};            // The value of the "UID Counter".
    uint8_t numOfAttributes_ {AVRC_TG_GIA_NUMBER_OF_ATTRIBUTES};  // The value of the "Number of Attributes".
    std::vector<uint32_t> attributes_ {};                         // The value of the "Attribute ID".
    std::vector<std::string> values_ {};                          // The value of the "Attribute Value".

    /**
     * @brief A constructor used to create an <b>AvrcTgGiaPacket</b> instance.
     */
    AvrcTgGiaPacket() = delete;

    void DisassemblePacketAttributes(Packet *pkt, uint8_t offset);
};

/******************************************************************
 * GetTotalNumberOfItems                                          *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>GetTotalNumberOfItems</b> command.
 */
enum AvrcTgGtnoi {
    AVRC_TG_GTNOI_FIXED_RSP_PARAMETER_LENGTH = 0x0007,  // The fixed "Parameter Length" of the response frame.
    AVRC_TG_GTNOI_NUMBER_OF_ITEMS = 0x0000,             // The default value of the "Number of Items".
    AVRC_TG_GTNOI_FIXED_CMD_FRAME_SIZE = 0x03,          // The fixed size of the command frame.
    AVRC_TG_GTNOI_FIXED_RSP_FRAME_SIZE = 0x0A,          // The fixed size of the response frame.
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>GetTotalNumberOfItems</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.4.4 GetTotalNumberOfItems.
 */
class AvrcTgGtnoiPacket : public AvrcTgBrowsePacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgGtnoiPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [uidCounter], [status],
     * [numOfItems] and [label].
     */
    AvrcTgGtnoiPacket(uint8_t status, uint16_t uidCounter, uint32_t numOfItems, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgGtnoiPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgGtnoiPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgGtnoiPacket</b> instance.
     */
    ~AvrcTgGtnoiPacket();

    /**
     * @brief Assembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Number of Items                                     4 octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Scope                                               1 octets<br>
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    virtual bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the "Scope".
     *
     * @return The value of the "Scope".
     */
    uint8_t GetScope(void) const
    {
        return scope_;
    }

private:
    uint8_t scope_ {AVRC_MEDIA_SCOPE_INVALID};             // The value of the "Scope".
    uint16_t uidCounter_ {AVRC_TG_BROWSE_UID_COUNTER};     // The value of the "UID Counter".
    uint32_t numOfItems_ {AVRC_TG_GTNOI_NUMBER_OF_ITEMS};  // The value of the "Number of items".

    /**
     * @brief A constructor used to create an <b>AvrcTgGtnoiPacket</b> instance.
     */
    AvrcTgGtnoiPacket() = delete;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_BROWSE_H