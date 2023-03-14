/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <unistd.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <cinttypes>
#include <string>
#include <gtest/gtest.h>
#include "hdf_io_service.h"
#include "hdf_log.h"
#include "hdf_power_state.h"
#include "hdf_sbuf.h"
#include "hdf_uhdf_test.h"
#include "ioservstat_listener.h"
#include "osal_time.h"
#include "sample_driver_test.h"
#include "svcmgr_ioservice.h"

using namespace testing::ext;

struct Eventlistener {
    struct HdfDevEventlistener listener;
    int32_t eventCount;
};

class IoServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static int OnDevEventReceived(
        struct HdfDevEventlistener *listener, struct HdfIoService *service, uint32_t id, struct HdfSBuf *data);

    void TestServiceStop(struct IoServiceStatusData* issd);
    static struct Eventlistener listener0;
    static struct Eventlistener listener1;
    const char *testSvcName = SAMPLE_SERVICE;
    const int eventWaitTimeUs = (150 * 1000);
    static int eventCount;
    const int servstatWaitTime = 15; // ms
};

int IoServiceTest::eventCount = 0;

struct Eventlistener IoServiceTest::listener0;
struct Eventlistener IoServiceTest::listener1;

void IoServiceTest::SetUpTestCase()
{
    listener0.listener.onReceive = OnDevEventReceived;
    listener0.listener.priv = (void *)"listener0";

    listener1.listener.onReceive = OnDevEventReceived;
    listener1.listener.priv = (void *)"listener1";
}

void IoServiceTest::TearDownTestCase()
{
}

void IoServiceTest::SetUp()
{
    listener0.eventCount = 0;
    listener1.eventCount = 0;
    eventCount = 0;
}

void IoServiceTest::TearDown()
{
}

int IoServiceTest::OnDevEventReceived(
    struct HdfDevEventlistener *listener, struct HdfIoService *service, uint32_t id, struct HdfSBuf *data)
{
    OsalTimespec time;
    OsalGetTime(&time);
    HDF_LOGE("%s: received event[%d] from %s at %" PRIu64 ".%" PRIu64 "", (char *)listener->priv, eventCount++,
        (char *)service->priv, time.sec, time.usec);

    const char *string = HdfSbufReadString(data);
    if (string == nullptr) {
        HDF_LOGE("failed to read string in event data");
        return 0;
    }
    struct Eventlistener *l = CONTAINER_OF(listener, struct Eventlistener, listener);
    l->eventCount++;
    HDF_LOGE("%s: dev event received: %u %s", (char *)service->priv, id, string);
    return 0;
}

static int SendEvent(struct HdfIoService *serv, const char *eventData, bool broadcast)
{
    OsalTimespec time;
    OsalGetTime(&time);

    int ret;
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == nullptr) {
        HDF_LOGE("fail to obtain sbuf data");
        return HDF_FAILURE;
    }

    struct HdfSBuf *reply = HdfSbufObtainDefaultSize();
    if (reply == nullptr) {
        HDF_LOGE("fail to obtain sbuf reply");
        HdfSbufRecycle(data);
        return HDF_DEV_ERR_NO_MEMORY;
    }

    uint32_t cmdId = broadcast ? SAMPLE_DRIVER_SENDEVENT_BROADCAST_DEVICE : SAMPLE_DRIVER_SENDEVENT_SINGLE_DEVICE;
    do {
        if (!HdfSbufWriteString(data, eventData)) {
            HDF_LOGE("fail to write sbuf");
            ret = HDF_FAILURE;
            break;
        }

        ret = serv->dispatcher->Dispatch(&serv->object, cmdId, data, reply);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("fail to send service call");
            break;
        }

        int replyData = 0;
        if (!HdfSbufReadInt32(reply, &replyData)) {
            HDF_LOGE("fail to get service call reply");
            ret = HDF_ERR_INVALID_OBJECT;
        } else if (replyData != INT32_MAX) {
            HDF_LOGE("service call reply check fail, replyData=0x%x", replyData);
            ret = HDF_ERR_INVALID_OBJECT;
        }
        HDF_LOGE("send event finish at %" PRIu64 ".%" PRIu64 "", time.sec, time.usec);
    } while (0);

    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

