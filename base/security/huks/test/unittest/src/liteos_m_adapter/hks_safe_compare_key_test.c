/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef _CUT_AUTHENTICATE_

#include "hks_safe_compare_key_test.h"

#include <hctest.h>
#include "hi_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_file_operator.h"
#include "hks_test_log.h"
#include "hks_test_mem.h"

static const char *g_storePath = "/storage/";
static const char *g_testOne = "TestOne";
static const char *g_testTwo = "TestTwo";

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(husk, huks_lite, HksSafeCompareKeyTest);

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksSafeCompareKeyTestSetUp()
{
    LiteTestPrint("setup\n");
    hi_watchdog_disable();
    TEST_ASSERT_TRUE(HksInitialize() == 0);
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksSafeCompareKeyTestTearDown()
{
    LiteTestPrint("tearDown\n");
    hi_watchdog_enable();
    return TRUE;
}


static const struct HksTestGenKeyParams g_testGenKeyParams[] = {
    /* x25519: tee sign/verify */
    { 0, HKS_SUCCESS, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true,
            true, HKS_ALG_X25519,
            true, HKS_CURVE25519_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
            false, 0,
            false, 0,
            false, 0,
            false, 0
        },
        { false, 0 },
    },
};

static int32_t SafeTestGenerateKey(struct HksBlob *keyAlias)
{
    uint32_t index = 0;
    uint32_t performTimes = 1;

    struct HksParamSet *paramSet = NULL;
    struct GenerateKeyParamSetStructure paramStruct = {
        &paramSet,
        g_testGenKeyParams[index].paramSetParams.paramSetExist,
        g_testGenKeyParams[index].paramSetParams.setAlg, g_testGenKeyParams[index].paramSetParams.alg,
        g_testGenKeyParams[index].paramSetParams.setKeySize, g_testGenKeyParams[index].paramSetParams.keySize,
        g_testGenKeyParams[index].paramSetParams.setPurpose, g_testGenKeyParams[index].paramSetParams.purpose,
        g_testGenKeyParams[index].paramSetParams.setDigest, g_testGenKeyParams[index].paramSetParams.digest,
        g_testGenKeyParams[index].paramSetParams.setPadding, g_testGenKeyParams[index].paramSetParams.padding,
        g_testGenKeyParams[index].paramSetParams.setBlockMode, g_testGenKeyParams[index].paramSetParams.mode,
        g_testGenKeyParams[index].paramSetParams.setKeyStorageFlag,
        g_testGenKeyParams[index].paramSetParams.keyStorageFlag
    };
    int32_t ret = TestConstructGenerateKeyParamSet(&paramStruct);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParamSet *paramSetOut = NULL;
    ret = TestConstructGenerateKeyParamSetOut(&paramSetOut,
        g_testGenKeyParams[index].paramSetParamsOut.paramSetExist,
        g_testGenKeyParams[index].paramSetParamsOut.paramSetSize);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksGenerateKeyRun(keyAlias, paramSet, paramSetOut, performTimes);
    if (ret != g_testGenKeyParams[index].expectResult) {
        HKS_TEST_LOG_I("failed, ret[%u] = %d", g_testGenKeyParams[index].testId, ret);
    }
    HKS_TEST_ASSERT(ret == g_testGenKeyParams[index].expectResult);

    if (ret == g_testGenKeyParams[index].expectResult) {
        ret = 0;
    }
    HksFreeParamSet(&paramSet);
    HksFreeParamSet(&paramSetOut);
    return ret;
}


static struct HksBlob g_storageImageBuffer;

static int32_t GetKeyOffsetByKeyAlias(const struct HksBlob *keyAlias, uint32_t *keyOffset)
{
    struct HksBlob storageBuf = g_storageImageBuffer;

    /* 1. get imageBuffer total Len */
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)storageBuf.data;
    uint32_t keyCount = keyInfoHead->keyCount;
    uint32_t totalLen = keyInfoHead->totalLen;

    /* 2. traverse imageBuffer to search for keyAlias */
    uint32_t offset = sizeof(*keyInfoHead);
    for (uint16_t i = 0; i < keyCount; ++i) {
        uint8_t *tmpBuf = storageBuf.data + offset;
        struct HksStoreKeyInfo *keyInfo = (struct HksStoreKeyInfo *)tmpBuf;
        if (keyInfo->aliasSize == keyAlias->size) {
            if (HksMemCmp(keyAlias->data, tmpBuf + sizeof(*keyInfo), keyAlias->size) == 0) {
                *keyOffset = offset;
                return HKS_SUCCESS;
            }
        }

        offset += keyInfo->keyInfoLen;
    }

    return HKS_ERROR_NOT_EXIST;
}


static int32_t CompareKeyData(struct HksBlob *keyAliasOne, struct HksBlob *keyAliasTwo)
{
    uint32_t sizeOne = HksTestFileSize(g_storePath, "hks_keystore");
    if (sizeOne == 0) {
        return 0;
    }
    uint8_t *bufOne = (uint8_t *)HksTestMalloc(sizeOne);
    if (bufOne == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    uint32_t sizeRead = HksTestFileRead(g_storePath, "hks_keystore", 0, bufOne, sizeOne);
    TEST_ASSERT_TRUE(sizeRead > 0);

    g_storageImageBuffer.data = bufOne;
    g_storageImageBuffer.size = sizeOne;

    int32_t offset1;
    int ret = GetKeyOffsetByKeyAlias(keyAliasOne, &offset1);
    TEST_ASSERT_TRUE(ret == 0);

    struct HksStoreKeyInfo *keyInfo1 = (struct HksStoreKeyInfo *)(g_storageImageBuffer.data + offset1);

    int32_t offset2;
    ret = GetKeyOffsetByKeyAlias(keyAliasTwo, &offset2);
    TEST_ASSERT_TRUE(ret == 0);

    struct HksStoreKeyInfo *keyInfo2 = (struct HksStoreKeyInfo *)(g_storageImageBuffer.data + offset2);

    TEST_ASSERT_TRUE(keyInfo1->keyInfoLen == keyInfo2->keyInfoLen);

    ret = memcmp(keyInfo1, keyInfo2, keyInfo1->keyInfoLen);
    HksTestFree(bufOne);
    return ret;
}

/**
 * @tc.name: HksSafeCompareKeyTest.HksSafeCompareKeyTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksSafeCompareKeyTest, HksSafeCompareKeyTest001, Level1)
{
    struct HksBlob keyAliasOne = { strlen(g_testOne), (uint8_t *)g_testOne };
    int32_t ret = SafeTestGenerateKey(&keyAliasOne);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob keyAliasTwo = { strlen(g_testTwo), (uint8_t *)g_testTwo };
    ret = SafeTestGenerateKey(&keyAliasTwo);
    HKS_TEST_ASSERT(ret == 0);

    ret = CompareKeyData(&keyAliasOne, &keyAliasTwo);
    HKS_TEST_ASSERT(ret != 0);
    TEST_ASSERT_TRUE(ret != 0);
}
RUN_TEST_SUITE(HksSafeCompareKeyTest);

#endif /* _CUT_AUTHENTICATE_ */
