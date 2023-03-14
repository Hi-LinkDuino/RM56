/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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
#include "common/any.h"
#define private public
#define protected public
#define UNIT_TEST 1

#include "plugin/plugins/ffmpeg_adapter/utils/bit_reader.h"

namespace OHOS {
namespace Media {
namespace Test {
using namespace Plugin;

class TestBitReader : public ::testing::Test {
public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
    Ffmpeg::BitReader bitReader;
};

TEST_F(TestBitReader, test_read_bits)
{
    const uint8_t data[] = {0xff, 0x11, 0x22, 0x33};
    bitReader.Reset(data, sizeof(data));
    uint32_t val = 0;
    EXPECT_EQ(bitReader.ReadBits(4, val), true);
    EXPECT_EQ(val, 0xf);

    EXPECT_EQ(bitReader.ReadBits(8, val), true);
    EXPECT_EQ(val, 0xf1);

    EXPECT_EQ(bitReader.GetAvailableBits(), 20);
    EXPECT_EQ(bitReader.ReadBits(20, val), true);
    EXPECT_EQ(val, 0x12233);

    EXPECT_EQ(bitReader.ReadBits(4, val), false);
}

TEST_F(TestBitReader, test_skip_bits)
{
    const uint8_t data[] = {0xff, 0x11, 0x22, 0x33};
    bitReader.Reset(data, sizeof(data));
    uint32_t val = 0;
    EXPECT_EQ(bitReader.ReadBits(4, val), true);
    EXPECT_EQ(val, 0xf);
    bitReader.SkipBits(4);
    EXPECT_EQ(bitReader.ReadBits(8, val), true);
    EXPECT_EQ(val, 0x11);
    bitReader.SkipBits(8);
    EXPECT_EQ(bitReader.ReadBits(8, val), true);
    EXPECT_EQ(val, 0x33);
}

TEST_F(TestBitReader, test_show_bits)
{
    const uint8_t data[] = {0xff, 0x11, 0x22, 0x33};
    bitReader.Reset(data, sizeof(data));
    uint32_t val = 0;
    EXPECT_EQ(bitReader.ReadBits(4, val), true);
    EXPECT_EQ(val, 0xf);
    EXPECT_EQ(bitReader.PeekBits(4, val), true);
    EXPECT_EQ(val, 0xf);
    bitReader.SkipBits(4);
    EXPECT_EQ(bitReader.PeekBits(8, val), true);
    EXPECT_EQ(val, 0x11);
    EXPECT_EQ(bitReader.ReadBits(8, val), true);
    EXPECT_EQ(val, 0x11);
}

TEST_F(TestBitReader, test_seek_bits)
{
    const uint8_t data[] = {0xff, 0x11, 0x22, 0x33};
    bitReader.Reset(data, sizeof(data));
    uint32_t val = 0;
    EXPECT_EQ(bitReader.ReadBits(4, val), true);
    EXPECT_EQ(val, 0xf);
    bitReader.SkipBits(20);
    EXPECT_EQ(bitReader.ReadBits(8, val), true);
    EXPECT_EQ(val, 0x33);
    EXPECT_EQ(bitReader.GetAvailableBits(), 0);
    EXPECT_EQ(bitReader.SeekTo(0), true);
    EXPECT_EQ(bitReader.ReadBits(4, val), true);
    EXPECT_EQ(val, 0xf);
}
} // namespace Test
} // namespace Media
} // namespace OHOS
