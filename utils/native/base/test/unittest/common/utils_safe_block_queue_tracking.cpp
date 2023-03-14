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
#include "safe_block_queue.h"

#include <array>
#include <future>
#include <gtest/gtest.h>
#include <thread>

#include <iostream>

#include <chrono> // std::chrono::seconds


using namespace testing::ext;
using namespace OHOS;
using namespace std;

class UtilsSafeBlockQueueTracking : public testing::Test {
};

void taskPut(SafeBlockQueueTracking<int>& q, int i)
{
    q.Push(i);
}

const unsigned int QUEUE_SLOTS = 10;
const unsigned int THREAD_NUM = QUEUE_SLOTS + 1;

class DemoThreadData {
public:
    DemoThreadData()
    {
        putStatus = false;
        getStatus = false;
        joinStatus = false;
    }

    static SafeBlockQueueTracking<int> shareQueue;
    static bool joinStatus;
    bool putStatus;
    bool getStatus;

    void Put(int i)
    {
        shareQueue.Push(i);
        putStatus = true;
    }

    void Get()
    {
        shareQueue.Pop();
        getStatus = true;
    }

    void GetAndOneTaskDone()
    {
        shareQueue.Pop();
        getStatus = true;
        shareQueue.OneTaskDone();
    }

    void Join()
    {
        shareQueue.Join();
        joinStatus = true;
    }
};

SafeBlockQueueTracking<int> DemoThreadData::shareQueue(QUEUE_SLOTS);
bool DemoThreadData::joinStatus = false;

void PutHandleThreadData(DemoThreadData& q, int i)
{
    q.Put(i);
}

void GetThreadDatePushedStatus(std::array<DemoThreadData, THREAD_NUM>& demoDatas,
                                             unsigned int& pushedIn, unsigned int& unpushedIn)
{
    pushedIn = 0;
    unpushedIn = 0;
    for (auto& t : demoDatas) {
        if (t.putStatus) {
            pushedIn++;
        } else {
            unpushedIn++;
        }
    }
}

void GetThreadDateGetedStatus(std::array<DemoThreadData, THREAD_NUM>& demoDatas,
                                           unsigned int& getedOut, unsigned int& ungetedOut)
{
    getedOut = 0;
    ungetedOut = 0;
    for (auto& t : demoDatas) {
        if (t.getStatus) {
            getedOut++;
        } else {
            ungetedOut++;
        }
    }
}

void PutHandleThreadDataTime(DemoThreadData& q, int i, std::chrono::system_clock::time_point absTime)
{
    std::this_thread::sleep_until(absTime);

    q.Put(i);
}

void GetAndOneTaskDoneHandleThreadDataTime(DemoThreadData& q, int i, std::chrono::system_clock::time_point absTime)
{
    std::this_thread::sleep_until(absTime);

    q.GetAndOneTaskDone();
}

/*
 * @tc.name: testPut001
 * @tc.desc: Single-threaded call put and get to determine that the normal scenario is working properly
 */
HWTEST_F(UtilsSafeBlockQueueTracking, testPut001, TestSize.Level0)
{
    SafeBlockQueueTracking<int> qi(10);
    int i = 1;
    qi.Push(i);
    EXPECT_EQ(static_cast<unsigned>(1), qi.Size());
    ASSERT_GT(qi.GetUnfinishTaskNum(), 0);
}

/*
 * @tc.name: testGet001
 * @tc.desc: Single-threaded call put and get to determine that the normal scenario is working properly
 */
HWTEST_F(UtilsSafeBlockQueueTracking, testGet001, TestSize.Level0)
{
    SafeBlockQueueTracking<int> qi(10);
    for (int i = 0; i < 3; i++) {
        qi.Push(i);
    }
    EXPECT_EQ(static_cast<unsigned>(3), qi.Size());
    int t = qi.Pop();
    ASSERT_EQ(t, 0);
    ASSERT_GT(qi.GetUnfinishTaskNum(), 0);
}

/*
 * @tc.name: testMutilthreadPutAndBlock001
 * @tc.desc: Multiple threads put until blocking runs, one thread gets, all threads finish running normally
 */
