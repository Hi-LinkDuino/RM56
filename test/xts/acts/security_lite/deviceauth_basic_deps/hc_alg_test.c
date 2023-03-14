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

#include "hc_alg_test.h"

#include <alg_loader.h>
#include <device_auth.h>
#include <hal_error.h>
#include <hctest.h>
#include <stdlib.h>
#include <string_util.h>
#include <unistd.h>

#include "hc_alg_test_case.h"
#include "print_log.h"
#include "test_timer.h"

#define LOG_ERROR_IF_POINTER_NULL(pointer) LOGE("Pointer %s is %s", #pointer, ((pointer) == NULL ? "NULL" : "not NULL"))

#ifdef __cplusplus
extern "C" {
#endif

static uint32_t CountZero(const uint8_t *buffer, uint32_t length)
{
    uint32_t zeroCount = 0;
    for (uint32_t i = 0; i < length; ++i) {
        if (buffer[i] == 0) {
            ++zeroCount;
        }
    }
    return zeroCount;
}

static void FillRandom(uint8_t *msg, uint32_t setZeroLength, uint32_t randomLength)
{
    if (randomLength < 1) {
        return;
    }
    int res;
    res = memset_s(msg, setZeroLength, 0, setZeroLength);
    TEST_ASSERT_EQUAL(EOK, res);

    msg[0] = rand() % (UINT8_MAX - 1) + 1;
    for (uint32_t i = 1; i < randomLength; ++i) {
        msg[i] = rand() % UINT8_MAX;
    }
    enum {
        // ignore rand correctness if the random length is equal or less than 3
        RANDOM_CONSECUTIVE_ZERO_LENGTH_LIMIT = 3,
    };
    if (randomLength <= RANDOM_CONSECUTIVE_ZERO_LENGTH_LIMIT + 1) {
        return;
    }
    TEST_ASSERT_NOT_EQUAL(randomLength - 1, CountZero(msg + 1, randomLength - 1));
}

// Note: sha256 and sha256Compare will be set to all zero
static void TestSha256Inner(
    const AlgLoader *loader, Uint8Buff *msg, Uint8Buff *sha256, Uint8Buff *sha256Compare)
{
    int res;
    res = memset_s(sha256->val, SHA256_LEN, 0, SHA256_LEN);
    TEST_ASSERT_EQUAL(EOK, res);
    res = memset_s(sha256Compare->val, SHA256_LEN, 0, SHA256_LEN);
    TEST_ASSERT_EQUAL(EOK, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, loader->sha256(msg, sha256));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
    TEST_ASSERT_EQUAL((uint32_t)SHA256_LEN, sha256->length);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->sha256(msg, sha256Compare));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
    TEST_ASSERT_EQUAL((uint32_t)SHA256_LEN, sha256Compare->length);

    uint32_t zeroCount = CountZero(sha256->val, sha256->length);
    TEST_ASSERT_NOT_EQUAL_MESSAGE((uint32_t)SHA256_LEN, zeroCount, "invalid all zero sha256");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(sha256->val, sha256Compare->val, SHA256_LEN,
        "different sha256 against the same message");
}

static void TestSha256(const AlgLoader *loader)
{
    TEST_ASSERT_NOT_NULL(loader->sha256);
    if (loader->sha256 == NULL) {
        LOGE("sha256 pointer is NULL! will not test!");
        return;
    }

    Uint8Buff msg = { (uint8_t *)malloc(SHA256_MSG_MAX_SIZE), SHA256_MSG_MAX_SIZE };
    TEST_ASSERT_NOT_NULL(msg.val);
    Uint8Buff sha256 = { (uint8_t *)malloc(SHA256_LEN), SHA256_LEN };
    TEST_ASSERT_NOT_NULL(sha256.val);
    Uint8Buff sha256Compare = { (uint8_t *)malloc(SHA256_LEN), SHA256_LEN };
    TEST_ASSERT_NOT_NULL(sha256Compare.val);

    for (int i = 1; i <= SHA256_TEST_MIN_LIMIT; ++i) {
        msg.length = i;
        LOGI("test sha256 with msg length %d", msg.length);
        FillRandom(msg.val, SHA256_MSG_MAX_SIZE, msg.length);
        TestSha256Inner(loader, &msg, &sha256, &sha256Compare);
    }

    msg.length = SHA256_MSG_MAX_SIZE;
    LOGI("test sha256 with max msg length %d", msg.length);
    FillRandom(msg.val, SHA256_MSG_MAX_SIZE, msg.length);
    TestSha256Inner(loader, &msg, &sha256, &sha256Compare);

    for (int i = 0; i < SHA256_TEST_TIMES; ++i) {
        msg.length = rand() % SHA256_MSG_MAX_SIZE + 1;
        LOGI("test sha256 with random msg length %d", msg.length);
        FillRandom(msg.val, SHA256_MSG_MAX_SIZE, msg.length);
        TestSha256Inner(loader, &msg, &sha256, &sha256Compare);
    }

    msg.val = (uint8_t *)(SHA256_EXAMPLE_MESSAGE);
    msg.length = strlen(SHA256_EXAMPLE_MESSAGE);
    TestSha256Inner(loader, &msg, &sha256, &sha256Compare);
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(sha256.val, SHA256_EXAMPLE_RESULT, SHA256_LEN,
        "incorrect sha256 result");
    PrintBuffer(SHA256_EXAMPLE_MESSAGE, sha256.val, sha256.length);

    free(msg.val);
    free(sha256.val);
    free(sha256Compare.val);
}

static void TestGenerateRandomInner(const AlgLoader *loader, Uint8Buff *random)
{
    uint32_t randomLength = random->length;
    int res;
    res = memset_s(random->val, GEN_RANDOM_MAX_SIZE, 0, GEN_RANDOM_MAX_SIZE);
    TEST_ASSERT_EQUAL(EOK, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, loader->generateRandom(random));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
    TEST_ASSERT_EQUAL(randomLength, random->length);

    uint32_t zeroCount = CountZero(random->val, random->length);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(random->length, zeroCount, "random all zero");
}

static void TestGenerateRandomNoPeriodicRepetition(const AlgLoader *loader,
    Uint8Buff *previous,
    Uint8Buff *current)
{
    int res;
    res = memset_s(previous->val, GEN_RANDOM_TEST_REPEAT_SIZE, 0, GEN_RANDOM_TEST_REPEAT_SIZE);
    TEST_ASSERT_EQUAL(EOK, res);
    for (int i = 0; i < GEN_RANDOM_TEST_REPEAT_TIMES; ++i) {
        res = memset_s(current->val, GEN_RANDOM_TEST_REPEAT_SIZE, 0, GEN_RANDOM_TEST_REPEAT_SIZE);
        TEST_ASSERT_EQUAL(EOK, res);
        RUN_AND_PRINT_ELAPSED_TIME(res, loader->generateRandom(current));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
        TEST_ASSERT_EQUAL(GEN_RANDOM_TEST_REPEAT_SIZE, current->length);
        int equalCount = 0;
        for (int j = 0; j < GEN_RANDOM_TEST_REPEAT_SIZE; ++j) {
            if (previous->val[j] == current->val[j]) {
                ++equalCount;
            }
        }
        TEST_ASSERT_NOT_EQUAL_MESSAGE(GEN_RANDOM_TEST_REPEAT_SIZE, equalCount,
            "random is the same as previous generated");
        res = memcpy_s(previous->val, GEN_RANDOM_TEST_REPEAT_SIZE, current->val, GEN_RANDOM_TEST_REPEAT_SIZE);
        TEST_ASSERT_EQUAL(EOK, res);
    }
}

static void TestGenerateRandom(const AlgLoader *loader)
{
    TEST_ASSERT_NOT_NULL(loader->generateRandom);
    if (loader->generateRandom == NULL) {
        LOGE("generateRandom pointer is NULL! will not test!");
        return;
    }

    Uint8Buff random = { (uint8_t *)malloc(GEN_RANDOM_MAX_SIZE), GEN_RANDOM_MAX_SIZE };
    TEST_ASSERT_NOT_NULL(random.val);

    for (int i = 1; i <= GEN_RANDOM_MIN_LIMIT; ++i) {
        random.length = i;
        LOGI("test generate random with length %d", random.length);
        TestGenerateRandomInner(loader, &random);
    }

    random.length = GEN_RANDOM_MAX_SIZE;
    LOGI("test generate random with max length %d", random.length);
    TestGenerateRandomInner(loader, &random);

    for (int i = 0; i < GEN_RANDOM_TEST_TIMES; ++i) {
        random.length = rand() % GEN_RANDOM_MAX_SIZE + 1;
        LOGI("test generate random with random length %d", random.length);
        TestGenerateRandomInner(loader, &random);
    }

    // test whether the random number is repeated
    Uint8Buff previousUint8Buff = { (uint8_t *)malloc(GEN_RANDOM_TEST_REPEAT_SIZE), GEN_RANDOM_TEST_REPEAT_SIZE };
    TEST_ASSERT_NOT_NULL(previousUint8Buff.val);
    Uint8Buff currentUint8Buff = { (uint8_t *)malloc(GEN_RANDOM_TEST_REPEAT_SIZE), GEN_RANDOM_TEST_REPEAT_SIZE };
    TEST_ASSERT_NOT_NULL(currentUint8Buff.val);

    TestGenerateRandomNoPeriodicRepetition(loader, &previousUint8Buff, &currentUint8Buff);

    free(currentUint8Buff.val);
    free(previousUint8Buff.val);
    free(random.val);
}

// key will not be modified
// message will be malloced if message->val is NULL
// hmac will be clear with zero
static void TestComputeHmacInner(
    const AlgLoader *loader, const Uint8Buff *key, Uint8Buff *msg, Uint8Buff *hmac, bool isAlias)
{
    int32_t res;
    bool allocMemory = false;
    if (msg->val == NULL) {
        msg->val = (uint8_t *)malloc(msg->length);
        TEST_ASSERT_NOT_NULL(msg->val);
        allocMemory = true;
        res = memset_s(msg->val, msg->length, 0, msg->length);
        TEST_ASSERT_EQUAL(EOK, res);
        FillRandom(msg->val, msg->length, msg->length);
    }
    res = memset_s(hmac->val, hmac->length, 0, hmac->length);
    TEST_ASSERT_EQUAL(EOK, res);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->computeHmac(key, msg, hmac, isAlias));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);

    uint32_t zeroCount = CountZero(hmac->val, hmac->length);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(hmac->length, zeroCount, "invalid all zero hmac");

    if (allocMemory && msg->val != NULL) {
        free(msg->val);
        msg->val = NULL;
    }
}

