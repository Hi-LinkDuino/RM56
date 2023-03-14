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
#include "emmc_if.h"
#include "hdf_io_service_if.h"
#include "hdf_uhdf_test.h"

using namespace testing::ext;

enum EmmcTestCmd {
    EMMC_GET_CID_01 = 0,
};

class HdfLiteEmmcTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteEmmcTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteEmmcTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteEmmcTest::SetUp()
{
}

void HdfLiteEmmcTest::TearDown()
{
}

static void TestUserEmmcGetCid(void)
{
    uint8_t cid[EMMC_CID_LEN] = {0};
    uint32_t i;

    EmmcGetHuid(cid, EMMC_CID_LEN);
    for (i = 0; i < EMMC_CID_LEN; i++) {
        printf("user interface get cid[%u] = 0x%x\n", i, cid[i]);
    }
}

/**
  * @tc.name: EmmcGetCid001
  * @tc.desc: test EmmcGetCid/EmmcGetHuid interface in kernel and user status.
  * @tc.type: FUNC
  * @tc.require: AR000F5LSD AR000F5LV3
  */
HWTEST_F(HdfLiteEmmcTest, EmmcGetCid001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_EMMC_TYPE, EMMC_GET_CID_01, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    TestUserEmmcGetCid();
}
