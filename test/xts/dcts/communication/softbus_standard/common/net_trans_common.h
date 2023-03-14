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

#ifndef NET_TRANS_COMMON_H
#define NET_TRANS_COMMON_H

#include "common_list.h"
#include "discovery_service.h"
#include "session.h"
#include "softbus_bus_center.h"
#include "softbus_common.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_utils.h"

#include <pthread.h>
#include <securec.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEF_GROUP_ID "DEF_GROUP_ID"
#define DEF_SESSION_KEY "DEF_SESSION_KEY"
#define DEF_PKG_NAME "com.communication.demo1"
#define PKG_NAME_TWO "com.communication.demo2"
#define PKG_NAME_THREE "com.communication.demo3"
#define PKG_NAME_FOUR "com.communication.demo4"

#define SESSION_NAME_DATA "com.communication.demo1.data"
#define SESSION_NAME_CTL "com.communication.demo1.control"
#define SESSION_NAME_PASS "com.communication.demo1.passive"
#define SESSION_NAME_FAIL "com.communication.demo1.fail"
#define SESSION_NAME_PERF "com.communication.demo1.perf"
#define SESSION_NAME_PROXY "com.communication.demo1.proxy"
#define SESSION_NAME_FILE "com.communication.demo1.file"
#define DSOFT_PKG_NAME "com.plrdtest.dsoftbus"
#define SESSION_NAME_DSOFT_DATA "com.plrdtest.dsoftbus.data"
#define RECV_FILE_PATH "/data/"

#define CTRL_MSG_CLOSE_WIFI_THREE_SEC "close wifi 3sec"
#define CTRL_MSG_CLOSE_WIFI_TEN_MIN "close wifi 10min"
#define CTRL_MSG_CLOSE_WIFI_FIVE_MIN "close wifi 5min"
#define CTRL_MSG_CLOSE_WIFI_TEN_SEC "close wifi 10sec"
#define CTRL_MSG_CHANGE_WIFI_TEN_SEC "change wifi 10sec"
#define CTRL_MSG_CHANGE_WIFI_SIXTY_SEC "change wifi 60sec"
#define CTRL_MSG_CLOSE_WIFI_LOOP20 "close wifi loop 20"
#define CTRL_MSG_CHANGE_WIFI_LOOP20 "change wifi loop 20"
#define CTRL_MSG_CLOSE_WIFI_LOOP100 "close wifi loop 100"
#define CTRL_MSG_CHANGE_WIFI_LOOP100 "change wifi loop 100"

#define CTRL_MSG_OPEN_SESSION "open session"
#define CTRL_MSG_OPEN_SESSION_MSG "open session msg"
#define CTRL_MSG_OPEN_SESSION_NOT_EXIST "open session not exist"
#define CTRL_MSG_OPEN_SESSION_FAIL "open session fail"
#define CTRL_MSG_CLOSE_SESSION "close session"

#define CTRL_MSG_CLOSE_BR "close br"
#define CTRL_MSG_CLOSE_OPEN_BR "close-open br"
#define CTRL_MSG_CLOSE_AIR "close air"
#define CTRL_MSG_CLOSE_OPEN_AIR "close-open air"
#define CTRL_MSG_SEND_BIG_DATA "send big data"

#define WAIT_DEF_VALUE (-1)
#define WAIT_SUCCESS_VALUE 1
#define WAIT_FAIL_VALUE 0
#define SESSION_ID_MIN 1
#define MAX_SESSION_NUM 16
#define BOOL_TRUE 1
#define BOOL_FALSE 0
#define OPEN_SESSION_TIMEOUT 19
#define MULTI_REMOTE_DEV_COUNT 3
#define DEF_REMOTE_DEV_COUNT 1
#define SIZE_1K 1024

