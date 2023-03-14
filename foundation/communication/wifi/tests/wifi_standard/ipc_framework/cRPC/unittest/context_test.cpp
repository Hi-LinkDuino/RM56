/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "context_test.h"
#include "securec.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
static int CalculateLeft(const Context *ctx)
{
    int left =
        (ctx->wBegin <= ctx->wEnd) ? (ctx->wCapacity - 1 - ctx->wEnd + ctx->wBegin) : (ctx->wBegin - ctx->wEnd - 1);
    return left;
}

HWTEST_F(ContextTest, CreateContextTest, TestSize.Level1)
{
    ctx = CreateContext(100);
    ASSERT_TRUE(ctx != nullptr);
    ASSERT_TRUE(ctx->wCapacity == CONTEXT_BUFFER_MIN_SIZE);
    ReleaseContext(ctx);
    ctx = CreateContext(102400000);
    ASSERT_TRUE(ctx != nullptr);
    ASSERT_TRUE(ctx->wCapacity == CONTEXT_BUFFER_MAX_SIZE);
}

HWTEST_F(ContextTest, AppendCacheTest, TestSize.Level1)
{
    ctx = CreateContext(1024);
    ASSERT_TRUE(ctx != nullptr) << "capacity :" << ctx->wCapacity;
    /* capacity size 1024 */
    ASSERT_TRUE(ctx->wCapacity == 1024);
    ASSERT_TRUE(ctx->wEnd == 0);

    /* calculate left capacity test */
    ctx->wBegin = ctx->wEnd = 0;
    ASSERT_TRUE(CalculateLeft(ctx) == 1023); /* capacity, use a space define endpos */
    ctx->wEnd = 1023;                    /* pos [0,1023], max pos 1023 */
    ASSERT_TRUE(CalculateLeft(ctx) == 0);
    ctx->wBegin = 1023;
    ctx->wEnd = 0;
    ASSERT_TRUE(CalculateLeft(ctx) == 1022); /* wEnd max num can be 1022, so from 0 - 1021 = 1022 */
    ctx->wEnd = 1022;
    ASSERT_TRUE(CalculateLeft(ctx) == 0);
    ctx->wBegin = ctx->wEnd = 0;

    char buff[] = "123456789 123456789 123456789 12";
    for (unsigned i = 0; i < 1024 / 32 - 1; ++i) {
        ContextAppendWrite(ctx, buff, strlen(buff));
        ASSERT_TRUE(ctx->wEnd == (i + 1) * 32);
    }
    ASSERT_TRUE(strcpy_s(buff, sizeof(buff), "123456789 123456789 123456789 1") == EOK);
    ContextAppendWrite(ctx, buff, strlen(buff));
    ASSERT_TRUE(ctx->wEnd == 1023);
    ASSERT_TRUE(CalculateLeft(ctx) == 0);
    ctx->wBegin = 1023;
    ASSERT_TRUE(strcpy_s(buff, sizeof(buff), "123456789 123456789 123456789 12") == EOK);
    for (unsigned i = 0; i < 1024 / 32 - 1; ++i) {
        ContextAppendWrite(ctx, buff, strlen(buff));
        ASSERT_TRUE(ctx->wEnd == (i + 1) * 32 - 1);
    }
    ASSERT_TRUE(CalculateLeft(ctx) == 31);
    ContextAppendWrite(ctx, buff, strlen(buff));
    ASSERT_TRUE(ctx->wCapacity == 2048);
    ASSERT_TRUE(ctx->wEnd == (1023 + 1024));
}

static int ExpandReadCache(Context *context, int len)
{
    int left = (context->rBegin <= context->rEnd) ? (context->rCapacity - 1 - context->rEnd + context->rBegin)
                                                  : (context->rBegin - context->rEnd - 1);
    if (left < len) {
        int capacity = context->rCapacity;
        while (left < len) {
            capacity += context->rCapacity;
            left += context->rCapacity;
        }
        char *p = (char *)calloc(capacity, sizeof(char));
        if (p == nullptr) {
            return -1;
        }
        if (memmove_s(p, capacity, context->szRead, context->rCapacity) != EOK) {
            free(p);
            return -1;
        }
        if (context->rBegin > context->rEnd &&
            memmove_s(p + context->rCapacity, context->rCapacity, p, context->rEnd) != EOK) {
            free(p);
            return -1;
        }
        char *pFree = context->szRead;
        context->szRead = p;
        if (context->rBegin > context->rEnd) {
            context->rEnd += context->rCapacity;
        }
        context->rCapacity = capacity;
        free(pFree);
    }
    return 0;
}

