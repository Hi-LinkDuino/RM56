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

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <fcntl.h>
#include "log.h"
#include "utils.h"
#include "SignalTest.h"

using namespace testing::ext;

// static member must init before use.
int IpcSignalTest::mReceivedSignal = 0;
int IpcSignalTest::mShmid = 0;
siginfo_t IpcSignalTest::mSiginfo;


// special signal handler for function 'abort'
void IpcSignalTest::SigAbortHandler(int signum)
{
    LOG("handler: recv a signal: %d", signum);
    int *shared = static_cast<int*>(shmat(mShmid, nullptr, 0));
    if (shared == reinterpret_cast<int*>(-1)) {
        LOG("SigAbortHandler: shmat fail, errno = %d", errno);
    } else {
        LOG("shared: %p", shared);
        *shared = signum;
        if (shmdt(shared) == -1) {
            LOG("SigAbortHandler: shmdt errno = %d", errno);
        }
    }
}

// special signal handler for function 'sigaction'
void IpcSignalTest::SigactionHandler(int signum, siginfo_t* si, void* ucontext)
{
    LOG("handler recv a signal: %s(%d)", ALL_SIGNALS[signum].signame, signum);
    mReceivedSignal = signum;
    // siginfo_t para is not supported yet
}

// general signal handler. note: not thread-safe
void IpcSignalTest::SignalHandler(int signum)
{
    LOG("handler recv a signal: %s(%d)", ALL_SIGNALS[signum].signame, signum);
    mReceivedSignal = signum;
}

SignalNameAction const ALL_SIGNALS[MAX_SIGNAL] = {
    {"NA",      "Unknown signal",           TERMINATE},
    {"SIGHUP",  "Hangup",                   TERMINATE}, //  1
    {"SIGINT",  "Interrupt",                TERMINATE},
    {"SIGQUIT", "Quit",                     COREDUMP},
    {"SIGILL",  "Illegal instruction",      COREDUMP},
    {"SIGTRAP", "Trace/breakpoint trap",    COREDUMP},
    {"SIGABRT", "Aborted",                  COREDUMP},  //  alias:  SIGIOT
    {"SIGBUS",  "Bus error",                COREDUMP},
    {"SIGFPE",  "Arithmetic exception",     COREDUMP},
    {"SIGKILL", "Killed",                   TERMINATE},
    {"SIGUSR1", "User defined signal 1",    TERMINATE}, //  10
    {"SIGSEGV", "Segmentation fault",       COREDUMP},
    {"SIGUSR2", "User defined signal 2",    TERMINATE},
    {"SIGPIPE", "Broken pipe",              TERMINATE},
    {"SIGALRM", "Alarm clock",              TERMINATE},
    {"SIGTERM", "Terminated",               TERMINATE},
    {"SIGSTKFLT", "Stack fault",            TERMINATE},
    {"SIGCHLD", "Child process status",     IGNORE},    //  alias:  SIGCLD
    {"SIGCONT", "Continued",                CONTINUE},
    {"SIGSTOP", "Stopped (signal)",         STOP},
    {"SIGTSTP", "Stopped",                  STOP},      //  20
    {"SIGTTIN", "Stopped (tty input)",      STOP},
    {"SIGTTOU", "Stopped (tty output)",     STOP},
    {"SIGURG",  "Urgent I/O condition",     IGNORE},
    {"SIGXCPU", "CPU time limit exceeded",  COREDUMP},
    {"SIGXFSZ", "File size limit exceeded", COREDUMP},
    {"SIGVTALRM", "Virtual timer expired",  TERMINATE},
    {"SIGPROF", "Profiling timer expired",  TERMINATE},
    {"SIGWINCH",  "Window changed",         IGNORE},
    {"SIGIO",   "I/O possible",             TERMINATE}, //  alias:  SIGPOLL
    {"SIGPWR",  "Power failure",            TERMINATE}, //  alias:  SIGINFO,    30
};

void IpcSignalTest::DefaultActionTest(const int signum, const bool expectStop, const bool coredump)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid > 0) { // parent
        Msleep(20);
        LOGD("before kill");
        kill(pid, signum);
        if (!expectStop) {
            Msleep(20);
            AssertProcAlive(pid);
            Msleep(20);
            WaitProcExitedOK(pid);
        } else {
            WaitProcKilled(pid, signum);
            if (coredump) {
                // check codedump, liteos not support yet
            }
        }
    } else { // child
        LOGD("child start");
        KeepRun(KEEP_RUN_TIME);
        if (expectStop) {
            LOG("Child should has been Terminated, but still alive.");
            exit(1);
        }
        exit(0);
    }
}

void IpcSignalTest::SendAndRecvTest(const int signum)
{
    pid_t pid1, pid2;
    bool useBrother = GetRandom(100) % 2; // if use brother to send the signal
    pid1 = fork();
    ASSERT_TRUE(pid1 >= 0) << "======== Fork1 Error! =========";
    if (pid1 > 0) { // parent
        LOGD("in parent...");
        if (useBrother) {
            pid2 = fork();
            ASSERT_TRUE(pid2 >= 0) << "======== Fork2 Error! =========";
            if (pid2 == 0) { // child 2
                Msleep(50);
                kill(pid1, signum);
                exit(0);
            }
            // parent
            Msleep(20);
            LOG("check if child2 exited OK");
            WaitProcExitedOK(pid2);
        } else {
            Msleep(100);
            kill(pid1, signum);
        }
        AssertProcAlive(pid1);

        Msleep(20); // child should exited now
        LOG("check if child exited OK");
        WaitProcExitedOK(pid1);
    } else { // child 1, the receiver
        LOGD("in child, pid=%d", getpid());
        handler_type rt = signal(signum, SignalHandler);
        if (rt == SIG_ERR) {
            LOG("set %d signal handler failed, errno=%d", signum, errno);
            exit(1);
        }
        Msleep(150);
        if (mReceivedSignal != signum) {
            LOG("SignalHandler check fail, expected=%d, received=%d", signum, mReceivedSignal);
            exit(1);
        }
        LOGD("child sleeping....");
        Msleep(20);
        LOGD("child exit 0....");
        exit(0);
    }
}

