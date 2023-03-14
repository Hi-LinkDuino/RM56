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

#ifndef AW_CXX_DISTRIBUTED_DISTRIBUTED_AGENT_H_
#define AW_CXX_DISTRIBUTED_DISTRIBUTED_AGENT_H_

#include <map>
#include <vector>
#include <thread>
#include <iostream>

#include "distributed.h"
#include "distributed_cfg.h"

#include "hilog/log.h"

#include "gtest/gtest.h"

namespace OHOS {
namespace DistributeSystemTest {
class DistributedAgent {
public:
    DistributedAgent();
    virtual ~DistributedAgent();
    // deal with testcase logic by struct or xml command.
    int OnProcessCmd(const std::string &strCommand, int cmdLen, const std::string &strExpectValue, int expectValueLen);
    int Start(std::string cfgFile);
    int Stop();
    void Join();
    static const int RECE_LEN = 10;
    static const int CMD_LENGTH = 50;
    static const int HALF_NUM = 2;
    static constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DistributedAgent"};

protected:
    virtual bool SetUp();
    virtual bool TearDown();
    virtual void OnLocalInit() {}
    virtual int OnProcessCmd(const std::string &strCommand, int cmdLen, const std::string &strArgs, int argsLen,
        const std::string &strExpectValue, int expectValueLen);
    virtual int OnProcessMsg(const std::string &strMsg, int msgLen, std::string &strReturnValue, int returnBufLen);
    virtual int OnNotify(const std::string &notifyType, const std::string &msg, int msgLen);
    virtual void OnNotifyImf(DistributedMsg *pcline);

private:
    int agentPort_;
    int clientSockFd_;
    std::string agentIpAddr_;
    std::unique_ptr<std::thread> mpthCmdProcess_;
    bool mbStop_;
    DistributedCfg agentCfg_;
    // init testcase agent to start agent server, only one client connected.
    int InitAgentServer();
    // response the messages from client.
    int DoCmdServer(int serverSockFd);
};
} // namespace DistributeSystemTest
} // namespace OHOS

#endif // AW_CXX_DISTRIBUTED_DISTRIBUTED_AGENT_H_

