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

#define MMAP_TESTFILE "/storage/testMmap.txt"

class MmapApiTest : public testing::Test {
};

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_0100
 * @tc.name   mmap function anonymous private map and read and write permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapAnonPrivate, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    int sum = 0;
    char testChar = 'A';
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    for (size_t i = 0; i < len; i++) {
        sum += mem[i];
    }
    ASSERT_TRUE(sum == 0) << "sum != 0, sum = " << sum;

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
        LOG("parent: mem[0] = %c (0x%02x)", mem[0], mem[0]);
        LOG("parent: mem[1] = %c (0x%02x)", mem[1], mem[1]);

        EXPECT_TRUE(mem[0] != testChar) << "mem[0] = " << mem[0];
        EXPECT_TRUE(mem[1] != (testChar + 3)) << "mem[1] = " << mem[1];
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_0200
 * @tc.name   mmap function anonymous share map and read and write permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapAnonShare, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    char testChar = 'A';
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_SHARED;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

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
        LOG("parent: mem[0] = %c (0x%02x)", mem[0], mem[0]);
        LOG("parent: mem[1] = %c (0x%02x)", mem[1], mem[1]);
        // not support at present, suggest shm, after support need change to ==
        EXPECT_TRUE(mem[0] != testChar) << "mem[0] = " << mem[0];
        EXPECT_TRUE(mem[1] != (testChar + 3)) << "mem[1] = " << mem[1];
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_0300
 * @tc.name   mmap function anonymous share map and only read permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapAnonShareOnlyRead, Function | MediumTest | Level1)
{
    size_t len = PAGE_SIZE;
    char testChar = 'A';
    int prot = PROT_READ;
    int flags = MAP_ANONYMOUS | MAP_SHARED;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        /* Only read permit and write data to this area cause process crash */
        mem[0] = testChar;
        mem[1] = testChar + 3;
        exit(0);
    } else {
        ExpectProcCrashed(pid);

        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_0400
 * @tc.name   mmap function anonymous share map and only write permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapAnonShareOnlyWrite, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    int prot = PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_SHARED;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    LOG("mem = %p", mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        char testChar = 'A';
        mem[0] = testChar;
        mem[1] = testChar + 3;

        LOG("mem[0] = 0x%02x", mem[0]);
        LOG("mem[1] = 0x%02x", mem[1]);
        exit(0);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_0500
 * @tc.name   mmap function anonymous private map and execute permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapAnonPrivateExec, Function | MediumTest | Level4)
{
    char *mem = nullptr;
    size_t len = PAGE_SIZE / 4;
    size_t i;
    int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;
    int (*fun)(void);

    mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    unsigned long fnReturnFive[] = {0xe52db004, 0xe28db000, 0xe3a03005,
                                    0xe1a00003, 0xe28bd000, 0xe49db004, 0xe12fff1e};
    char *ptr = (char *)fnReturnFive;

    for (i = 0; i < sizeof(fnReturnFive); i++) {
        mem[i] = ptr[i];
    }

    for (i = 0; i < 30; i++) {
        printf("%02x ", mem[i]);
    }
    printf("\r\n");

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        fun = (int (*)(void))mem;
        int five = fun();
        LOG("five = 0x%02x", five);

        if (five == 0x05) {
            exit(0);
        }
        exit(-1);
    } else {
        WaitProcExitedOK(pid);

        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_0600
 * @tc.name   mmap function anonymous share map and execute permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapAnonShareExec, Function | MediumTest | Level4)
{
    char *mem = nullptr;
    size_t len = PAGE_SIZE / 4;
    size_t i;
    int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    int flags = MAP_ANONYMOUS | MAP_SHARED;
    int (*fun)(void);

    mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    unsigned long fnReturnFive[] = {0xe52db004, 0xe28db000, 0xe3a03005,
                                    0xe1a00003, 0xe28bd000, 0xe49db004, 0xe12fff1e};
    char *ptr = (char *)fnReturnFive;

    for (i = 0; i < sizeof(fnReturnFive); i++) {
        mem[i] = ptr[i];
    }

    for (i = 0; i < 30; i++) {
        printf("%02x ", mem[i]);
    }
    printf("\r\n");

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        fun = (int (*)(void))mem;
        int five = fun();
        LOG("five = 0x%02x", five);

        if (five == 0x05) {
            exit(0);
        }
        exit(-1);
    } else {
        WaitProcExitedOK(pid);

        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_0700
 * @tc.name   mmap function anonymous share map and NO execute permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapAnonShareNoExec, Function | MediumTest | Level2)
{
    char *mem = nullptr;
    size_t len = PAGE_SIZE / 4;
    size_t i;
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_SHARED;
    int (*fun)(void);

    mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    unsigned long fnReturnFive[] = {0xe52db004, 0xe28db000, 0xe3a03005,
                                    0xe1a00003, 0xe28bd000, 0xe49db004, 0xe12fff1e};
    char *ptr = (char *)fnReturnFive;

    for (i = 0; i < sizeof(fnReturnFive); i++) {
        mem[i] = ptr[i];
    }

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        fun = (int (*)(void))mem;
        int five = fun();
        LOG("five = 0x%02x", five);

        exit(0);
    } else {
        ExpectProcCrashed(pid);

        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_0800
 * @tc.name   mmap function file private map and read and write permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapFilePrivate, Function | MediumTest | Level3)
{
    const size_t len = PAGE_SIZE;
    char testChar = 'A';
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE;
    char buf[PAGE_SIZE] = {0};
    char file[] = MMAP_TESTFILE;

    int fd = open(file, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, len);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        mem[0] = testChar;
        mem[1] = testChar + 3;
        exit(0);
    } else {
        WaitProcExitedOK(pid);

        LOG("mem[0] = %c (0x%02x)", mem[0], mem[0]);
        LOG("mem[1] = %c (0x%02x)", mem[1], mem[1]);

        EXPECT_TRUE(mem[0] != testChar) << "mem[0] = " << mem[0];
        EXPECT_TRUE(mem[1] != (testChar + 3)) << "mem[1] = " << mem[1];
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";

        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_0900
 * @tc.name   mmap function file share map and read and write permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapFileShare, Function | MediumTest | Level3)
{
    const size_t len = PAGE_SIZE;
    char testChar = 'A';
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED;
    char buf[PAGE_SIZE] = {0};
    char file[] = MMAP_TESTFILE;

    int fd = open(file, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, len);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        mem[0] = testChar;
        mem[1] = testChar + 3;
        exit(0);
    } else {
        WaitProcExitedOK(pid);

        LOG("mem[0] = %c (0x%02x)", mem[0], mem[0]);
        LOG("mem[1] = %c (0x%02x)", mem[1], mem[1]);

        EXPECT_TRUE(mem[0] == testChar) << "mem[0] = " << mem[0];
        EXPECT_TRUE(mem[1] == (testChar + 3)) << "mem[1] = " << mem[1];
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";

        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_1000
 * @tc.name   mmap function file share map and only read permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapFileShareOnlyRead, Function | MediumTest | Level1)
{
    const size_t len = PAGE_SIZE;
    char testChar = 'A';
    int prot = PROT_READ;
    int flags = MAP_SHARED;
    char buf[PAGE_SIZE] = {0};
    char file[] = MMAP_TESTFILE;
    int fd = open(file, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, len);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        /* Only read permit and write data to this area cause process crash */
        mem[0] = testChar;
        mem[1] = testChar + 3;

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
 * @tc.number SUB_KERNEL_MEM_MMAP_1100
 * @tc.name   mmap function file share map and only write permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapFileShareOnlyWrite, Function | MediumTest | Level3)
{
    const size_t len = PAGE_SIZE;
    int prot = PROT_WRITE;
    int flags = MAP_SHARED;
    char buf[PAGE_SIZE] = {0};
    char file[] = MMAP_TESTFILE;

    int fd = open(file, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, len);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        char testChar = 'A';
        mem[0] = testChar;
        mem[1] = testChar + 3;

        LOG("mem[0] = 0x%02x", mem[0]);
        LOG("mem[1] = 0x%02x", mem[1]);
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
 * @tc.number SUB_KERNEL_MEM_MMAP_1200
 * @tc.name   mmap function file private map and execute permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapFilePrivateExec, Function | MediumTest | Level4)
{
    char *mem = nullptr;
    size_t len = PAGE_SIZE / 4;
    int prot = PROT_READ | PROT_EXEC;
    int flags = MAP_PRIVATE;
    int (*fun)(void);
    unsigned long fnReturnFive[] = {0xe52db004, 0xe28db000, 0xe3a03005,
                                    0xe1a00003, 0xe28bd000, 0xe49db004, 0xe12fff1e};
    char file[] = MMAP_TESTFILE;

    int fd = open(file, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, fnReturnFive, sizeof(fnReturnFive));
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    EXPECT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        fun = (int (*)(void))mem;
        int five = fun();
        LOG("five = 0x%02x", five);

        if (five == 0x05) {
            exit(0);
        }
        exit(-1);
    } else {
        WaitProcExitedOK(pid);

        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_1300
 * @tc.name   mmap function file share map and execute permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapFileShareExec, Function | MediumTest | Level4)
{
    char *mem = nullptr;
    size_t len = PAGE_SIZE / 4;
    int prot = PROT_READ | PROT_EXEC;
    int flags = MAP_SHARED;
    int (*fun)(void);
    unsigned long fnReturnFive[] = {0xe52db004, 0xe28db000, 0xe3a03005,
                                    0xe1a00003, 0xe28bd000, 0xe49db004, 0xe12fff1e};
    char file[] = MMAP_TESTFILE;

    int fd = open(file, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, fnReturnFive, sizeof(fnReturnFive));
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    EXPECT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        fun = (int (*)(void))mem;
        int five = fun();
        LOG("five = 0x%02x", five);

        if (five == 0x05) {
            exit(0);
        }
        exit(-1);
    } else {
        WaitProcExitedOK(pid);

        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_1400
 * @tc.name   mmap function file share map and NO execute permission test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapFileShareNoExec, Function | MediumTest | Level2)
{
    char *mem = nullptr;
    size_t len = PAGE_SIZE / 4;
    int prot = PROT_READ;
    int flags = MAP_SHARED;
    int (*fun)(void);
    unsigned long fnReturnFive[] = {0xe52db004, 0xe28db000, 0xe3a03005,
                                    0xe1a00003, 0xe28bd000, 0xe49db004, 0xe12fff1e};
    char file[] = MMAP_TESTFILE;

    int fd = open(file, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, fnReturnFive, sizeof(fnReturnFive));
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    EXPECT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        fun = (int (*)(void))mem;
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
 * @tc.number SUB_KERNEL_MEM_MMAP_1500
 * @tc.name   mmap function overlay map test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapOverlay, Function | MediumTest | Level3)
{
    int i;
    void *addr[3];
    size_t len = 0x00200000;
    unsigned long fixAddr = 0x24000000UL;

    for (i = 0; i < 3; i++) {
        addr[i] = nullptr;
    }

    void *mem = mmap((void *)fixAddr, len, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    LOG("mem = %p, mem + len = %p", mem, (char *)mem + len);
    EXPECT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";
    addr[0] = mem;

    unsigned long before = fixAddr - 0x00100000;
    void *prev = mmap((void *)before, len, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    LOG("prev = %p, prev + len = %p", prev, (char *)prev + len);
    EXPECT_TRUE(prev != MAP_FAILED) << "mem != MAP_FAILED";
    EXPECT_TRUE(prev != mem) << "prev == mem";
    addr[1] = prev;

    unsigned long after = fixAddr + 0x00100000;
    void *next = mmap((void *)after, len, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    LOG("next = %p, next + len = %p", next, (char *)next + len);
    EXPECT_TRUE(next != MAP_FAILED) << "mem != MAP_FAILED";

    for (i = 0; i < 2; i++) {
        if (next == addr[i]) {
            break;
        }
    }
    EXPECT_EQ(i, 2) << "i != 2";
    addr[2] = next;

    size_t small = len - 0x00100000;
    void *belong = mmap((void *)after, small, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    LOG("belong = %p, belong + shinkLen = %p", belong, (char *)belong + small);
    EXPECT_TRUE(belong != MAP_FAILED) << "belong != MAP_FAILED";

    for (i = 0; i < 3; i++) {
        LOG("belong = %p, addr[%d] = %p", belong, i, addr[i]);
        if (belong == addr[i]) {
            break;
        }
    }
    EXPECT_EQ(i, 3) << "i != 3";

    if (belong != MAP_FAILED) {
        EXPECT_TRUE(munmap(belong, small) == 0) << "ERROR: munmap() != 0";
    }

    if (next != MAP_FAILED) {
        EXPECT_TRUE(munmap(next, len) == 0) << "ERROR: munmap() != 0";
    }

    if (prev != MAP_FAILED) {
        EXPECT_TRUE(munmap(prev, len) == 0) << "ERROR: munmap() != 0";
    }

    if (mem != MAP_FAILED) {
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_1600
 * @tc.name   mmap function write back to the file test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapWriteBack, Function | MediumTest | Level3)
{
    char testChar = 'A';
    size_t i, len = PAGE_SIZE;
    int failure = 0;
    int bufSize = len / 4;
    char wBuffer[PAGE_SIZE / 4];
    char rBuffer[PAGE_SIZE / 4];
    char file[] = MMAP_TESTFILE;

    int fd = open(file, O_CREAT | O_RDWR, 0777);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    for (i = 0; i < (size_t)bufSize; i++) {
        rBuffer[i] = 0;
        wBuffer[i] = '0';
    }

    EXPECT_TRUE(write(fd, wBuffer, bufSize) == bufSize) << "ERROR: write() != bufSize";

    void *mem = mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    LOG("TAG1 mem = %p", mem);
    EXPECT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    memset(mem, testChar, len);

    char *ptr = (char *)mem;
    for (i = 0; i < len; i++) {
        if (ptr[i] != testChar) {
            failure = 1;
            break;
        }
    }
    EXPECT_TRUE(failure == 0) << "i = " << i << ", ptr = " << ptr[i];

    EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";

    fd = open(file, O_RDONLY, 0777);
    EXPECT_TRUE(fd != -1) << "ERROR: open() == -1";

    EXPECT_TRUE(read(fd, rBuffer, bufSize) == bufSize) << "ERROR: read() != bufSize";

    failure = 0;
    for (i = 0; i < (size_t)bufSize; i++) {
        if (rBuffer[i] != testChar) {
            failure = 1;
            break;
        }
    }
    EXPECT_TRUE(failure == 0) << "i = " << i << ", rBuffer = " << rBuffer[i];

    EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
    Msleep(1000);
    EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_1700
 * @tc.name   mmap function errno for EACCES test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapEACCES, Function | MediumTest | Level3)
{
    int fd;
    void *mem = nullptr;
    size_t len = PAGE_SIZE;
    char file[] = MMAP_TESTFILE;

    /* file open read only access mode */
    fd = open(file, O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    mem = mmap(nullptr, len, PROT_WRITE, MAP_SHARED, fd, 0);
    EXPECT_TRUE(mem == MAP_FAILED) << "mem != MAP_FAILED, mem = " << mem;
    EXPECT_TRUE(errno == EACCES) << "ERROR: errno != EACCES, errno = " << errno << " EACCES = " << EACCES;

    if (mem != MAP_FAILED) {
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
    EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";

    /* file open append access mode */
    fd = open(file, O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
    EXPECT_TRUE(fd != -1) << "ERROR: open() == -1";

    mem = mmap(nullptr, len, PROT_WRITE, MAP_SHARED, fd, 0);
    EXPECT_TRUE(mem == MAP_FAILED) << "mem != MAP_FAILED, mem = " << mem;
    EXPECT_TRUE(errno == EACCES) << "ERROR: errno != EACCES, errno = " << errno << " EACCES = " << EACCES;

    if (mem != MAP_FAILED) {
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }
    EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
    Msleep(1000);
    EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_1800
 * @tc.name   mmap function errno for EBADF test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapEBADF, Function | MediumTest | Level3)
{
    void *mem = nullptr;
    size_t len = PAGE_SIZE;
    char file[] = MMAP_TESTFILE;
    int flags[] = {MAP_PRIVATE, MAP_SHARED};

    int fd = open(file, O_CREAT | O_RDWR, 0777);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";
    EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";

    for (int i = 0; i < 2; i++) {
        mem = mmap(nullptr, len, PROT_READ | PROT_WRITE, flags[i], fd, 0);
        EXPECT_TRUE(mem == MAP_FAILED) << "mem == MAP_FAILED";
        EXPECT_TRUE(errno == EBADF) << "ERROR: errno != EBADF, errno = " << errno << " EBADF = " << EBADF;

        if (mem != MAP_FAILED) {
            EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
        }
    }

    EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0";
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_1900
 * @tc.name   mmap function errno for EINVAL test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapEINVAL, Function | MediumTest | Level3)
{
    void *mem = nullptr;
    size_t len = PAGE_SIZE;
    char file[] = MMAP_TESTFILE;

    int fd = open(file, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    void *invalueAddr = (void *)(0x21f20000 | 0x123);
    mem = mmap(invalueAddr, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    EXPECT_TRUE(mem == MAP_FAILED) << "mem != MAP_FAILED, mem = " << mem;
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    if (mem != MAP_FAILED) {
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }

    len = 0x40000000;
    mem = mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    EXPECT_TRUE(mem == MAP_FAILED) << "mem != MAP_FAILED, mem = " << mem;
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    if (mem != MAP_FAILED) {
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }

    len = PAGE_SIZE;
    mem = mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xFFFFFFFF);
    EXPECT_TRUE(mem == MAP_FAILED) << "mem != MAP_FAILED, mem = " << mem;
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    if (mem != MAP_FAILED) {
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }

    len = 0;
    mem = mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    EXPECT_TRUE(mem == MAP_FAILED) << "mem != MAP_FAILED, mem = " << mem;
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    if (mem != MAP_FAILED) {
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }

    len = PAGE_SIZE;
    mem = mmap(nullptr, len, 0, MAP_SHARED, fd, 0);
    EXPECT_TRUE(mem != MAP_FAILED) << "mem != MAP_FAILED, mem = " << mem;

    if (mem != MAP_FAILED) {
        EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
    }

    int flags[3] = {0, MAP_ANON, MAP_PRIVATE | MAP_SHARED};

    len = PAGE_SIZE;
    for (int i = 0; i < 3; i++) {
        mem = mmap(nullptr, len, PROT_READ | PROT_WRITE, flags[i], fd, 0);
        EXPECT_TRUE(mem == MAP_FAILED) << "mem != MAP_FAILED, mem = " << mem;
        EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

        if (mem != MAP_FAILED) {
            EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
        }
    }

    EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
    Msleep(1000);
    EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
}

/**
 * @tc.number SUB_KERNEL_MEM_MMAP_2000
 * @tc.name   mmap function errno for ENOMEM test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMmapENOMEM, Function | MediumTest | Level3)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        int i, k, ret;
        void *mem[100];
        size_t len = 0x1000000;
        int flags = MAP_ANONYMOUS | MAP_PRIVATE;

        for (i = 0; i < 100; i++) {
            mem[i] = mmap(nullptr, len, PROT_READ | PROT_WRITE, flags, -1, 0);
            if (mem[i] == MAP_FAILED) {
                LOG("MAP_FAILED: i = %d, errno = %d, ENOMEM = %d", i, errno, ENOMEM);
                break;
            }
        }

        ret = 0;
        if (i == 100 || mem[i] != MAP_FAILED || errno != ENOMEM) {
            ret = 1;
            LOG("ERROR: i = %d, mem[i] = %p, MAP_FAILED = %d, errno = %d", i, mem[i], MAP_FAILED, errno);
        }

        for (k = 0; k < i; k++) {
            munmap(mem[k], len);
        }
        exit(ret);
    } else {
        WaitProcExitedOK(pid);
    }
}


/**
 * @tc.number SUB_KERNEL_MEM_MUNMAP_0100
 * @tc.name   munmap function errno for EINVAL test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MmapApiTest, testMunmapEINVAL, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;

    void *mem = mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    LOG("__LINE__ = %d, mem = %p", __LINE__, mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    EXPECT_TRUE(munmap(NULL, len) == -1) << "ERROR: munmap() != -1";
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    EXPECT_TRUE(munmap(mem, 0) == -1) << "ERROR: munmap() != -1";
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";
}
