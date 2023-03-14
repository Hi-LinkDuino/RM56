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
#include "c_test/list_test/v1_0/client/ilist_test.h"

using namespace OHOS;
using namespace testing::ext;

static struct IListTest *g_testClient = nullptr;

class CListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() {}
    void TearDown() {}
};

void CListTest::SetUpTestCase()
{
    g_testClient = HdiListTestGet();
    if (g_testClient == nullptr) {
        printf("CListTest: get g_testClient failed.\n");
    }
}

void CListTest::TearDownTestCase()
{
    if (g_testClient != nullptr) {
        HdiListTestRelease(g_testClient);
        g_testClient = nullptr;
    }
}

HWTEST_F(CListTest, CListTest_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, g_testClient);
}

HWTEST_F(CListTest, CListTest_002, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    bool *inParam = (bool*)OsalMemAlloc(sizeof(bool) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = false;
        } else {
            inParam[i] = true;
        }
    }

    bool *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->BoolListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ((inParam[i] ? 1 : 0), (outParam[i] ? 1 : 0));
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_003, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    int8_t *inParam = (int8_t*)OsalMemAlloc(sizeof(int8_t) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = 0;
        } else {
            inParam[i] = 1;
        }
    }

    int8_t *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->ByteListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }


    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_004, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    int16_t *inParam = (int16_t*)OsalMemAlloc(sizeof(int16_t) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = 0;
        } else {
            inParam[i] = 1;
        }
    }

    int16_t *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->ShortListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_005, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    int32_t *inParam = (int32_t*)OsalMemAlloc(sizeof(int32_t) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = 0;
        } else {
            inParam[i] = 1;
        }
    }

    int32_t *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->IntListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_006, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    int64_t *inParam = (int64_t*)OsalMemAlloc(sizeof(int64_t) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = 0;
        } else {
            inParam[i] = 1;
        }
    }

    int64_t *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->LongListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_007, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    uint8_t *inParam = (uint8_t*)OsalMemAlloc(sizeof(uint8_t) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = 0;
        } else {
            inParam[i] = 1;
        }
    }

    uint8_t *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->UCharListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_008, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    uint16_t *inParam = (uint16_t*)OsalMemAlloc(sizeof(uint16_t) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = 0;
        } else {
            inParam[i] = 1;
        }
    }

    uint16_t *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->UShortListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_009, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    uint32_t *inParam = (uint32_t*)OsalMemAlloc(sizeof(uint32_t) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = 0;
        } else {
            inParam[i] = 1;
        }
    }

    uint32_t *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->UIntListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_010, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    uint64_t *inParam = (uint64_t*)OsalMemAlloc(sizeof(uint64_t) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = 0;
        } else {
            inParam[i] = 1;
        }
    }

    uint64_t *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->ULongListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_011, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    float *inParam = (float*)OsalMemAlloc(sizeof(float) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = 0.5;
        } else {
            inParam[i] = 1.5;
        }
    }

    float *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->FloatListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);


    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_012, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    double *inParam = (double*)OsalMemAlloc(sizeof(double) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = 0.5;
        } else {
            inParam[i] = 1.5;
        }
    }

    double *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->DoubleListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);

    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_013, TestSize.Level1)
{
    uint32_t inParamLen = 2;
    char **inParam = (char**)OsalMemAlloc(sizeof(char*) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        inParam[i] = strdup("inParam string array");
    }

    char **outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->StringListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_STREQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        for (uint32_t i = 0; i < inParamLen; i++) {
            if (inParam[i] != nullptr) {
                OsalMemFree(inParam[i]);
            }
        }
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        for (uint32_t i = 0; i < outParamLen; i++) {
            if (outParam[i] != nullptr) {
                OsalMemFree(outParam[i]);
            }
        }
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_014, TestSize.Level1)
{
    uint32_t inParamLen = 2;

    int fd1 = open("/ListTest_014_0.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    int fd2 = open("/ListTest_014_1.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    ASSERT_NE(fd1, -1);
    ASSERT_NE(fd2, -1);

    int *inParam = (int*)OsalMemAlloc(sizeof(int) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    inParam[0] = fd1;
    inParam[1] = fd2;

    int *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->FdListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);

    std::cout << "inParam[0]:"<< inParam[0] << ", inParam[1]" << inParam[1] << std::endl;
    std::cout << "outParam[0]:"<< outParam[0] << ", outParam[1]" << outParam[1] << std::endl;
    close(fd1);
    close(fd2);

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && inParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_015, TestSize.Level1)
{
    uint32_t inParamLen = 4;
    enum ESample *inParam = (enum ESample*)OsalMemAlloc(sizeof(enum ESample) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        if ((i+1)/2 == 0) {
            inParam[i] = MEM_ONE;
        } else {
            inParam[i] = MEM_TWO;
        }
    }

    enum ESample *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->EnumListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        ASSERT_EQ(inParam[i], outParam[i]);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && inParamLen > 0) {
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_016, TestSize.Level1)
{
    uint32_t inParamLen = 2;
    struct SSample *inParam = (struct SSample *)OsalMemAlloc(sizeof(struct SSample) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        inParam[i].m1 = true;
        inParam[i].m2 = 1;
        inParam[i].m3 = 1.5;
        inParam[i].m4 = strdup("inParam.m4");
        ASSERT_NE(inParam[i].m4, nullptr);
    }

    struct SSample *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->StructListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    if (outParamLen > 0) {
        ASSERT_NE(outParam, nullptr);
        ASSERT_EQ(inParamLen, outParamLen);
        for (uint32_t i = 0; i < inParamLen; i++) {
            EXPECT_EQ(inParam[i].m1, outParam[i].m1);
            EXPECT_EQ(inParam[i].m2, outParam[i].m2);
            EXPECT_DOUBLE_EQ(inParam[i].m3, outParam[i].m3);
            EXPECT_STREQ(inParam[i].m4, outParam[i].m4);
        }
    }

    if (inParam != nullptr && inParamLen > 0) {
        for (uint32_t i = 0; i < inParamLen; i++) {
            if (inParam[i].m4 != nullptr) {
                SSampleFree(&inParam[i], false);
            }
        }
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && outParamLen > 0) {
        for (uint32_t i = 0; i < outParamLen; i++) {
            SSampleFree(&outParam[i], false);
        }
        OsalMemFree(outParam);
    }
}

HWTEST_F(CListTest, CListTest_017, TestSize.Level1)
{
    uint32_t inParamLen = 2;
    union USample *inParam = (union USample *)OsalMemAlloc(sizeof(union USample) * inParamLen);
    ASSERT_NE(inParam, nullptr);

    for (uint32_t i = 0; i < inParamLen; i++) {
        inParam[i].m1 = true;
        inParam[i].m2 = 1;
    }

    union USample *outParam = nullptr;
    uint32_t outParamLen = 0;

    int32_t ec = g_testClient->UnionListTest(g_testClient,
        inParam, inParamLen,
        &outParam, &outParamLen);
    ASSERT_EQ(ec, HDF_SUCCESS);

    ASSERT_NE(outParam, nullptr);
    ASSERT_EQ(inParamLen, outParamLen);
    for (uint32_t i = 0; i < inParamLen; i++) {
        EXPECT_EQ(inParam[i].m1, outParam[i].m1);
        EXPECT_EQ(inParam[i].m2, outParam[i].m2);
    }

    if (inParam != nullptr && inParamLen > 0) {
        OsalMemFree(inParam);
    }

    if (outParam != nullptr && inParamLen > 0) {
        OsalMemFree(outParam);
    }
}