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

#include "net_trans_common.h"

static int32_t g_currentSessionId4Data = -1;
static int32_t g_currentSessionId4Ctl = -1;

static int32_t g_waitFlag = WAIT_DEF_VALUE;
static int32_t g_waitFlag4Data = WAIT_DEF_VALUE;
static int32_t g_waitFlag4Ctl = WAIT_DEF_VALUE;
static int32_t g_passiveOpenRetFlag = SOFTBUS_OK;
static int32_t g_nodeOnlineCount = 0;
static int32_t g_nodeOfflineCount = 0;

static int32_t g_msgCount4Data = 0;
static int32_t g_byteCount4Data = 0;
static int32_t g_sessionCloseCount4Data = 0;
static int32_t g_sessionCloseCount4Ctrl = 0;
static int32_t g_sessionOpenCount4Data = 0;
static int32_t g_sessionOpenCount4Ctrl = 0;

static SessionAttribute* g_sessionAttr4Data = NULL;
static SessionAttribute* g_sessionAttr4Ctl = NULL;
static SessionAttribute* g_sessionAttr4Pass = NULL;
static SessionAttribute* g_sessionAttr4Perf = NULL;
static SessionAttribute* g_sessionAttr4Proxy = NULL;
static ISessionListener* g_sessionlistener4Data = NULL;
static ISessionListener* g_sessionlistener4Ctl = NULL;
static ISessionListener* g_sessionlistener4Pass = NULL;
static ISessionListener* g_sessionlistener4Perf = NULL;
static ISessionListener* g_sessionlistener4Proxy = NULL;

static IFileSendListener* g_fileSendListener = NULL;
static IFileReceiveListener* g_fileRecvListener = NULL;

static char g_networkId[NETWORK_ID_BUF_LEN] = {0};
static char g_networkIdArray[MULTI_REMOTE_DEV_COUNT][NETWORK_ID_BUF_LEN];
static INodeStateCb g_defNodeStateCallback;
static ConnectionAddr g_ethAddr = {
    .type = CONNECTION_ADDR_WLAN,
};

static int* g_sId4Task2;
static int* g_sId4Task3;
static char g_fillContentChar = 'd';
static unsigned int g_expectDataSize = 0;
static char* g_expectDataContent = NULL;
static pthread_barrier_t* g_barrier = NULL;

static int32_t g_threadResult4Data = SOFTBUS_ERR;
static int32_t g_threadResult4Ctl = SOFTBUS_ERR;
static int32_t g_recvMsgStat4Control[MAX_SESSION_NUM] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int32_t g_recvByteStat4Control[MAX_SESSION_NUM] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static uint64_t g_joinNetTimeStart;
static uint64_t g_joinNetTimeEnd;
static uint64_t g_leaveNetTimeStart;
static uint64_t g_leaveNetTimeEnd;
static uint64_t g_discoverTimeStart;
static uint64_t g_discoverTimeEnd;
static uint64_t g_openSessionTimeStart;
static uint64_t g_openSessionTimeEnd;
static uint64_t g_transTimeStart;
static uint64_t g_transTimeEnd;

/* discovery */
static int g_subscribeId = 0;
static IDiscoveryCallback g_defDiscCallback;
static SubscribeInfo g_subInfo = {
    .medium = COAP,
    .mode = DISCOVER_MODE_ACTIVE,
    .freq = MID,
    .capability = "ddmpCapability",
    .capabilityData = (unsigned char*)"cap data",
    .dataLen = sizeof("cap data"),
    .isSameAccount = true,
    .isWakeRemote = false,
};