static void TestComputeHmacWithIsAliasTrue(const AlgLoader *loader,
    Uint8Buff *msg, Uint8Buff *hmacKeyBuff, Uint8Buff *outHmacBuff)
#if TEST_COMPUTEHMAC_WITH_ISALIAS_TRUE
{
    TEST_ASSERT_NOT_NULL(loader->importSymmetricKey);
    if (!loader->importSymmetricKey) {
        LOGE("importSymmetricKey is NULL, can not test computeHmac with isAlias true");
        return;
    }
    LOGI("test hmac for isAlias true");
    msg->val = (uint8_t *)NULL;
    msg->length = TEST_HMAC_ISALIAS_TRUE_MSG_LENGTH;
    FillRandom(hmacKeyBuff->val, hmacKeyBuff->length, hmacKeyBuff->length);
    // write key with specified alias first, then do the test
    const ExtraInfo info = {
        .authId = *hmacKeyBuff,
        .userType = DEVICE_TYPE_ACCESSORY,
        .pairType = PAIR_TYPE_BIND,
    };
    int res = loader->importSymmetricKey(hmacKeyBuff, hmacKeyBuff, KEY_PURPOSE_MAC, &info);
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
    TestComputeHmacInner(loader, hmacKeyBuff, msg, outHmacBuff, true);
    // delete key after the test
    if (loader->deleteKey) {
        LOGD("test deleteKey");
        res = loader->deleteKey(hmacKeyBuff);
        TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
    }
}
#else // TEST_COMPUTEHMAC_WITH_ISALIAS_TRUE
{
    LOGE("no TEST_COMPUTEHMAC_WITH_ISALIAS_TRUE, do not test computeHmac with isAlias true");
#if (defined(DO_NOT_TEST_DEPRECATED_IMPORT_SYMMETRIC_KEY) && DO_NOT_TEST_DEPRECATED_IMPORT_SYMMETRIC_KEY)
    (void)(loader);
#else // DO_NOT_TEST_DEPRECATED_IMPORT_SYMMETRIC_KEY
    TEST_ASSERT_NULL(loader->importSymmetricKey);
#endif // DO_NOT_TEST_DEPRECATED_IMPORT_SYMMETRIC_KEY
    (void)(msg);
    (void)(hmacKeyBuff);
    (void)(outHmacBuff);
}
#endif // TEST_COMPUTEHMAC_WITH_ISALIAS_TRUE

static void TestComputeHmac(const AlgLoader *loader)
{
    TEST_ASSERT_NOT_NULL(loader->computeHmac);
    if (loader->computeHmac == NULL) {
        LOGE("computeHmac pointer is NULL! will not test!");
        return;
    }

    Uint8Buff hmacKeyBuff = { (uint8_t *)malloc(TEST_HMAC_KEY_LEN), TEST_HMAC_KEY_LEN };
    TEST_ASSERT_NOT_NULL(hmacKeyBuff.val);
    Uint8Buff msg = { (uint8_t *)NULL, 0 };
    uint8_t *outHmac = (uint8_t *)malloc(HMAC_LEN);
    TEST_ASSERT_NOT_NULL(outHmac);
    Uint8Buff outHmacBuff = { (uint8_t *)outHmac, HMAC_LEN };

    TestComputeHmacWithIsAliasTrue(loader, &msg, &hmacKeyBuff, &outHmacBuff);

    LOGI("test hmac for isAlias false");
    for (uint32_t i = 0; i < ARRAY_SIZE(TEST_HMAC_ISALIAS_FALSE_MSG_LENGTH_ARRAY); ++i) {
        msg.val = (uint8_t *)NULL;
        msg.length = TEST_HMAC_ISALIAS_FALSE_MSG_LENGTH_ARRAY[i];
        LOGI("test hmac for msg length %d", msg.length);
        FillRandom(hmacKeyBuff.val, hmacKeyBuff.length, hmacKeyBuff.length);
        TestComputeHmacInner(loader, &hmacKeyBuff, &msg, &outHmacBuff, false);
    }

    free(hmacKeyBuff.val);
    hmacKeyBuff.val = (uint8_t *)NULL;

    LOGI("test the correctness of hmac");
    hmacKeyBuff.val = (uint8_t *)(HMAC_EXAMPLE_KEY),
    hmacKeyBuff.length = sizeof(HMAC_EXAMPLE_KEY);
    msg.val = (uint8_t *)(HMAC_EXAMPLE_MESSAGE),
    msg.length = strlen(HMAC_EXAMPLE_MESSAGE);
    TestComputeHmacInner(loader, &hmacKeyBuff, &msg, &outHmacBuff, false);
    TEST_ASSERT_EQUAL(sizeof(HMAC_EXAMPLE_RESULT), outHmacBuff.length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(HMAC_EXAMPLE_RESULT, outHmacBuff.val, sizeof(HMAC_EXAMPLE_RESULT),
        "incorrect hmac result");
    PrintBuffer("hmac", outHmacBuff.val, outHmacBuff.length);

    free(outHmac);
    outHmac = NULL;
    outHmacBuff.val = (uint8_t *)NULL;
}

struct HkdfArgument {
    Uint8Buff baseKey;
    Uint8Buff salt;
    Uint8Buff keyInfo;
    Uint8Buff outHkdf;
    bool isAlias;
};

// baseKey, salt will be malloced and fill random if NULL
// keyInfo, isAlias will not be modified
// outHkdf will be clear with zero
static void TestComputeHkdfInner(
    const AlgLoader *loader, struct HkdfArgument *arg)
{
    bool baseAlloced = false, saltAlloced = false;
    if (arg->baseKey.val == NULL) {
        arg->baseKey.val = (uint8_t *)malloc(arg->baseKey.length);
        TEST_ASSERT_NOT_NULL(arg->baseKey.val);
        baseAlloced = true;
        FillRandom(arg->baseKey.val, arg->baseKey.length, arg->baseKey.length);
    }
    if (arg->salt.val == NULL) {
        arg->salt.val = (uint8_t *)malloc(arg->salt.length);
        TEST_ASSERT_NOT_NULL(arg->salt.val);
        saltAlloced = true;
        FillRandom(arg->salt.val, arg->salt.length, arg->salt.length);
    }
    int32_t res;
    res = memset_s(arg->outHkdf.val, arg->outHkdf.length, 0, arg->outHkdf.length);
    TEST_ASSERT_EQUAL(EOK, res);
    RUN_AND_PRINT_ELAPSED_TIME(
        res,
        loader->computeHkdf(
            &arg->baseKey, &arg->salt, &arg->keyInfo, &arg->outHkdf, arg->isAlias));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
    if (saltAlloced && arg->salt.val != NULL) {
        free(arg->salt.val);
        arg->salt.val = (uint8_t *)NULL;
    }
    if (baseAlloced && arg->baseKey.val != NULL) {
        free(arg->baseKey.val);
        arg->baseKey.val = (uint8_t *)NULL;
    }

    uint32_t zeroCount = CountZero(arg->outHkdf.val, arg->outHkdf.length);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(arg->outHkdf.length, zeroCount, "invalid all zero hkdf result");
}

#if TEST_HKDF_WITH_KEY_ALIAS_TRUE // {
static void TestComputeHkdfWithKeyAliasTrueFirstAndSecondStep(
    const AlgLoader *loader,
    Uint8Buff *privKeyPairAlias,
    Uint8Buff *pubKeyPairAlias,
    ExtraInfo *extraInfo,
    Uint8Buff *sharedKeyAlias)
{
    LOGI("First, generate two pairs of public-private key pairs");
    TEST_ASSERT_EQUAL(HAL_SUCCESS,
        loader->generateKeyPairWithStorage(privKeyPairAlias,
            TEST_COMPUTE_HKDF_WITH_KEY_ALIAS_TRUE_KEY_PAIR_KEY_ALIAS_LENGTH, ED25519, extraInfo));
    TEST_ASSERT_EQUAL(HAL_SUCCESS,
        loader->generateKeyPairWithStorage(pubKeyPairAlias,
            TEST_COMPUTE_HKDF_WITH_KEY_ALIAS_TRUE_KEY_PAIR_KEY_ALIAS_LENGTH, ED25519, extraInfo));

    LOGI("Second, agree shared key with privateKey and publicKey");
    KeyBuff privKeyBuff = { privKeyPairAlias->val, privKeyPairAlias->length, true };
    KeyBuff pubKeyBuff = { pubKeyPairAlias->val, pubKeyPairAlias->length, true };
    TEST_ASSERT_EQUAL(HAL_SUCCESS,
        loader->agreeSharedSecretWithStorage(&privKeyBuff, &pubKeyBuff, ED25519,
            TEST_COMPUTE_HKDF_WITH_KEY_ALIAS_TRUE_SHARED_KEY_LENGTH, sharedKeyAlias));
}

static void TestComputeHkdfWithKeyAliasTrueThirdStep(
    const AlgLoader *loader,
    struct HkdfArgument *argument,
    Uint8Buff *sharedKeyAlias)
{
    LOGI("Third, compute hkdf with shared key");
    argument->baseKey = *sharedKeyAlias;
    argument->salt.val = (uint8_t *)NULL;
    argument->salt.length = HKDF_PAKE_PSK_DERIVE_SECRET_ALIAS_SALT_LEN;
    argument->keyInfo.val = (uint8_t *)(TMP_AUTH_KEY_FACTOR);
    argument->keyInfo.length = strlen(TMP_AUTH_KEY_FACTOR);
    argument->outHkdf.val = (uint8_t *)malloc(HKDF_PAKE_PSK_DERIVE_SECRET_ALIAS_OUTKEY_LEN);
    TEST_ASSERT_NOT_NULL(argument->outHkdf.val);
    argument->outHkdf.length = HKDF_PAKE_PSK_DERIVE_SECRET_ALIAS_OUTKEY_LEN;
    argument->isAlias = true;
    TestComputeHkdfInner(loader, argument);
    free(argument->outHkdf.val);
    argument->outHkdf.val = (uint8_t *)NULL;
}

static void TestComputeHkdfWithKeyAliasTrue(
    const AlgLoader *loader,
    struct HkdfArgument *argument)
{
    LOGI("pake psk derive secret keyAlias true");
    TEST_ASSERT_NOT_NULL(loader->generateKeyPairWithStorage);
    TEST_ASSERT_NOT_NULL(loader->agreeSharedSecretWithStorage);
    if (!loader->generateKeyPairWithStorage || !loader->agreeSharedSecretWithStorage) {
        LOGE("generateKeyPairWithStorage or agreeSharedSecretWithStorage pointer is NULL! "
            "can not test hkdf with alias true!");
        return;
    }

    Uint8Buff privKeyPairAlias = { (uint8_t *)malloc(TEST_COMPUTE_HKDF_WITH_KEY_ALIAS_TRUE_KEY_PAIR_KEY_ALIAS_LENGTH),
        TEST_COMPUTE_HKDF_WITH_KEY_ALIAS_TRUE_KEY_PAIR_KEY_ALIAS_LENGTH };
    Uint8Buff pubKeyPairAlias = { (uint8_t *)malloc(TEST_COMPUTE_HKDF_WITH_KEY_ALIAS_TRUE_KEY_PAIR_KEY_ALIAS_LENGTH),
        TEST_COMPUTE_HKDF_WITH_KEY_ALIAS_TRUE_KEY_PAIR_KEY_ALIAS_LENGTH };
    ExtraInfo extraInfo = { { (uint8_t *)malloc(TEST_COMPUTE_HKDF_WITH_KEY_ALIAS_TRUE_AUTH_ID_LENGTH),
        TEST_COMPUTE_HKDF_WITH_KEY_ALIAS_TRUE_AUTH_ID_LENGTH },
        DEVICE_TYPE_ACCESSORY, PAIR_TYPE_BIND };
    TEST_ASSERT_NOT_NULL(privKeyPairAlias.val);
    TEST_ASSERT_NOT_NULL(pubKeyPairAlias.val);
    TEST_ASSERT_NOT_NULL(extraInfo.authId.val);
    FillRandom(privKeyPairAlias.val, privKeyPairAlias.length, privKeyPairAlias.length);
    FillRandom(pubKeyPairAlias.val, pubKeyPairAlias.length, pubKeyPairAlias.length);
    FillRandom(extraInfo.authId.val, extraInfo.authId.length, extraInfo.authId.length);
    Uint8Buff sharedKeyAlias = { (uint8_t *)malloc(HKDF_PAKE_PSK_DERIVE_SECRET_ALIAS_BASEKEY_LEN),
        HKDF_PAKE_PSK_DERIVE_SECRET_ALIAS_BASEKEY_LEN };
    TEST_ASSERT_NOT_NULL(sharedKeyAlias.val);
    FillRandom(sharedKeyAlias.val, sharedKeyAlias.length, sharedKeyAlias.length);

    TestComputeHkdfWithKeyAliasTrueFirstAndSecondStep(
        loader, &privKeyPairAlias, &pubKeyPairAlias, &extraInfo, &sharedKeyAlias);
    TestComputeHkdfWithKeyAliasTrueThirdStep(
        loader, argument, &sharedKeyAlias);
    if (loader->deleteKey) {
        LOGD("test deleteKey");
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->deleteKey(&privKeyPairAlias));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->deleteKey(&pubKeyPairAlias));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->deleteKey(&sharedKeyAlias));
    }

    LOGD("At last, free all memories allocated");
    free(privKeyPairAlias.val);
    privKeyPairAlias.val = (uint8_t *)NULL;
    free(pubKeyPairAlias.val);
    pubKeyPairAlias.val = (uint8_t *)NULL;
    free(extraInfo.authId.val);
    extraInfo.authId.val = (uint8_t *)NULL;
    free(sharedKeyAlias.val);
    sharedKeyAlias.val = (uint8_t *)NULL;
}
#else  // TEST_HKDF_WITH_KEY_ALIAS_TRUE // } {
static void TestComputeHkdfWithKeyAliasTrue(
    const AlgLoader *loader,
    struct HkdfArgument *argument)
{
    LOGE("no TEST_HKDF_WITH_KEY_ALIAS_TRUE, do not test hkdf with key alias true");
    TEST_ASSERT_TRUE(loader->generateKeyPairWithStorage == NULL || loader->agreeSharedSecretWithStorage == NULL);
    LOG_ERROR_IF_POINTER_NULL(loader->generateKeyPairWithStorage);
    LOG_ERROR_IF_POINTER_NULL(loader->agreeSharedSecretWithStorage);
    (void)(argument);
}
#endif // TEST_HKDF_WITH_KEY_ALIAS_TRUE // }

