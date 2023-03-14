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

using namespace testing::ext;

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_0100
 * @tc.name   sscanf basic function test of float
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfFloat, Function | MediumTest | Level1)
{
    float f;
    int ret = sscanf("1.196532544", "%f", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.1965325);

    long double f1;
    ret = sscanf("1.196532544", "%Lf", &f1);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f1, 1.1965325);

    ret = sscanf("1.66666", "%6f", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.6666);

    ret = sscanf("1.66666", "%8f", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.66666);
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_0200
 * @tc.name   sscanf basic function test of float with format E
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfFloatE, Function | MediumTest | Level1)
{
    float f;
    long double f1;
    int ret = sscanf("1.100000e+00", "%e", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.100000);

    ret = sscanf("1.100000e+00", "%Le", &f1);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f1, 1.100000);

    ret = sscanf("1.100000e+00", "%5e", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.100000);

    ret = sscanf("1.100000E+00", "%E", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.100000);

    ret = sscanf("1.100000E+00", "%LE", &f1);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f1, 1.100000);

    ret = sscanf("1.100000E+00", "%5E", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.100000);
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_0300
 * @tc.name   sscanf basic function test of float with format G
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfFloatG, Function | MediumTest | Level1)
{
    float f;
    long double f1;
    int ret = sscanf("1.23e-07", "%g", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 0.000000123);

    ret = sscanf("1.23e-07", "%Lg", &f1);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f1, 0.000000123);

    ret = sscanf("1.23E-07", "%G", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.2300001e-07);

    ret = sscanf("1.23E-07", "%LG", &f1);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f1, 1.2300001e-07);
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_0400
 * @tc.name   sscanf basic function test of float with format A
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfFloatA, Function | MediumTest | Level1)
{
    float f;
    long double f1;
    int ret = sscanf("0x1.199999999999ap+0", "%a", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.1);

    ret = sscanf("0x1.199999999999ap+0", "%La", &f1);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f1, 1.1);

    ret = sscanf("0x1.199999999999ap+0", "%5a", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.0625);

    ret = sscanf("0X1.199999999999AP+0", "%A", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.1);

    ret = sscanf("0X1.199999999999AP+0", "%LA", &f1);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f1, 1.1);

    ret = sscanf("0X1.199999999999AP+0", "%5A", &f);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_FLOAT_EQ(f, 1.0625);
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_0500
 * @tc.name   sscanf basic function test of char
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfChar, Function | MediumTest | Level1)
{
    char c, c1;
    int ret = sscanf("a", "%c", &c);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(c, 'a');

    char c2[10] = {0};
    ret = sscanf("helloworld", "%5c", c2);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(c2[0], 'h');
    EXPECT_EQ(c2[1], 'e');
    EXPECT_EQ(c2[2], 'l');
    EXPECT_EQ(c2[3], 'l');
    EXPECT_EQ(c2[4], 'o');

    ret = sscanf("(a) and |b|", "(%c) and |%c%*c|", &c, &c1);
    EXPECT_EQ(ret, 2) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(c, 'a');
    EXPECT_EQ(c1, 'b');
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_0600
 * @tc.name   sscanf basic function test of string
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfString, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sscanf("hello", "%s", str);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello");

    ret = sscanf("hello", "%3s", str);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "hel");

    ret = sscanf("hellowo", "%6s", str);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "hellow");

    ret = sscanf("hello world", "%*s%s", str);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "world");
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_0700
 * @tc.name   sscanf basic function test of int
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfIntD, Function | MediumTest | Level1)
{
    int i;
    int ret = sscanf("-2147483648", "%d", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, -2147483648);

    ret = sscanf("2147483647", "%d", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 2147483647);

    ret = sscanf("2147483647", "%15d", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 2147483647);

    ret = sscanf("2147483647", "%8d", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 21474836);

    unsigned intU;
    ret = sscanf("0", "%u", &intU);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intU, 0U);

    ret = sscanf("4294967295", "%u", &intU);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intU, 4294967295U);

    ret = sscanf("4294967295", "%15u", &intU);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intU, 4294967295U);

    ret = sscanf("4294967295", "%5u", &intU);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intU, 42949U);
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_0800
 * @tc.name   sscanf basic function test of signed char
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfIntHhd, Function | MediumTest | Level1)
{
    signed char intHhd;
    int ret = sscanf("-128", "%hhd", &intHhd);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHhd, -128);

    ret = sscanf("127", "%hhd", &intHhd);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHhd, 127);

    ret = sscanf("127", "%5hhd", &intHhd);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHhd, 127);

    ret = sscanf("127", "%2hhd", &intHhd);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHhd, 12);

    unsigned char intHhu;
    ret = sscanf("0", "%hhu", &intHhu);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHhu, 0);

    ret = sscanf("255", "%hhu", &intHhu);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHhu, 255);

    ret = sscanf("255", "%5hhu", &intHhu);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHhu, 255);

    ret = sscanf("255", "%2hhu", &intHhu);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHhu, 25);
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_0900
 * @tc.name   sscanf basic function test of short int
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfIntHd, Function | MediumTest | Level1)
{
    short intHd;
    int ret = sscanf("32767", "%hd", &intHd);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHd, 32767);

    ret = sscanf("-32768", "%hd", &intHd);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHd, -32768);

    ret = sscanf("-32768", "%10hd", &intHd);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHd, -32768);

    ret = sscanf("-32768", "%5hd", &intHd);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHd, -3276);

    unsigned short intHu;
    ret = sscanf("0", "%hu", &intHu);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHu, 0);

    ret = sscanf("65535", "%hu", &intHu);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHu, 65535);

    ret = sscanf("65535", "%8hu", &intHu);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHu, 65535);

    ret = sscanf("65535", "%3hu", &intHu);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intHu, 655);
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_1000
 * @tc.name   sscanf basic function test of long int
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfIntLd, Function | MediumTest | Level1)
{
    long intL;
    int ret = sscanf("-2147483648", "%ld", &intL);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intL, -2147483648);

    ret = sscanf("2147483647", "%ld", &intL);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intL, 2147483647);

    ret = sscanf("2147483647", "%15ld", &intL);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intL, 2147483647);

    ret = sscanf("2147483647", "%5ld", &intL);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intL, 21474);

    unsigned long intUl;
    ret = sscanf("0", "%lu", &intUl);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUl, 0UL);

    ret = sscanf("4294967295   ", "%lu", &intUl);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUl, 4294967295   );

    ret = sscanf("4294967295   ", "%15lu", &intUl);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUl, 4294967295   );

    ret = sscanf("4294967295   ", "%5lu", &intUl);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUl, 42949UL);
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_1100
 * @tc.name   sscanf basic function test of long long int
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfIntLld, Function | MediumTest | Level1)
{
    long long intLl;
    int ret = sscanf("-9223372036854775807", "%lld", &intLl);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intLl, -9223372036854775807);

    ret = sscanf("9223372036854775807", "%lld", &intLl);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intLl, 9223372036854775807);

    ret = sscanf("9223372036854775807", "%25lld", &intLl);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intLl, 9223372036854775807);

    ret = sscanf("9223372036854775807", "%5lld", &intLl);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intLl, 92233);

    unsigned long long intUll;
    ret = sscanf("0", "%llu", &intUll);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUll, 0ULL);

    ret = sscanf("18446744073709551615", "%llu", &intUll);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUll, 18446744073709551615ULL);

    ret = sscanf("18446744073709551615", "%25llu", &intUll);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUll, 18446744073709551615ULL);

    ret = sscanf("18446744073709551615", "%5llu", &intUll);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUll, 18446ULL);
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_1200
 * @tc.name   sscanf basic function test of intmax_t and uintmax_t
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfIntMax, Function | MediumTest | Level1)
{
    intmax_t intMax;
    int ret = sscanf("-9223372036854775807", "%jd", &intMax);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intMax, -9223372036854775807);

    ret = sscanf("9223372036854775807", "%jd", &intMax);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intMax, 9223372036854775807);

    ret = sscanf("9223372036854775807", "%25jd", &intMax);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intMax, 9223372036854775807);

    ret = sscanf("9223372036854775807", "%5jd", &intMax);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intMax, 92233);

    uintmax_t intUmax;
    ret = sscanf("0", "%jd", &intUmax);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUmax, 0U);

    ret = sscanf("18446744073709551615", "%jd", &intUmax);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUmax, 18446744073709551615ULL);

    ret = sscanf("18446744073709551615", "%25jd", &intUmax);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUmax, 18446744073709551615ULL);

    ret = sscanf("18446744073709551615", "%5jd", &intUmax);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intUmax, 18446U);
}

