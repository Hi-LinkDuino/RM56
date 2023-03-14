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

#include "hc_tlv_parser.h"
#include <stddef.h>

#define MAX_TLV_LENGTH (32 * 1024)

HcBool ParseTlvHead(TlvBase *tlv, HcParcel *parcel)
{
#ifdef IS_BIG_ENDIAN
    if (!ParcelReadUint16Revert(parcel, &tlv->tag)) {
        return HC_FALSE;
    }
#else
    if (!ParcelReadUint16(parcel, &tlv->tag)) {
        return HC_FALSE;
    }
#endif

    if (tlv->tag != tlv->checkTag) {
        return HC_FALSE;
    }
#ifdef IS_BIG_ENDIAN
    if (!ParcelReadUint16Revert(parcel, &tlv->length)) {
        return HC_FALSE;
    }
#else
    if (!ParcelReadUint16(parcel, &tlv->length)) {
        return HC_FALSE;
    }
#endif
    if (tlv->length > MAX_TLV_LENGTH) {
        return HC_FALSE;
    }
    return HC_TRUE;
}

int32_t ParseTlvNode(TlvBase *tlv, HcParcel *parcel, HcBool strict)
{
    if (!ParseTlvHead(tlv, parcel)) {
        return TLV_FAIL;
    } else {
        if (GetParcelDataSize(parcel) < tlv->length) {
            return TLV_FAIL;
        }

        int ret = tlv->parse(tlv, parcel, strict);
        if (ret < 0 || ret > MAX_TLV_LENGTH) {
            return TLV_FAIL;
        } else {
            return ret + sizeof(tlv->tag) + sizeof(tlv->length);
        }
    }
}

int32_t GetlenTlvNode(TlvBase *tlv)
{
    int32_t bodyLen = tlv->getlen(tlv);
    if (bodyLen < 0 || bodyLen > MAX_TLV_LENGTH) {
        return TLV_FAIL;
    } else {
        tlv->length = bodyLen + sizeof(tlv->tag) + sizeof(tlv->length);
        return tlv->length;
    }
}

void DeinitTlvNode(TlvBase *tlv)
{
    if (tlv != NULL) {
        tlv->deinit(tlv);
    }
}

int32_t EncodeTlvNode(TlvBase *tlv, HcParcel *parcel, HcBool isRoot)
{
    int32_t bodyLen = tlv->getlen(tlv);
    if (bodyLen < 0 || bodyLen > MAX_TLV_LENGTH) {
        return TLV_FAIL;
    } else if (bodyLen == 0) {
        if (isRoot) {
            ResetParcel(parcel, sizeof(uint16_t) + sizeof(uint16_t), 0);
        }
#ifdef IS_BIG_ENDIAN
        ParcelWriteUint16Revert(parcel, tlv->checkTag);
        ParcelWriteUint16Revert(parcel, bodyLen);
#else
        ParcelWriteUint16(parcel, tlv->checkTag);
        ParcelWriteUint16(parcel, bodyLen);
#endif
        return sizeof(tlv->tag) + sizeof(tlv->length);
    } else { // has value
        if (isRoot) {
            ResetParcel(parcel, sizeof(uint16_t) + sizeof(uint16_t) + bodyLen, 0);
        }
        int32_t encodeLen;
        tlv->length = (uint16_t)bodyLen;
#ifdef IS_BIG_ENDIAN
        ParcelWriteUint16Revert(parcel, tlv->checkTag);
        ParcelWriteUint16Revert(parcel, tlv->length);
#else
        ParcelWriteUint16(parcel, tlv->checkTag);
        ParcelWriteUint16(parcel, tlv->length);
#endif
        encodeLen = tlv->encode(tlv, parcel);
        if (encodeLen < 0 || encodeLen > MAX_TLV_LENGTH) {
            return TLV_FAIL;
        } else {
            return encodeLen + sizeof(tlv->tag) + sizeof(tlv->length);
        }
    }
}

