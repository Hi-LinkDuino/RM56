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

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#endif

#include "hks_attest.h"
#include "hks_attest_utils.h"
#include "hks_certs_and_key.h"
#include "hks_check_paramset.h"
#include "hks_common_check.h"
#include "hks_core_service.h"
#include "hks_crypto_adapter.h"
#include "hks_crypto_hal.h"
#include "hks_keyblob.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type_inner.h"
#include "sys/time.h"

#define ID_HUKS_BASE            0x2B, 0x06, 0x01, 0x04, 0x01, 0x8F, 0x5B
#define ID_HUKS_BASE_SIZE       0x07

#define ID_HUKS_PRODUCT         ID_HUKS_BASE, 0x02
#define ID_HUKS_PRODUCT_SIZE    (ID_HUKS_BASE_SIZE + 1)

#define ID_HUKS_PKI             ID_HUKS_PRODUCT, 0x82, 0x78
#define ID_HUKS_PKI_SIZE        (ID_HUKS_PRODUCT_SIZE + 2)

#define ID_HUKS_PKI_CERT_EXT        ID_HUKS_PKI, 0x01
#define ID_HUKS_PKI_CERT_EXT_SIZE   (ID_HUKS_PKI_SIZE + 1)

#define ID_HUKS_PKI_DEVICE_SECURITY_LEVEL       ID_HUKS_PKI_CERT_EXT, 0x01
#define ID_HUKS_PKI_DEVICE_SECURITY_LEVEL_SIZE      (ID_HUKS_PKI_CERT_EXT_SIZE + 1)

#define ID_HUKS_PKI_ATTESTATION         ID_HUKS_PKI_CERT_EXT, 0x02
#define ID_HUKS_PKI_ATTESTATION_SIZE        (ID_HUKS_PKI_CERT_EXT_SIZE + 1)

#define ID_HUKS_ATTESTATION_BASE        ID_HUKS_PKI, 0x02
#define ID_HUKS_ATTESTATION_BASE_SIZE       (ID_HUKS_PKI_SIZE + 1)

#define ID_KEY_PROPERTIES           ID_HUKS_ATTESTATION_BASE, 0x01
#define ID_KEY_PROPERTIES_SIZE      (ID_HUKS_ATTESTATION_BASE_SIZE + 1)

#define ID_SYSTEM_PROPERTIES        ID_HUKS_ATTESTATION_BASE, 0x02
#define ID_SYSTEM_PROPERTIES_SIZE       (ID_HUKS_ATTESTATION_BASE_SIZE + 1)

#define ID_SYSTEM_PROPERTIY_OS      ID_SYSTEM_PROPERTIES, 0x02
#define ID_SYSTEM_PROPERTIY_OS_SIZE         (ID_SYSTEM_PROPERTIES_SIZE + 1)

#define ID_SYSTEM_PROPERTIY_OS_VERSION      ID_SYSTEM_PROPERTIY_OS, 0x04
#define ID_SYSTEM_PROPERTIY_OS_VERSION_SIZE     (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksOsVersion, ID_SYSTEM_PROPERTIY_OS_VERSION)
DECLARE_OID(hksOsVersion)

#define ID_SYSTEM_PROPERTIY_OS_SEC_INFO         ID_SYSTEM_PROPERTIY_OS, 0x05
#define ID_SYSTEM_PROPERTIY_OS_SEC_INFO_SIZE        (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksSecInfo, ID_SYSTEM_PROPERTIY_OS_SEC_INFO)
DECLARE_OID(hksSecInfo)

#define ID_PRIVACY_PROPERTIES       ID_SYSTEM_PROPERTIES, 0x04
#define ID_PRIVACY_PROPERTIES_SIZE      (ID_SYSTEM_PROPERTIES_SIZE + 1)

#define ID_PRIVACY_PROPERTIY_IMEI       ID_PRIVACY_PROPERTIES, 0x01
#define ID_PRIVACY_PROPERTIY_IMEI_SIZE      (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksImei, ID_PRIVACY_PROPERTIY_IMEI)
DECLARE_OID(hksImei)

#define ID_PRIVACY_PROPERTIY_MEID       ID_PRIVACY_PROPERTIES, 0x02
#define ID_PRIVACY_PROPERTIY_MEID_SIZE       (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksMeid, ID_PRIVACY_PROPERTIY_MEID)
DECLARE_OID(hksMeid)

#define ID_PRIVACY_PROPERTIY_SN         ID_PRIVACY_PROPERTIES, 0x03
#define ID_PRIVACY_PROPERTIY_SN_SIZE         (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksSn, ID_PRIVACY_PROPERTIY_SN)
DECLARE_OID(hksSn)

#define ID_PRIVACY_PROPERTIY_BRAND      ID_PRIVACY_PROPERTIES, 0x04
#define ID_PRIVACY_PROPERTIY_BRAND_SIZE      (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksBrand, ID_PRIVACY_PROPERTIY_BRAND)
DECLARE_OID(hksBrand)

#define ID_PRIVACY_PROPERTIY_DEVICE      ID_PRIVACY_PROPERTIES, 0x05
#define ID_PRIVACY_PROPERTIY_DEVICE_SIZE      (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksDevice, ID_PRIVACY_PROPERTIY_DEVICE)
DECLARE_OID(hksDevice)

#define ID_PRIVACY_PROPERTIY_PRODUCT      ID_PRIVACY_PROPERTIES, 0x06
#define ID_PRIVACY_PROPERTIY_PRODUCT_SIZE      (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksProduct, ID_PRIVACY_PROPERTIY_PRODUCT)
DECLARE_OID(hksProduct)

#define ID_PRIVACY_PROPERTIY_MANUFACTURER      ID_PRIVACY_PROPERTIES, 0x07
#define ID_PRIVACY_PROPERTIY_MANUFACTURER_SIZE      (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksManufacturer, ID_PRIVACY_PROPERTIY_MANUFACTURER)
DECLARE_OID(hksManufacturer)

#define ID_PRIVACY_PROPERTIY_MODEL      ID_PRIVACY_PROPERTIES, 0x08
#define ID_PRIVACY_PROPERTIY_MODEL_SIZE      (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksModel, ID_PRIVACY_PROPERTIY_MODEL)
DECLARE_OID(hksModel)

#define ID_PRIVACY_PROPERTIY_SOCID      ID_PRIVACY_PROPERTIES, 0x09
#define ID_PRIVACY_PROPERTIY_SOCID_SIZE      (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksSocId, ID_PRIVACY_PROPERTIY_SOCID)
DECLARE_OID(hksSocId)

#define ID_PRIVACY_PROPERTIY_UDID      ID_PRIVACY_PROPERTIES, 0x0A
#define ID_PRIVACY_PROPERTIY_UDID_SIZE      (ID_SYSTEM_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksUdid, ID_PRIVACY_PROPERTIY_UDID)
DECLARE_OID(hksUdid)

#define ID_KEY_PROPERTY_USAGE       ID_KEY_PROPERTIES, 0x01
#define ID_KEY_PROPERTY_USAGE_SIZE      (ID_KEY_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksKeyUsage, ID_KEY_PROPERTY_USAGE)
DECLARE_OID(hksKeyUsage)

#if !defined(WS7200_ROUTER) && !defined(ADAPT_OID_TO_PHONE)
#define ID_KEY_PROPERTY_KEY_ID       ID_KEY_PROPERTIES, 0x02
#define ID_KEY_PROPERTY_KEY_ID_SIZE      (ID_KEY_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksKeyId, ID_KEY_PROPERTY_KEY_ID)
DECLARE_OID(hksKeyId)

#define ID_KEY_PROPERTY_APP_ID       ID_KEY_PROPERTIES, 0x03
#define ID_KEY_PROPERTY_APP_ID_SIZE      (ID_KEY_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksApplicationId, ID_KEY_PROPERTY_APP_ID)
DECLARE_OID(hksApplicationId)

#define ID_KEY_PROPERTY_APP_ID_RAW       ID_KEY_PROPERTY_APP_ID, 0x01
#define ID_KEY_PROPERTY_APP_ID_RAW_SIZE      (ID_KEY_PROPERTY_APP_ID_SIZE + 1)
DECLARE_TAG(hksApplicationIdRaw, ID_KEY_PROPERTY_APP_ID_RAW)
DECLARE_OID(hksApplicationIdRaw)

#define ID_KEY_PROPERTY_CHALLENGE       ID_KEY_PROPERTIES, 0x04
#define ID_KEY_PROPERTY_CHALLENGE_SIZE      (ID_KEY_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksAttestationChallenge, ID_KEY_PROPERTY_CHALLENGE)
DECLARE_OID(hksAttestationChallenge)
#endif

#define ID_KEY_PROPERTY_DIGEST       ID_KEY_PROPERTIES, 0x08
#define ID_KEY_PROPERTY_DIGEST_SIZE      (ID_KEY_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksDigest, ID_KEY_PROPERTY_DIGEST)
DECLARE_OID(hksDigest)

#define ID_KEY_PROPERTY_PADDING       ID_KEY_PROPERTIES, 0x09
#define ID_KEY_PROPERTY_PADDING_SIZE      (ID_KEY_PROPERTIES_SIZE + 1)
DECLARE_TAG(hksPadding, ID_KEY_PROPERTY_PADDING)
DECLARE_OID(hksPadding)

