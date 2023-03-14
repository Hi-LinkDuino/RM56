/* Copyright (c) 2020-2021 Huawei Device Co., Ltd.
    * Licensed under the Apache License, Version 2.0 (the "License");
    * you may not use this file except in compliance with the License.
    * You may obtain a copy of the License at
    *
    * http://www.apache.org/licenses/LICENSE-2.0
    *
    * Unless required by applicable law or agreed to in writing, software
    * distributed under the License is distributed on an "AS IS" BASIS,
    * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    * See the License for the specific language governing permissions and
    * limitations under the License.
*/
#include "SecurityDataHuks.h"
#include "hks_client.h"
#include "hks_types.h"
#include <securec.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gtest/gtest.h"
using namespace std;
using namespace testing::ext;

class SecurityDataHuksHashRandomHmacTestSuite : public testing::Test {
protected:
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void) {}
    // TearDownTestCase: Testsuit teardown, run after last testcase
    static void TearDownTestCase(void) {}
    // Testcase setup
    virtual void SetUp()
    {
        int32_t status;
        struct hks_file_callbacks fileCallbacks;

        fileCallbacks.read = FileRead;
        fileCallbacks.write = FileWrite;
        fileCallbacks.file_size = FileSize;

        status = hks_register_file_callbacks(&fileCallbacks);
        EXPECT_EQ(0, status);

        struct hks_log_f_group logFunc;
        logFunc.log_info = Logi;
        logFunc.log_warn = Logw;
        logFunc.log_error = Loge;
        logFunc.log_debug = Logd;

        status = hks_register_log_interface(&logFunc);
        EXPECT_EQ(0, status);

        status = hks_register_get_hardware_udid_callback(HksTestGetHardwareUdid);

        EXPECT_EQ(0, status);

        status = hks_init();
        if (status != 0) {
            status = hks_refresh_key_info();
        }
        EXPECT_EQ(0, status);
    }

    // Testcase teardown
    virtual void TearDown() {}
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Hash
// begin+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++1100-1190

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1100
 * @tc.name      : Hash, normal input parameters SHA256, src, dst
 * @tc.desc      : [C- SECURITY -1600]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHash1100, Function | MediumTest | Level1)
{
    struct hks_blob src, dst;
    src.data = (uint8_t *)"123456";
    src.size = NUM6;
    dst.data = (uint8_t *)malloc(NUM65);
    if (dst.data == NULL) {
        EXPECT_EQ(0, 1);
    }
    dst.size = NUM65;

    int32_t res = hks_hash(HKS_ALG_HASH_SHA_256, &src, &dst);
    EXPECT_EQ(0, res);
    free(dst.data);
};

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1110
 * @tc.name      : Hash, normal input parameters SHA512, src, dst
 * @tc.desc      : [C- SECURITY -1600]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHash1110, Function | MediumTest | Level1)
{
    struct hks_blob src, dst;
    src.data = (uint8_t *)"123456";
    src.size = NUM6;
    dst.data = (uint8_t *)malloc(NUM65);
    if (dst.data == NULL) {
        EXPECT_EQ(0, 1);
    }
    dst.size = NUM65;

    int32_t res = hks_hash(HKS_ALG_HASH_SHA_512, &src, &dst);
    EXPECT_EQ(0, res);
    free(dst.data);
};

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1120
 * @tc.name      : Hash, abnormal input parameters alg is not equal to HKS_ALG_HASH_SHA_256 or HKS_ALG_HASH_SHA_512
 * @tc.desc      : [C- SECURITY -1600]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHash1120, Function | MediumTest | Level2)
{
    struct hks_blob srcData = { 0 };
    char tmpData6[] = "30313233343536373839616263646566";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM16, 0);

    struct hks_blob hash = { 0 };
    char tmpData7[] = "3031323334353637383961626364656630313233343536373839616263646566";
    BuildBlobData(&hash, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = HKS_ALG_HASH_SHA_1;
    int32_t status = hks_hash(alg, &srcData, &hash);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&hash);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1130
 * @tc.name      : Hash, abnormal input parameters srcData is null
 * @tc.desc      : [C- SECURITY -1600]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHash1130, Function | MediumTest | Level2)
{
    struct hks_blob *srcData = nullptr;

    struct hks_blob hash = { 0 };
    const char tmpData7[] = "3031323334353637383961626364656630313233343536373839616263646566";
    BuildBlobData(&hash, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = HKS_ALG_HASH_SHA_256;
    int32_t status = hks_hash(alg, srcData, &hash);
    HksBlobDestroyT1(&hash);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1140
 * @tc.name      : Hash, abnormal input parameters srcData.data is null
 * @tc.desc      : [C- SECURITY -1600]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHash1140, Function | MediumTest | Level2)
{
    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "30313233343536373839616263646566";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM16, 1);
    struct hks_blob hash = { 0 };
    const char tmpData7[] = "3031323334353637383961626364656630313233343536373839616263646566";
    BuildBlobData(&hash, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = HKS_ALG_HASH_SHA_256;
    int32_t status = hks_hash(alg, &srcData, &hash);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&hash);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1150
 * @tc.name      : Hash, abnormal input parameters srcData.size is 0
 * @tc.desc      : [C- SECURITY -1600]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHash1150, Function | MediumTest | Level2)
{
    struct hks_blob srcData = { 0 };
    srcData.data = (uint8_t *)"1234567890abcdefghigkl0123456789";
    srcData.size = 0;
    srcData.type = HKS_BLOB_TYPE_RAW;

    struct hks_blob hash = { 0 };
    const char tmpData7[] = "3031323334353637383961626364656630313233343536373839616263646566";
    BuildBlobData(&hash, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = HKS_ALG_HASH_SHA_256;
    int32_t status = hks_hash(alg, &srcData, &hash);
    HksBlobDestroyT1(&hash);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1160
 * @tc.name      : Hash, abnormal input parameters hash is null
 * @tc.desc      : [C- SECURITY -1600]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHash1160, Function | MediumTest | Level2)
{
    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "30313233343536373839616263646566";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM16, 0);

    struct hks_blob *hash = nullptr;

    uint32_t alg = HKS_ALG_HASH_SHA_256;
    int32_t status = hks_hash(alg, &srcData, hash);
    HksBlobDestroyT1(&srcData);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1170
 * @tc.name      : Hash, abnormal input parameters hash.data is null
 * @tc.desc      : [C- SECURITY -1600]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHash1170, Function | MediumTest | Level2)
{
    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "30313233343536373839616263646566";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM16, 0);

    struct hks_blob hash = { 0 };
    const char tmpData7[] = "3031323334353637383961626364656630313233343536373839616263646566";
    BuildBlobData(&hash, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 1);

    uint32_t alg = HKS_ALG_HASH_SHA_256;
    int32_t status = hks_hash(alg, &srcData, &hash);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&hash);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1180
 * @tc.name      : Hash, abnormal input parameters hash.size is less than 32
 * @tc.desc      : [C- SECURITY -1600]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHash1180, Function | MediumTest | Level2)
{
    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "30313233343536373839616263646566";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM16, 0);

    struct hks_blob hash = { 0 };
    const char tmpData7[] = "303132333435363738396162636465663031323334353637383961611266";
    BuildBlobData(&hash, tmpData7, HKS_BLOB_TYPE_RAW, NUM30, 0);

    uint32_t alg = HKS_ALG_HASH_SHA_256;
    int32_t status = hks_hash(alg, &srcData, &hash);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&hash);
    EXPECT_EQ(NUM1007, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1190
 * @tc.name      : Hash, abnormal input parameters hash.size is less than 64
 * @tc.desc      : [C- SECURITY -1600]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHash1190, Function | MediumTest | Level2)
{
    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "30313233343536373839616263646566";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM16, 0);

    struct hks_blob hash = { 0 };
    const char tmpData7[] = "30313233343536373839616263646566303132333435363738396162636465663031323334"
    "3536373839616263646566303132333435363738396162";
    BuildBlobData(&hash, tmpData7, HKS_BLOB_TYPE_RAW, NUM60, 0);

    uint32_t alg = HKS_ALG_HASH_SHA_512;
    int32_t status = hks_hash(alg, &srcData, &hash);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&hash);
    EXPECT_EQ(NUM1007, status);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Hash
