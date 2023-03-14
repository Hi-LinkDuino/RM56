/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "hks_device_sec_test.h"

#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"
#include "hks_test_mem.h"
#include "hks_type.h"

using namespace testing::ext;
namespace {
class HksDeviceSecTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksDeviceSecTest::SetUpTestCase(void)
{
}

void HksDeviceSecTest::TearDownTestCase(void)
{
}

void HksDeviceSecTest::SetUp()
{
}

void HksDeviceSecTest::TearDown()
{
}

struct HksTestCertChain {
    bool certChainExist;
    bool certCountValid;
    bool certDataExist;
    uint32_t certDataSize;
};
static char g_secInfoData[] = "hi_security_level_info";
static char g_challengeData[] = "hi_challenge_data";
static char g_versionData[] = "hi_os_version_data";
static char g_udidData[] = "hi_udid_data";
static char g_snData[] = "hi_sn_data";
static uint32_t g_size = DEFAULT_PARAM_SET_OUT_SIZE;

static struct HksBlob g_secInfo = { sizeof(g_secInfoData), (uint8_t *)g_secInfoData };
static struct HksBlob g_challenge = { sizeof(g_challengeData), (uint8_t *)g_challengeData };
static struct HksBlob g_version = { sizeof(g_versionData), (uint8_t *)g_versionData };
static struct HksBlob g_udid = { sizeof(g_udidData), (uint8_t *)g_udidData };
static struct HksBlob g_sn = { sizeof(g_snData), (uint8_t *)g_snData };

static void FreeCertChain(struct HksCertChain **certChain, const uint32_t pos)
{
    if (certChain == NULL || *certChain == NULL) {
        return;
    }

    if ((*certChain)->certs == NULL) {
        HksTestFree(*certChain);
        *certChain = NULL;
        return;
    }

    for (uint32_t j = 0; j < pos; j++) {
        if ((*certChain)->certs[j].data != NULL) {
            HksTestFree((*certChain)->certs[j].data);
            (*certChain)->certs[j].data = NULL;
        }
    }

    if ((*certChain)->certs != NULL) {
        HksTestFree((*certChain)->certs);
        (*certChain)->certs = NULL;
    }

    if (*certChain != NULL) {
        HksTestFree(*certChain);
        *certChain = NULL;
    }
}

static int32_t ConstructDataToCertChain(struct HksCertChain **certChain,
    const struct HksTestCertChain *certChainParam)
{
    if (!certChainParam->certChainExist) {
        return 0;
    }
    *certChain = (struct HksCertChain *)HksTestMalloc(sizeof(struct HksCertChain));
    if (*certChain == nullptr) {
        HKS_TEST_LOG_E("malloc fail");
        return HKS_ERROR_MALLOC_FAIL;
    }
    if (!certChainParam->certCountValid) {
        (*certChain)->certsCount = 0;
        return 0;
    }
    (*certChain)->certsCount = CERT_COUNT;
    if (!certChainParam->certDataExist) {
        (*certChain)->certs = NULL;
        return 0;
    }
    (*certChain)->certs = (struct HksBlob *)HksTestMalloc(sizeof(struct HksBlob) * ((*certChain)->certsCount));
    if ((*certChain)->certs == NULL) {
        HksTestFree(*certChain);
        *certChain = NULL;
    }
    for (uint32_t i = 0; i < (*certChain)->certsCount; i++) {
        (*certChain)->certs[i].size = certChainParam->certDataSize;
        (*certChain)->certs[i].data = (uint8_t *)HksTestMalloc((*certChain)->certs[i].size);
        if ((*certChain)->certs[i].data == NULL) {
            HKS_TEST_LOG_E("malloc fail");
            FreeCertChain(certChain, i);
            return HKS_ERROR_MALLOC_FAIL;
        }
        memset_s((*certChain)->certs[i].data, certChainParam->certDataSize, 0, certChainParam->certDataSize);
    }
    return 0;
}

static int32_t TestGenerateKey(const struct HksBlob *keyAlias)
{
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
    };
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_TEST_LOG_E("HksInitParamSet failed");
        return ret;
    }

    ret = HksAddParams(paramSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    if (ret != HKS_SUCCESS) {
        HKS_TEST_LOG_E("HksAddParams failed");
        HksFreeParamSet(&paramSet);
        return ret;
    }

    ret = HksBuildParamSet(&paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_TEST_LOG_E("HksBuildParamSet failed");
        HksFreeParamSet(&paramSet);
        return ret;
    }

    ret = HksGenerateKey(keyAlias, paramSet, NULL);
    if (ret != HKS_SUCCESS) {
        HKS_TEST_LOG_E("HksGenerateKey failed");
    }
    HksFreeParamSet(&paramSet);
    return ret;
}

