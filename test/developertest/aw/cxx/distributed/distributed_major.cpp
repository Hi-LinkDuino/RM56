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

#include "distributed_major.h"

#include <cstring>
#include <map>
#include <cerrno>
#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "securec.h"
#include "hilog/log.h"

namespace OHOS {
namespace DistributeSystemTest {
using namespace std;
using namespace testing;
using namespace OHOS::HiviewDFX;
DistributeTestEnvironment  *g_pDistributetestEnv = nullptr;
namespace {
    const constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DistributeTestEnvironment"};
    const int CONNECT_TIME = 3;
    const int SLEEP_TIME = 1000;
    const int HALF_BUF_LEN = 2;
    const int CMD_LENGTH = 50;
}

DistributeTestEnvironment::DistributeTestEnvironment() : serverPort_(DEFAULT_AGENT_PORT)
{
}

DistributeTestEnvironment::DistributeTestEnvironment(std::string cfgFile) : serverPort_(DEFAULT_AGENT_PORT)
{
    Init(cfgFile);
}

void DistributeTestEnvironment::Init(std::string fileName)
{
    clientCfg_.OpenCfg(fileName);
    std::string iplist;
    if (!clientCfg_.GetCfgVal("agentlist", iplist)) {
        return;
    }
    std::string::size_type posend = 0;
    std::string::size_type pos = 0;
    do {
        std::string ipaddr;
        posend = iplist.find(",", pos);
        if (posend != std::string::npos) {
            ipaddr = iplist.substr(pos, posend - pos);
        } else {
            ipaddr = iplist.substr(pos);
        }
        AddClient(ipaddr);
        if (posend >= iplist.length()) {
            break;
        }
        pos = posend + 1;
    } while (posend != std::string::npos);
    std::string strPort;
    if (!clientCfg_.GetCfgVal("agentport", strPort)) {
        return;
    }
    if (sscanf_s(strPort.c_str(), "%d", &serverPort_) < 1) {
        serverPort_ = DEFAULT_AGENT_PORT;
    }
    HiLog::Info(LABEL, "get device port :  %d", serverPort_);
}

DistributeTestEnvironment::~DistributeTestEnvironment()
{
}

int DistributeTestEnvironment::GetSerial()
{
    static int serialno = 0;
    return serialno++;
}

int DistributeTestEnvironment::AddClient(std::string ipAddr)
{
    int count = clientList_.size();
    struct sockaddr_in addr;
    if (inet_pton(AF_INET, ipAddr.c_str(), &addr.sin_addr) == 1) {
        DistDeviceInfo dev;
        dev.devNo = count;
        dev.ipAddr = ipAddr;
        dev.fd = -1;
        clientList_.push_back(dev);
        count++;
    } else {
        return 0;
    }
    return 1;
}

int DistributeTestEnvironment::ConnectAgent(size_t devNo)
{
    if (devNo >= clientList_.size()) {
        return 0;
    }
    std::string serverIp = clientList_[devNo].ipAddr;
    struct sockaddr_in addr;
    int clientSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSockFd < 0) {
        return -1;
    }
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIp.c_str(), &addr.sin_addr);
    addr.sin_port = htons(serverPort_);
    int connectCount = 0;
    for (connectCount = 0; connectCount < CONNECT_TIME; connectCount++) {  // try connect to agent 3 times.
        if (!connect(clientSockFd, reinterpret<struct sockaddr *>(&addr), sizeof(addr))) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));  // delay 10ms
    }
    if (connectCount >= CONNECT_TIME) {
        HiLog::Error(LABEL, "connect to agent %s fail.", serverIp.c_str());
        close(clientSockFd);
        clientSockFd = -1;
        return 0;
    }
    HiLog::Info(LABEL, "connect to agent %s success.", serverIp.c_str());
    clientList_[devNo].fd = clientSockFd;
    return 1;
}

void DistributeTestEnvironment::SetUp()
{
    // initial connect agent;
    size_t clientNo;
    for (clientNo = 0; clientNo < clientList_.size(); clientNo++) {
        // create connect to agent of ipaddress is , port is 8642.
        ConnectAgent(clientNo);
    }
}

void DistributeTestEnvironment::TearDown()
{
    size_t clientNo;
    for (clientNo = 0; clientNo < clientList_.size(); clientNo++) {
        close(clientList_[clientNo].fd); // close socket
        clientList_[clientNo].fd = -1;
    }
}

