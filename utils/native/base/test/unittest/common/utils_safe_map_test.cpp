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
#include "safe_map.h"

#include <array>
#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <chrono> // std::chrono::seconds

using namespace testing::ext;
using namespace OHOS;
using namespace std;

class UtilsSafeMap : public testing::Test {
};

/*
 * @tc.name: testUtilsCopyAndAssign001
 * @tc.desc: single thread test the normal feature insert and erase and EnsureInsert
 */
HWTEST_F(UtilsSafeMap, testUtilsCopyAndAssign001, TestSize.Level0)
{
    SafeMap<string, int> demoData;
    // insert new
    demoData.Insert("A", 1);
    ASSERT_FALSE(demoData.IsEmpty());
    ASSERT_EQ(demoData.Size(), 1);

    SafeMap<string, int> newdemo = demoData;
    int tar = -1;
    ASSERT_TRUE(newdemo.Find("A", tar));
    ASSERT_EQ(1, tar);

    tar = -1;
    SafeMap<string, int> newdemo2;
    newdemo2 = demoData;
    ASSERT_TRUE(newdemo2.Find("A", tar));
    ASSERT_EQ(1, tar);
}

/*
 * @tc.name: testUtilsoperator001
 * @tc.desc: SafeMap
 */
HWTEST_F(UtilsSafeMap, testUtilsoperator001, TestSize.Level0)
{
    SafeMap<string, int> demoData;
    // insert new
    demoData.Insert("A", 1);
    ASSERT_FALSE(demoData.IsEmpty());
    ASSERT_EQ(demoData.Size(), 1);
    ASSERT_EQ(demoData["A"], 1);

    SafeMap<string, int> newdemo = demoData;
    ASSERT_EQ(newdemo["A"], 1);

    int tar = -1;
    newdemo["B"] = 6;
    ASSERT_TRUE(newdemo.Find("B", tar));
    ASSERT_EQ(6, tar);

    SafeMap<string, int> newdemo2;
    newdemo2 = newdemo;
    ASSERT_EQ(newdemo2["A"], 1);
}

/*
 * @tc.name: testUtilsNormalFeatureInsert001
 * @tc.desc: SafeMap
 */
HWTEST_F(UtilsSafeMap, testUtilsNormalFeatureInsert001, TestSize.Level0)
{
    SafeMap<string, int> demoData;
    ASSERT_TRUE(demoData.IsEmpty());
    // insert new
    demoData.Insert("A", 1);
    ASSERT_FALSE(demoData.IsEmpty());
    ASSERT_EQ(demoData.Size(), 1);

    // insert copy one should fail
    ASSERT_FALSE(demoData.Insert("A", 2));
    ASSERT_EQ(demoData.Size(), 1);
}

/*
 * @tc.name: testUtilsNormalFeatureEnsureInsert001
 * @tc.desc: SafeMap
 */
HWTEST_F(UtilsSafeMap, testUtilsNormalFeatureEnsureInsert001, TestSize.Level0)
{
    SafeMap<string, int> demoData;
    ASSERT_TRUE(demoData.IsEmpty());

    demoData.Insert("A", 1);
    demoData.EnsureInsert("B", 2);

    ASSERT_FALSE(demoData.IsEmpty());
    ASSERT_EQ(demoData.Size(), 2);
    // insert copy one and new one
    demoData.EnsureInsert("B", 5);
    demoData.EnsureInsert("C", 6);

    ASSERT_EQ(demoData.Size(), 3);
}

/*
 * @tc.name: testUtilsNormalFeatureFind001
 * @tc.desc: SafeMap
 */
HWTEST_F(UtilsSafeMap, testUtilsNormalFeatureFind001, TestSize.Level0)
{
    SafeMap<string, int> demoData;
    ASSERT_TRUE(demoData.IsEmpty());

    demoData.Insert("A", 1);
    demoData.Insert("B", 10000);
    demoData.EnsureInsert("B", 2);
    demoData.EnsureInsert("C", 6);

    ASSERT_FALSE(demoData.IsEmpty());
    ASSERT_EQ(demoData.Size(), 3);

    int i = 0;
    ASSERT_TRUE(demoData.Find("A", i));
    ASSERT_EQ(i, 1);
    ASSERT_TRUE(demoData.Find("B", i));
    ASSERT_EQ(i, 2);

    ASSERT_TRUE(demoData.Find("C", i));
    ASSERT_EQ(i, 6);
}

