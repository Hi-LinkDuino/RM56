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
 * @file avrcp_tg_vendor.h
 *
 * @brief Declares the class of the VENDOR DEPENDENT command, including attributes and methods.
 */

#ifndef AVRCP_TG_VENDOR_H
#define AVRCP_TG_VENDOR_H

#include "avrcp_tg_packet.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the values of the <b>VENDOR DEPENDENT</b> command.
 */
enum AvrcTgVendor {
    AVRC_TG_VENDOR_MIN_COMMAND_SIZE = 0x07,         // The min size of the command frame.
    AVRC_TG_VENDOR_FIXED_OPERAND_SIZE = 0x0A,       // The size of the fixed operand in the frame.
    AVRC_TG_VENDOR_PACKET_TYPE_SIZE = 0x01,         // The size of the "Packet Type".
    AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE = 0x02,    // The size of the "Parameter Length".
    AVRC_TG_VENDOR_ERROR_CODE_SIZE = 0x01,          // The size of the "Error Code".
    AVRC_TG_VENDOR_SUBUNIT_TYPE = 0x09,             // The value of the "Subunit_type".
    AVRC_TG_VENDOR_SUBUNIT_ID = 0x00,               // The value of the "Subunit_ID".
    AVRC_TG_VENDOR_PACKET_TYPE = 0x00,              // The value of the "Packet type".
    AVRC_TG_VENDOR_PACKET_TYPE_START = 0x01,        // The value of the "Packet type - Start(01)".
    AVRC_TG_VENDOR_PACKET_TYPE_CONTINUE = 0x02,     // The value of the "Packet type - Continue(10)".
    AVRC_TG_VENDOR_PACKET_TYPE_END = 0x03,          // The value of the "Packet type - End(11)".
    AVRC_TG_VENDOR_UID = 0xFFFFFFFFFFFFFFFF,        // The value of the "UID".
    AVRC_TG_VENDOR_UID_COUNTER = 0x0000,            // The value of the "UID Counter".
    AVRC_TG_VENDOR_PARAMETER_LENGTH = 0x0000,       // The value of the "Parameter Length".
    AVRC_TG_VENDOR_CTYPE_OFFSET = 0x00,             // The offset of the "Ctype".
    AVRC_TG_VENDOR_OPCODE_OFFSET = 0x02,            // The offset of the "Opcode".
    AVRC_TG_VENDOR_COMPANY_ID_OFFSET = 0x03,        // The offset of the "Company ID".
    AVRC_TG_VENDOR_PDU_ID_OFFSET = 0x06,            // The offset of the "PDU ID".
    AVRC_TG_VENDOR_PACKET_TYPE_OFFSET = 0x07,       // The offset of the "Packet Type".
    AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET = 0x08,  // The offset of the "Parameter Length".
    AVRC_TG_VENDOR_NUM_OF_PACKETS = 0x0000,         // The default number of the packets.
    AVRC_TG_VENDOR_BIT3 = 0x03                      // Offset three bits.
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the <b>VENDOR DEPENDENT</b>
 * command.
 */
class AvrcTgVendorPacket : public AvrcTgPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgVendorPacket</b> instance.
     */
    AvrcTgVendorPacket();

