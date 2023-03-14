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

#include "avrcp_ct_vendor.h"
#include "avrcp_ct_vendor_continuation.h"
#include "avrcp_ct_vendor_player_application_settings.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
/******************************************************************
 * AvrcCtVendorPacket                                             *
 ******************************************************************/

AvrcCtVendorPacket::AvrcCtVendorPacket() : AvrcCtPacket(), status_(AVRC_ES_CODE_INVALID)
{
    HILOGI("enter");
}

AvrcCtVendorPacket::~AvrcCtVendorPacket()
{
    HILOGI("enter");

    if (pkt_ != nullptr) {
        PacketFree(pkt_);
        pkt_ = nullptr;
    }
}

const Packet *AvrcCtVendorPacket::AssemblePacket(void)
{
    HILOGI("enter");

    pkt_ = AssembleFixedOperands();
    pkt_ = AssembleParameters(pkt_);

    return pkt_;
}

bool AvrcCtVendorPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    isValid_ = IsValidParameterLength(pkt);
    if (isValid_) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

        uint16_t offset = AVRC_CT_VENDOR_CTYPE_OFFSET;
        uint64_t payload = 0x00;
        PopOctets1((buffer + offset), payload);
        crCode_ = static_cast<uint8_t>(payload);
        HILOGI("crCode_: %{public}x", crCode_);

        offset = AVRC_CT_VENDOR_COMPANY_ID_OFFSET;
        payload = 0x00;
        PopOctets3((buffer + offset), payload);
        companyId_ = static_cast<uint32_t>(payload);
        HILOGI("companyId_: %{public}x", companyId_);

        offset = AVRC_CT_VENDOR_PACKET_TYPE_OFFSET;
        payload = 0x00;
        PopOctets1((buffer + offset), payload);
        packetType_ = static_cast<uint8_t>(payload);
        HILOGI("packetType_: %{public}x", packetType_);

        if ((crCode_ == AVRC_CT_RSP_CODE_REJECTED || crCode_ == AVRC_CT_RSP_CODE_NOT_IMPLEMENTED) &&
            parameterLength_ == AVRC_TG_VENDOR_REJECT_PARAMETER_LENGTH) {
            isValid_ = false;
            offset = AVRC_CT_VENDOR_STATUS_OFFSET;
            payload = 0x00;
            PopOctets1((buffer + offset), payload);
            status_ = static_cast<uint8_t>(payload);
            HILOGI("status_: %{public}x", status_);
        } else {
            isValid_ = DisassembleParameters(buffer);
        }
    } else {
        HILOGI("The parameter length is invalid!");
    }

    return isValid_;
}

Packet *AvrcCtVendorPacket::AssembleFixedOperands(void)
{
    HILOGI("enter");

    pkt_ = PacketMalloc(0x00, 0x00, AVRC_CT_VENDOR_MIN_COMMAND_SIZE);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x00;
    offset += PushOctets1((buffer + offset), crCode_);
    HILOGI("crCode_: %{public}x", crCode_);

    offset += PushOctets1((buffer + offset), (subunitType_ << AVRC_CT_VENDOR_MOVE_BIT_3) | subunitId_);
    HILOGI("subunitId_: %{public}x, subunitType_: %{public}x, subunitType_ << 3 | subunitId_: %{public}x",
        subunitId_, subunitType_, (subunitType_ << AVRC_CT_VENDOR_MOVE_BIT_3) | subunitId_);

    offset += PushOctets1((buffer + offset), opCode_);
    HILOGI("opCode_: %{public}x", opCode_);

    offset += PushOctets3((buffer + offset), companyId_);
    HILOGI("companyId_: %{public}x", companyId_);

    offset += PushOctets1((buffer + offset), pduId_);
    HILOGI("pduId_: %{public}x", pduId_);

    PushOctets1((buffer + offset), packetType_);
    HILOGI("packetType_: %{public}x", packetType_);

    return pkt_;
}

bool AvrcCtVendorPacket::IsValidParameterLength(Packet *pkt)
{
    HILOGI("enter");

    bool result = false;

    size_t size = PacketPayloadSize(pkt);
    if (size >= AVRC_CT_VENDOR_FIXED_OPERAND_SIZE) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

        uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint32_t>(payload);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        if (size - AVRC_CT_VENDOR_FIXED_OPERAND_SIZE == parameterLength_) {
            result = true;
        }
    } else {
        HILOGI("The size of the packet is invalid! actual size: %{public}zu valid min size: %{public}ju",
            size, AVRC_CT_VENDOR_FIXED_OPERAND_SIZE);
    }

    return result;
}

