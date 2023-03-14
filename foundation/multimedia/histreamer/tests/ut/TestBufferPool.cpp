/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "common/any.h"
#define private public
#define protected public
#define UNIT_TEST 1

#include <vector>

#include "utils/buffer_pool.h"
#include "utils/constants.h"
#include "pipeline/core/type_define.h"

namespace OHOS::Media::Test {
class BufferPoolTest : public ::testing::Test {
public:
    void SetUp() override
    {
        pool = std::make_shared<BufferPool<AVBuffer>>(DEFAULT_POOL_SIZE);
        pool->Init();
    }

    void TearDown() override
    {
    }

    std::shared_ptr<BufferPool<AVBuffer>> pool;
};

TEST_F(BufferPoolTest, buffer_pool_init_buffer)
{
    EXPECT_EQ(DEFAULT_POOL_SIZE, pool->Size());
}

TEST_F(BufferPoolTest, buffer_pool_recycle_succ)
{
    EXPECT_EQ(DEFAULT_POOL_SIZE, pool->Size());
    auto buffPtr = pool->AllocateBuffer();
    EXPECT_EQ(DEFAULT_POOL_SIZE - 1, pool->Size());
    buffPtr.reset();
    EXPECT_EQ(DEFAULT_POOL_SIZE, pool->Size());
}

TEST_F(BufferPoolTest, buffer_pool_return_nullptr_after_buffer_exhausted)
{
    EXPECT_EQ(DEFAULT_POOL_SIZE, pool->Size());
    std::vector<std::shared_ptr<AVBuffer>> buffers;
    buffers.reserve(DEFAULT_POOL_SIZE);
    for (size_t i = 0; i < DEFAULT_POOL_SIZE; ++i) {
        buffers.emplace_back(pool->AllocateBuffer());
    }
    EXPECT_EQ(true, pool->Empty());
    EXPECT_EQ(nullptr, pool->AllocateBufferNonBlocking());
}
} // namespace