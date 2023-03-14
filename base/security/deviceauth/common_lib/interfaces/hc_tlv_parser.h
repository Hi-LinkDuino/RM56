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

#ifndef HC_TLV_PARSER_H
#define HC_TLV_PARSER_H

#include <hc_parcel.h>
#include <hc_vector.h>
#include <hc_string.h>

#define USE_DEFAULT_TAG 0xFFFF
#define TLV_FAIL (-1)
#define NO_REVERT 0
#define NEED_REVERT 1
#define MAX_TOTOL_LEN (100 * 1024 * 1024)

typedef struct TlvBaseT {
    unsigned short tag;
    unsigned short length;
    unsigned short checkTag;
    unsigned short hasValue;
    int32_t (*parse)(struct TlvBaseT *, HcParcel *, HcBool);
    int32_t (*getlen)(struct TlvBaseT *);
    int32_t (*encode)(struct TlvBaseT *, HcParcel *);
    void (*deinit)(struct TlvBaseT *);
} TlvBase;

#define DECLARE_TLV_STRUCT(x) \
    TlvBase base; \
    unsigned int offsetCount; \
    unsigned int offset[x];

unsigned short GetTag(unsigned short checkTag, unsigned short defaultTag);

#define BEGIN_TLV_STRUCT_DEFINE(TlvS, CheckTag) \
void Init##TlvS(TlvS *tlv, unsigned short checkTag) \
{ \
    typedef TlvS TlvStructType; \
    unsigned int index = 0; \
    (void)memset_s(&tlv->base, sizeof(tlv->base), 0, sizeof(tlv->base)); \
    tlv->base.checkTag = GetTag(checkTag, CheckTag);

#define TLV_MEMBER_OPTION(TlvMember, TlvMemberName, CheckTag) \
    Init##TlvMember(&tlv->TlvMemberName, CheckTag); \
    tlv->TlvMemberName.base.option = 1; \
    tlv->offset[index++] = offsetof(TlvStructType, TlvMemberName);

#define TLV_MEMBER(TlvMember, TlvMemberName, CheckTag) \
    Init##TlvMember(&tlv->TlvMemberName, CheckTag); \
    tlv->offset[index++] = offsetof(TlvStructType, TlvMemberName);

#define END_TLV_STRUCT_DEFINE(void) \
    tlv->offsetCount = index; \
    tlv->base.parse = ParseTlvStruct; \
    tlv->base.getlen = GetLenTlvStruct; \
    tlv->base.encode = EncodeTlvStruct; \
    tlv->base.deinit = DeinitTlvStruct; \
}

#define DECLARE_TLV_FIX_LENGTH_TYPE(TlvName, TypeName) \
typedef struct \
{ \
    TlvBase base; \
    TypeName data; \
} TlvName;

DECLARE_TLV_FIX_LENGTH_TYPE(TlvInt32, int)
DECLARE_TLV_FIX_LENGTH_TYPE(TlvInt16, short)
DECLARE_TLV_FIX_LENGTH_TYPE(TlvInt8, char)
DECLARE_TLV_FIX_LENGTH_TYPE(TlvUint32, uint32_t)
DECLARE_TLV_FIX_LENGTH_TYPE(TlvUint16, uint16_t)
DECLARE_TLV_FIX_LENGTH_TYPE(TlvUint8, uint8_t)
DECLARE_TLV_FIX_LENGTH_TYPE(TlvUint64, uint64_t)
DECLARE_TLV_FIX_LENGTH_TYPE(TlvInt64, uint64_t)

