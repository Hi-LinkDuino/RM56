/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_BLOB_IF_H
#define HCS_BLOB_IF_H

#include "hdf_base.h"

#define CONFIG_NODE 0x1
#define CONFIG_ATTR 0x2
#define CONFIG_REFERENCE 0x3
#define CONFIG_ARRAY 0x4
#define CONFIG_BYTE 0x10
#define CONFIG_WORD 0x11
#define CONFIG_DWORD 0x12
#define CONFIG_QWORD 0x13
#define CONFIG_STRING 0x14

struct HbcHeader {
    uint32_t magicNumber;
    uint32_t versionMajor;
    uint32_t versionMinor;
    uint32_t checkSum;
    int32_t totalSize;
};

#define HBC_MAGIC_NUMBER 0xA00AA00A
#define HBC_HEADER_LENGTH sizeof(struct HbcHeader)
#define HBC_BLOB_MAX_LENGTH (1024 * 1024 * 10) // The maximum length is 10 MB.
#define HBC_ROOT_NAME "root"

bool HcsIsByteAlign(void);
#define HCS_ALIGN_SIZE 4
static inline size_t HcsAlignSize(size_t size)
{
    return (size + HCS_ALIGN_SIZE - 1) & (~(HCS_ALIGN_SIZE - 1));
}

static inline uint8_t HcsByteCodeToUint8(const char *start)
{
    return *(uint8_t *)(start);
}

static inline uint16_t HcsByteCodeToUint16(const char *start)
{
    return *(uint16_t *)(start);
}

static inline uint32_t HcsByteCodeToUint32(const char *start)
{
    return *(uint32_t *)(start);
}

static inline uint64_t HcsByteCodeToUint64(const char *start)
{
    return *(uint64_t *)(start);
}

static inline uint32_t HcsGetPrefix(const char *start)
{
    return HcsIsByteAlign() ? HcsByteCodeToUint32(start) : HcsByteCodeToUint8(start);
}

#define HCS_DWORD_LENGTH 4
#define HCS_QWORD_LENGTH 8
#define HCS_PREFIX_LENGTH (HcsIsByteAlign() ? HCS_DWORD_LENGTH : 1)
#define HCS_BYTE_LENGTH (HcsIsByteAlign() ? HCS_DWORD_LENGTH : 1)
#define HCS_WORD_LENGTH (HcsIsByteAlign() ? HCS_DWORD_LENGTH : 2)
#define HCS_STRING_LENGTH(str) (HcsIsByteAlign() ? HcsAlignSize(strlen(str) + 1) : (strlen(str) + 1))
int32_t HcsGetDataTypeOffset(const char *start);
int32_t HcsGetAttrLength(const char *start);
int32_t HcsGetNodeOrAttrLength(const char *start);
int32_t HcsGetNodeLength(const char *blob);
bool HcsCheckBlobFormat(const char *start, uint32_t length);
bool HcsSwapToUint8(uint8_t *value, const char *realValue, uint32_t type);
bool HcsSwapToUint16(uint16_t *value, const char *realValue, uint32_t type);
bool HcsSwapToUint32(uint32_t *value, const char *realValue, uint32_t type);
bool HcsSwapToUint64(uint64_t *value, const char *realValue, uint32_t type);

#endif /* HCS_BLOB_IF_H */