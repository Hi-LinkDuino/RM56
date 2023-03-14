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
#include "dhcp_options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "securec.h"

#undef LOG_TAG
#define LOG_TAG "WifiDhcpOptions"

/* Check packet option OPTION_OVERLOAD_OPTION. */
static bool CheckOptSoverloaded(const struct DhcpPacket *packet, int code, int maxLen, int *over, int *index)
{
    if (packet == NULL) {
        LOGE("CheckOptSoverloaded failed, packet == NULL!");
        return false;
    }

    const uint8_t *pOption = packet->options;
    if (*index + DHCP_OPT_LEN_INDEX + pOption[*index + DHCP_OPT_LEN_INDEX] >= maxLen) {
        LOGW("CheckOptSoverloaded code:%{public}d,*index:%{public}d more than max bytes:%{public}d!",
            code, *index, maxLen);
        return false;
    }
    *over = pOption[*index + DHCP_OPT_DATA_INDEX + DHCP_OPT_CODE_INDEX];
    *index += pOption[DHCP_OPT_LEN_INDEX] + DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES;
    return true;
}

/* Check packet options based on the code and index. */
static int CheckOptionsData(const struct DhcpPacket *packet, int code, int index, int maxLen)
{
    if (packet == NULL) {
        LOGE("CheckOptionsData failed, packet == NULL!");
        return DHCP_OPT_FAILED;
    }

    if (index >= maxLen) {
        LOGW("CheckOptionsData code:%{public}d,index:%{public}d more than max bytes:%{public}d!",
            code, index, maxLen);
        return DHCP_OPT_FAILED;
    }

    const uint8_t *pOption = packet->options;
    if (pOption[index + DHCP_OPT_CODE_INDEX] != code) {
        return DHCP_OPT_NONE;
    }

    if (index + DHCP_OPT_LEN_INDEX + pOption[index + DHCP_OPT_LEN_INDEX] >= maxLen) {
        LOGW("CheckOptionsData failed, options data too long, code:%{public}d,index:%{public}d!", code, index);
        return DHCP_OPT_FAILED;
    }

    return DHCP_OPT_SUCCESS;
}

/* Obtains the data type based on the code. */
static uint8_t GetDhcpOptionCodeType(const uint8_t code)
{
    if ((code <= PAD_OPTION) || (code >= END_OPTION)) {
        LOGE("GetDhcpOptionCodeType error, code:%{public}d is error!", code);
        return DHCP_OPTION_DATA_INVALID;
    }

    uint8_t nDataType = DHCP_OPTION_DATA_INVALID;
    switch (code) {
        case DHCP_MESSAGE_TYPE_OPTION:
        case FORCERENEW_NONCE_OPTION:
            nDataType = DHCP_OPTION_DATA_U8;
            break;
        case INTERFACE_MTU_OPTION:
        case MAXIMUM_DHCP_MESSAGE_SIZE_OPTION:
            nDataType = DHCP_OPTION_DATA_U16;
            break;
        case IP_ADDRESS_LEASE_TIME_OPTION:
            nDataType = DHCP_OPTION_DATA_U32;
            break;
        case SUBNET_MASK_OPTION:
        case BROADCAST_ADDRESS_OPTION:
        case REQUESTED_IP_ADDRESS_OPTION:
        case SERVER_IDENTIFIER_OPTION:
            nDataType = DHCP_OPTION_DATA_IP;
            break;
        case ROUTER_OPTION:
        case DOMAIN_NAME_SERVER_OPTION:
        case NETWORK_TIME_PROTOCOL_SERVERS_OPTION:
            nDataType = DHCP_OPTION_DATA_IP_LIST;
            break;
        case HOST_NAME_OPTION:
        case DOMAIN_NAME_OPTION:
        case MESSAGE_OPTION:
            nDataType = DHCP_OPTION_DATA_IP_STRING;
            break;
        default:
            LOGE("GetDhcpOptionCodeType failed, code:%{public}d is invalid!", code);
            break;
    }

    return nDataType;
}

/* Obtains the data length based on the code. */
uint8_t GetDhcpOptionDataLen(const uint8_t code)
{
    uint8_t nDataType = GetDhcpOptionCodeType(code);
    if (nDataType == DHCP_OPTION_DATA_INVALID) {
        LOGE("GetDhcpOptionDataLen code:%{public}d error, GetDhcpOptionCodeType invalid!", code);
        return 0;
    }

    uint8_t nDataLen = 0;
    switch (nDataType) {
        case DHCP_OPTION_DATA_U8:
            nDataLen = DHCP_UINT8_BYTES;
            break;
        case DHCP_OPTION_DATA_U16:
            nDataLen = DHCP_UINT16_BYTES;
            break;
        case DHCP_OPTION_DATA_U32:
            nDataLen = DHCP_UINT32_BYTES;
            break;
        case DHCP_OPTION_DATA_IP:
            nDataLen = DHCP_UINT32_BYTES;
            break;
        case DHCP_OPTION_DATA_IP_PAIR:
            nDataLen = DHCP_UINT32_DOUBLE_BYTES;
            break;
        default:
            LOGE("GetDhcpOptionDataLen code:%{public}d failed, nDataType:%{public}d is invalid!",
                code, nDataType);
            break;
    }

    return nDataLen;
}

