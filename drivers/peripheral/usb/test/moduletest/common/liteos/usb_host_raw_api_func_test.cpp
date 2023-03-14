/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <gtest/gtest.h>
#include "securec.h"
#include "usbhost_ddk_test.h"

using namespace std;
using namespace testing::ext;

namespace {
const int32_t TEST_COUNT = 3;
class UsbHostRawApiFuncTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void UsbHostRawApiFuncTest::SetUpTestCase()
{
    printf("------start UsbHostRawApiFuncTest------\n");
    const char *apiType = "-RAW";
    UsbHostDdkTestInit(const_cast<char*>(apiType));
}

void UsbHostRawApiFuncTest::TearDownTestCase()
{
    char writeBuf[] = "q";
    UsbHostDdkTestOpen(HOST_ACM_ASYNC_WRITE);
    UsbHostDdkTestAsyncWrite(writeBuf);
    UsbHostDdkTestClose(HOST_ACM_ASYNC_WRITE);
    TestExit();
    printf("------end UsbHostRawApiFuncTest------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_001，H_Lx_H_Sub_usb_IOread_write_003
 * @tc.name      : USB串口同步数据读写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostRawApiFuncTest, UsbSerialReadSync_001, TestSize.Level1)
{
    printf("------start UsbSerialReadSync_001------\n");
    char writeBuf[512] = "abc";
    char readBuf[512] = {0};
    UsbHostDdkTestOpen(HOST_ACM_SYNC_WRITE);
    UsbHostDdkTestSyncWrite(writeBuf);
    UsbHostDdkTestClose(HOST_ACM_SYNC_WRITE);
    UsbHostDdkTestOpen(HOST_ACM_SYNC_READ);
    UsbHostDdkTestSyncRead(readBuf);
    UsbHostDdkTestClose(HOST_ACM_SYNC_READ);
    EXPECT_EQ(strcmp(writeBuf, readBuf), 0);
    printf("------end UsbSerialReadSync_001------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_001，H_Lx_H_Sub_usb_IOread_write_006
 * @tc.name      : USB串口同步读+异步写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostRawApiFuncTest, UsbSerialReadSync_002, TestSize.Level1)
{
    printf("------start UsbSerialReadSync_002------\n");
    char writeBuf[512] = "abc";
    char readBuf[512] = {0};
    UsbHostDdkTestOpen(HOST_ACM_ASYNC_WRITE);
    UsbHostDdkTestAsyncWrite(writeBuf);
    UsbHostDdkTestClose(HOST_ACM_ASYNC_WRITE);
    UsbHostDdkTestOpen(HOST_ACM_SYNC_READ);
    UsbHostDdkTestSyncRead(readBuf);
    UsbHostDdkTestClose(HOST_ACM_SYNC_READ);
    EXPECT_EQ(strcmp(writeBuf, readBuf), 0);
    printf("------end UsbSerialReadSync_002------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_001，H_Lx_H_Sub_usb_IOread_write_003
 * @tc.name      : USB串口同步数据读写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostRawApiFuncTest, UsbSerialReadSync_003, TestSize.Level1)
{
    printf("------start UsbSerialReadSync_003------\n");
    const char *data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    char readBuf[512] = {0};
    for (int32_t i = 0; strlen(data[i]) > 0; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        UsbHostDdkTestOpen(HOST_ACM_SYNC_WRITE);
        UsbHostDdkTestSyncWrite(const_cast<char *>(data[i]));
        UsbHostDdkTestClose(HOST_ACM_SYNC_WRITE);
        UsbHostDdkTestOpen(HOST_ACM_SYNC_READ);
        UsbHostDdkTestSyncRead(readBuf);
        UsbHostDdkTestClose(HOST_ACM_SYNC_READ);
        EXPECT_EQ(strcmp(const_cast<char *>(data[i]), readBuf), 0);
    }
    printf("------end UsbSerialReadSync_003------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_001，H_Lx_H_Sub_usb_IOread_write_006
 * @tc.name      : USB串口同步读+异步写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostRawApiFuncTest, UsbSerialReadSync_004, TestSize.Level1)
{
    printf("------start UsbSerialReadSync_004------\n");
    const char *data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    char readBuf[512] = {0};
    for (int32_t i = 0; strlen(data[i]) > 0; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        UsbHostDdkTestOpen(HOST_ACM_ASYNC_WRITE);
        UsbHostDdkTestAsyncWrite(const_cast<char *>(data[i]));
        UsbHostDdkTestClose(HOST_ACM_ASYNC_WRITE);
        UsbHostDdkTestOpen(HOST_ACM_SYNC_READ);
        UsbHostDdkTestSyncRead(readBuf);
        UsbHostDdkTestClose(HOST_ACM_SYNC_READ);
        EXPECT_EQ(strcmp(const_cast<char *>(data[i]), readBuf), 0);
    }
    printf("------end UsbSerialReadSync_004------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_001，H_Lx_H_Sub_usb_IOread_write_003
 * @tc.name      : USB串口同步读写1KB数据
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostRawApiFuncTest, UsbSerialReadSync_005, TestSize.Level2)
{
    printf("------start UsbSerialReadSync_005------\n");
    const string s = "0123456789abcdef";
    string data;
    unsigned int totalSize = 1024;
    unsigned int writeCnt = 8;
    unsigned int n = 0;
    while (n < totalSize / writeCnt / s.size()) {
        data += s;
        n++;
    }
    char readBuf[512] = {0};
    char writeBuf[512] = {0};
    strcpy_s(writeBuf, sizeof(writeBuf), data.c_str());
    for (unsigned int i = 0; i < writeCnt; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        UsbHostDdkTestOpen(HOST_ACM_SYNC_WRITE);
        UsbHostDdkTestSyncWrite(writeBuf);
        UsbHostDdkTestClose(HOST_ACM_SYNC_WRITE);
        UsbHostDdkTestOpen(HOST_ACM_SYNC_READ);
        UsbHostDdkTestSyncRead(readBuf);
        UsbHostDdkTestClose(HOST_ACM_SYNC_READ);
        EXPECT_EQ(strcmp(writeBuf, readBuf), 0);
    }
    printf("------end UsbSerialReadSync_005------\n");
}

static void TestAsyncRead(char *readBuf, int32_t timeout)
{
    printf("------TestAsyncRead start-----");
    if (strlen(readBuf) > 0) {
        memset_s(readBuf, strlen(readBuf), 0, strlen(readBuf));
    }
    timeout = timeout * 1000;
    UsbHostDdkTestOpen(HOST_ACM_ASYNC_READ);
    while (timeout-- > 0) {
        UsbHostDdkTestAsyncRead(readBuf);
        if (strlen(readBuf) > 0) {
            break;
        }
        usleep(1000);
    }
    printf("------TestAsyncRead end-----");
}


/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_005， H_Lx_H_Sub_usb_IOread_write_006
 * @tc.name      : USB串口异步数据读写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostRawApiFuncTest, UsbSerialReadAsync_001, TestSize.Level1)
{
    printf("------start UsbSerialReadAsync_001------\n");
    char writeBuf[512] = "abc";
    char readBuf[512] = {0};
    UsbHostDdkTestOpen(HOST_ACM_ASYNC_WRITE);
    UsbHostDdkTestAsyncWrite(writeBuf);
    UsbHostDdkTestClose(HOST_ACM_ASYNC_WRITE);
    TestAsyncRead(readBuf, 5);
    EXPECT_EQ(strcmp(writeBuf, readBuf), 0);
    printf("------end UsbSerialReadAsync_001------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_005， H_Lx_H_Sub_usb_IOread_write_003
 * @tc.name      : USB串口异步读+同步写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostRawApiFuncTest, UsbSerialReadAsync_002, TestSize.Level1)
{
    printf("------start UsbSerialReadAsync_002------\n");
    char writeBuf[512] = "abc";
    char readBuf[512] = {0};
    UsbHostDdkTestOpen(HOST_ACM_SYNC_WRITE);
    UsbHostDdkTestSyncWrite(writeBuf);
    UsbHostDdkTestClose(HOST_ACM_SYNC_WRITE);
    TestAsyncRead(readBuf, 5);
    EXPECT_EQ(strcmp(writeBuf, readBuf), 0);
    printf("------end UsbSerialReadAsync_002------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_005， H_Lx_H_Sub_usb_IOread_write_006
 * @tc.name      : USB串口异步数据读写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostRawApiFuncTest, UsbSerialReadAsync_003, TestSize.Level1)
{
    printf("------start UsbSerialReadAsync_003------\n");
    const char *data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    char readBuf[512] = {0};
    for (int32_t i = 0; strlen(data[i]) > 0; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        UsbHostDdkTestOpen(HOST_ACM_ASYNC_WRITE);
        UsbHostDdkTestAsyncWrite(const_cast<char *>(data[i]));
        UsbHostDdkTestClose(HOST_ACM_ASYNC_WRITE);
        TestAsyncRead(readBuf, 5);
        EXPECT_EQ(strcmp(const_cast<char *>(data[i]), readBuf), 0);
    }
    printf("------end UsbSerialReadAsync_003------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_005， H_Lx_H_Sub_usb_IOread_write_003
 * @tc.name      : USB串口异步读+同步写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostRawApiFuncTest, UsbSerialReadAsync_004, TestSize.Level1)
{
    printf("------start UsbSerialReadAsync_004------\n");
    const char *data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    char readBuf[512] = {0};
    for (int32_t i = 0; strlen(data[i]) > 0; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        UsbHostDdkTestOpen(HOST_ACM_SYNC_WRITE);
        UsbHostDdkTestSyncWrite(const_cast<char *>(data[i]));
        UsbHostDdkTestClose(HOST_ACM_SYNC_WRITE);
        TestAsyncRead(readBuf, 5);
        EXPECT_EQ(strcmp(const_cast<char *>(data[i]), readBuf), 0);
    }
    printf("------end UsbSerialReadAsync_004------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_005， H_Lx_H_Sub_usb_IOread_write_006
 * @tc.name      : USB串口异步读写1KB数据
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostRawApiFuncTest, UsbSerialReadAsync_005, TestSize.Level2)
{
    printf("------start UsbSerialReadAsync_005------\n");
    const string s = "0123456789abcdef";
    string data;
    int32_t totalSize = 1024;
    int32_t writeCnt = 8;
    unsigned int n = 0;
    while (n < totalSize / writeCnt / s.size()) {
        data += s;
        n++;
    }
    char readBuf[512] = {0};
    char writeBuf[512] = {0};
    strcpy_s(writeBuf, sizeof(writeBuf), data.c_str());
    for (int32_t i = 0; i < writeCnt; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        UsbHostDdkTestOpen(HOST_ACM_ASYNC_WRITE);
        UsbHostDdkTestAsyncWrite(writeBuf);
        UsbHostDdkTestClose(HOST_ACM_ASYNC_WRITE);
        TestAsyncRead(readBuf, 5);
        EXPECT_EQ(strcmp(writeBuf, readBuf), 0);
    }
    printf("------end UsbSerialReadAsync_005------\n");
}
}
