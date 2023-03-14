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

#include "AlarmTest.h"

#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"

using namespace testing::ext;

// static membor must init before use.
int AlarmTest::mReceivedSignal = 0;

// general signal handler
void AlarmTest::SignalHandler(int signum)
{
    LOG("handler recv a signal: %d", signum);
    mReceivedSignal = signum;
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_ALARM_0100
 * @tc.name    alarm function test, cancel alarm. basic alarm function is tested in {signal}
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testAlarmCancel, Function | MediumTest | Level2)
{
    LOG("init alarm");
    int rt = alarm(1);
    EXPECT_EQ(rt, 0);
    Msleep(500);

    rt = alarm(0); // cancel alarm
    EXPECT_EQ(rt, 1);
    KeepRun(600);
    EXPECT_EQ(mReceivedSignal, 0);
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_ALARM_0200
 * @tc.name    alarm function test, multi alarm call test
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testAlarmMultiCall, Function | MediumTest | Level2)
{
    LOG("init alarm");
    int rt = alarm(3);
    EXPECT_EQ(rt, 0);

    sleep(1);
    LOG("set a new alarm");
    rt = alarm(4);
    EXPECT_EQ(rt, 2);
    EXPECT_EQ(mReceivedSignal, 0);

    LOG("sleep 2.5s...");
    Msleep(2500);
    EXPECT_EQ(mReceivedSignal, 0);

    LOG("sleep 2s...");
    Msleep(2000);
    EXPECT_EQ(mReceivedSignal, SIGALRM);
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_ALARM_0300
 * @tc.name    test thar alarm should not reserved to sub process via fork
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testAlarmFork, Function | MediumTest | Level2)
{
    int rt = alarm(1);
    EXPECT_EQ(rt, 0);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        Msleep(MSLEEP_TIME);
        if (mReceivedSignal != 0) {
            if (mReceivedSignal == SIGALRM) {
                LOG("child received SIGALRM!");
            } else {
                LOG("child received an unexpected signal: %d", mReceivedSignal);
            }
            exit(1);
        } else {
            exit(0);
        }
    } else { // parent
        Msleep(MSLEEP_TIME);
        if (mReceivedSignal != SIGALRM) {
            Msleep(500);
        }
        EXPECT_EQ(mReceivedSignal, SIGALRM) << " expect no equal" << errno;
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_UALARM_0100
 * @tc.name    ualarm function create oneshot mode timer
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testUalarmOneshot, Function | MediumTest | Level3)
{
    useconds_t rt = ualarm(50000, 0);
    EXPECT_EQ(rt, 0U) << "ERROR: ualarm return error!";
    Msleep(61);
    LOG("mReceivedSignal = %d, SIGALRM = %d", mReceivedSignal, SIGALRM);
    EXPECT_EQ(mReceivedSignal, SIGALRM) << "ERROR: mReceivedSignal != SIGALRM";
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_UALARM_0200
 * @tc.name    ualarm function create repeate mode timer
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testUalarmRepeate, Function | MediumTest | Level3)
{
    int count = 0;
    useconds_t rt = ualarm(50000, 50000);
    EXPECT_EQ(rt, 0U) << "ERROR: ualarm return error!";
    while (true) {
        if (mReceivedSignal == SIGALRM) {
            count++;
            if (count > 3) {
                break;
            }
            mReceivedSignal = 0;
        }
        Msleep(10);
    }

    LOG("mReceivedSignal = %d, SIGALRM = %d", mReceivedSignal, SIGALRM);
    EXPECT_EQ(mReceivedSignal, SIGALRM) << "ERROR: mReceivedSignal != SIGALRM";
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_UALARM_0300
 * @tc.name    ualarm function stop alarm test
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testUalarmStop, Function | MediumTest | Level3)
{
    int ret;

    mReceivedSignal = 0;
    ualarm(50000, 0);
    ret = ualarm(0, 0);
    LOG("ret = %d", ret);
    EXPECT_GE(ret, 50000) << "ERROR: ret < 50000";
    ret = ualarm(0, 0);
    LOG("ret = %d", ret);
    EXPECT_EQ(ret, 0) << "ERROR: ret != 0";
    Msleep(100);
    EXPECT_EQ(mReceivedSignal, 0) << "ERROR: mReceivedSignal != 0";

    ualarm(50000, 0);
    Msleep(20);
    ret = ualarm(0, 0);
    LOG("ret = %d", ret);
    EXPECT_LE(ret, 30000) << "ERROR: ret < 30000";
    Msleep(40);
    EXPECT_EQ(mReceivedSignal, 0) << "ERROR: mReceivedSignal != 0";

    ret = ualarm(0, 0);
    LOG("ret = %d", ret);
    EXPECT_EQ(ret, 0) << "ERROR: ret != 0";
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_UALARM_0400
 * @tc.name    ualarm function errno for ENOMEM test
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testUalarmEINVAL, Function | MediumTest | Level3)
{
    unsigned long OverMaxNum = 1000000;

    errno = 0;
    ualarm(OverMaxNum, 0);
    EXPECT_EQ(errno, EINVAL) << "ERROR: errno != EINVAL";

    errno = 0;
    ualarm(OverMaxNum, OverMaxNum);
    EXPECT_EQ(errno, EINVAL) << "ERROR: errno != EINVAL";

    errno = 0;
    ualarm(0, OverMaxNum);
    EXPECT_EQ(errno, EINVAL) << "ERROR: errno != EINVAL";
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_TIMER_CREATE_0100
 * @tc.name    timer_create function create a timer for give signal test
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testTimerCreateEventSignal, Function | MediumTest | Level3)
{
    timer_t tid = nullptr;
    struct sigevent ev = {0};
    struct itimerspec its = {0};

    ASSERT_NE(signal(SIGUSR1, SignalHandler), SIG_ERR) << "ERROR: signal()";
    ev.sigev_signo = SIGUSR1;
    ev.sigev_notify = SIGEV_SIGNAL;
    EXPECT_EQ(timer_create(CLOCK_REALTIME, &ev, &tid), 0) << "ERROR: timer_create() != 0";
    LOG("tid = %p", tid);

    its.it_value.tv_sec     = 0;
    its.it_value.tv_nsec    = 50000000;  // 50 millisecond
    its.it_interval.tv_sec  = 0;
    its.it_interval.tv_nsec = 0;
    EXPECT_EQ(timer_settime(tid, 0, &its, NULL), 0) << "ERROR: timer_settime() != 0";

    uint32_t setMillisec = its.it_value.tv_sec * 1000 + its.it_value.tv_nsec / 1000000;
    Msleep(setMillisec + ACCURACY_ERROR);
    EXPECT_EQ(mReceivedSignal, ev.sigev_signo) << "mReceivedSignal != ev.sigev_signo";
    EXPECT_EQ(timer_delete(tid), 0) << "ERROR: timer_delete() != 0";

    // restore
    signal(SIGUSR1, SIG_DFL);
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_TIMER_CREATE_0200
 * @tc.name    timer_create function test
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testTimerCreateEventDefault, Function | MediumTest | Level3)
{
    timer_t tid = nullptr;
    uint32_t setMillisec = 0;
    struct itimerspec its = {0};

    EXPECT_EQ(timer_create(CLOCK_REALTIME, NULL, &tid), 0) << "ERROR: timer_create() != 0";
    LOG("tid = %p", tid);

    its.it_value.tv_sec     = 0;
    its.it_value.tv_nsec    = 50000000;  // 50 millisecond
    its.it_interval.tv_sec  = 0;
    its.it_interval.tv_nsec = 0;
    EXPECT_EQ(timer_settime(tid, 0, &its, NULL), 0) << "ERROR: timer_settime() != 0";

    setMillisec = its.it_value.tv_sec * 1000 + its.it_value.tv_nsec / 1000000;
    LOG("setMillisec = %u", setMillisec);
    Msleep(setMillisec + ACCURACY_ERROR);
    EXPECT_EQ(mReceivedSignal, SIGALRM) << "mReceivedSignal != SIGALRM";

    mReceivedSignal = 0;
    /* 1 second */
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    EXPECT_EQ(timer_settime(tid, 0, &its, NULL), 0) << "ERROR: timer_settime() != 0";

    setMillisec = its.it_value.tv_sec * 1000 + its.it_value.tv_nsec / 1000000;
    LOG("setMillisec = %u", setMillisec);
    Msleep(setMillisec + ACCURACY_ERROR);

    EXPECT_EQ(mReceivedSignal, SIGALRM) << "mReceivedSignal != SIGALRM";
    EXPECT_EQ(timer_delete(tid), 0) << "ERROR: timer_delete() != 0";
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_TIMER_CREATE_0300
 * @tc.name    timer_create function create a timer for unique identify test
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testTimerCreateUniqueId, Function | MediumTest | Level2)
{
    int i, k, ret;
    const int max = 32;
    timer_t tid = NULL;
    timer_t tidArr[max];

    for (i = 0; i < max; i++) {
        tidArr[i] = (timer_t)-1;
    }

    for (k = 0; k < max; k++) {
        ret = timer_create(CLOCK_REALTIME, nullptr, &tid);
        EXPECT_EQ(ret, 0) << "ERROR: timer_create() != 0";

        for (i = 0; i < max; i++) {
            if (tid == tidArr[i]) {
                break;
            }
        }
        EXPECT_EQ(i, max) << "ERROR: i < max that timer id already exist";

        for (i = 0; i < max; i++) {
            if (tidArr[i] == ((timer_t)-1)) {
                break;
            }
        }
        EXPECT_LT(i, max) << "ERROR: i == max that timer id is full";

        if (i < max) {
            tidArr[i] = tid;
        }
    }

    for (k = 0; k < max; k++) {
        if (tidArr[k] != (timer_t)-1) {
            ret = timer_delete(tidArr[k]);
            EXPECT_EQ(ret, 0) << "ERROR: timer_delete() != 0";
        }
    }
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_TIMER_CREATE_0400
 * @tc.name    timer_create function that timer shall not be inherited by a child process across a fork
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testTimerCreateFork, Function | MediumTest | Level2)
{
    timer_t tid = nullptr;
    struct itimerspec its = {0};

    ASSERT_EQ(timer_create(CLOCK_REALTIME, NULL, &tid), 0) << "> timer_create fail, errno = " << errno;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 50000000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    EXPECT_EQ(timer_settime(tid, 0, &its, NULL), 0) << "ERROR: timer_settime() != 0";

    uint32_t setMillisec = its.it_value.tv_sec * 1000 + its.it_value.tv_nsec / 1000000;
    LOG("setMillisec = %u", setMillisec);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        Msleep(1100);
        if (mReceivedSignal == 0) {
            LOG("child process did not inherit timer!");
            exit(0);
        } else {
            LOG("child received an unexpected signal: %d", mReceivedSignal);
            exit(1);
        }
    } else { // parent
        Msleep(setMillisec + ACCURACY_ERROR);
        EXPECT_EQ(mReceivedSignal, SIGALRM);
        WaitProcExitedOK(pid);
        EXPECT_EQ(timer_delete(tid), 0) << "ERROR: timer_delete() != 0";
    }
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_TIMER_CREATE_0500
 * @tc.name    timer_create function errno for EINVAL test
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testTimerCreateEINVAL, Function | MediumTest | Level4)
{
    timer_t tid = nullptr;
    clockid_t clockid = GetRandom(2048);

    EXPECT_EQ(timer_create(clockid, NULL, &tid), -1) << "ERROR: timer_create() != -1";
    EXPECT_EQ(errno, EINVAL) << "ERROR: errno != EINVAL, errno = " << errno << " EINVAL = " << EINVAL;
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_TIMER_GETTIME_0100
 * @tc.name    timer_gettime function create a timer and get time test
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testTimerGetTime, Function | MediumTest | Level3)
{
    timer_t tid = nullptr;
    struct sigevent ev = {0};
    struct itimerspec its = {0};
    struct itimerspec getIts = {0};
    int index = 0;
    uint32_t getMsValue[20];
    const uint32_t delay = 200;

    ASSERT_NE(signal(SIGINT, SignalHandler), SIG_ERR) << "ERROR: signal()";
    ev.sigev_signo = SIGINT;
    ev.sigev_notify = SIGEV_SIGNAL;
    EXPECT_EQ(timer_create(CLOCK_REALTIME, &ev, &tid), 0) << "ERROR: timer_create() != 0";
    LOG("tid = %p", tid);

    its.it_value.tv_sec = 2;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    EXPECT_EQ(timer_settime(tid, 0, &its, NULL), 0) << "ERROR: timer_settime() != 0";
    EXPECT_EQ(timer_gettime(tid, &getIts), 0) << "ERROR: timer_gettime() != 0";

    uint32_t setMillisec = its.it_value.tv_sec * 1000 + its.it_value.tv_nsec / 1000000;
    uint32_t getMillisec = getIts.it_value.tv_sec * 1000 + getIts.it_value.tv_nsec / 1000000;
    LOG("setMillisec = %u, getMillisecv = %u", setMillisec, getMillisec);
    EXPECT_LE(getMillisec, setMillisec);
    LOG("%u, %u, %u", setMillisec, getMillisec, getMillisec - setMillisec);

    while (true) {
        Msleep(delay);
        EXPECT_EQ(timer_gettime(tid, &getIts), 0) << "ERROR: timer_gettime() != 0";
        getMillisec = getIts.it_value.tv_sec * 1000 + getIts.it_value.tv_nsec / 1000000;
        if (getMillisec == 0) {
            break;
        }
        getMsValue[index++] = getMillisec;
    }
    Msleep(150);

    for (int i = 0; i < index; i++) {
        /* delay should add 10 millisecond to ajust */
        if (setMillisec < (delay + 10)) {
            break;
        }
        setMillisec -= delay + 10;
        EXPECT_GE(getMsValue[i], setMillisec);
        LOG("%u, %u, %u", setMillisec, getMsValue[i], getMsValue[i] - setMillisec);
    }
    EXPECT_EQ(mReceivedSignal, ev.sigev_signo) << "mReceivedSignal != ev.sigev_signo";
    EXPECT_EQ(timer_delete(tid), 0) << "ERROR: timer_delete() != 0";

    // restore
    signal(SIGINT, SIG_DFL);
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_TIMER_GETOVERRUN_0100
 * @tc.name    timer_getoverrun function create a timer for SIGALRM signal and get overrun time test
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testTimerGetOverrun, Function | MediumTest | Level3)
{
    timer_t tid = nullptr;
    sigset_t mask;
    struct sigevent ev = {0};
    struct itimerspec its = {0};

    /* Block timer signal temporarily */
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    EXPECT_NE(sigprocmask(SIG_SETMASK, &mask, NULL), -1) << "ERROR: sigprocmask() == -1";

    ev.sigev_signo = SIGALRM;
    ev.sigev_notify = SIGEV_SIGNAL;
    EXPECT_EQ(timer_create(CLOCK_REALTIME, &ev, &tid), 0) << "ERROR: timer_create() != 0";

    its.it_value.tv_sec     = 0;
    its.it_value.tv_nsec    = 50000000;  // 50 millisecond
    its.it_interval.tv_sec  = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    EXPECT_EQ(timer_settime(tid, 0, &its, NULL), 0) << "ERROR: timer_settime() != 0";

    int setMillisec = its.it_value.tv_sec * 1000 + its.it_value.tv_nsec / 1000000;
    LOG("setMillisec = %u", setMillisec);
    Msleep(150);
    EXPECT_NE(sigprocmask(SIG_UNBLOCK, &mask, NULL), -1) << "ERROR: sigprocmask() == -1";

    int overrun = timer_getoverrun(tid);
    LOG("timer_getoverrun(tid) = %d", overrun);

    EXPECT_GE(overrun, 2) << "ERROR: timer_getoverrun(tid) < 2";
    EXPECT_EQ(timer_delete(tid), 0) << "ERROR: timer_delete() != 0";
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_SETITIMER_0100
 * @tc.name    setitimer function create oneshot mode timer
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testSetItTimerOneshot, Function | MediumTest | Level3)
{
    struct itimerval setItv = {0};
    uint32_t setMillisec;

    /* 50 millisecond */
    setItv.it_value.tv_sec = 0;
    setItv.it_value.tv_usec = 50000;
    setItv.it_interval.tv_sec = 0;
    setItv.it_interval.tv_usec = 0;
    EXPECT_EQ(setitimer(ITIMER_REAL, &setItv, NULL), 0) << "ERROR: setitimer() != 0";

    setMillisec = setItv.it_value.tv_sec * 1000 + setItv.it_value.tv_usec / 1000;
    Msleep(setMillisec + ACCURACY_ERROR);
    LOG("mReceivedSignal = %d, SIGALRM = %d", mReceivedSignal, SIGALRM);
    EXPECT_EQ(mReceivedSignal, SIGALRM) << "ERROR: mReceivedSignal != SIGALRM";

    mReceivedSignal = 0;
    /* 1 second */
    setItv.it_value.tv_sec = 1;
    setItv.it_value.tv_usec = 0;
    setItv.it_interval.tv_sec = 0;
    setItv.it_interval.tv_usec = 0;
    EXPECT_EQ(setitimer(ITIMER_REAL, &setItv, NULL), 0) << "ERROR: setitimer() != 0";
    setMillisec = setItv.it_value.tv_sec * 1000 + setItv.it_value.tv_usec / 1000;
    Msleep(setMillisec + ACCURACY_ERROR);
    LOG("mReceivedSignal = %d, SIGALRM = %d", mReceivedSignal, SIGALRM);
    EXPECT_EQ(mReceivedSignal, SIGALRM) << "ERROR: mReceivedSignal != SIGALRM";
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_SETITIMER_0200
 * @tc.name    setitimer function create repeate mode timer
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testSetItTimerRepeate, Function | MediumTest | Level3)
{
    int count = 0;
    struct itimerval setItv = {0};

    /* 50 millisecond */
    setItv.it_value.tv_sec = 0;
    setItv.it_value.tv_usec = 50000;
    setItv.it_interval.tv_sec = setItv.it_value.tv_sec;
    setItv.it_interval.tv_usec = setItv.it_value.tv_usec;
    EXPECT_EQ(setitimer(ITIMER_REAL, &setItv, NULL), 0) << "ERROR: setitimer() != 0";

    uint32_t setMillisec = setItv.it_value.tv_sec * 1000 + setItv.it_value.tv_usec / 1000;
    while (true) {
        count++;
        Msleep(setMillisec + ACCURACY_ERROR);
        if (mReceivedSignal == SIGALRM) {
            count++;
            if (count > 3) {
                break;
            }
            mReceivedSignal = 0;
        }
    }

    LOG("mReceivedSignal = %d, SIGALRM = %d", mReceivedSignal, SIGALRM);
    EXPECT_EQ(mReceivedSignal, SIGALRM) << "ERROR: mReceivedSignal != SIGALRM";
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_SETITIMER_0300
 * @tc.name    setitimer function that cancel timer
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testCancelTimer, Function | MediumTest | Level3)
{
    struct itimerval setItv = {0};

    LOG("start a timer");
    setItv.it_value.tv_sec = 0;
    setItv.it_value.tv_usec = 1000;
    setItv.it_interval.tv_sec = 0;
    setItv.it_interval.tv_usec = 1000;
    EXPECT_EQ(setitimer(ITIMER_REAL, &setItv, NULL), 0) << "setitimer fail, errno = " << errno;
    KeepRun(50);
    EXPECT_EQ(mReceivedSignal, SIGALRM);

    LOG("cancel timer");
    setItv.it_value.tv_usec = 0;
    EXPECT_EQ(setitimer(ITIMER_REAL, &setItv, NULL), 0) << "setitimer fail, errno = " << errno;
    mReceivedSignal = 0;
    KeepRun(100);
    EXPECT_EQ(mReceivedSignal, 0);
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_SETITIMER_0400
 * @tc.name    setitimer function test which ovalue is not null
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testSetItTimerOldvalue, Function | MediumTest | Level3)
{
    struct itimerval setItv = {0};
    struct itimerval oldItv = {0};
    uint32_t setMillisec;

    setItv.it_value.tv_sec = 1;
    setItv.it_value.tv_usec = 0;
    setItv.it_interval.tv_sec = 0;
    setItv.it_interval.tv_usec = 100000;
    EXPECT_EQ(setitimer(ITIMER_REAL, &setItv, NULL), 0) << "ERROR: setitimer() != 0";
    setMillisec = setItv.it_value.tv_sec * 1000 + setItv.it_value.tv_usec / 1000;
    usleep((setMillisec + ACCURACY_ERROR) * 1000);
    LOG("mReceivedSignal = %d, SIGALRM = %d", mReceivedSignal, SIGALRM);
    EXPECT_EQ(mReceivedSignal, SIGALRM) << "ERROR: mReceivedSignal != SIGALRM";

    EXPECT_EQ(setitimer(ITIMER_REAL, &setItv, &oldItv), 0) << "setitimer fail, errno = " << errno;
    uint32_t oldMillisec = oldItv.it_value.tv_sec * 1000 + oldItv.it_value.tv_usec / 1000;
    EXPECT_GE(setMillisec, oldMillisec);
    EXPECT_EQ(oldItv.it_interval.tv_sec, 0);
    EXPECT_EQ(oldItv.it_interval.tv_usec, 100000);
}

