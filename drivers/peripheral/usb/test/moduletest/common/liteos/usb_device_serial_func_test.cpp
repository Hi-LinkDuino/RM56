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
#include <gtest/gtest.h>
#include <unistd.h>
#include "securec.h"
#include "lib_acm_test.h"

using namespace std;
using namespace testing::ext;

namespace {
class UsbDeviceSerialFuncTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void UsbDeviceSerialFuncTest::SetUpTestCase()
{
    acm_open();
}

void UsbDeviceSerialFuncTest::TearDownTestCase()
{
    acm_close();
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Control_msg_004，H_Lx_D_Sub_usb_Control_msg_006
 * @tc.name      : 验证device sdk的数据读写
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbDeviceSerialFuncTest, DeviceIOTest_001, TestSize.Level1)
{
    printf("------start DeviceIOTest_001------\n");
    char writeBuf[] = "abc123";
    char readBuf[256] = {0};
    acm_write(writeBuf);
    acm_read(readBuf);
    EXPECT_EQ(strcmp(writeBuf, readBuf), 0);
    printf("------end DeviceIOTest_001------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Control_msg_004，H_Lx_D_Sub_usb_Control_msg_006
 * @tc.name      : 验证device sdk的数据读写
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbDeviceSerialFuncTest, DeviceIOTest_002, TestSize.Level1)
{
    printf("------start DeviceIOTest_002------\n");
    const char * data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    char readBuf[256] = {0};
    for (int32_t i = 0; strlen(data[i]) > 0; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        acm_write(const_cast<char *>(data[i]));
        acm_read(readBuf);
        EXPECT_EQ(strcmp(const_cast<char *>(data[i]), readBuf), 0);
    }
    printf("------end DeviceIOTest_002------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Control_msg_004，H_Lx_D_Sub_usb_Control_msg_006
 * @tc.name      : 验证device sdk的数据读写
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbDeviceSerialFuncTest, DeviceIOTest_003, TestSize.Level2)
{
    printf("------start DeviceIOTest_003------\n");
    char wdata[256] = "AAA1234567890";
    char rdata[256] = {0};
    for (int32_t i = 0; i < 30; i++) {
        acm_write(wdata);
        acm_read(rdata);
        memset_s(rdata, sizeof(rdata), 0, sizeof(rdata));
    }
    char writeBuf[] = "aaa111";
    char readBuf[256] = {0};
    acm_write(writeBuf);
    acm_read(readBuf);
    EXPECT_EQ(strcmp(writeBuf, readBuf), 0);
    printf("------end DeviceIOTest_003------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Control_msg_004，H_Lx_D_Sub_usb_Control_msg_006
 * @tc.name      : 验证device sdk的数据读写
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbDeviceSerialFuncTest, DeviceIOTest_004, TestSize.Level2)
{
    printf("------start DeviceIOTest_004------\n");
    const string s = "0123456789abcdef";
    string data;
    int32_t totalSize = 1024;
    int32_t writeCnt = 8;
    unsigned int n = 0;
    while (n < totalSize / writeCnt / s.size()) {
        data += s;
        n++;
    }
    char readBuf[256] = {0};
    char writeBuf[256] = {0};
    strcpy_s(writeBuf, sizeof(writeBuf), data.c_str());
    for (int32_t i = 0; i < writeCnt; i++) {
        memset_s(readBuf, sizeof(readBuf), 0, sizeof(readBuf));
        acm_write(writeBuf);
        acm_read(readBuf);
        EXPECT_EQ(strcmp(writeBuf, readBuf), 0);
    }
    char exitBuf[] = "q";
    acm_write(exitBuf);
    printf("------end DeviceIOTest_004------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Instance_002
 * @tc.name      : 支持应用添加自定义属性和添加Set/Get回调函数
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbDeviceSerialFuncTest, HcsPropTest_001, TestSize.Level1)
{
    printf("------start HcsPropTest_001------\n");
    const char *propName1 = "idVendor";
    const char *propName2 = "idProduct";
    char propValue1[] = "12d1";
    char propValue2[] = "5000";
    char readBuf1[256] = {0};
    char readBuf2[256] = {0};
    acm_prop_read(propName1, readBuf1);
    printf("readBuf1==>%s\n", readBuf1);
    EXPECT_EQ(strcmp(propValue1, readBuf1), 0);
    acm_prop_read(propName2, readBuf2);
    printf("readBuf2==>%s\n", readBuf2);
    EXPECT_EQ(strcmp(propValue2, readBuf2), 0);
    printf("------end HcsPropTest_001------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Instance_002
 * @tc.name      : 支持应用添加自定义属性和添加Set/Get回调函数
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbDeviceSerialFuncTest, HcsPropTest_002, TestSize.Level1)
{
    printf("------start HcsPropTest_002------\n");
    const char *propName = "idVendor";
    const char *writeBuf = "1234";
    char readBuf[256] = {0};
    acm_prop_write(propName, writeBuf);
    acm_prop_read(propName, readBuf);
    printf("readBuf==>%s\n", readBuf);
    EXPECT_EQ(strcmp(const_cast<char *>(writeBuf), readBuf), 0);
    printf("------end HcsPropTest_002------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Instance_002
 * @tc.name      : 支持应用添加自定义属性和添加Set/Get回调函数
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbDeviceSerialFuncTest, HcsPropTest_003, TestSize.Level1)
{
    printf("------start HcsPropTest_003------\n");
    const char *propName = "idVendor";
    const char *writeBuf = "12d1";
    char readBuf[256] = {0};
    acm_prop_write(propName, writeBuf);
    acm_prop_read(propName, readBuf);
    printf("readBuf==>%s\n", readBuf);
    EXPECT_EQ(strcmp(const_cast<char *>(writeBuf), readBuf), 0);
    printf("------end HcsPropTest_003------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Instance_002
 * @tc.name      : 支持应用添加自定义属性和添加Set/Get回调函数
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbDeviceSerialFuncTest, HcsPropTest_004, TestSize.Level1)
{
    printf("------start HcsPropTest_004------\n");
    const char *propName = "testp";
    const char *propValue = "a123";
    char readBuf[256] = {0};
    acm_prop_regist(propName, propValue);
    acm_prop_read(propName, readBuf);
    printf("readBuf==>%s", readBuf);
    EXPECT_EQ(strcmp(const_cast<char *>(propValue), readBuf), 0);
    printf("------end HcsPropTest_004------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Instance_002
 * @tc.name      : 支持应用添加自定义属性和添加Set/Get回调函数
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbDeviceSerialFuncTest, HcsPropTest_005, TestSize.Level1)
{
    printf("------start HcsPropTest_005------\n");
    const char *propName = "testp";
    const char *writeBuf = "111";
    char readBuf[256] = {0};
    acm_prop_write(propName, writeBuf);
    acm_prop_read(propName, readBuf);
    printf("readBuf==>%s\n", readBuf);
    EXPECT_EQ(strcmp(const_cast<char *>(writeBuf), readBuf), 0);
    printf("------end HcsPropTest_005------\n");
}
}
