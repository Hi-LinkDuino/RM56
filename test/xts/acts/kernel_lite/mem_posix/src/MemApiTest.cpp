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
#include <errno.h>
#include <securec.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"
#include "KernelConstants.h"

using namespace testing::ext;

class MemApiTest : public testing::Test {
};

/**
 * @tc.number SUB_KERNEL_MEM_MALLOC_0100
 * @tc.name   malloc function alloc random bytes test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMallocRandom, Function | MediumTest | Level1)
{
    size_t size;
    void *mem = nullptr;

    for (int i=0; i<100; i++) {
        size = GetRandom(0x200000);
        mem = malloc(size);
        ASSERT_TRUE(mem != nullptr) << "mem == nullptr";

        free(mem);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MALLOC_0200
 * @tc.name   malloc function alloc 0 bytes test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMallocZero, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        void *mem = malloc(0);
        free(mem);

        exit(0);
    } else {
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MALLOC_0300
 * @tc.name   malloc function errno for ENOMEM test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMallocENOMEM, Function | MediumTest | Level3)
{
    int i, k;
    void *mem[100] = {nullptr};
    size_t size = 0x08000000;

    for (i = 0; i < 100; i++) {
        mem[i] = malloc(size);
        if (mem[i] == nullptr) {
            LOG("mem[i] = NULL: i = %d, errno = %d, ENOMEM = %d", i, errno, ENOMEM);
            break;
        }
    }

    ASSERT_TRUE(i < 100);
    ASSERT_TRUE(mem[i] == nullptr) << "mem[i] != NULL, i = " << i;
    EXPECT_TRUE(errno == ENOMEM) << "ERROR: errno != ENOMEM, errno = " << errno << " ENOMEM = " << ENOMEM;

    for (k = 0; k < i; k++) {
        free(mem[k]);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_FREE_0100
 * @tc.name   free function ptr is NULL test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testFreeNULL, Function | MediumTest | Level1)
{
    int pid = fork();
    ASSERT_TRUE(pid >= 0) << " fork() < 0";
    if (pid == 0) {
        free(nullptr);
        exit(0);
    } else {
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_REALLOC_0100
 * @tc.name   realloc function realloc memory test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testReallocMem, Function | MediumTest | Level3)
{
    size_t k, len, mlen, rlen;
    void *mem = nullptr;
    char *data = nullptr;
    int i, failure;
    char testChar = 0x36;
    void *memp = nullptr;

    failure = 0;
    for (i = 1; i < 5; i++) {
        mlen = GetRandom(0x200000);
        mem = malloc(mlen);
        ASSERT_TRUE(mem != nullptr) << "mem == NULL";

        memset(mem, testChar, mlen);
        rlen = GetRandom(0x200000);
        memp = mem;
        mem = realloc(mem, rlen);
        if (mem == nullptr) {
            free(memp);
        } else {
            memp = nullptr;
        }
        ASSERT_TRUE(mem != nullptr) << "mem == NULL, i = " << i;

        len = mlen <= rlen ? mlen : rlen;

        data = (char *)mem;
        for (k = 0; k < len; k++) {
            if (data[k] != testChar) {
                failure = 1;
                LOG("ERROR: data[k] != testChar, data[%d] = %d, testChar = %d", k, data[k], testChar);
                break;
            }
        }
        free(mem);

        /* data[k] not equal to testChar */
        if (failure != 0) {
            ADD_FAILURE();
        }
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_REALLOC_0200
 * @tc.name   realloc function parameter is NULL test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testReallocNULL, Function | MediumTest | Level3)
{
    char *mem = nullptr;
    char *prev = nullptr;

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        for (int i = 0; i < 20; i++) {
            size_t len = GetRandom(0x200000);
            /* This call is equivalent to malloc(len) */
            mem = (char *)realloc(nullptr, len);
            prev = mem;
            mem[0] = 0x31;
            free(mem);

            mem = (char *)realloc(nullptr, len);
            if (mem != prev) {
                LOG("mem != prev, mem = %p, prev = %p", mem, prev);
                free(mem);
                exit(1);
            }
            free(mem);
        }
        exit(0);
    } else {
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_REALLOC_0300
 * @tc.name   realloc function errno for ENOMEM test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testReallocENOMEM, Function | MediumTest | Level3)
{
    size_t len = 4096;
    size_t large = 0x80000000;

    void *mem = malloc(len);
    ASSERT_TRUE(mem != nullptr) << "mem == NULL";
    LOG("__LINE__ = %d, mem = %p, errno = %d", __LINE__, mem, errno);

    void *reMem = realloc(mem, large);
    EXPECT_TRUE(reMem == nullptr) << "reMem != NULL, reMem = " << reMem;
    EXPECT_TRUE(errno == ENOMEM) << "ERROR: errno != ENOMEM, errno = " << errno << " ENOMEM = " << ENOMEM;

    if (reMem != nullptr) {
        mem = reMem;
    }
    free(mem);
}