/*
 * @tc.name: testUtilsNormalFeatureFindAndSet001
 * @tc.desc: SafeMap
 */
HWTEST_F(UtilsSafeMap, testUtilsNormalFeatureFindAndSet001, TestSize.Level0)
{
    SafeMap<string, int> demoData;
    ASSERT_TRUE(demoData.IsEmpty());

    demoData.Insert("A", 1);
    demoData.EnsureInsert("B", 2);

    int oldvalue = 0;
    int newvalue = 3;
    ASSERT_TRUE(demoData.FindOldAndSetNew("A", oldvalue, newvalue));
    // old value
    ASSERT_EQ(oldvalue, 1);

    newvalue = 4;
    ASSERT_TRUE(demoData.FindOldAndSetNew("B", oldvalue, newvalue));
    // old value
    ASSERT_EQ(oldvalue, 2);
    int i = -1;
    ASSERT_TRUE(demoData.Find("A", i));
    // new value
    ASSERT_EQ(i, 3);
    ASSERT_TRUE(demoData.Find("B", i));
    // new value
    ASSERT_EQ(i, 4);
}

/*
 * @tc.name: testUtilsNormalFeatureEraseAndClear001
 * @tc.desc: SafeMap
 */
HWTEST_F(UtilsSafeMap, testUtilsNormalFeatureEraseAndClear001, TestSize.Level0)
{
    SafeMap<string, int> demoData;
    ASSERT_TRUE(demoData.IsEmpty());

    demoData.Insert("A", 1);
    demoData.EnsureInsert("B", 2);

    ASSERT_EQ(demoData.Size(), 2);
    demoData.Erase("A");
    ASSERT_EQ(demoData.Size(), 1);

    demoData.Clear();
    ASSERT_EQ(demoData.Size(), 0);
}

/*
 * @tc.name: testUtilsConcurrentWriteAndRead001
 * @tc.desc: 100 threads test in writein to the same key of the map, while read at same time  and no throw
 */
const int THREAD_NUM = 100;
HWTEST_F(UtilsSafeMap, testUtilsConcurrentWriteAndRead001, TestSize.Level0)
{
    SafeMap<string, int> demoData;
    std::thread threads[THREAD_NUM];
    std::thread checkThread[THREAD_NUM];
    ASSERT_NO_THROW({
        auto lamfuncInsert = [](SafeMap<string, int>& data, const string& key,
            const int& value, const std::chrono::system_clock::time_point& absTime) {
            std::this_thread::sleep_until(absTime);
            data.EnsureInsert(key, value);
        };

        auto lamfuncCheck = [](SafeMap<string, int>& data, const string& key,
            std::chrono::system_clock::time_point absTime) {
            std::this_thread::sleep_until(absTime);
            thread_local int i = -1;
            data.Find(key, i);
        };

        using std::chrono::system_clock;

        std::time_t timeT = system_clock::to_time_t(system_clock::now());
        timeT += 2;
        string key("A");

        for (int i = 0; i < THREAD_NUM; ++i) {
            threads[i] = std::thread(lamfuncInsert, std::ref(demoData), key, i, system_clock::from_time_t(timeT));
            checkThread[i] = std::thread(lamfuncCheck, std::ref(demoData), key, system_clock::from_time_t(timeT));
        }

        std::this_thread::sleep_for(std::chrono::seconds(3));
        for (auto& t : threads) {
            t.join();
        }

        for (auto& t : checkThread) {
            t.join();
        }
    });
}

/*
 * @tc.name: testUtilsConcurrentWriteAndFind001
 * @tc.desc: 100 threads test in writein to the corresponding key of the map,
 * while read at same time  and check the results
 */