static int ContextAppendRead(Context *context, const char *buf, int len)
{
    if (ExpandReadCache(context, len) < 0) {
        return -1;
    }
    if (context->rEnd + len < context->rCapacity) {
        if (memmove_s(context->szRead + context->rEnd, context->rCapacity - context->rEnd, buf, len) != EOK) {
            return -1;
        }
        context->rEnd += len;
    } else {
        int tmp = context->rCapacity - context->rEnd;
        if (tmp > 0 && memmove_s(context->szRead + context->rEnd, tmp, buf, tmp) != EOK) {
            return -1;
        }
        if (tmp < len && memmove_s(context->szRead, len - tmp, buf + tmp, len - tmp) != EOK) {
            return -1;
        }
        context->rEnd = len - tmp;
    }
    return 0;
}

HWTEST_F(ContextTest, ContextGetReadRecordTest, TestSize.Level1)
{
    ctx = CreateContext(1024);
    ASSERT_TRUE(ctx != nullptr);
    ASSERT_TRUE(ctx->rCapacity == 1024);
    char *p = ContextGetReadRecord(ctx);
    ASSERT_TRUE(p == nullptr);

    int ret = ContextAppendRead(ctx, "hello", strlen("hello"));
    ASSERT_TRUE(ret == 0);
    p = ContextGetReadRecord(ctx);
    ASSERT_TRUE(p == nullptr);
    ret = ContextAppendRead(ctx, ctx->cMsgEnd, strlen(ctx->cMsgEnd));
    ASSERT_TRUE(ret == 0);
    p = ContextGetReadRecord(ctx);
    ASSERT_TRUE(p != nullptr);
    EXPECT_TRUE(strcmp(p, "hello") == 0);
    free(p);
    ctx->rBegin = ctx->rEnd = 1024 - 5;  // set end pos
    ret = ContextAppendRead(ctx, "hello, welcome", strlen("hello, welcome"));
    ASSERT_TRUE(ret == 0);
    p = ContextGetReadRecord(ctx);
    ASSERT_TRUE(p == nullptr);
    ret = ContextAppendRead(ctx, ctx->cMsgEnd, strlen(ctx->cMsgEnd));
    ASSERT_TRUE(ret == 0);
    p = ContextGetReadRecord(ctx);
    ASSERT_TRUE(p != nullptr);
    EXPECT_TRUE(strcmp(p, "hello, welcome") == 0);
    free(p);
}

HWTEST_F(ContextTest, ContextReadNetTest, TestSize.Level1)
{
    ctx = CreateContext(1024);
    ASSERT_TRUE(ctx != nullptr);
    int ret = ContextReadNet(ctx);
    EXPECT_TRUE(ret == MAX_ONE_LINE_SIZE - 1);
    ctx->rBegin = 1024 - 5;
    ret = ContextReadNet(ctx);
    EXPECT_TRUE(ret == MAX_ONE_LINE_SIZE - 1);
    EXPECT_TRUE(ctx->rCapacity == 2048);
}

HWTEST_F(ContextTest, ContextWriteNetTest, TestSize.Level1)
{
    ASSERT_TRUE(ContextWriteNet(nullptr) == -1);
    ctx = CreateContext(1024);
    ASSERT_TRUE(ctx != nullptr);
    int ret = ContextWriteNet(ctx);
    EXPECT_TRUE(ret == 0);
    char buff[] = "123456789 123456789 123456789 12";
    int size = strlen(buff);
    ContextAppendWrite(ctx, buff, size);
    ret = ContextWriteNet(ctx);
    EXPECT_TRUE(ret == size);
    ctx->wBegin = ctx->wEnd = 1023;
    ContextAppendWrite(ctx, buff, size);
    ret = ContextWriteNet(ctx);
    EXPECT_TRUE(ret == size - 1);
}
}  // namespace Wifi
}  // namespace OHOS