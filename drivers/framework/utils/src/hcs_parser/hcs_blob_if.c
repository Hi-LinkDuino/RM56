/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_blob_if.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hcs_blob_if

static bool g_byteAlign = false;

bool HcsIsByteAlign(void)
{
    return g_byteAlign;
}

int32_t HcsGetDataTypeOffset(const char *start)
{
    int32_t len;
    switch (HcsGetPrefix(start)) {
        case CONFIG_BYTE:
            len = HCS_BYTE_LENGTH + HCS_PREFIX_LENGTH;
            break;
        case CONFIG_WORD:
            len = HCS_WORD_LENGTH + HCS_PREFIX_LENGTH;
            break;
        case CONFIG_DWORD:
        case CONFIG_REFERENCE:
            len = HCS_DWORD_LENGTH + HCS_PREFIX_LENGTH;
            break;
        case CONFIG_QWORD:
            len = HCS_QWORD_LENGTH + HCS_PREFIX_LENGTH;
            break;
        case CONFIG_STRING:
            len = HCS_STRING_LENGTH(start + HCS_PREFIX_LENGTH) + HCS_PREFIX_LENGTH;
            break;
        default:
            len = HDF_FAILURE;
            break;
    }
    return len;
}

static int32_t HcsGetArrayLength(const char *start)
{
    int32_t arrayLen = HCS_PREFIX_LENGTH + HCS_WORD_LENGTH;
    uint16_t count;
    uint16_t i;
    if (!HcsSwapToUint16(&count, start + HCS_PREFIX_LENGTH, CONFIG_WORD)) {
        HDF_LOGE("%s failed", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < count; i++) {
        int32_t lenData = HcsGetDataTypeOffset(start + arrayLen);
        if (lenData < 0) {
            return HDF_FAILURE;
        }
        arrayLen += lenData;
    }
    return arrayLen;
}

int32_t HcsGetAttrLength(const char *start)
{
    int32_t length = HCS_PREFIX_LENGTH + HCS_STRING_LENGTH(start + HCS_PREFIX_LENGTH);
    int32_t dataLen;
    switch (HcsGetPrefix(start + length)) {
        case CONFIG_BYTE:
        case CONFIG_WORD:
        case CONFIG_DWORD:
        case CONFIG_QWORD:
        case CONFIG_STRING:
        case CONFIG_REFERENCE:
            dataLen = HcsGetDataTypeOffset(start + length);
            break;
        case CONFIG_ARRAY:
            dataLen = HcsGetArrayLength(start + length);
            break;
        default:
            dataLen = HDF_FAILURE;
            break;
    }
    return (dataLen < 0) ? HDF_FAILURE : (dataLen + length);
}

int32_t HcsGetNodeOrAttrLength(const char *start)
{
    int32_t length;
    switch (HcsGetPrefix(start)) {
        case CONFIG_NODE:
            length = HCS_STRING_LENGTH(start + HCS_PREFIX_LENGTH) + HCS_PREFIX_LENGTH + HCS_DWORD_LENGTH;
            break;
        case CONFIG_ATTR:
            length = HcsGetAttrLength(start);
            break;
        default:
            length = HDF_FAILURE;
            break;
    }
    return length;
}

int32_t HcsGetNodeLength(const char *blob)
{
    int32_t rootLen = HcsGetNodeOrAttrLength(blob);
    if (rootLen < 0) {
        HDF_LOGE("%s failed, the rootLen is %d", __func__, rootLen);
        return HDF_FAILURE;
    }
    rootLen += (int32_t)HcsByteCodeToUint32(blob + HCS_PREFIX_LENGTH + HCS_STRING_LENGTH(blob + HCS_PREFIX_LENGTH));
    return rootLen;
}

bool HcsSwapToUint8(uint8_t *value, const char *realValue, uint32_t type)
{
    if (type == CONFIG_BYTE) {
        *value = g_byteAlign ? (uint8_t)HcsByteCodeToUint32(realValue) : HcsByteCodeToUint8(realValue);
        return true;
    }
    HDF_LOGE("%s failed, type: %u", __func__, type);
    return false;
}

bool HcsSwapToUint16(uint16_t *value, const char *realValue, uint32_t type)
{
    uint8_t data;
    if (type == CONFIG_WORD) {
        *value =  g_byteAlign ? (uint16_t)HcsByteCodeToUint32(realValue) : HcsByteCodeToUint16(realValue);
        return true;
    }
    if (HcsSwapToUint8(&data, realValue, type)) {
        *value = data;
        return true;
    }
    HDF_LOGE("%s failed, type: %u", __func__, type);
    return false;
}

bool HcsSwapToUint32(uint32_t *value, const char *realValue, uint32_t type)
{
    uint16_t data;
    if (type == CONFIG_DWORD) {
        *value = HcsByteCodeToUint32(realValue);
        return true;
    }
    if (HcsSwapToUint16(&data, realValue, type)) {
        *value = data;
        return true;
    }
    HDF_LOGE("%s failed, type: %u", __func__, type);
    return false;
}

bool HcsSwapToUint64(uint64_t *value, const char *realValue, uint32_t type)
{
    uint32_t data;
    if (type == CONFIG_QWORD) {
        *value = HcsByteCodeToUint64(realValue);
        return true;
    }
    if (HcsSwapToUint32(&data, realValue, type)) {
        *value = data;
        return true;
    }
    HDF_LOGE("%s failed, type: %u", __func__, type);
    return false;
}

static bool CheckHcsBlobLength(uint32_t length, struct HbcHeader *header)
{
    uint32_t rootNodeLen = HCS_STRING_LENGTH(HBC_ROOT_NAME) + HCS_PREFIX_LENGTH + HCS_DWORD_LENGTH;
    uint32_t minLength = rootNodeLen + HBC_HEADER_LENGTH;
    uint32_t blobLength;
    if (header->totalSize >= 0) {
        blobLength = (uint32_t)(HBC_HEADER_LENGTH + header->totalSize);
        g_byteAlign = false;
        HDF_LOGI("%s: the blobLength: %u, byteAlign: %d", __func__, blobLength, g_byteAlign);
    } else {
        blobLength = (uint32_t)(HBC_HEADER_LENGTH - header->totalSize);
        g_byteAlign = true;
        HDF_LOGI("%s: the blobLength: %u, byteAlign: %d", __func__, blobLength, g_byteAlign);
    }
    if ((length != blobLength) || (blobLength < minLength)) {
        HDF_LOGE("%s failed, Hcsblob file length is %u,  but the calculated length is %u",
                 __func__, length, blobLength);
        return false;
    }
    return true;
}

bool HcsCheckBlobFormat(const char *start, uint32_t length)
{
    struct HbcHeader *header = NULL;
    if ((start == NULL) || (length < HBC_HEADER_LENGTH) || (length > HBC_BLOB_MAX_LENGTH)) {
        HDF_LOGE("%s failed, Hcsblob file length is %u", __func__, length);
        return false;
    }
    header = (struct HbcHeader *)start;
    if (header->magicNumber != HBC_MAGIC_NUMBER) {
        HDF_LOGE("%s failed, the magic number of HBC is %x", __func__, header->magicNumber);
        return false;
    }
    if (!CheckHcsBlobLength(length, header)) {
        return false;
    }
    return true;
}
