/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "ohos_types.h"
#include <securec.h>
#include "hctest.h"
#include "ohos_errno.h"
#include "kv_store.h"
#include "iot_watchdog.h"
#include "utils_config.h"

#define MAX_KEY_LEN_TEST    32
#define MAX_VALUE_LEN_TEST    128
#define MAX_KEY_NUM_TEST    50
#define MAX_CACHE_NUM_TEST    10
#define INVALID_KEY_NUM    128

#ifndef DATA_PATH
#define DATA_PATH "/data"
#endif

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is kvStore
 * @param        : test suit name is KvStoreFuncTestSuite
 */
LITE_TEST_SUIT(utils, kvStore, KvStoreFuncTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL KvStoreFuncTestSuiteSetUp(void)
{
    UtilsSetEnv(DATA_PATH);
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL KvStoreFuncTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

/* Create files in batches. */
BOOL SetKVFiles (int num, const char* key, const char* value)
{
    int size;
    int ret;
    char keytemp[MAX_KEY_LEN_TEST] = {0};
    char valuetemp[MAX_VALUE_LEN_TEST] = {0};
    char temp[MAX_VALUE_LEN_TEST] = {0};
    if (num <= 0) {
        return FALSE;
    }
    for (int i = 1; i <= num; i++) {
        size = sprintf_s(keytemp, MAX_KEY_LEN_TEST, "%s_%d", key, i);
        if (size < 0) {
            return FALSE;
        }
        size = sprintf_s(valuetemp, MAX_VALUE_LEN_TEST, "%s_%d", value, i);
        if (size < 0) {
            return FALSE;
        }
        ret = UtilsSetValue(keytemp, valuetemp);
        if (i <= MAX_KEY_NUM_TEST) {
            TEST_ASSERT_EQUAL_INT(0, ret);
        } else {
            TEST_ASSERT_EQUAL_INT(-1, ret);
        }
        ret = UtilsGetValue(keytemp, temp, MAX_VALUE_LEN_TEST);
        if (i <= MAX_KEY_NUM_TEST) {
#ifdef FEATURE_KV_CACHE
            TEST_ASSERT_EQUAL_INT(0, ret);
#else
            TEST_ASSERT_GREATER_THAN_INT(0, ret);
#endif
            TEST_ASSERT_EQUAL_STRING(valuetemp, temp);
        } else {
            TEST_ASSERT_EQUAL_INT(-1, ret);
        }
        memset_s(keytemp, MAX_KEY_LEN_TEST, 0, MAX_KEY_LEN_TEST);
        memset_s(temp, MAX_VALUE_LEN_TEST, 0, MAX_VALUE_LEN_TEST);
        memset_s(valuetemp, MAX_VALUE_LEN_TEST, 0, MAX_VALUE_LEN_TEST);
        IoTWatchDogKick();
    }
    IoTWatchDogKick();
    return TRUE;
}

/* Read files in batches. */
BOOL ReadKVFiles (int num, const char* key, const char* value)
{
    int size;
    int ret;
    int i = 1;
    char keytemp[MAX_KEY_LEN_TEST] = {0};
    char valuetemp[MAX_VALUE_LEN_TEST] = {0};
    char temp[MAX_VALUE_LEN_TEST] = {0};
    if (num <= 0) {
        return FALSE;
    }
    for (int loop = num; loop > 0; loop--) {
        size = sprintf_s(keytemp, MAX_KEY_LEN_TEST, "%s_%d", key, loop);
        if (size < 0) {
            return FALSE;
        }
        size = sprintf_s(valuetemp, MAX_VALUE_LEN_TEST, "%s_%d", value, loop);
        if (size < 0) {
            return FALSE;
        }
        ret = UtilsGetValue(keytemp, temp, MAX_VALUE_LEN_TEST);
        if (loop <= MAX_KEY_NUM_TEST) {
            if (i <= MAX_CACHE_NUM_TEST) {
#ifdef FEATURE_KV_CACHE
                TEST_ASSERT_EQUAL_INT(0, ret);
#else
                TEST_ASSERT_GREATER_THAN_INT(0, ret);
#endif
            } else {
                TEST_ASSERT_GREATER_THAN_INT(0, ret);
            }
            TEST_ASSERT_EQUAL_STRING(valuetemp, temp);
            i++;
        } else {
            TEST_ASSERT_EQUAL_INT(-1, ret);
        }
        memset_s(keytemp, MAX_KEY_LEN_TEST, 0, MAX_KEY_LEN_TEST);
        memset_s(temp, MAX_VALUE_LEN_TEST, 0, MAX_VALUE_LEN_TEST);
        memset_s(valuetemp, MAX_VALUE_LEN_TEST, 0, MAX_VALUE_LEN_TEST);
        IoTWatchDogKick();
    }
    IoTWatchDogKick();
    return TRUE;
}

/* Delete files in batches. */
BOOL DeleteKVFiles (int num, const char* key)
{
    int size;
    int ret;
    char keytemp[MAX_KEY_LEN_TEST] = {0};

    if (num <= 0) {
        return FALSE;
    }
    for (int i = 1; i <= num; i++) {
        size = sprintf_s(keytemp, MAX_KEY_LEN_TEST, "%s_%d", key, i);
        if (size < 0) {
            return FALSE;
        }
        ret = UtilsDeleteValue(keytemp);
        if (i <= MAX_KEY_NUM_TEST) {
            TEST_ASSERT_EQUAL_INT(0, ret);
        } else {
            TEST_ASSERT_EQUAL_INT(-1, ret);
        }
        memset_s(keytemp, MAX_KEY_LEN_TEST, 0, MAX_KEY_LEN_TEST);
        IoTWatchDogKick();
    }
    IoTWatchDogKick();
    return TRUE;
}

/**
 * @tc.number    : SUB_UTILS_KV_STORE_0100
 * @tc.name      : UtilsSetValue parameter legal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue001, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_0200
 * @tc.name      : UtilsSetValue parameter legal test using key with underline
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue002, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version_100";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_0300
 * @tc.name      : UtilsSetValue parameter legal test when key contains only number
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue003, Function | MediumTest | Level1)
{
    char key[] = "100";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_0400
 * @tc.name      : UtilsSetValue parameter legal test when key is equal to 31 Byte
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue004, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version.utilskvparameter";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_0500
 * @tc.name      : UtilsSetValue parameter Illegal test when key is equal to 32 Byte
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue005, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version.utilskvparameter1";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_0600
 * @tc.name      : UtilsSetValue parameter Illegal test when key is gteater than 32 Byte
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue006, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version.utilskvparameterforillegal";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_0700
 * @tc.name      : UtilsSetValue parameter Illegal test when key contains uppercase
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue007, Function | MediumTest | Level1)
{
    char key[] = "Rw.sys.version";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_0800
 * @tc.name      : UtilsSetValue parameter Illegal test when key contains horizontal line
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue008, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version-r3";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_0900
 * @tc.name      : UtilsSetValue parameter Illegal test when key contains plus sign
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue009, Function | MediumTest | Level1)
{
    char key[] = "re+r3";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_1000
 * @tc.name      : UtilsSetValue parameter Illegal test when key contains asterisk
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue010, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version*r3";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_1100
 * @tc.name      : UtilsSetValue parameter legal test when Value is equal to 127 characters
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue011, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "Two tigers Two tigers two tiger running so fast \
running so fast one has no ears one has no tail How strange How strangesleeping";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_1200
 * @tc.name      : UtilsSetValue parameter Illegal test when Value is equal to 128 characters
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue012, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "Two tigers Two tigers two tiger running so fast \
running so fast one has no ears one has no tail How strange How strange sleeping";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_1300
 * @tc.name      : UtilsSetValue parameter Illegal test when Value greater than 128 characters
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue013, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "Two tigers Two tigers two tiger running so fast \
running so fast one has no ears one has no tail How strange How strange  Are you sleeping";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_1400
 * @tc.name      : Value greater than 128 characters and key is an invalid character
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue014, Function | MediumTest | Level1)
{
    char key[] = "Rw.sys.version";
    char value[] = "Two tigers Two tigers two tiger running so fast \
running so fast one has no ears one has no tail How strange How strange  Are you sleeping";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_1500
 * @tc.name      : UtilsSetValue parameter legal test with Special characters
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue015, Function | MediumTest | Level1)
{
    char key[] = "_._..__...___";
    char value[] = "！@#￥%……&*（）——+~《》？，。、“‘；：、12345767890";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_1600
 * @tc.name      : UtilsSetValue parameter Illegal test when key contains blank
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue016, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version space test";
    char value[] = "Hello world !";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_1700
 * @tc.name      : UtilsSetValue parameter legal test when value contains only blank
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreSetValue017, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "                                                   ";
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_1800
 * @tc.name      : Use the interface to get the kv value or cache
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreGetValue001, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "It is never too old to learn";
    char temp[MAX_VALUE_LEN_TEST] = {0};
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsGetValue(key, temp, MAX_VALUE_LEN_TEST);
#ifdef FEATURE_KV_CACHE
        TEST_ASSERT_EQUAL_INT(0, ret);
#else
        TEST_ASSERT_GREATER_THAN_INT(0, ret);
#endif
    TEST_ASSERT_EQUAL_STRING(value, temp);
    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_1900
 * @tc.name      : Use the interface to get the kv value or cache when key contains only number
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreGetValue002, Function | MediumTest | Level1)
{
    char key[] = "100";
    char value[] = "！@#￥%……&*（）——+~《》？，。、“‘；：、12345767890";
    char temp[MAX_VALUE_LEN_TEST] = {0};
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsGetValue(key, temp, MAX_VALUE_LEN_TEST);
#ifdef FEATURE_KV_CACHE
        TEST_ASSERT_EQUAL_INT(0, ret);
#else
        TEST_ASSERT_GREATER_THAN_INT(0, ret);
#endif
    TEST_ASSERT_EQUAL_STRING(value, temp);
    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_2000
 * @tc.name      : Use the interface to get the kv value or cache after value is updated
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreGetValue003, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version.utilskvparameter";
    char value[] = "It is never too old to learn";
    char temp[MAX_VALUE_LEN_TEST] = {0};
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    // Update the value of key
    char value1[] = "Two tigers Two tigers two tiger running so fast \
running so fast one has no ears one has no tail How strange How strangesleeping";
    ret = UtilsSetValue(key, value1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsGetValue(key, temp, MAX_VALUE_LEN_TEST);
#ifdef FEATURE_KV_CACHE
        TEST_ASSERT_EQUAL_INT(0, ret);
#else
        TEST_ASSERT_GREATER_THAN_INT(0, ret);
#endif
    TEST_ASSERT_EQUAL_STRING(value1, temp);
    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_2100
 * @tc.name      : UtilsGetValue parameter Illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreGetValue004, Function | MediumTest | Level1)
{
    char key[] = "Rw.sys.version";
    char temp[MAX_VALUE_LEN_TEST] = {0};
    int ret = UtilsGetValue(key, temp, MAX_VALUE_LEN_TEST);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_2200
 * @tc.name      : Get the kv value or cache when key contains only special characters and value contains only blank
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreGetValue005, Function | MediumTest | Level1)
{
    char key[] = "_._..__...___";
    char value[] = "                                                 ";
    char temp[MAX_VALUE_LEN_TEST] = {0};
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = UtilsGetValue(key, temp, MAX_VALUE_LEN_TEST);
#ifdef FEATURE_KV_CACHE
        TEST_ASSERT_EQUAL_INT(0, ret);
#else
        TEST_ASSERT_GREATER_THAN_INT(0, ret);
#endif
    TEST_ASSERT_EQUAL_STRING(value, temp);
    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_2300
 * @tc.name      : UtilsDeleteValue parameter Illegal test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreDeleteValue, Function | MediumTest | Level1)
{
    char key[] = "Rw.sys.version";
    int ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(EC_INVALID, ret);
};

#ifdef FEATURE_KV_CACHE
/**
 * @tc.number    : SUB_UTILS_KV_STORE_2400
 * @tc.name      : Use the interface to clear cache
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreClearCache001, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "It is never too old to learn";
    char temp[MAX_VALUE_LEN_TEST] = {0};
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);
    // Get the value of key
    ret = UtilsGetValue(key, temp, MAX_VALUE_LEN_TEST);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_STRING(value, temp);
    // Clear cache
    ret = ClearKVCache();
    TEST_ASSERT_EQUAL_INT(0, ret);
    // Get the value of key
    memset_s(temp, MAX_VALUE_LEN_TEST, 0, MAX_VALUE_LEN_TEST);
    ret = UtilsGetValue(key, temp, MAX_VALUE_LEN_TEST);
    TEST_ASSERT_GREATER_THAN_INT(0, ret);
    TEST_ASSERT_EQUAL_STRING(value, temp);
    // Clear key
    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_2500
 * @tc.name      : Use the interface to clear cache after updating operation
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreClearCache002, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "It is never too old to learn";
    char temp[MAX_VALUE_LEN_TEST] = {0};
    int ret = UtilsSetValue(key, value);
    TEST_ASSERT_EQUAL_INT(0, ret);

    // Update the value of key
    char value1[] = "Two tigers,Two tigers,two tiger,running so fast";
    ret = UtilsSetValue(key, value1);
    TEST_ASSERT_EQUAL_INT(0, ret);
    // Get the value of key
    ret = UtilsGetValue(key, temp, MAX_VALUE_LEN_TEST);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_STRING(value1, temp);
    // Clear cache
    ret = ClearKVCache();
    TEST_ASSERT_EQUAL_INT(0, ret);
    // Get the value of key
    memset_s(temp, MAX_VALUE_LEN_TEST, 0, MAX_VALUE_LEN_TEST);
    ret = UtilsGetValue(key, temp, MAX_VALUE_LEN_TEST);
    TEST_ASSERT_GREATER_THAN_INT(0, ret);
    TEST_ASSERT_EQUAL_STRING(value1, temp);
    // Clear key
    ret = UtilsDeleteValue(key);
    TEST_ASSERT_EQUAL_INT(0, ret);
};
#endif

/**
 * @tc.number    : SUB_UTILS_KV_STORE_2600
 * @tc.name      : Specification test when MaxCacheNum is 9
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreCacheSize001, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "It is never too old to learn";
    BOOL ret = FALSE;

    ret = SetKVFiles(MAX_CACHE_NUM_TEST-1, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = ReadKVFiles(MAX_CACHE_NUM_TEST-1, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = DeleteKVFiles(MAX_CACHE_NUM_TEST-1, key);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_2700
 * @tc.name      : Specification test when MaxCacheNum is 10
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreCacheSize002, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "It is never too old to learn";
    BOOL ret = FALSE;

    ret = SetKVFiles(MAX_CACHE_NUM_TEST, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = ReadKVFiles(MAX_CACHE_NUM_TEST, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = DeleteKVFiles(MAX_CACHE_NUM_TEST, key);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_2800
 * @tc.name      : Specification test when MaxCacheNum is 11
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreCacheSize003, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "It is never too old to learn";
    BOOL ret = FALSE;

    ret = SetKVFiles(MAX_CACHE_NUM_TEST+1, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = ReadKVFiles(MAX_CACHE_NUM_TEST+1, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = DeleteKVFiles(MAX_CACHE_NUM_TEST+1, key);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_2900
 * @tc.name      : Specification test when MaxKeyNum is 49
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreMaxSize001, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "It is never too old to learn";
    BOOL ret = FALSE;

    ret = SetKVFiles(MAX_KEY_NUM_TEST-1, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = ReadKVFiles(MAX_KEY_NUM_TEST-1, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = DeleteKVFiles(MAX_KEY_NUM_TEST-1, key);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_3000
 * @tc.name      : Specification test when MaxKeyNum is 50
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreMaxSize002, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "It is never too old to learn";
    BOOL ret = FALSE;

    ret = SetKVFiles(MAX_KEY_NUM_TEST, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = ReadKVFiles(MAX_KEY_NUM_TEST, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = DeleteKVFiles(MAX_KEY_NUM_TEST, key);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_3100
 * @tc.name      : Specification test when MaxKeyNum is 51
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreMaxSize003, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "It is never too old to learn";
    BOOL ret = FALSE;

    ret = SetKVFiles(MAX_KEY_NUM_TEST+1, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = ReadKVFiles(MAX_KEY_NUM_TEST+1, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = DeleteKVFiles(MAX_KEY_NUM_TEST+1, key);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
};

/**
 * @tc.number    : SUB_UTILS_KV_STORE_3200
 * @tc.name      : Specification test using InvalidKeyNum
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(KvStoreFuncTestSuite, testKvStoreMaxSize004, Function | MediumTest | Level1)
{
    char key[] = "rw.sys.version";
    char value[] = "It is never too old to learn";
    char value1[] = "Two tigers Two tigers two tiger running so fast \
running so fast one has no ears one has no tail How strange How strange";
    BOOL ret = FALSE;

    ret = SetKVFiles(INVALID_KEY_NUM, key, value);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    // Update operation
    ret = SetKVFiles(INVALID_KEY_NUM, key, value1);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = ReadKVFiles(INVALID_KEY_NUM, key, value1);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
    ret = DeleteKVFiles(INVALID_KEY_NUM, key);
    if (ret != TRUE)
    {
        TEST_FAIL();
    }
};

RUN_TEST_SUITE(KvStoreFuncTestSuite);
