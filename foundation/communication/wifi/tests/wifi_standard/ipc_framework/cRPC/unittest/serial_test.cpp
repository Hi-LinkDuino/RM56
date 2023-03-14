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

#include "serial_test.h"
#include "serial.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(SerialTest, SerialTest, TestSize.Level1)
{
    ctx = CreateContext(1024);
    WriteBegin(ctx, 0);
    WriteFunc(ctx, "SerialTest");
    WriteInt(ctx, 100);
    WriteLong(ctx, 1234567890L);
    WriteInt64(ctx, 12345678909832323LL);
    WriteDouble(ctx, 3.14159);
    WriteChar(ctx, 'a');
    WriteStr(ctx, "Hello, world");
    int count = strlen("2c:f0:xx:xx:xx:be");
    WriteUStr(ctx, (const unsigned char *)"2c:f0:xx:xx:xx:be", count);
    WriteEnd(ctx);

    ctx->oneProcess = ctx->szWrite;
    ctx->nSize = ctx->wEnd;

    EXPECT_TRUE(strncmp(ctx->oneProcess, "N\t", 2) == 0);
    ctx->nPos = 2;
    char str[1024] = {0};
    ASSERT_EQ(ReadFunc(ctx, str, 1024), 0);
    EXPECT_TRUE(strcmp(str, "SerialTest") == 0);
    int i = 0;
    ASSERT_EQ(ReadInt(ctx, &i), 0);
    EXPECT_TRUE(i == 100);
    long l = 0;
    ASSERT_EQ(ReadLong(ctx, &l), 0);
    EXPECT_TRUE(l == 1234567890L);
    int64_t t = 0;
    ASSERT_EQ(ReadInt64(ctx, &t), 0);
    EXPECT_TRUE(t == 12345678909832323LL);
    double d = 0.0;
    ASSERT_EQ(ReadDouble(ctx, &d), 0);
    EXPECT_TRUE(d - 3.14159 < 0.000001 && d - 3.14159 > -0.000001);
    char c = ' ';
    ASSERT_EQ(ReadChar(ctx, &c), 0);
    EXPECT_TRUE(c == 'a');
    ASSERT_EQ(ReadStr(ctx, str, 1024), 0);
    EXPECT_TRUE(strcmp(str, "Hello, world") == 0);
    ASSERT_EQ(ReadUStr(ctx, (unsigned char *)str, count + 1), 0);
    EXPECT_TRUE(strcmp(str, "2c:f0:xx:xx:xx:be") == 0);
    EXPECT_TRUE(strncmp(ctx->oneProcess + ctx->nPos, "$$$$$$", 6) == 0);
}
}  // namespace Wifi
}  // namespace OHOS