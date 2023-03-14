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

#ifndef AVRCP_CT_BROWSE_H
#define AVRCP_CT_BROWSE_H

#include "avrcp_ct_internal.h"
#include "avrcp_ct_packet.h"
#include "avrcp_media.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the values of the <b>BROWSING</b> commands.
 */
enum AvrcCtBrowse {
    AVRC_CT_BROWSE_MIN_SIZE = 0x03,                  // The size of the "PDU ID" + The size of the "Parameter Length".
    AVRC_CT_BROWSE_PARAMETER_LENGTH = 0x0000,        // The default value of the "Parameter Length".
    AVRC_CT_BROWSE_UID = 0x0000000000000000,         // The default value of the "UID".
    AVRC_CT_BROWSE_UID_COUNTER = 0x0000,             // The default value of the "UID Counter".
    AVRC_CT_BROWSE_FOLDER_UID = 0xFFFFFFFFFFFFFFFF,  // The default value of the "Folder UID".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the <b>BROWSING</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 5.1.2 Browsing commands.
 */
class AvrcCtBrowsePacket : public AvrcCtPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtBrowsePacket</b> instance.
     */
    AvrcCtBrowsePacket();

    /**
     * @brief A destructor used to delete the <b>AvrcCtBrowsePacket</b> instance.
     */
    virtual ~AvrcCtBrowsePacket();

    /**
     * @brief Assembles the frame packet.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Other operands                                      n octets<br>
     * @return The frame packet.
     */
    virtual const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the frame packet.
     *
     * @details Response frame:<br>
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
    uint8_t pduId_;             // The value of the "PDU ID".
    uint16_t parameterLength_;  // The value of the "Parameter Length".
    uint16_t mtu_;              // The value of the maximum transmission unit.
    Packet *pkt_;               // The frame packet.
    bool isValid_;              // The packet is valid or not.

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcCtBrowsePacket);
};

/******************************************************************
 * SetBrowsedPlayer                                               *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>SetBrowsedPlayer</b> command.
 */
enum AvrcCtSbp {
    AVRC_CT_SBP_FIXED_CMD_PARAMETER_LENGTH = 0x0002,  // The fixed "Parameter Length" of the command frame.
    AVRC_CT_SBP_FIXED_RSP_PARAMETER_LENGTH = 0x000A,  // The fixed "Parameter Length" of the response frame.
    AVRC_CT_SBP_PLAYER_ID = 0xFFFF,                   // The default value of the "Player Id".
    AVRC_CT_SBP_NUMBER_OF_ITEMS = 0x0000,             // The default value of the "Number of Items".
    AVRC_CT_SBP_FOLDER_DEPTH = 0x03,                  // The default value of the "Folder Depth".
    AVRC_CT_SBP_FIXED_CMD_FRAME_SIZE = 0x05,          // The fixed size of the command frame.
    AVRC_CT_SBP_MIN_RSP_FRAME_SIZE = 0x0D,  // The minimum size of the response frame.(exclude operand behind "Status")
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>SetBrowsedPlayer</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.9.3 SetBrowsedPlayer.
 */
class AvrcCtSbpPacket : public AvrcCtBrowsePacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtSbpPacket</b> instance.
     */
    explicit AvrcCtSbpPacket(uint16_t playerId);

    /**
     * @brief A constructor used to create an <b>AvrcCtSbpPacket</b> instance.
     */
    explicit AvrcCtSbpPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtSbpPacket</b> instance.
     */
    ~AvrcCtSbpPacket();

