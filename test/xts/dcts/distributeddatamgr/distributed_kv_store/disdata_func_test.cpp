/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#define LOG_TAG "disDataTest"

#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <iostream>
#include <iterator>
#include <sstream> // 使用stringstream
#include <string>
#include <unistd.h>
#include <vector>

#include "distributed_kv_data_manager.h"
#include "log_print.h"
#include "shm_utils.h"
#include "types.h"

typedef enum {
    CTRL_CODE_SOFTBUS_TYPE = 1000,
    CTRL_CODE_SOFTBUS_OPEN_SESSION,
    CTRL_CODE_DATAMGR_TYPE = 2000,
    CTRL_CODE_DATAMGR_GET_DATA,
    CTRL_CODE_DATAMGR_GET_DATA_REPLY,
    CTRL_CODE_DATAMGR_CREATE_KV,
    CTRL_CODE_DATAMGR_DELETE_KV,
    CTRL_CODE_DATAMGR_PUT_DATA,
    CTRL_CODE_DATAMGR_DELETE_DATA,
    CTRL_CODE_DM_TYPE = 3000,
    CTRL_CODE_FILEMGR_TYPE = 4000,
    CTRL_CODE_DP_TYPE = 5000,
    CTRL_CODE_SEC_TYPE = 6000,
    CTRL_CODE_MEDIA_TYPE = 7000,
    CTRL_CODE_RESULT_TYPE = 9999,
} CtrlCodeType;

const int MAX_DATA_LENGTH = 1024;
const double DEFDELTA = 0.00001;
const int CODE_LEN_TEN = 10;

using namespace testing::ext;
using namespace OHOS::DistributedKv;
namespace {
const int USLEEP_TIME = 2000000;
}

class DeathRecipient : public KvStoreDeathRecipient {
public:
    DeathRecipient() {}
    virtual ~DeathRecipient() {}
    virtual void OnRemoteDied() override
    {
        std::cout << "server is dead" << std::endl;
    }
};

class DistributedKvDataManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static const uint32_t CHECK_WAITING_TIME = 50000; // 50ms
    static void RemoveAllStore(DistributedKvDataManager manager);

    DistributedKvDataManagerTest();
};

class DisKvTest {
public:
    static DistributedKvDataManager manager;
    static std::shared_ptr<SingleKvStore> KvStorePtr; // declare kvstore instance.
    static Status statusGetKvStore;
    static Status statusCloseKvStore;
    static Status statusDeleteKvStore;
    static UserId userId;
    static AppId appId;
    static StoreId storeIdTest;
};

DistributedKvDataManager DisKvTest::manager;
std::shared_ptr<SingleKvStore> DisKvTest::KvStorePtr = nullptr; // declare kvstore instance.
Status DisKvTest::statusGetKvStore = Status::ERROR;
Status DisKvTest::statusCloseKvStore = Status::ERROR;
Status DisKvTest::statusDeleteKvStore = Status::ERROR;
UserId DisKvTest::userId;
AppId DisKvTest::appId;
StoreId DisKvTest::storeIdTest;

void DistributedKvDataManagerTest::SetUpTestCase(void)
{
    // 连接共享内存
    createShm(SHM_SEND_KEY);
}

void DistributedKvDataManagerTest::TearDownTestCase(void)
{
    // 解关联共享内存
    disconnectShm();
}

void DistributedKvDataManagerTest::SetUp(void)
{
    DisKvTest::userId.userId = "account0";
    DisKvTest::appId.appId = "com.ohos.kvdatamanager3.test";
    DisKvTest::storeIdTest.storeId = "test3";

    Options options { .createIfMissing = true,
        .encrypt = false, //   .persistant = true,
        .autoSync = false,
        .backup = false,
        .kvStoreType = KvStoreType::SINGLE_VERSION };

    // 1.本地删除数据库
    RemoveAllStore(DisKvTest::manager);
    // 2.远端删除数据库
    char* str = (char*)malloc(MAX_DATA_LENGTH);
    if (str == nullptr) {
        std::cout << "ERROR: str malloc failed" << std::endl;
        return;
    }
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(str, strlen("") + 1, "");
    writeCodeDataToShm(CTRL_CODE_DATAMGR_DELETE_KV, str);

    char code[CODE_LEN_TEN] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    if (str == nullptr) {
        return;
    }

    if (strcpy_s(code, strlen(str) + 1, str) != EOK) {
        return;
    }
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    free(str);

    // 1.本地创建数据库
    auto deathRecipient = std::make_shared<DeathRecipient>();
    DisKvTest::manager.RegisterKvStoreServiceDeathRecipient(deathRecipient);
    DisKvTest::statusGetKvStore = DisKvTest::manager.GetSingleKvStore(
        options, { DisKvTest::appId }, { DisKvTest::storeIdTest }, DisKvTest::KvStorePtr);

    // 2. 远端创建数据库
    str = (char*)malloc(MAX_DATA_LENGTH);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(str, strlen("") + 1, "");
    writeCodeDataToShm(CTRL_CODE_DATAMGR_CREATE_KV, str);

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    if (memset_s(code, strlen(code), 0, CODE_LEN_TEN) != EOK) {
        return;
    }
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    if (str == nullptr) {
        return;
    }
    if (strcpy_s(code, strlen(str) + 1, str) != EOK) {
        return;
    }
    waitDataWithCode(code, str);
    free(str);
}
void DistributedKvDataManagerTest::TearDown(void) {}
void DistributedKvDataManagerTest::RemoveAllStore(DistributedKvDataManager manager)
{
    DisKvTest::statusCloseKvStore = DisKvTest::manager.CloseAllKvStore(DisKvTest::appId);
    DisKvTest::statusDeleteKvStore = DisKvTest::manager.DeleteAllKvStore(DisKvTest::appId);
}
DistributedKvDataManagerTest::DistributedKvDataManagerTest(void) {}