static int32_t TestKeyAttest(struct HksCertChain *certChain)
{
    HKS_TEST_LOG_E("testKeyAttest start");
    char alias[] = "testKey";
    struct HksBlob keyAlias = { sizeof(alias), (uint8_t *)alias };
    int32_t ret = TestGenerateKey(&keyAlias);
    if (ret != HKS_SUCCESS) {
        HKS_TEST_LOG_E("TestGenerateKey failed");
    }

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_ATTESTATION_ID_SEC_LEVEL_INFO, .blob = g_secInfo },
        { .tag = HKS_TAG_ATTESTATION_CHALLENGE, .blob = g_challenge },
        { .tag = HKS_TAG_ATTESTATION_ID_VERSION_INFO, .blob = g_version },
        { .tag = HKS_TAG_ATTESTATION_ID_UDID, .blob = g_udid },
        { .tag = HKS_TAG_ATTESTATION_ID_SERIAL, .blob = g_sn },
        { .tag = HKS_TAG_ATTESTATION_ID_ALIAS, .blob = keyAlias },
    };
    struct HksParamSet *paramSet = NULL;
    ret = HksInitParamSet(&paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_TEST_LOG_E("HksInitParamSet failed");
        return ret;
    }

    ret = HksAddParams(paramSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    if (ret != HKS_SUCCESS) {
        HKS_TEST_LOG_E("HksAddParams failed");
        HksFreeParamSet(&paramSet);
        return ret;
    }

    ret = HksBuildParamSet(&paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_TEST_LOG_E("HksBuildParamSet failed");
        HksFreeParamSet(&paramSet);
        return ret;
    }

    ret = HksAttestKey(&keyAlias, paramSet, certChain);
    if (ret != HKS_SUCCESS) {
        HKS_TEST_LOG_E("HksAttestKey failed");
    }
    HksFreeParamSet(&paramSet);
    return ret;
}

static void FreeBuf(uint8_t *a, uint8_t *b, uint8_t *c, uint8_t *d, uint8_t *e)
{
    if (a != nullptr) {
        HksTestFree(a);
    }
    if (b != nullptr) {
        HksTestFree(b);
    }
    if (c != nullptr) {
        HksTestFree(c);
    }
    if (d != nullptr) {
        HksTestFree(d);
    }
    if (e != nullptr) {
        HksTestFree(e);
    }
}
static int32_t ValidateCertChainTest(struct HksCertChain *certChain)
{
    HKS_TEST_LOG_E("validate cert chain test start");
    uint8_t *challengeData = (uint8_t *)HksTestMalloc(g_size);
    uint8_t *secInfoData = (uint8_t *)HksTestMalloc(g_size);
    uint8_t *versionData = (uint8_t *)HksTestMalloc(g_size);
    uint8_t *snData = (uint8_t *)HksTestMalloc(g_size);
    uint8_t *udidData = (uint8_t *)HksTestMalloc(g_size);
    if (challengeData == nullptr || secInfoData == nullptr || versionData == nullptr
        || snData == nullptr || udidData == nullptr) {
        FreeBuf(challengeData, secInfoData, versionData, snData, udidData);
        return HKS_ERROR_MALLOC_FAIL;
    }
    int32_t ret = HKS_ERROR_MALLOC_FAIL;
    struct HksParamSet *paramSet = nullptr;
    do {
        ret = HksInitParamSet(&paramSet);
        if (ret != HKS_SUCCESS) {
            break;
        }
        struct HksParam tmpParam[] = {
            { .tag = HKS_TAG_ATTESTATION_ID_SEC_LEVEL_INFO, .blob = { g_size, secInfoData } },
            { .tag = HKS_TAG_ATTESTATION_CHALLENGE, .blob = { g_size, challengeData } },
            { .tag = HKS_TAG_ATTESTATION_ID_VERSION_INFO, .blob = { g_size, versionData } },
            { .tag = HKS_TAG_ATTESTATION_ID_UDID, .blob = { g_size, udidData } },
            { .tag = HKS_TAG_ATTESTATION_ID_SERIAL, .blob = { g_size, snData } },
        };
        ret = HksAddParams(paramSet, tmpParam, sizeof(tmpParam) / sizeof(tmpParam[0]));
        if (ret != HKS_SUCCESS) {
            break;
        }
        ret = HksBuildParamSet(&paramSet);
        if (ret != HKS_SUCCESS) {
            break;
        }
        ret = HksValidateCertChain(certChain, paramSet);
        HKS_TEST_LOG_I("validate cert chain result is %x", ret);
        HKS_TEST_LOG_I("secinfo is %s\n", (char *)paramSet->params[0].blob.data);
        HKS_TEST_LOG_I("challenge is %s\n", (char *)paramSet->params[CERT_COUNT - 3].blob.data);
        HKS_TEST_LOG_I("version is %s\n", (char *)paramSet->params[CERT_COUNT - 2].blob.data);
        HKS_TEST_LOG_I("udid is %s\n", (char *)paramSet->params[CERT_COUNT - 1].blob.data);
        HKS_TEST_LOG_I("sn is %s\n", (char *)paramSet->params[CERT_COUNT].blob.data);
    } while (0);
    FreeBuf(challengeData, secInfoData, versionData, snData, udidData);
    HksFreeParamSet(&paramSet);
    return ret;
}

HWTEST_F(HksDeviceSecTest, HksDeviceSecTest001, TestSize.Level0)
{
    HksCertChain *certChain = NULL;
    const struct HksTestCertChain certParam = { true, true, true, g_size };
    int32_t ret = ConstructDataToCertChain(&certChain, &certParam);
    ret = TestKeyAttest(certChain);
    ASSERT_TRUE(ret == 0);
    ret = ValidateCertChainTest(certChain);
    FreeCertChain(&certChain, certChain->certsCount);
    ASSERT_TRUE(ret == 0);
}
}