#define CMD_DOWN_ETH "ifconfig eth0 down"
#define CMD_CONFIG_ETH_WRONG_IP "ifconfig eth0 193.168.7.88 up"
#define CMD_CONFIG_ETH_RIGHT_IP "ifconfig eth0 192.168.3.88 up"
#define CMD_PIDOF_SOFTBUS "pidof softbus_server"

#define CMD_CONFIG_WLAN_WRONG_IP "ifconfig wlan0 193.168.3.88 up"

#define MAX_SESSION_SERVER_NUM_CLIENT 8
#define MAX_SESSION_SERVER_NUM_SERVER 32
#define TRANS_BYTES_LENGTH_MAX (4 * 1024 * 1024)
#define TRANS_MESSAGE_LENGTH_MAX 4096
#define TRANS_PROXY_BYTES_LENGTH_MAX (4 * 1024)
#define TRANS_PROXY_MESSAGE_LENGTH_MAX 1024
#define DEF_SEND_DATA_SID_COUNT 4
#define GROUP_ID_LEN 4

#define CASE_0 0
#define CASE_1 1
#define CASE_2 2
#define CASE_3 3
#define CASE_4 4
#define CASE_5 5
#define CASE_6 6
#define CASE_7 7
#define CASE_8 8
#define CASE_9 9
#define CASE_10 10



typedef enum {
    SESSION_4DATA = 1,
    SESSION_4CTL,
} WaitSessionType;

typedef enum {
    DATA_TYPE_MSG = 1,
    DATA_TYPE_BYTE,
} DataType;

typedef enum {
    STATE_ONLINE = 1,
    STATE_OFFLINE,
} WaitNodeStateType;

typedef enum {
    CONC_CLOSE_SESSION = 1,
    CONC_LEAVE_NET,
    CONC_DIS_NET_RECOVERY,
    CONC_DIS_NET_NOT_RECOVERY,
    CONC_DOWN_NET,
    CONC_SEND_DATA_ONLY,
    CONC_CLOSE_SESSION_REMOTE,
} ConcurrentType;

typedef enum {
    CTRL_CODE_CLOSE_WIFI_TEN_SEC = 1,
    CTRL_CODE_CLOSE_WIFI_TEN_MIN,
    CTRL_CODE_CLOSE_WIFI_FIVE_MIN,
    CTRL_CODE_CLOSE_WIFI_THREE_SEC,
    CTRL_CODE_CHANGE_WIFI_TEN_SEC,
    CTRL_CODE_CHANGE_WIFI_SIXTY_SEC,
    CTRL_CODE_CHANGE_WIFI_LOOP20,
    CTRL_CODE_CLOSE_WIFI_LOOP20,
    CTRL_CODE_CHANGE_WIFI_LOOP100,
    CTRL_CODE_CLOSE_WIFI_LOOP100,
    CTRL_CODE_OPEN_SESSION,
    CTRL_CODE_OPEN_SESSION_MSG,
    CTRL_CODE_OPEN_SESSION_FAIL,
    CTRL_CODE_OPEN_SESSION_NOT_EXIST,
    CTRL_CODE_CLOSE_SESSION,
    CTRL_CODE_CLOSE_BR,
    CTRL_CODE_CLOSE_OPEN_BR,
    CTRL_CODE_CLOSE_AIR,
    CTRL_CODE_CLOSE_OPEN_AIR,
    CTRL_CODE_SEND_BIG_DATA,
} CtrlCodeType;