/**
 * @tc.number SUB_KERNEL_IO_SSCANF_1300
 * @tc.name   sscanf basic function test of size_t
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfSizeT, Function | MediumTest | Level1)
{
    size_t intS;
    int ret = sscanf("0", "%zd", &intS);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intS, 0U);

    ret = sscanf("4294967295", "%zd", &intS);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intS, 4294967295U);

    ret = sscanf("4294967295", "%15zd", &intS);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intS, 4294967295U);

    ret = sscanf("4294967295", "%5zd", &intS);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intS, 42949U);
}

/**
 * @tc.number SUB_KERNEL_IO_SSCANF_1400
 * @tc.name   sscanf basic function test of ptrdiff_t
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfPtrdiffT, Function | MediumTest | Level1)
{
    ptrdiff_t intP;
    int ret = sscanf("-2147483648", "%td", &intP);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intP, -2147483648);

    ret = sscanf("2147483647", "%td", &intP);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intP, 2147483647);

    ret = sscanf("2147483647", "%15td", &intP);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intP, 2147483647);

    ret = sscanf("2147483647", "%5td", &intP);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(intP, 21474);
}

/**
 * @tc.number SUB_KERNEL_IO_SSCANF_1500
 * @tc.name   sscanf basic function test of pointer
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfPointer, Function | MediumTest | Level1)
{
    int i;
    int ret = sscanf("0x61", "%p", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 97);

    ret = sscanf("0xffffff", "%15p", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 16777215);

    ret = sscanf("0xffffff", "%6p", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 65535);
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_1600
 * @tc.name   sscanf basic function test of hex and oct
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfIntHexOct, Function | MediumTest | Level1)
{
    int i;
    int ret = sscanf("a", "%x", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 10);

    ret = sscanf("abaaaaa", "%6x", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 11250346);

    ret = sscanf("aba", "%6x", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 2746);

    ret = sscanf("A", "%X", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 10);

    ret = sscanf("ABAAAAA", "%6X", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 11250346);

    ret = sscanf("ABA", "%6X", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 2746);

    ret = sscanf("12", "%o", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 10);

    ret = sscanf("123", "%4o", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 83);

    ret = sscanf("12345", "%4o", &i);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(i, 668);
}

/**
 * @tc.number SUB_KERNEL_IO_SSCANF_1700
 * @tc.name   sscanf basic function test of regexp
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfRegexp, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sscanf("hello!world", "%[^!]", str);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "hello");

    ret = sscanf("1234956abolcd", "%[1-6a-d]", str);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "1234");

    ret = sscanf("123456abolcd", "%[1-6a-d]", str);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "123456ab");

    ret = sscanf("123ABCD", "%[^A-Z]", str);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "123");

    ret = sscanf("ABC-DEF@123", "%*[^-]-%[^@]", str);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "DEF");
}

 /**
 * @tc.number SUB_KERNEL_IO_SSCANF_1800
 * @tc.name   sscanf basic function test of return value
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSscanfReturnValue, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = sscanf("helloworld", "%*s%s", str);
    EXPECT_EQ(ret, -1) << "> sscanf fail, errno = " << errno;

    ret = sscanf("hello", "ab%5s123%*3s|>>>|", str);
    EXPECT_EQ(ret, 0) << "> sscanf fail, errno = " << errno;

    ret = sscanf("", "%*s%s", str);
    EXPECT_EQ(ret, -1) << "> sscanf fail, errno = " << errno;

    ret = sscanf("ab/c", "%s/%c/%d/%f", str);
    EXPECT_EQ(ret, 1) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "ab/c");

    char c;
    ret = sscanf("ab c", "%s %c %d/%f", str, &c);
    EXPECT_EQ(ret, 2) << "> sscanf fail, errno = " << errno;
    EXPECT_STREQ(str, "ab");
    EXPECT_EQ(c, 'c');

    int ip[4] = {0};
    ret = sscanf("11.11.11/11", "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
    EXPECT_EQ(ret, 3) << "> sscanf fail, errno = " << errno;
    EXPECT_EQ(ip[0], 11);
    EXPECT_EQ(ip[1], 11);
    EXPECT_EQ(ip[2], 11);
    EXPECT_EQ(ip[3], 0);
}