#define ID_HUKS_PKI_OID     ID_HUKS_PKI, 0x01
#define ID_HUKS_PKI_OID_SIZE    (ID_HUKS_PKI_SIZE + 1)

#define ID_HUKS_ATTESTATION_EXTENSION       ID_HUKS_PKI_OID, 0x03
#define ID_HUKS_ATTESTATION_EXTENSION_SIZE       (ID_HUKS_PKI_OID_SIZE + 1)
DECLARE_TAG(hksHuaweiAttestationExtension, ID_HUKS_ATTESTATION_EXTENSION)
DECLARE_OID(hksHuaweiAttestationExtension)

#define ID_KEY_PROPERTY_GROUPS      ID_HUKS_ATTESTATION_BASE, 0x04
#define ID_KEY_PROPERTY_GROUPS_SIZE     (ID_HUKS_ATTESTATION_BASE_SIZE + 1)

#define ID_KEY_PROPERTY_GROUP_SIG       ID_KEY_PROPERTY_GROUPS, 0x01
#define ID_KEY_PROPERTY_GROUP_SIG_SIZE       (ID_KEY_PROPERTY_GROUPS_SIZE + 1)

#define ID_KEY_PROPERTY_GROUP_SIG_RSA       ID_KEY_PROPERTY_GROUP_SIG, 0x01
#define ID_KEY_PROPERTY_GROUP_SIG_RSA_SIZE       (ID_KEY_PROPERTY_GROUP_SIG_SIZE + 1)

#define ID_KEY_PROPERTY_GROUP_SIG_RSA_PKCS1_SHA256       ID_KEY_PROPERTY_GROUP_SIG, 0x02
#define ID_KEY_PROPERTY_GROUP_SIG_RSA_PKCS1_SHA256_SIZE       (ID_KEY_PROPERTY_GROUP_SIG_SIZE + 1)

#define ID_KEY_PROPERTY_GROUP_SIG_RSA_PSS_MGF1_SHA256       ID_KEY_PROPERTY_GROUP_SIG, 0x03
#define ID_KEY_PROPERTY_GROUP_SIG_RSA_PSS_MGF1_SHA256_SIZE       (ID_KEY_PROPERTY_GROUP_SIG_SIZE + 1)
DECLARE_TAG(hksGroupSigRsaPssMgf1Sha256, ID_KEY_PROPERTY_GROUP_SIG_RSA_PSS_MGF1_SHA256)
DECLARE_OID(hksGroupSigRsaPssMgf1Sha256)

#define ID_KEY_PROPERTY_GROUP_SIG_EDDSA       ID_KEY_PROPERTY_GROUP_SIG, 0x04
#define ID_KEY_PROPERTY_GROUP_SIG_EDDSA_SIZE       (ID_KEY_PROPERTY_GROUP_SIG_SIZE + 1)

#define ID_KEY_PROPERTY_GROUP_SIG_ECDSA_SHA256       ID_KEY_PROPERTY_GROUP_SIG, 0x05
#define ID_KEY_PROPERTY_GROUP_SIG_ECDSA_SHA256_SIZE       (ID_KEY_PROPERTY_GROUP_SIG_SIZE + 1)
DECLARE_TAG(hksGroupSigEcdsaSha256, ID_KEY_PROPERTY_GROUP_SIG_ECDSA_SHA256)
DECLARE_OID(hksGroupSigEcdsaSha256)

#define ID_KEY_PROPERTY_GROUP_ENC   ID_KEY_PROPERTY_GROUPS, 0x02
#define ID_KEY_PROPERTY_GROUP_ENC_SIZE      (ID_KEY_PROPERTY_GROUPS_SIZE + 1)

#define ID_KEY_PROPERTY_GROUP_ENC_RSA_PKCS1     ID_KEY_PROPERTY_GROUP_ENC, 0x01
#define ID_KEY_PROPERTY_GROUP_ENC_RSA_PKCS1_SIZE     (ID_KEY_PROPERTY_GROUP_ENC_SIZE + 1)

#define ID_KEY_PROPERTY_GROUP_ENC_RSA_OAEP     ID_KEY_PROPERTY_GROUP_ENC, 0x02
#define ID_KEY_PROPERTY_GROUP_ENC_RSA_OAEP_SIZE     (ID_KEY_PROPERTY_GROUP_ENC_SIZE + 1)
DECLARE_TAG(hksGroupEncRsaOaep, ID_KEY_PROPERTY_GROUP_ENC_RSA_OAEP)
DECLARE_OID(hksGroupEncRsaOaep)

#define ID_KEY_PROPERTY_GROUP_ENC_RSA_OAEP_MGF1_SHA256      ID_KEY_PROPERTY_GROUP_ENC, 0x03
#define ID_KEY_PROPERTY_GROUP_ENC_RSA_OAEP_MGF1_SHA256_SIZE     (ID_KEY_PROPERTY_GROUP_ENC_SIZE + 1)
DECLARE_TAG(hksGroupEncRsaOaepMgf1Sha256, ID_KEY_PROPERTY_GROUP_ENC_RSA_OAEP_MGF1_SHA256)
DECLARE_OID(hksGroupEncRsaOaepMgf1Sha256)

#define ID_KEY_PROPERTY_GROUP_KEY_AGREEMENT     ID_KEY_PROPERTY_GROUPS, 0x03
#define ID_KEY_PROPERTY_GROUP_KEY_AGREEMENT_SIZE        (ID_KEY_PROPERTY_GROUPS_SIZE + 1)

#define ID_KEY_PROPERTY_GROUP_KEY_AGREEMENT_HKDF_SHA256     ID_KEY_PROPERTY_GROUP_KEY_AGREEMENT, 0x01
#define ID_KEY_PROPERTY_GROUP_KEY_AGREEMENT_HKDF_SHA256_SIZE        (ID_KEY_PROPERTY_GROUP_KEY_AGREEMENT_SIZE + 1)

static uint8_t g_rsaSha256Tag[] = { 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0B };
DECLARE_OID(g_rsaSha256)

static uint8_t g_ecdsaSha256Tag[] = { 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02 };
DECLARE_OID(g_ecdsaSha256)

static const uint8_t g_attestTbsRsa[] = {
    0x30, 0x82, 0x01, 0xc7, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x01, 0x01, 0x30, 0x0b, 0x06, 0x09,
    0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x30, 0x1d, 0x31, 0x1b, 0x30, 0x19, 0x06,
    0x03, 0x55, 0x04, 0x03, 0x13, 0x12, 0x48, 0x75, 0x61, 0x77, 0x65, 0x69, 0x20, 0x4b, 0x65, 0x79,
    0x53, 0x74, 0x6f, 0x72, 0x65, 0x20, 0x20, 0x20, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x36, 0x30, 0x36,
    0x30, 0x34, 0x31, 0x38, 0x32, 0x38, 0x34, 0x34, 0x5a, 0x17, 0x0d, 0x32, 0x36, 0x30, 0x36, 0x30,
    0x34, 0x31, 0x38, 0x32, 0x38, 0x34, 0x34, 0x5a, 0x30, 0x1a, 0x31, 0x18, 0x30, 0x16, 0x06, 0x03,
    0x55, 0x04, 0x03, 0x13, 0x0f, 0x41, 0x20, 0x4b, 0x65, 0x79, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72,
    0x20, 0x4b, 0x65, 0x79, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
    0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a,
    0x02, 0x82, 0x01, 0x01, 0x00, 0xe8, 0xd8, 0xf9, 0x14, 0x5f, 0x88, 0xf7, 0x36, 0x63, 0xcb, 0x28,
    0x0c, 0x18, 0xf7, 0xc7, 0x3f, 0x0d, 0xa0, 0x73, 0x9a, 0x9c, 0xe0, 0x3c, 0xc6, 0x79, 0x5a, 0xd4,
    0xa3, 0xd7, 0x72, 0x2f, 0x46, 0x42, 0xe1, 0x3b, 0xc1, 0xc8, 0xf3, 0xbc, 0x51, 0xd3, 0x5d, 0x8d,
    0xc4, 0x18, 0x03, 0x92, 0x26, 0x5a, 0xd7, 0x92, 0xbb, 0x1e, 0x4f, 0xc0, 0x71, 0x51, 0x75, 0xc2,
    0x41, 0x31, 0xb9, 0xd2, 0xc7, 0xf2, 0x9c, 0x03, 0x6c, 0xff, 0x77, 0x75, 0x17, 0x82, 0x5b, 0x3c,
    0x05, 0x0d, 0x1c, 0x82, 0x1c, 0xa3, 0xd5, 0x25, 0xd9, 0x31, 0x0f, 0x2d, 0x2b, 0xf1, 0x82, 0xd2,
    0x2a, 0x0a, 0xdc, 0xe1, 0x0c, 0xcc, 0x8b, 0xc1, 0xd1, 0x9e, 0x20, 0xaf, 0x00, 0x2a, 0xcb, 0x7a,
    0x3c, 0xcb, 0x8f, 0x6d, 0xb0, 0x52, 0xc3, 0x3b, 0x17, 0x85, 0x56, 0xe7, 0x45, 0xb6, 0x1e, 0x3a,
    0x42, 0xb9, 0x38, 0xdb, 0xf7, 0x7b, 0x0f, 0x73, 0x37, 0x1b, 0xf4, 0x20, 0xcb, 0x85, 0xbf, 0xdc,
    0xb4, 0x2f, 0x3d, 0x77, 0x27, 0x31, 0x53, 0x31, 0xb4, 0x71, 0x72, 0x3a, 0x47, 0xcd, 0x98, 0xcf,
    0xf0, 0x34, 0x5d, 0x90, 0x1d, 0x71, 0xba, 0x19, 0x7d, 0xf6, 0xe9, 0xdc, 0xe6, 0xf9, 0x67, 0xf4,
    0x1c, 0x93, 0x7d, 0x10, 0xfd, 0x3a, 0x58, 0x71, 0xc2, 0xf5, 0x3d, 0x45, 0xca, 0xcf, 0xf9, 0x1b,
    0x6c, 0x27, 0x79, 0x5f, 0xcd, 0xf2, 0x4f, 0xa7, 0xa2, 0x91, 0x9f, 0xd1, 0x8b, 0xbb, 0x3b, 0x4c,
    0x36, 0x40, 0x2e, 0x73, 0xf6, 0xd9, 0xb8, 0xdf, 0x21, 0x6e, 0xcd, 0xae, 0x6b, 0x43, 0xb2, 0x99,
    0xea, 0x9e, 0xdd, 0x3d, 0x4c, 0xc8, 0x0a, 0xf4, 0x5a, 0xaa, 0x66, 0x24, 0x98, 0xce, 0xfd, 0xb1,
    0xfb, 0x16, 0x94, 0xd8, 0x87, 0xa2, 0x08, 0xc4, 0x55, 0x95, 0xf5, 0x95, 0xcd, 0x75, 0x03, 0xc9,
    0x99, 0x8a, 0x84, 0xe4, 0x57, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x2f, 0x30, 0x2d, 0x30, 0x0b,
    0x06, 0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03, 0x02, 0x07, 0x80, 0x30, 0x08, 0x06, 0x03, 0x55,
    0x1d, 0x1f, 0x04, 0x01, 0x00, 0x30, 0x14, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0xd6, 0x79,
    0x02, 0x01, 0x11, 0x04, 0x06, 0x30, 0x04, 0x04, 0x02, 0x0b, 0x0c
};