class KvStoreSyncCallbackTestImpl : public KvStoreSyncCallback {
public:
    void SyncCompleted(const std::map<std::string, Status>& results);
};

void KvStoreSyncCallbackTestImpl::SyncCompleted(const std::map<std::string, Status>& results)
{
    std::cout << "KvStoreSyncCallbackTestImpl timestamp:" << std::endl;
    for (const auto& result : results) {
        std::cout << result.first << " " << static_cast<int>(result.second) << std::endl;
    }
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Push_0100
 *  @tc.name: DistribitedKvDataManagerTest  sync push
 *  @tc.desc: sync push, put int data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Push_0100, TestSize.Level1 | Function | MediumTest)
{
    ZLOGI("DistribitedKvDataManager_Sync_Push_0100 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 本地数据库添加数据 100
    std::string stringKey = "math_score_int";
    Key keyInt = stringKey;
    Value valueInt = Value(TransferTypeToByteArray<int>(100));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        LOG("start sync device.deviceId = %s", device.deviceId.c_str());
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    char strKV[MAX_DATA_LENGTH] = { "math" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_int:100") + 1, "math_score_int:100");
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV); // 2001:math_score_int:10
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "int 100 get result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Push_0200
 *  @tc.name: DistribitedKvDataManagerTest  sync push
 *  @tc.desc: sync push, put float data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Push_0200, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Push_0200 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";

    // 本地数据库添加数据 3.14f
    std::string stringKey = "math_score_float";
    Key keyInt = stringKey;

    Value valueInt = Value(TransferTypeToByteArray<float>(3.14f));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);
    char strKV[MAX_DATA_LENGTH] = { "math" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_float:3.14") + 1, "math_score_float:3.14");
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV); // 2001:math_score_int:10
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " float 3.14f get result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Push_0300
 *  @tc.name: DistribitedKvDataManagerTest  sync push
 *  @tc.desc: sync push, put double data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Push_0300, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Push_0300 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 本地数据库添加数据 28.288f
    std::string stringKey = "math_score_double";
    Key keyInt = stringKey;
    Value valueInt = Value(TransferTypeToByteArray<double>(28.288f));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);
    // 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    char strKV[MAX_DATA_LENGTH] = { "math" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_double:28.288") + 1, "math_score_double:28.288");

    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV); // 2001:math_score_int:10
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "<double>(28.288f) get result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Push_0400
 *  @tc.name: DistribitedKvDataManagerTest  sync push
 *  @tc.desc: sync push, put int64_t data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Push_0400, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Push_0400 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";

    // 本地数据库添加数据 12345678
    Key keyInt = "math_score_int64_t";
    Value valueInt = Value(TransferTypeToByteArray<int64_t>(12345678));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 数据发送到远端并等待返回结果
    char strKV[MAX_DATA_LENGTH] = { "math" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_int64_t:12345678") + 1, "math_score_int64_t:12345678");
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV); // 2001:math_score_int:10
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " <int64_t>(12345678) get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Push_0500
 *  @tc.name: DistribitedKvDataManagerTest  sync push
 *  @tc.desc: sync push, put size_t data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Push_0500, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Push_0500 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 本地数据库添加数据 28
    Key keyInt = "math_score_size_t";
    Value valueInt = Value(TransferTypeToByteArray<size_t>(28));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 数据发送到远端并等待返回结果
    char strKV[MAX_DATA_LENGTH] = { "math" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_size_t:28") + 1, "math_score_size_t:28");

    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV); // 2001:math_score_int:10
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " <size_t>(28) get result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Push_0600
 *  @tc.name: DistribitedKvDataManagerTest  sync push
 *  @tc.desc: sync push, put string data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Push_0600, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Push_0600 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 本地数据库添加数据string
    Key keyInt = "math_score_string";
    Value valueInt = Value("{\"class\":20, \"age\":18, \"gradle\":\"good\"}");
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 数据发送到远端并等待返回结果
    char str[MAX_DATA_LENGTH] = { "math_score_string:{\"class\":20, \"age\":18, \"gradle\":\"good\"}" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, str); // 2001:math_score_int:10
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "string get result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Push_0700
 *  @tc.name: DistribitedKvDataManagerTest  sync push
 *  @tc.desc: sync push, put vector<uint8_t> data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Push_0700, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Push_0700 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 本地数据库添加数据 vector<int>
    std::string stringKey = "math_score_vector";
    Key keyInt = stringKey;
    std::vector<uint8_t> vect = { 0, 1, 2, 3, 4, 5, 6, 7 };
    Value valueInt = Value(vect);
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    usleep(USLEEP_TIME);

    // 数据发送到远端并等待返回结果
    std::string strvc;
    strvc.assign(vect.begin(), vect.end());

    char strKV[MAX_DATA_LENGTH] = { "math_score_int" };
    strcpy_s(strKV, strlen(stringKey.c_str()) + 1, stringKey.c_str());
    strcat(strKV, ":");
    strcat(strKV, strvc.c_str());

    std::cout << "strvc = " << strvc << std::endl;
    std::cout << "strvc.c_str() = " << strvc.c_str() << std::endl;
    std::cout << "strKV = " << strKV << std::endl;

    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "vector<int> get result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Pull_0100
 *  @tc.name: remote put data, local sync pull
 *  @tc.desc: sync pull, put int data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Pull_0100, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Pull_0100 begin.");
    // 1.本端创建KV
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";

    // 2.创建远端KV
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_int";
    Value valueInt = Value(TransferTypeToByteArray<int>(100));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端修改数据200
    char strKV[MAX_DATA_LENGTH] = { "math" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_int:200") + 1, "math_score_int:200");
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " put-200  get result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地pull远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);
    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PULL); // PUSH_PULL
    EXPECT_EQ(status, Status::SUCCESS);

    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据 与200比较
    usleep(USLEEP_TIME);

    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get("math_score_int", valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    int aaa = TransferByteArrayToType<int>(valueRetInt.Data());
    EXPECT_EQ(aaa, 200);

    // 8.数据发送到远端并等待返回结果
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " get-200 get result=" << str << std::endl;

    // 检查远端是否返回成功
    ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Pull_0200
 *  @tc.name: remote put data, local sync pull
 *  @tc.desc: sync pull, put float data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Pull_0200, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Pull_0200 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    std::string stringKey = "math_score_float";
    Key keyInt = stringKey;
    Value valueInt = Value(TransferTypeToByteArray<float>(3.14f));

    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端修改数据200
    char strKV[MAX_DATA_LENGTH] = { "math" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_float:9.99") + 1, "math_score_float:9.99");
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "put-9.99 get result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地pull远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);
    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PULL);
    EXPECT_EQ(status, Status::SUCCESS);

    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据 与200比较
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    float aaa = TransferByteArrayToType<float>(valueRetInt.Data());
    float delta = aaa - 9.99f;
    EXPECT_LE(std::abs(delta), DEFDELTA);

    // 8.数据发送到远端并等待返回结果
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "get-9.99 get result=" << str << std::endl;
    // 检查远端是否返回成功
    ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Pull_0300
 *  @tc.name: remote put data, local sync pull
 *  @tc.desc: sync pull, put double data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Pull_0300, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Pull_0300 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 2.创建远端Lv
    // 3. 本地数据库添加数据28.288f
    std::string stringKey = "math_score_double";
    Key keyInt = stringKey;
    Value valueInt = Value(TransferTypeToByteArray<double>(28.288));

    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);
    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);
    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端修改数据
    char strKV[MAX_DATA_LENGTH] = { "math_score_double:999.999" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "put double get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
    // 6.本地pull远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);
    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PULL);
    EXPECT_EQ(status, Status::SUCCESS);

    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据 与200比较
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    double aaa = TransferByteArrayToType<double>(valueRetInt.Data());
    double delta = aaa - 999.999;
    std::cout << "aaa = " << aaa << std::endl;
    std::cout << "delta = " << delta << std::endl;
    EXPECT_LE(std::abs(delta), DEFDELTA);

    // 8.数据发送到远端并等待返回结果
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "put double get result=" << str << std::endl;

    // 检查远端是否返回成功
    ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Pull_0400
 *  @tc.name: remote put data, local sync pull
 *  @tc.desc: sync pull, put int64_t data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Pull_0400, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Pull_0400 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_int64_t";
    Value valueInt = Value(TransferTypeToByteArray<int64_t>(12345678));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端修改数据200
    char strKV[MAX_DATA_LENGTH] = { "math" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_int64_t:99889988") + 1, "math_score_int64_t:99889988");

    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "put-200  get result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地pull远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);
    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }
    // 7.本地get数据 与200比较
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    int64_t aaa = TransferByteArrayToType<int64_t>(valueRetInt.Data());
    EXPECT_EQ(aaa, 99889988u);

    // 8.数据发送到远端并等待返回结果
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "put int64_t get result=" << str << std::endl;
    // 检查远端是否返回成功
    ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Pull_0500
 *  @tc.name: remote put data, local sync pull
 *  @tc.desc: sync pull, put size_t data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Pull_0500, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Pull_0100 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";

    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_size_t";
    Value valueInt = Value(TransferTypeToByteArray<size_t>(28));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);
    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端修改数据200
    char strKV[MAX_DATA_LENGTH] = { "math" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_size_t:88") + 1, "math_score_size_t:88");
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "size_t get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地pull远端数据
    // std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    // std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据 与200比较
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    size_t aaa = TransferByteArrayToType<size_t>(valueRetInt.Data());
    EXPECT_EQ(aaa, 88u);

    // 8.数据发送到远端并等待返回结果
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "get size_t  result=" << str << std::endl;
    // 检查远端是否返回成功
    ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Pull_0600
 *  @tc.name: remote put data, local sync pull
 *  @tc.desc: sync pull, put string data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Pull_0600, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Pull_0600 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";

    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_string";
    Value valueInt = Value("{\"class\":20, \"age\":18, \"gradle\":\"good\"}");
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端修改数据200
    char strPut[MAX_DATA_LENGTH] = { "math_score_string:{\"class\":88, \"age\":99, \"gradle\":\"QQWWQQ\"}" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strPut);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "put string result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地pull远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据 与200比较
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    std::string aaa = valueRetInt.ToString();
    std::string sstring = "{\"class\":88, \"age\":99, \"gradle\":\"QQWWQQ\"}";
    EXPECT_EQ(aaa, sstring);

    // 8.数据发送到远端并等待返回结果
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strPut);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "get string  result=" << str << std::endl;

    // 检查远端是否返回成功
    ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_Pull_0700
 *  @tc.name: remote put data, local sync pull
 *  @tc.desc: sync pull, put vector<uint8_t> data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_Pull_0700, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_Pull_0700 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";

    // 2.创建远端Lv
    // 3.本地数据库添加数据 vector<int>
    std::string stringKey = "math_score_vector";
    Key keyInt = stringKey;
    std::vector<uint8_t> vect = { 0, 1, 2, 3, 4, 5, 6, 7 };
    Value valueInt = Value(vect);
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端修改数据200

    std::vector<uint8_t> vect2 = { 9, 9, 8, 8, 7, 7, 6, 6 };
    std::string strvc;
    strvc.assign(vect2.begin(), vect2.end());

    char strKV[MAX_DATA_LENGTH] = { "math_score_int" };
    strcpy_s(strKV, strlen(stringKey.c_str()) + 1, stringKey.c_str());
    strcat(strKV, ":");
    strcat(strKV, strvc.c_str());

    std::cout << "strvc = " << strvc << std::endl;
    std::cout << "strvc.c_str() = " << strvc.c_str() << std::endl;
    std::cout << "strKV = " << strKV << std::endl;

    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-vector<uint8_t>  get result=" << str << std::endl;

    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
    // 6.本地pull远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据 与200比较
    usleep(USLEEP_TIME);

    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    std::string strValueRet = valueRetInt.ToString();
    EXPECT_EQ(strvc, strValueRet.c_str());

    // 8.数据发送到远端并等待返回结果
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "put vector<uint8_t> get result=" << str << std::endl;

    // 检查远端是否返回成功
    ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_PUSH_PULL_0100
 *  @tc.name: remote delete data, local sync push_pull
 *  @tc.desc: sync push_pull ,delete int data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_PUSH_PULL_0100, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_PUSH_PULL_0100 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_int";
    Value valueInt = Value(TransferTypeToByteArray<int>(100));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端delete 数据
    char strKV[MAX_DATA_LENGTH] = { "math_score_int:100" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_DELETE_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " delete int result=" << str << std::endl;

    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地 push_pull 远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH_PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据失败
    usleep(USLEEP_TIME);

    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::KEY_NOT_FOUND);

    // 8.远端get数据失败
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);

    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " get int  result=" << str << std::endl;

    // 检查远端是否返回成功
    ret = strcmp(str, "1");
    EXPECT_EQ(ret, 0);

    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_PUSH_PULL_0200
 *  @tc.name: remote delete data, local sync push_pull
 *  @tc.desc: sync push_pull ,delete float data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_PUSH_PULL_0200, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_PUSH_PULL_0200 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_int";
    Value valueInt = Value(TransferTypeToByteArray<float>(3.14f));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端delete 数据
    char strKV[MAX_DATA_LENGTH] = { "math_score_int:100" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_DELETE_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " delete float get result=" << str << std::endl;
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地 push_pull 远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);
    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH_PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据失败
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::KEY_NOT_FOUND);

    // 8.远端get数据失败
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " get result=" << str << std::endl;

    // 检查远端是否返回成功
    ret = strcmp(str, "1");
    EXPECT_EQ(ret, 0);

    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_PUSH_PULL_0300
 *  @tc.name: remote delete data, local sync push_pull
 *  @tc.desc: sync push_pull ,delete double data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_PUSH_PULL_0300, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_PUSH_PULL_0300 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";

    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_int";
    Value valueInt = Value(TransferTypeToByteArray<double>(28.288f));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端delete 数据
    char strKV[MAX_DATA_LENGTH] = { "math_score_int:100" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_DELETE_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "delete double result=" << str << std::endl;

    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地 push_pull 远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }

    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH_PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据失败
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::KEY_NOT_FOUND);

    // 8.远端get数据失败
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "get result=" << str << std::endl;

    // 检查远端是否返回成功
    ret = strcmp(str, "1");
    EXPECT_EQ(ret, 0);

    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_PUSH_PULL_0400
 *  @tc.name: remote delete data, local sync push_pull
 *  @tc.desc: sync push_pull ,delete int64_t data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_PUSH_PULL_0400, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_PUSH_PULL_0400 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_int";
    Value valueInt = Value(TransferTypeToByteArray<int64_t>(12345678));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端delete 数据
    char strKV[MAX_DATA_LENGTH] = { "math_score_int:100" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_DELETE_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " delete int64_t result=" << str << std::endl;

    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地 push_pull 远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);
    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH_PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据失败
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::KEY_NOT_FOUND);
    // 8.远端get数据失败
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "get  result=" << str << std::endl;

    // 检查远端是否返回成功
    ret = strcmp(str, "1");
    EXPECT_EQ(ret, 0);

    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_PUSH_PULL_0500
 *  @tc.name: remote delete data, local sync push_pull
 *  @tc.desc: sync push_pull ,delete size_t data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_PUSH_PULL_0500, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_PUSH_PULL_0500 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_int";
    Value valueInt = Value(TransferTypeToByteArray<size_t>(28));
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端delete 数据
    char strKV[MAX_DATA_LENGTH] = { "math_score_int:100" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_DELETE_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " delete size_t result=" << str << std::endl;
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地 push_pull 远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH_PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据失败
    usleep(USLEEP_TIME);

    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::KEY_NOT_FOUND);

    // 8.远端get数据失败
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " get  result=" << str << std::endl;

    // 检查远端是否返回成功
    ret = strcmp(str, "1");
    EXPECT_EQ(ret, 0);

    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_PUSH_PULL_0600
 *  @tc.name: remote delete data, local sync push_pull
 *  @tc.desc: sync push_pull ,delete string  data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_PUSH_PULL_0600, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_PUSH_PULL_0600 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_int";
    Value valueInt = Value("{\"class\":20, \"age\":18, \"gradle\":\"good\"}");
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端delete 数据
    char strKV[MAX_DATA_LENGTH] = { "math_score_int:100" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_DELETE_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { 0 };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "delete string result=" << str << std::endl;
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地 push_pull 远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH_PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据失败
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::KEY_NOT_FOUND);
    // 8.远端get数据失败
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " get result=" << str << std::endl;

    // 检查远端是否返回成功
    ret = strcmp(str, "1");
    EXPECT_EQ(ret, 0);
    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: DistribitedKvDataManager_Sync_PUSH_PULL_0700
 *  @tc.name: remote delete data, local sync push_pull
 *  @tc.desc: sync push_pull ,delete vector<uint8_t>  data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, DistribitedKvDataManager_Sync_PUSH_PULL_0700, TestSize.Level1)
{
    ZLOGI("DistribitedKvDataManager_Sync_PUSH_PULL_0700 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 2.创建远端Lv
    // 3. 本地数据库添加数据 100
    Key keyInt = "math_score_int";
    std::vector<uint8_t> vect = { 0, 1, 2, 3, 4, 5, 6, 7 };
    Value valueInt = Value(vect);
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    EXPECT_EQ(Status::SUCCESS, status);

    // 4. sync push 同步数据到远端
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);
    std::vector<std::string> deviceList;
    for (const auto& device : remoteDevice) {
        std::cout << "start sync" << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH);
    ASSERT_EQ(status, Status::SUCCESS);

    // 5.远端delete 数据
    char strKV[MAX_DATA_LENGTH] = { "math_score_int:100" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_DELETE_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " delete vector result=" << str << std::endl;
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 6.本地 push_pull 远端数据
    remoteDevice.clear();
    status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);
    deviceList.clear();
    for (const auto& device2 : remoteDevice) {
        std::cout << "start sync" << device2.deviceId << std::endl;
        deviceList.push_back(device2.deviceId);
    }
    status = DisKvTest::KvStorePtr->Sync(deviceList, SyncMode::PUSH_PULL);
    EXPECT_EQ(status, Status::SUCCESS);
    auto syncCallback = std::make_shared<KvStoreSyncCallbackTestImpl>();
    status = DisKvTest::KvStorePtr->RegisterSyncCallback(syncCallback);
    EXPECT_EQ(status, Status::SUCCESS) << "register sync callback failed.";
    if (status == Status::SUCCESS) {
        std::cout << "LOGdisDataTest--SUCCESS: register sync callback" << std::endl;
    } else {
        std::cout << "LOGdisDataTest--FAIL: register sync callback" << static_cast<int>(status) << std::endl;
    }

    // 7.本地get数据失败
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::KEY_NOT_FOUND);
    // 8.远端get数据失败
    writeCodeDataToShm(CTRL_CODE_DATAMGR_GET_DATA, strKV);
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << " get result=" << str << std::endl;
    // 检查远端是否返回成功
    ret = strcmp(str, "1");
    EXPECT_EQ(ret, 0);
    // 解注册回调
    status = DisKvTest::KvStorePtr->UnRegisterSyncCallback();
    EXPECT_EQ(status, Status::SUCCESS) << "un register sync callback failed.";
}

