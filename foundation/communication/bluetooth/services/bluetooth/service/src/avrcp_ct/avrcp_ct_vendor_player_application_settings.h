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

#ifndef AVRCP_CT_VENDOR_PLAYER_APPLICATION_SETTINGS_H
#define AVRCP_CT_VENDOR_PLAYER_APPLICATION_SETTINGS_H

#include "avrcp_ct_vendor.h"
#include "packet.h"
namespace OHOS {
namespace bluetooth {
/******************************************************************
 * ListPlayerApplicationSettingAttribute                          *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>ListPlayerApplicationSettingAttributes<b>
 */
enum AvrcCtLpasa {
    AVRC_CT_LPASA_PARAMETER_LENGTH = 0x00,
    AVRC_CT_LPASA_PARAMETER_LENGTH_SIZE = 0x0002,   // The Length of the "Parameter Length".
    AVRC_CT_LPASA_NUM_OF_ATTRIBUTE = 0x00,          // The Number of attribute
    AVRC_CT_LPASA_NUM_OF_ATTRIBUTES_OFFSET = 0x0A,  // The Offset of attribute
};

/**
 * @brief This class provides a set of methods associated with assemble / disassemble the packet of the
 * <b>ListPlayerApplicationSettingAttributes<b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.1 ListPlayerApplicationSettingAttributes.
 */
class AvrcCtLpasaPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtLpasaPacket</b> instance.
     */
    AvrcCtLpasaPacket();

    /**
     * @brief A constructor used to create an <b>AvrcCtLpasaPacket</b> instance.
     */
    explicit AvrcCtLpasaPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtLpasaPacket</b> instance.
     */
    ~AvrcCtLpasaPacket();

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
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID                 1 octets
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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingAttributes               1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID  1              1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttribtueID  N              1 octets
     * @param[in] buffer The buffer of the frame.
     * @return @c true  : The packet is valid.
     *         @c false : The packet is invalid.
     */
    bool DisassembleParameters(uint8_t *buffer) override;

    /**
     * @brief Gets the values of the "PlayerApplicationSettingAttribtueID".
     *
     * @return The values of the "PlayerApplicationSettingAttribtueID".
     */
    std::vector<uint8_t> GetAttributes(void) const
    {
        return attributes_;
    }

private:
    // The value of the "NumPlayerApplicationSettingAttributes".
    uint16_t numOfAttributes_ {AVRC_CT_LPASA_NUM_OF_ATTRIBUTE};
    std::vector<uint8_t> attributes_ {};  // A set of values of the "PlayerApplicationSettingAttributeID".
};

/******************************************************************
 * ListPlayerApplicationSettingValues                             *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>ListPlayerApplicationSettingValues<b> frame.
 */
enum AvrcCtLpasv {
    AVRC_CT_LPASV_PARAMETER_LENGTH = 0x0001,  // The value of the "Parameter Length".
    AVRC_CT_LPASV_NUM_OF_VALUES = 0x0000,     // The default value of the "NumPlayerApplicationSettingValues".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>ListPlayerApplicationSettingValues</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.2 ListPlayerApplicationSettingValues.
 */
class AvrcCtLpasvPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtLpasvPacket</b> instance.
     */
    explicit AvrcCtLpasvPacket(uint8_t attributes);

    /**
     * @brief A constructor used to create an <b>AvrcCtLpasvPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtLpasvPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtLpasvPacket</b> instance.
     */
    ~AvrcCtLpasvPacket();

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
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID                 1 octets<br>
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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingValues                   1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID  1                  1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID  n                  1 octets<br>
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
     * @brief Gets the "PlayerApplicationSettingAttributeID".
     *
     * @return The values of the "PlayerApplicationSettingValueID".
     */
    uint8_t GetAttribute(void) const
    {
        return attribute_;
    }