static const uint8_t g_attestTbs[] = {
    0x30, 0x81, 0xf6, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x01, 0x01, 0x30, 0x0a, 0x06, 0x08, 0x2a,
    0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30, 0x1d, 0x31, 0x1b, 0x30, 0x19, 0x06, 0x03, 0x55,
    0x04, 0x03, 0x13, 0x12, 0x41, 0x6e, 0x64, 0x72, 0x6f, 0x69, 0x64, 0x20, 0x4b, 0x65, 0x79, 0x6d,
    0x61, 0x73, 0x74, 0x65, 0x72, 0x32, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x36, 0x30, 0x36, 0x30, 0x31,
    0x31, 0x32, 0x31, 0x37, 0x32, 0x31, 0x5a, 0x17, 0x0d, 0x32, 0x36, 0x30, 0x36, 0x30, 0x31, 0x31,
    0x32, 0x31, 0x37, 0x32, 0x31, 0x5a, 0x30, 0x1a, 0x31, 0x18, 0x30, 0x16, 0x06, 0x03, 0x55, 0x04,
    0x03, 0x13, 0x0f, 0x41, 0x20, 0x4b, 0x65, 0x79, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x20, 0x4b,
    0x65, 0x79, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06,
    0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0xb0, 0x51, 0x4c,
    0x4c, 0x13, 0x54, 0x0f, 0x23, 0x97, 0xf5, 0x47, 0x19, 0xf3, 0x33, 0x2e, 0x7e, 0xf3, 0x8e, 0x42,
    0xad, 0x9d, 0xc0, 0x8b, 0x71, 0xe6, 0x60, 0xce, 0x16, 0xb9, 0xe8, 0x8d, 0x09, 0x4e, 0x7f, 0x3a,
    0xdc, 0x88, 0x8e, 0x94, 0x4d, 0x45, 0xd5, 0xe4, 0x59, 0x4e, 0x3f, 0xbe, 0x28, 0x91, 0x80, 0xbb,
    0x1c, 0xd7, 0xfc, 0x55, 0xd1, 0xb5, 0xc7, 0xcb, 0x50, 0x1c, 0x09, 0xca, 0x50, 0xa3, 0x2a, 0x30,
    0x28, 0x30, 0x0b, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03, 0x02, 0x07, 0x80, 0x30, 0x07,
    0x06, 0x03, 0x55, 0x1d, 0x1f, 0x04, 0x00, 0x30, 0x10, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01,
    0xd6, 0x79, 0x02, 0x01, 0x11, 0x04, 0x02, 0x30, 0x00
};

static const uint8_t g_attestExtTmpl[] = {
    0x30, 0x2c, 0x30, 0x0b, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03, 0x02, 0x07,
    0x80, 0x30, 0x08, 0x06, 0x03, 0x55, 0x1d, 0x1f, 0x04, 0x01, 0x00, 0x30, 0x13, 0x06, 0x0a, 0x2b,
    0x06, 0x01, 0x04, 0x01, 0xd6, 0x79, 0x02, 0x01, 0x11, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff
};

static const uint32_t g_monthLengths[2][MONS_PER_YEAR] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const uint32_t g_yearLengths[2] = {
    HKS_YEAR_DAYS, HKS_LEAP_YEAR_DAYS
};

static enum HksTag g_idAttestList[] = {
    HKS_TAG_ATTESTATION_ID_BRAND,
    HKS_TAG_ATTESTATION_ID_DEVICE,
    HKS_TAG_ATTESTATION_ID_PRODUCT,
    HKS_TAG_ATTESTATION_ID_SERIAL,
    HKS_TAG_ATTESTATION_ID_IMEI,
    HKS_TAG_ATTESTATION_ID_MEID,
    HKS_TAG_ATTESTATION_ID_MANUFACTURER,
    HKS_TAG_ATTESTATION_ID_MODEL,
    HKS_TAG_ATTESTATION_ID_SOCID,
    HKS_TAG_ATTESTATION_ID_UDID,
    HKS_TAG_ATTESTATION_ID_SEC_LEVEL_INFO,
    HKS_TAG_ATTESTATION_ID_VERSION_INFO,
};

