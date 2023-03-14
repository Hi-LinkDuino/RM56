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
#include <algorithm>
#include "avrcp_tg_vendor.h"
#include "avrcp_tg_vendor_continuation.h"
#include "avrcp_tg_vendor_player_application_settings.h"

namespace OHOS {
namespace bluetooth {
/******************************************************************
 * AvrcTgVendorPacket                                             *
 ******************************************************************/

AvrcTgVendorPacket::AvrcTgVendorPacket() : AvrcTgPacket(), status_(AVRC_ES_CODE_INVALID), label_(AVRC_INVALID_LABEL)
{
    HILOGI("enter");
}

AvrcTgVendorPacket::AvrcTgVendorPacket(uint8_t pduId, uint8_t status, uint8_t label) : status_(status), label_(label)
{
    HILOGI("pduId:%{public}d, status:%{public}d, label:%{public}d", pduId, status, label);

    crCode_ = AVRC_TG_RSP_CODE_REJECTED;
    pduId_ = pduId;
    status_ = status;
    label_ = label;

    AssembleRejectPacket();
}

AvrcTgVendorPacket::~AvrcTgVendorPacket()
{
    HILOGI("enter");

    for (auto &pkt : pkts_) {
        PacketFree(pkt);
        pkt = nullptr;
    }
    pkts_.clear();
}

Packet *AvrcTgVendorPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    return nullptr;
}

const Packet *AvrcTgVendorPacket::AssemblePacket(void)
{
    HILOGI("enter");

    return nullptr;
}

void AvrcTgVendorPacket::AssemblePackets(void)
{
    HILOGI("enter");

    Packet *pkt = AssembleRejectPacket();
    if (pkt != nullptr) {
        return;
    }

    /// Calculates the number of packets.
    uint16_t numOfPkts = GetNumberOfPackets();
    if (numOfPkts == 0x00) {
        crCode_ = AVRC_TG_RSP_CODE_REJECTED;
        AssembleRejectPacket();
        return;
    }

    for (int i = 0; i < numOfPkts; i++) {
        pkt = AssembleMinOperands();

        if (numOfPkts == 1) {
            packetType_ = AVRC_TG_VENDOR_PACKET_TYPE;
        } else if (i == 0) {
            packetType_ = AVRC_TG_VENDOR_PACKET_TYPE_START;
        } else if (i == numOfPkts - 1) {
            packetType_ = AVRC_TG_VENDOR_PACKET_TYPE_END;
        } else {
            packetType_ = AVRC_TG_VENDOR_PACKET_TYPE_CONTINUE;
        }

        pkt = AssembleParameters(pkt);
        pkts_.push_back(pkt);
    }
}

bool AvrcTgVendorPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    isValid_ = IsValidParameterLength(pkt);
    if (isValid_) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));
        ASSERT(buffer != nullptr);
        uint16_t offset = AVRC_TG_VENDOR_CTYPE_OFFSET;
        uint64_t payload = 0x00;
        PopOctets1((buffer + offset), payload);
        crCode_ = static_cast<uint8_t>(payload);
        HILOGI("crCode_: %{public}x", crCode_);

        offset = AVRC_TG_VENDOR_COMPANY_ID_OFFSET;
        payload = 0x00;
        PopOctets3((buffer + offset), payload);
        companyId_ = static_cast<uint32_t>(payload);
        HILOGI("companyId_: %{public}x", companyId_);

        offset = AVRC_TG_VENDOR_PACKET_TYPE_OFFSET;
        payload = 0x00;
        PopOctets1((buffer + offset), payload);
        packetType_ = static_cast<uint8_t>(payload);
        HILOGI("packetType_: %{public}x", packetType_);

        isValid_ = DisassembleParameters(buffer);
    } else {
        crCode_ = AVRC_TG_RSP_CODE_REJECTED;
        status_ = AVRC_ES_CODE_INVALID_PARAMETER;
        AssembleRejectPacket();
    }

    return isValid_;
}

bool AvrcTgVendorPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    return false;
}

uint16_t AvrcTgVendorPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    return AVRC_TG_VENDOR_PACKET_TYPE;
}

Packet *AvrcTgVendorPacket::PopAssembledPacket(void)
{
    HILOGI("enter");

    Packet *pkt = nullptr;

    if (pkts_.size() > 0) {
        pkt = pkts_.front();
        pkts_.pop_front();
    }

    HILOGI("pkts_.size: %{public}zu", pkts_.size());

    return pkt;
}

Packet *AvrcTgVendorPacket::AssembleRejectPacket(void)
{
    HILOGI("enter");

    Packet *pkt = nullptr;

    if ((crCode_ == AVRC_TG_RSP_CODE_REJECTED) || (crCode_ == AVRC_TG_RSP_CODE_NOT_IMPLEMENTED)) {
        size_t size = AVRC_TG_VENDOR_FIXED_OPERAND_SIZE;

        size += AVRC_TG_VENDOR_ERROR_CODE_SIZE;
        parameterLength_ = AVRC_TG_VENDOR_ERROR_CODE_SIZE;

        if (status_ == AVRC_ES_CODE_INVALID) {
            status_ = AVRC_ES_CODE_INTERNAL_ERROR;
        }

        pkt = PacketMalloc(0x00, 0x00, size);
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));
        ASSERT(buffer != nullptr);
        uint16_t offset = 0x00;
        offset += PushOctets1((buffer + offset), crCode_);
        HILOGI("crCode_: %{public}x", crCode_);

        offset += PushOctets1((buffer + offset), (subunitType_ << AVRC_TG_VENDOR_BIT3) | subunitId_);
        HILOGI("subunitType_: %{public}x, subunitId_: %{public}x", subunitType_, subunitId_);

        offset += PushOctets1((buffer + offset), opCode_);
        HILOGI("opCode_: %{public}x", opCode_);

        offset += PushOctets3((buffer + offset), companyId_);
        HILOGI("companyId_: %{public}x", companyId_);

        offset += PushOctets1((buffer + offset), pduId_);
        HILOGI("pduId_: %{public}x", pduId_);

        offset += PushOctets1((buffer + offset), packetType_);
        HILOGI("packetType_: %{public}x", packetType_);

        offset += PushOctets2((buffer + offset), parameterLength_);
        HILOGI("parameterLength_: %{public}u", parameterLength_);

        PushOctets1((buffer + offset), status_);
        HILOGI("status_: %{public}x", status_);

        pkts_.push_back(pkt);
    }

    return pkt;
}

Packet *AvrcTgVendorPacket::AssembleMinOperands(void)
{
    HILOGI("enter");

    Packet *pkt = PacketMalloc(0x00, 0x00, AVRC_TG_VENDOR_MIN_COMMAND_SIZE);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));
    ASSERT(buffer != nullptr);

    uint16_t offset = 0x00;
    offset += PushOctets1((buffer + offset), crCode_);
    HILOGI("crCode_: %{public}x", crCode_);

    offset += PushOctets1((buffer + offset), (subunitType_ << AVRC_TG_VENDOR_BIT3) | subunitId_);
    HILOGI("subunitType_: %{public}x, subunitId_: %{public}x", subunitType_, subunitId_);

    offset += PushOctets1((buffer + offset), opCode_);
    HILOGI("opCode_: %{public}x", opCode_);

    offset += PushOctets3((buffer + offset), companyId_);
    HILOGI("companyId_: %{public}x", companyId_);

    PushOctets1((buffer + offset), pduId_);
    HILOGI("pduId_: %{public}x", pduId_);

    return pkt;
}

