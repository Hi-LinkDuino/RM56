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
#include "test/cpp_test/list_test/v1_0/client/list_test_proxy.h"

using namespace OHOS;
using namespace testing::ext;
using namespace test::cpp_test::list_test::v1_0;
using namespace test::cpp_test::types::v1_0;

static sptr<IListTest> g_testClient = nullptr;

class CppListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

void CppListTest::SetUpTestCase()
{
    g_testClient = IListTest::Get();
    if (g_testClient == nullptr) {
        printf("CppListTest: get g_testClient failed.\n");
    }
}

HWTEST_F(CppListTest, CppListTest_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, g_testClient);
}

HWTEST_F(CppListTest, CppListTest_002, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<bool> inParam;

    for (uint32_t i = 0; i < len; i++) {
        if ((i+1)/2 == 0) {
            inParam.push_back(false);
        } else {
            inParam.push_back(true);
        }
    }

    std::vector<bool> outParam;

    int32_t ec = g_testClient->BoolListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ((inParam[i] ? 1 : 0), (outParam[i] ? 1 : 0));
    }
}

HWTEST_F(CppListTest, CppListTest_003, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<int8_t> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(4);
    }

    std::vector<int8_t> outParam;

    int32_t ec = g_testClient->ByteListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_004, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<int16_t> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(4);
    }

    std::vector<int16_t> outParam;

    int32_t ec = g_testClient->ShortListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_005, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<int32_t> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(4);
    }

    std::vector<int32_t> outParam;

    int32_t ec = g_testClient->IntListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_006, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<int64_t> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(4);
    }

    std::vector<int64_t> outParam;

    int32_t ec = g_testClient->LongListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_007, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<uint8_t> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(4);
    }

    std::vector<uint8_t> outParam;

    int32_t ec = g_testClient->UCharListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_008, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<uint16_t> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(4);
    }

    std::vector<uint16_t> outParam;

    int32_t ec = g_testClient->UShortListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_009, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<uint32_t> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(4);
    }

    std::vector<uint32_t> outParam;

    int32_t ec = g_testClient->UIntListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_010, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<uint64_t> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(4);
    }

    std::vector<uint64_t> outParam;

    int32_t ec = g_testClient->ULongListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_011, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<float> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(4);
    }

    std::vector<float> outParam;

    int32_t ec = g_testClient->FloatListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_FLOAT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_012, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<double> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(4);
    }

    std::vector<double> outParam;

    int32_t ec = g_testClient->DoubleListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_DOUBLE_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_013, TestSize.Level1)
{
    uint32_t len = 2;
    std::vector<std::string> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back("inParam string");
    }

    std::vector<std::string> outParam;

    int32_t ec = g_testClient->StringListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_014, TestSize.Level1)
{
    int fd1 = open("/CppListTest_014_0.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    int fd2 = open("/CppListTest_014_1.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    ASSERT_NE(fd1, -1);
    ASSERT_NE(fd2, -1);

    std::vector<int> inParam;
    inParam.push_back(fd1);
    inParam.push_back(fd2);

    std::vector<int> outParam;

    int32_t ec = g_testClient->FdListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());

    std::cout << "inParam[0]:"<< inParam[0] << ", inParam[1]" << inParam[1] << std::endl;
    std::cout << "outParam[0]:"<< outParam[0] << ", outParam[1]" << outParam[1] << std::endl;
    close(fd1);
    close(fd2);
}

HWTEST_F(CppListTest, CppListTest_015, TestSize.Level1)
{
    uint32_t len = 2;
    std::vector<ESample> inParam;

    for (uint32_t i = 0; i < len; i++) {
        inParam.push_back(ESample::MEM_ONE);
    }

    std::vector<ESample> outParam;

    int32_t ec = g_testClient->EnumListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }
}

HWTEST_F(CppListTest, CppListTest_016, TestSize.Level1)
{
    uint32_t len = 4;
    std::vector<SSample> inParam;

    for (uint32_t i = 0; i < len; i++) {
        SSample element;
        element.m1 = true;
        element.m2 = 1;
        element.m3 = 10.5;
        element.m4 = "hello";
        inParam.push_back(element);
    }

    std::vector<SSample> outParam;

    int32_t ec = g_testClient->StructListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ((inParam[i].m1 ? 1 : 0), (outParam[i].m1 ? 1 : 0));
        ASSERT_EQ(inParam[i].m2, outParam[i].m2);
        ASSERT_DOUBLE_EQ(inParam[i].m3, outParam[i].m3);
        ASSERT_EQ(inParam[i].m4, outParam[i].m4);
    }
}

HWTEST_F(CppListTest, CppListTest_017, TestSize.Level1)
{
    uint32_t len = 2;
    std::vector<USample> inParam;

    for (uint32_t i = 0; i < len; i++) {
        USample element;
        element.m2 = 10;
        inParam.push_back(element);
    }

    std::vector<USample> outParam;

    int32_t ec = g_testClient->UnionListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_EQ((inParam[i].m1 ? 1 : 0), (outParam[i].m1 ? 1 : 0));
        ASSERT_EQ(inParam[i].m2, outParam[i].m2);
    }
}

HWTEST_F(CppListTest, CppListTest_018, TestSize.Level1)
{
    std::vector<sptr<SequenceData>> inParam = {
        sptr<SequenceData>(new SequenceData(1, 1.2, "hello")),
        sptr<SequenceData>(new SequenceData(2, 2.2, "world")),
    };

    std::vector<sptr<SequenceData>> outParam;

    int32_t ec = g_testClient->SeqDataListTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_EQ(inParam.size(), outParam.size());
    for (uint32_t i = 0; i < inParam.size(); i++) {
        ASSERT_NE(outParam[i], nullptr);
        EXPECT_EQ(inParam[i]->m1_, outParam[i]->m1_);
        EXPECT_DOUBLE_EQ(inParam[i]->m2_, outParam[i]->m2_);
        EXPECT_EQ(inParam[i]->m3_, outParam[i]->m3_);
    }
}