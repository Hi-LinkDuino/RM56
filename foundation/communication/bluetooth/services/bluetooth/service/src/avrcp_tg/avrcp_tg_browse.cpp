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
#include "avrcp_tg_browse.h"

namespace OHOS {
namespace bluetooth {
/******************************************************************
 * AvrcCtBrowsePacket                                             *
 ******************************************************************/

AvrcTgBrowsePacket::AvrcTgBrowsePacket() : status_(AVRC_ES_CODE_NO_ERROR), label_(AVRC_INVALID_LABEL)
{
    HILOGI("enter");
}

AvrcTgBrowsePacket::AvrcTgBrowsePacket(uint8_t pduId, uint8_t status, uint8_t label) : status_(status), label_(label)
{
    HILOGI("pduId: %{public}d, status: %{public}d, label: %{public}d", pduId, status, label);

    pduId_ = pduId;
    status_ = status;
    label_ = label;
}

AvrcTgBrowsePacket::~AvrcTgBrowsePacket()
{
    HILOGI("enter");

    if (pkt_ != nullptr) {
        PacketFree(pkt_);
        pkt_ = nullptr;
    }
}

const Packet *AvrcTgBrowsePacket::AssemblePacket(void)
{
    HILOGI("enter");

    pkt_ = PacketMalloc(0x00, 0x00, AVRC_TG_BROWSE_MIN_SIZE + AVRC_TG_BROWSE_STATUS_SIZE);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x0000;
    offset += PushOctets1((buffer + offset), pduId_);
    HILOGI("pduId_: %{public}x", pduId_);

    parameterLength_ = AVRC_TG_BROWSE_STATUS_SIZE;
    offset += PushOctets2((buffer + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    PushOctets1((buffer + offset), status_);
    HILOGI("status_: %{public}x", status_);

    return pkt_;
}

bool AvrcTgBrowsePacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    pkt_ = pkt;

    return false;
}

bool AvrcTgBrowsePacket::IsValidParameterLength(Packet *pkt)
{
    HILOGI("enter");

    bool result = false;

    size_t size = PacketPayloadSize(pkt);
    if (size >= AVRC_TG_BROWSE_MIN_SIZE) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

        uint16_t offset = AVRC_TG_BROWSE_PARAMETER_LENGTH_OFFSET;
        uint64_t payload = 0x00;
        PopOctets2((buffer + offset), payload);
        parameterLength_ = static_cast<uint32_t>(payload);
        HILOGI("parameterLength_: %{public}u", parameterLength_);

        if (size - AVRC_TG_BROWSE_MIN_SIZE == parameterLength_) {
            result = true;
        }
    } else {
        HILOGI("The size of the packet is invalid! actual size: %{public}zu, valid size: %{public}u",
            size, AVRC_TG_BROWSE_MIN_SIZE);
    }

    return result;
}

/******************************************************************
 * SetBrowsedPlayer                                               *
 ******************************************************************/

AvrcTgSbpPacket::AvrcTgSbpPacket(uint16_t mtu, uint8_t status, uint16_t uidCounter, uint32_t numOfItems,
    const std::vector<std::string> &folderNames, uint8_t label)
    : AvrcTgBrowsePacket()
{
    HILOGI("enter");

    pduId_ = AVRC_TG_PDU_ID_SET_BROWSED_PLAYER;
    parameterLength_ = AVRC_TG_SBP_MIN_RSP_FRAME_SIZE;
    mtu_ = mtu;
    HILOGI("mtu_: %{public}u", mtu_);
    status_ = status;
    uidCounter_ = uidCounter;
    numOfItems_ = numOfItems;
    folderNames_ = folderNames;
    label_ = label;
}

AvrcTgSbpPacket::AvrcTgSbpPacket(Packet *pkt, uint8_t label) : AvrcTgBrowsePacket()
{
    HILOGI("label: %{public}d", label);

    pduId_ = AVRC_TG_PDU_ID_SET_BROWSED_PLAYER;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgSbpPacket::~AvrcTgSbpPacket()
{
    HILOGI("enter");
}

const Packet *AvrcTgSbpPacket::AssemblePacket(void)
{
    HILOGI("enter");

    uint16_t folderNamesSize = 0x0000;
    for (std::string folderName : folderNames_) {
        folderNamesSize += folderName.size();
    }
    uint16_t folderNameLength = folderDepth_ * AVRC_TG_SBP_FOLDER_NAME_LENGTH_SIZE;
    uint16_t frameSize = AVRC_TG_SBP_MIN_RSP_FRAME_SIZE + folderNamesSize + folderNameLength;

    (frameSize > mtu_) ? (frameSize = mtu_) : (frameSize);
    pkt_ = PacketMalloc(0x00, 0x00, frameSize);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x0000;
    offset += PushOctets1((buffer + offset), pduId_);
    HILOGI("pduId_: %{public}x", pduId_);

    parameterLength_ = frameSize - (AVRC_TG_BROWSE_PDU_ID_SIZE + AVRC_TG_BROWSE_PARAMETER_LENGTH_SIZE);
    offset += PushOctets2((buffer + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}u", parameterLength_);

    offset += PushOctets1((buffer + offset), status_);
    HILOGI("status_: %{public}x", status_);

    offset += PushOctets2((buffer + offset), uidCounter_);
    HILOGI("uidCounter_: %{public}x", uidCounter_);

    offset += PushOctets4((buffer + offset), numOfItems_);
    HILOGI("numOfItems_: %{public}d", numOfItems_);

    offset += PushOctets2((buffer + offset), AVRC_MEDIA_CHARACTER_SET_UTF8);
    HILOGI("characterSetId_: %{public}x", AVRC_MEDIA_CHARACTER_SET_UTF8);

    folderDepth_ = folderNames_.size();
    offset += PushOctets1((buffer + offset), folderDepth_);
    HILOGI("folderDepth_: %{public}d", folderDepth_);

    for (std::string folderName : folderNames_) {
        offset += PushOctets2((buffer + offset), folderName.size());
        HILOGI("folderNameLength_: %{public}zu", folderName.size());

        for (auto it = folderName.begin(); it != folderName.end(); it++) {
            offset += PushOctets1((buffer + offset), *it);
        }
        HILOGI("folderName_: %{public}s", folderName.c_str());
    }

    return pkt_;
}

bool AvrcTgSbpPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    isValid_ = IsValidParameterLength(pkt);
    if (isValid_) {
        size_t size = PacketPayloadSize(pkt);
        if (size >= AVRC_TG_SBP_FIXED_CMD_FRAME_SIZE) {
            auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

            do {
                uint16_t offset = 0x0000;
                uint64_t payload = 0x00;
                offset += PopOctets1((buffer + offset), payload);
                pduId_ = static_cast<uint8_t>(payload);
                HILOGI("pduId_: %{public}x", pduId_);

                offset += PopOctets2((buffer + offset), payload);
                parameterLength_ = static_cast<uint16_t>(payload);
                HILOGI("parameterLength_: %{public}u", parameterLength_);

                PopOctets2((buffer + offset), payload);
                playerId_ = static_cast<uint16_t>(payload);
                HILOGI("playerId_: %{public}x", playerId_);

                isValid_ = true;
            } while (false);
        }
    }

    return isValid_;
}

/******************************************************************
 * ChangePath                                                     *
 ******************************************************************/

AvrcTgCpPacket::AvrcTgCpPacket(uint8_t status, uint32_t numOfItems, uint8_t label)
    : AvrcTgBrowsePacket(), direction_(AVRC_FOLDER_DIRECTION_INVALID)
{
    HILOGI("status: %{public}d, numOfItems: %{public}u, label: %{public}d", status, numOfItems, label);

    pduId_ = AVRC_TG_PDU_ID_CHANGE_PATH;
    parameterLength_ = AVRC_TG_CP_FIXED_RSP_PARAMETER_LENGTH;
    status_ = status;
    label_ = label;
    numOfItems_ = numOfItems;
}

AvrcTgCpPacket::AvrcTgCpPacket(Packet *pkt, uint32_t uidCounter, uint8_t label)
    : AvrcTgBrowsePacket(), direction_(AVRC_FOLDER_DIRECTION_INVALID)
{
    HILOGI("uidCounter: %{public}u, label: %{public}d", uidCounter, label);

    pduId_ = AVRC_TG_PDU_ID_CHANGE_PATH;
    label_ = label;
    uidCounter_ = uidCounter;

    DisassemblePacket(pkt);
}

AvrcTgCpPacket::~AvrcTgCpPacket()
{
    HILOGI("enter");
}

const Packet *AvrcTgCpPacket::AssemblePacket(void)
{
    HILOGI("enter");

    pkt_ = PacketMalloc(0x00, 0x00, AVRC_TG_CP_FIXED_RSP_FRAME_SIZE);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x0000;
    offset += PushOctets1((buffer + offset), pduId_);
    HILOGI("pduId_: %{public}x", pduId_);

    offset += PushOctets2((buffer + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}u", parameterLength_);

    offset += PushOctets1((buffer + offset), status_);
    HILOGI("status_: %{public}x", status_);

    PushOctets4((buffer + offset), numOfItems_);
    HILOGI("numOfItems_: %{public}u", numOfItems_);

    return pkt_;
}

bool AvrcTgCpPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    isValid_ = false;
    size_t size = PacketPayloadSize(pkt);
    if (size >= AVRC_TG_CP_FIXED_CMD_FRAME_SIZE) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

        do {
            uint16_t offset = 0x0000;
            uint64_t payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            pduId_ = static_cast<uint8_t>(payload);
            HILOGI("pduId_: %{public}x", pduId_);

            offset += PopOctets2((buffer + offset), payload);
            parameterLength_ = static_cast<uint16_t>(payload);
            HILOGI("parameterLength_: %{public}u", parameterLength_);

            offset += PopOctets2((buffer + offset), payload);
            uint32_t uidCounter = static_cast<uint16_t>(payload);
            HILOGI("uidCounter: %{public}u", uidCounter);
            HILOGI("uidCounter_: %{public}u", uidCounter_);
            if (uidCounter != uidCounter_) {
                status_ = AVRC_ES_CODE_UID_CHANGED;
                break;
            }

            offset += PopOctets1((buffer + offset), payload);
            direction_ = static_cast<uint8_t>(payload);
            HILOGI("direction_: %{public}x", direction_);
            if (direction_ < AVRC_FOLDER_DIRECTION_UP || direction_ > AVRC_FOLDER_DIRECTION_DOWN) {
                status_ = AVRC_ES_CODE_INVALID_DIRECTION;
                break;
            }

            PopOctets8((buffer + offset), folderUid_);
            HILOGI("folderUid_: %{public}jx", folderUid_);

            isValid_ = true;
        } while (false);
    } else {
        HILOGI("The size of the packet is invalid! actual size: %{public}zu valid size: %{public}d",
            size, AVRC_TG_CP_FIXED_CMD_FRAME_SIZE);
    }

    return isValid_;
}

/******************************************************************
 * GetFolderItems                                                 *
 ******************************************************************/

AvrcTgGfiPacket::AvrcTgGfiPacket(
    uint16_t mtu, uint8_t status, uint32_t uidCounter, const std::vector<AvrcMpItem> &items, uint8_t label)
    : AvrcTgBrowsePacket()
{
    HILOGI("mtu: %{public}d, status: %{public}d, uidCounter: %{public}u, label: %{public}d",
        mtu, status, uidCounter, label);

    pduId_ = AVRC_TG_PDU_ID_GET_FOLDER_ITEMS;
    mtu_ = mtu;
    HILOGI("mtu_: %{public}d", mtu_);
    status_ = status;
    label_ = label;
    scope_ = AVRC_MEDIA_SCOPE_PLAYER_LIST;
    uidCounter_ = uidCounter;
    mpItems_ = items;
}

AvrcTgGfiPacket::AvrcTgGfiPacket(
    uint16_t mtu, uint8_t status, uint32_t uidCounter, const std::vector<AvrcMeItem> &items, uint8_t label)
    : AvrcTgBrowsePacket()
{
    HILOGI("mtu: %{public}d, status: %{public}d, uidCounter: %{public}u, label: %{public}d",
        mtu, status, uidCounter, label);

    pduId_ = AVRC_TG_PDU_ID_GET_FOLDER_ITEMS;
    mtu_ = mtu;
    HILOGI("mtu_: %{public}d", mtu_);
    status_ = status;
    label_ = label;
    scope_ = AVRC_MEDIA_SCOPE_NOW_PLAYING;
    uidCounter_ = uidCounter;
    meItems_ = items;
}

AvrcTgGfiPacket::AvrcTgGfiPacket(Packet *pkt, uint8_t label) : AvrcTgBrowsePacket(), scope_(AVRC_MEDIA_SCOPE_INVALID)
{
    HILOGI("label: %{public}d", label);

    pduId_ = AVRC_TG_PDU_ID_GET_FOLDER_ITEMS;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgGfiPacket::~AvrcTgGfiPacket()
{
    HILOGI("enter");

    attributes_.clear();
    mpItems_.clear();
    meItems_.clear();
}

const Packet *AvrcTgGfiPacket::AssemblePacket(void)
{
    HILOGI("enter");

    if (scope_ == AVRC_MEDIA_SCOPE_PLAYER_LIST) {
        pkt_ = AssembleMpPacket();
    } else {
        pkt_ = AssembleMePacket();
    }

    return pkt_;
}

uint16_t AvrcTgGfiPacket::CountMpFrameSize(void)
{
    HILOGI("enter");

    uint16_t frameSize = AVRC_TG_GFI_MIN_RSP_FRAME_SIZE + AVRC_TG_GFI_FIXED_RSP_PARAMETER_LENGTH;
    uint16_t tempSize = frameSize;

    uint16_t itemLength = AVRC_TG_GFI_PLAYER_ID_SIZE;
    itemLength += AVRC_TG_GFI_MAJOR_PLAYER_TYPE_SIZE;
    itemLength += AVRC_TG_GFI_PLAYER_SUB_TYPE_SIZE;
    itemLength += AVRC_TG_GFI_PLAY_STATUS_SIZE;
    itemLength += AVRC_TG_GFI_FEATURE_BIT_MASK;
    itemLength += AVRC_TG_GFI_CHARACTER_SET_ID_SIZE;
    itemLength += AVRC_TG_GFI_NAME_LENGTH_SIZE;

    numOfItems_ = AVRC_TG_GFI_NUMBER_OF_ITEMS;
    for (AvrcMpItem item : mpItems_) {
        tempSize += AVRC_TG_GFI_ITEM_TYPE_SIZE;
        tempSize += AVRC_TG_GFI_ITEM_LENGTH_SIZE;
        tempSize += itemLength;
        tempSize += item.name_.length();
        HILOGI("tempSize: %{public}u", tempSize);

        if (tempSize <= mtu_) {
            frameSize = tempSize;
            ++numOfItems_;
        } else {
            break;
        }
    }

    return frameSize;
}

Packet *AvrcTgGfiPacket::AssembleMpPacket(void)
{
    HILOGI("enter");

    uint16_t frameSize = CountMpFrameSize();

    pkt_ = PacketMalloc(0x00, 0x00, frameSize);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x0000;
    offset += PushOctets1((buffer + offset), pduId_);
    HILOGI("pduId_: %{public}x", pduId_);

    parameterLength_ = frameSize - AVRC_TG_GIA_MIN_RSP_FRAME_SIZE;
    offset += PushOctets2((buffer + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    if (numOfItems_ == AVRC_TG_GFI_NUMBER_OF_ITEMS) {
        status_ = AVRC_ES_CODE_RANGE_OUT_OF_BOUNDS;
    }
    offset += PushOctets1((buffer + offset), status_);
    HILOGI("status_: %{public}x", status_);

    offset += PushOctets2((buffer + offset), uidCounter_);
    HILOGI("uidCounter_: %{public}d", uidCounter_);

    offset += PushOctets2((buffer + offset), numOfItems_);
    HILOGI("numOfItems_: %{public}d", numOfItems_);

    AssembleMpItem(buffer, offset);

    return pkt_;
}

void AvrcTgGfiPacket::AssembleMpItem(uint8_t *buffer, uint16_t offset)
{
    HILOGI("offset: %{public}d", offset);

    uint16_t itemLength = AVRC_TG_GFI_PLAYER_ID_SIZE + AVRC_TG_GFI_MAJOR_PLAYER_TYPE_SIZE +
                          AVRC_TG_GFI_PLAYER_SUB_TYPE_SIZE + AVRC_TG_GFI_PLAY_STATUS_SIZE +
                          AVRC_TG_GFI_FEATURE_BIT_MASK + AVRC_TG_GFI_CHARACTER_SET_ID_SIZE +
                          AVRC_TG_GFI_NAME_LENGTH_SIZE;
    uint16_t counter = numOfItems_;
    uint16_t newOffset = offset;

    for (AvrcMpItem item : mpItems_) {
        if (counter-- <= 0) {
            break;
        }
        newOffset += PushOctets1((buffer + newOffset), item.itemType_);
        HILOGI("itemType_: %{public}x", item.itemType_);

        newOffset += PushOctets2((buffer + newOffset), itemLength + item.name_.size());
        HILOGI("itemLength: %{public}zu", itemLength + item.name_.size());

        newOffset += PushOctets2((buffer + newOffset), item.playerId_);
        HILOGI("playerId_: %{public}x", item.playerId_);

        newOffset += PushOctets1((buffer + newOffset), item.majorType_);
        HILOGI("majorType_: %{public}x", item.majorType_);

        newOffset += PushOctets4((buffer + newOffset), item.subType_);
        HILOGI("subType_: %{public}x", item.subType_);

        newOffset += PushOctets1((buffer + newOffset), item.playStatus_);
        HILOGI("playStatus_: %{public}x", item.playStatus_);

        for (int i = 0; i < AVRC_TG_GFI_VALID_FEATURE_OCTETS; i++) {
            if (i < static_cast<int>(item.features_.size())) {
                newOffset += PushOctets1((buffer + newOffset), item.features_.at(i));
                HILOGI("feature: %{public}x", item.features_.at(i));
            } else {
                newOffset += PushOctets1((buffer + newOffset), 0x00);
            }
        }

        newOffset += PushOctets2((buffer + newOffset), AVRC_MEDIA_CHARACTER_SET_UTF8);

        newOffset += PushOctets2((buffer + newOffset), item.name_.size());
        HILOGI("nameLength: %{public}zu", item.name_.size());

        for (auto it = item.name_.begin(); it != item.name_.end(); it++) {
            newOffset += PushOctets1((buffer + newOffset), *it);
        }
        HILOGI("value: %{public}s", item.name_.c_str());
    }
}

uint16_t AvrcTgGfiPacket::CountMeFrameSize(void)
{
    HILOGI("enter");

    uint16_t frameSize = AVRC_TG_GFI_MIN_RSP_FRAME_SIZE + AVRC_TG_GFI_FIXED_RSP_PARAMETER_LENGTH;
    uint16_t tempSize = frameSize;

    numOfItems_ = 0;
    for (AvrcMeItem item : meItems_) {
        tempSize += AVRC_TG_GFI_ITEM_TYPE_SIZE;
        tempSize += AVRC_TG_GFI_ITEM_LENGTH_SIZE;
        tempSize += AVRC_TG_GFI_UID_SIZE;
        tempSize += AVRC_TG_GFI_TYPE_SIZE;
        if (item.itemType_ == AVRC_MEDIA_TYPE_FOLDER_ITEM) {
            tempSize += AVRC_TG_GFI_IS_PLAYABLE_SIZE;
        }
        tempSize += AVRC_TG_GFI_CHARACTER_SET_ID_SIZE;
        tempSize += AVRC_TG_GFI_NAME_LENGTH_SIZE;
        tempSize += item.name_.length();
        if (item.itemType_ == AVRC_MEDIA_TYPE_MEDIA_ELEMENT_ITEM) {
            tempSize += AVRC_TG_GFI_NUMBER_OF_ATTRIBUTES_SIZE;
            for (size_t i = 0; i < item.attributes_.size(); i++) {
                tempSize += AVRC_TG_GFI_ATTRIBUTE_ID_SIZE;
                tempSize += AVRC_TG_GFI_CHARACTER_SET_ID_SIZE;
                tempSize += AVRC_TG_GFI_ATTRIBUTE_VALUE_LENGTH_SIZE;
                tempSize += item.values_.at(i).length();
            }
        }
        HILOGI("tempSize: %{public}d", tempSize);

        if (tempSize <= mtu_) {
            frameSize = tempSize;
            ++numOfItems_;
        } else {
            break;
        }
    }

    return frameSize;
}

Packet *AvrcTgGfiPacket::AssembleMePacket(void)
{
    HILOGI("enter");

    uint16_t frameSize = CountMeFrameSize();

    pkt_ = PacketMalloc(0x00, 0x00, frameSize);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x0000;
    offset += PushOctets1((buffer + offset), pduId_);
    HILOGI("pduId_: %{public}x", pduId_);

    parameterLength_ = frameSize - AVRC_TG_GIA_MIN_RSP_FRAME_SIZE;
    offset += PushOctets2((buffer + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    if (numOfItems_ == AVRC_TG_GFI_NUMBER_OF_ITEMS) {
        status_ = AVRC_ES_CODE_RANGE_OUT_OF_BOUNDS;
    }
    offset += PushOctets1((buffer + offset), status_);
    HILOGI("status_: %{public}x", status_);

    offset += PushOctets2((buffer + offset), uidCounter_);
    HILOGI("uidCounter_: %{public}d", uidCounter_);

    offset += PushOctets2((buffer + offset), numOfItems_);
    HILOGI("numOfItems_: %{public}d", numOfItems_);

    AssembleMeItem(buffer, offset);

    return pkt_;
}

void AvrcTgGfiPacket::AssembleMeItem(uint8_t *buffer, uint16_t offset)
{
    HILOGI("offset: %{public}d", offset);

    uint16_t itemLength =
        AVRC_TG_GFI_UID_SIZE + AVRC_TG_GFI_TYPE_SIZE + AVRC_TG_GFI_CHARACTER_SET_ID_SIZE + AVRC_TG_GFI_NAME_LENGTH_SIZE;
    uint16_t counter = numOfItems_;
    uint16_t newOffset = offset;

    for (AvrcMeItem item : meItems_) {
        if (counter-- <= 0) {
            break;
        }
        newOffset += PushOctets1((buffer + newOffset), item.itemType_);

        if (item.itemType_ == AVRC_MEDIA_TYPE_FOLDER_ITEM) {
            itemLength += AVRC_TG_GFI_IS_PLAYABLE_SIZE;
        }
        if (item.itemType_ == AVRC_MEDIA_TYPE_MEDIA_ELEMENT_ITEM) {
            for (std::string value : item.values_) {
                itemLength += AVRC_TG_GFI_ATTRIBUTE_ID_SIZE + AVRC_TG_GFI_CHARACTER_SET_ID_SIZE +
                              AVRC_TG_GFI_ATTRIBUTE_VALUE_LENGTH_SIZE + value.length();
            }
        }
        newOffset += PushOctets2((buffer + newOffset), itemLength + item.name_.size());

        newOffset += PushOctets8((buffer + newOffset), item.uid_);

        newOffset += PushOctets1((buffer + newOffset), item.type_);

        if (item.itemType_ == AVRC_MEDIA_TYPE_FOLDER_ITEM) {
            newOffset += PushOctets1((buffer + newOffset), item.playable_);
        }

        newOffset += PushOctets2((buffer + newOffset), AVRC_MEDIA_CHARACTER_SET_UTF8);

        newOffset += PushOctets2((buffer + newOffset), item.name_.size());

        for (auto it = item.name_.begin(); it != item.name_.end(); it++) {
            newOffset += PushOctets1((buffer + newOffset), *it);
        }

        if (item.itemType_ != AVRC_MEDIA_TYPE_MEDIA_ELEMENT_ITEM) {
            continue;
        }

        newOffset += PushOctets1((buffer + newOffset), item.attributes_.size());

        for (size_t i = 0; i < item.attributes_.size(); i++) {
            newOffset += PushOctets4((buffer + newOffset), item.attributes_.at(i));

            newOffset += PushOctets2((buffer + newOffset), AVRC_MEDIA_CHARACTER_SET_UTF8);

            newOffset += PushOctets2((buffer + newOffset), item.values_.at(i).size());

            for (auto it = item.values_.at(i).begin(); it != item.values_.at(i).end(); it++) {
                newOffset += PushOctets1((buffer + newOffset), *it);
            }
        }
    }
}

bool AvrcTgGfiPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    isValid_ = false;
    size_t size = PacketPayloadSize(pkt);
    if (size >= AVRC_TG_GFI_MIN_CMD_FRAME_SIZE) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

        do {
            uint16_t offset = 0x0000;
            uint64_t payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            pduId_ = static_cast<uint8_t>(payload);
            HILOGI("pduId_: %{public}x", pduId_);

            offset += PopOctets2((buffer + offset), payload);
            parameterLength_ = static_cast<uint16_t>(payload);
            HILOGI("parameterLength_: %{public}d", parameterLength_);

            offset += PopOctets1((buffer + offset), payload);
            scope_ = static_cast<uint8_t>(payload);
            HILOGI("scope_: %{public}x", scope_);
            if (scope_ < AVRC_MEDIA_SCOPE_PLAYER_LIST || scope_ > AVRC_MEDIA_SCOPE_NOW_PLAYING) {
                status_ = AVRC_ES_CODE_INVALID_SCOPE;
                break;
            }

            offset += PopOctets4((buffer + offset), payload);
            startItem_ = static_cast<uint32_t>(payload);
            HILOGI("startItem_: %{public}x", startItem_);

            offset += PopOctets4((buffer + offset), payload);
            endItem_ = static_cast<uint32_t>(payload);
            HILOGI("endItem_: %{public}x", endItem_);
            if (startItem_ > endItem_) {
                status_ = AVRC_ES_CODE_RANGE_OUT_OF_BOUNDS;
                break;
            }

            offset += PopOctets1((buffer + offset), payload);
            attributeCount_ = static_cast<uint8_t>(payload);
            HILOGI("attributeCount_: %{public}x", attributeCount_);

            if (scope_ == AVRC_MEDIA_SCOPE_PLAYER_LIST) {
                isValid_ = true;
                break;
            }

            DisassemblePacketAttributes(pkt, offset);

            isValid_ = true;
        } while (false);
    } else {
        HILOGI("The size of the packet is invalid! actual size: %{public}zu valid size: %{public}d",
            size, AVRC_TG_GIA_MIN_CMD_FRAME_SIZE);
    }

    return isValid_;
}

void AvrcTgGfiPacket::DisassemblePacketAttributes(Packet *pkt, uint8_t offset)
{
    HILOGI("offset: %{public}x", offset);

    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));
    uint64_t payload = 0x00;

    if (attributeCount_ == AVRC_ATTRIBUTE_COUNT_ALL) {
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_TITLE);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_ARTIST_NAME);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_ALBUM_NAME);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_TRACK_NUMBER);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_TOTAL_NUMBER_OF_TRACKS);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_GENRE);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_PLAYING_TIME);
    } else if (attributeCount_ == AVRC_ATTRIBUTE_COUNT_NO) {
        /// Do nothing!
    } else {
        for (int i = 0; i < attributeCount_; i++) {
            offset += PopOctets4((buffer + offset), payload);
            attributes_.push_back(static_cast<uint32_t>(payload));
            HILOGI("attribute: %{public}x", attributes_.back());
        }
    }
}

