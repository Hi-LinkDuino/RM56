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

#ifndef AVRCP_CT_VENDOR_H
#define AVRCP_CT_VENDOR_H

#include "avrcp_ct_packet.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the values of the <b>VENDOR DEPENDENT</b> command.
 */
enum AvrcCtVendor {
    AVRC_CT_VENDOR_MIN_COMMAND_SIZE = 0x08,           // The min size of the command frame.
    AVRC_CT_VENDOR_FIXED_OPERAND_SIZE = 0x0A,         // The size of the fixed operand in the frame.
    AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE = 0x0002,    // The size of the "Parameter Length".
    AVRC_CT_VENDOR_SUBUNIT_TYPE = 0x09,               // The value of the "Subunit_type".
    AVRC_CT_VENDOR_SUBUNIT_ID = 0x00,                 // The value of the "Subunit_ID".
    AVRC_CT_VENDOR_PACKET_TYPE = 0x00,                // The value of the "Packet type".
    AVRC_CT_VENDOR_PACKET_TYPE_START = 0x01,          // The value of the "Packet type - Start(01)".
    AVRC_CT_VENDOR_PACKET_TYPE_CONTINUE = 0x02,       // The value of the "Packet type - Continue(10)".
    AVRC_CT_VENDOR_PACKET_TYPE_END = 0x03,            // The value of the "Packet type - End(11)".
    AVRC_CT_VENDOR_PARAMETER_LENGTH = 0x0000,         // The value of the "Parameter Length".
    AVRC_TG_VENDOR_REJECT_PARAMETER_LENGTH = 0x0001,  // The value of the "Parameter Length".
    AVRC_CT_VENDOR_UID = 0xFFFFFFFFFFFFFFFF,          // The value of the "UID".
    AVRC_CT_VENDOR_UID_COUNTER = 0x0000,              // The value of the "UID Counter".
    AVRC_CT_VENDOR_CTYPE_OFFSET = 0x00,               // The offset of the "Ctype".
    AVRC_CT_VENDOR_OPCODE_OFFSET = 0x02,              // The offset of the "Opcode".
    AVRC_CT_VENDOR_COMPANY_ID_OFFSET = 0x03,          // The offset of the "Company ID".
    AVRC_CT_VENDOR_PDU_ID_OFFSET = 0x06,              // The offset of the "PDU ID".
    AVRC_CT_VENDOR_PACKET_TYPE_OFFSET = 0x07,         // The offset of the "Packet Type".
    AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET = 0x08,    // The offset of the "Parameter Length".
    AVRC_CT_VENDOR_STATUS_OFFSET = 0x0A,              // The offset of the "Error Code".
    AVRC_CT_VENDOR_RECEIVED_FRAGMENTS = 0x00,         // The value of the received fragments.
};

/**
 * @brief This enumeration declares the values of the moving bit
 */
enum AvrcCtVendorMoveBit {
    AVRC_CT_VENDOR_MOVE_BIT_3 = 0x03,
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the <b>VENDOR DEPENDENT</b>
 * command.
 */
class AvrcCtVendorPacket : public AvrcCtPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtVendorPacket</b> instance.
     */
    AvrcCtVendorPacket();

    /**
     * @brief A destructor used to delete the <b>AvrcCtVendorPacket</b> instance.
     */
    virtual ~AvrcCtVendorPacket();

    /**
     * @brief Assembles the operands behind the "Packet Type" of the frame.
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
     *                        0 0 0 0 | 0 0 0 0     other operands                                      n octets<br>
     * @return The frame packet.
     */
    virtual Packet *AssembleParameters(Packet *pkt) = 0;

    /**
     * @brief Assembles the packet of the frame.
     *
     * @retval nullptr Always return nullptr.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
     *
     * @details Command frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     ctype                                               4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The parameters is valid.
     * @retval false The parameters is invalid.
     */
    virtual bool DisassembleParameters(uint8_t *buffer) = 0;

    /**
     * @brief Gets the "ctype".
     *
     * @return The value of the "ctype".
     */

    uint8_t GetCrCode(void) const
    {
        return crCode_;
    };