TlvBase* GetEmptyStructNode(TlvBase *tlv, unsigned short tag)
{
    if (tlv == NULL) {
        return NULL;
    }

    unsigned int index;
    unsigned int memberCount = *(unsigned int *)((char *)tlv + offsetof(TlvOffsetExample, offsetCount));
    unsigned int *offset = (unsigned int *)((char *)tlv + offsetof(TlvOffsetExample, offset));
    for (index = 0; index < memberCount; ++index) {
        TlvBase *tlvChild = (TlvBase *)(((char *)tlv) + offset[index]);
        if (tlvChild->checkTag == tag && tlvChild->hasValue == 0) {
            return tlvChild;
        }
    }

    return NULL;
}

int32_t CheckStructNodeAllHasValue(TlvBase *tlv)
{
    if (tlv == NULL) {
        return 0;
    } else {
        unsigned int index;
        unsigned int memberCount = *(unsigned int *)((char *)tlv + offsetof(TlvOffsetExample, offsetCount));
        unsigned int *offset = (unsigned int *)((char *)tlv + offsetof(TlvOffsetExample, offset));
        for (index = 0; index < memberCount; ++index) {
            TlvBase *tlvChild = (TlvBase *)(((char *)tlv) + offset[index]);
            if (tlvChild->hasValue == 0) {
                return -1;
            }
        }
    }

    return 0;
}

void SetStructNodeHasValue(TlvBase *tlv)
{
    if (tlv != NULL) {
        tlv->hasValue = 1;
    }
}

int32_t ParseAndSkipTlvUnknownNode(HcParcel *parcel)
{
    // read tag
    uint16_t tag = 0;
    if (!ParcelReadUint16(parcel, &tag)) {
        return TLV_FAIL;
    }

    // read length
    uint16_t length = 0;
    if (!ParcelReadUint16(parcel, &length)) {
        return TLV_FAIL;
    }

    // pop data
    if (!ParcelPopFront(parcel, length)) {
        return TLV_FAIL;
    }

    return sizeof(tag) + sizeof(length) + length;
}

int32_t ParseTlvStruct(TlvBase *tlv, HcParcel *parcel, HcBool strict)
{
    uint32_t childTotalLength = 0;
    do {
        uint16_t tag = 0;
        if (!ParcelReadWithoutPopData(parcel, &tag, sizeof(tag))) {
            return TLV_FAIL;
        }
        TlvBase *tlvChild = GetEmptyStructNode(tlv, tag);
        if (tlvChild == NULL) {
            if (strict) {
                return TLV_FAIL;
            }

            int32_t unknownChildLength = ParseAndSkipTlvUnknownNode(parcel);
            if (unknownChildLength < 0 || unknownChildLength > MAX_TLV_LENGTH) {
                return TLV_FAIL;
            }
            childTotalLength += unknownChildLength;
            if (childTotalLength > MAX_TLV_LENGTH) {
                return TLV_FAIL;
            }
        } else {
            int32_t childLength = ParseTlvNode(tlvChild, parcel, strict);
            if (childLength < 0 || childLength > MAX_TLV_LENGTH) {
                return TLV_FAIL;
            }
            SetStructNodeHasValue(tlvChild);
            childTotalLength += childLength;
            if (childTotalLength > MAX_TLV_LENGTH) {
                return TLV_FAIL;
            }
        }
    } while (childTotalLength < tlv->length);

    if (childTotalLength > tlv->length) {
        return TLV_FAIL;
    }

    if (strict && CheckStructNodeAllHasValue(tlv) != 0) {
        return TLV_FAIL;
    }

    return childTotalLength;
}

