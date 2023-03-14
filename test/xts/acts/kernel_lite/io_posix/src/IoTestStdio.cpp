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
#include <stdarg.h>

#include <err.h>
#include <gtest/gtest.h>
#include <threads.h>

#include "log.h"
#include "libfs.h"
#include "utils.h"
#include "IoTest.h"

using namespace testing::ext;

/**
 * @tc.number SUB_KERNEL_IO_STDIO_0100
 * @tc.name   ungetc basic function test
 * @tc.desc   [C- SOFTWAret -0200]
 */
HWTEST_F(IoTest, testUngetc, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    INIT_TEST_FILE(fp);
    FOPEN_READ(fp);
    int ret = getc(fp);
    EXPECT_EQ(ret, 'h');
    ret = ungetc(ret, fp);
    EXPECT_EQ(ret, 'h');
    char str[50] = {0};
    char *retS = fgets(str, sizeof(str), fp);
    EXPECT_STREQ(retS, str);
    EXPECT_STREQ(str, "hello world");
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_0200
 * @tc.name   puts basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testPuts, Function | MediumTest | Level1)
{
    int ret = puts("this is string");
    ASSERT_NE(ret, -1);

    ret = puts("hello  world!");
    ASSERT_NE(ret, -1);
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_0300
 * @tc.name   fmemopen basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFmemopen, Function | MediumTest | Level1)
{
    static char buf[] = "HELLO WORLD";
    FILE *fp = fmemopen(buf, sizeof(buf), "r");
    ASSERT_NE(fp, nullptr) << "> fmemopen fail, errno = " << errno;
    for (unsigned int i = 0; i < sizeof(buf); i++) {
        char c = fgetc(fp);
        EXPECT_EQ(c, buf[i]) << "> fmemopen fail, errno = " << errno;
    }
    EXPECT_NE(fclose(fp), -1) <<  "fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_0400
 * @tc.name   getw basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testGetw, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    int ret = putw('a', fp);
    ASSERT_EQ(ret, 0);
    ASSERT_NE(fclose(fp), -1) << "> fclose fail ,errno = " << errno;

    FOPEN_READ(fp);
    ret = getw(fp);
    EXPECT_EQ(ret, 'a');
    ASSERT_NE(fclose(fp), -1) << "> fclose fail ,errno = " << errno;
}

int FormatVdprintf(int fd, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vdprintf(fd, format, args);
    va_end(args);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_0500
 * @tc.name   vdprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVdprintf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    int fd;
    FILENO(fp);
    int ret = FormatVdprintf(fd, "%s has %d words", "hello world", 11);
    EXPECT_EQ(ret, 24) << "> vdprintf fail, errno = " << errno;
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    char str[50] = {0};
    char *retC = fgets(str, sizeof(str), fp);
    EXPECT_STREQ(retC, str);
    EXPECT_STREQ(retC, "hello world has 11 words");
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_0600
 * @tc.name   dprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testDprintf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    int fd;
    FILENO(fp);
    int ret = dprintf(fd, "%s has %d words", "hello world", 11);
    EXPECT_EQ(ret, 24) << "> dprintf fail, errno = " << errno;
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    char str[50] = {0};
    char *retC = fgets(str, sizeof(str), fp);
    EXPECT_STREQ(retC, str);
    EXPECT_STREQ(retC, "hello world has 11 words");
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_0700
 * @tc.name   fprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFprintf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    int ret = fprintf(fp, "%s has %d words", "helloworld", 10);
    EXPECT_EQ(ret, 23) << "> fprintf fail, errno = " << errno;
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    char str[50] = {0};
    int i;
    ret = fscanf(fp, "%s has %d words", str, &i);
    EXPECT_EQ(ret, 2);
    EXPECT_STREQ(str, "helloworld");
    EXPECT_EQ(i, 10);
    ASSERT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

int FormatVfprintf(FILE *stream, char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vfprintf(stream, format, args);
    va_end(args);
    return ret;
}

int FormatVfscanf(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vfscanf(stream, format, args);
    va_end(args);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_0800
 * @tc.name   vfprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVfprintf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    int ret = FormatVfprintf(fp, (char *)"%s has %d words", "helloworld", 10);
    EXPECT_EQ(ret, 23) << "> vfprintf fail, errno = " << errno;
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    char str[50] = {0};
    int i;
    ret = FormatVfscanf(fp, "%s has %d words", str, &i);
    EXPECT_EQ(ret, 2);
    EXPECT_STREQ(str, "helloworld");
    EXPECT_EQ(i, 10);
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_0900
 * @tc.name   clearerr basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testClearerr, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    fgetc(fp);
    int ret = ferror(fp);
    ASSERT_NE(ret, 0);
    clearerr(fp);
    ret = ferror(fp);
    EXPECT_EQ(ret, 0);
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_1000
 * @tc.name   printf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testPrintf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = 0;
        fp = freopen(IOTEST_TEMPFILE, "w", stdout);
        if (fp == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            rt = 1;
        }
        if (printf("%x %o %.6d", 11, 11, 11) != 11) {
            LOG("printf fail, errno = %d", errno);
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
        char str[50] = {0};
        char *gStr = fgets(str, sizeof(str), fp1);
        EXPECT_STREQ(gStr, str) << "fgets fail, errno = " << errno;
        EXPECT_STREQ(str, "b 13 000011") << "fgets fail, errno = " << errno;
        EXPECT_NE(fclose(fp1), -1) << "> fclose fail, errno = " << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_1100
 * @tc.name   scanf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testScanf, Function | MediumTest | Level1)
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
        char c;
        if (scanf("%c", &c) != 1) {
            LOG("scanf fail, errno = %d", errno);
            rt = 1;
        }
        if (c != 'h') {
            LOG("scanf fail, errno = %d", errno);
            rt = 1;
        }
        if (fclose(fp1) == -1) {
            LOG("fclose fail, errno = %d", errno);
        }
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);
    }
}