    /**
     * @brief Sets the "ctype".
     *
     * @param[in] crCode The value the "ctype".
     */
    void SetCrCode(uint8_t crCode)
    {
        crCode_ = crCode;
    }

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
     * @brief Gets the received fragments.
     *
     * @return The value of the received fragments.
     */
    uint8_t GetReceivedFragments(void) const
    {
        return receivedFragments_;
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
    uint8_t crCode_ {AVRC_CT_CMD_CODE_STATUS};                       // The value of the "ctype" or the "response".
    uint8_t subunitType_ {AVRC_CT_VENDOR_SUBUNIT_TYPE};              // The value of the "Subunit_type".
    uint8_t subunitId_ {AVRC_CT_VENDOR_SUBUNIT_ID};                  // The value of the "Subunit_ID".
    uint8_t opCode_ {AVRC_CT_OP_CODE_VENDOR};                        // The value of the "Opcode".
    uint32_t companyId_ {AVRC_CT_DEFAULT_BLUETOOTH_SIG_COMPANY_ID};  // The value of the "Company ID".
    uint8_t pduId_ {AVRC_CT_PDU_ID_INVALID};                         // The value of the "PDU ID".
    uint8_t packetType_ {AVRC_CT_VENDOR_PACKET_TYPE};                // The value of the "Packet type".
    uint16_t parameterLength_ {AVRC_CT_VENDOR_PARAMETER_LENGTH};     // The value of the "Parameter Length".
    uint8_t status_ {AVRC_ES_CODE_INVALID};                          // The value of the "Status".
    uint8_t receivedFragments_ {AVRC_CT_VENDOR_RECEIVED_FRAGMENTS};  // The value of the received fragments.
    Packet *pkt_ {nullptr};                                          // The frame packet.
    bool isValid_ {false};                                           // The frame packet is valid or not.

    /**
     * @brief Assembles the operands in the front of the "Packet Type" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     * @return The frame packet.
     */
    Packet *AssembleFixedOperands(void);

    /**
     * @brief Checks the "Parameter Length" is valid or not.
     *
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The "Parameter Length" is valid.
     * @retval false The "Parameter Length" is invalid.
     */
    bool IsValidParameterLength(Packet *pkt);

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcCtVendorPacket);
};

/******************************************************************
 * GetCapability                                                  *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>GetCapability<b>
 */
enum AvrcCtGc {
    AVRC_CT_GC_PARAMETER_LENGTH = 0x01,  // The Length of each "Parameter".
    AVRC_CT_GC_CAPABILITY_COUT = 0x00,   // The Init value of "CapabilityID".
};

/**
 * @brief This class provides a set of methods associated with assemble / disassemble the packet of the
 * <b>GetCapability<b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.1 GetCapability.
 */

class AvrcCtGcPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>GetCapability</b> instance.
     */
    explicit AvrcCtGcPacket(uint8_t capabilityId);

    /**
     * @brief A constructor used to create an <b>GetCapability</b> instance.
     */
    explicit AvrcCtGcPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>GetCapability</b> instance.
     */
    ~AvrcCtGcPacket();

    /**
     * @brief Assembles the operands behind the "Packet Type" of the frame.
     *
     * @details Command frame:
     *                        msb           lsb
     *                        0 0 0 0 | 0 0 0 0     ctype                                               4 bits
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits
     *
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets
     *                        0 0 0 0 | 0 0 0 0     Capability ID 0x2 (CompanyID)/0x3 (EventsID)        1 octets
     * @return The packet of the frame.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Parameter Length" of the frame.
     *
     * @details Response frame:
     *                        msb           lsb
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits
     *
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets
     *                        0 0 0 0 | 0 0 0 0     Capability ID                                       1 octets
     *                        0 0 0 0 | 0 0 0 0     Capability Count                                    1 octets
     *                        0 0 0 0 | 0 0 0 0     Capability ID or EVENT               3*3n octets or n octets
     * @param[in] buffer The buffer of the frame.
     * @return @c true  : The packet is valid.
     *         @c false : The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the capabilities.
     *
     * @return The companyId of the packets.
     */
    const std::vector<uint32_t> &GetCompanies(void) const
    {
        return companies_;
    }

    /**
     * @brief Gets the capabilities.
     *
     * @return The eventId of the packets.
     */
    const std::vector<uint8_t> &GetEvents(void) const
    {
        return events_;
    }