bool AvrcTgVendorPacket::IsValidParameterLength(Packet *pkt)
{
    HILOGI("enter");

    bool result = false;

    size_t size = PacketPayloadSize(pkt);
    if (size >= AVRC_TG_VENDOR_FIXED_OPERAND_SIZE) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));
        ASSERT(buffer != nullptr);

        uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint32_t>(payload);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        if (size - AVRC_TG_VENDOR_FIXED_OPERAND_SIZE == parameterLength_) {
            result = true;
        }
    } else {
        HILOGI("The size of the packet is invalid! - actual size: %{public}zu valid min size: %{public}jx",
            size, AVRC_TG_VENDOR_FIXED_OPERAND_SIZE);
    }

    return result;
}

/******************************************************************
 * GetCapability                                                  *
 ******************************************************************/

AvrcTgGcPacket::AvrcTgGcPacket(uint8_t crCode, const std::vector<uint32_t> &companies, uint8_t label)
    : AvrcTgVendorPacket()
{
    HILOGI("crCode: %{public}d, label: %{public}d", crCode, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_GET_CAPABILITIES;
    capabilityId_ = AVRC_TG_GC_CAPABILITY_COMPANY;
    label_ = label;
    companies_ = companies;

    AssemblePackets();
}

AvrcTgGcPacket::AvrcTgGcPacket(uint8_t crCode, const std::vector<uint8_t> &events, uint8_t label) : AvrcTgVendorPacket()
{
    HILOGI("crCode: %{public}d, label: %{public}d", crCode, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_GET_CAPABILITIES;
    capabilityId_ = AVRC_TG_GC_CAPABILITY_EVENT;
    label_ = label;
    events_ = events;

    AssemblePackets();
}

AvrcTgGcPacket::AvrcTgGcPacket(Packet *pkt, uint8_t label) : AvrcTgVendorPacket()
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_STABLE;
    pduId_ = AVRC_TG_PDU_ID_GET_CAPABILITIES;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgGcPacket ::~AvrcTgGcPacket()
{
    HILOGI("enter");

    companies_.clear();
    events_.clear();
}

Packet *AvrcTgGcPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    if (capabilityId_ == AVRC_TG_GC_CAPABILITY_COMPANY) {
        AssembleCompanies(pkt);
    } else if (capabilityId_ == AVRC_TG_GC_CAPABILITY_EVENT) {
        AssembleEvents(pkt);
    } else {
        crCode_ = AVRC_TG_RSP_CODE_REJECTED;
        status_ = AVRC_ES_CODE_INVALID_PARAMETER;
        AssembleRejectPacket();
    }

    return pkt;
}

bool AvrcTgGcPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    do {
        uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        offset += PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint16_t>(payload);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        payload = 0x00;
        PopOctets1((buffer + offset), payload);
        capabilityId_ = static_cast<uint8_t>(payload);
        HILOGI("capabilityId_: %{public}x", capabilityId_);
        if (capabilityId_ != AVRC_TG_GC_CAPABILITY_COMPANY && capabilityId_ != AVRC_TG_GC_CAPABILITY_EVENT) {
            crCode_ = AVRC_TG_RSP_CODE_REJECTED;
            status_ = AVRC_ES_CODE_INVALID_PARAMETER;
            AssembleRejectPacket();
            break;
        }

        isValid_ = true;
        HILOGI("isValid_: %{public}d", isValid_);
    } while (false);

    return isValid_;
}

uint16_t AvrcTgGcPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    uint16_t numOfPkts = AVRC_TG_VENDOR_NUM_OF_PACKETS;

    if (companies_.size() > 0 || events_.size() > 0) {
        numOfPkts = AVRC_TG_GC_NUM_OF_PACKETS;
    } else {
        status_ = AVRC_ES_CODE_INVALID_PARAMETER;
    }
    HILOGI("numOfPkts: %{public}u", numOfPkts);

    return numOfPkts;
}

void AvrcTgGcPacket::AssembleCompanies(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE +
                        AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_TG_GC_COMPANY_PARA_LENGTH * companies_.size();
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}x", packetType_);

    parameterLength_ = bufferSize - (AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE);
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PushOctets1((bufferPtr + offset), capabilityId_);
    HILOGI("capabilityId_: %{public}x", capabilityId_);

    capabilityCount_ = companies_.size();
    offset += PushOctets1((bufferPtr + offset), capabilityCount_);
    HILOGI("capabilityCount_: %{public}d", capabilityCount_);

    for (auto company : companies_) {
        offset += PushOctets3((bufferPtr + offset), company);
        HILOGI("company: %{public}x", company);
    }

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;
}

void AvrcTgGcPacket::AssembleEvents(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = (AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE +
                         AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_TG_GC_EVENT_PARA_LENGTH * events_.size());
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;

    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}x", packetType_);

    parameterLength_ = bufferSize - (AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE);
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PushOctets1((bufferPtr + offset), capabilityId_);
    HILOGI("capabilityId_: %{public}x", capabilityId_);

    capabilityCount_ = events_.size();
    offset += PushOctets1((bufferPtr + offset), capabilityCount_);
    HILOGI("capabilityCount_: %{public}u", capabilityCount_);

    for (auto event : events_) {
        offset += PushOctets1((bufferPtr + offset), event);
        HILOGI("event: %{public}x", event);
    }

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;
}

/******************************************************************
 * ListPlayerApplicationSettingAttributes                         *
 ******************************************************************/

AvrcTgLpasaPacket::AvrcTgLpasaPacket(uint8_t crCode, const std::deque<uint8_t> &attributes, uint8_t label)
    : AvrcTgVendorPacket()
{
    HILOGI("crCode: %{public}d, label: %{public}d", crCode, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES;
    attributes_ = attributes;
    label_ = label;

    AssemblePackets();
}

AvrcTgLpasaPacket::AvrcTgLpasaPacket(Packet *pkt, uint8_t label) : AvrcTgVendorPacket()
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_STABLE;
    pduId_ = AVRC_TG_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgLpasaPacket::~AvrcTgLpasaPacket()
{
    HILOGI("enter");

    attributes_.clear();
}

Packet *AvrcTgLpasaPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    numOfAttributes_ = attributes_.size();
    size_t bufferSize = AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE +
                        AVRC_TG_LPASA_NUM_OF_ATTRIBUTES_SIZE + numOfAttributes_;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}x", packetType_);

    parameterLength_ = numOfAttributes_ + AVRC_TG_LPASA_NUM_OF_ATTRIBUTES_SIZE;
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

bool AvrcTgLpasaPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = true;

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

uint16_t AvrcTgLpasaPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    uint16_t numOfPkts = AVRC_TG_VENDOR_NUM_OF_PACKETS;

    if (attributes_.size() > 0) {
        numOfPkts = AVRC_TG_LPASA_NUM_OF_PACKETS;
    } else {
        status_ = AVRC_ES_CODE_INTERNAL_ERROR;
    }
    HILOGI("numOfPkts: %{public}d", numOfPkts);

    return numOfPkts;
}

/******************************************************************
 * ListPlayerApplicationSettingValues                             *
 ******************************************************************/

