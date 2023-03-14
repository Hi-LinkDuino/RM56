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

#include <stdio.h>

#include "IoTest.h"
#include <securec.h>

using namespace testing::ext;

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_0100
 * @tc.name   sprintf basic function test of int with boundary value
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfIntBoundary, Function | MediumTest | Level1)
{
    char str[100] = {0};
    int ret = sprintf(str, "%d", -2147483648);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-2147483648");

    ret = sprintf(str, "%d", 2147483647);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "2147483647");

    ret = sprintf(str, "%i", -2147483648);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-2147483648");

    ret = sprintf(str, "%i", 2147483647);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "2147483647");

    ret = sprintf(str, "%u", 0);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0");

    ret = sprintf(str, "%u", 4294967295);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "4294967295");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_0200
 * @tc.name   sprintf basic function test of long int with boundary value
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfIntLdBoundary, Function | MediumTest | Level1)
{
    char str[100] = {0};
    int ret = sprintf(str, "%ld", -2147483648); // 2^31-1
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-2147483648");

    ret = sprintf(str, "%ld", 2147483647);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "2147483647");

    ret = sprintf(str, "%li", -2147483648);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-2147483648");

    ret = sprintf(str, "%li", 2147483647);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "2147483647");

    ret = sprintf(str, "%lu", 0);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0");

    ret = sprintf(str, "%lu", 4294967295);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "4294967295");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_0300
 * @tc.name   sprintf basic function test of long long int with boundary value
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfIntLldBoundary, Function | MediumTest | Level1)
{
    char str[100] = {0};
    int ret = sprintf(str, "%lld", -9223372036854775807LL); // 2^63-1
    EXPECT_EQ(ret, 20) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-9223372036854775807");

    ret = sprintf(str, "%lld", 9223372036854775807);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "9223372036854775807");

    ret = sprintf(str, "%lli", -9223372036854775807);
    EXPECT_EQ(ret, 20) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-9223372036854775807");

    ret = sprintf(str, "%lli", 9223372036854775807);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "9223372036854775807");

    ret = sprintf(str, "%llu", (unsigned long long)0);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0");

    ret = sprintf(str, "%llu", 18446744073709551615llu);
    EXPECT_EQ(ret, 20) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "18446744073709551615");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_0400
 * @tc.name   sprintf basic function test of intmax_t and uintmax_t with boundary value
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfIntMaxTBoundary, Function | MediumTest | Level1)
{
    char str[100] = {0};
    int ret = sprintf(str, "%jd", -9223372036854775807LL);
    EXPECT_EQ(ret, 20) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-9223372036854775807");

    ret = sprintf(str, "%jd", 9223372036854775807LL);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "9223372036854775807");

    ret = sprintf(str, "%jd", 0ULL);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0");

    ret = sprintf(str, "%jd", 18446744073709551615ULL);
    EXPECT_EQ(ret, 2) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-1");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_0500
 * @tc.name   sprintf basic function test of ptrdiff_t with boundary value
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfPtrdiffTBoundary, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%td", 2147483647);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "2147483647");

    ret = sprintf(str, "%td", -2147483648);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-2147483648");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_0600
 * @tc.name   sprintf basic function test of size_t with boundary value
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfSizeTBoundary, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%zd", 2147483647);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "2147483647");

    ret = sprintf(str, "%zd", -2147483648);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-2147483648");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_0700
 * @tc.name   sprintf basic function test of signed char and short int with boundary value
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfIntHhdBoundary, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%hd %hhd %hu %hhu", 32767, 127, 65535, 255);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "32767 127 65535 255");

    ret = sprintf(str, "%hd %hhd %hu %hhu", -32768, -128, 0, 0);
    EXPECT_EQ(ret, 15) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-32768 -128 0 0");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_0800
 * @tc.name   sprintf basic function test of align right
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfAlighRight, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%5d %5d", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    1    -1");

    ret = sprintf(str, "%5i %5i", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    1    -1");

    ret = sprintf(str, "%5u %5u", 1, 0);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    1     0");

    ret = sprintf(str, "% 5d % 5d", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    1    -1");

    ret = sprintf(str, "% 5i % 5i", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    1    -1");

    ret = sprintf(str, "% 5u % 5u", 1, 0);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    1     0");

    ret = sprintf(str, "%+5d %+5d", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "   +1    -1");

    ret = sprintf(str, "%+5i %+5i", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "   +1    -1");

    ret = sprintf(str, "%+5u %+5u", 1, 0);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    1     0");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_0900
 * @tc.name   sprintf basic function test of align left
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfAlighLeft, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%-5d %-5d", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1     -1   ");

    ret = sprintf(str, "%-5i %-5i", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1     -1   ");

    ret = sprintf(str, "%-5u %-5u", 1, 0);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1     0    ");

    ret = sprintf(str, "%0-5d %0-5d", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1     -1   ");

    ret = sprintf(str, "%0-5i %0-5i", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1     -1   ");

    ret = sprintf(str, "%0-5u %0-5u", 1, 0);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1     0    ");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_1000
 * @tc.name   sprintf basic function test of other format
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfOtherFormat, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%.5d %.5d", 1, -1);
    EXPECT_EQ(ret, 12) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "00001 -00001");

    ret = sprintf(str, "%.5i %.5i", 1, -1);
    EXPECT_EQ(ret, 12) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "00001 -00001");

    ret = sprintf(str, "%.5u %.5u", 1, 0);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "00001 00000");

    ret = sprintf(str, "%#5d %#5d", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    1    -1");

    ret = sprintf(str, "%#5i %#5i", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    1    -1");

    ret = sprintf(str, "%8.5d %8.5d", 1, -1);
    EXPECT_EQ(ret, 17) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "   00001   -00001");

    ret = sprintf(str, "%#05d %#05d", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "00001 -0001");

    ret = sprintf(str, "%#05i %#05i", 1, -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "00001 -0001");

    ret = sprintf(str, "%8.5i %8.5i", 1, -1);
    EXPECT_EQ(ret, 17) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "   00001   -00001");

    ret = sprintf(str, "%#5u %#5u", 1, 0);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    1     0");

    ret = sprintf(str, "%#05u %#05u", 1, 0);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "00001 00000");

    ret = sprintf(str, "%8.5u %8.5u", 1, 0);
    EXPECT_EQ(ret, 17) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "   00001    00000");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_1100
 * @tc.name   sprintf basic function test of octal
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfOctal, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%o", 4294967295);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "37777777777");

    ret = sprintf(str, "%o", 0);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0");

    ret = sprintf(str, "%o", -1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "37777777777");

    ret = sprintf(str, "%lo", 4294967295);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "37777777777");

    ret = sprintf(str, "%lo", 0);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0");

    ret = sprintf(str, "%*o", 10, 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    177777");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_1200
 * @tc.name   sprintf basic function test of octal with width
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfOctalWidth, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%10o", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    177777");

    ret = sprintf(str, "% 10o", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    177777");

    ret = sprintf(str, "%010o", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0000177777");

    ret = sprintf(str, "%-10o", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "177777    ");

    ret = sprintf(str, "%+10o", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    177777");

    ret = sprintf(str, "%0-10o", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "177777    ");

    ret = sprintf(str, "%.10o", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0000177777");

    ret = sprintf(str, "%#10o", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "   0177777");

    ret = sprintf(str, "%#010o", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0000177777");

    ret = sprintf(str, "%10.8o", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "  00177777");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_1300
 * @tc.name   sprintf basic function test of lowercase hex with boundary value
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfLowercaseHex, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%x", 4294967295);
    EXPECT_EQ(ret, 8) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "ffffffff");

    ret = sprintf(str, "%x", 0);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0");

    ret = sprintf(str, "%x", -1);
    EXPECT_EQ(ret, 8) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "ffffffff");

    ret = sprintf(str, "%lx", 4294967295);
    EXPECT_EQ(ret, 8) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "ffffffff");

    ret = sprintf(str, "%lx", 0);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0");

    ret = sprintf(str, "%*x", 10, 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "      ffff");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_1400
 * @tc.name   sprintf basic function test of lowercase hex with width
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfLowercaseHexWidth, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%10x", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "      ffff");

    ret = sprintf(str, "% 10x", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "      ffff");

    ret = sprintf(str, "%010x", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "000000ffff");

    ret = sprintf(str, "%-10x", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "ffff      ");

    ret = sprintf(str, "%+10x", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "      ffff");

    ret = sprintf(str, "%0-10x", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "ffff      ");

    ret = sprintf(str, "%.10x", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "000000ffff");

    ret = sprintf(str, "%#10x", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    0xffff");

    ret = sprintf(str, "%#010x", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0x0000ffff");

    ret = sprintf(str, "%10.8x", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "  0000ffff");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_1500
 * @tc.name   sprintf basic function test of uppercase hex with boundary value
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfUppercaseHex, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%X", 4294967295);
    EXPECT_EQ(ret, 8) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "FFFFFFFF");

    ret = sprintf(str, "%X", 0);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0");

    ret = sprintf(str, "%X", -1);
    EXPECT_EQ(ret, 8) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "FFFFFFFF");

    ret = sprintf(str, "%lX", 4294967295);
    EXPECT_EQ(ret, 8) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "FFFFFFFF");

    ret = sprintf(str, "%lX", 0);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0");

    ret = sprintf(str, "%*X", 10, 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "      FFFF");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_1600
 * @tc.name   sprintf basic function test of uppercase hex with width
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfUppercaseHexWidth, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%10X", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "      FFFF");

    ret = sprintf(str, "% 10X", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "      FFFF");

    ret = sprintf(str, "%010X", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "000000FFFF");

    ret = sprintf(str, "%*X", 10, 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "      FFFF");

    ret = sprintf(str, "%-10X", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "FFFF      ");

    ret = sprintf(str, "%+10X", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "      FFFF");

    ret = sprintf(str, "%0-10X", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "FFFF      ");

    ret = sprintf(str, "%.10X", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "000000FFFF");

    ret = sprintf(str, "%#10X", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    0XFFFF");

    ret = sprintf(str, "%#010X", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0X0000FFFF");

    ret = sprintf(str, "%10.8X", 65535);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "  0000FFFF");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_1700
 * @tc.name   sprintf basic function test of float
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfFloat, Function | MediumTest | Level1)
{
    char str[100] = {0};
    int ret = sprintf(str, "%f", -1.1);
    EXPECT_EQ(ret, 9) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-1.100000");

    ret = sprintf(str, "%.3f", -.123456);
    EXPECT_EQ(ret, 6) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-0.123");

    ret = sprintf(str, "%Lf", 1.1);
    EXPECT_EQ(ret, 8) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.100000");

    ret = sprintf(str, "%5.1f %5.1f", 6., -6.);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "  6.0  -6.0");

    ret = sprintf(str, "%05.1f %05.1f", 6., -6.);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "006.0 -06.0");

    ret = sprintf(str, "%+05.1f %+05.1f", 6., -6.);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+06.0 -06.0");

    ret = sprintf(str, "%.3f %.5f", 1.6666, -1.1);
    EXPECT_EQ(ret, 14) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.667 -1.10000");

    ret = sprintf(str, "%.0f %.0f", 1.1, 1.6);
    EXPECT_EQ(ret, 3) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1 2");

    ret = sprintf(str, "%+3.2f %+3.2f", 1.1, -1.1);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+1.10 -1.10");

    ret = sprintf(str, "hello %+3.2f world", 1.1);
    EXPECT_EQ(ret, 17) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello +1.10 world");

    ret = sprintf(str, "%.*f", 3, 1.6666666666);
    EXPECT_EQ(ret, 5) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.667");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_1800
 * @tc.name   sprintf basic function test of float with format a
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfLowercaseA, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%a", 0.00457878e+16);
    EXPECT_EQ(ret, 17) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0x1.4d266c6ebp+45");

    ret = sprintf(str, "%La", 0.00457878e+16);
    EXPECT_EQ(ret, 17) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0x1.4d266c6ebp+45");

    ret = sprintf(str, "%.*a", 3, 1.6666666666);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0x1.aabp+0");

    ret = sprintf(str, "%.3a %.5a", 1.6666, -1.1);
    EXPECT_EQ(ret, 24) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0x1.aaap+0 -0x1.1999ap+0");

    ret = sprintf(str, "%5.1a %10.1a", 6., -6.);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0x1.8p+2  -0x1.8p+2");

    ret = sprintf(str, "%05.1a %010.1a", 6., -6.);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0x1.8p+2 -0x01.8p+2");

    ret = sprintf(str, "%+05.1a %+010.1a", 6., -6.);
    EXPECT_EQ(ret, 20) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+0x1.8p+2 -0x01.8p+2");

    ret = sprintf(str, "%.0a %.0a", 1.1, 1.6);
    EXPECT_EQ(ret, 13) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0x1p+0 0x2p+0");

    ret = sprintf(str, "%+3.2a %+3.2a", 1.1, -1.1);
    EXPECT_EQ(ret, 21) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+0x1.1ap+0 -0x1.1ap+0");

    ret = sprintf(str, "hello %+3.2a world", 1.1);
    EXPECT_EQ(ret, 22) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello +0x1.1ap+0 world");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_1900
 * @tc.name   sprintf basic function test of float with format A
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfUppercaseA, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%A", -0.00457878e+16);
    EXPECT_EQ(ret, 18) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-0X1.4D266C6EBP+45");

    ret = sprintf(str, "%LA", -0.00457878e+16);
    EXPECT_EQ(ret, 18) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-0X1.4D266C6EBP+45");

    ret = sprintf(str, "%.*A", 3, 1.6666666666);
    EXPECT_EQ(ret, 10) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0X1.AABP+0");

    ret = sprintf(str, "%.3A %.5A", 1.6666, -1.1);
    EXPECT_EQ(ret, 24) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0X1.AAAP+0 -0X1.1999AP+0");

    ret = sprintf(str, "%5.1A %10.1A", 6., -6.);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0X1.8P+2  -0X1.8P+2");

    ret = sprintf(str, "%05.1A %010.1A", 6., -6.);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0X1.8P+2 -0X01.8P+2");

    ret = sprintf(str, "%+05.1A %+010.1A", 6., -6.);
    EXPECT_EQ(ret, 20) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+0X1.8P+2 -0X01.8P+2");

    ret = sprintf(str, "%.0A %.0A", 1.1, 1.6);
    EXPECT_EQ(ret, 13) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0X1P+0 0X2P+0");

    ret = sprintf(str, "%+3.2A %+3.2A", 1.1, -1.1);
    EXPECT_EQ(ret, 21) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+0X1.1AP+0 -0X1.1AP+0");

    ret = sprintf(str, "hello %+3.2A world", 1.1);
    EXPECT_EQ(ret, 22) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello +0X1.1AP+0 world");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_2000
 * @tc.name   sprintf basic function test of float with format E,lowercase
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfLowercaseE, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%e", 0.00457878e+16);
    EXPECT_EQ(ret, 12) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "4.578780e+13");

    ret = sprintf(str, "%Le", 0.00457878e+16);
    EXPECT_EQ(ret, 12) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "4.578780e+13");

    ret = sprintf(str, "%.*e", 3, 1.6666666666);
    EXPECT_EQ(ret, 9) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.667e+00");

    ret = sprintf(str, "%5.1e %10.1e", 6., -6.);
    EXPECT_EQ(ret, 18) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "6.0e+00   -6.0e+00");

    ret = sprintf(str, "%05.1e %010.1e", 6., -6.);
    EXPECT_EQ(ret, 18) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "6.0e+00 -006.0e+00");

    ret = sprintf(str, "%+05.1e %+010.1e", 6., -6.);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+6.0e+00 -006.0e+00");

    ret = sprintf(str, "%.3e %.5e", 1.6666, -1.1);
    EXPECT_EQ(ret, 22) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.667e+00 -1.10000e+00");

    ret = sprintf(str, "%.0e %.0e", 1.1, 1.6);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1e+00 2e+00");

    ret = sprintf(str, "%+3.2e %+3.2e", 1.1, -1.1);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+1.10e+00 -1.10e+00");

    ret = sprintf(str, "hello %+3.2e world", 1.1);
    EXPECT_EQ(ret, 21) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello +1.10e+00 world");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_2100
 * @tc.name   sprintf basic function test of float with format E,uppercase
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfUppercaseE, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%E", -0.00457878e+16);
    EXPECT_EQ(ret, 13) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-4.578780E+13");

    ret = sprintf(str, "%LE", -0.00457878e+16);
    EXPECT_EQ(ret, 13) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "-4.578780E+13");

    ret = sprintf(str, "%.*E", 3, 1.6666666666);
    EXPECT_EQ(ret, 9) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.667E+00");

    ret = sprintf(str, "%5.1E %10.1E", 6., -6.);
    EXPECT_EQ(ret, 18) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "6.0E+00   -6.0E+00");

    ret = sprintf(str, "%05.1E %010.1E", 6., -6.);
    EXPECT_EQ(ret, 18) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "6.0E+00 -006.0E+00");

    ret = sprintf(str, "%+05.1E %+010.1E", 6., -6.);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+6.0E+00 -006.0E+00");

    ret = sprintf(str, "%.3E %.5E", 1.6666, -1.1);
    EXPECT_EQ(ret, 22) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.667E+00 -1.10000E+00");

    ret = sprintf(str, "%.0E %.0E", 1.1, 1.6);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1E+00 2E+00");

    ret = sprintf(str, "%+3.2E %+3.2E", 1.1, -1.1);
    EXPECT_EQ(ret, 19) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+1.10E+00 -1.10E+00");

    ret = sprintf(str, "hello %+3.2E world", 1.1);
    EXPECT_EQ(ret, 21) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello +1.10E+00 world");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_2200
 * @tc.name   sprintf basic function test of float with format G,lowercase
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfLowercaseG, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%g", 0.00457878e+16);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "4.57878e+13");

    ret = sprintf(str, "%Lg", 0.00457878e+16);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "4.57878e+13");

    ret = sprintf(str, "%.*g", 3, 1.6666666666);
    EXPECT_EQ(ret, 4) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.67");

    ret = sprintf(str, "%5.1g %10.1g", 6., -6.);
    EXPECT_EQ(ret, 16) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    6         -6");

    ret = sprintf(str, "%05.1g %010.1g", 6., -6.);
    EXPECT_EQ(ret, 16) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "00006 -000000006");

    ret = sprintf(str, "%+05.1g %+010.1g", 6., -6.);
    EXPECT_EQ(ret, 16) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+0006 -000000006");

    ret = sprintf(str, "%.3g %.5g", 1.6666, -1.1);
    EXPECT_EQ(ret, 9) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.67 -1.1");

    ret = sprintf(str, "%.0g %.0g", 1.1, 1.6);
    EXPECT_EQ(ret, 3) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1 2");

    ret = sprintf(str, "%+3.2g %+3.2g", 1.1, -1.1);
    EXPECT_EQ(ret, 9) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+1.1 -1.1");

    ret = sprintf(str, "hello %+3.2g world", 1.1);
    EXPECT_EQ(ret, 16) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello +1.1 world");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_2300
 * @tc.name   sprintf basic function test of float with format G,uppercase
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfUppercaseG, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sprintf(str, "%G", 0.00457878e+16);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "4.57878E+13");

    ret = sprintf(str, "%LG", 0.00457878e+16);
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "4.57878E+13");

    ret = sprintf(str, "%.*G", 3, 1.6666666666);
    EXPECT_EQ(ret, 4) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.67");

    ret = sprintf(str, "%5.1G %10.1G", 6., -6.);
    EXPECT_EQ(ret, 16) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    6         -6");

    ret = sprintf(str, "%05.1G %010.1G", 6., -6.);
    EXPECT_EQ(ret, 16) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "00006 -000000006");

    ret = sprintf(str, "%+05.1G %+010.1G", 6., -6.);
    EXPECT_EQ(ret, 16) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+0006 -000000006");

    ret = sprintf(str, "%.3G %.5G", 1.6666, -1.1);
    EXPECT_EQ(ret, 9) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1.67 -1.1");

    ret = sprintf(str, "%.0G %.0G", 1.1, 1.6);
    EXPECT_EQ(ret, 3) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "1 2");

    ret = sprintf(str, "%+3.2G %+3.2G", 1.1, -1.1);
    EXPECT_EQ(ret, 9) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "+1.1 -1.1");

    ret = sprintf(str, "hello %+3.2G world", 1.1);
    EXPECT_EQ(ret, 16) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello +1.1 world");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_2400
 * @tc.name   sprintf basic function test of string
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfString, Function | MediumTest | Level1)
{
    char str[100] = {0};
    int ret = sprintf(str, "%s", "hello world");
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello world");

    ret = sprintf(str, "%7s", "hello");
    EXPECT_EQ(ret, 7) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "  hello");

    ret = sprintf(str, "%-7s", "hello");
    EXPECT_EQ(ret, 7) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello  ");

    ret = sprintf(str, "%0-7s", "hello");
    EXPECT_EQ(ret, 7) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello  ");

    ret = sprintf(str, "%.6s", "hello");
    EXPECT_EQ(ret, 5) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello");

    ret = sprintf(str, "%.6s", "");
    EXPECT_EQ(ret, 0) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "");

    ret = sprintf(str, "%.3s", "hello");
    EXPECT_EQ(ret, 3) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hel");

    ret = sprintf(str, "%.3s%.6s", "abcd", "hello");
    EXPECT_EQ(ret, 8) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "abchello");

    ret = sprintf(str, "%.3s%.6s", "abcd", "hello");
    EXPECT_EQ(ret, 8) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "abchello");

    ret = sprintf(str, "%6.3s", "hello world");
    EXPECT_EQ(ret, 6) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "   hel");

    ret = sprintf(str, "%*s", 5, "hello world");
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello world");

    ret = sprintf(str, "%.*s", 5, "hello world");
    EXPECT_EQ(ret, 5) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello");

    ret = sprintf(str, "123 %s 000", "hello");
    EXPECT_EQ(ret, 13) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "123 hello 000");

    ret = sprintf(str, "th%s(a)|%s|", "is", "string");
    EXPECT_EQ(ret, 15) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "this(a)|string|");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_2500
 * @tc.name   sprintf basic function test of char
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfChar, Function | MediumTest | Level1)
{
    char str[100] = {0};
    int ret = sprintf(str, "%c", 97);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "a");

    ret = sprintf(str, "%5c", 97);
    EXPECT_EQ(ret, 5) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "    a");

    ret = sprintf(str, "%-5c", 97);
    EXPECT_EQ(ret, 5) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "a    ");

    ret = sprintf(str, "%0-5c", 97);
    EXPECT_EQ(ret, 5) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "a    ");

    ret = sprintf(str, "hello %c world", '!');
    EXPECT_EQ(ret, 13) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello ! world");

    ret = sprintf(str, "(%c) and |%.5c|", 'a', 'b');
    EXPECT_EQ(ret, 11) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "(a) and |b|");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_2600
 * @tc.name   sprintf basic function test of pointer
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfPointer, Function | MediumTest | Level1)
{
    char str[100] = {0};
    int ret = sprintf(str, "%p", 97);
    EXPECT_EQ(ret, 4) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0x61");

    ret = sprintf(str, "%p * %p", 255, 65535);
    EXPECT_EQ(ret, 13) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0xff * 0xffff");

    ret = sprintf(str, "%p", 0xfffff);
    EXPECT_EQ(ret, 7) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "0xfffff");
}

 /**
 * @tc.number SUB_KERNEL_IO_SPRINTF_2700
 * @tc.name   sprintf basic function test of percent format
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSprintfPercent, Function | MediumTest | Level1)
{
    char str[100] = {0};
    int ret = sprintf(str, "%%", 97);
    EXPECT_EQ(ret, 1) << "> sprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "%");
}