    /**
     * @brief Assembles the packet of the frame.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Player Id                                           2 octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
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
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    virtual bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the "Status".
     *
     * @return The value of the "Status".
     */
    uint8_t GetStatus(void) const
    {
        return status_;
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
     * @brief Gets the "Number of items".
     *
     * @return The value of the "Number of items".
     */
    uint32_t GetNumOfItems(void) const
    {
        return numOfItems_;
    }

    /**
     * @brief Gets the "Folder Name".
     *
     * @return The list of the "Folder Name".
     */
    const std::vector<std::string> &GetFolderNames(void) const
    {
        return folderNames_;
    }

private:
    uint16_t playerId_ {AVRC_CT_SBP_PLAYER_ID};          // The value of the "Player Id".
    uint8_t status_ {AVRC_ES_CODE_INVALID};              // The value of the "Status".
    uint16_t uidCounter_ {AVRC_CT_BROWSE_UID_COUNTER};   // The value of the "UID Counter".
    uint32_t numOfItems_ {AVRC_CT_SBP_NUMBER_OF_ITEMS};  // The value of the "Number of items".
    // The value of the "Folder Depth".The root folder has no name and its folder depth is 0.
    uint8_t folderDepth_ {AVRC_CT_SBP_FOLDER_DEPTH};
    std::vector<std::string> folderNames_ {};  // The value of the "Folder Name".

    /**
     * @brief  Disassembles the folder name of the packet of the frame.
     * @param[in] buffer The frame packet.
     * @param[in] offset The offset of the disassemble packet.
     */
    void DisassemblePacketName(uint8_t *buffer, uint16_t offset);
};

/******************************************************************
 * ChangePath                                                     *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>ChangePath</b> command.
 */
enum AvrcCtCp {
    AVRC_CT_CP_FIXED_CMD_PARAMETER_LENGTH = 0x000B,  // The fixed "Parameter Length" of the command frame.
    AVRC_CT_CP_FIXED_RSP_PARAMETER_LENGTH = 0x0005,  // The fixed "Parameter Length" of the response frame.
    AVRC_CT_CP_NUMBER_OF_ITEMS = 0x0000,             // The default value of the "Number of Items".
    AVRC_CT_CP_FIXED_CMD_FRAME_SIZE = 0x0E,          // The fixed size of the command frame.
    AVRC_CT_CP_FIXED_RSP_FRAME_SIZE = 0x08,          // The fixed size of the response frame.
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>ChangePath</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.4.1 ChangePath.
 */
class AvrcCtCpPacket : public AvrcCtBrowsePacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtCpPacket</b> instance.
     */
    AvrcCtCpPacket(uint16_t uidCounter, uint8_t direction, uint64_t folderUid);

    /**
     * @brief A constructor used to create an <b>AvrcCtCpPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtCpPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtCpPacket</b> instance.
     */
    ~AvrcCtCpPacket();

    /**
     * @brief Assembles the packet of the frame.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Direction                                           1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder UID                                          8 octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Number of Items                                     4 octets<br>
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the "Status".
     *
     * @return The value of the "Status".
     */
    uint8_t GetStatus(void) const
    {
        return status_;
    }

    /**
     * @brief Gets the "Number of items".
     *
     * @return The value of the "Number of items".
     */
    uint32_t GetNumOfItems(void) const
    {
        return numOfItems_;
    }

private:
    uint16_t uidCounter_ {AVRC_CT_BROWSE_UID_COUNTER};   // The value of the "UID Counter".
    uint8_t direction_ {AVRC_FOLDER_DIRECTION_INVALID};  // The value of the "Direction".
    uint64_t folderUid_ {AVRC_CT_BROWSE_FOLDER_UID};     // The value of the "Folder UID".
    uint8_t status_ {AVRC_ES_CODE_INVALID};              // The value of the "Status".
    uint32_t numOfItems_ {AVRC_CT_CP_NUMBER_OF_ITEMS};   // The value of the "Number of items".
};

/******************************************************************
 * GetFolderItems                                                 *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>GetFolderItems</b> command.
 */
enum AvrcCtGfi {
    AVRC_CT_GFI_VALID_FEATURE_OCTETS = 0x10,          // The valid octets of the "Feature Bit Mask".
    AVRC_CT_GFI_FIXED_CMD_PARAMETER_LENGTH = 0x000A,  // The fixed "Parameter Length" of the command frame.
    AVRC_CT_GFI_START_ITEM = 0x00000000,              // The default value of the "Start Item".
    AVRC_CT_GFI_END_ITEM = 0x00000000,                // The default value of the "End Item".
    AVRC_CT_GFI_ATTRIBUTE_COUNT = 0x00,               // The default value of the "AttributeCount".
    AVRC_CT_GFI_NUMBER_OF_ITEMS = 0x00,               // The default value of the "Number of Items".
    AVRC_CT_GFI_ATTRIBUTE_ID_SIZE = 0x04,             // The size of the "Attribute ID".
    AVRC_CT_GFI_MIN_CMD_FRAME_SIZE = 0x0D,            // The fixed size of the command frame.
    AVRC_CT_GFI_MIN_RSP_FRAME_SIZE = 0x08,  // The minimum size of the response frame.(exclude operand behind "Status")
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>GetFolderItems</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.4.2 GetFolderItems.
 */
class AvrcCtGfiPacket : public AvrcCtBrowsePacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtGfiPacket</b> instance.
     */
    AvrcCtGfiPacket(uint8_t scope, uint32_t startItem, uint32_t endItem, const std::vector<uint32_t> &attributes);

    /**
     * @brief A constructor used to create an <b>AvrcCtGfiPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtGfiPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtGfiPacket</b> instance.
     */
    ~AvrcCtGfiPacket();