bool DistributeTestEnvironment::SendToAgent(size_t devNo, int cmdType, void *pstrMsg, int len,
    std::function<bool(const std::string &, int)> onProcessReturn)
{
    static int globalCommandNo = 0;
    bool breturn = false;
    devNo = devNo - 1;
    if (devNo >= clientList_.size()) {
        HiLog::Info(LABEL, "can not find no %zu device.", devNo);
        return breturn;
    }
    if (clientList_[devNo].fd <= 0) {
        HiLog::Info(LABEL, "connect is failure %zu device.", devNo);
        return breturn;
    }
    if (pstrMsg == nullptr) {
        return false;
    }
    globalCommandNo++;
    char szrbuf[MAX_BUFF_LEN] = {0};
    auto pCmdMsg = reinterpret_cast<DistributedMsg *>(pstrMsg);
    pCmdMsg->no = globalCommandNo;
    pCmdMsg->cmdTestType = htons(cmdType);
    pCmdMsg->len = htons(len);
    int rlen = send(clientList_[devNo].fd, pCmdMsg, static_cast<size_t>(len + DST_COMMAND_HEAD_LEN), 0);
    if (rlen <= 0) {
        HiLog::Error(LABEL, "agent socket is closed.");
        return breturn;
    }
    // get ret value ;
    switch (cmdType) {
        case DST_COMMAND_CALL:
        case DST_COMMAND_MSG: {
            int times = CONNECT_TIME;
            while (times > 0) {
                rlen = recv(clientList_[devNo].fd, szrbuf, DST_COMMAND_HEAD_LEN, 0);
                if (static_cast<unsigned long>(rlen) >= DST_COMMAND_HEAD_LEN) {
                    auto pCmdTest = reinterpret_cast<DistributedMsg *>(szrbuf);
                    pCmdTest->cmdTestType = ntohs(pCmdTest->cmdTestType);
                    pCmdTest->len = ntohs(pCmdTest->len);
                    if (pCmdTest->len <= 0) {
                        times--;
                        continue;
                    }
                    recv(clientList_[devNo].fd, pCmdTest->alignmentCmd, pCmdTest->len, 0);
                    HiLog::Info(LABEL, "recv agent data : No.%d command type :%d length :%d",
                        pCmdTest->no, pCmdTest->cmdTestType, pCmdTest->len);
                    if ((globalCommandNo == pCmdTest->no) && (cmdType == pCmdTest->cmdTestType)) {
                        // get ret value ;
                        if (onProcessReturn != nullptr) {
                            breturn = onProcessReturn(pCmdTest->alignmentCmd, pCmdTest->len);
                        } else {
                            breturn = true;
                        }
                        break;
                    } else {
                        HiLog::Error(LABEL, "get error message. type is :%d", pCmdTest->cmdTestType);
                    }
                } else {
                    if (!rlen) {
                        // peer socket is closed.
                        HiLog::Error(LABEL, "device socket close.");
                        break;
                    }
                }
                usleep(SLEEP_TIME);
                times--;
            }
            break;
        }
        default: {
            breturn = true;
            break;
        }
    }
    return breturn;
}

bool DistributeTestEnvironment::RunTestCmd(size_t devNo, const std::string &strCommand, int cmdLen,
    const std::string &strExpectValue, int expectValueLen,
    std::function<bool(const std::string &, int)> onProcessReturn)
{
    // send command data length
    char szbuf[MAX_BUFF_LEN];
    bool breturn = false;
    size_t lenptr = 0;
    errno_t ret = EOK;
    ret = memset_s(szbuf, sizeof(szbuf), 0, MAX_BUFF_LEN);
    if (ret != EOK) {
        return breturn;
    }
    // add 2 '\0'
    size_t rlen = cmdLen + expectValueLen + DST_COMMAND_HEAD_LEN + sizeof(int)*HALF_BUF_LEN + HALF_BUF_LEN;
    if (rlen <= MAX_BUFF_LEN) {
        auto pCmdTest = reinterpret_cast<DistributedMsg *>(szbuf);
        pCmdTest->cmdTestType = DST_COMMAND_CALL;

        // alignmentCmd buff format:
        // cmd_size:int, cmd string, '\0' expectvalue_size:int
        // expectvalue string, '\0'
        lenptr = 0;
        *reinterpret_cast<int *>(pCmdTest->alignmentCmd + lenptr) = htons(cmdLen);
        lenptr += sizeof(int);
        ret = memcpy_s(pCmdTest->alignmentCmd + lenptr, MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN - lenptr,
            strCommand.c_str(), cmdLen);
        if (ret != EOK) {
            return breturn;
        }
        lenptr += cmdLen + 1;
        *reinterpret_cast<int *>(pCmdTest->alignmentCmd + lenptr) = htons(expectValueLen);
        lenptr += sizeof(int);
        ret = memcpy_s(pCmdTest->alignmentCmd + lenptr, MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN - lenptr,
            strExpectValue.c_str(), expectValueLen);
        if (ret != EOK) {
            return breturn;
        }
        lenptr += expectValueLen + 1;
        pCmdTest->len =  lenptr;
        breturn = SendToAgent(devNo, DST_COMMAND_CALL, pCmdTest, pCmdTest->len, onProcessReturn);
    } else {
        HiLog::Error(LABEL, "command data is too long \n");
    }
    return breturn;
};

