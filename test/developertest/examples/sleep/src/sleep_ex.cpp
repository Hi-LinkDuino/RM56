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

#include <sleep_ex.h>

#include <cstdlib>
#include <ctime>
#include <unistd.h>

#define UNUSED(x) (void)(x)

namespace SleepTest {
    const long ID_MS_TO_NS_LEVEL = 1000000; // from milliseconds to nanoseconds
    const int ID_SE_TO_MS_LEVEL = 1000; // from seconds to milliseconds
}

static double TimeDiff(struct timeval *x , struct timeval *y)
{
    if (x == nullptr || y == nullptr) {
        return 0;
    }

    double xUs = reinterpret_cast<double>(x->tv_sec * SleepTest::ID_MS_TO_NS_LEVEL)
     + reinterpret_cast<double>(x->tv_usec);
    double yUs = reinterpret_cast<double>(y->tv_sec * SleepTest::ID_MS_TO_NS_LEVEL)
     + reinterpret_cast<double>(y->tv_usec);

    return (yUs - xUs);
}

static int Nsleep(const struct timespec *req, struct timespec *rem)
{
    struct timespec tempRem;
    if (nanosleep(req, rem) == -1) {
        Nsleep(rem, &tempRem);
        return 0;
    }
    return 1;
}
 
int Msleep(unsigned long miliSec)
{
    struct timespec req = {0, 0};
    struct timespec rem = {0, 0};
    time_t sec = reinterpret_cast<int>(miliSec / SleepTest::ID_SE_TO_MS_LEVEL);
    miliSec = miliSec - (sec * SleepTest::ID_SE_TO_MS_LEVEL);
    req.tv_sec = sec;
    req.tv_nsec = miliSec * SleepTest::ID_MS_TO_NS_LEVEL;
    Nsleep(&req, &rem);
    return 1;
}

static void TmpFunc(const void* arg)
{
    UNUSED(arg);
}

double ElapsedTime(time_callback func, void* arg)
{
    struct timeval start;
    struct timeval stop;
    double diff;
    double selfSpent;

    if (func == nullptr) {
        // error process here.
        return 0.0;
    }

    // calc self spent.
    gettimeofday(&start, 0);
    TmpFunc(nullptr);
    gettimeofday(&stop, 0);
    selfSpent = TimeDiff(&start, &stop);

    gettimeofday(&start, 0);
    func(arg);
    gettimeofday(&stop, 0);
    diff = TimeDiff(&start, &stop);
    
    diff = diff - selfSpent;
    return (diff >= 0) ? diff : 0;
}