#define DEFINE_TLV_FIX_LENGTH_TYPE(TlvName, Revert) \
int32_t ParseTlv##TlvName(TlvBase *tlv, HcParcel *parcel, HcBool strict) \
{ \
    (void)strict; \
    TlvName *realTlv = (TlvName *)(tlv); \
    HcBool readRet = HC_FALSE; \
    if (tlv->length != sizeof(realTlv->data)) \
    { \
        return TLV_FAIL; \
    } \
\
    if (Revert) \
    { \
        readRet = ParcelReadRevert(parcel, &realTlv->data, sizeof(realTlv->data)); \
    } else { \
        readRet = ParcelRead(parcel, &realTlv->data, sizeof(realTlv->data)); \
    } \
    if (readRet) \
    { \
        return tlv->length; \
    } else { \
        return TLV_FAIL; \
    } \
} \
\
int32_t GetLenTlv##TlvName(TlvBase *tlv) \
{ \
    TlvName *realTlv = (TlvName *)(tlv); \
    return (int32_t)sizeof(realTlv->data); \
} \
\
int32_t EncodeTlv##TlvName(TlvBase *tlv, HcParcel *parcel) \
{ \
    HcBool writeRet = HC_FALSE; \
    TlvName *realTlv = (TlvName *)(tlv); \
    if (Revert) \
    { \
        writeRet = ParcelWriteRevert(parcel, &realTlv->data, sizeof(realTlv->data)); \
    } else { \
        writeRet = ParcelWrite(parcel, &realTlv->data, sizeof(realTlv->data)); \
    } \
    if (writeRet) \
    { \
        return sizeof(realTlv->data); \
    } else { \
        return TLV_FAIL; \
    } \
} \
\
DECLARE_TLV_PARSE_FUNC(TlvName, ParseTlv##TlvName, GetLenTlv##TlvName, EncodeTlv##TlvName);

void DeinitTlvFixMember(TlvBase *tlv);

#define DECLARE_TLV_PARSE_FUNC(TlvName, TlvParseFunc, TlvGetLenFunc, TlvEncodeFunc) \
void Init##TlvName(TlvName *tlv, unsigned short checkTag) \
{ \
    (void)memset_s(&tlv->base, sizeof(tlv->base), 0, sizeof(tlv->base)); \
    tlv->base.parse = TlvParseFunc; \
    tlv->base.getlen = TlvGetLenFunc; \
    tlv->base.encode = TlvEncodeFunc; \
    tlv->base.deinit = DeinitTlvFixMember; \
    tlv->base.checkTag = checkTag; \
}

#define TLV_INIT(TlvName, TlvData) Init##TlvName(TlvData, USE_DEFAULT_TAG);

#define TLV_DEINIT(TlvData) TlvData.base.deinit((TlvBase *)(&TlvData));
typedef struct {
    TlvBase base;
    unsigned int offsetCount;
    unsigned int offset[0];
} TlvOffsetExample;

HcBool ParseTlvHead(TlvBase *tlv, HcParcel *parcel);
int32_t ParseTlvNode(TlvBase *tlv, HcParcel *parcel, HcBool strict);
int32_t GetlenTlvNode(TlvBase *tlv);
void DeinitTlvNode(TlvBase *tlv);

int32_t ParseTlvStruct(TlvBase *tlv, HcParcel *parcel, HcBool strict);
int32_t EncodeTlvStruct(TlvBase *tlv, HcParcel *parcel);
int32_t GetLenTlvStruct(TlvBase *tlv);
void DeinitTlvStruct(TlvBase *tlv);
int32_t EncodeTlvNode(TlvBase *tlv, HcParcel *parcel, HcBool isRoot);
HcBool DecodeTlvMessage(TlvBase *msg, HcParcel *parcel, HcBool strict);
HcBool EncodeTlvMessage(TlvBase *msg, HcParcel *parcel);

typedef struct {
    TlvBase base;
    HcParcel data;
} TlvBuffer;

void InitTlvBuffer(TlvBuffer *tlv, unsigned short checkTag);
int32_t ParseTlvBuffer(TlvBase *tlv, HcParcel *parcel, HcBool strict);
int32_t GetlenTlvBuffer(TlvBase *tlv);
int32_t EncodeTlvBuffer(TlvBase *tlv, HcParcel *parcel);
void DeinitTlvBuffer(TlvBase *tlv);

typedef struct {
    TlvBase base;
    HcString data;
} TlvString;

void InitTlvString(TlvString *tlv, unsigned short checkTag);
int32_t ParseTlvString(TlvBase *tlv, HcParcel *parcel, HcBool strict);
int32_t GetlenTlvString(TlvBase *tlv);
int32_t EncodeTlvString(TlvBase *tlv, HcParcel *parcel);
void DeinitTlvString(TlvBase *tlv);

#define DECLEAR_INIT_FUNC(TlvStruct) \
void Init##TlvStruct(TlvStruct *tlv, unsigned short checkTag);

DECLEAR_INIT_FUNC(TlvUint64)
DECLEAR_INIT_FUNC(TlvUint32)
DECLEAR_INIT_FUNC(TlvUint16)
DECLEAR_INIT_FUNC(TlvUint8)
DECLEAR_INIT_FUNC(TlvInt64)
DECLEAR_INIT_FUNC(TlvInt32)
DECLEAR_INIT_FUNC(TlvInt16)
DECLEAR_INIT_FUNC(TlvInt8)

#define DECLARE_TLV_VECTOR(TlvVecName, TlvVecElement) \
DECLARE_HC_VECTOR(Vec##TlvVecName, TlvVecElement) \
typedef struct { \
    TlvBase base; \
    Vec##TlvVecName data; \
} TlvVecName; \
void DeinitTlv##TlvVecName(TlvBase *tlv); \
void Init##TlvVecName(TlvVecName *tlv, unsigned short checkTag);

#define IMPLEMENT_TLV_VECTOR(TlvVecName, TlvElementName, VecAllocCount) \
IMPLEMENT_HC_VECTOR(Vec##TlvVecName, TlvElementName, VecAllocCount) \
int32_t ParseTlv##TlvVecName(TlvBase *tlv, HcParcel *parcel, HcBool strict) \
{ \
    TlvVecName *realTlv = (TlvVecName *)(tlv); \
    uint32_t count = 0; \
    if (!ParcelReadUint32(parcel, &count)) { \
        return TLV_FAIL; \
    } \
    int32_t totalLen = sizeof(count); \
    uint32_t index = 0; \
    for (index = 0; index < count; ++index) { \
        TlvElementName tlvElement; \
        TlvElementName *curElement = realTlv->data.pushBack(&realTlv->data, &tlvElement); \
        if (curElement == NULL) { \
            return TLV_FAIL; \
        } \
        TLV_INIT(TlvElementName, curElement); \
\
        int32_t elementLen = ParseTlvNode((TlvBase *)curElement, parcel, strict); \
        if (elementLen < 0) { \
            return TLV_FAIL; \
        } \
        totalLen += elementLen; \
        if (totalLen >= MAX_TOTOL_LEN) { \
            return TLV_FAIL; \
        } \
    } \
\
    return totalLen; \
} \
\
int32_t EncodeTlv##TlvVecName(TlvBase *tlv, HcParcel *parcel) \
{ \
    TlvVecName *realTlv = (TlvVecName *)(tlv); \
    uint32_t index = 0; \
    TlvElementName *element = NULL; \
    uint32_t totalLen = 4; \
    uint32_t count = realTlv->data.size(&realTlv->data); \
    if (!ParcelWriteUint32(parcel, count)) { \
        return TLV_FAIL; \
    } \
\
    FOR_EACH_HC_VECTOR(realTlv->data, index, element) { \
        if (element != NULL) { \
            uint32_t len = EncodeTlvNode((TlvBase *)element, parcel, HC_FALSE); \
            totalLen += len; \
            if (totalLen >= MAX_TOTOL_LEN) { \
                return TLV_FAIL; \
            } \
        } \
    } \
    return totalLen; \
} \
int32_t GetLenTlv##TlvVecName(TlvBase *tlv) \
{ \
    TlvVecName *realTlv = (TlvVecName *)(tlv); \
    uint32_t index = 0; \
    TlvElementName *element = NULL; \
    uint32_t totalLen = sizeof(uint32_t); \
    FOR_EACH_HC_VECTOR(realTlv->data, index, element) { \
        if (element != NULL) { \
            totalLen += GetlenTlvNode((TlvBase *)element); \
            if (totalLen >= MAX_TOTOL_LEN) { \
                return TLV_FAIL; \
            } \
        } else { \
            return TLV_FAIL; \
        } \
    } \
    return totalLen; \
} \
\
void DeinitTlv##TlvVecName(TlvBase *tlv) \
{ \
    TlvVecName *realTlv = (TlvVecName *)(tlv); \
    uint32_t index = 0; \
    TlvElementName *element = NULL; \
    FOR_EACH_HC_VECTOR(realTlv->data, index, element) { \
        if (element != NULL) { \
            TLV_DEINIT((*element)); \
        } \
    } \
    DESTROY_HC_VECTOR(Vec##TlvVecName, &((TlvVecName *)tlv)->data); \
} \
\
void Init##TlvVecName(TlvVecName *tlv, unsigned short checkTag) \
{ \
    (void)memset_s(&tlv->base, sizeof(tlv->base), 0, sizeof(tlv->base)); \
    tlv->base.parse = ParseTlv##TlvVecName; \
    tlv->base.encode = EncodeTlv##TlvVecName; \
    tlv->base.getlen = GetLenTlv##TlvVecName; \
    tlv->base.deinit = DeinitTlv##TlvVecName; \
    tlv->base.checkTag = checkTag; \
    tlv->data = CREATE_HC_VECTOR(Vec##TlvVecName); \
}
#endif