/******************************************************************
 * GetItemAttributes                                              *
 ******************************************************************/

AvrcTgGiaPacket::AvrcTgGiaPacket(uint16_t mtu, uint8_t status, const std::vector<uint32_t> &attributes,
    const std::vector<std::string> &values, uint8_t label)
    : AvrcTgBrowsePacket()
{
    HILOGI("mtu: %{public}d, status: %{public}d, label: %{public}d", mtu, status, label);

    pduId_ = AVRC_TG_PDU_ID_GET_ITEM_ATTRIBUTES;
    mtu_ = mtu;
    HILOGI("mtu_: %{public}u", mtu_);
    status_ = status;
    label_ = label;
    attributes_ = attributes;
    values_ = values;
}

AvrcTgGiaPacket::AvrcTgGiaPacket(Packet *pkt, uint32_t uidCounter, uint8_t label)
    : AvrcTgBrowsePacket(), scope_(AVRC_MEDIA_SCOPE_INVALID)
{
    HILOGI("uidCounter: %{public}u, label: %{public}d", uidCounter, label);

    pduId_ = AVRC_TG_PDU_ID_GET_ITEM_ATTRIBUTES;
    label_ = label;
    uidCounter_ = uidCounter;

    DisassemblePacket(pkt);
}

AvrcTgGiaPacket::~AvrcTgGiaPacket()
{
    HILOGI("enter");

    attributes_.clear();
    values_.clear();
}

