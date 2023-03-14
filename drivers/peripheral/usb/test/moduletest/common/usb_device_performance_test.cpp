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
#include <csignal>
#include <unistd.h>
#include <gtest/gtest.h>
#include "hdf_base.h"
#include "usb_utils.h"

using namespace std;
using namespace testing::ext;

namespace {
class UsbDevicePerformanceTest : public testing::Test {
protected:
    static void SetUpTestCase(void)
    {
        printf("------start UsbDevicePerformancefTest------\n");
    }
    static void TearDownTestCase(void)
    {
        printf("------end UsbDevicePerformancefTest------\n");
    }
};

/**
 * @tc.number    : H_Lx_D_Sub_usb_performance_005
 * @tc.name      : Device SDK ROM占用<60K
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbDevicePerformanceTest, CheckDeviceSdkRom, TestSize.Level1)
{
    printf("------start CheckDeviceSdkRom------\n");
    const char *hostSdkPath = HDF_LIBRARY_FULL_PATH("libusb_ddk_device");
    int64_t size = 0;
    FILE *fp = fopen(hostSdkPath, "rb");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    EXPECT_LT(size, 60 * 1024);
    printf("------end CheckDeviceSdkRom------\n");
}

/**
 * @tc.number    : H_Lx_D_Sub_usb_performance_006, H_Lx_D_Sub_usb_performance_007,
 *                 H_Lx_D_Sub_usb_performance_008
 * @tc.name      : Device SDK RAM占用峰值<40K，RAM占用均值<30K;Device SDK CPU占用峰值<15%，CPU占用均值<10%;
 *                 Device SDK驱动框架下单进程加载SDK，最大并发线程数<5
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbDevicePerformanceTest, CheckDeviceSdkProcInfo, TestSize.Level1)
{
    printf("------start CheckDeviceSdkProcInfo------\n");
    const string logFile = "/data/usb_proclog.txt";
    const string script = "usb_watch_process.sh";
    int32_t processCount;
    pid_t watchPid = 0;
    char *pch = nullptr;
    FILE *res = nullptr;
    struct ProcInfo info = {0, 0, 0, 0, 0};
    ASSERT_EQ(access(script.c_str(), F_OK) , 0) << "ErrInfo: shell script not exists";
    if (access(script.c_str(), X_OK) == -1) {
        system(("chmod +x " + script).c_str());
    }
    printf("try to start usb_watch_process.sh...\n");
    ASSERT_EQ(system(("nohup sh " + script + " usbfnMaster_host > /data/nohup.out &").c_str()), 0);
    printf("usb_watch_process.sh is running...\n");
    for (int32_t i = 0; i < 1000; i++) {
        system("usb_dev_test -2 $RANDOM");
        printf("Write data %d times\n", i);
        usleep(100 * 1000);
    }
    res = popen("ps -ef | grep 'usb_watch_process.sh' | grep -v grep | cut -F 2", "r");
    pch = ParseSysCmdResult(*res, 1, 1);
    watchPid = stoi(pch);
    printf("try to kill usb_watch_process.sh, pid: %d\n", watchPid);
    kill(watchPid, SIGKILL);
    sleep(3);
    CalcProcInfoFromFile(info, logFile);
    EXPECT_LT(info.cpuPeak, 15) << "ErrInfo: cpu peak is not less than 15%";
    EXPECT_LT(info.cpuAvg, 10) << "ErrInfo: average cpu is not less than 10%";
    res = popen("ps -ef | grep 'usbfnMaster_host' | grep -v grep | wc -l", "r");
    pch = ParseSysCmdResult(*res, 1, 1);
    processCount = stoi(pch);
    EXPECT_EQ(processCount, 1) << "ErrInfo: device sdk process count is not equal to 1";
    printf("------end CheckDeviceSdkProcInfo------\n");
}
}
