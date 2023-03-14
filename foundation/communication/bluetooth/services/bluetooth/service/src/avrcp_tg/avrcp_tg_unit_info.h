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
 * @file avrcp_tg_unit_info.h
 *
 * @brief Declares the class of the UNIT INFO command, including attributes and methods.
 */

#ifndef AVRCP_TG_UNIT_INFO_H
#define AVRCP_TG_UNIT_INFO_H

#include "avrcp_tg_internal.h"
#include "avrcp_tg_packet.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the values of the <b>UNIT INFO</b> command.
 */
enum AvrcTgUnit {
    AVRC_TG_UNIT_COMMAND_SIZE = 0x0008,     // The size of the command frame.
    AVRC_TG_UNIT_RESPONSE_SIZE = 0x0008,    // The size of the response frame.
    AVRC_TG_UNIT_SUBUNIT_TYPE_UNIT = 0x1F,  // The value of the "Subunit_type".
    AVRC_TG_UNIT_SUBUNIT_ID_IGNORE = 0x07,  // The value of the "Subunit_ID".
    AVRC_TG_UNIT_OCTET_3 = 0x07,            // The fixed value and no name.
    AVRC_TG_UNIT_COMPANY_ID_OFFSET = 0x05,  // The offset of the "Company ID".
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the <b>UNIT INFO</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 4.2.1 UNIT INFO command / 25.1 UNIT INFO command.
 */
class AvrcTgUnitPacket : public AvrcTgPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgUnitPacket</b> instance.
     */
    AvrcTgUnitPacket();

    /**
     * @brief A constructor used to create an <b>AvrcTgUnitPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgUnitPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgUnitPacket</b> instance.
     */
    virtual ~AvrcTgUnitPacket(void);

    /**
     * @brief Assembles the frame packet.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                        4 bits<br>
     * subunit_type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                      3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     opcode                          1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     * uint_type    5 bits    0 0 0 0 0 | 0 0 0     unit                            2 bits<br>
     *                        0 0 0 0 | 0 0 0 0     company_ID                      1 octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the frame packet.
     *
     * @details Command frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     ctype                           4 bits<br>
     * subunit_type 5 bits    0 0 0 0 0 | 0 0 0     Subunit ID                      3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     opcode                          1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the "opcode".
     *
     * @return The value of the "opcode".
     */
    uint8_t GetOpCode(void) const
    {
        return opCode_;
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

protected:
    uint8_t crCode_ {AVRC_TG_RSP_CODE_STABLE};                       // The value of the "ctype" or the "response".
    uint8_t subunitType_ {AVRC_TG_UNIT_SUBUNIT_TYPE_UNIT};           // The value of the "Subunit_type".
    uint8_t subunitId_ {AVRC_TG_UNIT_SUBUNIT_ID_IGNORE};             // The value of the "Subunit_ID".
    uint8_t opCode_ {AVRC_TG_OP_CODE_UNIT_INFO};                     // The value of the "Opcode".
    uint8_t unit_type_ {AVRC_TG_AVC_COMMON_SUBUNIT_TYPE};            // The value of the "Unit_type".
    uint8_t unit_ {AVRC_TG_AVC_COMMON_SUBUNIT_ID};                   // The value of the "Unit".
    uint32_t companyId_ {AVRC_TG_DEFAULT_BLUETOOTH_SIG_COMPANY_ID};  // The value of the "Company ID".
    Packet *pkt_ {nullptr};                                          // The frame packet.
    uint8_t label_ {AVRC_DEFAULT_LABEL};  // The label which is used to distinguish different call.
    bool isValid_ {false};                // The packet is valid or not.

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcTgUnitPacket);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_UNIT_INFO_H