static void TestComputeHkdfCorrectness(
    const AlgLoader *loader,
    struct HkdfArgument *argument)
{
    LOGI("test the correctness of hkdf");
    argument->baseKey.val = (uint8_t *)(HKDF_EXAMPLE_MESSAGE);
    argument->baseKey.length = strlen(HKDF_EXAMPLE_MESSAGE);
    argument->salt.val = (uint8_t *)(HKDF_EXAMPLE_SALT);
    argument->salt.length = sizeof(HKDF_EXAMPLE_SALT);
    argument->keyInfo.val = (uint8_t *)(HKDF_EXAMPLE_INFO);
    argument->keyInfo.length = strlen(HKDF_EXAMPLE_INFO);
    argument->outHkdf.val = (uint8_t *)malloc(sizeof(HKDF_EXAMPLE_RESULT_KEY));
    TEST_ASSERT_NOT_NULL(argument->outHkdf.val);
    argument->outHkdf.length = sizeof(HKDF_EXAMPLE_RESULT_KEY);
    argument->isAlias = false;
    TestComputeHkdfInner(loader, argument);
    TEST_ASSERT_EQUAL(sizeof(HKDF_EXAMPLE_RESULT_KEY), argument->outHkdf.length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(HKDF_EXAMPLE_RESULT_KEY, argument->outHkdf.val,
        sizeof(HKDF_EXAMPLE_RESULT_KEY), "incorrect hkdf result");
    PrintBuffer("hkdf result", argument->outHkdf.val, argument->outHkdf.length);
    free(argument->outHkdf.val);
    argument->outHkdf.val = (uint8_t *)NULL;
}

static void TestComputeHkdf(const AlgLoader *loader)
{
    TEST_ASSERT_NOT_NULL(loader->computeHkdf);
    if (loader->computeHkdf == NULL) {
        LOGE("computeHkdf pointer is NULL! will not test!");
        return;
    }

    struct HkdfArgument argument;
    int res;
    res = memset_s(&argument, sizeof(argument), 0, sizeof(argument));
    TEST_ASSERT_EQUAL(EOK, res);

    for (uint32_t i = 0; i < ARRAY_SIZE(g_hkdfTestCase); ++i) {
        LOGI("test case for: \"%s\"", g_hkdfTestCase[i].caseName);
        LOGI("test case for: baseKeyLength = %d, saltLength = %d, keyInfo = \"%s\", outKeyLength = %d",
            g_hkdfTestCase[i].baseKeyLength, g_hkdfTestCase[i].saltLength,
            g_hkdfTestCase[i].keyInfo, g_hkdfTestCase[i].outKeyLength);
        argument.baseKey.val = (uint8_t *)NULL;
        argument.baseKey.length = g_hkdfTestCase[i].baseKeyLength;
        argument.salt.val = (uint8_t *)NULL;
        argument.salt.length = g_hkdfTestCase[i].saltLength;
        argument.keyInfo.val = (uint8_t *)(g_hkdfTestCase[i].keyInfo);
        argument.keyInfo.length = strlen(g_hkdfTestCase[i].keyInfo);
        argument.outHkdf.val = (uint8_t *)malloc(g_hkdfTestCase[i].outKeyLength);
        TEST_ASSERT_NOT_NULL(argument.outHkdf.val);
        argument.outHkdf.length = g_hkdfTestCase[i].outKeyLength;
        argument.isAlias = false;
        TestComputeHkdfInner(loader, &argument);
        free(argument.outHkdf.val);
        argument.outHkdf.val = (uint8_t *)NULL;
    }

    TestComputeHkdfWithKeyAliasTrue(loader, &argument);

    TestComputeHkdfCorrectness(loader, &argument);
}

#if TEST_IMPORT_SYMMETRIC_KEY // {
enum {
    WAIT_FOR_WATCH_DOG  = 1,
};
static void TestImportSymmetricKey(const AlgLoader *loader)
{
    TEST_ASSERT_NOT_NULL(loader->importSymmetricKey);
    if (loader->importSymmetricKey == NULL) {
        LOGE("importSymmetricKey pointer is NULL! will not test!");
        return;
    }

    int res;
    const uint32_t authIdLenArray[] = { TEST_IMPORT_SYMMETRIC_KEY_AUTH_ID_LENGTH_32,
        TEST_IMPORT_SYMMETRIC_KEY_AUTH_ID_LENGTH_64 };
    for (uint32_t i = 0; i < ARRAY_SIZE(authIdLenArray); ++i) {
        for (
            uint32_t j = IMPORT_SYMMETRIC_KEY_KEYALIAS_LEN_MIN;
            j <= IMPORT_SYMMETRIC_KEY_KEYALIAS_LEN_MAX;
            ++j) {
            LOGD("auth id length = %u, msg length = %u", authIdLenArray[i], j);
            Uint8Buff keyAlias = { (uint8_t *)malloc(j), j };
            Uint8Buff authToken = { (uint8_t *)malloc(IMPORT_SYMMETRIC_KEY_AUTHTOKEN_LEN),
                IMPORT_SYMMETRIC_KEY_AUTHTOKEN_LEN };
            ExtraInfo extraInfo = { { (uint8_t *)malloc(authIdLenArray[i]), authIdLenArray[i] },
                DEVICE_TYPE_ACCESSORY, PAIR_TYPE_BIND };
            TEST_ASSERT_NOT_NULL(keyAlias.val);
            TEST_ASSERT_NOT_NULL(authToken.val);
            TEST_ASSERT_NOT_NULL(extraInfo.authId.val);
            FillRandom(keyAlias.val, keyAlias.length, keyAlias.length);
            FillRandom(authToken.val, authToken.length, authToken.length);
            FillRandom(extraInfo.authId.val, extraInfo.authId.length, extraInfo.authId.length);
            RUN_AND_PRINT_ELAPSED_TIME(res,
                loader->importSymmetricKey(&keyAlias, &authToken, KEY_PURPOSE_MAC, &extraInfo));
            TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
            if (loader->checkKeyExist) {
                LOGD("test checkKeyExist");
                RUN_AND_PRINT_ELAPSED_TIME(res, loader->checkKeyExist(&keyAlias));
                TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
            }
            if (loader->deleteKey) {
                LOGD("test delete");
                RUN_AND_PRINT_ELAPSED_TIME(res, loader->deleteKey(&keyAlias));
                TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
            }
            free(keyAlias.val);
            keyAlias.val = (uint8_t *)NULL;
            free(authToken.val);
            authToken.val = (uint8_t *)NULL;
            free(extraInfo.authId.val);
            extraInfo.authId.val = (uint8_t *)NULL;
        }
        LOGI("sleep for %d seconds, waiting for watch dog", WAIT_FOR_WATCH_DOG);
        sleep(WAIT_FOR_WATCH_DOG);
    }
}
#else  // TEST_IMPORT_SYMMETRIC_KEY // } {
static void TestImportSymmetricKey(const AlgLoader *loader)
{
    LOGE("no TEST_IMPORT_SYMMETRIC_KEY, do not test importSymmetricKey");
    TEST_ASSERT_NULL(loader->importSymmetricKey);
}
#endif // TEST_IMPORT_SYMMETRIC_KEY // }

static void TestCheckKeyExist(const AlgLoader *loader)
#if TEST_CHECK_KEY_EXIST
{
    TEST_ASSERT_NOT_NULL(loader->checkKeyExist);
    if (loader->checkKeyExist == NULL) {
        LOGE("checkKeyExist pointer is NULL! will not test!");
        return;
    }

    const char bufferKeyAlias[] = "A non-existent key";
    Uint8Buff keyAlias = {
        .val = (uint8_t *)bufferKeyAlias,
        .length = strlen(bufferKeyAlias),
    };
    int32_t res;
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->checkKeyExist(&keyAlias));
    TEST_ASSERT_NOT_EQUAL(HAL_SUCCESS, res);
}
#else // TEST_CHECK_KEY_EXIST
{
    LOGE("no TEST_CHECK_KEY_EXIST, do not test checkKeyExist");
    TEST_ASSERT_NULL(loader->checkKeyExist);
}
#endif // TEST_CHECK_KEY_EXIST

