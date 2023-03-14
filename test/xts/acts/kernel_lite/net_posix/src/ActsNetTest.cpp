/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "securec.h"
#include "gtest/gtest.h"
#include "log.h"
#include "utils.h"
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bits/ioctl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <net/if.h>
#include <netdb.h>
#include <resolv.h>
#include <arpa/nameser.h>
#include <poll.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace testing::ext;

#define PROTOCOL_COUNT 36
#define TEST_FD_COUNT 10
#define STACK_PORT 2288
#define PEER_PORT 2288
#define BUF_SIZE (100)
static const char* g_udpMsg = "Hi, this is UDP";
static const char* g_srvMsg = "Hi, this is TCP server";
static const char* g_cliMsg = "Hello, this is TCP client";
static const char* g_localHost = "127.0.0.1";
static char g_protocolList[PROTOCOL_COUNT][11] = {
    "ip", "icmp", "igmp", "ggp", "ipencap", "st", "tcp",
    "egp", "pup", "udp", "hmp", "xns-idp", "rdp", "iso-tp4", "xtp", "ddp", "idpr-cmtp", "ipv6",
    "ipv6-route", "ipv6-frag", "idrp", "rsvp", "gre", "esp", "ah", "skip", "ipv6-icmp", "ipv6-nonxt",
    "ipv6-opts", "rspf", "vmtp", "ospf", "ipip", "encap", "pim", "raw"
};
static int g_protocolNumber[PROTOCOL_COUNT] = {
    0, 1, 2, 3, 4, 5, 6, 8, 12, 17, 20, 22, 27, 29, 36, 37,
    38, 41, 43, 44, 45, 46, 47, 50, 51, 57, 58, 59, 60, 73, 81, 89, 94, 98, 103, 255
};

static pthread_barrier_t g_barrier;
#define WAIT() pthread_barrier_wait(&g_barrier)

class ActsNetTest : public testing::Test {
protected:
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void) {}
    // TearDownTestCase: Testsuit teardown, run after last testcase
    static void TearDownTestCase(void) {}
    // Testcase setup
    virtual void SetUp() {}
    // Testcase teardown
    virtual void TearDown() {}
};

/* return fd */
static int CommInitTcpServer(int port)
{
    int srvFd = socket(AF_INET, SOCK_STREAM, 0);
    if (srvFd == -1) {
        printf("[###]CommInitTcpServer]socket fail!\n");
        return -1;
    }
    int flag = 1;
    int ret = setsockopt(srvFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    if (ret != 0) {
        printf("[###][CommInitTcpServer]setsockopt fail, ret[%d]!\n", ret);
    }
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(port);
    ret = ::bind(srvFd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    if (ret != 0) {
        printf("[###][CommInitTcpServer]::bind fail!\n");
        close(srvFd);
        return -1;
    }
    ret = listen(srvFd, 5);
    if (ret != 0) {
        printf("[###][CommInitTcpServer]listen fail!\n");
        close(srvFd);
        return -1;
    }
    printf("[###][CommInitTcpServer]success,Fd[%d]\n", srvFd);
    return srvFd;
}

static void* CommTcpClientTask(void *param)
{
    int* ret = (int *)malloc(sizeof(int));
    int memRet = -1;
    if (ret == nullptr) {
        printf("[---][comm client]malloc fail\n");
        return nullptr;
    }
    *ret = 0;
    int clnFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clnFd == -1) {
        printf("[@@@][comm client]socket fail\n");  
        *ret = -1;
        return ret;
    }
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(PEER_PORT + 1);
    int rst = connect(clnFd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    if (rst != 0) {
        printf("[@@@][comm client]connect fail\n");
        close(clnFd);
        *ret = -1;
        return ret;
    }
    int sleepSecond = *((int *)param);
    char dataBuf[50] = {0};
    char sendMsgList[5][50] = {
        "Hi, I'm client,FD:[%d]", "client:123456789abcdefg,FD:[%d]",
        "client:!!@@##$$%%^^&&**(()),FD:[%d]", "client:(((112233445566778899))),FD:[%d]", "bye"
    };
    for (int i = 0; i < 4; i++) {
        sleep(sleepSecond);
        memRet = memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
        EXPECT_EQ(0, memRet);
        rst = sprintf_s(dataBuf, sizeof(dataBuf), sendMsgList[i], clnFd);
        if (rst < 0) {
            printf("[@@@][comm client]sprintf_s error!\n");
            continue;
        }
        rst = send(clnFd, dataBuf, strlen(dataBuf), 0);
        if (rst <= 0) {
            *ret = -1;
            printf("[@@@][comm client][%d]send fail\n", clnFd);
        }
        memRet = memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
        EXPECT_EQ(0, memRet);
        rst = recv(clnFd, dataBuf, sizeof(dataBuf), 0);
        if (rst > 0) {
            printf("[@@@][comm client][%d]recv msg[%s]\n", clnFd, dataBuf);
        } else {
            printf("[@@@][comm client][%d]recv no data\n", clnFd);
        }
    }
    // send 'bye', finish
    rst = send(clnFd, sendMsgList[4], strlen(sendMsgList[4]), 0);
    if (rst <= 0) {
        *ret = -1;
        printf("[@@@][comm client][%d]send fail\n", clnFd);
    }
    rst = close(clnFd);
    if (rst != 0) {
        *ret = -1;
        printf("[@@@][comm client][%d]close fd fail\n", clnFd);
    }
    return ret;
}

static void* SampleTcpServerTask(void *p)
{
    int memRet = -1;
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
    memRet = memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
    ret = strcpy_s(buf, BUF_SIZE, g_srvMsg);
    EXPECT_EQ(0, ret);
    ret = send(clnFd, buf, srvMsgLen, 0);
    printf("[tcp server]send, ret=%d\n", ret);
    EXPECT_EQ(srvMsgLen, (unsigned int)ret);

    memRet =memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
    ret = recv(clnFd, buf, sizeof(buf), 0);
    printf("[tcp server]recv, ret=%d, msg[%s]\n", ret, buf);
    EXPECT_EQ(strlen(g_cliMsg), (unsigned int)ret);

    WAIT();
    int len = 2;
    struct msghdr msg = {nullptr};
    struct iovec iov[2] = {nullptr};
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(g_localHost);
    clnAddr.sin_port = htons(PEER_PORT);
    memRet = memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
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
    static char bufrec[BUF_SIZE + 1] = {0};
    memRet = memset_s(bufrec, sizeof(bufrec), 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
    memRet = memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    EXPECT_EQ(0, memRet);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    iov[0].iov_base = bufrec;
    iov[0].iov_len = sizeof(bufrec);
    ret = recvmsg(clnFd, &msg, 0);
    if (len * strlen(g_cliMsg) == (unsigned int)ret && ret >= 0) {
        bufrec[ret] = 0;
        printf("[tcp server]recvmsg on socket %d:%d, msg[%s]\n", clnFd, ret, buf);
    } else {
        EXPECT_TRUE(false);
        printf("[tcp server] recvmsg on socket %d： %d\n", clnFd, ret);
    }

    ret = shutdown(clnFd, SHUT_RDWR);
    printf("[tcp server]shutdown\n");
    EXPECT_EQ(0, ret);
    ret = close(clnFd);
    EXPECT_NE(-1, ret);
    ret = close(srvFd);
    EXPECT_NE(-1, ret);
    return nullptr;
}

static void* SampleTcpClientTask(void *p)
{
    int memRet = -1;
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
    memRet = memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
    ret = strcpy_s(buf, BUF_SIZE, g_cliMsg);
    EXPECT_EQ(0, ret);
    ret = send(clnFd, buf, cliMsgLen, 0);
    printf("[tcp client]send, ret=%d\n", ret);
    EXPECT_EQ(cliMsgLen, (unsigned int)ret);

    memRet = memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
    ret = recv(clnFd, buf, sizeof(buf), 0);
    printf("[tcp client]recv, ret=%d, msg[%s]\n", ret, buf);
    EXPECT_EQ(strlen(g_srvMsg), (unsigned int)ret);

    WAIT();
    int len = 2;
    struct msghdr msg = {nullptr};
    struct iovec iov[2] = {nullptr};
    struct sockaddr_in clnAddr = {0};
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(g_localHost);
    clnAddr.sin_port = htons(PEER_PORT);
    memRet = memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
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
    static char bufrec[BUF_SIZE + 1] = {0};
    memRet = memset_s(bufrec, sizeof(bufrec), 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
    memRet = memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    EXPECT_EQ(0, memRet);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    iov[0].iov_base = bufrec;
    iov[0].iov_len = sizeof(bufrec);
    ret = recvmsg(clnFd, &msg, 0);
    if (len * strlen(g_srvMsg) == (unsigned int)ret && ret >= 0) {
        bufrec[ret] = 0;
        printf("[tcp client]recvmsg, ret=%d, msg[%s]\n", ret, bufrec);
    } else {
        EXPECT_TRUE(false);
        printf("[tcp clien]recvmsg, ret=%d\n", ret);
    }

    ret = shutdown(clnFd, SHUT_RDWR);
    printf("[tcp client]shutdown\n");
    EXPECT_EQ(0, ret);
    ret = close(clnFd);
    EXPECT_NE(-1, ret);
    return nullptr;
}

static void *TcpServerLoopTask(void *p)
{
    int srvFd = CommInitTcpServer(STACK_PORT);
    EXPECT_NE(-1, srvFd);
    if (srvFd == -1)
    {
        LOG("CommInitTcpServer errno = %d\n", errno);
        ADD_FAILURE();
    }
    int i = 0;
    int clientFds[36];
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);
    while (i < 30)
    {
        clientFds[i] = accept(srvFd, (struct sockaddr *)&clnAddr, &clnAddrLen);
        printf("[***---][tcp server loop]accept <%s:%d>, fd[%d]i[%d]\n", inet_ntoa(clnAddr.sin_addr),
               ntohs(clnAddr.sin_port), clientFds[i], i);
        EXPECT_NE(-1, clientFds[i]);
        i++;
    }

    i--;
    int ret;
    while (i >= 0) {
        ret = shutdown(clientFds[i], SHUT_RDWR);
        EXPECT_EQ(0, ret);
        ret = close(clientFds[i]);
        EXPECT_NE(-1, ret) << "[***---][tcp server loop] close fd index[" << i << "]";
        i--;
    }
    ret = close(srvFd);
    EXPECT_NE(-1, ret);
    return nullptr;
}

static void* TcpClientOnlyConnectTask(void *p)
{
    int tId = *((int *)p);
    struct sockaddr addr = {0};
    socklen_t addrLen = sizeof(addr);
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(PEER_PORT);

    int ret;
    int index;
    int clientFds[10];
    for (index = 0; index < 10; index++) {
        clientFds[index] = socket(AF_INET, SOCK_STREAM, 0);
        printf("[------][tcp client[%d]]create socket fd[%d],index[%d]\n", tId, clientFds[index], index);
        EXPECT_NE(-1, clientFds[index]);
        ret = connect(clientFds[index], (struct sockaddr*)&srvAddr, sizeof(srvAddr));
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            break;
        }

        ret = getsockname(clientFds[index], &addr, &addrLen);
        printf("[------][tcp client[%d]]getsockname %s:%d\n", tId, inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr),
            ntohs(((struct sockaddr_in*)&addr)->sin_port));
        EXPECT_EQ(0, ret);
        EXPECT_EQ(inet_addr(g_localHost), ((struct sockaddr_in*)&addr)->sin_addr.s_addr);
        ret = getpeername(clientFds[index], &addr, &addrLen);
        EXPECT_EQ(0, ret);
        EXPECT_EQ(inet_addr(g_localHost), ((struct sockaddr_in*)&addr)->sin_addr.s_addr);
        sleep(1);
    }

    index--;
    for (int i = index; i >= 0; i--) {
        ret = shutdown(clientFds[i], SHUT_RDWR);
        EXPECT_EQ(0, ret);
        ret = close(clientFds[i]);
        EXPECT_NE(-1, ret) << "[------][tcp client] close fd index[" << i << "]";
    }
    return nullptr;
}

static int TcpServerForFork()
{
    int memRet = -1;
    int timeout = 10;
    char dataBuf[50] = {0};
    int srvFd = CommInitTcpServer(STACK_PORT + 1);
    if (srvFd == -1) {
        return -1;
    }
    printf("[---][server process]wait client...\n");
    int ret = -1;
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);
    int cliFd = accept(srvFd, (struct sockaddr*)&clnAddr, &clnAddrLen);
    if (cliFd == -1) {
        printf("[---][server process]accept fail!\n");
        goto FINALLY;
    }
    while (timeout > 0) {
        memRet = memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
        EXPECT_EQ(0, memRet);
        ret = recv(cliFd, dataBuf, sizeof(dataBuf), 0);
        if (ret > 0) {
            printf("[---][server process]recv msg[%s]\n", dataBuf);
            if (strcmp(dataBuf, "bye") == 0) {
                printf("[---][server process]break\n");
                break;
            }
        } else {
            printf("[---][server process]recv no data\n");
        }
        memRet = memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
        EXPECT_EQ(0, memRet);
        sprintf_s(dataBuf, sizeof(dataBuf), "server timeout:%d", timeout);
        ret = send(cliFd, dataBuf, strlen(dataBuf), 0);
        if (ret > 0) {
            printf("[---][server process]send success, msg[%s]\n", dataBuf);
        } else {
            printf("[---][server process]send fail!\n");
        }
        timeout--;
    }
    close(cliFd);
    printf("[---][server process]over\n");

FINALLY:
    close(srvFd);
    return ret;
}