HWTEST_F(UtilsSafeBlockQueueTracking, testMutilthreadPutAndBlock001, TestSize.Level0)
{
    std::thread threads[THREAD_NUM];

    std::array<DemoThreadData, THREAD_NUM> demoDatas;
    demoDatas.fill(DemoThreadData());
    demoDatas[0].Put(1);
    ASSERT_FALSE(demoDatas[0].joinStatus);

    // start thread to join
    DemoThreadData tmp = DemoThreadData();
    std::thread joinThread = std::thread(&DemoThreadData::Join, tmp);
    ASSERT_FALSE(demoDatas[0].joinStatus);

    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        threads[i] = std::thread(PutHandleThreadData, std::ref(demoDatas[i]), i);
    }
    ASSERT_FALSE(demoDatas[0].joinStatus);
    // 1. queue is full and some threads is blocked
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_TRUE(DemoThreadData::shareQueue.IsFull());

    ASSERT_FALSE(demoDatas[0].joinStatus);
    // 2.  get one out  and wait some put in
    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        demoDatas[0].GetAndOneTaskDone();
        ASSERT_FALSE(demoDatas[0].joinStatus);
    }
    ASSERT_FALSE(demoDatas[0].joinStatus);
    demoDatas[0].GetAndOneTaskDone();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ASSERT_TRUE(demoDatas[0].joinStatus);

    // recover state
    for (auto& t : threads) {
        t.join();
    }
    joinThread.join();

    while (!DemoThreadData::shareQueue.IsEmpty()) {
        demoDatas[0].GetAndOneTaskDone();
    }
    demoDatas[0].joinStatus = false;
}

/*
 * @tc.name: testMutilthreadConcurrentPutAndBlockInblankqueue001
 * @tc.desc: Multi-threaded put() on the empty queue. When n threads are waiting to reach a certain
 * time-point, everyone puts concurrent to see the status of the queue and the state of the thread.
 */
