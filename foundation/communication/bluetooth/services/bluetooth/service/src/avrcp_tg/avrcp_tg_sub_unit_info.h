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

#ifndef AVRCP_TG_SUB_UNIT_INFO_H
#define AVRCP_TG_SUB_UNIT_INFO_H

#include "avrcp_tg_internal.h"
#include "avrcp_tg_packet.h"
#include "avrcp_tg_unit_info.h"
#include "packet.h"

/**
 * @brief The bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the values of the SUB UNIT INFO command.
 */
using AvrcTgSubUnit = enum {
    AVRC_TG_SUB_UNIT_COMMAND_SIZE = 0x0008,     // The size of the command frame.
    AVRC_TG_SUB_UNIT_RESPONSE_SIZE = 0x0008,    // The size of the response frame.
    AVRC_TG_SUB_UNIT_SUBUNIT_TYPE_UNIT = 0x1F,  // The value of the "Subunit_type".
    AVRC_TG_SUB_UNIT_SUBUNIT_ID_IGNORE = 0x07,  // The value of the "Subunit_ID".
    AVRC_TG_SUB_UNIT_PAGE = 0x00,               // The value of the "Page".
    AVRC_TG_SUB_UNIT_EXTENTION_CODE = 0x07,     // The value of the "Extention_code".
    AVRC_TG_SUB_UNIT_OCTET_4 = 0xFF,            // The fixed value and no name.
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the packet of the <b>SUB UNIT INFO</b>
 * command.
 * @see Audio/Video Remote Control 1.6.2 Section 4.2.1 SUB UNIT INFO command / 25.1 SUB UNIT INFO command.
 */
class AvrcTgSubUnitPacket : public AvrcTgUnitPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgSubUnitPacket</b> instance.
     */
    AvrcTgSubUnitPacket(void);

    /**
     * @brief A constructor used to create an <b>AvrcTgSubUnitPacket</b> instance.
     *
     * @details You can use this constructor when wants to disassemble the packet.
     */
    AvrcTgSubUnitPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgSubUnitPacket</b> instance.
     */
    ~AvrcTgSubUnitPacket(void);

    /**
     * @brief Assembles the frame packet.
     *
     * @details Response frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     response                        4 bits<br>
     * subunit_type 5 bits    0 0 0 0 0 | 0 0 0     subunit_ID                      3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     opcode                          1 octets<br>
     * page         3 bits    0 0 0 0 | 0 0 0 0     extention_code                  3 bits<br>
     * subunit_type 5 bits    0 0 0 0 0 | 0 0 0     max_subunit_ID                  3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     *                        0 0 0 0 | 0 0 0 0     fixed value                     1 octets<br>
     * @return The frame packet.
     */
    const Packet *AssemblePacket(void) override;

    /**
     * @brief Disassembles the frame packet.
     *
     * @details Command frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     ctype                           4 bits<br>
     * subunit_type 5 bits    0 0 0 0 0 | 0 0 0     subunit_ID                      3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     opcode                          1 octets<br>
     * page         3 bits    0 0 0 0 | 0 0 0 0     extention_code                  3 bits<br>
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

private:
    uint8_t page_ {AVRC_TG_SUB_UNIT_PAGE};                     // The value of the "Page".
    uint8_t extentionCode_ {AVRC_TG_SUB_UNIT_EXTENTION_CODE};  // The value of the "Extention_code".

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcTgSubUnitPacket);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_SUB_UNIT_INFO_H