static void TestDeleteKey(const AlgLoader *loader)
#if TEST_DELETE_KEY
{
    TEST_ASSERT_NOT_NULL(loader->deleteKey);
    if (loader->deleteKey == NULL) {
        LOGE("deleteKey pointer is NULL! will not test!");
        return;
    }

    const char bufferKeyAlias[] = "A non-existent key";
    Uint8Buff keyAlias = {
        .val = (uint8_t *)bufferKeyAlias,
        .length = strlen(bufferKeyAlias),
    };
    int32_t res;
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->deleteKey(&keyAlias));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
}
#else // TEST_DELETE_KEY
{
    LOGE("no TEST_DELETE_KEY, do not test deleteKey");
    TEST_ASSERT_NULL(loader->deleteKey);
}
#endif // TEST_DELETE_KEY

static void TestAesGcmEncrypt(const AlgLoader *loader)
#if TEST_AES_GCM_ENCRYPT
{
    LOGI("begin to test the AES-GCM algorithm encryption");
    TEST_ASSERT_NOT_NULL(loader->aesGcmEncrypt);
    if (loader->aesGcmEncrypt == NULL) {
        LOGE("aesGcmEncrypt pointer is NULL! will not test!");
        return;
    }
    const Uint8Buff key = { (uint8_t *)AES_GCM_TEST_KEY, AES_GCM_KEY_LEN };
    const Uint8Buff plainBuffer = { (uint8_t *)PLAIN_CASE, PLAIN_LEN };

    uint8_t *nonce = (uint8_t *)malloc(NONCE_SIZE);
    TEST_ASSERT_NOT_NULL(nonce);
    TEST_ASSERT_EQUAL(EOK, memset_s(nonce, NONCE_SIZE, 0, NONCE_SIZE));

    GcmParam gcmParams;
    gcmParams.aad = (uint8_t *)EXCHANGE_AAD;
    gcmParams.aadLen = (uint32_t)strlen(EXCHANGE_AAD);
    gcmParams.nonce = nonce;
    gcmParams.nonceLen = NONCE_SIZE;

    uint8_t *cipher = (uint8_t *)malloc(CIPHER_LEN);
    TEST_ASSERT_NOT_NULL(cipher);
    TEST_ASSERT_EQUAL(EOK, memset_s(cipher, CIPHER_LEN, 0, CIPHER_LEN));
    Uint8Buff cipherBuffer = { cipher, CIPHER_LEN };

    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, loader->aesGcmEncrypt(&key, &plainBuffer, &gcmParams, false, &cipherBuffer));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);
    PrintBuffer("AES-GCM cipher", cipher, CIPHER_LEN);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(AES_GCM_CIPHER_CASE, cipher, CIPHER_LEN);

    free(nonce);
    free(cipher);
}
#else // TEST_AES_GCM_ENCRYPT
{
    LOGE("no TEST_AES_GCM_ENCRYPT, do not test aesGcmEncrypt");
    TEST_ASSERT_NULL(loader->aesGcmEncrypt);
}
#endif // TEST_AES_GCM_ENCRYPT

static void TestAesGcmDecrypt(const AlgLoader *loader)
#if TEST_AES_GCM_DECRYPT
{
    LOGI("begin to test the AES-GCM algorithm decryption");
    TEST_ASSERT_NOT_NULL(loader->aesGcmDecrypt);
    if (loader->aesGcmDecrypt == NULL) {
        LOGE("aesGcmDecrypt pointer is NULL! will not test!");
        return;
    }
    const Uint8Buff key = { (uint8_t *)AES_GCM_TEST_KEY, AES_GCM_KEY_LEN };
    const Uint8Buff cipherBuffer = { (uint8_t *)AES_GCM_CIPHER_CASE, CIPHER_LEN };

    uint8_t *nonce = (uint8_t *)malloc(NONCE_SIZE);
    TEST_ASSERT_NOT_NULL(nonce);
    TEST_ASSERT_EQUAL(EOK, memset_s(nonce, NONCE_SIZE, 0, NONCE_SIZE));

    GcmParam gcmParams;
    gcmParams.aad = (uint8_t *)EXCHANGE_AAD;
    gcmParams.aadLen = (uint32_t)strlen(EXCHANGE_AAD);
    gcmParams.nonce = nonce;
    gcmParams.nonceLen = NONCE_SIZE;

    uint8_t *plain = (uint8_t *)malloc(PLAIN_LEN);
    TEST_ASSERT_NOT_NULL(plain);
    TEST_ASSERT_EQUAL(EOK, memset_s(plain, PLAIN_LEN, 0, PLAIN_LEN));
    Uint8Buff plainBuffer = { plain, PLAIN_LEN };

    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, loader->aesGcmDecrypt(&key, &cipherBuffer, &gcmParams, false, &plainBuffer));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);
    PrintBuffer("AES-GCM plain", plain, PLAIN_LEN);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(PLAIN_CASE, plain, PLAIN_LEN);

    free(nonce);
    free(plain);
}
#else // TEST_AES_GCM_DECRYPT
{
    LOGE("no TEST_AES_GCM_DECRYPT, do not test aesGcmDecrypt");
    TEST_ASSERT_NULL(loader->aesGcmDecrypt);
}
#endif // TEST_AES_GCM_DECRYPT

static void TestHashToPoint(const AlgLoader *loader)
#if TEST_HASH_TO_POINT
{
    TEST_ASSERT_NOT_NULL(loader->hashToPoint);
    if (loader->hashToPoint == NULL) {
        LOGE("hashToPoint pointer is NULL! will not test!");
        return;
    }
    const Uint8Buff hash = { (uint8_t *)HASH_TO_POINT_CASE, SHA256_LEN };
    uint8_t *point = (uint8_t *)malloc(SHA256_LEN);
    TEST_ASSERT_NOT_NULL(point);
    TEST_ASSERT_EQUAL(EOK, memset_s(point, SHA256_LEN, 0, SHA256_LEN));
    Uint8Buff pointBuffer = { point, SHA256_LEN };

    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, loader->hashToPoint(&hash, X25519, &pointBuffer));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);
    PrintBuffer("hashToPoint", point, SHA256_LEN);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(HASH_TO_POINT_RESULT, point, PLAIN_LEN);

    free(point);
}
#else // TEST_HASH_TO_POINT
{
    LOGE("no TEST_HASH_TO_POINT, do not test loader->hashToPoint!");
    TEST_ASSERT_NULL(loader->hashToPoint);
}
#endif // TEST_HASH_TO_POINT

#if TEST_AGREE_SHARED_SECRET_WITH_STORAGE
static void VerifyAgreeSharedSecretWithStorage(const AlgLoader *loader, const Uint8Buff *key1, const Uint8Buff *key2)
{
    const Uint8Buff keyInfo = { (uint8_t *)HKDF_EXAMPLE_INFO, strlen(HKDF_EXAMPLE_INFO) };
    const Uint8Buff salt = { (uint8_t *)HKDF_EXAMPLE_SALT, sizeof(HKDF_EXAMPLE_SALT) };
    Uint8Buff outHkdf1 = { (uint8_t *)malloc(ED25519_KEY_BYTE_LEN), ED25519_KEY_BYTE_LEN };
    Uint8Buff outHkdf2 = { (uint8_t *)malloc(ED25519_KEY_BYTE_LEN), ED25519_KEY_BYTE_LEN };
    TEST_ASSERT_NOT_NULL(outHkdf1.val);
    TEST_ASSERT_NOT_NULL(outHkdf2.val);

    TEST_ASSERT_EQUAL(EOK, memset_s(outHkdf1.val, ED25519_KEY_BYTE_LEN, 0, ED25519_KEY_BYTE_LEN));
    TEST_ASSERT_EQUAL(EOK, memset_s(outHkdf2.val, ED25519_KEY_BYTE_LEN, 0, ED25519_KEY_BYTE_LEN));

    TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->computeHkdf(key1, &salt, &keyInfo, &outHkdf1, true));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->computeHkdf(key2, &salt, &keyInfo, &outHkdf2, true));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(outHkdf1.val, outHkdf2.val, ED25519_KEY_BYTE_LEN);

    free(outHkdf1.val);
    free(outHkdf2.val);
}

