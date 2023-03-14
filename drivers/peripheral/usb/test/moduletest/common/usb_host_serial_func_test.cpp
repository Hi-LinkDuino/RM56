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
const string RLOG_FILE = "/data/acm_read_xts";
const string PNP_LOG_FILE = "/data/usbhost_pnp_xts";

class UsbHostSerialFuncTest : public testing::Test {
protected:
    static void SetUpTestCase(void)
    {
        printf("------start UsbHostSerialFuncTest------\n");
        system("cat /dev/null > /data/acm_write_xts");
        system("cat /dev/null > /data/acm_read_xts");
        system("cat /dev/null > /data/usbhost_pnp_xts");
    }
    static void TearDownTestCase(void)
    {
        printf("------end UsbHostSerialFuncTest------\n");
    }
};

/**
 * @tc.number    : H_Lx_H_Sub_usb_insert_001
 * @tc.name      : 验证插入USB串口设备后加载驱动
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialInsertOnce, TestSize.Level1)
{
    printf("------start UsbSerialInsertOnce------\n");
    const string targetLog = "usb pnp sample device driver test add start";
    double startTs = GetNowTs();
    ASSERT_EQ(system("usbhost_pnp_test -add"), 0);
    sleep(1);
    EXPECT_TRUE(HasLog(targetLog, startTs, PNP_LOG_FILE)) << "ErrInfo: serial driver cannot be loaded in 1s";
    printf("------end UsbSerialInsertOnce------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_insert_002
 * @tc.name      : 验证拔出USB串口设备后卸载驱动
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialPullOutOnce, TestSize.Level1)
{
    printf("------start UsbSerialPullOutOnce------\n");
    const string targetLog = "usb pnp sample device driver test remove start";
    double startTs = GetNowTs();
    ASSERT_EQ(system("usbhost_pnp_test -remove"), 0);
    sleep(1);
    EXPECT_TRUE(HasLog(targetLog, startTs, PNP_LOG_FILE)) << "ErrInfo: serial driver cannot be unloaded in 1s";
    printf("------end UsbSerialPullOutOnce------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_insert_009
 * @tc.name      : 验证20次插拔串口设备每次都能够正常识别
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialInsertPullOut20, TestSize.Level2)
{
    printf("------start UsbSerialInsertPullOut20------\n");
    const string loadLog = "usb pnp sample device driver test add start";
    const string unloadLog = "usb pnp sample device driver test remove start";
    double startTs = GetNowTs();
    for (int32_t i = 0; i < 20; i++) {
        startTs = GetNowTs();
        ASSERT_EQ(system("usbhost_pnp_test -add"), 0);
        sleep(1);
        EXPECT_TRUE(HasLog(loadLog, startTs, PNP_LOG_FILE)) << "ErrInfo: serial driver cannot be loaded in 1s";
        ASSERT_EQ(system("usbhost_pnp_test -remove"), 0);
        sleep(1);
        EXPECT_TRUE(HasLog(unloadLog, startTs, PNP_LOG_FILE)) << "ErrInfo: serial driver cannot be unloaded in 1s";
    }
    printf("------end UsbSerialInsertPullOut20------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_insert_011
 * @tc.name      : 验证20次快速拔插串口后能否正常加载和卸载驱动
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialInsertPullOutQuickly20, TestSize.Level2)
{
    printf("------start UsbSerialInsertPullOutQuickly20------\n");
    const string loadLog = "usb pnp sample device driver test add start";
    const string unloadLog = "usb pnp sample device driver test remove start";
    for (int32_t i = 0; i < 20; i++) {
        ASSERT_EQ(system("usbhost_pnp_test -add"), 0);
        usleep(100 * 1000);
        ASSERT_EQ(system("usbhost_pnp_test -remove"), 0);
    }
    double startTs = GetNowTs();
    ASSERT_EQ(system("usbhost_pnp_test -add"), 0);
    sleep(1);
    EXPECT_TRUE(HasLog(loadLog, startTs, PNP_LOG_FILE)) << "ErrInfo: serial driver cannot be loaded in 1s";
    ASSERT_EQ(system("usbhost_pnp_test -remove"), 0);
    sleep(1);
    EXPECT_TRUE(HasLog(unloadLog, startTs, PNP_LOG_FILE)) << "ErrInfo: serial driver cannot be unloaded in 1s";
    printf("------end UsbSerialInsertPullOutQuickly20------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IO read_001，H_Lx_H_Sub_usb_IO read_007
 * @tc.name      : USB串口同步数据读写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, CheckUsbSerialIoWriteSync_001, TestSize.Level1)
{
    printf("------start CheckUsbSerialIoWriteSync_001------\n");
    const string data = "abc";
    double startTs = GetNowTs();
    string wlog, rlog;
    wlog = "send data[" + data + "] to device";
    rlog = "recv data[" + data + "] from device";
    ASSERT_EQ(system("usbhost_ddk_test -AR &"), 0);
    ASSERT_EQ(system(("usbhost_ddk_test -AW '" + data + "'").c_str()), 0);
    sleep(3);
    EXPECT_TRUE(HasLog(wlog, startTs)) << "ErrInfo: cannot find sync write log";
    EXPECT_TRUE(HasLog(rlog, startTs, RLOG_FILE)) << "ErrInfo: cannot find sync recv log";
    printf("------end CheckUsbSerialIoWriteSync_001------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IO read_001，H_Lx_H_Sub_usb_IO read_007
 * @tc.name      : USB串口同步数据读写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, CheckUsbSerialIoWriteSync_002, TestSize.Level1)
{
    printf("------start CheckUsbSerialIoWriteSync_002------\n");
    const string data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    double startTs = GetNowTs();
    string wlog, rlog;
    for (int32_t i = 0; data[i].size() > 0; i++) {
        wlog = "send data[" + data[i] + "] to device";
        rlog = "recv data[" + data[i] + "] from device";
        ASSERT_EQ(system("usbhost_ddk_test -AR &"), 0);
        ASSERT_EQ(system(("usbhost_ddk_test -AW '" + data[i] + "'").c_str()), 0);
        sleep(3);
        EXPECT_TRUE(HasLog(wlog, startTs)) << "ErrInfo: cannot find sync write log";
        EXPECT_TRUE(HasLog(rlog, startTs, RLOG_FILE)) << "ErrInfo: cannot find sync recv log";
    }
    printf("------end CheckUsbSerialIoWriteSync_002------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IO read_003, H_Lx_H_Sub_usb_IO read_009
 * @tc.name      : USB串口同步读写1KB数据
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, CheckUsbSerialIoWriteSync_003, TestSize.Level2)
{
    printf("------start CheckUsbSerialIoWriteSync_003------\n");
    const string s = "0123456789abcdef";
    string data;
    int32_t totalSize = 1024;
    int32_t writeCnt = 8;
    unsigned int n = 0;
    while (n < totalSize / writeCnt / s.size()) {
        data += s;
        n++;
    }
    const string wlog = "send data[" + data + "] to device";
    const string rlog = "recv data[" + data + "] from device";
    double startTs;
    for (int32_t i = 0; i < writeCnt; i++) {
        startTs = GetNowTs();
        ASSERT_EQ(system("usbhost_ddk_test -AR &"), 0);
        ASSERT_EQ(system(("usbhost_ddk_test -AW '" + data + "'").c_str()), 0);
        sleep(3);
        EXPECT_TRUE(HasLog(wlog, startTs)) << "ErrInfo: cannot find sync write log";
        EXPECT_TRUE(HasLog(rlog, startTs, RLOG_FILE)) << "ErrInfo: cannot find sync recv log";
    }
    printf("------end CheckUsbSerialIoWriteSync_003------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IO read_013， H_Lx_H_Sub_usb_IO read_017
 * @tc.name      : USB串口异步数据读写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, CheckUsbSerialIoWriteAsync_001, TestSize.Level1)
{
    printf("------start CheckUsbSerialIoWriteAsync_001------\n");
    ASSERT_EQ(system("usbhost_ddk_test -Ar &"), 0) << \
    "ErrInfo:  failed to start async read";
    sleep(3);
    const string data = "abc";
    double startTs = GetNowTs();
    string wlog, rlog;
    wlog = "send data[" + data + "] to device";
    rlog = "recv data[" + data + "] from device";
    ASSERT_EQ(system(("usbhost_ddk_test -Aw '" + data + "'").c_str()), 0);
    sleep(3);
    EXPECT_TRUE(HasLog(wlog, startTs)) << "ErrInfo: cannot find async write log";
    EXPECT_TRUE(HasLog(rlog, startTs, RLOG_FILE)) << "ErrInfo: cannot find async recv log";
    printf("------end CheckUsbSerialIoWriteAsync_001------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IO read_013， H_Lx_H_Sub_usb_IO read_017
 * @tc.name      : USB串口异步数据读写
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, CheckUsbSerialIoWriteAsync_002, TestSize.Level1)
{
    printf("------start CheckUsbSerialIoWriteAsync_002------\n");
    const string data[] = {
        "0123456789",
        "Z",
        "0!a@1#b$2%c^3&D*4(E)5-F_",
        ""
    };
    double startTs = GetNowTs();
    string wlog, rlog;
    for (int32_t i = 0; data[i].size() > 0; i++) {
        wlog = "send data[" + data[i] + "] to device";
        rlog = "recv data[" + data[i] + "] from device";
        ASSERT_EQ(system(("usbhost_ddk_test -Aw '" + data[i] + "'").c_str()), 0);
        sleep(3);
        EXPECT_TRUE(HasLog(wlog, startTs)) << "ErrInfo: cannot find async write log";
        EXPECT_TRUE(HasLog(rlog, startTs, RLOG_FILE)) << "ErrInfo: cannot find async recv log";
    }
    printf("------end CheckUsbSerialIoWriteAsync_002------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_IO read_019
 * @tc.name      : USB串口异步读写1KB数据
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, CheckUsbSerialIoWriteAsync_003, TestSize.Level2)
{
    printf("------start CheckUsbSerialIoWriteAsync_003------\n");
    const string s = "0123456789abcdef";
    string data;
    int32_t totalSize = 1024;
    int32_t writeCnt = 8;
    unsigned int n = 0;
    while (n < totalSize / writeCnt / s.size()) {
        data += s;
        n++;
    }
    const string wlog = "send data[" + data + "] to device";
    const string rlog = "recv data[" + data + "] from device";
    double startTs;
    for (int32_t i = 0; i < writeCnt; i++) {
        startTs = GetNowTs();
        ASSERT_EQ(system(("usbhost_ddk_test -Aw '" + data + "'").c_str()), 0);
        sleep(3);
        EXPECT_TRUE(HasLog(wlog, startTs)) << "ErrInfo: cannot find async write log";
        EXPECT_TRUE(HasLog(rlog, startTs, RLOG_FILE)) << "ErrInfo: cannot find async recv log";
    }
    ASSERT_EQ(system("killall usbhost_ddk_test"), 0) << "ErrInfo:  failed to kill async read";
    printf("------end CheckUsbSerialIoWriteAsync_003------\n");
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
    string targetLog;
    double startTs = GetNowTs();
    ASSERT_EQ(system("usbhost_ddk_test -AC"), 0);
    targetLog = "usb serial control command[CMD_STD_CTRL_GET_DESCRIPTOR] done";
    EXPECT_TRUE(HasLog(targetLog, startTs));
    ASSERT_EQ(system("usbhost_ddk_test -Ai"), 0);
    targetLog = "usb serial control command[CMD_STD_CTRL_GET_INTERFACE] done";
    EXPECT_TRUE(HasLog(targetLog, startTs));
    ASSERT_EQ(system("usbhost_ddk_test -Ag"), 0);
    targetLog = "usb serial control command[CMD_STD_CTRL_GET_CONFIGURATION] done";
    EXPECT_TRUE(HasLog(targetLog, startTs));
    ASSERT_EQ(system("usbhost_ddk_test -As"), 0);
    targetLog = "usb serial control command[CMD_STD_CTRL_GET_STATUS] done";
    EXPECT_TRUE(HasLog(targetLog, startTs));
    printf("------end UsbSerialStdCtrlCmdSync_001------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_Control_002
 * @tc.name      : USB串口标准控制命令的同步处理
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 2
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialStdCtrlCmdSync_002, TestSize.Level2)
{
    printf("------start UsbSerialStdCtrlCmdSync_002------\n");
    string targetLog;
    double startTs = GetNowTs();
    ASSERT_EQ(system("usbhost_ddk_test -As"), 0);
    ASSERT_EQ(system("usbhost_ddk_test -As"), 0);
    targetLog = "usb serial control command[CMD_STD_CTRL_GET_STATUS] done";
    EXPECT_TRUE(HasLog(targetLog, startTs));
    printf("------end UsbSerialStdCtrlCmdSync_002------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_Control_002
 * @tc.name      : USB串口类控制命令的同步处理
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialClsCtrlCmdSync, TestSize.Level1)
{
    printf("------start UsbSerialClsCtrlCmdSync------\n");
    string targetLog;
    double startTs = GetNowTs();
    ASSERT_EQ(system("usbhost_ddk_test -Ac"), 0);
    targetLog = "usb serial control command[CMD_CLASS_CTRL] done";
    EXPECT_TRUE(HasLog(targetLog, startTs));
    printf("------end UsbSerialClsCtrlCmdSync------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_Control_004
 * @tc.name      : USB串口标准控制命令的异步处理
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, UsbSerialStdCtrlCmdAsync, TestSize.Level1)
{
    printf("------start UsbSerialStdCtrlCmdAsync------\n");
    string targetLog;
    double startTs = GetNowTs();
    ASSERT_EQ(system("usbhost_ddk_test -Ad"), 0);
    targetLog = "usb serial control command[CMD_STD_CTRL_GET_DESCRIPTOR_ASYNC] done";
    EXPECT_TRUE(HasLog(targetLog, startTs));
    printf("------end UsbSerialStdCtrlCmdAsync------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_Instance_001, H_Lx_D_Sub_usb_Descriptor_002, H_Lx_D_Sub_usb_Descriptor_001
 * @tc.name      : 动态实例化USB串口设备, 支持通用属性可配置, 从HCS导入属性及默认值
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, CheckUsbSerialDeviceInfo, TestSize.Level1)
{
    printf("------start CheckUsbSerialDeviceInfo------\n");
    const char *idVendor = "0x12d1";
    const char *idProduct = "0x5000";
    const char *bDeviceClass = "0x00";
    const char *bDeviceSubClass = "0x00";
    const char *bDeviceProtocol = "0x00";
    const int32_t logMaxLen = 1000;
    char targetLog[logMaxLen] = {0};
    const char *fmt = "device descriptor info:[%s %s %s %s %s]";
    snprintf_s(targetLog, logMaxLen, logMaxLen -1, fmt, \
    idVendor, idProduct, bDeviceClass, bDeviceSubClass, bDeviceProtocol);
    printf("targetLog==>%s\n", targetLog);
    double startTs = GetNowTs();
    ASSERT_EQ(system("usbhost_ddk_test -AC"), 0);
    EXPECT_TRUE(HasLog(string(targetLog), startTs, RLOG_FILE));
    printf("------end CheckUsbSerialDeviceInfo------\n");
}

/**
 * @tc.number    : H_Lx_H_Sub_usb_DFR_007
 * @tc.name      : 验证进程被杀掉后SDK自启动功能
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSerialFuncTest, KillHostSdkProcess, TestSize.Level1)
{
    printf("------start KillHostSdkProcess------\n");
    system("kill $(pidof pnp_host)");
    printf("Please waiting for restarting sdk process...\n");
    sleep(5);
    ASSERT_EQ(system("usbhost_ddk_test -Ar &"), 0) << \
    "ErrInfo:  failed to start async read";
    sleep(3);
    const string data = "abc";
    double startTs = GetNowTs();
    string wlog, rlog;
    wlog = "send data[" + data + "] to device";
    rlog = "recv data[" + data + "] from device";
    ASSERT_EQ(system(("usbhost_ddk_test -Aw '" + data + "'").c_str()), 0);
    sleep(3);
    EXPECT_TRUE(HasLog(wlog, startTs)) << "ErrInfo: cannot find async write log";
    EXPECT_TRUE(HasLog(rlog, startTs, RLOG_FILE)) << "ErrInfo: cannot find async recv log";
    system("killall usbhost_ddk_test");
    printf("------end KillHostSdkProcess------\n");
}
}