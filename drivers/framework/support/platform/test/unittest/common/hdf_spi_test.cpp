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
#include "spi_test.h"

using namespace testing::ext;

class HdfLiteSpiTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteSpiTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteSpiTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteSpiTest::SetUp()
{
}

void HdfLiteSpiTest::TearDown()
{
}

/**
  * @tc.name: SpiTransferTest001
  * @tc.desc: spi function test
  * @tc.type: FUNC
  * @tc.require: SR000DQ0VO
  */
HWTEST_F(HdfLiteSpiTest, SpiTransferTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SPI_TYPE, SPI_TRANSFER_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, SpiTestExecute(SPI_TRANSFER_TEST));
}

/**
  * @tc.name: SpiMultiTransferTest001
  * @tc.desc: spi function test
  * @tc.type: FUNC
  * @tc.require: SR000DQ0VO
  */
#ifdef __LITEOS__
HWTEST_F(HdfLiteSpiTest, SpiMultiTransferTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SPI_TYPE, SPI_MULTI_TRANSFER_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, SpiTestExecute(SPI_MULTI_TRANSFER_TEST));
}
#endif

/**
  * @tc.name: SpiDmaTransferTest001
  * @tc.desc: Spi function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteSpiTest, SpiDmaTransferTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SPI_TYPE, SPI_DMA_TRANSFER_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, SpiTestExecute(SPI_DMA_TRANSFER_TEST));
}

/**
  * @tc.name: SpiIntTransferTest001
  * @tc.desc: Spi function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteSpiTest, SpiIntTransferTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SPI_TYPE, SPI_INT_TRANSFER_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, SpiTestExecute(SPI_INT_TRANSFER_TEST));
}

/**
  * @tc.name: SpiReliabilityTest001
  * @tc.desc: spi function test
  * @tc.type: FUNC
  * @tc.require: SR000DQ0VO
  */
HWTEST_F(HdfLiteSpiTest, SpiReliabilityTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_SPI_TYPE, SPI_RELIABILITY_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, SpiTestExecute(SPI_RELIABILITY_TEST));
}

/**
  * @tc.name: SpiIfPerformanceTest001
  * @tc.desc: spi function test
  * @tc.type: FUNC
  * @tc.require: SR000DQ0VO
  */
HWTEST_F(HdfLiteSpiTest, SpiIfPerformanceTest001, TestSize.Level1)
{
    EXPECT_EQ(0, SpiTestExecute(SPI_PERFORMANCE_TEST));
}
