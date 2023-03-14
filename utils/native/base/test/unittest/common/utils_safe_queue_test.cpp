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
#include "safe_queue.h"

#include <array>
#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <chrono>   // std::chrono::seconds

using namespace testing::ext;
using namespace OHOS;
using namespace std;

class UtilsSafeQueue : public testing::Test
{
};

const unsigned int QUEUE_SLOTS = 10;
const unsigned int THREAD_NUM = QUEUE_SLOTS + 1;

class DemoThreadData
{
public:
    DemoThreadData()
    {
        putStatus = false;
        getStatus = false;
    }
    static SafeQueue<int> shareQueue;
    bool putStatus;
    bool getStatus;

    void Put(int i)
    {
        shareQueue.Push(i);
        putStatus = true;
    }

    void Get(int &i)
    {
        shareQueue.Pop(i);
        getStatus = true;
    }
};
SafeQueue<int> DemoThreadData::shareQueue;

void PutHandleThreadDataTime(DemoThreadData &q, int i, std::chrono::system_clock::time_point absTime)
{
    cout << "thread-" << std::this_thread::get_id() << " run time: "
        << std::chrono::system_clock::to_time_t(absTime) << endl;
    std::this_thread::sleep_until(absTime);

    q.Put(i);
}

void GetHandleThreadDataTime(DemoThreadData &q, int i, std::chrono::system_clock::time_point absTime)
{
    cout << "thread-" << std::this_thread::get_id() << " run time: "
        << std::chrono::system_clock::to_time_t(absTime) << endl;
    std::this_thread::sleep_until(absTime);
    int t = 0;
    q.Get(t);
}

class TestThreading
{

public:
    TestThreading()
    {
        demoDatas.fill(DemoThreadData());
    }

    void AllThreadPut(std::time_t &timeT)
    {
        using std::chrono::system_clock;
        for (unsigned int i = 0; i < THREAD_NUM; i++)
        {
            threads[i] = std::thread(PutHandleThreadDataTime,
                std::ref(demoDatas[i]), i, system_clock::from_time_t(timeT));
        }
    }
    void AllThreadGet(std::time_t &timeT)
    {
        using std::chrono::system_clock;
        for (unsigned int i = 0; i < THREAD_NUM; i++)
        {
            threads[i] = std::thread(GetHandleThreadDataTime,
                std::ref(demoDatas[i]), i, system_clock::from_time_t(timeT));
        }
    }

    void GetThreadDatePushedStatus(unsigned int &pushedIn, unsigned int &unpushedIn)
    {
        pushedIn = 0;
        unpushedIn = 0;
        for (auto &t : demoDatas)
        {
            if (t.putStatus)
                pushedIn++;
            else
            {
                unpushedIn++;
            }
        }
    }

    void GetThreadDateGetedStatus(unsigned int &getedOut, unsigned int &ungetedOut)
    {
        getedOut = 0;
        ungetedOut = 0;
        for (auto &t : demoDatas)
        {
            if (t.getStatus)
                getedOut++;
            else
            {
                ungetedOut++;
            }
        }
    }

    void ResetStatus()
    {
        for (auto &t : threads)
        {
            t.join();
        }

        DemoThreadData::shareQueue.Clear();
    }

    std::thread threads[THREAD_NUM];
    std::array<DemoThreadData, THREAD_NUM> demoDatas;
};

/*
* Feature: SafeBlockQueue
* Function:put
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: Multiple threads put , one thread gets, all threads finish running normally
*/

