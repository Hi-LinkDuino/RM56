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

#include "hc_parcel.h"
#include "securec.h"
#include "clib_types.h"

const int PARCEL_DEFAULT_INCREASE_STEP = 16;
const uint32_t PARCEL_UINT_MAX = 0xffffffffU;

HcParcel CreateParcel(uint32_t size, uint32_t allocUnit)
{
    HcParcel parcel;
    (void)memset_s(&parcel, sizeof(parcel), 0, sizeof(parcel));
    parcel.allocUnit = allocUnit;
    if (parcel.allocUnit == 0) {
        parcel.allocUnit = PARCEL_DEFAULT_INCREASE_STEP;
    }
    if (size > 0) {
        parcel.data = (char *)ClibMalloc(size, 0);
        if (parcel.data != NULL) {
            parcel.length = size;
        }
    }
    return parcel;
}

void DeleteParcel(HcParcel *parcel)
{
    if (parcel == NULL) {
        return;
    }

    if (parcel->data != NULL) {
        ClibFree(parcel->data);
        parcel->data = 0;
    }
    parcel->length = 0;
    parcel->beginPos = 0;
    parcel->endPos = 0;
}

void ClearParcel(HcParcel *parcel)
{
    if (parcel != NULL) {
        parcel->beginPos = 0;
        parcel->endPos = 0;
    }
}

void ResetParcel(HcParcel *parcel, uint32_t size, uint32_t allocUnit)
{
    if (parcel != NULL) {
        DeleteParcel(parcel);
        HcParcel newParcel = CreateParcel(size, allocUnit);
        (void)memcpy_s(parcel, sizeof(HcParcel), &newParcel, sizeof(HcParcel));
    }
}

uint32_t GetParcelDataSize(const HcParcel *parcel)
{
    if (parcel == NULL) {
        return 0;
    }
    if (parcel->endPos >= parcel->beginPos) {
        return parcel->endPos - parcel->beginPos;
    }
    return 0;
}

const char *GetParcelData(const HcParcel *parcel)
{
    if (parcel == NULL) {
        return NULL;
    }
    return parcel->data + parcel->beginPos;
}

const char *GetParcelLastChar(const HcParcel *parcel)
{
    if (parcel == NULL || GetParcelDataSize(parcel) == 0) {
        return NULL;
    }
    return parcel->data + parcel->endPos - 1;
}

HcBool ParcelReadWithoutPopData(HcParcel *parcel, void *dst, uint32_t dataSize)
{
#ifdef IS_BIG_ENDIAN
    HcBool ret = ParcelReadRevert(parcel, dst, dataSize);
#else
    HcBool ret = ParcelRead(parcel, dst, dataSize);
#endif
    if (ret == HC_TRUE) {
        parcel->beginPos -= dataSize;
    }
    return ret;
}

HcBool ParcelRead(HcParcel *parcel, void *dst, uint32_t dataSize)
{
    errno_t rc;
    if (parcel == NULL || dst == NULL || dataSize == 0) {
        return HC_FALSE;
    }
    if (parcel->beginPos > PARCEL_UINT_MAX - dataSize) {
        return HC_FALSE;
    }
    if (parcel->beginPos + dataSize > parcel->endPos) {
        return HC_FALSE;
    }
    rc = memmove_s(dst, dataSize, parcel->data + parcel->beginPos, dataSize);
    if (rc != EOK) {
        return HC_FALSE;
    }
    parcel->beginPos += dataSize;
    return HC_TRUE;
}

HcBool ParcelEraseBlock(HcParcel *parcel, uint32_t start, uint32_t dataSize, void *dst)
{
    errno_t rc;
    if (parcel == NULL || dst == NULL || dataSize == 0) {
        return HC_FALSE;
    }
    if (start > PARCEL_UINT_MAX - dataSize) {
        return HC_FALSE;
    }
    uint32_t parcelSizeOrg = GetParcelDataSize(parcel);
    if (parcelSizeOrg < start + dataSize) {
        return HC_FALSE;
    }
    char *beginCopy = parcel->data + parcel->beginPos + start;
    uint32_t copySize = parcelSizeOrg - start - dataSize;

    rc = memmove_s(dst, dataSize, beginCopy, dataSize);
    if (rc != EOK) {
        return HC_FALSE;
    }
    if (copySize != 0) {
        rc = memmove_s(beginCopy, copySize, beginCopy + dataSize, copySize);
        if (rc != EOK) {
            return HC_FALSE;
        }
    }
    parcel->endPos -= dataSize;
    return HC_TRUE;
}