AvrcTgLpasvPacket::AvrcTgLpasvPacket(uint8_t crCode, const std::deque<uint8_t> &values, uint8_t label)
    : AvrcTgVendorPacket()
{
    HILOGI("crCode: %{public}d, label: %{public}d", crCode, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES;
    values_ = values;
    label_ = label;

    AssemblePackets();
}

AvrcTgLpasvPacket::AvrcTgLpasvPacket(Packet *pkt, uint8_t label)
    : AvrcTgVendorPacket(), attribute_(AVRC_PLAYER_ATTRIBUTE_ILLEGAL)
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_STABLE;
    pduId_ = AVRC_TG_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgLpasvPacket::~AvrcTgLpasvPacket()
{
    HILOGI("enter");

    values_.clear();
}

Packet *AvrcTgLpasvPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    if (GetNumberOfPackets() > 0) {
        numOfValues_ = values_.size();
        if (numOfValues_ > AVRC_TG_LPASV_MAX_NUM_OF_VALUES) {
            numOfValues_ = AVRC_TG_LPASV_MAX_NUM_OF_VALUES;
        }
        size_t bufferSize = AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE +
                            AVRC_TG_LPASV_NUM_OF_VALUES_SIZE + numOfValues_;
        HILOGI("BufferMalloc: %{public}zu", bufferSize);

        auto buffer = BufferMalloc(bufferSize);
        if (buffer == nullptr) {
            HILOGE("BufferMalloc fail");
            return pkt;
        }
        auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

        uint16_t offset = 0x0000;
        offset += PushOctets1((bufferPtr + offset), packetType_);
        HILOGI("packetType_: %{public}x", packetType_);

        parameterLength_ = numOfValues_ + AVRC_TG_LPASV_NUM_OF_VALUES_SIZE;
        offset += PushOctets2((bufferPtr + offset), parameterLength_);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        offset += PushOctets1((bufferPtr + offset), numOfValues_);
        HILOGI("numOfValues_: %{public}d", numOfValues_);

        for (uint16_t i = 0; i < numOfValues_; i++) {
            offset += PushOctets1((bufferPtr + offset), values_.at(i));
            HILOGI("value: %{public}x", values_.at(i));
        }
        HILOGI("before erase values_.size: %{public}zu", values_.size());
        values_.erase(values_.begin(), values_.begin() + numOfValues_);
        HILOGI("after  erase values_.size: %{public}zu", values_.size());

        PacketPayloadAddLast(pkt, buffer);

        BufferFree(buffer);
        buffer = nullptr;
        bufferPtr = nullptr;
    }

    return pkt;
}

bool AvrcTgLpasvPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    do {
        uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        offset += PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint8_t>(payload);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        payload = 0x00;
        PopOctets1((buffer + offset), payload);
        attribute_ = static_cast<uint8_t>(payload);
        HILOGI("attribute_: %{public}x", attribute_);

        if (!IsValidAttribute()) {
            crCode_ = AVRC_TG_RSP_CODE_REJECTED;
            status_ = AVRC_ES_CODE_INVALID_PARAMETER;
            AssembleRejectPacket();
            break;
        }

        isValid_ = true;
    } while (false);

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

uint16_t AvrcTgLpasvPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    uint16_t numOfPkts = AVRC_TG_VENDOR_NUM_OF_PACKETS;

    if (values_.size() > 0) {
        double paramSize = values_.size();
        numOfPkts = ceil(paramSize / AVRC_TG_LPASV_MAX_NUM_OF_VALUES);
    } else {
        status_ = AVRC_ES_CODE_INTERNAL_ERROR;
    }
    HILOGI("numOfPkts: %{public}d", numOfPkts);

    return numOfPkts;
}

bool AvrcTgLpasvPacket::IsValidAttribute(void) const
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

AvrcTgGcpasvPacket::AvrcTgGcpasvPacket(uint16_t mtu, uint8_t crCode, const std::deque<uint8_t> &attributes,
    const std::deque<uint8_t> &values, uint8_t label)
    : AvrcTgVendorPacket()
{
    HILOGI("mtu: %{public}d, crCode: %{public}d, label: %{public}d", mtu, crCode, label);

    if (mtu > AVRC_TG_DEFAULT_CONTROL_MTU_SIZE) {
        mtu_ = mtu;
    }

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE;
    attributes_ = attributes;
    values_ = values;
    label_ = label;

    AssemblePackets();
}

AvrcTgGcpasvPacket::AvrcTgGcpasvPacket(Packet *pkt, uint8_t label) : AvrcTgVendorPacket()
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_STABLE;
    pduId_ = AVRC_TG_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgGcpasvPacket::~AvrcTgGcpasvPacket()
{
    HILOGI("enter");

    attributes_.clear();
    values_.clear();
}

Packet *AvrcTgGcpasvPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    if (GetNumberOfPackets() > 0) {
        numOfValues_ = GetAvailableParameterSize() / AVRC_TG_LPASV_NUM_OF_VALUES_AND_ATTRIBUTE;
        if (numOfValues_ > values_.size()) {
            numOfValues_ = values_.size();
        }
        size_t bufferSize = AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE +
                            AVRC_TG_GCPASV_NUM_OF_VALUES_SIZE + numOfValues_ + numOfValues_;
        HILOGI("BufferMalloc: %{public}zu", bufferSize);

        auto buffer = BufferMalloc(bufferSize);
        if (buffer == nullptr) {
            HILOGE("BufferMalloc fail");
            return pkt;
        }
        auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

        uint16_t offset = 0x0000;
        offset += PushOctets1((bufferPtr + offset), packetType_);
        HILOGI("packetType_: %{public}x", packetType_);

        parameterLength_ = numOfValues_ + numOfValues_ + AVRC_TG_GCPASV_NUM_OF_VALUES_SIZE;
        offset += PushOctets2((bufferPtr + offset), parameterLength_);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        offset += PushOctets1((bufferPtr + offset), numOfValues_);
        HILOGI("numOfValues_: %{public}d", numOfValues_);

        for (int i = 0; i < numOfValues_; i++) {
            offset += PushOctets1((bufferPtr + offset), attributes_.at(i));
            HILOGI("attribute: %{public}x", attributes_.at(i));
            offset += PushOctets1((bufferPtr + offset), values_.at(i));
            HILOGI("value: %{public}x", values_.at(i));
        }

        HILOGI("before erase attributes_.size: %{public}zu, values_.size: %{public}zu",
            attributes_.size(), values_.size());
        attributes_.erase(attributes_.begin(), attributes_.begin() + numOfValues_);
        values_.erase(values_.begin(), values_.begin() + numOfValues_);
        HILOGI("after erase attributes_.size: %{public}zu, values_.size: %{public}zu",
            attributes_.size(), values_.size());

        PacketPayloadAddLast(pkt, buffer);

        BufferFree(buffer);
        buffer = nullptr;
        bufferPtr = nullptr;
    }

    return pkt;
}

bool AvrcTgGcpasvPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    do {
        uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        offset += PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint8_t>(payload);
        HILOGI("parameterLength_: %{public}u", parameterLength_);

        payload = 0x00;
        offset += PopOctets1((buffer + offset), payload);
        numOfAttributes_ = static_cast<uint8_t>(payload);
        HILOGI("numOfAttributes_: %{public}u", numOfAttributes_);

        for (int i = 0; i < numOfAttributes_; i++) {
            payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            attributes_.push_back(static_cast<uint8_t>(payload));
            HILOGI("attribute: %{public}u", attributes_.back());
        }

        if (!IsValidAttribute()) {
            crCode_ = AVRC_TG_RSP_CODE_REJECTED;
            status_ = AVRC_ES_CODE_INVALID_PARAMETER;
            AssembleRejectPacket();
            break;
        }

        isValid_ = true;
    } while (false);

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

uint16_t AvrcTgGcpasvPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    uint16_t numOfPkts = AVRC_TG_VENDOR_NUM_OF_PACKETS;

    if (values_.size() > 0) {
        double paramSize = attributes_.size() + values_.size();
        numOfPkts = ceil(paramSize / GetAvailableParameterSize());
    } else {
        crCode_ = AVRC_TG_RSP_CODE_NOT_IMPLEMENTED;
    }

    HILOGI("numOfPkts: %{public}hu", numOfPkts);

    return numOfPkts;
}

