/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include <cstdlib>
#include <memory>
#include <string>
#include "gtest/gtest.h"
#define private public
#define protected public
#include "filters/demux/data_packer.h"

namespace OHOS {
namespace Media {
namespace Test {
using namespace OHOS::Media::Plugin;

class TestDataPacker : public ::testing::Test {
public:
    std::shared_ptr<DataPacker> dataPacker;
    void SetUp() override
    {
        dataPacker = std::make_shared<DataPacker>();
    }

    void TearDown() override
    {
    }
};

AVBufferPtr CreateBuffer(size_t size, uint64_t offset = 0)
{
    const uint8_t* data = (uint8_t*)"1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    auto buffer = std::make_shared<AVBuffer>();
    buffer->AllocMemory(nullptr, size);
    buffer->GetMemory()->Write(data + offset, size);
    return buffer;
}

AVBufferPtr CreateEmptyBuffer(size_t size)
{
    auto buffer = std::make_shared<AVBuffer>();
    buffer->AllocMemory(nullptr, size);
    buffer->GetMemory()->Write((uint8_t*)"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", size);
    return buffer;
}

TEST_F(TestDataPacker, can_push_one_buffer_to_datapacker)
{
    auto bufferPtr = CreateBuffer(10);
    dataPacker->PushData(bufferPtr, 0);
    ASSERT_STREQ("DataPacker (offset 0, size 10, buffer count 1)", dataPacker->ToString().c_str());
}

TEST_F(TestDataPacker, can_push_two_buffers_to_datapacker)
{
    auto bufferPtr = CreateBuffer(10);
    dataPacker->PushData(bufferPtr, 0);
    auto bufferPtr2 = CreateBuffer(4, 10);
    dataPacker->PushData(bufferPtr2, 10);
    ASSERT_STREQ("DataPacker (offset 0, size 14, buffer count 2)", dataPacker->ToString().c_str());
}

TEST_F(TestDataPacker, should_return_true_if_check_range_in_datapacker)
{
    auto bufferPtr = CreateBuffer(10);
    dataPacker->PushData(bufferPtr, 0);
    uint64_t curOffset = 0;
    ASSERT_TRUE(dataPacker->IsDataAvailable(3, 2, curOffset));
    ASSERT_EQ(curOffset, 10);
}

TEST_F(TestDataPacker, should_return_false_if_check_range_not_in_datapacker)
{
    auto bufferPtr = CreateBuffer(10);
    dataPacker->PushData(bufferPtr, 0);
    uint64_t curOffset = 0;
    ASSERT_FALSE(dataPacker->IsDataAvailable(11, 2, curOffset));
}

TEST_F(TestDataPacker, should_update_cur_offset_to_get_range_offset_if_get_range_start_not_in_datapacker)
{
    auto bufferPtr = CreateBuffer(10);
    dataPacker->PushData(bufferPtr, 0);
    uint64_t curOffset = 0;
    ASSERT_FALSE(dataPacker->IsDataAvailable(11, 2, curOffset));
    ASSERT_EQ(curOffset, 11);
}

TEST_F(TestDataPacker, should_update_cur_offset_to_datapacker_offset_end_if_get_range_start_in_datapacker)
{
    auto bufferPtr = CreateBuffer(10);
    dataPacker->PushData(bufferPtr, 0);
    uint64_t curOffset = 0;
    ASSERT_FALSE(dataPacker->IsDataAvailable(8, 10, curOffset));
    ASSERT_EQ(curOffset, 10);
}

TEST_F(TestDataPacker, can_get_data_in_the_middle_of_one_buffer)
{
    auto bufferPtr = CreateBuffer(10);
    dataPacker->PushData(bufferPtr, 0);
    ASSERT_STREQ("DataPacker (offset 0, size 10, buffer count 1)", dataPacker->ToString().c_str());
    uint64_t curOffset = 0;
    ASSERT_TRUE(dataPacker->IsDataAvailable(3, 2, curOffset));
    ASSERT_EQ(curOffset, 10);
    auto bufferOut = CreateEmptyBuffer(3);
    dataPacker->GetRange(3, 2, bufferOut);
    ASSERT_STREQ("45", (const char *)(bufferOut->GetMemory()->GetReadOnlyData()));
}

TEST_F(TestDataPacker, remove_old_data_after_second_get_range)
{
    auto bufferPtr = CreateBuffer(10);
    dataPacker->PushData(bufferPtr, 0);
    ASSERT_STREQ("DataPacker (offset 0, size 10, buffer count 1)", dataPacker->ToString().c_str());
    uint64_t curOffset = 0;
    ASSERT_TRUE(dataPacker->IsDataAvailable(3, 2, curOffset));
    ASSERT_EQ(curOffset, 10);
    auto bufferOut = CreateEmptyBuffer(5);
    dataPacker->GetRange(3, 4, bufferOut);
    bufferOut->GetMemory()->UpdateDataSize(0);
    dataPacker->GetRange(5, 2, bufferOut);
    ASSERT_STREQ("DataPacker (offset 5, size 5, buffer count 1)", dataPacker->ToString().c_str());
}

TEST_F(TestDataPacker, remove_old_data_after_second_get_range_when_two_buffers_in_datapacker)
{
    auto bufferPtr = CreateBuffer(10);
    dataPacker->PushData(bufferPtr, 0);
    auto bufferPtr2 = CreateBuffer(4, 10);
    dataPacker->PushData(bufferPtr2, 10);
    ASSERT_STREQ("DataPacker (offset 0, size 14, buffer count 2)", dataPacker->ToString().c_str());

    auto bufferOut = CreateEmptyBuffer(5);
    dataPacker->GetRange(3, 4, bufferOut);
    ASSERT_STREQ("DataPacker (offset 0, size 14, buffer count 2)", dataPacker->ToString().c_str());

    bufferOut->GetMemory()->UpdateDataSize(0);
    dataPacker->GetRange(11, 2, bufferOut);
    ASSERT_STREQ("DataPacker (offset 11, size 3, buffer count 1)", dataPacker->ToString().c_str());
}
TEST_F(TestDataPacker, can_get_data_from_two_buffers)
{
    auto bufferPtr = CreateBuffer(10);
    dataPacker->PushData(bufferPtr, 0);
    uint64_t curOffset = 0;
    ASSERT_FALSE(dataPacker->IsDataAvailable(0, 15, curOffset));
    ASSERT_EQ(curOffset, 10);
    auto bufferPtr2 = CreateBuffer(10, curOffset);
    dataPacker->PushData(bufferPtr2, curOffset);
    auto bufferOut = CreateEmptyBuffer(16);
    ASSERT_TRUE(dataPacker->GetRange(0, 15, bufferOut));
    ASSERT_EQ(15, bufferOut->GetMemory()->GetSize());
    ASSERT_STREQ("1234567890abcde", (const char*)(bufferOut->GetMemory()->GetReadOnlyData()));
}
} // namespace Test
} // namespace Media
} // namespace OHOS