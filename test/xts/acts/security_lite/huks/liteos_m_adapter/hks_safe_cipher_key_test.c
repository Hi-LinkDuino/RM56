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
#ifndef _CUT_ED25519_

#include "hks_safe_cipher_key_test.h"

#include <hctest.h>
#include "iot_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_curve25519.h"
#include "hks_test_log.h"
#include "hks_test_mem.h"
#include "stdlib.h"

const char *g_storePath = "/storage/";
const char *g_testEd25519 = "test_ed25519";

#define TEST_TASK_STACK_SIZE      0x2000
#define WAIT_TO_TEST_DONE         4

static osPriority_t g_setPriority;

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(security, securityData, HksSafeCipherKeyTest);

static void ExecHksInitialize(void const *argument)
{
    LiteTestPrint("HksInitialize Begin!\n");
    TEST_ASSERT_TRUE(HksInitialize() == 0);
    LiteTestPrint("HksInitialize End!\n");
    osThreadExit();
}

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksSafeCipherKeyTestSetUp()
{
    LiteTestPrint("setup\n");
    IoTWatchDogDisable();
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityAboveNormal6;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)ExecHksInitialize, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksSafeCipherKeyTestSetUp End2!\n"); 

    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksSafeCipherKeyTestTearDown()
{
    LiteTestPrint("tearDown\n");
    IoTWatchDogEnable();
    return TRUE;
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
    for (uint32_t i = 0; i < keyCount; ++i) {
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

static int32_t CompareTwoKey(const struct HksBlob *keyAliasOne, const struct HksBlob *keyAliasTwo)
{
    uint32_t sizeOne = HksTestFileSize(g_storePath, "hks_keystore");
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
 * @tc.name: HksSafeCipherKeyTest.HksSafeCipherKeyTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksSafeCipherKeyTest, HksSafeCipherKeyTest001, Level1)
{
    struct HksBlob ed25519Alias = { strlen(g_testEd25519), (uint8_t *)g_testEd25519 };
    uint8_t pubKey[32] = {0};
    uint32_t pubKeyLen = 32;
    struct HksBlob pubKeyInfo = { pubKeyLen, pubKey };
    int32_t ret = TestGenerateEd25519Key(ed25519Alias);
    TEST_ASSERT_TRUE(ret == 0);

    ret = HksExportPublicKey(&ed25519Alias, NULL, &pubKeyInfo);
    TEST_ASSERT_TRUE(ret == 0);

    ret = HksDeleteKey(&ed25519Alias, NULL);
    TEST_ASSERT_TRUE(ret == 0);

    struct HksBlob newAliasOne = { strlen("test_ed25519_1"), (uint8_t *)"test_ed25519_1" };
    ret = TestImportEd25519(newAliasOne, &pubKeyInfo);
    TEST_ASSERT_TRUE(ret == 0);

    struct HksBlob newAliasTwo = { strlen("test_ed25519_2"), (uint8_t *)"test_ed25519_2" };
    ret = TestImportEd25519(newAliasTwo, &pubKeyInfo);
    TEST_ASSERT_TRUE(ret == 0);

    ret = CompareTwoKey(&newAliasOne, &newAliasTwo);
    TEST_ASSERT_TRUE(ret != 0);

    ret = HksDeleteKey(&newAliasOne, NULL);
    HKS_TEST_ASSERT(ret == 0);
    TEST_ASSERT_TRUE(ret == 0);
    ret = HksDeleteKey(&newAliasTwo, NULL);
    TEST_ASSERT_TRUE(ret == 0);
}
RUN_TEST_SUITE(HksSafeCipherKeyTest);

#endif
#endif /* _CUT_AUTHENTICATE_ */
