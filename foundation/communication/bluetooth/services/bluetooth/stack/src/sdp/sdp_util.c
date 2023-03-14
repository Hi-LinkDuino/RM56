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

#include "sdp_util.h"

#include <stdio.h>
#include <string.h>

#include "bt_endian.h"
#include "log.h"

static bool g_sdpEnableState = true;

void SdpSetEnableState()
{
    g_sdpEnableState = true;
}

void SdpSetDisableState()
{
    g_sdpEnableState = false;
}

bool SdpGetEnableState()
{
    return g_sdpEnableState;
}

void SdpReverseForBigEndian(const uint8_t *src, uint8_t *dst, int length)
{
    for (int i = 0; i < length; i++) {
        dst[length - 1 - i] = src[i];
    }
}

uint16_t SdpAddAttributeForUuid(uint8_t *buffer, uint16_t offset, const BtUuid *uuid)
{
    switch (uuid->type) {
        case BT_UUID_16:
            /// Data Element: UUID 2 bytes (0x19)
            buffer[offset] = (DE_TYPE_UUID << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_16;
            offset++;
            /// UUID
            *(uint16_t *)(buffer + offset) = H2BE_16(uuid->uuid16);
            offset += SDP_UUID16_LENGTH;
            break;
        case BT_UUID_32:
            /// Data Element: UUID 4 bytes (0x1A)
            buffer[offset] = (DE_TYPE_UUID << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_32;
            offset++;
            /// UUID
            *(uint32_t *)(buffer + offset) = H2BE_32(uuid->uuid32);
            offset += SDP_UUID32_LENGTH;
            break;
        case BT_UUID_128:
            /// Data Element: UUID 16 bytes (0x1C)
            buffer[offset] = (DE_TYPE_UUID << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_128;
            offset++;
            /// UUID
            SdpReverseForBigEndian(uuid->uuid128, buffer + offset, SDP_UUID128_LENGTH);
            offset += SDP_UUID128_LENGTH;
            break;
        default:
            LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, uuid->type);
            break;
    }

    return offset;
}

/**
 * @brief Get length from type
 * @details
 * @param
 * @return
 */
uint16_t SdpGetLengthFromType(const uint8_t *buffer, uint8_t type, uint32_t *length)
{
    uint16_t offset = 0;
    SdpDescriptorSize size = type & 0x07;

    switch (size) {
        case DE_SIZE_8:
            offset = SDP_UINT8_LENGTH;
            *length = SDP_UINT8_LENGTH;
            break;
        case DE_SIZE_16:
            offset = SDP_UINT16_LENGTH;
            *length = SDP_UINT16_LENGTH;
            break;
        case DE_SIZE_32:
            offset = SDP_UINT32_LENGTH;
            *length = SDP_UINT32_LENGTH;
            break;
        case DE_SIZE_64:
            offset = SDP_UINT64_LENGTH;
            *length = SDP_UINT64_LENGTH;
            break;
        case DE_SIZE_128:
            offset = SDP_UINT128_LENGTH;
            *length = SDP_UINT128_LENGTH;
            break;
        case DE_SIZE_VAR_8:
            offset = 1;
            *length = buffer[0];
            break;
        case DE_SIZE_VAR_16:
            offset = SDP_UINT16_LENGTH;
            *length = BE2H_16(*(uint16_t *)buffer);
            break;
        case DE_SIZE_VAR_32:
            offset = SDP_UINT32_LENGTH;
            *length = BE2H_32(*(uint32_t *)buffer);
            break;
        default:
            LOG_ERROR("[%{public}s][%{public}d] Wrong size [%{public}d]", __FUNCTION__, __LINE__, size);
            break;
    }

    return offset;
}

uint16_t SdpGetUuid(uint8_t *buffer, BtUuid *uuid)
{
    uint32_t length = 0;
    uint16_t offset = 0;
    uint8_t type = buffer[0];

    offset++;
    SdpGetLengthFromType(buffer + offset, type, &length);
    if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UUID) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
        return offset;
    }
    switch (length) {
        case SDP_UUID16_LENGTH:
            uuid->type = BT_UUID_16;
            uuid->uuid16 = BE2H_16(*(uint16_t *)(buffer + offset));
            break;
        case SDP_UUID32_LENGTH:
            uuid->type = BT_UUID_32;
            uuid->uuid32 = BE2H_32(*(uint32_t *)(buffer + offset));
            break;
        case SDP_UUID128_LENGTH:
            uuid->type = BT_UUID_128;
            SdpReverseForBigEndian(buffer + offset, uuid->uuid128, SDP_UUID128_LENGTH);
            break;
        default:
            LOG_ERROR("[%{public}s][%u] Wrong length [0x%02x]", __FUNCTION__, __LINE__, length);
            break;
    }
    offset += length;

    return offset;
}