int32_t EncodeTlvStruct(TlvBase *tlv, HcParcel *parcel)
{
    unsigned int index;
    unsigned int memberCount = *(unsigned int *)((char *)tlv + offsetof(TlvOffsetExample, offsetCount));
    unsigned int *offset = (unsigned int *)((char *)tlv + offsetof(TlvOffsetExample, offset));

    uint32_t totalLen = 0;
    for (index = 0; index < memberCount; ++index) {
        TlvBase *tlvChild = (TlvBase *)(((char *)tlv) + offset[index]);
        int32_t childLen = EncodeTlvNode(tlvChild, parcel, HC_FALSE);
        if (childLen < 0 || childLen > MAX_TLV_LENGTH) {
            return TLV_FAIL;
        } else {
            totalLen += childLen;
        }
        if (totalLen > MAX_TLV_LENGTH) {
            return TLV_FAIL;
        }
    }

    return totalLen;
}

int32_t GetLenTlvStruct(TlvBase *tlv)
{
    unsigned int index;
    unsigned int memberCount = *(unsigned int *)((char *)tlv + offsetof(TlvOffsetExample, offsetCount));
    unsigned int *offset = (unsigned int *)((char *)tlv + offsetof(TlvOffsetExample, offset));
    uint32_t childTotalLength = 0;

    for (index = 0; index < memberCount; ++index) {
        TlvBase *tlvChild = (TlvBase *)(((char *)tlv) + offset[index]);
        int32_t childLength = GetlenTlvNode(tlvChild);
        if (childLength <= 0 || childLength > MAX_TLV_LENGTH) {
            return TLV_FAIL;
        } else {
            childTotalLength += childLength;
        }
        if (childTotalLength > MAX_TLV_LENGTH) {
            return TLV_FAIL;
        }
    }

    return childTotalLength;
}

void DeinitTlvStruct(TlvBase *tlv)
{
    unsigned int index;
    unsigned int memberCount = *(unsigned int *)((char *)tlv + offsetof(TlvOffsetExample, offsetCount));
    unsigned int *offset = (unsigned int *)((char *)tlv + offsetof(TlvOffsetExample, offset));

    for (index = 0; index < memberCount; ++index) {
        TlvBase *tlvChild = (TlvBase *)(((char *)tlv) + offset[index]);
        DeinitTlvNode(tlvChild);
    }
}

HcBool DecodeTlvMessage(TlvBase *msg, HcParcel *parcel, HcBool strict)
{
    if (msg == NULL || parcel == NULL) {
        return HC_FALSE;
    } else {
        int32_t msgLen = ParseTlvNode(msg, parcel, strict);
        if (msgLen > MAX_TLV_LENGTH) {
            return HC_FALSE;
        }
        if ((int32_t)(msg->length + sizeof(msg->length) + sizeof(msg->tag)) != msgLen) {
            return HC_FALSE;
        }

        if (GetParcelDataSize(parcel) != 0) {
            return HC_FALSE;
        }
    }

    return HC_TRUE;
}


HcBool EncodeTlvMessage(TlvBase *msg, HcParcel *parcel)
{
    if (msg == NULL || parcel == NULL) {
        return HC_FALSE;
    } else {
        if (EncodeTlvNode(msg, parcel, HC_TRUE) < 0) {
            return HC_FALSE;
        }
    }

    return HC_TRUE;
}

int32_t ParseTlvBuffer(TlvBase *tlv, HcParcel *parcel, HcBool strict)
{
    (void)strict;
    TlvBuffer *realTlv = (TlvBuffer *)(tlv);
    if (tlv->length == 0 || ParcelReadParcel(parcel, &realTlv->data, tlv->length, HC_FALSE)) {
        return tlv->length;
    } else {
        return TLV_FAIL;
    }
}

int32_t GetlenTlvBuffer(TlvBase *tlv)
{
    TlvBuffer *realTlv = (TlvBuffer *)(tlv);
    return (int32_t)GetParcelDataSize(&realTlv->data);
}

