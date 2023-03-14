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
 * @file avrcp_ct_vendor_continuation.h
 *
 * @brief Declares the class of the VENDOR DEPENDENT command related to the continuation PDUS, including attributes and
 * methods.
 */

#ifndef AVRCP_TG_VENDOR_CONTINUATION_H
#define AVRCP_TG_VENDOR_CONTINUATION_H

#include "avrcp_tg_vendor.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
/******************************************************************
 * RequestContinuingResponse                                      *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values of the <b>RequestContinuingResponse</b> command.
 */
enum AvrcTgRcr {
    AVRC_TG_RCR_REQUEST_CONTINUING_PDU_ID_OFFSET = 0x000A,  // The offset of the "ContinuePDU_ID".
    AVRC_TG_RCR_NUM_OF_PACKETS = 0x0000,                    // The number of the packets.
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>RequestContinuingResponse</b> frame.
 * @see Audio/Video Remote Control 1.6.2 Section 6.8.1 RequestContinuingResponse.
 */
class AvrcTgRcrPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgRcrPacket</b> instance.
     */
    AvrcTgRcrPacket();

    /**
     * @brief A constructor used to create an <b>AvrcTgRcrPacket</b> instance.
     */
    AvrcTgRcrPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgRcrPacket</b> instance.
     */
    ~AvrcTgRcrPacket();

    /**
     * @brief Assembles the operands behind the "PDU ID" of the frame.
     *
     * @retval nullptr Always return nullptr.
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
     *
     * Reserved     7 bits    0 0 0 0 0 0 0 | 0     Packet Type                                         1 bits<br>
     *                        0 0 0 0 | 0 0 0 0     Parameter Length                                    2 octets<br>
     *                        0 0 0 0 | 0 0 0 0     ContinuePDU_ID                                      1 octets<br>
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
     * @brief Gets the "ContinuePDU_ID".
     *
     * @return The value of the "ContinuePDU_ID".
     */
    uint8_t GetRequestContinuingPduId(void) const
    {
        return continuePduId_;
    }

private:
    uint8_t continuePduId_ {AVRC_TG_PDU_ID_INVALID};  // The value of the "ContinuePDU_ID".
};

/******************************************************************
 * AbortContinuingResponse                                        *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values of the <b>AbortContinuingResponse</b> command.
 */
enum AvrcTgAcr {
    AVRC_TG_ACR_PARAMETER_LENGTH = 0x0000,                  // The value of the "Parameter Length".
    AVRC_TG_ACR_REQUEST_CONTINUING_PDU_ID_OFFSET = 0x000A,  // The offset of the "ContinueAbort PDU_ID".
    AVRC_TG_ACR_NUM_OF_PACKETS = 0x0001,                    // The number of the packets.
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>AbortContinuingResponse</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.8.2 AbortContinuingResponse.
 */
class AvrcTgAcrPacket : public AvrcTgVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgAcrPacket</b> instance.
     */
    AvrcTgAcrPacket();

    /**
     * @brief A constructor used to create an <b>AvrcTgAcrPacket</b> instance.
     */
    AvrcTgAcrPacket(uint8_t pduId, uint8_t label);

    /**
     * @brief A constructor used to create an <b>AvrcTgAcrPacket</b> instance.
     */
    AvrcTgAcrPacket(Packet *pkt, uint8_t label);

    /**
     * @brief A destructor used to delete the <b>AvrcTgAcrPacket</b> instance.
     */
    ~AvrcTgAcrPacket();

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
     *                        0 0 0 0 | 0 0 0 0     ContinuePDU_ID                                      1 octets<br>
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
     * @brief Gets the "ContinuePDU_ID".
     *
     * @return The value of the "ContinuePDU_ID".
     */
    uint8_t GetRequestContinuingPduId(void) const
    {
        return continuePduId_;
    }

private:
    uint8_t continuePduId_ {AVRC_TG_PDU_ID_INVALID};  // The value of the "ContinuePDU_ID".
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_VENDOR_CONTINUATION_H