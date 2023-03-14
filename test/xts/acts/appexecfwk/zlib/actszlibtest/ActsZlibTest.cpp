/**
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "zlib.h"

using namespace std;
using namespace testing::ext;
namespace {
static const char DICTIONARY[] = "hello";
static const char GARBAGE[] = "garbage";
static const char TESTFILE[] = "foo.gz";
static char HELLO[] = "hello, hello!";
static unsigned int CALLOC_SIZE = 1;
static int ONE = 1;
static int FOUR = 4;
static int SIX = 6;
static int EIGHT = 8;
static unsigned BUFFER_SIZE = 8192;
}

static unsigned pull(void *desc, unsigned char **buf)
{
    static unsigned int next = 0;
    static unsigned char dat[] = {0x63, 0, 2, 0};

    if (!desc) {
        next = 0;
        return 0;   /* no input (already provided at next_in) */
    }
    return next < sizeof(dat) ? (*buf = dat + next++, 1) : 0;
}

static int push(void *desc, unsigned char *buf, unsigned len)
{
    buf += len;
    return desc != nullptr;      /* force error if desc not null */
}

static int TestGzPrintf(gzFile file, const char *format, ...)
{
    va_list va;
    int ret;

    va_start(va, format);
    ret = gzvprintf(file, format, va);
    va_end(va);
    return ret;
}

class ActsZlibTest : public testing::Test {
protected:
    ActsZlibTest();
    ~ActsZlibTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
};

ActsZlibTest::ActsZlibTest()
{}

ActsZlibTest::~ActsZlibTest()
{}

void ActsZlibTest::SetUpTestCase()
{}

void ActsZlibTest::TearDownTestCase()
{}

/* these items are strung together in a linked list, one for each allocation */
struct mem_item {
    void *ptr;                  /* pointer to allocated memory */
    size_t size;                /* requested size of allocation */
    struct mem_item *next;      /* pointer to next item in list, or NULL */
};

/* this structure is at the root of the linked list, and tracks statistics */
struct mem_zone {
    struct mem_item *first;     /* pointer to first item in list, or NULL */
    size_t total, highwater;    /* total allocations, and largest total */
    size_t limit;               /* memory allocation limit, or 0 if no limit */
    int notlifo, rogue;         /* counts of non-LIFO frees and rogue frees */
};

/**
 * @tc.number    : ActsZlibTest_0100
 * @tc.name      : Test compress and uncompress test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestCompress, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestCompress Z_SOLO**********\n");
#else
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    uLong len = static_cast<uLong>(strlen(HELLO)) + 1;
    err = compress(compr, &comprLen, reinterpret_cast<const Bytef*>(HELLO), len);
    fprintf(stderr, "compress error: %d\n", err);
    ASSERT_EQ(err, Z_OK);

    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    err = uncompress(uncompr, &uncomprLen, compr, comprLen);
    fprintf(stderr, "uncompress error: %d\n", err);
    ASSERT_EQ(err, Z_OK);
    fprintf(stderr, "uncompress: %s\n", reinterpret_cast<char *>(uncompr));
    free(compr);
    free(uncompr);
#endif
}

/**
 * @tc.number    : ActsZlibTest_0200
 * @tc.name      : Test gzio
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzio, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzio Z_SOLO**********\n");
#else
    int err = Z_OK;
    int len = static_cast<int>(strlen(HELLO)) + 1;
    gzFile file;
    z_off_t pos;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);

    gzputc(file, 'h');
    ASSERT_TRUE(gzputs(file, "ello") == FOUR);
    if (gzprintf(file, ", %s!", "hello") != EIGHT) {
        fprintf(stderr, "gzprintf err: %s\n", gzerror(file, &err));
        ASSERT_TRUE(false);
    }

    gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
    gzclose(file);
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);

    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    ASSERT_TRUE(gzread(file, uncompr, static_cast<unsigned>(uncomprLen)) == len);
    ASSERT_FALSE(strcmp(reinterpret_cast<char *>(uncompr), HELLO));

    pos = gzseek(file, -8L, SEEK_CUR);
    ASSERT_FALSE(pos != SIX || gztell(file) != pos);
    ASSERT_FALSE(gzgetc(file) != ' ');
    ASSERT_FALSE(gzungetc(' ', file) != ' ');

    fprintf(stderr, "gzgets\n");
    gzgets(file, reinterpret_cast<char *>(uncompr), static_cast<int>(uncomprLen));
    ASSERT_FALSE(strcmp(reinterpret_cast<char *>(uncompr), HELLO + SIX));
    gzclose(file);
    free(compr);
    free(uncompr);
#endif
}

/**
 * @tc.number    : ActsZlibTest_0300
 * @tc.name      : Test deflate
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestDeflate, Function | MediumTest | Level2)
{
    Byte *compr;
    uLong comprLen = 10000 * sizeof(int);
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL);

    z_stream c_stream; /* compression stream */
    int err = Z_OK;
    uLong len = static_cast<uLong>(strlen(HELLO)) + 1;
    c_stream.zalloc = nullptr;
    c_stream.zfree = nullptr;
    c_stream.opaque = nullptr;
    err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
    fprintf(stderr, "deflateInit result: %d\n", err);
    ASSERT_EQ(err, Z_OK);

    c_stream.next_in  = reinterpret_cast<z_const unsigned char *>(HELLO);
    c_stream.next_out = compr;
    fprintf(stderr, "deflate\n");
    while (c_stream.total_in != len && c_stream.total_out < comprLen) {
        c_stream.avail_in = c_stream.avail_out = 1; /* force small buffers */
        err = deflate(&c_stream, Z_NO_FLUSH);
        fprintf(stderr, "deflate result: %d\n", err);
        ASSERT_EQ(err, Z_OK);
    }

    for (;;) {
        c_stream.avail_out = 1;
        err = deflate(&c_stream, Z_FINISH);
        fprintf(stderr, "deflate result: %d\n", err);
        if (err == Z_STREAM_END) {
            break;
        }
        ASSERT_EQ(err, Z_OK);
    }

    err = deflateEnd(&c_stream);
    fprintf(stderr, "deflateEnd result: %d\n", err);
    ASSERT_EQ(err, Z_OK);
    free(compr);
}