/**
 * @tc.number  SUB_KERNEL_TIME_API_GETITIMER_0100
 * @tc.name    getitimer function create a timer and get time test
 * @tc.desc    [C- SOFTWARE -0200]
 */
HWTEST_F(AlarmTest, testGetItTimer, Function | MediumTest | Level3)
{
    struct itimerval setItv = {0};
    struct itimerval getItv = {0};
    int index = 0;
    uint32_t getMsValue[20];
    const uint32_t delay = 200;

    /* 50 millisecond */
    setItv.it_value.tv_sec = 2;
    setItv.it_value.tv_usec = 0;
    setItv.it_interval.tv_sec = 0;
    setItv.it_interval.tv_usec = 0;
    EXPECT_EQ(setitimer(ITIMER_REAL, &setItv, NULL), 0) << "ERROR: setitimer() != 0";
    EXPECT_EQ(getitimer(ITIMER_REAL, &getItv), 0) << "ERROR: getitimer() != 0";

    uint32_t setMillisec = setItv.it_value.tv_sec * 1000 + setItv.it_value.tv_usec / 1000;
    uint32_t getMillisec = getItv.it_value.tv_sec * 1000 + getItv.it_value.tv_usec / 1000;
    EXPECT_LE(getMillisec, setMillisec);
    LOG("%u, %u, %u", setMillisec, getMillisec, getMillisec - setMillisec);

    while (true) {
        Msleep(delay);
        EXPECT_EQ(getitimer(ITIMER_REAL, &getItv), 0) << "ERROR: getitimer() != 0";
        getMillisec = getItv.it_value.tv_sec * 1000 + getItv.it_value.tv_usec / 1000;
        if (getMillisec == 0) {
            break;
        }
        getMsValue[index++] = getMillisec;
    }
    Msleep(150);

    for (int i = 0; i < index; i++) {
        /* delay should add 10 millisecond to ajust */
        if (setMillisec < (delay + 10)) {
            break;
        }
        setMillisec -= delay + 10;
        EXPECT_GE(getMsValue[i], setMillisec);
        LOG("%u, %u, %u", setMillisec, getMsValue[i], getMsValue[i] - setMillisec);
    }
    LOG("mReceivedSignal = %d, SIGALRM = %d", mReceivedSignal, SIGALRM);
    EXPECT_EQ(mReceivedSignal, SIGALRM) << "ERROR: mReceivedSignal != SIGALRM";
}