/* *
 * @tc.name: HdfIoService001
 * @tc.desc: service bind test
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService001, TestSize.Level0)
{
    struct HdfIoService *testServ = HdfIoServiceBind(testSvcName);
    ASSERT_NE(testServ, nullptr);
    HdfIoServiceRecycle(testServ);
}

/* *
 * @tc.name: HdfIoService002
 * @tc.desc: service group listen test
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService002, TestSize.Level0)
{
    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv0";

    struct HdfIoServiceGroup *group = HdfIoServiceGroupObtain();
    ASSERT_NE(group, nullptr);

    int ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    ret = HdfDeviceUnregisterEventListener(serv, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    HdfIoServiceGroupRecycle(group);
    group = HdfIoServiceGroupObtain();
    ASSERT_NE(group, nullptr);

    ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(2, listener0.eventCount);
    HdfIoServiceGroupRecycle(group);

    HdfIoServiceRecycle(serv);
}

/* *
 * @tc.name: HdfIoService003
 * @tc.desc: remove service from service group by recycle group test
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService003, TestSize.Level0)
{
    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv0";

    struct HdfIoService *serv1 = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv1, nullptr);
    serv1->priv = (void *)"serv1";

    struct HdfIoServiceGroup *group = HdfIoServiceGroupObtain();
    ASSERT_NE(group, nullptr);

    int ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    ret = HdfIoServiceGroupAddService(group, serv1);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfDeviceUnregisterEventListener(serv, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfDeviceRegisterEventListener(serv, &listener1.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(2, listener0.eventCount);
    ASSERT_EQ(1, listener1.eventCount);
    HdfIoServiceGroupRecycle(group);
    HdfDeviceUnregisterEventListener(serv, &listener1.listener);
    HdfIoServiceRecycle(serv);
    HdfIoServiceRecycle(serv1);
}

/* *
 * @tc.name: HdfIoService004
 * @tc.desc: single service listen test
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService004, TestSize.Level0)
{
    struct HdfIoService *serv1 = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv1, nullptr);
    serv1->priv = (void *)"serv1";

    int ret = HdfDeviceRegisterEventListener(serv1, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);
    ret = SendEvent(serv1, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    ret = HdfDeviceUnregisterEventListener(serv1, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);
    HdfIoServiceRecycle(serv1);
}

/* *
 * @tc.name: HdfIoService005
 * @tc.desc: service group add remove test
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService005, TestSize.Level0)
{
    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    struct HdfIoServiceGroup *group = HdfIoServiceGroupObtain();
    ASSERT_NE(group, nullptr);

    int ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    HdfIoServiceGroupRemoveService(group, serv);

    ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(2, listener0.eventCount);
    HdfIoServiceGroupRecycle(group);
}

/* *
 * @tc.name: HdfIoService006
 * @tc.desc: service group add remove listener test
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService006, TestSize.Level0)
{
    struct HdfIoServiceGroup *group = HdfIoServiceGroupObtain();
    ASSERT_NE(group, nullptr);

    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    struct HdfIoService *serv1 = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv1, nullptr);
    serv1->priv = (void *)"serv1";

    int ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupAddService(group, serv1);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    ret = SendEvent(serv1, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(2, listener0.eventCount);

    HdfIoServiceGroupRemoveService(group, serv);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(2, listener0.eventCount);

    ret = SendEvent(serv1, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(3, listener0.eventCount);

    ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(4, listener0.eventCount);

    HdfIoServiceGroupRecycle(group);
    HdfIoServiceRecycle(serv);
    HdfIoServiceRecycle(serv1);
}

/* *
 * @tc.name: HdfIoService007
 * @tc.desc: duplicate remove group listener
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService007, TestSize.Level0)
{
    struct HdfIoServiceGroup *group = HdfIoServiceGroupObtain();
    ASSERT_NE(group, nullptr);

    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    int ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    ret = HdfIoServiceGroupUnregisterListener(group, &listener0.listener);
    EXPECT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupUnregisterListener(group, &listener0.listener);
    EXPECT_NE(ret, HDF_SUCCESS);

    HdfIoServiceGroupRecycle(group);
    HdfIoServiceRecycle(serv);
}

/* *
 * @tc.name: HdfIoService008
 * @tc.desc: duplicate add group listener
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService008, TestSize.Level0)
{
    struct HdfIoServiceGroup *group = HdfIoServiceGroupObtain();
    ASSERT_NE(group, nullptr);

    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    int ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    EXPECT_NE(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupUnregisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    HdfIoServiceGroupRecycle(group);
    HdfIoServiceRecycle(serv);
}

/* *
 * @tc.name: HdfIoService008
 * @tc.desc: duplicate add service
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService009, TestSize.Level0)
{
    struct HdfIoServiceGroup *group = HdfIoServiceGroupObtain();
    ASSERT_NE(group, nullptr);

    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    int ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupAddService(group, serv);
    EXPECT_NE(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    ret = HdfIoServiceGroupUnregisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    HdfIoServiceGroupRecycle(group);
    HdfIoServiceRecycle(serv);
}

/* *
 * @tc.name: HdfIoService010
 * @tc.desc: duplicate remove service
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService010, TestSize.Level0)
{
    struct HdfIoServiceGroup *group = HdfIoServiceGroupObtain();
    ASSERT_NE(group, nullptr);

    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    int ret = HdfIoServiceGroupAddService(group, serv);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupAddService(group, serv);
    EXPECT_NE(ret, HDF_SUCCESS);

    ret = HdfIoServiceGroupRegisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    HdfIoServiceGroupRemoveService(group, serv);
    HdfIoServiceGroupRemoveService(group, serv);

    ret = HdfIoServiceGroupUnregisterListener(group, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    HdfIoServiceGroupRecycle(group);
    HdfIoServiceRecycle(serv);
}

/* *
 * @tc.name: HdfIoService011
 * @tc.desc: duplicate add service listener
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService011, TestSize.Level0)
{
    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    int ret = HdfDeviceRegisterEventListener(serv, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    ret = HdfDeviceRegisterEventListener(serv, &listener0.listener);
    EXPECT_NE(ret, HDF_SUCCESS);

    ret = HdfDeviceUnregisterEventListener(serv, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);
    HdfIoServiceRecycle(serv);
}

/* *
 * @tc.name: HdfIoService012
 * @tc.desc: duplicate remove service listener
 * @tc.type: FUNC
 * @tc.require: AR000F869B
 */
