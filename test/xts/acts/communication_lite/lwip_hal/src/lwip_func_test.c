/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "hctest.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "securec.h"
#include "cmsis_os2.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define LOCAL_HOST "127.0.0.1"
#define STACK_PORT 2277
#define STACK_IP LOCAL_HOST
#define PEER_PORT STACK_PORT
#define PEER_IP LOCAL_HOST
#define UDP_MSG "Hi, I am UDP"
#define SRV_MSG "Hi, I am TCP server"
#define CLI_MSG "Hi, I am TCP client"
#define BUF_SIZE (1024 * 8)
#define LWIP_TEST_SUCCESS 0
#define LWIP_TEST_FAIL (-1)
#define DEF_TASK_STACK 2000
#define DEF_TASK_PRIORITY 20
#define ONE_SECOND 1
#define TIMEOUT 4
#define TEST_FD_COUNT 10

static char g_buf[BUF_SIZE + 1] = {0};
static int g_clientWait = 0;
static int g_clientWaitOver = 0;
static int g_serverWait = 0;
static int g_serverWaitOver = 0;
static int g_clientResult = 0;
static int g_serverResult = 0;
static int g_selectResult = 0;
static int g_selectFlag = 1;
static int g_selectTimeout = 2;

static void WaitClient(void)
{
    while (1) {
        sleep(ONE_SECOND);
        if (g_clientWait) {
            break;
        }
    }
    g_clientWaitOver = 1;
    g_clientWait = 0;
}

static void WaitServer(void)
{
    while (1) {
        sleep(ONE_SECOND);
        if (g_serverWait) {
            break;
        }
    }
    g_serverWaitOver = 1;
    g_serverWait = 0;
}