/******************************************************************
 * GetCapability                                                  *
 ******************************************************************/

AvrcCtGcPacket::AvrcCtGcPacket(uint8_t capabilityId) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_CAPABILITIES;
    parameterLength_ = AVRC_CT_GC_PARAMETER_LENGTH;
    capabilityId_ = capabilityId;
}

AvrcCtGcPacket::AvrcCtGcPacket(Packet *pkt) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_CAPABILITIES;

    DisassemblePacket(pkt);
}

AvrcCtGcPacket::~AvrcCtGcPacket()
{
    HILOGI("enter");

    companies_.clear();
    events_.clear();
}

Packet *AvrcCtGcPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    auto buffer = BufferMalloc(AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_GC_PARAMETER_LENGTH);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));
    HILOGI("BufferMalloc: %{public}ju", (AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_GC_PARAMETER_LENGTH));

    uint16_t offset = 0x0000;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}hu", parameterLength_);

    PushOctets1((bufferPtr + offset), capabilityId_);
    HILOGI("capabilityId_: %{public}hhu", capabilityId_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtGcPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PopOctets1((buffer + offset), payload);
    capabilityId_ = static_cast<uint8_t>(payload);
    HILOGI("capabilityId_: %{public}d", capabilityId_);

    offset += PopOctets1((buffer + offset), payload);
    capabilityCount_ = static_cast<uint8_t>(payload);
    HILOGI("capabilityCount_: %{public}d", capabilityCount_);

    if (capabilityId_ == AVRC_CAPABILITY_COMPANYID) {
        for (int i = 0; i < capabilityCount_; i++) {
            payload = 0x00;
            offset += PopOctets3((buffer + offset), payload);
            companies_.push_back(payload);
            HILOGI("companyId_: %{public}x", static_cast<uint32_t>(payload));
        }
    } else if (capabilityId_ == AVRC_CAPABILITY_EVENTID) {
        for (int i = 0; i < capabilityCount_; i++) {
            payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            events_.push_back(static_cast<uint8_t>(payload));
            HILOGI("events_: %{public}x", static_cast<uint8_t>(payload));
        }
    }

    isValid_ = true;

    return isValid_;
}

/******************************************************************
 * ListPlayerApplicationSettingAttribute                          *
 ******************************************************************/

AvrcCtLpasaPacket::AvrcCtLpasaPacket() : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES;
    parameterLength_ = AVRC_CT_LPASA_PARAMETER_LENGTH;
}

AvrcCtLpasaPacket::AvrcCtLpasaPacket(Packet *pkt) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES;

    DisassemblePacket(pkt);
}

AvrcCtLpasaPacket::~AvrcCtLpasaPacket()
{
    HILOGI("enter");

    attributes_.clear();
}

Packet *AvrcCtLpasaPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    auto buffer = BufferMalloc(AVRC_CT_LPASA_PARAMETER_LENGTH_SIZE);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));
    HILOGI("BufferMalloc: %{public}d", (AVRC_CT_LPASA_PARAMETER_LENGTH_SIZE));

    uint16_t offset = 0x0000;
    PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtLpasaPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PopOctets1((buffer + offset), payload);
    numOfAttributes_ = static_cast<uint8_t>(payload);
    HILOGI("numOfAttributes_: %{public}d", numOfAttributes_);

    for (int i = 0; i < numOfAttributes_; i++) {
        offset += PopOctets1((buffer + offset), payload);
        attributes_.push_back(static_cast<uint8_t>(payload));
        HILOGI("attributes: %{public}x", static_cast<uint8_t>(payload));
    }
    isValid_ = true;

    return isValid_;
}

/******************************************************************
 * ListPlayerApplicationSettingValues                             *
 ******************************************************************/

AvrcCtLpasvPacket::AvrcCtLpasvPacket(uint8_t attributes)
    : AvrcCtVendorPacket(), attribute_(AVRC_PLAYER_ATTRIBUTE_ILLEGAL)
{
    HILOGI("attributes: %{public}d", attributes);

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES;
    parameterLength_ = AVRC_CT_LPASV_PARAMETER_LENGTH;

    attribute_ = attributes;
}

AvrcCtLpasvPacket::AvrcCtLpasvPacket(Packet *pkt) : AvrcCtVendorPacket(), attribute_(AVRC_PLAYER_ATTRIBUTE_ILLEGAL)
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES;

    DisassemblePacket(pkt);
}

AvrcCtLpasvPacket::~AvrcCtLpasvPacket()
{
    HILOGI("enter");

    values_.clear();
}

