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
#include "cpp_test/data_test/v1_0/client/data_test_proxy.h"

using namespace OHOS;
using namespace testing::ext;
using namespace test::cpp_test::data_test::v1_0;
using namespace test::cpp_test::types::v1_0;

static sptr<IDataTest> g_testClient = nullptr;

class CppDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

void CppDataTest::SetUpTestCase()
{
    g_testClient = IDataTest::Get();
    if (g_testClient == nullptr) {
        printf("CppDataTest: get g_testClient failed.\n");
    }
}

HWTEST_F(CppDataTest, CppDataTest_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, g_testClient);
}

HWTEST_F(CppDataTest, CppDataTest_002, TestSize.Level1)
{
    bool inParam = true;
    bool outParam = false;
    int32_t ec = g_testClient->BoolTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_TRUE(inParam);
    EXPECT_TRUE(outParam);
}

HWTEST_F(CppDataTest, CppDataTest_003, TestSize.Level1)
{
    int8_t inParam = 10;
    int8_t outParam = 0;

    int32_t ec = g_testClient->ByteTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_004, TestSize.Level1)
{
    int16_t inParam = 10;
    int16_t outParam = 0;

    int32_t ec = g_testClient->ShortTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_005, TestSize.Level1)
{
    int32_t inParam = 10;
    int32_t outParam = 0;

    int32_t ec = g_testClient->IntTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_006, TestSize.Level1)
{
    int64_t inParam = 10;
    int64_t outParam = 0;

    int32_t ec = g_testClient->LongTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_007, TestSize.Level1)
{
    uint8_t inParam = 10;
    uint8_t outParam = 0;

    int32_t ec = g_testClient->UCharTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_008, TestSize.Level1)
{
    uint16_t inParam = 10;
    uint16_t outParam = 0;

    int32_t ec = g_testClient->UShortTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_009, TestSize.Level1)
{
    uint32_t inParam = 10;
    uint32_t outParam = 0;

    int32_t ec = g_testClient->UIntTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_010, TestSize.Level1)
{
    uint64_t inParam = 10;
    uint64_t outParam = 0;

    int32_t ec = g_testClient->ULongTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_011, TestSize.Level1)
{
    float inParam = 10.5;
    float outParam = 0;

    int32_t ec = g_testClient->FloatTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_FLOAT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_012, TestSize.Level1)
{
    double inParam = 10.5;
    double outParam = 0;

    int32_t ec = g_testClient->DoubleTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_DOUBLE_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_013, TestSize.Level1)
{
    std::string inParam("hello");
    std::string outParam;

    int32_t ec = g_testClient->StringTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_014, TestSize.Level1)
{
    int fd = open("/cpp_data_test_014.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    printf("file open, fd = %d\n", fd);

    const char* oldMsg = "client:message\n";
    int size = 0;
    if ((size = write(fd, oldMsg, strlen(oldMsg))) > 0) {
        printf("write oldMsg success, size:%d\n", size);
    }

    lseek(fd, 0, SEEK_SET);

    int32_t ec = g_testClient->FdTest(fd);
    ASSERT_EQ(ec, HDF_SUCCESS);

    lseek(fd, 0, SEEK_SET);

    char recvMsg[1024] = {0};
    ssize_t readSize = read(fd, recvMsg, sizeof(recvMsg));
    if (readSize < 0) {
        printf("read failed\n");
        goto finished;
    } else {
        printf("read size:%d\n", readSize);
    }
    printf("recvMsg:%s", recvMsg);

finished:
    close(fd);
}

HWTEST_F(CppDataTest, CppDataTest_015, TestSize.Level1)
{
    ESample inParam = ESample::MEM_ONE;
    ESample outParam = ESample::MEM_TWO;

    int32_t ec = g_testClient->EnumTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CppDataTest, CppDataTest_016, TestSize.Level1)
{
    SSample inParam;
    inParam.m1 = true;
    inParam.m2 = 1;
    inParam.m3 = 10.5;
    inParam.m4 = "inParam string";

    SSample outParam;

    int32_t ec = g_testClient->StructTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ((inParam.m1 ? 1 : 0), (outParam.m1 ? 1 : 0));
    EXPECT_EQ(inParam.m2, outParam.m2);
    EXPECT_DOUBLE_EQ(inParam.m3, outParam.m3);
    EXPECT_EQ(inParam.m4, outParam.m4);
}

HWTEST_F(CppDataTest, CppDataTest_017, TestSize.Level1)
{
    USample inParam;
    inParam.m2 = 10;

    USample outParam;

    int32_t ec = g_testClient->UnionTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ((inParam.m1 ? 1 : 0), (outParam.m1 ? 1 : 0));
    EXPECT_DOUBLE_EQ(inParam.m2, outParam.m2);
}

HWTEST_F(CppDataTest, CppDataTest_018, TestSize.Level1)
{
    sptr<SequenceData> inParam(new SequenceData(1, 1.2, "hello"));

    sptr<SequenceData> outParam = nullptr;

    int32_t ec = g_testClient->SequenceDataTest(inParam, outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam->m1_, outParam->m1_);
    EXPECT_DOUBLE_EQ(inParam->m2_, outParam->m2_);
    EXPECT_EQ(inParam->m3_, outParam->m3_);
}