/**
 *  @tc.number: SubscribeWithQuery_0100
 *  @tc.name: SubscribeWithQuery
 *  @tc.desc: SubscribeWithQuery, int data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SubscribeWithQuery_0100, TestSize.Level1)
{
    ZLOGI("SubscribeWithQuery_0100 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv
    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_int" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_int:200") + 1, "math_score_int:200");
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);
    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);
    // 4. 本段sync远端数据 SubscribeWithQuery
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }
    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SubscribeWithQuery";

    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);
    Value valueRetInt;
    Key keyInt = "math_score_int";
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    int aaa = TransferByteArrayToType<int>(valueRetInt.Data());
    std::cout << "LOGdisDataTest--本地get数据与200比较: aaa = " << aaa << std::endl;
    EXPECT_EQ(aaa, 200);
    // 取消订阅
    auto unSubscribeStatus = DisKvTest::KvStorePtr->UnsubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(unSubscribeStatus, Status::SUCCESS) << "LOGdisDataTest--ERR:SubscribeWithQuery";
}

/**
 *  @tc.number: SubscribeWithQuery_0200
 *  @tc.name: SubscribeWithQuery
 *  @tc.desc: SubscribeWithQuery, float data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SubscribeWithQuery_0200, TestSize.Level1)
{
    ZLOGI("SubscribeWithQuery_0200 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_float:9.99" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SubscribeWithQuery
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SubscribeWithQuery";

    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Value valueRetInt;
    std::string stringKey = "math_score_float";
    Key keyInt = stringKey;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS) << "LOGdisDataTest--ERR:Get(keyInt, valueRetInt)";
    float aaa = TransferByteArrayToType<float>(valueRetInt.Data());
    float delta = aaa - 9.99f;
    EXPECT_LE(std::abs(delta), DEFDELTA);
    // 取消订阅
    auto unSubscribeStatus = DisKvTest::KvStorePtr->UnsubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(unSubscribeStatus, Status::SUCCESS) << "LOGdisDataTest--ERR:SubscribeWithQuery";
}

/**
 *  @tc.number: SubscribeWithQuery_0300
 *  @tc.name: SubscribeWithQuery
 *  @tc.desc: SubscribeWithQuery, double data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SubscribeWithQuery_0300, TestSize.Level1)
{
    ZLOGI("SubscribeWithQuery_0300 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_double:999.999" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SubscribeWithQuery
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SubscribeWithQuery";

    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Value valueRetInt;
    std::string stringKey = "math_score_double";
    Key keyInt = stringKey;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    double aaa = TransferByteArrayToType<double>(valueRetInt.Data());
    double delta = aaa - 999.999;
    std::cout << "aaa = " << aaa << std::endl;
    std::cout << "delta = " << delta << std::endl;
    EXPECT_LE(std::abs(delta), DEFDELTA);

    // 取消订阅
    auto unSubscribeStatus = DisKvTest::KvStorePtr->UnsubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(unSubscribeStatus, Status::SUCCESS) << "LOGdisDataTest--ERR:SubscribeWithQuery";
}

/**
 *  @tc.number: SubscribeWithQuery_0400
 *  @tc.name: SubscribeWithQuery
 *  @tc.desc: SubscribeWithQuery, int64_t data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SubscribeWithQuery_0400, TestSize.Level1)
{
    ZLOGI("SubscribeWithQuery_0400 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_int64_t:99889988" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SubscribeWithQuery
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SubscribeWithQuery";

    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Key keyInt = "math_score_int64_t";
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    int64_t aaa = TransferByteArrayToType<int64_t>(valueRetInt.Data());
    EXPECT_EQ(aaa, 99889988u);

    // 取消订阅
    auto unSubscribeStatus = DisKvTest::KvStorePtr->UnsubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(unSubscribeStatus, Status::SUCCESS) << "LOGdisDataTest--ERR:SubscribeWithQuery";
}

/**
 *  @tc.number: SubscribeWithQuery_0500
 *  @tc.name: SubscribeWithQuery
 *  @tc.desc: SubscribeWithQuery, size_t data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SubscribeWithQuery_0500, TestSize.Level1)
{
    ZLOGI("SubscribeWithQuery_0500 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_size_t:88" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SubscribeWithQuery
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SubscribeWithQuery";

    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Key keyInt = "math_score_size_t";
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    size_t aaa = TransferByteArrayToType<size_t>(valueRetInt.Data());
    EXPECT_EQ(aaa, 88u);
    // 取消订阅
    auto unSubscribeStatus = DisKvTest::KvStorePtr->UnsubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(unSubscribeStatus, Status::SUCCESS) << "LOGdisDataTest--ERR:SubscribeWithQuery";
}

/**
 *  @tc.number: SubscribeWithQuery_0600
 *  @tc.name: SubscribeWithQuery
 *  @tc.desc: SubscribeWithQuery, string data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SubscribeWithQuery_0600, TestSize.Level1)
{
    ZLOGI("SubscribeWithQuery_0600 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_string:{\"class\":88, \"age\":99, \"gradle\":\"QQWWQQ\"}" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SubscribeWithQuery
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SubscribeWithQuery";
    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Key keyInt = "math_score_string";
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    std::string aaa = valueRetInt.ToString();
    std::string sstring = "{\"class\":88, \"age\":99, \"gradle\":\"QQWWQQ\"}";
    EXPECT_EQ(aaa, sstring);
    // 取消订阅
    auto unSubscribeStatus = DisKvTest::KvStorePtr->UnsubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(unSubscribeStatus, Status::SUCCESS) << "LOGdisDataTest--ERR:SubscribeWithQuery";
}

/**
 *  @tc.number: SubscribeWithQuery_0700
 *  @tc.name: SubscribeWithQuery
 *  @tc.desc: SubscribeWithQuery, vector<uint8_t>  data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SubscribeWithQuery_0700, TestSize.Level1)
{
    ZLOGI("SubscribeWithQuery_0700 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv
    // 3. 远端put int
    std::string stringKey = "math_score_vector";
    Key keyInt = stringKey;
    std::vector<uint8_t> vect2 = { 9, 9, 8, 8, 7, 7, 6, 6 };
    std::string strvc;
    strvc.assign(vect2.begin(), vect2.end());

    char strKV[MAX_DATA_LENGTH] = { "math_score_int" };
    strcpy_s(strKV, strlen(stringKey.c_str()) + 1, stringKey.c_str());
    strcat(strKV, ":");
    strcat(strKV, strvc.c_str());
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SubscribeWithQuery
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SubscribeWithQuery";
    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    std::string strValueRet = valueRetInt.ToString();
    EXPECT_EQ(strvc, strValueRet.c_str());

    // 取消订阅
    auto unSubscribeStatus = DisKvTest::KvStorePtr->UnsubscribeWithQuery(deviceList, dataQuery);
    EXPECT_EQ(unSubscribeStatus, Status::SUCCESS) << "LOGdisDataTest--ERR:SubscribeWithQuery";
}

/**
 *  @tc.number: SyncWithCondition_0100
 *  @tc.name: SyncWithCondition
 *  @tc.desc: SyncWithCondition, int data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SyncWithCondition_0100, TestSize.Level1)
{
    ZLOGI("SyncWithCondition_0100 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int

    char strKV[MAX_DATA_LENGTH] = { "math_score_int" };
    memset_s(strKV, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    strcpy_s(strKV, strlen("math_score_int:200") + 1, "math_score_int:200");
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SyncWithCondition
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SyncWithCondition(deviceList, SyncMode::PULL, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SyncWithCondition";

    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Value valueRetInt;
    Key keyInt = "math_score_int";
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    int aaa = TransferByteArrayToType<int>(valueRetInt.Data());
    std::cout << "LOGdisDataTest--本地get数据与200比较: aaa = " << aaa << std::endl;
    EXPECT_EQ(aaa, 200);
}

/**
 *  @tc.number: SyncWithCondition_0200
 *  @tc.name: SyncWithCondition
 *  @tc.desc: SyncWithCondition, float data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SyncWithCondition_0200, TestSize.Level1)
{
    ZLOGI("SyncWithCondition_0200 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_float:9.99" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SyncWithCondition
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SyncWithCondition(deviceList, SyncMode::PULL, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SyncWithCondition";
    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Value valueRetInt;
    std::string stringKey = "math_score_float";
    Key keyInt = stringKey;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS) << "LOGdisDataTest--ERR:Get(keyInt, valueRetInt)";
    float aaa = TransferByteArrayToType<float>(valueRetInt.Data());
    float delta = aaa - 9.99f;
    EXPECT_LE(std::abs(delta), DEFDELTA);
}

/**
 *  @tc.number: SyncWithCondition_0300
 *  @tc.name: SyncWithCondition
 *  @tc.desc: SyncWithCondition, double data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SyncWithCondition_0300, TestSize.Level1)
{
    ZLOGI("SyncWithCondition_0300 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_double:999.999" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SyncWithCondition
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SyncWithCondition(deviceList, SyncMode::PULL, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SyncWithCondition";

    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Value valueRetInt;
    std::string stringKey = "math_score_double";
    Key keyInt = stringKey;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    double aaa = TransferByteArrayToType<double>(valueRetInt.Data());
    double delta = aaa - 999.999;
    std::cout << "aaa = " << aaa << std::endl;
    std::cout << "delta = " << delta << std::endl;
    EXPECT_LE(std::abs(delta), DEFDELTA);
}

/**
 *  @tc.number: SyncWithCondition_0400
 *  @tc.name: SyncWithCondition
 *  @tc.desc: SyncWithCondition, int64_t data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SyncWithCondition_0400, TestSize.Level1)
{
    ZLOGI("SyncWithCondition_0400 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_int64_t:99889988" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SyncWithCondition
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SyncWithCondition(deviceList, SyncMode::PULL, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SyncWithCondition";

    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Key keyInt = "math_score_int64_t";
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    int64_t aaa = TransferByteArrayToType<int64_t>(valueRetInt.Data());
    EXPECT_EQ(aaa, 99889988u);
}

/**
 *  @tc.number: SyncWithCondition_0500
 *  @tc.name: SyncWithCondition
 *  @tc.desc: SyncWithCondition, size_t data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SyncWithCondition_0500, TestSize.Level1)
{
    ZLOGI("SyncWithCondition_0500 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_size_t:88" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SyncWithCondition
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SyncWithCondition(deviceList, SyncMode::PULL, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SyncWithCondition";

    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);

    Key keyInt = "math_score_size_t";
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    size_t aaa = TransferByteArrayToType<size_t>(valueRetInt.Data());
    EXPECT_EQ(aaa, 88u);
}

/**
 *  @tc.number: SyncWithCondition_0600
 *  @tc.name: SyncWithCondition
 *  @tc.desc: SyncWithCondition, string data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SyncWithCondition_0600, TestSize.Level1)
{
    ZLOGI("SyncWithCondition_0600 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    char strKV[MAX_DATA_LENGTH] = { "math_score_string:{\"class\":88, \"age\":99, \"gradle\":\"QQWWQQ\"}" };
    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SyncWithCondition
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SyncWithCondition(deviceList, SyncMode::PULL, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SyncWithCondition";

    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);
    Key keyInt = "math_score_string";
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    std::string aaa = valueRetInt.ToString();
    std::string sstring = "{\"class\":88, \"age\":99, \"gradle\":\"QQWWQQ\"}";
    EXPECT_EQ(aaa, sstring);
}

/**
 *  @tc.number: SyncWithCondition_0700
 *  @tc.name: SyncWithCondition
 *  @tc.desc: SyncWithCondition, vector<uint8_t>  data
 *  @tc.type: FUNC
 */