Packet *AvrcCtLpasvPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_LPASV_PARAMETER_LENGTH;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PushOctets1((bufferPtr + offset), attribute_);
    HILOGI("attribute_: %{public}x", attribute_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtLpasvPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    receivedFragments_++;
    HILOGI("receivedFragments_: %{public}hhu", receivedFragments_);

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PopOctets1((buffer + offset), payload);
    numOfValues_ = static_cast<uint8_t>(payload);
    HILOGI("numOfValues_: %{public}d", numOfValues_);

    for (int i = 0; i < numOfValues_; i++) {
        offset += PopOctets1((buffer + offset), payload);
        values_.push_back(static_cast<uint8_t>(payload));
        HILOGI("value: %{public}x", values_.back());
    }

    numOfValues_ = values_.size();

    HILOGI("values_.size: %{public}zu", values_.size());

    if (numOfValues_ == values_.size() && numOfValues_ > 0) {
        isValid_ = true;
    }

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

bool AvrcCtLpasvPacket::IsValidAttribute(void) const
{
    HILOGI("enter");

    bool result = false;

    do {
        if (attribute_ == AVRC_PLAYER_ATTRIBUTE_ILLEGAL) {
            break;
        }

        if (attribute_ >= AVRC_PLAYER_ATTRIBUTE_RESERVED_MIN && attribute_ <= AVRC_PLAYER_ATTRIBUTE_RESERVED_MAX) {
            break;
        }

        result = true;
    } while (false);

    HILOGI("result: %{public}d", result);

    return result;
}

/******************************************************************
 * GetCurrentPlayerApplicationSettingValue                        *
 ******************************************************************/

AvrcCtGcpasvPacket::AvrcCtGcpasvPacket(const std::vector<uint8_t> &attributes) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE;
    attributes_ = attributes;
}

AvrcCtGcpasvPacket::AvrcCtGcpasvPacket(Packet *pkt) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE;

    DisassemblePacket(pkt);
}

AvrcCtGcpasvPacket::~AvrcCtGcpasvPacket()
{
    HILOGI("enter");

    attributes_.clear();
    values_.clear();
}

Packet *AvrcCtGcpasvPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize =
        AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_GCPASV_NUM_OF_ATTRIBUTES_SIZE + attributes_.size();
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    parameterLength_ = bufferSize - AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    numOfAttributes_ = attributes_.size();
    offset += PushOctets1((bufferPtr + offset), numOfAttributes_);
    HILOGI("numOfAttributes_: %{public}d", numOfAttributes_);

    for (auto attribute : attributes_) {
        offset += PushOctets1((bufferPtr + offset), attribute);
        HILOGI("attribute: %{public}x", attribute);
    }

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtGcpasvPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    receivedFragments_++;
    HILOGI("receivedFragments_: %{public}d", receivedFragments_);

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PopOctets1((buffer + offset), payload);
    numOfValues_ = static_cast<uint8_t>(payload);
    HILOGI("numOfValues_: %{public}d", numOfValues_);

    for (int i = 0; i < numOfValues_; i++) {
        offset += PopOctets1((buffer + offset), payload);
        attributes_.push_back(static_cast<uint8_t>(payload));
        HILOGI("attribute: %{public}x", attributes_.back());

        offset += PopOctets1((buffer + offset), payload);
        values_.push_back(static_cast<uint8_t>(payload));
        HILOGI("value: %{public}x", values_.back());
    }

    if (numOfValues_ == values_.size() && numOfValues_ > 0) {
        isValid_ = true;
    }

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

bool AvrcCtGcpasvPacket::IsValidAttribute(void) const
{
    HILOGI("enter");

    bool result = false;

    for (auto attribute : attributes_) {
        do {
            if (attribute == AVRC_PLAYER_ATTRIBUTE_ILLEGAL) {
                break;
            }

            if (attribute >= AVRC_PLAYER_ATTRIBUTE_RESERVED_MIN && attribute <= AVRC_PLAYER_ATTRIBUTE_RESERVED_MAX) {
                break;
            }

            result = true;
        } while (false);
    }

    HILOGI("result: %{public}d", result);

    return result;
}

/******************************************************************
 * SetPlayerApplicationSettingValue                               *
 ******************************************************************/

AvrcCtSpasvPacket::AvrcCtSpasvPacket(const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values)
    : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE;

    attributes_ = attributes;
    values_ = values;
}

AvrcCtSpasvPacket::AvrcCtSpasvPacket(Packet *pkt) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE;

    DisassemblePacket(pkt);
}

AvrcCtSpasvPacket::~AvrcCtSpasvPacket()
{
    HILOGI("enter");

    attributes_.clear();
    values_.clear();
}