#define LOG(format, ...)                                           \
    do {                                                           \
        time_t timeSec;                                            \
        time(&timeSec);                                            \
        struct tm tmRst;                                           \
        localtime_r(&timeSec, &tmRst);                             \
        char strTime[10];                                          \
        strftime(strTime, sizeof(strTime), "%H:%M:%S", &tmRst);    \
        fprintf(stdout, "[Test-softbus] %s " format "\n", strTime, \
                ##__VA_ARGS__);                                    \
    } while (0)

int Wait(int timeout);
int Wait4Session(int timeout, WaitSessionType type);
int WaitNodeCount(int timeout, WaitNodeStateType state, int expectCount);

void TestSetUp(void);
void TestTearDown(void);

int StartDiscoveryDevice(void);

int JoinNetwork(void);
int LeaveNetWork(void);
int RegisterDeviceStateDefCallback(void);
int UnRegisterDeviceStateDefCallback(void);
int DiscoverAndJoinNetwork(void);
int CheckRemoteDeviceIsNull(int isSetNetId);
int GetRemoteDeviceNetId(char** netId);
int SetCurrentNetworkId(int index);
int SetRemoteDeviceNetIdToGarray(void);

uint64_t GetCurrentTimeOfMs(void);

void ResetWaitFlag(void);
void ResetWaitFlag4Data(void);
void ResetWaitFlag4Ctl(void);
void ResetWaitCount4Offline(void);
void ResetWaitCount4Online(void);

int SendData4Data(DataType type, int size);
int SendCtrlMsgToRemote(CtrlCodeType code);

int CreateSsAndOpenSession4Data(void);
int CreateSsAndOpenSession4Ctl(void);
int OpenSession4Ctl(void);
int OpenSession4Data(void);
int OpenSessionBatch4Data(char groupId[][GROUP_ID_LEN],
                          int* sessionId,
                          int count);
int OpenSessionBatch4Ctl(char groupId[][GROUP_ID_LEN],
                         int* sessionId,
                         int count);
int CloseSessionBatch4Data(int* sessionId, int count);
int CloseSessionBatch4Ctl(int* sessionId, int count);
void* OpenSessionTask4Data(void* param);
void* OpenSessionTask4Ctl(void* param);

int OpenSession4Perf(void);
int OpenSession4PerfWithParam(const char* sessionName,
                              const char* groupId,
                              char* netId);
int SendData4Perf(int sessionId, char* dataMsg, char* dataByte);
void SetTransStartTime(void);

void* SendDataTask1(void* param);
void* SendDataTask2(void* param);
void* SendDataTask3(void* param);

char* GetSoftbusPid(void);
char* GetNetworkId(void);
int GetCurrentSessionId4Data(void);
int GetCurrentSessionId4Ctl(void);
void SetCurrentSessionId4Data(int sessionId);
void SetCurrentSessionId4Ctl(int sessionId);
ISessionListener* GetSessionListenser4Data(void);
ISessionListener* GetSessionListenser4Ctl(void);
ISessionListener* GetSessionListenser4Pass(void);
ISessionListener* GetSessionListenser4Perf(void);
ISessionListener* GetSessionListenser4Proxy(void);
SessionAttribute* GetSessionAttr4Pass(void);
SessionAttribute* GetSessionAttr4Ctl(void);
SessionAttribute* GetSessionAttr4Data(void);
SessionAttribute* GetSessionAttr4Perf(void);
SessionAttribute* GetSessionAttr4Proxy(void);
IFileReceiveListener* GetRecvFileListener(void);
IFileSendListener* GetSendFileListener(void);

pthread_barrier_t* GetThreadBarrier(void);
int GetThreadResult4Data(void);
int GetThreadResult4Ctl(void);
int* GetSid4Task2(void);
int* GetSid4Task3(void);
void ResetMsgStat4Control(void);
void ResetByteStat4Control(void);
ConnectionAddr* GetConnectAddr(void);

void ResetClosedSessionCount4Data(void);
void ResetClosedSessionCount4Ctrl(void);
int GetClosedSessionCount4Data(void);
int GetClosedSessionCount4Ctrl(void);
void ResetOpenSessionCount4Data(void);
void ResetOpenSessionCount4Ctrl(void);
int GetOpenSessionCount4Data(void);
int GetOpenSessionCount4Ctrl(void);

int CloseSessionAndRemoveSs4Data(void);
int CloseSessionAndRemoveSs4Ctl(void);

#ifdef __cplusplus
}
#endif
#endif