static void GetTimeStampTee(uint8_t *timeStamp, const struct DataTime *time)
{
    int i = 0;
    uint32_t year = time->year - ((time->year / HKS_DECIMAL_HUNDRED) * HKS_DECIMAL_HUNDRED);
    timeStamp[i++] = (year / HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (year % HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (time->month / HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (time->month % HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (time->day / HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (time->day % HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (time->hour / HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (time->hour % HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (time->min / HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (time->min % HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (time->seconds / HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = (time->seconds % HKS_DECIMAL_TEN) + '0';
    timeStamp[i++] = 'Z';
}

static void GenerateSysDateTime(const uint32_t rtcTime, struct DataTime *time)
{
    uint32_t seconds = rtcTime;
    uint32_t tDays = seconds / HKS_SECOND_TO_DAY;
    uint32_t remainSec = seconds - tDays * HKS_SECOND_TO_DAY;
    uint32_t year = EPOCH_YEAR;
    const uint32_t *ip = NULL;

    while (tDays >= g_yearLengths[GetYearIndex(year)]) {
        uint32_t carryOver = tDays / HKS_LEAP_YEAR_DAYS;
        if (carryOver == 0) {
            carryOver = 1;
        }
        uint32_t newYear = year + carryOver;
        uint32_t leapDays = GetLeapDays(newYear - 1) - GetLeapDays(year - 1);
        tDays -= (newYear - year) * HKS_YEAR_DAYS;
        tDays -= leapDays;
        year = newYear;
    }

    uint32_t iDays = tDays;
    time->year = year;
    time->hour = remainSec / HKS_SECOND_TO_HOUR;
    remainSec %= HKS_SECOND_TO_HOUR;
    time->min = remainSec / HKS_SECOND_TO_MINUTE;
    time->seconds = remainSec % HKS_SECOND_TO_MINUTE;
    ip = g_monthLengths[GetYearIndex(year)];
    for (time->month = 0; iDays >= ip[time->month]; ++(time->month)) {
        iDays -= ip[time->month];
    }
    ++time->month;
    time->day = iDays + 1;
}

static void AddYears(uint8_t *end, const uint8_t *start, uint32_t years)
{
    if (memmove_s(end, UTCTIME_LEN, start, UTCTIME_LEN) != EOK) {
        HKS_LOG_E("memmove_s failed.");
        return;
    }
    uint32_t tens = start[0] - '0';
    uint32_t units = start[1] - '0';
    units += years;
    tens += units / 10; /* 10 is base */
    units %= 10; /* 10 is base */
    end[0] = tens + '0';
    end[1] = units + '0';
}

static void SetAttestCertValid(struct ValidPeriod *valid)
{
    uint64_t activeDateTime;
    struct timeval curTime = {0};

    int ret = gettimeofday(&curTime, NULL);
    if (ret < 0) {
        HKS_LOG_E("Unable to get system UTC time stamp\n");
        return;
    }

    uint64_t curTimeValue = (uint64_t)curTime.tv_sec;
    curTimeValue = curTimeValue * SECOND_TO_MILLI + ((uint64_t)curTime.tv_usec) / SECOND_TO_MILLI;
    activeDateTime = curTimeValue;
    HKS_LOG_I("set active dateTime to default : %llu\n", activeDateTime);

    struct DataTime notBefore = {0};
    uint64_t tmpSec = (activeDateTime >> 10); /* 10 is uesed for uint64 dividing 1000 in 32 bit system. */
    tmpSec = tmpSec + ((3 * tmpSec) >> 7) + ((9 * tmpSec) >> 14); /* 3/7/9/14 are same with 10 */
    tmpSec = tmpSec + (uint32_t)(activeDateTime - tmpSec * SECOND_TO_MILLI) / SECOND_TO_MILLI;
    GenerateSysDateTime((uint32_t)tmpSec, &notBefore);
    HKS_LOG_I("notBefore:%u%u%u %u:%u:%uZ\n", notBefore.year, notBefore.month, notBefore.day, notBefore.hour,
        notBefore.min, notBefore.seconds);
    GetTimeStampTee(valid->start, &notBefore);

    HKS_LOG_I("set expired date to default.\n");
    AddYears(valid->end, valid->start, 10); /* default set to 10 years after current time */
}

static int32_t IsValidTlv(const struct HksAsn1Obj obj)
{
    uint32_t length = 0;
    if ((obj.value.data != NULL)  && (obj.header.data != NULL)) {
        length = obj.value.size + obj.header.size;
    } else {
        HKS_LOG_E("value or header is NULL");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (length < ASN_1_MIN_HEADER_LEN) {
        HKS_LOG_E("len %u < %u.", length, ASN_1_MIN_HEADER_LEN);
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

static int32_t IsValidAttestTbs(struct HksAttestTbsSpec *tbsSpec)
{
    if (IsValidTlv(tbsSpec->version)) {
        HKS_LOG_E("invalid version.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (IsValidTlv(tbsSpec->serial)) {
        HKS_LOG_E("invalid serial.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (IsValidTlv(tbsSpec->signature)) {
        HKS_LOG_E("invalid signature.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (IsValidTlv(tbsSpec->issuer)) {
        HKS_LOG_E("invalid issuer.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (IsValidTlv(tbsSpec->validity)) {
        HKS_LOG_E("invalid validity.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (IsValidTlv(tbsSpec->subject)) {
        HKS_LOG_E("invalid subject.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (IsValidTlv(tbsSpec->spki)) {
        HKS_LOG_E("invalid spki.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (IsValidTlv(tbsSpec->extensions)) {
        HKS_LOG_E("invalid extensions.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

static int32_t IsValidAttestCert(struct HksAttestCert *cert)
{
    if (IsValidAttestTbs(&cert->tbs)) {
        HKS_LOG_E("invalid AttestTbs.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (IsValidTlv(cert->signAlg)) {
        HKS_LOG_E("invalid signAlg.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (IsValidTlv(cert->signature)) {
        HKS_LOG_E("invalid signature.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if ((cert->signAlg.value.size != cert->tbs.signature.value.size) ||
        (HksMemCmp(cert->signAlg.value.data, cert->tbs.signature.value.data, cert->signAlg.value.size) != 0)) {
        HKS_LOG_E("algorithm identifiers not match.\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

static void ParseAttestTbs(const struct HksBlob *template, struct HksAttestTbsSpec *tbsSpec)
{
    struct HksAsn1Obj obj = {0};
    struct HksBlob skip = { 0, NULL };
    int32_t ret = HksAsn1ExtractTag(&skip, &obj, template, ASN_1_TAG_TYPE_SEQ);
    struct HksBlob val = { obj.value.size, obj.value.data };

    ret += HksAsn1ExtractTag(&val, &tbsSpec->version, &val, ASN_1_TAG_TYPE_CTX_SPEC0);
    ret += HksAsn1ExtractTag(&val, &tbsSpec->serial, &val, ASN_1_TAG_TYPE_INT);
    ret += HksAsn1ExtractTag(&val, &tbsSpec->signature, &val, ASN_1_TAG_TYPE_SEQ);
    ret += HksAsn1ExtractTag(&val, &tbsSpec->issuer, &val, ASN_1_TAG_TYPE_SEQ);
    ret += HksAsn1ExtractTag(&val, &tbsSpec->validity, &val, ASN_1_TAG_TYPE_SEQ);
    ret += HksAsn1ExtractTag(&val, &tbsSpec->subject, &val, ASN_1_TAG_TYPE_SEQ);
    ret += HksAsn1ExtractTag(&val, &tbsSpec->spki, &val, ASN_1_TAG_TYPE_SEQ);
    ret += HksAsn1ExtractTag(&val, &tbsSpec->extensions, &val, ASN_1_TAG_TYPE_CTX_SPEC3);
    ret += IsValidAttestTbs(tbsSpec);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("invalid tbs.\n");
    }
}

static void ParseAttestCert(const struct HksBlob *devCert, struct HksAttestCert *cert)
{
    struct HksAsn1Obj obj = {0};
    struct HksBlob next = { 0, NULL };

    int32_t ret = HksAsn1ExtractTag(&next, &obj, devCert, ASN_1_TAG_TYPE_SEQ);
    struct HksBlob val = { obj.value.size, obj.value.data };
    ParseAttestTbs(&val, &cert->tbs);
    struct HksAsn1Obj skip = {0};
    ret += HksAsn1ExtractTag(&val, &skip, &val, ASN_1_TAG_TYPE_SEQ);
    ret += HksAsn1ExtractTag(&val, &cert->signAlg, &val, ASN_1_TAG_TYPE_SEQ);
    ret += HksAsn1ExtractTag(&val, &cert->signature, &val, ASN_1_TAG_TYPE_BIT_STR);

    ret += IsValidAttestCert(cert);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("invalid dev cert.\n");
    }
}

static void ParseAttestExtension(const struct HksBlob *data, struct HksAttestExt *ext)
{
    struct HksBlob val = *data;
    int32_t ret = HksAsn1ExtractTag(&val, &ext->seq, &val, ASN_1_TAG_TYPE_SEQ);
    val.data = ext->seq.value.data;
    val.size = ext->seq.value.size;

    ret += HksAsn1ExtractTag(&val, &ext->keyUsage, &val, ASN_1_TAG_TYPE_SEQ);
    ret += HksAsn1ExtractTag(&val, &ext->crl, &val, ASN_1_TAG_TYPE_SEQ);
    ret += HksAsn1ExtractTag(&val, &ext->claims, &val, ASN_1_TAG_TYPE_SEQ);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("invalid extension.\n");
    }
}

static uint32_t EncodeUtcTime(const uint8_t *time, uint8_t *buf)
{
    uint8_t *tmp = buf;
    tmp[0] = (uint8_t)ASN_1_TAG_TYPE_UTC_TIME;
    tmp++;
    tmp[0] = (uint8_t)UTCTIME_LEN;
    tmp++;
    if (memcpy_s(tmp, UTCTIME_LEN, time, UTCTIME_LEN) != EOK) {
        HKS_LOG_E("memcpy_s fail.");
        return 0;
    }
    tmp += UTCTIME_LEN;
    return tmp - buf;
}

static void EncodeValidity(const struct ValidPeriod *validity, struct HksBlob *out)
{
    uint8_t *start = out->data + ASN_1_MAX_HEADER_LEN;
    uint8_t *p = start;
    p += EncodeUtcTime(validity->start, p);
    p += EncodeUtcTime(validity->end, p);

    struct HksAsn1Blob insertValid = { ASN_1_TAG_TYPE_SEQ, (p - start), start };
    int32_t ret = HksAsn1WriteFinal(out, &insertValid);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("encode validity fail.\n");
    }
}

static uint8_t EncodeKeyUsageBits(const uint32_t usage, uint8_t *bits)
{
    uint8_t v = 0;
    uint8_t unused = 8; /* one byte haa 8 bits, so init to 8 */
    if (IsSignPurpose((enum HksKeyPurpose)usage)) {
        v |= 0x80;
        unused = 7; /* 7 bits are unused */
    }
    if (IsCipherPurpose((enum HksKeyPurpose)usage)) {
        v |= 0x20;
        unused = 5; /* 5 bits are unused */
    }
    if (IsAgreementPurpose((enum HksKeyPurpose)usage)) {
        v |= 0x08;
        unused = 3; /* 3 bits are unused */
    }
    *bits = v;
    return unused;
}

static int32_t EncodeClaims(const struct HksBlob *oid, const struct HksBlob *claim, struct HksBlob *seq)
{
    if (memcpy_s(seq->data, seq->size, oid->data, oid->size) != EOK) {
        HKS_LOG_E("copy claim oid failed!");
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }
    uint32_t size = oid->size;
    struct HksBlob tmp = *seq;
    tmp.data += size;
    tmp.size -= size;

    struct HksAsn1Blob value = { ASN_1_TAG_TYPE_OCT_STR, claim->size, claim->data };
    int32_t ret = HksAsn1WriteFinal(&tmp, &value);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("write claim oct str failed!");
        return HKS_ERROR_BAD_STATE;
    }
    size += tmp.size;

    value.type = ASN_1_TAG_TYPE_SEQ;
    value.data = seq->data;
    value.size = size;
    return HksAsn1WriteFinal(seq, &value);
}

static int32_t CreateAttestExtension(const struct HksAttestSpec *attestSpec, struct HksBlob *extension)
{
    struct HksAttestExt tmplExt = {0};
    struct HksBlob extensionTmpl = { sizeof(g_attestExtTmpl), (uint8_t *)g_attestExtTmpl };
    ParseAttestExtension(&extensionTmpl, &tmplExt);
    if (memcpy_s(extension->data, extension->size, extensionTmpl.data, extensionTmpl.size) != EOK) {
        HKS_LOG_E("copy extension failed!");
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }

    uint8_t usage = 0;
    uint8_t unusedBits = EncodeKeyUsageBits(attestSpec->usageSpec.purpose, &usage);
    extension->data[ATTESTATION_KEY_USAGE_OFFSET] = usage;
    extension->data[ATTESTATION_KEY_USAGE_OFFSET - 1] = unusedBits;

    uint32_t claimsOffset = tmplExt.claims.header.data - extensionTmpl.data;
    struct HksBlob tmp = *extension;
    tmp.data += claimsOffset;
    tmp.size -= claimsOffset;

    int32_t ret = EncodeClaims(&attestSpec->claimsOid, &attestSpec->claims, &tmp);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("EncodeClaims failed!");
        return ret;
    }

    uint32_t usageOffset = tmplExt.keyUsage.header.data - extensionTmpl.data;
    struct HksAsn1Blob seqValue = {
        .type = ASN_1_TAG_TYPE_SEQ,
        .size = claimsOffset + tmp.size - usageOffset,
        .data = extension->data + usageOffset
    };
    return HksAsn1WriteFinal(extension, &seqValue);
}

static int32_t EncodeTbs(const struct HksAttestTbsSpec *tbs, struct HksBlob *out)
{
    struct HksBlob tmp = *out;
    tmp.data += ATT_CERT_HEADER_SIZE;
    tmp.size -= ATT_CERT_HEADER_SIZE;

    int32_t ret = HksAsn1InsertValue(&tmp, NULL, &tbs->version.value);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert version failed!");
        return HKS_ERROR_BAD_STATE;
    }
    ret = HksAsn1InsertValue(&tmp, NULL, &tbs->serial.value);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert serial failed!");
        return HKS_ERROR_BAD_STATE;
    }
    ret = HksAsn1InsertValue(&tmp, NULL, &tbs->signature.value);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert signature failed!");
        return HKS_ERROR_BAD_STATE;
    }
    ret = HksAsn1InsertValue(&tmp, NULL, &tbs->issuer.value);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert issuer failed!");
        return HKS_ERROR_BAD_STATE;
    }
    ret = HksAsn1InsertValue(&tmp, NULL, &tbs->validity.value);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert validity failed!");
        return HKS_ERROR_BAD_STATE;
    }
    ret = HksAsn1InsertValue(&tmp, NULL, &tbs->subject.value);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert subject failed!");
        return HKS_ERROR_BAD_STATE;
    }
    ret = HksAsn1InsertValue(&tmp, NULL, &tbs->spki.value);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert spki failed!");
        return HKS_ERROR_BAD_STATE;
    }
    ret = HksAsn1InsertValue(&tmp, NULL, &tbs->extensions.value);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert extensions failed!");
        return ret;
    }

    uint8_t *data = out->data + ATT_CERT_HEADER_SIZE;
    uint32_t size = tmp.data - data;
    struct HksAsn1Blob seq = { ASN_1_TAG_TYPE_SEQ, size, data };
    return HksAsn1WriteFinal(out, &seq);
}

static void GetSignatureByAlg(uint32_t signAlg, struct HksAsn1Blob *sigature)
{
    if (signAlg == HKS_ALG_RSA) {
        sigature->size = g_rsaSha256Oid.size;
        sigature->data = g_rsaSha256Oid.data;
    } else {
        sigature->size = g_ecdsaSha256Oid.size;
        sigature->data = g_ecdsaSha256Oid.data;
    }
}
static int32_t CreateTbs(const struct HksBlob *template, const struct HksAttestSpec *attestSpec,
    struct HksBlob *tbs, uint32_t signAlg)
{
    struct HksAttestTbsSpec draftTbs = {0};
    ParseAttestTbs(template, &draftTbs);
    struct HksAsn1Blob sigature = { ASN_1_TAG_TYPE_SEQ, 0, NULL };
    GetSignatureByAlg(signAlg, &sigature);
    draftTbs.signature.value = sigature;

    uint8_t validityBuf[VALIDITY_BUF_SIZE] = {0};
    struct HksBlob validity = { sizeof(validityBuf), validityBuf };
    EncodeValidity(&attestSpec->validity, &validity);
    struct HksAsn1Blob validBlob = { ASN_1_TAG_TYPE_RAW, validity.size, validity.data };
    draftTbs.validity.value = validBlob;

    struct HksAttestCert devCert = {0};
    ParseAttestCert(&attestSpec->devCert, &devCert);
    draftTbs.issuer = devCert.tbs.subject;

    uint8_t pubKey[PUBKEY_DER_LEN] = {0};
    struct HksBlob pubKeyBlob = { PUBKEY_DER_LEN, pubKey };
    int32_t ret = HksCryptoHalGetPubKey(&attestSpec->attestKey, &pubKeyBlob);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get attest public key failed!");
        return ret;
    }
    uint8_t pubKeyDer[PUBKEY_DER_LEN] = {0};
    struct HksBlob spkiBlob = {PUBKEY_DER_LEN, pubKeyDer };
    ret = HksGetPublicKey(&spkiBlob, (struct HksPubKeyInfo *)pubKeyBlob.data, &attestSpec->usageSpec);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("der public key failed!");
        return ret;
    }
    struct HksAsn1Blob insertSpki = { ASN_1_TAG_TYPE_RAW, spkiBlob.size, spkiBlob.data };
    draftTbs.spki.value = insertSpki;

    uint8_t *extBuf = HksMalloc(EXT_MAX_SIZE + attestSpec->claims.size);
    if (extBuf == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    struct HksBlob extension = { EXT_MAX_SIZE + attestSpec->claims.size, extBuf };
    ret = CreateAttestExtension(attestSpec, &extension);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("create extensions failed!");
        HksFree(extBuf);
        return ret;
    }

    struct HksAsn1Blob extBlob = { ASN_1_TAG_TYPE_CTX_SPEC3, extension.size, extension.data };
    draftTbs.extensions.value = extBlob;
    ret = EncodeTbs(&draftTbs, tbs);
    HksFree(extBuf);
    return ret;
}

static int32_t GetPrivateKeyMaterial(struct HksBlob *val, struct HksBlob *material)
{
    struct HksAsn1Obj obj = {0};
    struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)material->data;

    keyMaterial->keyAlg = HKS_ALG_RSA;
    keyMaterial->keySize = HKS_RSA_KEY_SIZE_2048;

    uint32_t offset = sizeof(struct KeyMaterialRsa);
    int32_t ret = HksAsn1ExtractTag(val, &obj, val, ASN_1_TAG_TYPE_INT);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("extract n fail!");
        return ret;
    }
    if (memcpy_s(material->data + offset, material->size - offset, obj.value.data, obj.value.size) != EOK) {
        HKS_LOG_E("copy n fail!");
        return HKS_ERROR_BAD_STATE;
    }
    keyMaterial->nSize = obj.value.size;

    offset += keyMaterial->nSize;
    ret = HksAsn1ExtractTag(val, &obj, val, ASN_1_TAG_TYPE_INT);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("extract e fail!");
        return ret;
    }
    if (memcpy_s(material->data + offset, material->size - offset, obj.value.data, obj.value.size) != EOK) {
        HKS_LOG_E("copy e fail!");
        return HKS_ERROR_BAD_STATE;
    }
    keyMaterial->eSize = obj.value.size;

    offset += keyMaterial->eSize;
    ret = HksAsn1ExtractTag(val, &obj, val, ASN_1_TAG_TYPE_INT);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("extract d fail!");
        return ret;
    }
    if (memcpy_s(material->data + offset, material->size - offset, obj.value.data, obj.value.size) != EOK) {
        HKS_LOG_E("copy d fail!");
        return HKS_ERROR_BAD_STATE;
    }
    keyMaterial->dSize = obj.value.size;
    material->size = offset + keyMaterial->dSize;
    return HKS_SUCCESS;
}

static int32_t StepIntoPrivateKey(const struct HksBlob *key, struct HksBlob *val)
{
    struct HksAsn1Obj obj = {0};
    struct HksBlob skip = { 0, NULL };
    int32_t ret;

    ret = HksAsn1ExtractTag(&skip, &obj, key, ASN_1_TAG_TYPE_SEQ);
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    val->data = obj.value.data;
    val->size = obj.value.size;

    ret = HksAsn1ExtractTag(val, &obj, val, ASN_1_TAG_TYPE_INT);
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    return HKS_SUCCESS;
}

int32_t HksGetDevicePrivateKey(const struct HksBlob *key, struct HksBlob *out)
{
    struct HksBlob val = { 0, NULL };
    int32_t ret = StepIntoPrivateKey(key, &val);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("step into private key fail!");
        return ret;
    }

    uint32_t keySize = HKS_RSA_KEY_SIZE_2048;
    uint32_t materialLen = sizeof(struct KeyMaterialRsa) + keySize / HKS_BITS_PER_BYTE * 3; /* 3 bits are unused */
    uint8_t *material = (uint8_t *)HksMalloc(materialLen);
    if (material == NULL) {
        HKS_LOG_E("malloc key materail fail!");
        return HKS_ERROR_MALLOC_FAIL;
    }
    struct HksBlob materialBlob = { materialLen, material };
    ret = GetPrivateKeyMaterial(&val, &materialBlob);
    if (ret != HKS_SUCCESS) {
        (void)memset_s(material, materialLen, 0, materialLen);
        HksFree(material);
        HKS_LOG_E("get key materail fail!");
        return ret;
    }

    out->data = materialBlob.data;
    out->size = materialBlob.size;
    HKS_LOG_I("prikey size %x", out->size);

    return HKS_SUCCESS;
}

static int32_t SignTbs(struct HksBlob *sig, const struct HksBlob *tbs, const struct HksBlob *key, uint32_t signAlg)
{
    (void)key;
    uint8_t buffer[HKS_DIGEST_SHA256_LEN] = {0};
    struct HksBlob message = { HKS_DIGEST_SHA256_LEN, buffer };
    int32_t ret = HksCryptoHalHash(HKS_DIGEST_SHA256, tbs, &message);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("calc hash of tbs failed!");
        return ret;
    }

    struct HksUsageSpec usageSpec = {0};
    usageSpec.digest = HKS_DIGEST_SHA256;
    if (signAlg == HKS_ALG_RSA) {
        usageSpec.padding = HKS_PADDING_PKCS1_V1_5;
        usageSpec.algType = HKS_ALG_RSA;
    } else {
        usageSpec.padding = HKS_PADDING_NONE;
        usageSpec.algType = HKS_ALG_ECC;
    }

    struct HksBlob priKey = { 0, NULL };
    ret = HksGetDevicePrivateKey(key, &priKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get private key failed!");
        return ret;
    }

    ret = HksCryptoHalSign(&priKey, &usageSpec, &message, sig);
    (void)memset_s(priKey.data, priKey.size, 0, priKey.size);
    HksFree(priKey.data);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("sign tbs failed!");
        return ret;
    }
    return HKS_SUCCESS;
}

static int32_t CreateAttestCert(struct HksBlob *attestCert, struct HksBlob *template,
    const struct HksAttestSpec *attestSpec, uint32_t signAlg)
{
    struct HksBlob tbs = *attestCert;
    tbs.data += ATT_CERT_HEADER_SIZE;
    tbs.size -= ATT_CERT_HEADER_SIZE;
    int32_t ret = CreateTbs(template, attestSpec, &tbs, signAlg);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("CreateTbs failed!");
        return HKS_ERROR_BAD_STATE;
    }

    uint8_t sigBuf[SIG_MAX_SIZE] = {0};
    struct HksBlob signature = { sizeof(sigBuf), sigBuf };
    ret = SignTbs(&signature, &tbs, &attestSpec->devKey, signAlg);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("SignTbs failed!");
        return HKS_ERROR_BAD_STATE;
    }

    uint32_t certSize = tbs.size;
    struct HksAsn1Obj obj = {0};
    struct HksBlob tmp = *attestCert;
    tmp.data += ATT_CERT_HEADER_SIZE + tbs.size;
    tmp.size -= ATT_CERT_HEADER_SIZE + tbs.size;
    struct HksAsn1Blob signOidBlob = { ASN_1_TAG_TYPE_SEQ, g_rsaSha256Oid.size, g_rsaSha256Oid.data };
    if (signAlg == HKS_ALG_ECC) {
        signOidBlob.size = g_ecdsaSha256Oid.size;
        signOidBlob.data = g_ecdsaSha256Oid.data;
    }
    ret = HksAsn1InsertValue(&tmp, &obj, &signOidBlob);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert sign oid failed!");
        return HKS_ERROR_BAD_STATE;
    }
    certSize += obj.header.size + obj.value.size;

    struct HksAsn1Blob sigBlob = { ASN_1_TAG_TYPE_BIT_STR, signature.size, signature.data };
    ret = HksAsn1InsertValue(&tmp, &obj, &sigBlob);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert sign failed!");
        return HKS_ERROR_BAD_STATE;
    }
    certSize += obj.header.size + obj.value.size;

    struct HksAsn1Blob certBlob = { ASN_1_TAG_TYPE_SEQ, certSize, attestCert->data + ATT_CERT_HEADER_SIZE };
    return HksAsn1WriteFinal(attestCert, &certBlob);
}

static int32_t InsertSignatureGroupClaim(struct HksBlob *out, const struct HksUsageSpec *attetUsageSpec,
    uint32_t secLevel)
{
    if (!(attetUsageSpec->purpose & (HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY))) {
        return HKS_ERROR_NOT_SUPPORTED;
    }
    if (attetUsageSpec->digest != HKS_DIGEST_SHA256) {
        return HKS_ERROR_NOT_SUPPORTED;
    }

    uint8_t asn1True = ASN_1_TRUE_VALUE;
    struct HksAsn1Blob booleanTrue = { ASN_1_TAG_TYPE_BOOL, 1, &asn1True };
    if (attetUsageSpec->algType == HKS_ALG_RSA) {
        if (attetUsageSpec->padding == HKS_PADDING_PSS) {
            HKS_LOG_I("inserting SigRsaPssMgf1Sha256 group\n");
            return HksInsertClaim(out, &hksGroupSigRsaPssMgf1Sha256Oid, &booleanTrue, secLevel);
        }
    } else if (attetUsageSpec->algType == HKS_ALG_ECC) {
        if (attetUsageSpec->padding == HKS_PADDING_NONE) {
            HKS_LOG_I("inserting SigEcdsaSha256 group\n");
            return HksInsertClaim(out, &hksGroupSigEcdsaSha256Oid, &booleanTrue, secLevel);
        }
    }
    return HKS_ERROR_NOT_SUPPORTED;
}

static int32_t InsertEncryptionGroupClaim(struct HksBlob *out, const struct HksUsageSpec *attetUsageSpec,
    uint32_t secLevel)
{
    if (!(attetUsageSpec->purpose & (HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT))) {
        return HKS_ERROR_NOT_SUPPORTED;
    }

    uint8_t asn1True = ASN_1_TRUE_VALUE;
    struct HksAsn1Blob booleanTrue = { ASN_1_TAG_TYPE_BOOL, 1, &asn1True };
    if (attetUsageSpec->algType == HKS_ALG_RSA) {
        if (attetUsageSpec->digest == HKS_DIGEST_SHA256) {
            if (attetUsageSpec->padding == HKS_PADDING_OAEP) {
                HKS_LOG_I("inserting EncRsaOaepMgf1Sha256 group\n");
                return HksInsertClaim(out, &hksGroupEncRsaOaepMgf1Sha256Oid, &booleanTrue, secLevel);
            }
        } else {
            if (attetUsageSpec->padding == HKS_PADDING_OAEP) {
                HKS_LOG_I("inserting EncRsaOaep group\n");
                return HksInsertClaim(out, &hksGroupEncRsaOaepOid, &booleanTrue, secLevel);
            }
        }
    }
    return HKS_ERROR_NOT_SUPPORTED;
}

static int32_t InsertGroupClaim(bool *isInsert, struct HksBlob *out, const struct HksUsageSpec *attetUsageSpec,
    uint32_t secLevel)
{
    if (isInsert == NULL || CheckBlob(out) != HKS_SUCCESS || attetUsageSpec == NULL) {
        HKS_LOG_E("invalid input");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    uint8_t *p = out->data;
    int32_t ret = InsertSignatureGroupClaim(out, attetUsageSpec, secLevel);
    if (ret != HKS_SUCCESS && ret != HKS_ERROR_NOT_SUPPORTED) {
        HKS_LOG_E("fail to insert signature group claim");
        return ret;
    }
    ret = InsertEncryptionGroupClaim(out, attetUsageSpec, secLevel);
    if (ret != HKS_SUCCESS && ret != HKS_ERROR_NOT_SUPPORTED) {
        HKS_LOG_E("fail to insert encryption group claim");
        return ret;
    }
    if (p != out->data) {
        *isInsert = true;
    } else {
        *isInsert = false;
    }
    return HKS_SUCCESS;
}

static int32_t VerifyIdsInfo(enum HksTag tag, struct HksParam *param)
{
    (void)tag;
    (void)param;
    return HKS_SUCCESS;
}

static int32_t InsertAppIdClaim(struct HksBlob *out, const struct HksParamSet *paramSet, uint32_t secLevel)
{
    struct HksParam *appId = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ATTESTATION_APPLICATION_ID, &appId);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_I("not contain appId param!");
        return HKS_SUCCESS;
    }

    uint8_t buf[ASN_1_MAX_HEADER_LEN + MAX_OID_LEN + HKS_APP_ID_SIZE] = {0};
    uint8_t *tmp = buf;
    if (memcpy_s(tmp, MAX_OID_LEN, hksApplicationIdRawOid.data, hksApplicationIdRawOid.size) != EOK) {
        HKS_LOG_I("invalid oid of app id!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    tmp += hksApplicationIdRawOid.size;

    struct HksBlob tmpBlob = { sizeof(buf) - hksApplicationIdRawOid.size, tmp };
    struct HksAsn1Blob value = { ASN_1_TAG_TYPE_OCT_STR, appId->blob.size, appId->blob.data };
    ret = HksAsn1WriteFinal(&tmpBlob, &value);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("write final value fail\n");
        return ret;
    }

    uint32_t seqSize = tmp - buf;
    struct HksAsn1Blob seq = { ASN_1_TAG_TYPE_SEQ, seqSize, buf };
    return HksInsertClaim(out, &hksApplicationIdOid, &seq, secLevel);
}

static int32_t BuildAttestMsgClaims(struct HksBlob *out, const struct HksParamSet *paramSet)
{
    uint32_t secLevel = HKS_SECURITY_LEVEL_HIGH;
    uint8_t version = HKS_HW_ATTESTATION_VERSION;
    struct HksAsn1Blob versionBlob = { ASN_1_TAG_TYPE_INT, 1, &version };
    int32_t ret = HksAsn1InsertValue(out, NULL, &versionBlob);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert version fail\n");
        return ret;
    }

    struct HksParam *challenge = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_ATTESTATION_CHALLENGE, &challenge);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get challenge param failed!");
        return ret;
    }

    struct HksAsn1Blob challengeBlob = { ASN_1_TAG_TYPE_OCT_STR, challenge->blob.size, challenge->blob.data };
    ret = HksInsertClaim(out, &hksAttestationChallengeOid, &challengeBlob, HKS_SECURITY_LEVEL_LOW);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert challenge fail\n");
        return ret;
    }

    struct HksParam *keyId = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_ATTESTATION_ID_ALIAS, &keyId);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get keyId param failed!");
        return ret;
    }
    struct HksAsn1Blob keyIdBlob = { ASN_1_TAG_TYPE_OCT_STR, keyId->blob.size, keyId->blob.data };
    ret = HksInsertClaim(out, &hksKeyIdOid, &keyIdBlob, secLevel);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert keyId fail\n");
        return ret;
    }

    ret = InsertAppIdClaim(out, paramSet, secLevel);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert appId fail\n");
        return ret;
    }
    return HKS_SUCCESS;
}