Packet *AvrcCtSpasvPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_SPASV_NUM_OF_ATTRIBUTES_SIZE +
                        attributes_.size() + values_.size();
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    parameterLength_ = AVRC_CT_SPASV_NUM_OF_ATTRIBUTES_SIZE + attributes_.size() + values_.size();
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    numOfAttributes_ = attributes_.size();
    offset += PushOctets1((bufferPtr + offset), numOfAttributes_);
    HILOGI("numOfAttributes_: %{public}d", numOfAttributes_);

    for (int i = 0; i < numOfAttributes_; i++) {
        offset += PushOctets1((bufferPtr + offset), attributes_.at(i));
        HILOGI("attribute: %{public}x", attributes_.at(i));
        offset += PushOctets1((bufferPtr + offset), values_.at(i));
        HILOGI("value: %{public}x", values_.at(i));
    }

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtSpasvPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    isValid_ = true;

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

bool AvrcCtSpasvPacket::IsValidAttribute(void) const
{
    HILOGI("enter");

    bool result = false;

    for (auto attribute : attributes_) {
        do {
            if (attribute == AVRC_PLAYER_ATTRIBUTE_ILLEGAL) {
                break;
            }

            if (attribute >= AVRC_PLAYER_ATTRIBUTE_RESERVED_MIN && attribute <= AVRC_PLAYER_ATTRIBUTE_RESERVED_MAX) {
                break;
            }

            result = true;
        } while (false);
    }

    HILOGI("result: %{public}d", result);

    return result;
}

/******************************************************************
 * GetPlayerApplicationSettingAttributeText                       *
 ******************************************************************/

AvrcCtGpasatPacket::AvrcCtGpasatPacket(std::vector<uint8_t> attributes) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;
    attributes_ = attributes;
    numOfAttributes_ = attributes.size();
}

AvrcCtGpasatPacket::AvrcCtGpasatPacket(Packet *pkt) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;

    DisassemblePacket(pkt);
}

AvrcCtGpasatPacket::~AvrcCtGpasatPacket()
{
    HILOGI("enter");

    attributes_.clear();
    attributeName_.clear();
}

Packet *AvrcCtGpasatPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_GPASAT_ATTRIBUTE_NUM_LENGTH + attributes_.size();
    HILOGI("BufferMalloc: %{public}zu", bufferSize);
    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    parameterLength_ = bufferSize - AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PushOctets1((bufferPtr + offset), numOfAttributes_);
    HILOGI("numOfAttributes_: %{public}d", numOfAttributes_);

    for (auto attribute : attributes_) {
        offset += PushOctets1((bufferPtr + offset), attribute);
        HILOGI("attribute: %{public}x", attribute);
    }

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtGpasatPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    receivedFragments_++;
    HILOGI("receivedFragments_: %{public}d", receivedFragments_);

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PopOctets1((buffer + offset), payload);
    numOfAttributes_ = static_cast<uint8_t>(payload);
    HILOGI("numOfAttributes_: %{public}d", numOfAttributes_);

    for (int i = 0; i < numOfAttributes_; i++) {
        offset += PopOctets1((buffer + offset), payload);
        attributes_.push_back(static_cast<uint32_t>(payload));
        HILOGI("attributeId: %{public}x", attributes_.back());

        offset += PopOctets2((buffer + offset), payload);
        HILOGI("characterSetID: %{public}x", static_cast<uint16_t>(payload));

        offset += PopOctets1((buffer + offset), payload);
        AttributeValueLength_ = static_cast<uint8_t>(payload);
        HILOGI("AttributeValueLength: %{public}d", AttributeValueLength_);

        char *tempName = nullptr;
        tempName = new char[AttributeValueLength_ + 1];
        for (int j = 0; j < AttributeValueLength_; j++) {
            payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            tempName[j] = static_cast<char>(payload);
        }
        tempName[AttributeValueLength_] = '\0';
        std::string name(tempName);
        AvrcpCtSafeDeleteArray(tempName, AttributeValueLength_ + 1);
        attributeName_.push_back(name);
        HILOGI("AttributeValue: %{public}s", attributeName_.back().c_str());
    }

    isValid_ = true;
    HILOGI("result: %{public}d", isValid_);

    return isValid_;
}

/******************************************************************
 * GetPlayerApplicationSettingValueText                           *
 ******************************************************************/

AvrcCtGpasvtPacket::AvrcCtGpasvtPacket(uint8_t attribtueId, std::vector<uint8_t> values)
    : AvrcCtVendorPacket(), attributeId_(AVRC_PLAYER_ATTRIBUTE_ILLEGAL)
{
    HILOGI("attribtueId: %{public}d", attribtueId);

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT;
    values_ = values;
    numOfValues_ = values.size();
    attributeId_ = attribtueId;
}