const Packet *AvrcTgGiaPacket::AssemblePacket(void)
{
    HILOGI("enter");

    uint16_t frameSize = AVRC_TG_GIA_MIN_RSP_FRAME_SIZE + AVRC_TG_GIA_FIXED_RSP_PARAMETER_LENGTH;
    uint16_t tempSize = frameSize;

    numOfAttributes_ = 0;
    for (size_t i = 0; i < attributes_.size(); i++) {
        tempSize += AVRC_TG_GIA_ATTRIBUTE_ID_SIZE;
        tempSize += AVRC_TG_GIA_CHARACTER_SET_ID_SIZE;
        tempSize += AVRC_TG_GIA_ATTRIBUTE_VALUE_LENGTH_SIZE;
        tempSize += values_.at(i).length();
        HILOGI("tempSize: %{public}d", tempSize);

        if (tempSize <= mtu_) {
            frameSize = tempSize;
            ++numOfAttributes_;
        } else {
            break;
        }
    }

    pkt_ = PacketMalloc(0x00, 0x00, frameSize);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x0000;
    offset += PushOctets1((buffer + offset), pduId_);
    HILOGI("pduId_: %{public}x", pduId_);

    parameterLength_ = frameSize - AVRC_TG_GIA_MIN_RSP_FRAME_SIZE;
    offset += PushOctets2((buffer + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}d", parameterLength_);

    offset += PushOctets1((buffer + offset), status_);
    HILOGI("status_: %{public}x", status_);

    offset += PushOctets1((buffer + offset), numOfAttributes_);
    HILOGI("numOfAttributes_: %{public}d", numOfAttributes_);

    for (int i = 0; i < numOfAttributes_; i++) {
        offset += PushOctets4((buffer + offset), attributes_.at(i));
        HILOGI("attribute: %{public}x", attributes_.at(i));

        offset += PushOctets2((buffer + offset), AVRC_MEDIA_CHARACTER_SET_UTF8);
        HILOGI("characterSetId: %{public}x", AVRC_MEDIA_CHARACTER_SET_UTF8);

        offset += PushOctets2((buffer + offset), values_.at(i).size());
        HILOGI("valueLength: %{public}zu", values_.at(i).size());

        for (auto it = values_.at(i).begin(); it != values_.at(i).end(); it++) {
            offset += PushOctets1((buffer + offset), *it);
        }
        HILOGI("value: %{public}s", values_.at(i).c_str());
    }

    HILOGI("offset: %{public}hu", offset);

    return pkt_;
}

bool AvrcTgGiaPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    isValid_ = false;
    size_t size = PacketPayloadSize(pkt);
    if (size >= AVRC_TG_GIA_MIN_CMD_FRAME_SIZE) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

        do {
            uint16_t offset = 0x0000;
            uint64_t payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            pduId_ = static_cast<uint8_t>(payload);
            HILOGI("pduId_: %{public}x", pduId_);

            offset += PopOctets2((buffer + offset), payload);
            parameterLength_ = static_cast<uint16_t>(payload);
            HILOGI("parameterLength_: %{public}d", parameterLength_);

            offset += PopOctets1((buffer + offset), payload);
            scope_ = static_cast<uint8_t>(payload);
            HILOGI("scope_: %{public}x", scope_);
            if ((scope_ < AVRC_MEDIA_SCOPE_PLAYER_LIST) || (scope_ > AVRC_MEDIA_SCOPE_NOW_PLAYING)) {
                status_ = AVRC_ES_CODE_INVALID_SCOPE;
                break;
            }

            offset += PopOctets8((buffer + offset), payload);
            uid_ = static_cast<uint64_t>(payload);
            HILOGI("uid_: %{public}jx", uid_);

            offset += PopOctets2((buffer + offset), payload);
            uint32_t uidCounter = static_cast<uint16_t>(payload);
            HILOGI("uidCounter: %{public}u", uidCounter);
            HILOGI("uidCounter_: %{public}u", uidCounter_);
            if (uidCounter != uidCounter_) {
                status_ = AVRC_ES_CODE_UID_CHANGED;
                break;
            }

            offset += PopOctets1((buffer + offset), payload);
            numOfAttributes_ = static_cast<uint8_t>(payload);
            HILOGI("numOfAttributes_: %{public}x", numOfAttributes_);

            DisassemblePacketAttributes(pkt, offset);

            isValid_ = true;
        } while (false);
    } else {
        HILOGI("The size of the packet is invalid! actual size: %{public}zu, valid size: %{public}d",
            size, AVRC_TG_GFI_MIN_CMD_FRAME_SIZE);
    }

    return isValid_;
}