/**
 * @tc.number SUB_KERNEL_MEM_CALLOC_0100
 * @tc.name   calloc function alloc random size memory block test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testCallocRandom, Function | MediumTest | Level3)
{
    char *mem = nullptr;
    int i, sum = 0;
    size_t k, len, nmemb = 32;

    for (i = 0; i < 10; i++) {
        len = GetRandom(64 * 1024);
        mem = (char *)calloc(nmemb, len);
        ASSERT_TRUE(mem != nullptr) << "mem == NULL, i = " << i;

        for (k = 0; k < len * nmemb; k++) {
            sum += mem[k];
        }
        EXPECT_TRUE(sum == 0) << "sum != 0, sum = " << sum;

        free(mem);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_CALLOC_0200
 * @tc.name   calloc function alloc zero size and zero memory block test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testCallocZero, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        struct {
            size_t nmemb;
            size_t len;
        } var[3] = {{32, 0}, {0, GetRandom(4096)}, {0, 0}};

        for (int i=0; i<3; i++) {
            void *mem = calloc(var[i].nmemb, var[i].len);
            free(mem);
        }
        exit(0);
    } else {
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_CALLOC_0300
 * @tc.name   calloc function errno for ENOMEM test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testCallocENOMEM, Function | MediumTest | Level3)
{
    int i, k;
    void *mem[100] = {nullptr};
    size_t nmemb = 128;
    size_t size = 1024 * 1024;

    for (i = 0; i < 100; i++) {
        mem[i] = calloc(nmemb, size);
        if (mem[i] == nullptr) {
            LOG("mem[i] = NULL: i = %d, errno = %d, ENOMEM = %d", i, errno, ENOMEM);
            break;
        }
    }

    ASSERT_TRUE(i < 100);
    ASSERT_TRUE(mem[i] == nullptr) << "mem[i] != NULL, i = " << i;
    EXPECT_TRUE(errno == ENOMEM) << "ERROR: errno != ENOMEM, errno = " << errno << " ENOMEM = " << ENOMEM;

    for (k = 0; k < i; k++) {
        free(mem[k]);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_VALLOC_0100
 * @tc.name   valloc function alloc more than 4096 bytes test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testVallocBytes, Function | MediumTest | Level3)
{
    void *mem = nullptr;
    int pageSize = 0x1000;
    size_t len = 0;

    for (int i = 0; i < 10; i++) {
        len += 0x00100000;
        mem = valloc(len);
        EXPECT_TRUE(mem != nullptr);
        EXPECT_TRUE((((unsigned long)mem) & (pageSize - 1)) == 0);

        free(mem);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_VALLOC_0200
 * @tc.name   valloc function alloc 0 byte test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testVallocZero, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        void *mem = valloc(0);
        free(mem);

        exit(0);
    } else {
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_VALLOC_0300
 * @tc.name   valloc function errno for ENOMEM test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testVallocENOMEM, Function | MediumTest | Level3)
{
    int i, k;
    void *mem[100] = {nullptr};
    size_t size = 0x08000000;

    for (i = 0; i < 100; i++) {
        mem[i] = valloc(size);
        if (mem[i] == nullptr) {
            LOG("mem[i] = NULL: i = %d, errno = %d, ENOMEM = %d", i, errno, ENOMEM);
            break;
        }
    }

    ASSERT_TRUE(i < 100);
    ASSERT_TRUE(mem[i] == nullptr) << "mem != NULL";
    EXPECT_TRUE(errno == ENOMEM) << "ERROR: errno != ENOMEM, errno = " << errno << " ENOMEM = " << ENOMEM;

    for (k = 0; k < i; k++) {
        free(mem[k]);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MEMALIHN_0100
 * @tc.name   memalign function alloc memory for 2 ^ n align test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMemalignTwoAlign, Function | MediumTest | Level2)
{
    void *mem = nullptr;
    int i, align;
    size_t len = 0x1000;

    for (i = 2; i < 21; i++) {
        align = 1 << i;
        mem = memalign(align, len);
        ASSERT_TRUE(mem != nullptr) << "mem == NULL";
        EXPECT_TRUE((((unsigned long)mem) & (align - 1)) == 0);

        free(mem);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MEMALIHN_0200
 * @tc.name   memalign function errno for EINVAL test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMemalignEINVAL, Function | MediumTest | Level3)
{
    void *mem = nullptr;
    int i, align;
    size_t size = 0x1000;

    mem = memalign(0, size);
    EXPECT_TRUE(mem != nullptr) << "mem == nullptr";
    free(mem);

    for (i = 1; i < 10; i++) {
        align = (1 << i) + 1;
        mem = memalign(align, size);
        ASSERT_TRUE(mem == nullptr) << "mem != nullptr";
        EXPECT_TRUE(errno == EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;

        free(mem);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MEMALIHN_0300
 * @tc.name   memalign function errno for ENOMEM test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMemalignENOMEM, Function | MediumTest | Level3)
{
    int i, k;
    int align = 4096;
    void *mem[100] = {nullptr};
    size_t size = 0x08000000;

    for (i = 0; i < 100; i++) {
        mem[i] = memalign(align, size);
        if (mem[i] == nullptr) {
            LOG("mem[i] = NULL: i = %d, errno = %d, ENOMEM = %d", i, errno, ENOMEM);
            break;
        }
    }

    ASSERT_TRUE(i < 100);
    ASSERT_TRUE(mem[i] == nullptr) << "mem[i] != nullptr, i = " << i;
    EXPECT_TRUE(errno == ENOMEM) << "ERROR: errno != ENOMEM, errno = " << errno << " ENOMEM = " << ENOMEM;

    for (k = 0; k < i; k++) {
        free(mem[k]);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_POSIX_MEMALIGN_0100
 * @tc.name   posix_memalign function alloc memory for 2 ^ n align test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testPosixMemalignTwoAlign, Function | MediumTest | Level3)
{
    void *mem = nullptr;
    int i, err;
    size_t align;
    size_t len = GetRandom(4096);

    for (i = 2; i < 21; i++) {
        align = 1 << i;

        if (align % sizeof(void *)) {
            continue;
        }
        err = posix_memalign(&mem, align, len);
        ASSERT_TRUE(err == 0) << "err = " << err << ", i = " << i;
        EXPECT_TRUE((((size_t)(uintptr_t)mem) & (align - 1)) == 0);

        free(mem);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_POSIX_MEMALIGN_0200
 * @tc.name   posix_memalign function alloc 0 byte test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testPosixMemalignZero, Function | MediumTest | Level2)
{
    void *mem = nullptr;
    int align = 1024;

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "Fork Error";
    if (pid == 0) {
        posix_memalign(&mem, align, 0);
        free(mem);
        exit(0);
    } else {
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_POSIX_MEMALIGN_0300
 * @tc.name   posix_memalign function errno for EINVAL test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testPosixMemalignEINVAL, Function | MediumTest | Level3)
{
    void *mem = nullptr;
    int i, align, err;

    err = posix_memalign(&mem, 0, 16);
    EXPECT_TRUE((err != 0) && (err == EINVAL)) << "err = " << err;
    LOG("__LINE__ = %d, ret = %d (0x%08x)", __LINE__, err, err);
    if (err == 0) {
        free(mem);
    }

    align = sizeof(void *) + 1;
    err = posix_memalign(&mem, align, 16);
    ASSERT_TRUE((err != 0) && (err == EINVAL)) << "err = " << err;
    LOG("__LINE__ = %d, ret = %d (0x%08x), align = 0x%08x %d", __LINE__, err, err, align - 1, align);
    free(mem);

    for (i = 1; i < 10; i++) {
        align = (1 << i) + 1;
        err = posix_memalign(&mem, align, 16);
        EXPECT_TRUE((err != 0) && (err == EINVAL)) << "err = " << err;
        if (err == 0) {
            free(mem);
        }
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_POSIX_MEMALIGN_0400
 * @tc.name   posix_memalign function errno for ENOMEM test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testPosixMemalignENOMEM, Function | MediumTest | Level3)
{
    int i, k, err;
    int align = 4096;
    void *mem[100] = {nullptr};
    size_t size = 0x08000000;

    for (i = 0; i < 100; i++) {
        err = posix_memalign(&mem[i], align, size);
        if (mem[i] == nullptr) {
            LOG("mem[i] = NULL: i = %d, errno = %d, ENOMEM = %d", i, errno, ENOMEM);
            break;
        }
    }
    ASSERT_TRUE(i < 100);
    ASSERT_TRUE(mem[i] == nullptr) << "mem[i] != nullptr, i = " << i;
    EXPECT_TRUE((err != 0) && (err == ENOMEM)) << "ERROR: errno != ENOMEM err = " << err;

    for (k = 0; k < i; k++) {
        free(mem[k]);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_OPEN_MEMSTREAM_0100
 * @tc.name   open_memstream function test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testOpenMemstreamBase, Function | MediumTest | Level2)
{
    size_t i, len;
    int failure = 0;
    char *buf = nullptr;
    const char wBuf[] = "A simple string to write";
    char largeBuf[1024];
    const int largeLen = 800;

    FILE *stream = open_memstream(&buf, &len);
    ASSERT_TRUE(stream != nullptr) << "stream == nullptr";
    ASSERT_TRUE(buf != nullptr && len == 0) << "buf == nullptr or len != 0";

    fprintf(stream, wBuf);
    fflush(stream);
    LOG("buf = %s, len = %d, buf[len] = 0x%02x", buf, len, buf[len]);
    LOG("len = %ld, sizeof(wBuf) - 1 = %d", len, sizeof(wBuf) - 1);
    EXPECT_TRUE(len == sizeof(wBuf) - 1) << "len != sizeof (wBuf) - 1";

    for (i = 0; i < len; i++) {
        if (buf[i] != wBuf[i]) {
            failure = 1;
            break;
        }
    }
    EXPECT_TRUE(failure == 0) << "buf[i] != wBuf[i], buf[i] = " << buf[i] << " wBuf[i] = " << wBuf[i];
    EXPECT_TRUE(ftello(stream) == len) << "ftello() != len";
    EXPECT_TRUE(fseeko(stream, 0, SEEK_SET) == 0);
    LOG("buf = %s, len = %d, buf[len] = 0x%02x", buf, len, buf[len]);

    for (i = 0; i < largeLen; i++) {
        largeBuf[i] = 0x36;
    }
    largeBuf[i] = 0;
    fprintf(stream, largeBuf);
    fflush(stream);

    for (i = 0; i < len; i++) {
        if (buf[i] != largeBuf[i]) {
            failure = 1;
            break;
        }
    }
    EXPECT_TRUE(failure == 0) << "buf[i] != largeBuf[i], buf[i] = " << buf[i] << " largeBuf[i] = " << largeBuf[i];
    LOG("buf = %p, len = %d, buf[len] = 0x%02x", buf, len, buf[len]);
    EXPECT_TRUE(fclose(stream) == 0) << "fclose() != 0";
    free(buf);
}

/**
 * @tc.number SUB_KERNEL_MEM_MEMSET_0100
 * @tc.name   memset function set buffer value test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMemset, Function | MediumTest | Level1)
{
    char chr = 'A';
    int i, len, failure;
    len = GetRandom(1024);
    errno_t err = EOK;

    char buf[1024];
    err = memset_s(buf, sizeof(buf), chr, len);
    if(err != EOK) {
        LOG("memset_s failed, err = %d\n", err);
    }
    failure = 0;
    for (i = 0; i < len; i++) {
        if (buf[i] != chr) {
            failure = 1;
            break;
        }
    }
    ASSERT_TRUE(failure == 0) << "buf[i] != chr, buf[i] = " << buf[i] << " chr = " << chr;
}

/**
 * @tc.number SUB_KERNEL_MEM_MEMCPY_0100
 * @tc.name   memcpy function copy buffer test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMemcpy, Function | MediumTest | Level2)
{
    char chr = 'A';
    int i, len, failure;
    char src[1024];
    char dst[1024];

    len = GetRandom(1024);

    for (i = 0; i < len; i++) {
        src[i] = chr + i % 26;
    }

    memcpy(dst, src, len);
    failure = 0;
    for (i = 0; i < len; i++) {
        if (dst[i] != src[i]) {
            failure = 1;
            break;
        }
    }
    ASSERT_TRUE(failure == 0) << "dst[i] != src[i], dst[i] = " << dst[i] << " src[i] = " << src[i];
}

/**
 * @tc.number SUB_KERNEL_MEM_MEMCPY_0200
 * @tc.name   memcpy function overlay copy test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMemcpyOverlay, Function | MediumTest | Level3)
{
    int len;
    char chr = 'A';
    char buf[1024];

    len = sizeof(buf);
    for (int i = 0; i < len; i++) {
        buf[i] = chr + GetRandom(26);
    }

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "Fork Error";

    if (pid == 0) {
        memcpy(&buf[16], &buf[0], len / 2);
        for (int i = 0; i < 16; i++) {
            if (buf[i + 16] != buf[i]) {
                LOG("buf[i + 16] != buf[i], buf[i + 16] = %d, buf[i] = %d", buf[i + 16], buf[i]);
                exit(1);
            }
        }
        exit(0);
    } else {
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_MEM_MEMMOVE_0100
 * @tc.name   memmove function move buffer test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMemmove, Function | MediumTest | Level2)
{
    char chr = 'A';
    char buf[1024];
    int i, len, failure;

    len = sizeof(buf);
    for (i = 0; i < len; i++) {
        buf[i] = chr + GetRandom(26);
    }
    memmove(&buf[0], &buf[len / 2], len / 2);

    failure = 0;
    for (i = 0; i < len / 2; i++) {
        if (buf[i] != buf[len / 2 + i]) {
            failure = 1;
            LOG("buf[i] != buf[len / 2 + i], buf[i] = %d, buf[len / 2 + i] = %d", buf[i], buf[len / 2 + i]);
            break;
        }
    }
    /* buf[i] not equal to buf[len / 2 + i] */
    ASSERT_TRUE(failure == 0);
}

