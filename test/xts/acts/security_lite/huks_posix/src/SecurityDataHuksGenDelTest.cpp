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
#include <hks_file_api.h>
#include "gtest/gtest.h"
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include "hks_errno.h"
#include <sys/time.h>
#include <hks_hardware_api.h>
using namespace std;
using namespace testing::ext;

errno_t FopenS(FILE **fp, const char *filename, const char *modes)
{
    *fp = fopen(filename, modes);
    return 0;
}

int32_t FileSize(const char *filename)
{
    FILE *fp = nullptr;
    errno_t err;
    int32_t size;

    if (filename == nullptr) {
        return -1;
    }

    err = FopenS(&fp, filename, "rb");
    if (err != 0 || fp == nullptr) {
        return -1;
    }

    if (fseek(fp, 0L, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }

    size = ftell(fp);
    fclose(fp);

    return size;
}
int32_t FileWrite(const char *filename, uint32_t offset, const uint8_t *buf, uint32_t len)
{
    FILE *fp = nullptr;
    errno_t err;
    size_t size;

    if (filename == NULL || buf == NULL) {
        return -1;
    }

    err = FopenS(&fp, filename, "wb+");
    if (err != 0 || fp == nullptr) {
        return -1;
    }

    size = fwrite(buf, 1, len, fp);
    fclose(fp);

    if (size != len) {
        return -1;
    }

    return 0;
}
int32_t FileRead(const char *filename, uint32_t offset, uint8_t *buf, uint32_t len)
{
    FILE* fp = nullptr;
    errno_t err;
    size_t size;

    if (filename == NULL || buf == NULL) {
        return -1;
    }

    if (access(filename, 0) == -1) {
        return 0;
    }

    err = FopenS(&fp, filename, "rb");
    if (err == NUM2) {
        return 0;
    }
    if (err != 0 || fp == nullptr) {
        return -1;
    }

    size = fread(buf, 1, len, fp);
    fclose(fp);

    if (size == 0) {
        return -1;
    }

    return size;
}

uint64_t GetTimeMs()
{
    struct timeval timeVal;
    gettimeofday(&timeVal, nullptr);

    return (uint64_t)NUM1000000 * timeVal.tv_sec + timeVal.tv_usec;
}

void AddLog(const char* logType, const char *tag, const char *func, const char *format, const va_list* ap)
{
    char* buf = (char*)malloc(NUM2048);
    if (buf == nullptr) {
        return;
    }
    int offset = sprintf_s(buf, (NUM2048), "[%s][%llu]%s %s: ", logType, (unsigned long long)GetTimeMs(), tag, func);
    if (offset >= 0) {
        offset += vsprintf_s(buf + offset, (NUM2048) - offset, format, *ap);
    }
	
    free(buf);
    buf = nullptr;
}

void Logi(const char *tag, const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    AddLog("Info", tag, func, format, &args);
    va_end(args);
}

void Logw(const char *tag, const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    AddLog("Warn", tag, func, format, &args);
    va_end(args);
}

void Loge(const char *tag, const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    AddLog("Error", tag, func, format, &args);
    va_end(args);
}

void Logd(const char *tag, const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    AddLog("Debug", tag, func, format, &args);
    va_end(args);
}

uint8_t g_hksHardwareUdidId[32] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0C, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1A, 0x1C, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
};

int32_t HksTestGetHardwareUdid(uint8_t* udid, uint32_t udidLen)
{
    int err;
    if ((udid == NULL) || (udidLen != NUM32)) {
        return -1;
    }

    int32_t rc = 1234567;
    if (rc <= 0) {
        err = memcpy_s(udid, NUM32, g_hksHardwareUdidId, NUM32);
        if (err !=  EOK) {
            return -1;
        }
    }

    char buf[128] = {0};
    uint32_t offset = 0;
    for (uint32_t i = 0; i < udidLen; ++i) {
        offset += sprintf_s(buf + offset, NUM128 - offset, "%02x ", udid[i]);
    }

    Logd("[hks_tester]", __func__, buf);
    return 0;
}

void HksStBlobInit1(struct hks_blob *blob, size_t nmemb, size_t size, uint8_t type)
{
    if (blob == nullptr || nmemb == 0 || size == 0) {
        EXPECT_EQ(0, 1);
        return;
    }
    blob->data = (uint8_t *)calloc(nmemb, size);
    if (blob->data == NULL) {
        EXPECT_EQ(0, 1);
        return;
    }
    if (memset_s(blob->data, size, 0, size) != EOK) {
        EXPECT_EQ(0, 1);
        return;
    }
    blob->size = size;
    blob->type = type;
}