HWTEST_F(DistributedKvDataManagerTest, SyncWithCondition_0700, TestSize.Level1)
{
    ZLOGI("SyncWithCondition_0700 begin.");
    EXPECT_EQ(Status::SUCCESS, DisKvTest::statusGetKvStore) << "statusGetKvStore return wrong status";
    ASSERT_NE(nullptr, DisKvTest::KvStorePtr) << "KvStorePtr is nullptr";
    // 1.本端创建Lv
    // 2.远端创建kv

    // 3. 远端put int
    std::string stringKey = "math_score_vector";
    Key keyInt = stringKey;

    std::vector<uint8_t> vect2 = { 9, 9, 8, 8, 7, 7, 6, 6 };
    std::string strvc;
    strvc.assign(vect2.begin(), vect2.end());

    char strKV[MAX_DATA_LENGTH] = { "math_score_int" };
    strcpy_s(strKV, strlen(stringKey.c_str()) + 1, stringKey.c_str());
    strcat(strKV, ":");
    strcat(strKV, strvc.c_str());

    std::cout << "strvc = " << strvc << std::endl;
    std::cout << "strvc.c_str() = " << strvc.c_str() << std::endl;
    std::cout << "strKV = " << strKV << std::endl;

    writeCodeDataToShm(CTRL_CODE_DATAMGR_PUT_DATA, strKV);

    char str[MAX_DATA_LENGTH] = { 0 };
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    char code[CODE_LEN_TEN] = { "9999" };
    memset_s(code, CODE_LEN_TEN, 0, CODE_LEN_TEN);
    Int2String(CTRL_CODE_RESULT_TYPE, str);
    ASSERT_NE(nullptr, str);
    strcpy_s(code, strlen(str) + 1, str); // 9999
    memset_s(str, MAX_DATA_LENGTH, 0, MAX_DATA_LENGTH);
    waitDataWithCode(code, str);
    std::cout << "yput-200  get result=" << str << std::endl;
    // 检查远端是否返回成功
    int ret = strcmp(str, "0");
    EXPECT_EQ(ret, 0);

    // 4. 本段sync远端数据 SyncWithCondition
    std::vector<OHOS::DistributedKv::DeviceInfo> remoteDevice;
    remoteDevice.clear();
    Status status = DisKvTest::manager.GetDeviceList(remoteDevice, DeviceFilterStrategy::NO_FILTER);
    EXPECT_EQ(status, Status::SUCCESS);

    std::vector<std::string> deviceList;
    deviceList.clear();
    for (const auto& device : remoteDevice) {
        std::cout << "start sync deviceId = " << device.deviceId << std::endl;
        deviceList.push_back(device.deviceId);
    }

    DataQuery dataQuery;
    dataQuery.KeyPrefix("math");
    auto syncStatus = DisKvTest::KvStorePtr->SyncWithCondition(deviceList, SyncMode::PULL, dataQuery);
    EXPECT_EQ(syncStatus, Status::SUCCESS) << "LOGdisDataTest--SUCCESS:SyncWithCondition";
    // 5.本地get数据与200比较
    usleep(USLEEP_TIME);
    Value valueRetInt;
    status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    EXPECT_EQ(status, Status::SUCCESS);
    std::string strValueRet = valueRetInt.ToString();
    EXPECT_EQ(strvc, strValueRet.c_str());
}