private:
    uint8_t capabilityId_ {AVRC_CT_GC_CAPABILITY_INVALID};  // Specific capability requested.
    uint8_t capabilityCount_ {AVRC_CT_GC_CAPABILITY_COUT};  // Specifies the number of CompanyID returned.
    std::vector<uint32_t> companies_ {};                    // List of CompanyID.
    std::vector<uint8_t> events_ {};                        // Minimum of two mandatory EventIDs.

    /**
     * @brief A constructor used to create an <b>GetCapability</b> instance.
     */
    AvrcCtGcPacket() = delete;
};

/******************************************************************
 * GetElementAttribute                                            *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>GetElementAttribute<b> frame.
 */
enum AvrcCtGea {
    AVRC_CT_GEA_IDENTIFIER_SIZE = 0x08,         // The length of the "Identifier" size.
    AVRC_CT_GEA_PARAMETER_LENGTH_SIZE = 0x0002,  // The length of the "Parameter Length" size.
    AVRC_CT_GEA_ATTRIBUTE_COUNT_SIZE = 0x01,     // The length of the "Attribute Count" size.
    AVRC_CT_GEA_ATTRIBUTE_SIZE = 0x04,           // The length of the "Attribute" size.
    AVRC_CT_GEA_INITIALIZATION = 0x00,           // The value of initialization.
    AVRC_CT_GEA_MTU = 0x0200,                    // The "mtu" of the packet.
};

/**
 * @brief This class provides a set of methods associated with assemble / disassemble the packet of the
 * <b>GetElementAttribute<b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.6.1 GetElementAttribute.
 */
class AvrcCtGeaPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtGeaPacket</b> instance.
     */
    AvrcCtGeaPacket(uint64_t identifier, std::vector<uint32_t> attributes);

    /**
     * @brief A constructor used to create an <b>AvrcCtGeaPacket</b> instance.
     */
    explicit AvrcCtGeaPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtGeaPacket</b> instance.
     */
    ~AvrcCtGeaPacket();

    /**
     * @brief Assembles the operands behind the "Packet Type" of the frame.
     *
     * @details Command frame:
     *                        msb           lsb
     *                        0 0 0 0 | 0 0 0 0     ctype                                               4 bits
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits
     *
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets
     *                        0 0 0 0 | 0 0 0 0     Identifier                                          8 octets
     *                        0 0 0 0 | 0 0 0 0     AttributeCount                                      4 octets
     *                        0 0 0 0 | 0 0 0 0     Attribute1  N                                       1 octets
     * @return The packet of the frame.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Parameter Length" of the frame.
     *
     * @details Response frame:
     *                        msb           lsb
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets
     *
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets
     *                        0 0 0 0 | 0 0 0 0     Number of Attributes                                1 octets
     *
     *                        0 0 0 0 | 0 0 0 0     Attribute ID1                                       4 octets
     *                        0 0 0 0 | 0 0 0 0     CharacterSetID1                                     2 octets
     *                        0 0 0 0 | 0 0 0 0     AttributeValueLength1                               2 octets
     *                        0 0 0 0 | 0 0 0 0     AttributeValue1                  AttributeValueLength octets
     * @param[in] buffer The buffer of the frame.
     * @return @c true  : The packet is valid.
     *         @c false : The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the "Packet Type" of the frame.
     *
     * @return The "Packet Type" of the frame.
     */
    uint8_t GetPacketType(void) const
    {
        return packetType_;
    }

    /**
     * @brief  Gets the number of  Attribute.
     *
     * @return The number of  Attribute.
     */

    uint8_t GetNumberOfAttribute(void) const
    {
        return attributes_.size();
    }

    /**
     * @brief Specifies the attribute ID for the attributes to be retrieved.
     *
     * @return Specifies the attribute ID for the attributes to be retrieved.
     */
    const std::vector<uint32_t> &GetAttributes(void) const
    {
        return attributes_;
    }

    /**
     * @brief Clears the "ElementAttributeID".
     *
     * @return The ID of the "ElementAttributeID".
     */
    void ClearAttributes(void)
    {
        attributes_.clear();
    }

    /**
     * @brief Clears the "PlayerApplicationSettingValueID".
     *
     * @return The values of the "PlayerApplicationSettingValueID".
     */
    void ClearValues(void)
    {
        values_.clear();
    }

    /**
     * @brief Gets the "ElementAttributes".
     *
     * @return The values of the "ElementAttributesValueID".
     */
    const std::vector<std::string> &GetValues(void) const
    {
        return values_;
    }

