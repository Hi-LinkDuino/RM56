/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <iostream>
#include <thread>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "test/cpp_test/struct_test/v1_0/client/struct_test_proxy.h"

using namespace OHOS;
using namespace testing::ext;
using namespace test::cpp_test::struct_test::v1_0;
using namespace test::cpp_test::types::v1_0;

static sptr<IStructTest> g_testClient = nullptr;

class CppStructTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

void CppStructTest::SetUpTestCase()
{
    g_testClient = IStructTest::Get();
    if (g_testClient == nullptr) {
        printf("CppStructTest: get g_testClient failed.\n");
    }
}

HWTEST_F(CppStructTest, CppStructTest_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, g_testClient);
}

HWTEST_F(CppStructTest, CppStructTest_002, TestSize.Level1)
{
    SSample srcObj = {true, 1, 1000.125, "hello world"};

    SSample destObj;

    int32_t ec = g_testClient->SSampleTest(srcObj, destObj);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ((srcObj.m1 ? 1 : 0), (destObj.m1 ? 1 : 0));
    EXPECT_EQ(srcObj.m2, destObj.m2);
    EXPECT_DOUBLE_EQ(srcObj.m3, destObj.m3);
    EXPECT_EQ(srcObj.m4, destObj.m4);
}

HWTEST_F(CppStructTest, CppStructTest_003, TestSize.Level1)
{
    SSample2 srcObj = {true, 1, 2, 3, 4, 65, 20, 30, 40, 100.25, 1000.125};

    SSample2 destObj;

    int32_t ec = g_testClient->SSample2Test(srcObj, destObj);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ((srcObj.m1 ? 1 : 0), (destObj.m1 ? 1 : 0));
    EXPECT_EQ(srcObj.m2, destObj.m2);
    EXPECT_EQ(srcObj.m3, destObj.m3);
    EXPECT_EQ(srcObj.m4, destObj.m4);
    EXPECT_EQ(srcObj.m5, destObj.m5);
    EXPECT_EQ(srcObj.m6, destObj.m6);
    EXPECT_EQ(srcObj.m7, destObj.m7);
    EXPECT_EQ(srcObj.m8, destObj.m8);
    EXPECT_EQ(srcObj.m9, destObj.m9);
    EXPECT_FLOAT_EQ(srcObj.m10, destObj.m10);
    EXPECT_DOUBLE_EQ(srcObj.m11, destObj.m11);
}

HWTEST_F(CppStructTest, CppStructTest_004, TestSize.Level1)
{
    int fd = open("/fdtest3.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    SSample3 srcObj = {
        "hello world",
        ESample::MEM_ONE,
        {true, 1, 2, 3, 4, 65, 20, 30, 40, 100.25, 1000.125},
        fd,
    };

    SSample3 destObj;
    int32_t ec = g_testClient->SSample3Test(srcObj, destObj);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(srcObj.m1, destObj.m1);
    EXPECT_EQ(srcObj.m2, destObj.m2);
    EXPECT_EQ((srcObj.m3.m1 ? 1 : 0), (destObj.m3.m1 ? 1 : 0));
    EXPECT_EQ(srcObj.m3.m2, destObj.m3.m2);
    EXPECT_EQ(srcObj.m3.m3, destObj.m3.m3);
    EXPECT_EQ(srcObj.m3.m4, destObj.m3.m4);
    EXPECT_EQ(srcObj.m3.m5, destObj.m3.m5);
    EXPECT_EQ(srcObj.m3.m6, destObj.m3.m6);
    EXPECT_EQ(srcObj.m3.m7, destObj.m3.m7);
    EXPECT_EQ(srcObj.m3.m8, destObj.m3.m8);
    EXPECT_EQ(srcObj.m3.m9, destObj.m3.m9);
    EXPECT_FLOAT_EQ(srcObj.m3.m10, destObj.m3.m10);
    EXPECT_DOUBLE_EQ(srcObj.m3.m11, destObj.m3.m11);
    close(srcObj.m4);
    close(destObj.m4);
}

static void CompareSSample4Part1(const SSample4& srcObj, const SSample4& destObj) const
{
    for (size_t i = 0; i < srcObj.m1.size(); i++) {
        EXPECT_EQ((srcObj.m1[i] ? 1 : 0), (destObj.m1[i] ? 1 : 0));
    }

    for (size_t i = 0; i < srcObj.m2.size(); i++) {
        EXPECT_EQ(srcObj.m2[i], destObj.m2[i]);
    }

    for (size_t i = 0; i < srcObj.m3.size(); i++) {
        EXPECT_EQ(srcObj.m3[i], destObj.m3[i]);
    }

    for (size_t i = 0; i < srcObj.m4.size(); i++) {
        EXPECT_EQ(srcObj.m4[i], destObj.m4[i]);
    }

    for (size_t i = 0; i < srcObj.m5.size(); i++) {
        EXPECT_EQ(srcObj.m5[i], destObj.m5[i]);
    }
}