    /**
     * @brief A constructor used to create an <b>AvrcTgVendorPacket</b> instance.
     */
    AvrcTgVendorPacket(uint8_t pduId, uint8_t status, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgVendorPacket</b> instance.
     */
    virtual ~AvrcTgVendorPacket();

    /**
     * @brief Assembles the operands behind the "PDU ID" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     other operands                                      n octets<br>
     * @return The frame packet.
     */
    virtual Packet *AssembleParameters(Packet *pkt);

    /**
     * @brief Assembles the packet of the frame.
     *
     * @retval nullptr Always return nullptr.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the packet of the frame.
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
     *                        0 0 0 0 | 0 0 0 0     other operands                                      n octets<br>
     */
    void AssemblePackets(void);

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
     *                        0 0 0 0 | 0 0 0 0     other operands                                      n octets<br>
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
    virtual bool DisassembleParameters(uint8_t *buffer);

    /**
     * @brief Gets the number of the packets.
     *
     * @return The number of the packets.
     */
    virtual uint16_t GetNumberOfPackets(void);

    /**
     * @brief Pops a assembled frame packet.
     *
     * @return The frame packet.
     */
    Packet *PopAssembledPacket(void);

    /**
     * @brief Checks frame the packet is exist or not.
     *
     * @return The result of the method execution.
     * @retval true   The packet is exist.
     * @retval false  The packet is not exist.
     */
    bool IsPacketExist(void) const
    {
        return pkts_.size() > 0;
    }

    /**
     * @brief Gets the "response".
     *
     * @return The value of the "response".
     */
    uint8_t GetCrCode(void) const
    {
        return crCode_;
    };

    /**
     * @brief Sets the "response".
     *
     * @param[in] crCode The value the "response".
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
     * @brief Gets the label.
     *
     * @return The value of the label.
     */
    uint8_t GetLabel(void) const
    {
        return label_;
    }

    /**
     * @brief Sets the label.
     *
     * @param[in] label The value of the label.
     */
    void SetLabel(uint8_t label)
    {
        label_ = label;
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
    uint8_t crCode_ {AVRC_TG_RSP_CODE_STABLE};                       // The value of the "ctype" or the "response".
    uint8_t subunitType_ {AVRC_TG_VENDOR_SUBUNIT_TYPE};              // The value of the "Subunit_type".
    uint8_t subunitId_ {AVRC_TG_VENDOR_SUBUNIT_ID};                  // The value of the "Subunit_ID".
    uint8_t opCode_ {AVRC_TG_OP_CODE_VENDOR};                        // The value of the "Opcode".
    uint32_t companyId_ {AVRC_TG_DEFAULT_BLUETOOTH_SIG_COMPANY_ID};  // The value of the "Company ID".
    uint8_t pduId_ {AVRC_TG_PDU_ID_INVALID};                         // The value of the "PDU ID".
    uint8_t packetType_ {AVRC_TG_VENDOR_PACKET_TYPE};                // The value of the "Packet type".
    uint16_t parameterLength_ {AVRC_TG_VENDOR_PARAMETER_LENGTH};     // The value of the "Parameter Length".
    uint8_t status_ {AVRC_ES_CODE_INVALID};                          // The value of the "Status".
    uint16_t mtu_ {AVRC_TG_DEFAULT_CONTROL_MTU_SIZE};                // The value of the maximum transmission unit.
    std::deque<Packet *> pkts_ {};                                   // The frame packets.
    uint8_t label_ {AVRC_INVALID_LABEL};  // The label which is used to distinguish different call.
    bool isValid_ {false};                // The frame packet is valid or not.

    /**
     * @brief Assembles the reject packet of the frame.
     *
     * @details Reject Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status(some command have this operand)              1 octets<br>
     * @return The frame packet.
     * @retval nullptr The packet is not reject packet, need to assemble packet.
     */
    Packet *AssembleRejectPacket(void);

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
     *
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     other operands                                      n octets<br>
     * @return The frame packet.
     */
    Packet *AssembleMinOperands(void);

    /**
     * @brief Checks the "Parameter Length" is valid or not.
     *
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The "Parameter Length" is valid.
     * @retval false The "Parameter Length" is invalid.
     */
    bool IsValidParameterLength(Packet *pkt);

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcTgVendorPacket);
};

/******************************************************************
 * GetCapability                                                  *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>GetCapability<b>
 * frame.
 */
enum AvrcTgGc {
    AVRC_TG_GC_EVENT_PARA_LENGTH = 0x01,    // The Length of each "EventID" supported by "TG".
    AVRC_TG_GC_COMPANY_PARA_LENGTH = 0x03,  // The Length of each "CompanyID" supported by  "TG".
    AVRC_TG_GC_NUM_OF_PACKETS = 0x01,       // The packet num sent by "TG".
    AVRC_TG_GC_CAPABILITY_COUT = 0x00,      // The init value of "CapabilityID".
};

/**
 * @brief This class provides a set of methods associated with assemble / disassemble the packet of the
 * <b>GetCapabilities<b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.4.1 GetCapabilities.
 */
class AvrcTgGcPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>GetCapabilities</b> instance.
     *
     * @details You can use this constructor when wants to initialize the [label].
     */
    AvrcTgGcPacket(uint8_t crCode, const std::vector<uint32_t> &companies, uint8_t label);