HcBool ParcelReadRevert(HcParcel *parcel, void *dst, uint32_t dataSize)
{
    if (ParcelRead(parcel, dst, dataSize)) {
        DataRevert(dst, dataSize);
        return HC_TRUE;
    } else {
        return HC_FALSE;
    }
}

HcBool ParcelWriteRevert(HcParcel *parcel, const void *src, uint32_t dataSize)
{
    errno_t rc;
    void *srcCopy = ClibMalloc(dataSize, 0);
    if (srcCopy == NULL) {
        return HC_FALSE;
    }
    rc = memmove_s(srcCopy, dataSize, src, dataSize);
    if (rc != EOK) {
        ClibFree(srcCopy);
        return HC_FALSE;
    }
    DataRevert(srcCopy, dataSize);
    HcBool ret = ParcelWrite(parcel, srcCopy, dataSize);
    ClibFree(srcCopy);
    return ret;
}

HcBool ParcelReadInt32(HcParcel *parcel, int *dst)
{
    return ParcelRead(parcel, dst, sizeof(int));
}

HcBool ParcelReadUint32(HcParcel *parcel, uint32_t *dst)
{
    return ParcelRead(parcel, dst, sizeof(uint32_t));
}

HcBool ParcelReadInt16(HcParcel *parcel, short *dst)
{
    return ParcelRead(parcel, dst, sizeof(short));
}

HcBool ParcelReadUint16(HcParcel *parcel, uint16_t *dst)
{
    return ParcelRead(parcel, dst, sizeof(uint16_t));
}

HcBool ParcelReadInt8(HcParcel *parcel, char *dst)
{
    return ParcelRead(parcel, dst, sizeof(char));
}

HcBool ParcelReadUint8(HcParcel *parcel, uint8_t *dst)
{
    return ParcelRead(parcel, dst, sizeof(uint8_t));
}

HcBool ParcelReadUint64(HcParcel *parcel, uint64_t *dst)
{
    return ParcelRead(parcel, dst, sizeof(uint64_t));
}

HcBool ParcelReadInt64(HcParcel *parcel, int64_t *dst)
{
    return ParcelRead(parcel, dst, sizeof(int64_t));
}

static HcBool ParcelRealloc(HcParcel *parcel, uint32_t size)
{
    if (parcel->length >= size) {
        return HC_FALSE;
    }
    char *newData = (char *)ClibMalloc(size, 0);
    if (newData == NULL) {
        return HC_FALSE;
    }
    if (memcpy_s(newData, size, parcel->data, parcel->length) != EOK) {
        ClibFree(newData);
        return HC_FALSE;
    }
    ClibFree(parcel->data);
    parcel->data = newData;
    parcel->length = size;
    return HC_TRUE;
}

static HcBool ParcelIncrease(HcParcel *parcel, uint32_t size)
{
    if (parcel == NULL || size == 0) {
        return HC_FALSE;
    }
    if (parcel->data == NULL) {
        if (parcel->length != 0) {
            return HC_FALSE;
        }
        *parcel = CreateParcel(size, parcel->allocUnit);
        if (parcel->data == NULL) {
            return HC_FALSE;
        } else {
            return HC_TRUE;
        }
    } else {
        return ParcelRealloc(parcel, size);
    }
}

static void ParcelRecycle(HcParcel *parcel)
{
    if (parcel == NULL) {
        return;
    }
    if (parcel->data == NULL || parcel->beginPos < parcel->allocUnit) {
        return;
    }

    uint32_t contentSize = parcel->endPos - parcel->beginPos;
    if (contentSize > 0) {
        if (memmove_s(parcel->data, parcel->endPos - parcel->beginPos,
            parcel->data + parcel->beginPos, parcel->endPos - parcel->beginPos) != EOK) {
        }
    }
    parcel->beginPos = 0;
    parcel->endPos = contentSize;
}