bool DistributeTestEnvironment::SendMessage(size_t devNo, const std::string &strMsg, int msgLen,
    std::function<bool(const std::string &, int)> onProcessReturnMsg)
{
    bool breturn = false;
    if ((msgLen + DST_COMMAND_HEAD_LEN) <= MAX_BUFF_LEN) {
        char szbuf[MAX_BUFF_LEN] = {0};
        auto pCmdTest = reinterpret_cast<DistributedMsg *>(szbuf);
        pCmdTest->cmdTestType = DST_COMMAND_MSG;
        errno_t ret = memcpy_s(pCmdTest->alignmentCmd, MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN, strMsg.c_str(), msgLen);
        if (ret != EOK) {
            return breturn;
        }
        pCmdTest->len = msgLen;
        breturn = SendToAgent(devNo, DST_COMMAND_MSG, pCmdTest, msgLen, onProcessReturnMsg);
    } else {
        HiLog::Info(LABEL, "message data is too long.\n");
    }
    return breturn;
}

bool DistributeTestEnvironment::Notify(size_t devNo, const std::string &strMsg, int msgLen)
{
    int dstMax = MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN;
    if (msgLen < 0 || msgLen >= dstMax) {
        return false;
    }

    bool breturn = false;
    if ((msgLen + DST_COMMAND_HEAD_LEN) <= MAX_BUFF_LEN) {
        char szbuf[MAX_BUFF_LEN] = {0};
        auto pCmdTest = reinterpret_cast<DistributedMsg *>(szbuf);
        pCmdTest->cmdTestType = DST_COMMAND_NOTIFY;
        errno_t ret = memcpy_s(pCmdTest->alignmentCmd, dstMax, strMsg.c_str(), msgLen);
        if (ret != EOK) {
            return breturn;
        }
        pCmdTest->alignmentCmd[msgLen] = 0;
        pCmdTest->len = msgLen;
        breturn = SendToAgent(devNo, DST_COMMAND_NOTIFY, pCmdTest, msgLen, nullptr);
    } else {
        HiLog::Info(LABEL, "notify data is too long.\n");
    }
    return breturn;
}

DistributeTest::DistributeTest()
{
    returnVal_ = 0;
}

DistributeTest::~DistributeTest()
{
}

int DistributeTest::CheckStatus()
{
    return 0;
}

/*
 * function : the testcase execute command on agent device, tell device something, agent process it.
 *     this interface is opened for user.
 * param :
 *     devNo: agent device serial number.
 *     strCommand: command of the testcase to send.
 *     cmdLen : length of command
 *     strExpectValue: expected return value
 *     expectValueLen: real length of return value
 * return : if false is return, execute operation failed.
 */
bool DistributeTest::RunCmdOnAgent(AGENT_NO devNo, const std::string &strCommand, int cmdLen,
    const std::string &strExpectValue, int expectValueLen)
{
    if (g_pDistributetestEnv != nullptr) {
        return g_pDistributetestEnv->RunTestCmd(devNo, strCommand, cmdLen, strExpectValue, expectValueLen,
            [&](const std::string &strReturn, int strLen)->bool {
                return OnProcessValue(strReturn, strLen);
            });
    }
    return false;
}

/*
 * function : the testcase execute command on agent device, include command parameter.
 *     this interface is opened for user.
 * param :
 *     devNo: agent device serial number.
 *     strCmd: command of the testcase to send.
 *     strArgs: command parameter.
 *     strExpectValue: expected return value
 * return : if false is return, execute operation failed.
 */
