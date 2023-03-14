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

#ifndef AVRCP_CT_VENDOR_CONTINUATION_H
#define AVRCP_CT_VENDOR_CONTINUATION_H

#include "avrcp_ct_vendor.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
/******************************************************************
 * RequestContinuingResponse                                      *
 ******************************************************************/
/**
 * @brief This enumeration declares a set of values of the <b>RequestContinuingResponse</b> command.
 */
enum AvrcCtRcr {
    AVRC_CT_RCR_PARAMETER_LENGTH = 0x0001,  // The value of the "Parameter Length".
};

/**
 * @brief This class provides a set of methods of assemble / disassemble the packet of the
 * <b>RequestContinuingResponse</b> frame.
 * @see Audio/Video Remote Control 1.6.2 Section 6.8.1 RequestContinuingResponse.
 */
class AvrcCtRcrPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtRcrPacket</b> instance.
     */
    AvrcCtRcrPacket();

    /**
     * @brief A constructor used to create an <b>AvrcCtRcrPacket</b> instance.
     */
    explicit AvrcCtRcrPacket(uint8_t pduId);

    /**
     * @brief A destructor used to delete the <b>AvrcCtRcrPacket</b> instance.
     */
    ~AvrcCtRcrPacket();

    /**
     * @brief Assembles the operands behind the "PDU ID" of the frame.
     *
     * @retval nullptr Always return nullptr.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands behind the "Packet Type" of the frame.
     *
     * @details response frame:
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
     * @brief Gets the "ContinuePDU_ID".
     *
     * @return The value of the "ContinuePDU_ID".
     */
    uint8_t GetRequestContinuingPduId(void) const
    {
        return continuePduId_;
    }

private:
    uint8_t continuePduId_ {AVRC_CT_PDU_ID_INVALID};  // The "PDU ID" in the frame.
};

/******************************************************************
 * AbortContinuingResponse                                        *
 ******************************************************************/

/**
 * @brief This enumeration declares a set of values associated with the <b>AbortContinuingResponse</b>.
 */
enum AvrcCtAcr {
    AVRC_CT_ACR_PARAMETER_LENGTH = 0x0001,  // The value of the "Parameter Length".
};

/**
 * @brief This class provides a set of methods associated with assemble / disassemble the packet of the
 * <b>AbortContinuingResponse</b> command.
 * @see Audio/Video Remote Control 1.6.2 Section 6.8.1 AbortContinuingResponse.
 */
class AvrcCtAcrPacket : public AvrcCtVendorPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtAcrPacket</b> instance.
     */
    AvrcCtAcrPacket();

    /**
     * @brief A constructor used to create an <b>AvrcCtAcrPacket</b> instance.
     */
    explicit AvrcCtAcrPacket(uint8_t pduId);

    /**
     * @brief A constructor used to create an <b>AvrcCtAcrPacket</b> instance.
     */
    explicit AvrcCtAcrPacket(Packet *pkt);

    /**
     * @brief A destructor used to delete the <b>AvrcCtAcrPacket</b> instance.
     */
    ~AvrcCtAcrPacket();

    /**
     * @brief Assembles the operands that behind the "Packet Type" of the frame.
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
     *                        0 0 0 0 | 0 0 0 0     PDU ID                                              1 octets<br>
     * @return The frame packet.
     */
    Packet *AssembleParameters(Packet *pkt) override;

    /**
     * @brief Disassembles the operands that behind the "Packet Type" of the frame.
     *
     * @details response frame:
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
     * @brief Gets the "ContinuePDU_ID".
     *
     * @return The value of the "ContinuePDU_ID".
     */
    uint8_t GetRequestContinuingPduId(void) const
    {
        return continuePduId_;
    }

private:
    uint8_t continuePduId_ {AVRC_CT_PDU_ID_INVALID};  // The "PDU ID" in the frame.
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_CT_VENDOR_CONTINUATION_H