/**
 * @tc.number    : ActsZlibTest_0400
 * @tc.name      : Test inflate
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflate, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = 0;
    d_stream.next_out = uncompr;
    err = inflateInit(&d_stream);
    fprintf(stderr, "inflateInit result: %d\n", err);
    ASSERT_EQ(err, Z_OK);

    while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END || err == Z_DATA_ERROR) {
            break;
        }
        fprintf(stderr, "inflate result: %d\n", err);
        ASSERT_EQ(err, Z_OK);
    }

    fprintf(stderr, "inflateEnd result: %d\n", inflateEnd(&d_stream));
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_0500
 * @tc.name      : Test deflate with large buffers and dynamic change of compression level
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestLargeDeflate, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    z_stream c_stream; /* compression stream */
    int err = Z_OK;
    c_stream.zalloc = nullptr;
    c_stream.zfree = nullptr;
    c_stream.opaque = nullptr;

    err = deflateInit(&c_stream, Z_BEST_SPEED);
    fprintf(stderr, "deflateInit result: %d\n", err);
    ASSERT_EQ(err, Z_OK);

    c_stream.next_out = compr;
    c_stream.avail_out = static_cast<uInt>(comprLen);

    /* At this point, uncompr is still mostly zeroes, so it should compress
     * very well:
     */
    c_stream.next_in = uncompr;
    c_stream.avail_in = static_cast<uInt>(uncomprLen);
    err = deflate(&c_stream, Z_NO_FLUSH);
    fprintf(stderr, "deflate result: %d\n", err);
    ASSERT_EQ(err, Z_OK);
    ASSERT_TRUE(!c_stream.avail_in);

    /* Feed in already compressed data and switch to no compression: */
    deflateParams(&c_stream, Z_NO_COMPRESSION, Z_DEFAULT_STRATEGY);
    c_stream.next_in = compr;
    c_stream.avail_in = static_cast<uInt>(comprLen) / 2;
    err = deflate(&c_stream, Z_NO_FLUSH);
    fprintf(stderr, "deflate result: %d\n", err);
    ASSERT_EQ(err, Z_OK);

    /* Switch back to compressing mode: */
    deflateParams(&c_stream, Z_BEST_COMPRESSION, Z_FILTERED);
    c_stream.next_in = uncompr;
    c_stream.avail_in = static_cast<uInt>(uncomprLen);
    err = deflate(&c_stream, Z_NO_FLUSH);
    ASSERT_EQ(err, Z_OK);

    err = deflate(&c_stream, Z_FINISH);
    ASSERT_EQ(err, Z_STREAM_END);

    err = deflateEnd(&c_stream);
    ASSERT_EQ(err, Z_OK);
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_0600
 * @tc.name      : Test inflate with large buffers
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestLargeInflate, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = static_cast<uInt>(comprLen);
    err = inflateInit(&d_stream);
    ASSERT_EQ(err, Z_OK);

    for (;;) {
        d_stream.next_out = uncompr;            /* discard the output */
        d_stream.avail_out = static_cast<uInt>(uncomprLen);
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END || err == Z_DATA_ERROR) {
            break;
        }
        ASSERT_EQ(err, Z_OK);
    }

    err = inflateEnd(&d_stream);
    ASSERT_EQ(err, Z_OK);
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_0700
 * @tc.name      : Test deflate with full flush
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestFlush, Function | MediumTest | Level2)
{
    Byte *compr;
    uLong comprLen = 10000 * sizeof(int);
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL);

    z_stream c_stream; /* compression stream */
    int err = Z_OK;
    uInt len = static_cast<uInt>(strlen(HELLO)) + 1;
    c_stream.zalloc = nullptr;
    c_stream.zfree = nullptr;
    c_stream.opaque = nullptr;
    err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
    ASSERT_EQ(err, Z_OK);

    c_stream.next_in  = reinterpret_cast<z_const unsigned char *>(HELLO);
    c_stream.next_out = compr;
    c_stream.avail_in = 3;
    c_stream.avail_out = static_cast<uInt>(comprLen);
    err = deflate(&c_stream, Z_FULL_FLUSH);
    ASSERT_EQ(err, Z_OK);

    compr[3]++; /* force an error in first compressed block */
    c_stream.avail_in = len - 3;
    err = deflate(&c_stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        ASSERT_EQ(err, Z_OK);
    }

    err = deflateEnd(&c_stream);
    ASSERT_EQ(err, Z_OK);
    comprLen = c_stream.total_out;
    free(compr);
}