static int32_t BuildAttestKeyClaims(struct HksBlob *out, const struct HksUsageSpec *attetUsageSpec)
{
    uint32_t secLevel = HKS_SECURITY_LEVEL_HIGH;
    bool isGroupInsert = false;
    int32_t ret = InsertGroupClaim(&isGroupInsert, out, attetUsageSpec, secLevel);
    if (ret != HKS_SUCCESS && ret != HKS_ERROR_NOT_SUPPORTED) {
        HKS_LOG_E("insert group fail, ret %d\n", ret);
        return ret;
    }

    if (isGroupInsert) {
        return HKS_SUCCESS;
    }

    struct HksAsn1Blob usage = { ASN_1_TAG_TYPE_OCT_STR, sizeof(uint32_t), (uint8_t *)&attetUsageSpec->purpose };
    ret = HksInsertClaim(out, &hksKeyUsageOid, &usage, secLevel);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert usage fail\n");
        return ret;
    }
    struct HksAsn1Blob digest = { ASN_1_TAG_TYPE_OCT_STR, sizeof(uint32_t), (uint8_t *)&attetUsageSpec->digest };
    ret = HksInsertClaim(out, &hksDigestOid, &digest, secLevel);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert digest fail\n");
        return ret;
    }

    if (attetUsageSpec->algType != HKS_ALG_RSA) {
        struct HksAsn1Blob padding = { ASN_1_TAG_TYPE_OCT_STR, sizeof(uint32_t), (uint8_t *)&attetUsageSpec->padding };
        ret = HksInsertClaim(out, &hksPaddingOid, &padding, secLevel);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("insert padding fail\n");
            return ret;
        }
    }
    return HKS_SUCCESS;
}

