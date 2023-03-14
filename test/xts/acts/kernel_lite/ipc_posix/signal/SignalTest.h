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

#ifndef SIGNAL_TEST
#define SIGNAL_TEST

#include <gtest/gtest.h>
#include <signal.h>

const int MAX_SIGNAL = 31;
const int MAX_DESCRP_LEN = 32;
const int KEEP_RUN_TIME = 150;
enum SignalAction {TERMINATE, COREDUMP, IGNORE, STOP, CONTINUE};
struct SignalNameAction {
    char signame[12];
    char description[MAX_DESCRP_LEN];
    SignalAction action;
};
extern SignalNameAction const ALL_SIGNALS[MAX_SIGNAL];

using handler_type = void (*) (int);

class IpcSignalTest : public::testing::TestWithParam<int> {
public:
    void SetUp()
    {
        mReceivedSignal = 0;
    };
    // fail test util-func for sig api
    void SignalFailTest(int signum, handler_type h, int expectErrno = EINVAL);
    void SigpendingFailTest(sigset_t* pset);
    void SigtimedwaitFailTest(const sigset_t *set, siginfo_t* info,
        const struct timespec* timeout, int expectErrno = EINVAL);

    // utils for all signal test
    void DefaultActionTest(int signum, bool expectStop, bool coredump = false);
    void SendAndRecvTest(int signum);
    int CheckSigString(const char* outfile, const char* expectStr);

    // general signal handler
    static void SignalHandler(int signum);
    // special signal handler for function 'abort'
    static void SigAbortHandler(int signum);
    // special signal handler for function 'sigaction'
    static void SigactionHandler(int signum, siginfo_t* si, void* ucontext);

    static void* ThreadFunc1(void* arg);
    static void* ThreadFunc2(void* arg);
    static void* ThreadFuncForSigmask1(void* arg);
    static void* ThreadFuncForSigmask2(void* arg);

protected:
    static int mReceivedSignal;
    static int mShmid;
    static siginfo_t mSiginfo;
};


#endif