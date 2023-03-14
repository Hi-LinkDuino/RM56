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

#ifndef HKS_ATTEST_H
#define HKS_ATTEST_H

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#endif

#include "hks_asn1.h"
#include "hks_crypto_hal.h"
#include "hks_keyblob.h"

#define HKS_SECURITY_LEVEL_LOW 0
#define HKS_SECURITY_LEVEL_MEDIUM 1
#define HKS_SECURITY_LEVEL_HIGH 2
#define HKS_SECURITY_LEVEL_SUPER 3

#define UTCTIME_LEN 13
#define MONS_PER_YEAR 12
#define ATTEST_CLAIM_BUF_LEN 10240
#define MAX_OID_LEN 20
#define ATT_CERT_HEADER_SIZE 5
#define HKS_HW_ATTESTATION_VERSION 0
#define EXT_MAX_SIZE 512
#define SIG_MAX_SIZE 512
#define VALIDITY_BUF_SIZE (ASN_1_MAX_HEADER_LEN + 2 * (UTCTIME_LEN + 2))
#define PUBKEY_DER_LEN 1024
#define SECOND_TO_MILLI 1000
#define EPOCH_YEAR 1970

#define ATTESTATION_KEY_USAGE_OFFSET 14
#define HKS_APP_ID_SIZE 32
#define HKS_ATTEST_CERT_SIZE 3072
#define HKS_ATTEST_CERT_COUNT 4
#define HKS_ATTEST_CHALLENGE_MIN_SIZE 16
#define HKS_ATTEST_CHALLENGE_MAX_SIZE 128
#define HKS_DECIMAL_TEN 10
#define HKS_DECIMAL_HUNDRED 100
#define HKS_YEAR_DAYS 365
#define HKS_LEAP_YEAR_DAYS 366
#define HKS_SECOND_TO_MINUTE 60
#define HKS_SECOND_TO_HOUR (60 * 60)
#define HKS_SECOND_TO_DAY (60 * 60 * 24)

#define HKS_IMPORT_DEV_CERT_NAME "ecc_cert_0"
#define HKS_IMPORT_CA_CERT_NAME "ecc_cert_1"
#define HKS_IMPORT_ROOT_CERT_NAME "ecc_cert_2"

#define HKS_IMPORT_CERT_PER_BUF_MAX_SIZE 2048

struct DataTime {
    uint32_t seconds;
    uint32_t millis;
    uint32_t min;
    uint32_t hour;
    uint32_t day;
    uint32_t month;
    uint32_t year;
};

struct ValidPeriod {
    uint8_t start[UTCTIME_LEN];
    uint8_t end[UTCTIME_LEN];
};

struct HksAttestTbsSpec {
    struct HksAsn1Obj version;
    struct HksAsn1Obj serial;
    struct HksAsn1Obj signature;
    struct HksAsn1Obj issuer;
    struct HksAsn1Obj validity;
    struct HksAsn1Obj subject;
    struct HksAsn1Obj spki;
    struct HksAsn1Obj extensions;
};

struct HksAttestCert {
    struct HksAttestTbsSpec tbs;
    struct HksAsn1Obj signAlg;
    struct HksAsn1Obj signature;
};

struct HksAttestExt {
    struct HksAsn1Obj seq;
    struct HksAsn1Obj keyUsage;
    struct HksAsn1Obj crl;
    struct HksAsn1Obj claims;
};

struct HksAttestSpec {
    struct HksBlob claimsOid;
    struct HksBlob claims;
    struct HksBlob devCert;
    struct HksBlob devKey;
    struct HksBlob attestKey;
    struct HksUsageSpec usageSpec;
    struct ValidPeriod validity;
};

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t GetYearIndex(uint32_t year)
{
    if ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0))) { /* 4/100/400 check whether it is a leap year */
        return 1;
    } else {
        return 0;
    }
}

static inline uint32_t GetLeapDays(uint32_t year)
{
    return ((year / 4) - (year / 100) + (year / 400)); /* 4/100/400 check whether it is a leap year */
}

static inline bool IsSignPurpose(enum HksKeyPurpose purpose)
{
    return ((((uint32_t)purpose & HKS_KEY_PURPOSE_SIGN) == HKS_KEY_PURPOSE_SIGN) ||
        (((uint32_t)purpose & HKS_KEY_PURPOSE_VERIFY) == HKS_KEY_PURPOSE_VERIFY));
}

static inline bool IsCipherPurpose(enum HksKeyPurpose purpose)
{
    return ((((uint32_t)purpose & HKS_KEY_PURPOSE_ENCRYPT) == HKS_KEY_PURPOSE_ENCRYPT) ||
        (((uint32_t)purpose & HKS_KEY_PURPOSE_DECRYPT) == HKS_KEY_PURPOSE_DECRYPT) ||
        (((uint32_t)purpose & HKS_KEY_PURPOSE_WRAP) == HKS_KEY_PURPOSE_WRAP) ||
        (((uint32_t)purpose & HKS_KEY_PURPOSE_UNWRAP) == HKS_KEY_PURPOSE_UNWRAP));
}

static inline bool IsAgreementPurpose(enum HksKeyPurpose purpose)
{
    return (((uint32_t)purpose & HKS_KEY_PURPOSE_DERIVE) == HKS_KEY_PURPOSE_DERIVE);
}

int32_t HksCreateHwAttestCert(struct HksBlob *attestCert, const struct HksKeyNode *attestKey,
    struct HksBlob *devKey, struct HksAttestSpec *attestSpec);

int32_t HksSoftAttestKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *certChain);

#ifdef __cplusplus
}
#endif

#endif
