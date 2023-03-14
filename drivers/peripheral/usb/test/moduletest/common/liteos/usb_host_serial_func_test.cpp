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

#include "usb_host_serial_func_test.h"
#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include <unistd.h>
#include "securec.h"
#include "usbhost_ddk_test.h"

using namespace std;
using namespace testing::ext;

namespace {

class UsbHostSerialFuncTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void UsbHostSerialFuncTest::SetUpTestCase()
{
    printf("------start UsbHostSerialFuncTest------\n");
    const char *apiType = "-SDK";
    UsbHostDdkTestInit(const_cast<char*>(apiType));
}

void UsbHostSerialFuncTest::TearDownTestCase()
{
    const char *apiType = "-SDK";
    UsbHostDdkTestInit(const_cast<char*>(apiType));
    char writeBuf[] = "q";
    UsbHostDdkTestOpen(HOST_ACM_ASYNC_WRITE);
    UsbHostDdkTestAsyncWrite(writeBuf);
    UsbHostDdkTestClose(HOST_ACM_ASYNC_WRITE);
    TestExit();
    printf("------end UsbHostSerialFuncTest------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_001，H_Lx_H_Sub_usb_IOread_write_003
 * @tc.name      : USB串口同步数据读写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialReadSync_001, TestSize.Level1)
{
    printf("------start UsbSerialReadSync_001------\n");
    char writeBuf[DATA_MAX_LEN] = "abc";
    char readBuf[DATA_MAX_LEN] = {0};
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
HWTEST_F(UsbHostSerialFuncTest, UsbSerialReadSync_002, TestSize.Level1)
{
    printf("------start UsbSerialReadSync_002------\n");
    char writeBuf[DATA_MAX_LEN] = "abc";
    char readBuf[DATA_MAX_LEN] = {0};
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
HWTEST_F(UsbHostSerialFuncTest, UsbSerialReadSync_003, TestSize.Level1)
{
    printf("------start UsbSerialReadSync_003------\n");
    const char *data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    char readBuf[DATA_MAX_LEN] = {0};
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
HWTEST_F(UsbHostSerialFuncTest, UsbSerialReadSync_004, TestSize.Level1)
{
    printf("------start UsbSerialReadSync_004------\n");
    const char *data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    char readBuf[DATA_MAX_LEN] = {0};
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
HWTEST_F(UsbHostSerialFuncTest, UsbSerialReadSync_005, TestSize.Level2)
{
    printf("------start UsbSerialReadSync_005------\n");
    const string s = "0123456789abcdef";
    string data;
    int32_t totalSize = 1024;
    int32_t writeCnt = 8;
    unsigned int n = 0;
    while (n < totalSize / writeCnt / s.size()) {
        data += s;
        n++;
    }
    char readBuf[DATA_MAX_LEN] = {0};
    char writeBuf[DATA_MAX_LEN] = {0};
    strcpy_s(writeBuf, sizeof(writeBuf), data.c_str());
    for (int32_t i = 0; i < writeCnt; i++) {
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

extern "C" {
static void TestAsyncRead(char *readBuf, int32_t timeout)
{
    printf("------TestAsyncRead start-----\n");
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
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IOread_write_005， H_Lx_H_Sub_usb_IOread_write_006
 * @tc.name      : USB串口异步数据读写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialReadAsync_001, TestSize.Level1)
{
    printf("------start UsbSerialReadAsync_001------\n");
    char writeBuf[DATA_MAX_LEN] = "abc";
    char readBuf[DATA_MAX_LEN] = {0};
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
HWTEST_F(UsbHostSerialFuncTest, UsbSerialReadAsync_002, TestSize.Level1)
{
    printf("------start UsbSerialReadAsync_002------\n");
    char writeBuf[DATA_MAX_LEN] = "abc";
    char readBuf[DATA_MAX_LEN] = {0};
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
HWTEST_F(UsbHostSerialFuncTest, UsbSerialReadAsync_003, TestSize.Level1)
{
    printf("------start UsbSerialReadAsync_003------\n");
    const char *data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    char readBuf[DATA_MAX_LEN] = {0};
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
HWTEST_F(UsbHostSerialFuncTest, UsbSerialReadAsync_004, TestSize.Level1)
{
    printf("------start UsbSerialReadAsync_004------\n");
    const char *data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    char readBuf[DATA_MAX_LEN] = {0};
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
HWTEST_F(UsbHostSerialFuncTest, UsbSerialReadAsync_005, TestSize.Level2)
{
    printf("------start UsbSerialReadAsync_005------\n");
    const string s = "0123456789abcdef";
    string data;
    unsigned int totalSize = 1024;
    unsigned int writeCnt = 8;
    unsigned int n = 0;
    while (n < totalSize / writeCnt / s.size()) {
        data += s;
        n++;
    }
    char readBuf[DATA_MAX_LEN] = {0};
    char writeBuf[DATA_MAX_LEN] = {0};
    strcpy_s(writeBuf, sizeof(writeBuf), data.c_str());
    for (unsigned int i = 0; i < writeCnt; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        UsbHostDdkTestOpen(HOST_ACM_ASYNC_WRITE);
        UsbHostDdkTestAsyncWrite(writeBuf);
        UsbHostDdkTestClose(HOST_ACM_ASYNC_WRITE);
        TestAsyncRead(readBuf, 5);
        EXPECT_EQ(strcmp(writeBuf, readBuf), 0);
    }
    printf("------end UsbSerialReadAsync_005------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_Control_001
 * @tc.name      : USB串口标准控制命令的同步处理
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialStdCtrlCmdSync_001, TestSize.Level1)
{
    printf("------start UsbSerialStdCtrlCmdSync_001------\n");
    char readBuf[DATA_MAX_LEN] = {0};
    UsbHostDdkTestOpen(HOST_ACM_CTRL_SYNC_DESCRIPTOR);
    UsbHostDdkTestStdGetDes(readBuf);
    UsbHostDdkTestClose(HOST_ACM_CTRL_SYNC_DESCRIPTOR);
    EXPECT_NE(strlen(readBuf), 0);
    printf("------end UsbSerialStdCtrlCmdSync_001------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_Control_001
 * @tc.name      : USB串口标准控制命令的同步处理
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialStdCtrlCmdSync_002, TestSize.Level1)
{
    printf("------start UsbSerialStdCtrlCmdSync_002------\n");
    char readBuf[DATA_MAX_LEN] = {0};
    UsbHostDdkTestOpen(HOST_ACM_CTRL_GET_STATUS);
    UsbHostDdkTestStdGetStatus(readBuf);
    UsbHostDdkTestClose(HOST_ACM_CTRL_GET_STATUS);
    EXPECT_NE(strlen(readBuf), 0);
    printf("------end UsbSerialStdCtrlCmdSync_002------\n");
}


/**
 * @tc.number    : H_Lx_H_Sub_usb_Control_001
 * @tc.name      : USB串口标准控制命令的同步处理
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialStdCtrlCmdSync_003, TestSize.Level2)
{
    printf("------start UsbSerialStdCtrlCmdSync_003------\n");
    char readBuf[DATA_MAX_LEN] = {0};
    for (int32_t i=0; i < TEST_COUNT; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        UsbHostDdkTestOpen(HOST_ACM_CTRL_SYNC_DESCRIPTOR);
        UsbHostDdkTestStdGetDes(readBuf);
        UsbHostDdkTestClose(HOST_ACM_CTRL_SYNC_DESCRIPTOR);
        EXPECT_NE(strlen(readBuf), 0);
    }
    printf("------end UsbSerialStdCtrlCmdSync_003------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_Control_002
 * @tc.name      : USB串口类控制命令的同步处理
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialClsCtrlCmdSync_001, TestSize.Level1)
{
    printf("------start UsbSerialClsCtrlCmdSync_001------\n");
    char readBuf[DATA_MAX_LEN] = {0};
    UsbHostDdkTestOpen(HOST_ACM_CTRL_CLASS_SYNC);
    UsbHostDdkTestCtrlClass(readBuf);
    UsbHostDdkTestClose(HOST_ACM_CTRL_CLASS_SYNC);
    EXPECT_NE(strlen(readBuf), 0);
    printf("------end UsbSerialClsCtrlCmdSync_001------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_Control_002
 * @tc.name      : USB串口类控制命令的同步处理
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialClsCtrlCmdSync_002, TestSize.Level2)
{
    printf("------start UsbSerialClsCtrlCmdSync_002------\n");
    char readBuf[DATA_MAX_LEN] = {0};
    for (int32_t i = 0; i < TEST_COUNT; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        UsbHostDdkTestOpen(HOST_ACM_CTRL_CLASS_SYNC);
        UsbHostDdkTestCtrlClass(readBuf);
        UsbHostDdkTestClose(HOST_ACM_CTRL_CLASS_SYNC);
        EXPECT_NE(strlen(readBuf), 0);
    }
    printf("------end UsbSerialClsCtrlCmdSync_002------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_Control_004, H_Lx_H_Sub_usb_Control_005, H_Lx_H_Sub_usb_Control_006
 * @tc.name      : USB串口控制命令的异步处理
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialCtrlCmdAsync_001, TestSize.Level1)
{
    printf("------start UsbSerialCtrlCmdAsync_001------\n");
    char readBuf[DATA_MAX_LEN] = {0};
    UsbHostDdkTestOpen(HOST_ACM_CTRL_ASYNC_DESCRIPTOR);
    UsbHostDdkTestStdGetDesAsync(readBuf);
    UsbHostDdkTestClose(HOST_ACM_CTRL_ASYNC_DESCRIPTOR);
    printf("readBuf==>%s\n", readBuf);
    EXPECT_NE(strlen(readBuf), 0);
    printf("------end UsbSerialCtrlCmdAsync_001------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_Control_004, H_Lx_H_Sub_usb_Control_005, H_Lx_H_Sub_usb_Control_006
 * @tc.name      : USB串口控制命令的异步处理
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialCtrlCmdAsync_002, TestSize.Level2)
{
    printf("------start UsbSerialCtrlCmdAsync_002------\n");
    char readBuf[DATA_MAX_LEN] = {0};
    for (int32_t i = 0; i < TEST_COUNT; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        UsbHostDdkTestOpen(HOST_ACM_CTRL_ASYNC_DESCRIPTOR);
        UsbHostDdkTestStdGetDesAsync(readBuf);
        UsbHostDdkTestClose(HOST_ACM_CTRL_ASYNC_DESCRIPTOR);
        printf("readBuf==>%s\n", readBuf);
        EXPECT_NE(strlen(readBuf), 0);
    }
    printf("------end UsbSerialCtrlCmdAsync_002------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Instance_001, H_Lx_D_Sub_usb_Descriptor_002, H_Lx_D_Sub_usb_Descriptor_001
 * @tc.name      : 动态实例化USB串口设备, 支持通用属性可配置, 从HCS导入属性及默认值
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialGetDeviceInfo_001, TestSize.Level1)
{
    printf("------start UsbSerialGetDeviceInfo_001------\n");
    const char *idVendor = "0x12d1";
    const char *idProduct = "0x5000";
    const char *bDeviceClass = "0x00";
    const char *bDeviceSubClass = "0x00";
    const char *bDeviceProtocol = "0x00";
    const int32_t logMaxLen = 256;
    char targetLog[logMaxLen] = {0};
    const char *fmt = "device descriptor info:[%s %s %s %s %s]\n";
    if (snprintf_s(targetLog, logMaxLen, logMaxLen -1, fmt, \
        idVendor, idProduct, bDeviceClass, bDeviceSubClass, bDeviceProtocol) < 0) {
        printf("UsbSerialGetDeviceInfo_001 snprintf_s failed\n");
    }
    printf("targetLog==>%s-%d\n", targetLog, strlen(targetLog));
    char readBuf[DATA_MAX_LEN] = {0};
    UsbHostDdkTestOpen(HOST_ACM_CTRL_SYNC_DESCRIPTOR);
    UsbHostDdkTestStdGetDes(readBuf);
    UsbHostDdkTestClose(HOST_ACM_CTRL_SYNC_DESCRIPTOR);
    printf("readBuf==>%s-%d\n", readBuf, strlen(readBuf));
    EXPECT_EQ(strcmp(targetLog, readBuf), 0);
    printf("------end UsbSerialGetDeviceInfo_001------\n");
}

/**
 * @tc.number    : add/remove interface
 * @tc.name      : add/remove interface
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialAddOrRemoveInterface_001, TestSize.Level2)
{
    printf("------start UsbSerialAddOrRemoveInterface_001------\n");
    const char *ecmApiType = "-ECM";
    const char *acmApiType = "-SDK";
    TestExit();
    UsbHostDdkTestInit(const_cast<char*>(ecmApiType));
    int32_t acmInterface = 2;
    UsbHostDdkTestOpen(HOST_ACM_REMOVE_INTERFACE);
    UsbHostDdkTestRemoveInterface(acmInterface);
    TestExit();
    sleep(2);
    int32_t status = 0;
    status = UsbHostDdkTestInit(const_cast<char*>(acmApiType));
    printf("acm init status=>%d\n", status);
    EXPECT_NE(status, 0);
    printf("------end UsbSerialAddOrRemoveInterface_001------\n");
}

/**
 * @tc.number    : add/remove interface
 * @tc.name      : add/remove interface
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialAddOrRemoveInterface_002, TestSize.Level2)
{
    printf("------start UsbSerialAddOrRemoveInterface_002------\n");
    const char *ecmApiType = "-ECM";
    const char *acmApiType = "-SDK";
    int32_t acmInterface = 2;
    UsbHostDdkTestInit(const_cast<char*>(ecmApiType));
    UsbHostDdkTestOpen(HOST_ACM_ADD_INTERFACE);
    UsbHostDdkTestAddInterface(acmInterface);
    TestExit();
    sleep(2);
    int32_t status = 0;
    status = UsbHostDdkTestInit(const_cast<char*>(acmApiType));
    printf("acm init status=>%d\n", status);
    EXPECT_EQ(status, 0);
    TestExit();
    printf("------end UsbSerialAddOrRemoveInterface_002------\n");
}

/**
 * @tc.number    : add/remove interface
 * @tc.name      : add/remove interface
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialAddOrRemoveInterface_003, TestSize.Level2)
{
    printf("------start UsbSerialAddOrRemoveInterface_003------\n");
    const char *ecmApiType = "-ECM";
    const char *acmApiType = "-SDK";
    int32_t acmInterface = 3;
    UsbHostDdkTestInit(const_cast<char*>(ecmApiType));
    UsbHostDdkTestOpen(HOST_ACM_REMOVE_INTERFACE);
    UsbHostDdkTestRemoveInterface(acmInterface);
    TestExit();
    sleep(2);
    int32_t status = 0;
    status = UsbHostDdkTestInit(const_cast<char*>(acmApiType));
    printf("acm init status=>%d\n", status);
    EXPECT_NE(status, 0);
    printf("------end UsbSerialAddOrRemoveInterface_003------\n");
}

/**
 * @tc.number    : add/remove interface
 * @tc.name      : add/remove interface
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialAddOrRemoveInterface_004, TestSize.Level2)
{
    printf("------start UsbSerialAddOrRemoveInterface_004------\n");
    const char *ecmApiType = "-ECM";
    const char *acmApiType = "-SDK";
    int32_t acmInterface = 3;
    UsbHostDdkTestInit(const_cast<char*>(ecmApiType));
    UsbHostDdkTestOpen(HOST_ACM_ADD_INTERFACE);
    UsbHostDdkTestAddInterface(acmInterface);
    TestExit();
    sleep(2);
    int32_t status = 0;
    status = UsbHostDdkTestInit(const_cast<char*>(acmApiType));
    printf("acm init status=>%d\n", status);
    EXPECT_EQ(status, 0);
    TestExit();
    printf("------end UsbSerialAddOrRemoveInterface_004------\n");
}

/**
 * @tc.number    : add/remove interface
 * @tc.name      : add/remove interface
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialAddOrRemoveInterface_005, TestSize.Level2)
{
    printf("------start UsbSerialAddOrRemoveInterface_005------\n");
    const char *ecmApiType = "-ECM";
    const char *acmApiType = "-SDK";
    int32_t ecmInterface = 0;
    UsbHostDdkTestInit(const_cast<char*>(acmApiType));
    UsbHostDdkTestOpen(HOST_ACM_REMOVE_INTERFACE);
    UsbHostDdkTestRemoveInterface(ecmInterface);
    TestExit();
    sleep(2);
    int32_t status = 0;
    status = UsbHostDdkTestInit(const_cast<char*>(ecmApiType));
    printf("ecm init status=>%d\n", status);
    EXPECT_NE(status, 0);
    printf("------end UsbSerialAddOrRemoveInterface_005------\n");
}

/**
 * @tc.number    : add/remove interface
 * @tc.name      : add/remove interface
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialAddOrRemoveInterface_006, TestSize.Level2)
{
    printf("------start UsbSerialAddOrRemoveInterface_006------\n");
    const char *ecmApiType = "-ECM";
    const char *acmApiType = "-SDK";
    int32_t ecmInterface = 0;
    UsbHostDdkTestInit(const_cast<char*>(acmApiType));
    UsbHostDdkTestOpen(HOST_ACM_ADD_INTERFACE);
    UsbHostDdkTestAddInterface(ecmInterface);
    TestExit();
    sleep(2);
    int32_t status = 0;
    status = UsbHostDdkTestInit(const_cast<char*>(ecmApiType));
    printf("ecm init status=>%d\n", status);
    EXPECT_EQ(status, 0);
    printf("------end UsbSerialAddOrRemoveInterface_006------\n");
}
}
