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

class SecurityDataHuksDecTestSuite : public testing::Test {
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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Decrypt key
// begin++++++++++++++++++++++++++++++++++++++++++++++++++++0760-0990

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0760
 * @tc.name      : Aead Decrypt, normal input parameters key, keyParam.ken_len is 128, cryptParam, plaintext, ciphertext
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0760, Function | MediumTest | Level1)
{
    char alias[] = "test_hks_aead_encrypt";
    int32_t statusEncrypt;
    int32_t statusDecrypt;

    struct hks_blob key;
    HksStBlobInit1(&key, 1, NUM16, HKS_BLOB_TYPE_KEY);
    hks_generate_random(&key);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;
    struct hks_blob nonce = {0};
    HksStBlobInit1(&nonce, 1, NUM16, HKS_BLOB_TYPE_IV);
    hks_generate_random(&nonce);

    struct hks_blob aad = {0};
    HksStBlobInit1(&aad, 1, NUM16, HKS_BLOB_TYPE_AAD);
    hks_generate_random(&aad);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob plaintext;
    HksStBlobInit1(&plaintext, 1, NUM64, HKS_BLOB_TYPE_PLAIN_TEXT);
    hks_generate_random(&plaintext);

    struct hks_blob ciphertext;
    HksStBlobInit1(&ciphertext, 1, NUM64 + HKS_SALT_MAX_SIZE, HKS_BLOB_TYPE_CIPHER_TEXT);
    statusEncrypt = hks_aead_encrypt(&key, &keyParam, &cryptParam, &plaintext, &ciphertext);
    EXPECT_EQ(0, statusEncrypt);

    struct hks_blob decrypted;
    HksStBlobInit1(&decrypted, 1, NUM64, HKS_BLOB_TYPE_PLAIN_TEXT);
    statusDecrypt = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(0, statusDecrypt);

    size_t k;
    for (k = 0; k < decrypted.size; k++) {
        EXPECT_EQ(plaintext.data[k], decrypted.data[k]);
    }
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&nonce);
    HksBlobDestroyT1(&aad);
    HksBlobDestroyT1(&plaintext);
    HksBlobDestroyT1(&ciphertext);
    HksBlobDestroyT1(&decrypted);
};

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0770
 * @tc.name      : Aead Decrypt, normal input parameters key, keyParam.ken_len is 192, cryptParam, plaintext, ciphertext
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0770, Function | MediumTest | Level1)
{
    char alias[] = "test_hks_aead_encrypt";
    int32_t statusEncrypt;
    int32_t statusDecrypt;

    struct hks_blob key;
    HksStBlobInit1(&key, 1, NUM24, HKS_BLOB_TYPE_KEY);
    hks_generate_random(&key);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM192;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;
    struct hks_blob nonce = {0};
    HksStBlobInit1(&nonce, 1, NUM16, HKS_BLOB_TYPE_IV);
    hks_generate_random(&nonce);

    struct hks_blob aad = {0};
    HksStBlobInit1(&aad, 1, NUM16, HKS_BLOB_TYPE_AAD);
    hks_generate_random(&aad);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob plaintext;
    HksStBlobInit1(&plaintext, 1, NUM64, HKS_BLOB_TYPE_PLAIN_TEXT);
    hks_generate_random(&plaintext);

    struct hks_blob ciphertext;
    HksStBlobInit1(&ciphertext, 1, NUM64 + HKS_SALT_MAX_SIZE, HKS_BLOB_TYPE_CIPHER_TEXT);
    statusEncrypt = hks_aead_encrypt(&key, &keyParam, &cryptParam, &plaintext, &ciphertext);
    EXPECT_EQ(0, statusEncrypt);

    struct hks_blob decrypted;
    HksStBlobInit1(&decrypted, 1, NUM64, HKS_BLOB_TYPE_PLAIN_TEXT);
    statusDecrypt = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(0, statusDecrypt);

    size_t k;
    for (k = 0; k < decrypted.size; k++) {
        EXPECT_EQ(plaintext.data[k], decrypted.data[k]);
    }
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&nonce);
    HksBlobDestroyT1(&aad);
    HksBlobDestroyT1(&plaintext);
    HksBlobDestroyT1(&ciphertext);
    HksBlobDestroyT1(&decrypted);
};

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0780
 * @tc.name      : Aead Decrypt, normal input parameters key, keyParam.ken_len is 256, cryptParam, plaintext, ciphertext
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0780, Function | MediumTest | Level1)
{
    char alias[] = "test_hks_aead_encrypt";
    int32_t statusEncrypt;
    int32_t statusDecrypt;

    struct hks_blob key;
    HksStBlobInit1(&key, 1, NUM32, HKS_BLOB_TYPE_KEY);
    hks_generate_random(&key);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM256;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;
    struct hks_blob nonce = {0};
    HksStBlobInit1(&nonce, 1, NUM16, HKS_BLOB_TYPE_IV);
    hks_generate_random(&nonce);

    struct hks_blob aad = {0};
    HksStBlobInit1(&aad, 1, NUM16, HKS_BLOB_TYPE_AAD);
    hks_generate_random(&aad);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob plaintext;
    HksStBlobInit1(&plaintext, 1, NUM64, HKS_BLOB_TYPE_PLAIN_TEXT);
    hks_generate_random(&plaintext);

    struct hks_blob ciphertext;
    HksStBlobInit1(&ciphertext, 1, NUM64 + HKS_SALT_MAX_SIZE, HKS_BLOB_TYPE_CIPHER_TEXT);
    statusEncrypt = hks_aead_encrypt(&key, &keyParam, &cryptParam, &plaintext, &ciphertext);
    EXPECT_EQ(0, statusEncrypt);

    struct hks_blob decrypted;
    HksStBlobInit1(&decrypted, 1, NUM64, HKS_BLOB_TYPE_PLAIN_TEXT);
    statusDecrypt = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(0, statusDecrypt);

    size_t k;
    for (k = 0; k < decrypted.size; k++) {
        EXPECT_EQ(plaintext.data[k], decrypted.data[k]);
    }
    HksBlobDestroyT1(&key);
    HksBlobDestroyT1(&nonce);
    HksBlobDestroyT1(&aad);
    HksBlobDestroyT1(&plaintext);
    HksBlobDestroyT1(&ciphertext);
    HksBlobDestroyT1(&decrypted);
};

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0790
 * @tc.name      : Aead Decrypt, abnormal input parameters key is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0790, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob *key = nullptr;

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0800
 * @tc.name      : Aead Decrypt, abnormal input parameters key.size is not equal to keyParam.key_len divided by 8
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0800, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = NUM32;
    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM1006, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0810
 * @tc.name      : Aead Decrypt, abnormal input parameters key.data is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0810, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = NULL;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM1006, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0820
 * @tc.name      : Aead Decrypt, abnormal input parameters key.type is not equal to HKS_KEY_TYPE_AES
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0820, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = 0;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM1006, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0830
 * @tc.name      : Aead Decrypt, abnormal input parameters keyParam is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0830, Function | MediumTest | Level2)
{
    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param *keyParam = nullptr;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0840
 * @tc.name      : Aead Decrypt, abnormal input parameters keyParam.key_mode is not equal to HKS_ALG_GCM
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0840, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = 0;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM134, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0850
 * @tc.name      : Aead Decrypt, abnormal input parameters keyParam.key_len is not equal to 128, 192, 256
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0850, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM12;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM134, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0860
 * @tc.name      : Aead Decrypt, abnormal input parameters keyParam.key_type is not equal to HKS_KEY_TYPE_AES
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0860, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = 0;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM134, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0870
 * @tc.name      : Aead Decrypt, abnormal input parameters keyParam.key_pad is not equal to HKS_PADDING_NONE
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0870, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = NUM88;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM134, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0880
 * @tc.name      : Aead Decrypt, abnormal input parameters keyParam.key_usage
                   is not equal to HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0880, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = 0;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM134, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0890
 * @tc.name      : Aead Decrypt, abnormal input parameters cryptParam is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0890, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";
    uint8_t key1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param *cryptParam = nullptr;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0900
 * @tc.name      : Aead Decrypt, abnormal input parameters nonce.size is 0
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0900, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = 0;

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0910
 * @tc.name      : Aead Decrypt, abnormal input parameters nonce.data is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0910, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = NULL;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0920
 * @tc.name      : Aead Decrypt, abnormal input parameters aad.size is 0
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0920, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = 0;

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0930
 * @tc.name      : Aead Decrypt, abnormal input parameters aad.data is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0930, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = NULL;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0940
 * @tc.name      : Aead Decrypt, abnormal input decrypted plaintext is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0940, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob *decrypted = nullptr;

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, decrypted, &ciphertext);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0950
 * @tc.name      : Aead Decrypt, abnormal input decrypted plaintext.data is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0950, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = NULL;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0960
 * @tc.name      : Aead Decrypt, abnormal input parameters decrypted.size is less than ciphertext.size minus 16
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0960, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = NUM15;

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0970
 * @tc.name      : Aead Decrypt, abnormal input parameters ciphertext.size is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0970, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob *ciphertext = nullptr;

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, ciphertext);
    EXPECT_EQ(NUM1000, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0980
 * @tc.name      : Aead Decrypt, abnormal input parameters ciphertext.data is null
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0980, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = NULL;
    ciphertext.size = sizeof(ciphertext1);

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM135, status);
}

