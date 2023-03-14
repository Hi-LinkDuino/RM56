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
 * @file avrcp_tg_player_application_settings.h
 *
 * @brief Declares the class of the VENDOR DEPENDENT command related to the player application settings PDUS, including
 * attributes and methods.
 */

#ifndef AVRCP_TG_VENDOR_PLAYER_APPLICATION_SETTINGS_H
#define AVRCP_TG_VENDOR_PLAYER_APPLICATION_SETTINGS_H

#include<deque>
#include "avrcp_tg_vendor.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
/******************************************************************
 * ListPlayerApplicationSettingAttributes                         *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>ListPlayerApplicationSettingAttributes</b> command.
 */
enum AvrcTgLpasa {
    AVRC_TG_LPASA_NUM_OF_PACKETS = 0x01,          // The number of the packets.
    AVRC_TG_LPASA_NUM_OF_ATTRIBUTES = 0x00,       // The value of the "NumPlayerApplicationSettingAttributes".
    AVRC_TG_LPASA_NUM_OF_ATTRIBUTES_SIZE = 0x01,  // The size of the "NumPlayerApplicationSettingAttributes".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>ListPlayerApplicationSettingAttributes</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.2 ListPlayerApplicationSettingAttributes.
 */
class AvrcTgLpasaPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgLpasaPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [values] and [label].
     */
    AvrcTgLpasaPacket(uint8_t crCode, const std::deque<uint8_t> &attributes, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgLpasaPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgLpasaPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgLpasaPacket</b> instance.
     */
    ~AvrcTgLpasaPacket();

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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingValues                   1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID  1              1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID  N              1 octets
     * @return The frame packet.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
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

private:
    // The value of the "NumPlayerApplicationSettingAttributes".
    uint8_t numOfAttributes_ {AVRC_TG_LPASA_NUM_OF_ATTRIBUTES};
    std::deque<uint8_t> attributes_ {};  // A set of the "PlayerApplicationSettingAttributeID".

    /**
     * @brief A constructor used to create an <b>AvrcTgLpasaPacket</b> instance.
     */
    AvrcTgLpasaPacket() = delete;
};

/******************************************************************
 * ListPlayerApplicationSettingValues                             *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>ListPlayerApplicationSettingValues</b> command.
 */
enum AvrcTgLpasv {
    AVRC_TG_LPASV_NUM_OF_PACKETS = 0x01,              // The number of the packets.
    AVRC_TG_LPASV_NUM_OF_VALUES = 0x00,               // The value of the "NumPlayerApplicationSettingValues".
    AVRC_TG_LPASV_MAX_NUM_OF_VALUES = 0xFF,           // The maximum value of the "NumPlayerApplicationSettingValues".
    AVRC_TG_LPASV_NUM_OF_VALUES_SIZE = 0x01,          // The size of the "NumPlayerApplicationSettingValues".
    AVRC_TG_LPASV_NUM_OF_VALUES_AND_ATTRIBUTE = 0x02  // The size of the value and attribute.
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>ListPlayerApplicationSettingValues</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.2 ListPlayerApplicationSettingValues.
 */
class AvrcTgLpasvPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgLpasvPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [values] and [label].
     */
    AvrcTgLpasvPacket(uint8_t crCode, const std::deque<uint8_t> &values, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgLpasvPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgLpasvPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgLpasvPacket</b> instance.
     */
    ~AvrcTgLpasvPacket();

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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingValues                   1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID  1                  1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID  N                  1 octets<br>
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
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID                 1 octets<br>
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
     * @brief Gets the attribute of the player application setting.
     *
     * @return The attribute of the player application setting.
     */
    uint8_t GetAttribute(void) const
    {
        return attribute_;
    }

    /**
     * @brief Checks the attribute of the player application setting is valid or not.
     *
     * @return The result of the method execution.
     * @retval true  The attribute is valid.
     * @retval false The attribute is invalid.
     */
    bool IsValidAttribute(void) const;

private:
    uint8_t attribute_ {AVRC_PLAYER_ATTRIBUTE_ILLEGAL};   // The value of the "PlayerApplicationSettingAttributeID".
    uint16_t numOfValues_ {AVRC_TG_LPASV_NUM_OF_VALUES};  // The value of the "NumPlayerApplicationSettingValues".
    std::deque<uint8_t> values_ {};                       // A set of values of the "PlayerApplicationSettingValueID".

    /**
     * @brief A constructor used to create an <b>AvrcTgLpasvPacket</b> instance.
     */
    AvrcTgLpasvPacket() = delete;
};

/******************************************************************
 * GetCurrentPlayerApplicationSettingValue                        *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>GetCurrentPlayerApplicationSettingValue</b> command.
 */
enum AvrcTgGcpasv {
    AVRC_TG_GCPASV_NUM_OF_ATTRIBUTES = 0x00,      // The value of the "NumPlayerApplicationSettingAttributeID".
    AVRC_TG_GCPASV_MAX_NUM_OF_ATTRIBUTES = 0xFF,  // The maximum value of the "NumPlayerApplicationSettingAttributeID".
    AVRC_TG_GCPASV_NUMBER_OF_VALUES = 0x00,       // The value of "NumPlayerApplicationSettingValues".
    AVRC_TG_GCPASV_MAX_NUM_OF_VALUES = 0xFF,      // The maximum number of "NumPlayerApplicationSettingValues".
    AVRC_TG_GCPASV_ATTRIBUTE_SIZE = 0x01,         // The size of the "NumPlayerApplicationSettingAttributeID".
    AVRC_TG_GCPASV_NUM_OF_VALUES_SIZE = 0x01,     // The size of the "NumPlayerApplicationSettingValues".
    AVRC_TG_GCPASV_VALUE_SIZE = 0x01,             // The size of the "PlayerApplicationSettingValueID".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>GetCurrentPlayerApplicationSettingValue</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.3 GetCurrentPlayerApplicationSettingValue.
 */
class AvrcTgGcpasvPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgGcpasvPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [attributes], [values] and [label].
     */
    AvrcTgGcpasvPacket(uint16_t mtu, uint8_t crCode, const std::deque<uint8_t> &attributes,
        const std::deque<uint8_t> &values, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgGcpasvPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgGcpasvPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgLpasvPacket</b> instance.
     */
    ~AvrcTgGcpasvPacket();

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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingValues                   1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID 1               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID 1                   1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID n               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID n                   1 octets<br>
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
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingAttributeID              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID 1               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID n               1 octets<br>
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
     * @brief Gets the attribute of the player application setting.
     *
     * @return The attribute of the player application setting.
     */
    const std::deque<uint8_t> &GetAttributes(void) const
    {
        return attributes_;
    }

    /**
     * @brief Checks the attribute of the player application setting is valid or not.
     *
     * @return The result of the method execution.
     * @retval true  The attribute is valid.
     * @retval false The attribute is invalid.
     */
    bool IsValidAttribute(void) const;

private:
    // The value of the "NumPlayerApplicationSettingAttributeID". Allowed Values: 1-255.
    uint8_t numOfAttributes_ {AVRC_TG_GCPASV_NUMBER_OF_VALUES};
    std::deque<uint8_t> attributes_ {};  // The value of the "PlayerApplicationSettingAttributeID".
    // The value of the "NumPlayerApplicationSettingValues". Allowed Values: 1-255.
    uint8_t numOfValues_ {AVRC_TG_GCPASV_NUM_OF_ATTRIBUTES};
    std::deque<uint8_t> values_ {};  // A set of values of the "PlayerApplicationSettingValueID".

    /**
     * @brief A constructor used to create an <b>AvrcTgGcpasvPacket</b> instance.
     */
    AvrcTgGcpasvPacket() = delete;

    /**
     * @brief Gets the available size of the parameter.
     *
     * @return The available parameter size.
     */
    uint16_t GetAvailableParameterSize(void) const;
};

/******************************************************************
 * SetPlayerApplicationSettingValue                               *
 ******************************************************************/

/**
 * @brief This enumeration declares the values of the <b>SetPlayerApplicationSettingValue</b> command.
 */
enum AvrcTgSpasv {
    AVRC_TG_SPASV_NUM_OF_PACKETS = 0x01,     // The number of the packets.
    AVRC_TG_SPASV_NUM_OF_ATTRIBUTES = 0x00,  // The value of the "NumPlayerApplicationSettingAttributes".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>SetPlayerApplicationSettingValue</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.4 SetPlayerApplicationSettingValue.
 */
class AvrcTgSpasvPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgSpasvPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the attributes [crCode] and [label].
     */
    AvrcTgSpasvPacket(uint8_t crCode, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgSpasvPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgSpasvPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgSpasvPacket</b> instance.
     */
    ~AvrcTgSpasvPacket();

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
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
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
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the number of the packets.
     *
     * @return The number of the packets.
     */
    uint16_t GetNumberOfPackets(void) override;

    /**
     * @brief Gets the attribute of the player application setting.
     *
     * @return The attribute of the player application setting.
     */
    const std::deque<uint8_t> &GetAttributes(void) const
    {
        return attributes_;
    }

    /**
     * @brief Gets the "PlayerApplicationSettingValueID".
     *
     * @return The values of the "PlayerApplicationSettingValueID".
     */
    const std::deque<uint8_t> &GetValues(void) const
    {
        return values_;
    }

    /**
     * @brief Checks the attribute and value of the player application setting is valid or not.
     *
     * @return The result of the method execution.
     * @retval true  The attribute and value is valid.
     * @retval false The attribute and value is invalid.
     */
    bool IsValidAttributeAndValue(void) const;

private:
    // The value of the "NumPlayerApplicationSettingAttributeID". Allowed Values: 1-255.
    std::deque<uint8_t> attributes_ {};  // The value of the "PlayerApplicationSettingAttributeID".
    uint8_t numOfAttributes_ {AVRC_TG_SPASV_NUM_OF_ATTRIBUTES};
    std::deque<uint8_t> values_ {};  // A set of values of the "PlayerApplicationSettingValueID".

    /**
     * @brief A constructor used to create an <b>AvrcTgSpasvPacket</b> instance.
     */
    AvrcTgSpasvPacket() = delete;
};

/******************************************************************
 * GetPlayerApplicationSettingAttributeText                       *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>GetPlayerApplicationSettingAttributeText<b>
 * frame.
 */
enum AvrcTgGpasat {
    AVRC_TG_GPASAT_ATTRIBUTE_NUM_LENGTH = 0x01,            // The Length of Attribtue num.
    AVRC_TG_GPASAT_PARAMETER_LENGTH_SIZE = 0x03,           // The Length of the "Parameter Length".
    AVRC_TG_GPASAT_NUM_OF_ATTRIBUTE = 0x00,                // The Number of attribute
    AVRC_TG_GPASAT_ASSEMBLE_MAX_SIZE = 0x01f5,             // max last size of a frame.
    AVRC_TG_GPASAT_PARAMETER_EXCEPT_ATTRIBUTE_NUM = 0x04,  // the size of parameter except attribute num.
};

/**
 * @brief This class provides a set of methods associated with assemble / disassemble the packet of the
 * <b>GetPlayerApplicationSettingAttributeText<b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.5 GetPlayerApplicationSettingAttributeText.
 */
class AvrcTgGpasatPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgGpasatPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the [label].
     */
    AvrcTgGpasatPacket(uint8_t crCode, const std::vector<uint8_t> &attributes, const std::vector<std::string> &attrName,
        uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgGpasatPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgGpasatPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgGpasatPacket</b> instance.
     */
    ~AvrcTgGpasatPacket();

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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingAttributes               1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID  1              1 octets
     *                        0 0 0 0 | 0 0 0 0     CharacterSetID1                                     1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttribtueID  N              1 octets
     * @return The packet of the frame.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Parameter Length" of the frame.
     *
     * @details Response frame:
     *                        msb           lsb
     *                        0 0 0 0 | 0 0 0 0     ctype                                               4 bits
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits
     *
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets
     *                        0 0 0 0 | 0 0 0 0     AttributeNum                                        1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID                 N octets
     * @param[in] buffer The buffer of the frame.
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
     * @brief Gets the values of the "PlayerApplicationSettingAttribtueID".
     *
     * @return The values of the "PlayerApplicationSettingAttribtueID".
     */
    const std::vector<uint8_t> &GetAttributes(void) const
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
    void ClearValueName(void)
    {
        attributeName_.clear();
    }

    /**
     * @brief Gets the "ElementAttributes".
     *
     * @return The values of the "ElementAttributesValueID".
     */

    const std::vector<std::string> &GetAttributeName(void) const
    {
        return attributeName_;
    }

    /**
     * @brief Checks the attribute of the player application setting is valid or not.
     *
     * @return The result of the method execution.
     * @retval true  The attribute is valid.
     * @retval false The attribute is invalid.
     */
    static bool IsValidAttribute(uint8_t attribute);

private:
    uint16_t numOfAttributes_ {
    AVRC_TG_GPASAT_NUM_OF_ATTRIBUTE};       // The value of the "NumPlayerApplicationSettingAttributes".
    std::vector<uint8_t> attributes_ {};         // A set of values of the "PlayerApplicationSettingAttributeID".
    std::vector<std::string> attributeName_ {};  // Specifies the player application setting attribute string in UTF-8.
    std::deque<std::pair<uint8_t, uint16_t>> packetPos_ {};  // Record each packet's attribute count and "parameter
                                                            // length".
    uint8_t number_ {AVRC_TG_GPASAT_NUM_OF_ATTRIBUTE};       // The num of Attribute in one packet.

    /**
     * @brief A constructor used to create an <b>AvrcTgGpasatPacket</b> instance.
     */
    AvrcTgGpasatPacket() = delete;
};

/******************************************************************
 * GetPlayerApplicationSettingValueText                           *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>GetPlayerApplicationSettingAttributeText<b>
 * frame.
 */
enum AvrcTgGpasvt {
    AVRC_TG_GPASVT_NUM_OF_ATTRIBUTE_SIZE = 0x01,           // The Size of attribute num.
    AVRC_TG_GPASVT_PARAMETER_LENGTH_SIZE = 0x0003,         // The Length of the "Parameter Length".
    AVRC_TG_GPASVT_NUM_OF_ATTRIBUTE = 0x00,                // The Number of attribute.
    AVRC_TG_GPASVT_NUM_OF_ATTRIBUTES_OFFSET = 0x0A,        // The Offset of attribute.
    AVRC_TG_GPASVT_ASSEMBLE_MAX_SIZE = 0x01f5,             // max last size of a frame.
    AVRC_TG_GPASVT_PARAMETER_EXCEPT_ATTRIBUTE_NUM = 0x04,  // The size of parameter except value num.
    AVRC_TG_GPASVT_INITIALIZATION = 0x00,                  // The value of initialization.
};

/**
 * @brief This class provides a set of methods associated with assemble / disassemble the packet of the
 * <b>GetPlayerApplicationSettingValueText<b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.6 GetPlayerApplicationSettingValueText.
 */
class AvrcTgGpasvtPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgGpasvtPacket</b> instance.
     *
     * @details You can use this constructor when wants to initialize the [label].
     */
    AvrcTgGpasvtPacket(uint8_t crCode, const std::vector<uint8_t> &values,
    const std::vector<std::string> &valueName, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgGpasvtPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgGpasvtPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgGpasvtPacket</b> instance.
     */
    ~AvrcTgGpasvtPacket();

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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingAttributes               1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID1                    1 octets
     *                        0 0 0 0 | 0 0 0 0     CharacterSetID1                                     2 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueStringLength1          1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueString1                1 octets     *
     * @return The packet of the frame.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Parameter Length" of the frame.
     *
     * @details Response frame:
     *                        msb           lsb
     *                        0 0 0 0 | 0 0 0 0     ctype                                               4 bits
     * Subunit type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                                          3 bits
     *                        0 0 0 0 | 0 0 0 0     Opcode                                              1 octets
     *                        0 0 0 0 | 0 0 0 0     Company ID : Bluetooth SIG registered CompanyID     3 octets
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits
     *
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID                 1 octets
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingValue                    1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID                     N octets
     * @param[in] buffer The buffer of the frame.
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
     * @brief Gets the attribute of the player application setting.
     *
     * @return The attribute of the player application setting.
     */
    uint8_t GetAttributeId(void) const
    {
        return attributeId_;
    }

    /**
     * @brief Gets the values of the "PlayerApplicationSettingAttribtueID".
     *
     * @return The values of the "PlayerApplicationSettingAttribtueID".
     */
    const std::vector<uint8_t> &GetValues(void) const
    {
        return values_;
    }

    /**
     * @brief clear the values of the "PlayerApplicationSettingAttribtueID".
     *
     * @return clear values of the "PlayerApplicationSettingAttribtueID".
     */
    void ClearValues(void)
    {
        values_.clear();
    }

    /**
     * @brief Clears the "PlayerApplicationSettingValueName.
     */
    void ClearValueName(void)
    {
        valueName_.clear();
    }

    /**
     * @brief Gets the "PlayerApplicationSettingValueString".
     *
     * @return The values of the "PlayerApplicationSettingValueString".
     */
    const std::vector<std::string> &GetValueName(void) const
    {
        return valueName_;
    }

    /**
     * @brief Checks the attribute of the player application setting is valid or not.
     *
     * @return The result of the method execution.
     * @retval true  The attribute is valid.
     * @retval false The attribute is invalid.
     */
    bool IsValidAttributeAndValue(void) const;

private:
    uint8_t attributeId_ {AVRC_TG_GPASVT_INITIALIZATION};   // Player application setting attribute ID.
    uint16_t numOfValues_ {AVRC_TG_GPASVT_INITIALIZATION};  // The value of the "NumPlayerApplicationSettingValues".
    std::vector<uint8_t> values_ {};                        // Player application setting value ID.
    std::vector<std::string> valueName_ {};  // Specifies the value string of player application setting value in
                                            // UTF-8.
    std::deque<std::pair<uint8_t, uint16_t>> packetPos_ {};  // record each packet's "Attribute count" and "Parameter
                                                            // length".
    uint8_t number_ {AVRC_TG_GPASVT_INITIALIZATION};         // The num of Attribute in one packet.

    /**
     * @brief A constructor used to create an <b>AvrcTgGpasvtPacket</b> instance.
     */
    AvrcTgGpasvtPacket() = delete;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_VENDOR_PLAYER_APPLICATION_SETTINGS_H