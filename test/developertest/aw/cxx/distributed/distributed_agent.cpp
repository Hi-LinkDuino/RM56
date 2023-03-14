/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "distributed_agent.h"

#include <cstring>
#include <cerrno>

#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace OHOS {
namespace DistributeSystemTest {
using namespace std;
using namespace OHOS::HiviewDFX;

DistributedAgent::DistributedAgent()
{
    agentPort_ = 0;
    agentIpAddr_ = "";
    mpthCmdProcess_ = nullptr;
    mbStop_ = false;
    clientSockFd_ = -1;
    agentCfg_ = DistributedCfg();
}

DistributedAgent::~DistributedAgent()
{
    if (mpthCmdProcess_ != nullptr) {
        mbStop_ = true;
        if (mpthCmdProcess_->joinable()) {
            mpthCmdProcess_->join();
        }
    }
    close(clientSockFd_);
    clientSockFd_ = -1;
}

/*
 * The result of init test environment is returned. if false is return, the init operation failed.
 * Note that, the test environment should be inited for the test case.
 */
bool DistributedAgent::SetUp()
{
    return true;
}

/*
 * The result of reset test environment is returned. if false is return, the reset operation failed.
 * Note that, the test environment should be reset by the test case in the end.
 */
bool DistributedAgent::TearDown()
{
    return true;
}

int DistributedAgent::InitAgentServer() 
{
    HiLog::Info(DistributedAgent::LABEL, "begin create agent server.\n");
    volatile int serverSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSockFd < 0) {
        return -1;
    }

    int num = 1;
    if (setsockopt(serverSockFd, SOL_SOCKET, SO_REUSEADDR, &num, sizeof(num))) {
        close(serverSockFd);
        serverSockFd = -1;
        return serverSockFd;
    }

