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

#ifndef AVRCP_TG_PACKET_H
#define AVRCP_TG_PACKET_H

#include "avrcp_tg_internal.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
const uint32_t AVRC_TG_INVALID_BLUETOOTH_SIG_COMPANY_ID = 0xFFFFFF;

/**
 * @brief This enumeration declares the values of the frame.
 */
enum AvrcTgAvcCommon {
    AVRC_TG_AVC_COMMON_SUBUNIT_TYPE = 0x09,          // The value of the "Subunit_type".
    AVRC_TG_AVC_COMMON_SUBUNIT_ID = 0x00,            // The value of the "Subunit_ID".
    AVRC_TG_AVC_COMMON_PARAMETER_LENGTH = 0x00,      // The value of the "Parameter Length".
    AVRC_TG_AVC_COMMON_PACKET_TYPE = 0x00,           // The value of the "Packet type".
    AVRC_TG_AVC_COMMON_PACKET_TYPE_START = 0x01,     // The value of the "Start".
    AVRC_TG_AVC_COMMON_PACKET_TYPE_CONTINUE = 0x02,  // The value of the "Continue".
    AVRC_TG_AVC_COMMON_PACKET_TYPE_END = 0x03,       // The value of the "End".
    AVRC_TG_AVC_COMMON_CTYPE_OFFSET = 0x00,          // The offset of the "ctype".
    AVRC_TG_AVC_COMMON_OPCODE_OFFSET = 0x02,         // The offset of the "opcode".
    AVRC_TG_AVC_COMMON_COMPANY_ID_OFFSET = 0x03,     // The offset of the "Company ID".
    AVRC_TG_AVC_COMMON_VENDOR_PDU_ID_OFFSET = 0x06,  // The offset of the "PDU ID".
    AVRC_TG_AVC_COMMON_BROWSE_PDU_ID_OFFSET = 0x00,  // The offset of the "PDU ID".
};

/**
 * @brief This enumeration declares the values of the "ctype" and "response".
 */
enum AvrcTgCrCode {
    AVRC_TG_CMD_CODE_CONTROL = 0x00,          // Control command.
    AVRC_TG_CMD_CODE_STATUS = 0x01,           // Status command.
    AVRC_TG_CMD_CODE_NOTIFY = 0x03,           // Notify command.
    AVRC_TG_RSP_CODE_NOT_IMPLEMENTED = 0x08,  // Not implemented command.
    AVRC_TG_RSP_CODE_ACCEPTED = 0x09,         // Accepted command.
    AVRC_TG_RSP_CODE_REJECTED = 0x0A,         // Rejected command.
    AVRC_TG_RSP_CODE_STABLE = 0x0C,           // Status stable.
    AVRC_TG_RSP_CODE_CHANGED = 0x0D,          // Status changed.
    AVRC_TG_RSP_CODE_INTERIM = 0x0F,          // Interim response.
};

/**
 * @brief This enumeration declares the values of the "Opcode".
 */
enum AvrcTgOpCode {
    AVRC_TG_OP_CODE_VENDOR = 0x00,         // Vendor Dependent command.
    AVRC_TG_OP_CODE_UNIT_INFO = 0x30,      // Unit Info command.
    AVRC_TG_OP_CODE_SUB_UNIT_INFO = 0x31,  // Subunit Info command.
    AVRC_TG_OP_CODE_PASS_THROUGH = 0x7C,   // Pass Through command.
    // @see AV/C Digital Interface Command Set Version 1.0 Section 5.AV/C frames - Operation(opcode).
    AVRC_TG_OP_CODE_INVALID = 0xFF,
};

/**
 * @brief This enumeration declares the values of the "PDU ID".
 */