bool DistributeTest::RunCmdOnAgent(AGENT_NO devNo, const std::string &strCmd, const std::string &strArgs,
    const std::string &strExpectValue)
{
    if (g_pDistributetestEnv != nullptr) {
        std::string strBuf = strCmd + ":" + strArgs;
        int cmdLen = strBuf.length() + 1;
        int expectValueLen = strExpectValue.length() + 1;
        return g_pDistributetestEnv->RunTestCmd(devNo, strBuf, cmdLen, strExpectValue, expectValueLen,
            [&](const std::string &strReturn, int strLen)->bool {
                return OnProcessValue(strReturn, strLen);
            });
    }
    return false;
}

/*
 * function : the testcase execute command on agent device, include command parameter and callback.
 *     this interface is opened for user.
 * param :
 *     devNo: agent device serial number.
 *     strCmd: command of the testcase to send.
 *     strArgs: command parameter.
 *     strExpectValue: expected return value
 *     onReturnCall: callback function to handle return value and real length of return value.
 * return : if false is return, execute operation failed.
 */
bool DistributeTest::RunCmdOnAgent(AGENT_NO devNo, const std::string &strCmd, const std::string &strArgs,
    const std::string &strExpectValue, std::function<bool(const std::string &, int)> onReturnCall)
{
    if (g_pDistributetestEnv != nullptr) {
        std::string strBuf = strCmd + ":" + strArgs;
        int cmdLen = strBuf.length() + 1;
        int expectValueLen = strExpectValue.length() + 1;
        return g_pDistributetestEnv->RunTestCmd(devNo, strBuf, cmdLen, strExpectValue, expectValueLen, onReturnCall);
    }
    return false;
}

bool DistributeTest::OnProcessValue(const std::string &szbuf, int len)
{
    if (szbuf == "") {
        return false;
    }
    if (sscanf_s(szbuf.c_str(), "%d", &returnVal_) < 1) {
        return false;
    }
    return true;
}

int DistributeTest::GetReturnVal()
{
    return returnVal_;
}

/*
 * function : testcase send message to agent device, tell agent device something, agent process it.
 *     this interface is opened for user.
 * param :
 *     devNo: the serial number of agent device.
 *     msg : message of the testcase sent to the agent
 *     len: length of strMsg
 * return : if false is return, send operation failed.
 */
bool DistributeTest::SendMessage(AGENT_NO devNo, const std::string &msg, int len)
{
    if (g_pDistributetestEnv != nullptr) {
        return g_pDistributetestEnv->SendMessage(devNo, msg, len,
            [&](const std::string &szreturnbuf, int rlen)->bool {
                HiLog::Info(LABEL, "onprocessmsg len :%d.", rlen);
                return OnMsgProc(szreturnbuf, rlen);
            });
    }
    return false;
}

/*
 * function : testcase send message to agent device, include callback.
 *     this interface is opened for user.
 * param :
 *     devNo: the serial number of agent device.
 *     msg : message of the testcase sent to the agent
 *     len: length of message
 *     onProcessReturnMsg: callback function that handles the agent device return message and real
 *                         length of return value
 * return : if false is return, send operation failed.
 */
bool DistributeTest::SendMessage(AGENT_NO devNo, const std::string &msg, int len,
    std::function<bool(const std::string &, int)> onProcessReturnMsg)
{
    if (g_pDistributetestEnv != nullptr) {
        return g_pDistributetestEnv->SendMessage(devNo, msg, len, onProcessReturnMsg);
    }
    return false;
}

bool DistributeTest::OnMsgProc(const std::string &szbuf, int len)
{
    return (szbuf == "") ? false : true;
}

/*
 * function : testcase send message to agent device, no return value from agent device.
 *     this interface is opened for user.
 * param :
 *     devNo: the serial number of agent device.
 *     notifyType : message of the testcase notify the agent
 *     msg: message of the testcase notify the agent, message format: type:message
 *     msgLen: length of message
 * return : if false is return, notify operation failed.
 */
bool DistributeTest::Notify(AGENT_NO devNo, const std::string &notifyType, const std::string &msg, int msgLen)
{
    // maybe need justify if the length of notifytype+msg is bigger than MAX_BUFF_LEN/2;
    // the length of notifytype must be less than 50;
    if (notifyType.size() < CMD_LENGTH) {
        if (g_pDistributetestEnv != nullptr) {
            std::string strBuf = notifyType + ":" + msg;
            return g_pDistributetestEnv->Notify(devNo, strBuf, strBuf.length() + 1);
        }
    }
    return false;
}
} // namespace DistributeSystemTest
} // namespace OHOS