/**
 * @tc.number    : ActsZlibTest_0800
 * @tc.name      : Test inflateSync
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestSync, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = 2; /* just read the zlib header */
    err = inflateInit(&d_stream);
    ASSERT_EQ(err, Z_OK);

    d_stream.next_out = uncompr;
    d_stream.avail_out = static_cast<uInt>(uncomprLen);

    inflate(&d_stream, Z_NO_FLUSH);
    d_stream.avail_in = static_cast<uInt>(comprLen) - 2;   /* read all compressed data */
    inflateSync(&d_stream);
    inflate(&d_stream, Z_FINISH);
    inflateEnd(&d_stream);
    printf("after inflateSync: hel%s\n", reinterpret_cast<char *>(uncompr));
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_0900
 * @tc.name      : Test deflate with preset dictionary
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestDictDeflate, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    z_stream c_stream; /* compression stream */
    int err = Z_OK;
    c_stream.zalloc = nullptr;
    c_stream.zfree = nullptr;
    c_stream.opaque = nullptr;
    err = deflateInit(&c_stream, Z_BEST_COMPRESSION);
    ASSERT_EQ(err, Z_OK);

    err = deflateSetDictionary(&c_stream,
                reinterpret_cast<const Bytef*>(DICTIONARY), static_cast<int>(sizeof(DICTIONARY)));
    ASSERT_EQ(err, Z_OK);

    c_stream.next_out = compr;
    c_stream.avail_out = static_cast<uInt>(comprLen);
    c_stream.next_in  = reinterpret_cast<z_const unsigned char *>(HELLO);
    c_stream.avail_in = static_cast<uInt>(strlen(HELLO)) + 1;
    err = deflate(&c_stream, Z_FINISH);
    ASSERT_EQ(err, Z_STREAM_END);

    err = deflateEnd(&c_stream);
    ASSERT_EQ(err, Z_OK);
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_1000
 * @tc.name      : Test inflate with a preset dictionary
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestDictInflate, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = static_cast<uInt>(comprLen);
    err = inflateInit(&d_stream);
    ASSERT_EQ(err, Z_OK);
    d_stream.next_out = uncompr;
    d_stream.avail_out = static_cast<uInt>(uncomprLen);
    for (;;) {
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) {
            break;
        }
        if (err == Z_NEED_DICT) {
            err = inflateSetDictionary(
                &d_stream, reinterpret_cast<const Bytef*>(DICTIONARY), static_cast<int>(sizeof(DICTIONARY)));
        }
        if (err == Z_DATA_ERROR) {
            break;
        }
        ASSERT_EQ(err, Z_OK);
    }

    err = inflateEnd(&d_stream);
    ASSERT_EQ(err, Z_OK);
    if (strcmp(reinterpret_cast<char *>(uncompr), HELLO)) {
        fprintf(stderr, "bad inflate with dict\n");
    } else {
        printf("inflate with dictionary: %s\n", reinterpret_cast<char *>(uncompr));
    }

    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_1100
 * @tc.name      : Test compress2 with Z_BEST_COMPRESSION level
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestCompress2, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestCompress2 Z_BEST_COMPRESSION Z_SOLO**********\n");
#else
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    uLong len = static_cast<uLong>(strlen(HELLO)) + 1;
    uLong outLen = compressBound(len);
    fprintf(stderr, "compressBound result: %lu\n", outLen);
    err = compress2(compr, &comprLen, reinterpret_cast<const Bytef*>(HELLO), outLen, Z_BEST_COMPRESSION);
    fprintf(stderr, "compress2 Z_BEST_COMPRESSION result: %d\n", err);
    ASSERT_EQ(err, Z_OK);

    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    err = uncompress2(uncompr, &uncomprLen, compr, &comprLen);
    fprintf(stderr, "uncompress2 Z_BEST_COMPRESSION result: %d\n", err);
    ASSERT_EQ(err, Z_OK);
    fprintf(stderr, "uncompress2: %s\n", reinterpret_cast<char *>(uncompr));
    free(compr);
    free(uncompr);
#endif
}

