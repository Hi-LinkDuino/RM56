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

#include <iostream>

#include "distributed_major.h"

#include "app_types.h"
#include "refbase.h"
#include "app_kvstore.h"
#include "app_distributed_kv_data_manager.h"
#include "hilog/log.h"


using namespace std;
using namespace OHOS;
using namespace testing::ext;
using namespace OHOS::DistributeSystemTest;
using namespace OHOS::HiviewDFX;
using namespace OHOS::AppDistributedKv;

namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, 0, "DistributeDemo"};
    const int SYNC_REC_TIME = 3;
    const int MSG_LENGTH = 100;
    const int EXPECT_RETURN_VALUE = 111;
    const int EXPECT_ADD_TEST_SUM = 500;
    const int SLEEP_TIME = 100000;
}

class DistributeDemo : public DistributeTest {
public:
    DistributeDemo() = default;
    ~DistributeDemo() = default;

    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}

    virtual void SetUp() {}
    virtual void TearDown() {}
};

/**
 * @tc.name: msgsendtest_001
 * @tc.desc: Verify the distributed test framework interface SendMessage.
 * @tc.type: FUNC
 * @tc.require: AR000CQGMV
 */
HWTEST_F(DistributeDemo, msgsendtest_001, TestSize.Level0)
{
    char msgbuf[MSG_LENGTH] = "I am testcase 1.";
    EXPECT_TRUE(SendMessage(AGENT_NO::ONE, msgbuf, MSG_LENGTH));
};

/**
 * @tc.name: msgsendtest_002
 * @tc.desc: Verify the distributed test framework interface SendMessage, Maximum data length.
 * @tc.type: FUNC
 * @tc.require: AR000CQGMV
 */
HWTEST_F(DistributeDemo, msgsendtest_002, TestSize.Level0)
{
    char msgbuf[MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN] = "I am testcase 2.";
    EXPECT_TRUE(SendMessage(AGENT_NO::ONE, msgbuf, MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN));
};

/**
 * @tc.name: msgsendtest_003
 * @tc.desc: Verify the distributed test framework interface SendMessage,
 *           Verify the data maximum length of the sent message interface .
 * @tc.type: FUNC
 * @tc.require: AR000CQGMV
 */
HWTEST_F(DistributeDemo, msgsendtest_003, TestSize.Level0)
{
    char msgbuf[MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN + 1] = "too big message.";
    EXPECT_FALSE(SendMessage(AGENT_NO::ONE, msgbuf, MAX_BUFF_LEN - DST_COMMAND_HEAD_LEN + 1));
};

/**
 * @tc.name: msgsendtest_004
 * @tc.desc: Verify the distributed test framework interface SendMessage, constains callback.
 * @tc.type: FUNC
 * @tc.require: AR000CQGMV
 */
HWTEST_F(DistributeDemo, msgsendtest_004, TestSize.Level0)
{
    std::string msgBuf = "recall function message test.";
    int ret = SendMessage(AGENT_NO::ONE, msgBuf, MSG_LENGTH,
        [&](const std::string &szreturnbuf, int rlen)->bool {
        std::string szbuf = "I get recall message.";
        EXPECT_TRUE(szbuf == szreturnbuf);
        HiLog::Info(LABEL, "msgsendtest_004 strncmp=%s", szbuf.c_str());
        return true;
    });
    EXPECT_TRUE(ret > 0);
};

/**
 * @tc.name: runcmdonagent_001
 * @tc.desc: Verify the distributed test framework interface RunCmdOnAgent.
 * @tc.type: FUNC
 * @tc.require: AR000CQGMV
 */
HWTEST_F(DistributeDemo, runcmdonagent_001, TestSize.Level0)
{
    std::string command = "query_command";
    std::string cmdArgs = "query a name?";
    std::string expectValue = "111";
    RunCmdOnAgent(AGENT_NO::ONE, command, cmdArgs, expectValue);
    EXPECT_EQ(GetReturnVal(), EXPECT_RETURN_VALUE);
};

/**
 * @tc.name: runcmdonagent_002
 * @tc.desc: Verify the distributed test framework interface RunCmdOnAgent, Using command map.
 * @tc.type: FUNC
 * @tc.require: AR000CQGMV
 */
HWTEST_F(DistributeDemo, runcmdonagent_002, TestSize.Level0) {
    std::string command = "add_for_two_int";
    std::string cmdArgs = "200 300";
    std::string expectValue = "500";
    RunCmdOnAgent(AGENT_NO::ONE, command, cmdArgs, expectValue);
    EXPECT_EQ(GetReturnVal(), EXPECT_ADD_TEST_SUM);
};

