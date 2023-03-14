/**
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include "securec.h"

using namespace std;
using namespace testing::ext;

static const int TEST_FD_COUNT = 10;
static const int STACK_PORT = 2277;
static const int PEER_PORT = 2277;
static const int BUF_SIZE = (1024 * 8);
static const char* g_udpMsg = "Hi, I am UDP";
static const char* g_srvMsg = "Hi, I am TCP server";
static const char* g_cliMsg = "Hi, I am TCP client";
static const char* g_localHost = "127.0.0.1";

static pthread_barrier_t g_barrier;
#define WAIT() pthread_barrier_wait(&g_barrier)

class ActsLwipTest : public testing::Test {
protected:
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void)
    {
        // Ignore SIGPIPE Signals
        signal(SIGPIPE, SIG_IGN);
    }
    // TearDownTestCase: Testsuit teardown, run after last testcase
    static void TearDownTestCase(void) {}
    // Testcase setup
    virtual void SetUp() {}
    // Testcase teardown
    virtual void TearDown() {}
};

static int CommInitTcpServer(int port)
{
    int srvFd = socket(AF_INET, SOCK_STREAM, 0);
    if (srvFd == -1) {
        printf("CommInitTcpServer]socket fail!\n");
        return -1;
    }
    int flag = 1;
    int ret = setsockopt(srvFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    if (ret != 0) {
        printf("[CommInitTcpServer]setsockopt fail, ret[%d]!\n", ret);
    }
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(port);
    ret = ::bind(srvFd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    if (ret != 0) {
        printf("[CommInitTcpServer]::bind fail!\n");
        close(srvFd);
        return -1;
    }
    int backlog = 5;
    ret = listen(srvFd, backlog);
    if (ret != 0) {
        printf("[CommInitTcpServer]listen fail!\n");
        close(srvFd);
        return -1;
    }
    printf("[CommInitTcpServer]success,Fd[%d]\n", srvFd);
    return srvFd;
}

static void* CommTcpClientTask(void* param)
{
    int* ret = (int *)malloc(sizeof(int));
    if (ret == nullptr) {
        printf("[comm client]malloc fail\n");
        return nullptr;
    }
    *ret = 0;
    int clnFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clnFd == -1) {
        printf("[comm client]socket fail\n");  
        *ret = -1;
        return ret;
    }
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(PEER_PORT + 1);
    int rst = connect(clnFd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    if (rst != 0) {
        printf("[comm client]connect fail\n");
        close(clnFd);
        *ret = -1;
        return ret;
    }
    int sleepSecond = *((int *)param);
    int count = 4;
    char dataBuf[50] = {0};
    char sendMsgList[5][50] = {"Hi, I'm client,FD:[%d]", "client:123456789abcdefg,FD:[%d]",
        "client:!!@@##$$%%^^&&**(()),FD:[%d]", "client:(((112233445566778899))),FD:[%d]", "bye"};
    for (int i = 0; i < count; i++) {
        sleep(sleepSecond);
        (void) memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
        rst = sprintf_s(dataBuf, sizeof(dataBuf), sendMsgList[i], clnFd);
        if (rst < 0) {
            printf("[comm client]sprintf_s error!\n");
            continue;
        }
        rst = send(clnFd, dataBuf, strlen(dataBuf), 0);
        if (rst <= 0) {
            *ret = -1;
            printf("[comm client][%d]send fail\n", clnFd);
        }
        (void) memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
        rst = recv(clnFd, dataBuf, sizeof(dataBuf), 0);
        if (rst > 0) {
            printf("[comm client][%d]recv msg[%s]\n", clnFd, dataBuf);
        } else {
            printf("[comm client][%d]recv no data\n", clnFd);
        }
    }
    // send 'bye', finish
    rst = send(clnFd, sendMsgList[count], strlen(sendMsgList[count]), 0);
    if (rst <= 0) {
        *ret = -1;
        printf("[comm client][%d]send fail\n", clnFd);
    }
    rst = close(clnFd);
    if (rst != 0) {
        *ret = -1;
        printf("[comm client][%d]close fd fail\n", clnFd);
    }
    return ret;
}

static void* SampleTcpServerTask(void* p)
{
    int srvFd = CommInitTcpServer(STACK_PORT);
    EXPECT_NE(-1, srvFd);

    WAIT();
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);
    int clnFd = accept(srvFd, (struct sockaddr*)&clnAddr, &clnAddrLen);
    printf("[tcp server]accept <%s:%d>\n", inet_ntoa(clnAddr.sin_addr), ntohs(clnAddr.sin_port));
    EXPECT_NE(-1, clnFd);
    printf("[tcp server]lsfd[%d], sfd[%d]\n", srvFd, clnFd);

    struct sockaddr addr = {0};
    socklen_t addrLen = sizeof(addr);
    int ret = getsockname(clnFd, &addr, &addrLen);
    printf("[tcp server]getsockname %s:%d\n", inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr),
        ntohs(((struct sockaddr_in*)&addr)->sin_port));
    EXPECT_EQ(0, ret);
    EXPECT_EQ(inet_addr(g_localHost), ((struct sockaddr_in*)&addr)->sin_addr.s_addr);

    ret = getpeername(clnFd, &addr, &addrLen);
    printf("[tcp server]getpeername %s:%d\n", inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr),
        ntohs(((struct sockaddr_in*)&addr)->sin_port));
    EXPECT_EQ(0, ret);
    EXPECT_EQ(inet_addr(g_localHost), ((struct sockaddr_in*)&addr)->sin_addr.s_addr);

    static char buf[BUF_SIZE + 1] = {0};
    unsigned int srvMsgLen = strlen(g_srvMsg);
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(buf, BUF_SIZE, g_srvMsg);
    EXPECT_EQ(0, ret);
    ret = send(clnFd, buf, srvMsgLen, 0);
    printf("[tcp server]send, ret=%d\n", ret);
    EXPECT_EQ(srvMsgLen, (unsigned int)ret);

    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = recv(clnFd, buf, sizeof(buf), 0);
    printf("[tcp server]recv, ret=%d, msg[%s]\n", ret, buf);
    EXPECT_EQ(strlen(g_cliMsg), (unsigned int)ret);

    WAIT();
    int len = 2;
    struct msghdr msg = {0};
    struct iovec iov[2] = {0};
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(g_localHost);
    clnAddr.sin_port = htons(PEER_PORT);
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(buf, BUF_SIZE, g_srvMsg);
    EXPECT_EQ(0, ret);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = len;
    iov[0].iov_base = buf;
    iov[0].iov_len = srvMsgLen;
    iov[1].iov_base = buf;
    iov[1].iov_len = srvMsgLen;
    ret = sendmsg(clnFd, &msg, 0);
    printf("[tcp server]sendmsg, ret=%d\n", ret);
    EXPECT_EQ(len * srvMsgLen, (unsigned int)ret);

    WAIT();
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    (void) memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    iov[0].iov_base = buf;
    iov[0].iov_len = sizeof(buf);
    ret = recvmsg(clnFd, &msg, 0);
    printf("[tcp server]recvmsg on socket %d:%d, msg[%s]\n", clnFd, ret, buf);
    EXPECT_EQ(len * strlen(g_cliMsg), (unsigned int)ret);

    ret = shutdown(clnFd, SHUT_RDWR);
    printf("[tcp server]shutdown\n");
    EXPECT_EQ(0, ret);
    ret = close(clnFd);
    EXPECT_NE(-1, ret);
    ret = close(srvFd);
    EXPECT_NE(-1, ret);
    return nullptr;
}

static void* SampleTcpClientTask(void* p)
{
    int clnFd = socket(AF_INET, SOCK_STREAM, 0);
    printf("[tcp client]create socket\n");
    EXPECT_NE(-1, clnFd);

    WAIT();
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(PEER_PORT);
    int ret = connect(clnFd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    printf("[tcp client]connect %s:%d, fd[%d]\n", inet_ntoa(srvAddr.sin_addr), ntohs(srvAddr.sin_port), clnFd);
    EXPECT_EQ(0, ret);

    struct sockaddr addr = {0};
    socklen_t addrLen = sizeof(addr);
    ret = getpeername(clnFd, &addr, &addrLen);
    printf("[tcp client]getpeername %s:%d\n", inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr),
        ntohs(((struct sockaddr_in*)&addr)->sin_port));
    EXPECT_EQ(0, ret);
    EXPECT_EQ(inet_addr(g_localHost), ((struct sockaddr_in*)&addr)->sin_addr.s_addr);

    ret = getsockname(clnFd, &addr, &addrLen);
    printf("[tcp client]getsockname %s:%d\n", inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr),
        ntohs(((struct sockaddr_in*)&addr)->sin_port));
    EXPECT_EQ(0, ret);
    EXPECT_EQ(inet_addr(g_localHost), ((struct sockaddr_in*)&addr)->sin_addr.s_addr);

    static char buf[BUF_SIZE + 1] = {0};
    unsigned int cliMsgLen = strlen(g_cliMsg);
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(buf, BUF_SIZE, g_cliMsg);
    EXPECT_EQ(0, ret);
    ret = send(clnFd, buf, cliMsgLen, 0);
    printf("[tcp client]send, ret=%d\n", ret);
    EXPECT_EQ(cliMsgLen, (unsigned int)ret);

    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = recv(clnFd, buf, sizeof(buf), 0);
    printf("[tcp client]recv, ret=%d, msg[%s]\n", ret, buf);
    EXPECT_EQ(strlen(g_srvMsg), (unsigned int)ret);

    WAIT();
    int len = 2;
    struct msghdr msg = {0};
    struct iovec iov[2] = {0};
    struct sockaddr_in clnAddr = {0};
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(g_localHost);
    clnAddr.sin_port = htons(PEER_PORT);
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(buf, BUF_SIZE, g_cliMsg);
    EXPECT_EQ(0, ret);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = len;
    iov[0].iov_base = buf;
    iov[0].iov_len = cliMsgLen;
    iov[1].iov_base = buf;
    iov[1].iov_len = cliMsgLen;
    ret = sendmsg(clnFd, &msg, 0);
    printf("[tcp client]sendmsg, ret=%d\n", ret);
    EXPECT_EQ(len * cliMsgLen, (unsigned int)ret);

    WAIT();
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    (void) memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    iov[0].iov_base = buf;
    iov[0].iov_len = sizeof(buf);
    ret = recvmsg(clnFd, &msg, 0);
    printf("[tcp client]recvmsg, ret=%d, msg[%s]\n", ret, buf);
    EXPECT_EQ(len * strlen(g_srvMsg), (unsigned int)ret);

    ret = shutdown(clnFd, SHUT_RDWR);
    printf("[tcp client]shutdown\n");
    EXPECT_EQ(0, ret);
    ret = close(clnFd);
    EXPECT_NE(-1, ret);
    return nullptr;
}

static int TcpServerForFork()
{
    int timeout = 10;
    char dataBuf[50] = {0};
    int srvFd = CommInitTcpServer(STACK_PORT + 1);
    if (srvFd == -1) {
        return -1;
    }
    printf("[server process]wait client...\n");
    int ret = -1;
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);
    int cliFd = accept(srvFd, (struct sockaddr*)&clnAddr, &clnAddrLen);
    if (cliFd == -1) {
        printf("[server process]accept fail!\n");
        goto FINALLY;
    }
    while (timeout > 0) {
        (void) memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
        ret = recv(cliFd, dataBuf, sizeof(dataBuf), 0);
        if (ret > 0) {
            printf("[server process]recv msg[%s]\n", dataBuf);
            if (strcmp(dataBuf, "bye") == 0) {
                printf("[server process]break\n");
                break;
            }
        } else {
            printf("[server process]recv no data\n");
        }
        (void) memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
        sprintf_s(dataBuf, sizeof(dataBuf), "server timeout:%d", timeout);
        ret = send(cliFd, dataBuf, strlen(dataBuf), 0);
        if (ret > 0) {
            printf("[server process]send success, msg[%s]\n", dataBuf);
        } else {
            printf("[server process]send fail!\n");
        }
        timeout--;
    }
    close(cliFd);
    printf("[server process]over\n");

FINALLY:
    close(srvFd);
    return ret;
}

static int WaitProcessExit(int pid, int* code)
{
    int status;
    int rt = waitpid(pid, &status, 0);
    errno = 0;
    int retError = -2;
    if (rt == -1) {
        printf("[WaitProcessExit]waitpid return -1, errno=%d:%s\n", errno, strerror(errno));
        return -1;
    } else if (rt == 0) {
        return 0;
    } else if (rt != pid) { // waitpid return error
        printf("[WaitProcessExit]waitpid error\n");
        return retError;
    }

    int retWterm = 2;
    int retWstop = 3;
    int other = 4;
    if (WIFEXITED(status)) {
        *code = WEXITSTATUS(status);
        return 1;
    } else if (WIFSIGNALED(status)) {
        *code = WTERMSIG(status);
        return retWterm;
    } else if (WIFSTOPPED(status)) {
        *code = WSTOPSIG(status);
        return retWstop;
    }
    return other;
}

static int SelectServerForFork(unsigned int timeoutSec)
{
    int srvFd = CommInitTcpServer(STACK_PORT + 1);
    if (srvFd == -1) {
        return -1;
    }
    int ret;
    int cliCount = 0;
    int maxFd = srvFd;
    fd_set readSet;
    char dataBuf[50] = {0};
    int fds[TEST_FD_COUNT] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    fds[0] = srvFd;
    long timeout = static_cast<long>(timeoutSec);
    struct timeval timev = {.tv_sec = timeout, .tv_usec = 0};
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);
    while (1) {
        FD_ZERO(&readSet);
        for (int i = 0; i < TEST_FD_COUNT; i++) {
            if (fds[i] != -1) {
                FD_SET(fds[i], &readSet);
                if (maxFd < fds[i]) {
                    maxFd = fds[i];
                }
                printf("[select process]fd info[%d:%d]\n", i, fds[i]);
            }
        }
        ret = select(maxFd + 1, &readSet, NULL, NULL, &timev);
        if (ret == 0) {
            printf("[select process]select timeout!\n");
            ret = -2;
            break;
        }
        if (ret == -1) {
            fprintf(stderr, "[select process]select fail[%s]\n", strerror(errno));
            break;
        }
        if (FD_ISSET(srvFd, &readSet)) {
            int cliFd = accept(srvFd, (struct sockaddr*)&clnAddr, &clnAddrLen);
            if (cliFd == -1) {
                printf("[select process]accept fail!\n");
                ret = -1;
                break;
            } else {
                for (int i = 0; i < TEST_FD_COUNT; i++) {
                    if (fds[i] == -1) {
                        fds[i] = cliFd;
                        break;
                    }
                }
                cliCount++;
                printf("[select process]accept success, cliCount[%d]\n", cliCount);
            }
            continue;
        }
        for (int i = 0; i < TEST_FD_COUNT; i++) {
            if (fds[i] == -1) {
                continue;
            }
            if (FD_ISSET(fds[i], &readSet)) {
                (void) memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
                int len = recv(fds[i], dataBuf, sizeof(dataBuf), 0);
                if (len > 0) {
                    printf("[select process][%d]recv msg[%s]\n", fds[i], dataBuf);
                    if (strcmp(dataBuf, "bye") == 0) {
                        printf("[select process][%d]client bye,cliCount[%d]\n", fds[i], cliCount);
                        FD_CLR(fds[i], &readSet);
                        close(fds[i]);
                        fds[i] = -1;
                        cliCount--;
                        continue;
                    }
                    len = send(fds[i], dataBuf, strlen(dataBuf), 0);
                    if (len > 0) {
                        printf("[select process][%d]send success\n", fds[i]);
                    } else {
                        printf("[select process][%d]send fail\n", fds[i]);
                    }
                }
            }
        }
        if (cliCount == 0) {
            ret = 0;
            printf("[select process]cliCount=0, over!\n");
            break;
        }
    }
    close(srvFd);
    return ret;
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0100
 * @tc.name      : sample test tcp
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testTcpSample, Function | MediumTest | Level1)
{
    pthread_t srv;
    pthread_t cli;

    int ret = pthread_barrier_init(&g_barrier, 0, 2);
    EXPECT_EQ(0, ret);

    ret = pthread_create(&srv, nullptr, SampleTcpServerTask, nullptr);
    EXPECT_EQ(0, ret);
    ret = pthread_create(&cli, nullptr, SampleTcpClientTask, nullptr);
    EXPECT_EQ(0, ret);

    ret = pthread_join(cli, nullptr);
    EXPECT_EQ(0, ret);
    ret = pthread_join(srv, nullptr);
    EXPECT_EQ(0, ret);

    ret = pthread_barrier_destroy(&g_barrier);
    EXPECT_EQ(0, ret);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0200
 * @tc.name      : sample test udp
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testUdpSample, Function | MediumTest | Level2)
{
    int len = 2;
    char buf[BUF_SIZE + 1];
    struct msghdr msg = {0};
    struct iovec iov[2] = {0};
    struct sockaddr_in srvAddr = {0};
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);

    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(-1, sfd);
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(STACK_PORT);
    int ret = ::bind(sfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    ASSERT_EQ(0, ret);

    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(g_localHost);
    clnAddr.sin_port = htons(PEER_PORT);
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(buf, BUF_SIZE, g_udpMsg);
    EXPECT_EQ(0, ret);
    ret = sendto(sfd, buf, strlen(g_udpMsg), 0, (struct sockaddr*)&clnAddr, (socklen_t)sizeof(clnAddr));
    EXPECT_NE(-1, ret);

    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr*)&clnAddr, &clnAddrLen);
    EXPECT_EQ(strlen(g_udpMsg), (unsigned int)ret);

    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(g_localHost);
    clnAddr.sin_port = htons(PEER_PORT);
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(buf, BUF_SIZE, g_udpMsg);
    EXPECT_EQ(0, ret);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = len;
    iov[0].iov_base = buf;
    iov[0].iov_len = strlen(g_udpMsg);
    iov[1].iov_base = buf;
    iov[1].iov_len = strlen(g_udpMsg);
    ret = sendmsg(sfd, &msg, 0);
    EXPECT_EQ(len*strlen(g_udpMsg), (unsigned int)ret);

    ret = close(sfd);
    EXPECT_NE(-1, ret);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0300
 * @tc.name      : test inter-process network communication
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInterProcessComm, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) {
        // child:tcp server
        int exitCode = 0;
        int ret = TcpServerForFork();
        if (ret == -1) {
            printf("[tcp server]run fail\n");
            exitCode = 1;
        }
        exit(exitCode);
    } else {
        // parent:tcp client
        sleep(2);
        int second = 1;
        void *ret = CommTcpClientTask(&second);
        if (ret == nullptr) {
            printf("[test select]CommClientTask ret is null\n");
            EXPECT_TRUE(0);
        } else {
            printf("[test select]CommClientTask ret...\n");
            int rst = *((int *)ret);
            printf("[test select]CommClientTask ret [%d]\n", rst);
            EXPECT_EQ(0, rst);
            free(ret);
        }
        int exitCode;
        int procStat = WaitProcessExit(pid, &exitCode);
        ASSERT_EQ(1, procStat);
        ASSERT_EQ(0, exitCode);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0400
 * @tc.name      : test select timeout
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testSelectTimeout, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) {
        // child: server
        int ret = SelectServerForFork(2);
        if (ret == -2) {
            exit(0);
        } else {
            printf("[select server]fail, ret is[%d]\n", ret);
            exit(1);
        }
    } else {
        int exitCode;
        int procStat = WaitProcessExit(pid, &exitCode);
        ASSERT_EQ(procStat, 1);
        ASSERT_EQ(exitCode, 0);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0500
 * @tc.name      : test select with one client
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testSelectOneClient, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) {
        // child: server
        int exitCode = 0;
        int ret = SelectServerForFork(15);
        if (ret != 0) {
            printf("[select server]run fail\n");
            exitCode = 1;
        }
        printf("[select server]ret[%d]\n", ret);
        exit(exitCode);
    } else {
        // parent: client
        sleep(2);
        int second = 1;
        void *ret = CommTcpClientTask(&second);
        if (ret == nullptr) {
            printf("[test select]CommClientTask ret is null\n");
            EXPECT_TRUE(0);
        } else {
            int rst = *((int *)ret);
            printf("[test select]CommClientTask ret [%d]\n", rst);
            EXPECT_EQ(0, rst);
            free(ret);
        }
        int exitCode;
        int procStat = WaitProcessExit(pid, &exitCode);
        ASSERT_EQ(1, procStat);
        ASSERT_EQ(0, exitCode);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0600
 * @tc.name      : test select with multi clients
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testSelectMultiClients, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) {
        // child: server
        int exitCode = 0;
        int ret = SelectServerForFork(15);
        if (ret != 0) {
            printf("[select server]run fail\n");
            exitCode = 1;
        }
        printf("[select server]ret[%d]\n", ret);
        exit(exitCode);
    } else {
        // parent: client
        sleep(2);
        int ret;
        pthread_t pCli[4];
        int interval[4] = {2, 1, 3, 1};
        for (int i = 0; i < 4; i++) {
            ret = pthread_create(&pCli[i], nullptr, CommTcpClientTask, &interval[i]);
            EXPECT_EQ(0, ret) << "Errinfo:pthread_create index is [" << i << "]";
        }

        void *rst = nullptr;
        for (int i = 0; i < 4; i++) {
            ret = pthread_join(pCli[i], &rst);
            EXPECT_EQ(0, ret) << "Errinfo:pthread_join index is [" << i << "]";
            if (rst != nullptr) {
                printf("[test select]client thread[%d] return[%d]\n", i, *((int *)rst));
                EXPECT_EQ(0, *((int *)rst));
                free(rst);
            }
        }
        int exitCode;
        int procStat = WaitProcessExit(pid, &exitCode);
        ASSERT_EQ(1, procStat);
        ASSERT_EQ(0, exitCode);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0700
 * @tc.name      : test socket operation
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testSocketOpt, Function | MediumTest | Level2)
{
    socklen_t len;
    struct timeval timeout = {0};
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    EXPECT_NE(-1, fd);

    int error = -1;
    len = sizeof(error);
    int ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, error);

    len = sizeof(timeout);
    ret = getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, &len);
    EXPECT_EQ(0, ret);

    timeout.tv_sec = 1000;
    len = sizeof(timeout);
    ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, len);
    EXPECT_EQ(0, ret);

    (void) memset_s(&timeout, len, 0, len);
    ret = getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, &len);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(1000, timeout.tv_sec);

    int flag = 1;
    ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    EXPECT_EQ(0, ret);

    flag = 0;
    len = sizeof(flag);
    ret = getsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, &len);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(1, flag);

    error = -1;
    len = sizeof(error);
    ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, error);

    ret = close(fd);
    EXPECT_EQ(0, ret);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0800
 * @tc.name      : test getsockname and getpeername invalid input
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testGetSocketNameInvalidInput, Function | MediumTest | Level3)
{
    struct sockaddr addr = {0};
    socklen_t addrLen = sizeof(addr);
    int ret = getsockname(-1, &addr, &addrLen);
    EXPECT_EQ(-1, ret);
    ret = getpeername(-1, &addr, &addrLen);
    EXPECT_EQ(-1, ret);
    ret = getsockname(0, &addr, &addrLen);
    EXPECT_EQ(-1, ret);
    ret = getpeername(0, &addr, &addrLen);
    EXPECT_EQ(-1, ret);
    ret = getsockname(1, &addr, &addrLen);
    EXPECT_EQ(-1, ret);
    ret = getpeername(1, &addr, &addrLen);
    EXPECT_EQ(-1, ret);
    ret = getsockname(130, &addr, &addrLen);
    EXPECT_EQ(-1, ret);
    ret = getpeername(130, &addr, &addrLen);
    EXPECT_EQ(-1, ret);
    ret = getsockname(10, nullptr, &addrLen);
    EXPECT_EQ(-1, ret);
    ret = getpeername(10, nullptr, &addrLen);
    EXPECT_EQ(-1, ret);
    ret = getsockname(10, &addr, nullptr);
    EXPECT_EQ(-1, ret);
    ret = getpeername(10, &addr, nullptr);
    EXPECT_EQ(-1, ret);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0900
 * @tc.name      : test inet_pton IPv4 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInetPtonIpv4Normal, Function | MediumTest | Level2)
{
    int ret;
    struct in_addr rst = {0};
    char cpAddrs[4][16] = {"10.58.212.100", "0.0.0.0", "255.0.0.0", "255.255.255.255"};
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned int expectLittle[4] = {1691630090, 0, 255, 4294967295};
#else
    unsigned int expectBig[4] = {171627620, 0, 4278190080, 4294967295};
#endif

    for (int i = 0; i < 4; i++) {
        ret = inet_pton(AF_INET, cpAddrs[i], &rst);
        EXPECT_EQ(1, ret);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        EXPECT_EQ(expectLittle[i], rst.s_addr);
#else
        EXPECT_EQ(expectBig[i], rst.s_addr);
#endif
        printf("inet_pton %s: un[%u],s[%d],hex[%x]\n", cpAddrs[i], rst.s_addr, rst.s_addr, rst.s_addr);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1000
 * @tc.name      : test inet_pton IPv4 abnormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInetPtonIpv4Abnormal, Function | MediumTest | Level2)
{
    int ret;
    struct in_addr rst = {0};
    char cpAddrs[6][16] = {"256.0.0.1", "a.a.a.a", "....", "#", "127.0.0.f", "0:0:0:0:0:0:0:1"};
    for (int i = 0; i < 6; i++) {
        ret = inet_pton(AF_INET, cpAddrs[i], &rst);
        EXPECT_EQ(0, ret) << "ErrInfo:inet_pton abnormal [" << cpAddrs[i] << "]";
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1100
 * @tc.name      : test inet_pton IPv6 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInetPtonIpv6Normal, Function | MediumTest | Level2)
{
    int ret;
    struct in6_addr rst = {0};
    char cpAddrs[6][40] = {"fc00:0101:0011:0011:0011:0011:0011:0011", "0:0:0:0:0:0:0:0", 
        "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF", "::", "1::", "fc00:0011:0011:0011:0011:11:11:11"};
    for (int i = 0; i < 6; i++) {
        ret = inet_pton(AF_INET6, cpAddrs[i], &rst);
        EXPECT_EQ(1, ret) << "ErrInfo:inet_pton [" << cpAddrs[i] << "]";
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1200
 * @tc.name      : test inet_pton IPv6 abnormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInetPtonIpv6Abnormal, Function | MediumTest | Level2)
{
    int ret;
    struct in6_addr rst = {0};
    char cpAddrs[7][40] = {"127.0.0.1", "f", ":", "0:0", "1:::", ":::::::",
        "1111:1111:1111:1111:1111:1111:1111:111G"};
    for (int i = 0; i < 7; i++) {
        ret = inet_pton(AF_INET6, cpAddrs[i], &rst);
        EXPECT_EQ(0, ret) << "ErrInfo:inet_pton abnormal [" << cpAddrs[i] << "]";
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1300
 * @tc.name      : test inet_pton with invalid family
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInetPtonInvalidFamily, Function | MediumTest | Level2)
{
    struct in_addr rst = {0};
    int ret = inet_pton(AF_IPX, "127.0.0.1", &rst);
    EXPECT_EQ(-1, ret);
    ret = inet_pton(-1, "127.0.0.1", &rst);
    EXPECT_EQ(-1, ret);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1400
 * @tc.name      : test inet_ntop IPv4 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInetNtopIpv4Normal, Function | MediumTest | Level2)
{
    const char* ret = nullptr;
    struct in_addr inputAddr = {0};
    char rstBuff[INET_ADDRSTRLEN];
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned int inputLittle[4] = {0x64d43a0a, 0, 255, 4294967295};
#else
    unsigned int inputBig[4] = {171627620, 0, 4278190080, 4294967295};
#endif

    char expectAddrs[4][16] = {"10.58.212.100", "0.0.0.0", "255.0.0.0", "255.255.255.255"};
    for (int i = 0; i < 4; i++) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        inputAddr.s_addr = inputLittle[i];
#else
        inputAddr.s_addr = inputBig[i];
#endif
        ret = inet_ntop(AF_INET, &inputAddr, rstBuff, sizeof(rstBuff));
        ASSERT_NE(ret, nullptr);
        printf("inet_ntop expect [%s]: ret[%s], buf[%s]\n", expectAddrs[i], ret, rstBuff);
        EXPECT_STREQ(expectAddrs[i], ret);
        EXPECT_STREQ(expectAddrs[i], rstBuff);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1500
 * @tc.name      : test inet_ntop IPv4 boundary input
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInetNtopIpv4Abnormal, Function | MediumTest | Level2)
{
    const char* ret = nullptr;
    struct in_addr inputAddr = {0};
    char rstBuff[INET_ADDRSTRLEN];
    char expectStr[2][16] = {"255.255.255.255", "0.0.0.0"};
    for (int i = 0; i < 2; i++) {
        inputAddr.s_addr = (i == 0 ? -1 : 4294967296);
        ret = inet_ntop(AF_INET, &inputAddr, rstBuff, sizeof(rstBuff));
        ASSERT_TRUE(ret != NULL);
        EXPECT_STREQ(expectStr[i], ret);
        EXPECT_STREQ(expectStr[i], rstBuff);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1600
 * @tc.name      : test inet_ntop IPv6 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInetNtopIpv6Normal, Function | MediumTest | Level2)
{
    int iret;
    const char* ret = nullptr;
    struct in6_addr inputAddr = {0};
    char rstBuff[INET6_ADDRSTRLEN];
    char inputAddrs[6][40] = {"fc00:0101:0011:0011:0011:0011:0011:0011", "0:0:0:0:0:0:0:0", 
        "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF", "::", "1::", "fc00:0011:0011:0011:1100:11:11:11"};
    char expectAddrs[6][40] = {"fc00:101:11:11:11:11:11:11", "::", 
            "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", "::", "1::", "fc00:11:11:11:1100:11:11:11"};
    for (int i = 0; i < 6; i++) {
        iret = inet_pton(AF_INET6, inputAddrs[i], &inputAddr);
        ASSERT_EQ(1, iret);
        ret = inet_ntop(AF_INET6, &inputAddr, rstBuff, sizeof(rstBuff));
        if (ret == nullptr) {
            EXPECT_TRUE(ret != NULL) << "ErrInfo:inet_ntop NULL [" << expectAddrs[i] << "]";
        } else {
            printf("inet_ntop expect [%s]: ret[%s], buf[%s]\n", expectAddrs[i], ret, rstBuff);
            EXPECT_STREQ(expectAddrs[i], ret);
            EXPECT_STREQ(expectAddrs[i], rstBuff);
        }
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1700
 * @tc.name      : test inet_ntop IPv6 boundary input
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInetNtopIpv6Abnormal, Function | MediumTest | Level2)
{
    const char* ret = nullptr;
    struct in6_addr inputAddr = {0};
    char rstBuff[INET6_ADDRSTRLEN];
    char expectStr[2][40] = {"ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", "::"};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 16; j++) {
            inputAddr.s6_addr[j] = (i == 0 ? -1 : 256);
        }
        ret = inet_ntop(AF_INET6, &inputAddr, rstBuff, sizeof(rstBuff));
        ASSERT_TRUE(ret != NULL);
        EXPECT_STREQ(expectStr[i], ret);
        EXPECT_STREQ(expectStr[i], rstBuff);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1800
 * @tc.name      : test inet_ntop with invalid family
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInetNtopInvalidFamily, Function | MediumTest | Level2)
{
    int iret;
    const char* ret = nullptr;
    struct in6_addr inputAddr = {0};
    char rstBuff[INET6_ADDRSTRLEN];

    iret = inet_pton(AF_INET6, "1::", &inputAddr);
    ASSERT_EQ(1, iret);
    ret = inet_ntop(AF_IPX, &inputAddr, rstBuff, sizeof(rstBuff));
    EXPECT_TRUE(ret == NULL);
    ret = inet_ntop(-1, &inputAddr, rstBuff, sizeof(rstBuff));
    EXPECT_TRUE(ret == NULL);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1900
 * @tc.name      : test invalid parameter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsLwipTest, testInvalidParameter, Function | MediumTest | Level3)
{
    int ret;
    int fdFail = -1;
    int fdSuccess = -1;

    fdFail = socket(0, 0, 0);
    EXPECT_EQ(-1, fdFail);
    fdSuccess = socket(AF_INET, SOCK_STREAM, 0);
    EXPECT_NE(-1, fdSuccess);

    ret = ::bind(fdFail, nullptr, sizeof(struct sockaddr_in));
    EXPECT_EQ(-1, ret);
    ret = ::bind(fdSuccess, nullptr, sizeof(struct sockaddr_in));
    EXPECT_EQ(-1, ret);

    ret = connect(fdFail, nullptr, sizeof(struct sockaddr));
    EXPECT_EQ(-1, ret);
    ret = connect(fdSuccess, nullptr, sizeof(struct sockaddr));
    EXPECT_EQ(-1, ret);

    ret = accept(fdFail, nullptr, nullptr);
    EXPECT_EQ(-1, ret);
    ret = accept(fdSuccess, nullptr, nullptr);
    EXPECT_EQ(-1, ret);

    ret = getsockname(fdFail, nullptr, nullptr);
    EXPECT_EQ(-1, ret);
    ret = getsockname(fdSuccess, nullptr, nullptr);
    EXPECT_EQ(-1, ret);

    ret = getpeername(fdFail, nullptr, nullptr);
    EXPECT_EQ(-1, ret);
    ret = getpeername(fdSuccess, nullptr, nullptr);
    EXPECT_EQ(-1, ret);

    ret = send(fdFail, nullptr, strlen(g_srvMsg), 0);
    EXPECT_EQ(-1, ret);
    ret = send(fdSuccess, nullptr, strlen(g_srvMsg), 0);
    EXPECT_EQ(-1, ret);

    ret = sendto(fdFail, nullptr, strlen(g_srvMsg), 0, nullptr, (socklen_t)sizeof(struct sockaddr_in));
    EXPECT_EQ(-1, ret);
    ret = sendto(fdSuccess, nullptr, strlen(g_srvMsg), 0, nullptr, (socklen_t)sizeof(struct sockaddr_in));
    EXPECT_EQ(-1, ret);

    ret = recv(fdFail, nullptr, strlen(g_srvMsg), 0);
    EXPECT_EQ(-1, ret);
    ret = recv(fdSuccess, nullptr, strlen(g_srvMsg), 0);
    EXPECT_EQ(-1, ret);

    struct msghdr msg = {0};
    ret = recvmsg(fdFail, &msg, 0);
    EXPECT_EQ(-1, ret);
    ret = recvmsg(fdSuccess, &msg, 0);
    EXPECT_EQ(-1, ret);

    ret = recvfrom(fdFail, nullptr, strlen(g_srvMsg), 0, nullptr, nullptr);
    EXPECT_EQ(-1, ret);
    ret = recvfrom(fdSuccess, nullptr, strlen(g_srvMsg), 0, nullptr, nullptr);
    EXPECT_EQ(-1, ret);

    ret = setsockopt(fdFail, SOL_SOCKET, SO_RCVTIMEO, nullptr, (socklen_t)sizeof(struct timeval));
    EXPECT_EQ(-1, ret);
    ret = setsockopt(fdSuccess, SOL_SOCKET, SO_RCVTIMEO, nullptr, (socklen_t)sizeof(struct timeval));
    EXPECT_EQ(-1, ret);
    ret = getsockopt(fdFail, SOL_SOCKET, SO_RCVTIMEO, nullptr, (socklen_t*)sizeof(struct timeval));
    EXPECT_EQ(-1, ret);
    ret = getsockopt(fdSuccess, SOL_SOCKET, SO_RCVTIMEO, nullptr, (socklen_t*)sizeof(struct timeval));
    EXPECT_EQ(-1, ret);

    ret = sendmsg(fdFail, nullptr, 0);
    EXPECT_EQ(-1, ret);
    ret = sendmsg(fdSuccess, nullptr, 0);
    EXPECT_EQ(-1, ret);

    ret = listen(fdFail, 0);
    EXPECT_EQ(-1, ret);
    ret = select(fdFail, nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(-1, ret);

    ret = shutdown(fdFail, SHUT_RD);
    EXPECT_EQ(-1, ret);
    ret = shutdown(fdSuccess, -1);
    EXPECT_EQ(-1, ret);

    ret = close(fdSuccess);
    EXPECT_EQ(0, ret);
}
