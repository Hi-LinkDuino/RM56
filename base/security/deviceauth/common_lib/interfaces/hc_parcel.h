/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HC_PARCEL_H
#define HC_PARCEL_H

#include <stdint.h>
#include "clib_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PARCEL_DEFAULT_LENGTH 0
#define PARCEL_DEFAULT_ALLOC_UNIT 0

typedef struct {
    char *data;
    unsigned int beginPos;
    unsigned int endPos;
    unsigned int length;
    unsigned int allocUnit;
} HcParcel;

HcParcel CreateParcel(uint32_t size, uint32_t allocUnit);
void DeleteParcel(HcParcel *parcel);
void ClearParcel(HcParcel *parcel);
void ResetParcel(HcParcel *parcel, uint32_t size, uint32_t allocUnit);
HcBool ParcelReadWithoutPopData(HcParcel *parcel, void *dst, uint32_t dataSize);
HcBool ParcelRead(HcParcel *parcel, void *dst, uint32_t dataSize);
HcBool ParcelWrite(HcParcel *parcel, const void *src, uint32_t dataSize);
HcBool ParcelReadRevert(HcParcel *parcel, void *dst, uint32_t dataSize);
HcBool ParcelWriteRevert(HcParcel *parcel, const void *src, uint32_t dataSize);
uint32_t GetParcelDataSize(const HcParcel *parcel);
const char *GetParcelData(const HcParcel *parcel);
const char* GetParcelLastChar(const HcParcel *parcel);

HcBool ParcelReadInt32(HcParcel *parcel, int *dst);
HcBool ParcelReadUint32(HcParcel *parcel, uint32_t *dst);
HcBool ParcelReadInt16(HcParcel *parcel, short *dst);
HcBool ParcelReadUint16(HcParcel *parcel, uint16_t *dst);
HcBool ParcelReadInt8(HcParcel *parcel, char *dst);
HcBool ParcelReadUint8(HcParcel *parcel, uint8_t *dst);
HcBool ParcelReadUint64(HcParcel *parcel, uint64_t *dst);
HcBool ParcelReadInt64(HcParcel *parcel, int64_t *dst);
HcBool ParcelWriteInt32(HcParcel *parcel, int src);
HcBool ParcelWriteUint32(HcParcel *parcel, uint32_t src);
HcBool ParcelWriteInt16(HcParcel *parcel, short src);
HcBool ParcelWriteUint16(HcParcel *parcel, uint16_t src);
HcBool ParcelWriteInt8(HcParcel *parcel, char src);
HcBool ParcelWriteUint8(HcParcel *parcel, uint8_t src);
HcBool ParcelWriteUint64(HcParcel *parcel, uint64_t src);
HcBool ParcelWriteInt64(HcParcel *parcel, int64_t src);
HcBool ParcelWriteString(HcParcel *parcel, const char *str);
HcBool ParcelReadString(HcParcel *parcel, char **str);
HcBool ParcelReadParcel(HcParcel *src, HcParcel *dst, uint32_t size, HcBool copy);
HcBool ParcelCopy(HcParcel *src, HcParcel *dst);

HcBool ParcelReadInt32Revert(HcParcel *parcel, int32_t *dst);
HcBool ParcelReadUint32Revert(HcParcel *parcel, uint32_t *dst);
HcBool ParcelReadInt16Revert(HcParcel *parcel, short *dst);
HcBool ParcelReadUint16Revert(HcParcel *parcel, uint16_t *dst);
HcBool ParcelReadInt8Revert(HcParcel *parcel, char *dst);
HcBool ParcelReadUint8Revert(HcParcel *parcel, uint8_t *dst);
HcBool ParcelReadUint64Revert(HcParcel *parcel, uint64_t *dst);
HcBool ParcelReadInt64Revert(HcParcel *parcel, int64_t *dst);
HcBool ParcelWriteInt32Revert(HcParcel *parcel, int src);
HcBool ParcelWriteUint32Revert(HcParcel *parcel, uint32_t src);
HcBool ParcelWriteInt16Revert(HcParcel *parcel, short src);
HcBool ParcelWriteUint16Revert(HcParcel *parcel, uint16_t src);
HcBool ParcelWriteInt8Revert(HcParcel *parcel, char src);
HcBool ParcelWriteUint8Revert(HcParcel *parcel, uint8_t src);
HcBool ParcelWriteUint64Revert(HcParcel *parcel, uint64_t src);
HcBool ParcelWriteInt64Revert(HcParcel *parcel, int64_t src);

void DataRevert(void *data, uint32_t length);
HcBool ParcelPopBack(HcParcel *parcel, uint32_t size);
HcBool ParcelPopFront(HcParcel *parcel, uint32_t size);
HcBool ParcelEraseBlock(HcParcel *parcel, uint32_t start, uint32_t data_size, void *dst);

#ifdef __cplusplus
}
#endif
#endif