    /**
     * @brief A constructor used to create an <b>GetCapabilities</b> instance.
     *
     * @details You can use this constructor when wants to initialize the [label].
     */
    AvrcTgGcPacket(uint8_t crCode, const std::vector<uint8_t> &events, uint8_t label);

    /**
     * @brief A constructor used to create an <b>GetCapabilities</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgGcPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>GetCapabilities</b> instance.
     */
    ~AvrcTgGcPacket();

    /**
     * @brief Assembles the operands behind the "Packet Type" of the frame.
     *
     * @details Command frame:
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
     * @return The packet of the frame.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Parameter Length" of the frame.
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
     * @param[in] buffer The buffer of the frame.
     * @return @c true  : The packet is valid.
     *         @c false : The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the number of the packets.
     *
     * @return The number of the packets.
     */
    uint16_t GetNumberOfPackets(void) override;

    /**
     * @brief Gets the capabilities.
     *
     * @return The companies of the packets.
     */
    const std::vector<uint32_t> &GetCompanies(void) const
    {
        return companies_;
    }

    /**
     * @brief Gets the capabilities.
     *
     * @return The events of the packets.
     */
    const std::vector<uint8_t> &GetEventId(void) const
    {
        return events_;
    }

    /**
     * @brief Gets the capabilityId.
     *
     * @return The capabilityId of the packets.
     */
    uint8_t GetCapabilityId(void) const
    {
        return capabilityId_;
    }

private:
    uint8_t capabilityId_ {AVRC_TG_GC_CAPABILITY_INVALID};  // Specific "Capability" requested.
    uint8_t capabilityCount_ {AVRC_TG_GC_CAPABILITY_COUT};  // The Num of "Capability" Supported by "TG".
    std::vector<uint32_t> companies_ {};                    // The list of CompanyID supported by TG.
    std::vector<uint8_t> events_ {};                        // The list of EventIDs supported by the TG.

    /**
     * @brief A constructor used to create an <b>GetCapabilities</b> instance.
     */
    AvrcTgGcPacket() = delete;

    /**
     * @brief Assemble the companies.
     */
    void AssembleCompanies(Packet *pkt);

    /**
     * @brief Assemble the events.
     */
    void AssembleEvents(Packet *pkt);
};

/******************************************************************
 * GetElementAttributes                                           *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>GetElementAttribute</b> command.
 */
enum AvrcTgGea {
    AVRC_TG_GEA_ATTRIBUTE_LENGTH = 0x08,          // The value of the attributeID charactersetID AttributeValueLength
    AVRC_TG_GEA_SIZE_OF_ATTRIBUTE_LENGTH = 0x01,  // The remaining length of each packet
    AVRC_TG_GEA_MAX_NUM_OF_ATTRIBUTES = 0xff,     // The number of the packets.
    AVRC_TG_GEA_PARAMETER_LENGTH_SIZE = 0x03,     // The size of parameter length.
    AVRC_TG_GEA_ASSEMBLE_MAX_SIZE = 0x01f5,       // max size of the parameter of frame.
    AVRC_TG_GEA_INITIALIZATION = 0x00,            // The value of init parameter.
    AVRC_TG_GEA_NUMBER_OF_ATTRIBUTES = 0x00       // The num of attributes.
};

class AvrcTgGeaPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgGeaPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the [attributes] [values] and [label].
     */
    AvrcTgGeaPacket(
        uint8_t crCode, const std::vector<uint32_t> &attribtues, const std::vector<std::string> &values, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgGeaPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgGeaPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgGeaPacket</b> instance.
     */
    ~AvrcTgGeaPacket();

    /**
     * @brief Assembles the operands behind the "PDU ID" of the frame.
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
     * @return The packet of the frame.
     */
    Packet *AssembleParameters(Packet *pkt) override;

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
     * @return @c true  : The packet is valid.
     *         @c false : The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief  Gets the number of the packets.
     *
     * @return The number of the packets.
     */
    uint16_t GetNumberOfPackets(void) override;

    /**
     * @brief  Get the Unique identifier.
     *
     * @return The Unique identifier.
     */
    uint64_t GetIdentifier(void) const
    {
        return identifier_;
    }

    /**
     * @brief  Gets the number of  Attribute.
     *
     * @return The number of  Attribute.
     */
    uint8_t GetNumberOfAttribute(void) const
    {
        return number_;
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

private:
    uint8_t number_ {AVRC_TG_GEA_INITIALIZATION};  // The num of the "Element Attribute" in one packet.
    uint64_t identifier_ {AVRC_TG_VENDOR_UID};     // Unique identifier to identify an element on TG.
    std::vector<uint32_t> attributes_ {};          // Specifies the attribute ID for the attributes to be retrieved.
    uint16_t numOfValues_ {AVRC_TG_GEA_INITIALIZATION};      // The value of the "NumGetElementAttribute".
    std::vector<std::string> values_ {};                     // The list of the value of this attribute.
    std::deque<std::pair<uint8_t, uint16_t>> packetPos_ {};  // record each packet's attribute count and parameter
                                                            // length.

    /**
     * @brief A constructor used to create an <b>AvrcTgGeaPacket</b> instance.
     */
    AvrcTgGeaPacket() = delete;
};

/******************************************************************
 * GetPlayStatus                                                  *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>GetPlayStatus</b> command.
 */
enum AvrcTgGps {
    AVRC_TG_GPS_PARAMETER_LENGTH = 0x0009,  // The value of the "Parameter Length".
    AVRC_TG_GPS_NUM_OF_PACKETS = 0x0001     // The number of the packets.
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the <b>GetPlayStatus</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.7.1 GetPlayStatus.
 */
class AvrcTgGpsPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgGpsPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [songLength], [songPosition],
     * [playStatus] and [label].
     */
    AvrcTgGpsPacket(uint8_t crCode, uint32_t songLength, uint32_t songPosition, uint8_t playStatus, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgGpsPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgGpsPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgGpsPacket</b> instance.
     */
    ~AvrcTgGpsPacket();

    /**
     * @brief Assembles the operands behind the "PDU ID" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Song Length                                         4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Song Position                                       4 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Play Status                                         1 octets<br>
     * @return The frame packet.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
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

    /**
     * @brief A constructor used to create an <b>AvrcTgGpsPacket</b> instance.
     */
    AvrcTgGpsPacket() = delete;
};

/******************************************************************
 * SetAddressedPlayer                                             *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>SetAddressedPlayer</b> command.
 */
enum AvrcTgSap {
    AVRC_TG_SAP_PARAMETER_LENGTH = 0x0001,  // The value of the "Parameter Length".
    AVRC_TG_SAP_PLAYER_ID = 0xFFFF,         // The value of the "Player Id".
    AVRC_TG_SAP_NUM_OF_PACKETS = 0x0001,    // The number of the packets.
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the
 * <b>SetAddressedPlayer</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.9.1 SetAddressedPlayer.
 */
class AvrcTgSapPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgSapPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [crCode], [status] and [label].
     */
    AvrcTgSapPacket(uint8_t crCode, uint8_t status, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgSapPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgSapPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgSapPacket</b> instance.
     */
    ~AvrcTgSapPacket();

    /**
     * @brief Assembles the operands behind the "PDU ID" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     * @return The frame packet.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
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
     * @brief Gets the unique media player id.
     *
     * @return The value of the "Player Id".
     */
    uint16_t GetPlayerId(void) const
    {
        return playerId_;
    }

private:
    uint16_t playerId_ {AVRC_TG_SAP_PLAYER_ID};  // The value of the "Player Id".

    /**
     * @brief A constructor used to create an <b>AvrcTgSapPacket</b> instance.
     */
    AvrcTgSapPacket() = delete;
};

/******************************************************************
 * PlayItem                                                       *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values of the <b>PlayItemResponse</b> command.
 */
enum AvrcTgPi {
    AVRC_TG_PI_PARAMETER_LENGTH = 0x0001,  // The value of the "Parameter Length".
    AVRC_TG_PI_SCOPE_OFFSET = 0x0a,        // The offset of the "scope" in the frame.
    AVRC_TG_PI_NUM_OF_PACKETS = 0x01,      // The packet num sent by "TG".
};

class AvrcTgPiPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgPiPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [crCode], [status] and [label].
     */
    AvrcTgPiPacket(uint8_t crCode, uint8_t status, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgPiPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgPiPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgPiPacket</b> instance.
     */
    ~AvrcTgPiPacket(void);

    /**
     * @brief Assembles the payload into the PLAYITEM response.
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
     * @return The frame packet.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the payload from the PLAYITEM command.
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
     * @param[in] pkt The packet of the frame.
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
     * @brief Sets the scope of the PLAYITEM command.
     *
     * @param[in] uid which media content navigation may take place.
     */
    uint8_t GetScope() const
    {
        return scope_;
    }

    /**
     * @brief Gets the uid of the PLAYITEM command.
     *
     * @return uid which is the unique identification of a Media Element Item.
     */
    uint64_t GetUid(void) const
    {
        return uid_;
    }

    /**
     * @brief Gets the uid counter of the PLAYITEM command.
     *
     * @return uid counter which is the count of uid change .
     */
    uint16_t GetUidCounter(void) const
    {
        return uidCounter_;
    }

private:
    uint8_t scope_ {AVRC_MEDIA_SCOPE_INVALID};          // The scope in which media content navigation may take place.
    uint64_t uid_ {AVRC_TG_VENDOR_UID};                 // The uid in the frame.
    uint16_t uidCounter_ {AVRC_TG_VENDOR_UID_COUNTER};  // The uid counter in the frame.

    /**
     * @brief A constructor used to create an <b>AvrcTgPiPacket</b> instance.

     */
    AvrcTgPiPacket() = delete;
};

/******************************************************************
 * AddToNowPlaying                                                *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>AddToNowPlaying</b> command.
 */
enum AvrcTgAtnp {
    AVRC_TG_ATNP_PARAMETER_LENGTH = 0x0001,  // The value of the "Parameter Length".
    AVRC_TG_ATNP_NUM_OF_PACKETS = 0x0001,    // The number of the packets.
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the <b>AddToNowPlaying</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.12.2 AddToNowPlaying.
 */
class AvrcTgAtnpPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgAtnpPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [label].
     */
    AvrcTgAtnpPacket(uint8_t crCode, uint8_t status, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgAtnpPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgAtnpPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgAtnpPacket</b> instance.
     */
    ~AvrcTgAtnpPacket();

    /**
     * @brief Assembles the operands behind the "PDU ID" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Status                                              1 octets<br>
     * @return The frame packet.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
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
     * @brief Gets the scope in which the UID of the media element item or folder item.
     *
     * @return The value of the "Scope".
     */
    uint8_t GetScope(void) const
    {
        return scope_;
    }

    /**
     * @brief Gets the UID of the media element item or folder item.
     *
     * @return The value of the "UID".
     */
    uint64_t GetUid(void) const
    {
        return uid_;
    }

    /**
     * @brief Gets the UID Counter.
     *
     * @return The value of the "UID Counter".
     */
    uint16_t GetUidCounter(void) const
    {
        return uidCounter_;
    }

private:
    uint8_t scope_ {AVRC_MEDIA_SCOPE_INVALID};          // The value of the "Scope".
    uint64_t uid_ {AVRC_TG_VENDOR_UID};                 // The value of the "UID".
    uint16_t uidCounter_ {AVRC_TG_VENDOR_UID_COUNTER};  // The value of the "UID Counter".

    /**
     * @brief A constructor used to create an <b>AvrcTgAtnpPacket</b> instance.
     */
    AvrcTgAtnpPacket() = delete;
};

/******************************************************************
 * SetAbsoluteVolume                                              *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>SetAbsoluteVolume</b> command.
 */
enum AvrcTgSav {
    AVRC_TG_SAV_NUM_OF_PACKETS = 0x0001,  // The number of the packets.
    AVRC_TG_SAV_VOLUME_SIZE = 0x01,       // The size of the "Absolute Volume".
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the <b>SetAbsoluteVolume</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.13.2 SetAbsoluteVolume.
 */
class AvrcTgSavPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgSavPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [volume] and [label].
     */
    AvrcTgSavPacket(uint8_t crCode, uint8_t volume, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgSavPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgSavPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgSavPacket</b> instance.
     */
    ~AvrcTgSavPacket();

    /**
     * @brief Assembles the operands behind the "PDU ID" of the frame.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                                            4 bits<br>
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     *
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     Absolute Volume                                     1 octets<br>
     * @return The frame packet.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
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
     * @brief A constructor used to create an <b>AvrcTgSavPacket</b> instance.
     */
    AvrcTgSavPacket() = delete;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_VENDOR_H