AvrcCtGpasvtPacket::AvrcCtGpasvtPacket(Packet *pkt) : AvrcCtVendorPacket(), attributeId_(AVRC_PLAYER_ATTRIBUTE_ILLEGAL)
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT;

    DisassemblePacket(pkt);
}

AvrcCtGpasvtPacket::~AvrcCtGpasvtPacket()
{
    HILOGI("enter");

    values_.clear();
    valueName_.clear();
}

Packet *AvrcCtGpasvtPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_GPASVT_PARAMETER_LENGTH_SIZE + numOfValues_;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);
    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    parameterLength_ = bufferSize - AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PushOctets1((bufferPtr + offset), attributeId_);
    HILOGI("attributeId_: %{public}x", attributeId_);

    offset += PushOctets1((bufferPtr + offset), numOfValues_);
    HILOGI("numOfValues_: %{public}d", numOfValues_);

    for (auto value : values_) {
        offset += PushOctets1((bufferPtr + offset), value);
        HILOGI("value: %{public}x", value);
    }

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtGpasvtPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    receivedFragments_++;
    HILOGI("receivedFragments_: %{public}d", receivedFragments_);

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PopOctets1((buffer + offset), payload);
    numOfValues_ = static_cast<uint8_t>(payload);
    HILOGI("numOfValues_: %{public}d", numOfValues_);

    for (int i = 0; i < numOfValues_; i++) {
        offset += PopOctets1((buffer + offset), payload);
        values_.push_back(static_cast<uint32_t>(payload));
        HILOGI("valueId: %{public}d", values_.back());

        offset += PopOctets2((buffer + offset), payload);
        HILOGI("characterSetID: %{public}x", static_cast<uint16_t>(payload));

        offset += PopOctets1((buffer + offset), payload);
        AttributeValueLength_ = static_cast<uint8_t>(payload);
        HILOGI("AttributeValueLength: %{public}d", AttributeValueLength_);

        char *tempName = nullptr;
        tempName = new char[AttributeValueLength_ + 1];
        for (int j = 0; j < AttributeValueLength_; j++) {
            offset += PopOctets1((buffer + offset), payload);
            tempName[j] = static_cast<char>(payload);
        }
        tempName[AttributeValueLength_] = '\0';
        std::string name(tempName);
        AvrcpCtSafeDeleteArray(tempName, AttributeValueLength_ + 1);
        valueName_.push_back(name);
        HILOGI("Value: %{public}s", valueName_.back().c_str());
    }

    isValid_ = true;

    HILOGI("result: %{public}d", isValid_);

    return isValid_;
}

/******************************************************************
 * GetElementAttribute                                            *
 ******************************************************************/

AvrcCtGeaPacket::AvrcCtGeaPacket(uint64_t identifier, std::vector<uint32_t> attributes) : AvrcCtVendorPacket()
{
    HILOGI("identifier: %{public}llu", static_cast<unsigned long long>(identifier));

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_ELEMENT_ATTRIBUTES;
    number_ = attributes.size();
    identifier_ = identifier;
    attributes_ = attributes;
}

AvrcCtGeaPacket::AvrcCtGeaPacket(Packet *pkt) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_ELEMENT_ATTRIBUTES;

    DisassemblePacket(pkt);
}

AvrcCtGeaPacket::~AvrcCtGeaPacket()
{
    HILOGI("enter");

    attributes_.clear();
    values_.clear();
}

bool AvrcCtGeaPacket::DisassemblePacketAttributes(uint8_t *buffer, uint16_t offset)
{
    HILOGI("offset: %{public}d", offset);
    uint64_t payload = 0x00;

    for (int i = 0; i < number_; i++) {
        offset += PopOctets4((buffer + offset), payload);
        size_t pos;
        size_t attrNum = attributes_.size();
        for (pos = 0; pos < attrNum; pos++) {
            if (attributes_.at(pos) == payload) {
                break;
            }
        }
        if (pos == attrNum) {
            attributes_.push_back(static_cast<uint32_t>(payload));
            HILOGI("attributeId: %{public}d", attributes_.back());
        } else {
            HILOGI("attributeId: %{public}d", attributes_[pos]);
        }
        offset += PopOctets2((buffer + offset), payload);
        auto characterSetID = static_cast<uint16_t>(payload);
        offset += PopOctets2((buffer + offset), payload);
        AttributeValueLength_ = static_cast<uint16_t>(payload);
        HILOGI("characterSetID:%{public}hu, AttributeValueLength_:%{public}hu", characterSetID, AttributeValueLength_);
        char tempName[AttributeValueLength_ + 1];
        if (memset_s(tempName, AttributeValueLength_ + 1, 0, AttributeValueLength_ + 1) != EOK) {
            return false;
        };
        int j;
        for (j = 0; j < AttributeValueLength_; j++) {
            payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            HILOGI("offset: %{public}d", offset);
            tempName[j] = static_cast<char>(payload);
            if (offset >= parameterLength_ + AVRC_CT_VENDOR_MIN_COMMAND_SIZE + AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE ||
                offset >= AVRC_CT_GEA_MTU) {
                j++;
                break;
            }
        }
        tempName[j] = '\0';
        std::string name(tempName);
        if (pos == attrNum) {
            values_.push_back(name);
            HILOGI("AttributeValue: %{public}s", values_.back().c_str());
        } else {
            values_.at(pos).append(name);
            HILOGI("AttributeValue: %{public}s", values_.at(pos).c_str());
        }
    }
    return true;
}

