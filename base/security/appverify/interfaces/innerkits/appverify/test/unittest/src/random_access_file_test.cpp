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

#include "random_access_file_test.h"

#include <string>

#include <gtest/gtest.h>

#include "common/hap_byte_buffer.h"
#include "common/random_access_file.h"
#include "interfaces/hap_verify_result.h"

#include "hap_signing_block_utils_test.h"
#include "test_const.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;

namespace {
class RandomAccessFileTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void RandomAccessFileTest::SetUpTestCase(void)
{
}

void RandomAccessFileTest::TearDownTestCase(void)
{
}

void RandomAccessFileTest::SetUp()
{
}

void RandomAccessFileTest::TearDown()
{
}

/**
 * @tc.name: Test ReadFileFullyFromOffset function
 * @tc.desc: The static function will return each reading result;
 * @tc.type: FUNC
 */
HWTEST_F(RandomAccessFileTest, ReadFileFullyFromOffsetTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. use null buffer to ReadFileFullyFromOffset .
     * @tc.expected: step1. the return will be DEST_BUFFER_IS_NULL.
     */
    std::string filePath = "./test_hapverify.zip";
    SignatureInfo si0;
    int sumLen = CreatTestZipFile(filePath, si0);
    RandomAccessFile hapTestFile1;
    bool initRet = hapTestFile1.Init(filePath);
    ASSERT_TRUE(initRet);
    ASSERT_TRUE(hapTestFile1.GetLength() == sumLen);
    ReadFileErrorCode targetCode = DEST_BUFFER_IS_NULL;
    long long ret = hapTestFile1.ReadFileFullyFromOffset(nullptr, 0, 0);
    ASSERT_TRUE(ret == targetCode);
    HapByteBuffer nullBuffer;
    ret = hapTestFile1.ReadFileFullyFromOffset(nullBuffer, 0);
    ASSERT_TRUE(ret == targetCode);
    /*
     * @tc.steps: step2. use a buffer to read a null file.
     * @tc.expected: step2. the return will be FILE_IS_CLOSE.
     */
    filePath = "./test_hapverify1.zip";
    RandomAccessFile nullTestFile;
    initRet = nullTestFile.Init(filePath);
    ASSERT_FALSE(initRet);
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(TEST_RANDOMREAD_LENGTH);
    ASSERT_TRUE(buffer != nullptr);
    ret = nullTestFile.ReadFileFullyFromOffset(buffer.get(), 0, TEST_RANDOMREAD_LENGTH);
    ASSERT_EQ(ret, FILE_IS_CLOSE);
    /*
     * @tc.steps: step3. use a large buffer to read a mini file.
     * @tc.expected: step3. the return will be READ_DATA_NOT_ENOUGH.
     */
    std::string testFile = "./test_hapverify.txt";
    SignatureInfo si;
    sumLen = CreatTestZipFile(testFile, si);
    RandomAccessFile hapTestFile2;
    initRet = hapTestFile2.Init(testFile);
    ASSERT_TRUE(initRet);
    ASSERT_TRUE(hapTestFile2.GetLength() == sumLen);
    ret = hapTestFile2.ReadFileFullyFromOffset(buffer.get(), 0, TEST_RANDOMREAD_LENGTH);
    ASSERT_EQ(ret, READ_OFFSET_OUT_OF_RANGE);
    HapByteBuffer hapBuffer(TEST_RANDOMREAD_LENGTH);
    ret = hapTestFile2.ReadFileFullyFromOffset(hapBuffer, 0);
    ASSERT_EQ(ret, READ_OFFSET_OUT_OF_RANGE);
    /*
     * @tc.steps: step4. use a negative offset to read a file.
     * @tc.expected: step4. the return will be READ_OFFSET_OUT_OF_RANGE.
     */
    ret = hapTestFile2.ReadFileFullyFromOffset(hapBuffer, -1);
    ASSERT_TRUE(ret == READ_OFFSET_OUT_OF_RANGE);
    ret = hapTestFile2.ReadFileFullyFromOffset(buffer.get(), -1, TEST_RANDOMREAD_LENGTH);
    ASSERT_TRUE(ret == READ_OFFSET_OUT_OF_RANGE);
    buffer.reset(nullptr);
}
}
