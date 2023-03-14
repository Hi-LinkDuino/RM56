/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "softbus_errcode.h"
#include "softbus_adapter_crypto.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
class DsoftbusCryptoTest : public testing::Test {
protected:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};
void DsoftbusCryptoTest::SetUpTestCase(void)
{
}
void DsoftbusCryptoTest::TearDownTestCase(void)
{
}
void DsoftbusCryptoTest::SetUp()
{
}
void DsoftbusCryptoTest::TearDown()
{
}

/*
* @tc.name: SoftBusBase64Decode001
* @tc.desc: parameters is Legal
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusBase64Encode001, TestSize.Level0)
{
    char dst[100];
    char src[] = "abcde";
    size_t olen = 10;
    int32_t ret = SoftBusBase64Encode((unsigned char *)dst, sizeof(dst), &olen, (unsigned char *)src, sizeof(src));
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/*
* @tc.name: SoftBusBase64Decode002
* @tc.desc: parameter is nullptr
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusBase64Encode002, TestSize.Level0)
{
    char dst[100];
    char src[] = "abcde";
    size_t olen = 10;
    int32_t ret = SoftBusBase64Encode(NULL, sizeof(dst), &olen, (unsigned char *)src, sizeof(src));
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SoftBusBase64Encode((unsigned char *)dst, sizeof(dst), NULL, (unsigned char *)src, sizeof(src));
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SoftBusBase64Encode((unsigned char *)dst, sizeof(dst), &olen, NULL, sizeof(src));
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusBase64Decode003
* @tc.desc: dlen and slen is illegal
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusBase64Encode003, TestSize.Level0)
{
    char dst[100];
    char src[] = "abcde";
    size_t olen = 10;
    size_t dlen = 0;
    int32_t ret = SoftBusBase64Encode((unsigned char *)dst, dlen, &olen, (unsigned char *)src, sizeof(src));
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    size_t slen = 0;
    ret = SoftBusBase64Encode((unsigned char *)dst, sizeof(dst), &olen, (unsigned char *)src, slen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusBase64Encode001
* @tc.desc: parameters is Legal
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusBase64Decode001, TestSize.Level0)
{
    char dst[100];
    char src[] = "abcde";
    size_t dlen = 1;
    size_t olen = 10;
    size_t slen = 1;
    int32_t ret = SoftBusBase64Decode((unsigned char *)dst, dlen, &olen, (unsigned char *)src, slen);
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/*
* @tc.name: SoftBusBase64Encode002
* @tc.desc: parameters is nullptr
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusBase64Decode002, TestSize.Level0)
{
    char dst[100];
    char src[] = "abcde";
    size_t dlen = 1;
    size_t olen = 10;
    size_t slen = 1;

    int32_t ret = SoftBusBase64Decode(NULL, dlen, &olen, (unsigned char *)src, slen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SoftBusBase64Decode((unsigned char *)dst, dlen, NULL, (unsigned char *)src, slen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SoftBusBase64Decode((unsigned char *)dst, dlen, &olen, NULL, slen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusBase64Encode003
* @tc.desc: dlen and slen is illegal
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusBase64Decode003, TestSize.Level0)
{
    char dst[100];
    char src[] = "abcde";
    size_t dlen = 1;
    size_t olen = 10;
    size_t slen = 1;
    size_t dlen1 = 0;
    int32_t ret = SoftBusBase64Decode((unsigned char *)dst, dlen1, &olen, (unsigned char *)src, slen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    size_t slen1 = 0;
    SoftBusBase64Decode((unsigned char *)dst, dlen, &olen, (unsigned char *)src, slen1);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusGenerateSessionKey001
* @tc.desc: parameters is Legal
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusGenerateSessionKey001, TestSize.Level0)
{
    char key[10];
    int32_t ret = SoftBusGenerateSessionKey(key, sizeof(key));
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/*
* @tc.name: SoftBusGenerateSessionKey002
* @tc.desc: key is nullptr
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusGenerateSessionKey002, TestSize.Level0)
{
    int32_t len = 10;
    int32_t ret = SoftBusGenerateSessionKey(NULL, len);
    EXPECT_EQ(SOFTBUS_ENCRYPT_ERR, ret);
}

/*
* @tc.name: SoftBusGenerateSessionKey003
* @tc.desc: len is illegal
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusGenerateSessionKey003, TestSize.Level0)
{
    char key[10];
    int32_t len = 0;
    int32_t ret = SoftBusGenerateSessionKey(key, len);
    EXPECT_EQ(SOFTBUS_ENCRYPT_ERR, ret);
}

/*
* @tc.name: SoftBusGenerateStrHash001
* @tc.desc: parameters is Legal
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusGenerateStrHash001, TestSize.Level0)
{
    char str[] = "abcde";
    char hash[100];
    int32_t ret = SoftBusGenerateStrHash((unsigned char*)str, sizeof(str), (unsigned char*)hash);
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/*
* @tc.name: SoftBusGenerateStrHash002
* @tc.desc: str and hash is nullptr
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusGenerateStrHash002, TestSize.Level0)
{
    char str[] = "abcde";
    char hash[100];
    int32_t ret = SoftBusGenerateStrHash(NULL, sizeof(str), (unsigned char*)hash);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SoftBusGenerateStrHash((unsigned char*)str, sizeof(str), NULL);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    uint32_t len = 0;
    ret = SoftBusGenerateStrHash((unsigned char*)str, len, (unsigned char*)hash);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusGenerateRandomArrayTest001
* @tc.desc: randStr and len is valid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusGenerateRandomArrayTest001, TestSize.Level0)
{
    char randStr[64];
    int32_t ret = SoftBusGenerateRandomArray((unsigned char *)randStr, sizeof(randStr));
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/*
* @tc.name: SoftBusGenerateRandomArrayTest002
* @tc.desc: randStr and len is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusGenerateRandomArrayTest002, TestSize.Level0)
{
    uint32_t len = 1;
    char randStr[64];
    uint32_t len1 = 0;
    int32_t ret = SoftBusGenerateRandomArray(nullptr, len);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusGenerateRandomArray((unsigned char *)randStr, len1);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusEncryptData001
* @tc.desc: all valid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusEncryptData001, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    int32_t ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input,
                                     inLen, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/*
* @tc.name: SoftBusEncryptData002
* @tc.desc: cipherKey、input and inLen is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusEncryptData002, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    uint32_t inLen1 = 0;
    char encryptData[32];
    uint32_t encryptLen = 32;
    int32_t ret = SoftBusEncryptData(nullptr, (unsigned char*)input, inLen, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusEncryptData(&cipherKey, nullptr, inLen, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input, inLen1, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusEncryptData003
* @tc.desc: encryptData and encryptLen is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusEncryptData003, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    int32_t ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input, inLen, nullptr, &encryptLen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input, inLen, (unsigned char*)encryptData, nullptr);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusEncryptDataWithSeq001
* @tc.desc: all valid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusEncryptDataWithSeq001, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    int32_t seqNum = 1;
    int32_t ret = SoftBusEncryptDataWithSeq(&cipherKey, (unsigned char*)input,
                                            inLen, (unsigned char*)encryptData, &encryptLen, seqNum);
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/*
* @tc.name: SoftBusEncryptDataWithSeq002
* @tc.desc: cipherKey and input is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusEncryptDataWithSeq002, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    int32_t seqNum = 1;
    int32_t ret = SoftBusEncryptDataWithSeq(nullptr, (unsigned char*)input,
                                            inLen, (unsigned char*)encryptData, &encryptLen, seqNum);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusEncryptDataWithSeq(&cipherKey, nullptr, inLen, (unsigned char*)encryptData, &encryptLen, seqNum);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusEncryptDataWithSeq003
* @tc.desc: inLen is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusEncryptDataWithSeq003, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 0;
    char encryptData[32];
    uint32_t encryptLen = 32;
    int32_t seqNum = 1;
    int32_t ret = SoftBusEncryptDataWithSeq(&cipherKey, (unsigned char*)input,
                                            inLen, (unsigned char*)encryptData, &encryptLen, seqNum);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusEncryptDataWithSeq004
* @tc.desc: encryptData and encryptLen is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusEncryptDataWithSeq004, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    int32_t seqNum = 1;
    int32_t ret = SoftBusEncryptDataWithSeq(&cipherKey, (unsigned char*)input,
                                            inLen, nullptr, &encryptLen, seqNum);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusEncryptDataWithSeq(&cipherKey, (unsigned char*)input,
                                    inLen, (unsigned char*)encryptData, nullptr, seqNum);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusDecryptData001
* @tc.desc: all valid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusDecryptData001, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    char decryptData[32];
    uint32_t decryptLen = 32;
    int32_t ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input,
                                     inLen, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_OK, ret);
    ret = SoftBusDecryptData(&cipherKey, (unsigned char*)encryptData,
                             encryptLen, (unsigned char*)decryptData, &decryptLen);
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/*
* @tc.name: SoftBusDecryptData002
* @tc.desc: cipherKey、input and inLen is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusDecryptData002, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    uint32_t inLen1 = 0;
    char encryptData[32];
    uint32_t encryptLen = 32;
    char decryptData[32];
    uint32_t decryptLen = 32;
    int32_t ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input,
                                     inLen, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_OK, ret);
    ret = SoftBusDecryptData(nullptr, (unsigned char*)encryptData,
                             encryptLen, (unsigned char*)decryptData, &decryptLen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusDecryptData(&cipherKey, nullptr,
                             encryptLen, (unsigned char*)decryptData, &decryptLen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusDecryptData(&cipherKey, (unsigned char*)encryptData,
                             inLen1, (unsigned char*)decryptData, &decryptLen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusDecryptData003
* @tc.desc: decryptData and decryptLen is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusDecryptData003, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    char decryptData[32];
    uint32_t decryptLen = 32;
    int32_t ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input,
                                     inLen, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_OK, ret);
    ret = SoftBusDecryptData(&cipherKey, (unsigned char*)encryptData,
                             encryptLen, nullptr, &decryptLen);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusDecryptData(&cipherKey, (unsigned char*)encryptData,
                             encryptLen, (unsigned char*)decryptData, nullptr);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusDecryptDataWithSeq001
* @tc.desc: all valid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusDecryptDataWithSeq001, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    char decryptData[32];
    uint32_t decryptLen = 32;
    int32_t seqNum = 1;
    int32_t ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input,
                                     inLen, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_OK, ret);
    ret = SoftBusDecryptDataWithSeq(&cipherKey, (unsigned char*)encryptData,
                                    encryptLen, (unsigned char*)decryptData, &decryptLen, seqNum);
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/*
* @tc.name: SoftBusDecryptDataWithSeq002
* @tc.desc: cipherKey and input is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusDecryptDataWithSeq002, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    char decryptData[32];
    uint32_t decryptLen = 32;
    int32_t seqNum = 1;
    int32_t ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input,
                                     inLen, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_OK, ret);
    ret = SoftBusDecryptDataWithSeq(nullptr, (unsigned char*)encryptData,
                                    encryptLen, (unsigned char*)decryptData, &decryptLen, seqNum);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusDecryptDataWithSeq(&cipherKey, nullptr,
                                    encryptLen, (unsigned char*)decryptData, &decryptLen, seqNum);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusDecryptDataWithSeq003
* @tc.desc: inLen is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusDecryptDataWithSeq003, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    uint32_t encryptLen1 = 0;
    char decryptData[32];
    uint32_t decryptLen = 32;
    int32_t seqNum = 1;
    int32_t ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input,
                                     inLen, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_OK, ret);
    ret = SoftBusDecryptDataWithSeq(&cipherKey, (unsigned char*)encryptData,
                                    encryptLen1, (unsigned char*)decryptData, &decryptLen, seqNum);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/*
* @tc.name: SoftBusDecryptDataWithSeq004
* @tc.desc: encryptData and encryptLen is invalid param
* @tc.type: FUNC
* @tc.require: 1
*/
HWTEST_F(DsoftbusCryptoTest, SoftBusDecryptDataWithSeq004, TestSize.Level0)
{
    AesGcmCipherKey cipherKey;
    cipherKey.keyLen = 32;
    char input[32];
    uint32_t inLen = 32;
    char encryptData[32];
    uint32_t encryptLen = 32;
    char decryptData[32];
    uint32_t decryptLen = 32;
    int32_t seqNum = 1;
    int32_t ret = SoftBusEncryptData(&cipherKey, (unsigned char*)input,
                                     inLen, (unsigned char*)encryptData, &encryptLen);
    EXPECT_EQ(SOFTBUS_OK, ret);
    ret = SoftBusDecryptDataWithSeq(&cipherKey, (unsigned char*)encryptData,
                                    encryptLen, nullptr, &decryptLen, seqNum);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
    ret = SoftBusDecryptDataWithSeq(&cipherKey, (unsigned char*)encryptData,
                                    encryptLen, (unsigned char*)decryptData, nullptr, seqNum);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}
}