void HksBlobDestroyT1(struct hks_blob *blob)
{
    if (blob == nullptr) {
        EXPECT_EQ(0, 1);
        return;
    }
    if (blob && blob->data) {
        if (memset_s(blob->data, blob->size, 0, blob->size) != EOK) {
        EXPECT_EQ(0, 1);
        return;
        }
        HKS_FREE_PTR1(blob->data);
    }
    blob->data = NULL;
    blob->size = 0;
    blob->type = HKS_BLOB_TYPE_RAW;
}

void HexStringToByte(const char *str, int nLen, unsigned char *pHex)
{
    unsigned int number = 4;
    if (nLen % NUM2) {
        EXPECT_EQ(0, 1);
        return;
    }
    int nHexLen = nLen / NUM2;
    unsigned char nibble[2];

    if (nHexLen >= MAX_INT) {
        return;
    }
    for (int i = 0; i < nHexLen; i++) {
        nibble[0] = str[i * NUM2];
        nibble[1] = str[i * NUM2 + NUM1];
        for (int j = 0; j < NUM2; j++) {
            if (nibble[j] <= 'F' && nibble[j] >= 'A') {
                nibble[j] = nibble[j] - 'A' + NUM10;
            } else if (nibble[j] <= 'f' && nibble[j] >= 'a') {
                nibble[j] = nibble[j] - 'a' + NUM10;
            } else if (nibble[j] >= '0' && nibble[j] <= '9') {
                nibble[j] = nibble[j] - '0';
            } else {
                EXPECT_EQ(0, 1);
                return;
            }
        }
        pHex[i] = nibble[0] << number;
        pHex[i] |= nibble[1];
    }
}

void BuildBlobData(struct hks_blob *param, const char *str, uint8_t type, uint32_t size, uint8_t isDataNull)
{
    if (param == nullptr) {
        EXPECT_EQ(0, 1);
        return;
    }
    param->type = type;
    param->size = size;
    if (isDataNull == 1)
        param->data = NULL;
    else {
        if (size + NUM2 == 0) {
            EXPECT_EQ(0, 1);
            return;
        }
        unsigned char *buff = (unsigned char *)malloc(size + NUM2);
        if (buff == nullptr) {
            EXPECT_EQ(0, 1);
            return;
        }
        if (memset_s(buff, size + NUM2, 0, size + NUM2) != EOK) {
            EXPECT_EQ(0, 1);
            free(buff);
            buff = nullptr;
            return;
        }
        HexStringToByte(str, size * NUM2, buff);
        param->data = (uint8_t *)buff;
    }
}