HWTEST_F(UtilsSafeBlockQueueTracking, testMutilthreadConcurrentPutAndBlockInblankqueue001, TestSize.Level0)
{
    // 1. prepare
    std::thread threads[THREAD_NUM];
    std::array<DemoThreadData, THREAD_NUM> demoDatas;
    demoDatas.fill(DemoThreadData());

    using std::chrono::system_clock;

    std::time_t timeT = system_clock::to_time_t(system_clock::now());
    timeT += 2;

    // 2. start thread
    ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());
    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        threads[i] = std::thread(PutHandleThreadDataTime, std::ref(demoDatas[i]), i, system_clock::from_time_t(timeT));
    }

    // 3. queue is full and some threads is blocked
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // start thread to join
    DemoThreadData tmp = DemoThreadData();
    std::thread joinThread = std::thread(&DemoThreadData::Join, tmp);
    ASSERT_FALSE(demoDatas[0].joinStatus);

    ASSERT_TRUE(DemoThreadData::shareQueue.IsFull());
    unsigned int pushedIn = 0;
    unsigned int unpushedIn = 0;
    unsigned int getedOut = 0;
    unsigned int ungetedOut = 0;
    GetThreadDatePushedStatus(demoDatas, pushedIn, unpushedIn);

    ASSERT_EQ(pushedIn, QUEUE_SLOTS);
    ASSERT_EQ(unpushedIn, THREAD_NUM - QUEUE_SLOTS);

    // get one out and wait some put in
    for (unsigned int i = 0; i < THREAD_NUM - QUEUE_SLOTS; i++) {
        demoDatas[0].GetAndOneTaskDone();
        ASSERT_FALSE(demoDatas[0].joinStatus);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    // queue is full and some threads is blocked and is not joined
    ASSERT_TRUE(DemoThreadData::shareQueue.IsFull());
    GetThreadDatePushedStatus(demoDatas, pushedIn, unpushedIn);
    GetThreadDateGetedStatus(demoDatas, getedOut, ungetedOut);
    ASSERT_EQ(pushedIn, THREAD_NUM);
    ASSERT_EQ(getedOut, THREAD_NUM - QUEUE_SLOTS);

    for (auto& t : threads) {
        t.join();
    }

    while (!DemoThreadData::shareQueue.IsEmpty()) {
        demoDatas[0].GetAndOneTaskDone();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(demoDatas[0].joinStatus);
    demoDatas[0].joinStatus = false;
    joinThread.join();
}

/*
 * @tc.name: testMutilthreadConcurrentPutAndBlockInfullqueue001
 * @tc.desc: Multi-threaded put() on the full queue. When n threads are waiting to reach a certain
 * time-point, everyone puts concurrent to see the status of the queue and the state of the thread.
 */
HWTEST_F(UtilsSafeBlockQueueTracking, testMutilthreadConcurrentPutAndBlockInfullqueue001, TestSize.Level0)
{
    // 1. prepare
    std::thread threads[THREAD_NUM];
    std::array<DemoThreadData, THREAD_NUM> demoDatas;
    demoDatas.fill(DemoThreadData());

    using std::chrono::system_clock;

    std::time_t timeT = system_clock::to_time_t(system_clock::now());
    timeT += 2;
    ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());
    for (unsigned int i = 0; i < QUEUE_SLOTS; i++) {
        int t = i;
        DemoThreadData::shareQueue.Push(t);
    }
    ASSERT_TRUE(DemoThreadData::shareQueue.IsFull());

    // 2. start thread put in full queue
    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        threads[i] = std::thread(PutHandleThreadDataTime, std::ref(demoDatas[i]), i, system_clock::from_time_t(timeT));
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 3. now thread is running and all is blocked
    DemoThreadData tmp = DemoThreadData();
    std::thread joinThread = std::thread(&DemoThreadData::Join, tmp);
    ASSERT_FALSE(demoDatas[0].joinStatus);

    unsigned int pushedIn = 0;
    unsigned int unpushedIn = 0;
    GetThreadDatePushedStatus(demoDatas, pushedIn, unpushedIn);
    ASSERT_EQ(pushedIn, static_cast<unsigned int>(0));
    ASSERT_EQ(unpushedIn, THREAD_NUM);
    ASSERT_TRUE(DemoThreadData::shareQueue.IsFull());
    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        DemoThreadData::shareQueue.Pop();
        DemoThreadData::shareQueue.OneTaskDone();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        ASSERT_TRUE(DemoThreadData::shareQueue.IsFull());
        GetThreadDatePushedStatus(demoDatas, pushedIn, unpushedIn);
        ASSERT_EQ(pushedIn, i + 1);
        ASSERT_EQ(unpushedIn, THREAD_NUM - (i + 1));
    }

    for (auto& t : threads) {
        t.join();
    }

    while (!DemoThreadData::shareQueue.IsEmpty()) {
        demoDatas[0].GetAndOneTaskDone();
    }
    demoDatas[0].joinStatus = false;
    joinThread.join();
}

/*
 * @tc.name: testMutilthreadConcurrentGetAndBlockInblankqueue001
 * @tc.desc: Multi-threaded get() on the empty queue. When n threads are waiting to reach a certain
 * time-point, everyone gets concurrent to see the status of the queue and the state of the thread.
 */
HWTEST_F(UtilsSafeBlockQueueTracking, testMutilthreadConcurrentGetAndBlockInblankqueue001, TestSize.Level0)
{
    // 1. prepare
    std::thread threads[THREAD_NUM];
    std::array<DemoThreadData, THREAD_NUM> demoDatas;
    demoDatas.fill(DemoThreadData());

    using std::chrono::system_clock;

    std::time_t timeT = system_clock::to_time_t(system_clock::now());
    timeT += 2;
    ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());

    // 2. start thread put in empty queue
    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        threads[i] = std::thread(GetAndOneTaskDoneHandleThreadDataTime,
            std::ref(demoDatas[i]), i, system_clock::from_time_t(timeT));
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 3. now thread is running and all is blocked
    unsigned int getedOut = 0;
    unsigned int ungetedOut = 0;
    GetThreadDateGetedStatus(demoDatas, getedOut, ungetedOut);
    ASSERT_EQ(getedOut, static_cast<unsigned int>(0));
    ASSERT_EQ(ungetedOut, THREAD_NUM);
    ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());

    // start thread to join
    DemoThreadData tmp = DemoThreadData();
    std::thread joinThread = std::thread(&DemoThreadData::Join, tmp);
    ASSERT_FALSE(demoDatas[0].joinStatus);

    int value = 1;
    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        DemoThreadData::shareQueue.Push(value);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());
        GetThreadDateGetedStatus(demoDatas, getedOut, ungetedOut);
        ASSERT_EQ(getedOut, i + 1);
        ASSERT_EQ(ungetedOut, THREAD_NUM - (i + 1));
    }

    for (auto& t : threads) {
        t.join();
    }

    while (!DemoThreadData::shareQueue.IsEmpty()) {
        demoDatas[0].GetAndOneTaskDone();
    }

    ASSERT_TRUE(demoDatas[0].joinStatus);
    demoDatas[0].joinStatus = false;
    joinThread.join();
}

