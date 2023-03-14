/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */
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

#include <stdarg.h>

#include <wchar.h>

#include "IoTest.h"
#include <securec.h>

using namespace testing::ext;


int FormatVfwscanf(FILE *fp, const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vfwscanf(fp, format, args);
    va_end(args);
    return ret;
}

int FormatVfwprintf(FILE *fp, const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vfwprintf(fp, format, args);
    va_end(args);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_0100
 * @tc.name   vfwprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVfwprintf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    int ret = FormatVfwprintf(fp, L"%s has %d words ", "helloworld", 10);
    EXPECT_EQ(ret, 24);
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    wchar_t wStr[50] = {0};
    int i;
    ret = FormatVfwscanf(fp, L"%ls has %d words ", wStr, &i);
    EXPECT_EQ(ret, 2) << "> vfwscanf return fail, errno = " << errno;
    EXPECT_STREQ(wStr, L"helloworld");
    EXPECT_EQ(i, 10);

    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = "<< errno;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_0200
 * @tc.name   fwprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFwprintf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    int ret = fwprintf(fp, L"%s has %d words ", "helloworld", 10);
    EXPECT_EQ(ret, 24);
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    wchar_t wStr[50] = {0};
    int i;
    ret = fwscanf(fp, L"%ls has %d words ", wStr, &i);
    EXPECT_EQ(ret, 2) << "> vfwscanf return fail, errno = " << errno;
    EXPECT_STREQ(wStr, L"helloworld");
    EXPECT_EQ(i, 10);

    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = "<< errno;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_0300
 * @tc.name   fwide basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFwide, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    INIT_TEST_FILE(fp);
    FOPEN_READ(fp);
    int ret = fwide(fp, -6);
    EXPECT_EQ(ret, -1);
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = "<< errno;

    FOPEN_READ(fp);
    ret = fwide(fp, 0);
    EXPECT_EQ(ret, 0);
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = "<< errno;

    FOPEN_READ(fp);
    ret = fwide(fp, 8);
    EXPECT_EQ(ret, 1);
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = "<< errno;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_0400
 * @tc.name   fgetws basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFgetws, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    wchar_t wStr[50] = {0};
    wchar_t wStrT[50] = L"hello world";
    int ret = fputws(wStrT, fp);
    ASSERT_NE(ret, -1) << "> fputws fail, errno = " << errno;
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = "<< errno;

    FOPEN_READ(fp);
    wchar_t *retW = fgetws(wStr, sizeof(wStr)/sizeof(wStr[0]), fp);
    EXPECT_STREQ(retW, wStrT);
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = "<< errno;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_0500
 * @tc.name   putwc basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testPutwc, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    wint_t ret = putwc(L'A', fp);
    EXPECT_EQ(ret, L'A');
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    ret = getwc(fp);
    EXPECT_EQ(ret, L'A');
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_0600
 * @tc.name   ungetwc basic function test
 * @tc.desc   [C- SOFTWAret -0200]
 */
HWTEST_F(IoTest, testUngetwc, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    INIT_TEST_FILE(fp);
    FOPEN_READ(fp);
    wint_t ret = getwc(fp);
    EXPECT_EQ(ret, 104U);
    ret = ungetc(ret, fp);
    EXPECT_EQ(ret, 104U);
    wchar_t str[50] = {0};
    wchar_t *retS = fgetws(str, sizeof(str)/sizeof(str[0]), fp);
    EXPECT_STREQ(retS, str);
    EXPECT_STREQ(str, L"hello world");
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_0700
 * @tc.name   fputwc basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFputwc, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    wint_t ret = fputwc(L'A', fp);
    EXPECT_EQ(ret, L'A');
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    ret = fgetwc(fp);
    EXPECT_EQ(ret, L'A');
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_0800
 * @tc.name   fwscanf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFwscanf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    int ret = fwprintf(fp, L"%ls has %d words", L"helloworld", 10);
    EXPECT_EQ(ret, 23);
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = "<< errno;

    FOPEN_READ(fp);
    wchar_t wStr[50] = {0};
    int i;
    ret = fwscanf(fp, L"%ls has %d words", wStr, &i);
    EXPECT_EQ(ret, 2);
    EXPECT_EQ(i, 10);
    EXPECT_STREQ(wStr, L"helloworld");
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = "<< errno;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_0900
 * @tc.name   wmemset  basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testWmemset, Function | MediumTest | Level1)
{
    wchar_t dest[50] = {0};
    wchar_t *ret = wmemset(dest, L'=', 8);
    EXPECT_STREQ(ret, dest);
    EXPECT_STREQ(dest, L"========");

    ret = wmemset(dest, L' ', 8);
    EXPECT_STREQ(ret, dest);
    EXPECT_STREQ(dest, L"        ");

    ret = wmemset(dest, L'!', 0);
    EXPECT_STREQ(ret, dest);
    EXPECT_STREQ(dest, L"        ");
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_1000
 * @tc.name   wmemchr basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testWmemchr, Function | MediumTest | Level1)
{
    wchar_t src[] = L"hello world";
    wchar_t *ret = wmemchr(src, L' ', sizeof(src)/sizeof(src[0]) - 1);
    EXPECT_STREQ(ret, L" world");

    wchar_t srcT[] = L"this is string";
    ret = wmemchr(srcT, L'?', sizeof(srcT)/sizeof(srcT[0]) - 1);
    EXPECT_STREQ(ret, nullptr);
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_1100
 * @tc.name   wmemcpy basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testWmemcpy, Function | MediumTest | Level1)
{
    wchar_t src[] = L"hello";
    wchar_t dest[] = L"world ! ! !";
    wchar_t *ret = wmemcpy(dest, src, 5);
    EXPECT_STREQ(ret, L"hello ! ! !");

    wchar_t srcT[] = L"this is";
    wchar_t destT[] = L"string";
    ret = wmemcpy(destT, srcT, 0);
    EXPECT_STREQ(ret, destT);
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_1200
 * @tc.name   wmemmove basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testWmemmove, Function | MediumTest | Level1)
{
    wchar_t src[] = L"hello";
    wchar_t dest[] = L"world ! ! !";
    wchar_t *ret = wmemmove(dest, src, 5);
    EXPECT_STREQ(ret, L"hello ! ! !");

    wchar_t srcT[] = L"this is";
    wchar_t destT[] = L"string";
    ret = wmemmove(destT, srcT, 0);
    EXPECT_STREQ(ret, destT);
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_1300
 * @tc.name   putwchar basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testPutwchar, Function | MediumTest | Level1)
{
    for (wchar_t wc = L'A'; wc <= L'Z'; ++wc) {
        wint_t ret = putwchar(wc);
        EXPECT_EQ(ret, wc);
    }
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_1400
 * @tc.name   wmemcmp basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testWmemcmp, Function | MediumTest | Level1)
{
    wchar_t dest[] = L"BBCDEFG";
    wchar_t src[] = L"CBCDEFF";
    int ret = wmemcmp(dest, src, 7);
    EXPECT_LT(ret, 0);

    ret = wmemcmp(L"abcdefg", L"ABCDEFG", 2);
    EXPECT_GT(ret, 0);

    ret = wmemcmp(L"ABCDEFG", L"ABCDEFG", 6);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_1500
 * @tc.name   mbsinit basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testMbsinit, Function | MediumTest | Level1)
{
    mbstate_t *ps = nullptr;
    int ret = mbsinit(ps);
    int memRet = -1;
    EXPECT_NE(ret, 0);

    mbstate_t psF;
    memRet = memset_s(&psF, sizeof(psF), 0, sizeof(psF));
    EXPECT_EQ(0, memRet);
    ret = mbsinit(&psF);
    EXPECT_NE(ret, 0);
}

int FormatVswscanf(const wchar_t *str, const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vswscanf(str, format, args);
    va_end(args);
    return ret;
}

int FormatVswprintf(const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
    wchar_t str[50] = {0};
    int ret = vswprintf(str, sizeof(str)/sizeof(str[0]), format, args);
    fputws(str, stdout);
    va_end(args);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_1600
 * @tc.name   vswprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVswprintf, Function | MediumTest | Level1)
{
    int ret = FormatVswprintf(L"%ls has %d words", L"helloworld", 10);
    EXPECT_EQ(ret, 23);

    int i;
    wchar_t wBuf[50] = {0};
    wchar_t str[] = L"helloworld has 10 words";
    ret = FormatVswscanf(str, L"%ls has %d words", wBuf, &i);
    EXPECT_EQ(ret, 2) << "> vswcanf return fail, errno = " << errno;
    EXPECT_STREQ(wBuf, L"helloworld");
    EXPECT_EQ(i, 10);
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_1700
 * @tc.name   swprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSwprintf, Function | MediumTest | Level1)
{
    wchar_t wBuf[50] = {0};
    int ret = swprintf(wBuf, sizeof(wBuf)/sizeof(wBuf[0]), L"%ls has %d words", L"helloworld", 10);
    EXPECT_EQ(ret, 23);
    EXPECT_STREQ(wBuf, L"helloworld has 10 words");

    wchar_t wStr[50] = {0};
    int i;
    ret = swscanf(wBuf, L"%ls has %d words", wStr, &i);
    EXPECT_EQ(ret, 2) << "> swscanf return fail, errno = " << errno;
    EXPECT_EQ(i, 10);
    EXPECT_STREQ(wStr, L"helloworld");
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_1800
 * @tc.name   wprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testWprintf, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = 0;
        FILE *fp = freopen(IOTEST_TEMPFILE, "w", stdout);
        if (fp == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            rt = 1;
        }
        if (wprintf(L"%ls %d", L"helloworld", 10) != 13) {
            LOG("wprintf fail, errno = %d", errno);
            rt = 1;
        }
        if (fclose(fp) == -1) {
            LOG("fclose fail, errno = %d", errno);
            rt = 1;
        }
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);

        FILE *fp1 = fopen(IOTEST_TEMPFILE, "r");
        ASSERT_NE(fp1, nullptr) << "fopen fail, errno = " << errno;
        wchar_t str[50] = {0};
        wchar_t *gStr = fgetws(str, sizeof(str)/sizeof(str[0]), fp1);
        EXPECT_STREQ(gStr, str) << "fgetws fail, errno = " << errno;
        EXPECT_STREQ(str, L"helloworld 10") << "fgetws fail, errno = " << errno;
        EXPECT_NE(fclose(fp1), -1) << "> fclose fail, errno = " << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_1900
 * @tc.name   wscanf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testWscanf, Function | MediumTest | Level1)
{
    FILE  *fp = nullptr;
    INIT_TEST_FILE(fp);
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = 0;
        FILE *fp1 = freopen(IOTEST_TEMPFILE, "r", stdin);
        if (fp1 == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            rt = 1;
        }
        wchar_t str[50] = {0};
        if (wscanf(L"%ls", str) != 1) {
            LOG("wscanf fail, errno = %d", errno);
            rt = 1;
            if (wcscmp(str, L"hello world") != 0) {
                LOG("wscanf fail, errno = %d", errno);
                rt = 1;
            }
        }
        if (fclose(fp1) == -1) {
            LOG("fclose  fail, errno = %d", errno);
        }
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);
    }
}

