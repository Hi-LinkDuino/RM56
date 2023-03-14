/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef HKS_ASN1_H
#define HKS_ASN1_H

#include "hks_type_inner.h"

#define ASN_1_TAG_TYPE_BOOL 0x01
#define ASN_1_TAG_TYPE_INT 0x02
#define ASN_1_TAG_TYPE_BIT_STR 0x03
#define ASN_1_TAG_TYPE_OCT_STR 0x04
#define ASN_1_TAG_TYPE_NULL 0x05
#define ASN_1_TAG_TYPE_OID 0x06
#define ASN_1_TAG_TYPE_ENUMERATED 0x0A
#define ASN_1_TAG_TYPE_UTF8_STR 0x0C
#define ASN_1_TAG_TYPE_PRINTABLE_STR 0x13
#define ASN_1_TAG_TYPE_UTC_TIME 0x17

#define ASN_1_TAG_TYPE_SEQ 0x30
#define ASN_1_TAG_TYPE_SET 0x31

#define ASN_1_TAG_TYPE_CTX_SPEC0 0xA0
#define ASN_1_TAG_TYPE_CTX_SPEC3 0xA3

#define ASN_1_TAG_TYPE_RAW 0xff000001

#define ASN_1_MAX_VAL_NO_EXTRA_LEN_BYTE 0x7F
#define ASN_1_MIN_VAL_1_EXTRA_LEN_BYTE 0x80
#define ASN_1_MIN_VAL_2_EXTRA_LEN_BYTE 0x100

#define ASN_1_TAG_TYPE_1_BYTE_LEN 0x81
#define ASN_1_TAG_TYPE_2_BYTE_LEN 0x82

#define ASN_1_MAX_HEADER_LEN 0x5
#define ASN_1_MIN_HEADER_LEN 0x3

#define ASN_1_TRUE_VALUE 0xFF

#define ASN_1_MAX_SIZE (0x10000 - 0x100)

struct HksAsn1Blob {
    uint32_t type;
    uint32_t size;
    uint8_t *data;
};

struct HksAsn1Obj {
    struct HksAsn1Blob header;
    struct HksAsn1Blob value;
};

#define HKS_ASN1_ENCODE_BYTE(ptr, value) \
do { \
    (ptr)[0] = (uint8_t)((value) & 0xff); \
    (ptr)++; \
} while (0)

#define HKS_ASN1_ENCODE_TWO_BYTE(ptr, value) \
do { \
    (ptr)[0] = (uint8_t)(((value) >> 8) & 0xff); \
    (ptr)++; \
    (ptr)[0] = (uint8_t)((value) & 0xff); \
    (ptr)++; \
} while (0)

#define HKS_ASN1_DECODE_BYTE(ptr, value) \
do { \
    (value) = (uint32_t)((ptr)[0]); \
    (ptr)++; \
} while (0)

#define HKS_ASN1_DECODE_TWO_BYTE(ptr, value) \
do { \
    (value) = (uint32_t)((ptr)[0] & 0xff) << 8; \
    (ptr)++; \
    (value) |= (uint32_t)((ptr)[0] & 0xff); \
    (ptr)++; \
} while (0)

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksAsn1InsertValue(struct HksBlob *buf, struct HksAsn1Obj *obj, const struct HksAsn1Blob *tlv);

int32_t HksAsn1WriteFinal(struct HksBlob *final, const struct HksAsn1Blob *tlv);

int32_t HksAsn1GetObj(struct HksBlob *next, struct HksAsn1Obj *obj, const struct HksBlob *data);

int32_t HksAsn1ExtractTag(struct HksBlob *next, struct HksAsn1Obj *obj, const struct HksBlob *data,
    uint32_t expected_tag);

static inline int32_t CheckAsn1Blob(const struct HksAsn1Blob *blob)
{
    if ((blob == NULL) || (blob->data == NULL) || (blob->size == 0)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}
static inline int32_t CheckAsn1Obj(const struct HksAsn1Obj *obj)
{
    if ((obj == NULL) || (CheckAsn1Blob(&obj->header) != HKS_SUCCESS) || (CheckAsn1Blob(&obj->value) != HKS_SUCCESS)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif
