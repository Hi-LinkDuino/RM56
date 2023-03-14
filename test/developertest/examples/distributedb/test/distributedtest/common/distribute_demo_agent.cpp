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

#include <string>
#include <iostream>

#include "app_types.h"
#include "refbase.h"
#include "app_kvstore.h"
#include "app_distributed_kv_data_manager.h"
#include "hilog/log.h"

using namespace testing;
using namespace OHOS;
using namespace OHOS::DistributeSystemTest;
using namespace OHOS::HiviewDFX;
using namespace OHOS::AppDistributedKv;

namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, 0, "DistributeDemoAgent"};
    std::shared_ptr<AppDistributedKvDataManager> g_manager;
    std::string g_appId = "com.ohos.nb.service.user1_test";
    std::string g_storeId = "student_1";
    std::unique_ptr<AppKvStore> g_kvStorePtr;
    const int CMD_LEN = 4;
    const int RETURN_HALF = 2;
    const int MSG_CALL_LEN = 6;
    const int CMD_RETURN_TWO = 111;
}

class DistributeDemoAgent : public DistributedAgent {
public:
    DistributeDemoAgent();
    ~DistributeDemoAgent();

    virtual bool SetUp();
    virtual bool TearDown();
    virtual int OnProcessMsg(const std::string &strMsg, int len, std::string &strReturnValue, int returnBufLen);
    virtual int OnProcessCmd(const std::string &strCommand, int cmdLen, const std::string &strArgs, int argsLen,
        const std::string &strExpectValue, int expectValueLen);

    int GetKvValue(const std::string &strArgs, int argsLen, const std::string &strExpectValue,
        int expectValueLen);
    int AddTwoValue(const std::string &strArgs, int argsLen, const std::string &strExpectValue,
        int expectValueLen);
    int ProcessByUseMap(const std::string &strCommand, int cmdLen, const std::string &strArgs, int argsLen,
        const std::string &strExpectValue, int expectValueLen);

private:
    typedef	int	(DistributeDemoAgent::*self_func)(const std::string &, int, const std::string &, int);
    std::map<std::string, self_func> cmdFunMap_;
};

DistributeDemoAgent::DistributeDemoAgent()
{
}
DistributeDemoAgent::~DistributeDemoAgent()
{
}

bool DistributeDemoAgent::SetUp()
{
    /**
     * @tc.setup: Initialize test agent, start data management service
     */
    cmdFunMap_["getkvstore"] = &DistributeDemoAgent::GetKvValue;
    cmdFunMap_["add_for_two_int"] = &DistributeDemoAgent::AddTwoValue;
    g_manager = AppDistributedKvDataManager::GetInstance(g_appId, "/data/test");
    Options options;
    options.createIfMissing = true;
    options.encrypt = false;
    options.persistent = true;
    std::string storeId = g_storeId;
    Status status = g_manager->GetKvStore(options, storeId, [&](std::unique_ptr<AppKvStore> kvStore) {
        g_kvStorePtr = std::move(kvStore);
    });
    if (status == Status::SUCCESS) {
        return true;
    }
    return false;
}

bool DistributeDemoAgent::TearDown()
{
    /**
     * @tc.teardown: Recovery test agent device environment
     */
    return true;
}

// The entry of handlingthe major test case message
int DistributeDemoAgent::OnProcessMsg(const std::string &strMsg, int len,
    std::string &strReturnValue, int returnBufLen)
{
    int nret = 0;
    std::string returnStr = "agent return message.";
    if ((len > CMD_LEN) && (strMsg.find("\0\1\0\1") == 1)) {
        for (int i = 0; i < returnBufLen; i++) {
            strReturnValue  += std::to_string((i + 1) % RETURN_HALF);
        }
        nret = returnBufLen;
    } else {
        HiLog::Info(LABEL, "receive message=%s.", strMsg.c_str());
        if (!strncmp(strMsg.c_str(), "recall", MSG_CALL_LEN)) {
            returnStr = "I get recall message.";
            int ptrlen = returnStr.size();
            if (ptrlen > returnBufLen) {
                ptrlen = returnBufLen - 1;
            }
            strReturnValue = returnStr;
            nret = ptrlen;
        } else {
            nret =  DistributedAgent::OnProcessMsg(strMsg, len, strReturnValue, returnBufLen);
        }
    }
    return nret;
}

// The entry of handling the major test case command
int DistributeDemoAgent::OnProcessCmd(const std::string &strCommand, int cmdLen, const std::string &strArgs,
    int argsLen, const std::string &strExpectValue, int expectValueLen)
{
    if (strCommand == "query_command") {
        if (strArgs == "query a name?") {
            return CMD_RETURN_TWO;
        }
    }
    return DistributeDemoAgent::ProcessByUseMap(strCommand, cmdLen, strArgs, argsLen, strExpectValue, expectValueLen);
}

// Handling major test case command: getkvstore
int DistributeDemoAgent::GetKvValue(const std::string &strArgs, int argsLen, const std::string &strExpectValue,
    int expectValueLen)
{
    if (!g_kvStorePtr || (int)strArgs.length() > argsLen) {
        HiLog::Error(LABEL, "[ohos-test-fm]agent ERROR.");
        return -1;
    }
    ReadOptions readOptions;
    readOptions.local = false;
    Key key(strArgs);
    Value getValue;
    Status status = g_kvStorePtr->Get(readOptions, key, getValue);
    if (status == Status::SUCCESS) {
        HiLog::Info(LABEL, "Get Value SUCCESS, key=%s.", key.ToString().c_str());
    } else {
        HiLog::Error(LABEL, "Get Value Failed.");
    }
    std::vector<Entry> allEntry;
    Status entryStatus = g_kvStorePtr->GetEntries("", allEntry);
    if (entryStatus == Status::SUCCESS) {
        HiLog::Info(LABEL, "GetEntries SUCCESS.");
        for (auto entry : allEntry) {
            HiLog::Info(LABEL, "GetEntries key= %s.", entry.key.ToString().c_str());
        }
    }
    if (getValue.ToString() == strExpectValue) {
        if (expectValueLen < (int)getValue.ToString().length()) {
            HiLog::Error(LABEL, "[ohos-test-fm]agent end xxx.");
            return -1;
        }
        HiLog::Error(LABEL, "[ohos-test-fm]agent end 200.");
        return CMD_RETURN_TWO;
    }
    return -1;
}

// Handling major test case command: add_for_two_int
int DistributeDemoAgent::AddTwoValue(const std::string &strArgs, int argsLen, const std::string &strExpectValue,
    int expectValueLen)
{
    int val1 = 0;
    int val2 = 0;
    int val3 = 0;
    int ret1;
    int ret2;
    ret1 = sscanf_s(strArgs.c_str(), "%d %d", &val1, &val2);
    if (ret1 <=0 || ret1 == 1) {
        HiLog::Error(LABEL, "sscanf_s failed. ret1 != 2");
        return -1;
    }
    ret2 = sscanf_s(strExpectValue.c_str(), "%d", &val3);
    if (ret2 != 1) {
        HiLog::Error(LABEL, "sscanf_s failed. ret2 != 1");
        return -1;
    }
    EXPECT_TRUE(val3 == (val1 + val2));
    return val1 + val2;
}

// Handling major test cases through already registered commands and function mappings
int DistributeDemoAgent::ProcessByUseMap(const std::string &strCommand, int cmdLen, const std::string &strArgs,
    int argsLen, const std::string &strExpectValue, int expectValueLen)
{
    std::map<std::string, self_func>::iterator ite = cmdFunMap_.find(strCommand);
    if (ite != cmdFunMap_.end()) {
        self_func callProcCmd = cmdFunMap_[strCommand];
        if (callProcCmd != nullptr) {
            return (this->*callProcCmd)(strArgs, argsLen, strExpectValue, expectValueLen);
        } else {
            return DistributedAgent::OnProcessCmd(strCommand,
                                                  cmdLen,
                                                  strArgs,
                                                  argsLen,
                                                  strExpectValue,
                                                  expectValueLen);
        }
    }
    return -1;
}

int main()
{
    // Test agent main function
    DistributeDemoAgent obj;
    if (obj.SetUp()) {
        obj.Start("agent.desc");
        obj.Join();
    } else {
        HiLog::Error(LABEL, "Init environment failed.");
    }
    if (obj.TearDown()) {
        return 0;
    } else {
        HiLog::Error(LABEL, "Clear environment failed.");
        return -1;
    }
}