static int32_t GenerateKeyPairWithStorage(const AlgLoader *loader, const Uint8Buff *keyAlias)
{
    const ExtraInfo extraInfo = {
        .authId = {
            .val = (uint8_t *)malloc(TEST_GENERATE_KEY_PAIR_WITH_STORAGE_AUTH_ID_LENGTH),
            .length = TEST_GENERATE_KEY_PAIR_WITH_STORAGE_AUTH_ID_LENGTH,
        },
        .userType = DEVICE_TYPE_ACCESSORY,
        .pairType = PAIR_TYPE_BIND,
    };
    TEST_ASSERT_NOT_NULL(extraInfo.authId.val);
    TEST_ASSERT_EQUAL(EOK, memset_s(extraInfo.authId.val, extraInfo.authId.length, 0, extraInfo.authId.length));

    int32_t ret = loader->generateKeyPairWithStorage(keyAlias, ED25519_KEY_BYTE_LEN, ED25519, &extraInfo);
    free(extraInfo.authId.val);
    return ret;
}

static void TestAgreeSharedSecretWithStorage(const AlgLoader *loader)
{
    TEST_ASSERT_NOT_NULL(loader->agreeSharedSecretWithStorage);
    TEST_ASSERT_NOT_NULL(loader->computeHkdf);
    TEST_ASSERT_NOT_NULL(loader->generateKeyPairWithStorage);
    if (!loader->agreeSharedSecretWithStorage || !loader->computeHkdf
        || !loader->generateKeyPairWithStorage) {
        LOGE("one of required function pointer is NULL! will not test!");
        return;
    }

    Uint8Buff keyAliasA = { (uint8_t *)KEY_PAIR_ALIAS_1, strlen(KEY_PAIR_ALIAS_1) };
    Uint8Buff keyAliasB = { (uint8_t *)KEY_PAIR_ALIAS_2, strlen(KEY_PAIR_ALIAS_2) };

    int ret;
    ret = GenerateKeyPairWithStorage(loader, &keyAliasA);
    TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);

    ret = GenerateKeyPairWithStorage(loader, &keyAliasB);
    TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);

    if (ret != HAL_SUCCESS) {
        LOGE("GenerateKeyPair failed! will not test!");
        return;
    }

    KeyBuff keyA = { keyAliasA.val, keyAliasA.length, true };
    KeyBuff keyB = { keyAliasB.val, keyAliasB.length, true };

    Uint8Buff sharedKeyAlias1 = { (uint8_t *)SHARED_KEY_ALIAS_1, strlen(SHARED_KEY_ALIAS_1) };
    Uint8Buff sharedKeyAlias2 = { (uint8_t *)SHARED_KEY_ALIAS_2, strlen(SHARED_KEY_ALIAS_1) };

    Algorithm alg = ED25519;
    const uint32_t keyLength = ED25519_KEY_BYTE_LEN;

    RUN_AND_PRINT_ELAPSED_TIME(ret, loader->agreeSharedSecretWithStorage(&keyA,
        &keyB, alg, keyLength, &sharedKeyAlias1));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);

    RUN_AND_PRINT_ELAPSED_TIME(ret, loader->agreeSharedSecretWithStorage(&keyB,
        &keyA, alg, keyLength, &sharedKeyAlias2));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);

    VerifyAgreeSharedSecretWithStorage(loader, &sharedKeyAlias1, &sharedKeyAlias2);
    if (loader->deleteKey) {
        LOGD("test deleteKey");
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->deleteKey(&sharedKeyAlias1));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->deleteKey(&sharedKeyAlias2));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->deleteKey(&keyAliasA));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->deleteKey(&keyAliasB));
    }
}
#else // TEST_AGREE_SHARED_SECRET_WITH_STORAGE
static void TestAgreeSharedSecretWithStorage(const AlgLoader *loader)
{
    LOGE("no TEST_AGREE_SHARED_SECRET_WITH_STORAGE, do not test loader->agreeSharedSecretWithStorage!");
    TEST_ASSERT_TRUE(loader->agreeSharedSecretWithStorage == NULL ||
        loader->computeHkdf == NULL ||
        loader->generateKeyPairWithStorage == NULL);
    LOG_ERROR_IF_POINTER_NULL(loader->agreeSharedSecretWithStorage);
    LOG_ERROR_IF_POINTER_NULL(loader->computeHkdf);
    LOG_ERROR_IF_POINTER_NULL(loader->generateKeyPairWithStorage);
}
#endif // TEST_AGREE_SHARED_SECRET_WITH_STORAGE

static void TestAgreeSharedSecret(const AlgLoader *loader)
#if TEST_AGREE_SHARED_SECRET
{
    TEST_ASSERT_NOT_NULL(loader->agreeSharedSecret);
    if (!loader->agreeSharedSecret) {
        LOGE("agreeSharedSecret pointer is NULL! will not test!");
        return;
    }

    Algorithm alg = X25519;
    const uint32_t keyLength = X25519_KEY_BYTE_LEN;

    KeyBuff privateKeyA = { (uint8_t *)X25519_PRIVATE_KEY_A, keyLength, false };
    KeyBuff publicKeyA = { (uint8_t *)X25519_PUBLIC_KEY_A, keyLength, false };

    KeyBuff privateKeyB = { (uint8_t *)X25519_PRIVATE_KEY_B, keyLength, false };
    KeyBuff publicKeyB = { (uint8_t *)X25519_PUBLIC_KEY_B, keyLength, false };

    Uint8Buff sharedKey1 = { (uint8_t *)malloc(keyLength), keyLength };
    Uint8Buff sharedKey2 = { (uint8_t *)malloc(keyLength), keyLength };

    TEST_ASSERT_NOT_NULL(sharedKey1.val);
    TEST_ASSERT_NOT_NULL(sharedKey2.val);
    TEST_ASSERT_EQUAL(EOK, memset_s(sharedKey1.val, keyLength, 0, keyLength));
    TEST_ASSERT_EQUAL(EOK, memset_s(sharedKey2.val, keyLength, 0, keyLength));

    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, loader->agreeSharedSecret(&privateKeyA, &publicKeyB, alg, &sharedKey1));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);
    RUN_AND_PRINT_ELAPSED_TIME(ret, loader->agreeSharedSecret(&privateKeyB, &publicKeyA, alg, &sharedKey2));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);
    PrintBuffer("shared key 1", sharedKey1.val, keyLength);
    PrintBuffer("shared key 2", sharedKey2.val, keyLength);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sharedKey1.val, sharedKey2.val, ED25519_KEY_BYTE_LEN);

    free(sharedKey1.val);
    free(sharedKey2.val);
}
#else // TEST_AGREE_SHARED_SECRET
{
    LOGE("no TEST_AGREE_SHARED_SECRET, do not test loader->agreeSharedSecret");
    TEST_ASSERT_NULL(loader->agreeSharedSecret);
}
#endif // TEST_AGREE_SHARED_SECRET

static void TestGenerateKeyPairWithStorage(const AlgLoader *loader)
#if TEST_GENERATE_KEY_PAIR_WITH_STORAGE
{
    TEST_ASSERT_NOT_NULL(loader->generateKeyPairWithStorage);
    if (!loader->generateKeyPairWithStorage) {
        LOGE("generateKeyPairWithStorage pointer is NULL! will not test!");
        return;
    }

    Uint8Buff keyAlias = { (uint8_t *)KEY_PAIR_ALIAS_1, strlen(KEY_PAIR_ALIAS_1) };
    const ExtraInfo extraInfo = {
        .authId = {
            .val = (uint8_t *)malloc(TEST_GENERATE_KEY_PAIR_WITH_STORAGE_AUTH_ID_LENGTH),
            .length = TEST_GENERATE_KEY_PAIR_WITH_STORAGE_AUTH_ID_LENGTH,
        },
        .userType = DEVICE_TYPE_ACCESSORY,
        .pairType = PAIR_TYPE_BIND,
    };
    TEST_ASSERT_NOT_NULL(extraInfo.authId.val);
    TEST_ASSERT_EQUAL(EOK, memset_s(extraInfo.authId.val, extraInfo.authId.length, 0, extraInfo.authId.length));

    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret,
        loader->generateKeyPairWithStorage(&keyAlias, ED25519_KEY_BYTE_LEN, ED25519, &extraInfo));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);
    if (loader->checkKeyExist) {
        LOGD("test checkKeyExist");
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->checkKeyExist(&keyAlias));
    }
    if (loader->deleteKey) {
        LOGD("test deleteKey");
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->deleteKey(&keyAlias));
    }

    free(extraInfo.authId.val);
}
#else // TEST_GENERATE_KEY_PAIR_WITH_STORAGE
{
    LOGE("no TEST_GENERATE_KEY_PAIR_WITH_STORAGE, do not test loader->generateKeyPairWithStorage!");
    TEST_ASSERT_NULL(loader->generateKeyPairWithStorage);
}
#endif // TEST_GENERATE_KEY_PAIR_WITH_STORAGE

