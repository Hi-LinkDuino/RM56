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
#include "gpio_test.h"
#include "hdf_uhdf_test.h"
#include "hdf_io_service_if.h"

using namespace testing::ext;

class HdfLiteGpioTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteGpioTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteGpioTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteGpioTest::SetUp()
{
}

void HdfLiteGpioTest::TearDown()
{
}

/**
  * @tc.name: GpioTestSetGetDir001
  * @tc.desc: gpio set and get dir test
  * @tc.type: FUNC
  * @tc.require: AR000F868H
  */
HWTEST_F(HdfLiteGpioTest, GpioTestSetGetDir001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_GPIO_TYPE, GPIO_TEST_SET_GET_DIR, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, GpioTestExecute(GPIO_TEST_SET_GET_DIR));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: GpioTestWriteRead001
  * @tc.desc: gpio write and read test
  * @tc.type: FUNC
  * @tc.require: AR000F868H
  */
HWTEST_F(HdfLiteGpioTest, GpioTestWriteRead001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_GPIO_TYPE, GPIO_TEST_WRITE_READ, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, GpioTestExecute(GPIO_TEST_WRITE_READ));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: GpioTestIrqLevel001
  * @tc.desc: gpio level irq trigger test
  * @tc.type: FUNC
  * @tc.require: AR000F868H
  */
HWTEST_F(HdfLiteGpioTest, GpioTestIrqLevel001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_GPIO_TYPE, GPIO_TEST_IRQ_LEVEL, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, GpioTestExecute(GPIO_TEST_IRQ_LEVEL));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: GpioTestIrqEdge001
  * @tc.desc: gpio edge irq trigger test
  * @tc.type: FUNC
  * @tc.require: AR000F868H
  */
HWTEST_F(HdfLiteGpioTest, GpioTestIrqEdge001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_GPIO_TYPE, GPIO_TEST_IRQ_EDGE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, GpioTestExecute(GPIO_TEST_IRQ_EDGE));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: GpioTestIrqThread001
  * @tc.desc: gpio thread irq trigger test
  * @tc.type: FUNC
  * @tc.require: AR000F868H
  */
HWTEST_F(HdfLiteGpioTest, GpioTestIrqThread001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_GPIO_TYPE, GPIO_TEST_IRQ_THREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, GpioTestExecute(GPIO_TEST_IRQ_THREAD));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: GpioTestReliability001
  * @tc.desc: gpio reliability test
  * @tc.type: FUNC
  * @tc.require: AR000F868H
  */
HWTEST_F(HdfLiteGpioTest, GpioTestReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_GPIO_TYPE, GPIO_TEST_RELIABILITY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    printf("%s: kernel test done, then for user...\n", __func__);

    EXPECT_EQ(0, GpioTestExecute(GPIO_TEST_RELIABILITY));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: GpioIfPerformanceTest001
  * @tc.desc: gpio user if performance test
  * @tc.type: FUNC
  * @tc.require:
  */
HWTEST_F(HdfLiteGpioTest, GpioIfPerformanceTest001, TestSize.Level1)
{
    EXPECT_EQ(0, GpioTestExecute(GPIO_TEST_PERFORMANCE));
}