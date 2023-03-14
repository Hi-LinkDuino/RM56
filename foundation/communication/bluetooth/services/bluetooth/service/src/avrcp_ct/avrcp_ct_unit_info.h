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

#ifndef AVRCP_CT_UNIT_INFO_H
#define AVRCP_CT_UNIT_INFO_H

#include "avrcp_ct_packet.h"
#include "base_def.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the values of the <b>UNIT INFO</b> command.
 */
enum AvrcCtUnit {
    AVRC_CT_UNIT_COMMAND_SIZE = 0x0008,     // The size of the command frame.
    AVRC_CT_UNIT_RESPONSE_SIZE = 0x0008,    // The size of the response frame.
    AVRC_CT_UNIT_SUBUNIT_TYPE_UNIT = 0x1F,  // The value of the "Subunit_type".
    AVRC_CT_UNIT_SUBUNIT_ID_IGNORE = 0x07,  // The value of the "Subunit_ID".
    AVRC_CT_UNIT_OCTET_3 = 0xFF,            // The fixed value and no name.
    AVRC_CT_UNIT_COMPANY_ID_OFFSET = 0x05,  // The offset of the "Company ID".
};

/**
 * @brief This enumeration declares the values of the moving bit
 */
enum AvrcCtUnitMoveBit {
    AVRC_CT_UNIT_MOVE_BIT_3 = 0x03,
};
/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the <b>UNIT INFO</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 4.2.1 UNIT INFO command / 25.1 UNIT INFO command.
 */
class AvrcCtUnitPacket : public AvrcCtPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtUnitPacket</b> instance.
     */
    AvrcCtUnitPacket();

    /**
     * @brief A constructor used to create an <b>AvrcCtUnitPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    explicit AvrcCtUnitPacket(Packet *pkt);

    /**
     * @brief A constructor used to create an <b>AvrcCtUnitPacket</b> instance.
     */
    virtual ~AvrcCtUnitPacket(void);

    /**
     * @brief Assembles the frame packet.
     *
     * @details Command frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     ctype                           4 bits<br>
     * subunit_type 5 bits    0 0 0 0 0 | 0 0 0     subunit_ID                      3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     opcode                          1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the frame packet.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                        4 bits<br>
     * subunit_type 5 bits    0 0 0 0 0 | 0 0 0     subunit_ID                      3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     opcode                          1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     * uint_type    5 bits    0 0 0 0 0 | 0 0 0     unit                            2 bits<br>
     *                        0 0 0 0 | 0 0 0 0     company_ID                      1 octets<br>
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePacket(Packet *pkt) override;

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
     * @brief Gets the "opcode".
     *
     * @return The value of the "opcode".
     */
    uint8_t GetOpCode(void) const
    {
        return opCode_;
    }

protected:
    uint8_t crCode_ {AVRC_CT_CMD_CODE_STATUS};                       // The value of the "ctype" or the "response".
    uint8_t subunitType_ {AVRC_CT_UNIT_SUBUNIT_TYPE_UNIT};           // The value of the "Subunit_type".
    uint8_t subunitId_ {AVRC_CT_UNIT_SUBUNIT_ID_IGNORE};             // The value of the "Subunit_ID".
    uint8_t opCode_ {AVRC_CT_OP_CODE_UNIT_INFO};                     // The value of the "Opcode".
    uint8_t unit_type_ {AVRC_CT_AVC_COMMON_SUBUNIT_TYPE};            // The value of the "Unit_type".
    uint8_t unit_ {AVRC_CT_AVC_COMMON_SUBUNIT_ID};                   // The value of the "Unit".
    uint32_t companyId_ {AVRC_CT_DEFAULT_BLUETOOTH_SIG_COMPANY_ID};  // The value of the "Company ID".
    Packet *pkt_ {nullptr};                                          // The frame packet.
    bool isValid_ {false};                                           // The packet is valid or not.

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcCtUnitPacket);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_CT_UNIT_INFO_H