/**
 * @tc.number    : ActsZlibTest_1200
 * @tc.name      : Test adler32
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestAdler, Function | MediumTest | Level2)
{
    uLong err = Z_ERRNO;
    uLong adler1 = 0L;
    uLong adler2 = 0L;
    const Bytef *buf = reinterpret_cast<const Bytef*>(DICTIONARY);
    err = adler32(0L, buf, 0);
    fprintf(stderr, "adler32 result: %lu\n", err);
    ASSERT_NE(err, Z_ERRNO);

    err = adler32_z(0L, buf, 0);
    fprintf(stderr, "adler32_z result: %lu\n", err);
    ASSERT_NE(err, Z_ERRNO);
#ifdef Z_SOLO
#ifndef Z_LARGE64
    err = adler32_combine64(adler1, adler2, 0);
    fprintf(stderr, "adler32_combine64 result: %lu\n", err);
    ASSERT_NE(err, Z_ERRNO);
#endif
#else
    err = adler32_combine(adler1, adler2, 0);
    fprintf(stderr, "adler32_combine result: %lu\n", err);
    ASSERT_NE(err, Z_ERRNO);
#endif
}

/**
 * @tc.number    : ActsZlibTest_1300
 * @tc.name      : Test deflate state
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestDeflateState, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    int *bits = nullptr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    gz_headerp headerp = nullptr;
    z_stream c_stream; /* compression stream */
    int err = Z_OK;
    int windowBits = EIGHT;
    int memLevel = EIGHT;
    c_stream.zalloc = nullptr;
    c_stream.zfree = nullptr;
    c_stream.opaque = nullptr;
    err = deflateInit2(
         &c_stream, Z_BEST_COMPRESSION, Z_DEFLATED, windowBits, memLevel, Z_FILTERED);
    ASSERT_EQ(err, Z_OK);
    deflateSetHeader(&c_stream, headerp);
    deflateTune(&c_stream, ONE, FOUR, EIGHT, ONE);
    memLevel = ONE;
    err = deflateParams(&c_stream, memLevel, Z_DEFAULT_STRATEGY);
    fprintf(stderr, "deflateParams result: %d\n", err);
    ASSERT_EQ(err, Z_OK);

    err = deflatePending(&c_stream, nullptr, bits);
    fprintf(stderr, "deflatePending result: %d\n", err);
    ASSERT_EQ(err, Z_OK);

    err = deflateSetDictionary(&c_stream,
                reinterpret_cast<const Bytef*>(DICTIONARY), static_cast<int>(sizeof(DICTIONARY)));
    fprintf(stderr, "deflateGetDictionary result: %d\n", err);
    ASSERT_EQ(err, Z_OK);

    err = deflateGetDictionary(&c_stream, uncompr, nullptr);
    fprintf(stderr, "deflateGetDictionary result: %d\n", err);
    err = deflatePrime(&c_stream, EIGHT, ONE);
    fprintf(stderr, "deflatePrime result: %d\n", err);
    c_stream.next_out = compr;
    c_stream.avail_out = static_cast<uInt>(comprLen);
    c_stream.next_in  = reinterpret_cast<z_const unsigned char *>(HELLO);
    c_stream.avail_in = static_cast<uInt>(strlen(HELLO)) + 1;
    err = deflate(&c_stream, Z_FINISH);
    ASSERT_EQ(err, Z_STREAM_END);
    err = deflateEnd(&c_stream);
    ASSERT_EQ(err, Z_OK);
#ifdef Z_SOLO
    err = deflateResetKeep(&c_stream);
    fprintf(stderr, "deflateReset result: %d\n", err);
#else
    err = deflateReset(&c_stream);
    fprintf(stderr, "deflateReset result: %d\n", err);
#endif
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_1400
 * @tc.name      : Test deflateBound
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestDeflateBound, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    z_stream defstream;
    char *inBuf = reinterpret_cast<char *>(HELLO);
    uint32_t inLen = strlen(inBuf) + 1;
    uint8_t *outBuf = nullptr;
    uint32_t outLen = 0;
    int err = Z_OK;

    defstream.zalloc = nullptr;
    defstream.zfree = nullptr;
    defstream.opaque = nullptr;
    defstream.avail_in = static_cast<uInt>(inLen);
    defstream.next_in = reinterpret_cast<Bytef *>(inBuf);
    defstream.avail_out = static_cast<uInt>(outLen);
    defstream.next_out = reinterpret_cast<Bytef *>(outBuf);
    err = deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
    fprintf(stderr, "deflateInit result: %d\n", err);
    ASSERT_EQ(err, Z_OK);
    uint32_t  estimateLen = deflateBound(&defstream, inLen);
    outBuf = reinterpret_cast<uint8_t *>(malloc(estimateLen));
    defstream.avail_out = static_cast<uInt>(estimateLen);
    deflate(&defstream, Z_FINISH);
    deflateEnd(&defstream);
    z_stream outStream;
    err = deflateCopy(&defstream, &outStream);
    fprintf(stderr, "deflateCopy result: %d\n", err);
    free(inBuf);
    free(outBuf);
#endif
}