    /**
     * @brief Assembles the packet of the frame.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Scope                                               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Start Item                                          4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     End Item                                            4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     AttributeCount                                      1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute ID 1                                      4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Attribute ID n                                      n octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Number of Items                                     2 octets<br>
     *
     *                        0 0 0 0 | 0 0 0 0     Item Type (Media Player Item)                       1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Item Length                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Player Id                                           2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Major Player Type                                   1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Player Sub Type                                     4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Play Status                                         1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Feature Bit Mask                                   16 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name Length                             2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name                                    n octets<br>
     *
     *                        0 0 0 0 | 0 0 0 0     Item Type (Folder Item)                             1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Item Length                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder UID                                          8 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Folder Type                                         1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Is Playable                                         1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name Length                             2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name                                    n octets<br>
     *
     *                        0 0 0 0 | 0 0 0 0     Item Type (Media Element Item)                      1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Item Length                                         2 octets<br>
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
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePacket(Packet *pkt) override;

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
     * @brief Gets the "Status".
     *
     * @return The value of the "Status".
     */
    uint8_t GetStatus(void) const
    {
        return status_;
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
     * @brief Gets the media player list.
     *
     * @return The reference of the list of the <b>AvrcMpItem</b> class.
     */
    const std::vector<AvrcMpItem> &GetMediaPlayers(void) const
    {
        return mpItems_;
    }

    /**
     * @brief Gets the folder and media element list.
     *
     * @return The reference of the list of the <b>AvrcMpItem</b> class.
     */
    const std::vector<AvrcMeItem> &GetMediaItems(void) const
    {
        return meItems_;
    }

private:
    uint8_t scope_ {AVRC_MEDIA_SCOPE_INVALID};              // The value of the "Scope".
    uint32_t startItem_ {AVRC_CT_GFI_START_ITEM};           // The value of the "Start Item".
    uint32_t endItem_ {AVRC_CT_GFI_END_ITEM};               // The value of the "End Item".
    uint8_t attributeCount_ {AVRC_CT_GFI_ATTRIBUTE_COUNT};  // The value of the "AttributeCount".
    std::vector<uint32_t> attributes_ {};                   // The value of the "Attribute ID".
    uint8_t status_ {AVRC_ES_CODE_INVALID};                 // The value of the "Status".
    uint16_t uidCounter_ {AVRC_CT_BROWSE_UID_COUNTER};      // The value of the "UID Counter".
    uint16_t numOfItems_ {AVRC_CT_GFI_NUMBER_OF_ITEMS};     // The value of the "Number of Items".
    std::vector<AvrcMpItem> mpItems_ {};                    // The list of the <b>AvrcMpItem</b> class.
    std::vector<AvrcMeItem> meItems_ {};                    // The list of the <b>AvrcMeItem</b> class.

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     Item Type (Media Player Item)                       1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Item Length                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Player Id                                           2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Major Player Type                                   1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Player Sub Type                                     4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Play Status                                         1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Feature Bit Mask                                   16 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Character Set Id                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name Length                             2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Displayable Name                                    n octets<br>
     * @param[in] buffer The frame packet.
     * @return The read offset of the frame packet.
     */
    uint16_t DisassembleMpParameter(uint8_t *buffer);

    /**
     * @brief  Disassembles the folder name of the packet of the frame.
     * @param[in]  buffer The frame packet.
     * @param[in]  offset The offset of the disassemble packet.
     * @param[out] name   The name disassembled.
     * @return The read offset of the frame packet.
     */
    uint16_t DisassembleMpParameterName(uint8_t *buffer, uint16_t offset, std::string &name);

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Response frame:<br>
     *                        msb(7)     lsb(0)<br>
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
     * @param[in] buffer The frame packet.
     * @return The read offset of the frame packet.
     */
    uint16_t DisassembleMeParameter(uint8_t *buffer);

    /**
     * @brief  Disassembles the folder name of the packet of the frame.
     * @param[in]  buffer The frame packet.
     * @param[in]  offset The offset of the disassemble packet.
     * @param[out] name   The name disassembled.
     * @return The read offset of the frame packet.
     */
    uint16_t DisassembleMeParameterName(uint8_t *buffer, uint16_t offset, std::string &name);

    /**
     * @brief  Disassembles the folder name of the packet of the frame.
     * @param[in]  buffer The frame packet.
     * @param[in]  offset The offset of the disassemble packet.
     * @param[out] name   The name disassembled.
     * @return The read offset of the frame packet.
     */
    uint16_t DisassembleMeParameterAttributesAndValues(uint8_t *buffer, uint16_t offset, uint8_t itemType,
       std::vector<uint32_t> &attributes, std::vector<std::string> &values);
};

/******************************************************************
 * GetItemAttributes                                              *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>GetItemAttributes</b> command.
 */
enum AvrcCtGia {
    AVRC_CT_GIA_FIXED_CMD_PARAMETER_LENGTH = 0x000C,  // The fixed "Parameter Length" of the command frame.
    AVRC_CT_GIA_NUMBER_OF_ATTRIBUTES = 0x00,          // The default value of the "Number of Attributes".
    AVRC_CT_GIA_ATTRIBUTE_ID_SIZE = 0x04,             // The size of the "Attribute ID".
    AVRC_CT_GIA_MIN_CMD_FRAME_SIZE = 0x0F,            // The fixed size of the command frame.
    AVRC_CT_GIA_MIN_RSP_FRAME_SIZE = 0x05,  // The minimum size of the response frame.(exclude operand behind "Status")
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>GetItemAttributes</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.4.3 GetItemAttributes.
 */
class AvrcCtGiaPacket : public AvrcCtBrowsePacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtGiaPacket</b> instance.
     */
    AvrcCtGiaPacket(uint8_t scope, uint64_t uid, uint16_t uidCounter, const std::vector<uint32_t> &attributes);