/* Obtains the data pointer and length from the packet based on the code. */
const uint8_t *GetDhcpOption(const struct DhcpPacket *packet, int code, size_t *length)
{
    *length = 0;
    if (packet == NULL) {
        LOGE("GetDhcpOption failed, packet == NULL!");
        return NULL;
    }

    const uint8_t *pOption = packet->options;
    int nIndex = 0, maxLen = DHCP_OPTIONS_SIZE, nOver = 0, nFinished = 0, nFlag = OPTION_FIELD;
    while (nFinished == 0) {
        int nRet = CheckOptionsData(packet, code, nIndex, maxLen);
        if (nRet == DHCP_OPT_SUCCESS) {
            *length = pOption[nIndex + DHCP_OPT_LEN_INDEX];
            return pOption + nIndex + DHCP_OPT_DATA_INDEX;
        } else if (nRet == DHCP_OPT_FAILED) {
            return NULL;
        }

        switch (pOption[nIndex + DHCP_OPT_CODE_INDEX]) {
            case PAD_OPTION:
                nIndex++;
                break;
            case OPTION_OVERLOAD_OPTION:
                if (!CheckOptSoverloaded(packet, code, maxLen, &nOver, &nIndex)) {
                    return NULL;
                }
                break;
            case END_OPTION:
                if ((nFlag == OPTION_FIELD) && (nOver & FILE_FIELD)) {
                    pOption = packet->file;
                    nIndex = 0;
                    maxLen = DHCP_BOOT_FILE_LENGTH;
                    nFlag = FILE_FIELD;
                } else if ((nFlag == FILE_FIELD) && (nOver & SNAME_FIELD)) {
                    pOption = packet->sname;
                    nIndex = 0;
                    maxLen = DHCP_HOST_NAME_LENGTH;
                    nFlag = SNAME_FIELD;
                } else {
                    nFinished = 1;
                }
                break;
            default:
                nIndex += DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOption[nIndex + DHCP_OPT_LEN_INDEX];
                break;
        }
    }
    LOGW("GetDhcpOption options no find code:%{public}d, nIndex:%{public}d!", code, nIndex);
    return NULL;
}

/* Obtains the uint8 data from the packet based on the code. */
bool GetDhcpOptionUint8(const struct DhcpPacket *packet, int code, uint8_t *data)
{
    size_t len = 0;
    const uint8_t *p = GetDhcpOption(packet, code, &len);
    if (p == NULL) {
        LOGW("GetDhcpOptionUint8 GetDhcpOption NULL, code:%{public}d!", code);
        return false;
    }
    if (len < (ssize_t)sizeof(uint8_t)) {
        LOGE("GetDhcpOptionUint8 failed, len:%{public}zu less data:%{public}zu, code:%{public}d!",
            len, sizeof(uint8_t), code);
        return false;
    }
    if (memcpy_s(data, sizeof(uint8_t), p, sizeof(uint8_t)) != EOK) {
        return false;
    }
    return true;
}

/* Obtains the uint32 data from the packet based on the code. */
bool GetDhcpOptionUint32(const struct DhcpPacket *packet, int code, uint32_t *data)
{
    size_t len = 0;
    const uint8_t *p = GetDhcpOption(packet, code, &len);
    if (p == NULL) {
        LOGW("GetDhcpOptionUint32 GetDhcpOption NULL, code:%{public}d!", code);
        return false;
    }
    uint32_t uData = 0;
    if (len < (ssize_t)sizeof(uData)) {
        LOGE("GetDhcpOptionUint32 failed, len:%{public}zu less uData:%{public}zu, code:%{public}d!",
            len, sizeof(uData), code);
        return false;
    }
    if (memcpy_s(&uData, sizeof(uData), p, sizeof(uData)) != EOK) {
        return false;
    }
    if (uData > 0) {
        *data = ntohl(uData);
    }
    return true;
}