    /**
     * @brief Gets the "PlayerApplicationSettingValueID".
     *
     * @return The values of the "PlayerApplicationSettingValueID".
     */
    const std::vector<uint8_t> &GetValues(void) const
    {
        return values_;
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
    uint16_t numOfValues_ {AVRC_CT_LPASV_NUM_OF_VALUES};  // The value of the "NumPlayerApplicationSettingValues".
    std::vector<uint8_t> values_ {};                      // A set of values of the "PlayerApplicationSettingValueID".

    /**
     * @brief A constructor used to create an <b>AvrcCtLpasvPacket</b> instance.
     */
    AvrcCtLpasvPacket() = delete;
};

/******************************************************************
 * GetCurrentPlayerApplicationSettingValue                        *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>GetCurrentPlayerApplicationSettingValue<b>
 * frame.
 */
enum AvrcCtGcpasv {
    AVRC_CT_GCPASV_NUM_OF_ATTRIBUTES = 0x0000,     // The value of the "NumPlayerApplicationSettingAttributeID".
    AVRC_CT_GCPASV_NUM_OF_VALUES = 0x0000,         // The value of the "NumPlayerApplicationSettingValues".
    AVRC_CT_GCPASV_NUM_OF_ATTRIBUTES_SIZE = 0x01,  // The size of the "NumPlayerApplicationSettingAttributeID".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>GetCurrentPlayerApplicationSettingValue</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.3 GetCurrentPlayerApplicationSettingValue.
 */
class AvrcCtGcpasvPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtGcpasvPacket</b> instance.
     */
    explicit AvrcCtGcpasvPacket(const std::vector<uint8_t> &attributes);

    /**
     * @brief A constructor used to create an <b>AvrcCtGcpasvPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtGcpasvPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtGcpasvPacket</b> instance.
     */
    ~AvrcCtGcpasvPacket();

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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingAttributeID              1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID 1               1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID n               1 octets<br>
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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingValues                   1 octets<br>
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
     * @brief Gets the "Packet Type".
     *
     * @return The value of the "Packet Type".
     */
    uint8_t GetPacketType(void) const
    {
        return packetType_;
    }