enum AvrcTgPduId {
    AVRC_TG_PDU_ID_GET_CAPABILITIES = 0x10,                               // AV/C STATUS
    AVRC_TG_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES = 0x11,     // AV/C STATUS
    AVRC_TG_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES = 0x12,         // AV/C STATUS
    AVRC_TG_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE = 0x13,   // AV/C STATUS
    AVRC_TG_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE = 0x14,           // AV/C CONTROL
    AVRC_TG_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT = 0x15,  // AV/C STATUS
    AVRC_TG_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT = 0x16,      // AV/C STATUS
    AVRC_TG_PDU_ID_GET_ELEMENT_ATTRIBUTES = 0x20,                         // AV/C STATUS
    AVRC_TG_PDU_ID_GET_PLAY_STATUS = 0x30,                                // AV/C STATUS
    AVRC_TG_PDU_ID_REGISTER_NOTIFICATION = 0x31,                          // AV/C NOTIFY
    AVRC_TG_PDU_ID_REQUEST_CONTINUING_RESPONSE = 0x40,                    // AV/C CONTROL
    AVRC_TG_PDU_ID_ABORT_CONTINUING_RESPONSE,                             // AV/C CONTROL
    AVRC_TG_PDU_ID_SET_ABSOLUTE_VOLUME = 0x50,                            // AV/C CONTROL
    AVRC_TG_PDU_ID_SET_ADDRESSED_PLAYER = 0x60,                           // AV/C CONTROL
    AVRC_TG_PDU_ID_SET_BROWSED_PLAYER = 0x70,                             // Browsing
    AVRC_TG_PDU_ID_GET_FOLDER_ITEMS = 0x71,                               // Browsing
    AVRC_TG_PDU_ID_CHANGE_PATH = 0x72,                                    // Browsing
    AVRC_TG_PDU_ID_GET_ITEM_ATTRIBUTES = 0x73,                            // Browsing
    AVRC_TG_PDU_ID_PLAY_ITEM = 0x74,                                      // AV/C CONTROL
    AVRC_TG_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS = 0x75,                      // Browsing
    AVRC_TG_PDU_ID_ADD_TO_NOW_PLAYING = 0x90,                             // AV/C CONTROL
    AVRC_TG_PDU_ID_GENERAL_REJECT = 0xa0,                                 // Browsing
    AVRC_TG_PDU_ID_INVALID = 0xFF,
};

/**
 * @brief This enumeration declares a set of values associated with the <b>GetCapability<b>
 * frame.
 */
enum AvrcTgGcCapabilityId : uint8_t {
    AVRC_TG_GC_CAPABILITY_INVALID = 0x00,
    AVRC_TG_GC_CAPABILITY_COMPANY = 0x02,
    AVRC_TG_GC_CAPABILITY_EVENT = 0x03,
};

/**
 * @brief The class provides a set of methods for pushing / popping the payload of the frame.
 */
class AvrcTgPacket {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgPacket</b> instance.
     */
    AvrcTgPacket(void) = default;

    /**
     * @brief A destructor used to delete the <b>AvrcTgPacket</b> instance.
     */
    virtual ~AvrcTgPacket(void) = default;

    /**
     * @brief Pushes one octet into the specified memory.
     *
     * @param[out] pkt     The memory that need to push the payload.
     * @param[in]  payload The payload that need to be pushed.
     * @return The number of octets that is pushed into the specified memory.
     */
    virtual uint16_t PushOctets1(uint8_t *pkt, uint8_t payload) final;

    /**
     * @brief Pushes two octets into the specified memory.
     *
     * @param[out] pkt     The memory that need to push the payload.
     * @param[in]  payload The payload that need to be pushed.
     * @return The number of octets that is pushed into the specified memory.
     */
    virtual uint16_t PushOctets2(uint8_t *pkt, uint16_t payload) final;

    /**
     * @brief Pushes three octets into the specified memory.
     *
     * @param[out] pkt     The memory that need to push the payload.
     * @param[in]  payload The payload that need to be pushed.
     * @return The number of octets that is pushed into the specified memory.
     */
    virtual uint16_t PushOctets3(uint8_t *pkt, uint32_t payload) final;

    /**
     * @brief Pushes four octets into the specified memory.
     *
     * @param[out] pkt     The memory that need to push the payload.
     * @param[in]  payload The payload that need to be pushed.
     * @return The number of octets that is pushed into the specified memory.
     */
    virtual uint16_t PushOctets4(uint8_t *pkt, uint32_t payload) final;

    /**
     * @brief Pushes eight octets into the specified memory.
     *
     * @param[out] pkt     The memory that need to push the payload.
     * @param[in]  payload The payload that need to be pushed.
     * @return The number of octets that is pushed into the specified memory.
     */
    virtual uint16_t PushOctets8(uint8_t *pkt, uint64_t payload) final;

    /**
     * @brief Pushes the octets into the specified memory.
     *
     * @param[out] pkt     The memory that need to push the payload.
     * @param[in]  payload The payload that need to be pushed.
     * @return The number of octets that is pushed into the specified memory.
     */
    virtual uint16_t PushOctets(uint8_t *pkt, uint8_t *playload, uint16_t length) final;

