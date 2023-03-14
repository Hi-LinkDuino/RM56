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

#include <functional>
#include <gtest/gtest.h>
#include <hdf_io_service_if.h>
#include <hdf_log.h>
#include <hdi_smq.h>
#include <idevmgr_hdi.h>
#include <iostream>
#include <ipc_object_stub.h>
#include <iservmgr_hdi.h>
#include <osal_time.h>
#include <string>

#include "sample_hdi.h"

#define HDF_LOG_TAG service_manager_test_cpp

using namespace testing::ext;
using OHOS::IRemoteObject;
using OHOS::sptr;
using OHOS::HDI::Base::SharedMemQueue;
using OHOS::HDI::Base::SharedMemQueueMeta;
using OHOS::HDI::Base::SmqType;
using OHOS::HDI::DeviceManager::V1_0::IDeviceManager;
using OHOS::HDI::ServiceManager::V1_0::IServiceManager;
using OHOS::HDI::ServiceManager::V1_0::IServStatListener;
using OHOS::HDI::ServiceManager::V1_0::ServiceStatus;
using OHOS::HDI::ServiceManager::V1_0::ServStatListenerStub;
static constexpr const char *TEST_SERVICE_NAME = "sample_driver_service";
static constexpr const char16_t *TEST_SERVICE_INTERFACE_DESC = u"hdf.test.sampele_service";
static constexpr int PAYLOAD_NUM = 1234;
static constexpr int SMQ_TEST_QUEUE_SIZE = 10;
static constexpr int SMQ_TEST_WAIT_TIME = 100;
static constexpr int WAIT_LOAD_UNLOAD_TIME = 300;

class HdfServiceMangerHdiTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        auto devmgr = IDeviceManager::Get();
        if (devmgr != nullptr) {
            HDF_LOGI("%{public}s:%{public}d", __func__, __LINE__);
            devmgr->LoadDevice(TEST_SERVICE_NAME);
        }
    }
    static void TearDownTestCase()
    {
        auto devmgr = IDeviceManager::Get();
        if (devmgr != nullptr) {
            HDF_LOGI("%{public}s:%{public}d", __func__, __LINE__);
            devmgr->UnloadDevice(TEST_SERVICE_NAME);
        }
    }
    void TestServiceListenerStop(const sptr<IDeviceManager>& devmgr, const sptr<IServiceManager>& servmgr);
    void TestSampleService(sptr<IRemoteObject>& sampleService, const sptr<IDeviceManager>& devmgr,
        const sptr<IServiceManager>& servmgr);
    void SetUp() {};
    void TearDown() {};
};

HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest001, TestSize.Level1)
{
    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);
}

HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest002, TestSize.Level1)
{
    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);

    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);

    ASSERT_TRUE(sampleService != nullptr);

    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    bool ret = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(ret, true);
    data.WriteCString("sample_service test call");

    OHOS::MessageOption option;
    int status = sampleService->SendRequest(SAMPLE_SERVICE_PING, data, reply, option);
    ASSERT_EQ(status, 0);
}

class IPCObjectStubTest : public OHOS::IPCObjectStub {
public:
    explicit IPCObjectStubTest() : OHOS::IPCObjectStub(u"") {};
    virtual ~IPCObjectStubTest() = default;
    int OnRemoteRequest(
        uint32_t code, OHOS::MessageParcel &data, OHOS::MessageParcel &reply, OHOS::MessageOption &option) override
    {
        HDF_LOGI("IPCObjectStubTest::OnRemoteRequest called, code = %{public}d", code);
        payload = data.ReadInt32();

        return HDF_SUCCESS;
    }

    static int32_t payload;
};

int32_t IPCObjectStubTest::payload = 0;

HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest003, TestSize.Level1)
{
    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);

    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService != nullptr);

    sptr<IRemoteObject> callback = new IPCObjectStubTest();
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    int32_t payload = PAYLOAD_NUM;
    bool ret = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(ret, true);
    data.WriteInt32(payload);
    data.WriteRemoteObject(callback);

    OHOS::MessageOption option;
    int status = sampleService->SendRequest(SAMPLE_SERVICE_CALLBACK, data, reply, option);
    ASSERT_EQ(status, 0);
    ASSERT_EQ(IPCObjectStubTest::payload, payload);
}

HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest004, TestSize.Level1)
{
    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);

    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService != nullptr);

    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    bool ret = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(ret, true);
    data.WriteInt32(PAYLOAD_NUM);
    data.WriteInt32(PAYLOAD_NUM);

    OHOS::MessageOption option;
    int status = sampleService->SendRequest(SAMPLE_SERVICE_SUM, data, reply, option);
    ASSERT_EQ(status, 0);
    int32_t result = reply.ReadInt32();
    int32_t expRes = PAYLOAD_NUM + PAYLOAD_NUM;
    ASSERT_EQ(result, expRes);
}

HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest006, TestSize.Level1)
{
    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);

    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService != nullptr);

    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;

    constexpr int buffersize = 10;
    uint8_t dataBuffer[buffersize];
    for (int i = 0; i < buffersize; i++) {
        dataBuffer[i] = i;
    }
    bool ret = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(ret, true);
    ret = data.WriteUnpadBuffer(dataBuffer, sizeof(dataBuffer));
    ASSERT_TRUE(ret);

    OHOS::MessageOption option;
    int status = sampleService->SendRequest(SAMPLE_BUFFER_TRANS, data, reply, option);
    ASSERT_EQ(status, 0);

    const uint8_t *retBuffer = reply.ReadUnpadBuffer(buffersize);
    ASSERT_TRUE(retBuffer != nullptr);

    for (int i = 0; i < buffersize; i++) {
        ASSERT_EQ(retBuffer[i], i);
    }
}

/*
 * Test device manager Load/UnLoad device and driver dynamic register device
 */
HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest007, TestSize.Level1)
{
    auto devmgr = IDeviceManager::Get();
    ASSERT_TRUE(devmgr != nullptr);
    devmgr->UnloadDevice(TEST_SERVICE_NAME);

    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);
    OsalMSleep(WAIT_LOAD_UNLOAD_TIME);
    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService == nullptr);

    int ret = devmgr->LoadDevice(TEST_SERVICE_NAME);
    ASSERT_EQ(ret, HDF_SUCCESS);
    OsalMSleep(WAIT_LOAD_UNLOAD_TIME);
    sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService != nullptr);

    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    ret = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(ret, true);
    const char *newServName = "sample_driver_service2";
    ret = data.WriteCString(newServName);
    ASSERT_TRUE(ret);

    int status = sampleService->SendRequest(SAMPLE_REGISTER_DEVICE, data, reply, option);
    ASSERT_EQ(status, HDF_SUCCESS);

    auto sampleService2 = servmgr->GetService(newServName);
    ASSERT_TRUE(sampleService != nullptr);

    data.FlushBuffer();
    reply.FlushBuffer();
    ret = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(ret, true);
    data.WriteInt32(PAYLOAD_NUM);
    data.WriteInt32(PAYLOAD_NUM);

    status = sampleService2->SendRequest(SAMPLE_SERVICE_SUM, data, reply, option);
    ASSERT_EQ(status, 0);
    int32_t result = reply.ReadInt32();

    int32_t expRes = PAYLOAD_NUM + PAYLOAD_NUM;
    ASSERT_EQ(result, expRes);
    sampleService2 = nullptr;

    data.FlushBuffer();
    reply.FlushBuffer();
    ret = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(ret, true);
    data.WriteCString(newServName);

    status = sampleService->SendRequest(SAMPLE_UNREGISTER_DEVICE, data, reply, option);
    ASSERT_EQ(status, HDF_SUCCESS);

    sampleService2 = servmgr->GetService(newServName);
    ASSERT_TRUE(sampleService2 == nullptr);

    ret = devmgr->UnloadDevice(TEST_SERVICE_NAME);
    ASSERT_EQ(ret, HDF_SUCCESS);
    OsalMSleep(WAIT_LOAD_UNLOAD_TIME);
    sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService == nullptr);
}

class ServStatListener : public OHOS::HDI::ServiceManager::V1_0::ServStatListenerStub {
public:
    using StatusCallback = std::function<void(const ServiceStatus &)>;
    explicit ServStatListener(StatusCallback callback) : callback_(std::move(callback)) {}
    ~ServStatListener() = default;
    void OnReceive(const ServiceStatus &status) override
    {
        callback_(status);
    }

private:
    StatusCallback callback_;
};

/*
 * Test service start status listener
 */