static int SelectServerForFork(unsigned int timeoutSec)
{
    int memRet = -1;
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
    struct timeval timev = {.tv_sec = timeoutSec, .tv_usec = 0};
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
                printf("[&&&][select process]fd info[%d:%d]\n", i, fds[i]);
            }
        }
        ret = select(maxFd + 1, &readSet, nullptr, nullptr, &timev);
        if (ret == 0) {
            printf("[---][select process]select timeout!\n");
            ret = -2;
            break;
        }
        if (ret == -1) {
            fprintf(stderr, "[---][select process]select fail[%s]\n", strerror(errno));
            break;
        }
        if (FD_ISSET(srvFd, &readSet)) {
            int cliFd = accept(srvFd, (struct sockaddr*)&clnAddr, &clnAddrLen);
            if (cliFd == -1) {
                printf("[---][select process]accept fail!\n");
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
                printf("[***][select process]accept success, cliCount[%d]\n", cliCount);
            }
            continue;
        }
        for (int i = 0; i < TEST_FD_COUNT; i++) {
            if (fds[i] == -1) {
                continue;
            }
            if (FD_ISSET(fds[i], &readSet)) {
                memRet = memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
                EXPECT_EQ(0, memRet);
                int len = recv(fds[i], dataBuf, sizeof(dataBuf), 0);
                if (len > 0) {
                    printf("[***][select process][%d]recv msg[%s]\n", fds[i], dataBuf);
                    if (strcmp(dataBuf, "bye") == 0) {
                        printf("[###][select process][%d]client bye,cliCount[%d]\n", fds[i], cliCount);
                        FD_CLR(fds[i], &readSet);
                        close(fds[i]);
                        fds[i] = -1;
                        cliCount--;
                        continue;
                    }
                    len = send(fds[i], dataBuf, strlen(dataBuf), 0);
                    if (len > 0) {
                        printf("[***][select process][%d]send success\n", fds[i]);
                    } else {
                        printf("[---][select process][%d]send fail\n", fds[i]);
                    }
                }
            }
        }
        if (cliCount == 0) {
            ret = 0;
            printf("[###][select process]cliCount=0, over!\n");
            break;
        }
    }
    close(srvFd);
    return ret;
}

