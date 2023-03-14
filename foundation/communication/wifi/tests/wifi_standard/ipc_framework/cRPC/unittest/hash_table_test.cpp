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

#include "hash_table_test.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
static unsigned g_pNum[] = {
    3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
static unsigned FindNextNum(unsigned num)
{
    for (unsigned i = 0; i < sizeof(g_pNum) / sizeof(g_pNum[0]); ++i) {
        if (g_pNum[i] >= num) {
            return g_pNum[i];
        }
    }
    return 0;
}

HWTEST_F(HashTableTest, InitHashTableTest, TestSize.Level1)
{
    unsigned slots = 13;
    table = InitHashTable(slots);
    EXPECT_TRUE(table != nullptr);
    EXPECT_TRUE(table->list != nullptr);
    EXPECT_TRUE(table->slots == FindNextNum(slots));
    for (unsigned i = 0; i < table->slots; ++i) {
        EXPECT_TRUE(table->list[i] == nullptr);
    }
}

HWTEST_F(HashTableTest, UseHashTableTest, TestSize.Level1)
{
    table = InitHashTable(3);
    ASSERT_NE(table, nullptr);
    for (int fd = 1; fd <= 100; ++fd) {
        Context *ctx = (Context *)calloc(1, sizeof(Context));
        ctx->fd = fd;
        ASSERT_EQ(InsertHashTable(table, ctx), 0);
    }
    ASSERT_EQ(table->size, 100);
    Context *ctx = (Context *)calloc(1, sizeof(Context));
    ctx->fd = 1;
    EXPECT_EQ(InsertHashTable(table, ctx), -2);
    free(ctx);
    int del = 0;
    for (int fd = 1; fd <= 100; ++fd) {
        Context *pCtx = FindContext(table, fd);
        EXPECT_NE(pCtx, nullptr);
        EXPECT_EQ(pCtx->fd, fd);
        if (fd % 10 == 0) {
            ++del;
            DeleteHashTable(table, pCtx);
        }
    }
    EXPECT_EQ(table->size, 100 - del);
}
}  // namespace Wifi
}  // namespace OHOS