int FormatVasprintf(char **buffer, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vasprintf(buffer, fmt, ap);
    va_end(ap);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_1200
 * @tc.name   vasprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVasprintf, Function | MediumTest | Level1)
{
    char *buf = nullptr;
    int ret = FormatVasprintf(&buf, "%s has %d words", "hello world", 11);
    EXPECT_EQ(ret, 24);
    EXPECT_STREQ(buf, "hello world has 11 words");

    ret = FormatVasprintf(&buf, "%f and %c as well as %ld\n", 2.2, 'c', 6);
    EXPECT_EQ(ret, 28);
    EXPECT_STREQ(buf, "2.200000 and c as well as 6\n");
    free(buf);
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_1300
 * @tc.name   getline basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testGetline, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    char str[] = "hello world";
    int ret = fputs(str, fp);
    ASSERT_NE(ret, -1);
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    char *linePtr = nullptr;
    size_t len = 0;
    ssize_t retS = getline(&linePtr, &len, fp);
    EXPECT_EQ(retS, 11);
    EXPECT_STREQ(linePtr, "hello world");
    free(linePtr);
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_1400
 * @tc.name   getdelim basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testGetdelim, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    char str[] = "hello world";
    int ret = fputs(str, fp);
    ASSERT_NE(ret, -1);
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    char *linePtr = nullptr;
    size_t len = 0;
    ssize_t retS = getdelim(&linePtr, &len, ' ', fp);
    EXPECT_EQ(retS, 6);
    EXPECT_STREQ(linePtr, "hello ");
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
    free(linePtr);
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_1500
 * @tc.name   fgetpos basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFgetpos, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    INIT_TEST_FILE(fp);
    FOPEN_READ(fp);
    fpos_t pos;
    int ret = fgetpos(fp, &pos);
    EXPECT_EQ(ret, 0) << "fgetpos fail, errno = " << errno;
    char str[50] = {0};
    char *gStr = fgets(str, sizeof(str), fp);
    EXPECT_STREQ(gStr, str);
    EXPECT_STREQ(str, "hello world");
    ret = fsetpos(fp, &pos);
    EXPECT_EQ(ret, 0) << "fgetpos fail, errno = " << errno;
    gStr = fgets(str, sizeof(str), fp);
    EXPECT_STREQ(gStr, str);
    EXPECT_STREQ(str, "hello world");
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_1600
 * @tc.name   snprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSnprintf, Function | MediumTest | Level1)
{
    char buf[100] = {0};
    int ret = snprintf(buf, sizeof(buf), "%s has %d words", "hello world", 11);
    EXPECT_EQ(ret, 24) << "> snprintf fail, errno = " <<errno;
    EXPECT_STREQ(buf, "hello world has 11 words");

    ret = snprintf(buf, sizeof(buf), "%.2f %c\n", 1.1, 'c');
    EXPECT_EQ(ret, 7) << "> snprintf fail, errno = " <<errno;
    EXPECT_STREQ(buf, "1.10 c\n");
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_1700
 * @tc.name   asprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testAsprintf, Function | MediumTest | Level1)
{
    char *buf = nullptr;
    int ret = asprintf(&buf, "%s has %d words ", "hello world", 11);
    EXPECT_EQ(ret, 25);
    EXPECT_STREQ(buf, "hello world has 11 words ");

    ret = asprintf(&buf, "%o %c %x %f", 1, 'c', 2, 3.4);
    EXPECT_EQ(ret, 14);
    EXPECT_STREQ(buf, "1 c 2 3.400000");
    free(buf);
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_1800
 * @tc.name   gets basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testGets, Function | MediumTest | Level1)
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
        char str[50] = {0};
        if (gets(str) != str) {
            LOG("gets fail, errno = %d", errno);
            rt = 1;
            if (strcmp(gets(str), "hello world") != 0) {
                LOG("gets fail, errno = %d", errno);
                rt = 1;
            }
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

int FormatVsprintf(char *str, char *format, ...)
{
    va_list aptr;
    va_start(aptr, format);
    int ret = vsprintf(str, format, aptr);
    va_end(aptr);
    return(ret);
}

int FormatVsscanf(const char *str, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vsscanf(str, format, args);
    va_end(args);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_1900
 * @tc.name   vsprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVsprintf, Function | MediumTest | Level1)
{
    char str[50] = {0};
    int ret = FormatVsprintf(str, (char *)"%s has %d words", "helloworld", 10);
    EXPECT_EQ(ret, 23) << "> vsprintf fail, errno = " << errno;
    EXPECT_STREQ(str, "helloworld has 10 words");

    char strTemp[50] = {0};
    int i;
    ret = FormatVsscanf(str, "%s has %d words", strTemp, &i);
    EXPECT_EQ(ret, 2) << "> vsscanf fail, errno = " << errno;
    EXPECT_STREQ(strTemp, "helloworld");
    EXPECT_EQ(i, 10);
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_2000
 * @tc.name   fgetln basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFgetln, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    size_t len = 0;
    INIT_TEST_FILE(fp);
    FOPEN_READ(fp);
    char *ret = fgetln(fp, &len);
    EXPECT_STREQ(ret, "hello world") << "> fgetln fail , errno = " << errno;
    EXPECT_EQ(len, 11U) << "> fgetln fail , errno = " << errno;
    EXPECT_NE(fclose(fp), -1) << "fclose fail, errno = " << errno;
}

int FormatVsnprintf(char *format, ...)
{
    va_list vArgList;
    va_start(vArgList, format);
    char str[50] = {0};
    int ret = vsnprintf(str, sizeof(str), format, vArgList);
    va_end(vArgList);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_2100
 * @tc.name   vsnprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVsnprintf, Function | MediumTest | Level1)
{
    int ret = FormatVsnprintf((char *)"%s has %d words", "hello world", 11);
    EXPECT_EQ(ret, 24) << "> vsnprintf fail, errno = " << errno;

    ret = FormatVsnprintf((char *)"%f and %c as well as %ld\n", 2.2, 'c', 6);
    EXPECT_EQ(ret, 28) << "> vsnprintf fail, errno = " << errno;
}

void VwarnTest(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vwarn(fmt, ap);
    va_end(ap);
}

void VwarnxTest(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vwarnx(fmt, ap);
    va_end(ap);
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_2200
 * @tc.name   perror basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testPerror, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = 0;
        fp = freopen(IOTEST_TEMPFILE, "w", stderr);
        if (fp == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            rt = 1;
        }
        errno = 22;
        perror("perror msg");
        VwarnTest((char *)"vwarn msg");
        VwarnxTest((char *)"vwarnx msg");
        if (fclose(fp) == -1) {
            LOG("fclose fail, errno = %d", errno);
            rt = 1;
        }
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);

        FILE *fp1 = fopen(IOTEST_TEMPFILE, "r");
        ASSERT_NE(fp1, nullptr) << "fopen fail, errno = " << errno;
        char str[100] = {0};
        char *gStr = fgets(str, sizeof(str), fp1);
        EXPECT_STREQ(gStr, str);
        EXPECT_STREQ(str, "perror msg: Invalid argument\n");
        gStr = fgets(str, sizeof(str), fp1);
        EXPECT_STREQ(gStr, str);
        EXPECT_STREQ(str, "ActsIoApiTest.bin: vwarn msg: Invalid argument\n");
        gStr = fgets(str, sizeof(str), fp1);
        EXPECT_STREQ(gStr, str);
        EXPECT_STREQ(str, "ActsIoApiTest.bin: vwarnx msg\n");
        EXPECT_NE(fclose(fp1), -1) << "> fclose fail, errno = " << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_2300
 * @tc.name   rewind basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testRewind, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    INIT_TEST_FILE(fp);
    FOPEN_READ(fp);
    int ret = fgetc(fp);
    EXPECT_EQ(ret, 104);
    rewind(fp);
    ret = fgetc(fp);
    EXPECT_EQ(ret, 104);
    EXPECT_NE(fclose(fp), -1) << "fclose fail, errno = " <<  errno;
}