static int32_t InsertIdOrSecInfoData(enum HksTag tag, uint32_t type, const struct HksBlob *oid, struct HksBlob *out,
    const struct HksParamSet *paramSet)
{
    struct HksParam *param = NULL;
    int32_t ret = HksGetParam(paramSet, tag, &param);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get id param failed! tag is %x", tag);
        return ret;
    }

    ret = VerifyIdsInfo(tag, param);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("VerifyIdsInfo failed! tag is %x", tag);
        return ret;
    }

    struct HksAsn1Blob paramBlob = { type, param->blob.size, param->blob.data };
    ret = HksInsertClaim(out, oid, &paramBlob, HKS_SECURITY_LEVEL_SUPER);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("insert id cliam failed! tag is %x", tag);
        return ret;
    }
    return ret;
}

static int32_t InsertIdOrSecInfoByOid(enum HksTag tagOne, enum HksTag tagTwo,
    struct HksBlob *out, const struct HksParamSet *paramSet)
{
    if (tagOne != tagTwo) {
        return HKS_SUCCESS;
    }
    switch (tagOne) {
        case HKS_TAG_ATTESTATION_ID_BRAND:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_UTF8_STR, &hksBrandOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_DEVICE:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_UTF8_STR, &hksDeviceOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_PRODUCT:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_UTF8_STR, &hksProductOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_SERIAL:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_UTF8_STR, &hksSnOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_IMEI:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_UTF8_STR, &hksImeiOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_MEID:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_UTF8_STR, &hksMeidOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_MANUFACTURER:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_UTF8_STR, &hksManufacturerOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_MODEL:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_UTF8_STR, &hksModelOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_SOCID:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_UTF8_STR, &hksSocIdOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_UDID:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_UTF8_STR, &hksUdidOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_SEC_LEVEL_INFO:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_OCT_STR, &hksSecInfoOid, out, paramSet);
        case HKS_TAG_ATTESTATION_ID_VERSION_INFO:
            return InsertIdOrSecInfoData(tagOne, ASN_1_TAG_TYPE_OCT_STR, &hksOsVersionOid, out, paramSet);
        default:
            break;
    }
    return HKS_FAILURE;
}