static int CommInitTcpServer(int port)
{
    int srvFd = socket(AF_INET, SOCK_STREAM, 0);
    if (srvFd == -1) {
        printf("CommInitTcpServer]socket fail!, errinfo[%s]\n", strerror(errno));
        return -1;
    }
    int flag = 1;
    int ret = setsockopt(srvFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    if (ret != 0) {
        printf("[CommInitTcpServer]setsockopt fail, ret[%d]!\n", ret);
    }
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    srvAddr.sin_port = htons(port);
    ret = bind(srvFd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    if (ret != 0) {
        printf("[CommInitTcpServer]::bind fail!\n");
        lwip_close(srvFd);
        return -1;
    }
    int backlog = 5;
    ret = listen(srvFd, backlog);
    if (ret != 0) {
        printf("[CommInitTcpServer]listen fail!\n");
        lwip_close(srvFd);
        return -1;
    }
    printf("[CommInitTcpServer]success,Fd[%d]\n", srvFd);
    return srvFd;
}

static void CommTcpClientTask(void)
{
    int clnFd = socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT_NOT_EQUAL(-1, clnFd);
    if (clnFd == -1) {
        printf("[comm client]socket fail, errinfo[%s]\n", strerror(errno));
        return;
    }
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    srvAddr.sin_port = htons(PEER_PORT+1);
    int rst = connect(clnFd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    TEST_ASSERT_EQUAL_INT(0, rst);
    if (rst != 0) {
        printf("[comm client]connect fail\n");
        lwip_close(clnFd);
        return;
    }
    int count = 4;
    char dataBuf[50] = {0};
    char sendMsgList[5][50] = {"Hi, I'm client,FD:[%d]", "client:123456789abcdefg,FD:[%d]",
        "client:!!@@##$$%%^^&&**(()),FD:[%d]", "client:(((112233445566778899))),FD:[%d]", "bye"};
    for (int i = 0; i < count; i++) {
        sleep(ONE_SECOND);
        (void) memset_s(dataBuf, sizeof(dataBuf), 0, sizeof(dataBuf));
        rst = sprintf_s(dataBuf, sizeof(dataBuf), sendMsgList[i], clnFd);
        if (rst < 0) {
            printf("[comm client]sprintf_s error!\n");
            continue;
        }
        rst = send(clnFd, dataBuf, strlen(dataBuf), 0);
        TEST_ASSERT_TRUE(rst > 0);
        if (rst <= 0) {
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
    TEST_ASSERT_TRUE(rst > 0);
    if (rst <= 0) {
        printf("[comm client][%d]send fail\n", clnFd);
    }
    rst = lwip_close(clnFd);
    TEST_ASSERT_EQUAL_INT(0, rst);
    if (rst != 0) {
        printf("[comm client][%d]close fd fail\n", clnFd);
    }
}

static void SampleTcpServerTask(void)
{
    int ret;
    struct msghdr msg = {0};
    struct iovec iov[2] = {};
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);
    static char buf[BUF_SIZE + 1] = {0};

    int srvFd = CommInitTcpServer(STACK_PORT);
    TEST_ASSERT_NOT_EQUAL(-1, srvFd);

    WaitServer();
    int clientFd = accept(srvFd, (struct sockaddr*)&clnAddr, &clnAddrLen);
    printf("[tcp server]accept <%s:%d>\n", inet_ntoa(clnAddr.sin_addr), ntohs(clnAddr.sin_port));
    TEST_ASSERT_NOT_EQUAL(LWIP_TEST_FAIL, clientFd);

    int srvMsgLen = strlen(SRV_MSG);
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(buf, BUF_SIZE, SRV_MSG);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);
    ret = send(clientFd, buf, srvMsgLen, 0);
    printf("[tcp server]send, ret=%d\n", ret);
    TEST_ASSERT_EQUAL_INT(srvMsgLen, ret);

    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = recv(clientFd, buf, sizeof(buf), 0);
    printf("[tcp server]recv, ret=%d\n", ret);
    TEST_ASSERT_EQUAL_INT(strlen(CLI_MSG), ret);

    WaitServer();
    int len = 2;
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    clnAddr.sin_port = htons(PEER_PORT);
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(buf, BUF_SIZE, SRV_MSG);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = len;
    iov[0].iov_base = buf;
    iov[0].iov_len = srvMsgLen;
    iov[1].iov_base = buf;
    iov[1].iov_len = srvMsgLen;
    ret = sendmsg(clientFd, &msg, 0);
    printf("[tcp server]sendmsg, ret=%d\n", ret);
    TEST_ASSERT_EQUAL_INT(len * srvMsgLen, ret);

    WaitServer();
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    int recvSum = 0;
    int recvTimes = 50;
    while (recvTimes > 0) {
        ret = recv(clientFd, buf, sizeof(buf), 0);
        recvSum += ret;
        if ((unsigned int)recvSum == (len * strlen(SRV_MSG)) || ret <= 0) {
            break;
        }
        recvTimes--;
    }
    printf("[tcp server]recv, recvSum=%d,recvTimes=%d\n", recvSum, recvTimes);
    TEST_ASSERT_EQUAL_INT(len * strlen(CLI_MSG), recvSum);

    ret = shutdown(clientFd, SHUT_RDWR);
    printf("[tcp server]shutdown, ret=%d\n", ret);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);

    lwip_close(clientFd);
    lwip_close(srvFd);
    g_serverResult = 1;
    WaitServer();
}

static void SampleTcpClientTask(void)
{
    int ret;
    struct msghdr msg = {0};
    struct iovec iov[2] = {};
    struct sockaddr addr;
    socklen_t addrLen = sizeof(addr);
    struct sockaddr_in srvAddr = {0};
    struct sockaddr_in clnAddr = {0};
    static char buf[BUF_SIZE+1] = {0};

    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT_NOT_EQUAL(LWIP_TEST_FAIL, clientFd);
    if (clientFd == -1) {
        printf("[tcp client]socket fail, errinfo[%s]\n", strerror(errno));
    }

    WaitClient();
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    srvAddr.sin_port = htons(PEER_PORT);
    ret = connect(clientFd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    printf("[tcp client]connect %s:%d, ret=%d\n", inet_ntoa(srvAddr.sin_addr), ntohs(srvAddr.sin_port), ret);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);

    ret = getpeername(clientFd, &addr, &addrLen);
    printf("[tcp client]getpeername %s:%d, ret=%d\n", inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr),
        ntohs(((struct sockaddr_in*)&addr)->sin_port), ret);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);
    TEST_ASSERT_EQUAL_INT(sizeof(struct sockaddr_in), addrLen);
    TEST_ASSERT_EQUAL_INT(inet_addr(PEER_IP), ((struct sockaddr_in*)&addr)->sin_addr.s_addr);

    ret = getsockname(clientFd, &addr, &addrLen);
    printf("[tcp client]getsockname %s:%d, ret=%d\n", inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr),
        ntohs(((struct sockaddr_in*)&addr)->sin_port), ret);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);
    TEST_ASSERT_EQUAL_INT(sizeof(struct sockaddr_in), addrLen);
    TEST_ASSERT_EQUAL_INT(inet_addr(STACK_IP), ((struct sockaddr_in*)&addr)->sin_addr.s_addr);

    int cliMsgLen = strlen(CLI_MSG);
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(buf, BUF_SIZE, CLI_MSG);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);
    ret = send(clientFd, buf, cliMsgLen, 0);
    printf("[tcp client]send, ret=%d\n", ret);
    TEST_ASSERT_EQUAL_INT(cliMsgLen, ret);

    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = recv(clientFd, buf, sizeof(buf), 0);
    printf("[tcp client]recv,ret=%d\n", ret);
    TEST_ASSERT_EQUAL_INT(strlen(SRV_MSG), ret);

    WaitClient();
    int len = 2;
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    clnAddr.sin_port = htons(PEER_PORT);
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(buf, BUF_SIZE, CLI_MSG);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = len;
    iov[0].iov_base = buf;
    iov[0].iov_len = cliMsgLen;
    iov[1].iov_base = buf;
    iov[1].iov_len = cliMsgLen;
    ret = sendmsg(clientFd, &msg, 0);
    printf("[tcp client]sendmsg, ret=%d\n", ret);
    TEST_ASSERT_EQUAL_INT(len * cliMsgLen, ret);

    WaitClient();
    (void) memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    int recvSum = 0;
    int recvTimes = 50;
    while (recvTimes > 0) {
        ret = recv(clientFd, buf, sizeof(buf), 0);
        recvSum += ret;
        if ((unsigned int)recvSum == (len * strlen(SRV_MSG)) || ret <= 0) {
            break;
        }
        recvTimes--;
    }
    printf("[tcp client]recv, recvSum=%d,recvTimes=%d\n", recvSum, recvTimes);
    TEST_ASSERT_EQUAL_INT(len * strlen(SRV_MSG), recvSum);

    ret = shutdown(clientFd, SHUT_RDWR);
    printf("[tcp client]shutdown, ret=%d\n", ret);
    TEST_ASSERT_EQUAL_INT(0, ret);

    lwip_close(clientFd);
    g_clientResult = 1;
    WaitClient();
}