static int PollServerForFork(int timeoutSec)
{
    int srvFd = CommInitTcpServer(STACK_PORT + 1);
    if (srvFd == -1) {
        return -1;
    }
    int ret;
    int cliCount = 0;
    int fdCount = 1;
    char dataBuf[50] = {0};
    struct pollfd fds[TEST_FD_COUNT];
    int memRet = -1;
    fds[0].events = POLLIN | POLLPRI;
    fds[0].fd = srvFd;
    for (int i = 1; i < TEST_FD_COUNT; i++) {
        fds[i].fd = -1;
    }
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);
    while (1) {
        ret = poll(fds, fdCount, timeoutSec);
        if (ret == 0) {
            printf("[---][poll process]poll timeout!\n");
            ret = -2;
            break;
        }
        if (ret == -1) {
            fprintf(stderr, "[---][poll process]poll fail[%s]\n", strerror(errno));
            break;
        }
        if (fds[0].revents & POLLIN) {
            int cliFd = accept(srvFd, (struct sockaddr*)&clnAddr, &clnAddrLen);
            if (cliFd == -1) {
                printf("[---][poll process]accept fail!\n");
                ret = -1;
                break;
            } else {
                for (int i = 1; i < TEST_FD_COUNT; i++) {
                    if (fds[i].fd == -1) {
                        fds[i].fd = cliFd;
                        fds[i].events = POLLIN | POLLPRI;
                        break;
                    }
                }
                cliCount++;
                fdCount++;
                printf("[***][poll process]accept success, cliCount[%d]\n", cliCount);
            }
            continue;
        }
        for (int i = 0; i < TEST_FD_COUNT; i++) {
            if (fds[i].fd == -1) {
                continue;
            }
            if (fds[i].revents & POLLIN) {
                memRet = memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
                EXPECT_EQ(0, memRet);
                int len = recv(fds[i].fd, dataBuf, sizeof(dataBuf), 0);
                if (len > 0) {
                    printf("[***][poll process][%d]recv msg[%s]\n", fds[i].fd, dataBuf);
                    if (strcmp(dataBuf, "bye") == 0) {
                        printf("[###][select process][%d]client bye,cliCount[%d]\n", fds[i].fd, cliCount);
                        cliCount--;
                        continue;
                    }
                    len = send(fds[i].fd, dataBuf, strlen(dataBuf), 0);
                    if (len > 0) {
                        printf("[***][poll process][%d]send success\n", fds[i].fd);
                    } else {
                        printf("[---][poll process][%d]send fail\n", fds[i].fd);
                    }
                }
            }
        }
        if (cliCount == 0) {
            ret = 0;
            printf("[###][poll process]cliCount=0, over!\n");
            break;
        }
    }
    for (int i = 0; i < TEST_FD_COUNT; i++) {
        if (fds[i].fd != -1) {
            close(fds[i].fd);
        }
    }
    close(srvFd);
    return ret;
}

