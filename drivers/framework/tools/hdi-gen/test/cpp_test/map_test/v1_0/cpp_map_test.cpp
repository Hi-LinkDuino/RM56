/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "cpp_test/map_test/v1_0/client/map_test_proxy.h"

using namespace OHOS;
using namespace testing::ext;
using namespace test::cpp_test::map_test::v1_0;
using namespace test::cpp_test::types::v1_0;

static sptr<IMapTest> g_testClient = nullptr;

class CppMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

void CppMapTest::SetUpTestCase()
{
    g_testClient = IMapTest::Get();
    if (g_testClient == nullptr) {
        std::cout << "CppMapTest: get g_testClient failed." << std::endl;
    }
}

HWTEST_F(CppMapTest, CppMapTest_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, g_testClient);
}

HWTEST_F(CppMapTest, CppMapTest_002, TestSize.Level1)
{
    std::map<int32_t, int8_t> inParam;
    inParam[1] = 'A';
    inParam[2] = 'B';

    std::map<int32_t, int8_t> outParam;

    int32_t ec = g_testClient->MapIntByteTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            ASSERT_EQ(inIter->second, outIter->second);
        } else {
            std::cout << "CppMapTest_002 TestCase is failed!" << std::endl;
        }
    }
}

HWTEST_F(CppMapTest, CppMapTest_003, TestSize.Level1)
{
    std::map<int32_t, int16_t> inParam;
    inParam[1] = 1;
    inParam[2] = 2;

    std::map<int32_t, int16_t> outParam;

    int32_t ec = g_testClient->MapIntShortTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            ASSERT_EQ(inIter->second, outIter->second);
        } else {
            std::cout << "CppMapTest_003 TestCase is failed!" << std::endl;
        }
    }
}

HWTEST_F(CppMapTest, CppMapTest_004, TestSize.Level1)
{
    std::map<int32_t, int32_t> inParam;
    inParam[1] = 1;
    inParam[2] = 2;

    std::map<int32_t, int32_t> outParam;

    int32_t ec = g_testClient->MapIntIntTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            ASSERT_EQ(inIter->second, outIter->second);
        } else {
            std::cout << "CppMapTest_004 TestCase is failed!" << std::endl;
        }
    }
}

HWTEST_F(CppMapTest, CppMapTest_005, TestSize.Level1)
{
    std::map<int32_t, int64_t> inParam;
    inParam[1] = 100;
    inParam[2] = 200;

    std::map<int32_t, int64_t> outParam;

    int32_t ec = g_testClient->MapIntLongTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            ASSERT_EQ(inIter->second, outIter->second);
        } else {
            std::cout << "CppMapTest_005 TestCase is failed!" << std::endl;
        }
    }
}

HWTEST_F(CppMapTest, CppMapTest_006, TestSize.Level1)
{
    std::map<int32_t, float> inParam;
    inParam[1] = 10.5;
    inParam[2] = 20.5;

    std::map<int32_t, float> outParam;

    int32_t ec = g_testClient->MapIntFloatTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            ASSERT_EQ(inIter->second, outIter->second);
        } else {
            std::cout << "CppMapTest_006 TestCase is failed!" << std::endl;
        }
    }
}

HWTEST_F(CppMapTest, CppMapTest_007, TestSize.Level1)
{
    std::map<int32_t, double> inParam;
    inParam[1] = 10.55;
    inParam[2] = 20.55;

    std::map<int32_t, double> outParam;

    int32_t ec = g_testClient->MapIntDoubleTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            ASSERT_DOUBLE_EQ(inIter->second, outIter->second);
        } else {
            std::cout << "CppMapTest_007 TestCase is failed!" << std::endl;
        }
    }
}

HWTEST_F(CppMapTest, CppMapTest_008, TestSize.Level1)
{
    std::map<int32_t, std::string> inParam;
    inParam[1] = "hello";
    inParam[2] = "world";

    std::map<int32_t, std::string> outParam;

    int32_t ec = g_testClient->MapIntStringTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            ASSERT_STREQ(inIter->second.c_str(), outIter->second.c_str());
        } else {
            std::cout << "CppMapTest_008 TestCase is failed!" << std::endl;
        }
    }
}