Packet *AvrcCtGeaPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_GEA_IDENTIFIER_SIZE +
                        AVRC_CT_GEA_ATTRIBUTE_COUNT_SIZE + number_ * AVRC_CT_GEA_ATTRIBUTE_SIZE;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);
    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    parameterLength_ = bufferSize - AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE;

    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}hu", parameterLength_);

    offset += PushOctets8((bufferPtr + offset), identifier_);
    HILOGI("identifier_: %{public}jx", identifier_);

    offset += PushOctets1((bufferPtr + offset), number_);
    HILOGI("number_: %{public}hhu", number_);

    for (auto attribute : attributes_) {
        offset += PushOctets4((bufferPtr + offset), attribute);
        HILOGI("attribute: %{public}x", attribute);
    }

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtGeaPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;
    receivedFragments_++;
    HILOGI("receivedFragments_: %{public}d", receivedFragments_);
    do {
        uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        offset += PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint16_t>(payload);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        offset += PopOctets1((buffer + offset), payload);
        number_ = static_cast<uint8_t>(payload);
        HILOGI("number_: %{public}d", number_);

        DisassemblePacketAttributes(buffer, offset);

        numOfAttributes_ = attributes_.size();
        numOfValues_ = values_.size();

        HILOGI("values_.size: %{public}zu", values_.size());

        if (numOfValues_ > 0) {
            isValid_ = true;
        }
    } while (0);

    HILOGI("result: %{public}d", isValid_);

    return isValid_;
}

/******************************************************************
 * GetPlayStatus                                                  *
 ******************************************************************/

AvrcCtGpsPacket::AvrcCtGpsPacket()
    : AvrcCtVendorPacket(),
      songLength_(AVRC_PLAY_STATUS_INVALID_SONG_LENGTH),
      songPosition_(AVRC_PLAY_STATUS_INVALID_SONG_POSITION),
      playStatus_(AVRC_PLAY_STATUS_ERROR)
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_PLAY_STATUS;
    parameterLength_ = AVRC_CT_GPS_PARAMETER_LENGTH;
}

AvrcCtGpsPacket::AvrcCtGpsPacket(Packet *pkt)
    : AvrcCtVendorPacket(),
      songLength_(AVRC_PLAY_STATUS_INVALID_SONG_LENGTH),
      songPosition_(AVRC_PLAY_STATUS_INVALID_SONG_POSITION),
      playStatus_(AVRC_PLAY_STATUS_ERROR)
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_STATUS;
    pduId_ = AVRC_CT_PDU_ID_GET_PLAY_STATUS;

    DisassemblePacket(pkt);
}

AvrcCtGpsPacket::~AvrcCtGpsPacket(void)
{
    HILOGI("enter");
}

Packet *AvrcCtGpsPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_GPS_PARAMETER_LENGTH;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtGpsPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    switch (parameterLength_) {
        case AVRC_CT_VENDOR_PARAMETER_LENGTH:
            songLength_ = AVRC_PLAY_STATUS_INVALID_SONG_LENGTH;
            HILOGI("songLength_: %{public}d", songLength_);

            songPosition_ = AVRC_PLAY_STATUS_INVALID_SONG_POSITION;
            HILOGI("songPosition_: %{public}d", songPosition_);

            playStatus_ = AVRC_PLAY_STATUS_ERROR;
            HILOGI("playStatus_: %{public}x", playStatus_);
            break;
        default:
            offset = AVRC_CT_GPS_SONG_LENGTH_OFFSET;
            payload = 0x00;
            offset += PopOctets4((buffer + offset), payload);
            songLength_ = static_cast<uint32_t>(payload);
            HILOGI("songLength_: %{public}d", songLength_);

            payload = 0x00;
            offset += PopOctets4((buffer + offset), payload);
            songPosition_ = static_cast<uint32_t>(payload);
            HILOGI("songPosition_: %{public}d", songPosition_);

            payload = 0x00;
            PopOctets1((buffer + offset), payload);
            playStatus_ = static_cast<uint8_t>(payload);
            HILOGI("playStatus_: %{public}x", playStatus_);

            if (crCode_ == AVRC_CT_RSP_CODE_STABLE || crCode_ == AVRC_CT_RSP_CODE_INTERIM) {
                isValid_ = true;
            }
            break;
    }

    return isValid_;
}

