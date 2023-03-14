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

#include <stdlib.h>

#include <gtest/gtest.h>

#include "log.h"
#include "IoTest.h"

using namespace testing::ext;

/**
 * @tc.number SUB_KERNEL_IO_STDLIB_0100
 * @tc.name   gcvt basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testGcvt, Function | MediumTest | Level1)
{
    char str[50] = {0};
    char *ret = gcvt(999, 3, str);
    EXPECT_STREQ(ret, str);
    EXPECT_STREQ(str, "999");

    ret = gcvt(-99.456, 5, str);
    EXPECT_STREQ(ret, str);
    EXPECT_STREQ(str, "-99.456");

    ret = gcvt(0.968e8, 8, str);
    EXPECT_STREQ(ret, str);
    EXPECT_STREQ(str, "96800000");
}

/**
 * @tc.number SUB_KERNEL_IO_STDLIB_0200
 * @tc.name   fcvt basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFcvt, Function | MediumTest | Level1)
{
    int decpt, sign;
    char *ret = fcvt(123.456, 5, &decpt, &sign);
    EXPECT_STREQ(ret, "12345600");
    EXPECT_EQ(decpt, 3);
    EXPECT_EQ(sign, 0);

    ret = fcvt(-7.89e13, 12, &decpt, &sign);
    EXPECT_STREQ(ret, "789000000000000");
    EXPECT_EQ(decpt, 14);
    EXPECT_EQ(sign, 1);
}

/**
 * @tc.number SUB_KERNEL_IO_STDLIB_0300
 * @tc.name   swab basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSwab, Function | MediumTest | Level1)
{
    char src[20] = "hello world";
    char dest[20] = {0};
    swab(src, dest, sizeof(src));
    EXPECT_STREQ(dest, "ehll oowlr");

    char destS[20] = {0};
    swab(src, destS, -2);
    EXPECT_STREQ(destS, "");

    char srcT[20] = "1111111111";
    char destT[20] = {0};
    swab(srcT, destT, sizeof(srcT));
    EXPECT_STREQ(destT, "1111111111");
}

/**
 * @tc.number SUB_KERNEL_IO_STDLIB_0400
 * @tc.name   strtof basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtof, Function | MediumTest | Level1)
{
    char *endPtr = nullptr;
    float ret = strtof("  -3.40E+38 hello", &endPtr);
    EXPECT_FLOAT_EQ(ret, -3.40E+38);
    EXPECT_STREQ(endPtr, " hello");

    ret = strtof("  3.40E+38 ===", &endPtr);
    EXPECT_FLOAT_EQ(ret, 3.40E+38);
    EXPECT_STREQ(endPtr, " ===");

    ret = strtof("-9.6e17  this   123", &endPtr);
    EXPECT_FLOAT_EQ(ret, -9.6e17);
    EXPECT_STREQ(endPtr, "  this   123");

    ret = strtof("this is string", &endPtr);
    EXPECT_FLOAT_EQ(ret, 0);
    EXPECT_STREQ(endPtr, "this is string");
}

/**
 * @tc.number SUB_KERNEL_IO_STDLIB_0500
 * @tc.name   strtod basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtod, Function | MediumTest | Level1)
{
    char *endPtr = nullptr;
    double ret = strtod("  -1.79E+308 hello", &endPtr);
    EXPECT_DOUBLE_EQ(ret, -1.79E+308) << "strtod fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " hello") << "strtod fail, errno = " << errno;

    ret = strtod("1.79E+308  ===", &endPtr);
    EXPECT_DOUBLE_EQ(ret, 1.79E+308) << "strtod fail, errno = " << errno;
    EXPECT_STREQ(endPtr, "  ===") << "strtod fail, errno = " << errno;

    ret = strtod("-9.6e17  this   123", &endPtr);
    EXPECT_DOUBLE_EQ(ret, -9.6e17);
    EXPECT_STREQ(endPtr, "  this   123");

    ret = strtod("this is string", &endPtr);
    EXPECT_DOUBLE_EQ(ret, 0) << "strtod fail, errno = " << errno;
    EXPECT_STREQ(endPtr, "this is string") << "strtod fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDLIB_0600
 * @tc.name   strtold basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtold, Function | MediumTest | Level1)
{
    char *endPtr = nullptr;
    long double ret = strtold("  2.22507e-308 hello", &endPtr);
    EXPECT_NEAR(ret, 2.22507e-308, 0.0001) << "strtold fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " hello") << "strtold fail, errno = " << errno;

    ret = strtold("  1.79769e+308 ===", &endPtr);
    EXPECT_NEAR(ret, 1.79769e+308, 0.0001) << "strtold fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " ===") << "strtold fail, errno = " << errno;

    ret = strtold("-9.6e17  this   123", &endPtr);
    EXPECT_DOUBLE_EQ(ret, -9.6e17) << "strtold fail, errno = " << errno;
    EXPECT_STREQ(endPtr, "  this   123") << "strtold fail, errno = " << errno;

    ret = strtold("this is string", &endPtr);
    EXPECT_DOUBLE_EQ(ret, 0) << "strtold fail, errno = " << errno;
    EXPECT_STREQ(endPtr, "this is string") << "strtold fail, errno = " << errno;
}


/**
 * @tc.number SUB_KERNEL_IO_STDLIB_0700
 * @tc.name   strtol basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtol, Function | MediumTest | Level1)
{
    char nPtr[] = "12 0110 0XDEFE 0666 -1.6";
    char *endPtr = nullptr;
    long ret = strtol(nPtr, &endPtr, 10); // decimal
    EXPECT_EQ(ret, 12) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0110 0XDEFE 0666 -1.6");

    ret = strtol(endPtr, &endPtr, 2); // binary
    EXPECT_EQ(ret, 6) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0XDEFE 0666 -1.6");

    ret = strtol(endPtr, &endPtr, 16); // hexadecimal
    EXPECT_EQ(ret, 0XDEFE) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0666 -1.6");


    ret = strtol(endPtr, &endPtr, 8); // octal
    EXPECT_EQ(ret, 0666) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " -1.6");

    ret = strtol(endPtr, &endPtr, 65); // Invalid number format
    EXPECT_EQ(ret, 0) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " -1.6");

    ret = strtol(endPtr, &endPtr, 0); // decimal
    EXPECT_EQ(ret, -1) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".6");

    ret = strtol(endPtr, &endPtr, 10); // Invalid transform
    EXPECT_EQ(ret, 0) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".6");

    char nPtr1[] = "2147483647 2147483648 -2147483648 -2147483649"; // boundary value：2^31-1
    ret = strtol(nPtr1, &endPtr, 10);
    EXPECT_EQ(ret, 2147483647) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 2147483648 -2147483648 -2147483649");

    ret = strtol(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, 2147483647) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " -2147483648 -2147483649");

    ret = strtol(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, -2147483648) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " -2147483649");

    ret = strtol(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, -2147483648) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, "");
}

/**
 * @tc.number SUB_KERNEL_IO_STDLIB_0800
 * @tc.name   strtoul basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtoul, Function | MediumTest | Level1)
{
    char nPtr[] = "12 0110 0XDEFE 0666 4294967295 4294967296 12.34"; // 2^32-1
    char *endPtr = nullptr;
    unsigned long ret = strtoul(nPtr, &endPtr, 10);
    EXPECT_EQ(ret, 12UL) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0110 0XDEFE 0666 4294967295 4294967296 12.34");

    ret = strtoul(endPtr, &endPtr, 2);
    EXPECT_EQ(ret, 6UL) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0XDEFE 0666 4294967295 4294967296 12.34");

    ret = strtoul(endPtr, &endPtr, 16);
    EXPECT_EQ(ret, 0XDEFEUL) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0666 4294967295 4294967296 12.34");


    ret = strtoul(endPtr, &endPtr, 8);
    EXPECT_EQ(ret, 0666UL) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 4294967295 4294967296 12.34");

    ret = strtoul(endPtr, &endPtr, 0);
    EXPECT_EQ(ret, 4294967295UL) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 4294967296 12.34");

    ret = strtoul(endPtr, &endPtr, 0);
    EXPECT_EQ(ret, 4294967295UL) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 12.34");

    ret = strtoul(endPtr, &endPtr, 65);
    EXPECT_EQ(ret, 0UL) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 12.34");

    ret = strtoul(endPtr, &endPtr, 0);
    EXPECT_EQ(ret, 12UL) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".34");

    ret = strtoul(endPtr, &endPtr, 0);
    EXPECT_EQ(ret, 0UL) << "> strtol fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".34");
}

/**
 * @tc.number SUB_KERNEL_IO_STDLIB_0900
 * @tc.name   strtoll basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtoll, Function | MediumTest | Level1)
{
    char nPtr[] = "12 0110 0XDEFE 0666 -1.6";
    char *endPtr = nullptr;
    long long ret = strtoll(nPtr, &endPtr, 10);
    EXPECT_EQ(ret, 12) << "> strtoll fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0110 0XDEFE 0666 -1.6");

    ret = strtoll(endPtr, &endPtr, 2);
    EXPECT_EQ(ret, 6) << "> strtoll fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0XDEFE 0666 -1.6");

    ret = strtoll(endPtr, &endPtr, 16);
    EXPECT_EQ(ret, 0XDEFE) << "> strtoll fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0666 -1.6");


    ret = strtoll(endPtr, &endPtr, 8);
    EXPECT_EQ(ret, 0666) << "> strtoll fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " -1.6");

    ret = strtoll(endPtr, &endPtr, 65);
    EXPECT_EQ(ret, 0) << "> strtoll fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " -1.6");

    ret = strtoll(endPtr, &endPtr, 0);
    EXPECT_EQ(ret, -1) << "> strtoll fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".6");

    ret = strtoll(endPtr, &endPtr, 0);
    EXPECT_EQ(ret, 0) << "> strtoll fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".6");

    char nPtr1[] = "9223372036854775807 9223372036854775808 -9223372036854775807"; // 2^63-1
    ret = strtoll(nPtr1, &endPtr, 10);
    EXPECT_EQ(ret, 9223372036854775807) << "> strtoll fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 9223372036854775808 -9223372036854775807");

    ret = strtoll(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, 9223372036854775807) << "> strtoll fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " -9223372036854775807");

    ret = strtoll(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, -9223372036854775807) << "> strtoll fail, errno = " << errno;
    EXPECT_STREQ(endPtr, "");
}

/**
 * @tc.number SUB_KERNEL_IO_STDLIB_1000
 * @tc.name   strtoull basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtoull, Function | MediumTest | Level1)
{
    char nPtr[] = "12 0110 0XDEFE 0666 1.6";
    char *endPtr = nullptr;
    unsigned long long ret = strtoull(nPtr, &endPtr, 10);
    EXPECT_EQ(ret, 12ULL) << "> strtoull fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0110 0XDEFE 0666 1.6");

    ret = strtoull(endPtr, &endPtr, 2);
    EXPECT_EQ(ret, 6ULL) << "> strtoull fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0XDEFE 0666 1.6");

    ret = strtoull(endPtr, &endPtr, 16);
    EXPECT_EQ(ret, 0XDEFEULL) << "> strtoull fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0666 1.6");


    ret = strtoull(endPtr, &endPtr, 8);
    EXPECT_EQ(ret, 0666ULL) << "> strtoull fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 1.6");

    ret = strtoull(endPtr, &endPtr, 65);
    EXPECT_EQ(ret, 0ULL) << "> strtoull fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 1.6");

    ret = strtoull(endPtr, &endPtr, 0);
    EXPECT_EQ(ret, 1ULL) << "> strtoull fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".6");

    ret = strtoull(endPtr, &endPtr, 0);
    EXPECT_EQ(ret, 0ULL) << "> strtoull fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".6");

    char nPtr1[] = "18446744073709551615 18446744073709551616"; // 2^64-1
    ret = strtoull(nPtr1, &endPtr, 10);
    EXPECT_EQ(ret, 18446744073709551615ULL) << "> strtoull fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 18446744073709551616");

    ret = strtoull(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, 18446744073709551615ULL) << "> strtoull fail, errno = " << errno;
    EXPECT_STREQ(endPtr, "");
}

/**
 * @tc.number SUB_KERNEL_IO_STDLIB_1100
 * @tc.name   a64l basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testA64l, Function | MediumTest | Level1)
{
    long ret = a64l("./0123");
    EXPECT_EQ(ret, 1141645376);

    ret = a64l("./01234");
    EXPECT_EQ(ret, 1141645376);

    ret = a64l("abcdef");
    EXPECT_EQ(ret, -358184474);

    ret = a64l("Az0\0opq");
    EXPECT_EQ(ret, 12236);

    ret = a64l("v/");
    EXPECT_EQ(ret, 123);
}