uint16_t AvrcTgGcpasvPacket::GetAvailableParameterSize(void) const
{
    HILOGI("enter");

    uint16_t remain = mtu_ - (AVRC_TG_VENDOR_FIXED_OPERAND_SIZE + AVRC_TG_GCPASV_NUM_OF_VALUES_SIZE);
    if (remain > (AVRC_TG_GCPASV_MAX_NUM_OF_ATTRIBUTES + AVRC_TG_GCPASV_MAX_NUM_OF_VALUES)) {
        remain = (AVRC_TG_GCPASV_MAX_NUM_OF_ATTRIBUTES + AVRC_TG_GCPASV_MAX_NUM_OF_VALUES);
    }
    uint16_t unitsOfParam = AVRC_TG_GCPASV_ATTRIBUTE_SIZE + AVRC_TG_GCPASV_VALUE_SIZE;

    return (remain / unitsOfParam) * unitsOfParam;
}

bool AvrcTgGcpasvPacket::IsValidAttribute(void) const
{
    HILOGI("enter");

    bool result = false;

    for (auto attribute : attributes_) {
        do {
            if (attribute == AVRC_PLAYER_ATTRIBUTE_ILLEGAL) {
                break;
            }

            if ((attribute >= AVRC_PLAYER_ATTRIBUTE_RESERVED_MIN)
                && (attribute <= AVRC_PLAYER_ATTRIBUTE_RESERVED_MAX)) {
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

AvrcTgSpasvPacket::AvrcTgSpasvPacket(uint8_t crCode, uint8_t label)
{
    HILOGI("crCode: %{public}d, label: %{public}d", crCode, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE;
    label_ = label;

    AssemblePackets();
}

AvrcTgSpasvPacket::AvrcTgSpasvPacket(Packet *pkt, uint8_t label)
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_ACCEPTED;
    pduId_ = AVRC_TG_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgSpasvPacket::~AvrcTgSpasvPacket()
{
    HILOGI("enter");
}

Packet *AvrcTgSpasvPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize = AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}x", packetType_);

    PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);

    return pkt;
}

bool AvrcTgSpasvPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    do {
        uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        offset += PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint8_t>(payload);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        payload = 0x00;
        offset += PopOctets1((buffer + offset), payload);
        numOfAttributes_ = static_cast<uint8_t>(payload);
        HILOGI("numOfAttributes_: %{public}d", numOfAttributes_);

        for (int i = 0; i < numOfAttributes_; i++) {
            payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            attributes_.push_back(static_cast<uint8_t>(payload));
            HILOGI("attribute: %{public}x", attributes_.back());

            payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            values_.push_back(static_cast<uint8_t>(payload));
            HILOGI("value: %{public}x", values_.back());
        }

        if (!IsValidAttributeAndValue()) {
            crCode_ = AVRC_TG_RSP_CODE_REJECTED;
            status_ = AVRC_ES_CODE_INVALID_PARAMETER;
            AssembleRejectPacket();
            break;
        }

        isValid_ = true;
    } while (false);

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

uint16_t AvrcTgSpasvPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    return AVRC_TG_SPASV_NUM_OF_PACKETS;
}

bool AvrcTgSpasvPacket::IsValidAttributeAndValue(void) const
{
    HILOGI("enter");

    bool result = false;

    for (size_t i = 0; i < attributes_.size(); i++) {
        do {
            if (attributes_.at(i) == AVRC_PLAYER_ATTRIBUTE_ILLEGAL) {
                break;
            }

            if (attributes_.at(i) >= AVRC_PLAYER_ATTRIBUTE_RESERVED_MIN &&
                attributes_.at(i) <= AVRC_PLAYER_ATTRIBUTE_RESERVED_MAX) {
                break;
            }

            if (values_.at(i) >= AVRC_PLAYER_ATTRIBUTE_RESERVED_MIN &&
                values_.at(i) <= AVRC_PLAYER_ATTRIBUTE_RESERVED_MAX) {
                break;
            }

            if (attributes_.at(i) == AVRC_PLAYER_ATTRIBUTE_EQUALIZER &&
                (values_.at(i) < AVRC_EQUALIZER_OFF || values_.at(i) > AVRC_EQUALIZER_ON)) {
                break;
            }

            if (attributes_.at(i) == AVRC_PLAYER_ATTRIBUTE_REPEAT &&
                (values_.at(i) < AVRC_REPEAT_OFF || values_.at(i) > AVRC_REPEAT_GROUP)) {
                break;
            }

            if (attributes_.at(i) == AVRC_PLAYER_ATTRIBUTE_SHUFFLE &&
                (values_.at(i) < AVRC_SHUFFLE_OFF || values_.at(i) > AVRC_SHUFFLE_GROUP)) {
                break;
            }

            if (attributes_.at(i) == AVRC_PLAYER_ATTRIBUTE_SCAN &&
                (values_.at(i) < AVRC_SCAN_OFF || values_.at(i) > AVRC_SCAN_GROUP)) {
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

AvrcTgGpasatPacket::AvrcTgGpasatPacket(
    uint8_t crCode, const std::vector<uint8_t> &attributes, const std::vector<std::string> &attrName, uint8_t label)
{
    HILOGI("crCode: %{public}d, label: %{public}d", crCode, label);

    pduId_ = AVRC_TG_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;
    crCode_ = crCode;
    attributes_ = attributes;
    attributeName_ = attrName;
    numOfAttributes_ = attributes.size();
    HILOGI("numOfAttributes_: %{public}d", numOfAttributes_);
    label_ = label;

    AssemblePackets();
}

AvrcTgGpasatPacket::AvrcTgGpasatPacket(Packet *pkt, uint8_t label)
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_STABLE;
    pduId_ = AVRC_TG_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgGpasatPacket ::~AvrcTgGpasatPacket()
{
    HILOGI("enter");

    attributes_.clear();
    attributeName_.clear();
}

Packet *AvrcTgGpasatPacket ::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    if (packetPos_.size() > 0) {
        number_ = packetPos_.front().first;
        parameterLength_ = packetPos_.front().second + AVRC_TG_GPASAT_ATTRIBUTE_NUM_LENGTH;
    }
    size_t bufferSize = parameterLength_ + AVRC_TG_GPASAT_PARAMETER_LENGTH_SIZE;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);
    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}d", packetType_);
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);
    offset += PushOctets1((bufferPtr + offset), number_);
    HILOGI("number_: %{public}d", number_);
    for (int i = 0; i < number_; i++) {
        offset += PushOctets1((bufferPtr + offset), attributes_.at(i));
        offset += PushOctets2((bufferPtr + offset), static_cast<uint16_t>(AVRC_MEDIA_CHARACTER_SET_UTF8));
        offset += PushOctets1((bufferPtr + offset), attributeName_.at(i).length());
        for (auto it = attributeName_.at(i).begin(); it != attributeName_.at(i).end(); it++) {
            offset += PushOctets1((bufferPtr + offset), *it);
        }
        HILOGI("attributeName_: %{public}s", attributeName_.at(i).c_str());
    }
    HILOGI("values_.size: %{public}zu", attributeName_.size());
    attributes_.erase(attributes_.begin(), attributes_.begin() + number_);
    attributeName_.erase(attributeName_.begin(), attributeName_.begin() + number_);
    packetPos_.pop_front();

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcTgGpasatPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    payload = 0x00;
    offset += PopOctets1((buffer + offset), payload);
    number_ = static_cast<uint8_t>(payload);
    HILOGI("number_: %{public}x", number_);

    for (int i = 0; i < number_; i++) {
        payload = 0x00;
        offset += PopOctets1((buffer + offset), payload);
        attributes_.push_back(static_cast<uint8_t>(payload));
        HILOGI("attribute: %{public}d", attributes_.at(i));
    }
    isValid_ = true;
    for (auto attribute : attributes_) {
        auto result = IsValidAttribute(attribute);
        if (!result) {
            isValid_ = false;
            break;
        }
    }
    if (!isValid_) {
        crCode_ = AVRC_TG_RSP_CODE_REJECTED;
        status_ = AVRC_ES_CODE_INVALID_PARAMETER;
        AssembleRejectPacket();
    }

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

uint16_t AvrcTgGpasatPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");
    uint16_t totalLength = 0x00;
    uint8_t totalNumber = 0x00;
    int packetNum = 1;
    std::pair<uint16_t, uint16_t> tem;
    for (int i = 0; i < numOfAttributes_; i++) {
        if (totalLength + attributeName_[i].length() + AVRC_TG_GPASAT_PARAMETER_EXCEPT_ATTRIBUTE_NUM <
            AVRC_TG_GPASAT_ASSEMBLE_MAX_SIZE) {
            totalLength += (attributeName_[i].length() + AVRC_TG_GPASAT_PARAMETER_EXCEPT_ATTRIBUTE_NUM);
            totalNumber++;
        } else {
            packetNum++;
            i--;
            tem = std::make_pair(totalNumber, totalLength);
            packetPos_.push_back(tem);
            totalLength = 0x00;
            totalNumber = 0x00;
        }
    }
    if (packetNum == 1 || totalNumber != 0) {
        tem = std::make_pair(totalNumber, totalLength);
        packetPos_.push_back(tem);
    }
    HILOGI("packetNum: %{public}x", packetNum);

    return packetNum;
}

bool AvrcTgGpasatPacket::IsValidAttribute(uint8_t attribute)
{
    HILOGI("attribute: %{public}d", attribute);

    bool result = false;

    do {
        if (attribute == AVRC_PLAYER_ATTRIBUTE_ILLEGAL) {
            break;
        }

        if (attribute >= AVRC_PLAYER_ATTRIBUTE_RESERVED_MIN && attribute <= AVRC_PLAYER_ATTRIBUTE_RESERVED_MAX) {
            break;
        }

        result = true;
    } while (false);

    HILOGI("result: %{public}d", result);

    return result;
}

/******************************************************************
 * GetPlayerApplicationSettingValueText                           *
 ******************************************************************/

AvrcTgGpasvtPacket::AvrcTgGpasvtPacket(
    uint8_t crCode, const std::vector<uint8_t> &values, const std::vector<std::string> &valueName, uint8_t label)
{
    HILOGI("crCode: %{public}d, label: %{public}d", crCode, label);

    pduId_ = AVRC_TG_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT;
    crCode_ = crCode;
    values_ = values;
    valueName_ = valueName;
    numOfValues_ = values.size();
    label_ = label;

    AssemblePackets();
}

AvrcTgGpasvtPacket::AvrcTgGpasvtPacket(Packet *pkt, uint8_t label)
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_STABLE;
    pduId_ = AVRC_TG_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgGpasvtPacket ::~AvrcTgGpasvtPacket()
{
    HILOGI("enter");

    values_.clear();
    valueName_.clear();
}

Packet *AvrcTgGpasvtPacket ::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    if (packetPos_.size() > 0) {
        number_ = packetPos_.front().first;
        parameterLength_ = packetPos_.front().second + AVRC_TG_GPASVT_NUM_OF_ATTRIBUTE_SIZE;
    }

    size_t bufferSize = parameterLength_ + AVRC_TG_GPASVT_PARAMETER_LENGTH_SIZE;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);
    auto buffer = BufferMalloc(bufferSize);
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}d", packetType_);
    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);
    offset += PushOctets1((bufferPtr + offset), number_);
    HILOGI("number_: %{public}d", number_);
    for (int i = 0; i < number_; i++) {
        offset += PushOctets1((bufferPtr + offset), values_.at(i));
        offset += PushOctets2((bufferPtr + offset), static_cast<uint16_t>(AVRC_MEDIA_CHARACTER_SET_UTF8));
        offset += PushOctets1((bufferPtr + offset), valueName_.at(i).length());
        for (auto it = valueName_.at(i).begin(); it != valueName_.at(i).end(); it++) {
            offset += PushOctets1((bufferPtr + offset), *it);
        }
        HILOGI("valueName_: %{public}s", valueName_.at(i).c_str());
    }
    HILOGI("values_.size: %{public}zu", valueName_.size());
    values_.erase(values_.begin(), values_.begin() + number_);
    valueName_.erase(valueName_.begin(), valueName_.begin() + number_);
    packetPos_.pop_front();

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcTgGpasvtPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");
    do {
        uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        offset += PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint16_t>(payload);
        HILOGI("parameterLength_: %{public}d", parameterLength_);
        payload = 0x00;
        offset += PopOctets1((buffer + offset), payload);
        attributeId_ = static_cast<uint8_t>(payload);
        HILOGI("attributeId_: %{public}d", attributeId_);

        payload = 0x00;
        offset += PopOctets1((buffer + offset), payload);
        number_ = static_cast<uint8_t>(payload);
        HILOGI("number_: %{public}x", number_);

        for (int i = 0; i < number_; i++) {
            payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            values_.push_back(static_cast<uint8_t>(payload));
            HILOGI("value: %{public}x", values_.at(i));
        }
        if (!IsValidAttributeAndValue()) {
            crCode_ = AVRC_TG_RSP_CODE_REJECTED;
            status_ = AVRC_ES_CODE_INVALID_PARAMETER;
            AssembleRejectPacket();
            isValid_ = false;
            break;
        }
        isValid_ = true;
    } while (false);

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