HWTEST_F(UtilsSafeMap, testUtilsConcurrentWriteAndFind001, TestSize.Level0)
{
    SafeMap<string, int> demoData;
    std::thread threads[THREAD_NUM];
    std::vector<std::future<int>> vcfi;

    ASSERT_NO_THROW({
        auto lamfuncInsert = [](SafeMap<string, int>& data, const string& key,
            const int& value, const std::chrono::system_clock::time_point& absTime) {
            std::this_thread::sleep_until(absTime);
            data.EnsureInsert(key, value);
        };

        auto lamfuncCheckLoop = [](SafeMap<string, int>& data, const string& key,
            std::chrono::system_clock::time_point absTime) {
            std::this_thread::sleep_until(absTime);
            thread_local int i = -1;
            while (!data.Find(key, i)) {
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
            return i;
        };

        using std::chrono::system_clock;

        std::time_t timeT = system_clock::to_time_t(system_clock::now());
        timeT += 2;
        string key("A");

        for (int i = 0; i < THREAD_NUM; ++i) {
            threads[i] = std::thread(lamfuncInsert, std::ref(demoData),
                key + std::to_string(i), i, system_clock::from_time_t(timeT));
            vcfi.push_back(std::async(std::launch::async, lamfuncCheckLoop,
                std::ref(demoData), key + std::to_string(i), system_clock::from_time_t(timeT)));
        }

        std::this_thread::sleep_for(std::chrono::seconds(4));
        for (auto& t : threads) {
            t.join();
        }

        vector<int> result;

        for (auto& t : vcfi) {
            result.push_back(t.get());
        }

        std::sort(result.begin(), result.end());

        for (int i = 0; i < THREAD_NUM; ++i) {
            ASSERT_EQ(i, result[i]);
        }
    });
}

/*
 * @tc.name: testUtilsConcurrentWriteAndFindAndSet001
 * @tc.desc: 100 threads test in writein to the corresponding key of the map,
 * while findandfix at same time  and check the results
 */
HWTEST_F(UtilsSafeMap, testUtilsConcurrentWriteAndFindAndSet001, TestSize.Level0)
{
    SafeMap<string, int> demoData;
    std::thread threads[THREAD_NUM];
    std::vector<std::future<int>> vcfi;

    ASSERT_NO_THROW({
        auto lamfuncInsert = [](SafeMap<string, int>& data, const string& key,
            const int& value, const std::chrono::system_clock::time_point& absTime) {
            std::this_thread::sleep_until(absTime);
            data.EnsureInsert(key, value);
        };

        auto lamfuncCheckLoop = [](SafeMap<string, int>& data, const string& key,
            const int& newvalue, std::chrono::system_clock::time_point absTime) {
            std::this_thread::sleep_until(absTime);
            thread_local int i = -1;
            while (!data.FindOldAndSetNew(key, i, newvalue)) {
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
            return i;
        };

        using std::chrono::system_clock;

        std::time_t timeT = system_clock::to_time_t(system_clock::now());
        timeT += 2;
        string key("A");

        for (int i = 0; i < THREAD_NUM; ++i) {
            threads[i] = std::thread(lamfuncInsert, std::ref(demoData),
                key + std::to_string(i), i, system_clock::from_time_t(timeT));
            vcfi.push_back(std::async(std::launch::async, lamfuncCheckLoop,
                std::ref(demoData), key + std::to_string(i), i + 1, system_clock::from_time_t(timeT)));
        }

        std::this_thread::sleep_for(std::chrono::seconds(4));
        for (auto& t : threads) {
            t.join();
        }

        vector<int> result;
        for (auto& t : vcfi) {
            result.push_back(t.get());
        }

        std::sort(result.begin(), result.end());

        for (int i = 0; i < THREAD_NUM; ++i) {
            ASSERT_EQ(i, result[i]);
        }

        int t = 0;
        result.clear();
        for (int i = 0; i < THREAD_NUM; ++i) {
            t = -1;
            ASSERT_TRUE(demoData.Find("A" + std::to_string(i), t));
            result.push_back(t);
        }

        std::sort(result.begin(), result.end());

        for (int i = 0; i < THREAD_NUM; ++i) {
            ASSERT_EQ(i + 1, result[i]);
        }
    });
}