static void CompareSSample4Part2(const SSample4& srcObj, const SSample4& destObj) const
{
    for (size_t i = 0; i < srcObj.m6.size(); i++) {
        EXPECT_EQ(srcObj.m6[i], destObj.m6[i]);
    }

    for (size_t i = 0; i < srcObj.m7.size(); i++) {
        EXPECT_EQ(srcObj.m7[i], destObj.m7[i]);
    }

    for (size_t i = 0; i < srcObj.m8.size(); i++) {
        EXPECT_EQ(srcObj.m8[i], destObj.m8[i]);
    }

    for (size_t i = 0; i < srcObj.m9.size(); i++) {
        EXPECT_EQ(srcObj.m9[i], destObj.m9[i]);
    }

    for (size_t i = 0; i < srcObj.m10.size(); i++) {
        EXPECT_FLOAT_EQ(srcObj.m10[i], destObj.m10[i]);
    }

    for (size_t i = 0; i < srcObj.m11.size(); i++) {
        EXPECT_DOUBLE_EQ(srcObj.m11[i], destObj.m11[i]);
    }
}

static void CompareSSample4Part3(const SSample4& srcObj, const SSample4& destObj) const
{
    for (size_t i = 0; i < srcObj.m12.size(); i++) {
        EXPECT_EQ(srcObj.m12[i], destObj.m12[i]);
    }

    for (size_t i = 0; i < srcObj.m13.size(); i++) {
        EXPECT_EQ(srcObj.m13[i], destObj.m13[i]);
    }

    for (size_t i = 0; i < srcObj.m14.size(); i++) {
        EXPECT_EQ(((srcObj.m14[i]).m1 ? 1 : 0), ((destObj.m14[i]).m1 ? 1 : 0));
        EXPECT_EQ((srcObj.m14[i]).m2, (destObj.m14[i]).m2);
        EXPECT_DOUBLE_EQ((srcObj.m14[i]).m3, (destObj.m14[i]).m3);
        EXPECT_EQ((srcObj.m14[i]).m4, (destObj.m14[i]).m4);
    }

    for (size_t i = 0; i < srcObj.m15.size(); i++) {
        sptr<SequenceData> var1 = srcObj.m15[i];
        sptr<SequenceData> var2 = destObj.m15[i];

        if (var1 != nullptr && var2 != nullptr) {
            EXPECT_EQ(var1->m1_, var2->m1_);
            EXPECT_DOUBLE_EQ(var1->m2_, var2->m2_);
            EXPECT_EQ(var1->m3_, var2->m3_);
        } else {
            std::cout << "var1 or var2 is nullptr" << std::endl;
        }
    }
}

static void CompareSSample4(const SSample4& srcObj, const SSample4& destObj) const
{
    CompareSSample4Part1(srcObj, destObj);
    CompareSSample4Part2(srcObj, destObj);
    CompareSSample4Part3(srcObj, destObj);
}

HWTEST_F(CppStructTest, CppStructTest_005, TestSize.Level1)
{
    SSample4 srcObj = {
        {true, false},
        {65, 66},
        {3, 4},
        {5, 6},
        {7, 8},
        {97, 98},
        {30, 40},
        {50, 60},
        {70, 80},
        {10.5, 20.5},
        {30.125, 30.125},
        {"hello", "world"},
        {ESample::MEM_ONE, ESample::MEM_THREE},
        {{true, 1, 1000.125, "hello"}, {false, 1, 1000.125, "world"}},
        {sptr<SequenceData>(new SequenceData(1, 1.2, "hello")), sptr<SequenceData>(new SequenceData(2, 2.2, "world"))},
    };

    SSample4 destObj;

    int32_t ec = g_testClient->SSample4Test(srcObj, destObj);
    ASSERT_EQ(ec, HDF_SUCCESS);

    CompareSSample4(srcObj, destObj);
}