static int32_t BuildAttestDeviceClaims(struct HksBlob *out, const struct HksParamSet *paramSet)
{
    int32_t ret = HKS_SUCCESS;
    for (uint32_t i = 0; i < sizeof(g_idAttestList) / sizeof(g_idAttestList[0]); i++) {
        for (uint32_t j = 0; j < paramSet->paramsCnt; j++) {
            ret = InsertIdOrSecInfoByOid(paramSet->params[j].tag, g_idAttestList[i], out, paramSet);
            if (ret != HKS_SUCCESS) {
                HKS_LOG_E("insert ids %x fail\n", paramSet->params[i].tag);
                return ret;
            }
        }
    }
    return ret;
}

static int32_t BuildAttestClaims(const struct HksParamSet *paramSet, struct HksAttestSpec *attestSpec)
{
    uint8_t *claims = HksMalloc(ATTEST_CLAIM_BUF_LEN + ASN_1_MAX_HEADER_LEN);
    if (claims == NULL) {
        HKS_LOG_E("malloc claims fail\n");
        return HKS_ERROR_MALLOC_FAIL;
    }
    attestSpec->claims.data = claims;
    attestSpec->claims.size = ATTEST_CLAIM_BUF_LEN + ASN_1_MAX_HEADER_LEN;

    struct HksBlob tmp = { ATTEST_CLAIM_BUF_LEN + ASN_1_MAX_HEADER_LEN, claims };
    tmp.data += ASN_1_MAX_HEADER_LEN;
    tmp.size -= ASN_1_MAX_HEADER_LEN;

    int32_t ret = BuildAttestMsgClaims(&tmp, paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("build attest message claims fail\n");
        return ret;
    }

    ret = BuildAttestDeviceClaims(&tmp, paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("build attest device claims fail\n");
        return ret;
    }

    ret = BuildAttestKeyClaims(&tmp, &attestSpec->usageSpec);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("build attest key claims fail\n");
        return ret;
    }

    uint8_t *seqData = claims + ASN_1_MAX_HEADER_LEN;
    uint32_t seqSize = tmp.data - seqData;
    struct HksAsn1Blob seqDataBlob = { ASN_1_TAG_TYPE_SEQ, seqSize, seqData };
    ret = HksAsn1WriteFinal(&attestSpec->claims, &seqDataBlob);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("build attest claims fail\n");
        return ret;
    }

    return HKS_SUCCESS;
}

static int32_t ReadCertOrKey(const uint8_t *inData, uint32_t size, struct HksBlob *out)
{
    uint8_t *data = HksMalloc(size);
    if (data == NULL) {
        HKS_LOG_E("malloc data fail\n");
        return HKS_ERROR_MALLOC_FAIL;
    }
    if (memcpy_s(data, size, inData, size) != EOK) {
        HKS_LOG_E("memcpy cert or key failed");
        HKS_FREE_PTR(data);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }
    out->size = size;
    out->data = data;
    return HKS_SUCCESS;
}