/**
 * @tc.number    : ActsZlibTest_1500
 * @tc.name      : Test adler32
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestCRC, Function | MediumTest | Level2)
{
    uLong err = Z_ERRNO;
    uLong crc1 = 0L;
    uLong crc2 = 0L;
    const Bytef *buf = reinterpret_cast<const Bytef*>(DICTIONARY);
    err = crc32(0L, buf, 0);
    fprintf(stderr, "crc32 result: %lu\n", err);
    ASSERT_NE(err, Z_ERRNO);

    err = crc32_z(0L, buf, 0);
    fprintf(stderr, "crc32_z result: %lu\n", err);
    ASSERT_NE(err, Z_ERRNO);
#ifdef Z_SOLO
#ifndef Z_LARGE64
    err = crc32_combine64(crc1, crc2, 0);
    fprintf(stderr, "crc32_combine64 result: %lu\n", err);
    ASSERT_NE(err, Z_ERRNO);
#endif
#else
    err = adler32_combine(crc1, crc2, 0);
    fprintf(stderr, "crc32_combine result: %lu\n", err);
    ASSERT_NE(err, Z_ERRNO);
#endif
}

/**
 * @tc.number    : ActsZlibTest_1600
 * @tc.name      : Test get_crc_table
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGetCrcTable, Function | MediumTest | Level2)
{
    auto table = get_crc_table();
    ASSERT_TRUE(table != nullptr);
}

/**
 * @tc.number    : ActsZlibTest_1700
 * @tc.name      : Test gzBuffer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzBuffer, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzBuffer Z_SOLO**********\n");
#else
    int err = Z_OK;
    int len = static_cast<int>(strlen(HELLO)) + 1;
    gzFile file;
    z_off_t pos;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);

    err = gzbuffer(file, BUFFER_SIZE);
    ASSERT_EQ(err, Z_OK);

    gzclearerr(file);
    gzputc(file, 'h');
    ASSERT_TRUE(gzputs(file, "ello") == FOUR);
    if (gzprintf(file, ", %s!", "hello") != EIGHT) {
        fprintf(stderr, "gzprintf err: %s\n", gzerror(file, &err));
        ASSERT_TRUE(false);
    }

    gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
    gzclearerr(file);
    gzclose_w(file);
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);

    int res = gzdirect(file);
    fprintf(stderr, "gzdirect result: %d\n", res);
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    ASSERT_TRUE(gzread(file, uncompr, static_cast<unsigned>(uncomprLen)) == len);
    ASSERT_FALSE(strcmp(reinterpret_cast<char *>(uncompr), HELLO));

    pos = gzseek(file, -8L, SEEK_CUR);
    ASSERT_FALSE(pos != SIX || gztell(file) != pos);
    ASSERT_FALSE(gzgetc(file) != ' ');
    ASSERT_FALSE(gzungetc(' ', file) != ' ');

    fprintf(stderr, "gzgets\n");
    gzgets(file, reinterpret_cast<char *>(uncompr), static_cast<int>(uncomprLen));
    ASSERT_FALSE(strcmp(reinterpret_cast<char *>(uncompr), HELLO + SIX));
    gzclose_r(file);
    free(compr);
    free(uncompr);
#endif
}

/**
 * @tc.number    : ActsZlibTest_1800
 * @tc.name      : Test gzflush
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzFlush, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzFlush Z_SOLO**********\n");
#else
    int err = Z_OK;
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);

    gzputc(file, 'h');
    ASSERT_TRUE(gzputs(file, "ello") == FOUR);
    if (gzprintf(file, ", %s!", "hello") != EIGHT) {
        fprintf(stderr, "gzprintf err: %s\n", gzerror(file, &err));
        ASSERT_TRUE(false);
    }
    gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
    gzflush(file, Z_FINISH); /* add one zero byte */
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_1900
 * @tc.name      : Test gzfread
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzFread, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzFread Z_SOLO**********\n");
#else
    int err = Z_OK;
    int len = static_cast<int>(strlen(HELLO)) + 1;
    gzFile file;
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);
    err = gzfread(HELLO, len, len, file);
    ASSERT_EQ(err, 1);
    gzclose(file);

#endif
}

/**
 * @tc.number    : ActsZlibTest_2000
 * @tc.name      : Test gzfwrite
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzWrite, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzWrite Z_SOLO**********\n");
#else
    int err = Z_OK;
    int len = static_cast<int>(strlen(HELLO)) + 1;
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    err = gzfwrite(HELLO, len, len, file);
    ASSERT_EQ(err, len);
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_2100
 * @tc.name      : Test gzgetc
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzGetc, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzGetc Z_SOLO**********\n");
#else
    int err = Z_OK;
    gzFile file;
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);
    err = gzgetc(file);
    ASSERT_TRUE(err == 'h');
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_2200
 * @tc.name      : Test gzgetc_
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzGetc_, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzGetc_ Z_SOLO**********\n");
#else
    int err = Z_OK;
    gzFile file;
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);
    err = gzgetc_(file);
    ASSERT_TRUE(err == 'h');
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_2300
 * @tc.name      : Test gzgets
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzGets, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzGets Z_SOLO**********\n");
#else
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    Byte *uncompr;
    uLong uncomprLen = 10000 * sizeof(int);
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    fprintf(stderr, "gzgets\n");
    gzgets(file, reinterpret_cast<char *>(uncompr), static_cast<int>(uncomprLen));
    ASSERT_TRUE(strcmp(reinterpret_cast<char *>(uncompr), HELLO + SIX));
    gzclose(file);
    free(uncompr);
#endif
}

/**
 * @tc.number    : ActsZlibTest_2400
 * @tc.name      : Test gzoffset64
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzOffset64, Function | MediumTest | Level2)
{
#ifndef Z_LARGE64
    fprintf(stderr, "*********ActsZlibTestGzOffset64 Z_LARGE64**********\n");
#else
    int err = Z_OK;
    int len = static_cast<int>(strlen(HELLO)) + 1;
    gzFile file;
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);
    err = gzoffset64(file);
    ASSERT_TRUE(err != Z_OK);
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_2500
 * @tc.name      : Test gzopen and gzopen64 and gzopen_w
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzOpen, Function | MediumTest | Level2)
{
#ifndef Z_SOLO
    int err = Z_OK;
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    gzputc(file, 'h');
    ASSERT_TRUE(gzputs(file, "ello") == FOUR);
    if (gzprintf(file, ", %s!", "hello") != EIGHT) {
        fprintf(stderr, "gzprintf err: %s\n", gzerror(file, &err));
        ASSERT_TRUE(false);
    }
    gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
    gzclose(file);
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);
    gzclose(file);
#endif
#ifdef Z_LARGE64
    int err = Z_OK;
    gzFile file;
    file = gzopen64(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    gzputc(file, 'h');
    ASSERT_TRUE(gzputs(file, "ello") == FOUR);
    if (gzprintf(file, ", %s!", "hello") != EIGHT) {
        fprintf(stderr, "gzprintf err: %s\n", gzerror(file, &err));
        ASSERT_TRUE(false);
    }
    gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
    gzclose(file);
    file = gzopen64(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);
    gzclose(file);
#endif
#if (defined(_WIN32) || defined(__CYGWIN__)) && !defined(Z_SOLO)
    gzFile file;
    file = gzopen_w(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_2600
 * @tc.name      : Test gzprintf
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzPrintf, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzPrintf Z_SOLO**********\n");
#else
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    ASSERT_TRUE(gzprintf(file, ", %s!", "hello") == EIGHT);
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_2700
 * @tc.name      : Test gzputc
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzPutc, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzPutc Z_SOLO**********\n");
#else
    char err;
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    err = gzputc(file, 'h');
    ASSERT_TRUE(err == 'h');
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_2800
 * @tc.name      : Test gzputs
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzPuts, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzPuts Z_SOLO**********\n");
#else
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    ASSERT_TRUE(gzputs(file, "ello") == FOUR);
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_2900
 * @tc.name      : Test gzread
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzRead, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzRead Z_SOLO**********\n");
#else
    int err = Z_OK;
    int len = static_cast<int>(strlen(HELLO)) + 1;
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);

    gzputc(file, 'h');
    ASSERT_TRUE(gzputs(file, "ello") == FOUR);
    if (gzprintf(file, ", %s!", "hello") != EIGHT) {
        fprintf(stderr, "gzprintf err: %s\n", gzerror(file, &err));
        ASSERT_TRUE(false);
    }

    gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
    gzclose(file);
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);

    Byte *uncompr;
    uLong uncomprLen = 10000 * sizeof(int);
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(uncompr != Z_NULL);

    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    ASSERT_TRUE(gzread(file, uncompr, static_cast<unsigned>(uncomprLen)) == len);
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_3000
 * @tc.name      : Test gzrewind
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzRewind, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzRewind Z_SOLO**********\n");
#else
    int err = Z_OK;
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    gzseek(file, 0L, SEEK_SET);
    err = gzrewind(file);
    ASSERT_TRUE(err == -1);
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_3100
 * @tc.name      : Test gzseek and gzseek64
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzseek, Function | MediumTest | Level2)
{
    long err = 0L;
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzseek Z_SOLO**********\n");
#else
    err = gzseek(file, 1L, SEEK_CUR);
#endif
#ifdef Z_LARGE64
    err = gzseek64(file, 1L, SEEK_CUR);
#endif
    ASSERT_TRUE(err == 1L);
    gzclose(file);
}

/**
 * @tc.number    : ActsZlibTest_3200
 * @tc.name      : Test gzsetparams
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzSetParams, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzSetParams Z_SOLO**********\n");
#else
    int err = Z_OK;
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    err = gzsetparams(file, Z_DEFAULT_COMPRESSION, Z_DEFAULT_STRATEGY);
    ASSERT_TRUE(err == Z_OK);
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_3300
 * @tc.name      : Test gztell and gztell64
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzTell, Function | MediumTest | Level2)
{
#  ifndef Z_LARGE64
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
    gzclose(file);
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);
    z_off64_t pos;
    pos = gzseek(file, -8L, SEEK_CUR);
    ASSERT_FALSE(gztell(file) == pos); /* define gztell gztell64 in zlib.h */
    gzclose(file);