void HdfServiceMangerHdiTest::TestServiceListenerStop(const sptr<IDeviceManager>& devmgr,
    const sptr<IServiceManager>& servmgr)
{
    std::string servInfo;
    uint16_t devClass;
    uint16_t servStatus;
    bool callbacked = false;
    ::OHOS::sptr<IServStatListener> listener
        = new ServStatListener(ServStatListener::StatusCallback([&](const ServiceStatus &status) {
                HDF_LOGI("service status callback");
                if (status.serviceName == std::string(TEST_SERVICE_NAME)) {
                    servInfo = status.info;
                    devClass = status.deviceClass;
                    servStatus = status.status;
                    callbacked = true;
                }
            }));

    int status = servmgr->RegisterServiceStatusListener(listener, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(status, HDF_SUCCESS);

    int ret = devmgr->LoadDevice(TEST_SERVICE_NAME);
    ASSERT_EQ(ret, HDF_SUCCESS);
    constexpr int WAIT_COUNT = 300;
    int count = WAIT_COUNT;
    while (!callbacked && count > 0) {
        OsalMSleep(1);
        count--;
    }
    ASSERT_TRUE(callbacked);
    ASSERT_EQ(devClass, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(servInfo, std::string(TEST_SERVICE_NAME));
    ASSERT_EQ(servStatus, OHOS::HDI::ServiceManager::V1_0::SERVIE_STATUS_START);

    callbacked = false;
    ret = devmgr->UnloadDevice(TEST_SERVICE_NAME);
    ASSERT_EQ(ret, HDF_SUCCESS);

    count = WAIT_COUNT;
    while (!callbacked && count > 0) {
        OsalMSleep(1);
        count--;
    }
    ASSERT_TRUE(callbacked);
    ASSERT_EQ(devClass, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(servInfo, std::string(TEST_SERVICE_NAME));
    ASSERT_EQ(servStatus, OHOS::HDI::ServiceManager::V1_0::SERVIE_STATUS_STOP);

    status = servmgr->UnregisterServiceStatusListener(listener);
    ASSERT_EQ(status, HDF_SUCCESS);
}

HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest008, TestSize.Level1)
{
    auto devmgr = IDeviceManager::Get();
    ASSERT_TRUE(devmgr != nullptr);
    devmgr->UnloadDevice(TEST_SERVICE_NAME);

    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);
    OsalMSleep(WAIT_LOAD_UNLOAD_TIME);
    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService == nullptr);

    TestServiceListenerStop(devmgr, servmgr);
}

/*
 * Test service status listener update service info
 */
void HdfServiceMangerHdiTest::TestSampleService(sptr<IRemoteObject>& sampleService,
    const sptr<IDeviceManager>& devmgr, const sptr<IServiceManager>& servmgr)
{
    std::string servInfo;
    uint16_t devClass;
    uint16_t servStatus;
    bool callbacked = false;
    ::OHOS::sptr<IServStatListener> listener
        = new ServStatListener(
            ServStatListener::StatusCallback([&](const ServiceStatus &status) {
                if (status.serviceName == std::string(TEST_SERVICE_NAME) &&
                    status.status == OHOS::HDI::ServiceManager::V1_0::SERVIE_STATUS_CHANGE) {
                    HDF_LOGI("sample service status callback");
                    servInfo = status.info;
                    devClass = status.deviceClass;
                    servStatus = status.status;
                    callbacked = true;
                }
            }));

    int status = servmgr->RegisterServiceStatusListener(listener, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(status, HDF_SUCCESS);
    constexpr int FIRST_WAIT = 20;
    OsalMSleep(FIRST_WAIT); // skip callback on register
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    bool res = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(res, true);
    std::string info = "foo";
    data.WriteCString(info.data());
    callbacked = false;
    status = sampleService->SendRequest(SAMPLE_UPDATE_SERVIE, data, reply, option);
    ASSERT_EQ(status, HDF_SUCCESS);
    constexpr int WAIT_COUNT = 300;
    int count = WAIT_COUNT;
    while (!callbacked && count > 0) {
        OsalMSleep(1);
        count--;
    }
    ASSERT_TRUE(callbacked);
    ASSERT_EQ(devClass, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(servInfo, info);
    int ret = devmgr->UnloadDevice(TEST_SERVICE_NAME);
    ASSERT_EQ(ret, HDF_SUCCESS);

    status = servmgr->UnregisterServiceStatusListener(listener);
    ASSERT_EQ(status, HDF_SUCCESS);
}

HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest009, TestSize.Level1)
{
    auto devmgr = IDeviceManager::Get();
    ASSERT_TRUE(devmgr != nullptr);
    devmgr->UnloadDevice(TEST_SERVICE_NAME);

    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);
    OsalMSleep(WAIT_LOAD_UNLOAD_TIME);
    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService == nullptr);

    int ret = devmgr->LoadDevice(TEST_SERVICE_NAME);
    ASSERT_EQ(ret, HDF_SUCCESS);
    OsalMSleep(WAIT_LOAD_UNLOAD_TIME);
    sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService != nullptr);

    TestSampleService(sampleService, devmgr, servmgr);
}