class SecurityDataHuksGenDelTestSuite : public testing::Test {
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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++generate key
// begin+++++++++++++++++++++++++++++++++++++++++++++++++++0000-0120

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0000
 * @tc.name      : Generate key, normal input parameters keyAlias and keyParam
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0000, Function | MediumTest | Level1)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    int32_t statusGenerate;
    int32_t statusDelete;
    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;
    struct hks_blob keyAliasGenerate;
    HksStBlobInit1(&keyAliasGenerate, sizeof(uint8_t), sizeof(testFileName) + NUM3, HKS_BLOB_TYPE_ALIAS);
    if (memcpy_s(keyAliasGenerate.data, sizeof(testFileName), testFileName, sizeof(testFileName)) != EOK) {
        HksBlobDestroyT1(&keyAliasGenerate);
        EXPECT_EQ(0, 1);
        return;
    }
    char tmpGenerate[NUM3] = { 0 };
    if (strcat_s((char *)keyAliasGenerate.data,
        strlen((char *)keyAliasGenerate.data) + strlen(tmpGenerate) + 1,
        tmpGenerate) != EOK) {
        HksBlobDestroyT1(&keyAliasGenerate);
        EXPECT_EQ(0, 1);
        return;
    }
    keyAliasGenerate.size = strlen((char *)keyAliasGenerate.data);
    statusGenerate = hks_generate_key(&keyAliasGenerate, &keyParam);
    EXPECT_EQ(0, statusGenerate);
    HksBlobDestroyT1(&keyAliasGenerate);
    struct hks_blob keyAliasDelete;
    HksStBlobInit1(&keyAliasDelete, sizeof(uint8_t), sizeof(testFileName) + NUM3, HKS_BLOB_TYPE_ALIAS);
    if (memcpy_s(keyAliasDelete.data, sizeof(testFileName), testFileName, sizeof(testFileName)) != EOK) {
        HksBlobDestroyT1(&keyAliasDelete);
        EXPECT_EQ(0, 1);
        return;
    }
    char tmpDelete[NUM3] = { 0 };
    if (strcat_s((char *)keyAliasDelete.data,
        strlen((char *)keyAliasDelete.data) + strlen(tmpDelete) + 1,
        tmpDelete) != EOK) {
        HksBlobDestroyT1(&keyAliasDelete);
        EXPECT_EQ(0, 1);
        return;
    }
    keyAliasDelete.size = strlen((char *)keyAliasDelete.data);
    statusDelete = hks_delete_key(&keyAliasDelete);
    EXPECT_EQ(0, statusDelete);
    HksBlobDestroyT1(&keyAliasDelete);
};

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0010
 * @tc.name      : Generate key, abnormal input parameters keyAlias is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0010, Function | MediumTest | Level2)
{
    char testFileName1[] = "key_auth_id1";
    struct hks_blob *keyAlias = nullptr;
    int32_t status;

    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;

    status = hks_generate_key(keyAlias, &keyParam);
    EXPECT_EQ(NUM1000, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0020
 * @tc.name      : Generate key, abnormal input parameters keyAlias.size is 0
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0020, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    struct hks_blob keyAlias;
    int32_t status;

    keyAlias.type = HKS_BLOB_TYPE_ALIAS;
    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = 0;

    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;

    status = hks_generate_key(&keyAlias, &keyParam);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0030
 * @tc.name      : Generate key, abnormal input parameters keyAlias.size is more than 64
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0030, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    struct hks_blob keyAlias;
    int32_t status;

    keyAlias.type = HKS_BLOB_TYPE_ALIAS;
    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = NUM65;

    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;

    status = hks_generate_key(&keyAlias, &keyParam);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0040
 * @tc.name      : Generate key, abnormal input parameters keyAlias.type is not equal to HKS_BLOB_TYPE_ALIAS
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0040, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    struct hks_blob keyAlias;
    int32_t status;

    keyAlias.type = 0;
    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = sizeof(testFileName);

    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;

    status = hks_generate_key(&keyAlias, &keyParam);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0050
 * @tc.name      : Generate key, abnormal input parameters keyAlias.data is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0050, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    struct hks_blob keyAlias;

    keyAlias.type = HKS_BLOB_TYPE_ALIAS;
    keyAlias.data = NULL;
    keyAlias.size = sizeof(testFileName);

    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;

    int32_t status = hks_generate_key(&keyAlias, &keyParam);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0060
 * @tc.name      : Generate key, abnormal input parameters keyParam is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0060, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1";
    struct hks_blob keyAlias;
    int32_t status;

    keyAlias.type = HKS_BLOB_TYPE_ALIAS;
    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = sizeof(testFileName);

    struct hks_key_param *keyParam = nullptr;

    status = hks_generate_key(&keyAlias, keyParam);
    EXPECT_EQ(NUM1000, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0070
 * @tc.name      : Generate key, abnormal input parameters keyParam.key_type
                   is not equal to HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0070, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    struct hks_blob keyAlias;
    int32_t status;

    keyAlias.type = HKS_BLOB_TYPE_ALIAS;
    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = sizeof(testFileName);

    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = 1;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;

    status = hks_generate_key(&keyAlias, &keyParam);
    EXPECT_EQ(NUM134, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0080
 * @tc.name      : Generate key, abnormal input parameters keyParam.key_auth_id.size is 0
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0080, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    struct hks_blob keyAlias;
    int32_t status;

    keyAlias.type = HKS_BLOB_TYPE_ALIAS;
    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = sizeof(testFileName);

    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = 0;
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;

    status = hks_generate_key(&keyAlias, &keyParam);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0090
 * @tc.name      : Generate key, abnormal input parameters keyParam.key_auth_id.size is more than 64
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0090, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    struct hks_blob keyAlias;
    int32_t status;

    keyAlias.type = HKS_BLOB_TYPE_ALIAS;
    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = sizeof(testFileName);

    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = NUM65;
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;

    status = hks_generate_key(&keyAlias, &keyParam);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0100
 * @tc.name      : Generate key, abnormal input parameters keyParam.key_auth_id.type
                   is not equal to HKS_BLOB_TYPE_AUTHID
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0100, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    struct hks_blob keyAlias;
    int32_t status;

    keyAlias.type = HKS_BLOB_TYPE_ALIAS;
    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = sizeof(testFileName);

    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = 0;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;

    status = hks_generate_key(&keyAlias, &keyParam);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0110
 * @tc.name      : Generate key, abnormal input parameters keyParam.key_auth_id.data is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0110, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    struct hks_blob keyAlias;
    int32_t status;

    keyAlias.type = HKS_BLOB_TYPE_ALIAS;
    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = sizeof(testFileName);

    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = NULL;
    keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;

    status = hks_generate_key(&keyAlias, &keyParam);
    EXPECT_EQ(NUM1000, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0120
 * @tc.name      : Generate key, the number of stored keys is more than 20
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataGenerateKey0120, Function | MediumTest | Level2)
{
    char testFileName[] = "keyalias1", testFileName1[] = "key_auth_id1";
    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t*)testFileName1; keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID; keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0; keyParam.key_usage = 0; keyParam.key_pad = 0;
    for (int j = 0; j < NUM21; j++) {
        struct hks_blob keyAlias;
        HksStBlobInit1(&keyAlias, sizeof(uint8_t), sizeof(testFileName) + NUM3, HKS_BLOB_TYPE_ALIAS);
        if (memcpy_s(keyAlias.data, sizeof(testFileName), testFileName, sizeof(testFileName)) != EOK) {
            HksBlobDestroyT1(&keyAlias);
            EXPECT_EQ(0, 1);
            return;
        }
        char tmp[NUM3] = { 0 };
        sprintf_s(tmp, sizeof(tmp), "%d", j);
        if (strcat_s((char*)keyAlias.data, strlen((char*)keyAlias.data) + strlen(tmp) + 1, tmp) != EOK) {
            HksBlobDestroyT1(&keyAlias);
            EXPECT_EQ(0, 1);
            return;
        }
        keyAlias.size = strlen((char*)keyAlias.data);
        int status = hks_generate_key(&keyAlias, &keyParam);
        if (j < NUM20){EXPECT_EQ(0, status); }
        else {EXPECT_EQ(NUM142, status); }
        HksBlobDestroyT1(&keyAlias);
    }
    for (int j = 0; j < NUM20; j++) {
        struct hks_blob keyAlias;
        HksStBlobInit1(&keyAlias, sizeof(uint8_t), sizeof(testFileName) + NUM3, HKS_BLOB_TYPE_ALIAS);
        if (memcpy_s(keyAlias.data, sizeof(testFileName), testFileName, sizeof(testFileName)) != EOK) {
            HksBlobDestroyT1(&keyAlias);
            EXPECT_EQ(0, 1);
            return;
        }
        char tmp[NUM3] = { 0 };
        sprintf_s(tmp, sizeof(tmp), "%d", j);
        if (strcat_s((char*)keyAlias.data, strlen((char*)keyAlias.data) + strlen(tmp) + 1, tmp) != EOK) {
            HksBlobDestroyT1(&keyAlias);
            EXPECT_EQ(0, 1);
            return;
        }
        keyAlias.size = strlen((char*)keyAlias.data);
        int status = hks_delete_key(&keyAlias);
        EXPECT_EQ(0, status);
        HksBlobDestroyT1(&keyAlias);
    }
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++generate key
// end+++++++++++++++++++++++++++++++++++++++++++++++++++++0000-0120

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Delete key
// begin+++++++++++++++++++++++++++++++++++++++++++++++++++++0130-0190

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0130
 * @tc.name      : Delete key, normal input parameters keyAlias and keyParam
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataDeleteKey0130, Function | MediumTest | Level1)
{
    char testFileName[] = "keyalias1";
    char testFileName1[] = "key_auth_id1";
    int32_t statusGenerate;
    int32_t statusDelete;
    struct hks_key_param keyParam;
    keyParam.key_auth_id.data = (uint8_t *)testFileName1;
    keyParam.key_auth_id.size = sizeof(testFileName1);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;
    keyParam.key_type = HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519;
    keyParam.key_len = 0;
    keyParam.key_usage = 0;
    keyParam.key_pad = 0;
    struct hks_blob keyAliasGenerate;
    HksStBlobInit1(&keyAliasGenerate, sizeof(uint8_t), sizeof(testFileName) + NUM3, HKS_BLOB_TYPE_ALIAS);
    if (memcpy_s(keyAliasGenerate.data, sizeof(testFileName), testFileName, sizeof(testFileName)) != EOK) {
        HksBlobDestroyT1(&keyAliasGenerate);
        EXPECT_EQ(0, 1);
        return;
    }
    char tmpGenerate[NUM3] = { 0 };
    if (strcat_s((char *)keyAliasGenerate.data,
        strlen((char *)keyAliasGenerate.data) + strlen(tmpGenerate) + 1,
        tmpGenerate) != EOK) {
        HksBlobDestroyT1(&keyAliasGenerate);
        EXPECT_EQ(0, 1);
        return;
    }
    keyAliasGenerate.size = strlen((char *)keyAliasGenerate.data);
    statusGenerate = hks_generate_key(&keyAliasGenerate, &keyParam);
    EXPECT_EQ(0, statusGenerate);
    HksBlobDestroyT1(&keyAliasGenerate);
    struct hks_blob keyAliasDelete;
    HksStBlobInit1(&keyAliasDelete, sizeof(uint8_t), sizeof(testFileName) + NUM3, HKS_BLOB_TYPE_ALIAS);
    if (memcpy_s(keyAliasDelete.data, sizeof(testFileName), testFileName, sizeof(testFileName)) != EOK) {
        HksBlobDestroyT1(&keyAliasDelete);
        EXPECT_EQ(0, 1);
        return;
    }
    char tmpDelete[NUM3] = { 0 };
    if (strcat_s((char *)keyAliasDelete.data,
        strlen((char *)keyAliasDelete.data) + strlen(tmpDelete) + 1,
        tmpDelete) != EOK) {
        HksBlobDestroyT1(&keyAliasDelete);
        EXPECT_EQ(0, 1);
        return;
    }
    keyAliasDelete.size = strlen((char *)keyAliasDelete.data);
    statusDelete = hks_delete_key(&keyAliasDelete);
    EXPECT_EQ(0, statusDelete);
    HksBlobDestroyT1(&keyAliasDelete);
};

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0140
 * @tc.name      : Delete key, abnormal input parameters keyAlias is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataDeleteKey0140, Function | MediumTest | Level2)
{
    int32_t status;
    struct hks_blob *keyAlias = nullptr;

    status = hks_delete_key(keyAlias);
    EXPECT_EQ(NUM1000, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0150
 * @tc.name      : Delete key, abnormal input parameters keyAlias.data is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataDeleteKey0150, Function | MediumTest | Level2)
{
    int32_t status;
    char testFileName[] = "Test_file_north_interfaces";
    struct hks_blob keyAlias = { 0 };

    keyAlias.data = NULL;
    keyAlias.size = sizeof(testFileName);
    keyAlias.type = HKS_BLOB_TYPE_ALIAS;

    status = hks_delete_key(&keyAlias);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0160
 * @tc.name      : Delete key, abnormal input parameters keyAlias.type is not equal to HKS_BLOB_TYPE_ALIAS
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataDeleteKey0160, Function | MediumTest | Level2)
{
    int32_t status;
    char testFileName[] = "Test_file_north_interfaces";
    struct hks_blob keyAlias = { 0 };

    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = sizeof(testFileName);
    keyAlias.type = 0;

    status = hks_delete_key(&keyAlias);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0170
 * @tc.name      : Delete key, abnormal input parameters keyAlias.size is 0
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataDeleteKey0170, Function | MediumTest | Level2)
{
    int32_t status;
    char testFileName[] = "Test_file_north_interfaces";
    struct hks_blob keyAlias = { 0 };

    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = 0;
    keyAlias.type = HKS_BLOB_TYPE_ALIAS;

    status = hks_delete_key(&keyAlias);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0180
 * @tc.name      : Delete key, abnormal input parameters keyAlias.size is more than 64
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataDeleteKey0180, Function | MediumTest | Level2)
{
    int32_t status;
    char testFileName[] = "Test_file_north_interfaces";
    struct hks_blob keyAlias = { 0 };

    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = NUM65;
    keyAlias.type = HKS_BLOB_TYPE_ALIAS;

    status = hks_delete_key(&keyAlias);
    EXPECT_EQ(NUM135, status);
}

/* *
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0190
 * @tc.name      : Delete key, the key does not exist
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksGenDelTestSuite, securityDataDeleteKey0190, Function | MediumTest | Level2)
{
    int32_t status;
    char testFileName[] = "Test_file_north_interfaces";
    struct hks_blob keyAlias = { 0 };

    keyAlias.data = (uint8_t *)testFileName;
    keyAlias.size = sizeof(testFileName);
    keyAlias.type = HKS_BLOB_TYPE_ALIAS;

    status = hks_delete_key(&keyAlias);
    EXPECT_EQ(NUM1010, status);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Delete key
// end+++++++++++++++++++++++++++++++++++++++++++++++++++++++0130-0190
