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
#include "usb_utils.h"

using namespace std;
using namespace testing::ext;

namespace {
const string WLOG_FILE = "/data/acm_write_xts";
const string RLOG_FILE = "/data/acm_read_xts";

class UsbDeviceSerialFuncTest : public testing::Test {
protected:
    static void SetUpTestCase(void)
    {
        printf("------start UsbDeviceSerialFuncTest------\n");
        system("cat /dev/null > /data/acm_write_xts");
        system("cat /dev/null > /data/acm_read_xts");
    }
    static void TearDownTestCase(void)
    {
        printf("------end UsbDeviceSerialFuncTest------\n");
    }
};

/**
 * @tc.number    : H_Lx_H_Sub_usb_IO read_002，H_Lx_H_Sub_usb_IO read_008
 * @tc.name      : 验证device sdk的数据读写
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbDeviceSerialFuncTest, DeviceIOTest_001, TestSize.Level1)
{
    printf("------start DeviceIOTest_001------\n");
    ASSERT_EQ(system("usb_dev_test -1 abc &"), 0) << "ErrInfo:  failed to start acm read";
    sleep(2);
    const string data = "abc123";
    double startTs = GetNowTs();
    string wlog, rlog;
    ASSERT_EQ(system(("usb_dev_test -2 '" + data + "'").c_str()), 0);
    wlog = "send data[" + data + "] to host";
    rlog = "recv data[" + data + "] from host";
    sleep(2);
    EXPECT_TRUE(HasLog(wlog, startTs, WLOG_FILE));
    EXPECT_TRUE(HasLog(rlog, startTs, RLOG_FILE));
    printf("------end DeviceIOTest_001------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IO read_002，H_Lx_H_Sub_usb_IO read_008
 * @tc.name      : 验证device sdk的数据读写
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbDeviceSerialFuncTest, DeviceIOTest_002, TestSize.Level1)
{
    printf("------start DeviceIOTest_002------\n");
    const string data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    double startTs = GetNowTs();
    string wlog, rlog;
    for (int32_t i = 0; data[i].size() > 0; i++) {
        ASSERT_EQ(system(("usb_dev_test -2 '" + data[i] + "'").c_str()), 0);
        wlog = "send data[" + data[i] + "] to host";
        rlog = "recv data[" + data[i] + "] from host";
        sleep(2);
        EXPECT_TRUE(HasLog(wlog, startTs, WLOG_FILE));
        EXPECT_TRUE(HasLog(rlog, startTs, RLOG_FILE));
    }
    printf("------end DeviceIOTest_002------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IO read_002，H_Lx_H_Sub_usb_IO read_008
 * @tc.name      : 验证device sdk的数据读写
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbDeviceSerialFuncTest, DeviceIOTest_003, TestSize.Level2)
{
    printf("------start DeviceIOTest_003------\n");
    for (int32_t i = 0; i < 30; i++) {
        system("usb_dev_test -2 `date +%s%N | md5sum | cut -c 1-32`");
    }
    sleep(1);
    double startTs = GetNowTs();
    string wlog, rlog;
    const string data = "abc";
    ASSERT_EQ(system(("usb_dev_test -2 '" + data + "'").c_str()), 0);
    wlog = "send data[" + data + "] to host";
    rlog = "recv data[" + data + "] from host";
    sleep(2);
    EXPECT_TRUE(HasLog(wlog, startTs, WLOG_FILE));
    EXPECT_TRUE(HasLog(rlog, startTs, RLOG_FILE));
    printf("------end DeviceIOTest_003------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Instance_002, H_Lx_D_Sub_usb_Descriptor_002, H_Lx_D_Sub_usb_Descriptor_001,
 * @tc.name      : 验证HCS中配置的设备描述符、配置描述符、接口描述符和端点描述符解析是否正确
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbDeviceSerialFuncTest, HcsConfigTest_001, TestSize.Level1)
{
    printf("------start HcsConfigTest_001------\n");
    const char *idVendor = "12d1";
    const char *idPorduct = "5000";
    const char *bcdDevice = "0223";
    const char *configurationValue = "1";
    const int32_t logMaxLen = 100;
    char targetLog[logMaxLen] = {0};
    const char *fmt = "recv data[%s %s %s %s] from host";
    snprintf_s(targetLog, logMaxLen, logMaxLen -1, fmt, idVendor, \
             idPorduct, bcdDevice, configurationValue);
    const char *cmd = "usb_dev_test -2 GET_DESCRIPTOR";
    double startTs = GetNowTs();
    ASSERT_EQ(system(cmd), 0);
    sleep(1);
    EXPECT_TRUE(HasLog(string(targetLog), startTs, RLOG_FILE));
    ASSERT_EQ(system("killall usb_dev_test"), 0) << "ErrInfo:  failed to kill usb_dev_test";
    printf("------end HcsConfigTest_001------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Instance_002
 * @tc.name      : 验证HCS中配置的设备描述符解析是否正确
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbDeviceSerialFuncTest, HcsConfigTest_002, TestSize.Level2)
{
    printf("------start HcsConfigTest_002------\n");
    const char *idVendor = "18d2";
    const char *idProduct = "4ee8";
    const char *bDeviceProtocol = "01";

    ASSERT_EQ(system(("prop_test -s idVendor " + string(idVendor)).c_str()), 0);
    ASSERT_EQ(system(("prop_test -s idProduct " + string(idProduct)).c_str()), 0);
    ASSERT_EQ(system(("prop_test -s bDeviceProtocol " + string(bDeviceProtocol)).c_str()), 0);
    ASSERT_EQ(system("prop_test -g idVendor"), 0);
    ASSERT_EQ(system("prop_test -g idProduct"), 0);
    ASSERT_EQ(system("prop_test -g bDeviceProtocol"), 0);
    ASSERT_EQ(system("prop_test -s idVendor 12d1"), 0);
    ASSERT_EQ(system("prop_test -s idProduct 5000"), 0);
    ASSERT_EQ(system("prop_test -s bDeviceProtocol 00"), 0);
    printf("------end HcsConfigTest_002------\n");
}
}