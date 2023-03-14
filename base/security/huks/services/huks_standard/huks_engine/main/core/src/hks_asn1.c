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

#include "hks_asn1.h"
#include "hks_log.h"

#define BIT_NUM_OF_UINT8 8
#define ASN_1_EXPLICIT_TAG_LEN 3
#define ASN_1_EXPLICIT_TAG_TYPE_BOOL 0xA0
#define ASN_1_TAG_TYPE_EXTRA_IDENTIFIER 0x1F
#define TLV_HEADER_TYPE_2_LEN 4
#define BASE128_ENCODE_BIT_LEN 7


static void BuildTlvHeader(struct HksAsn1Blob *header, const struct HksAsn1Blob *tlv)
{
    uint32_t len = tlv->size;
    uint8_t *hptr = header->data;
    HKS_ASN1_ENCODE_BYTE(hptr, tlv->type);

    uint32_t pad = 0;
    if ((tlv->type == ASN_1_TAG_TYPE_BIT_STR) || ((tlv->type == ASN_1_TAG_TYPE_INT) &&
        (tlv->data[0] > ASN_1_MAX_VAL_NO_EXTRA_LEN_BYTE))) {
            pad++;
            len++;
    }

    if (len < ASN_1_MIN_VAL_1_EXTRA_LEN_BYTE) {
        HKS_ASN1_ENCODE_BYTE(hptr, len);
    } else if (len < ASN_1_MIN_VAL_2_EXTRA_LEN_BYTE) {
        HKS_ASN1_ENCODE_BYTE(hptr, ASN_1_TAG_TYPE_1_BYTE_LEN);
        HKS_ASN1_ENCODE_BYTE(hptr, len);
    } else {
        HKS_ASN1_ENCODE_BYTE(hptr, ASN_1_TAG_TYPE_2_BYTE_LEN);
        HKS_ASN1_ENCODE_TWO_BYTE(hptr, len);
    }

    if (pad != 0) {
        HKS_ASN1_ENCODE_BYTE(hptr, 0);
    }

    header->size = hptr - header->data;
}

static int32_t Asn1InsertValue(struct HksBlob *buf, struct HksAsn1Obj *obj, const struct HksAsn1Blob *tlv)
{
    struct HksAsn1Blob value = *tlv;
    if ((tlv->type != ASN_1_TAG_TYPE_OCT_STR) && (tlv->type != ASN_1_TAG_TYPE_BIT_STR)) {
        while ((value.size > 1) && (value.data[0] == 0)) {
            value.data++;
            value.size--;
        }
    }

    uint8_t headerBuf[ASN_1_MAX_HEADER_LEN] = {0};
    struct HksAsn1Blob header = { 0, ASN_1_MAX_HEADER_LEN, headerBuf };
    BuildTlvHeader(&header, &value);

    uint32_t outSize = header.size + value.size;
    if (buf->size < outSize) {
        HKS_LOG_E("output buffer too small: %u. expected%u", buf->size, outSize);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memmove_s(buf->data + header.size, buf->size - header.size, value.data, value.size) != EOK) {
        HKS_LOG_E("memmove_s failed");
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }
    if (memcpy_s(buf->data, buf->size, header.data, header.size) != EOK) {
        HKS_LOG_E("memcpy head failed");
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }
    buf->data += outSize;
    buf->size -= outSize;
    if (obj != NULL) {
        obj->header.type = tlv->type;
        obj->header.data = buf->data;
        obj->header.size = header.size;
        obj->value.type = tlv->type;
        obj->value.data = buf->data + header.size;
        obj->value.size = value.size;
    }

    return HKS_SUCCESS;
}

int32_t HksAsn1InsertValue(struct HksBlob *buf, struct HksAsn1Obj *obj, const struct HksAsn1Blob *tlv)
{
    if ((CheckBlob(buf) != HKS_SUCCESS) || (CheckAsn1Blob(tlv) != HKS_SUCCESS) || (tlv->size > ASN_1_MAX_SIZE)) {
        HKS_LOG_E("invalid buf or tlv.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (tlv->type == ASN_1_TAG_TYPE_RAW) {
        if (memcpy_s(buf->data, buf->size, tlv->data, tlv->size) != EOK) {
            HKS_LOG_E("copy raw data fail");
            return HKS_ERROR_BUFFER_TOO_SMALL;
        }
        buf->data += tlv->size;
        buf->size -= tlv->size;
        return HKS_SUCCESS;
    }

    return Asn1InsertValue(buf, obj, tlv);
}

int32_t HksAsn1WriteFinal(struct HksBlob *final, const struct HksAsn1Blob *tlv)
{
    if (CheckBlob(final) != HKS_SUCCESS) {
        HKS_LOG_E("invalid asn1 final buf.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    struct HksBlob tmp = { final->size, final->data };
    int32_t ret = HksAsn1InsertValue(&tmp, NULL, tlv);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert value fail\n");
        return ret;
    }
    final->size -= tmp.size;
    return HKS_SUCCESS;
}

static int32_t Asn1GetObj(struct HksBlob *next, struct HksAsn1Obj *obj, const struct HksBlob *data)
{
    uint8_t *buf = data->data;
    uint32_t length = 0;
    obj->header.data = buf;
    HKS_ASN1_DECODE_BYTE(buf, obj->header.type);
    if (buf[0] < ASN_1_MIN_VAL_1_EXTRA_LEN_BYTE) {
        HKS_ASN1_DECODE_BYTE(buf, length);
    } else {
        uint32_t b;
        HKS_ASN1_DECODE_BYTE(buf, b);

        switch (b) {
            case ASN_1_TAG_TYPE_1_BYTE_LEN:
                HKS_ASN1_DECODE_BYTE(buf, length);
                break;
            case ASN_1_TAG_TYPE_2_BYTE_LEN:
                if (data->size < ASN_1_MIN_HEADER_LEN + 1) {
                    HKS_LOG_E("invalid data to decode two bytes.\n");
                    return HKS_ERROR_INSUFFICIENT_DATA;
                }
                HKS_ASN1_DECODE_TWO_BYTE(buf, length);
                break;
            default:
                HKS_LOG_E("Object length does not make sense.\n");
                return HKS_ERROR_INVALID_ARGUMENT;
        }
    }
    obj->header.size = buf - data->data;
    if (length > data->size - obj->header.size) {
        HKS_LOG_E("data buffer is not big enough to hold %u bytes.\n", length);
        return HKS_ERROR_INSUFFICIENT_DATA;
    }

    obj->value.type = obj->header.type;
    obj->value.size = length;
    obj->value.data = buf;
    next->data = data->data + obj->header.size + obj->value.size;
    next->size = data->size - obj->header.size - obj->value.size;
    return HKS_SUCCESS;
}

int32_t HksAsn1ExtractTag(struct HksBlob *next, struct HksAsn1Obj *obj, const struct HksBlob *data,
    uint32_t expectedTag)
{
    if ((next == NULL) || (obj == NULL) || (data == NULL) || (data->size < ASN_1_MIN_HEADER_LEN)) {
        HKS_LOG_E("invalid params");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    int32_t ret = Asn1GetObj(next, obj, data);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get asn1 obj fail.\n");
        return ret;
    }
    if (obj->header.type != expectedTag) {
        HKS_LOG_E("tag %u does not match expected: %u\n", obj->header.type, expectedTag);
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}