static int32_t GetCertOrKey(enum HksCertType type, struct HksBlob *out)
{
    switch (type) {
        case HKS_DEVICE_KEY:
            return ReadCertOrKey(g_deviceKey, sizeof(g_deviceKey), out);
        case HKS_DEVICE_CERT:
            return ReadCertOrKey(g_deviceCert, sizeof(g_deviceCert), out);
        case HKS_CA_CERT:
            return ReadCertOrKey(g_caCert, sizeof(g_caCert), out);
        case HKS_ROOT_CERT:
            return ReadCertOrKey(g_rootCert, sizeof(g_rootCert), out);
        default:
            break;
    }
    return HKS_ERROR_NOT_SUPPORTED;
}

static int32_t GetCertAndKey(const struct HksKeyNode *keyNode, struct HksAttestSpec *attestSpec)
{
    int32_t ret = GetCertOrKey(HKS_DEVICE_CERT, &attestSpec->devCert);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get devCert fail");
        return ret;
    }
    ret = GetCertOrKey(HKS_DEVICE_KEY, &attestSpec->devKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get devKey fail");
        return ret;
    }
    ret = HksGetRawKey(keyNode->paramSet, &attestSpec->attestKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get attestKey fail");
    }
    return ret;
}

static void FreeAttestSpec(struct HksAttestSpec **attestSpec)
{
    struct HksAttestSpec *spec = *attestSpec;

    if (spec == NULL) {
        return;
    }
    if (spec->claims.data != NULL) {
        HKS_FREE_PTR(spec->claims.data);
    }
    if (spec->devCert.data != NULL) {
        HKS_FREE_PTR(spec->devCert.data);
    }
    if (spec->devKey.data != NULL) {
        (void)memset_s(spec->devKey.data, spec->devKey.size, 0, spec->devKey.size);
        HKS_FREE_PTR(spec->devKey.data);
    }
    HksFree(spec);
    *attestSpec = NULL;
}

static int32_t CheckAttestUsageSpec(const struct HksUsageSpec *usageSpec)
{
    if ((usageSpec->algType != HKS_ALG_RSA) && (usageSpec->algType != HKS_ALG_ECC) &&
        (usageSpec->algType != HKS_ALG_X25519)) {
            HKS_LOG_E("invalid alg %u\n", usageSpec->algType);
            return HKS_ERROR_INVALID_ARGUMENT;
    }
    if ((usageSpec->algType == HKS_ALG_RSA) && (usageSpec->padding != HKS_PADDING_PSS)) {
        HKS_LOG_E("invalid padding\n");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

static int32_t BuildAttestSpec(const struct HksKeyNode *keyNode, const struct HksParamSet *paramSet,
    struct HksAttestSpec **outAttestSpec)
{
    struct HksAttestSpec *attestSpec = HksMalloc(sizeof(struct HksAttestSpec));
    if (attestSpec == NULL) {
        HKS_LOG_E("malloc attestSpec fail\n");
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(attestSpec, sizeof(struct HksAttestSpec), 0, sizeof(struct HksAttestSpec));

    SetAttestCertValid(&attestSpec->validity);

    HksFillUsageSpec(keyNode->paramSet, &attestSpec->usageSpec);
    int32_t ret = CheckAttestUsageSpec(&attestSpec->usageSpec);
    if (ret != HKS_SUCCESS) {
        FreeAttestSpec(&attestSpec);
        return ret;
    }

    ret = BuildAttestClaims(paramSet, attestSpec);
    if (ret != HKS_SUCCESS) {
        FreeAttestSpec(&attestSpec);
        return ret;
    }

    attestSpec->claimsOid = hksHuaweiAttestationExtensionOid;

    ret = GetCertAndKey(keyNode, attestSpec);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get cert and key fail\n");
        FreeAttestSpec(&attestSpec);
        return ret;
    }
    *outAttestSpec = attestSpec;
    return HKS_SUCCESS;
}

static int32_t CreateHwAttestCert(const struct HksAttestSpec *attestSpec, struct HksBlob *outAttestCert,
    uint32_t signAlg)
{
    struct HksBlob template = { 0, NULL };
    if (signAlg == HKS_ALG_RSA) {
        template.data = (uint8_t *)g_attestTbsRsa;
        template.size = sizeof(g_attestTbsRsa);
    } else {
        template.data = (uint8_t *)g_attestTbs;
        template.size = sizeof(g_attestTbs);
    }

    uint8_t *attest = HksMalloc(HKS_ATTEST_CERT_SIZE + attestSpec->claims.size);
    HKS_LOG_E("mattestSpec->claims.size is %d!", attestSpec->claims.size);
    if (attest == NULL) {
        HKS_LOG_E("malloc attest cert failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }
    struct HksBlob attestCert = { HKS_ATTEST_CERT_SIZE + attestSpec->claims.size, attest };
    int32_t ret = CreateAttestCert(&attestCert, &template, attestSpec, signAlg);
    if (ret != HKS_SUCCESS) {
        HksFree(attest);
        HKS_LOG_E("CreateAttestCert failed!");
        return ret;
    }
    *outAttestCert = attestCert;
    return HKS_SUCCESS;
}

static int32_t CopyBlobToBuffer(const struct HksBlob *blob, struct HksBlob *buf)
{
    if (buf->size < sizeof(blob->size) + ALIGN_SIZE(blob->size)) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }
    if (memcpy_s(buf->data, buf->size, &blob->size, sizeof(blob->size)) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }
    buf->data += sizeof(blob->size);
    buf->size -= sizeof(blob->size);
    if (memcpy_s(buf->data, buf->size, blob->data, blob->size) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }
    buf->data += ALIGN_SIZE(blob->size);
    buf->size -= ALIGN_SIZE(blob->size);
    return HKS_SUCCESS;
}

static int32_t FormatCertToBuf(enum HksCertType type, struct HksBlob *buf)
{
    struct HksBlob cert = { 0, NULL };
    int32_t ret = GetCertOrKey(type, &cert);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get cert failed!");
        return HKS_ERROR_BAD_STATE;
    }

    ret = CopyBlobToBuffer(&cert, buf);
    HksFree(cert.data);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy cert fail");
        return ret;
    }
    return ret;
}

static int32_t FormatAttestChain(const struct HksBlob *attestCert, const struct HksAttestSpec *attestSpec,
    struct HksBlob *certChain)
{
    struct HksBlob tmp = *certChain;
    *((uint32_t *)tmp.data) = HKS_ATTEST_CERT_COUNT;
    tmp.data += sizeof(uint32_t);
    tmp.size -= sizeof(uint32_t);

    int32_t ret = CopyBlobToBuffer(attestCert, &tmp);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy attest cert fail");
        return ret;
    }

    ret = CopyBlobToBuffer(&attestSpec->devCert, &tmp);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy dev cert fail");
        return ret;
    }

    ret = FormatCertToBuf(HKS_CA_CERT, &tmp);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("format ca cert failed!");
        return HKS_ERROR_BAD_STATE;
    }

    ret = FormatCertToBuf(HKS_ROOT_CERT, &tmp);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("format root cert failed!");
        return HKS_ERROR_BAD_STATE;
    }

    certChain->size = tmp.data - certChain->data;
    HKS_LOG_I("certChain size after format is %u", certChain->size);
    return HKS_SUCCESS;
}

static int32_t CreateHwAttestCertChain(struct HksKeyNode *keyNode, const struct HksParamSet *paramSet,
    struct HksBlob *certChain)
{
    struct HksAttestSpec *attestSpec = NULL;
    int32_t ret = BuildAttestSpec(keyNode, paramSet, &attestSpec);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("build attest spec failed");
        return ret;
    }

    struct HksBlob attestCert;
    ret = CreateHwAttestCert(attestSpec, &attestCert, HKS_ALG_RSA);
    if (ret != HKS_SUCCESS) {
        FreeAttestSpec(&attestSpec);
        HKS_LOG_E("build attest spec failed");
        return ret;
    }

    ret = FormatAttestChain(&attestCert, attestSpec, certChain);
    HKS_FREE_BLOB(attestCert);
    FreeAttestSpec(&attestSpec);
    return ret;
}

static int32_t CheckAttestKeyParams(const struct HksBlob *key, const struct HksParamSet *paramSet,
    struct HksBlob *certChain)
{
    if (CheckBlob(key) != HKS_SUCCESS) {
        HKS_LOG_E("invalid key!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if ((CheckBlob(certChain) != HKS_SUCCESS) || (certChain->size < HKS_ATTEST_CERT_SIZE)) {
        HKS_LOG_E("invalid cert chain!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (HksCheckParamSetValidity(paramSet) != HKS_SUCCESS) {
        HKS_LOG_E("invalid paramSet!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

int32_t HksSoftAttestKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *certChain)
{
    int32_t ret = CheckAttestKeyParams(key, paramSet, certChain);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    struct HksKeyNode *keyNode = HksGenerateKeyNode(key);
    if (keyNode == NULL) {
        HKS_LOG_E("generate keynode failed");
        return HKS_ERROR_BAD_STATE;
    }
    ret = CreateHwAttestCertChain(keyNode, paramSet, certChain);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("create attest cert chain failed!");
    }
    HksFreeKeyNode(&keyNode);
    return ret;
}