/*
 * @tc.name: testMutilthreadConcurrentGetAndBlockInfullqueue001
 * @tc.desc: Multi-threaded get() on the full queue. When n threads are waiting to reach a certain
 * time-point, everyone gets concurrent to see the status of the queue and the state of the thread.
 */
HWTEST_F(UtilsSafeBlockQueueTracking, testMutilthreadConcurrentGetAndBlockInfullqueue001, TestSize.Level0)
{
    // 1. prepare
    std::thread threads[THREAD_NUM];
    std::array<DemoThreadData, THREAD_NUM> demoDatas;
    demoDatas.fill(DemoThreadData());

    using std::chrono::system_clock;

    std::time_t timeT = system_clock::to_time_t(system_clock::now());
    timeT += 2;
    ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());
    int t = 1;
    for (unsigned int i = 0; i < QUEUE_SLOTS; i++) {
        DemoThreadData::shareQueue.Push(t);
    }
    ASSERT_TRUE(DemoThreadData::shareQueue.IsFull());

    // start thread to join
    demoDatas[0].joinStatus = false;
    DemoThreadData tmp = DemoThreadData();
    std::thread joinThread = std::thread(&DemoThreadData::Join, tmp);
    ASSERT_FALSE(demoDatas[0].joinStatus);

    // 2. start thread get in full queue
    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        threads[i] = std::thread(GetAndOneTaskDoneHandleThreadDataTime,
            std::ref(demoDatas[i]), i, system_clock::from_time_t(timeT));
    }

    std::this_thread::sleep_for(std::chrono::seconds(4));

    // 3. start judge
    ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());
    ASSERT_TRUE(demoDatas[0].joinStatus);

    unsigned int getedOut = 0;
    unsigned int ungetedOut = 0;
    GetThreadDateGetedStatus(demoDatas, getedOut, ungetedOut);

    ASSERT_EQ(getedOut, QUEUE_SLOTS);
    ASSERT_EQ(ungetedOut, THREAD_NUM - QUEUE_SLOTS);

    // put one in and wait some get out
    for (unsigned int i = 0; i < THREAD_NUM - QUEUE_SLOTS; i++) {
        demoDatas[0].Put(t);
    }

    demoDatas[0].joinStatus = false;
    DemoThreadData tmp2 = DemoThreadData();
    std::thread joinThread2 = std::thread(&DemoThreadData::Join, tmp2);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // queue is full and some threads is blocked and is not joined
    ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());
    ASSERT_TRUE(demoDatas[0].joinStatus);

    GetThreadDateGetedStatus(demoDatas, getedOut, ungetedOut);
    ASSERT_EQ(getedOut, THREAD_NUM);
    ASSERT_EQ(ungetedOut, static_cast<unsigned int>(0));

    for (auto& t : threads) {
        t.join();
    }

    while (!DemoThreadData::shareQueue.IsEmpty()) {
        demoDatas[0].GetAndOneTaskDone();
    }

    demoDatas[0].joinStatus = false;
    joinThread.join();
    joinThread2.join();
}

/*
 * @tc.name: testMutilthreadConcurrentGetAndBlockInnotfullqueue001
 * @tc.desc: Multi-threaded get() on the notfull queue. When n threads are waiting to reach a certain
 * time-point, everyone get concurrent to see the status of the queue and the state of the thread.
 */