    /**
     * @brief Pops one octet from the specified memory.
     *
     * @param[in]  pkt     The memory that need to pop the payload.
     * @param[out] payload The store that saves the payload.
     * @return The number of octets that is popped from the specified memory.
     */
    virtual uint16_t PopOctets1(uint8_t *pkt, uint64_t &payload) final;

    /**
     * @brief Pops two octets from the specified memory.
     *
     * @param[in]  pkt     The memory that need to pop the payload.
     * @param[out] payload The store that saves the payload.
     * @return The number of octets that is popped from the specified memory.
     */
    virtual uint16_t PopOctets2(uint8_t *pkt, uint64_t &payload) final;

    /**
     * @brief Pops three octets from the specified memory.
     *
     * @param[in]  pkt     The memory that need to pop the payload.
     * @param[out] payload The store that saves the payload.
     * @return The number of octets that is popped from the specified memory.
     */
    virtual uint16_t PopOctets3(uint8_t *pkt, uint64_t &payload) final;

    /**
     * @brief Pops four octets from the specified memory.
     *
     * @param[in]  pkt     The memory that need to pop the payload.
     * @param[out] payload The store that saves the payload.
     * @return The number of octets that is popped from the specified memory.
     */
    virtual uint16_t PopOctets4(uint8_t *pkt, uint64_t &payload) final;

    /**
     * @brief Pops eight octets from the specified memory.
     *
     * @param[in]  pkt     The memory that need to pop the payload.
     * @param[out] payload The store that saves the payload.
     * @return The number of octets that is popped from the specified memory.
     */
    virtual uint16_t PopOctets8(uint8_t *pkt, uint64_t &payload) final;

    /**
     * @brief Pops octets from the specified memory.
     *
     * @param[in]  pkt     The memory that need to pop the payload.
     * @param[out] payload The store that saves the payload.
     * @return The number of octets that is popped from the specified memory.
     */
    virtual uint16_t PopOctets(uint8_t *pkt, uint8_t *playload, uint16_t length) final;

    /**
     * @brief Gets the operation code of the specified command frame.
     *
     * @param[in] pkt The packet of the specified command frame.
     * @return The operation code of the specified command frame.
     */
    static uint8_t GetOpCode(Packet *pkt);

    /**
     * @brief Gets the PDU ID of the VENDOR DEPENDENT command frame.
     *
     * @param[in] pkt he packet of the VENDOR DEPENDENT command frame.
     * @return The PDU ID of the VENDOR DEPENDENT command frame.
     */
    static uint8_t GetVendorPdu(Packet *pkt);

    /**
     * @brief Gets the PDU ID of the BROWSING command frame.
     *
     * @param[in] pkt The packet of the BROWSING command frame.
     * @return The PDU ID of the BROWSING command frame.
     */
    static uint8_t GetBrowsePdu(Packet *pkt);

    /**
     * @brief Checks the opcode is valid or not.
     *
     * @param[in] code The operation code of the specified command frame
     * @return @c true  The opcode is valid.
     *         @c false The opcode is invalid.
     */
    static bool IsValidOpCode(uint8_t code);

    /**
     * @brief Assembles the payload into the specified response frame.
     *
     * @param[in] pkt The memory that need to push the payload.
     * @return The packet of the specified response frame.
     */
    virtual const Packet *AssemblePacket(void) = 0;

    /**
     * @brief Disassembles the payload from the specified command frame.
     *
     * @param[in] pkt The memory that need to pop the payload.
     * @return @c true  The packet is valid.
     *         @c false The packet is invalid.
     */
    virtual bool DisassemblePacket(Packet *pkt) = 0;

private:
    /**
     * @brief Pushes specific octets into specific memory.
     *
     * @param[out] pkt     The memory that need to push the payload.
     * @param[in]  size    The size of the payload need to be pushed.
     * @param[in]  payload The store that saves the payload.
     * @return The number of octets that is pushed into the specified memory.
     */
    static uint16_t PushOctetsUpto8(uint8_t *pkt, size_t size, uint64_t payload);

    /**
     * @brief Pops specific octets into specific memory.
     *
     * @param[out] pkt     The memory that need to pop the payload.
     * @param[in]  size    The size of the payload need to be popped.
     * @param[in]  payload The store that saves the payload.
     * @return The number of octets that is popped into the specified memory.
     */
    static uint16_t PopOctetsUpto8(uint8_t *pkt, size_t size, uint64_t &payload);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // !AVRCP_TG_PACKET_H