HWTEST_F(IoServiceTest, HdfIoService012, TestSize.Level0)
{
    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    int ret = HdfDeviceRegisterEventListener(serv, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, false);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);

    ret = HdfDeviceUnregisterEventListener(serv, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfDeviceUnregisterEventListener(serv, &listener0.listener);
    EXPECT_NE(ret, HDF_SUCCESS);

    HdfIoServiceRecycle(serv);
}

/* *
 * @tc.name: HdfIoService013
 * @tc.desc: devmgr power state change test
 * @tc.type: FUNC
 */
HWTEST_F(IoServiceTest, HdfIoService013, TestSize.Level0)
{
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    ASSERT_NE(data, nullptr);

    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);

    HdfSbufWriteUint32(data, POWER_STATE_SUSPEND);
    int ret = serv->dispatcher->Dispatch(&serv->object, SAMPLE_DRIVER_PM_STATE_INJECT, data, nullptr);
    ASSERT_EQ(ret, HDF_SUCCESS);

    HdfSbufFlush(data);
    HdfSbufWriteUint32(data, POWER_STATE_RESUME);
    ret = serv->dispatcher->Dispatch(&serv->object, SAMPLE_DRIVER_PM_STATE_INJECT, data, nullptr);
    ASSERT_EQ(ret, HDF_SUCCESS);

    HdfSbufFlush(data);
    HdfSbufWriteUint32(data, POWER_STATE_DOZE_SUSPEND);
    ret = serv->dispatcher->Dispatch(&serv->object, SAMPLE_DRIVER_PM_STATE_INJECT, data, nullptr);
    ASSERT_EQ(ret, HDF_SUCCESS);

    HdfSbufFlush(data);
    HdfSbufWriteUint32(data, POWER_STATE_DOZE_RESUME);
    ret = serv->dispatcher->Dispatch(&serv->object, SAMPLE_DRIVER_PM_STATE_INJECT, data, nullptr);
    ASSERT_EQ(ret, HDF_SUCCESS);
    HdfIoServiceRecycle(serv);
    HdfSbufRecycle(data);
}