HWTEST_F(UtilsSafeBlockQueueTracking, testMutilthreadConcurrentGetAndBlockInnotfullqueue001, TestSize.Level0)
{
    // 1. prepare
    std::thread threads[THREAD_NUM];
    std::array<DemoThreadData, THREAD_NUM> demoDatas;
    demoDatas.fill(DemoThreadData());

    using std::chrono::system_clock;

    const unsigned int REMAIN_SLOTS = 5;
    std::time_t timeT = system_clock::to_time_t(system_clock::now());
    timeT += 2;
    ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());
    for (unsigned int i = 0; i < QUEUE_SLOTS - REMAIN_SLOTS; i++) {
        int t = i;
        DemoThreadData::shareQueue.Push(t);
    }

    // start thread to join
    demoDatas[0].joinStatus = false;
    DemoThreadData tmp = DemoThreadData();
    std::thread joinThread = std::thread(&DemoThreadData::Join, tmp);
    ASSERT_FALSE(demoDatas[0].joinStatus);

    // 2. start thread put in not full queue
    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        threads[i] = std::thread(GetAndOneTaskDoneHandleThreadDataTime,
            std::ref(demoDatas[i]), i, system_clock::from_time_t(timeT));
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    unsigned int getedOut = 0;
    unsigned int ungetedOut = 0;
    GetThreadDateGetedStatus(demoDatas, getedOut, ungetedOut);
    ASSERT_EQ(getedOut, QUEUE_SLOTS - REMAIN_SLOTS);
    ASSERT_EQ(ungetedOut, THREAD_NUM - (QUEUE_SLOTS - REMAIN_SLOTS));
    ASSERT_TRUE(demoDatas[0].joinStatus);
    // 3. put ungetedOut
    for (unsigned int i = 0; i < ungetedOut; i++) {
        int t = i;
        DemoThreadData::shareQueue.Push(t);
    }
    demoDatas[0].joinStatus = false;
    DemoThreadData tmp2 = DemoThreadData();
    std::thread joinThread2 = std::thread(&DemoThreadData::Join, tmp2);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    GetThreadDateGetedStatus(demoDatas, getedOut, ungetedOut);
    ASSERT_EQ(getedOut, THREAD_NUM);
    ASSERT_EQ(ungetedOut, static_cast<unsigned int>(0));

    for (auto& t : threads) {
        t.join();
    }

    ASSERT_TRUE(demoDatas[0].joinStatus);
    demoDatas[0].joinStatus = false;
    joinThread.join();
    joinThread2.join();
}

/*
 * @tc.name: testMutilthreadConcurrentPutAndBlockInnotfullqueue001
 * @tc.desc: Multi-threaded put() on the not full queue. When n threads are waiting to reach a certain
 * time-point, everyone puts concurrent to see the status of the queue and the state of the thread.
 */
