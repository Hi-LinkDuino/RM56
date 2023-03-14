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

#include <sys/uio.h>

#include <grp.h>
#include <shadow.h>
#include <pwd.h>
#include <inttypes.h>
#include <iconv.h>

#include "IoTest.h"
#include <securec.h>

using namespace testing::ext;

/**
 * @tc.number SUB_KERNEL_IO_OTHER_0400
 * @tc.name   toupper basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testToupper, Function | MediumTest | Level1)
{
    for (int i = 0; i < 26; i++) {
        int ret = _toupper('a' + i);
        EXPECT_EQ(ret, 'A' + i);
    }
}

/**
 * @tc.number SUB_KERNEL_IO_OTHER_0500
 * @tc.name   strtoimax basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtuimax, Function | MediumTest | Level1)
{
    char nPtr[] = "12 0110 0XDEFE 0666 -1.6";
    char *endPtr = nullptr;
    intmax_t ret = strtoimax(nPtr, &endPtr, 10);
    EXPECT_EQ(ret, 12) << "> strtoimax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0110 0XDEFE 0666 -1.6");

    ret = strtoimax(endPtr, &endPtr, 2);
    EXPECT_EQ(ret, 6) << "> strtoimax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0XDEFE 0666 -1.6");

    ret = strtoimax(endPtr, &endPtr, 16);
    EXPECT_EQ(ret, 0XDEFE) << "> strtoimax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0666 -1.6");


    ret = strtoimax(endPtr, &endPtr, 8);
    EXPECT_EQ(ret, 0666) << "> strtoimax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " -1.6");

    ret = strtoimax(endPtr, &endPtr, 65);
    EXPECT_EQ(ret, 0) << "> strtoimax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " -1.6");

    ret = strtoimax(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, -1) << "> strtoimax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".6");

    ret = strtoimax(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, 0) << "> strtoimax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".6");

    char nPtr1[] = "9223372036854775807 9223372036854775808 -9223372036854775807"; // 2^63-1
    ret = strtoimax(nPtr1, &endPtr, 10);
    EXPECT_EQ(ret, 9223372036854775807) << "> strtoimax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 9223372036854775808 -9223372036854775807");

    ret = strtoimax(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, 9223372036854775807) << "> strtoimax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " -9223372036854775807");

    ret = strtoimax(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, -9223372036854775807) << "> strtoimax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, "");
}

/**
 * @tc.number SUB_KERNEL_IO_OTHER_0600
 * @tc.name   strtoumax basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtoumax, Function | MediumTest | Level1)
{
    char nPtr[] = "12 0110 0XDEFE 0666 1.6";
    char *endPtr = nullptr;
    uintmax_t ret = strtoumax(nPtr, &endPtr, 10);
    EXPECT_EQ(ret, 12ULL) << "> strtoumax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0110 0XDEFE 0666 1.6");

    ret = strtoumax(endPtr, &endPtr, 2);
    EXPECT_EQ(ret, 6ULL) << "> strtoumax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0XDEFE 0666 1.6");

    ret = strtoumax(endPtr, &endPtr, 16);
    EXPECT_EQ(ret, 57086ULL) << "> strtoumax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 0666 1.6");


    ret = strtoumax(endPtr, &endPtr, 8);
    EXPECT_EQ(ret, 438ULL) << "> strtoumax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 1.6");

    ret = strtoumax(endPtr, &endPtr, 65);
    EXPECT_EQ(ret, 0ULL) << "> strtoumax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 1.6");

    ret = strtoumax(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, 1ULL) << "> strtoumax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".6");

    ret = strtoumax(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, 0ULL) << "> strtoumax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, ".6");

    char nPtr1[] = "18446744073709551615 18446744073709551616"; // 2^64-1
    ret = strtoumax(nPtr1, &endPtr, 10);
    EXPECT_EQ(ret, 18446744073709551615ULL) << "> strtoumax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, " 18446744073709551616");

    ret = strtoumax(endPtr, &endPtr, 10);
    EXPECT_EQ(ret, 18446744073709551615ULL) << "> strtoumax fail, errno = " << errno;
    EXPECT_STREQ(endPtr, "");
}

/**
 * @tc.number SUB_KERNEL_IO_OTHER_0700
 * @tc.name   writev basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testWritev, Function | MediumTest | Level1)
{
    struct iovec iov[2];
    char buf1[6] = {0};
    iov[0].iov_base = buf1;
    iov[0].iov_len = sizeof(buf1) - 1;

    char buf2[6] = {0};
    iov[1].iov_base = buf2;
    iov[1].iov_len = sizeof(buf2) - 1;

    FILE *fp = nullptr;
    INIT_TEST_FILE(fp);
    FOPEN_READ(fp);
    int fd = 0;
    FILENO(fp);
    ssize_t ret = readv(fd, iov, 2);
    EXPECT_EQ(ret, 10);
    EXPECT_STREQ(buf1, "hello");
    EXPECT_STREQ(buf2, " worl");
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_WRITE(fp);
    FILENO(fp);
    ret = writev(fd, iov, 2);
    EXPECT_EQ(ret, 10);
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    char str[50] = {0};
    char *gStr = fgets(str, sizeof(str), fp);
    EXPECT_STREQ(gStr, str);
    EXPECT_STREQ(str, "hello worl");
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_OTHER_0800
 * @tc.name   pread basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testPread, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    int fd = 0;
    FILENO(fp);
    char str[50] = "hello world";
    ssize_t retR = pwrite(fd, str, strlen(str), 6);
    EXPECT_EQ(retR, 11);
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    FILENO(fp);
    char buf[50] = {0};
    retR = pread(fd, buf, strlen(str), 6);
    EXPECT_EQ(retR, 11);
    EXPECT_STREQ(buf, "hello world");
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_OTHER_0900
 * @tc.name   strptime basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrptime, Function | MediumTest | Level1)
{
    struct tm tm;
    int memRet = -1;
    memRet = memset_s(&tm, sizeof(struct tm), 0, sizeof(struct tm));
    EXPECT_EQ(0, memRet);
    char *ret = strptime("2020-10-29 21:24:00abc", "%Y-%m-%d %H:%M:%S", &tm);
    EXPECT_STREQ(ret, "abc");
    EXPECT_EQ(tm.tm_year, 120);
    EXPECT_EQ(tm.tm_mon, 9);
    EXPECT_EQ(tm.tm_mday, 29);
    EXPECT_EQ(tm.tm_hour, 21);
    EXPECT_EQ(tm.tm_min, 24);

    ret = strptime("14 Oct October 20 09:24:00 Sat Saturday 363", "%d %b %B %y %I:%M:%S %a %A %j", &tm);
    EXPECT_STREQ(ret, "");
    EXPECT_EQ(tm.tm_year, 120);
    EXPECT_EQ(tm.tm_mon, 9);
    EXPECT_EQ(tm.tm_mday, 14);
    EXPECT_EQ(tm.tm_hour, 9);
    EXPECT_EQ(tm.tm_wday, 6);
    EXPECT_EQ(tm.tm_yday, 362);
}

/**
 * @tc.number SUB_KERNEL_IO_OTHER_1000
 * @tc.name   iconv basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testIconv, Function | MediumTest | Level1)
{
    char inBuf[50] = "hello";
    size_t inLen = strlen(inBuf);
    char outBuf[50] = {0};
    size_t outLen = sizeof(outBuf);
    char *inSrc = inBuf;
    char *outSrc = outBuf;
    iconv_t cd = iconv_open("UTF_32", "UTF_16");
    size_t ret = iconv(cd, &inSrc, &inLen, &outSrc, &outLen);
    EXPECT_EQ(ret, 4294967295U) << "iconv fail, errno = " << errno;
    EXPECT_STREQ(inSrc, "o") << "iconv fail, errno = " << errno;
    EXPECT_EQ(inLen, 1U) << "iconv fail, errno = " << errno;
    EXPECT_STREQ(outSrc, "") << "iconv fail, errno = " << errno;
    EXPECT_EQ(outLen, 42U) << "iconv fail, errno = " << errno;
    EXPECT_STREQ(outBuf, "") << "iconv fail, errno = " << errno;
    EXPECT_EQ(iconv_close(cd), 0) << "iconv_close fail, errn = " << errno;
}