int IpcSignalTest::CheckSigString(const char* outfile, const char* expectStr)
{
    const int bufLen = 64;
    char buffer[bufLen];

    FILE *fp = fopen(outfile, "rb");
    size_t bytes = fread(buffer, 1, bufLen, fp);
    buffer[bytes] = 0;
    LOGD("%d bytes read from logfile:%s", bytes, buffer);
    fclose(fp);
    size_t expectLen = strlen(expectStr);
    LOGD("expect string len = %d", expectLen);
    if (bytes != (expectLen + 1)) {
        LOG("bytes number read from stderr file error, expect:%d, actual:%d",
            expectLen + 1, bytes);
        return 1;
    }
    return strncmp(expectStr, buffer, expectLen);
}

void IpcSignalTest::SignalFailTest(int signum, handler_type h, int expectErrno)
{
    errno = 0;
    handler_type rt = signal(signum, h);
    ASSERT_EQ(rt, SIG_ERR) << "signal error for " << signum;
    ASSERT_EQ(errno, expectErrno) << "signal error for " << signum;
}
void IpcSignalTest::SigpendingFailTest(sigset_t* pset)
{
    errno = 0;
    int rt = sigpending(pset);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EFAULT);
}
void IpcSignalTest::SigtimedwaitFailTest(const sigset_t *set, siginfo_t* info, const timespec* timeout, int expectErrno)
{
    errno = 0;
    int rt = sigtimedwait(set, info, timeout);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, expectErrno);
}

// thread function for testcase: 'testPthreadkill'
void* IpcSignalTest::ThreadFunc1(void* arg)
{
    int sigNo = (int)((uintptr_t)arg);
    signal(sigNo, SignalHandler);
    Msleep(60);
    return nullptr;
}

// thread function for testcase: 'testPthreadkillMain'
void* IpcSignalTest::ThreadFunc2(void* arg)
{
    pthread_t* tid = (pthread_t*)arg;
    int ret = pthread_kill(*tid, SIGINT);
    if (ret != 0) {
        LOG("pthread_kill failed, errno=%d", ret);
        return (void*)1;
    }
    LOG("pthread_kill send signal(%d) ok", SIGINT);
    return nullptr;
}

// thread function for testcase: 'testPthreadSigmask'
void* IpcSignalTest::ThreadFuncForSigmask1(void* arg)
{
    int rt;
    int type = (int)((uintptr_t)arg);
    if (type == 1) {
        signal(SIGINT, SignalHandler);
    }

    LOG("block SIGINT");
    sigset_t sigmask, oldmask;
    sigemptyset(&sigmask);
    sigemptyset(&oldmask);
    sigaddset(&sigmask, SIGINT);
    sigaddset(&sigmask, SIGUSR1);
    rt = pthread_sigmask(SIG_BLOCK, &sigmask, &oldmask);
    EXPECT_EQ(rt, 0);
    raise(SIGINT);
    EXPECT_EQ(mReceivedSignal, 0) << "SignalHandler check fail, SIGINT should has blocked!";

    LOG("unblock SIGINT");
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGINT);
    rt = pthread_sigmask(SIG_UNBLOCK, &sigmask, nullptr);
    EXPECT_EQ(rt, 0);

    LOG("check the new block set");
    sigemptyset(&oldmask);
    pthread_sigmask(SIG_UNBLOCK, nullptr, &oldmask);
    EXPECT_EQ(sigismember(&oldmask, SIGINT), 0) << "SIGINT should has deleted from block set!";
    EXPECT_EQ(sigismember(&oldmask, SIGUSR1), 1) << "SIGUSR1 should still in block set!";

    EXPECT_EQ(mReceivedSignal, SIGINT) << "SignalHandler check fail, SIGINT should deliver after unblock!";
    return nullptr;
}

// thread function for testcase: 'testPthreadSigmaskInherit'
void* IpcSignalTest::ThreadFuncForSigmask2(void* arg)
{
    sigset_t sigmask, oldmask;
    sigemptyset(&sigmask);
    sigemptyset(&oldmask);

    LOG("SubThread: check the block set is inherited");
    pthread_sigmask(SIG_BLOCK, nullptr, &oldmask);
    if (!sigismember(&oldmask, SIGUSR1)) {
        LOG("initial block set should include SIGUSR1!");
        return (void*)1;
    }

    LOG("SubThread: add SIGINT to block set");
    sigaddset(&sigmask, SIGINT);
    int rt = pthread_sigmask(SIG_BLOCK, &sigmask, &oldmask);
    EXPECT_EQ(rt, 0);

    LOG("SubThread: check the new block set");
    sigemptyset(&oldmask);
    pthread_sigmask(SIG_UNBLOCK, nullptr, &oldmask);
    if (!sigismember(&oldmask, SIGINT)) {
        LOG("failed to add SIGINT to block set!");
        return (void*)1;
    }
    if (!sigismember(&oldmask, SIGUSR1)) {
        LOG("SIGUSR1 should still in block set!");
        return (void*)1;
    }
    return nullptr;
}