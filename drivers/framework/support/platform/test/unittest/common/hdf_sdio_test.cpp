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

enum SdioTestCmd {
    SDIO_DISABLE_FUNC_01 = 0,
    SDIO_ENABLE_FUNC_01,
    SDIO_SET_BLOCK_SIZE_01,
    SDIO_INCR_ADDR_READ_AND_WRITE_BYTES_01,
    SDIO_FIXED_ADDR_READ_AND_WRITE_BYTES_01,
    SDIO_FUNC0_READ_AND_WRITE_BYTES_01,
    SDIO_SET_AND_GET_COMMON_INFO_01,
};

class HdfLiteSdioTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteSdioTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteSdioTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteSdioTest::SetUp()
{
}

void HdfLiteSdioTest::TearDown()
{
}

/**
  * @tc.name: SdioDisableFunc001
  * @tc.desc: SdioDisableFunc Interface test
  * @tc.type: FUNC
  * @tc.require: AR000F868B
  */
HWTEST_F(HdfLiteSdioTest, SdioDisableFunc001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SDIO_TYPE, SDIO_DISABLE_FUNC_01, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: SdioEnableFunc001
  * @tc.desc: SdioEnableFunc Interface test
  * @tc.type: FUNC
  * @tc.require: AR000F868B
  */
HWTEST_F(HdfLiteSdioTest, SdioEnableFunc001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SDIO_TYPE, SDIO_ENABLE_FUNC_01, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: SdioSetBlockSize001
  * @tc.desc: SdioSetBlockSize Interface test
  * @tc.type: FUNC
  * @tc.require: AR000F868B
  */
HWTEST_F(HdfLiteSdioTest, SdioSetBlockSize001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SDIO_TYPE, SDIO_SET_BLOCK_SIZE_01, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: SdioIncrAddrReadAndWriteBytes001
  * @tc.desc: SdioReadBytes and SdioWriteBytes Interface test
  * @tc.type: FUNC
  * @tc.require: AR000F868B
  */
HWTEST_F(HdfLiteSdioTest, SdioIncrAddrReadAndWriteBytes001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SDIO_TYPE, SDIO_INCR_ADDR_READ_AND_WRITE_BYTES_01, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: SdioFixedAddrReadAndWriteBytes001
  * @tc.desc: SdioReadBytesFromFixedAddr and SdioWriteBytesToFixedAddr Interface test
  * @tc.type: FUNC
  * @tc.require: AR000F868B
  */
HWTEST_F(HdfLiteSdioTest, SdioFixedAddrReadAndWriteBytes001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SDIO_TYPE, SDIO_FIXED_ADDR_READ_AND_WRITE_BYTES_01, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: SdioFunc0ReadAndWriteBytes001
  * @tc.desc: SdioReadBytesFromFunc0 and SdioWriteBytesToFunc0 Interface test
  * @tc.type: FUNC
  * @tc.require: AR000F868B
  */
HWTEST_F(HdfLiteSdioTest, SdioFunc0ReadAndWriteBytes001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_SDIO_TYPE, SDIO_FUNC0_READ_AND_WRITE_BYTES_01, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: SdioSetAndGetCommonInfo001
  * @tc.desc: SdioGetCommonInfo and SdioSetCommonInfo Interface test
  * @tc.type: FUNC
  * @tc.require: AR000F868B
  */
HWTEST_F(HdfLiteSdioTest, SdioSetAndGetCommonInfo001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SDIO_TYPE, SDIO_SET_AND_GET_COMMON_INFO_01, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