/*
 * Test service status listener unregister
 */
HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest010, TestSize.Level1)
{
    auto devmgr = IDeviceManager::Get();
    ASSERT_TRUE(devmgr != nullptr);
    devmgr->UnloadDevice(TEST_SERVICE_NAME);

    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);
    OsalMSleep(WAIT_LOAD_UNLOAD_TIME);
    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService == nullptr);

    std::string servInfo;
    uint16_t devClass;
    uint16_t servStatus;
    bool callbacked = false;
    ::OHOS::sptr<IServStatListener> listener
        = new ServStatListener(
            ServStatListener::StatusCallback([&](const ServiceStatus &status) {
                HDF_LOGI("service status callback");
                if (status.serviceName == std::string(TEST_SERVICE_NAME) &&
                    status.status == OHOS::HDI::ServiceManager::V1_0::SERVIE_STATUS_START) {
                    servInfo = status.info;
                    devClass = status.deviceClass;
                    servStatus = status.status;
                    callbacked = true;
                }
            }));

    int status = servmgr->RegisterServiceStatusListener(listener, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(status, HDF_SUCCESS);

    int ret = devmgr->LoadDevice(TEST_SERVICE_NAME);
    ASSERT_EQ(ret, HDF_SUCCESS);

    constexpr int WAIT_COUNT = 300;
    int count = WAIT_COUNT;
    while (!callbacked && count > 0) {
        OsalMSleep(1);
        count--;
    }
    ASSERT_TRUE(callbacked);
    ASSERT_EQ(devClass, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(servInfo, std::string(TEST_SERVICE_NAME));

    sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService != nullptr);

    status = servmgr->UnregisterServiceStatusListener(listener);
    ASSERT_EQ(status, HDF_SUCCESS);

    callbacked = false;
    ret = devmgr->UnloadDevice(TEST_SERVICE_NAME);
    ASSERT_EQ(ret, HDF_SUCCESS);

    OsalMSleep(10);
    ASSERT_FALSE(callbacked);

    ret = devmgr->LoadDevice(TEST_SERVICE_NAME);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

/*
 * smq test normal read/write
 */
HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest011, TestSize.Level1)
{
    HDF_LOGI("%{public}s:%{public}d", __func__, __LINE__);
    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);

    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService != nullptr);

    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    std::unique_ptr<SharedMemQueue<SampleSmqElement>> smq =
        std::make_unique<SharedMemQueue<SampleSmqElement>>(SMQ_TEST_QUEUE_SIZE, SmqType::SYNCED_SMQ);
    ASSERT_TRUE(smq->IsGood());
    bool ret = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(ret, true);
    ret = smq->GetMeta()->Marshalling(data);
    ASSERT_TRUE(ret);
    data.WriteUint32(1);

    int status = sampleService->SendRequest(SAMPLE_TRANS_SMQ, data, reply, option);
    ASSERT_EQ(status, 0);

    constexpr int SEND_TIMES = 20;
    for (size_t i = 0; i < SEND_TIMES; i++) {
        SampleSmqElement t = {0};
        t.data32 = i;
        t.data64 = i + 1;

        HDF_LOGI("%{public}s:write smq message %{public}zu", __func__, i);
        auto status = smq->Write(&t, 1, OHOS::MillisecToNanosec(SMQ_TEST_WAIT_TIME));
        ASSERT_EQ(status, 0);
    }
}

/*
 * smq test with overflow wait
 */
HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest012, TestSize.Level1)
{
    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);

    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService != nullptr);

    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    std::unique_ptr<SharedMemQueue<SampleSmqElement>> smq =
        std::make_unique<SharedMemQueue<SampleSmqElement>>(SMQ_TEST_QUEUE_SIZE, SmqType::SYNCED_SMQ);
    ASSERT_TRUE(smq->IsGood());
    bool ret = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(ret, true);
    constexpr uint32_t ELEMENT_SIZE = 2;
    ret = smq->GetMeta()->Marshalling(data);
    ASSERT_TRUE(ret);

    data.WriteUint32(ELEMENT_SIZE);
    int status = sampleService->SendRequest(SAMPLE_TRANS_SMQ, data, reply, option);
    ASSERT_EQ(status, 0);

    constexpr int SEND_TIMES = 20;
    for (int i = 0; i < SEND_TIMES; i++) {
        SampleSmqElement t[ELEMENT_SIZE] = {};
        t[0].data32 = i;
        t[0].data64 = i + 1;
        t[1].data32 = i + 1;
        t[1].data64 = i + 1;
        HDF_LOGI("%{public}s:write smq message %{public}zu", __func__, i);
        auto status = smq->Write(&t[0], ELEMENT_SIZE, OHOS::MillisecToNanosec(SMQ_TEST_WAIT_TIME));
        ASSERT_EQ(status, 0);
    }
}

/*
 * smq test UNSYNC_SMQ
 */
HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest013, TestSize.Level1)
{
    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);

    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_TRUE(sampleService != nullptr);

    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;

    std::unique_ptr<SharedMemQueue<SampleSmqElement>> smq =
        std::make_unique<SharedMemQueue<SampleSmqElement>>(SMQ_TEST_QUEUE_SIZE, SmqType::UNSYNC_SMQ);
    ASSERT_TRUE(smq->IsGood());
    bool ret = data.WriteInterfaceToken(TEST_SERVICE_INTERFACE_DESC);
    ASSERT_EQ(ret, true);
    constexpr uint32_t ELEMENT_SIZE = 2;
    ret = smq->GetMeta()->Marshalling(data);
    ASSERT_TRUE(ret);
    data.WriteUint32(ELEMENT_SIZE);
    auto status = sampleService->SendRequest(SAMPLE_TRANS_SMQ, data, reply, option);
    ASSERT_EQ(status, 0);

    SampleSmqElement t[ELEMENT_SIZE] = {};
    status = smq->Write(&t[0], ELEMENT_SIZE);
    EXPECT_NE(status, 0);
    constexpr int SEND_TIMES = 20;
    for (int i = 0; i < SEND_TIMES; i++) {
        t[0].data32 = i;
        t[0].data64 = i + 1;
        t[1].data32 = i + 1;
        t[1].data64 = i + 1;
        HDF_LOGI("%{public}s:write smq message %{public}zu", __func__, i);
        status = smq->WriteNonBlocking(&t[0], ELEMENT_SIZE);
        ASSERT_EQ(status, 0);
    }
}

/*
 * Test service status listener get serviec callback on register
 */
HWTEST_F(HdfServiceMangerHdiTest, ServMgrTest014, TestSize.Level1)
{
    auto servmgr = IServiceManager::Get();
    ASSERT_TRUE(servmgr != nullptr);

    auto sampleService = servmgr->GetService(TEST_SERVICE_NAME);
    ASSERT_NE(sampleService, nullptr);

    bool callbacked = false;
    bool sampleServiceStarted = false;
    uint16_t servStatus = 0;
    ::OHOS::sptr<IServStatListener> listener
        = new ServStatListener(
            ServStatListener::StatusCallback([&](const ServiceStatus &status) {
                HDF_LOGI("service status callback, service is %{public}s", status.serviceName.data());
                callbacked = true;
                if (status.serviceName == std::string(TEST_SERVICE_NAME)) {
                    sampleServiceStarted = true;
                    servStatus = status.status;
                }
            }));

    int status = servmgr->RegisterServiceStatusListener(listener, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(status, HDF_SUCCESS);
    constexpr int WAIT_COUNT = 100;
    int count = WAIT_COUNT;
    while (!sampleServiceStarted && count > 0) {
        OsalMSleep(1);
        count--;
    }
    ASSERT_TRUE(callbacked);
    ASSERT_TRUE(sampleServiceStarted);
    ASSERT_EQ(servStatus, OHOS::HDI::ServiceManager::V1_0::SERVIE_STATUS_START);
    status = servmgr->UnregisterServiceStatusListener(listener);
    ASSERT_EQ(status, HDF_SUCCESS);
}