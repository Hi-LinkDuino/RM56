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

#ifndef HKS_ATTEST_UTILS_H
#define HKS_ATTEST_UTILS_H

#include "hks_asn1.h"
#include "hks_crypto_hal.h"
#include "hks_type.h"

#define DECLARE_TAG(name, id) \
    static uint8_t name##Tag[] = { 0x06, id##_SIZE, id };

#define DECLARE_OID(name) \
    static const struct HksBlob name##Oid = { sizeof(name##Tag), name##Tag };

enum KmTagType {
    KM_INVALID = 0 << 28,
    KM_ENUM = 1 << 28,
    KM_ENUM_REP = 2 << 28,
    KM_UINT = 3 << 28,
    KM_UINT_REP = 4 << 28,
    KM_ULONG = 5 << 28,
    KM_DATE = 6 << 28,
    KM_BOOL = 7 << 28,
    KM_BIGNUM = 8 << 28,
    KM_BYTES = 9 << 28,
    KM_ULONG_REP = 10 << 28,
};

enum HksCertType {
    HKS_ROOT_CERT = 0,
    HKS_CA_CERT,
    HKS_DEVICE_CERT,
    HKS_DEVICE_KEY,
};

enum KmTag {
    KM_TAG_PRODUCT_MODEL = KM_BYTES | 760,
    KM_TAG_DISPLAY_ID = KM_BYTES | 761,
    KM_TAG_VERSION_INCREMENTAL = KM_BYTES | 762,
    KM_TAG_ATTESTATION_SECURITY_CREDENTIAL = KM_BOOL | 763,
    KM_TAG_ATTESTATION_ID_SEC_LEVEL_INFO = KM_BYTES | 764,
    KM_TAG_ATTESTATION_VERSION_INFO = KM_BYTES | 765,

    KM_TAG_ATTESTATION_ID_UDID = KM_BYTES | 10006,
};

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksInsertClaim (struct HksBlob *out, const struct HksBlob *oid, const struct HksAsn1Blob *value,
    uint32_t secLevel);

int32_t HksGetPublicKey (struct HksBlob *key, const struct HksPubKeyInfo *info, const struct HksUsageSpec *usageSpec);

uint32_t ConvertTag(enum HksTag tag);

#ifdef __cplusplus
}
#endif

#endif