static uint32_t GetParcelIncreaseSize(HcParcel *parcel, uint32_t newSize)
{
    if (parcel == NULL || parcel->allocUnit == 0) {
        return 0;
    }
    if (newSize % parcel->allocUnit) {
        return (newSize / parcel->allocUnit + 1) * parcel->allocUnit;
    } else {
        return (newSize / parcel->allocUnit) * parcel->allocUnit;
    }
}

HcBool ParcelWrite(HcParcel *parcel, const void *src, uint32_t dataSize)
{
    errno_t rc;
    if (parcel == NULL || src == NULL || dataSize == 0) {
        return HC_FALSE;
    }
    if (parcel->endPos > PARCEL_UINT_MAX - dataSize) {
        return HC_FALSE;
    }
    if (parcel->endPos + dataSize > parcel->length) {
        ParcelRecycle(parcel);
        if (parcel->endPos + dataSize > parcel->length) {
            uint32_t newSize = GetParcelIncreaseSize(parcel, parcel->endPos + dataSize);
            if (!ParcelIncrease(parcel, newSize)) {
                return HC_FALSE;
            }
        }
    }
    rc = memmove_s(parcel->data + parcel->endPos, dataSize, src, dataSize);
    if (rc != EOK) {
        return HC_FALSE;
    }
    parcel->endPos += dataSize;
    return HC_TRUE;
}

HcBool ParcelWriteInt32(HcParcel *parcel, int src)
{
    return ParcelWrite(parcel, &src, sizeof(src));
}

HcBool ParcelWriteUint32(HcParcel *parcel, uint32_t src)
{
    return ParcelWrite(parcel, &src, sizeof(src));
}

HcBool ParcelWriteInt16(HcParcel *parcel, short src)
{
    return ParcelWrite(parcel, &src, sizeof(src));
}

HcBool ParcelWriteUint16(HcParcel *parcel, uint16_t src)
{
    return ParcelWrite(parcel, &src, sizeof(src));
}

HcBool ParcelWriteInt8(HcParcel *parcel, char src)
{
    return ParcelWrite(parcel, &src, sizeof(src));
}

HcBool ParcelWriteUint8(HcParcel *parcel, uint8_t src)
{
    return ParcelWrite(parcel, &src, sizeof(src));
}

HcBool ParcelWriteUint64(HcParcel *parcel, uint64_t src)
{
    return ParcelWrite(parcel, &src, sizeof(src));
}

HcBool ParcelWriteInt64(HcParcel *parcel, int64_t src)
{
    return ParcelWrite(parcel, &src, sizeof(src));
}

HcBool ParcelReadParcel(HcParcel *src, HcParcel *dst, uint32_t size, HcBool copy)
{
    if (src == NULL || dst == NULL) {
        return HC_FALSE;
    }
    if (GetParcelDataSize(src) < size) {
        return HC_FALSE;
    }
    if (!ParcelWrite(dst, (void *)GetParcelData(src), size)) {
        return HC_FALSE;
    }

    if (!copy) {
        src->beginPos += size;
    }
    return HC_TRUE;
}

HcBool ParcelCopy(HcParcel *src, HcParcel *dst)
{
    if (src == NULL || dst == NULL) {
        return HC_FALSE;
    }
    if (GetParcelDataSize(src) == 0) {
        return HC_TRUE;
    }
    return ParcelReadParcel(src, dst, GetParcelDataSize(src), HC_TRUE);
}

void DataRevert(void *data, uint32_t length)
{
    if (data != NULL) {
        uint8_t *pc = (uint8_t *) data;
        uint32_t i = 0;
        for (; i < length / 2; ++i) { /* half of the length */
            /* swap p[i] and p[length-i-1] */
            pc[i] ^= pc[length - i - 1];
            pc[length - i - 1] ^= pc[i];
            pc[i] ^= pc[length - i - 1];
        }
    }
}

HcBool ParcelReadInt32Revert(HcParcel *parcel, int32_t *dst)
{
    HcBool ret = ParcelRead(parcel, dst, sizeof(int));
    if (ret) {
        DataRevert(dst, sizeof(int));
    }
    return ret;
}

HcBool ParcelReadUint32Revert(HcParcel *parcel, uint32_t *dst)
{
    HcBool ret = ParcelRead(parcel, dst, sizeof(uint32_t));
    if (ret) {
        DataRevert(dst, sizeof(uint32_t));
    }
    return ret;
}

HcBool ParcelReadInt16Revert(HcParcel *parcel, short *dst)
{
    HcBool ret = ParcelRead(parcel, dst, sizeof(short));
    if (ret) {
        DataRevert(dst, sizeof(short));
    }
    return ret;
}

HcBool ParcelReadUint16Revert(HcParcel *parcel, uint16_t *dst)
{
    if (parcel == NULL || dst == NULL) {
        return HC_FALSE;
    }
    HcBool ret = ParcelRead(parcel, dst, sizeof(uint16_t));
    if (ret) {
        DataRevert(dst, sizeof(uint16_t));
    }
    return ret;
}

HcBool ParcelReadInt8Revert(HcParcel *parcel, char *dst)
{
    return ParcelRead(parcel, dst, sizeof(char));
}

HcBool ParcelReadUint8Revert(HcParcel *parcel, uint8_t *dst)
{
    return ParcelRead(parcel, dst, sizeof(uint8_t));
}

HcBool ParcelReadUint64Revert(HcParcel *parcel, uint64_t *dst)
{
    HcBool ret = ParcelRead(parcel, dst, sizeof(uint64_t));
    if (ret) {
        DataRevert(dst, sizeof(uint64_t));
    }
    return ret;
}

HcBool ParcelReadInt64Revert(HcParcel *parcel, int64_t *dst)
{
    HcBool ret = ParcelRead(parcel, dst, sizeof(int64_t));
    if (ret) {
        DataRevert(dst, sizeof(int64_t));
    }
    return ret;
}

HcBool ParcelWriteInt32Revert(HcParcel *parcel, int src)
{
    DataRevert(&src, sizeof(src));
    return ParcelWriteInt32(parcel, src);
}

HcBool ParcelWriteUint32Revert(HcParcel *parcel, uint32_t src)
{
    DataRevert(&src, sizeof(src));
    return ParcelWriteUint32(parcel, src);
}

HcBool ParcelWriteInt16Revert(HcParcel *parcel, short src)
{
    DataRevert(&src, sizeof(src));
    return ParcelWriteInt16(parcel, src);
}

HcBool ParcelWriteUint16Revert(HcParcel *parcel, uint16_t src)
{
    DataRevert(&src, sizeof(src));
    return ParcelWriteUint16(parcel, src);
}

HcBool ParcelWriteInt8Revert(HcParcel *parcel, char src)
{
    return ParcelWriteInt8(parcel, src);
}

HcBool ParcelWriteUint8Revert(HcParcel *parcel, uint8_t src)
{
    return ParcelWriteUint8(parcel, src);
}

HcBool ParcelWriteUint64Revert(HcParcel *parcel, uint64_t src)
{
    DataRevert(&src, sizeof(src));
    return ParcelWriteUint64(parcel, src);
}

HcBool ParcelWriteInt64Revert(HcParcel *parcel, int64_t src)
{
    DataRevert(&src, sizeof(src));
    return ParcelWriteInt64(parcel, src);
}

HcBool ParcelPopBack(HcParcel *parcel, uint32_t size)
{
    if (parcel != NULL && size > 0 && GetParcelDataSize(parcel) >= size) {
        parcel->endPos -= size;
        return HC_TRUE;
    }
    return HC_FALSE;
}

HcBool ParcelPopFront(HcParcel *parcel, uint32_t size)
{
    if ((parcel != NULL) && (size > 0) && (GetParcelDataSize(parcel) >= size)) {
        parcel->beginPos += size;
        return HC_TRUE;
    }
    return HC_FALSE;
}