static void TestBigNumExpMod(const AlgLoader *loader)
#if TEST_BIG_NUM_EXP_MOD
{
    TEST_ASSERT_NOT_NULL(loader->bigNumExpMod);
    if (loader->bigNumExpMod == NULL) {
        LOGE("bigNumExpMod pointer is NULL! will not test!");
        return;
    }

    for (uint32_t i = 0; i < ARRAY_SIZE(BIG_NUM_TEST_CASES); ++i) {
        uint32_t baseLength = BIG_NUM_TEST_CASES[i].baseLength;
        uint32_t expLength = BIG_NUM_TEST_CASES[i].expLength;
        uint32_t primeLength = strlen(BIG_NUM_TEST_CASES[i].prime) >> 1;

        const Uint8Buff base = { (uint8_t *)BIG_NUM_TEST_CASES[i].base, baseLength };
        const Uint8Buff exp = { (uint8_t *)BIG_NUM_TEST_CASES[i].exp, expLength };
        Uint8Buff modResult = { (uint8_t *)malloc(primeLength), primeLength };
        TEST_ASSERT_NOT_NULL(modResult.val);
        TEST_ASSERT_EQUAL(EOK, memset_s(modResult.val, primeLength, 0, primeLength));

        int ret;
        LOGI("test the big num case: base = %u, exp = %u, prime = %u", baseLength, expLength, primeLength);
        RUN_AND_PRINT_ELAPSED_TIME(ret, loader->bigNumExpMod(&base, &exp, BIG_NUM_TEST_CASES[i].prime, &modResult));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, ret);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(BIG_NUM_TEST_CASES[i].result, modResult.val, primeLength);
        PrintBuffer("mod result", modResult.val, modResult.length);
        free(modResult.val);
    }
}
#else // TEST_BIG_NUM_EXP_MOD
{
    LOGE("no TEST_BIG_NUM_EXP_MOD, do not test loader->bigNumExpMod!");
    TEST_ASSERT_NULL(loader->bigNumExpMod);
}
#endif // TEST_BIG_NUM_EXP_MOD

static void TestGenerateKeyPair(const AlgLoader *loader)
#if TEST_GENERATE_KEY_PAIR
{
    TEST_ASSERT_NOT_NULL(loader->generateKeyPair);
    LOGE("nobody use generateKeyPair, do not test");
}
#else // TEST_GENERATE_KEY_PAIR
{
    TEST_ASSERT_NULL(loader->generateKeyPair);
    LOGE("no TEST_GENERATE_KEY_PAIR, do not test generateKeyPair");
}
#endif // TEST_GENERATE_KEY_PAIR

#if (TEST_EXPORT_PUBLIC_KEY || TEST_IMPORT_PUBLIC_KEY || TEST_ALGORITHM_SIGN || TEST_ALGORITHM_VERIFY)
struct TestExportImportSignVerifyParam {
    ExtraInfo extInfo;
    Uint8Buff keyAlias;
    Uint8Buff outPubKey;
    Uint8Buff anotherKeyAlias;
    Uint8Buff messages[ARRAY_SIZE(TEST_SIGN_MSG_LENGTHES)];
    Uint8Buff signatures[ARRAY_SIZE(TEST_SIGN_MSG_LENGTHES)];
    Uint8Buff anotherOutPk;
};

static void TestExportImportSignVerifyPrepareResources(struct TestExportImportSignVerifyParam *param)
{
    param->extInfo.authId.length = TEST_EX_IM_SN_VF_EXTRA_INFO_AUTH_ID_LENGTH;
    param->extInfo.authId.val = (uint8_t *)malloc(param->extInfo.authId.length);
    param->extInfo.userType = DEVICE_TYPE_ACCESSORY;
    param->extInfo.pairType = PAIR_TYPE_BIND;
    TEST_ASSERT_NOT_NULL(param->extInfo.authId.val);
    FillRandom(param->extInfo.authId.val, param->extInfo.authId.length, param->extInfo.authId.length);

    param->keyAlias.length = TEST_EX_IM_SN_VF_KEY_ALIAS_LENGTH;
    param->keyAlias.val = (uint8_t *)malloc(param->keyAlias.length);
    TEST_ASSERT_NOT_NULL(param->keyAlias.val);
    FillRandom(param->keyAlias.val, param->keyAlias.length, param->keyAlias.length);

    param->outPubKey.length = TEST_EX_IM_SN_VF_OUT_PUB_KEY_LENGTH;
    param->outPubKey.val = (uint8_t *)malloc(param->outPubKey.length);
    TEST_ASSERT_NOT_NULL(param->outPubKey.val);
    TEST_ASSERT_EQUAL(EOK, memset_s(param->outPubKey.val, param->outPubKey.length, 0, param->outPubKey.length));

    param->anotherKeyAlias.length = TEST_EX_IM_SN_VF_KEY_ALIAS_LENGTH;
    param->anotherKeyAlias.val = (uint8_t *)malloc(param->anotherKeyAlias.length);
    TEST_ASSERT_NOT_NULL(param->anotherKeyAlias.val);
    do {
        FillRandom(param->anotherKeyAlias.val, param->anotherKeyAlias.length, param->anotherKeyAlias.length);
    } while (memcmp(param->keyAlias.val, param->anotherKeyAlias.val, TEST_EX_IM_SN_VF_KEY_ALIAS_LENGTH) == 0);

    for (uint32_t i = 0; i < ARRAY_SIZE(TEST_SIGN_MSG_LENGTHES); ++i) {
        param->messages[i].length = TEST_SIGN_MSG_LENGTHES[i];
        param->messages[i].val = (uint8_t *)malloc(param->messages[i].length);
        TEST_ASSERT_NOT_NULL(param->messages[i].val);
        FillRandom(param->messages[i].val, param->messages[i].length, param->messages[i].length);
        param->signatures[i].length = TEST_SIGN_SIGNATURE_LENGTH;
        param->signatures[i].val = (uint8_t *)malloc(param->signatures[i].length);
        TEST_ASSERT_NOT_NULL(param->signatures[i].val);
        TEST_ASSERT_EQUAL(EOK,
            memset_s(param->signatures[i].val, param->signatures[i].length, 0, param->signatures[i].length));
    }

    param->anotherOutPk.length = TEST_EX_IM_SN_VF_OUT_PUB_KEY_LENGTH;
    param->anotherOutPk.val = (uint8_t *)malloc(param->anotherOutPk.length);
    TEST_ASSERT_NOT_NULL(param->anotherOutPk.val);
    TEST_ASSERT_EQUAL(EOK,
        memset_s(param->anotherOutPk.val, param->anotherOutPk.length, 0, param->anotherOutPk.length));
}

static void TestExportImportSignVerifyInnerVerfiyNormalAndAbnormalCases(
    const AlgLoader *loader, const Algorithm alg, struct TestExportImportSignVerifyParam *param)
{
    int res;
    for (uint32_t i = 0; i < ARRAY_SIZE(TEST_SIGN_MSG_LENGTHES); ++i) {
        // test normal case
        RUN_AND_PRINT_ELAPSED_TIME(res,
            loader->verify(&param->anotherOutPk, &param->messages[i], alg, &param->signatures[i], false));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
        // test tamper message
        uint8_t originMsg = param->messages[i].val[0];
        param->messages[i].val[0] = ((originMsg == UINT8_MAX) ? 0 : (originMsg + 1));
        TEST_ASSERT_NOT_EQUAL(originMsg, param->messages[i].val[0]);
        RUN_AND_PRINT_ELAPSED_TIME(res,
            loader->verify(&param->anotherOutPk, &param->messages[i], alg, &param->signatures[i], false));
        TEST_ASSERT_NOT_EQUAL(HAL_SUCCESS, res);
        param->messages[i].val[0] = originMsg;
        // test normal case
        RUN_AND_PRINT_ELAPSED_TIME(res,
            loader->verify(&param->anotherOutPk, &param->messages[i], alg, &param->signatures[i], false));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
        // test tamper signature
        uint8_t originSig = param->signatures[i].val[0];
        param->signatures[i].val[0] = ((originSig == UINT8_MAX) ? 0 : (originSig + 1));
        TEST_ASSERT_NOT_EQUAL(originSig, param->signatures[i].val[0]);
        RUN_AND_PRINT_ELAPSED_TIME(res,
            loader->verify(&param->anotherOutPk, &param->messages[i], alg, &param->signatures[i], false));
        TEST_ASSERT_NOT_EQUAL(HAL_SUCCESS, res);
        param->signatures[i].val[0] = originSig;
        // test normal case
        RUN_AND_PRINT_ELAPSED_TIME(res,
            loader->verify(&param->anotherOutPk, &param->messages[i], alg, &param->signatures[i], false));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
    }
}

static void TestExportImportSignVerifyInner(
    const AlgLoader *loader, const Algorithm alg, struct TestExportImportSignVerifyParam *param)
{
    int res;
    // 1. generate one public private key pair
    TEST_ASSERT_EQUAL(HAL_SUCCESS,
        loader->generateKeyPairWithStorage(&param->keyAlias, TEST_EX_IM_SN_VF_KEY_LENGTH, alg, &param->extInfo));

    // 2. export the public key which is generated in the first step
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->exportPublicKey(&param->keyAlias, &param->outPubKey));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);

    // 3. import the public key which is exported in the second step with a different name
    RUN_AND_PRINT_ELAPSED_TIME(res,
        loader->importPublicKey(&param->anotherKeyAlias, &param->outPubKey, alg, &param->extInfo));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);

    // 4. sign with the public key which is generated in the first step
    for (uint32_t i = 0; i < ARRAY_SIZE(TEST_SIGN_MSG_LENGTHES); ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res,
            loader->sign(&param->keyAlias, &param->messages[i], alg, &param->signatures[i], true));
        TEST_ASSERT_EQUAL(HAL_SUCCESS, res);
        TEST_ASSERT_GREATER_THAN(0u, param->signatures[i].length);
        TEST_ASSERT_LESS_OR_EQUAL(TEST_SIGN_SIGNATURE_LENGTH, param->signatures[i].length);
        TEST_ASSERT_NOT_EQUAL(param->signatures[i].length,
            CountZero(param->signatures[i].val, param->signatures[i].length));
    }

    // 5. export the public key which is imported in the third step
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->exportPublicKey(&param->anotherKeyAlias, &param->anotherOutPk));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);

    // 6. verify the signature that is signed in the fourth step with the public key that is imported in the fifth step
    TestExportImportSignVerifyInnerVerfiyNormalAndAbnormalCases(loader, alg, param);

    // 7. compare the public keys which are exported in the second and fifth step
    PrintBuffer("exported public key", param->outPubKey.val, param->outPubKey.length);
    TEST_ASSERT_GREATER_THAN(0u, param->outPubKey.length);
    TEST_ASSERT_LESS_OR_EQUAL(TEST_EX_IM_SN_VF_OUT_PUB_KEY_LENGTH, param->outPubKey.length);
    TEST_ASSERT_NOT_EQUAL(param->outPubKey.length, CountZero(param->outPubKey.val, param->outPubKey.length));
    TEST_ASSERT_EQUAL_MESSAGE(param->outPubKey.length, param->anotherOutPk.length,
        TEST_EXPORT_DIFFERENT_ERROR_MESSAGE);
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(param->outPubKey.val, param->anotherOutPk.val, param->outPubKey.length,
        TEST_EXPORT_DIFFERENT_ERROR_MESSAGE);

    if (loader->deleteKey) {
        LOGD("test deleteKey");

        // 8. delete the key pair which is generated in the first step
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->deleteKey(&param->keyAlias));

        // 9. delete the public key which is imported in the third step
        TEST_ASSERT_EQUAL(HAL_SUCCESS, loader->deleteKey(&param->anotherKeyAlias));
    }
}

