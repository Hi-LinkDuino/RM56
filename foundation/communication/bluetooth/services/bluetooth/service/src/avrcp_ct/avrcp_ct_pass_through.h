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

#ifndef AVRCP_CT_PASS_THROUGH_H
#define AVRCP_CT_PASS_THROUGH_H

#include "avrcp_ct_internal.h"
#include "avrcp_ct_packet.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the values of the <b>PASS THROUGH</b> command.
 */
enum AvrcCtPass {
    AVRC_CT_PASS_COMMAND_SIZE = 0x0005,               // The size of the command frame.
    AVRC_CT_PASS_RESPONSE_SIZE = 0x0005,              // The size of the response frame.
    AVRC_CT_PASS_OPERATION_DATA_FIELD_LENGTH = 0x00,  // The value of "Operation data field length".
    AVRC_CT_PASS_OPERATION_ID_OFFSET = 0x03,          // The offset of the "Operation ID".
};

/**
 * @brief This enumeration declares the values of the moving bit
 */
enum AvrcCtPassMoveBit {
    AVRC_CT_PASS_MOVE_BIT_3 = 0x03,
    AVRC_CT_PASS_MOVE_BIT_7 = 0x07,
};

/**
 * @brief This class provides a set of methods for assembling / disassembling the frame packet of the <b>PASS
 * THROUGH</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 4.4.1 PASS THROUGH command / 25.3 PASS THROUGH command.
 */
class AvrcCtPassPacket : public AvrcCtPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtPassPacket</b> instance.
     */
    AvrcCtPassPacket();

    /**
     * @brief A constructor used to create an <b>AvrcCtPassPacket</b> instance.
     */
    AvrcCtPassPacket(uint8_t oper, uint8_t state);

    /**
     * @brief A constructor used to create an <b>AvrcCtPassPacket</b> instance.
     */
    explicit AvrcCtPassPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtPassPacket</b> instance.
     */
    ~AvrcCtPassPacket(void);

    /**
     * @brief Assembles the frame packet.
     *
     * @details Command frame:<br>
     *                        msb           lsb<br>
     *                        0 0 0 0 | 0 0 0 0     ctype                           4 bits<br>
     * subunit_type 5 bits    0 0 0 0 0 | 0 0 0     subunit_ID                      3 bits<br>
     *                        0 0 0 0 | 0 0 0 0     opcode                          1 octets<br>
     * state_flag   1 bit     0 | 0 0 0 0 0 0 0     operation_ID                    7 bits<br>
     *                        0 0 0 0 | 0 0 0 0     operation_data_field_length     1 octets<br>
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
     * state_flag   1 bit     0 | 0 0 0 0 0 0 0     operation_ID                    7 bits<br>
     *                        0 0 0 0 | 0 0 0 0     operation_data_field_length     1 octets<br>
     * @param[in] pkt The frame packet.
     * @return The result of the method execution.
     * @retval true  The packet is valid.
     * @retval false The packet is invalid.
     */
    bool DisassemblePacket(Packet *pkt) override;

    /**
     * @brief Gets the "response".
     *
     * @return The value of the "response".
     */
    uint8_t GetCrCode(void) const
    {
        return crCode_;
    }

    /**
     * @brief Gets the "state_flag".
     *
     * @return The value of the "state_flag".
     */
    uint8_t GetKeyState(void) const
    {
        return stateFlag_;
    };

    /**
     * @brief Gets the "operation_ID".
     *
     * @return The value of the "operation_ID".
     */
    uint8_t GetKeyOperation(void) const
    {
        return operationId_;
    };

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

    /**
     * @brief Checks the "state_flag" is valid or not.
     *
     * @return The result of the method execution.
     * @retval true  The key state is valid.
     * @retval false The key state is invalid.
     */
    bool IsValidKeyState(void) const;

    /**
     * @brief Checks the specified "state_flag" is valid or not.
     *
     * @param[in] state value of the "state_flag".
     * @return The result of the method execution.
     * @retval true  The key state is valid.
     * @retval false The key state is invalid.
     */
    static bool IsValidKeyState(uint8_t state);

    /**
     * @brief Checks the "operation_ID" is supported or not.
     *
     * @return The result of the method execution.
     * @retval true  The key operation is supported.
     * @retval false The key operation is unsupported.
     */
    bool IsSupportedKeyOperation(void) const;

    /**
     * @brief Checks the specified "operation_ID" is supported or not.
     *
     * @param key The value of the "operation_ID".
     * @return The result of the method execution.
     * @retval true  The key operation is supported.
     * @retval false The key operation is unsupported.
     */
    static bool IsSupportedKeyOperation(uint8_t key);

private:
    uint8_t crCode_ {AVRC_CT_CMD_CODE_CONTROL};              // The value of the "ctype" or the "response".
    uint8_t subunitType_ {AVRC_CT_AVC_COMMON_SUBUNIT_TYPE};  // The value of the "Subunit_type".
    uint8_t subunitId_ {AVRC_CT_AVC_COMMON_SUBUNIT_ID};      // The value of the "Subunit_ID".
    uint8_t opCode_ {AVRC_CT_OP_CODE_PASS_THROUGH};          // The value of the "Opcode".
    uint8_t stateFlag_ {AVRC_KEY_STATE_INVALID};             // The value of the "State flag".
    uint8_t operationId_ {AVRC_KEY_OPERATION_INVALID};       // The value of the "Operation_ID".
    uint8_t operationDataFieldLength_ {AVRC_CT_PASS_OPERATION_DATA_FIELD_LENGTH};
    // The value of the "Operation_data_field_length".
    Packet *pkt_ {nullptr};                          // The frame packet.
    bool isValid_ {false};                           // The frame packet is valid or not.

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcCtPassPacket);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_CT_PASS_THROUGH_H
