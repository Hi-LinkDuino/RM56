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

#include "SignalTest.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "log.h"
#include "utils.h"
#include "KernelConstants.h"

using namespace testing::ext;

/**
 * @tc.number SUB_KERNEL_IPC_SIG_ALL_0100
 * @tc.name   test the default action when receive a signal
              default action: http://man7.org/linux/man-pages/man7/signal.7.html
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_P(IpcSignalTest, testAllSigDefaultAction, Function | MediumTest | Level1)
{
    int index = GetParam();
    SignalAction a = ALL_SIGNALS[index].action;
    bool expectStop, coredump;
    if (a == STOP) { // SIGSTOP should has no effect in liteos
        expectStop = false;
        coredump = false;
    } else if (a == COREDUMP) {
        expectStop = true;
        coredump = true;
    } else if (a == TERMINATE) {
        expectStop = true;
        coredump = false;
    } else {
        // CONTINUE's default action is ignore, if the process is not Stopped
        expectStop = false;
        coredump = false;
    }
    LOG("all supported signal DefaultActionTest test %d: %s, expectStop=%d",
        index, ALL_SIGNALS[index].signame, expectStop);
    DefaultActionTest(index, expectStop, coredump);
    Msleep(100); // don't test too fast
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_ALL_0200
 * @tc.name   test all supported signal's send and receive. SIGKILL, SIGSTOP SIGCONT is special, not tested here
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_P(IpcSignalTest, testAllSigSendAndRecv, Function | MediumTest | Level1)
{
    int index = GetParam();
    // SIGKILL/SIGSTOP cannot be caught, blocked, or ignored, can not test here
    if (index==SIGKILL || index==SIGSTOP || index==SIGCONT) {
        return;
    }
    LOG("all supported signal SendAndRecv test %d: %s", index, ALL_SIGNALS[index].signame);
    SendAndRecvTest(index);
    Msleep(100); // don't test too fast
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_ALL_0250
 * @tc.name   test all signal's description string
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_P(IpcSignalTest, testAllSigDescription, Function | MediumTest | Level3)
{
    int signo = GetParam();
    const char* descipt = ALL_SIGNALS[signo].description;
    LOG("all supported signal's description string test: %d(%s)", signo, ALL_SIGNALS[signo].signame);

    // we use child to do the test here, cause 'restore of stderr' is not work by now.
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        // redirect stderr to a file, so we can check the content.
        char* outfile = WRITABLE_TEST_DIR "stderr.txt";
        if (freopen(outfile, "w", stderr) == NULL) {
            LOG("redirect stderr fail, freopen errno=%d\n", errno);
            exit(1);
        }
        psignal(signo, nullptr);
        int rt = CheckSigString(outfile, descipt);
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);
    }
    Msleep(100); // don't test too fast
}
INSTANTIATE_TEST_CASE_P(AllSignalTest, IpcSignalTest, testing::Range(1, MAX_SIGNAL));

/**
 * @tc.number SUB_KERNEL_IPC_SIG_ALL_0300
 * @tc.name   test SIGKILL can't blocked. SIGSTOP can't be blocked too, but not supported by liteos
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testBlockSIGKILL, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid > 0) { // parent
        LOG("parent in");
        Msleep(50);
        LOGD("parent: before kill");
        kill(pid, SIGKILL);
        Msleep(150); // wait to exit
        WaitProcKilled(pid, SIGKILL);
    } else { // child
        LOG("child in");
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGKILL);
        int rt = sigprocmask(SIG_BLOCK, &set, nullptr);
        LOG("sigprocmask rt = %d", rt);
        KeepRun(100);
        LOG("child exit 0");
        exit(0);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_ALL_0400
 * @tc.name   test SIGKILL can't suspended.
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSuspendSIGKILL, Function | MediumTest | Level3)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid > 0) { // parent
        Msleep(20);
        kill(pid, SIGKILL);
        Msleep(150); // wait to exit
        WaitProcKilled(pid, SIGKILL);
    } else { // child
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGKILL);
        LOG("before sigsuspend");
        int rt = sigsuspend(&set);
        LOG("sigsuspend rt = %d", rt);
        KeepRun(100);
        exit(0);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGNAL_0100
 * @tc.name   test SIGKILL and SIGSTOP's action can not changed by signal
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSignalFailSig, Function | MediumTest | Level2)
{
    SignalFailTest(SIGKILL, SignalHandler);
    SignalFailTest(SIGKILL, SIG_IGN);
    SignalFailTest(SIGSTOP, SignalHandler);
    SignalFailTest(SIGSTOP, SIG_IGN);
    SignalFailTest(31, SignalHandler);
    SignalFailTest(32, SignalHandler);
    SignalFailTest(33, SignalHandler);
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGNAL_0200
 * @tc.name   test SIG_IGN and SIG_DFL
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSignal_DFL_IGN, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        LOG("change handler to SIG_IGN");
        signal(SIGURG, SIG_IGN);
        Msleep(40);
        if (mReceivedSignal != 0) {
            LOG("Received signal check fail, expected 0, but get %d", mReceivedSignal);
            exitCode = 1;
        }

        LOG("change handler to SignalHandler");
        signal(SIGURG, SignalHandler);
        mReceivedSignal = 0;
        Msleep(50);
        if (mReceivedSignal != SIGURG) {
            LOG("Received signal check fail, expected %d, but get %d", SIGURG, mReceivedSignal);
            exitCode = 1;
        }

        LOG("change handler to SIG_DFL");
        signal(SIGURG, SIG_DFL);
        mReceivedSignal = 0;
        Msleep(50);
        if (mReceivedSignal != 0) {
            LOG("Received signal check fail, expected 0, but get %d", mReceivedSignal);
            exitCode = 1;
        }
        exit(exitCode);
    } else { // parent
        Msleep(20);
        LOG("------ Send SIGURG 1");
        kill(pid, SIGURG);
        Msleep(50);
        LOG("------ Send SIGURG 2");
        kill(pid, SIGURG);
        Msleep(50);
        LOG("------ Send SIGURG 3");
        kill(pid, SIGURG);
        Msleep(50);
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_RAISE_0100
 * @tc.name   raise basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testRaiseBasic, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        signal(SIGQUIT, SignalHandler);
        raise(SIGQUIT);
        if (mReceivedSignal != SIGQUIT) {
            LOG("Received signal check fail, expected signal=%d", SIGQUIT);
            exitCode = 1;
        }
        exit(exitCode);
    } else { // parent
        Msleep(30);
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_ABORT_0100
 * @tc.name   abort function test: SIGABRT handler
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testAbortHandler, Function | MediumTest | Level1)
{
    const int memSize = 32;
    mShmid = shmget(IPC_PRIVATE, memSize, 0666 | IPC_CREAT);
    if (mShmid == -1){
        LOG("shmget errno = %d\n", errno);
        ADD_FAILURE();
    }

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        Msleep(30);
        signal(SIGABRT, SigAbortHandler);
        // try BLOCK SIGABRT
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGABRT);
        sigprocmask(SIG_BLOCK, &set, nullptr);

        LOG("before abort");
        abort();
        // should never get here
        LOG("after abort");
        exit(1);
    } else { // parent
        Msleep(50);
        WaitProcKilled(pid, SIGABRT);

        int *shared = static_cast<int*>(shmat(mShmid, NULL, 0));
        if (shared == reinterpret_cast<int*>(-1)) {
            LOG("shmat fail, errno = %d", errno);
            ADD_FAILURE();
        } else if (*shared != SIGABRT) {
            LOG("Received signal check fail, expected signal=%d", SIGABRT);
            ADD_FAILURE();
        }
        shmdt(shared);
        shmctl(mShmid, IPC_RMID, nullptr);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGACTION_0100
 * @tc.name   sigaction function test: read siginfo
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigactionSiginfo, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        struct sigaction act = {0};
        act.sa_sigaction = SigactionHandler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO;
        int rt = sigaction(SIGALRM, &act, nullptr);
        if (rt != 0) {
            LOG("sigaction return fail, rt=%d", rt);
            exit(1);
        }
        alarm(1);
        Msleep(1100);

        if (mReceivedSignal != SIGALRM) {
            LOG("SigactionHandler check fail, expected signal:%d, actual:%d", SIGALRM, mReceivedSignal);
            exit(1);
        }
        // other area of siginfo_t is not supported yet, test code deleted
        LOG("child exited with code=%d", exitCode);
        exit(exitCode);
    } else { // parent
        Msleep(1200);
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGWAIT_0100
 * @tc.name   sigwait basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigwaitBasic, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = 0;
        int exitCode = 0;
        sigset_t set;
        int sig;
        sigemptyset(&set);
        sigaddset(&set, SIGALRM);
        sigaddset(&set, SIGTERM);
        signal(SIGTERM, SignalHandler);
        signal(SIGALRM, SignalHandler);

        rt = sigwait(&set, &sig);
        LOG("sigwait1 returned: %d, signo=%d, mReceivedSignal1: %d", rt, sig, mReceivedSignal);
        if (mReceivedSignal != SIGALRM  || sig != SIGALRM) {
            LOG("Received signal check fail, expected signal=%d", SIGALRM);
            exitCode = 1;
        }

        rt = sigwait(&set, &sig);
        LOG("sigwait2 returned: %d, signo=%d, mReceivedSignal1: %d", rt, sig, mReceivedSignal);
        if (mReceivedSignal != SIGTERM  || sig != SIGTERM) {
            LOG("Received signal check fail, expected signal=%d", SIGALRM);
            exitCode = 1;
        }

        rt = sigwait(&set, &sig);
        LOG("sigwait3 returned: %d, signo=%d, mReceivedSignal3: %d", rt, sig, mReceivedSignal);
        if (mReceivedSignal != SIGALRM  || sig != SIGALRM) {
            LOG("Received signal check fail, expected signal=%d", SIGALRM);
            exitCode = 1;
        }

        LOG("child exited with code=%d", exitCode);
        exit(exitCode);
    } else { // parent
        Msleep(30);
        LOG("calling kill 1, signo=%d", SIGALRM);
        kill(pid, SIGALRM);
        Msleep(50);
        LOG("calling kill 2, signo=%d", SIGTERM);
        kill(pid, SIGTERM);
        Msleep(50);
        LOG("calling kill 3, signo=%d", SIGALRM);
        kill(pid, SIGALRM);
        Msleep(100);
        AssertProcExitedOK(pid);
    }
}
/**
 * @tc.number SUB_KERNEL_IPC_SIGWAIT_0200
 * @tc.name   test that 'sigwait' should not return if a not-in-set signal is received
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigwaitNotInSet, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = 0;
        int exitCode = 0;
        sigset_t set;
        int sig;
        sigemptyset(&set);
        sigaddset(&set, SIGTERM);
        signal(SIGTERM, SignalHandler);
        signal(SIGALRM, SignalHandler);

        rt = sigwait(&set, &sig);
        LOG("sigwait1 returned: %d, signo=%d, mReceivedSignal1: %d", rt, sig, mReceivedSignal);
        LOG("child exiting with code=%d", exitCode);
        exit(exitCode);
    } else { // parent
        LOG("parent pid is %d, child pid is %d", getpid(), pid);
        Msleep(30);
        LOG("calling kill 1, signo=%d", SIGALRM);
        kill(pid, SIGALRM);
        Msleep(50);
        AssertProcAlive(pid);

        LOG("calling kill 2, signo=%d", SIGKILL);
        kill(pid, SIGKILL);
        Msleep(100);
        AssertProcKilled(pid, SIGKILL);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGTIMEDWAIT_0100
 * @tc.name   sigtimedwait still work even blocked by sigprocmask
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigtimedwaitBlock, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        signal(SIGSEGV, SignalHandler);
        struct timespec time1 = {0, 100*1000000};
        sigset_t sigmask, timeset;
        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGINT);
        sigaddset(&sigmask, SIGSEGV);
        sigemptyset(&timeset);
        sigaddset(&timeset, SIGSEGV);

        sigprocmask(SIG_BLOCK, &sigmask, nullptr);
        Msleep(80);

        int rt = sigtimedwait(&timeset, nullptr, &time1);
        if (rt != SIGSEGV) {
            LOG("sigtimedwait return fail, expected:%d, actual:%d", SIGSEGV, rt);
            exitCode = 1;
        }

        // check the sigprocmask set not changed
        sigemptyset(&sigmask);
        sigprocmask(SIG_UNBLOCK, nullptr, &sigmask);
        if (sigismember(&sigmask, SIGINT) != 1) {
            LOG("SIGINT should still in block set!");
            exitCode = 1;
        }
        if (sigismember(&sigmask, SIGSEGV) != 1) {
            LOG("SIGSEGV should still in block set!");
            exitCode = 1;
        }
        exit(exitCode);
    } else { // parent
        Msleep(40);
        kill(pid, SIGSEGV);
        Msleep(200);
        WaitProcExitedOK(pid);
    }
    sigset_t pending;
    sigemptyset(&pending);
    sigpending(&pending);
    if (sigisemptyset(&pending)) {
        LOG("pending set empty");
        return;
    }
    LOG("========pending set not empty=========");
    if (sigismember(&pending, SIGCHLD)) {
        LOG("pending set is SIGCHLD");
        return;
    } else {
        LOG("pending set is not SIGCHLD!");
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGTIMEDWAIT_0200
 * @tc.name   sigtimedwait error test: timeout or interrupted.
 *            by now, liteos sigtimedwait cannot interrupted
 *            so the interrupted-check-code will run only when 'POSIX_TEST' is set
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigtimedwaitTimeout, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt;
        int exitCode = 0;
        sigset_t set;
        siginfo_t si;
        struct timespec ts = {0, 500*1000000};
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);

        struct timespec time1 = {0, 0};
        struct timespec time2 = {0, 0};
        errno = 0;
        clock_gettime(CLOCK_MONOTONIC, &time1);
        rt = sigtimedwait(&set, &si, &ts);
        clock_gettime(CLOCK_MONOTONIC, &time2);
        LOG("sigtimedwait returned: %d", rt);
        if (rt != -1 || errno != EAGAIN) {
            LOG("sigtimedwait error check fail, expected errno=%d(EAGAIN), actual=%d", EAGAIN, errno);
            exitCode = 1;
        }
        double duration = (time2.tv_sec - time1.tv_sec)*1000.0 + (time2.tv_nsec - time1.tv_nsec)/1000000.0;
        LOG("clock_gettime1 : tv_sec=%ld, tv_nsec=%ld", time1.tv_sec, time1.tv_nsec);
        LOG("clock_gettime2 : tv_sec=%ld, tv_nsec=%ld", time2.tv_sec, time2.tv_nsec);
        LOG("duration : %f ms", duration);
        if (CheckValueClose(1000, duration)) {
            LOG("Timeout value accuracy check fail, expected=1000, actual=%f", duration);
            exitCode = 1;
        }

        LOG("child exited with code=%d", exitCode);
        exit(exitCode);
    } else { // parent
        sleep(1);
        WaitProcExitedOK(pid);
    }
}


/**
 * @tc.number SUB_KERNEL_IPC_SIGTIMEDWAIT_0300
 * @tc.name   sigtimedwait siginfo_t test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigtimedwaitSiginfo, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        signal(SIGINT, SignalHandler);
        struct timespec time1 = {0, 100*1000000};
        sigset_t set;
        siginfo_t si;
        sigemptyset(&set);
        sigaddset(&set, SIGINT);
        int rt = sigtimedwait(&set, &si, &time1);
        if (rt != SIGINT) {
            LOG("sigtimedwait should return the signal, expected:%d, actual:%d", SIGINT, rt);
            exitCode = 1;
        }
        LOGD("si: %d,%d,%d,%p", si.si_signo, si.si_code, si.si_value.sival_int, si.si_value.sival_int);
        if (si.si_signo != SIGINT) {
            LOG("sigtimedwait set siginfo_t fail, si_signo=%d", si.si_signo);
            exitCode = 1;
        }
        // other area of siginfo_t is not supported yet, test code deleted
        LOG("child exited with code=%d", exitCode);
        exit(exitCode);
    } else { // parent
        Msleep(20);
        kill(pid, SIGINT);
        Msleep(150);
        AssertProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGWAITINFO_0100
 * @tc.name   sigwaitinfo basic test: a pending signal should cause sigwaitinfo return immediately
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigwaitinfoBasic, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        sigset_t set, pending;
        sigemptyset(&pending);
        sigemptyset(&set);
        sigaddset(&set, SIGALRM);
        signal(SIGALRM, SignalHandler);
        if (sigprocmask(SIG_BLOCK, &set, nullptr) == -1) {
            LOG("sigprocmask failed");
            exit(1);
        }
        LOG("raise SIGALRM");
        raise(SIGALRM);

        sigpending(&pending);
        if (!sigismember(&pending, SIGALRM)) {
            LOG("SIGALRM is not in pending set");
            exit(1);
        }

        LOGD("before sigwaitinfo");
        struct timespec time1 = {0, 0};
        struct timespec time2 = {0, 0};
        clock_gettime(CLOCK_MONOTONIC, &time1);
        int rt = sigwaitinfo(&set, nullptr);
        clock_gettime(CLOCK_MONOTONIC, &time2);

        LOGD("after sigwaitinfo");
        double duration = (time2.tv_sec - time1.tv_sec)*1000.0 + (time2.tv_nsec - time1.tv_nsec)/1000000.0;
        LOG("duration: %f ms", duration);
        if (CheckValueClose(0.1, duration)) {
            LOG("sigwaitinfo should return immediately, but %f ms used", duration);
            exitCode = 1;
        }
        if (rt != SIGALRM) {
            LOG("sigwaitinfo should return the signal, expected:%d, actual:%d", SIGALRM, rt);
            exitCode = 1;
        }
        LOG("child exited with code=%d", exitCode);
        exit(exitCode);
    } else { // parent
        sleep(1);
        AssertProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_PAUSE_0100
 * @tc.name   pause basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testPauseBasic, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        struct timespec time1 = {0};
        struct timespec time2 = {0};
        int exitCode = 0;
        signal(SIGQUIT, SignalHandler);
        Msleep(100);

        clock_gettime(CLOCK_MONOTONIC, &time1);
        int rt = pause();
        clock_gettime(CLOCK_MONOTONIC, &time2);
        if (rt != -1) {
            LOG("pause should return -1, but rt=%d", rt);
            exitCode = 1;
        }
        if (errno != EINTR) {
            LOG("pause should set errno to 4(EINTR),but get %d", errno);
            exitCode = 1;
        }

        long duration = (time2.tv_sec - time1.tv_sec)*1000 + (time2.tv_nsec - time1.tv_nsec)/1000000;
        LOG("paused time: %ld ms", duration);
        if (! CheckValueClose(100, duration, 0.2)) {
            LOG("paused time check error.");
            exitCode = 1;
        }
        if (mReceivedSignal != SIGQUIT) {
            LOG("Received signal check fail, expected signal=%d", SIGQUIT);
            exitCode = 1;
        }
        exit(exitCode);
    } else { // parent
        Msleep(200);
        kill(pid, SIGQUIT);
        Msleep(200);
        AssertProcExitedOK(pid);
    }
}
/**
 * @tc.number SUB_KERNEL_IPC_SIG_PAUSE_0200
 * @tc.name   pause and mask test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testPauseAndMask, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        signal(SIGINT, SignalHandler);
        signal(SIGTRAP, SignalHandler);

        sigset_t sigmask;
        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGINT);
        int rt = sigprocmask(SIG_BLOCK, &sigmask, nullptr);
        if (rt != 0) {
            LOG("sigprocmask fail, rt=%d, errno=%d", rt, errno);
            exit(1);
        }
        LOG("before pause");
        rt = pause();
        if (rt != -1) {
            LOG("pause should return -1, but rt=%d", rt);
            exitCode = 1;
        }
        if (errno != EINTR) {
            LOG("pause should set errno to 4(EINTR),but get %d", errno);
            exitCode = 1;
        }
        LOG("after pause");
        if (mReceivedSignal != SIGTRAP) {
            LOG("Received signal check fail, expected %d,but get %d", SIGINT, mReceivedSignal);
            exitCode = 1;
        }
        exit(exitCode);
    } else { // parent
        Msleep(20);
        kill(pid, SIGINT);
        Msleep(20);
        AssertProcAlive(pid);
        kill(pid, SIGTRAP);
        Msleep(20);
        WaitProcExitedOK(pid);
    }
    sigset_t pending;
    sigemptyset(&pending);
    sigpending(&pending);
    if (sigisemptyset(&pending)) {
        LOG("pending set empty");
    } else {
        LOG("========pending set not empty=========");
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGPENDING_0100
 * @tc.name   sigpending basic function test, also test pending-signal-set should not inherited after fork,
              but signal mask set should inherited, and the change of child's pending set should not effect parent's.
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigpendingBasic, Function | MediumTest | Level1)
{
    int rt;
    sigset_t sigmask, oldmask;
    sigset_t pending;
    sigemptyset(&sigmask);
    sigemptyset(&oldmask);
    sigemptyset(&pending);
    sigpending(&pending);
    EXPECT_EQ(1, sigisemptyset(&pending)) << "initial pending set should empty";

    signal(SIGALRM, SignalHandler);

    sigaddset(&sigmask, SIGALRM);
    sigaddset(&sigmask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigmask, &oldmask);
    EXPECT_EQ(1, sigisemptyset(&oldmask)) << "initial sig mask set should empty";
    rt = sigpending(&pending);
    EXPECT_EQ(rt, 0);
    EXPECT_EQ(1, sigisemptyset(&pending)) << "SIG_BLOCK set should not effect on pending set";

    LOGD("raise SIGALRM");
    raise(SIGALRM);
    sigpending(&pending);
    EXPECT_EQ(1, sigismember(&pending, SIGALRM));

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        sigemptyset(&pending);
        sigpending(&pending);
        if (sigisemptyset(&pending) != 1) {
            LOG("pending signal set should not reserved via fork");
            exitCode = 1;
        }

        sigemptyset(&oldmask);
        sigprocmask(SIG_BLOCK, nullptr, &oldmask); // read mask
        if ((sigismember(&oldmask, SIGALRM) != 1) || (sigismember(&oldmask, SIGUSR1) != 1)) {
            LOG("signal mask set should reserved via fork");
            exitCode = 1;
        }

        LOG("unblock SIGALRM.");
        rt = sigprocmask(SIG_UNBLOCK, &sigmask, nullptr);
        sigemptyset(&pending);
        sigpending(&pending);
        if (sigisemptyset(&pending) != 1) {
            LOG("pending signal set is not empty after unblock");
            exitCode = 1;
        }

        LOG("child exit(%d).", exitCode);
        exit(exitCode);
    } else { // parent
        Msleep(80);
        WaitProcExitedOK(pid);
        // check child's pending set should not effect parent's
        sigemptyset(&pending);
        sigpending(&pending);
        EXPECT_EQ(1, sigismember(&pending, SIGALRM)) << "parent's pending set is changed!";
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGPROCMASK_0100
 * @tc.name   sigprocmask function test: use raise and kill to send signal
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigprocmaskBasic, Function | MediumTest | Level1)
{
    int rt;
    sigset_t sigmask, oldmask;
    sigset_t pending;
    sigemptyset(&sigmask);
    sigemptyset(&oldmask);
    sigemptyset(&pending);
    sigaddset(&sigmask, SIGINT);
    sigaddset(&sigmask, SIGUSR1);
    rt = sigprocmask(SIG_BLOCK, &sigmask, &oldmask);
    EXPECT_EQ(rt, 0);

    signal(SIGINT, SignalHandler);
    signal(SIGUSR1, SignalHandler);
    ASSERT_EQ(mReceivedSignal, 0);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        raise(SIGINT);
        if (mReceivedSignal != 0) {
            LOG("SignalHandler check fail, expected=%d, actual=%d", 0, mReceivedSignal);
            LOG("SIGINT(%d) should has blocked!", SIGINT);
            exit(1);
        }

        LOG("unblock SIGINT");
        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGINT);
        rt = sigprocmask(SIG_UNBLOCK, &sigmask, nullptr);
        EXPECT_EQ(rt, 0);

        // check the new block set
        sigemptyset(&oldmask);
        sigprocmask(SIG_UNBLOCK, nullptr, &oldmask);
        if (sigismember(&oldmask, SIGINT) == 1) {
            LOG("SIGINT should has deleted from block set!");
            exitCode = 1;
        }
        if (sigismember(&oldmask, SIGUSR1) != 1) {
            LOG("SIGUSR1 should still in block set!");
            exitCode = 1;
        }
        if (mReceivedSignal != SIGINT) {
            LOG("SignalHandler check fail, expected=%d, actual=%d", SIGINT, mReceivedSignal);
            LOG("SIGALRM should deliver after unblock!");
            exitCode = 1;
        }

        // test kill
        mReceivedSignal = 0;
        Msleep(80);
        sigpending(&pending);
        if (!sigismember(&pending, SIGUSR1)) {
            LOG("SIGUSR1 is not in pending set!");
            exitCode = 1;
        } else {
            LOG("SIGUSR1 is in pending set.");
        }

        if (mReceivedSignal != 0) {
            LOG("SignalHandler check fail, expected=%d, actual=%d", 0, mReceivedSignal);
            LOG("SIGUSR1(%d) should has blocked!", SIGUSR1);
            exit(1);
        }

        LOG("unblock SIGUSR1");
        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGUSR1);
        sigprocmask(SIG_UNBLOCK, &sigmask, nullptr);

        if (mReceivedSignal != SIGUSR1) {
            LOG("SignalHandler check fail, expected=%d, actual=%d", SIGUSR1, mReceivedSignal);
            LOG("SIGUSR1 should deliver after unblock!");
            exitCode = 1;
        }

        LOG("child exit(%d).", exitCode);
        exit(exitCode);
    } else { // parent
        Msleep(50);
        kill(pid, SIGUSR1);
        Msleep(50);
        WaitProcExitedOK(pid);
    }
    // restore default config
    signal(SIGINT, SIG_DFL);
    signal(SIGUSR1, SIG_DFL);
    rt = sigprocmask(SIG_UNBLOCK, &sigmask, nullptr);
    EXPECT_EQ(rt, 0);
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGPROCMASK_0200
 * @tc.name   sigprocmask function test: use alarm to send the signal
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigprocmaskAlarm, Function | MediumTest | Level2)
{
    int rt;
    sigset_t sigmask, oldmask;
    sigset_t pending;
    sigemptyset(&sigmask);
    sigemptyset(&oldmask);
    sigemptyset(&pending);
    sigaddset(&sigmask, SIGALRM);
    rt = sigprocmask(SIG_BLOCK, &sigmask, &oldmask);
    ASSERT_EQ(rt, 0);

    signal(SIGALRM, SignalHandler);
    ASSERT_EQ(mReceivedSignal, 0);
    alarm(1);
    Msleep(1100);

    sigpending(&pending);
    EXPECT_EQ(1, sigismember(&pending, SIGALRM));
    EXPECT_EQ(mReceivedSignal, 0);

    LOG("unblock SIGALRM.");
    rt = sigprocmask(SIG_UNBLOCK, &sigmask, nullptr);
    EXPECT_EQ(rt, 0);
    ASSERT_EQ(mReceivedSignal, SIGALRM);

    signal(SIGALRM, SIG_DFL);
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGPROCMASK_0300
 * @tc.name   sigprocmask function test: mask operation
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigprocmaskSetMask, Function | MediumTest | Level2)
{
    int rt;
    sigset_t sigmask, oldmask, pending;
    sigemptyset(&sigmask);
    sigemptyset(&oldmask);
    sigemptyset(&pending);
    sigaddset(&sigmask, SIGALRM);
    rt = sigprocmask(SIG_BLOCK, &sigmask, nullptr);
    ASSERT_EQ(rt, 0);
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGINT);
    rt = sigprocmask(SIG_BLOCK, &sigmask, nullptr);
    ASSERT_EQ(rt, 0);

    LOG("add a new sig to block set");
    rt = sigprocmask(SIG_BLOCK, nullptr, &oldmask); // check
    ASSERT_EQ(rt, 0);
    EXPECT_EQ(1, sigismember(&oldmask, SIGALRM));
    EXPECT_EQ(1, sigismember(&oldmask, SIGINT));

    LOG("unblock a not-in-set sig");
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGTRAP);
    rt = sigprocmask(SIG_UNBLOCK, &sigmask, nullptr);
    ASSERT_EQ(rt, 0);
    rt = sigprocmask(SIG_UNBLOCK, nullptr, &oldmask); // check
    EXPECT_EQ(1, sigismember(&oldmask, SIGALRM));
    EXPECT_EQ(1, sigismember(&oldmask, SIGINT));
    EXPECT_EQ(0, sigismember(&oldmask, SIGTRAP));

    LOG("set new block set, clear old one");
    rt = sigprocmask(SIG_SETMASK, &sigmask, nullptr);
    ASSERT_EQ(rt, 0);
    rt = sigprocmask(10, nullptr, &oldmask); // check
    ASSERT_EQ(rt, 0);
    EXPECT_EQ(0, sigismember(&oldmask, SIGALRM));
    EXPECT_EQ(0, sigismember(&oldmask, SIGINT));
    EXPECT_EQ(1, sigismember(&oldmask, SIGTRAP));

    LOG("unblock git, clear the block set");
    rt = sigprocmask(SIG_UNBLOCK, &sigmask, nullptr);
    ASSERT_EQ(rt, 0);
    rt = sigprocmask(100, nullptr, &oldmask); // check
    ASSERT_EQ(rt, 0);
    EXPECT_EQ(1, sigisemptyset(&oldmask));
}
/**
 * @tc.number SUB_KERNEL_IPC_SIGPROCMASK_0400
 * @tc.name   sigprocmask function test: don't block signal that not in block-set with caught
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigprocmaskNotinSet, Function | MediumTest | Level2)
{
    LOG("Test: not-in-set signal could be caught.");
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        sigset_t sigmask;
        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGTRAP);
        int rt = sigprocmask(SIG_BLOCK, &sigmask, nullptr);
        if (rt != 0) {
            LOG("sigprocmask fail, rt=%d, errno=%d", rt, errno);
            exit(1);
        }
        signal(SIGINT, SignalHandler);
        signal(SIGTRAP, SignalHandler);

        Msleep(80);
        if (mReceivedSignal != SIGINT) {
            LOG("SignalHandler check fail, expected=SIGINT, actual=%d", mReceivedSignal);
            exit(1);
        }
        exit(0);
    } else { // parent
        Msleep(30);
        kill(pid, SIGINT);
        Msleep(50);
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGPROCMASK_0800
 * @tc.name   sigprocmask function test: don't block signal that not in block-set with terminate
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigprocmaskNotinSet0800, Function | MediumTest | Level2)
{
    LOG("Test: not-in-set signal could terminate the process.");
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        sigset_t sigmask;
        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGTRAP);
        int rt = sigprocmask(SIG_BLOCK, &sigmask, nullptr);
        if (rt != 0) {
            LOG("sigprocmask fail, rt=%d, errno=%d", rt, errno);
            exit(1);
        }
        Msleep(60);
        exit(0);
    } else { // parent
        Msleep(30);
        kill(pid, SIGINT);
        Msleep(30);
        WaitProcKilled(pid, SIGINT);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGSUSPEND_0100
 * @tc.name   sigsuspend basic function test1: the sigsuspend-signal terminates the process
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsuspendTerminate, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        signal(SIGTRAP, SignalHandler);
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGHUP);
        int rt = sigsuspend(&set);
        // should not get here:
        LOG("sigsuspend rt = %d, errno=%d", rt, errno);
        exit(1);
    } else { // parent
        Msleep(20);

        kill(pid, SIGHUP); // this should blocked by sigsuspend
        Msleep(20);
        AssertProcAlive(pid);

        kill(pid, SIGTRAP); // this should interrupt sigsuspend
        Msleep(100);
        WaitProcKilled(pid, SIGHUP);
    }
}
/**
 * @tc.number SUB_KERNEL_IPC_SIGSUSPEND_0200
 * @tc.name   sigsuspend basic function test2: the sigsuspend-signal caught, and process continue run
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsuspendContinue, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        signal(SIGTRAP, SignalHandler);
        signal(SIGHUP, SignalHandler);
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGHUP);
        int rt = sigsuspend(&set);
        if (rt != -1) {
            LOG("sigsuspend should return -1, but rt=%d", rt);
            exitCode = 1;
        }
        if (errno != EINTR) {
            LOG("sigsuspend should set errno to 4(EINTR),but get %d", errno);
            exitCode = 1;
        }
        if (mReceivedSignal != SIGHUP) {
            LOG("Received signal check fail, expected signal=%d", SIGHUP);
            exitCode = 1;
        }
        exit(exitCode);
    } else { // parent
        Msleep(20);

        kill(pid, SIGHUP);
        Msleep(20);
        AssertProcAlive(pid);

        kill(pid, SIGTRAP);
        Msleep(100);
        AssertProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIGSUSPEND_0300
 * @tc.name   sigsuspend test with sigprocmask
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsuspendAndMask, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        sigset_t procMask, suspendMask;
        sigemptyset(&suspendMask);
        sigemptyset(&procMask);
        sigaddset(&suspendMask, SIGQUIT);
        sigaddset(&procMask, SIGINT);

        signal(SIGINT, SignalHandler);
        signal(SIGQUIT, SignalHandler);

        LOG("Block SIGINT...");
        int rt = sigprocmask(SIG_SETMASK, &procMask, nullptr);
        if (rt != 0) {
            LOG("sigprocmask fail, rt=%d, errno=%d", rt, errno);
            exit(1);
        }
        LOG("Suspend SIGQUIT...");
        rt = sigsuspend(&suspendMask);
        if (rt != -1) {
            LOG("sigsuspend should return -1, but rt=%d", rt);
            exitCode = 1;
        }
        // signal in procmask should received 1st
        // need change to check SIGQUIT only if multi-signal issue fixed
        if ((mReceivedSignal != SIGQUIT) && (mReceivedSignal != SIGINT)) {
            LOG("Received signal check fail, get %d", mReceivedSignal);
            exitCode = 1;
        }

        LOG("Check current mask...");
        sigemptyset(&procMask);
        sigprocmask(10, nullptr, &procMask);
        if (!sigismember(&procMask, SIGINT)) {
            LOG("SIGINT should in block set!");
            exitCode = 1;
        }
        if (sigismember(&procMask, SIGQUIT)) {
            LOG("SIGQUIT should not in block set!");
            exitCode = 1;
        }
        exit(exitCode);
    } else { // parent
        Msleep(20);
        kill(pid, SIGQUIT);
        Msleep(20);
        AssertProcAlive(pid);

        kill(pid, SIGINT); // this should interrupt sigsuspend
        Msleep(100);
        AssertProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_PTDKILL_0100
 * @tc.name   pthread_kill function test: basic
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testPthreadkill, Function | MediumTest | Level1)
{
    pthread_t tid;
    int sigNo = SIGXCPU;
    int ret = pthread_create(&tid, NULL, ThreadFunc1, (void*)sigNo);
    ASSERT_EQ(ret, 0) << "pthread_create failed, errno=" << ret;
    Msleep(30);
    ret = pthread_kill(tid, sigNo);
    EXPECT_EQ(ret, 0) << "pthread_kill failed, errno=" << ret;
    ret = pthread_join(tid, NULL);
    EXPECT_EQ(ret, 0) << "pthread_join failed, errno=" << ret;
    EXPECT_EQ(mReceivedSignal, sigNo) << "Thread received signal check fail";
    // restore handler
    handler_type rt = signal(sigNo, SIG_DFL);
    ASSERT_NE(rt, SIG_ERR) << "restore signal failed, errno=" << errno;
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_PTDKILL_0200
 * @tc.name   pthread_kill function test: try kill main thread
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testPthreadkillMain, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        pthread_t mainThread = pthread_self();
        signal(SIGINT, SignalHandler);
        pthread_t tid;
        int ret = pthread_create(&tid, NULL, ThreadFunc2, &mainThread);
        if (ret != 0) {
            LOG("pthread_create failed, errno=%d", ret);
            exit(1);
        }

        void* threadExitCode = nullptr;
        ret = pthread_join(tid, &threadExitCode);
        if (ret != 0) {
            LOG("pthread_join failed, errno=%d", ret);
            exit(1);
        }
        int c = (int)((uintptr_t)threadExitCode);
        if (c != 0) {
            LOG("sub thread exited failed, code=%d", c);
            exit(1);
        }

        if (mReceivedSignal != SIGINT) {
            LOG("SignalHandler check fail, expected signal:%d, actual:%d", SIGINT, mReceivedSignal);
            exit(1);
        }
        exit(0);
    } else { // parent
        Msleep(100);
        AssertProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_PTDSIGMASK_0100
 * @tc.name   pthread_sigmask function test: basic
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testPthreadSigmask, Function | MediumTest | Level1)
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, ThreadFuncForSigmask1, (void*)1);
    ASSERT_EQ(ret, 0) << "pthread_create failed, errno=" << ret;
    ret = pthread_join(tid, NULL);
    EXPECT_EQ(ret, 0) << "pthread_join failed, errno=" << ret;
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_PTDSIGMASK_0200
 * @tc.name   pthread_sigmask function test: handler
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testPthreadSigmaskHandler, Function | MediumTest | Level1)
{
    signal(SIGINT, SignalHandler);
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, ThreadFuncForSigmask1, (void*)2);
    EXPECT_EQ(ret, 0) << "pthread_create failed, errno=" << ret;

    ret = pthread_join(tid, NULL);
    EXPECT_EQ(ret, 0) << "pthread_join failed, errno=" << ret;

    signal(SIGINT, SIG_DFL);
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_PTDSIGMASK_0300
 * @tc.name   test that the signal-mask should inherited from the creator
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testPthreadSigmaskInherit, Function | MediumTest | Level3)
{
    sigset_t sigmask, oldmask;
    sigemptyset(&sigmask);
    sigemptyset(&oldmask);

    LOG("add SIGUSR1 to block set");
    sigaddset(&sigmask, SIGUSR1);
    int rt = sigprocmask(SIG_BLOCK, &sigmask, &oldmask);
    EXPECT_EQ(rt, 0);

    pthread_t tid;
    int ret = pthread_create(&tid, NULL, ThreadFuncForSigmask2, NULL);
    EXPECT_EQ(ret, 0) << "pthread_create failed, errno=" << ret;

    void* threadExitCode = nullptr;
    ret = pthread_join(tid, &threadExitCode);
    EXPECT_EQ(ret, 0) << "pthread_join failed, errno=" << ret;
    int code = (int)((uintptr_t)threadExitCode);
    EXPECT_EQ(code, 0);

    LOG("check the block set is not effected");
    sigprocmask(SIG_UNBLOCK, nullptr, &oldmask);

    EXPECT_EQ(sigismember(&oldmask, SIGINT), 0) << "SIGINT should not in block set!";
    EXPECT_EQ(sigismember(&oldmask, SIGUSR1), 1) << "SIGUSR1 should still in block set!";

    LOG("restore default config");
    rt = sigprocmask(SIG_UNBLOCK, &sigmask, nullptr);
    EXPECT_EQ(rt, 0);
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_0100
 * @tc.name   test sig set operator APIs:sigemptyset,sigaddset,sigisemptyset,sigismember
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetBasic, Function | MediumTest | Level2)
{
    sigset_t set;
    int rt = sigemptyset(&set);
    EXPECT_EQ(rt, 0) << "sigandset failed";
    EXPECT_EQ(sigisemptyset(&set), 1) << "set should empty";

    rt = sigaddset(&set, SIGINT);
    EXPECT_EQ(rt, 0) << "sigaddset failed, errno=" << errno;
    EXPECT_EQ(sigismember(&set, SIGINT), 1) << "SIGINT should in set";
    EXPECT_EQ(sigismember(&set, SIGQUIT), 0) << "SIGQUIT should not in set";

    rt = sigaddset(&set, SIGQUIT);
    EXPECT_EQ(rt, 0) << "sigaddset failed, errno=" << errno;
    EXPECT_EQ(sigismember(&set, SIGQUIT), 1) << "SIGQUIT should in set";

    rt = sigaddset(&set, -1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    rt = sigaddset(&set, MAX_SIGNAL_NUMBER + 1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    LOG("check that set not changed after failed-sigadd");
    EXPECT_EQ(sigismember(&set, SIGINT), 1) << "SIGINT should in set";
    EXPECT_EQ(sigismember(&set, SIGQUIT), 1) << "SIGQUIT should in set";

    sigemptyset(&set);
    EXPECT_EQ(sigisemptyset(&set), 1) << "set should empty";
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_0200
 * @tc.name   test sig set operator API: sigandset
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetAnd, Function | MediumTest | Level2)
{
    sigset_t set1, set2, setAnd;
    sigemptyset(&set1);
    sigemptyset(&set2);
    sigemptyset(&setAnd);
    int rt;

    LOG("test and-set with empty set:");
    sigaddset(&set1, SIGINT);
    rt = sigandset(&setAnd, &set1, &set2);
    EXPECT_EQ(rt, 0) << "sigandset failed, errno=" << errno;
    EXPECT_EQ(sigisemptyset(&setAnd), 1) << "setAnd should empty";
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_0300
 * @tc.name   test sig set operator API: sigorset
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetOr, Function | MediumTest | Level2)
{
    sigset_t set1, set2, setOr;
    sigemptyset(&set1);
    sigemptyset(&set2);
    sigemptyset(&setOr);
    int rt;

    LOG("test or-set with empty set:");
    sigaddset(&set1, SIGINT);
    rt = sigorset(&setOr, &set1, &set2);
    EXPECT_EQ(rt, 0) << "sigorset failed, errno=" << errno;
    EXPECT_EQ(sigismember(&setOr, SIGINT), 1) << "SIGINT should in setOr";
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_0400
 * @tc.name   test sig set operator APIs: sigdelset
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetDelete, Function | MediumTest | Level2)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    LOG("delete in-set sig:");
    int rt = sigdelset(&set, SIGINT);
    EXPECT_EQ(rt, 0) << "sigdelset failed, errno=" << errno;
    EXPECT_EQ(sigisemptyset(&set), 1);

    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    rt = sigdelset(&set, SIGINT);
    EXPECT_EQ(rt, 0) << "sigdelset failed, errno=" << errno;
    EXPECT_EQ(sigismember(&set, SIGQUIT), 1) << "SIGQUIT should in set";
    EXPECT_EQ(sigismember(&set, SIGINT), 0) << "SIGINT should not in set";
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_0500
 * @tc.name   test sig set operator APIs: sigfillset
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetFill, Function | MediumTest | Level2)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    int rt = sigfillset(&set);
    EXPECT_EQ(rt, 0) << "sigfillset failed, errno=" << errno;
    for (int i = 1; i <= MAX_SIGNAL; i++) {
        EXPECT_EQ(sigismember(&set, i), 1) << i << " not in set!";
    }

    sigdelset(&set, SIGINT);
    for (int i = 1; i <= MAX_SIGNAL; i++) {
        if (i == SIGINT) {
            EXPECT_EQ(sigismember(&set, i), 0) << "SIGINT should not in set!";
        } else {
            EXPECT_EQ(sigismember(&set, i), 1) << i << " not in set!";
        }
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_0600
 * @tc.name   test sigdelset operator APIs: sigdelset
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetDelete0600, Function | MediumTest | Level2)
{
    int rt;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    LOG("delete not-in-set sig:");
    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    rt = sigdelset(&set, SIGINT);
    EXPECT_EQ(rt, 0) << "sigdelset failed, errno=" << errno;
    EXPECT_EQ(sigismember(&set, SIGQUIT), 1) << "SIGQUIT should in set";
    EXPECT_EQ(sigismember(&set, SIGINT), 0) << "SIGINT should not in set";
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_0700
 * @tc.name   test sig set operator APIs: sigdelset with invalid
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetDelete0700, Function | MediumTest | Level2)
{
    int rt;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    LOG("delete invalid sig:");
    rt = sigdelset(&set, -1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    rt = sigdelset(&set, MAX_SIGNAL_NUMBER + 1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    LOG("check that set not changed after failed-delete");
    EXPECT_EQ(sigismember(&set, SIGQUIT), 0) << "SIGQUIT should in set";
    EXPECT_EQ(sigismember(&set, SIGINT), 1) << "SIGINT should not in set";
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_1000
 * @tc.name   test sig set operator API: sigorset with non-overlap
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetOr1000, Function | MediumTest | Level2)
{
    sigset_t set1, set2, setOr;
    sigemptyset(&set1);
    sigemptyset(&set2);
    sigemptyset(&setOr);
    int rt;

    LOG("test or-set with non-overlap sets:");
    sigaddset(&set1, SIGINT);
    sigaddset(&set2, SIGABRT);
    rt = sigorset(&setOr, &set1, &set2);
    EXPECT_EQ(rt, 0) << "sigorset failed, errno=" << errno;
    EXPECT_EQ(sigismember(&setOr, SIGINT), 1) << "SIGINT should in setOr";
    EXPECT_EQ(sigismember(&setOr, SIGABRT), 1) << "SIGABRT should in setOr";
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_1100
 * @tc.name   test sig set operator API: sigorset with overlapping
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetOr1100, Function | MediumTest | Level2)
{
    sigset_t set1, set2, setOr;
    sigemptyset(&set1);
    sigemptyset(&set2);
    sigemptyset(&setOr);
    int rt;

    LOG("test or-set with overlapping sets:");
    sigaddset(&setOr, SIGHUP);  // original siganl in setOr
    sigaddset(&set1, SIGINT);
    sigaddset(&set2, SIGABRT);
    sigaddset(&set1, SIGQUIT);
    sigaddset(&set2, SIGQUIT);
    rt = sigorset(&setOr, &set1, &set2);
    EXPECT_EQ(rt, 0) << "sigorset failed, errno=" << errno;
    EXPECT_EQ(sigismember(&setOr, SIGINT), 1) << "SIGINT should in setOr";
    EXPECT_EQ(sigismember(&setOr, SIGABRT), 1) << "SIGABRT should in setOr";
    EXPECT_EQ(sigismember(&setOr, SIGQUIT), 1) << "SIGQUIT should in setOr";
    EXPECT_EQ(sigismember(&setOr, SIGHUP), 0) << "original siganl in setOr should deleted";
}


/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_1200
 * @tc.name   test sig set operator API: sigandset with non-overlap
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetAnd1200, Function | MediumTest | Level2)
{
    sigset_t set1, set2, setAnd;
    sigemptyset(&set1);
    sigemptyset(&set2);
    sigemptyset(&setAnd);
    int rt;

    LOG("test and-set with non-overlap sets:");
    sigaddset(&set1, SIGINT);
    sigaddset(&set2, SIGABRT);
    rt = sigandset(&setAnd, &set1, &set2);
    EXPECT_EQ(rt, 0) << "sigandset failed, errno=" << errno;
    EXPECT_EQ(sigisemptyset(&setAnd), 1) << "setAnd should empty";
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_1300
 * @tc.name   test sig set operator API: sigandset with overlapping
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetAnd1300, Function | MediumTest | Level2)
{
    sigset_t set1, set2, setAnd;
    sigemptyset(&set1);
    sigemptyset(&set2);
    sigemptyset(&setAnd);
    int rt;

    LOG("test and-set with overlapping sets:");
    sigaddset(&set1, SIGINT);
    sigaddset(&set2, SIGABRT);
    sigaddset(&set1, SIGQUIT);
    sigaddset(&set2, SIGQUIT);
    sigaddset(&setAnd, SIGHUP);  // original siganl in setAnd
    rt = sigandset(&setAnd, &set1, &set2);
    EXPECT_EQ(rt, 0) << "sigandset failed, errno=" << errno;
    EXPECT_EQ(sigismember(&setAnd, SIGQUIT), 1) << "SIGQUIT should in setAnd";
    EXPECT_EQ(sigismember(&setAnd, SIGINT), 0) << "SIGINT should not in setAnd";
    EXPECT_EQ(sigismember(&setAnd, SIGABRT), 0) << "SIGABRT should not in setAnd";
    EXPECT_EQ(sigismember(&setAnd, SIGHUP), 0) << "original siganl in setAnd should deleted";
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SETOP_1400
 * @tc.name   test sig set operator API: sigandset with multi-overlapping
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigsetAnd1400, Function | MediumTest | Level2)
{
    sigset_t set1, set2, setAnd;
    sigemptyset(&set1);
    sigemptyset(&set2); 
    sigemptyset(&setAnd);
    int rt;
    LOG("test and-set with multi-overlapping sets:");
    sigaddset(&set1, SIGINT);
    sigaddset(&set2, SIGABRT);
    sigaddset(&set1, SIGQUIT);
    sigaddset(&set2, SIGQUIT);
    sigaddset(&setAnd, SIGHUP);  // original siganl in setAnd
    sigaddset(&set1, SIGUSR1);
    sigaddset(&set2, SIGUSR1);
    rt = sigandset(&setAnd, &set1, &set2);
    EXPECT_EQ(rt, 0) << "sigandset failed, errno=" << errno;
    EXPECT_EQ(sigismember(&setAnd, SIGQUIT), 1) << "SIGQUIT should in setAnd";
    EXPECT_EQ(sigismember(&setAnd, SIGUSR1), 1) << "SIGUSR1 should in setAnd";
    EXPECT_EQ(sigismember(&setAnd, SIGINT), 0) << "SIGINT should not in setAnd";
    EXPECT_EQ(sigismember(&setAnd, SIGABRT), 0) << "SIGABRT should not in setAnd";
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SIGHOLD_0100
 * @tc.name   sighold function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigHold, Function | MediumTest | Level2)
{
    int sigNo = SIGINT;
    int rt = sighold(sigNo);
    ASSERT_EQ(rt, 0) << "sighold failed, errno=" << errno;

    LOG("check that the signal is added to the mask");
    sigset_t oldmask;
    sigemptyset(&oldmask);
    rt = sigprocmask(SIG_BLOCK, nullptr, &oldmask);
    EXPECT_EQ(rt, 0);
    EXPECT_EQ(sigismember(&oldmask, sigNo), 1);

    // restore default config
    rt = sigprocmask(SIG_UNBLOCK, &oldmask, nullptr);
    EXPECT_EQ(rt, 0);
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SIGHOLD_0200
 * @tc.name   sighold function error test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigHoldError, Function | MediumTest | Level2)
{
    int rt = sighold(-1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    rt = sighold(MAX_SIGNAL_NUMBER + 1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SIGRELSE_0100
 * @tc.name   sigrelse function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigRelse, Function | MediumTest | Level2)
{
    LOG("add sig to mask");
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    int rt = sigprocmask(SIG_BLOCK, &mask, nullptr);
    ASSERT_EQ(rt, 0);

    LOG("delete signal from the mask");
    rt = sigrelse(SIGINT);
    EXPECT_EQ(rt, 0);

    LOG("check that the signal is deleted");
    sigemptyset(&mask);
    rt = sigprocmask(SIG_BLOCK, nullptr, &mask);
    EXPECT_EQ(rt, 0);
    EXPECT_EQ(sigisemptyset(&mask), 1);
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_SIGRELSE_0200
 * @tc.name   sigrelse function error test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testSigRelseError, Function | MediumTest | Level2)
{
    int rt = sigrelse(-1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    rt = sigrelse(MAX_SIGNAL_NUMBER + 1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_PSIGNAL_0100
 * @tc.name   psignal function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testPsignal, Function | MediumTest | Level2)
{
    // we use child to do the test here, cause 'restore of stderr' is not work by now.
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        // redirect stderr to a file, so we can check the content.
        char* outfile = WRITABLE_TEST_DIR "stderr.txt";
        if (freopen(outfile, "w", stderr) == NULL) {
            LOG("redirect stderr fail, freopen errno=%d\n", errno);
            exit(1);
        }
        psignal(SIGFPE, "SIGFPE");
        int rt = CheckSigString(outfile, "SIGFPE: Arithmetic exception");
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_PSIGNAL_0200
 * @tc.name   psignal function error test1
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testPsignalError1, Function | MediumTest | Level3)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        char* outfile = WRITABLE_TEST_DIR "stderr.txt";
        if (freopen(outfile, "w", stderr) == NULL) {
            LOG("redirect stderr fail, freopen errno=%d\n", errno);
            exit(1);
        }
        psignal(-1, "===");
        int rt = CheckSigString(outfile, "===: Unknown signal");
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_SIG_PSIGNAL_0300
 * @tc.name   psignal function error test2
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IpcSignalTest, testPsignalError2, Function | MediumTest | Level3)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        char* outfile = WRITABLE_TEST_DIR "stderr.txt";
        if (freopen(outfile, "w", stderr) == NULL) {
            LOG("redirect stderr fail, freopen errno=%d\n", errno);
            exit(1);
        }
        psignal(MAX_SIGNAL_NUMBER + 1, "***");
        int rt = CheckSigString(outfile, "***: Unknown signal");
        exit(rt);
    } else { // parent
        WaitProcExitedOK(pid);
    }
}