    struct sockaddr_in addr;
    errno_t ret = EOK;
    ret = memset_s(&addr, sizeof(addr), 0, sizeof(addr));
    if (ret != EOK) {
        return -1;
    }
    addr.sin_family = AF_INET;
    if (agentIpAddr_ != "") {
        inet_pton(AF_INET, agentIpAddr_.c_str(), &addr.sin_addr);
    } else {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    addr.sin_port = htons(agentPort_);
    int err = ::bind(serverSockFd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
    if (err < 0) {
        HiLog::Error(DistributedAgent::LABEL, "agent bind error.\n");
        close(serverSockFd);
        serverSockFd = -1;
        return serverSockFd;
    }

    if (listen(serverSockFd, 1) < 0) {
        HiLog::Error(DistributedAgent::LABEL, "agent listen error.\n");
        close(serverSockFd);
        serverSockFd = -1;
        return serverSockFd;
    }

    mpthCmdProcess_ = std::make_unique<std::thread>([=]() {
        DoCmdServer(serverSockFd);
    });

    OnLocalInit();
    return serverSockFd;
}

int DistributedAgent::DoCmdServer(int serverSockFd)
{
    char buff[MAX_BUFF_LEN] = {0};
    char returnValue[MAX_BUFF_LEN] = {0};
    int rlen = 0;
    int receiveLen = 0;
    struct sockaddr_in clientAddr;
    socklen_t sinSize = 0;
    int clientSockFd = -1;
    sinSize = sizeof(struct sockaddr_in);
    bzero(&clientAddr, sizeof(clientAddr));
    receiveLen = DistributedAgent::RECE_LEN;

    while (receiveLen > 0) {
        HiLog::Info(DistributedAgent::LABEL, "wait client .......\n");
        if ((clientSockFd = accept(serverSockFd, reinterpret_cast<struct sockaddr *>(&clientAddr), &sinSize)) > 0) {
            break;
        }
        receiveLen--;
        continue;
    }
    if (receiveLen <= 0) {
        HiLog::Error(DistributedAgent::LABEL, "test case runner can not work because I can not accept it.");
        close(serverSockFd);
        return -1;
    }
    clientSockFd_ = clientSockFd;
    HiLog::Info(DistributedAgent::LABEL, "accept testcase runner IP:%s port:%d \n",
                inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
    while (mbStop_ != true) {
        errno_t ret = EOK;
        ret = memset_s(buff, sizeof(buff), 0, MAX_BUFF_LEN);
        if (ret != EOK) {
            return -1;
        }
        // every cmd length less than MAX_BUFF_LEN bytes;
        int recvCmdLen = recv(clientSockFd_, buff, DST_COMMAND_HEAD_LEN, 0);
        if (static_cast<unsigned long>(recvCmdLen) <  DST_COMMAND_HEAD_LEN) {
            if (!recvCmdLen) {
                HiLog::Info(DistributedAgent::LABEL, "agent connect socket closed, IP:%s .\n",
                            inet_ntoa(clientAddr.sin_addr));
                mbStop_ = true;
            }
            continue;
        }
        auto pcline = reinterpret_cast<DistributedMsg *>(buff);
        pcline->cmdTestType = ntohs(pcline->cmdTestType);
        pcline->len = ntohs(pcline->len);
        HiLog::Info(DistributedAgent::LABEL, "test agent get message type:%d .\n", pcline->cmdTestType);
        if (pcline->len > 0 && static_cast<unsigned long>(pcline->len) <= (MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN)) {
            receiveLen = recv(clientSockFd_, pcline->alignmentCmd, static_cast<size_t>(pcline->len), 0);
            HiLog::Info(DistributedAgent::LABEL, "agent get message cmd=%s .\n", pcline->alignmentCmd);
            switch (pcline->cmdTestType) {
                case DST_COMMAND_CALL: {
                    errno_t ret = EOK;
                    unsigned int cmdLen = ntohs(*reinterpret_cast<int *>(pcline->alignmentCmd));
                    rlen = sizeof(int);
                    // check cmdLen length < 100, copy command + args data ;
                    char *pAlignmentCmd = new char[cmdLen + 1];
                    ret = memcpy_s(pAlignmentCmd, cmdLen + 1, pcline->alignmentCmd + rlen, cmdLen);
                    if (ret != EOK) {
                        delete []pAlignmentCmd;
                        return -1;
                    }
                    pAlignmentCmd[cmdLen] = '\0';
                    rlen += cmdLen + 1;
                    unsigned int eValueLen = ntohs(*reinterpret_cast<int *>(pcline->alignmentCmd + rlen));
                    rlen += sizeof(int);
                    char *pszEValue = new char[eValueLen + 1];
                    ret = memcpy_s(pszEValue, eValueLen + 1, pcline->alignmentCmd + rlen, eValueLen);
                    if (ret != EOK) {
                        delete []pAlignmentCmd;
                        delete []pszEValue;
                        return -1;
                    }
                    pszEValue[eValueLen] = '\0';
                    int nresult = OnProcessCmd(pAlignmentCmd, cmdLen, pszEValue, eValueLen);
                    ret = memset_s(returnValue, sizeof(returnValue), 0, MAX_BUFF_LEN);
                    if (ret != EOK) {
                        delete []pAlignmentCmd;
                        delete []pszEValue;
                        return -1;
                    }
                    auto pclinereturn = reinterpret_cast<DistributedMsg *>(returnValue);
                    pclinereturn->no = pcline->no;
                    pclinereturn->cmdTestType = htons(DST_COMMAND_CALL);
                    (void)sprintf_s(pclinereturn->alignmentCmd, (MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN),
                                    "%d", nresult) < 0);
                    rlen = strlen(pclinereturn->alignmentCmd) + 1;
                    pclinereturn->len = htons(rlen);
                    HiLog::Info(DistributedAgent::LABEL, "agent get message :%s .\n",
                                pclinereturn->alignmentCmd);
                    send(clientSockFd_, pclinereturn, static_cast<size_t>(rlen + DST_COMMAND_HEAD_LEN), 0);
                    delete []pAlignmentCmd;
                    delete []pszEValue;
                    break;
                }
                case DST_COMMAND_MSG: {
                    errno_t ret = EOK;
                    int nresult = 0;
                    ret = memset_s(returnValue, sizeof(returnValue), 0, MAX_BUFF_LEN);
                    if (ret != EOK) {
                        return -1;
                    }
                    auto pclinereturn = reinterpret_cast<DistributedMsg *>(returnValue);
                    pclinereturn->no = pcline->no;
                    pclinereturn->cmdTestType = htons(DST_COMMAND_MSG);
                    pclinereturn->len = MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN;
                    std::string resultcmd = pclinereturn->alignmentCmd;
                    nresult = OnProcessMsg(pcline->alignmentCmd, pcline->len, resultcmd, pclinereturn->len);
                    if (resultcmd == "") {
                        resultcmd = "agent return message";
                    }
                    if (strcpy_s(pclinereturn->alignmentCmd, pclinereturn->len, resultcmd.c_str()) != EOK) {
                        return -1;
                    }
                    pclinereturn->len = htons(nresult);
                    send(clientSockFd_, pclinereturn, static_cast<size_t>(nresult + DST_COMMAND_HEAD_LEN), 0);
                    break;
                }
                case DST_COMMAND_NOTIFY: {
                    OnNotifyImf(pcline);
                    break;
                }
                case DST_COMMAND_END:
                    mbStop_ = true;
                    break;
                default:
                    break;
            }
        }
        if (EAGAIN == errno) {
            continue;
        }
    }
    return 0;
}

void DistributedAgent::OnNotifyImf(DistributedMsg *pcline)
{
    char alignmentCmd[DistributedAgent::CMD_LENGTH];
    char szMsg[MAX_BUFF_LEN / DistributedAgent::HALF_NUM - DistributedAgent::CMD_LENGTH];
    int cmdNo = 0;
    errno_t ret = EOK;
    ret = memset_s(alignmentCmd, sizeof(alignmentCmd), 0, DistributedAgent::CMD_LENGTH);
    if (ret != EOK) {
        return;
    }
    (void)memset_s(szMsg,
        MAX_BUFF_LEN / DistributedAgent::HALF_NUM - DistributedAgent::CMD_LENGTH, 0,
        MAX_BUFF_LEN / DistributedAgent::HALF_NUM - DistributedAgent::CMD_LENGTH);
    for (cmdNo = 0; cmdNo < DistributedAgent::CMD_LENGTH; cmdNo++) {
        if (pcline->alignmentCmd[cmdNo] == ':') {
            break;
        }
    }
    if (cmdNo >= DistributedAgent::CMD_LENGTH) {
        HiLog::Error(DistributedAgent::LABEL, "error command.\n");
    }  else {
        errno_t ret = EOK;
        ret = memcpy_s(alignmentCmd, sizeof(alignmentCmd), pcline->alignmentCmd, cmdNo);
        if (ret != EOK) {
            return;
        }
        ret = memcpy_s(szMsg, MAX_BUFF_LEN / DistributedAgent::HALF_NUM - DistributedAgent::CMD_LENGTH,
            pcline->alignmentCmd + cmdNo + 1, pcline->len - cmdNo - 1);
        if (ret != EOK) {
            return;
        }
        OnNotify(alignmentCmd, szMsg, pcline->len - cmdNo);
    }
}

/*
 * function : when testcase need this device to do something, or tell device something, agent process it.
 *     this interface is opened for user.
 * param :
 *     strMsg: message from testcase .
 *     len : length of strMsg
 *     strReturnValue: return message buffer
 *     returnbuflen: max length of strReturnValue
 * return : real length of strReturnValue filled
 */
int DistributedAgent::OnProcessMsg(const std::string &strMsg, int msgLen,
    std::string &strReturnValue, int returnBufLen)
{
    // default test code
    std::string returnStr = "agent return message";
    int returnLen = returnStr.size();
    if (strReturnValue != "" && returnLen <= returnBufLen) {
        strReturnValue = returnStr;
    }
    return returnLen;
}

/*
 * function : execute command from testcase.
 * param :
 *     strCommand: command from testcase ,format is : command_string:args_string.
 *     cmdLen : length of strCommand
 *     strExpectValue: expectvalue string
 *     expectvaluelen: length of strExpectValue
 * return : return integer value, default 0 is returned.
 */
int DistributedAgent::OnProcessCmd(const std::string &strCommand, int cmdLen,
    const std::string &strExpectValue, int expectValueLen)
{
    int nresult = 0;
    char alignmentCmd[DistributedAgent::CMD_LENGTH];
    char szArgs[MAX_BUFF_LEN / DistributedAgent::HALF_NUM - DistributedAgent::CMD_LENGTH];
    int cmdNo = 0;
    errno_t ret = EOK;
    ret = memset_s(alignmentCmd, sizeof(alignmentCmd), 0, DistributedAgent::CMD_LENGTH);
    if (ret != EOK) {
        return -1;
    }
    (void)memset_s(szArgs,
        MAX_BUFF_LEN / DistributedAgent::HALF_NUM - DistributedAgent::CMD_LENGTH, 0,
        MAX_BUFF_LEN / DistributedAgent::HALF_NUM - DistributedAgent::CMD_LENGTH);
    for (cmdNo = 0; cmdNo < DistributedAgent::CMD_LENGTH; cmdNo++) {
        if (strCommand[cmdNo] == ':') {
            break;
        }
    }
    if (cmdNo >= DistributedAgent::CMD_LENGTH) {
        HiLog::Error(DistributedAgent::LABEL, "error command.\n");
        nresult = -1;
        return nresult;
    }

    errno_t ret = EOK;
    ret = memcpy_s(alignmentCmd, sizeof(alignmentCmd), strCommand.c_str(), cmdNo);
    if (ret != EOK) {
        return -1;
    }
    ret = memcpy_s(szArgs,
        MAX_BUFF_LEN / DistributedAgent::HALF_NUM - DistributedAgent::CMD_LENGTH,
        strCommand.c_str() + cmdNo + 1,
        cmdLen - cmdNo - 1);
    if (ret != EOK) {
        return -1;
    }

    nresult = OnProcessCmd(alignmentCmd, cmdNo, szArgs, cmdLen - cmdNo, strExpectValue, expectValueLen);
    return nresult;
}

int DistributedAgent::OnProcessCmd(const std::string &strCommand, int cmdLen, const std::string &strArgs,
    int argsLen, const std::string &strExpectValue, int expectValueLen)
{
    int nresult = 0;
    HiLog::Info(DistributedAgent::LABEL, "this is a agent.\n");
    if (static_cast<int>(strExpectValue.size()) > expectValueLen ||
        static_cast<int>(strCommand.size()) > cmdLen ||
        static_cast<int>(strArgs.size()) > argsLen) {
        return -1;
    }
    return nresult;
}

int DistributedAgent::Start(std::string cfgFile)
{
    std::string agentPort;
    agentCfg_.OpenCfg(cfgFile);
    if (!agentCfg_.GetCfgVal("agentport", agentPort)) {
        HiLog::Error(DistributedAgent::LABEL, "agent can not get port.\n");
        return 0;
    }
    if (sscanf_s(agentPort.c_str(), "%d", &agentPort_) < 1) {
        agentPort_ = DEFAULT_AGENT_PORT;
    }
    return InitAgentServer();
}

void DistributedAgent::Join()
{
    if (mpthCmdProcess_ != nullptr) {
        mpthCmdProcess_->join();
    }
}

int DistributedAgent::OnNotify(const std::string &notifyType, const std::string &msg, int msgLen)
{
    if (strcmp(notifyType.c_str(), "testcasename")) {
        HiLog::Error(DistributedAgent::LABEL, "onNotify: %s.\n", msg.c_str());
    }
    if (msgLen < 0) {
        HiLog::Error(DistributedAgent::LABEL, "msgLen < 0.");
    }
    return 0;
}

int DistributedAgent::Stop()
{
    return 0;
}
} // namespace DistributeSystemTest
} // namespace OHOS