static void SelectServerTask(void)
{
    int srvFd = CommInitTcpServer(STACK_PORT + 1);
    if (srvFd == -1) {
        g_selectResult = -1;
        g_selectFlag = 0;
        return;
    }
    int ret;
    int cliCount = 0;
    int maxFd = srvFd;
    fd_set readSet;
    char dataBuf[50] = {0};
    int fds[TEST_FD_COUNT] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    fds[0] = srvFd;
    struct timeval timev = {
        .tv_sec = g_selectTimeout,
        .tv_usec = 0
    };
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
            int retTimeout  = -2;
            g_selectResult = retTimeout;
            break;
        }
        if (ret == -1) {
            g_selectResult = -1;
            printf("[select process]select fail, errinfo[%s]!\n", strerror(errno));
            break;
        }
        if (FD_ISSET(srvFd, &readSet)) {
            int cliFd = accept(srvFd, (struct sockaddr*)&clnAddr, &clnAddrLen);
            if (cliFd == -1) {
                printf("[select process]accept fail!\n");
                g_selectResult = -1;
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
                        lwip_close(fds[i]);
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
            g_selectResult = 0;
            printf("[select process]cliCount=0, over!\n");
            break;
        }
    }
    lwip_close(srvFd);
    g_selectFlag = 0;
}

/**
 * @tc.desc      : register a test suite, this suite is used to test function
 * @param        : subsystem name is communication
 * @param        : module name is lwip
 * @param        : test suit name is LwipFuncTestSuite
 */
