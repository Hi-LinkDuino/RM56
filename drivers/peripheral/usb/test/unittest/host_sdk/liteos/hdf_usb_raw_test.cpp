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
#include "hdf_uhdf_test.h"
#include "hdf_io_service_if.h"

using namespace testing::ext;

#define HDF_TEST_NAME "/dev/hdf_test"
namespace {
enum HdfLiteUsbRawTestCmd {
    USB_RAW_SDK_IF_START_IO,
    USB_RAW_SDK_IF_INIT_001_TEST,
    USB_RAW_SDK_IF_EXIT_001_TEST,
    USB_RAW_SDK_IF_INIT_002_TEST,
    USB_RAW_SDK_IF_EXIT_002_TEST,
    USB_RAW_SDK_IF_INIT_003_TEST,
    USB_RAW_SDK_IF_INIT_004_TEST,
    USB_RAW_SDK_IF_INIT_005_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_001_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_002_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_003_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_004_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_005_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_006_TEST,
    USB_RAW_SDK_IF_RESET_DEVICE_001_TEST,
    USB_RAW_SDK_IF_RESET_DEVICE_002_TEST,
    USB_RAW_SDK_IF_CLOSE_DEVICE_001_TEST,
    USB_RAW_SDK_IF_CLOSE_DEVICE_002_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_007_TEST,
    USB_RAW_SDK_IF_GET_CONFIGURATION_001_TEST,
    USB_RAW_SDK_IF_GET_CONFIGURATION_002_TEST,
    USB_RAW_SDK_IF_GET_CONFIGURATION_003_TEST,
    USB_RAW_SDK_IF_GET_CONFIGURATION_004_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_001_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_002_TEST,
    USB_RAW_SDK_IF_GET_CONFIG_DESC_001_TEST,
    USB_RAW_SDK_IF_GET_CONFIG_DESC_002_TEST,
    USB_RAW_SDK_IF_GET_CONFIG_DESC_003_TEST,
    USB_RAW_SDK_IF_GET_CONFIG_DESC_004_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_001_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_002_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_003_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_004_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_005_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_006_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_007_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_008_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_DESC_001_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_DESC_002_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_DESC_003_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_DESC_004_TEST,
    USB_RAW_SDK_IF_GET_CONFIG_DESC_005_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_DESC_005_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_001_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_002_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_003_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_004_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_005_TEST,
    USB_RAW_SDK_IF_RELEASE_INTERFACE_001_TEST,
    USB_RAW_SDK_IF_RELEASE_INTERFACE_002_TEST,
    USB_RAW_SDK_IF_RELEASE_INTERFACE_003_TEST,
    USB_RAW_SDK_IF_RELEASE_INTERFACE_004_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_006_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_001_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_002_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_003_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_004_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_005_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_006_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_007_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_008_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_010_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_011_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_006_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_001_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_002_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_003_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_005_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_006_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_001_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_002_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_003_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_005_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_009_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_001_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_002_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_003_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_004_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_005_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_006_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_007_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_008_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_009_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_010_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_011_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_012_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_013_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_014_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_015_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_016_TEST,
    USB_RAW_SDK_IF_FILL_BULK_REQUEST_001_TEST,
    USB_RAW_SDK_IF_FILL_BULK_REQUEST_002_TEST,
    USB_RAW_SDK_IF_FILL_INT_REQUEST_001_TEST,
    USB_RAW_SDK_IF_FILL_INT_REQUEST_002_TEST,
    USB_RAW_SDK_IF_FILL_INT_REQUEST_003_TEST,
    USB_RAW_SDK_IF_FILL_INT_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_001_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_002_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_003_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_005_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_006_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_007_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_008_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_SETUP_001_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_SETUP_002_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_SETUP_003_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_SETUP_004_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_001_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_002_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_003_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_004_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_005_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_006_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_007_TEST,
    USB_RAW_SDK_IF_SEND_BULK_REQUEST_001_TEST,
    USB_RAW_SDK_IF_SEND_BULK_REQUEST_002_TEST,
    USB_RAW_SDK_IF_SEND_BULK_REQUEST_003_TEST,
    USB_RAW_SDK_IF_SEND_BULK_REQUEST_004_TEST,
    USB_RAW_SDK_IF_SEND_BULK_REQUEST_005_TEST,
    USB_RAW_SDK_IF_SEND_INT_REQUEST_001_TEST,
    USB_RAW_SDK_IF_SEND_INT_REQUEST_002_TEST,
    USB_RAW_SDK_IF_SEND_INT_REQUEST_003_TEST,
    USB_RAW_SDK_IF_SEND_INT_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FILL_BULK_REQUEST_003_TEST,
    USB_RAW_SDK_IF_FILL_BULK_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FILL_INT_REQUEST_005_TEST,
    USB_RAW_SDK_IF_SUBMIT_REQUEST_001_TEST,
    USB_RAW_SDK_IF_SUBMIT_REQUEST_002_TEST,
    USB_RAW_SDK_IF_SUBMIT_REQUEST_003_TEST,
    USB_RAW_SDK_IF_SUBMIT_REQUEST_004_TEST,
    USB_RAW_SDK_IF_CANCEL_REQUEST_001_TEST,
    USB_RAW_SDK_IF_CANCEL_REQUEST_002_TEST,
    USB_RAW_SDK_IF_CANCEL_REQUEST_003_TEST,
    USB_RAW_SDK_IF_CANCEL_REQUEST_004_TEST,
    USB_RAW_SDK_IF_STOP_IO,
};

class HdfUsbRawTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void HdfUsbRawTest::SetUpTestCase()
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_START_IO, -1};
    HdfTestOpenService();
    HdfTestSendMsgToService(&msg);
}