private:
    uint64_t identifier_ {AVRC_CT_VENDOR_UID};               // Unique identifier to identify an element on TG.
    uint8_t number_ {AVRC_CT_GEA_INITIALIZATION};            // The num of the "Element Attribute" of one packet.
    uint16_t numOfAttributes_ {AVRC_CT_GEA_INITIALIZATION};  // The num of the "AttributeNum".
    std::vector<uint32_t> attributes_ {};  // Specifies the attribute ID for the attributes to be retrieved.
    uint16_t AttributeValueLength_ {AVRC_CT_GEA_INITIALIZATION};  // Length of the value of the attribute.
    uint16_t numOfValues_ {AVRC_CT_GEA_INITIALIZATION};           // The num of the "ValueNum".
    std::vector<std::string> values_ {};                          // The values of the "ElementAttributesID".

    bool DisassemblePacketAttributes(uint8_t *buffer, uint16_t offset);

    /**
     * @brief A constructor used to create an <b>AvrcCtGeaPacket</b> instance.
     */
    AvrcCtGeaPacket() = delete;
};

/******************************************************************
 * GetPlayStatus                                                  *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>GetPlayStatus</b> command.
 */
enum AvrcCtGps {
    AVRC_CT_GPS_PARAMETER_LENGTH = 0x0001,  // The value of the "Parameter Length".
    AVRC_CT_GPS_SONG_LENGTH_OFFSET = 0x0A,  // The offset of the "Song length".
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the <b>GetPlayStatus</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.7.1 GetPlayStatus.
 */
class AvrcCtGpsPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtGpsPacket</b> instance.
     */
    AvrcCtGpsPacket();

    /**
     * @brief A constructor used to create an <b>AvrcCtGpsPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtGpsPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtGpsPacket</b> instance.
     */
    ~AvrcCtGpsPacket();

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
     *                        0 0 0 0 | 0 0 0 0     Song Length                                         4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Song Position                                       4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Play Status                                         1 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the total length of the playing song in milliseconds.
     *
     * @return The total length of the playing song in milliseconds.
     */
    uint32_t GetSongLength() const
    {
        return songLength_;
    }

    /**
     * @brief Gets the current position of the playing in milliseconds elapsed.
     *
     * @return The current position of the playing in milliseconds elapsed.
     */
    uint32_t GetSongPosition(void) const
    {
        return songPosition_;
    }

    /**
     * @brief Gets the current status of playing.
     *
     * @return The current status of playing.
     */
    uint8_t GetPlayStatus(void) const
    {
        return playStatus_;
    }

private:
    uint32_t songLength_ {AVRC_PLAY_STATUS_INVALID_SONG_LENGTH};      // The value of the "SongLength".
    uint32_t songPosition_ {AVRC_PLAY_STATUS_INVALID_SONG_POSITION};  // The value of the "SongPosition".
    uint8_t playStatus_ {AVRC_PLAY_STATUS_ERROR};                     // The value of the "PlayStatus".
};

/******************************************************************
 * SetAddressedPlayer                                             *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>SetAddressedPlayer</b> command.
 */
enum AvrcCtSap {
    AVRC_CT_SAP_PARAMETER_LENGTH = 0x0002,  // The value of the "Parameter Length".
    AVRC_CT_SAP_PLAYER_ID = 0xFFFF,         // The value of the "Player Id".
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the
 * <b>SetAddressedPlayer</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.9.1 SetAddressedPlayer.
 */
class AvrcCtSapPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtSapPacket</b> instance.
     */
    explicit AvrcCtSapPacket(uint16_t playerId);

    /**
     * @brief A constructor used to create an <b>AvrcCtSapPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtSapPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtSapPacket</b> instance.
     */
    ~AvrcCtSapPacket();

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
     *                        0 0 0 0 | 0 0 0 0     Player Id                                           2 octets<br>
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
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the result of the operation.
     *
     * @return The value of the "Status".
     */
    uint8_t GetStatus(void) const
    {
        return status_;
    }

private:
    uint16_t playerId_ {AVRC_CT_SAP_PLAYER_ID};  // The value of the "Player Id".

    /**
     * @brief A constructor used to create an <b>AvrcCtSapPacket</b> instance.
     */
    AvrcCtSapPacket() = delete;
};

/******************************************************************
 * PlayItem                                                       *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the PlayItem frame.
 */