/* Obtains the uint32n data from the packet based on the code. */
bool GetDhcpOptionUint32n(const struct DhcpPacket *packet, int code, uint32_t *data1, uint32_t *data2)
{
    size_t len = 0;
    const uint8_t *p = GetDhcpOption(packet, code, &len);
    if (p == NULL) {
        LOGW("GetDhcpOptionUint32n GetDhcpOption NULL, code:%{public}d!", code);
        return false;
    }
    uint32_t uData = 0;
    if ((len < (ssize_t)sizeof(uData)) || (len % (ssize_t)sizeof(uData) != 0)) {
        LOGE("GetDhcpOptionUint32n failed, len:%{public}zu is not %{public}zu * n, code:%{public}d!",
            len, sizeof(uData), code);
        return false;
    }
    if (memcpy_s(&uData, sizeof(uData), p, sizeof(uData)) != EOK) {
        return false;
    }
    if (uData > 0) {
        *data1 = ntohl(uData);
    }
    if (len > (ssize_t)sizeof(uData)) {
        p += sizeof(uData);
        uData = 0;
        if (memcpy_s(&uData, sizeof(uData), p, sizeof(uData)) != EOK) {
            return false;
        }
        if (uData > 0) {
            *data2 = ntohl(uData);
        }
    }
    return true;
}

/* Obtains the string data from the packet based on the code. */
char *GetDhcpOptionString(const struct DhcpPacket *packet, int code)
{
    size_t len;
    const uint8_t *p = GetDhcpOption(packet, code, &len);
    if ((p == NULL) || (*p == '\0')) {
        LOGW("GetDhcpOptionString GetDhcpOption NULL, code:%{public}d!", code);
        return NULL;
    }
    if (len < (ssize_t)sizeof(uint8_t)) {
        LOGE("GetDhcpOptionString failed, len:%{public}zu less data:%{public}zu, code:%{public}d!",
            len, sizeof(uint8_t), code);
        return NULL;
    }

    char *s = (char *)malloc(sizeof(char) * (len + 1));
    if (s) {
        if (memcpy_s(s, len + 1, p, len) != EOK) {
            free(s);
            s = NULL;
            return NULL;
        }
        s[len] = '\0';
    }
    return s;
}

/* Obtain the end index from options. */
int GetEndOptionIndex(uint8_t *pOpts)
{
    int nIndex = 0;
    while (pOpts[nIndex] != END_OPTION) {
        if (pOpts[nIndex] != PAD_OPTION) {
            nIndex += pOpts[nIndex + DHCP_OPT_LEN_INDEX] + DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES;
            continue;
        }
        nIndex++;
    }
    return nIndex;
}

/* Adds a single option string to options. */
int AddOptStrToOpts(uint8_t *pOpts, uint8_t *pOpt, int nOptLen)
{
    int optStrLen = DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + pOpt[DHCP_OPT_LEN_INDEX];
    if (nOptLen != optStrLen) {
        LOGE("AddOptStrToOpts() code:%{public}u nOptLen:%{public}d no equal optStrLen:%{public}d!",
            pOpt[DHCP_OPT_CODE_INDEX], nOptLen, optStrLen);
        return 0;
    }

    int nEndIndex = GetEndOptionIndex(pOpts);
    if ((nEndIndex + nOptLen + 1) >= DHCP_OPTIONS_SIZE) {
        LOGE("AddOptStrToOpts() code:%{public}u did not fit into the packet!", pOpt[DHCP_OPT_CODE_INDEX]);
        return 0;
    }

    LOGI("AddOptStrToOpts() adding option code %{public}u.", pOpt[DHCP_OPT_CODE_INDEX]);
    if (memcpy_s(pOpts + nEndIndex, nOptLen + 1, pOpt, nOptLen) != EOK) {
        return 0;
    }
    pOpts[nEndIndex + nOptLen] = END_OPTION;
    return nOptLen;
}

/* Adds a single option value to options. */
int AddOptValueToOpts(uint8_t *pOpts, uint8_t code, uint32_t value)
{
    uint8_t uLen = GetDhcpOptionDataLen(code);
    if (uLen == 0) {
        LOGE("AddOptValueToOpts() code:%{public}d failed, GetDhcpOptionDataLen uLen:0!", code);
        return 0;
    }

    uint32_t uValue = 0;
    uint8_t *pUint8 = (uint8_t *)&uValue;
    uint16_t *pUint16 = (uint16_t *)&uValue;
    uint32_t *pUint32 = &uValue;
    switch (uLen) {
        case DHCP_UINT8_BYTES:
            *pUint8 =  value;
            break;
        case DHCP_UINT16_BYTES:
            *pUint16 = value;
            break;
        case DHCP_UINT32_BYTES:
            *pUint32 = value;
            break;
        default:
            LOGE("AddOptValueToOpts() uLen:%{public}d error, break!", uLen);
            break;
    }

    uint8_t uOption[DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + DHCP_UINT32_BYTES];
    uOption[DHCP_OPT_CODE_INDEX] = code;
    uOption[DHCP_OPT_LEN_INDEX] = uLen;
    if (memcpy_s(uOption + DHCP_OPT_DATA_INDEX, sizeof(uint32_t), &uValue, uLen) != EOK) {
        return 0;
    }

    int nLen = DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + uOption[DHCP_OPT_LEN_INDEX];
    return AddOptStrToOpts(pOpts, uOption, nLen);
}
