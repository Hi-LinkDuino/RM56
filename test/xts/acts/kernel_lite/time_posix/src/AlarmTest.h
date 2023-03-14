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

#ifndef TIME_ALARM_TEST
#define TIME_ALARM_TEST

#include <signal.h>
#include <gtest/gtest.h>

const int ACCURACY_ERROR = 8;
const int MSLEEP_TIME = 1200;
class AlarmTest : public testing::Test {
public:
    void SetUp()
    {
        mReceivedSignal = 0;
    }
    static void SetUpTestCase()
    {
        signal(SIGALRM, SignalHandler);
    }
    static void TearDownTestCase()
    {
        signal(SIGALRM, SIG_DFL);
    }

    static void SignalHandler(int signum);
    static int mReceivedSignal;
};

#endif