HWTEST_F(UtilsSafeBlockQueueTracking, testMutilthreadConcurrentPutAndBlockInnotfullqueue001, TestSize.Level0)
{
    // 1. prepare
    std::thread threads[THREAD_NUM];
    std::array<DemoThreadData, THREAD_NUM> demoDatas;
    demoDatas.fill(DemoThreadData());

    using std::chrono::system_clock;

    const unsigned int REMAIN_SLOTS = 5;
    std::time_t timeT = system_clock::to_time_t(system_clock::now());
    timeT += 2;
    ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());
    for (unsigned int i = 0; i < QUEUE_SLOTS - REMAIN_SLOTS; i++) {
        int t = i;
        DemoThreadData::shareQueue.Push(t);
    }

    // start thread to join
    demoDatas[0].joinStatus = false;
    DemoThreadData tmp = DemoThreadData();
    std::thread joinThread = std::thread(&DemoThreadData::Join, tmp);
    ASSERT_FALSE(demoDatas[0].joinStatus);

    // 2. start thread put in not full queue
    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        threads[i] = std::thread(PutHandleThreadDataTime, std::ref(demoDatas[i]), i, system_clock::from_time_t(timeT));
    }
    ASSERT_FALSE(demoDatas[0].joinStatus);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    ASSERT_FALSE(demoDatas[0].joinStatus);
    unsigned int putedin = 0;
    unsigned int unputedin = 0;
    GetThreadDatePushedStatus(demoDatas, putedin, unputedin);
    ASSERT_EQ(putedin, REMAIN_SLOTS);
    ASSERT_EQ(unputedin, THREAD_NUM - REMAIN_SLOTS);

    // 3. put ungetedOut
    for (unsigned int i = 0; i < unputedin; i++) {
        DemoThreadData::shareQueue.Pop();
        DemoThreadData::shareQueue.OneTaskDone();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    GetThreadDatePushedStatus(demoDatas, putedin, unputedin);
    ASSERT_EQ(putedin, THREAD_NUM);
    ASSERT_EQ(unputedin, static_cast<unsigned int>(0));

    ASSERT_FALSE(demoDatas[0].joinStatus);

    for (auto& t : threads) {
        t.join();
    }

    while (!DemoThreadData::shareQueue.IsEmpty()) {
        demoDatas[0].GetAndOneTaskDone();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(demoDatas[0].joinStatus);
    joinThread.join();
    demoDatas[0].joinStatus = false;
}

/*
 * @tc.name: testMutilthreadConcurrentGetAndPopInfullqueue001
 * @tc.desc: Multi-threaded put() and Multi-threaded get() on the full queue. When all threads are waiting to reach a
 * certain time-point, everyone run concurrently to see the status of the queue and the state of the thread.
 */
HWTEST_F(UtilsSafeBlockQueueTracking, testMutilthreadConcurrentGetAndPopInfullqueue001, TestSize.Level0)
{
    // 1. prepare
    std::thread threadsout[THREAD_NUM];
    std::array<DemoThreadData, THREAD_NUM> demoDatas;
    demoDatas.fill(DemoThreadData());

    std::thread threadsin[THREAD_NUM];

    using std::chrono::system_clock;

    std::time_t timeT = system_clock::to_time_t(system_clock::now());
    timeT += 2;
    ASSERT_TRUE(DemoThreadData::shareQueue.IsEmpty());
    int t = 1;
    for (unsigned int i = 0; i < QUEUE_SLOTS; i++) {
        DemoThreadData::shareQueue.Push(t);
    }
    ASSERT_TRUE(DemoThreadData::shareQueue.IsFull());

    // start thread to join
    demoDatas[0].joinStatus = false;
    DemoThreadData tmp = DemoThreadData();
    std::thread joinThread = std::thread(&DemoThreadData::Join, tmp);
    ASSERT_FALSE(demoDatas[0].joinStatus);

    // 2. start thread put in not full queue
    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        threadsin[i] = std::thread(PutHandleThreadDataTime,
            std::ref(demoDatas[i]), i, system_clock::from_time_t(timeT));
    }

    for (unsigned int i = 0; i < THREAD_NUM; i++) {
        threadsout[i] = std::thread(GetAndOneTaskDoneHandleThreadDataTime,
            std::ref(demoDatas[i]), i, system_clock::from_time_t(timeT));
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    ASSERT_TRUE(DemoThreadData::shareQueue.IsFull());
    unsigned int getedOut = 0;
    unsigned int ungetedOut = 0;
    unsigned int pushedIn = 0;
    unsigned int unpushedIn = 0;
    GetThreadDateGetedStatus(demoDatas, getedOut, ungetedOut);
    GetThreadDatePushedStatus(demoDatas, pushedIn, unpushedIn);

    ASSERT_EQ(pushedIn, THREAD_NUM);
    ASSERT_EQ(getedOut, THREAD_NUM);

    ASSERT_FALSE(demoDatas[0].joinStatus);
    demoDatas[0].joinStatus = false;

    for (auto& t : threadsout) {
        t.join();
    }

    for (auto& t : threadsin) {
        t.join();
    }

    while (!DemoThreadData::shareQueue.IsEmpty()) {
        demoDatas[0].GetAndOneTaskDone();
    }
    joinThread.join();
    ASSERT_TRUE(demoDatas[0].joinStatus);
    demoDatas[0].joinStatus = false;
}
