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

#include "hap_byte_buffer_test.h"

#include <gtest/gtest.h>

#include "common/hap_byte_buffer.h"
#include "securec.h"

#include "test_const.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;
namespace {
class HapByteBufferTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapByteBufferTest::SetUpTestCase(void)
{
}

void HapByteBufferTest::TearDownTestCase(void)
{
}

void HapByteBufferTest::SetUp()
{
}

void HapByteBufferTest::TearDown()
{
}

/**
 * @tc.name: Test HapByteBuffer Constructor and overload function.
 * @tc.desc: The static function will return an object of HapByteBuffer;
 * @tc.type: FUNC
 */
HWTEST_F (HapByteBufferTest, HapByteBuffer001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Run HapByteBuffer(buffer1).
     * @tc.expected: step1. The returned object is the same as buffer1.
     */
    HapByteBuffer buffer1;
    HapByteBuffer buffer2(buffer1);
    bool judge = (buffer2.GetCapacity() == buffer1.GetCapacity()) &&
        (buffer2.GetPosition() == buffer1.GetPosition()) &&
        (buffer2.GetLimit() == buffer1.GetLimit()) && (buffer2.GetBufferPtr() == nullptr);
    ASSERT_TRUE(judge);
    /*
     * @tc.steps: step1. Run overloaded function =.
     * @tc.expected: step1. The left object is the same as right object.
     */
    HapByteBuffer buffer3(TEST_HAPBYTEBUFFER_LENGTH);
    buffer3.PutInt32(0, 0);
    HapByteBuffer buffer4(TEST_HAPBYTEBUFFER_LENGTH);
    buffer4.PutInt32(0, TEST_HAPBYTEBUFFER_INT32_DATA);
    buffer3 = buffer4;
    ASSERT_TRUE(buffer3.IsEqual(buffer4));
}

/**
 * @tc.name: Test a HapByteBuffer object's operation of GetInt and Put
 * @tc.desc: The static function will return data from HapByteBuffer's buffer
 * @tc.type: FUNC
 */
HWTEST_F (HapByteBufferTest, GetIntAndPutOperation001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Create an empty buffer and get data from it.
     * @tc.expected: step1. The return result is false.
     */
    HapByteBuffer emptyBuffer;
    int dataInt32;
    ASSERT_FALSE(emptyBuffer.GetInt32(dataInt32));
    long long dataInt64;
    ASSERT_FALSE(emptyBuffer.GetInt64(dataInt64));
    unsigned short dataUInt16;
    ASSERT_FALSE(emptyBuffer.GetUInt16(0, dataUInt16));
    /*
     * @tc.steps: step2. Create a HapByteBuffer with one byte's buffer and get data from second byte.
     * @tc.expected: step2. The return result is false.
     */
    HapByteBuffer testBuffer(1);
    char testChar = TEST_HAPBYTEBUFFER_CHAR_DATA;
    testBuffer.PutData(0, &testChar, sizeof(testChar));
    unsigned int dataUInt32;
    ASSERT_FALSE(testBuffer.GetUInt32(1, dataUInt32));
    ASSERT_FALSE(testBuffer.GetInt32(1, dataInt32));
    ASSERT_FALSE(testBuffer.GetInt64(1, dataInt64));
    ASSERT_FALSE(testBuffer.GetUInt16(1, dataUInt16));
    /*
     * @tc.steps: step3. Get data from negative position.
     * @tc.expected: step3. The return result is false.
     */
    ASSERT_FALSE(testBuffer.GetInt64(TEST_HAPBYTEBUFFER_INVALID_INDEX, dataInt64));
    /*
     * @tc.steps: step4. Put data to buffer and get data from it.
     * @tc.expected: step4. The return data is same as which we put.
     */
    HapByteBuffer testBuffer2(TEST_HAPBYTEBUFFER_LENGTH);
    testBuffer2.PutByte(0, testChar);
    char testUInt16[TEST_HAPBYTEBUFFER_UINT16_LENGTH];
    errno_t err;
    err = memcpy_s(testUInt16, sizeof(testUInt16), &TEST_HAPBYTEBUFFER_UINT16_DATA,
        sizeof(TEST_HAPBYTEBUFFER_UINT16_DATA));
    ASSERT_EQ(err, EOK);
    testBuffer2.PutData(sizeof(char), testUInt16, sizeof(testUInt16));
    int testInt32 = TEST_HAPBYTEBUFFER_INT32_DATA;
    testBuffer2.PutInt32(sizeof(char) + sizeof(unsigned short), testInt32);
    char testInt64[TEST_HAPBYTEBUFFER_INT64_LENGTH];
    err = memcpy_s(testInt64, sizeof(testInt64), &TEST_HAPBYTEBUFFER_INT64_DATA,
        sizeof(TEST_HAPBYTEBUFFER_INT64_DATA));
    ASSERT_EQ(err, EOK);
    testBuffer2.PutData(TEST_HAPBYTEBUFFER_LENGTH - sizeof(testInt64), testInt64, sizeof(testInt64));
    ASSERT_TRUE(testBuffer2.GetUInt32(sizeof(char) + sizeof(unsigned short), dataUInt32));
    ASSERT_EQ(dataUInt32, TEST_HAPBYTEBUFFER_UINT32_DATA);
    ASSERT_TRUE(testBuffer2.GetInt32(sizeof(char) + sizeof(unsigned short), dataInt32));
    ASSERT_EQ(dataInt32, TEST_HAPBYTEBUFFER_INT32_DATA);
    ASSERT_TRUE(testBuffer2.GetInt64(TEST_HAPBYTEBUFFER_LENGTH - sizeof(testInt64), dataInt64));
    ASSERT_EQ(dataInt64, TEST_HAPBYTEBUFFER_INT64_DATA);
    ASSERT_TRUE(testBuffer2.GetUInt16(sizeof(char), dataUInt16));
    ASSERT_EQ(dataUInt16, TEST_HAPBYTEBUFFER_UINT16_DATA);
}

/**
 * @tc.name: Test HapByteBuffer function of slice
 * @tc.desc: The static function will get an object after slice and detect it is right;
 * @tc.type: FUNC
 */
HWTEST_F (HapByteBufferTest, Slice001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Set a fixed length buffer.
     * @tc.expected: step1. The return is same as value is set.
     */
    HapByteBuffer buffer1(TEST_HAPBYTEBUFFER_LENGTH_2);
    buffer1.SetCapacity(TEST_HAPBYTEBUFFER_LENGTH);
    ASSERT_TRUE(buffer1.GetCapacity() == TEST_HAPBYTEBUFFER_LENGTH);
    /*
     * @tc.steps: step2. Slice buffer.
     * @tc.expected: step2. The return is the target length after slice.
     */
    buffer1.PutInt32(0, TEST_HAPBYTEBUFFER_INT32_DATA);
    buffer1.PutInt32(sizeof(int), TEST_HAPBYTEBUFFER_INT32_DATA_2);
    buffer1.SetPosition(sizeof(int));
    buffer1.SetLimit(sizeof(int) + sizeof(int));
    buffer1.Slice();
    ASSERT_TRUE(buffer1.Remaining() == sizeof(int));
    /*
     * @tc.steps: step3. Get int32 from buffer1.
     * @tc.expected: step3. The return result is equal to TEST_HAPBYTEBUFFER_INT32_DATA_2.
     */
    int testDataInt32;
    ASSERT_TRUE(buffer1.GetInt32(testDataInt32));
    ASSERT_EQ(testDataInt32, TEST_HAPBYTEBUFFER_INT32_DATA_2);
    /*
     * @tc.steps: step4. Slice continue, reset position and limit, and calculate if buffer has remain.
     * @tc.expected: step4. The return result is true.
     */
    buffer1.Slice();
    buffer1.Clear();
    ASSERT_TRUE(buffer1.HasRemaining());
}

/**
 * @tc.name: Test HapByteBuffer function of CopyPartialBuffer
 * @tc.desc: The static function will copy data from an object, detect the data is right;
 * @tc.type: FUNC
 */
HWTEST_F (HapByteBufferTest, CopyPartialBuffer001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Copy 8 bytes data from 10st-position in a 15 bytes length buffer.
     * @tc.expected: step1. The return result is false.
     */
    HapByteBuffer buffer1(TEST_HAPBYTEBUFFER_LENGTH);
    buffer1.PutInt32(0, TEST_HAPBYTEBUFFER_INT32_DATA);
    buffer1.PutInt32(sizeof(int), TEST_HAPBYTEBUFFER_INT32_DATA_2);
    HapByteBuffer buffer2(TEST_HAPBYTEBUFFER_LENGTH);
    buffer1.SetPosition(TEST_HAPBYTEBUFFER_POSITION);
    ASSERT_FALSE(buffer2.CopyPartialBuffer(buffer1, TEST_HAPBYTEBUFFER_LENGTH_2));
    /*
     * @tc.steps: step2. Copy 8 bytes data from first-position in a 15 bytes length buffer.
     * @tc.expected: step2. Buffer2 return is targeted value.
     */
    buffer1.Clear();
    buffer2.CopyPartialBuffer(buffer1, TEST_HAPBYTEBUFFER_LENGTH_2);
    int target1;
    ASSERT_TRUE(buffer2.GetInt32(target1));
    ASSERT_EQ(target1, TEST_HAPBYTEBUFFER_INT32_DATA);
    int target2;
    ASSERT_TRUE(buffer2.GetInt32(target2));
    ASSERT_EQ(target2, TEST_HAPBYTEBUFFER_INT32_DATA_2);
}

/**
 * @tc.name: Test HapByteBuffer function of IsEqual001
 * @tc.desc: The static function will return two object whether is equal.
 * @tc.type: FUNC
 */
HWTEST_F (HapByteBufferTest, IsEqual001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Create a buffer, and compare it with itself.
     * @tc.expected: step1. The return result is true.
     */
    char testChar[TEST_HAPBYTEBUFFER_LENGTH] = "Hello, world!!";
    HapByteBuffer buffer1(TEST_HAPBYTEBUFFER_LENGTH);
    buffer1.PutData(0, testChar, TEST_HAPBYTEBUFFER_LENGTH);
    ASSERT_TRUE(buffer1.IsEqual(buffer1));
    /*
     * @tc.steps: step2. Create another buffer and compare it with buffer1.
     * @tc.expected: step2. The return result is false.
     */
    HapByteBuffer buffer2;
    ASSERT_FALSE(buffer1.IsEqual(buffer2));
    /*
     * @tc.steps: step3. Set length of buffer2 same as buffer1.
     * @tc.expected: step3. The return result is false.
     */
    buffer2.SetCapacity(TEST_HAPBYTEBUFFER_LENGTH);
    ASSERT_FALSE(buffer1.IsEqual(buffer2));
    /*
     * @tc.steps: step4. Use copy constructor to create an buffer3, and compare it with buffer1.
     * @tc.expected: step4. The return result is true.
     */
    HapByteBuffer buffer3(buffer1);
    ASSERT_TRUE(buffer1.IsEqual(buffer3));
}

/**
 * @tc.name: Test HapByteBuffer function of IsEqual002
 * @tc.desc: The static function will return whether the value in buffer is equal to a string.
 * @tc.type: FUNC
 */
HWTEST_F (HapByteBufferTest, IsEqual002, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Create a buffer and string, and compare.
     * @tc.expected: step1. The return is false.
     */
    std::string testStr = "Hello, world!!!";
    HapByteBuffer buffer1;
    const HapByteBuffer& buffer2 = buffer1;
    buffer1 = buffer2;
    ASSERT_FALSE(buffer1.IsEqual(testStr));
    /*
     * @tc.steps: step2. Set length of buffer1 same as string.
     * @tc.expected: step2. The return is false.
     */
    buffer1.SetCapacity(static_cast<int>(testStr.size()));
    ASSERT_FALSE(buffer1.IsEqual(testStr));
    /*
     * @tc.steps: step3. Put string to buffer1 and compare.
     * @tc.expected: step3. The return is true.
     */
    for (int i = 0; i < static_cast<int>(testStr.size()); i++) {
        buffer1.PutByte(i, testStr[i]);
    }
    ASSERT_TRUE(buffer1.IsEqual(testStr));
}
}
