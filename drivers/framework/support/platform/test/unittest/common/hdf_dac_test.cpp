/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "dac_test.h"
#include "hdf_io_service_if.h"

using namespace testing::ext;

class HdfLiteDacTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteDacTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteDacTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteDacTest::SetUp()
{
}

void HdfLiteDacTest::TearDown()
{
}

/**
  * @tc.name: DacTestRead001
  * @tc.desc: dac write test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteDacTest, DacTestWrite001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_DAC_TYPE, DAC_TEST_CMD_WRITE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    printf("%s: kernel test done, then for user...\n", __func__);
    EXPECT_EQ(0, DacTestExecute(DAC_TEST_CMD_WRITE));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: DacTestMultiThread001
  * @tc.desc: dac multi thread test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteDacTest, DacTestMultiThread001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_DAC_TYPE, DAC_TEST_CMD_MULTI_THREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    printf("%s: kernel test done, then for user...\n", __func__);
    EXPECT_EQ(0, DacTestExecute(DAC_TEST_CMD_MULTI_THREAD));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: DacTestReliability001
  * @tc.desc: dac reliability test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteDacTest, DacTestReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_DAC_TYPE, DAC_TEST_CMD_RELIABILITY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    printf("%s: kernel test done, then for user...\n", __func__);
    EXPECT_EQ(0, DacTestExecute(DAC_TEST_CMD_RELIABILITY));
    printf("%s: exit!\n", __func__);
}

/**
  * @tc.name: DacIfPerformanceTest001
  * @tc.desc: dac user if performance test
  * @tc.type: FUNC
  * @tc.require:
  */
HWTEST_F(HdfLiteDacTest, DacIfPerformanceTest001, TestSize.Level1)
{
    EXPECT_EQ(0, DacTestExecute(DAC_TEST_CMD_IF_PERFORMANCE));
}