/******************************************************************
 * RequestContinuingResponse                                      *
 ******************************************************************/

AvrcCtRcrPacket::AvrcCtRcrPacket() : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_REQUEST_CONTINUING_RESPONSE;
    parameterLength_ = AVRC_CT_RCR_PARAMETER_LENGTH;
}

AvrcCtRcrPacket::AvrcCtRcrPacket(uint8_t pduId) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_REQUEST_CONTINUING_RESPONSE;
    parameterLength_ = AVRC_CT_RCR_PARAMETER_LENGTH;
    continuePduId_ = pduId;
}

AvrcCtRcrPacket::~AvrcCtRcrPacket()
{
    HILOGI("enter");
}

Packet *AvrcCtRcrPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    auto buffer = BufferMalloc(AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_RCR_PARAMETER_LENGTH);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PushOctets1((bufferPtr + offset), continuePduId_);
    HILOGI("continuePduId_: %{public}x", continuePduId_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtRcrPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    return isValid_;
}

/******************************************************************
 * AbortContinuingResponse                                        *
 ******************************************************************/

AvrcCtAcrPacket::AvrcCtAcrPacket() : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_ABORT_CONTINUING_RESPONSE;
    parameterLength_ = AVRC_CT_ACR_PARAMETER_LENGTH;
}

AvrcCtAcrPacket::AvrcCtAcrPacket(uint8_t pduId) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_ABORT_CONTINUING_RESPONSE;
    parameterLength_ = AVRC_CT_ACR_PARAMETER_LENGTH;
    continuePduId_ = pduId;
}

AvrcCtAcrPacket::AvrcCtAcrPacket(Packet *pkt) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_ABORT_CONTINUING_RESPONSE;

    DisassemblePacket(pkt);
}

AvrcCtAcrPacket::~AvrcCtAcrPacket()
{
    HILOGI("enter");
}

Packet *AvrcCtAcrPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    auto buffer = BufferMalloc(AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_ACR_PARAMETER_LENGTH);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PushOctets1((bufferPtr + offset), continuePduId_);
    HILOGI("continuePduId_: %{public}x", continuePduId_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtAcrPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    isValid_ = true;

    return isValid_;
}

/******************************************************************
 * SetAddressedPlayer                                             *
 ******************************************************************/

AvrcCtSapPacket::AvrcCtSapPacket(uint16_t playerId) : AvrcCtVendorPacket()
{
    HILOGI("playerId: %{public}d", playerId);

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_SET_ADDRESSED_PLAYER;
    parameterLength_ = AVRC_CT_SAP_PARAMETER_LENGTH;
    playerId_ = playerId;
}

AvrcCtSapPacket::AvrcCtSapPacket(Packet *pkt) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_SET_ADDRESSED_PLAYER;

    DisassemblePacket(pkt);
}

AvrcCtSapPacket::~AvrcCtSapPacket()
{
    HILOGI("enter");
}

Packet *AvrcCtSapPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    auto buffer = BufferMalloc(AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_SAP_PARAMETER_LENGTH);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));
    HILOGI("BufferMalloc: %{public}ju", (AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_SAP_PARAMETER_LENGTH));

    uint16_t offset = 0x0000;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PushOctets2((bufferPtr + offset), playerId_);
    HILOGI("playerId_: %{public}d", playerId_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtSapPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PopOctets1((buffer + offset), payload);
    status_ = static_cast<uint8_t>(payload);
    HILOGI("status_: %{public}x", status_);

    isValid_ = true;

    return isValid_;
}

/******************************************************************
 * PlayItem                                                       *
 ******************************************************************/

AvrcCtPiPacket::AvrcCtPiPacket(uint8_t scope, uint64_t uid, uint16_t uidCounter)
    : AvrcCtVendorPacket(), scope_(AVRC_MEDIA_SCOPE_INVALID)
{
    HILOGI("scope: %{public}d, uid: %{public}llu, uidCounter: %{public}d",
        scope, static_cast<unsigned long long>(uid), uidCounter);

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_PLAY_ITEM;
    parameterLength_ = AVRC_CT_PI_PARAMETER_LENGTH;
    scope_ = scope;
    uid_ = uid;
    uidCounter_ = uidCounter;
}