uint16_t AvrcTgGpasvtPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    uint16_t totalLength = 0x00;
    uint8_t totalNumber = 0x00;
    int packetNum = 1;
    std::pair<uint16_t, uint16_t> tem;
    for (int i = 0; i < numOfValues_; i++) {
        if (totalLength + valueName_[i].length() + AVRC_TG_GPASVT_PARAMETER_EXCEPT_ATTRIBUTE_NUM <
            AVRC_TG_GPASVT_ASSEMBLE_MAX_SIZE) {
            totalLength += (valueName_[i].length() + AVRC_TG_GPASVT_PARAMETER_EXCEPT_ATTRIBUTE_NUM);
            totalNumber++;
        } else {
            packetNum++;
            i--;
            tem = std::make_pair(totalNumber, totalLength);
            packetPos_.push_back(tem);
            totalLength = 0x00;
            totalNumber = 0x00;
        }
    }
    if (packetNum == 1 || totalNumber != 0) {
        tem = std::make_pair(totalNumber, totalLength);
        packetPos_.push_back(tem);
    }

    HILOGI("packetNum: %{public}x", packetNum);

    return packetNum;
}

bool AvrcTgGpasvtPacket::IsValidAttributeAndValue(void) const
{
    HILOGI("enter");

    bool result = true;

    switch (attributeId_) {
        case AVRC_PLAYER_ATTRIBUTE_EQUALIZER:
            if (any_of(values_.begin(), values_.end(),
                [](int value){return (value < AVRC_EQUALIZER_OFF || value > AVRC_EQUALIZER_ON);})){
                result = false;
            }
            break;
        case AVRC_PLAYER_ATTRIBUTE_REPEAT:
            if (any_of(values_.begin(), values_.end(),
                [](int value){return (value < AVRC_REPEAT_OFF || value > AVRC_REPEAT_GROUP);})){
                result = false;
            }
            break;
        case AVRC_PLAYER_ATTRIBUTE_SHUFFLE:
            if (any_of(values_.begin(), values_.end(),
                [](int value){return (value < AVRC_SHUFFLE_OFF || value > AVRC_SHUFFLE_GROUP);})){
                result = false;
            }
            break;
        case AVRC_PLAYER_ATTRIBUTE_SCAN:
            if (any_of(values_.begin(), values_.end(),
                [](int value){return (value < AVRC_SCAN_OFF || value > AVRC_SCAN_GROUP);})){
                result = false;
            }
            break;
        default:
            if (attributeId_ > AVRC_PLAYER_ATTRIBUTE_RESERVED_MIN &&
                attributeId_ < AVRC_PLAYER_ATTRIBUTE_RESERVED_MAX) {
                result = false;
            }

            break;
    };

    HILOGI("result: %{public}d", result);

    return result;
}