int FormatVwprintf(const wchar_t *format, ...)
{
    va_list argPtr;
    va_start(argPtr, format);
    int ret = vwprintf(format, argPtr);
    va_end(argPtr);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_2000
 * @tc.name   vwprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVwprintf, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = 0;
        FILE *fp = freopen(IOTEST_TEMPFILE, "w", stdout);
        if (fp == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            rt = 1;
        }
        if (FormatVwprintf(L"%ls %d", L"helloworld", 10) != 13) {
            LOG("vwprintf fail, errno = %d", errno);
            rt = 1;
        }
        if (fclose(fp) == -1) {
            LOG("fclose fail, errno = %d", errno);
            rt = 1;
        }
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);

        FILE *fp1 = fopen(IOTEST_TEMPFILE, "r");
        ASSERT_NE(fp1, nullptr) << "fopen fail, errno = " << errno;
        wchar_t str[50] = {0};
        wchar_t *gStr = fgetws(str, sizeof(str)/sizeof(str[0]), fp1);
        EXPECT_STREQ(gStr, str) << "fgetws fail, errno = " << errno;
        EXPECT_STREQ(str, L"helloworld 10") << "fgetws fail, errno = " << errno;
        EXPECT_NE(fclose(fp1), -1) << "> fclose fail, errno = " << errno;
    }
}