// end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++1100-1190


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Generate random
// beign++++++++++++++++++++++++++++++++++++++++++++++++1200-1230

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1200
 * @tc.name      : Generate Random, normal input parameters random
 * @tc.desc      : [C- SECURITY -1700]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataGenerateRandom1200, Function | MediumTest | Level1)
{
    int32_t status;
    struct hks_blob random = { 0 };

    random.data = (uint8_t *)calloc(1, NUM64);
    if (random.data == NULL) {
        EXPECT_EQ(0, 1);
        return;
    }
    if (memset_s(random.data, NUM64, 0, NUM64) != EOK) {
        EXPECT_EQ(0, 1);
    }
    random.size = NUM64;
    random.type = 0;
    status = hks_generate_random(&random);
    EXPECT_EQ(0, status);

    HksBlobDestroyT1(&random);
};

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1210
 * @tc.name      : Generate Random, abnormal input parameters random is null
 * @tc.desc      : [C- SECURITY -1700]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataGenerateRandom1210, Function | MediumTest | Level2)
{
    int32_t status;
    struct hks_blob *random = nullptr;

    status = hks_generate_random(random);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1220
 * @tc.name      : Generate Random, abnormal input parameters random.data is null
 * @tc.desc      : [C- SECURITY -1700]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataGenerateRandom1220, Function | MediumTest | Level2)
{
    int32_t status;
    struct hks_blob random = { 0 };

    random.data = NULL;
    random.size = NUM32;
    random.type = HKS_BLOB_TYPE_KEY;

    status = hks_generate_random(&random);
    EXPECT_EQ(NUM1000, status);
    HksBlobDestroyT1(&random);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1230
 * @tc.name      : Generate Random, abnormal input parameters random.size is more than 1024
 * @tc.desc      : [C- SECURITY -1700]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataGenerateRandom1230, Function | MediumTest | Level2)
{
    int32_t status;
    struct hks_blob random = { 0 };

    random.data = (uint8_t *)malloc(NUM1025);
    if (random.data == NULL) {
        EXPECT_EQ(0, 1);
    }
    random.size = NUM1025;
    random.type = HKS_BLOB_TYPE_KEY;

    status = hks_generate_random(&random);
    EXPECT_EQ(NUM135, status);
    HksBlobDestroyT1(&random);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Generate random
// end++++++++++++++++++++++++++++++++++++++++++++++++++1200-1230

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Hmac
// begin+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++1240-1380

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1240
 * @tc.name      : Hmac, normal input parameters SHA256, keyAlias, alg, srcData, output
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1240, Function | MediumTest | Level1)
{
    struct hks_blob keyAlias;
    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    struct hks_blob srcData;
    srcData.data = (uint8_t *)"123456789";
    srcData.size = NUM9;

    struct hks_blob output;
    output.data = (uint8_t *)malloc(NUM65);
    output.size = NUM65;
    keyAlias.data = (uint8_t *)"1234567890abcdefghigkl0123456789";
    keyAlias.size = NUM32;
    keyAlias.type = HKS_KEY_USAGE_EXPORT;

    int32_t status = hks_hmac(&keyAlias, alg, &srcData, &output);
    EXPECT_EQ(0, status);

    HKS_FREE_PTR1(output.data);
};

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1250
 * @tc.name      : Hmac, normal input parameters SHA512, keyAlias, alg, srcData, output
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1250, Function | MediumTest | Level1)
{
    struct hks_blob keyAlias;
    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_512);
    struct hks_blob srcData;
    srcData.data = (uint8_t *)"123456789";
    srcData.size = NUM9;

    struct hks_blob output;
    output.data = (uint8_t *)malloc(NUM65);
    output.size = NUM65;
    keyAlias.data = (uint8_t *)"1234567890abcdefghigkl0123456789";
    keyAlias.size = NUM65;
    keyAlias.type = HKS_KEY_USAGE_EXPORT;

    int32_t status = hks_hmac(&keyAlias, alg, &srcData, &output);
    EXPECT_EQ(0, status);

    HKS_FREE_PTR1(output.data);
};

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1260
 * @tc.name      : Hmac, abnormal input parameters key is null
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1260, Function | MediumTest | Level2)
{
    struct hks_blob *key = nullptr;
    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, 0);

    struct hks_blob output = { 0 };
    const char tmpData7[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    int32_t status = hks_hmac(key, alg, &srcData, &output);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1270
 * @tc.name      : Hmac, abnormal input parameters key.data is null
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1270, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM32, NUM1);

    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, 0);

    struct hks_blob output = { 0 };
    const char tmpData7[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    int32_t status = hks_hmac(&key, alg, &srcData, &output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1280
 * @tc.name      : Hmac, abnormal input parameters key.size is 0
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1280, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    key.data = (uint8_t *)"1234567890abcdefghigkl0123456789";
    key.size = 0;
    key.type = HKS_BLOB_TYPE_RAW;

    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, 0);

    struct hks_blob output = { 0 };
    const char tmpData7[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    int32_t status = hks_hmac(&key, alg, &srcData, &output);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1290
 * @tc.name      : Hmac, abnormal input parameters alg is not equal to sha256 or sha512
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1290, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM32, 0);

    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, 0);

    struct hks_blob output = { 0 };
    const char tmpData7[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_1);
    int32_t status = hks_hmac(&key, alg, &srcData, &output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1300
 * @tc.name      : Hmac, abnormal input parameters srcData is null
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1300, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM32, 0);

    struct hks_blob *srcData = nullptr;

    struct hks_blob output = { 0 };
    const char tmpData7[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    int32_t status = hks_hmac(&key, alg, srcData, &output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1310
 * @tc.name      : Hmac, abnormal input parameters srcData.data is null
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1310, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM32, 0);

    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, NUM1);

    struct hks_blob output = { 0 };
    const char tmpData7[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    int32_t status = hks_hmac(&key, alg, &srcData, &output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1320
 * @tc.name      : Hmac, abnormal input parameters srcData.size is 0
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1320, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM32, 0);

    struct hks_blob srcData = { 0 };
    srcData.data = (uint8_t *)"1234567890abcdefghigkl0123456789";
    srcData.size = 0;
    srcData.type = HKS_BLOB_TYPE_RAW;

    struct hks_blob output = { 0 };
    const char tmpData7[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    int32_t status = hks_hmac(&key, alg, &srcData, &output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1330
 * @tc.name      : Hmac, abnormal input parameters output is null
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1330, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM32, 0);

    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, 0);

    struct hks_blob *output = nullptr;

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    int32_t status = hks_hmac(&key, alg, &srcData, output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&srcData);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1340
 * @tc.name      : Hmac, abnormal input parameters output.data is null
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1340, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM32, 0);

    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, 0);

    struct hks_blob output = { 0 };
    const char tmpData7[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, NUM1);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    int32_t status = hks_hmac(&key, alg, &srcData, &output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1350
 * @tc.name      : Hmac, abnormal input parameters alg is sha256 and key.size is less than 32
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1350, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "303132333435363730313233343536373031323334353637303132333411";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM30, 0);

    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, 0);

    struct hks_blob output = { 0 };
    const char tmpData7[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM32, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    int32_t status = hks_hmac(&key, alg, &srcData, &output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1360
 * @tc.name      : Hmac, abnormal input parameters alg is sha512 and key.size is less than 64
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1360, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "30313233343536373031323334353637303132333435363730313233343536373031323334"
    "35363730313233343536373031323334353637303132333435";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM62, 0);

    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, 0);

    struct hks_blob output = { 0 };
    const char tmpData7[] = "30313233343536373031323334353637303132333435363730313233343536373031323334"
    "353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM64, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_512);
    int32_t status = hks_hmac(&key, alg, &srcData, &output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1370
 * @tc.name      : Hmac, abnormal input parameters alg is sha256 and output.size is less than 32
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1370, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "3031323334353637303132333435363730313233343536373031323334353637";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM32, 0);

    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, 0);


    struct hks_blob output = { 0 };
    const char tmpData7[] = "303132333435363730313233343536373031323334353637303132333413";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM30, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_256);
    int32_t status = hks_hmac(&key, alg, &srcData, &output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM1007, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_1380
 * @tc.name      : Hmac, abnormal input parameters alg is sha512 and output.size is less than 64
 * @tc.desc      : [C- SECURITY -2000]
 */
HWTEST_F(SecurityDataHuksHashRandomHmacTestSuite, securityDataHmac1380, Function | MediumTest | Level2)
{
    struct hks_blob key = { 0 };
    const char tmpData5[] = "303132333435363730313233343536373031323334353637303132333435363730313233343"
    "53637303132333435363730313233343536373031323334353637";
    BuildBlobData(&key, tmpData5, HKS_BLOB_TYPE_RAW, NUM64, 0);

    struct hks_blob srcData = { 0 };
    const char tmpData6[] = "3031323334353637";
    BuildBlobData(&srcData, tmpData6, HKS_BLOB_TYPE_RAW, NUM8, 0);

    struct hks_blob output = { 0 };
    const char tmpData7[] = "303132333435363730313233343536373031323334353637303132333435363730313233343"
    "5363730313233343536373031323334353637303132333435";
    BuildBlobData(&output, tmpData7, HKS_BLOB_TYPE_RAW, NUM62, 0);

    uint32_t alg = hks_alg_hmac(HKS_ALG_HASH_SHA_512);
    int32_t status = hks_hmac(&key, alg, &srcData, &output);
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&srcData);
    HksBlobDestroyT1(&output);
    EXPECT_EQ(NUM1007, status);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Hmac
// end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++1240-1380