static void CompareSSample5Part1(const SSample5& srcObj, const SSample5& destObj) const
{
    for (size_t i = 0; i < srcObj.m1.size(); i++) {
        EXPECT_EQ((srcObj.m1[i] ? 1 : 0), (destObj.m1[i] ? 1 : 0));
    }

    for (size_t i = 0; i < srcObj.m2.size(); i++) {
        EXPECT_EQ(srcObj.m2[i], destObj.m2[i]);
    }

    for (size_t i = 0; i < srcObj.m3.size(); i++) {
        EXPECT_EQ(srcObj.m3[i], destObj.m3[i]);
    }

    for (size_t i = 0; i < srcObj.m4.size(); i++) {
        EXPECT_EQ(srcObj.m4[i], destObj.m4[i]);
    }

    for (size_t i = 0; i < srcObj.m5.size(); i++) {
        EXPECT_EQ(srcObj.m5[i], destObj.m5[i]);
    }
}

static void CompareSSample5Part2(const SSample5& srcObj, const SSample5& destObj) const
{
    for (size_t i = 0; i < srcObj.m6.size(); i++) {
        EXPECT_EQ(srcObj.m6[i], destObj.m6[i]);
    }

    for (size_t i = 0; i < srcObj.m7.size(); i++) {
        EXPECT_EQ(srcObj.m7[i], destObj.m7[i]);
    }

    for (size_t i = 0; i < srcObj.m8.size(); i++) {
        EXPECT_EQ(srcObj.m8[i], destObj.m8[i]);
    }

    for (size_t i = 0; i < srcObj.m9.size(); i++) {
        EXPECT_EQ(srcObj.m9[i], destObj.m9[i]);
    }

    for (size_t i = 0; i < srcObj.m10.size(); i++) {
        EXPECT_FLOAT_EQ(srcObj.m10[i], destObj.m10[i]);
    }

    for (size_t i = 0; i < srcObj.m11.size(); i++) {
        EXPECT_DOUBLE_EQ(srcObj.m11[i], destObj.m11[i]);
    }
}

static void CompareSSample5Part3(const SSample5& srcObj, const SSample5& destObj) const
{
    for (size_t i = 0; i < srcObj.m12.size(); i++) {
        EXPECT_EQ(srcObj.m12[i], destObj.m12[i]);
    }

    for (size_t i = 0; i < srcObj.m13.size(); i++) {
        EXPECT_EQ(srcObj.m13[i], destObj.m13[i]);
    }

    for (size_t i = 0; i < srcObj.m14.size(); i++) {
        EXPECT_EQ(((srcObj.m14[i]).m1 ? 1 : 0), ((destObj.m14[i]).m1 ? 1 : 0));
        EXPECT_EQ((srcObj.m14[i]).m2, (destObj.m14[i]).m2);
        EXPECT_DOUBLE_EQ((srcObj.m14[i]).m3, (destObj.m14[i]).m3);
        EXPECT_EQ((srcObj.m14[i]).m4, (destObj.m14[i]).m4);
    }

    for (size_t i = 0; i < srcObj.m15.size(); i++) {
        sptr<SequenceData> var1 = srcObj.m15[i];
        sptr<SequenceData> var2 = destObj.m15[i];

        if (var1 != nullptr && var2 != nullptr) {
            EXPECT_EQ(var1->m1_, var2->m1_);
            EXPECT_DOUBLE_EQ(var1->m2_, var2->m2_);
            EXPECT_EQ(var1->m3_, var2->m3_);
        } else {
            std::cout << "var1 or var2 is nullptr" << std::endl;
        }
    }
}

static void CompareSSample5(const SSample5& srcObj, const SSample5& destObj) const
{
    CompareSSample5Part1(srcObj, destObj);
    CompareSSample5Part2(srcObj, destObj);
    CompareSSample5Part3(srcObj, destObj);
}

HWTEST_F(CppStructTest, CppStructTest_006, TestSize.Level1)
{
    SSample5 srcObj = {
        {true, false},
        {65, 66},
        {3, 4},
        {5, 6},
        {7, 8},
        {97, 98},
        {30, 40},
        {50, 60},
        {70, 80},
        {10.5, 20.5},
        {30.125, 30.125},
        {"hello", "world"},
        {ESample::MEM_ONE, ESample::MEM_THREE},
        {{true, 1, 1000.125, "hello"}, {false, 1, 1000.125, "world"}},
        {sptr<SequenceData>(new SequenceData(1, 1.2, "hello")), sptr<SequenceData>(new SequenceData(2, 2.2, "world"))},
    };

    SSample5 destObj;

    int32_t ec = g_testClient->SSample5Test(srcObj, destObj);
    ASSERT_EQ(ec, HDF_SUCCESS);

    CompareSSample5(srcObj, destObj);
}