void AvrcTgGiaPacket::DisassemblePacketAttributes(Packet *pkt, uint8_t offset)
{
    HILOGI("offset: %{public}d", offset);

    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

    uint64_t payload = 0x00;

    for (int i = 0; i < numOfAttributes_; i++) {
        offset += PopOctets4((buffer + offset), payload);
        attributes_.push_back(static_cast<uint32_t>(payload));
        HILOGI("attribute: %{public}x", attributes_.back());
    }

    if (numOfAttributes_ == AVRC_TG_GIA_NUMBER_OF_ATTRIBUTES) {
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_TITLE);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_ARTIST_NAME);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_ALBUM_NAME);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_TRACK_NUMBER);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_TOTAL_NUMBER_OF_TRACKS);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_GENRE);
        attributes_.push_back(AVRC_MEDIA_ATTRIBUTE_PLAYING_TIME);
    }
}

/******************************************************************
 * GetTotalNumberOfItems                                          *
 ******************************************************************/

AvrcTgGtnoiPacket::AvrcTgGtnoiPacket(uint8_t status, uint16_t uidCounter, uint32_t numOfItems, uint8_t label)
    : AvrcTgBrowsePacket()
{
    HILOGI("status: %{public}d, uidCounter: %{public}d, numOfItems: %{public}u, label: %{public}d",
        status, uidCounter, numOfItems, label);

    pduId_ = AVRC_TG_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS;
    parameterLength_ = AVRC_TG_GTNOI_FIXED_RSP_PARAMETER_LENGTH;
    status_ = status;
    label_ = label;
    uidCounter_ = uidCounter;
    numOfItems_ = numOfItems;
}