/******************************************************************
 * GetElementAttributePacket                                      *
 ******************************************************************/

AvrcTgGeaPacket::AvrcTgGeaPacket(
    uint8_t crCode, const std::vector<uint32_t> &attribtues, const std::vector<std::string> &values, uint8_t label)
{
    HILOGI("crCode: %{public}d, label: %{public}d", crCode, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_GET_ELEMENT_ATTRIBUTES;
    attributes_ = attribtues;
    values_ = values;
    numOfValues_ = values.size();
    label_ = label;

    AssemblePackets();
}

AvrcTgGeaPacket::AvrcTgGeaPacket(Packet *pkt, uint8_t label)
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_STABLE;
    pduId_ = AVRC_TG_PDU_ID_GET_ELEMENT_ATTRIBUTES;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgGeaPacket::~AvrcTgGeaPacket()
{
    HILOGI("enter");

    values_.clear();
    attributes_.clear();
    packetPos_.clear();
}

Packet *AvrcTgGeaPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");
    if (packetPos_.size() > 0) {
        number_ = packetPos_.front().first;
        parameterLength_ = packetPos_.front().second + AVRC_TG_GEA_SIZE_OF_ATTRIBUTE_LENGTH;
    }

    size_t bufferSize = parameterLength_ + AVRC_TG_GEA_PARAMETER_LENGTH_SIZE;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);
    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}x", packetType_);

    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PushOctets1((bufferPtr + offset), number_);
    HILOGI("number_: %{public}d", number_);

    for (int i = 0; i < number_; i++) {
        offset += PushOctets4((bufferPtr + offset), attributes_.at(i));
        HILOGI("attribute: %{public}x", attributes_.at(i));

        offset += PushOctets2((bufferPtr + offset), static_cast<uint16_t>(AVRC_MEDIA_CHARACTER_SET_UTF8));

        offset += PushOctets2((bufferPtr + offset), values_.at(i).length());
        HILOGI("valuelength: %{public}zu", values_.at(i).length());

        size_t length = values_.at(i).length();
        while (length > 0 && offset < AVRC_TG_GEA_ASSEMBLE_MAX_SIZE + AVRC_TG_GEA_SIZE_OF_ATTRIBUTE_LENGTH +
            AVRC_TG_GEA_PARAMETER_LENGTH_SIZE) {
            offset += PushOctets1((bufferPtr + offset), values_.at(i).front());
            values_.at(i).erase(values_.at(i).begin());
            length--;
        }
    }
    HILOGI("values_.size: %{public}zu", values_.size());
    if (values_.at(number_ - 1).length() == 0) {
        attributes_.erase(attributes_.begin(), attributes_.begin() + number_);
        values_.erase(values_.begin(), values_.begin() + number_);
    } else {
        attributes_.erase(attributes_.begin(), attributes_.begin() + number_ - 1);
        values_.erase(values_.begin(), values_.begin() + number_ - 1);
    }

    packetPos_.pop_front();
    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcTgGeaPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    payload = 0x00;
    offset += PopOctets8((buffer + offset), payload);
    identifier_ = payload;
    HILOGI("identifier_: %{public}jx", identifier_);

    payload = 0x00;
    offset += PopOctets1((buffer + offset), payload);
    number_ = static_cast<uint8_t>(payload);
    HILOGI("number_: %{public}hhx", number_);

    if (number_ != AVRC_TG_GEA_NUMBER_OF_ATTRIBUTES) {
        for (int i = 0; i < number_; i++) {
            payload = 0x00;
            offset += PopOctets4((buffer + offset), payload);
            attributes_.push_back(static_cast<uint32_t>(payload));
            HILOGI("attributes_: %{public}x", attributes_.at(i));
        }
    } else {
        attributes_.push_back(bluetooth::AVRC_MEDIA_ATTRIBUTE_TITLE);
        attributes_.push_back(bluetooth::AVRC_MEDIA_ATTRIBUTE_ARTIST_NAME);
        attributes_.push_back(bluetooth::AVRC_MEDIA_ATTRIBUTE_ALBUM_NAME);
        attributes_.push_back(bluetooth::AVRC_MEDIA_ATTRIBUTE_TRACK_NUMBER);
        attributes_.push_back(bluetooth::AVRC_MEDIA_ATTRIBUTE_TOTAL_NUMBER_OF_TRACKS);
        attributes_.push_back(bluetooth::AVRC_MEDIA_ATTRIBUTE_GENRE);
        attributes_.push_back(bluetooth::AVRC_MEDIA_ATTRIBUTE_PLAYING_TIME);
        attributes_.push_back(bluetooth::AVRC_MEDIA_ATTRIBUTE_DEFAULT_COVER_ART);
    }

    isValid_ = true;

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

uint16_t AvrcTgGeaPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    uint16_t totalLength = 0x00;
    uint8_t totalNumber = 0x00;
    int packetNum = 1;
    std::pair<uint16_t, uint16_t> tem;
    for (int i = 0; i < numOfValues_; i++) {
        if (totalLength + AVRC_TG_GEA_ATTRIBUTE_LENGTH < AVRC_TG_GEA_ASSEMBLE_MAX_SIZE) {
            totalLength += +AVRC_TG_GEA_ATTRIBUTE_LENGTH;
            totalNumber++;
        } else {
            packetNum++;
            i--;
            tem = std::make_pair(totalNumber, totalLength);
            packetPos_.push_back(tem);
            totalLength = 0x00;
            totalNumber = 0x00;
            continue;
        }

        if (totalLength + values_.at(i).length() <= AVRC_TG_GEA_ASSEMBLE_MAX_SIZE) {
            totalLength += values_.at(i).length();
            continue;
        }
        int length = values_.at(i).length();
        do {
            if (length > AVRC_TG_GEA_ASSEMBLE_MAX_SIZE - totalLength) {
                length -= (AVRC_TG_GEA_ASSEMBLE_MAX_SIZE - totalLength);
                packetNum++;
                tem = std::make_pair(totalNumber, AVRC_TG_GEA_ASSEMBLE_MAX_SIZE);
                packetPos_.push_back(tem);
                totalNumber = 1;
                totalLength = AVRC_TG_GEA_ATTRIBUTE_LENGTH;
            } else {
                totalLength += length;
                break;
            }
        } while (length > 0);
    }
    if (packetNum == 1 || totalNumber != 0) {
        tem = std::make_pair(totalNumber, totalLength);
        packetPos_.push_back(tem);
    }
    HILOGI("packetNum: %{public}x", packetNum);
    return packetNum;
}

/******************************************************************
 * GetPlayStatus                                                  *
 ******************************************************************/