#else
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
    gzclose(file);
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);
    z_off_t pos;
    pos = gzseek(file, -8L, SEEK_CUR);
    ASSERT_FALSE(pos != SIX || gztell(file) != pos);
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_3400
 * @tc.name      : Test gzungetc
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzUnGetc, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzUnGetc Z_SOLO**********\n");
#else
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
    gzclose(file);
    file = gzopen(TESTFILE, "rb");
    ASSERT_TRUE(file != NULL);
    ASSERT_FALSE(gzungetc(' ', file) != ' ');
#endif
}

/**
 * @tc.number    : ActsZlibTest_3500
 * @tc.name      : Test gzvprintf
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzVprintf, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzVprintf Z_SOLO**********\n");
#else
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);

    int err = TestGzPrintf(file, ", %s!", "hello");
    fprintf(stderr, "gzvprintf result: %d\n", err);
#endif
}

/**
 * @tc.number    : ActsZlibTest_3600
 * @tc.name      : Test gzwrite
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzwrite, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzWrite Z_SOLO**********\n");
#else
    int len = static_cast<int>(strlen(HELLO)) + 1;
    gzFile file;
    file = gzopen(TESTFILE, "wb");
    ASSERT_TRUE(file != NULL);
    int err = gzwrite(file, HELLO, len);
    ASSERT_EQ(err, len);
    gzclose(file);
#endif
}

/**
 * @tc.number    : ActsZlibTest_3700
 * @tc.name      : Test inflateBackInit, inflateBack, inflateBackEnd
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestGzInflateBack, Function | MediumTest | Level2)
{
#ifdef Z_SOLO
    fprintf(stderr, "*********ActsZlibTestGzInflateBack Z_SOLO**********\n");
#else
    int err = Z_OK;
    unsigned char *window;
    z_stream strm;
    unsigned char match[65280 + 2]; /* buffer for reversed match or gzip 32K sliding window */
    Byte *uncompr;
    uLong uncomprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    /* initialize inflateBack state for repeated use */
    window = match; /* reuse match buffer */
    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.opaque = nullptr;
    err = inflateBackInit(&strm, 15, window);
    ASSERT_EQ(err, Z_OK);
    if (err != Z_OK) {
        fprintf(stderr, "gun out of memory error--aborting\n");
        ASSERT_TRUE(false);
    }
    strm.next_in = uncompr;
    strm.avail_in = 1;
    err = inflateBack(&strm, pull, nullptr, push, &strm);
    ASSERT_TRUE(err != Z_OK);
    err = inflateBackEnd(&strm);
    ASSERT_EQ(err, Z_OK);
#endif
}