int32_t EncodeTlvBuffer(TlvBase *tlv, HcParcel *parcel)
{
    TlvBuffer *realTlv = (TlvBuffer *)(tlv);
    int32_t len = GetlenTlvBuffer(tlv);
    if (len <= 0 || len > MAX_TLV_LENGTH) {
        return TLV_FAIL;
    }

    if (ParcelReadParcel(&realTlv->data, parcel, len, HC_TRUE)) {
        return len;
    } else {
        return TLV_FAIL;
    }
}

void DeinitTlvBuffer(TlvBase *tlv)
{
    DeleteParcel(&((TlvBuffer *)tlv)->data);
}

void InitTlvBuffer(TlvBuffer *tlv, unsigned short checkTag)
{
    (void)memset_s(&tlv->base, sizeof(tlv->base), 0, sizeof(tlv->base));
    tlv->base.parse = ParseTlvBuffer;
    tlv->base.getlen = GetlenTlvBuffer;
    tlv->base.encode = EncodeTlvBuffer;
    tlv->base.deinit = DeinitTlvBuffer;
    tlv->base.checkTag = checkTag;
    tlv->data = CreateParcel(PARCEL_DEFAULT_LENGTH, PARCEL_DEFAULT_ALLOC_UNIT);
}

int32_t ParseTlvString(TlvBase *tlv, HcParcel *parcel, HcBool strict)
{
    (void)strict;
    TlvString *realTlv = (TlvString *)(tlv);
    ClearParcel(&realTlv->data.parcel);
    if (tlv->length == 0 || ParcelReadParcel(parcel, &realTlv->data.parcel, tlv->length, HC_FALSE)) {
        return tlv->length;
    } else {
        return TLV_FAIL;
    }
}

int32_t GetlenTlvString(TlvBase *tlv)
{
    TlvString *realTlv = (TlvString *)(tlv);
    return (int32_t)GetParcelDataSize(&realTlv->data.parcel);
}

int32_t EncodeTlvString(TlvBase *tlv, HcParcel *parcel)
{
    TlvString *realTlv = (TlvString *)(tlv);
    int32_t len = GetlenTlvString(tlv);
    if (len <= 0 || len > MAX_TLV_LENGTH) {
        return TLV_FAIL;
    }

    if (ParcelReadParcel(&realTlv->data.parcel, parcel, len, HC_TRUE)) {
        return len;
    } else {
        return TLV_FAIL;
    }
}

void DeinitTlvString(TlvBase *tlv)
{
    DeleteString(&((TlvString*)tlv)->data);
}

void InitTlvString(TlvString *tlv, unsigned short checkTag)
{
    (void)memset_s(&tlv->base, sizeof(tlv->base), 0, sizeof(tlv->base));
    tlv->base.parse = ParseTlvString;
    tlv->base.getlen = GetlenTlvString;
    tlv->base.encode = EncodeTlvString;
    tlv->base.deinit = DeinitTlvString;
    tlv->base.checkTag = checkTag;
    tlv->data = CreateString();
}

unsigned short GetTag(unsigned short checkTag, unsigned short defaultTag)
{
    if (checkTag == USE_DEFAULT_TAG) {
        return defaultTag;
    } else {
        return checkTag;
    }
}

void DeinitTlvFixMember(TlvBase* tlv)
{
    (void)tlv;
    return;
}

#ifdef IS_BIG_ENDIAN
DEFINE_TLV_FIX_LENGTH_TYPE(TlvInt64, NEED_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvInt32, NEED_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvInt16, NEED_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvInt8, NEED_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvUint64, NEED_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvUint32, NEED_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvUint16, NEED_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvUint8, NEED_REVERT)
#else
DEFINE_TLV_FIX_LENGTH_TYPE(TlvInt64, NO_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvInt32, NO_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvInt16, NO_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvInt8, NO_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvUint64, NO_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvUint32, NO_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvUint16, NO_REVERT)
DEFINE_TLV_FIX_LENGTH_TYPE(TlvUint8, NO_REVERT)
#endif
