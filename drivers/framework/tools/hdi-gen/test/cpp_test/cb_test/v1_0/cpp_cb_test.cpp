/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include <iostream>
#include <thread>
#include <unistd.h>
#include <gtest/gtest.h>
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "test/cpp_test/cb_test/v1_0/client/cb_test_proxy.h"

using namespace OHOS;
using namespace testing::ext;
using namespace test::cpp_test::cb_test::v1_0;

#define HDF_LOG_TAG sapmle_c_client_test

static sptr<ICbTest> g_testClient = nullptr;

static sptr<ICallback> g_callbackObj = nullptr;

class CppCbTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

void CppCbTest::SetUpTestCase()
{
    g_testClient = ICbTest::Get();
    if (g_testClient == nullptr) {
        printf("CppCbTest: get g_testClient failed.\n");
    }
    g_callbackObj = new CallbackService();
    if (g_callbackObj == nullptr) {
        printf("CppCbTest: get g_callbackObj failed.\n");
    }
}

HWTEST_F(CppCbTest, CppCbTest_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, g_testClient);
}

HWTEST_F(CppCbTest, CppCbTest_002, TestSize.Level1)
{
    int32_t ec = g_testClient->CallbackTest(g_callbackObj);
    ASSERT_EQ(ec, HDF_SUCCESS);
}