using AvrcCtPi = enum {
    AVRC_CT_PI_PARAMETER_LENGTH = 0x000B,  // The value of the "Parameter Length".
    AVRC_CT_PI_STATUS_OFFSET = 0x0A,       // The offset of the frame body.
};

class AvrcCtPiPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtPiPacket</b> instance.
     */
    AvrcCtPiPacket(uint8_t scope, uint64_t uid, uint16_t uidCounter);

    /**
     * @brief A constructor used to create an <b>AvrcCtPiPacket</b> instance.
     */
    explicit AvrcCtPiPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtAtnpPacket</b> instance.
     */
    ~AvrcCtPiPacket(void);

    /**
     * @brief Assembles the payload into the PlayItem response.
     *
     * @details Command frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Scope                                               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID                                                 8 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         1 octets<br>
     * @return The frame packet.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the payload from the PlayItem command.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     status                                              1 octets<br>
     * @param[in] pkt The packet of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the status of the PlayItem response.
     *
     * @return status which is the result of the PlayItem operation .
     */
    uint8_t GetStatus(void) const
    {
        return status_;
    }

private:
    uint8_t scope_ {AVRC_MEDIA_SCOPE_INVALID};          // The scope in which media content navigation may take place.
    uint64_t uid_ {AVRC_CT_VENDOR_UID};                 // The uid in the frame.
    uint16_t uidCounter_ {AVRC_CT_VENDOR_UID_COUNTER};  // The uid counter in the frame.

    /**
     * @brief A constructor used to create an <b>AvrcCtPiPacket</b> instance.
     */
    AvrcCtPiPacket() = delete;
};

/******************************************************************
 * AddToNowPlaying                                                *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>AddToNowPlaying</b> command.
 */
enum AvrcCtAtnp {
    AVRC_CT_ATNP_PARAMETER_LENGTH = 0x000B,  // The value of the "Parameter Length".
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the <b>AddToNowPlaying</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.12.2 AddToNowPlaying.
 */
class AvrcCtAtnpPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtAtnpPacket</b> instance.
     */
    AvrcCtAtnpPacket(uint8_t scope, uint64_t uid, uint16_t uidCounter);

    /**
     * @brief A constructor used to create an <b>AvrcCtAtnpPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtAtnpPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtAtnpPacket</b> instance.
     */
    ~AvrcCtAtnpPacket();

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
     *                        0 0 0 0 | 0 0 0 0     Scope                                               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID                                                 8 octets<br>
     *                        0 0 0 0 | 0 0 0 0     UID Counter                                         2 octets<br>
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
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the result of the operation.
     *
     * @return The value of the "Status".
     */
    uint8_t GetStatus(void) const
    {
        return status_;
    }

private:
    uint8_t scope_ {AVRC_MEDIA_SCOPE_INVALID};          // The value of the "Scope".
    uint64_t uid_ {AVRC_CT_VENDOR_UID};                 // The value of the "UID".
    uint16_t uidCounter_ {AVRC_CT_VENDOR_UID_COUNTER};  // The value of the "UID Counter".

    /**
     * @brief A constructor used to create an <b>AvrcCtAtnpPacket</b> instance.
     */
    AvrcCtAtnpPacket() = delete;
};

/******************************************************************
 * SetAbsoluteVolume                                              *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>SetAbsoluteVolume</b> command.
 */
enum AvrcCtSav {
    AVRC_CT_SAV_PARAMETER_LENGTH = 0x0001,  // The value of the "Parameter Length".
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the <b>SetAbsoluteVolume</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.13.2 SetAbsoluteVolume.
 */
class AvrcCtSavPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtSavPacket</b> instance.
     */
    explicit AvrcCtSavPacket(uint8_t volume);

    /**
     * @brief A constructor used to create an <b>AvrcCtAtnpPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtSavPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtGpsPacket</b> instance.
     */
    ~AvrcCtSavPacket();

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
     *                        0 0 0 0 | 0 0 0 0     Absolute Volume                                     1 octets<br>
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
     *                        0 0 0 0 | 0 0 0 0     Absolute Volume                                     1 octets<br>
     * @param[in] buffer The buffer of the frame.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

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
    uint8_t volume_ {AVRC_ABSOLUTE_VOLUME_INVALID};  // The value of the "Absolute Volume".

    /**
     * @brief A constructor used to create an <b>AvrcCtSavPacket</b> instance.
     */
    AvrcCtSavPacket() = delete;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_CT_VENDOR_H