/**
 * @tc.number    : SUB_KERNEL_NET_0100
 * @tc.name      : sample test tcp
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testTcpSample, Function | MediumTest | Level2)
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
 * @tc.number    : SUB_KERNEL_NET_0200
 * @tc.name      : test tcp concurrent connect
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testTcpConcurrentConnect, Function | MediumTest | Level2)
{
    pthread_t srv;
    int ret = pthread_create(&srv, nullptr, TcpServerLoopTask, nullptr);
    EXPECT_EQ(0, ret);

    sleep(2);
    pthread_t cliOne;
    int tIdOne = 1;
    ret = pthread_create(&cliOne, nullptr, TcpClientOnlyConnectTask, &tIdOne);
    EXPECT_EQ(0, ret);
    pthread_t cliTwo;
    int tIdTwo = 2;
    ret = pthread_create(&cliTwo, nullptr, TcpClientOnlyConnectTask, &tIdTwo);
    EXPECT_EQ(0, ret);
    pthread_t cliThree;
    int tIdThree = 3;
    ret = pthread_create(&cliThree, nullptr, TcpClientOnlyConnectTask, &tIdThree);
    EXPECT_EQ(0, ret);

    ret = pthread_join(srv, nullptr);
    EXPECT_EQ(0, ret);
    ret = pthread_join(cliOne, nullptr);
    EXPECT_EQ(0, ret);
    printf("[******]testTcpConnect thread[1] finish!\n");
    ret = pthread_join(cliTwo, nullptr);
    EXPECT_EQ(0, ret);
    printf("[******]testTcpConnect thread[2] finish!\n");
    ret = pthread_join(cliThree, nullptr);
    EXPECT_EQ(0, ret);
    printf("[******]testTcpConnect thread[3] finish!\n");
}

/**
 * @tc.number    : SUB_KERNEL_NET_0300
 * @tc.name      : sample test upd
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testUdpSample, Function | MediumTest | Level2)
{
    int len = 2;
    char buf[BUF_SIZE + 1];
    int memRet = -1;
    struct msghdr msg = {nullptr};
    struct iovec iov[2] = {nullptr};
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
    memRet = memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
    ret = strcpy_s(buf, BUF_SIZE, g_udpMsg);
    EXPECT_EQ(0, ret);
    ret = sendto(sfd, buf, strlen(g_udpMsg), 0, (struct sockaddr*)&clnAddr, (socklen_t)sizeof(clnAddr));
    EXPECT_NE(-1, ret);

    memRet = memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
    ret = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr*)&clnAddr, &clnAddrLen);
    EXPECT_EQ(strlen(g_udpMsg), (unsigned int)ret);

    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(g_localHost);
    clnAddr.sin_port = htons(PEER_PORT);
    memRet = memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    EXPECT_EQ(0, memRet);
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
 * @tc.number    : SUB_KERNEL_NET_0400
 * @tc.name      : test inter-process network communication
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInterProcessComm, Function | MediumTest | Level2)
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
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_0500
 * @tc.name      : test select timeout
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testSelectTimeout, Function | MediumTest | Level2)
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
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_0510
 * @tc.name      : test select with one client
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testSelectOneClient, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) {
        // child: server
        int exitCode = 0;
        int ret = SelectServerForFork(5);
        if (ret != 0) {
            printf("[###][select server]run fail\n");
            exitCode = 1;
        }
        printf("[###][select server]ret[%d]\n", ret);
        exit(exitCode);
    } else {
        // parent: client
        sleep(2);
        int second = 1;
        void* ret = CommTcpClientTask(&second);
        if (ret == nullptr) {
            printf("[###][test select]CommClientTask ret is null\n");
            EXPECT_TRUE(0);
        } else {
            int rst = *((int *)ret);
            printf("[###][test select]CommClientTask ret [%d]\n", rst);
            EXPECT_EQ(0, rst);
            free(ret);
        }
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_0520
 * @tc.name      : test select with multi clients
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testSelectMultiClients, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) {
        // child: server
        int exitCode = 0;
        int ret = SelectServerForFork(5);
        if (ret != 0) {
            printf("[###][select server]run fail\n");
            exitCode = 1;
        }
        printf("[###][select server]ret[%d]\n", ret);
        exit(exitCode);
    } else {
        // parent: client
        sleep(2);
        int ret;
        pthread_t pCli[4];
        int pEroId[4] = {0};  
        int interval[4] = {2, 1, 3, 1};
        for (int i = 0; i < 4; i++) {
            ret = pthread_create(&pCli[i], nullptr, CommTcpClientTask, &interval[i]);
            if (ret == -1) {
                pEroId[i] = -1;
            } else {
            EXPECT_EQ(0, ret) << "Errinfo:pthread_create index is [" << i << "]";
            }
        }

        void* rst = nullptr;
        for (int i = 0; i < 4; i++)
        {
            if (pEroId[i] == 0)
            {
                ret = pthread_join(pCli[i], &rst);
                EXPECT_EQ(0, ret) << "Errinfo:pthread_join index is [" << i << "]";
                if (rst != nullptr)
                {
                    printf("[###][test select]client thread[%d] return[%d]\n", i, *((int *)rst));
                    EXPECT_EQ(0, *((int *)rst));
                    free(rst);
                }
            }
        }
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_0600
 * @tc.name      : test poll timeout
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testPollTimeout, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) {
        // child: server
        int ret = PollServerForFork(2000);
        if (ret == -2) {
            exit(0);
        } else {
            printf("[###][poll server]fail, ret is[%d]\n", ret);
            exit(1);
        }
    } else {
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_0610
 * @tc.name      : test poll with one client
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testPollOneClient, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) {
        // child: server
        int exitCode = 0;
        int ret = PollServerForFork(5000);
        if (ret != 0) {
            printf("[###][poll server]run fail\n");
            exitCode = 1;
        }
        printf("[###][poll server]ret[%d]\n", ret);
        exit(exitCode);
    } else {
        // parent: client
        sleep(2);
        int second = 1;
        void* ret = CommTcpClientTask(&second);
        if (ret == nullptr) {
            printf("[###][test poll]CommClientTask ret is null\n");
            EXPECT_TRUE(0);
        } else {
            int rst = *((int *)ret);
            printf("[###][test poll]CommClientTask ret [%d]\n", rst);
            EXPECT_EQ(0, rst);
            free(ret);
        }
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_0620
 * @tc.name      : test poll with multi clients
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testPollMultiClients, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) {
        // child: server
        int exitCode = 0;
        int ret = PollServerForFork(5000);
        if (ret != 0) {
            printf("[###][poll server]run fail\n");
            exitCode = 1;
        }
        printf("[###][poll server]ret[%d]\n", ret);
        exit(exitCode);
    } else {
        // parent: client
        sleep(2);
        int ret;
        pthread_t pCli[6];
        int  pEroId[6] = {0};             
        int interval[6] = {2, 1, 3, 1, 2, 1};
        for (int i = 0; i < 6; i++)
        {
            ret = pthread_create(&pCli[i], nullptr, CommTcpClientTask, &interval[i]);
            if (ret == -1)
            {
                pEroId[i] = -1;
            }
            else
            {
                EXPECT_EQ(0, ret) << "Errinfo:pthread_create index is [" << i << "]";
            }
        }

        void* rst = nullptr;
        for (int i = 0; i < 6; i++)
        {
            if (pEroId[i] == 0)
            {
                ret = pthread_join(pCli[i], &rst);
                EXPECT_EQ(0, ret) << "Errinfo:pthread_join index is [" << i << "]";
                if (rst != nullptr)
                {
                    printf("[###][test poll]client thread[%d] return[%d]\n", i, *((int *)rst));
                    EXPECT_EQ(0, *((int *)rst));
                    free(rst);
                }
            }
        }
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_0700
 * @tc.name      : test ioctl SIOCGIFCONF
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testIoctlIfconf, Function | MediumTest | Level2)
{
    int memRet = -1;
    int udpFd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(-1, udpFd);

    struct ifreq ifr[5];
    struct ifconf ifc = {0};
    memRet = memset_s(&ifc, sizeof(struct ifconf), 0, sizeof(struct ifconf));
    EXPECT_EQ(0, memRet);
    ifc.ifc_len = 5 * sizeof(struct ifreq);
    ifc.ifc_buf = (char *)ifr;
    int ret = ioctl(udpFd, SIOCGIFCONF, (char *)&ifc);
    ASSERT_EQ(0, ret) << "ioctl fail[SIOCGIFCONF], errinfo[" << strerror(errno) << "]";
    int ifrCount = ifc.ifc_len / sizeof(struct ifreq);
    EXPECT_TRUE(ifrCount >= 2);
    for (int i = 0; i < ifrCount; i++) {
        printf("[###]interface name is %s\n", ifr[i].ifr_name);
    }
    ret = close(udpFd);
    EXPECT_EQ(0, ret);
}

/**
 * @tc.number    : SUB_KERNEL_NET_0710
 * @tc.name      : test ioctl get set delete IFADDR,FIONREAD, localhost
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testIoctlIfnetAddrLocal, Function | MediumTest | Level2)
{
    int udpFd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(-1, udpFd);

    // get ifnet address
    struct ifreq ifr = {0};
    const char *deviceLo = "lo";
    int ret = strcpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), deviceLo);
    EXPECT_EQ(0, ret) << "strcpy_s fail!";
    ret = ioctl(udpFd, SIOCGIFADDR, &ifr);
    ASSERT_EQ(0, ret) << "ioctl fail[SIOCGIFADDR], errinfo[" << strerror(errno) << "]";
    struct sockaddr_in *srvAddr = (struct sockaddr_in *)&ifr.ifr_addr;
    EXPECT_STREQ(g_localHost, inet_ntoa(srvAddr->sin_addr));
    srvAddr->sin_family = AF_INET;
    srvAddr->sin_port = htons(STACK_PORT);

    // get read buffer bytes
    ret = ::bind(udpFd, (struct sockaddr*)srvAddr, sizeof(sockaddr_in));
    struct sockaddr_in clnAddr = {0};
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_port = htons(PEER_PORT);
    clnAddr.sin_addr.s_addr = inet_addr(g_localHost);
    const char *msg = "Hi, test ioctl...";
    unsigned int msgLen = strlen(msg);
    ret = sendto(udpFd, msg, msgLen, 0, (struct sockaddr*)&clnAddr, (socklen_t)sizeof(clnAddr));
    EXPECT_NE(-1, ret);
    EXPECT_EQ(msgLen, (unsigned int)ret);
    int bytes;
    ret = ioctl(udpFd, FIONREAD, &bytes);
    EXPECT_EQ(0, ret) << "ioctl fail[FIONREAD], errinfo[" << strerror(errno) << "]";
    EXPECT_EQ(msgLen, (unsigned int)bytes);

    // delete lo address, will fail
    ret = ioctl(udpFd, SIOCDIFADDR, &ifr);
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ("Operation not permitted", strerror(errno));
    // set lo address, will fail
    srvAddr->sin_addr.s_addr = inet_addr(g_localHost);
    ret = ioctl(udpFd, SIOCSIFADDR, &ifr);
    EXPECT_STREQ("Operation not permitted", strerror(errno));

    ret = close(udpFd);
    EXPECT_EQ(0, ret);
}

/**
 * @tc.number    : SUB_KERNEL_NET_0720
 * @tc.name      : test ioctl get and set IFADDR others
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testIoctlIfnetAddrOthers, Function | MediumTest | Level2)
{
    int udpFd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(-1, udpFd);
    int memRet = -1;

    struct ifreq ifr[5];
    struct ifconf ifc = {0};
    memRet = memset_s(&ifc, sizeof(struct ifconf), 0, sizeof(struct ifconf));
    EXPECT_EQ(0, memRet);
    ifc.ifc_len = 5 * sizeof(struct ifreq);
    ifc.ifc_buf = (char *)ifr;
    int ret = ioctl(udpFd, SIOCGIFCONF, (char *)&ifc);
    ASSERT_EQ(0, ret) << "ioctl fail[SIOCGIFCONF], errinfo[" << strerror(errno) << "]";

    struct ifreq ifrTmp = {0};
    struct sockaddr_in *addr1 = nullptr;
    struct sockaddr_in *addr2 = nullptr;
    int ifrCount = ifc.ifc_len / sizeof(struct ifreq);
    EXPECT_TRUE(ifrCount >= 2);
    for (int i = 0; i < ifrCount; i++) {
        addr1 = (struct sockaddr_in *)&ifr[i].ifr_addr;
        if (strcmp("lo", ifr[i].ifr_name) != 0) {
            // get inet addr
            memRet = memset_s(&ifrTmp, sizeof(struct ifreq), 0, sizeof(struct ifreq));
            EXPECT_EQ(0, memRet);
            ret = strcpy_s(ifrTmp.ifr_name, sizeof(ifrTmp.ifr_name), ifr[i].ifr_name);
            ret = ioctl(udpFd, SIOCGIFADDR, &ifrTmp);
            ASSERT_EQ(0, ret) << "ioctl fail[SIOCGIFADDR], errinfo[" << strerror(errno) << "]";
            addr2 = (struct sockaddr_in *)&ifrTmp.ifr_addr;
            EXPECT_EQ(addr1->sin_addr.s_addr, addr2->sin_addr.s_addr);
            printf("[###]get %s\n", ifr[i].ifr_name);

            // set inet addr
            addr2->sin_addr.s_addr = addr1->sin_addr.s_addr;
            ret = ioctl(udpFd, SIOCSIFADDR, &ifrTmp);
            if (strcmp("0.0.0.0", inet_ntoa(addr1->sin_addr)) == 0) {
                // can't set 0.0.0.0
                EXPECT_NE(0, ret) << "ioctl[0.0.0.0] fail[SIOCSIFADDR], errinfo[" << strerror(errno) << "]";
            } else {
                EXPECT_EQ(0, ret) << "ioctl fail[SIOCSIFADDR], errinfo[" << strerror(errno) << "]";
            }
            // get again
            ret = ioctl(udpFd, SIOCGIFADDR, &ifrTmp);
            EXPECT_EQ(0, ret) << "ioctl fail[SIOCGIFADDR], errinfo[" << strerror(errno) << "]";
            EXPECT_EQ(addr1->sin_addr.s_addr, addr2->sin_addr.s_addr);
        }
    }
    ret = close(udpFd);
    EXPECT_EQ(0, ret);
}

/**
 * @tc.number    : SUB_KERNEL_NET_0730
 * @tc.name      : test ioctl get and set IFHWADDR
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testIoctlIfhwAddr, Function | MediumTest | Level2)
{
    int udpFd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(-1, udpFd);
    int memRet = -1;

    struct ifreq ifr[5];
    struct ifconf ifc = {0};
    memRet = memset_s(&ifc, sizeof(struct ifconf), 0, sizeof(struct ifconf));
    EXPECT_EQ(0, memRet);
    ifc.ifc_len = 5 * sizeof(struct ifreq);
    ifc.ifc_buf = (char *)ifr;
    int ret = ioctl(udpFd, SIOCGIFCONF, (char *)&ifc);
    ASSERT_EQ(0, ret) << "ioctl fail[SIOCGIFCONF], errinfo[" << strerror(errno) << "]";

    char rst1[18];

    char* macPtr = nullptr;
    struct ifreq ifrTmp = {0};
    struct sockaddr_in *addr = nullptr;
    int ifrCount = ifc.ifc_len / sizeof(struct ifreq);
    EXPECT_TRUE(ifrCount >= 2);
    for (int i = 0; i < ifrCount; i++) {
        addr = (struct sockaddr_in *)&ifr[i].ifr_addr;
        if (strcmp("lo", ifr[i].ifr_name) != 0) {
            // get ifhwaddr
            memRet = memset_s(&ifrTmp, sizeof(struct ifreq), 0, sizeof(struct ifreq));
            EXPECT_EQ(0, memRet);
            ret = strcpy_s(ifrTmp.ifr_name, sizeof(ifrTmp.ifr_name), ifr[i].ifr_name);
            EXPECT_EQ(0, ret) << "strcpy_s error";
            ret = ioctl(udpFd, SIOCGIFHWADDR, &ifrTmp);
            ASSERT_EQ(0, ret) << "ioctl fail[SIOCGIFHWADDR], errinfo[" << strerror(errno) << "]";
            macPtr = ifrTmp.ifr_hwaddr.sa_data;
            ret = sprintf_s(rst1, sizeof(rst1), "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", *macPtr, *(macPtr+1),
                *(macPtr+2), *(macPtr+3), *(macPtr+4), *(macPtr+5));
            EXPECT_EQ(strlen(rst1), (unsigned int)ret) << "sprintf_s error";
            printf("[###]get %s \n", ifrTmp.ifr_name);
        }
    }
    ret = close(udpFd);
    EXPECT_EQ(0, ret);
}

/**
 * @tc.number    : SUB_KERNEL_NET_1000
 * @tc.name      : test socket operation
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testSocketOpt, Function | MediumTest | Level2)
{
    socklen_t len;
    int memRet = -1;
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

    memRet = memset_s(&timeout, len, 0, len);
    EXPECT_EQ(0, memRet);
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
 * @tc.number    : SUB_KERNEL_NET_1100
 * @tc.name      : test getsockname and getpeername invalid input
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testGetSocketNameInvalidInput, Function | MediumTest | Level3)
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
 * @tc.number    : SUB_KERNEL_NET_1200
 * @tc.name      : test domain name compress and expand normally
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testDnCompExpandNormal, Function | MediumTest | Level2)
{
    const char* domain1 = "www.testohos.com";
    unsigned char buf[MAXDNAME] = {0};
    unsigned char* bufPtr = buf;
    int ret = dn_comp(domain1, bufPtr, MAXDNAME, nullptr, nullptr);
    EXPECT_NE(-1, ret) << "dn_comp fail, buf is '" << buf << "'";
    int len = ret + 4;

    char expandDn[MAXDNAME];
    unsigned char* eomorig = buf + MAXDNAME;
    ret = dn_expand(buf, eomorig, buf, expandDn, MAXDNAME);
    EXPECT_NE(-1, ret);
    EXPECT_STREQ(domain1, expandDn);
    printf("dn_expand ret=%d, expandDn is '%s'\n", ret, expandDn);

    const char* domain2 = "www.baidu.com";
    unsigned char* dnptrs[5] = {bufPtr, nullptr};
    unsigned char** lastdnptr = dnptrs + 5;
    ret = dn_comp(domain2, bufPtr + len, MAXDNAME - len, dnptrs, lastdnptr);
    EXPECT_NE(-1, ret) << "dn_comp(with dnptrs) fail, buf+len is '" << bufPtr + len << "'";

    char expandDnWithDnptrs[MAXDNAME];
    unsigned char* eomorigWithDnptrs = buf + MAXDNAME;
    ret = dn_expand(dnptrs[1], eomorigWithDnptrs, dnptrs[1], expandDnWithDnptrs, MAXDNAME);
    EXPECT_NE(-1, ret);
    EXPECT_STREQ(domain2, expandDnWithDnptrs);
}

/**
 * @tc.number    : SUB_KERNEL_NET_1300
 * @tc.name      : test domain name compress and expand abnormally
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testDnCompExpandAbnormal, Function | MediumTest | Level3)
{
    const char* src = "!#%^.)(-+{}\r\n";
    unsigned char destComp[MAXDNAME];
    int ret = dn_comp(src, destComp, 10, nullptr, nullptr);
    EXPECT_EQ(-1, ret);
    printf("dn_comp ret=%d, compDn is '%s'\n", ret, destComp);

    ret = dn_comp("www.baidu.com", destComp, MAXDNAME, nullptr, nullptr);
    EXPECT_NE(-1, ret);

    // dest array too small
    char destExpand[5];
    unsigned char *eomorig = destComp + 10;
    ret = dn_expand(destComp, eomorig, destComp, destExpand, MAXDNAME);
    EXPECT_EQ(-1, ret);
    printf("dn_expand ret=%d, expandDn is '%s'\n", ret, destExpand);

    // compress info is 0
    unsigned char destCompError[20] = {0};
    eomorig = destCompError + 20;
    ret = dn_expand(destCompError, eomorig, destCompError, destExpand, MAXDNAME);
    EXPECT_EQ(1, ret);
    printf("dn_expand ret=%d, expandDn is '%s'\n", ret, destExpand);
}

/**
 * @tc.number    : SUB_KERNEL_NET_1400
 * @tc.name      : test convert 48 bit Ethernet host address normally
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testEtherConvertNormal, Function | MediumTest | Level2)
{
    char atonByteRst[24];
    char* ntoaRst = nullptr;
    struct ether_addr *atonRst = nullptr;
    char addrHex[3][18] = {"FF:FF:FF:FF:FF:FF", "00:00:00:00:00:00", "0A:1B:2C:3D:4E:5F"};
    char addrByte[3][24] = {"255.255.255.255.255.255", "0.0.0.0.0.0", "10.27.44.61.78.95"};
    for (int i = 0; i < 3; i++) {
        atonRst = nullptr;
        atonRst = ether_aton(addrHex[i]);
        ASSERT_TRUE(atonRst != nullptr);
        int ret = sprintf_s(atonByteRst, sizeof(atonByteRst), "%u.%u.%u.%u.%u.%u",
            atonRst->ether_addr_octet[0], atonRst->ether_addr_octet[1],
            atonRst->ether_addr_octet[2], atonRst->ether_addr_octet[3],
            atonRst->ether_addr_octet[4], atonRst->ether_addr_octet[5]);
        EXPECT_STREQ(addrByte[i], atonByteRst) << "ErrInfo:result '" << atonByteRst << "',ret=" << ret;

        ntoaRst = nullptr;
        ntoaRst = ether_ntoa(atonRst);
        EXPECT_TRUE(ntoaRst != nullptr);
        EXPECT_STREQ(addrHex[i], ntoaRst) << "ErrInfo:result '" << ntoaRst << "'";
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_1500
 * @tc.name      : test convert 48 bit Ethernet host address abnormally
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testEtherConvertAbnormal, Function | MediumTest | Level3)
{
    char* addrError = (char *)":::F:Z:";
    struct ether_addr *atonRst = nullptr;
    atonRst = ether_aton(addrError);
    EXPECT_TRUE(atonRst == nullptr);
}

/**
 * @tc.number    : SUB_KERNEL_NET_1600
 * @tc.name      : test thread safe convert 48 bit Ethernet host address normally
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testEtherConvertNormalWithThreadSafe, Function | MediumTest | Level2)
{
    int ret;
    char ntoaDataRst[18];
    char* ntoaPointRst = nullptr;
    struct ether_addr *atonPointRst = nullptr;
    struct ether_addr *atonDataRst = (ether_addr *)malloc(sizeof(ether_addr));
    ASSERT_TRUE(atonDataRst != nullptr);
    char addrHex[3][18] = {"FF:FF:FF:FF:FF:FF", "00:00:00:00:00:00", "5F:4E:2C:3D:1B:0A"};
    char addrByte[3][24] = {"255.255.255.255.255.255", "0.0.0.0.0.0", "95.78.44.61.27.10"};
    for (int i = 0; i < 3; i++) {
        atonPointRst = nullptr;
        atonPointRst = ether_aton_r(addrHex[i], atonDataRst);
        ASSERT_TRUE(atonPointRst != nullptr);
        char byteRst[24];
        ret = sprintf_s(byteRst, sizeof(byteRst), "%u.%u.%u.%u.%u.%u",
            atonDataRst->ether_addr_octet[0], atonDataRst->ether_addr_octet[1],
            atonDataRst->ether_addr_octet[2], atonDataRst->ether_addr_octet[3],
            atonDataRst->ether_addr_octet[4], atonDataRst->ether_addr_octet[5]);
        EXPECT_STREQ(addrByte[i], byteRst) << "ErrInfo:ether_aton_r rst[" << byteRst << "], ret[" << ret << "]";
        ret = sprintf_s(byteRst, sizeof(byteRst), "%u.%u.%u.%u.%u.%u",
            atonPointRst->ether_addr_octet[0], atonPointRst->ether_addr_octet[1],
            atonPointRst->ether_addr_octet[2], atonPointRst->ether_addr_octet[3],
            atonPointRst->ether_addr_octet[4], atonPointRst->ether_addr_octet[5]);
        EXPECT_STREQ(addrByte[i], byteRst) << "ErrInfo:ether_aton_r rst[" << byteRst << "], ret[" << ret << "]";

        ntoaPointRst = nullptr;
        ntoaPointRst = ether_ntoa_r(atonDataRst, ntoaDataRst);
        EXPECT_STREQ(addrHex[i], ntoaDataRst);
        EXPECT_STREQ(addrHex[i], ntoaPointRst);
        printf("ether_ntoa_r result '%s', '%s'\n", ntoaDataRst, ntoaPointRst);
    }

    free(atonDataRst);
}

/**
 * @tc.number    : SUB_KERNEL_NET_1700
 * @tc.name      : test get protocals by normal name and number
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testGetProtoByNormal, Function | MediumTest | Level2)
{
    int number;
    struct protoent *protoPtr = nullptr;
    for (int i = 0; i < PROTOCOL_COUNT; i++) {
        protoPtr = getprotobyname(g_protocolList[i]);
        if (protoPtr == nullptr) {
            printf("getprotobyname is NULL! [%s]\n", g_protocolList[i]);
            continue;
        }
        EXPECT_STREQ(g_protocolList[i], protoPtr->p_name);
        number = protoPtr->p_proto;

        protoPtr = nullptr;
        protoPtr = getprotobynumber(number);
        if (protoPtr == nullptr) {
            EXPECT_TRUE(protoPtr != NULL) << "ErrInfo:getprotobynumber NULL '" << g_protocolList[i] << "'";
            continue;
        }
        EXPECT_STREQ(g_protocolList[i], protoPtr->p_name);
        protoPtr = nullptr;
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_1800
 * @tc.name      : test get protocals by invalid name and number
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testGetProtoByAbNormal, Function | MediumTest | Level2)
{
    struct protoent* protoPtr = nullptr;
    protoPtr = getprotobyname("ipv4");
    EXPECT_TRUE(protoPtr == NULL);
    protoPtr = getprotobyname("idpr-cmtp-s");
    EXPECT_TRUE(protoPtr == NULL);
    protoPtr = getprotobyname("12&^%$\n\0?><");
    EXPECT_TRUE(protoPtr == NULL);

    protoPtr = getprotobynumber(-1);
    EXPECT_TRUE(protoPtr == NULL);
    protoPtr = getprotobynumber(256);
    EXPECT_TRUE(protoPtr == NULL);
}

/**
 * @tc.number    : SUB_KERNEL_NET_1900
 * @tc.name      : test getprotoent
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testGetProtoent, Function | MediumTest | Level2)
{
    int index = 0;
    struct protoent* protoPtr = nullptr;
    setprotoent(0);
    while (index < PROTOCOL_COUNT) {
        protoPtr = getprotoent();
        if (protoPtr == NULL) {
            EXPECT_TRUE(protoPtr != NULL) << "ErrInfo:getprotoent NULL, index[" << index << "]";
        } else {
            EXPECT_STREQ(g_protocolList[index], protoPtr->p_name);
            EXPECT_EQ(g_protocolNumber[index], protoPtr->p_proto);
        }
        protoPtr = nullptr;
        index ++;
    }

    protoPtr = getprotoent();
    EXPECT_TRUE(protoPtr == NULL);
}

/**
 * @tc.number    : SUB_KERNEL_NET_2000
 * @tc.name      : test setprotoent
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testSetProtoent, Function | MediumTest | Level2)
{
    int index = 0;
    struct protoent* protoPtr = nullptr;
    setprotoent(0);
    while (index < PROTOCOL_COUNT) {
        protoPtr = getprotoent();
        if (protoPtr == nullptr) {
            EXPECT_TRUE(protoPtr != NULL) << "ErrInfo:getprotoent NULL, index[" << index << "]";
        } else {
            EXPECT_STREQ(g_protocolList[index], protoPtr->p_name);
            EXPECT_EQ(g_protocolNumber[index], protoPtr->p_proto);
        }
        protoPtr = nullptr;
        index ++;
    }
    protoPtr = getprotoent();
    EXPECT_TRUE(protoPtr == NULL);

    setprotoent(0);
    protoPtr = nullptr;
    protoPtr = getprotoent();
    ASSERT_TRUE(protoPtr != NULL);
    EXPECT_STREQ(g_protocolList[0], protoPtr->p_name);
    EXPECT_EQ(g_protocolNumber[0], protoPtr->p_proto);

    index = 0;
    protoPtr = nullptr;
    setprotoent(0);
    while (index < PROTOCOL_COUNT) {
        protoPtr = getprotoent();
        if (protoPtr == nullptr) {
            EXPECT_TRUE(protoPtr != NULL) << "ErrInfo:getprotoent NULL, index[" << index << "]";
        } else {
            EXPECT_STREQ(g_protocolList[index], protoPtr->p_name);
            EXPECT_EQ(g_protocolNumber[index], protoPtr->p_proto);
        }
        protoPtr = nullptr;
        index ++;
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_2100
 * @tc.name      : test herror,no output and return value, so no check point
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testHerror, Function | MediumTest | Level2)
{
    const char* fileName = "/storage/test_herror";
    FILE* fp = freopen(fileName, "w", stderr);
    ASSERT_NE(nullptr, fp);

    const char *msg = "herror msg";
    herror(msg);
    const int errorCode[5] = {HOST_NOT_FOUND, TRY_AGAIN, NO_RECOVERY, NO_DATA, NO_ADDRESS};
    for (int i = 0; i < 5; i++) {
        h_errno = errorCode[i];
        herror(msg);
    }
    h_errno = -1;
    herror(msg);
    h_errno = 0;
    herror(msg);
    herror(msg);
    fclose(stderr);

    char buffer[256];
    const char* expectStr = "herror msg: Unknown errorherror msg: Host not foundherror msg: \
Try againherror msg: Non-recoverable errorherror msg: Address not availableherror msg: \
Address not availableherror msg: Unknown errorherror msg: Unknown errorherror msg: Unknown error";
    FILE* fpRead = fopen(fileName, "rb");
    size_t bytes = fread(buffer, 1, sizeof(buffer), fpRead);
    buffer[bytes] = 0;
    EXPECT_STREQ(expectStr, buffer);

    int ret = fclose(fpRead);
    EXPECT_EQ(0, ret);
    ret = remove(fileName);
    EXPECT_EQ(0, ret);
}

/**
 * @tc.number    : SUB_KERNEL_NET_2200
 * @tc.name      : test hstrerror normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testHstrerrorNormal, Function | MediumTest | Level2)
{
    const char errorMsgs[5][22] = {"Host not found", "Try again", "Non-recoverable error", "Address not available",
        "Address not available"};
    const int errorCode[5] = {HOST_NOT_FOUND, TRY_AGAIN, NO_RECOVERY, NO_DATA, NO_ADDRESS};
    for (int i = 0; i < 5; i++) {
        EXPECT_STREQ(errorMsgs[i], hstrerror(errorCode[i]));
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_2300
 * @tc.name      : test hstrerror abnormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testHstrerrorAbnormal, Function | MediumTest | Level2)
{
    const char* expectStr = "Unknown error";
    EXPECT_STREQ(expectStr, hstrerror(0));
    EXPECT_STREQ(expectStr, hstrerror(-1));
    EXPECT_STREQ(expectStr, hstrerror(16));
}

/**
 * @tc.number    : SUB_KERNEL_NET_2400
 * @tc.name      : test convert value from host to network byte order
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testHostToNetwork, Function | MediumTest | Level2)
{
    uint32_t intInput1 = 0;
    uint32_t intRst1 = htonl(intInput1);
    uint32_t intInput2 = 65536;
    uint32_t intRst2 = htonl(intInput2);

    uint16_t shortInput1 = 0;
    uint16_t shortRst1 = htons(shortInput1);
    uint16_t shortInput2 = 255;
    uint16_t shortRst2 = htons(shortInput2);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint32_t expectZero = 0;
    uint32_t expectForIinput2 = 256;
    uint32_t expectForSinput2 = 65280;
    EXPECT_EQ(expectZero, intRst1);
    EXPECT_EQ(expectForIinput2, intRst2);
    EXPECT_EQ(expectZero, shortRst1);
    EXPECT_EQ(expectForSinput2, shortRst2);
#else
    EXPECT_EQ(intInput1, intRst1);
    EXPECT_EQ(intInput2, intRst2);
    EXPECT_EQ(shortInput1, shortRst1);
    EXPECT_EQ(shortInput2, shortRst2);
#endif
}

/**
 * @tc.number    : SUB_KERNEL_NET_2500
 * @tc.name      : test convert value from network to host byte order
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testNetworkToHost, Function | MediumTest | Level2)
{
    uint32_t intInput1 = 0;
    uint32_t intRst1 = ntohl(intInput1);
    uint32_t intInput2 = 65536;
    uint32_t intRst2 = ntohl(intInput2);
    uint16_t shortInput1 = 0;
    uint16_t shortRst1 = ntohs(shortInput1);
    uint16_t shortInput2 = 255;
    uint16_t shortRst2 = ntohs(shortInput2);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint32_t expectZero = 0;
    uint32_t expectForIinput2 = 256;
    uint32_t expectForSinput2 = 65280;
    EXPECT_EQ(expectZero, intRst1);
    EXPECT_EQ(expectForIinput2, intRst2);
    EXPECT_EQ(expectZero, shortRst1);
    EXPECT_EQ(expectForSinput2, shortRst2);
#else
    EXPECT_EQ(intInput1, intRst1);
    EXPECT_EQ(intInput2, intRst2);
    EXPECT_EQ(shortInput1, shortRst1);
    EXPECT_EQ(shortInput2, shortRst2);
#endif
}

/**
 * @tc.number    : SUB_KERNEL_NET_2600
 * @tc.name      : test inet_pton IPv4 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetPtonIpv4Normal, Function | MediumTest | Level2)
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
        printf("[###]inet_pton : un[%u],s[%d],hex[%x]\n", rst.s_addr, rst.s_addr, rst.s_addr);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_2700
 * @tc.name      : test inet_pton IPv4 abnormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetPtonIpv4Abnormal, Function | MediumTest | Level2)
{
    int ret;
    struct in_addr rst = {0};
    char cpAddrs[10][16] = {"255", "256", "255.0", "255.0.0", "256.0.0.1", "a.a.a.a", "....", "#",
        "127.0.0.f", "0:0:0:0:0:0:0:1"};
    for (int i = 0; i < 10; i++) {
        ret = inet_pton(AF_INET, cpAddrs[i], &rst);
        EXPECT_EQ(0, ret) << "ErrInfo:inet_pton abnormal [" << cpAddrs[i] << "]";
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_2800
 * @tc.name      : test inet_pton IPv6 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetPtonIpv6Normal, Function | MediumTest | Level2)
{
    int ret;
    struct in6_addr rst = {0};
    char cpAddrs[6][40] = {"0101:0101:0101:0101:1010:1010:1010:1010", "0:0:0:0:0:0:0:0", 
        "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF", "::", "1::", "0011:0011:0011:0011:11:11:11:11"};
    for (int i = 0; i < 6; i++) {
        ret = inet_pton(AF_INET6, cpAddrs[i], &rst);
        EXPECT_EQ(1, ret) << "ErrInfo:inet_pton [" << cpAddrs[i] << "]";
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_2900
 * @tc.name      : test inet_pton IPv6 abnormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetPtonIpv6Abnormal, Function | MediumTest | Level2)
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
 * @tc.number    : SUB_KERNEL_NET_3000
 * @tc.name      : test inet_pton with invalid family
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetPtonInvalidFamily, Function | MediumTest | Level2)
{
    struct in_addr rst = {0};
    int ret = inet_pton(AF_IPX, "127.0.0.1", &rst);
    EXPECT_EQ(-1, ret);
    ret = inet_pton(-1, "127.0.0.1", &rst);
    EXPECT_EQ(-1, ret);
}

/**
 * @tc.number    : SUB_KERNEL_NET_3100
 * @tc.name      : test inet_ntop IPv4 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetNtopIpv4Normal, Function | MediumTest | Level2)
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
        if (ret == nullptr) {
            EXPECT_TRUE(ret != NULL) << "ErrInfo:inet_ntop NULL [" << expectAddrs[i] << "]";
        } else {
            printf("[###]inet_ntop expect [%s]: ret[%s], buf[%s]\n", expectAddrs[i], ret, rstBuff);
            EXPECT_STREQ(expectAddrs[i], ret);
            EXPECT_STREQ(expectAddrs[i], rstBuff);
        }
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_3200
 * @tc.name      : test inet_ntop IPv4 boundary input
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetNtopIpv4Abnormal, Function | MediumTest | Level2)
{
    const char* ret = nullptr;
    struct in_addr inputAddr = {0};
    char rstBuff[INET_ADDRSTRLEN];
    char expectStr[2][16] = {"255.255.255.255", "0.0.0.0"};
    for (int i = 0; i < 2; i++) {
        inputAddr.s_addr = in_addr_t(i == 0 ? -1 : 4294967296);
        ret = inet_ntop(AF_INET, &inputAddr, rstBuff, sizeof(rstBuff));
        ASSERT_TRUE(ret != NULL);
        EXPECT_STREQ(expectStr[i], ret);
        EXPECT_STREQ(expectStr[i], rstBuff);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_3300
 * @tc.name      : test inet_ntop IPv6 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetNtopIpv6Normal, Function | MediumTest | Level2)
{
    int iret;
    const char* ret = nullptr;
    struct in6_addr inputAddr = {0};
    char rstBuff[INET6_ADDRSTRLEN];
    char inputAddrs[6][40] = {"0101:0101:0101:0101:1010:1010:1010:1010", "0:0:0:0:0:0:0:0", 
        "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF", "::", "1::", "0011:0011:0011:0011:11:11:11:11"};
    char expectAddrs[6][40] = {"101:101:101:101:1010:1010:1010:1010", "::", 
        "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", "::", "1::", "11:11:11:11:11:11:11:11"};
    for (int i = 0; i < 6; i++) {
        iret = inet_pton(AF_INET6, inputAddrs[i], &inputAddr);
        ASSERT_EQ(1, iret);
        ret = inet_ntop(AF_INET6, &inputAddr, rstBuff, sizeof(rstBuff));
        if (ret == nullptr) {
            EXPECT_TRUE(ret != NULL) << "ErrInfo:inet_ntop NULL [" << expectAddrs[i] << "]";
        } else {
            printf("[###]inet_ntop expect [%s]: ret[%s], buf[%s]\n", expectAddrs[i], ret, rstBuff);
            EXPECT_STREQ(expectAddrs[i], ret);
            EXPECT_STREQ(expectAddrs[i], rstBuff);
        }
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_3400
 * @tc.name      : test inet_ntop IPv6 boundary input
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetNtopIpv6Abnormal, Function | MediumTest | Level2)
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
 * @tc.number    : SUB_KERNEL_NET_3500
 * @tc.name      : test inet_ntop with invalid family
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetNtopInvalidFamily, Function | MediumTest | Level2)
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
 * @tc.number    : SUB_KERNEL_NET_3600
 * @tc.name      : test inet_addr normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetAddrNormal, Function | MediumTest | Level2)
{
    in_addr_t rst;
    char cpAddrs[7][16] = {"10.58.212.100", "0.0.0.0", "255", "0.255.0", "255.0.0.0",
        "255.255.255.255", "257"};
    unsigned int expectList[7] = {1691630090, 0, 4278190080, 65280, 255, 4294967295, 16842752};
    for (int i = 0; i < 7; i++) {
        rst = inet_addr(cpAddrs[i]);
        EXPECT_EQ(expectList[i], rst);
        printf("[###]inet_addr %s: uns[%u],s[%d],hex[%x]\n", cpAddrs[i], rst, rst, rst);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_3700
 * @tc.name      : test inet_addr abnormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetAddrAbnormal, Function | MediumTest | Level2)
{
    in_addr_t rst;
    unsigned int expect = 4294967295;
    char invalidAddrs[4][10] = {"-1", "a", "%^", "257.1.1.1"};
    for (int i = 0; i < 4; i++) {
        rst = inet_addr(invalidAddrs[i]);
        EXPECT_EQ(expect, rst) << "ErrInfo:inet_addr [" << invalidAddrs[i] << "]";
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_3800
 * @tc.name      : test inet_aton normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetAtonNormal, Function | MediumTest | Level2)
{
    int ret;
    struct in_addr addr = {0};
    char cpAddrs[7][16] = {"10.58.212.100", "0.0.0.0", "255", "0.255.0", "255.0.0.0",
        "255.255.255.255", "257"};
    unsigned int expectList[7] = {1691630090, 0, 4278190080, 65280, 255, 4294967295, 16842752};
    for (int i = 0; i < 7; i++) {
        ret = inet_aton(cpAddrs[i], &addr);
        EXPECT_EQ(1, ret);
        EXPECT_EQ(expectList[i], addr.s_addr);
        printf("##inet_aton %s: uns[%u],s[%d],hex[%x]\n", cpAddrs[i], addr.s_addr, addr.s_addr, addr.s_addr);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_3900
 * @tc.name      : test inet_aton abnormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetAtonAbnormal, Function | MediumTest | Level2)
{
    int ret;
    struct in_addr addr = {0};
    char invalidAddrs[4][10] = {"-1", "a", "%^", "257.1.1.1"};
    for (int i = 0; i < 4; i++) {
        ret = inet_aton(invalidAddrs[i], &addr);
        EXPECT_EQ(0, ret) << "ErrInfo:inet_addr [" << invalidAddrs[i] << "]";
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_4000
 * @tc.name      : test inet_ntoa normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetNtoaNormal, Function | MediumTest | Level2)
{
    char *rst = nullptr;
    struct in_addr addr = {0};
    char expectAddrs[6][16] = {"10.58.212.100", "0.0.0.0", "255.255.255.255", "0.0.0.255", "0.255.0.0", "0.0.1.1"};
    unsigned int inputList[6] = {1691630090, 0, 4294967295, 4278190080, 65280, 16842752};
    for (int i = 0; i < 6; i++) {
        addr.s_addr = inputList[i];
        rst = inet_ntoa(addr);
        if (rst == nullptr) {
            EXPECT_TRUE(rst != NULL);
        } else {
            EXPECT_STREQ(expectAddrs[i], rst);
        }
        printf("##inet_ntoa %u: rst is [%s]\n", inputList[i], expectAddrs[i]);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_4100
 * @tc.name      : test inet_ntoa boundary input
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetNtoaAbnormal, Function | MediumTest | Level2)
{
    const char* ret = nullptr;
    struct in_addr addrInput = {0};
    char expectStr[2][16] = {"255.255.255.255", "0.0.0.0"};
    for (int i = 0; i < 2; i++) {
        addrInput.s_addr = (i == 0 ? -1 : 4294967296);
        ret = inet_ntoa(addrInput);
        ASSERT_TRUE(ret != NULL);
        EXPECT_STREQ(expectStr[i], ret);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_4200
 * @tc.name      : test inet_network normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetNetworkNormal, Function | MediumTest | Level2)
{
    in_addr_t rst;
    char cpAddrs[7][16] = {"10.58.212.100", "0.0.0.0", "255", "0.255.0", "255.0.0.0",
        "255.255.255.255", "257"};
    unsigned int expectList[7] = {171627620, 0, 255, 16711680, 4278190080, 4294967295, 257};
    for (int i = 0; i < 7; i++) {
        rst = inet_network(cpAddrs[i]);
        EXPECT_EQ(expectList[i], rst);
        printf("##inet_network %s: uns[%u],s[%d],hex[%x]\n", cpAddrs, rst, rst, rst);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_4300
 * @tc.name      : test inet_network abnormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetNetworkAbnormal, Function | MediumTest | Level2)
{
    in_addr_t rst;
    unsigned int expect = 4294967295;
    char invalidAddrs[4][10] = {"-1", "a", "%^", "257.1.1.1"};
    for (int i = 0; i < 4; i++) {
        rst = inet_network(invalidAddrs[i]);
        EXPECT_EQ(expect, rst) << "ErrInfo:inet_network [" << invalidAddrs[i] << "]";
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_4400
 * @tc.name      : test inet_lnaof normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetLnaofNormal, Function | MediumTest | Level2)
{
    in_addr_t rst;
    struct in_addr addr = {0};
    char cpAddrs[7][16] = {"10.58.212.100", "0.0.0.0", "255", "0.255.0", "255.0.0.0",
        "255.255.255.255", "257"};
    unsigned int expectList[7] = {13908490, 0, 0, 65280, 255, 255, 65536};
    for (int i = 0; i < 7; i++) {
        addr.s_addr = inet_network(cpAddrs[i]);
        rst = inet_lnaof(addr);
        EXPECT_EQ(expectList[i], rst);
        printf("[###]inet_lnaof %s: uns[%u],s[%d],hex[%x]\n", cpAddrs[i], rst, rst, rst);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_4500
 * @tc.name      : test inet_netof normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetNetofNormal, Function | MediumTest | Level2)
{
    in_addr_t rst;
    struct in_addr addr = {0};
    char cpAddrs[7][16] = {"10.58.212.100", "0.0.0.0", "255", "0.255.0", "255.0.0.0",
        "255.255.255.255", "257"};
    unsigned int expectList[7] = {100, 0, 16711680, 0, 0, 16777215, 1};
    for (int i = 0; i < 7; i++) {
        addr.s_addr = inet_network(cpAddrs[i]);
        rst = inet_netof(addr);
        EXPECT_EQ(expectList[i], rst);
        printf("[###]inet_netof %s: uns[%u],s[%d],hex[%x]\n", cpAddrs[i], rst, rst, rst);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_4600
 * @tc.name      : test inet_makeaddr normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInetMakeaddrNormal, Function | MediumTest | Level2)
{
    struct in_addr addrRst = {0};
    int hostList[7] = {13908490, 0, 0, 65280, 255, 255, 65536};
    int netList[7] = {100, 0, 16711680, 0, 0, 16777215, 1};
    unsigned int expectList[7] = {171627620, 0, 255, 16711680, 4278190080, 4294967295, 257};
    for (int i = 0; i < 7; i++) {
        addrRst = inet_makeaddr(netList[i], hostList[i]);
        EXPECT_EQ(expectList[i], addrRst.s_addr);
        printf("[###]inet_makeaddr: uns[%u],s[%d],hex[%x]\n", addrRst.s_addr, addrRst.s_addr, addrRst.s_addr);
    }
}

/**
 * @tc.number    : SUB_KERNEL_NET_4700
 * @tc.name      : test invalid parameter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsNetTest, testInvalidParameter, Function | MediumTest | Level3)
{
    int ret;
    int fdFail = -1;
    int fdSuccess = -1;

    fdFail = socket(0, 0, 0);
    EXPECT_EQ(fdFail, -1);
    fdSuccess = socket(AF_INET, SOCK_STREAM, 0);
    EXPECT_NE(fdSuccess, -1);

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

    ret = recv(fdFail, nullptr, sizeof(g_srvMsg), 0);
    EXPECT_EQ(-1, ret);
    ret = recv(fdSuccess, nullptr, sizeof(g_srvMsg), 0);
    EXPECT_EQ(-1, ret);

    struct msghdr msg = {nullptr};
    ret = recvmsg(fdFail, &msg, 0);
    EXPECT_EQ(-1, ret);
    ret = recvmsg(fdSuccess, &msg, 0);
    EXPECT_EQ(-1, ret);

    ret = recvfrom(fdFail, nullptr, sizeof(g_srvMsg), 0, nullptr, nullptr);
    EXPECT_EQ(-1, ret);
    ret = recvfrom(fdSuccess, nullptr, sizeof(g_srvMsg), 0, nullptr, nullptr);
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