/**
 * @tc.number    : ActsZlibTest_3800
 * @tc.name      : Test inflateCodesUsed
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflateCodesUsed, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);
    unsigned long err;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = 0;
    d_stream.next_out = uncompr;
    err = inflateCodesUsed(&d_stream);
    ASSERT_TRUE(err != Z_OK);
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_3900
 * @tc.name      : Test inflateCopy and inflateEnd
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflateCopy_END, Function | MediumTest | Level2)
{
    int err = Z_OK;
    err = inflate(nullptr, 0);
    ASSERT_TRUE(err == Z_STREAM_ERROR);
    err = inflateEnd(nullptr);
    ASSERT_TRUE(err == Z_STREAM_ERROR);
    err = inflateCopy(nullptr, nullptr);
    ASSERT_TRUE(err == Z_STREAM_ERROR);
}

/**
 * @tc.number    : ActsZlibTest_4000
 * @tc.name      : Test inflateGetDictionary
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflateGetDictionary, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = static_cast<uInt>(comprLen);
    err = inflateInit(&d_stream);
    ASSERT_EQ(err, Z_OK);
    d_stream.next_out = uncompr;
    d_stream.avail_out = static_cast<uInt>(uncomprLen);
    err = inflate(&d_stream, Z_NO_FLUSH);
    err = inflateGetDictionary(&d_stream, uncompr, nullptr);
    ASSERT_EQ(err, Z_OK);
    err = inflateEnd(&d_stream);
    ASSERT_EQ(err, Z_OK);
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_4100
 * @tc.name      : Test inflateGetHeader
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflateGetHeader, Function | MediumTest | Level2)
{
    struct mem_zone *zone;
    zone = (struct mem_zone *)malloc(sizeof(struct mem_zone));
    ASSERT_TRUE(zone != NULL);
    zone->first = nullptr;
    zone->total = 0;
    zone->highwater = 0;
    zone->limit = 0;
    zone->notlifo = 0;
    zone->rogue = 0;
    int err = Z_OK;
    unsigned len = 1;
    unsigned char *out;
    z_stream strm;
    gz_header head;
    strm.opaque = zone;
    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.avail_in = 0;
    strm.next_in = nullptr;
    err = inflateInit2(&strm, 1);
    ASSERT_TRUE(err != Z_OK);
    out = (unsigned char *)malloc(len);
    ASSERT_TRUE(out != NULL);
    head.extra = out;
    head.extra_max = len;
    head.name = out;
    head.name_max = len;
    head.comment = out;
    head.comm_max = len;
    err = inflateGetHeader(&strm, &head);
    ASSERT_TRUE(err != Z_DATA_ERROR);
    free(out);
    free(zone);
}

/**
 * @tc.number    : ActsZlibTest_4200
 * @tc.name      : Test inflateInit_ and inflateInit2_
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflateInit_, Function | MediumTest | Level2)
{
    int err = Z_OK;
    int windowBits = 8;
    z_stream strm;
    struct mem_zone *zone;
    zone = (struct mem_zone *)malloc(sizeof(struct mem_zone));
    ASSERT_TRUE(zone != NULL);
    zone->first = nullptr;
    zone->total = 0;
    zone->highwater = 0;
    zone->limit = 0;
    zone->notlifo = 0;
    zone->rogue = 0;
    strm.opaque = zone;
    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.avail_in = 0;
    strm.next_in = nullptr;
    err = inflateInit(&strm);
    ASSERT_TRUE(err == Z_OK);
    err = inflatePrime(&strm, 5, 31);
    ASSERT_TRUE(err == Z_OK);
    err = inflatePrime(&strm, -1, 0);
    ASSERT_TRUE(err == Z_OK);
    err = inflateSetDictionary(&strm, nullptr, 0);
    ASSERT_TRUE(err == Z_STREAM_ERROR);
    err = inflateEnd(&strm);
    ASSERT_TRUE(err == Z_OK);
    strm.avail_in = 0;
    strm.next_in = nullptr;
    err = inflateInit_(&strm, ZLIB_VERSION - 1, static_cast<int>(sizeof(z_stream)));
    ASSERT_TRUE(err == Z_VERSION_ERROR);

    err = inflateInit2_(&strm, windowBits, ZLIB_VERSION - 1, static_cast<int>(sizeof(z_stream)));
    ASSERT_TRUE(err == Z_VERSION_ERROR);
    free(zone);

#ifdef Z_PREFIX
    deflate_state state;
    _tr_init(&state);
    _dist_code distCode = SIX;
    printf("_dist_code: %d\n", reinterpret_cast<int>(distCode));
    _length_code engthCode = FOUR;
    printf("_length_code: %d\n", reinterpret_cast<int>(engthCode));
    _tr_align trAlign;
    printf("_length_code: %d\n", sizeof(trAlign));
    _tr_flush_bits bits;
    printf("_length_code: %d\n", sizeof(bits));
    _tr_flush_block flushBlock;
    printf("_length_code: %d\n", sizeof(flushBlock));
    _tr_stored_block storedBlock;
    printf("_length_code: %d\n", sizeof(storedBlock));
    _tr_tally tally;
    printf("_length_code: %d\n", sizeof(tally));
#endif
}

/**
 * @tc.number    : ActsZlibTest_4300
 * @tc.name      : Test inflatePrime
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflatePrime, Function | MediumTest | Level2)
{
    int ret;
    z_stream strm;
    struct mem_zone *zone;
    zone = (struct mem_zone *)malloc(sizeof(struct mem_zone));
    ASSERT_TRUE(zone != NULL);
    zone->first = nullptr;
    zone->total = 0;
    zone->highwater = 0;
    zone->limit = 0;
    zone->notlifo = 0;
    zone->rogue = 0;
    strm.opaque = zone;
    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.avail_in = 0;
    strm.next_in = nullptr;
    ret = inflateInit(&strm);
    ASSERT_TRUE(ret == Z_OK);
    ret = inflatePrime(&strm, 5, 31);
    ASSERT_TRUE(ret == Z_OK);
    free(zone);
}

/**
 * @tc.number    : ActsZlibTest_4400
 * @tc.name      : Test inflateReset, inflateReset2, inflateResetKeep
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflateReset, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);
    int err = Z_OK;
    int windowBits = 8;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = 2; /* just read the zlib header */
    err = inflateInit(&d_stream);
    ASSERT_EQ(err, Z_OK);
    d_stream.next_out = uncompr;
    d_stream.avail_out = static_cast<uInt>(uncomprLen);
    inflate(&d_stream, Z_NO_FLUSH);
    err = inflateReset(&d_stream);
    ASSERT_TRUE(err == Z_OK);

    err = inflateResetKeep(&d_stream);
    ASSERT_TRUE(err == Z_OK);
    err = inflateInit2(&d_stream, windowBits);
    inflate(&d_stream, Z_NO_FLUSH);
    err = inflateReset2(&d_stream, windowBits);
    ASSERT_TRUE(err == Z_OK);
}