/*
 * @tc.number    : SUB_SEC_DataPro_HuksL1_0990
 * @tc.name      : Aead Decrypt, abnormal input parameters ciphertext.size is less than 16
 * @tc.desc      : [C- SECURITY -1500]
 */
HWTEST_F(SecurityDataHuksDecTestSuite, securityDataAeadDecrypt0990, Function | MediumTest | Level2)
{
    char alias[] = "test_hks_aead_decrypt";

    uint8_t key1[NUM16], nonce1[NUM16], aad1[NUM16], decrypted1[NUM100], ciphertext1[NUM100];

    int32_t status;
    struct hks_blob key;
    key.type = HKS_BLOB_TYPE_KEY;
    key.data = (uint8_t *)key1;
    key.size = sizeof(key1);

    struct hks_key_param keyParam;
    keyParam.key_type = HKS_KEY_TYPE_AES;
    keyParam.key_len = NUM128;
    keyParam.key_usage = HKS_KEY_USAGE_ENCRYPT | HKS_KEY_USAGE_DECRYPT;
    keyParam.key_mode = HKS_ALG_GCM;
    keyParam.key_pad = HKS_PADDING_NONE;
    keyParam.key_auth_id.data = (uint8_t *)alias;
    keyParam.key_auth_id.size = sizeof(alias);
    keyParam.key_auth_id.type = HKS_BLOB_TYPE_AUTH_ID;

    struct hks_crypt_param cryptParam;

    struct hks_blob nonce = {0};
    nonce.data = (uint8_t *)nonce1;
    nonce.size = sizeof(nonce1);

    struct hks_blob aad = {0};
    aad.data = (uint8_t *)aad1;
    aad.size = sizeof(aad1);

    cryptParam.nonce = nonce;
    cryptParam.aad = aad;

    struct hks_blob decrypted;
    decrypted.data = (uint8_t *)decrypted1;
    decrypted.size = sizeof(decrypted1);

    struct hks_blob ciphertext;
    ciphertext.data = (uint8_t *)ciphertext1;
    ciphertext.size = NUM10;

    status = hks_aead_decrypt(&key, &keyParam, &cryptParam, &decrypted, &ciphertext);
    EXPECT_EQ(NUM135, status);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Decrypt key
// begin++++++++++++++++++++++++++++++++++++++++++++++++++++0760-0990

