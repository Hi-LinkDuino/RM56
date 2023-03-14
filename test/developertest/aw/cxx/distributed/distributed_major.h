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

#ifndef AW_CXX_DISTRIBUTED_DISTRIBUTED_MAJOR_H_
#define AW_CXX_DISTRIBUTED_DISTRIBUTED_MAJOR_H_

#include <vector>
#include <map>
#include <cstdio>
#include <iostream>
#include <thread>

#include <sys/socket.h>
#include "securec.h"

#include "distributed.h"
#include "distributed_cfg.h"

#include "gtest/gtest.h"

namespace OHOS {
namespace DistributeSystemTest {

class DistributeTestEnvironment : public testing::Environment {
public:
    DistributeTestEnvironment();
    explicit DistributeTestEnvironment(std::string cfgFile);
    virtual ~DistributeTestEnvironment();

    bool RunTestCmd(size_t devNo, const std::string &strCommand, int cmdLen, const std::string &strExpectValue,
        int expectValueLen, std::function<bool(const std::string &, int)> onProcessReturn);

    bool SendMessage(size_t devNo, const std::string &strMsg, int msgLen,
        std::function<bool(const std::string &, int)> onProcessReturnMsg) ;

    bool Notify(size_t devNo, const std::string &strMsg, int msgLen);

    virtual void SetUp();
    virtual void TearDown();

    void Init(std::string fileName);

    std::vector<DistDeviceInfo> clientList_;

private:
    DistributedCfg clientCfg_;
    int serverPort_;

    int AddClient(std::string ipAddr);
    int ConnectAgent(size_t devNo);
    int GetSerial();
    bool SendToAgent(size_t devNo, int cmdType, void *pstrMsg, int len,
        std::function<bool(const std::string &, int)> onProcessReturn);

};

extern DistributeTestEnvironment  *g_pDistributetestEnv;
class DistributeTest : public testing::Test {
public:
    DistributeTest();
    virtual ~DistributeTest();

protected:
    bool RunCmdOnAgent(AGENT_NO devNo, const std::string &strCommand, int cmdLen,
            const std::string &strExpectValue, int expectValueLen);

    bool RunCmdOnAgent(AGENT_NO devNo, const std::string &strCmd, const std::string &strArgs,
        const std::string &strExpectValue);
    bool RunCmdOnAgent(AGENT_NO devNo, const std::string &strCmd, const std::string &strArgs,
        const std::string &strExpectValue, std::function<bool(const std::string &, int)> onReturnCall);

    bool SendMessage(AGENT_NO devNo, const std::string &msg, int len);
    bool SendMessage(AGENT_NO devNo, const std::string &msg, int len,
        std::function<bool(const std::string &, int)> onProcessReturnMsg);
    bool Notify(AGENT_NO devNo, const std::string &notifyType, const std::string &msg, int msgLen);
    bool NotifyTestInfo(AGENT_NO devNo, const std::string &status);

    // monitor the status of Agent
    int CheckStatus();
    int GetReturnVal();

private:
    void OnProcessNotify() {}
    int returnVal_;

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    // init network environment, obtain all the address of agent and connect
    int InitEnv(std::vector<std::string> clientList);

    virtual void SetUp() {}
    virtual void TearDown() {}

    virtual void OnNotify() {}
    virtual bool OnProcessValue(const std::string &szbuf, int len);
    virtual bool OnMsgProc(const std::string &szbuf, int len);
};
} // namespace DistributeSystemTest
} // namespace OHOS

#endif // AW_CXX_DISTRIBUTED_DISTRIBUTED_MAJOR_H_