static void TestExportImportSignVerifyReleaseResources(struct TestExportImportSignVerifyParam *param)
{
    free(param->extInfo.authId.val);
    param->extInfo.authId.val = NULL;
    free(param->keyAlias.val);
    param->keyAlias.val = NULL;
    free(param->outPubKey.val);
    param->outPubKey.val = NULL;
    free(param->anotherKeyAlias.val);
    param->anotherKeyAlias.val = NULL;
    for (uint32_t i = 0; i < ARRAY_SIZE(TEST_SIGN_MSG_LENGTHES); ++i) {
        free(param->messages[i].val);
        param->messages[i].val = NULL;
        free(param->signatures[i].val);
        param->signatures[i].val = NULL;
    }
    free(param->anotherOutPk.val);
    param->anotherOutPk.val = NULL;
}

static void TestExportImportSignVerify(const AlgLoader *loader, const Algorithm alg)
{
    TEST_ASSERT_NOT_NULL(loader->generateKeyPairWithStorage);
    // only two test cases for exportPublicKey, P256 and ED25519, outKey length is 32
    TEST_ASSERT_NOT_NULL(loader->exportPublicKey);
    TEST_ASSERT_NOT_NULL(loader->importPublicKey);
    // sign message length varies from min 1 to max 1024
    TEST_ASSERT_NOT_NULL(loader->sign);
    TEST_ASSERT_NOT_NULL(loader->verify);
    TEST_ASSERT_TRUE((alg == ED25519) || (alg == P256));
    if (((alg != ED25519) && (alg != P256)) || !loader->generateKeyPairWithStorage ||
        !loader->exportPublicKey || !loader->importPublicKey || !loader->sign || !loader->verify) {
        LOGE("one of requested pointer is NULL or alg is invalid, can not test!");
        return;
    }

    struct TestExportImportSignVerifyParam param;

    TestExportImportSignVerifyPrepareResources(&param);

    TestExportImportSignVerifyInner(loader, alg, &param);

    TestExportImportSignVerifyReleaseResources(&param);
}

static void TestExportImportSignVerifyP256AndEd25519(const AlgLoader *loader)
{
#if TEST_EXPORT_IMPORT_SIGN_VERIFY_ED25519
    LOGI("test export import sign verify ED25519");
    TestExportImportSignVerify(loader, ED25519);
#else // TEST_EXPORT_IMPORT_SIGN_VERIFY_ED25519
    LOGE("no TEST_EXPORT_IMPORT_SIGN_VERIFY_ED25519, do not test ED25519");
#endif // TEST_EXPORT_IMPORT_SIGN_VERIFY_ED25519

#if TEST_EXPORT_IMPORT_SIGN_VERIFY_P256
    LOGI("test export import sign verify P256");
    TestExportImportSignVerify(loader, P256);
#else // TEST_EXPORT_IMPORT_SIGN_VERIFY_P256
    LOGE("no TEST_EXPORT_IMPORT_SIGN_VERIFY_P256, do not test P256");
#endif // TEST_EXPORT_IMPORT_SIGN_VERIFY_P256
}
#endif // (TEST_EXPORT_PUBLIC_KEY || TEST_IMPORT_PUBLIC_KEY || TEST_ALGORITHM_SIGN || TEST_ALGORITHM_VERIFY)

static void TestExportPublicKey(const AlgLoader *loader)
#if TEST_EXPORT_PUBLIC_KEY
{
    TEST_ASSERT_NOT_NULL(loader->exportPublicKey);
    if (!loader->exportPublicKey) {
        LOGE("exportPublicKey pointer is NULL, can not test!");
        return;
    }

    TestExportImportSignVerifyP256AndEd25519(loader);
}
#else // TEST_EXPORT_PUBLIC_KEY
{
    TEST_ASSERT_NULL(loader->exportPublicKey);
    LOGE("no TEST_EXPORT_PUBLIC_KEY, do not test exportPublicKey");
}
#endif // TEST_EXPORT_PUBLIC_KEY

static void TestSign(const AlgLoader *loader)
#if TEST_ALGORITHM_SIGN
{
    TEST_ASSERT_NOT_NULL(loader->sign);
    if (!loader->sign) {
        LOGE("sign pointer is NULL, can not test!");
        return;
    }

    TestExportImportSignVerifyP256AndEd25519(loader);
}
#else // TEST_ALGORITHM_SIGN
{
    TEST_ASSERT_NULL(loader->sign);
    LOGE("no TEST_ALGORITHM_SIGN, do not test sign");
}
#endif // TEST_ALGORITHM_SIGN

static void TestVerify(const AlgLoader *loader)
#if TEST_ALGORITHM_VERIFY
{
    TEST_ASSERT_NOT_NULL(loader->verify);
    if (!loader->verify) {
        LOGE("verify pointer is NULL, can not test!");
        return;
    }

    TestExportImportSignVerifyP256AndEd25519(loader);
}
#else // TEST_ALGORITHM_VERIFY
{
    TEST_ASSERT_NULL(loader->verify);
    LOGE("no TEST_ALGORITHM_VERIFY, do not test verify");
}
#endif // TEST_ALGORITHM_VERIFY

static void TestImportPublicKey(const AlgLoader *loader)
#if TEST_IMPORT_PUBLIC_KEY
{
    TEST_ASSERT_NOT_NULL(loader->importPublicKey);
    if (!loader->importPublicKey) {
        LOGE("importPublicKey pointer is NULL, can not test!");
        return;
    }

    TestExportImportSignVerifyP256AndEd25519(loader);
}
#else // TEST_IMPORT_PUBLIC_KEY
{
    TEST_ASSERT_NULL(loader->importPublicKey);
    LOGE("no TEST_IMPORT_PUBLIC_KEY, do not test importPublicKey");
}
#endif // TEST_IMPORT_PUBLIC_KEY

#if TEST_CHECK_DL_PUBLIC_KEY // {
static void TestCheckDlPublicKeyInner(const AlgLoader *loader,
    Uint8Buff *key, const char *primeHex)
{
    bool res;
    int randIndex;

    // 1. all zero
    TEST_ASSERT_EQUAL(EOK, memset_s(key->val, key->length, 0, key->length));
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->checkDlPublicKey(key, primeHex));
    TEST_ASSERT_EQUAL(false, res);

    // 2. value is 1
    TEST_ASSERT_EQUAL(EOK, memset_s(key->val, key->length, 0, key->length));
    key->val[key->length - 1] = TEST_CHECK_DL_VALUE_ONE;
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->checkDlPublicKey(key, primeHex));
    TEST_ASSERT_EQUAL(false, res);

    // 3. value is 2
    TEST_ASSERT_EQUAL(EOK, memset_s(key->val, key->length, 0, key->length));
    key->val[key->length - 1] = TEST_CHECK_DL_VALUE_TWO;
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->checkDlPublicKey(key, primeHex));
    TEST_ASSERT_EQUAL(true, res);

    // 4. value is between 2 and primeHex - 2
    // first, init value with 2
    TEST_ASSERT_EQUAL(EOK, memset_s(key->val, key->length, 0, key->length));
    key->val[key->length - 1] = TEST_CHECK_DL_VALUE_TWO;
    // then add 1 on rand byte
    randIndex = rand() % key->length;
    key->val[randIndex] += TEST_CHECK_DL_VALUE_ONE;
    PrintBuffer("rand key", key->val, key->length);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->checkDlPublicKey(key, primeHex));
    TEST_ASSERT_EQUAL(true, res);
    // first, init value with primeHex - 2
    TEST_ASSERT_EQUAL(EOK, memset_s(key->val, key->length, 0, key->length));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, HexStringToByte(primeHex, key->val, key->length));
    TEST_ASSERT_GREATER_OR_EQUAL(TEST_CHECK_DL_VALUE_TWO, key->val[key->length - 1]);
    key->val[key->length - 1] -= TEST_CHECK_DL_VALUE_TWO;
    // then sub 1 on rand byte
    do {
        randIndex = rand() % key->length;
    } while (key->val[randIndex] < TEST_CHECK_DL_VALUE_ONE);
    key->val[randIndex] -= TEST_CHECK_DL_VALUE_ONE;
    PrintBuffer("rand key", key->val, key->length);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->checkDlPublicKey(key, primeHex));
    TEST_ASSERT_EQUAL(true, res);

    // 5. value is primeHex - 2
    TEST_ASSERT_EQUAL(EOK, memset_s(key->val, key->length, 0, key->length));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, HexStringToByte(primeHex, key->val, key->length));
    TEST_ASSERT_GREATER_OR_EQUAL(TEST_CHECK_DL_VALUE_TWO, key->val[key->length - 1]);
    key->val[key->length - 1] -= TEST_CHECK_DL_VALUE_TWO;
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->checkDlPublicKey(key, primeHex));
    TEST_ASSERT_EQUAL(true, res);

    // 6. value is primeHex - 1
    TEST_ASSERT_EQUAL(EOK, memset_s(key->val, key->length, 0, key->length));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, HexStringToByte(primeHex, key->val, key->length));
    TEST_ASSERT_GREATER_OR_EQUAL(TEST_CHECK_DL_VALUE_ONE, key->val[key->length - 1]);
    key->val[key->length - 1] -= TEST_CHECK_DL_VALUE_ONE;
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->checkDlPublicKey(key, primeHex));
    TEST_ASSERT_EQUAL(false, res);

    // 7. value is primeHex
    TEST_ASSERT_EQUAL(EOK, memset_s(key->val, key->length, 0, key->length));
    TEST_ASSERT_EQUAL(HAL_SUCCESS, HexStringToByte(primeHex, key->val, key->length));
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->checkDlPublicKey(key, primeHex));
    TEST_ASSERT_EQUAL(false, res);
}