LITE_TEST_SUIT(communication, lwip, LwipFuncTestSuite);

/**
 * @tc.setup     : setup for every testcase
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL LwipFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for every testcase
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL LwipFuncTestSuiteTearDown(void)
{
    printf("+----------------------------------------------------------+\n");
    return TRUE;
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0100
 * @tc.name      : sample test tcp
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testTcp, Function | MediumTest | Level2)
{
    osThreadAttr_t attrServer;
    attrServer.name = "SampleTcpServerTask";
    attrServer.attr_bits = 0U;
    attrServer.cb_mem = NULL;
    attrServer.cb_size = 0U;
    attrServer.stack_mem = NULL;
    attrServer.stack_size = DEF_TASK_STACK;
    attrServer.priority = DEF_TASK_PRIORITY;

    osThreadAttr_t attrClient;
    attrClient.name = "SampleTcpClientTask";
    attrClient.attr_bits = 0U;
    attrClient.cb_mem = NULL;
    attrClient.cb_size = 0U;
    attrClient.stack_mem = NULL;
    attrClient.stack_size = DEF_TASK_STACK;
    attrClient.priority = DEF_TASK_PRIORITY;

    osThreadId_t serverTaskId = osThreadNew((osThreadFunc_t)SampleTcpServerTask, NULL, &attrServer);
    TEST_ASSERT_NOT_NULL(serverTaskId);
    if (serverTaskId == NULL) {
        printf("[testTcp]create server task fail!\n");
    }
    osThreadId_t clientTaskId = osThreadNew((osThreadFunc_t)SampleTcpClientTask, NULL, &attrClient);
    TEST_ASSERT_NOT_NULL(clientTaskId);
    if (clientTaskId == NULL) {
        printf("[testTcp]create client task fail!\n");
    }

    sleep(ONE_SECOND);
    int timeout = TIMEOUT;
    g_serverWait = 1;
    g_clientWait = 1;
    while (timeout > 0) {
        if (g_serverWaitOver == 1 && g_clientWaitOver == 1) {
            printf("[testTcp] wait success[1]!\n");
            break;
        }
        timeout--;
        sleep(ONE_SECOND);
        printf("[testTcp] wait[1]...\n");
    }

    sleep(ONE_SECOND);
    timeout = TIMEOUT;
    g_serverWait = 1;
    g_clientWait = 1;
    while (timeout > 0) {
        if (g_serverWaitOver == 1 && g_clientWaitOver == 1) {
            printf("[testTcp] wait success[2]!\n");
            break;
        }
        timeout--;
        sleep(ONE_SECOND);
        printf("[testTcp] wait[2]...\n");
    }

    sleep(ONE_SECOND);
    timeout = TIMEOUT;
    g_serverWait = 1;
    g_clientWait = 1;
    while (timeout > 0) {
        if (g_serverWaitOver == 1 && g_clientWaitOver == 1) {
            printf("[testTcp] wait success[3]!\n");
            break;
        }
        timeout--;
        sleep(ONE_SECOND);
        printf("[testTcp] wait[3]...\n");
    }
    sleep(ONE_SECOND);
    timeout = TIMEOUT;
    g_serverWait = 1;
    g_clientWait = 1;
    while (timeout > 0) {
        if (g_serverWaitOver == 1 && g_clientWaitOver == 1) {
            printf("[testTcp] wait success[4]!\n");
            break;
        }
        timeout--;
        sleep(ONE_SECOND);
        printf("[testTcp] wait[4]...\n");
    }
    TEST_ASSERT_EQUAL_INT(1, g_clientResult);
    TEST_ASSERT_EQUAL_INT(1, g_serverResult);
    
    sleep(ONE_SECOND);
    g_serverWait = 1;
    g_clientWait = 1;
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0200
 * @tc.name      : sample test udp
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testUdp, Function | MediumTest | Level2)
{
    int ret;
    int len = 2;
    struct msghdr msg = {0};
    struct iovec iov[2] = {0};
    struct sockaddr_in srvAddr = {0};
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    TEST_ASSERT_NOT_EQUAL(LWIP_TEST_FAIL, ret);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(STACK_IP);
    srvAddr.sin_port = htons(STACK_PORT);
    ret = bind(fd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);

    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    clnAddr.sin_port = htons(PEER_PORT);
    (void) memset_s(g_buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(g_buf, BUF_SIZE, UDP_MSG);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);
    ret = sendto(fd, g_buf, strlen(UDP_MSG), 0, (struct sockaddr*)&clnAddr, (socklen_t)sizeof(clnAddr));
    TEST_ASSERT_NOT_EQUAL(LWIP_TEST_FAIL, ret);

    (void) memset_s(g_buf, BUF_SIZE, 0, BUF_SIZE);
    ret = recvfrom(fd, g_buf, sizeof(g_buf), 0, (struct sockaddr*)&clnAddr, &clnAddrLen);
    TEST_ASSERT_EQUAL_INT(strlen(UDP_MSG), ret);

    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    clnAddr.sin_port = htons(PEER_PORT);
    (void) memset_s(g_buf, BUF_SIZE, 0, BUF_SIZE);
    ret = strcpy_s(g_buf, BUF_SIZE, UDP_MSG);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_SUCCESS, ret);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = len;
    iov[0].iov_base = g_buf;
    iov[0].iov_len = strlen(UDP_MSG);
    iov[1].iov_base = g_buf;
    iov[1].iov_len = strlen(UDP_MSG);
    ret = sendmsg(fd, &msg, 0);
    TEST_ASSERT_EQUAL_INT(len * strlen(UDP_MSG), ret);

    ret = lwip_close(fd);
    TEST_ASSERT_NOT_EQUAL(LWIP_TEST_FAIL, ret);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0400
 * @tc.name      : test select timeout
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testSelectTimeout, Function | MediumTest | Level2)
{
    osThreadAttr_t tSelect;
    tSelect.name = "SelectServerTask";
    tSelect.attr_bits = 0U;
    tSelect.cb_mem = NULL;
    tSelect.cb_size = 0U;
    tSelect.stack_mem = NULL;
    tSelect.stack_size = DEF_TASK_STACK;
    tSelect.priority = DEF_TASK_PRIORITY;

    g_selectTimeout = 2;
    osThreadId_t serverTaskId = osThreadNew((osThreadFunc_t)SelectServerTask, NULL, &tSelect);
    TEST_ASSERT_NOT_NULL(serverTaskId);
    if (serverTaskId == NULL) {
        printf("create select server task fail!\n");
    } else {
        while (g_selectFlag) {
            sleep(ONE_SECOND);
            printf("wait select server finish...\n");
        }
        TEST_ASSERT_EQUAL_INT(-2, g_selectResult);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0500
 * @tc.name      : test select with one clients
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testSelectOneClient, Function | MediumTest | Level2)
{
    osThreadAttr_t tSelect;
    tSelect.name = "SelectServerTask";
    tSelect.attr_bits = 0U;
    tSelect.cb_mem = NULL;
    tSelect.cb_size = 0U;
    tSelect.stack_mem = NULL;
    tSelect.stack_size = DEF_TASK_STACK;
    tSelect.priority = DEF_TASK_PRIORITY;

    g_selectTimeout = 5;
    osThreadId_t serverTaskId = osThreadNew((osThreadFunc_t)SelectServerTask, NULL, &tSelect);
    TEST_ASSERT_NOT_NULL(serverTaskId);
    sleep(ONE_SECOND);
    if (serverTaskId == NULL) {
        printf("create select server task fail!\n");
    } else {
        osThreadAttr_t tClient;
        osThreadId_t clientTaskId;
        tClient.name = "CommTcpClientTask";
        tClient.attr_bits = 0U;
        tClient.cb_mem = NULL;
        tClient.cb_size = 0U;
        tClient.stack_mem = NULL;
        tClient.stack_size = DEF_TASK_STACK;
        tClient.priority = DEF_TASK_PRIORITY;
        clientTaskId = osThreadNew((osThreadFunc_t)CommTcpClientTask, NULL, &tClient);
        TEST_ASSERT_NOT_NULL(clientTaskId);
        }

        g_selectFlag = 1;
        while (g_selectFlag) {
            sleep(ONE_SECOND);
            printf("wait select server finish...\n");
        }
        TEST_ASSERT_EQUAL_INT(0, g_selectResult);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0700
 * @tc.name      : test socket operation
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testSocketOpt, Function | MediumTest | Level2)
{
    socklen_t len;
    struct timeval timeout;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    int error = -1;
    len = sizeof(error);
    int ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, error);

    len = sizeof(timeout);
    ret = getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, &len);
    TEST_ASSERT_EQUAL_INT(0, ret);

    timeout.tv_sec = 1000;
    len = sizeof(timeout);
    ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, len);
    TEST_ASSERT_EQUAL_INT(0, ret);

    (void) memset_s(&timeout, len, 0, len);
    ret = getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, &len);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(1000, timeout.tv_sec);

    int flag = 1;
    ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    TEST_ASSERT_EQUAL_INT(0, ret);

    flag = 0;
    len = sizeof(flag);
    ret = getsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, &len);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(1, flag);

    error = -1;
    len = sizeof(error);
    ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, error);

    ret = lwip_close(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0800
 * @tc.name      : test getsockname and getpeername invalid input
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testGetSocketNameInvalidInput, Function | MediumTest | Level3)
{
    struct sockaddr addr;
    socklen_t addrLen = sizeof(addr);
    int ret = getsockname(-1, &addr, &addrLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getpeername(-1, &addr, &addrLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getsockname(0, &addr, &addrLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getpeername(0, &addr, &addrLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getsockname(1, &addr, &addrLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getpeername(1, &addr, &addrLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getsockname(130, &addr, &addrLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getpeername(130, &addr, &addrLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getsockname(10, NULL, &addrLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getpeername(10, NULL, &addrLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getsockname(10, &addr, NULL);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = getpeername(10, &addr, NULL);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0900
 * @tc.name      : test inet_pton IPv4 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInetPtonIpv4Normal, Function | MediumTest | Level2)
{
    int ret;
    struct in_addr rst;
    char cpAddrs[4][16] = {"10.58.212.100", "0.0.0.0", "255.0.0.0", "255.255.255.255"};
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned int expectLittle[4] = {1691630090, 0, 255, 4294967295};
#else
    unsigned int expectBig[4] = {171627620, 0, 4278190080, 4294967295};
#endif

    for (int i = 0; i < 4; i++) {
        ret = inet_pton(AF_INET, cpAddrs[i], &rst);
        TEST_ASSERT_EQUAL_INT(1, ret);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        TEST_ASSERT_EQUAL_INT(expectLittle[i], rst.s_addr);
#else
        TEST_ASSERT_EQUAL_INT(expectBig[i], rst.s_addr);
#endif
        printf("inet_pton %s: un[%u],s[%d],hex[%x]\n", cpAddrs[i], rst.s_addr, rst.s_addr, rst.s_addr);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1000
 * @tc.name      : test inet_pton IPv4 abnormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInetPtonIpv4Abnormal, Function | MediumTest | Level2)
{
    int ret;
    struct in_addr rst;
    char cpAddrs[6][16] = {"256.0.0.1", "a.a.a.a", "....", "#", "127.0.0.f", "0:0:0:0:0:0:0:1"};
    for (int i = 0; i < 6; i++) {
        ret = inet_pton(AF_INET, cpAddrs[i], &rst);
        TEST_ASSERT_EQUAL_INT(0, ret);
        printf("inet_pton error, cpAddr[%s]\n", cpAddrs[i]);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1100
 * @tc.name      : test inet_pton IPv6 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInetPtonIpv6Normal, Function | MediumTest | Level2)
{
    int ret;
    struct in6_addr rst;
    char cpAddrs[6][40] = {"fc00:0101:0011:0011:0011:0011:0011:0011", "0:0:0:0:0:0:0:0", 
        "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF", "::", "1::", "fc00:0011:0011:0011:11:11:11:11"};
    for (int i = 0; i < 6; i++) {
        ret = inet_pton(AF_INET6, cpAddrs[i], &rst);
        TEST_ASSERT_EQUAL_INT(1, ret);
        printf("inet_pton error, cpAddr[%s]\n", cpAddrs[i]);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1200
 * @tc.name      : test inet_pton IPv6 abnormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInetPtonIpv6Abnormal, Function | MediumTest | Level2)
{
    int ret;
    struct in6_addr rst;
    char cpAddrs[7][40] = {"127.0.0.1", "f", ":", "0:0", "1:::", ":::::::",
        "1111:1111:1111:1111:1111:1111:1111:111G"};
    for (int i = 0; i < 7; i++) {
        ret = inet_pton(AF_INET6, cpAddrs[i], &rst);
        TEST_ASSERT_EQUAL_INT(0, ret);
        printf("inet_pton error, cpAddr[%s]\n", cpAddrs[i]);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1300
 * @tc.name      : test inet_pton with invalid family
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInetPtonInvalidFamily, Function | MediumTest | Level2)
{
    struct in_addr rst;
    int ret = inet_pton(AF_IPX, "127.0.0.1", &rst);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    ret = inet_pton(-1, "127.0.0.1", &rst);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1400
 * @tc.name      : test inet_ntop IPv4 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInetNtopIpv4Normal, Function | MediumTest | Level2)
{
    const char* ret = NULL;
    struct in_addr inputAddr;
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
        if (ret == NULL) {
            TEST_ASSERT_TRUE(ret != NULL);
            printf("inet_ntop error, cpAddr[%s]\n", expectAddrs[i]);
        } else {
            printf("inet_ntop expect [%s]: ret[%s], buf[%s]\n", expectAddrs[i], ret, rstBuff);
            TEST_ASSERT_EQUAL_STRING(expectAddrs[i], ret);
            TEST_ASSERT_EQUAL_STRING(expectAddrs[i], rstBuff);
        }
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1500
 * @tc.name      : test inet_ntop IPv4 boundary input
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInetNtopIpv4Abnormal, Function | MediumTest | Level2)
{
    const char* ret = NULL;
    struct in_addr inputAddr;
    char rstBuff[INET_ADDRSTRLEN];
    char expectStr[2][16] = {"255.255.255.255", "0.0.0.0"};
    for (int i = 0; i < 2; i++) {
        inputAddr.s_addr = (i == 0 ? -1 : 4294967296);
        ret = inet_ntop(AF_INET, &inputAddr, rstBuff, sizeof(rstBuff));
        TEST_ASSERT_TRUE(ret != NULL);
        TEST_ASSERT_EQUAL_STRING(expectStr[i], ret);
        TEST_ASSERT_EQUAL_STRING(expectStr[i], rstBuff);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1600
 * @tc.name      : test inet_ntop IPv6 normal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInetNtopIpv6Normal, Function | MediumTest | Level2)
{
    int iret;
    const char* ret = NULL;
    struct in6_addr inputAddr;
    char rstBuff[INET6_ADDRSTRLEN];
    char inputAddrs[6][40] = {"fc00:0101:0011:0011:0011:0011:0011:0011", "0:0:0:0:0:0:0:0", 
        "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF", "::", "1::", "fc00:0011:0011:0011:1100:11:11:11"};
    char expectAddrs[6][40] = {"FC00:101:11:11:11:11:11:11", "::", 
            "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF", "::", "1::", "FC00:11:11:11:1100:11:11:11"};
    for (int i = 0; i < 6; i++) {
        iret = inet_pton(AF_INET6, inputAddrs[i], &inputAddr);
        TEST_ASSERT_EQUAL_INT(1, iret);
        ret = inet_ntop(AF_INET6, &inputAddr, rstBuff, sizeof(rstBuff));
        if (ret == NULL) {
            TEST_ASSERT_TRUE(ret != NULL);
            printf("inet_ntop error, cpAddr[%s]\n", expectAddrs[i]);
        } else {
            printf("inet_ntop expect [%s]: ret[%s], buf[%s]\n", expectAddrs[i], ret, rstBuff);
            TEST_ASSERT_EQUAL_STRING(expectAddrs[i], ret);
            TEST_ASSERT_EQUAL_STRING(expectAddrs[i], rstBuff);
        }
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1700
 * @tc.name      : test inet_ntop IPv6 boundary input
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInetNtopIpv6Abnormal, Function | MediumTest | Level2)
{
    const char* ret = NULL;
    struct in6_addr inputAddr;
    char rstBuff[INET6_ADDRSTRLEN];
    char expectStr[2][40] = {"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF", "::"};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 16; j++) {
            inputAddr.s6_addr[j] = (i == 0 ? -1 : 256);
        }
        ret = inet_ntop(AF_INET6, &inputAddr, rstBuff, sizeof(rstBuff));
        TEST_ASSERT_TRUE(ret != NULL);
        TEST_ASSERT_EQUAL_STRING(expectStr[i], ret);
        TEST_ASSERT_EQUAL_STRING(expectStr[i], rstBuff);
    }
}

/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_1800
 * @tc.name      : test inet_ntop with invalid family
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInetNtopInvalidFamily, Function | MediumTest | Level2)
{
    int iret;
    const char* ret = NULL;
    struct in6_addr inputAddr;
    char rstBuff[INET6_ADDRSTRLEN];

    iret = inet_pton(AF_INET6, "1::", &inputAddr);
    TEST_ASSERT_EQUAL_INT(1, iret);
    ret = inet_ntop(AF_IPX, &inputAddr, rstBuff, sizeof(rstBuff));
    TEST_ASSERT_TRUE(ret == NULL);
    ret = inet_ntop(-1, &inputAddr, rstBuff, sizeof(rstBuff));
    TEST_ASSERT_TRUE(ret == NULL);
}
/**
 * @tc.number    : SUB_COMMUNICATION_LWIP_SDK_0500
 * @tc.name      : test invalid parameter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(LwipFuncTestSuite, testInvalidParameter, Function | MediumTest | Level2)
{
    int ret;
    int fdFail = -1;
    int fdSuccess = -1;

    fdFail = socket(0, 0, 0);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, fdFail);
    fdSuccess = socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT_NOT_EQUAL(LWIP_TEST_FAIL, fdSuccess);

    ret = bind(fdFail, NULL, sizeof(struct sockaddr_in));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = bind(fdSuccess, NULL, sizeof(struct sockaddr_in));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = connect(fdFail, NULL, sizeof(struct sockaddr));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = connect(fdSuccess, NULL, sizeof(struct sockaddr));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = accept(fdFail, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = accept(fdSuccess, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = getsockname(fdFail, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = getsockname(fdSuccess, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = getpeername(fdFail, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = getpeername(fdSuccess, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = send(fdFail, NULL, strlen(SRV_MSG), 0);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = send(fdSuccess, NULL, strlen(SRV_MSG), 0);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = sendto(fdFail, NULL, strlen(SRV_MSG), 0, NULL, (socklen_t)sizeof(struct sockaddr_in));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = sendto(fdSuccess, NULL, strlen(SRV_MSG), 0, NULL, (socklen_t)sizeof(struct sockaddr_in));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = recv(fdFail, NULL, sizeof(SRV_MSG), 0);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = recv(fdSuccess, NULL, sizeof(SRV_MSG), 0);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = recvfrom(fdFail, NULL, sizeof(SRV_MSG), 0, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = recvfrom(fdSuccess, NULL, sizeof(SRV_MSG), 0, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = setsockopt(fdFail, SOL_SOCKET, SO_RCVTIMEO, NULL, (socklen_t)sizeof(struct timeval));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = setsockopt(fdSuccess, SOL_SOCKET, SO_RCVTIMEO, NULL, (socklen_t)sizeof(struct timeval));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = getsockopt(fdFail, SOL_SOCKET, SO_RCVTIMEO, NULL, (socklen_t*)sizeof(struct timeval));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = getsockopt(fdSuccess, SOL_SOCKET, SO_RCVTIMEO, NULL, (socklen_t*)sizeof(struct timeval));
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = sendmsg(fdFail, NULL, 0);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = sendmsg(fdSuccess, NULL, 0);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = listen(fdFail, 0);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = listen(fdSuccess, -1);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = select(fdFail, NULL, NULL, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = shutdown(fdFail, SHUT_RD);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);
    ret = shutdown(fdSuccess, -1);
    TEST_ASSERT_EQUAL_INT(LWIP_TEST_FAIL, ret);

    ret = lwip_close(fdSuccess);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

RUN_TEST_SUITE(LwipFuncTestSuite);