int FormatVwscanf(const wchar_t *format, ...)
{
    va_list argPtr;
    va_start(argPtr, format);
    int ret = vwscanf(format, argPtr);
    va_end(argPtr);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_2100
 * @tc.name   vwscanf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVwscanf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    INIT_TEST_FILE(fp);
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = 0;
        FILE *fp1 = freopen(IOTEST_TEMPFILE, "r", stdin);
        if (fp1 == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            rt = 1;
        }
        wchar_t str[50] = {0};
        if (FormatVwscanf(L"%ls", str) != 1) {
            LOG("vwscanf fail, errno = %d", errno);
            rt = 1;
            if (wcscmp(str, L"hello world") != 0) {
                LOG("vwscanf fail, errno = %d", errno);
                rt = 1;
            }
        }
        if (fclose(fp1) == -1) {
            LOG("fclose fail, errno = %d", errno);
        }
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_2200
 * @tc.name   getwchar basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testGetwchar, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    INIT_TEST_FILE(fp);
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = 0;
        FILE *fp1 = freopen(IOTEST_TEMPFILE, "r", stdin);
        if (fp1 == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            rt = 1;
        }
        if (getwchar() != 104) {
            LOG("getwchar fail, errno = %d", errno);
            rt = 1;
        }
        if (fclose(fp1) == -1) {
            LOG("fclose fail, errno = %d", errno);
            rt = 1;
        }
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IO_WCHAR_2300
 * @tc.name   btowc basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testBtowc, Function | MediumTest | Level1)
{
    EXPECT_EQ(btowc('6'), L'6');

    EXPECT_EQ(btowc('A'), L'A');

    EXPECT_EQ(btowc('&'), L'&');

    EXPECT_EQ(btowc(EOF), WEOF);
}