AvrcCtPiPacket::AvrcCtPiPacket(Packet *pkt) : AvrcCtVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_PLAY_ITEM;

    DisassemblePacket(pkt);
}

AvrcCtPiPacket::~AvrcCtPiPacket(void)
{
    HILOGI("enter");
}

Packet *AvrcCtPiPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_PI_PARAMETER_LENGTH;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x00;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PushOctets1((bufferPtr + offset), scope_);
    HILOGI("scope_: %{public}x", scope_);

    offset += PushOctets8((bufferPtr + offset), uid_);
    HILOGI("uid_: %{public}jx", uid_);

    PushOctets2((bufferPtr + offset), uidCounter_);
    HILOGI("uidCounter_: %{public}d", uidCounter_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtPiPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PopOctets1((buffer + offset), payload);
    status_ = static_cast<uint8_t>(payload);
    HILOGI("status_: %{public}x", status_);

    isValid_ = true;

    return isValid_;
}

/******************************************************************
 * AddToNowPlaying                                                *
 ******************************************************************/

AvrcCtAtnpPacket::AvrcCtAtnpPacket(uint8_t scope, uint64_t uid, uint16_t uidCounter)
    : AvrcCtVendorPacket(), scope_(AVRC_MEDIA_SCOPE_INVALID)
{
    HILOGI("scope: %{public}d, uid: %{public}llu, uidCounter: %{public}d",
        scope, static_cast<unsigned long long>(uid), uidCounter);

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_ADD_TO_NOW_PLAYING;
    parameterLength_ = AVRC_CT_ATNP_PARAMETER_LENGTH;

    scope_ = scope;
    uid_ = uid;
    uidCounter_ = uidCounter;
}

AvrcCtAtnpPacket::AvrcCtAtnpPacket(Packet *pkt) : AvrcCtVendorPacket(), scope_(AVRC_MEDIA_SCOPE_INVALID)
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_ADD_TO_NOW_PLAYING;

    DisassemblePacket(pkt);
}

AvrcCtAtnpPacket::~AvrcCtAtnpPacket()
{
    HILOGI("enter");
}

Packet *AvrcCtAtnpPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    auto buffer = BufferMalloc(AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_ATNP_PARAMETER_LENGTH);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PushOctets1((bufferPtr + offset), scope_);
    HILOGI("scope_: %{public}hhx", scope_);

    offset += PushOctets8((bufferPtr + offset), uid_);
    HILOGI("uid_: %{public}jx", uid_);

    PushOctets2((bufferPtr + offset), uidCounter_);
    HILOGI("uidCounter_: %{public}hu", uidCounter_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtAtnpPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PopOctets1((buffer + offset), payload);
    status_ = static_cast<uint8_t>(payload);
    HILOGI("status_: %{public}x", status_);

    isValid_ = true;

    return isValid_;
}

/******************************************************************
 * SetAbsoluteVolume                                              *
 ******************************************************************/

AvrcCtSavPacket::AvrcCtSavPacket(uint8_t volume) : AvrcCtVendorPacket(), volume_(AVRC_ABSOLUTE_VOLUME_INVALID)
{
    HILOGI("volume: %{public}d", volume);

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_SET_ABSOLUTE_VOLUME;
    parameterLength_ = AVRC_CT_SAV_PARAMETER_LENGTH;
    volume_ = volume;
}

AvrcCtSavPacket::AvrcCtSavPacket(Packet *pkt) : AvrcCtVendorPacket(), volume_(AVRC_ABSOLUTE_VOLUME_INVALID)
{
    HILOGI("enter");

    crCode_ = AVRC_CT_CMD_CODE_CONTROL;
    pduId_ = AVRC_CT_PDU_ID_SET_ABSOLUTE_VOLUME;

    DisassemblePacket(pkt);
}

AvrcCtSavPacket::~AvrcCtSavPacket()
{
    HILOGI("enter");
}

Packet *AvrcCtSavPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_CT_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_CT_SAV_PARAMETER_LENGTH;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PushOctets1((bufferPtr + offset), volume_ & 0b01111111);
    HILOGI("volume_: %{public}x", volume_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcCtSavPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_CT_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PopOctets1((buffer + offset), payload);
    volume_ = static_cast<uint8_t>(payload) & 0b01111111;
    HILOGI("volume_: %{public}x", volume_);

    isValid_ = true;

    return isValid_;
}
}  // namespace bluetooth
}  // namespace OHOS