AvrcTgGtnoiPacket::AvrcTgGtnoiPacket(Packet *pkt, uint8_t label)
    : AvrcTgBrowsePacket(), scope_(AVRC_MEDIA_SCOPE_INVALID)
{
    HILOGI("label: %{public}d", label);

    pduId_ = AVRC_TG_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS;
    label_ = label;

    DisassemblePacket(pkt);
}

AvrcTgGtnoiPacket::~AvrcTgGtnoiPacket()
{
    HILOGI("enter");
}

const Packet *AvrcTgGtnoiPacket::AssemblePacket(void)
{
    HILOGI("enter");

    pkt_ = PacketMalloc(0x00, 0x00, AVRC_TG_GTNOI_FIXED_RSP_FRAME_SIZE);
    auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt_)));

    uint16_t offset = 0x0000;
    offset += PushOctets1((buffer + offset), pduId_);
    HILOGI("pduId_: %{public}x", pduId_);

    offset += PushOctets2((buffer + offset), parameterLength_);
    HILOGI("parameterLength_: %{public}u", parameterLength_);

    offset += PushOctets1((buffer + offset), status_);
    HILOGI("status_: %{public}x", status_);

    offset += PushOctets2((buffer + offset), uidCounter_);
    HILOGI("uidCounter_: %{public}x", uidCounter_);

    offset += PushOctets4((buffer + offset), numOfItems_);
    HILOGI("numOfItems_: %{public}u", numOfItems_);

    HILOGI("offset: %{public}u", offset);

    return pkt_;
}