HWTEST_F(CppMapTest, CppMapTest_009, TestSize.Level1)
{
    std::map<int32_t, int> inParam;
    inParam[1] = open("/cpp_test_map_009.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    inParam[2] = open("/cpp_test_map_009.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);

    std::map<int32_t, int> outParam;

    int32_t ec = g_testClient->MapIntFdTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            std::cout << "inParam[" << inIter->first << "]:" << inIter->second << std::endl;
            std::cout << "outParam[" << outIter->first << "]:" << outIter->second << std::endl;
        } else {
            std::cout << "CppMapTest_009 TestCase is failed!" << std::endl;
        }
    }

    close(inParam[1]);
    close(outParam[0]);
}

HWTEST_F(CppMapTest, CppMapTest_010, TestSize.Level1)
{
    std::map<int32_t, sptr<SequenceData>> inParam = {
        {0, sptr<SequenceData>(new SequenceData(1, 1.2, "hello"))},
        {1, sptr<SequenceData>(new SequenceData(2, 2.2, "world"))}
    };

    std::map<int32_t, sptr<SequenceData>> outParam;

    int32_t ec = g_testClient->MapIntSeqTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            EXPECT_EQ(inIter->second->m1_, outIter->second->m1_);
            EXPECT_DOUBLE_EQ(inIter->second->m2_, outIter->second->m2_);
            EXPECT_EQ(inIter->second->m3_, outIter->second->m3_);
        } else {
            std::cout << "CppMapTest_010 TestCase is failed!" << std::endl;
        }
    }
}

HWTEST_F(CppMapTest, CppMapTest_011, TestSize.Level1)
{
    std::map<int32_t, ESample> inParam;
    inParam[1] = ESample::MEM_ONE;
    inParam[2] = ESample::MEM_TWO;

    std::map<int32_t, ESample> outParam;

    int32_t ec = g_testClient->MapIntEnumTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            ASSERT_EQ(inIter->second, outIter->second);
        } else {
            std::cout << "CppMapTest_011 TestCase is failed!" << std::endl;
        }
    }
}

HWTEST_F(CppMapTest, CppMapTest_012, TestSize.Level1)
{
    std::map<int32_t, SSample> inParam;
    for (int32_t i = 0; i < 2; i++) {
        SSample value;
        value.m1 = true;
        value.m2 = 10;
        value.m3 = 10.5;
        value.m4 = "inParam string";
        inParam[i] = value;
    }

    std::map<int32_t, SSample> outParam;

    int32_t ec = g_testClient->MapIntStructTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            ASSERT_EQ(((inIter->second).m1 ? 1 : 0), ((outIter->second).m1 ? 1 : 0));
            ASSERT_EQ((inIter->second).m2, (outIter->second).m2);
            ASSERT_DOUBLE_EQ((inIter->second).m3, (outIter->second).m3);
            ASSERT_EQ((inIter->second).m4, (outIter->second).m4);
        } else {
            std::cout << "CppMapTest_012 TestCase is failed!" << std::endl;
        }
    }
}

HWTEST_F(CppMapTest, CppMapTest_013, TestSize.Level1)
{
    std::map<int32_t, USample> inParam;
    for (int32_t i = 0; i < 2; i++) {
        USample value;
        value.m1 = true;
        value.m2 = 10;
        inParam[i] = value;
    }

    std::map<int32_t, USample> outParam;

    int32_t ec = g_testClient->MapIntUnionTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    for (auto inIter = inParam.begin(); inIter != inParam.end(); ++inIter) {
        auto outIter = outParam.find(inIter->first);
        if (outIter != outParam.end()) {
            ASSERT_EQ(((inIter->second).m1 ? 1 : 0), ((outIter->second).m1 ? 1 : 0));
            ASSERT_EQ((inIter->second).m2, (outIter->second).m2);
        } else {
            std::cout << "CppMapTest_013 TestCase is failed!" << std::endl;
        }
    }
}