/**
 * @tc.number    : ActsZlibTest_4500
 * @tc.name      : Test inflateSetDictionary
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflateSetDictionary, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = static_cast<uInt>(comprLen);
    err = inflateInit(&d_stream);
    ASSERT_EQ(err, Z_OK);
    d_stream.next_out = uncompr;
    d_stream.avail_out = static_cast<uInt>(uncomprLen);
    inflate(&d_stream, Z_NO_FLUSH);
    inflateSetDictionary(&d_stream, reinterpret_cast<const Bytef*>(DICTIONARY), static_cast<int>(sizeof(DICTIONARY)));
    err = inflateEnd(&d_stream);
    ASSERT_EQ(err, Z_OK);
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_4600
 * @tc.name      : Test inflateSyncPoint
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflateSyncPoint, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = static_cast<uInt>(comprLen);
    err = inflateInit(&d_stream);
    ASSERT_EQ(err, Z_OK);
    d_stream.next_out = uncompr;
    d_stream.avail_out = static_cast<uInt>(uncomprLen);
    err = inflateSyncPoint(&d_stream);
    ASSERT_EQ(err, Z_OK);
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_4700
 * @tc.name      : Test inflateUndermine
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflateUndermine, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = static_cast<uInt>(comprLen);
    err = inflateInit(&d_stream);
    ASSERT_EQ(err, Z_OK);
    d_stream.next_out = uncompr;
    d_stream.avail_out = static_cast<uInt>(uncomprLen);
    err = inflateUndermine(&d_stream, 1);
    ASSERT_EQ(err, Z_DATA_ERROR);
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_4800
 * @tc.name      : Test inflateValidate
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestInflateValidate, Function | MediumTest | Level2)
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000 * sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    compr = static_cast<Byte*>(calloc(static_cast<uInt>(comprLen), CALLOC_SIZE));
    uncompr = static_cast<Byte*>(calloc(static_cast<uInt>(uncomprLen), CALLOC_SIZE));
    ASSERT_TRUE(compr != Z_NULL && uncompr != Z_NULL);

    int err = Z_OK;
    z_stream d_stream; /* decompression stream */
    strcpy(reinterpret_cast<char *>(uncompr), GARBAGE);
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = compr;
    d_stream.avail_in = static_cast<uInt>(comprLen);
    err = inflateInit(&d_stream);
    ASSERT_EQ(err, Z_OK);
    d_stream.next_out = uncompr;
    d_stream.avail_out = static_cast<uInt>(uncomprLen);
    err = inflateValidate(&d_stream, 1);
    ASSERT_EQ(err, Z_OK);
    free(compr);
    free(uncompr);
}

/**
 * @tc.number    : ActsZlibTest_4900
 * @tc.name      : Test zlibCompileFlags
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestzlibCompileFlags, Function | MediumTest | Level2)
{
    static const char* myVersion = ZLIB_VERSION;

    if (zlibVersion()[0] != myVersion[0]) {
        fprintf(stderr, "incompatible zlib version\n");
        ASSERT_TRUE(false);

    } else if (strcmp(zlibVersion(), ZLIB_VERSION)) {
        fprintf(stderr, "warning: different zlib version\n");
    }

    printf("zlib version %s = 0x%04x, compile flags = 0x%lx\n",
            ZLIB_VERSION, ZLIB_VERNUM, zlibCompileFlags());
}

/**
 * @tc.number    : ActsZlibTest_5000
 * @tc.name      : Test zError
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestzError, Function | MediumTest | Level2)
{
    const char* err;
    err = zError(Z_DATA_ERROR);
    ASSERT_EQ(err, "data error");
}

/**
 * @tc.number    : ActsZlibTest_5100
 * @tc.name      : Test zlibVersion
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsZlibTest, ActsZlibTestzlibVersion, Function | MediumTest | Level2)
{
    static const char* myVersion = ZLIB_VERSION;
    static const char* err;
    err = zlibVersion();
    ASSERT_EQ(err, myVersion);
}