int Wait(int timeout)
{
    LOG("start wait,timeout:%d", timeout);
    int count = 0;
    int t = timeout;
    while (t > 0) {
        sleep(1);
        if (g_waitFlag != WAIT_DEF_VALUE) {
            LOG("Wait success[flag:%d] [time:%d]", g_waitFlag, count);
            break;
        }
        t--;
        count++;
    }
    if (g_waitFlag != WAIT_SUCCESS_VALUE) {
        LOG("Wait fail[exp:%d, real:%d][used time:%d]", WAIT_SUCCESS_VALUE,
            g_waitFlag, count);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int WaitNodeCount(int timeout, WaitNodeStateType state, int expectCount)
{
    LOG("Wait4Node,timeout:%d, type:%d, exp count:%d", timeout, state,
        expectCount);
    int hitFlag = -1;
    int t = timeout;
    while (t > 0) {
        sleep(1);
        switch (state) {
            case STATE_ONLINE:
                if (g_nodeOnlineCount == expectCount) {
                    LOG("Wait4Node[online] succ,timeout:%d", t);
                    hitFlag = 1;
                }
                break;
            case STATE_OFFLINE:
                if (g_nodeOfflineCount == expectCount) {
                    LOG("Wait4Node[offline] succ,timeout:%d", t);
                    hitFlag = 1;
                }
                break;
            default:
                LOG("Wait4Node state error");
                hitFlag = 1;
                break;
        }
        if (hitFlag != -1) {
            break;
        }
        t--;
    }
    switch (state) {
        case STATE_ONLINE:
            if (g_nodeOnlineCount != expectCount) {
                LOG("Wait4Node[online] fail[exp:%d, real:%d]", expectCount,
                    g_nodeOnlineCount);
                return SOFTBUS_ERR;
            }
            break;
        case STATE_OFFLINE:
            if (g_nodeOfflineCount != expectCount) {
                LOG("Wait4Node[offline] fail[exp:%d, real:%d]", expectCount,
                    g_nodeOfflineCount);
                return SOFTBUS_ERR;
            }
            break;
        default:
            return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int Wait4Session(int timeout, WaitSessionType type)
{
    int hitFlag = -1;
    int t = timeout;
    while (t > 0) {
        sleep(1);
        switch (type) {
            case SESSION_4CTL:
                if (g_waitFlag4Ctl != WAIT_DEF_VALUE) {
                    LOG("Wait4Session[ctrl] succ, flag:%d", g_waitFlag4Ctl);
                    hitFlag = 1;
                }
                break;
            case SESSION_4DATA:
                if (g_waitFlag4Data != WAIT_DEF_VALUE) {
                    LOG("Wait4Session[data] succ,flag:%d", g_waitFlag4Data);
                    hitFlag = 1;
                }
                break;
            default:
                LOG("Wait4Session type error");
                hitFlag = 1;
                break;
        }
        if (hitFlag != -1) {
            break;
        }
        t--;
    }
    switch (type) {
        case SESSION_4CTL:
            if (g_waitFlag4Ctl != WAIT_SUCCESS_VALUE) {
                LOG("Wait4Session[ctrl] fail[exp:%d, real:%d]",
                    WAIT_SUCCESS_VALUE, g_waitFlag4Ctl);
                return SOFTBUS_ERR;
            }
            break;
        case SESSION_4DATA:
            if (g_waitFlag4Data != WAIT_SUCCESS_VALUE) {
                LOG("Wait4Session[data] fail[exp:%d, real:%d]",
                    WAIT_SUCCESS_VALUE, g_waitFlag4Data);
                return SOFTBUS_ERR;
            }
            break;
        default:
            return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

uint64_t GetCurrentTimeOfMs(void)
{
    int unit = 1000;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * unit + tv.tv_usec / unit);
}

char* GetSoftbusPid(void)
{
    FILE* file = NULL;
    int buffSize = 20;
    char* buffer = (char*)malloc(buffSize);
    if (buffer == NULL) {
        LOG("malloc fail");
        return NULL;
    }

    file = popen(CMD_PIDOF_SOFTBUS, "r");
    if (file == NULL) {
        LOG("call popen fail");
        free(buffer);
        return NULL;
    }
    (void)fgets(buffer, buffSize, file);
    LOG("##################Softbus PID:%s", buffer);
    pclose(file);

    return buffer;
}

static int IncrementSubId(void)
{
    g_subscribeId++;
    return g_subscribeId;
}

static int OnReceiveFileStarted(int sessionId, const char* files, int fileCnt)
{
    LOG("[recv file]start,sid:%d, fileCnt:%d", sessionId, fileCnt);
    return 0;
}

static int OnReceiveFileProcess(int sessionId,
                                const char* firstFile,
                                uint64_t bytesUpload,
                                uint64_t bytesTotal)
{
    return 0;
}

static void OnReceiveFileFinished(int sessionId,
                                  const char* files,
                                  int fileCnt)
{
    LOG("[recv file]finish,sid:%d, fileCnt:%d", sessionId, fileCnt);
}

static void OnRecvFileTransError(int sessionId)
{
    LOG("[recv file]trans error,sid:%d", sessionId);
}

static int OnSendFileProcess(int sessionId,
                             uint64_t bytesUpload,
                             uint64_t bytesTotal)
{
    return 0;
}

static int OnSendFileFinished(int sessionId, const char* firstFile)
{
    g_waitFlag = WAIT_SUCCESS_VALUE;
    LOG("[send file]finish,sid:%d, firstFile:%s\n", sessionId, firstFile);
    return 0;
}

static void OnSendFileTransError(int sessionId)
{
    g_waitFlag = WAIT_FAIL_VALUE;
    LOG("[send file]trans error,sid = %d\n", sessionId);
}

static void OnDefDeviceFound(const DeviceInfo* device)
{
    g_discoverTimeEnd = GetCurrentTimeOfMs();

    if (device == NULL) {
        LOG("[cb]device found,but is null");
        g_waitFlag = WAIT_FAIL_VALUE;
        return;
    }

    uint16_t port = device->addr[0].info.ip.port;
    char ipTmp[IP_STR_MAX_LEN];
    if (strncpy_s(ipTmp, IP_STR_MAX_LEN, device->addr[0].info.ip.ip,
                  strlen(device->addr[0].info.ip.ip)) != 0) {
        LOG("[cb]device found, strncpy_s ipTmp fail");
        return;
    }
    LOG("[cb]device found, type:%d", device->addr[0].type);
    LOG("[cb]device found, addr:%s, prot:%d", ipTmp, port);

    g_ethAddr.info.ip.port = port;
    if (strncpy_s(g_ethAddr.info.ip.ip, IP_STR_MAX_LEN, ipTmp, strlen(ipTmp)) !=
        0) {
        LOG("[cb]device found, strncpy_s ip fail");
        g_waitFlag = WAIT_FAIL_VALUE;
        return;
    }

    g_waitFlag = WAIT_SUCCESS_VALUE;
}

static void OnDefDiscoverFail(int subscribeId, DiscoveryFailReason failReason)
{
    LOG("[cb]discover fail, sub id:%d, reason:%d", subscribeId, failReason);
}

static void OnDefDiscoverSuccess(int subscribeId)
{
    LOG("[cb]discover success, sub id:%d", subscribeId);
}

static void OnDefNodeOnline(NodeBasicInfo* info)
{
    if (info == NULL) {
        LOG("[cb]Online: info is null");
        return;
    }
    (void)strncpy_s(g_networkId, NETWORK_ID_BUF_LEN, info->networkId,
                    NETWORK_ID_BUF_LEN);
    LOG("[cb]Online id:%s, name:%s, type id:%u", info->networkId,
        info->deviceName, info->deviceTypeId);
    g_nodeOnlineCount++;
}

static void OnDefNodeOffline(NodeBasicInfo* info)
{
    if (info == NULL) {
        LOG("[cb]Offline: info is null");
        return;
    }
    LOG("[cb]Offline id:%s, name:%s, type id:%u", info->networkId,
        info->deviceName, info->deviceTypeId);
    g_nodeOfflineCount++;
}

static void OnDefNodeBasicInfoChanged(NodeBasicInfoType type,
                                      NodeBasicInfo* info)
{
    if (info == NULL) {
        LOG("[cb]InfoChanged: info is null, type[%d]", type);
        return;
    }
    LOG("[cb]InfoChanged id: %s,name: %s", info->networkId, info->deviceName);
}

static void OnJoinNetCallBack(ConnectionAddr* addr,
                              const char* networkId,
                              int32_t retCode)
{
    g_joinNetTimeEnd = GetCurrentTimeOfMs();

    if (networkId == NULL || retCode != SOFTBUS_OK) {
        LOG("[cb]JoinNet netid/ret error, ret:%d", retCode);
        g_waitFlag = WAIT_FAIL_VALUE;
        return;
    }

    LOG("[cb]JoinNet, ret:%d, id:%s", retCode, networkId);
    switch (addr->type) {
        case CONNECTION_ADDR_WLAN:
            LOG("[cb]JoinNet type is [WLAN]");
            break;
        case CONNECTION_ADDR_BR:
            LOG("[cb]JoinNet type is [BR]");
            break;
        case CONNECTION_ADDR_ETH:
            LOG("[cb]JoinNet type is [ETH]");
            break;
        default:
            LOG("[cb]JoinNet type error");
            g_waitFlag = WAIT_FAIL_VALUE;
            return;
    }

    int ret = strncpy_s(g_networkId, NETWORK_ID_BUF_LEN, networkId, NETWORK_ID_BUF_LEN);
    if (ret != SOFTBUS_OK)
    {
        LOG("[cb]strncpy_s fail");
        return;
    }
    
    g_waitFlag = WAIT_SUCCESS_VALUE;
}

static void OnLeaveNetCallBack(const char* networkId, int32_t ret)
{
    g_leaveNetTimeEnd = GetCurrentTimeOfMs();

    LOG("[cb]LeaveLNN ret[%d]", ret);
    if (ret == SOFTBUS_OK) {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    } else {
        g_waitFlag = WAIT_FAIL_VALUE;
    }
}

static int DataSessionOpened(int sessionId, int result)
{
    // wait 1s, ensure set current session id
    sleep(1);
    g_sessionOpenCount4Data++;
    if (result == SOFTBUS_OK) {
        if (sessionId == g_currentSessionId4Data) {
            LOG("[cb][data]open session check success sid[%d]", sessionId);
            g_waitFlag4Data = WAIT_SUCCESS_VALUE;
        } else {
            LOG("[cb][data]open session callback sid[%d] not match open "
                "sid[%d]",
                sessionId, g_currentSessionId4Data);
            g_waitFlag4Data = WAIT_FAIL_VALUE;
        }
    } else {
        g_waitFlag4Data = WAIT_FAIL_VALUE;
    }
    return SOFTBUS_OK;
}

static void DataSessionClosed(int sessionId)
{
    g_sessionCloseCount4Data++;
    if (sessionId == g_currentSessionId4Data) {
        LOG("[cb][data]closed session,check sid[%d] success", sessionId);
        g_currentSessionId4Data = -1;
        g_waitFlag4Data = WAIT_SUCCESS_VALUE;
    } else {
        LOG("[cb][data]closed session, callback sid[%d] not match open sid[%d]",
            sessionId, g_currentSessionId4Data);
        g_waitFlag4Data = WAIT_FAIL_VALUE;
    }
}

static void CheckReceiveData(int sessionId,
                             const void* data,
                             unsigned int dataLen)
{
    // check session id
    if (sessionId == g_currentSessionId4Data && dataLen == g_expectDataSize) {
        LOG("[check]sid/size ok[sid:%d,size:%u]", sessionId, dataLen);
    } else {
        LOG("[check]sid/size fail[sid exp:%d,real:%d][size exp:%u,real:%u]",
            g_currentSessionId4Data, sessionId, g_expectDataSize, dataLen);
        g_waitFlag = WAIT_FAIL_VALUE;
        return;
    }

    // check data content
    int cmpRst = strncmp(g_expectDataContent, (char*)data, dataLen);
    if (cmpRst == 0) {
        LOG("[check] cmp content ok");
        g_waitFlag = WAIT_SUCCESS_VALUE;
    } else {
        LOG("[check] cmp content fail[exp:%s,real:%s]", g_expectDataContent,
            (char*)data);
        g_waitFlag = WAIT_FAIL_VALUE;
    }
}

static void DataBytesReceived(int sessionId,
                              const void* data,
                              unsigned int dataLen)
{
    LOG("[cb][data]ByteRec start");
    CheckReceiveData(sessionId, data, dataLen);
    g_byteCount4Data++;
    LOG("[cb][data]ByteRec end");
}

static void DataMessageReceived(int sessionId,
                                const void* data,
                                unsigned int dataLen)
{
    LOG("[cb][data]MessageRec start");
    CheckReceiveData(sessionId, data, dataLen);
    g_msgCount4Data++;
    LOG("[cb][data]MessageRec end");
}

static int ControlSessionOpened(int sessionId, int result)
{
    // wait 1s, ensure set current session id
    sleep(1);
    g_sessionOpenCount4Ctrl++;
    if (result == SOFTBUS_OK) {
        if (sessionId == g_currentSessionId4Ctl) {
            LOG("[cb][ctrl]open session check success sid[%d]", sessionId);
            g_waitFlag4Ctl = WAIT_SUCCESS_VALUE;
        } else {
            LOG("[cb][ctrl]open session callback sid[%d] not match open "
                "sid[%d]",
                sessionId, g_currentSessionId4Ctl);
            g_waitFlag4Ctl = WAIT_FAIL_VALUE;
        }
    } else {
        g_waitFlag4Ctl = WAIT_FAIL_VALUE;
    }
    return SOFTBUS_OK;
}

static void ControlSessionClosed(int sessionId)
{
    g_sessionCloseCount4Ctrl++;
    if (sessionId == g_currentSessionId4Ctl) {
        LOG("[cb][ctrl]closed session check sid[%d] success", sessionId);
        g_currentSessionId4Ctl = -1;
        g_waitFlag4Ctl = WAIT_SUCCESS_VALUE;
    } else {
        LOG("[cb][ctrl]closed session callback sid[%d] not match open sid[%d]",
            sessionId, g_currentSessionId4Ctl);
        g_waitFlag4Ctl = WAIT_FAIL_VALUE;
    }
}

static void ControlBytesReceived(int sessionId,
                                 const void* data,
                                 unsigned int dataLen)
{
    LOG("[cb][ctrl]ByteRec sid:%d, data len:%u", sessionId, dataLen);
    if (sessionId < 0 || sessionId > MAX_SESSION_NUM) {
        LOG("[cb][ctrl]ByteRec invalid session sid[%d]", sessionId);
        return;
    }
    if (data == NULL) {
        LOG("[cb][ctrl]ByteRec invalid data=null sid[%d]", sessionId);
        return;
    }
    g_recvByteStat4Control[sessionId]++;
}

static void ControlMessageReceived(int sessionId,
                                   const void* data,
                                   unsigned int dataLen)
{
    LOG("[cb][ctrl]MessageRec sid:%d, data len:%u", sessionId, dataLen);
    if (sessionId < 0 || sessionId > MAX_SESSION_NUM) {
        LOG("[cb][ctrl]MessageRec invalid session sid[%d]", sessionId);
        return;
    }
    if (data == NULL) {
        LOG("[cb][ctrl]MessageRec invalid data=null sid[%d]", sessionId);
        return;
    }
    g_recvMsgStat4Control[sessionId]++;
}

static int PassiveSessionOpened(int sessionId, int result)
{
    LOG("[cb][pass]open session passive sid[%d],rst[%d]", sessionId, result);
    if (result == SOFTBUS_OK) {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    } else {
        g_waitFlag = WAIT_FAIL_VALUE;
    }
    if (g_passiveOpenRetFlag == SOFTBUS_ERR) {
        LOG("[cb][pass]return fail");
        g_passiveOpenRetFlag = SOFTBUS_OK;
        return SOFTBUS_ERR;
    } else {
        return SOFTBUS_OK;
    }
}

static void PassiveSessionClosed(int sessionId)
{
    LOG("[cb][pass]close session passive sid[%d]", sessionId);
    g_waitFlag = WAIT_SUCCESS_VALUE;
}

static void PassiveBytesReceived(int sessionId,
                                 const void* data,
                                 unsigned int dataLen)
{
    LOG("[cb][pass]ByteRec sid:%d, data len:%u", sessionId, dataLen);
    if (data == NULL) {
        LOG("[cb][pass]ByteRec invalid data=null sid[%d]", sessionId);
        g_waitFlag = WAIT_FAIL_VALUE;
        return;
    }
    int ret = SendBytes(sessionId, data, dataLen);
    LOG("[cb][pass]ByteRec send back ret[%d]", ret);
    if (ret == SOFTBUS_OK) {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    } else {
        g_waitFlag = WAIT_FAIL_VALUE;
    }
}

static void PassiveMessageReceived(int sessionId,
                                   const void* data,
                                   unsigned int dataLen)
{
    LOG("[cb][pass]MessageRec sid:%d, data len:%u", sessionId, dataLen);
    if (data == NULL) {
        LOG("[cb][pass]MessageRec invalid data=null sid[%d]", sessionId);
        g_waitFlag = WAIT_FAIL_VALUE;
        return;
    }
    int ret = SendMessage(sessionId, data, dataLen);
    LOG("[cb][pass]MessageRec send back ret[%d]", ret);
    if (ret == SOFTBUS_OK) {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    } else {
        g_waitFlag = WAIT_FAIL_VALUE;
    }
}

static int PerfSessionOpened(int sessionId, int result)
{
    g_openSessionTimeEnd = GetCurrentTimeOfMs();

    if (result == SOFTBUS_OK) {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    } else {
        LOG("[cb][perf]open session result fail, ret:%d", result);
        g_waitFlag = WAIT_FAIL_VALUE;
    }
    return SOFTBUS_OK;
}

static void PerfSessionClosed(int sessionId)
{
    LOG("[cb][perf]closed session,sid[%d]", sessionId);
}

static void PerfBytesReceived(int sessionId,
                              const void* data,
                              unsigned int dataLen)
{
    g_transTimeEnd = GetCurrentTimeOfMs();

    LOG("[cb][perf]Byte recv");
    g_waitFlag = WAIT_SUCCESS_VALUE;
}

static void PerfMessageReceived(int sessionId,
                                const void* data,
                                unsigned int dataLen)
{
    LOG("[cb][perf]Message recv");
}

static int ProxySessionOpened(int sessionId, int result)
{
    LOG("[cb][proxy]open session proxy sid[%d],rst[%d]", sessionId, result);
    if (result == SOFTBUS_OK) {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    } else {
        g_waitFlag = WAIT_FAIL_VALUE;
    }
    return SOFTBUS_OK;
}

static void ProxySessionClosed(int sessionId)
{
    LOG("[cb][proxy]close session proxy sid[%d]", sessionId);
    g_waitFlag = WAIT_SUCCESS_VALUE;
}

static void ProxyBytesReceived(int sessionId,
                               const void* data,
                               unsigned int dataLen)
{
    LOG("[cb][proxy]ByteRec sid:%d, data len:%u", sessionId, dataLen);
    if (data == NULL) {
        LOG("[cb][proxy]ByteRec invalid data=null sid:[%d]", sessionId);
        g_waitFlag = WAIT_FAIL_VALUE;
    } else {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    }
}

static void ProxyMessageReceived(int sessionId,
                                 const void* data,
                                 unsigned int dataLen)
{
    LOG("[cb][proxy]MessageRec sid:%d, data len:%u", sessionId, dataLen);
    if (data == NULL) {
        LOG("[cb][proxy]MessageRec invalid data=null sid[%d]", sessionId);
        g_waitFlag = WAIT_FAIL_VALUE;
    } else {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    }
}

void ResetWaitFlag(void)
{
    g_waitFlag = WAIT_DEF_VALUE;
}

void ResetWaitFlag4Data(void)
{
    g_waitFlag4Data = WAIT_DEF_VALUE;
}

void ResetWaitFlag4Ctl(void)
{
    g_waitFlag4Ctl = WAIT_DEF_VALUE;
}

void ResetWaitCount4Offline(void)
{
    g_nodeOfflineCount = 0;
}

void ResetWaitCount4Online(void)
{
    g_nodeOnlineCount = 0;
}

int StartDiscoveryDevice(void)
{
    int ret;
    int timeout = 10;
    ResetWaitFlag();
    g_subInfo.subscribeId = IncrementSubId();
    LOG("subscribeId:%d", g_subInfo.subscribeId);
    g_discoverTimeStart = GetCurrentTimeOfMs();
    ret = StartDiscovery(DEF_PKG_NAME, &g_subInfo, &g_defDiscCallback);
    if (ret != SOFTBUS_OK) {
        LOG("call StartDiscovery fail, ret:%d", ret);
        return ret;
    }
    ret = Wait(timeout);
    if (ret != SOFTBUS_OK) {
        LOG("discover device fail");
    }
    return ret;
}

int JoinNetwork(void)
{
    int ret;
    int timeout = 15;
    ResetWaitFlag();
    g_joinNetTimeStart = GetCurrentTimeOfMs();
    ret = JoinLNN(DEF_PKG_NAME, &g_ethAddr, OnJoinNetCallBack);
    if (ret != SOFTBUS_OK) {
        LOG("call JoinLNN fail, ret:%d", ret);
        return ret;
    }
    ret = Wait(timeout);
    if (ret != SOFTBUS_OK) {
        LOG("JoinNetwork fail");
    }
    return ret;
}

int JoinNetworkByAddr(ConnectionAddr* addr)
{
    int ret;
    int timeout = 15;
    ResetWaitFlag();
    LOG("JoinNetwork[input addr]ip:%s, port:%d", addr->info.ip.ip,
        addr->info.ip.port);
    g_joinNetTimeStart = GetCurrentTimeOfMs();
    ret = JoinLNN(DEF_PKG_NAME, addr, OnJoinNetCallBack);
    if (ret != SOFTBUS_OK) {
        LOG("call JoinLNN fail[input addr], ret:%d", ret);
        return ret;
    }
    ret = Wait(timeout);
    if (ret != SOFTBUS_OK) {
        LOG("JoinNetwork[input addr] fail");
    } else {
        LOG("JoinNetwork[input addr] success");
    }
    return ret;
}

int DiscoverAndJoinNetwork(void)
{
    int ret = StartDiscoveryDevice();
    int retStop = StopDiscovery(DEF_PKG_NAME, g_subscribeId);
    if (retStop != SOFTBUS_OK) {
        LOG("call StopDiscovery fail, ret:%d", ret);
    }
    if (ret != SOFTBUS_OK) {
        return ret;
    }
    sleep(1);

    return JoinNetwork();
}

int LeaveNetWork(void)
{
    int ret;
    int timeout = 10;

    ret = CheckRemoteDeviceIsNull(BOOL_TRUE);
    if (ret != SOFTBUS_OK) {
        return ret;
    }

    ResetWaitFlag();
    g_leaveNetTimeStart = GetCurrentTimeOfMs();
    ret = LeaveLNN(DEF_PKG_NAME, g_networkId, OnLeaveNetCallBack);
    if (ret != SOFTBUS_OK) {
        LOG("call LeaveLNN fail, ret:%d", ret);
        return ret;
    }
    ret = Wait(timeout);
    if (ret != SOFTBUS_OK) {
        LOG("LeaveNetwork fail");
    }

    return ret;
}

int RegisterDeviceStateDefCallback(void)
{
    return RegNodeDeviceStateCb(DEF_PKG_NAME, &g_defNodeStateCallback);
}

int UnRegisterDeviceStateDefCallback(void)
{
    return UnregNodeDeviceStateCb(&g_defNodeStateCallback);
}

int CreateSsAndOpenSession4Data(void)
{
    int ret;
    int timeout = 10;
    ret = CreateSessionServer(DEF_PKG_NAME, SESSION_NAME_DATA,
                              g_sessionlistener4Data);
    if (ret != SOFTBUS_OK) {
        LOG("call CreateSessionServer[data] fail, ret:%d", ret);
        return ret;
    }

    ResetWaitFlag4Data();
    int sessionId;
    sessionId = OpenSession(SESSION_NAME_DATA, SESSION_NAME_DATA, g_networkId,
                            DEF_GROUP_ID, g_sessionAttr4Data);
    if (sessionId < SESSION_ID_MIN) {
        LOG("call OpenSession[data] fail, ret sid:%d,netId:%s", sessionId,
            g_networkId);
        ret = RemoveSessionServer(DEF_PKG_NAME, SESSION_NAME_DATA);
        if (ret != SOFTBUS_OK) {
            LOG("RemoveSessionServer[data] fail, ret:%d", ret);
        }
        return SOFTBUS_ERR;
    }
    SetCurrentSessionId4Data(sessionId);
    LOG("call OpenSession[data] success,sid:%d", sessionId);

    ret = Wait4Session(timeout, SESSION_4DATA);
    if (ret != SOFTBUS_OK) {
        LOG("OpenSession[data] fail");
        ret = RemoveSessionServer(DEF_PKG_NAME, SESSION_NAME_DATA);
        if (ret != SOFTBUS_OK) {
            LOG("RemoveSessionServer[data] fail, ret:%d", ret);
        }
        return SOFTBUS_ERR;
    }
    return ret;
}

int OpenSession4Data(void)
{
    int ret;
    int sessionId;
    int timeout = 10;
    ResetWaitFlag4Data();
    sessionId = OpenSession(SESSION_NAME_DATA, SESSION_NAME_DATA, g_networkId,
                            DEF_GROUP_ID, g_sessionAttr4Data);
    if (sessionId < SESSION_ID_MIN) {
        LOG("call OpenSession[data] fail, ret sid:%d,netId:%s", sessionId,
            g_networkId);
        return SOFTBUS_ERR;
    }
    SetCurrentSessionId4Data(sessionId);
    LOG("call OpenSession[data] success,sid:%d", sessionId);
    ret = Wait4Session(timeout, SESSION_4DATA);
    if (ret != SOFTBUS_OK) {
        LOG("OpenSession[data] fail");
        return SOFTBUS_ERR;
    }
    return ret;
}

int CreateSsAndOpenSession4Ctl(void)
{
    int ret;
    int timeout = 10;
    ret = CreateSessionServer(DEF_PKG_NAME, SESSION_NAME_CTL,
                              g_sessionlistener4Ctl);
    if (ret != SOFTBUS_OK) {
        LOG("call CreateSessionServer[ctl] fail, ret:%d", ret);
        return ret;
    }

    int sessionId;
    ResetWaitFlag4Ctl();
    sessionId = OpenSession(SESSION_NAME_CTL, SESSION_NAME_CTL, g_networkId,
                            DEF_GROUP_ID, g_sessionAttr4Ctl);
    if (sessionId < SESSION_ID_MIN) {
        LOG("call OpenSession[ctl] fail, ret sid:%d", sessionId);
        ret = RemoveSessionServer(DEF_PKG_NAME, SESSION_NAME_CTL);
        if (ret != SOFTBUS_OK) {
            LOG("RemoveSessionServer[ctl] fail, ret:%d", ret);
        }
        return SOFTBUS_ERR;
    }
    SetCurrentSessionId4Ctl(sessionId);
    LOG("call OpenSession[ctl] success,sid:%d", sessionId);

    ret = Wait4Session(timeout, SESSION_4CTL);
    if (ret != SOFTBUS_OK) {
        LOG("OpenSession[ctl] fail");
        ret = RemoveSessionServer(DEF_PKG_NAME, SESSION_NAME_CTL);
        if (ret != SOFTBUS_OK) {
            LOG("RemoveSessionServer[ctl] fail, ret:%d", ret);
        }
        return SOFTBUS_ERR;
    }
    return ret;
}

int OpenSession4Ctl(void)
{
    int ret;
    int timeout = 10;
    int sessionId;
    ResetWaitFlag4Ctl();
    sessionId = OpenSession(SESSION_NAME_CTL, SESSION_NAME_CTL, g_networkId,
                            DEF_GROUP_ID, g_sessionAttr4Ctl);
    if (sessionId < SESSION_ID_MIN) {
        LOG("call OpenSession[ctl] fail, ret sid:%d", sessionId);
        return SOFTBUS_ERR;
    }
    SetCurrentSessionId4Ctl(sessionId);
    LOG("call OpenSession[ctl] success,sid:%d", sessionId);

    ret = Wait4Session(timeout, SESSION_4CTL);
    if (ret != SOFTBUS_OK) {
        LOG("OpenSession[ctl] fail");
        return SOFTBUS_ERR;
    }
    return ret;
}

int SendCtrlMsgToRemote(CtrlCodeType code)
{
    int ret = -1;
    switch (code) {
        case CTRL_CODE_CLOSE_WIFI_TEN_SEC:
            ret =
                SendMessage(g_currentSessionId4Ctl, CTRL_MSG_CLOSE_WIFI_TEN_SEC,
                            strlen(CTRL_MSG_CLOSE_WIFI_TEN_SEC));
            break;
        case CTRL_CODE_CLOSE_WIFI_THREE_SEC:
            ret = SendMessage(g_currentSessionId4Ctl,
                              CTRL_MSG_CLOSE_WIFI_THREE_SEC,
                              strlen(CTRL_MSG_CLOSE_WIFI_THREE_SEC));
            break;
        case CTRL_CODE_CLOSE_WIFI_TEN_MIN:
            ret =
                SendMessage(g_currentSessionId4Ctl, CTRL_MSG_CLOSE_WIFI_TEN_MIN,
                            strlen(CTRL_MSG_CLOSE_WIFI_TEN_MIN));
            break;
        case CTRL_CODE_CLOSE_WIFI_FIVE_MIN:
            ret = SendMessage(g_currentSessionId4Ctl,
                              CTRL_MSG_CLOSE_WIFI_FIVE_MIN,
                              strlen(CTRL_MSG_CLOSE_WIFI_FIVE_MIN));
            break;
        case CTRL_CODE_CHANGE_WIFI_TEN_SEC:
            ret = SendMessage(g_currentSessionId4Ctl,
                              CTRL_MSG_CHANGE_WIFI_TEN_SEC,
                              strlen(CTRL_MSG_CHANGE_WIFI_TEN_SEC));
            break;
        case CTRL_CODE_CHANGE_WIFI_SIXTY_SEC:
            ret = SendMessage(g_currentSessionId4Ctl,
                              CTRL_MSG_CHANGE_WIFI_SIXTY_SEC,
                              strlen(CTRL_MSG_CHANGE_WIFI_SIXTY_SEC));
            break;
        case CTRL_CODE_CHANGE_WIFI_LOOP20:
            ret =
                SendMessage(g_currentSessionId4Ctl, CTRL_MSG_CHANGE_WIFI_LOOP20,
                            strlen(CTRL_MSG_CHANGE_WIFI_LOOP20));
            break;
        case CTRL_CODE_CLOSE_WIFI_LOOP20:
            ret =
                SendMessage(g_currentSessionId4Ctl, CTRL_MSG_CLOSE_WIFI_LOOP20,
                            strlen(CTRL_MSG_CLOSE_WIFI_LOOP20));
            break;
        case CTRL_CODE_CHANGE_WIFI_LOOP100:
            ret = SendMessage(g_currentSessionId4Ctl,
                              CTRL_MSG_CHANGE_WIFI_LOOP100,
                              strlen(CTRL_MSG_CHANGE_WIFI_LOOP100));
            break;
        case CTRL_CODE_CLOSE_WIFI_LOOP100:
            ret =
                SendMessage(g_currentSessionId4Ctl, CTRL_MSG_CLOSE_WIFI_LOOP100,
                            strlen(CTRL_MSG_CLOSE_WIFI_LOOP100));
            break;
        case CTRL_CODE_CLOSE_BR:
            ret = SendMessage(g_currentSessionId4Ctl, CTRL_MSG_CLOSE_BR,
                              strlen(CTRL_MSG_CLOSE_BR));
            break;
        case CTRL_CODE_CLOSE_OPEN_BR:
            ret = SendMessage(g_currentSessionId4Ctl, CTRL_MSG_CLOSE_OPEN_BR,
                              strlen(CTRL_MSG_CLOSE_OPEN_BR));
            break;
        case CTRL_CODE_OPEN_SESSION:
            ret = SendMessage(g_currentSessionId4Ctl, CTRL_MSG_OPEN_SESSION,
                              strlen(CTRL_MSG_OPEN_SESSION));
            break;
        case CTRL_CODE_OPEN_SESSION_MSG:
            ret = SendMessage(g_currentSessionId4Ctl, CTRL_MSG_OPEN_SESSION_MSG,
                              strlen(CTRL_MSG_OPEN_SESSION_MSG));
            break;
        case CTRL_CODE_OPEN_SESSION_FAIL:
            g_passiveOpenRetFlag = SOFTBUS_ERR;
            ret =
                SendMessage(g_currentSessionId4Ctl, CTRL_MSG_OPEN_SESSION_FAIL,
                            strlen(CTRL_MSG_OPEN_SESSION_FAIL));
            break;
        case CTRL_CODE_CLOSE_SESSION:
            ret = SendMessage(g_currentSessionId4Ctl, CTRL_MSG_CLOSE_SESSION,
                              strlen(CTRL_MSG_CLOSE_SESSION));
            break;
        case CTRL_CODE_OPEN_SESSION_NOT_EXIST:
            ret = SendMessage(g_currentSessionId4Ctl,
                              CTRL_MSG_OPEN_SESSION_NOT_EXIST,
                              strlen(CTRL_MSG_OPEN_SESSION_NOT_EXIST));
            break;
        case CTRL_CODE_SEND_BIG_DATA:
            ret = SendMessage(g_currentSessionId4Ctl, CTRL_MSG_SEND_BIG_DATA,
                              strlen(CTRL_MSG_SEND_BIG_DATA));
            break;
        default:
            LOG("[send ctrl msg]not support this code");
            break;
    }
    LOG("[send ctrl msg]send ret:%d", ret);
    return ret;
}

int SendData4Data(DataType type, int size)
{
    int ret;
    if (size > 0) {
        g_expectDataContent = (char*)calloc(1, size);
        if (g_expectDataContent == NULL) {
            LOG("[send data]calloc fail");
            return SOFTBUS_ERR;
        }
        (void)memset_s(g_expectDataContent, size, g_fillContentChar, size);
    } else {
        LOG("[send data]invalid param[size>=1]");
        return SOFTBUS_ERR;
    }

    g_expectDataSize = size;
    ResetWaitFlag();
    if (type == DATA_TYPE_MSG) {
        ret = SendMessage(g_currentSessionId4Data, g_expectDataContent, size);
    } else if (type == DATA_TYPE_BYTE) {
        ret = SendBytes(g_currentSessionId4Data, g_expectDataContent, size);
    } else {
        LOG("[send data]invalid param[DataType]");
        free(g_expectDataContent);
        return SOFTBUS_ERR;
    }
    if (ret != SOFTBUS_OK) {
        LOG("[send data]call SendX fail, ret:%d", ret);
        free(g_expectDataContent);
        return SOFTBUS_ERR;
    }
    LOG("[send data]call SendX success");
    int sleeptime = 15;
    int timeout = 5;
    if (size > SIZE_1K * SIZE_1K) {
        timeout = sleeptime;
    }
    ret = Wait(timeout);
    free(g_expectDataContent);
    g_expectDataContent = NULL;
    return ret;
}

// if open session success, return session id
int OpenSession4Perf(void)
{
    int ret;
    int sessionId;
    int timeout = 10;
    ResetWaitFlag();
    g_openSessionTimeStart = GetCurrentTimeOfMs();
    sessionId = OpenSession(SESSION_NAME_PERF, SESSION_NAME_PERF, g_networkId,
                            DEF_GROUP_ID, g_sessionAttr4Perf);
    if (sessionId < SESSION_ID_MIN) {
        LOG("call OpenSession[perf] fail, ret sid:%d", sessionId);
        return SOFTBUS_ERR;
    }
    ret = Wait(timeout);
    if (ret != SOFTBUS_OK) {
        LOG("OpenSession[perf] fail");
        return SOFTBUS_ERR;
    }
    return sessionId;
}

int OpenSession4PerfWithParam(const char* sessionName,
                              const char* groupId,
                              char* netId)
{
    int ret;
    int sessionId;
    int timeout = 10;
    ResetWaitFlag();
    g_openSessionTimeStart = GetCurrentTimeOfMs();
    sessionId = OpenSession(sessionName, sessionName, netId, groupId,
                            g_sessionAttr4Perf);
    if (sessionId < SESSION_ID_MIN) {
        LOG("call OpenSession[perf] fail, ret sid:%d", sessionId);
        return SOFTBUS_ERR;
    }
    ret = Wait(timeout);
    if (ret != SOFTBUS_OK) {
        LOG("OpenSession[perf] fail");
        return SOFTBUS_ERR;
    }
    return sessionId;
}

int SendData4Perf(int sessionId, char* dataMsg, char* dataByte)
{
    int sleeptime = 500000;
    int timeout = 5;
    int isHitFail = SOFTBUS_OK;
    ResetWaitFlag();
    g_transTimeStart = GetCurrentTimeOfMs();
    int ret = SendMessage(sessionId, dataMsg, TRANS_MESSAGE_LENGTH_MAX);
    g_transTimeEnd = GetCurrentTimeOfMs();
    if (ret != SOFTBUS_OK) {
        LOG("call SendMessage[perf] fail, ret:%d", ret);
        isHitFail = SOFTBUS_ERR;
    } else {
    }
    usleep(sleeptime);

    ResetWaitFlag();
    g_transTimeStart = GetCurrentTimeOfMs();
    ret = SendBytes(sessionId, dataByte, TRANS_BYTES_LENGTH_MAX);
    if (ret != SOFTBUS_OK) {
        LOG("call SendBytes[perf] fail, ret:%d", ret);
        isHitFail = SOFTBUS_ERR;
    } else {
        ret = Wait(timeout);
        if (ret != SOFTBUS_OK) {
            LOG("wait byte callback fail ret:%d", ret);
            isHitFail = SOFTBUS_ERR;
        }
    }
    return isHitFail;
}

void SetTransStartTime(void)
{
    g_transTimeStart = GetCurrentTimeOfMs();
}

int CloseSessionAndRemoveSs4Data(void)
{
    int ret4Close;
    int timeout = 2;
    // 主动close不回调
    ResetWaitFlag4Data();
    CloseSession(g_currentSessionId4Data);
    ret4Close = Wait4Session(timeout, SESSION_4DATA);
    if (ret4Close == SOFTBUS_OK) {
        LOG("CloseSession[data] recv callback");
    }

    int ret4Ss;
    ret4Ss = RemoveSessionServer(DEF_PKG_NAME, SESSION_NAME_DATA);
    if (ret4Ss != SOFTBUS_OK) {
        LOG("RemoveSessionServer[data] fail, ret:%d", ret4Ss);
    }

    if (ret4Ss != SOFTBUS_OK || ret4Close == SOFTBUS_OK) {
        return SOFTBUS_ERR;
    } else {
        return SOFTBUS_OK;
    }
}

int CloseSessionAndRemoveSs4Ctl(void)
{
    int ret4Close;
    int timeout = 2;
    ResetWaitFlag4Ctl();
    CloseSession(g_currentSessionId4Ctl);
    ret4Close = Wait4Session(timeout, SESSION_4CTL);
    if (ret4Close == SOFTBUS_OK) {
        LOG("CloseSession[ctrl] recv callback");
    }

    int ret4Ss;
    ret4Ss = RemoveSessionServer(DEF_PKG_NAME, SESSION_NAME_CTL);
    if (ret4Ss != SOFTBUS_OK) {
        LOG("RemoveSessionServer[ctl] fail, ret:%d", ret4Ss);
    }

    if (ret4Ss != SOFTBUS_OK || ret4Close == SOFTBUS_OK) {
        return SOFTBUS_ERR;
    } else {
        return SOFTBUS_OK;
    }
}

int OpenSessionBatch4Data(char groupId[][GROUP_ID_LEN],
                          int* sessionId,
                          int count)
{
    int ret;
    int timeout = 10;
    int rstFlag = SOFTBUS_OK;
    for (int i = 0; i < count; i++) {
        ResetWaitFlag4Data();
        int sId = OpenSession(SESSION_NAME_DATA, SESSION_NAME_DATA, g_networkId,
                              groupId[i], g_sessionAttr4Data);
        if (sId < SESSION_ID_MIN) {
            LOG("call OpenSession[data] fail,sid:%d,i:%d", sId, i);
            rstFlag = SOFTBUS_ERR;
            continue;
        }
        SetCurrentSessionId4Data(sId);
        *(sessionId + i) = sId;
        ret = Wait4Session(timeout, SESSION_4DATA);
        if (ret != SOFTBUS_OK) {
            LOG("Wait4Session[data] fail,i:%d", i);
            rstFlag = SOFTBUS_ERR;
        }
    }
    return rstFlag;
}

int OpenSessionBatch4Ctl(char groupId[][GROUP_ID_LEN],
                         int* sessionId,
                         int count)
{
    int ret;
    int timeout = 10;
    int rstFlag = SOFTBUS_OK;
    for (int i = 0; i < count; i++) {
        ResetWaitFlag4Ctl();
        int sId = OpenSession(SESSION_NAME_CTL, SESSION_NAME_CTL, g_networkId,
                              groupId[i], g_sessionAttr4Ctl);
        if (sId < SESSION_ID_MIN) {
            LOG("call OpenSession[ctrl] fail,sid:%d,i:%d", sId, i);
            rstFlag = SOFTBUS_ERR;
            continue;
        }
        SetCurrentSessionId4Ctl(sId);
        *(sessionId + i) = sId;
        ret = Wait4Session(timeout, SESSION_4CTL);
        if (ret != SOFTBUS_OK) {
            LOG("Wait4Session[ctrl] fail,i:%d", i);
            rstFlag = SOFTBUS_ERR;
        }
    }
    return rstFlag;
}

int CloseSessionBatch4Data(int* sessionId, int count)
{
    for (int i = 0; i < count; i++) {
        int sid = *(sessionId + i);
        LOG("close session[data] sid:%d", sid);

        SetCurrentSessionId4Data(sid);
        CloseSession(sid);
    }
    return SOFTBUS_OK;
}

int CloseSessionBatch4Ctl(int* sessionId, int count)
{
    for (int i = 0; i < count; i++) {
        int sid = *(sessionId + i);
        LOG("close session[ctrl] sid:%d", sid);

        SetCurrentSessionId4Ctl(sid);
        CloseSession(sid);
    }
    return SOFTBUS_OK;
}

void* OpenSessionTask4Data(void* param)
{
    LOG("OpenSessionTask[Data] start");
    int ret;
    int step1Flag;
    int step2Flag;
    int count = MAX_SESSION_NUM / 2;
    int sessionId4Data[count];
    char groupId[][GROUP_ID_LEN] = {"g1", "g2", "g3", "g4",
                                    "g5", "g6", "g7", "g8"};

    pthread_barrier_wait(g_barrier);
    ret = OpenSessionBatch4Data(groupId, sessionId4Data, count);
    if (ret != SOFTBUS_OK) {
        LOG("OpenSessionBatch4Data ret fail");
    } else {
        LOG("OpenSessionBatch4Data ret success");
    }
    step1Flag = ret;

    pthread_barrier_wait(g_barrier);
    // create more, expect fail
    int moreCount = 3;
    step2Flag = SOFTBUS_OK;
    char groupIdMore[][GROUP_ID_LEN] = {"m1", "m2", "m3"};
    for (int i = 0; i < moreCount; i++) {
        int sessionId =
            OpenSession(SESSION_NAME_DATA, SESSION_NAME_DATA, g_networkId,
                        groupIdMore[i], g_sessionAttr4Data);
        if (sessionId >= SESSION_ID_MIN) {
            LOG("OpenSessionTask[Ctrl] open more success, expect fail,i:%d, "
                "sid:%d",
                i, sessionId);
            step2Flag = SOFTBUS_ERR;
        }
    }

    if (step1Flag == SOFTBUS_OK && step2Flag == SOFTBUS_OK) {
        g_threadResult4Data = SOFTBUS_OK;
    } else {
        g_threadResult4Data = SOFTBUS_ERR;
    }

    LOG("OpenSessionTask[Data] end");
    return NULL;
}

void* OpenSessionTask4Ctl(void* param)
{
    LOG("OpenSessionTask[Ctrl] start");
    int ret;
    int step1Flag;
    int step2Flag;
    int count = MAX_SESSION_NUM / 2;
    int sessionId4Ctrl[count];
    char groupId[][GROUP_ID_LEN] = {"g1", "g2", "g3", "g4",
                                    "g5", "g6", "g7", "g8"};

    pthread_barrier_wait(g_barrier);
    ret = OpenSessionBatch4Ctl(groupId, sessionId4Ctrl, count);
    if (ret != SOFTBUS_OK) {
        LOG("OpenSessionBatch4Ctl ret fail");
    } else {
        LOG("OpenSessionBatch4Ctl ret success");
    }
    step1Flag = ret;

    pthread_barrier_wait(g_barrier);
    // create more, expect fail
    int moreCount = 3;
    step2Flag = SOFTBUS_OK;
    char groupIdMore[][GROUP_ID_LEN] = {"m1", "m2", "m3"};
    for (int i = 0; i < moreCount; i++) {
        int sessionId =
            OpenSession(SESSION_NAME_CTL, SESSION_NAME_CTL, g_networkId,
                        groupIdMore[i], g_sessionAttr4Ctl);
        if (sessionId >= SESSION_ID_MIN) {
            LOG("OpenSessionTask[Ctrl] open more success, expect fail,i:%d, "
                "sid:%d",
                i, sessionId);
            step2Flag = SOFTBUS_ERR;
        }
    }

    if (step1Flag == SOFTBUS_OK && step2Flag == SOFTBUS_OK) {
        g_threadResult4Ctl = SOFTBUS_OK;
    } else {
        g_threadResult4Ctl = SOFTBUS_ERR;
    }

    LOG("OpenSessionTask[Ctrl] end");
    return NULL;
}

// set/get function
void ResetClosedSessionCount4Data(void)
{
    g_sessionCloseCount4Data = 0;
}

void ResetClosedSessionCount4Ctrl(void)
{
    g_sessionCloseCount4Ctrl = 0;
}

int GetClosedSessionCount4Data(void)
{
    return g_sessionCloseCount4Data;
}

int GetClosedSessionCount4Ctrl(void)
{
    return g_sessionCloseCount4Ctrl;
}

void ResetOpenSessionCount4Data(void)
{
    g_sessionOpenCount4Data = 0;
}

void ResetOpenSessionCount4Ctrl(void)
{
    g_sessionOpenCount4Ctrl = 0;
}

int GetOpenSessionCount4Data(void)
{
    return g_sessionOpenCount4Data;
}

int GetOpenSessionCount4Ctrl(void)
{
    return g_sessionOpenCount4Ctrl;
}

int* GetSid4Task2(void)
{
    return g_sId4Task2;
}

int* GetSid4Task3(void)
{
    return g_sId4Task3;
}

void ResetMsgStat4Control(void)
{
    for (int i = 0; i < MAX_SESSION_NUM; i++) {
        g_recvMsgStat4Control[i] = 0;
    }
}

void ResetByteStat4Control(void)
{
    for (int i = 0; i < MAX_SESSION_NUM; i++) {
        g_recvByteStat4Control[i] = 0;
    }
}

pthread_barrier_t* GetThreadBarrier(void)
{
    return g_barrier;
}

char* GetNetworkId(void)
{
    return g_networkId;
}

// To one device only
int CheckRemoteDeviceIsNull(int isSetNetId)
{
    int nodeNum = 0;
    NodeBasicInfo* nodeInfo = NULL;
    int ret = GetAllNodeDeviceInfo(DEF_PKG_NAME, &nodeInfo, &nodeNum);
    LOG("[check]get node number is:%d, ret:%d", nodeNum, ret);
    if (nodeInfo != NULL && nodeNum > 0) {
        LOG("[check]get netid is:%s", nodeInfo->networkId);
        if (isSetNetId == BOOL_TRUE) {
            (void)strncpy_s(g_networkId, NETWORK_ID_BUF_LEN,
                            nodeInfo->networkId, NETWORK_ID_BUF_LEN);
        }
        FreeNodeInfo(nodeInfo);
        return SOFTBUS_OK;
    } else {
        LOG("[check]get nodeInfo is null");
        return SOFTBUS_ERR;
    }
}

int GetRemoteDeviceNetId(char** netId)
{
    int nodeNum = 0;
    NodeBasicInfo* nodeInfo = NULL;
    NodeBasicInfo* nodeInfoTmp = NULL;
    int ret = GetAllNodeDeviceInfo(DEF_PKG_NAME, &nodeInfo, &nodeNum);
    nodeInfoTmp = nodeInfo;
    LOG("[get]node number is:%d, ret:%d", nodeNum, ret);
    if (nodeInfo != NULL && nodeNum > 0) {
        for (int i = 0; i < nodeNum; i++) {
            LOG("[get] netid is:%s", nodeInfo->networkId);
            *(netId + i) = (char*)malloc(NETWORK_ID_BUF_LEN * sizeof(char));
            (void)strncpy_s(*(netId + i), NETWORK_ID_BUF_LEN,
                            nodeInfo->networkId, NETWORK_ID_BUF_LEN);
            LOG("[get] netid is [strcpy]:%s", *(netId + i));
            nodeInfo++;
        }
        FreeNodeInfo(nodeInfoTmp);
    } else {
        LOG("[get] nodeInfo is null");
    }
    return nodeNum;
}

// get remote device network id, then set to global array
int SetRemoteDeviceNetIdToGarray(void)
{
    int nodeNum = 0;
    NodeBasicInfo* nodeInfo = NULL;
    NodeBasicInfo* nodeInfoTmp = NULL;
    int ret = GetAllNodeDeviceInfo(DEF_PKG_NAME, &nodeInfo, &nodeNum);
    nodeInfoTmp = nodeInfo;
    LOG("[setdev netid]dev number is:%d, ret:%d", nodeNum, ret);
    if (nodeInfo != NULL && nodeNum > 0) {
        for (int i = 0; i < nodeNum; i++) {
            (void)strncpy_s(g_networkIdArray[i], NETWORK_ID_BUF_LEN,
                            nodeInfo->networkId, NETWORK_ID_BUF_LEN);
            LOG("[setdev netid] netid:%s", g_networkIdArray[i]);
            nodeInfo++;
        }
        FreeNodeInfo(nodeInfoTmp);
    } else {
        LOG("[setdev netid] nodeInfo is null");
    }
    return nodeNum;
}

int SetCurrentNetworkId(int index)
{
    if (index < 0 || index >= MULTI_REMOTE_DEV_COUNT) {
        LOG("input index is error");
        return SOFTBUS_ERR;
    }
    int ret = strncpy_s(g_networkId, NETWORK_ID_BUF_LEN, g_networkIdArray[index], NETWORK_ID_BUF_LEN);
    if (ret != SOFTBUS_OK)
    {
        LOG("strncpy_s fail");
        return SOFTBUS_ERR;
    }
    
    return SOFTBUS_OK;
}

ISessionListener* GetSessionListenser4Data(void)
{
    return g_sessionlistener4Data;
}

ISessionListener* GetSessionListenser4Ctl(void)
{
    return g_sessionlistener4Ctl;
}

ISessionListener* GetSessionListenser4Pass(void)
{
    return g_sessionlistener4Pass;
}

ISessionListener* GetSessionListenser4Perf(void)
{
    return g_sessionlistener4Perf;
}

ISessionListener* GetSessionListenser4Proxy(void)
{
    return g_sessionlistener4Proxy;
}

IFileSendListener* GetSendFileListener(void)
{
    return g_fileSendListener;
}

IFileReceiveListener* GetRecvFileListener(void)
{
    return g_fileRecvListener;
}

SessionAttribute* GetSessionAttr4Data(void)
{
    return g_sessionAttr4Data;
}

SessionAttribute* GetSessionAttr4Ctl(void)
{
    return g_sessionAttr4Ctl;
}

SessionAttribute* GetSessionAttr4Pass(void)
{
    return g_sessionAttr4Pass;
}

SessionAttribute* GetSessionAttr4Perf(void)
{
    return g_sessionAttr4Perf;
}

SessionAttribute* GetSessionAttr4Proxy(void)
{
    return g_sessionAttr4Proxy;
}

void SetCurrentSessionId4Data(int sessionId)
{
    g_currentSessionId4Data = sessionId;
}

void SetCurrentSessionId4Ctl(int sessionId)
{
    g_currentSessionId4Ctl = sessionId;
}

int GetCurrentSessionId4Data(void)
{
    return g_currentSessionId4Data;
}

int GetCurrentSessionId4Ctl(void)
{
    return g_currentSessionId4Ctl;
}

int GetThreadResult4Data(void)
{
    return g_threadResult4Data;
}

int GetThreadResult4Ctl(void)
{
    return g_threadResult4Ctl;
}

ConnectionAddr* GetConnectAddr(void)
{
    return &g_ethAddr;
}

void TestSetUp(void)
{
    g_defNodeStateCallback.events = EVENT_NODE_STATE_MASK;
    g_defNodeStateCallback.onNodeOnline = OnDefNodeOnline;
    g_defNodeStateCallback.onNodeOffline = OnDefNodeOffline;
    g_defNodeStateCallback.onNodeBasicInfoChanged = OnDefNodeBasicInfoChanged;

    g_defDiscCallback.OnDeviceFound = OnDefDeviceFound;
    g_defDiscCallback.OnDiscoverFailed = OnDefDiscoverFail;
    g_defDiscCallback.OnDiscoverySuccess = OnDefDiscoverSuccess;

    if (g_sessionlistener4Data == NULL) {
        g_sessionlistener4Data =
            (ISessionListener*)calloc(1, sizeof(ISessionListener));
        g_sessionlistener4Data->OnSessionOpened = DataSessionOpened;
        g_sessionlistener4Data->OnSessionClosed = DataSessionClosed;
        g_sessionlistener4Data->OnMessageReceived = DataMessageReceived;
        g_sessionlistener4Data->OnBytesReceived = DataBytesReceived;
    }
    if (g_sessionlistener4Ctl == NULL) {
        g_sessionlistener4Ctl =
            (ISessionListener*)calloc(1, sizeof(ISessionListener));
        g_sessionlistener4Ctl->OnSessionOpened = ControlSessionOpened;
        g_sessionlistener4Ctl->OnSessionClosed = ControlSessionClosed;
        g_sessionlistener4Ctl->OnMessageReceived = ControlMessageReceived;
        g_sessionlistener4Ctl->OnBytesReceived = ControlBytesReceived;
    }
    if (g_sessionlistener4Pass == NULL) {
        g_sessionlistener4Pass =
            (ISessionListener*)calloc(1, sizeof(ISessionListener));
        g_sessionlistener4Pass->OnSessionOpened = PassiveSessionOpened;
        g_sessionlistener4Pass->OnSessionClosed = PassiveSessionClosed;
        g_sessionlistener4Pass->OnMessageReceived = PassiveMessageReceived;
        g_sessionlistener4Pass->OnBytesReceived = PassiveBytesReceived;
    }
    if (g_sessionlistener4Perf == NULL) {
        g_sessionlistener4Perf =
            (ISessionListener*)calloc(1, sizeof(ISessionListener));
        g_sessionlistener4Perf->OnSessionOpened = PerfSessionOpened;
        g_sessionlistener4Perf->OnSessionClosed = PerfSessionClosed;
        g_sessionlistener4Perf->OnMessageReceived = PerfMessageReceived;
        g_sessionlistener4Perf->OnBytesReceived = PerfBytesReceived;
    }
    if (g_sessionlistener4Proxy == NULL) {
        g_sessionlistener4Proxy =
            (ISessionListener*)calloc(1, sizeof(ISessionListener));
        g_sessionlistener4Proxy->OnSessionOpened = ProxySessionOpened;
        g_sessionlistener4Proxy->OnSessionClosed = ProxySessionClosed;
        g_sessionlistener4Proxy->OnMessageReceived = ProxyMessageReceived;
        g_sessionlistener4Proxy->OnBytesReceived = ProxyBytesReceived;
    }

    if (g_fileSendListener == NULL) {
        g_fileSendListener =
            (IFileSendListener*)calloc(1, sizeof(IFileSendListener));
        g_fileSendListener->OnSendFileProcess = OnSendFileProcess;
        g_fileSendListener->OnSendFileFinished = OnSendFileFinished;
        g_fileSendListener->OnFileTransError = OnSendFileTransError;
    };
    if (g_fileRecvListener == NULL) {
        g_fileRecvListener =
            (IFileReceiveListener*)calloc(1, sizeof(IFileReceiveListener));
        g_fileRecvListener->OnReceiveFileStarted = OnReceiveFileStarted;
        g_fileRecvListener->OnReceiveFileProcess = OnReceiveFileProcess;
        g_fileRecvListener->OnReceiveFileFinished = OnReceiveFileFinished;
        g_fileRecvListener->OnFileTransError = OnRecvFileTransError;
    }

    if (g_sessionAttr4Data == NULL) {
        g_sessionAttr4Data =
            (SessionAttribute*)calloc(1, sizeof(SessionAttribute));
        g_sessionAttr4Data->dataType = TYPE_BYTES;
    }
    if (g_sessionAttr4Ctl == NULL) {
        g_sessionAttr4Ctl =
            (SessionAttribute*)calloc(1, sizeof(SessionAttribute));
        g_sessionAttr4Ctl->dataType = TYPE_BYTES;
    }
    if (g_sessionAttr4Pass == NULL) {
        g_sessionAttr4Pass =
            (SessionAttribute*)calloc(1, sizeof(SessionAttribute));
        g_sessionAttr4Pass->dataType = TYPE_BYTES;
    }
    if (g_sessionAttr4Perf == NULL) {
        g_sessionAttr4Perf =
            (SessionAttribute*)calloc(1, sizeof(SessionAttribute));
        g_sessionAttr4Perf->dataType = TYPE_BYTES;
    }
    if (g_sessionAttr4Proxy == NULL) {
        g_sessionAttr4Proxy =
            (SessionAttribute*)calloc(1, sizeof(SessionAttribute));
        g_sessionAttr4Proxy->dataType = TYPE_MESSAGE;
    }

    if (g_barrier == NULL) {
        g_barrier = (pthread_barrier_t*)calloc(1, sizeof(pthread_barrier_t));
    }

    if (g_sId4Task2 == NULL) {
        g_sId4Task2 = (int*)malloc(sizeof(int) * DEF_SEND_DATA_SID_COUNT);
    }
    if (g_sId4Task3 == NULL) {
        g_sId4Task3 = (int*)malloc(sizeof(int) * DEF_SEND_DATA_SID_COUNT);
    }
}

void TestTearDown(void)
{
    if (g_sessionlistener4Data != NULL) {
        free(g_sessionlistener4Data);
        g_sessionlistener4Data = NULL;
    }
    if (g_sessionlistener4Ctl != NULL) {
        free(g_sessionlistener4Ctl);
        g_sessionlistener4Ctl = NULL;
    }
    if (g_sessionlistener4Pass != NULL) {
        free(g_sessionlistener4Pass);
        g_sessionlistener4Pass = NULL;
    }
    if (g_sessionlistener4Perf != NULL) {
        free(g_sessionlistener4Perf);
        g_sessionlistener4Perf = NULL;
    }
    if (g_sessionlistener4Proxy != NULL) {
        free(g_sessionlistener4Proxy);
        g_sessionlistener4Proxy = NULL;
    }

    if (g_fileRecvListener != NULL) {
        free(g_fileRecvListener);
        g_fileRecvListener = NULL;
    }
    if (g_fileSendListener != NULL) {
        free(g_fileSendListener);
        g_fileSendListener = NULL;
    }

    if (g_sessionAttr4Data != NULL) {
        free(g_sessionAttr4Data);
        g_sessionAttr4Data = NULL;
    }
    if (g_sessionAttr4Ctl != NULL) {
        free(g_sessionAttr4Ctl);
        g_sessionAttr4Ctl = NULL;
    }
    if (g_sessionAttr4Pass != NULL) {
        free(g_sessionAttr4Pass);
        g_sessionAttr4Pass = NULL;
    }
    if (g_sessionAttr4Perf != NULL) {
        free(g_sessionAttr4Perf);
        g_sessionAttr4Perf = NULL;
    }
    if (g_sessionAttr4Proxy != NULL) {
        free(g_sessionAttr4Proxy);
        g_sessionAttr4Proxy = NULL;
    }

    if (g_barrier != NULL) {
        free(g_barrier);
        g_barrier = NULL;
    }

    if (g_sId4Task2 != NULL) {
        free(g_sId4Task2);
        g_sId4Task2 = NULL;
    }
    if (g_sId4Task3 != NULL) {
        free(g_sId4Task3);
        g_sId4Task3 = NULL;
    }
}
