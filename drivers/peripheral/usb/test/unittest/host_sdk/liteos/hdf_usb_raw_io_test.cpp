/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_usb_raw_io_test.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>
#include "hdf_uhdf_test.h"

using namespace testing::ext;

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

class HdfUsbRawIoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void HdfUsbRawIoTest::SetUpTestCase()
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_START_IO, -1};
    HdfTestOpenService();
    HdfTestSendMsgToService(&msg);
}

void HdfUsbRawIoTest::TearDownTestCase()
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_STOP_IO, -1};
    HdfTestSendMsgToService(&msg);
    HdfTestCloseService();
}


/**
 * @tc.number    : CheckRawSdkIfInit001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfInit001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_INIT_001_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfOpenDevice007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_OPEN_DEVICE_007_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfiguration004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfGetConfiguration004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIGURATION_004_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfGetDevice002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_DEVICE_002_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfGetConfigDescriptor004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_GET_CONFIG_DESC_004_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration008
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfSetConfiguration008, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SET_CONFIGURATION_008_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfClaimInterface006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_CLAMIN_INTERFACE_006_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest009
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfAllocRequest009, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_ALLOC_REQUEST_009_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillBulkRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfFillBulkRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_BULK_REQUEST_001_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillBulkRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfFillBulkRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_BULK_REQUEST_002_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillInterruptRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfFillInterruptRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_INT_REQUEST_001_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfFillControlRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_001_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFillControlSetup004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfFillControlSetup004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FILL_CONTROL_SETUP_004_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfSendControlRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_004_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendBulkRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfSendBulkRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_BULK_REQUEST_001_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfSendBulkRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfSendBulkRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_SEND_BULK_REQUEST_002_TEST, -1};
    printf("%s+++%d\n", __func__, __LINE__);
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfFreeRequest001, TestSize.Level1)
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
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfFreeRequest002, TestSize.Level1)
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
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfFreeRequest003, TestSize.Level1)
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
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfFreeRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_FREE_REQUEST_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfReleaseInterface002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_RELEASE_INTERFACE_002_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfReleaseInterface004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_RELEASE_INTERFACE_004_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.number    : CheckRawSdkIfCloseDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfCloseDevice002, TestSize.Level1)
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
HWTEST_F(HdfUsbRawIoTest, CheckRawSdkIfExit001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_HOST_RAW_TYPE, USB_RAW_SDK_IF_EXIT_001_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