/**
 * @tc.name: getkvstore_001
 * @tc.desc: Verify distributed data read and write and synchronization.
 * @tc.type: FUNC
 * @tc.require: AR000CQGMV
 */
HWTEST_F(DistributeDemo, getkvstore_001, TestSize.Level0) {
    /**
     * @tc.steps: step1.init appkvstore, Get the kvstore pointer for the specified appId and storeId
     */
    static std::shared_ptr<AppDistributedKvDataManager> manager;
    Options options;
    options.createIfMissing = true;
    options.encrypt = false;
    options.persistent = true;
    std::string appId = "com.ohos.nb.service.user1_test";
    std::string storeId = "student_1";
    manager = AppDistributedKvDataManager::GetInstance(appId, "/data/test");
    std::unique_ptr<AppKvStore> kvStorePtr;
    Status status = manager->GetKvStore(options, storeId, [&](std::unique_ptr<AppKvStore> kvStore) {
        kvStorePtr = std::move(kvStore);
    });
    if (!kvStorePtr) {
        HiLog::Error(LABEL, "client get kvStorePtr error.");
    }

    ASSERT_TRUE(kvStorePtr != nullptr);

    /**
     * @tc.steps: step2.Write specific K and V values to the database
     */
    if (status == Status::SUCCESS) {
        WriteOptions writeOptions;
        writeOptions.local = false;
        Key key("student_name_user1");
        Value value = "good boy.";
        kvStorePtr->Put(writeOptions, key, value);
    }

    /**
     * @tc.steps: step3.Synchronize data to a specific remote device
     */
    std::vector<std::string> deviceIdList;
    deviceIdList.push_back("192.168.43.200");
    std::map<std::string, Status> sync_map;
    Status syncinfo = kvStorePtr->Sync(deviceIdList, SyncMode::PUSH,
                            [&](const std::map<std::string, Status> &syncMapinfo){
                                sync_map = std::move(syncMapinfo);
                            });
    if (syncinfo == Status::SUCCESS) {
        HiLog::Info(LABEL, "agent sync success");
    }

    /**
     * @tc.steps: step4.Judge if the data is successfully synchronized to the remote device
     */
    int n = 0;
    while (n < SYNC_REC_TIME) {
        std::string id = "";
        for (auto mapstatus: sync_map) {
            HiLog::Info(LABEL, "client map_id==%s", mapstatus.first.c_str());
            if (mapstatus.second == Status::SUCCESS) {
                id = mapstatus.first;
                HiLog::Info(LABEL, "real client Sync success.");
            } else {
                HiLog::Info(LABEL, "mapstatus.second=%d.", (int)mapstatus.second);
            }
            n++;
        }
        if (id != "") {
            break;
        }
        usleep(SLEEP_TIME);
    }
    if (syncinfo == Status::SUCCESS) {
        HiLog::Info(LABEL, "client sync success.");
    }

    /**
     * @tc.steps: step5.Query whether the value inserted in the local database is correct
     */
    std::string command = "getkvstore";
    std::string cmdArgs = "student_name_user1";
    std::string expectValue = "good boy.";
    ReadOptions readOptions;
    readOptions.local = false;
    Key key(cmdArgs);
    Value getValue;
    Status getTstatus = kvStorePtr->Get(readOptions, key, getValue);
    if (getTstatus == Status::SUCCESS) {
        HiLog::Info(LABEL, "Get Value SUCCESS, key=%s.", key.ToString().c_str());
    } else {
        HiLog::Error(LABEL, "Get Value Failed.");
    }

    /**
     * @tc.steps: step6.Send a command to the remote device to query the database with parameters and
     *                  expected return values
     */
    RunCmdOnAgent(AGENT_NO::ONE, command, cmdArgs, expectValue);

    /**
     * @tc.steps: step7.Judge the return value
     * @tc.expected: step7. The remote device queried the data successfully.
     */
    EXPECT_TRUE(GetReturnVal() == EXPECT_RETURN_VALUE);
};

int main(int argc, char *argv[])
{
    g_pDistributetestEnv = new DistributeTestEnvironment("major.desc");
    testing::AddGlobalTestEnvironment(g_pDistributetestEnv);
    testing::GTEST_FLAG(output) = "xml:./";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
