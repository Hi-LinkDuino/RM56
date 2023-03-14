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
#include <string>
#include <unistd.h>
#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"
#include "hdf_io_service_if.h"
#include "uart_test.h"

using namespace testing::ext;

class HdfLiteUartTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteUartTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteUartTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteUartTest::SetUp()
{
}

void HdfLiteUartTest::TearDown()
{
}

/**
 * @tc.name: UartSetTransModeTest001
 * @tc.desc: uart function test
 * @tc.type: FUNC
 * @tc.require: AR000F8689
 */
HWTEST_F(HdfLiteUartTest, UartSetTransModeTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_UART_TYPE, UART_TEST_CMD_SET_TRANSMODE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, UartTestExecute(UART_TEST_CMD_SET_TRANSMODE));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: UartWriteTest001
  * @tc.desc: uart function test
  * @tc.type: FUNC
  * @tc.require: AR000F8689
  */
HWTEST_F(HdfLiteUartTest, UartWriteTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_UART_TYPE, UART_TEST_CMD_WRITE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, UartTestExecute(UART_TEST_CMD_WRITE));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: UartReadTest001
  * @tc.desc: uart function test
  * @tc.type: FUNC
  * @tc.require: AR000F8689
  */
HWTEST_F(HdfLiteUartTest, UartReadTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_UART_TYPE, UART_TEST_CMD_READ, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, UartTestExecute(UART_TEST_CMD_READ));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: UartSetBaudTest001
  * @tc.desc: uart function test
  * @tc.type: FUNC
  * @tc.require: AR000F8689
  */
HWTEST_F(HdfLiteUartTest, UartSetBaudTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_UART_TYPE, UART_TEST_CMD_SET_BAUD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, UartTestExecute(UART_TEST_CMD_SET_BAUD));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: UartGetBaudTest001
  * @tc.desc: uart function test
  * @tc.type: FUNC
  * @tc.require: AR000F8689
  */
HWTEST_F(HdfLiteUartTest, UartGetBaudTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_UART_TYPE, UART_TEST_CMD_GET_BAUD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, UartTestExecute(UART_TEST_CMD_GET_BAUD));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: UartSetAttributeTest001
  * @tc.desc: uart function test
  * @tc.type: FUNC
  * @tc.require: AR000F8689
  */
HWTEST_F(HdfLiteUartTest, UartSetAttributeTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_UART_TYPE, UART_TEST_CMD_SET_ATTRIBUTE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, UartTestExecute(UART_TEST_CMD_SET_ATTRIBUTE));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: UartGetAttributeTest001
  * @tc.desc: uart function test
  * @tc.type: FUNC
  * @tc.require: AR000F8689
  */
HWTEST_F(HdfLiteUartTest, UartGetAttributeTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_UART_TYPE, UART_TEST_CMD_GET_ATTRIBUTE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, UartTestExecute(UART_TEST_CMD_GET_ATTRIBUTE));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: UartReliabilityTest001
  * @tc.desc: uart function test
  * @tc.type: FUNC
  * @tc.require: AR000F8689
  */
HWTEST_F(HdfLiteUartTest, UartReliabilityTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_UART_TYPE, UART_TEST_CMD_RELIABILITY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, UartTestExecute(UART_TEST_CMD_RELIABILITY));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: UartIfPerformanceTest001
  * @tc.desc: uart user if performance test
  * @tc.type: FUNC
  * @tc.require:
  */
HWTEST_F(HdfLiteUartTest, UartIfPerformanceTest001, TestSize.Level1)
{
    EXPECT_EQ(0, UartTestExecute(UART_TEST_CMD_PERFORMANCE));
}