bool AvrcTgGtnoiPacket::DisassemblePacket(Packet *pkt)
{
    HILOGI("enter");

    isValid_ = IsValidParameterLength(pkt);
    if (isValid_) {
        auto buffer = static_cast<uint8_t *>(BufferPtr(PacketContinuousPayload(pkt)));

        do {
            uint16_t offset = 0x0000;
            uint64_t payload = 0x00;
            offset += PopOctets1((buffer + offset), payload);
            pduId_ = static_cast<uint8_t>(payload);
            HILOGI("pduId_: %{public}x", pduId_);

            payload = 0x00;
            offset += PopOctets2((buffer + offset), payload);
            parameterLength_ = static_cast<uint16_t>(payload);
            HILOGI("parameterLength_: %{public}d", parameterLength_);

            payload = 0x00;
            PopOctets1((buffer + offset), payload);
            scope_ = static_cast<uint8_t>(payload);
            HILOGI("scope_: %{public}x", scope_);
            if (scope_ < AVRC_MEDIA_SCOPE_PLAYER_LIST || scope_ > AVRC_MEDIA_SCOPE_NOW_PLAYING) {
                status_ = AVRC_ES_CODE_INVALID_SCOPE;
                break;
            }

            isValid_ = true;
        } while (false);
    } else {
        status_ = AVRC_ES_CODE_INVALID_PARAMETER;
    }

    return isValid_;
}
}  // namespace bluetooth
}  // namespace OHOS