AvrcTgGpsPacket::AvrcTgGpsPacket(
    uint8_t crCode, uint32_t songLength, uint32_t songPosition, uint8_t playStatus, uint8_t label)
    : AvrcTgVendorPacket()
{
    HILOGI("crCode:%{public}d, songLength:%{public}u, songPosition:%{public}u, playStatus:%{public}d, "
        "label:%{public}d", crCode, songLength, songPosition, playStatus, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_GET_PLAY_STATUS;
    parameterLength_ = AVRC_TG_GPS_PARAMETER_LENGTH;
    songLength_ = songLength;
    songPosition_ = songPosition;
    playStatus_ = playStatus;
    label_ = label;

    AssemblePackets();
}

AvrcTgGpsPacket::AvrcTgGpsPacket(Packet *pkt, uint8_t label)
    : AvrcTgVendorPacket(),
      songLength_(AVRC_PLAY_STATUS_INVALID_SONG_LENGTH),
      songPosition_(AVRC_PLAY_STATUS_INVALID_SONG_POSITION),
      playStatus_(AVRC_PLAY_STATUS_ERROR)
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_STABLE;
    pduId_ = AVRC_TG_PDU_ID_GET_PLAY_STATUS;
    parameterLength_ = AVRC_TG_GPS_PARAMETER_LENGTH;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgGpsPacket::~AvrcTgGpsPacket(void)
{
    HILOGI("enter");
}

Packet *AvrcTgGpsPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    auto buffer = BufferMalloc(
        AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_TG_GPS_PARAMETER_LENGTH);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));
    HILOGI("BufferMalloc: %{public}ju",
        (AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_TG_GPS_PARAMETER_LENGTH));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}hhx", packetType_);

    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}hu", parameterLength_);

    offset += PushOctets4((bufferPtr + offset), songLength_);
    HILOGI("songLength_: %{public}u", songLength_);

    offset += PushOctets4((bufferPtr + offset), songPosition_);
    HILOGI("songPosition_: %{public}u", songPosition_);

    PushOctets1((bufferPtr + offset), playStatus_);
    HILOGI("playStatus_: %{public}hhx", playStatus_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcTgGpsPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = true;

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

uint16_t AvrcTgGpsPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    return AVRC_TG_GPS_NUM_OF_PACKETS;
}

/******************************************************************
 * RequestContinuingResponse                                      *
 ******************************************************************/

AvrcTgRcrPacket::AvrcTgRcrPacket() : AvrcTgVendorPacket()
{
    HILOGI("enter");

    crCode_ = AVRC_TG_RSP_CODE_STABLE;
    pduId_ = AVRC_TG_PDU_ID_REQUEST_CONTINUING_RESPONSE;
}

AvrcTgRcrPacket::AvrcTgRcrPacket(Packet *pkt, uint8_t label) : AvrcTgVendorPacket()
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_STABLE;
    pduId_ = AVRC_TG_PDU_ID_REQUEST_CONTINUING_RESPONSE;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgRcrPacket::~AvrcTgRcrPacket()
{
    HILOGI("enter");
}

Packet *AvrcTgRcrPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    return nullptr;
}

bool AvrcTgRcrPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset = AVRC_TG_RCR_REQUEST_CONTINUING_PDU_ID_OFFSET;
    payload = 0x00;
    PopOctets1((buffer + offset), payload);
    continuePduId_ = static_cast<uint8_t>(payload);
    HILOGI("continuePduId_: %{public}x", continuePduId_);

    isValid_ = true;

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

uint16_t AvrcTgRcrPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    return AVRC_TG_RCR_NUM_OF_PACKETS;
}

/******************************************************************
 * AbortContinuingResponse                                        *
 ******************************************************************/

AvrcTgAcrPacket::AvrcTgAcrPacket()
{
    HILOGI("enter");
}

AvrcTgAcrPacket::AvrcTgAcrPacket(uint8_t pduId, uint8_t label) : AvrcTgVendorPacket()
{
    HILOGI("pduId: %{public}d, label: %{public}d", pduId, label);

    crCode_ = AVRC_TG_RSP_CODE_ACCEPTED;
    pduId_ = AVRC_TG_PDU_ID_ABORT_CONTINUING_RESPONSE;
    parameterLength_ = AVRC_TG_ACR_PARAMETER_LENGTH;
    continuePduId_ = pduId;

    AssemblePackets();
}

AvrcTgAcrPacket::AvrcTgAcrPacket(Packet *pkt, uint8_t label) : AvrcTgVendorPacket()
{
    HILOGI("label: %{public}d", label);

    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgAcrPacket::~AvrcTgAcrPacket()
{
    HILOGI("enter");
}

Packet *AvrcTgAcrPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    auto buffer = BufferMalloc(AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));
    HILOGI("BufferMalloc: %{public}ju",
        (AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}hhx", packetType_);

    PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}hx", parameterLength_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcTgAcrPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset = AVRC_TG_ACR_REQUEST_CONTINUING_PDU_ID_OFFSET;
    payload = 0x00;
    PopOctets1((buffer + offset), payload);
    continuePduId_ = static_cast<uint8_t>(payload);
    HILOGI("continuePduId_: %{public}x", continuePduId_);

    isValid_ = true;

    HILOGI("isValid_: %{public}d", isValid_);

    return isValid_;
}

uint16_t AvrcTgAcrPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    return AVRC_TG_ACR_NUM_OF_PACKETS;
}

/******************************************************************
 * SetAddressedPlayer                                             *
 ******************************************************************/

AvrcTgSapPacket::AvrcTgSapPacket(uint8_t crCode, uint8_t status, uint8_t label) : AvrcTgVendorPacket()
{
    HILOGI("crCode: %{public}d, status: %{public}d, label: %{public}d", crCode, status, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_SET_ADDRESSED_PLAYER;
    parameterLength_ = AVRC_TG_SAP_PARAMETER_LENGTH;
    status_ = status;
    label_ = label;

    AssemblePackets();
}

AvrcTgSapPacket::AvrcTgSapPacket(Packet *pkt, uint8_t label) : AvrcTgVendorPacket()
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_ACCEPTED;
    pduId_ = AVRC_TG_PDU_ID_SET_ADDRESSED_PLAYER;
    parameterLength_ = AVRC_TG_SAP_PARAMETER_LENGTH;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgSapPacket::~AvrcTgSapPacket()
{
    HILOGI("enter");
}

Packet *AvrcTgSapPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize =
        AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_TG_SAP_PARAMETER_LENGTH;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}x", packetType_);

    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PushOctets1((bufferPtr + offset), status_);
    HILOGI("status_: %{public}x", status_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcTgSapPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    do {
        uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        offset += PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint16_t>(payload);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        payload = 0x00;
        PopOctets2((buffer + offset), payload);
        playerId_ = static_cast<uint16_t>(payload);
        HILOGI("playerId_: %{public}x", playerId_);

        isValid_ = true;

        HILOGI("isValid_: %{public}d", isValid_);
    } while (false);

    return isValid_;
}

uint16_t AvrcTgSapPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    return AVRC_TG_SAP_NUM_OF_PACKETS;
}

/******************************************************************
 * PlayItem                                                       *
 ******************************************************************/