    /**
     * @brief Gets the "PlayerApplicationSettingAttributeID".
     *
     * @return The values of the "PlayerApplicationSettingValueID".
     */
    const std::vector<uint8_t> &GetAttributes(void) const
    {
        return attributes_;
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
     * @brief Gets the "PlayerApplicationSettingValueID".
     *
     * @return The values of the "PlayerApplicationSettingValueID".
     */
    const std::vector<uint8_t> &GetValues(void) const
    {
        return values_;
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
    uint8_t numOfAttributes_ {AVRC_CT_GCPASV_NUM_OF_ATTRIBUTES};
    std::vector<uint8_t> attributes_ {};  // The value of the "PlayerApplicationSettingAttributeID".
    // The value of the "NumPlayerApplicationSettingValues". Allowed Values: 1-255.
    uint8_t numOfValues_ {AVRC_CT_GCPASV_NUM_OF_VALUES};
    std::vector<uint8_t> values_ {};  // A set of values of the "PlayerApplicationSettingValueID".

    /**
     * @brief A constructor used to create an <b>AvrcCtGcpasvPacket</b> instance.
     */
    AvrcCtGcpasvPacket() = delete;
};

/******************************************************************
 * SetPlayerApplicationSettingValue                               *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>SetPlayerApplicationSettingValue<b>
 * frame.
 */
enum AvrcCtSpasv {
    AVRC_CT_SPASV_NUM_OF_ATTRIBUTES = 0x0000,     // The value of the "NumPlayerApplicationSettingAttributeID".
    AVRC_CT_SPASV_NUM_OF_VALUES = 0x0000,         // The value of the "NumPlayerApplicationSettingValues".
    AVRC_CT_SPASV_NUM_OF_ATTRIBUTES_SIZE = 0x01,  // The size of the "NumPlayerApplicationSettingAttributeID".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>SetPlayerApplicationSettingValue</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.4 SetPlayerApplicationSettingValue.
 */
class AvrcCtSpasvPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtSpasvPacket</b> instance.
     */
    AvrcCtSpasvPacket(const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values);

    /**
     * @brief A constructor used to create an <b>AvrcCtSpasvPacket</b> instance.
     */
    explicit AvrcCtSpasvPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtSpasvPacket</b> instance.
     */
    ~AvrcCtSpasvPacket();

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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingAttributeID              1 octets<br>
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
     * @brief Gets the "PlayerApplicationSettingAttributeID".
     *
     * @return The values of the "PlayerApplicationSettingValueID".
     */
    const std::vector<uint8_t> &GetAttributes(void) const
    {
        return attributes_;
    }

    /**
     * @brief Gets the "PlayerApplicationSettingValueID".
     *
     * @return The values of the "PlayerApplicationSettingValueID".
     */
    const std::vector<uint8_t> &GetValues(void) const
    {
        return values_;
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
    uint8_t numOfAttributes_ {AVRC_CT_SPASV_NUM_OF_ATTRIBUTES};
    std::vector<uint8_t> attributes_ {};  // The value of the "PlayerApplicationSettingAttributeID".
    std::vector<uint8_t> values_ {};      // A set of values of the "PlayerApplicationSettingValueID".

    /**
     * @brief A constructor used to create an <b>AvrcCtSpasvPacket</b> instance.
     */
    AvrcCtSpasvPacket() = delete;
};

/******************************************************************
 * GetPlayerApplicationSettingAttributeText                       *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>GetPlayerApplicationSettingAttributeText<b>
 * frame.
 */
enum AvrcCtGpasat {
    AVRC_CT_GPASAT_ATTRIBUTE_NUM_LENGTH = 0x01,  // The Length of "AttributeNum".
    AVRC_CT_GPASAT_NUM_OF_ATTRIBUTE = 0x00,      // The Number of "Attribute".
    AVRC_CT_GPASAT_VALUE_LENGTH = 0x00,          // The Length of "Attribute Name".
};

/**
 * @brief This class provides a set of methods associated with assemble / disassemble the packet of the
 * <b>GetPlayerApplicationSettingAttributeText<b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.5 GetPlayerApplicationSettingAttributeText.
 */
class AvrcCtGpasatPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtGpasatPacket</b> instance.
     */
    explicit AvrcCtGpasatPacket(std::vector<uint8_t> attributes);

    /**
     * @brief A constructor used to create an <b>AvrcCtGpasatPacket</b> instance.
     */
    explicit AvrcCtGpasatPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtGpasatPacket</b> instance.
     */
    ~AvrcCtGpasatPacket();

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
     *                        0 0 0 0 | 0 0 0 0     AttributeNum                                        1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID                 N octets
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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingAttributes               1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID  1              1 octets
     *                        0 0 0 0 | 0 0 0 0     CharacterSetID1                                     1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttribtueID  N              1 octets
     * @param[in] buffer The buffer of the frame.
     * @return @c true  : The packet is valid.
     *         @c false : The packet is invalid.
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
     * @brief Gets the values of the "PlayerApplicationSettingAttribtueID".
     *
     * @return The values of the "PlayerApplicationSettingAttribtueID".
     */
    const std::vector<uint8_t> &GetAttributes(void) const
    {
        return attributes_;
    }

    /**
     * @brief Clears the "PlayerApplicationSettingAttribtueID".
     *
     * @return The ID of the "PlayerApplicationSettingAttribtueID".
     */
    void ClearAttributes(void)
    {
        attributes_.clear();
    }

    /**
     * @brief Clears the "PlayerApplicationSettingAttributeString".
     *
     * @return The values of the "PlayerApplicationSettingAttributeString".
     */
    void ClearValueName(void)
    {
        attributeName_.clear();
    }

    /**
     * @brief Gets the "Attributes".
     *
     * @return The values of the "AttributesNameString".
     */
    const std::vector<std::string> &GetAttributeName(void) const
    {
        return attributeName_;
    }

private:
    uint16_t numOfAttributes_ {AVRC_CT_GPASAT_NUM_OF_ATTRIBUTE};
    // The value of the "NumPlayerApplicationSettingAttributes".
    std::vector<uint8_t> attributes_ {};
    // A set of values of the "PlayerApplicationSettingAttributeID".
    std::vector<std::string> attributeName_ {};  // Specifies the player application setting attribute name.
    uint8_t AttributeValueLength_ {AVRC_CT_GPASAT_VALUE_LENGTH};
    // Length of the player application setting value string.

    /**
     * @brief A constructor used to create an <b>AvrcCtGpasatPacket</b> instance.
     */
    AvrcCtGpasatPacket() = delete;
};

/******************************************************************
 * GetPlayerApplicationSettingValueText                           *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>GetPlayerApplicationSettingValueText<b>
 * frame.
 */
enum AvrcCtGpasvt {
    AVRC_CT_GPASVT_PARAMETER_LENGTH_SIZE = 0x0002,  // The Length of the "Parameter Length".
    AVRC_CT_GPASVT_NUM_OF_VALUE = 0x00,             // The Number of value.
    AVRC_CT_GPASVT_VALUE_LENGTH = 0x00,             // The Length of value.
};

/**
 * @brief This class provides a set of methods associated with assemble / disassemble the packet of the
 * <b>GetPlayerApplicationSettingValueText<b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.5.6 GetPlayerApplicationSettingValueText.
 */
class AvrcCtGpasvtPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtGpasvtPacket</b> instance.
     */
    AvrcCtGpasvtPacket(uint8_t attribtueId, std::vector<uint8_t> values);

    /**
     * @brief A constructor used to create an <b>AvrcCtGpasvtPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtGpasvtPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtGpasvtPacket</b> instance.
     */
    ~AvrcCtGpasvtPacket();

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
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingAttributeID                 1 octets
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingValue                    1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID                     N octets
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
     *                        0 0 0 0 | 0 0 0 0     NumPlayerApplicationSettingAttributes               1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueID  1                  1 octets
     *                        0 0 0 0 | 0 0 0 0     CharacterSetID1                                     2 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueStringLength  1        1 octets
     *                        0 0 0 0 | 0 0 0 0     PlayerApplicationSettingValueString1                1 octets
     * @param[in] buffer The buffer of the frame.
     * @return @c true  : The packet is valid.
     *         @c false : The packet is invalid.
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
     * @brief Gets the values of the "PlayerApplicationSettingAttribtueID".
     *
     * @return The values of the "PlayerApplicationSettingAttribtueID".
     */
    const std::vector<uint8_t> &GetValues(void) const
    {
        return values_;
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
     * @brief Clears the "PlayerApplicationSettingValueString".
     *
     * @return The values of the "PlayerApplicationSettingValueString".
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
    bool IsValidAttribute(void) const;

private:
    uint8_t attributeId_ {AVRC_PLAYER_ATTRIBUTE_ILLEGAL};  // Player application setting attribute ID.
    uint16_t numOfValues_ {AVRC_CT_GPASVT_NUM_OF_VALUE};
    // The value of the "NumPlayerApplicationSettingValues". Allowed Values: 1-255.
    std::vector<uint8_t> values_ {};    // Player application setting value ID.
    std::vector<std::string> valueName_ {};  // Specifies the value string of player application setting value.
    uint8_t AttributeValueLength_ {AVRC_CT_GPASVT_VALUE_LENGTH};
    // Length of the player application setting value string.

    /**
     * @brief A constructor used to create an <b>AvrcCtGpasvtPacket</b> instance.
     */
    AvrcCtGpasvtPacket() = delete;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_CT_VENDOR_PLAYER_APPLICATION_SETTINGS_H