/**
 * @tc.number SUB_KERNEL_MEM_MEMMOVE_0200
 * @tc.name   memmove function overlay move buffer test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMemmoveOverlay, Function | MediumTest | Level3)
{
    char chr = 'A';
    char buf[1024];
    char backup[1024];
    int i, len, failure;

    len = sizeof(buf);
    for (i = 0; i < len; i++) {
        buf[i] = chr + GetRandom(26);
        backup[i] = buf[i];
    }
    memmove(&buf[16], &buf[0], len / 2);

    failure = 0;
    for (i = 0; i < len / 2; i++) {
        if (buf[i + 16] != backup[i]) {
            failure = 1;
            LOG("buf[i + 16] != backup[i], buf[i + 16] = %d, backup[i] = %d", buf[i + 16], backup[i]);
            break;
        }
    }
    ASSERT_TRUE(failure == 0) << "buf[i + 16] != backup[i]";
}


/**
 * @tc.number SUB_KERNEL_MEM_MEMCMP_0100
 * @tc.name   memmove function move buffer test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMemcmp, Function | MediumTest | Level2)
{
    char orign[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    char lt[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x77};
    char eq[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    char gt[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x99};

    int ret;
    int len = sizeof(orign);

    ret = memcmp(lt, orign, len);
    ASSERT_TRUE(ret < 0);

    ret = memcmp(eq, orign, len);
    ASSERT_TRUE(ret == 0);

    ret = memcmp(gt, orign, len);
    ASSERT_TRUE(ret > 0);

    ret = memcmp(gt, orign, 0);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.number SUB_KERNEL_MEM_MEMRCHR_0100
 * @tc.name   memrchr function find the last value in the string test
 * @tc.desc   [C-L*-311] MUST NOT alter NDK API behavior.
 */
HWTEST_F(MemApiTest, testMemrchr, Function | MediumTest | Level2)
{
    char orign[] = "This is test string";
    int len = sizeof(orign);

    /* add the terminal characteric for the string */
    orign[4] = '\0';
    orign[7] = '\0';

    char *last = (char *)memrchr(orign, 'k', len);
    ASSERT_TRUE(last == nullptr);

    char *first = (char *)memchr(orign, 's', len);
    ASSERT_TRUE(first == &orign[3]);

    last = (char *)memrchr(orign, 's', len);
    ASSERT_TRUE(last == &orign[13]);
}

