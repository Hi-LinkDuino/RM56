/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "hap_signing_block_utils_test.h"

#include <fstream>
#include <map>

#include <gtest/gtest.h>

#include "common/hap_byte_buffer_data_source.h"
#include "common/random_access_file.h"
#include "util/hap_signing_block_utils.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;

namespace OHOS {
namespace Security {
namespace Verify {
void CreateHapSubSignBlockHead(HapSubSignBlockHead& signBlob, HapSubSignBlockHead& profileBlob,
    HapSubSignBlockHead& propertyBlob)
{
    signBlob.type = HAP_SIGN_BLOB;
    signBlob.length = TEST_FILE_BLOCK_LENGTH;
    signBlob.offset = sizeof(HapSubSignBlockHead) * TEST_FILE_BLOCK_COUNT;
    profileBlob.type = PROFILE_BLOB;
    profileBlob.length = TEST_FILE_BLOCK_LENGTH;
    profileBlob.offset = signBlob.offset + signBlob.length;
    propertyBlob.type = PROPERTY_BLOB;
    propertyBlob.length = TEST_FILE_BLOCK_LENGTH;
    propertyBlob.offset = profileBlob.offset + profileBlob.length;
}

long long CreatTestZipFile(const std::string& pathFile, SignatureInfo& signInfo)
{
    std::ofstream hapFile(pathFile.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    if (!hapFile.is_open()) {
        return 0;
    }
    char block[TEST_FILE_BLOCK_LENGTH] = {0};
    /* input contents of ZIP entries */
    hapFile.seekp(0, std::ios_base::beg);
    hapFile.write(block, sizeof(block));
    /* input sign block */
    HapSubSignBlockHead signBlob;
    HapSubSignBlockHead profileBlob;
    HapSubSignBlockHead propertyBlob;
    CreateHapSubSignBlockHead(signBlob, profileBlob, propertyBlob);
    hapFile.write(reinterpret_cast<char*>(&signBlob), sizeof(signBlob));
    hapFile.write(reinterpret_cast<char*>(&profileBlob), sizeof(profileBlob));
    hapFile.write(reinterpret_cast<char*>(&propertyBlob), sizeof(propertyBlob));
    for (int i = 0; i < TEST_FILE_BLOCK_COUNT; i++) {
        hapFile.write(block, sizeof(block));
    }
    int blockCount = TEST_FILE_BLOCK_COUNT;
    hapFile.write(reinterpret_cast<char*>(&blockCount), sizeof(blockCount));
    long long signBlockSize = (sizeof(HapSubSignBlockHead) + sizeof(block)) * TEST_FILE_BLOCK_COUNT +
        HapSigningBlockUtils::ZIP_HEAD_OF_SIGNING_BLOCK_LENGTH;
    hapFile.write(reinterpret_cast<char*>(&signBlockSize), sizeof(signBlockSize));
    long long magic = HapSigningBlockUtils::HAP_SIG_BLOCK_MAGIC_LOW_OLD;
    hapFile.write(reinterpret_cast<char*>(&magic), sizeof(magic));
    magic = HapSigningBlockUtils::HAP_SIG_BLOCK_MAGIC_HIGH_OLD;
    hapFile.write(reinterpret_cast<char*>(&magic), sizeof(magic));
    int version = 1;
    hapFile.write(reinterpret_cast<char*>(&version), sizeof(version));
    /* input central direction */
    hapFile.write(block, sizeof(block));
    /* input end of central direction */
    int zidEocdSign = HapSigningBlockUtils::ZIP_EOCD_SEGMENT_FLAG;
    hapFile.write(reinterpret_cast<char*>(&zidEocdSign), sizeof(zidEocdSign));
    hapFile.write(reinterpret_cast<char*>(&magic), sizeof(magic));
    unsigned int centralDirLen = sizeof(block);
    hapFile.write(reinterpret_cast<char*>(&centralDirLen), sizeof(centralDirLen));
    unsigned int centralDirOffset = TEST_FILE_BLOCK_LENGTH + signBlockSize;
    hapFile.write(reinterpret_cast<char*>(&centralDirOffset), sizeof(centralDirOffset));
    short eocdCommentLen = 0;
    hapFile.write(reinterpret_cast<char*>(&eocdCommentLen), sizeof(eocdCommentLen));
    hapFile.close();
    signInfo.hapCentralDirOffset = centralDirOffset;
    signInfo.hapEocdOffset = centralDirOffset + centralDirLen;
    signInfo.hapSignatureBlock.SetCapacity(TEST_FILE_BLOCK_LENGTH);
    signInfo.hapSignatureBlock.PutData(0, block, sizeof(block));
    long long sumLen = signInfo.hapEocdOffset + sizeof(zidEocdSign) + sizeof(centralDirLen) +
        sizeof(centralDirOffset) + sizeof(magic) + sizeof(eocdCommentLen);
    return sumLen;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS

namespace {
class HapSigningBlockUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    static const int TEST_ZIP_BLOCKS_NUM_NEED_DIGEST;
};

const int HapSigningBlockUtilsTest::TEST_ZIP_BLOCKS_NUM_NEED_DIGEST = 3;

void HapSigningBlockUtilsTest::SetUpTestCase(void)
{
}

void HapSigningBlockUtilsTest::TearDownTestCase(void)
{
}

void HapSigningBlockUtilsTest::SetUp()
{
}

void HapSigningBlockUtilsTest::TearDown()
{
}

/**
 * @tc.name: Test FindHapSignature function
 * @tc.desc: input one right file and two error file, The static function will return correct result;
 * @tc.type: FUNC
 */
HWTEST_F(HapSigningBlockUtilsTest, FindHapSignatureTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. create a test zip file.
     */
    std::string pathFile = "./test_hapverify.hap";
    SignatureInfo signInfo;
    int sumLen = CreatTestZipFile(pathFile, signInfo);
    /*
     * @tc.steps: step2. test FindHapSignature function
     * @tc.expected: step2. the return will be true.
     */
    RandomAccessFile hapTestFile;
    ASSERT_TRUE(hapTestFile.Init(pathFile));
    ASSERT_EQ(hapTestFile.GetLength(), sumLen);
    HapSigningBlockUtils hapSignBlockUtils;
    SignatureInfo hapSignInfo;
    ASSERT_TRUE(hapSignBlockUtils.FindHapSignature(hapTestFile, hapSignInfo));
    /*
     * @tc.steps: step3. make central offset error, and test FindHapSignature function
     * @tc.expected: step3. can not find central direction, the return will be false.
     */
    std::ofstream hapFile;
    hapFile.open(pathFile.c_str(), std::ios::binary | std::ios::out);
    ASSERT_TRUE(hapFile.is_open());
    hapFile.seekp(sumLen - TEST_FILE_CD_OFFSET_POSITION, std::ios_base::beg);
    unsigned int errorCentralDirOffset = 20;
    hapFile.write(reinterpret_cast<char*>(&errorCentralDirOffset), sizeof(errorCentralDirOffset));
    hapFile.close();
    ASSERT_FALSE(hapSignBlockUtils.FindHapSignature(hapTestFile, hapSignInfo));
    /*
     * @tc.steps: step4. make eocdCommentLen error, and test FindHapSignature function
     * @tc.expected: step4. can not find eocd, the return will be false.
     */
    std::ofstream hapFile2;
    hapFile2.open(pathFile.c_str(), std::ios::binary | std::ios::out);
    ASSERT_TRUE(hapFile2.is_open());
    short errorEocdCommentLen = 10;
    hapFile2.write(reinterpret_cast<char*>(&errorEocdCommentLen), sizeof(errorEocdCommentLen));
    hapFile2.close();
    ASSERT_FALSE(hapSignBlockUtils.FindHapSignature(hapTestFile, hapSignInfo));
}

/**
 * @tc.name: Test VerifyHapIntegrity function
 * @tc.desc: create a file and input error digest, The static function will return false;
 * @tc.type: FUNC
 */
HWTEST_F(HapSigningBlockUtilsTest, VerifyHapIntegrityTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. create a test zip file without eocd.
     */
    std::string pathFile = "./test_hapverify.hap";
    SignatureInfo signInfo;
    CreatTestZipFile(pathFile, signInfo);
    /*
     * @tc.steps: step2. create an error digest to test VerifyHapIntegrity function
     * @tc.expected: step2. the return will be false.
     */
    Pkcs7Context digestInfo;
    digestInfo.content.SetCapacity(TEST_FILE_BLOCK_LENGTH);
    RandomAccessFile hapTestFile;
    hapTestFile.Init(pathFile);
    HapSigningBlockUtils hapSignBlockUtils;
    ASSERT_FALSE(hapSignBlockUtils.VerifyHapIntegrity(digestInfo, hapTestFile, signInfo));
}

/**
 * @tc.name: Test FindEocdInHap function
 * @tc.desc: create a file with invalid length, The function will return false;
 * @tc.type: FUNC
 */
HWTEST_F(HapSigningBlockUtilsTest, FindEocdInHapTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. create a test file with invalid length.
     */
    std::string pathFile = "./test_hapverify.hap";
    std::ofstream hapFile;
    hapFile.open(pathFile.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    ASSERT_TRUE(hapFile.is_open());
    unsigned int centralDirLen = TEST_FILE_BLOCK_LENGTH;
    hapFile.write(reinterpret_cast<char*>(&centralDirLen), sizeof(centralDirLen));
    hapFile.close();
    /*
     * @tc.steps: step2. run function
     * @tc.expected: step2. the return will be false.
     */
    RandomAccessFile hapTestFile;
    hapTestFile.Init(pathFile);
    std::pair<HapByteBuffer, long long> eocd;
    HapSigningBlockUtils hapSignBlockUtils;
    ASSERT_FALSE(hapSignBlockUtils.FindEocdInHap(hapTestFile, eocd));
    int maxCommentSize = TEST_INVALID_MAX_COMMENT_SIZE;
    ASSERT_FALSE(hapSignBlockUtils.FindEocdInHap(hapTestFile, maxCommentSize, eocd));
    maxCommentSize = TEST_MAX_COMMENT_SIZE;
    ASSERT_FALSE(hapSignBlockUtils.FindEocdInHap(hapTestFile, maxCommentSize, eocd));
    HapByteBuffer testHapBuffer(TEST_HAPBYTEBUFFER_LENGTH);
    int offset = 0;
    ASSERT_FALSE(hapSignBlockUtils.FindEocdInSearchBuffer(testHapBuffer, offset));
}

/**
 * @tc.name: Test GetCentralDirectoryOffset function
 * @tc.desc: create an ecod with invalid central offset and length,
 *           The function will return TEST_NOT_FIND_TARGET_OFFSET;
 * @tc.type: FUNC
 */
HWTEST_F(HapSigningBlockUtilsTest, GetCentralDirectoryOffsetTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. create a test eocd with invalid central offset and length.
     */
    HapByteBuffer testEocd(TEST_ZIP_EOCD_SIZE);
    int centralDirLen = TEST_FILE_BLOCK_LENGTH;
    testEocd.PutInt32(TEST_ZIP_ECD_SIZE_FIELD_OFFSET, centralDirLen);
    int centralDirOffset = TEST_FILE_BLOCK_LENGTH;
    testEocd.PutInt32(TEST_ZIP_ECD_OFFSET_FIELD_OFFSET, centralDirOffset);
    HapSigningBlockUtils hapSignBlockUtils;
    ASSERT_FALSE(hapSignBlockUtils.SetUnsignedInt32(testEocd, 0, TEST_INVALID_MAX_COMMENT_SIZE));
    HapByteBuffer emptyEocd;
    /*
     * @tc.steps: step2. run function with error eocdoffset
     * @tc.expected: step2. the return will be NOT_FIND_TARGET_OFFSET.
     */
    long long offset;
    ASSERT_FALSE(hapSignBlockUtils.GetCentralDirectoryOffset(emptyEocd, 0, offset));
    ASSERT_FALSE(hapSignBlockUtils.GetCentralDirectoryOffset(testEocd, 0, offset));
    ASSERT_FALSE(hapSignBlockUtils.GetCentralDirectoryOffset(testEocd, TEST_FILE_BLOCK_LENGTH, offset));
}

/**
 * @tc.name: Test ClassifyHapSubSigningBlock function
 * @tc.desc: test function of classify optional block
 * @tc.type: FUNC
 */
HWTEST_F(HapSigningBlockUtilsTest, ClassifyHapSubSigningBlock001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. run function with input of property block
     * @tc.expected: step1. the return signInfo.optionBlocks has property block which inputed.
     */
    SignatureInfo signInfo;
    HapByteBuffer subBlock;
    unsigned int type = PROPERTY_BLOB;
    HapSigningBlockUtils hapSignBlockUtils;
    hapSignBlockUtils.ClassifyHapSubSigningBlock(signInfo, subBlock, type);
    ASSERT_FALSE(signInfo.optionBlocks.empty());
    ASSERT_TRUE(signInfo.optionBlocks[0].optionalType == PROPERTY_BLOB);
}

/**
 * @tc.name: Test ComputeDigestsWithOptionalBlock function
 * @tc.desc: use an error nid and a right nid to compute digest
 * @tc.type: FUNC
 */
HWTEST_F(HapSigningBlockUtilsTest, ComputeDigestsWithOptionalBlock001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. run function with an error nid and a right nid
     * @tc.expected: step1. the return will be false and right respectively.
     */
    OptionalBlock testOptionalBlock;
    testOptionalBlock.optionalType = PROPERTY_BLOB;
    testOptionalBlock.optionalBlockValue.SetCapacity(TEST_HAPBYTEBUFFER_LENGTH);
    std::vector<OptionalBlock> optionalBlocks;
    optionalBlocks.push_back(testOptionalBlock);
    HapSigningBlockUtils hapSignBlockUtils;
    HapByteBuffer chunkDigest(TEST_HAPBYTEBUFFER_LENGTH);
    HapByteBuffer finalDigest;
    int nid = TEST_NULL_NID;
    DigestParameter errorParam = hapSignBlockUtils.GetDigestParameter(nid);
    int ret = hapSignBlockUtils.ComputeDigestsWithOptionalBlock(errorParam, optionalBlocks, chunkDigest, finalDigest);
    ASSERT_FALSE(ret);
    nid = TEST_SHA256_NID;
    DigestParameter digestParam = hapSignBlockUtils.GetDigestParameter(nid);
    ret = hapSignBlockUtils.ComputeDigestsWithOptionalBlock(digestParam, optionalBlocks, chunkDigest, finalDigest);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: Test GetSumOfChunkDigestLen function
 * @tc.desc: Test GetSumOfChunkDigestLen with some error inputs
 * @tc.type: FUNC
 */
HWTEST_F(HapSigningBlockUtilsTest, GetSumOfChunkDigestLenTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. input some error input to test GetSumOfChunkDigestLen
     * @tc.expected: step1. the return will be false.
     */
    int chunkCount = 0;
    int sumOfChunkDigestLen = 0;
    DataSource* contents[TEST_ZIP_BLOCKS_NUM_NEED_DIGEST] = { nullptr, nullptr, nullptr };
    bool ret = HapSigningBlockUtils::GetSumOfChunkDigestLen(contents, 0, 0, chunkCount, sumOfChunkDigestLen);
    ASSERT_FALSE(ret);
    ret = HapSigningBlockUtils::GetSumOfChunkDigestLen(contents,
        TEST_ZIP_BLOCKS_NUM_NEED_DIGEST, 0, chunkCount, sumOfChunkDigestLen);
    ASSERT_FALSE(ret);
    HapByteBuffer testBuffer(TEST_ZIP_EOCD_SIZE);
    HapByteBufferDataSource testSource(testBuffer);
    for (int i = 0; i < TEST_ZIP_BLOCKS_NUM_NEED_DIGEST; i++) {
        contents[i] = &testSource;
    }
    ret = HapSigningBlockUtils::GetSumOfChunkDigestLen(contents,
        TEST_ZIP_BLOCKS_NUM_NEED_DIGEST, INT_MAX, chunkCount, sumOfChunkDigestLen);
    ASSERT_FALSE(ret);
}
}