/* *
 * @tc.name: HdfIoService014
 * @tc.desc: multiple clients listen to a service
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IoServiceTest, HdfIoService014, TestSize.Level0)
{
    struct HdfIoServiceGroup *group = HdfIoServiceGroupObtain();
    ASSERT_NE(group, nullptr);

    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    struct HdfIoService *serv1 = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv1, nullptr);
    serv1->priv = (void *)"serv1";

    int ret = HdfDeviceRegisterEventListener(serv, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfDeviceRegisterEventListener(serv1, &listener1.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = SendEvent(serv, testSvcName, true);
    ASSERT_EQ(ret, HDF_SUCCESS);

    usleep(eventWaitTimeUs);
    ASSERT_EQ(1, listener0.eventCount);
    ASSERT_EQ(1, listener1.eventCount);

    ret = HdfDeviceUnregisterEventListener(serv, &listener0.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    ret = HdfDeviceUnregisterEventListener(serv, &listener0.listener);
    EXPECT_NE(ret, HDF_SUCCESS);

    ret = HdfDeviceUnregisterEventListener(serv1, &listener1.listener);
    ASSERT_EQ(ret, HDF_SUCCESS);

    HdfIoServiceRecycle(serv);
    HdfIoServiceRecycle(serv1);
}

struct IoServiceStatusData {
    IoServiceStatusData(): devClass(0), servStatus(0), callbacked(false)
    {
    }
    ~IoServiceStatusData() = default;
    std::string servName;
    std::string servInfo;
    uint16_t devClass;
    uint16_t servStatus;
    bool callbacked;
};

static void TestOnServiceStatusReceived(struct ServiceStatusListener *listener, struct ServiceStatus *servstat)
{
    struct IoServiceStatusData *issd = (struct IoServiceStatusData *)listener->priv;
    if (issd == nullptr) {
        return;
    }
    issd->servName = servstat->serviceName;
    issd->servInfo = ((servstat->info != nullptr) ? (servstat->info) : (""));
    issd->devClass = servstat->deviceClass;
    issd->servStatus = servstat->status;
    issd->callbacked = true;

    HDF_LOGI("service status listener callback: %{public}s, %{public}s, %{public}d", servstat->serviceName,
        issd->servName.data(), issd->servStatus);
}

/* *
 * @tc.name: HdfIoService015
 * @tc.desc: ioservice status listener test
 * @tc.type: FUNC
 * @tc.require
 */
