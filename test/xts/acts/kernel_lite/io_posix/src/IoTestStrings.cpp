/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <strings.h>

#include "IoTest.h"

using namespace testing::ext;

/**
 * @tc.number SUB_KERNEL_IO_STRINGS_0100
 * @tc.name   ffsl basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFfsl, Function | MediumTest | Level1)
{
    int ret;
    for (int i = 0; i < 32; i++) {
        ret = ffsl(1 << i);
        EXPECT_EQ(ret, i + 1);
    }

    ret = ffsl(26);
    EXPECT_EQ(ret, 2);

    ret = ffsl(0);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number SUB_KERNEL_IO_STRINGS_0200
 * @tc.name   strncasecmp basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrncasecmp, Function | MediumTest | Level1)
{
    int ret = strncasecmp("abcdefg", "abcdEFg", 7);
    EXPECT_EQ(ret, 0);

    ret = strncasecmp("abcdefg", "abcdEF", 7);
    EXPECT_GT(ret, 0);

    ret = strncasecmp("abcdef", "abcdEFg", 7);
    EXPECT_LT(ret, 0);
}

/**
 * @tc.number SUB_KERNEL_IO_STRINGS_0300
 * @tc.name   strcasecmp basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrcasecmp, Function | MediumTest | Level1)
{
    int ret = strcasecmp("abcdefg", "abcdEFg");
    EXPECT_EQ(ret, 0);

    ret = strcasecmp("abcdefg", "abcdEF");
    EXPECT_GT(ret, 0);

    ret = strcasecmp("abcdef", "abcdEFg");
    EXPECT_LT(ret, 0);
}

/**
 * @tc.number SUB_KERNEL_IO_STRINGS_0400
 * @tc.name   index basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testIndex, Function | MediumTest | Level1)
{
    char src[] = "abcdef123456abcdef";
    char *ret = index(src, '1');
    EXPECT_STREQ(ret, "123456abcdef");

    ret = index(src, 'f');
    EXPECT_STREQ(ret, "f123456abcdef");

    ret = index(src, ' ');
    EXPECT_STREQ(ret, nullptr);

    char srcFir[50] = {0};
    ret = index(srcFir, '?');
    EXPECT_STREQ(ret, nullptr);
}

/**
 * @tc.number SUB_KERNEL_IO_STRINGS_0500
 * @tc.name   rindex basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testRindex, Function | MediumTest | Level1)
{
    char src[] = "abcdef123456abcdef";
    char *ret = rindex(src, '1');
    EXPECT_STREQ(ret, "123456abcdef");

    ret = rindex(src, 'f');
    EXPECT_STREQ(ret, "f");

    ret = rindex(src, ' ');
    EXPECT_STREQ(ret, nullptr);

    char srcFir[50] = {0};
    ret = rindex(srcFir, '?');
    EXPECT_STREQ(ret, nullptr);
}