AvrcTgPiPacket::AvrcTgPiPacket(uint8_t crCode, uint8_t status, uint8_t label)
    : AvrcTgVendorPacket(), scope_(AVRC_MEDIA_SCOPE_INVALID)
{
    HILOGI("crCode: %{public}d, status: %{public}d, label: %{public}d", crCode, status, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_PLAY_ITEM;
    parameterLength_ = AVRC_TG_PI_PARAMETER_LENGTH;
    label_ = label;
    status_ = status;

    AssemblePackets();
}

AvrcTgPiPacket::AvrcTgPiPacket(Packet *pkt, uint8_t label) : AvrcTgVendorPacket(), scope_(AVRC_MEDIA_SCOPE_INVALID)
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_ACCEPTED;
    pduId_ = AVRC_TG_PDU_ID_PLAY_ITEM;
    parameterLength_ = AVRC_TG_PI_PARAMETER_LENGTH;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgPiPacket::~AvrcTgPiPacket(void)
{
    HILOGI("enter");
}

Packet *AvrcTgPiPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize =
        AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_TG_PI_PARAMETER_LENGTH;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}x", packetType_);

    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PushOctets1((bufferPtr + offset), status_);
    HILOGI("status_: %{public}x", status_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcTgPiPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
    uint64_t payload = 0x00;
    offset += PopOctets2((buffer + offset), payload);
    parameterLength_ = static_cast<uint16_t>(payload);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    payload = 0x00;
    offset += PopOctets1((buffer + offset), payload);
    scope_ = static_cast<uint8_t>(payload);
    HILOGI("scope_: %{public}x", scope_);

    payload = 0x00;
    offset += PopOctets8((buffer + offset), payload);
    uid_ = payload;
    HILOGI("uid_: %{public}jx", uid_);

    payload = 0x00;
    PopOctets2((buffer + offset), payload);
    uidCounter_ = static_cast<uint16_t>(payload);
    HILOGI("uidCounter_: %{public}d", uidCounter_);

    isValid_ = true;

    return isValid_;
}

uint16_t AvrcTgPiPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    return AVRC_TG_PI_NUM_OF_PACKETS;
}

/******************************************************************
 * AddToNowPlaying                                                *
 ******************************************************************/

AvrcTgAtnpPacket::AvrcTgAtnpPacket(uint8_t crCode, uint8_t status, uint8_t label)
    : AvrcTgVendorPacket(), scope_(AVRC_MEDIA_SCOPE_INVALID)
{
    HILOGI("crCode: %{public}d, status: %{public}d, label: %{public}d", crCode, status, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_ADD_TO_NOW_PLAYING;
    parameterLength_ = AVRC_TG_ATNP_PARAMETER_LENGTH;
    status_ = status;
    label_ = label;

    AssemblePackets();
}

AvrcTgAtnpPacket::AvrcTgAtnpPacket(Packet *pkt, uint8_t label) : AvrcTgVendorPacket(), scope_(AVRC_MEDIA_SCOPE_INVALID)
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_ACCEPTED;
    pduId_ = AVRC_TG_PDU_ID_ADD_TO_NOW_PLAYING;
    parameterLength_ = AVRC_TG_ATNP_PARAMETER_LENGTH;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgAtnpPacket::~AvrcTgAtnpPacket()
{
    HILOGI("enter");
}

Packet *AvrcTgAtnpPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize =
        AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_TG_ATNP_PARAMETER_LENGTH;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}x", packetType_);

    offset += PushOctets2((bufferPtr + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PushOctets1((bufferPtr + offset), status_);
    HILOGI("status_: %{public}x", status_);

    PacketPayloadAddLast(pkt, buffer);

    BufferFree(buffer);
    buffer = nullptr;
    bufferPtr = nullptr;

    return pkt;
}

bool AvrcTgAtnpPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    do {
        uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        offset += PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint16_t>(payload);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        payload = 0x00;
        offset += PopOctets1((buffer + offset), payload);
        scope_ = static_cast<uint8_t>(payload);
        HILOGI("scope_: %{public}x", scope_);
        if (scope_ < AVRC_MEDIA_SCOPE_PLAYER_LIST || scope_ > AVRC_MEDIA_SCOPE_NOW_PLAYING) {
            status_ = AVRC_ES_CODE_INVALID_SCOPE;
            break;
        }

        payload = 0x00;
        offset += PopOctets8((buffer + offset), payload);
        uid_ = static_cast<uint64_t>(payload);
        HILOGI("uid_: %{public}jx", uid_);

        payload = 0x00;
        PopOctets2((buffer + offset), payload);
        uidCounter_ = static_cast<uint16_t>(payload);
        HILOGI("uidCounter_: %{public}d", uidCounter_);

        isValid_ = true;

        HILOGI("isValid_: %{public}d", isValid_);
    } while (false);

    return isValid_;
}

uint16_t AvrcTgAtnpPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    return AVRC_TG_ATNP_NUM_OF_PACKETS;
}

/******************************************************************
 * SetAbsoluteVolume                                              *
 ******************************************************************/

AvrcTgSavPacket::AvrcTgSavPacket(uint8_t crCode, uint8_t volume, uint8_t label)
    : AvrcTgVendorPacket(), volume_(AVRC_ABSOLUTE_VOLUME_INVALID)
{
    HILOGI("crCode: %{public}d, volume: %{public}d, label: %{public}d", crCode, volume, label);

    crCode_ = crCode;
    pduId_ = AVRC_TG_PDU_ID_SET_ABSOLUTE_VOLUME;
    parameterLength_ = AVRC_TG_SAV_VOLUME_SIZE;
    label_ = label;
    volume_ = volume;

    AssemblePackets();
}

AvrcTgSavPacket::AvrcTgSavPacket(Packet *pkt, uint8_t label)
    : AvrcTgVendorPacket(), volume_(AVRC_ABSOLUTE_VOLUME_INVALID)
{
    HILOGI("label: %{public}d", label);

    crCode_ = AVRC_TG_RSP_CODE_ACCEPTED;
    pduId_ = AVRC_TG_PDU_ID_SET_ABSOLUTE_VOLUME;
    parameterLength_ = AVRC_TG_SAV_VOLUME_SIZE;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgSavPacket::~AvrcTgSavPacket()
{
    HILOGI("enter");
}

Packet *AvrcTgSavPacket::AssembleParameters(Packet *pkt)
{
    HILOGI("enter");

    size_t bufferSize =
        AVRC_TG_VENDOR_PACKET_TYPE_SIZE + AVRC_TG_VENDOR_PARAMETER_LENGTH_SIZE + AVRC_TG_SAV_VOLUME_SIZE;
    HILOGI("BufferMalloc: %{public}zu", bufferSize);

    auto buffer = BufferMalloc(bufferSize);
    if (buffer == nullptr) {
        HILOGE("BufferMalloc fail");
        return pkt;
    }
    auto bufferPtr = static_cast<uint8_t *>(BufferPtr(buffer));

    uint16_t offset = 0x0000;
    offset += PushOctets1((bufferPtr + offset), packetType_);
    HILOGI("packetType_: %{public}x", packetType_);

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

bool AvrcTgSavPacket::DisassembleParameters(uint8_t *buffer)
{
    HILOGI("enter");

    isValid_ = false;

    do {
        uint16_t offset = AVRC_TG_VENDOR_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        offset += PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint16_t>(payload);
        HILOGI("parameterLength_: %{public}d", parameterLength_);

        payload = 0x00;
        PopOctets1((buffer + offset), payload);
        volume_ = static_cast<uint8_t>(payload) & 0b01111111;
        HILOGI("volume_: %{public}x", volume_);

        if (parameterLength_ == AVRC_TG_VENDOR_PARAMETER_LENGTH || volume_ < AVRC_ABSOLUTE_VOLUME_PERCENTAGE_0 ||
            volume_ > AVRC_ABSOLUTE_VOLUME_PERCENTAGE_100) {
            crCode_ = AVRC_TG_RSP_CODE_REJECTED;
            status_ = AVRC_ES_CODE_INVALID_PARAMETER;
            AssembleRejectPacket();
            break;
        }

        isValid_ = true;

        HILOGI("isValid_: %{public}d", isValid_);
    } while (false);

    return isValid_;
}

uint16_t AvrcTgSavPacket::GetNumberOfPackets(void)
{
    HILOGI("enter");

    return AVRC_TG_SAV_NUM_OF_PACKETS;
}
}  // namespace bluetooth
}  // namespace OHOS
