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

#define MREMAP_TESTFILE "/storage/testMremap.txt"

class MremapApiTest : public testing::Test {
};

/**
 * @tc.number SUB_KERNEL_MEM_SBRK_0100
 * @tc.name   sbrk function increment 0 test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testSbrkZero, Function | MediumTest | Level2)
{
    void *mem = sbrk(0);
    LOG("mem = %p", mem);
    ASSERT_TRUE(mem != nullptr);
}

/**
 * @tc.number SUB_KERNEL_MEM_SBRK_0200
 * @tc.name   sbrk function errno for ENOMEM test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testSbrkENOMEM, Function | MediumTest | Level3)
{
    void *mem = sbrk(GetRandom(4096));
    LOG("mem = %p, errno = %d", mem, errno);
    ASSERT_TRUE(mem == (void *)-1);
    EXPECT_TRUE(errno == ENOMEM) << "ERROR: errno != ENOMEM, errno = " << errno << " ENOMEM = " << ENOMEM;
}

/**
 * @tc.number SUB_KERNEL_MEM_MREMAP_0100
 * @tc.name   mremap function anonymous remap expand area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testMremapAnonExpand, Function | MediumTest | Level2)
{
    size_t len = PAGE_SIZE;
    size_t expandSize = len * 2;
    char testChar = 'A';
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    LOG("mem = %p", mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    mem = (char *)mremap(mem, len, expandSize, 0);
    LOG("__LINE__ = %d, mem = %p", __LINE__, mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED, expandSize = " << expandSize;

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        mem[0] = testChar;
        mem[len - 1] = testChar + 3;
        /* expand area operate test */
        mem[len + 0] = testChar;
        mem[expandSize - 1] = testChar + 3;

        LOG("mem[0] = 0x%02x", mem[0]);
        LOG("mem[len - 1] = 0x%02x", mem[len - 1]);
        LOG("mem[len + 0] = 0x%02x", mem[len + 0]);
        LOG("mem[expandSize - 1] = 0x%02x", mem[expandSize - 1]);

        exit(0);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, expandSize) == 0) << "ERROR: munmap() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MREMAP_0200
 * @tc.name   mremap function anonymous remap shrink area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testMremapAnonShrink, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE * 2;
    size_t shrinkSize = len / 2;
    char testChar = 'A';
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    LOG("mem = %p", mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    mem = (char *)mremap(mem, len, shrinkSize, 0);
    LOG("__LINE__ = %d, mem = %p", __LINE__, mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED, shrinkSize = " << shrinkSize;

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        mem[0] = testChar;
        mem[shrinkSize - 1] = testChar + 3;
        LOG("mem[0] = 0x%02x", mem[0]);
        LOG("mem[shrinkSize - 1] = 0x%02x", mem[shrinkSize - 1]);
        /* this operate will cause process crash */
        mem[shrinkSize + 4] = testChar;

        exit(0);
    } else {
        ExpectProcCrashed(pid);
        EXPECT_TRUE(munmap(mem, shrinkSize) == 0) << "ERROR: munmap() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MREMAP_0300
 * @tc.name   mremap function anonymous remap and expand area to fix address test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testMremapAnonExpandFixAddr, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    size_t expandSize = len * 2;
    char testChar = 'A';
    char *fixAddr = (char *)0x27000000;
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;
    int reFlag = MREMAP_MAYMOVE | MREMAP_FIXED;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    LOG("mem = %p", mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    mem = (char *)mremap(mem, len, expandSize, reFlag, (void *)fixAddr);
    LOG("__LINE__ = %d, mem = %p, fixAddr = %p", __LINE__, mem, fixAddr);
    ASSERT_TRUE(mem == fixAddr) << "mem != fixAddr";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        fixAddr[0] = testChar;
        fixAddr[len - 1] = testChar + 3;
        /* expand area operate test */
        fixAddr[len + 0] = testChar;
        fixAddr[expandSize - 1] = testChar + 3;

        LOG("fixAddr[0] = 0x%02x", fixAddr[0]);
        LOG("fixAddr[len - 1] = 0x%02x", fixAddr[len - 1]);
        LOG("fixAddr[len + 0] = 0x%02x", fixAddr[len + 0]);
        LOG("fixAddr[expandSize - 1] = 0x%02x", fixAddr[expandSize - 1]);

        exit(0);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, expandSize) == 0) << "ERROR: munmap() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MREMAP_0400
 * @tc.name   mremap function anonymous remap and shrink area to fix address test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testMremapAnonShrinkFixAddr, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE * 2;
    size_t shrinkSize = len / 2;
    char testChar = 'A';
    char *fixAddr = (char *)0x27000000;
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;
    int reFlag = MREMAP_MAYMOVE | MREMAP_FIXED;

    char *mem = (char *)mmap(nullptr, len, prot, flags, -1, 0);
    LOG("mem = %p", mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    mem = (char *)mremap(mem, len, shrinkSize, reFlag, (void *)fixAddr);
    LOG("__LINE__ = %d, mem = %p, fixAddr = %p", __LINE__, mem, fixAddr);
    ASSERT_TRUE(mem == fixAddr) << "mem != fixAddr";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        fixAddr[0] = testChar;
        fixAddr[shrinkSize - 1] = testChar + 3;
        LOG("fixAddr[0] = 0x%02x", fixAddr[0]);
        LOG("fixAddr[shrinkSize - 1] = 0x%02x", fixAddr[shrinkSize - 1]);
        /* this operate will cause process crash */
        fixAddr[shrinkSize + 4] = testChar;

        exit(0);
    } else {
        ExpectProcCrashed(pid);
        EXPECT_TRUE(munmap(mem, shrinkSize) == 0) << "ERROR: munmap() != 0" <<errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MREMAP_0500
 * @tc.name   mremap function file remap expand area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testMremapFileExpand, Function | MediumTest | Level2)
{
    size_t len = PAGE_SIZE;
    size_t expandSize = len * 2;
    char testChar = 'A';
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED;
    char buf[PAGE_SIZE * 2] = {0};
    char file[] = MREMAP_TESTFILE;

    int fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, expandSize);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    LOG("mem = %p", mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    mem = (char *)mremap(mem, len, expandSize, 0);
    LOG("__LINE__ = %d, mem = %p", __LINE__, mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED, expandSize = " << expandSize;

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        mem[0] = testChar;
        mem[len - 1] = testChar + 3;
        /* expand area operate test */
        mem[len + 0] = testChar;
        mem[expandSize - 1] = testChar + 3;
        LOG("mem[0] = 0x%02x", mem[0]);
        LOG("mem[len - 1] = 0x%02x", mem[len - 1]);
        LOG("mem[len + 0] = 0x%02x", mem[len + 0]);
        LOG("mem[expandSize - 1] = 0x%02x", mem[expandSize - 1]);

        exit(0);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, expandSize) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MREMAP_0600
 * @tc.name   mremap function file remap shrink area test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testMremapFileShrink, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE * 2;
    size_t shrinkSize = len / 2;
    char testChar = 'A';
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED;
    char buf[PAGE_SIZE * 2] = {0};
    char file[] = MREMAP_TESTFILE;

    int fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, len);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    LOG("mem = %p", mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    mem = (char *)mremap(mem, len, shrinkSize, 0);
    LOG("__LINE__ = %d, mem = %p", __LINE__, mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED, shrinkSize = " << shrinkSize;

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        mem[0] = testChar;
        mem[shrinkSize - 1] = testChar + 3;
        LOG("mem[0] = 0x%02x", mem[0]);
        LOG("mem[shrinkSize - 1] = 0x%02x", mem[shrinkSize - 1]);

        /* this operate will cause process crash */
        mem[shrinkSize + 4] = testChar;

        exit(0);
    } else {
        ExpectProcCrashed(pid);
        EXPECT_TRUE(munmap(mem, shrinkSize) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MREMAP_0700
 * @tc.name   mremap function file remap and expand area to fix address test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testMremapFileExpandFixAddr, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE;
    size_t expandSize = len * 2;
    char testChar = 'A';
    char *fixAddr = (char *)0x27000000;
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED;
    int reFlag = MREMAP_MAYMOVE | MREMAP_FIXED;
    char buf[PAGE_SIZE * 2] = {0};
    char file[] = MREMAP_TESTFILE;

    int fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, expandSize);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    LOG("mem = %p", mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    mem = (char *)mremap(mem, len, expandSize, reFlag, (void *)fixAddr);
    LOG("__LINE__ = %d, mem = %p, fixAddr = %p", __LINE__, mem, fixAddr);
    ASSERT_TRUE(mem == fixAddr) << "mem != fixAddr";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        fixAddr[0] = testChar;
        fixAddr[len - 1] = testChar + 3;
        /* expand area operate test */
        fixAddr[len + 0] = testChar;
        fixAddr[expandSize - 1] = testChar + 3;

        LOG("fixAddr[0] = 0x%02x", fixAddr[0]);
        LOG("fixAddr[len - 1] = 0x%02x", fixAddr[len - 1]);
        LOG("fixAddr[len + 0] = 0x%02x", fixAddr[len + 0]);
        LOG("fixAddr[expandSize - 1] = 0x%02x", fixAddr[expandSize - 1]);

        exit(0);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_TRUE(munmap(mem, expandSize) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MREMAP_0800
 * @tc.name   mremap function file remap and shrink area to fix address test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testMremapFileShrinkFixAddr, Function | MediumTest | Level3)
{
    size_t len = PAGE_SIZE * 2;
    size_t shrinkSize = len / 2;
    char testChar = 'A';
    char *fixAddr = (char *)0x27000000;
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED;
    int reFlag = MREMAP_MAYMOVE | MREMAP_FIXED;

    char buf[PAGE_SIZE * 2] = {0};
    char file[] = MREMAP_TESTFILE;

    int fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    ASSERT_TRUE(fd != -1) << "ERROR: open() == -1";

    int wByte = write(fd, buf, len);
    EXPECT_TRUE(wByte > 0) << "ERROR: write() <= 0";

    char *mem = (char *)mmap(nullptr, len, prot, flags, fd, 0);
    LOG("mem = %p", mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    mem = (char *)mremap(mem, len, shrinkSize, reFlag, (void *)fixAddr);
    LOG("__LINE__ = %d, mem = %p, fixAddr = %p", __LINE__, mem, fixAddr);
    ASSERT_TRUE(mem == fixAddr) << "mem != fixAddr";

    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        fixAddr[0] = testChar;
        fixAddr[shrinkSize - 1] = testChar + 3;
        LOG("fixAddr[0] = 0x%02x", fixAddr[0]);
        LOG("fixAddr[shrinkSize - 1] = 0x%02x", fixAddr[shrinkSize - 1]);

        /* this operate will cause process crash */
        fixAddr[shrinkSize + 4] = testChar;

        exit(0);
    } else {
        ExpectProcCrashed(pid);

        EXPECT_TRUE(munmap(mem, shrinkSize) == 0) << "ERROR: munmap() != 0";
        EXPECT_TRUE(close(fd) != -1) << "ERROR: close() == -1";
        Msleep(1000);
        EXPECT_TRUE(remove(file) == 0) << "ERROR: remove() != 0" << errno;
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MREMAP_0900
 * @tc.name   mremap function errno for EINVAL test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MremapApiTest, testMremapEINVAL, Function | MediumTest | Level4)
{
    void *mem = nullptr;
    void *newMem = nullptr;
    size_t len = PAGE_SIZE;
    unsigned long fixAddr = 0x27700000;

    mem = mmap((void *)fixAddr, len, PROT_READ | PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
    LOG("__LINE__ = %d, mem = %p", __LINE__, mem);
    ASSERT_TRUE(mem != MAP_FAILED) << "mem == MAP_FAILED";

    fixAddr |= 0x123;
    newMem = mremap((void *)fixAddr, len, len, 0);
    LOG("__LINE__ = %d, newMem = %p", __LINE__, newMem);
    EXPECT_TRUE(newMem == MAP_FAILED) << "mem != MAP_FAILED errno = " << errno;
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    newMem = mremap(mem, len, len, 0x04);
    LOG("__LINE__ = %d, newMem = %p", __LINE__, newMem);
    EXPECT_TRUE(newMem == MAP_FAILED) << "mem != MAP_FAILED errno = " << errno;
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    newMem = mremap(mem, len, 0, 0);
    LOG("__LINE__ = %d, newMem = %p", __LINE__, newMem);
    EXPECT_TRUE(newMem == MAP_FAILED) << "mem != MAP_FAILED errno = " << errno;
    EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

    EXPECT_TRUE(munmap(mem, len) == 0) << "ERROR: munmap() != 0";

    if (newMem != MAP_FAILED && newMem != mem) {
        EXPECT_TRUE(munmap(newMem, len) == 0) << "ERROR: munmap() != 0" << errno;
    }
}
