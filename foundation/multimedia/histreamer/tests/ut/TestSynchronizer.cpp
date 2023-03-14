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
#define private public
#define protected public

#undef UNIT_TEST

#include <atomic>   // NOLINT
#include <chrono>   // NOLINT
#include <iostream> // NOLINT
#include <memory>   // NOLINT
#include "osal/base/synchronizer.h"
#include "osal/thread/task.h"

namespace OHOS {
namespace Media {
namespace Test {
using namespace OSAL;

class TestSynchronizer : public ::testing::Test {
public:
    void SetUp() override
    {
        task1 = std::make_shared<Task>("workTask1");
        task2 = std::make_shared<Task>("workTask2");
        task3 = std::make_shared<Task>("workTask3");
        isDataRcved = false;
        isStarted = false;
    }

    void TearDown() override
    {
    }

    std::shared_ptr<Task> task1;
    std::shared_ptr<Task> task2;
    std::shared_ptr<Task> task3;
    std::atomic<bool> isDataRcved;
    std::atomic<bool> isStarted;
    static Synchronizer<int, int> synchronizer;
};

Synchronizer<int, int> TestSynchronizer::synchronizer("sync");

TEST_F(TestSynchronizer, test_waitfor_fail)
{
    int syncId = 0;
    task1->RegisterHandler([this, syncId] { synchronizer.Notify(syncId, 1234); });
    int timeoutMs = 100;
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = synchronizer.WaitFor(
        syncId, [] {}, timeoutMs);
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = static_cast<std::chrono::duration<double>>(end - start).count() * 1000;
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    std::cout << "TestSynchronizer time diff: " << diff << std::endl;
}

TEST_F(TestSynchronizer, test_waitfor_succ)
{
    int syncId = 0;
    task1->RegisterHandler([this, syncId] { synchronizer.Notify(syncId, 1234); });
    task1->Start();
    int timeoutMs = 1000;
    auto rtv = synchronizer.WaitFor(
        syncId, [] {}, timeoutMs);
    EXPECT_EQ(true, rtv);
}

TEST_F(TestSynchronizer, test_waitfor_with_result_succ)
{
    int syncId = 0;
    int expect = 1234;
    task1->RegisterHandler([this, syncId, expect] { synchronizer.Notify(syncId, expect); });
    task1->Start();
    int timeoutMs = 1000;
    int result = 0;
    auto rtv = synchronizer.WaitFor(
        syncId, [] { return true; }, timeoutMs, result);
    EXPECT_EQ(true, rtv);
    EXPECT_EQ(expect, result);
}

TEST_F(TestSynchronizer, test_wait_succ)
{
    int syncId = 0;
    int result = 0;
    int expect = 1234;
    task1->RegisterHandler([this, syncId, &result] {
        if (!isDataRcved.load()) {
            synchronizer.Wait(
                syncId, [] {}, result);
            isDataRcved = true;
        }
    });
    task1->Start();
    task2->RegisterHandler([this, syncId, expect] { synchronizer.Notify(syncId, expect); });
    task2->Start();
    while (!isDataRcved.load()) {}
    EXPECT_EQ(expect, result);
}
} // namespace Test
} // namespace Media
} // namespace OHOS