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
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"
#include "KernelConstants.h"

using namespace testing::ext;

#define MPROTECT_TESTFILE "/storage/testMprotect.txt"

class MprotectApiTest : public testing::Test {
};

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_0100
 * @tc.name   mprotect function add read permission to the anonymous map area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectAnonAddReadPerm, Function | MediumTest | Level1)
{
    size_t len = PAGE_SIZE;
    volatile int sum = 0;
    int prot = PROT_WRITE;
    int mprot = prot | PROT_READ;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    EXPECT_TRUE(mprotect(mem, len, mprot) == 0) << "ERROR: mprotect() != 0";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        sum = mem[0] + mem[1];
        LOG("child: sum    = %d (0x%04x)", sum, sum);
        LOG("child: mem[0] = %c (0x%02x)", mem[0], mem[0]);
        LOG("child: mem[1] = %c (0x%02x)", mem[1], mem[1]);

        exit(0);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_0200
 * @tc.name   mprotect function add write permission to the anonymous map area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectAnonAddWritePerm, Function | MediumTest | Level2)
{
    size_t len = PAGE_SIZE;
    char testChar = 'A';
    int prot = PROT_READ;
    int mprot = prot | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    EXPECT_TRUE(mprotect(mem, len, mprot) == 0) << "ERROR: mprotect() != 0";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        mem[0] = testChar;
        mem[1] = testChar + 3;
        LOG("child: mem[0] = %c (0x%02x)", mem[0], mem[0]);
        LOG("child: mem[1] = %c (0x%02x)", mem[1], mem[1]);

        exit(0);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_0300
 * @tc.name   mprotect function add execute permission to the anonymous map area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectAnonAddExecutePerm, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    int prot = PROT_READ | PROT_WRITE;
    int mprot = prot | PROT_EXEC;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    unsigned long fnReturnFive[] = {0xe52db004, 0xe28db000, 0xe3a03005,
                                    0xe1a00003, 0xe28bd000, 0xe49db004, 0xe12fff1e};
    char *ptr = (char *)fnReturnFive;
    for (size_t i = 0; i < sizeof(fnReturnFive); i++) {
        mem[i] = ptr[i];
    }

    EXPECT_TRUE(mprotect(mem, len, mprot) == 0) << "ERROR: mprotect() != 0";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        int (*fun)(void) = (int (*)(void))mem;
        int five = fun();
        LOG("five = 0x%02x", five);
        exit(0);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_0500
 * @tc.name   mprotect function delete write permission to the anonymous map area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectAnonDelWritePerm, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    char testChar = 'A';
    int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    int mprot = PROT_READ | PROT_EXEC;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    EXPECT_TRUE(mprotect(mem, len, mprot) == 0) << "ERROR: mprotect() != 0";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        mem[0] = testChar;
        mem[1] = testChar + 3;
        LOG("child: mem[0] = %c (0x%02x)", mem[0], mem[0]);
        LOG("child: mem[1] = %c (0x%02x)", mem[1], mem[1]);
        exit(0);
    } else {
        ExpectProcCrashed(pid);
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_0600
 * @tc.name   mprotect function delete execute permission to the anonymous map area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectAnonDelExecutePerm, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    int mprot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    unsigned long fnReturnFive[] = {0xe52db004, 0xe28db000, 0xe3a03005,
                                    0xe1a00003, 0xe28bd000, 0xe49db004, 0xe12fff1e};
    char *ptr = (char *)fnReturnFive;

    for (size_t i = 0; i < sizeof(fnReturnFive); i++) {
        mem[i] = ptr[i];
    }

    EXPECT_TRUE(mprotect(mem, len, mprot) == 0) << "ERROR: mprotect() != 0";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        int (*fun)(void) = (int (*)(void))mem;
        int five = fun();
        LOG("five = 0x%02x", five);
        exit(0);
    } else {
        ExpectProcCrashed(pid);
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_0700
 * @tc.name   mprotect function add read permission to the file map area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectFileAddReadPerm, Function | MediumTest | Level2)
{
    size_t len = PAGE_SIZE;
    char testChar = 'A';
    int failure = 0;
    int prot = PROT_WRITE;
    int mprot = prot | PROT_READ;
    int flags = MAP_PRIVATE;

    char buf[PAGE_SIZE] = {testChar, (char)(testChar + 3)};
    char file[] = MPROTECT_TESTFILE;

    int fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, len);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    EXPECT_TRUE(mprotect(mem, len, mprot) == 0) << "ERROR: mprotect() != 0";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        read(fd, buf, len);
        if (buf[0] != mem[0] || mem[0] != testChar) {
            failure = -1;
        }
        if (buf[1] != mem[1] || mem[1] != (testChar + 3)) {
            failure = -1;
        }
        LOG("child: mem[0] = %c (0x%02x)", mem[0], mem[0]);
        LOG("child: mem[1] = %c (0x%02x)", mem[1], mem[1]);
        exit(failure);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_0800
 * @tc.name   mprotect function add write permission to the file map area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectFileAddWritePerm, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    char testChar = 'A';
    int failure = 0;
    int prot = PROT_READ;
    int mprot = prot | PROT_WRITE;
    int flags = MAP_SHARED;

    char buf[PAGE_SIZE] = {0};
    char file[] = MPROTECT_TESTFILE;

    int fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, len);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    EXPECT_TRUE(mprotect(mem, len, mprot) == 0) << "ERROR: mprotect() != 0";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        mem[0] = testChar;
        mem[1] = testChar + 3;
        LOG("child: mem[0] = %c (0x%02x)", mem[0], mem[0]);
        LOG("child: mem[1] = %c (0x%02x)", mem[1], mem[1]);
        exit(failure);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";

        fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
        EXPECT_TRUE(fd != -1) << "ERROR: open() == -1";

        read(fd, buf, len);

        LOG("parent: buf[0] = %c (0x%02x)", buf[0], buf[0]);
        LOG("parent: buf[1] = %c (0x%02x)", buf[1], buf[1]);

        EXPECT_TRUE(buf[0] == testChar) << "ERROR: buf[0] != testChar";
        EXPECT_TRUE(buf[1] == (testChar + 3)) << "ERROR: buf[1] != (testChar + 3)";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_0900
 * @tc.name   mprotect function add execute permission to the file map area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectFileAddExecutePerm, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    int prot = PROT_READ | PROT_WRITE;
    int mprot = prot | PROT_EXEC;
    int flags = MAP_PRIVATE;
    char file[] = MPROTECT_TESTFILE;

    unsigned long fnReturnFive[] = {0xe52db004, 0xe28db000, 0xe3a03005,
                                    0xe1a00003, 0xe28bd000, 0xe49db004, 0xe12fff1e};

    int fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, fnReturnFive, sizeof(fnReturnFive));
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    EXPECT_TRUE(mprotect(mem, len, mprot) == 0) << "ERROR: mprotect() != 0";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        int (*fun)(void) = (int (*)(void))mem;
        int five = fun();
        LOG("five = 0x%02x", five);
        exit(0);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_1100
 * @tc.name   mprotect function delete write permission to the file map area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectFileDelWritePerm, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    char testChar = 'A';
    int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    int mprot = PROT_READ | PROT_EXEC;
    int flags = MAP_PRIVATE;

    char buf[PAGE_SIZE] = {0};
    char file[] = MPROTECT_TESTFILE;

    int fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, len);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    EXPECT_TRUE(mprotect(mem, len, mprot) == 0) << "ERROR: mprotect() != 0";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        mem[0] = testChar;
        mem[1] = testChar + 3;
        LOG("child: mem[0] = %c (0x%02x)", mem[0], mem[0]);
        LOG("child: mem[1] = %c (0x%02x)", mem[1], mem[1]);
        exit(0);
    } else {
        ExpectProcCrashed(pid);

        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_1200
 * @tc.name   mprotect function delete execute permission to the file map area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectFileDelExecutePerm, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    int mprot = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE;
    char file[] = MPROTECT_TESTFILE;

    unsigned long fnReturnFive[] = {0xe52db004, 0xe28db000, 0xe3a03005,
                                    0xe1a00003, 0xe28bd000, 0xe49db004, 0xe12fff1e};

    int fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, fnReturnFive, sizeof(fnReturnFive));
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    EXPECT_TRUE(mprotect(mem, len, mprot) == 0) << "ERROR: mprotect() != 0";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        int (*fun)(void) = (int (*)(void))mem;
        int five = fun();
        LOG("five = 0x%02x", five);
        exit(0);
    } else {
        ExpectProcCrashed(pid);

        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_1300
 * @tc.name   mprotect function errno for EACCES test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectEACCES, Function | MediumTest | Level4)
{
    void *mem = nullptr;
    size_t len = PAGE_SIZE;
    char file[] = MPROTECT_TESTFILE;

    int fd = open(file, O_CREAT|O_RDONLY, S_IRUSR|S_IRGRP|S_IROTH);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    mem = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
    LOG("__LINE__ = %d, mem = %p", __LINE__, mem);
    EXPECT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED, errno = " << errno;

    int ret = mprotect(mem, len, PROT_WRITE);
    EXPECT_TRUE(ret == -1) << "mprotect() != -1, ret = " << ret;
    EXPECT_TRUE(errno == EACCES) << "ERROR: errno != EACCES, errno = " << errno << " EACCES = " << EACCES;

    EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
    Msleep(1000);
    EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
}

/**
 * @tc.number SUB_KERNEL_MEM_MPROTECT_1400
 * @tc.name   mprotect function errno for EINVAL test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MprotectApiTest, testMprotectEINVAL, Function | MediumTest | Level4)
{
    size_t len = PAGE_SIZE;

    void *mem = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
    LOG("__LINE__ = %d, mem = %p", __LINE__, mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED, errno = " << errno;

    size_t invalueAddr = ((size_t)(uintptr_t)mem) | 0x123;
    int ret = mprotect((void *)invalueAddr, len, PROT_WRITE);
    EXPECT_TRUE(ret == -1) << "mprotect() != -1, ret = " << ret;
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    ret = mprotect((void *)nullptr, len, PROT_WRITE);
    EXPECT_TRUE(ret == -1) << "mprotect() != -1, ret = " << ret;
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
}