static void TestCheckDlPublicKey(const AlgLoader *loader)
{
    TEST_ASSERT_NOT_NULL(loader->checkDlPublicKey);
    if (!loader->checkDlPublicKey) {
        LOGE("checkDlPublicKey pointer is NULL, can not test!");
        return;
    }

    Uint8Buff key;

    key.length = PAKE_DL_PRIME_LEN_256;
    key.val = (uint8_t *)malloc(key.length);
    TEST_ASSERT_NOT_NULL(key.val);
    TestCheckDlPublicKeyInner(loader, &key, LARGE_PRIME_NUM_HEX_256);
    free(key.val);

    key.length = PAKE_DL_PRIME_LEN_384;
    key.val = (uint8_t *)malloc(key.length);
    TEST_ASSERT_NOT_NULL(key.val);
    TestCheckDlPublicKeyInner(loader, &key, LARGE_PRIME_NUM_HEX_384);
    free(key.val);
}
#else // TEST_CHECK_DL_PUBLIC_KEY // } {
static void TestCheckDlPublicKey(const AlgLoader *loader)
{
    TEST_ASSERT_NULL(loader->checkDlPublicKey);
    LOGE("no TEST_CHECK_DL_PUBLIC_KEY, do not test checkDlPublicKey");
}
#endif // TEST_CHECK_DL_PUBLIC_KEY // }

static void TestCheckEcPublicKey(const AlgLoader *loader)
{
    TEST_ASSERT_NULL(loader->checkEcPublicKey);
    LOGE("loader->checkEcPublicKey is NULL, and nobody use it, do not test!");
}

#if TEST_BIG_NUM_COMPARE // {
static void TestBigNumCompareInnerMinMax(const AlgLoader *loader,
    Uint8Buff *x, Uint8Buff *y)
{
    // 0. 0xFFFFFF > 0x000000
    int res;
    TEST_ASSERT_EQUAL(EOK, memset_s(x->val, x->length, UINT8_MAX, x->length));
    TEST_ASSERT_EQUAL(EOK, memset_s(y->val, y->length, 0, y->length));
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(x, y));
    TEST_ASSERT_EQUAL(1, res);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(y, x));
    TEST_ASSERT_EQUAL(-1, res);
}

static void TestBigNumCompareInnerFull(const AlgLoader *loader,
    Uint8Buff *x, Uint8Buff *y)
{
    int previous, res;

    // 1. 0x123456 > 0x123455
    TEST_ASSERT_EQUAL(EOK, memcpy_s(x->val, x->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL)));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(y->val, y->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(x->val, y->val, TEST_BIG_NUM_COMPARE_NUM_LENGTH_FULL);
    previous = y->val[y->length - 1];
    y->val[y->length - 1] -= 1;
    TEST_ASSERT_LESS_THAN(previous, y->val[y->length - 1]);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(x, y));
    TEST_ASSERT_EQUAL(1, res);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(y, x));
    TEST_ASSERT_EQUAL(-1, res);

    // 2. 0x123456 = 0x123456
    TEST_ASSERT_EQUAL(EOK, memcpy_s(x->val, x->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL)));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(y->val, y->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(x->val, y->val, TEST_BIG_NUM_COMPARE_NUM_LENGTH_FULL);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(x, y));
    TEST_ASSERT_EQUAL(0, res);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(y, x));
    TEST_ASSERT_EQUAL(0, res);

    // 3. 0x123456 < 0x123457
    TEST_ASSERT_EQUAL(EOK, memcpy_s(x->val, x->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL)));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(y->val, y->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_FULL)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(x->val, y->val, TEST_BIG_NUM_COMPARE_NUM_LENGTH_FULL);
    previous = y->val[y->length - 1];
    y->val[y->length - 1] += 1;
    TEST_ASSERT_GREATER_THAN(previous, y->val[y->length - 1]);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(x, y));
    TEST_ASSERT_EQUAL(-1, res);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(y, x));
    TEST_ASSERT_EQUAL(1, res);
}

static void TestBigNumCompareInnerHalf(const AlgLoader *loader,
    Uint8Buff *x, Uint8Buff *y)
{
    int previous, res;

    // 4. 0x003456 > 0x3455
    TEST_ASSERT_EQUAL(EOK,
        memset_s(x->val, x->length, 0, TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(
        x->val + TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF, x->length - TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF)));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(y->val, y->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(x->val + TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF, y->val,
        TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF);
    previous = y->val[y->length - 1];
    y->val[y->length - 1] -= 1;
    TEST_ASSERT_LESS_THAN(previous, y->val[y->length - 1]);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(x, y));
    TEST_ASSERT_EQUAL(1, res);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(y, x));
    TEST_ASSERT_EQUAL(-1, res);

    // 5. 0x003456 = 0x3456
    TEST_ASSERT_EQUAL(EOK,
        memset_s(x->val, x->length, 0, TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(
        x->val + TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF, x->length - TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF)));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(y->val, y->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(x->val + TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF, y->val,
        TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(x, y));
    TEST_ASSERT_EQUAL(0, res);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(y, x));
    TEST_ASSERT_EQUAL(0, res);

    // 6. 0x003456 < 0x3457
    TEST_ASSERT_EQUAL(EOK,
        memset_s(x->val, x->length, 0, TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(
        x->val + TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF, x->length - TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF)));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(y->val, y->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(x->val + TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF, y->val,
        TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF);
    previous = y->val[y->length - 1];
    y->val[y->length - 1] += 1;
    TEST_ASSERT_GREATER_THAN(previous, y->val[y->length - 1]);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(x, y));
    TEST_ASSERT_EQUAL(-1, res);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(y, x));
    TEST_ASSERT_EQUAL(1, res);
}

static void TestBigNumCompareInnerCompareFullAgainstHalf(const AlgLoader *loader,
    Uint8Buff *x, Uint8Buff *y)
{
    int res;

    // 7. 0x123456 > 0x3456
    TEST_ASSERT_EQUAL(EOK, memcpy_s(
        x->val, x->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF)));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(
        x->val + TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF, x->length - TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF)));
    TEST_ASSERT_EQUAL(EOK, memcpy_s(y->val, y->length,
        TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF, sizeof(TEST_BIG_NUM_COMPARE_NUM_EXAMPLE_HALF)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(x->val + TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF, y->val,
        TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(x, y));
    TEST_ASSERT_EQUAL(1, res);
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->bigNumCompare(y, x));
    TEST_ASSERT_EQUAL(-1, res);
}

static void TestBigNumCompare(const AlgLoader *loader)
{
    TEST_ASSERT_NOT_NULL(loader->bigNumCompare);
    if (!loader->bigNumCompare) {
        LOGE("bigNumCompare pointer is NULL, can not test!");
        return;
    }

    Uint8Buff x = { (uint8_t *)malloc(TEST_BIG_NUM_COMPARE_NUM_LENGTH_FULL), TEST_BIG_NUM_COMPARE_NUM_LENGTH_FULL };
    TEST_ASSERT_NOT_NULL(x.val);
    Uint8Buff y = { (uint8_t *)malloc(TEST_BIG_NUM_COMPARE_NUM_LENGTH_FULL), TEST_BIG_NUM_COMPARE_NUM_LENGTH_FULL };
    TEST_ASSERT_NOT_NULL(y.val);

    TestBigNumCompareInnerMinMax(loader, &x, &y);

    TestBigNumCompareInnerFull(loader, &x, &y);

    free(y.val);

    y.length = TEST_BIG_NUM_COMPARE_NUM_LENGTH_HALF;
    y.val = (uint8_t *)malloc(y.length);
    TEST_ASSERT_NOT_NULL(y.val);

    TestBigNumCompareInnerHalf(loader, &x, &y);

    TestBigNumCompareInnerCompareFullAgainstHalf(loader, &x, &y);

    free(x.val);
    free(y.val);
}
#else // TEST_BIG_NUM_COMPARE // } {
static void TestBigNumCompare(const AlgLoader *loader)
{
    TEST_ASSERT_NULL(loader->bigNumCompare);
    LOGE("no TEST_BIG_NUM_COMPARE, do not test loader->bigNumCompare");
}
#endif // TEST_BIG_NUM_COMPARE // {

void TestHcAlg(void)
{
    const AlgLoader *loader = GetLoaderInstance();
    TEST_ASSERT_NOT_NULL(loader);
    if (loader == NULL) {
        LOGE("alg loader instance is NULL");
        return;
    }

    int32_t res;
    RUN_AND_PRINT_ELAPSED_TIME(res, loader->initAlg());
    TEST_ASSERT_EQUAL(HAL_SUCCESS, res);

    TestSha256(loader);

    TestGenerateRandom(loader);

    TestComputeHmac(loader);

    TestComputeHkdf(loader);

#if (defined(DO_NOT_TEST_DEPRECATED_IMPORT_SYMMETRIC_KEY) && DO_NOT_TEST_DEPRECATED_IMPORT_SYMMETRIC_KEY)
    (void)(TestImportSymmetricKey);
#else // DO_NOT_TEST_DEPRECATED_IMPORT_SYMMETRIC_KEY
    TestImportSymmetricKey(loader);
#endif // DO_NOT_TEST_DEPRECATED_IMPORT_SYMMETRIC_KEY

    TestCheckKeyExist(loader);

    TestDeleteKey(loader);

    TestAesGcmEncrypt(loader);

    TestAesGcmDecrypt(loader);

    TestHashToPoint(loader);

    TestGenerateKeyPairWithStorage(loader);

    TestAgreeSharedSecretWithStorage(loader);

    TestAgreeSharedSecret(loader);

    TestBigNumExpMod(loader);

    TestGenerateKeyPair(loader);

    TestExportPublicKey(loader);

    TestSign(loader);

    TestVerify(loader);

    TestImportPublicKey(loader);

    TestCheckDlPublicKey(loader);

    TestCheckEcPublicKey(loader);

    TestBigNumCompare(loader);
}

#ifdef __cplusplus
}
#endif