int FormatVprintf(char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_2400
 * @tc.name   vprintf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVprintf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = 0;
        fp = freopen(IOTEST_TEMPFILE, "w", stdout);
        if (fp == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            rt = 1;
        }
        if (FormatVprintf((char *)"%x %o %.6d", 11, 11, 11) != 11) {
            LOG("vprintf fail, errno = %d", errno);
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
        char str[50] = {0};
        char *gStr = fgets(str, sizeof(str), fp1);
        EXPECT_STREQ(gStr, str) << "fgets fail, errno = " << errno;
        EXPECT_STREQ(str, "b 13 000011") << "fgets fail, errno = " << errno;
        EXPECT_NE(fclose(fp1), -1) << "> fclose fail, errno = " << errno;
    }
}

int FormatVscanf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vscanf(format, args);
    va_end(args);
    return ret;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_2500
 * @tc.name   vscanf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testVscanf, Function | MediumTest | Level1)
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
        char c;
        if (FormatVscanf("%c", &c) != 1) {
            LOG("vscanf fail, errno = %d", errno);
            rt = 1;
        }
        if (c != 'h') {
            LOG("scanf fail, errno = %d", errno);
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

void *Thread(void *arg)
{
    FILE *fp = fopen(IOTEST_TEMPFILE, "w");
    EXPECT_NE(fp, nullptr) << "fopen fail, errno = " << errno;
    if (fp) {
        EXPECT_NE(fputs("hello world", fp), -1) << "fputs fail, errno = " << errno;
        EXPECT_NE(fclose(fp), -1) << "fclose fail, errno = " << errno;
    }

    FILE *fp1 = freopen(IOTEST_TEMPFILE, "r", stdin);
    EXPECT_TRUE(fp1 != nullptr) << "freopen fail, errno = " <<  errno;
    if (fp1) {
        if (getchar_unlocked() != EOF) {
            EXPECT_NE(getchar_unlocked(), -1) <<  "getchar_unlocked fail,  errno = " << errno;
        }
        EXPECT_NE(fclose(fp1), -1) << "fclose fail, errno = " << errno;
    }
    return nullptr;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_2600
 * @tc.name   getchar_unlocked basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testGetcharUnlocked, Function | MediumTest | Level1)
{
    pthread_t tid;
    int retI = pthread_create(&tid, NULL, Thread, NULL);
    ASSERT_EQ(retI, 0) << "> create thread fail !!!";
    retI = pthread_join(tid, NULL);

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
        if (getchar_unlocked() != EOF) {
            if (getchar_unlocked() == -1) {
                LOG("getchar_unlocked fail, errno = %d", errno);
                rt = 1;
            }
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
 * @tc.number SUB_KERNEL_IO_STDIO_2700
 * @tc.name   putchar_unlocked basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testPutcharUnlocked, Function | MediumTest | Level1)
{
    for (int i = 0; i < 128; i++) {
        int ret = putchar_unlocked(i);
        EXPECT_EQ(ret, i);
    }
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_2800
 * @tc.name   putc_unlocked basic function test
 * @tc.desc   [C- SOFTWAret -0200]
 */
HWTEST_F(IoTest, testPutcUnlocked, Function | MediumTest | Level1)
{
    FILE *fp;
    FOPEN_WRITE(fp);
    int ret = putc_unlocked('a', fp);
    EXPECT_EQ(ret, 'a');
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    ret = getc_unlocked(fp);
    EXPECT_EQ(ret, 'a');
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_2900
 * @tc.name   fputc_unlocked basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testFputcUnlocked, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    int ret = fputc_unlocked('a', fp);
    EXPECT_EQ(ret, 'a');
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    ret = fgetc_unlocked(fp);
    EXPECT_EQ(ret, 'a');
    EXPECT_NE(fclose(fp), -1) << "> fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_3000
 * @tc.name   setbuffer basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSetbuffer, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    char buf[100] = {0};
    int ret = setvbuf(fp, buf, _IOFBF, sizeof(buf));
    EXPECT_EQ(ret, 0) << "> setvbuf fail, errno = " << errno;
    ret = fprintf(fp, "%s", "abc");
    EXPECT_EQ(ret, 3) << "> fprintf fail, errno = " << errno;
    EXPECT_EQ(buf[8], 'a');
    EXPECT_EQ(buf[9], 'b');
    EXPECT_EQ(buf[10], 'c');

    buf[8] = 'k';
    EXPECT_EQ(buf[8], 'k');
    EXPECT_NE(fclose(fp), -1) << "fclose fail, errno = " << errno;

    EXPECT_EQ(buf[8], 'k');
    EXPECT_EQ(buf[9], 'b');
    EXPECT_EQ(buf[10], 'c');

    FOPEN_READ(fp);
    char str4[50] = {0};
    char *gStr = fgets(str4, sizeof(str4), fp);
    EXPECT_STREQ(gStr, str4);
    EXPECT_STREQ(gStr, "kbc");
    EXPECT_NE(fclose(fp), -1) << "fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_3100
 * @tc.name   setlinebuf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSetlinebuf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    char buf[100] = {0};
    int ret = setvbuf(fp, buf, _IOLBF, sizeof(buf));
    EXPECT_EQ(ret, 0) << "> setvbuf fail, errno = " << errno;
    ret = fprintf(fp, "%s", "he\nllo");
    EXPECT_EQ(ret, 6) << "> fprintf fail, errno = " << errno;
    EXPECT_EQ(buf[8], 'l');
    EXPECT_EQ(buf[9], 'l');
    EXPECT_EQ(buf[10], 'o');

    buf[9] = 'b';
    EXPECT_EQ(buf[9], 'b');
    EXPECT_NE(fclose(fp), -1) << "fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    char str4[50] = {0};
    char *gStr = fgets(str4, sizeof(str4), fp);
    EXPECT_STREQ(gStr, str4);
    EXPECT_STREQ(gStr, "he\n");
    EXPECT_NE(fclose(fp), -1) << "fclose fail, errno = " << errno;
}

/**
 * @tc.number SUB_KERNEL_IO_STDIO_3200
 * @tc.name   setbuf basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testSetbuf, Function | MediumTest | Level1)
{
    FILE *fp = nullptr;
    FOPEN_WRITE(fp);
    char buf[100] = {0};
    int ret = setvbuf(fp, nullptr, _IONBF, 0);
    EXPECT_EQ(ret, 0) << "> setvbuf fail, errno = " << errno;
    ret = fprintf(fp, "%s", "123");
    EXPECT_EQ(ret, 3) << "> fprintf fail, errno = " << errno;
    EXPECT_STREQ(buf, "") << "> setvbuf fail, errno = " << errno;
    EXPECT_EQ(buf[8], '\0');
    EXPECT_EQ(buf[9], '\0');
    EXPECT_EQ(buf[10], '\0');

    buf[10] = '1';
    EXPECT_EQ(buf[10], '1');
    EXPECT_NE(fclose(fp), -1) << "fclose fail, errno = " << errno;

    FOPEN_READ(fp);
    char str4[50] = {0};
    char *gStr = fgets(str4, sizeof(str4), fp);
    EXPECT_STREQ(gStr, str4);
    EXPECT_STREQ(gStr, "123");
    EXPECT_NE(fclose(fp), -1) << "fclose fail, errno = " << errno;
}