void IoServiceTest::TestServiceStop(struct IoServiceStatusData* issd)
{
    struct HdfIoService *testService = HdfIoServiceBind(SAMPLE_SERVICE);
    ASSERT_TRUE(testService != nullptr);
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    ASSERT_TRUE(data != nullptr);
    const char *newServName = "sample_service1";
    ASSERT_TRUE(HdfSbufWriteString(data, "sample_driver"));
    ASSERT_TRUE(HdfSbufWriteString(data, newServName));

    int ret = testService->dispatcher->Dispatch(&testService->object, SAMPLE_DRIVER_REGISTER_DEVICE, data, nullptr);
    ASSERT_EQ(ret, HDF_SUCCESS);

    int count = servstatWaitTime;
    while (!issd->callbacked && count > 0) {
        OsalMSleep(1);
        count--;
    }
    ASSERT_TRUE(issd->callbacked);
    ASSERT_EQ(issd->servName, newServName);
    ASSERT_EQ(issd->devClass, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(issd->servInfo, std::string(SAMPLE_SERVICE));
    ASSERT_EQ(issd->servStatus, SERVIE_STATUS_START);

    issd->callbacked = false;
    ret = testService->dispatcher->Dispatch(&testService->object, SAMPLE_DRIVER_UNREGISTER_DEVICE, data, nullptr);
    ASSERT_TRUE(ret == HDF_SUCCESS);

    count = servstatWaitTime;
    while (!issd->callbacked && count > 0) {
        OsalMSleep(1);
        count--;
    }
    ASSERT_TRUE(issd->callbacked);
    ASSERT_EQ(issd->servName, newServName);
    ASSERT_EQ(issd->devClass, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(issd->servInfo, std::string(SAMPLE_SERVICE));
    ASSERT_EQ(issd->servStatus, SERVIE_STATUS_STOP);

    HdfIoServiceRecycle(testService);
    HdfSbufRecycle(data);
}

HWTEST_F(IoServiceTest, HdfIoService015, TestSize.Level0)
{
    struct ISvcMgrIoservice *servmgr = SvcMgrIoserviceGet();
    ASSERT_NE(servmgr, nullptr);

    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    struct IoServiceStatusData issd;
    struct ServiceStatusListener *listener = IoServiceStatusListenerNewInstance();
    listener->callback = TestOnServiceStatusReceived;
    listener->priv = (void *)&issd;

    int status = servmgr->RegisterServiceStatusListener(servmgr, listener, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(status, HDF_SUCCESS);

    TestServiceStop(&issd);

    status = servmgr->UnregisterServiceStatusListener(servmgr, listener);
    ASSERT_EQ(status, HDF_SUCCESS);

    IoServiceStatusListenerFree(listener);
    SvcMgrIoserviceRelease(servmgr);
}

/* *
 * @tc.name: HdfIoService016
 * @tc.desc: ioservice status listener update info test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IoServiceTest, HdfIoService016, TestSize.Level0)
{
    struct ISvcMgrIoservice *servmgr = SvcMgrIoserviceGet();
    ASSERT_NE(servmgr, nullptr);

    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    struct IoServiceStatusData issd;
    struct ServiceStatusListener *listener = IoServiceStatusListenerNewInstance();
    listener->callback = TestOnServiceStatusReceived;
    listener->priv = (void *)&issd;

    int status = servmgr->RegisterServiceStatusListener(servmgr, listener, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(status, HDF_SUCCESS);

    struct HdfIoService *testService = HdfIoServiceBind(SAMPLE_SERVICE);
    ASSERT_TRUE(testService != nullptr);
    HdfSBuf *data = HdfSbufObtainDefaultSize();
    ASSERT_TRUE(data != nullptr);

    std::string servinfo = "foo";
    ASSERT_TRUE(HdfSbufWriteString(data, servinfo.data()));
    int ret = testService->dispatcher->Dispatch(&testService->object, SAMPLE_DRIVER_UPDATE_SERVICE_INFO, data, nullptr);
    ASSERT_EQ(ret, HDF_SUCCESS);

    int count = servstatWaitTime;
    while (!issd.callbacked && count > 0) {
        OsalMSleep(1);
        count--;
    }
    ASSERT_TRUE(issd.callbacked);
    ASSERT_EQ(issd.servName, SAMPLE_SERVICE);
    ASSERT_EQ(issd.devClass, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(issd.servInfo, servinfo);
    ASSERT_EQ(issd.servStatus, SERVIE_STATUS_CHANGE);

    status = servmgr->UnregisterServiceStatusListener(servmgr, listener);
    ASSERT_EQ(status, HDF_SUCCESS);

    IoServiceStatusListenerFree(listener);
    HdfIoServiceRecycle(testService);
    SvcMgrIoserviceRelease(servmgr);
    HdfSbufRecycle(data);
}

/* *
 * @tc.name: HdfIoService017
 * @tc.desc: ioservice status listener unregister test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IoServiceTest, HdfIoService017, TestSize.Level0)
{
    struct ISvcMgrIoservice *servmgr = SvcMgrIoserviceGet();
    ASSERT_NE(servmgr, nullptr);

    struct HdfIoService *serv = HdfIoServiceBind(testSvcName);
    ASSERT_NE(serv, nullptr);
    serv->priv = (void *)"serv";

    struct IoServiceStatusData issd;
    struct ServiceStatusListener *listener = IoServiceStatusListenerNewInstance();
    listener->callback = TestOnServiceStatusReceived;
    listener->priv = (void *)&issd;

    HDF_LOGI("%{public}s:%{public}d", __func__, __LINE__);
    int status = servmgr->RegisterServiceStatusListener(servmgr, listener, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(status, HDF_SUCCESS);

    struct HdfIoService *testService = HdfIoServiceBind(SAMPLE_SERVICE);
    ASSERT_TRUE(testService != nullptr);
    HdfSBuf *data = HdfSbufObtainDefaultSize();
    ASSERT_TRUE(data != nullptr);
    const char *newServName = "sample_service1";
    ASSERT_TRUE(HdfSbufWriteString(data, "sample_driver"));
    ASSERT_TRUE(HdfSbufWriteString(data, newServName));
    int ret = testService->dispatcher->Dispatch(&testService->object, SAMPLE_DRIVER_REGISTER_DEVICE, data, nullptr);
    ASSERT_EQ(ret, HDF_SUCCESS);

    int count = 10;
    while (!issd.callbacked && count > 0) {
        OsalMSleep(1);
        count--;
    }
    ASSERT_TRUE(issd.callbacked);
    ASSERT_EQ(issd.servName, newServName);
    ASSERT_EQ(issd.devClass, DEVICE_CLASS_DEFAULT);
    ASSERT_EQ(issd.servInfo, std::string(SAMPLE_SERVICE));
    ASSERT_EQ(issd.servStatus, SERVIE_STATUS_START);

    status = servmgr->UnregisterServiceStatusListener(servmgr, listener);
    ASSERT_EQ(status, HDF_SUCCESS);

    issd.callbacked = false;
    ret = testService->dispatcher->Dispatch(&testService->object, SAMPLE_DRIVER_UNREGISTER_DEVICE, data, nullptr);
    ASSERT_EQ(status, HDF_SUCCESS);

    OsalMSleep(10);

    ASSERT_FALSE(issd.callbacked);
    status = servmgr->UnregisterServiceStatusListener(servmgr, listener);
    ASSERT_NE(status, HDF_SUCCESS);
    IoServiceStatusListenerFree(listener);
    HdfIoServiceRecycle(testService);
    SvcMgrIoserviceRelease(servmgr);
    HdfSbufRecycle(data);
}
