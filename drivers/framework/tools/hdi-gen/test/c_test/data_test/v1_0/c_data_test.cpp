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
#include "test/c_test/data_test/v1_0/client/idata_test.h"

using namespace OHOS;
using namespace testing::ext;

static struct IDataTest *g_testClient = nullptr;
static struct ICallback *g_callback = nullptr;

class CDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() {}
    void TearDown() {}
};

void CDataTest::SetUpTestCase()
{
    g_testClient = HdiDataTestGet();
    if (g_testClient == nullptr) {
        printf("CDataTest: get g_testClient failed.\n");
    }
}

void CDataTest::TearDownTestCase()
{
    if (g_testClient != nullptr) {
        HdiDataTestRelease(g_testClient);
        g_testClient = nullptr;
    }
}

HWTEST_F(CDataTest, CDataTest_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, g_testClient);
}

HWTEST_F(CDataTest, CDataTest_002, TestSize.Level1)
{
    bool inParam = true;
    bool outParam = false;

    int32_t ec = g_testClient->BoolTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_TRUE(inParam);
    EXPECT_TRUE(outParam);
}

HWTEST_F(CDataTest, CDataTest_003, TestSize.Level1)
{
    int8_t inParam = 10;
    int8_t outParam = 0;

    int32_t ec = g_testClient->ByteTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_004, TestSize.Level1)
{
    int16_t inParam = 10;
    int16_t outParam = 0;

    int32_t ec = g_testClient->ShortTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_005, TestSize.Level1)
{
    int32_t inParam = 10;
    int32_t outParam = 0;

    int32_t ec = g_testClient->IntTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_006, TestSize.Level1)
{
    int64_t inParam = 10;
    int64_t outParam = 0;

    int32_t ec = g_testClient->LongTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_007, TestSize.Level1)
{
    uint8_t inParam = 10;
    uint8_t outParam = 0;

    int32_t ec = g_testClient->UCharTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_008, TestSize.Level1)
{
    uint16_t inParam = 10;
    uint16_t outParam = 0;

    int32_t ec = g_testClient->UShortTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_009, TestSize.Level1)
{
    uint32_t inParam = 10;
    uint32_t outParam = 0;

    int32_t ec = g_testClient->UIntTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_010, TestSize.Level1)
{
    uint64_t inParam = 10;
    uint64_t outParam = 0;

    int32_t ec = g_testClient->ULongTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_011, TestSize.Level1)
{
    float inParam = 10.5;
    float outParam = 0;

    int32_t ec = g_testClient->FloatTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_FLOAT_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_012, TestSize.Level1)
{
    double inParam = 10.5;
    double outParam = 0;

    int32_t ec = g_testClient->DoubleTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_DOUBLE_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_013, TestSize.Level1)
{
    const char* inParam = "hello";
    char* outParam = nullptr;

    int32_t ec = g_testClient->StringTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_STREQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_014, TestSize.Level1)
{
    int fd = open("/c_data_test_014.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    printf("file open, fd = %d\n", fd);

    const char* oldMsg = "client:message\n";
    int size = 0;
    if ((size = write(fd, oldMsg, strlen(oldMsg))) > 0) {
        printf("write oldMsg success, size:%d\n", size);
    }

    lseek(fd, 0, SEEK_SET);

    int32_t ec = g_testClient->FdTest(g_testClient, fd);
    ASSERT_EQ(ec, HDF_SUCCESS);

    lseek(fd, 0, SEEK_SET);

    char recvMsg[1024] = {0};
    ssize_t readSize = read(fd, recvMsg, sizeof(recvMsg));
    if (readSize < 0) {
        printf("read failed\n");
        goto FINISHED;
    } else {
        printf("read size:%d\n", readSize);
    }
    printf("recvMsg:%s", recvMsg);

FINISHED:
    close(fd);
}

HWTEST_F(CDataTest, CDataTest_015, TestSize.Level1)
{
    enum ESample inParam = MEM_ONE;
    enum ESample outParam = MEM_TWO;

    int32_t ec = g_testClient->EnumTest(g_testClient, inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    EXPECT_EQ(inParam, outParam);
}

HWTEST_F(CDataTest, CDataTest_016, TestSize.Level1)
{
    struct SSample inParam;
    inParam.m1 = true;
    inParam.m2 = 1;
    inParam.m3 = 1.5;
    inParam.m4 = strdup("inParam string");
    ASSERT_NE(inParam.m4, nullptr);

    struct SSample *outParam = nullptr;

    int32_t ec = g_testClient->StructTest(g_testClient, &inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    EXPECT_EQ((inParam.m1 ? 1 : 0), (outParam->m1 ? 1 : 0));
    EXPECT_EQ(inParam.m2, outParam->m2);
    EXPECT_DOUBLE_EQ(inParam.m3, outParam->m3);
    ASSERT_NE(outParam->m4, nullptr);
    EXPECT_STREQ(inParam.m4, outParam->m4);


    SSampleFree(&inParam, false);


    if (outParam != nullptr) {
        SSampleFree(outParam, true);
    }
}

HWTEST_F(CDataTest, CDataTest_017, TestSize.Level1)
{
    union USample inParam;
    inParam.m2 = 10;

    union USample *outParam = nullptr;

    int32_t ec = g_testClient->UnionTest(g_testClient, &inParam, &outParam);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    EXPECT_EQ((inParam.m1 ? 1 : 0), (outParam->m1 ? 1 : 0));
    EXPECT_EQ(inParam.m2, outParam->m2);

    if (outParam != nullptr) {
        OsalMemFree(outParam);
    }
}