HWTEST_F(UtilsSafeQueue, testMutilthreadPutAndOneThreadGetOnemptyQueue, TestSize.Level0)
{

    TestThreading testThread;
    using std::chrono::system_clock;

    std::time_t timeT = system_clock::to_time_t(system_clock::now());
    timeT += 2;
    testThread.AllThreadPut(timeT);

    // 1. queue is full and some threads is blocked
    std::this_thread::sleep_for(std::chrono::seconds(3));
    ASSERT_TRUE(DemoThreadData::shareQueue.Size() > 0);

    unsigned int pushedIn = 0;
    unsigned int unpushedIn = 0;

    testThread.GetThreadDatePushedStatus(pushedIn, unpushedIn);

    ASSERT_EQ(pushedIn, THREAD_NUM);

    //2.  get one out  and wait some put in
    for (unsigned int i = 0; i < THREAD_NUM; i++)
    {
        int t = 0;
        testThread.demoDatas[0].Get(t);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    // queue is full and some threads is blocked and is not joined
    ASSERT_TRUE(DemoThreadData::shareQueue.Size() == 0);

    // here means all thread end ok or if some operation blocked and the testcase blocked
    testThread.ResetStatus();
}

/*
* Feature: SafeBlockQueue
* Function:put
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: Multi-threaded put() and Multi-threaded get() on the empty queue. When all threads are waiting to reach a certain
* time-point, everyone run concurrently to see the status of the queue and the state of the thread.
*/

HWTEST_F(UtilsSafeQueue, testMutilthreadPutAndGetConcurrently, TestSize.Level0)
{

    using std::chrono::system_clock;

    std::time_t timeT = system_clock::to_time_t(system_clock::now());
    timeT += 2;

    TestThreading putInTestThread;
    putInTestThread.AllThreadPut(timeT);

    TestThreading getOutTestThread;
    getOutTestThread.AllThreadGet(timeT);

    // 1. queue is full and some threads is blocked
    std::this_thread::sleep_for(std::chrono::seconds(4));

    unsigned int pushedIn = 0;
    unsigned int unpushedIn = 0;
    unsigned int getedOut = 0;
    unsigned int ungetedOut = 0;
    putInTestThread.GetThreadDatePushedStatus(pushedIn, unpushedIn);
    getOutTestThread.GetThreadDateGetedStatus(getedOut, ungetedOut);
    cout << "DemoThreadData::shareQueue.Size() = " << DemoThreadData::shareQueue.Size() << endl;
    ASSERT_EQ(pushedIn, THREAD_NUM);
    ASSERT_EQ(getedOut, THREAD_NUM);

    putInTestThread.ResetStatus();
    getOutTestThread.ResetStatus();
}

/*
* Feature: SafeBlockQueue
* Function:put
* SubFunction: NA
* FunctionPoints:
* EnvConditions: NA
* CaseDescription: Multi-threaded put() and Multi-threaded get() on the not empty queue. When all threads are waiting to reach a certain
* time-point, everyone run concurrently to see the status of the queue and the state of the thread.
*/
HWTEST_F(UtilsSafeQueue, testMutilthreadConcurrentGetAndPopInNotEmptyQueue, TestSize.Level0)
{
    //1. prepare
    using std::chrono::system_clock;

    std::time_t timeT = system_clock::to_time_t(system_clock::now());
    timeT += 2;

    ASSERT_TRUE(DemoThreadData::shareQueue.Size() == 0);
    int t = 1;
    for (unsigned int i = 0; i < THREAD_NUM; i++)
    {
        DemoThreadData::shareQueue.Push(t);
    }

    ASSERT_TRUE(DemoThreadData::shareQueue.Size() == THREAD_NUM);

    //2. start thread put in not full queue

    TestThreading putInTestThread;
    putInTestThread.AllThreadPut(timeT);

    TestThreading getOutTestThread;
    getOutTestThread.AllThreadGet(timeT);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    ASSERT_TRUE(DemoThreadData::shareQueue.Size() == THREAD_NUM);

    unsigned int getedOut = 0;
    unsigned int ungetedOut = 0;
    unsigned int pushedIn = 0;
    unsigned int unpushedIn = 0;
    getOutTestThread.GetThreadDateGetedStatus(getedOut, ungetedOut);
    putInTestThread.GetThreadDatePushedStatus(pushedIn, unpushedIn);

    ASSERT_EQ(pushedIn, THREAD_NUM);
    ASSERT_EQ(getedOut, THREAD_NUM);

    // 3. reset status
    putInTestThread.ResetStatus();
    getOutTestThread.ResetStatus();
}