void HdfUsbRawTest::TearDownTestCase()
{
    HdfTestCloseService();
}

/**
 * @tc.number    : CheckRawSdkIfInit001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfInit001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_INIT_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfExit001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfExit001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_EXIT_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfInit002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfInit002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_INIT_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfExit002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfExit002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_EXIT_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfInit003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfInit003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_INIT_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfInit004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfInit004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_INIT_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfInit005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfInit005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_INIT_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfOpenDevice001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_OPEN_DEVICE_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfOpenDevice002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_OPEN_DEVICE_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfOpenDevice003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_OPEN_DEVICE_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfOpenDevice004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_OPEN_DEVICE_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfOpenDevice005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_OPEN_DEVICE_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfOpenDevice006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_OPEN_DEVICE_006_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfResetDevice001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfResetDevice001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_RESET_DEVICE_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfResetDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfResetDevice002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_RESET_DEVICE_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfCloseDevice001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfCloseDevice001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CLOSE_DEVICE_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfCloseDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfCloseDevice002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CLOSE_DEVICE_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice007
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfOpenDevice007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_OPEN_DEVICE_007_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfiguration001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetConfiguration001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIGURATION_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfiguration002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetConfiguration002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIGURATION_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfiguration003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetConfiguration003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIGURATION_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfiguration004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetConfiguration004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIGURATION_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDevice001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDevice001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DEVICE_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDevice002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DEVICE_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetConfigDescriptor001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIG_DESC_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetConfigDescriptor002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIG_DESC_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetConfigDescriptor003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIG_DESC_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetConfigDescriptor004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIG_DESC_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSetConfiguration001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SET_CONFIGURATION_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSetConfiguration002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SET_CONFIGURATION_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSetConfiguration003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SET_CONFIGURATION_003_TEST, -1};
    EXPECT_EQ(HDF_FAILURE, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSetConfiguration004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SET_CONFIGURATION_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSetConfiguration005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SET_CONFIGURATION_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSetConfiguration006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SET_CONFIGURATION_006_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration007
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSetConfiguration007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SET_CONFIGURATION_007_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration008
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSetConfiguration008, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SET_CONFIGURATION_008_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDeviceDescriptor001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDeviceDescriptor001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DEVICE_DESC_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDeviceDescriptor002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDeviceDescriptor002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DEVICE_DESC_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDeviceDescriptor003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDeviceDescriptor003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DEVICE_DESC_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDeviceDescriptor004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDeviceDescriptor004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DEVICE_DESC_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetConfigDescriptor005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIG_DESC_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDeviceDescriptor005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDeviceDescriptor005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DEVICE_DESC_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfClaimInterface001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CLAMIN_INTERFACE_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfClaimInterface002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CLAMIN_INTERFACE_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfClaimInterface003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CLAMIN_INTERFACE_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfClaimInterface004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CLAMIN_INTERFACE_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfClaimInterface005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CLAMIN_INTERFACE_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfReleaseInterface001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_RELEASE_INTERFACE_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfReleaseInterface002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_RELEASE_INTERFACE_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfReleaseInterface003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_RELEASE_INTERFACE_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfReleaseInterface004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_RELEASE_INTERFACE_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfClaimInterface006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CLAMIN_INTERFACE_006_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
 * @tc.number    : CheckRawSdkIfAllocRequest006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_006_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
 * @tc.number    : CheckRawSdkIfAllocRequest007
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_007_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest008
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest008, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_008_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
 * @tc.number    : CheckRawSdkIfAllocRequest010
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest010, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_010_TEST, -1};
    EXPECT_EQ(HDF_FAILURE, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest011
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest011, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_011_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillIsoRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillIsoRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_ISO_REQUEST_001_TEST, -1};
    EXPECT_EQ(HDF_FAILURE, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillIsoRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillIsoRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_ISO_REQUEST_002_TEST, -1};
    EXPECT_EQ(HDF_FAILURE, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillIsoRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillIsoRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_ISO_REQUEST_003_TEST, -1};
    EXPECT_EQ(HDF_FAILURE, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillIsoRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillIsoRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_ISO_REQUEST_004_TEST, -1};
    EXPECT_EQ(HDF_FAILURE, HdfTestSendMsgToService(&msg));
}
/**
 * @tc.number    : CheckRawSdkIfFillIsoRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillIsoRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_ISO_REQUEST_005_TEST, -1};
    EXPECT_EQ(HDF_FAILURE, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillIsoRequest006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillIsoRequest006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_ISO_REQUEST_006_TEST, -1};
    EXPECT_EQ(HDF_FAILURE, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFreeRequest006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_006_TEST, -1};
    EXPECT_EQ(HDF_FAILURE, HdfTestSendMsgToService(&msg));
}
/**
 * @tc.number    : CheckRawSdkIfFreeRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFreeRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFreeRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFreeRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFreeRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFreeRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest009
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfAllocRequest009, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_009_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor007
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_007_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor008
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor008, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_008_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor009
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor009, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_009_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor010
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor010, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_010_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor011
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor011, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_011_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor012
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor012, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_012_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor013
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor013, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_013_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor014
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor014, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_014_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor015
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor015, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_015_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor016
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfGetDescriptor016, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DESCRIPTION_016_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillBulkRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillBulkRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_BULK_REQUEST_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillBulkRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillBulkRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_BULK_REQUEST_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillInterruptRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillInterruptRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_INT_REQUEST_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillInterruptRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillInterruptRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_INT_REQUEST_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillInterruptRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillInterruptRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_INT_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillInterruptRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillInterruptRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_INT_REQUEST_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlRequest006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_006_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest007
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlRequest007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_007_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest008
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlRequest008, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_008_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlSetup001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlSetup001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_SETUP_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlSetup002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlSetup002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_SETUP_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlSetup003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlSetup003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_SETUP_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlSetup004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillControlSetup004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_SETUP_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendControlRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendControlRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendControlRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendControlRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendControlRequest006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_006_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest007
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendControlRequest007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_007_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendBulkRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendBulkRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_BULK_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendBulkRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendBulkRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_BULK_REQUEST_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendBulkRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendBulkRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_BULK_REQUEST_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendInterruptRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendInterruptRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_INT_REQUEST_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendInterruptRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendInterruptRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_INT_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendInterruptRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSendInterruptRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_INT_REQUEST_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillBulkRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillBulkRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_BULK_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillBulkRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillBulkRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_BULK_REQUEST_004_TEST, -1};
    EXPECT_EQ(-1, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillInterruptRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFillInterruptRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_INT_REQUEST_005_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSubmitRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSubmitRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SUBMIT_REQUEST_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSubmitRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSubmitRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SUBMIT_REQUEST_002_TEST, -1};
    EXPECT_EQ(-1, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSubmitRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSubmitRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SUBMIT_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSubmitRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfSubmitRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SUBMIT_REQUEST_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfCancelRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfCancelRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CANCEL_REQUEST_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfCancelRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfCancelRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CANCEL_REQUEST_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfCancelRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfCancelRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CANCEL_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfCancelRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfCancelRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CANCEL_REQUEST_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : UsbStopIo
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, UsbStopIo, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_STOP_IO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFreeRequest004_close, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFreeRequest003_close, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_003_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFreeRequest001_close, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfFreeRequest002_close, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfReleaseInterface004_close, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_RELEASE_INTERFACE_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfReleaseInterface002_close, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_RELEASE_INTERFACE_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfCloseDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfCloseDevice002_close, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CLOSE_DEVICE_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfExit001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawTest, CheckRawSdkIfExit001_close, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_EXIT_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