    /**
     * @brief A constructor used to create an <b>AvrcCtGiaPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtGiaPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtGiaPacket</b> instance.
     */
    ~AvrcCtGiaPacket();

    /**
     * @brief Assembles the packet of the frame.
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
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
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
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the "Status".
     *
     * @return The value of the "Status".
     */
    uint8_t GetStatus(void) const
    {
        return status_;
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

    /**
     * @brief Gets the "Folder Name".
     *
     * @return The list of the "Folder Name".
     */
    const std::vector<std::string> &GetValues(void) const
    {
        return values_;
    }

private:
    uint8_t scope_ {AVRC_MEDIA_SCOPE_INVALID};                    // The value of the "Scope".
    uint64_t uid_ {AVRC_CT_BROWSE_UID};                           // The value of the "UID".
    uint16_t uidCounter_ {AVRC_CT_BROWSE_UID_COUNTER};            // The value of the "UID Counter".
    uint8_t numOfAttributes_ {AVRC_CT_GIA_NUMBER_OF_ATTRIBUTES};  // The value of the "Number of Attributes".
    std::vector<uint32_t> attributes_ {};                         // The value of the "Attribute ID".
    uint8_t status_ {AVRC_ES_CODE_INVALID};                       // The value of the "Status".
    std::vector<std::string> values_ {};                          // The value of the "Attribute Value".

    /**
     * @brief  Disassembles the folder name of the packet of the frame.
     * @param[in] buffer The frame packet.
     * @param[in] offset The offset of the disassemble packet.
     */
    void DisassemblePacketName(uint8_t *buffer, uint16_t &offset);
};

/******************************************************************
 * GetTotalNumberOfItems                                          *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>GetTotalNumberOfItems</b> command.
 */
enum AvrcCtGtnoi {
    AVRC_CT_GTNOI_FIXED_CMD_PARAMETER_LENGTH = 0x0001,  // The fixed "Parameter Length" of the command frame.
    AVRC_CT_GTNOI_FIXED_RSP_PARAMETER_LENGTH = 0x0007,  // The fixed "Parameter Length" of the response frame.
    AVRC_CT_GTNOI_NUMBER_OF_ITEMS = 0x0000,             // The default value of the "Number of Items".
    AVRC_CT_GTNOI_FIXED_CMD_FRAME_SIZE = 0x04,          // The fixed size of the command frame.
    AVRC_CT_GTNOI_FIXED_RSP_FRAME_SIZE = 0x0A,          // The fixed size of the response frame.
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>GetTotalNumberOfItems</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.4.4 GetTotalNumberOfItems.
 */
class AvrcCtGtnoiPacket : public AvrcCtBrowsePacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtGtnoiPacket</b> instance.
     */
    explicit AvrcCtGtnoiPacket(uint8_t scope);

    /**
     * @brief A constructor used to create an <b>AvrcCtGtnoiPacket</b> instance.
     */
    explicit AvrcCtGtnoiPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtGtnoiPacket</b> instance.
     */
    ~AvrcCtGtnoiPacket();

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Command frame:<br>
     *                        msb(7)     lsb(0)<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Scope                                               1 octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

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
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    virtual bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the "Status".
     *
     * @return The value of the "Status".
     */
    uint8_t GetStatus(void) const
    {
        return status_;
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
     * @brief Gets the "Number of items".
     *
     * @return The value of the "Number of items".
     */
    uint32_t GetNumOfItems(void) const
    {
        return numOfItems_;
    }

private:
    uint8_t scope_ {AVRC_MEDIA_SCOPE_INVALID};             // The value of the "Scope".
    uint16_t uidCounter_ {AVRC_CT_BROWSE_UID_COUNTER};     // The value of the "UID Counter".
    uint8_t status_ {AVRC_ES_CODE_INVALID};                // The value of the "Status".
    uint32_t numOfItems_ {AVRC_CT_GTNOI_NUMBER_OF_ITEMS};  // The value of the "Number of items".
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_CT_BROWSE_H