/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>

#include "hdf_io_service.h"
#include "hdf_uhdf_test.h"
#include "osal_time.h"
#include "sample_driver_test.h"

using namespace testing::ext;

class HdfManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfManagerTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfManagerTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfManagerTest::SetUp() {}

void HdfManagerTest::TearDown() {}

/**
 * @tc.name: HdfIoServiceBind001
 * @tc.desc: service bind test
 * @tc.type: FUNC
 * @tc.require: AR000F8698 AR000F8699 AR000F869A AR000F869B AR000F869C
 */
HWTEST_F(HdfManagerTest, HdfIoServiceBind001, TestSize.Level0)
{
    const char *svcName = "HDF_TEST";
    struct HdfIoService *hdfSvc = HdfIoServiceBind(svcName);
    EXPECT_TRUE(hdfSvc != nullptr);
    HdfIoServiceRecycle(hdfSvc);
}

/**
 * @tc.name: HdfIoServiceBind002
 * @tc.desc: service bind test
 * @tc.type: FUNC
 * @tc.require: AR000F8698 AR000F8699 AR000F869A AR000F869B AR000F869C
 */
HWTEST_F(HdfManagerTest, HdfIoServiceBind002, TestSize.Level0)
{
    struct HdfIoService *hdfSvc = HdfIoServiceBind(DEV_MGR_NODE);
    EXPECT_TRUE(hdfSvc != nullptr);
    HdfIoServiceRecycle(hdfSvc);
}

/**
 * @tc.name: HdfRegisterDevice001
 * @tc.desc: register device
 * @tc.type: FUNC
 * @tc.require: SR000F8697
 */
HWTEST_F(HdfManagerTest, HdfRegisterDevice001, TestSize.Level0)
{
    int32_t ret = HDF_FAILURE;
    struct HdfSBuf *data = nullptr;
    struct HdfIoService *ioService = HdfIoServiceBind(SAMPLE_SERVICE);
    ASSERT_TRUE(ioService != nullptr);
    data = HdfSbufObtainDefaultSize();
    EXPECT_TRUE(data != nullptr);
    EXPECT_TRUE(HdfSbufWriteString(data, "sample_driver"));
    EXPECT_TRUE(HdfSbufWriteString(data, "sample_service1"));
    uint64_t timeBefore = OsalGetSysTimeMs();
    ret = ioService->dispatcher->Dispatch(&ioService->object, SAMPLE_DRIVER_REGISTER_DEVICE, data, nullptr);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    uint64_t timeAfter = OsalGetSysTimeMs();
    EXPECT_TRUE((timeAfter - timeBefore) < 100);

    struct HdfIoService *ioService1 = HdfIoServiceBind("sample_service1");
    ASSERT_TRUE(ioService1 != nullptr);
    HdfIoServiceRecycle(ioService1);

    ret = ioService->dispatcher->Dispatch(&ioService->object, SAMPLE_DRIVER_UNREGISTER_DEVICE, data, nullptr);
    EXPECT_TRUE(ret == HDF_SUCCESS);

    ioService1 = HdfIoServiceBind("sample_service1");
    EXPECT_TRUE(ioService1 == nullptr);
    HdfIoServiceRecycle(ioService);
    HdfIoServiceRecycle(ioService1);
    HdfSbufRecycle(data);
}

/**
 * @tc.name: HdfGetServiceNameByDeviceClass001
 * @tc.desc: get service test
 * @tc.type: FUNC
 * @tc.require: AR000F8698 AR000F8699 AR000F869A AR000F869B AR000F869C
 */
HWTEST_F(HdfManagerTest, HdfGetServiceNameByDeviceClass001, TestSize.Level0)
{
    struct HdfSBuf *data = HdfSbufObtain(2000);
    ASSERT_TRUE(data != nullptr);

    bool flag = false;
    for (size_t i = DEVICE_CLASS_DEFAULT; i < DEVICE_CLASS_MAX; i++) {
        int32_t ret = HdfGetServiceNameByDeviceClass((DeviceClass)i, data);
        std::cout << "clasee " << i << " device list:" << std::endl;
        EXPECT_TRUE(ret == HDF_SUCCESS);
        const char *svcName = nullptr;
        while (true) {
            svcName = HdfSbufReadString(data);
            if (svcName == nullptr) {
                break;
            }
            std::cout << svcName << std::endl;
            if (strcmp(svcName, "sample_service") == 0) {
                flag = true;
            }
        }
        HdfSbufFlush(data);
    }
    HdfSbufRecycle(data);
    EXPECT_TRUE(flag);
}
