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

#include "usb_host_serial_loopback.h"
#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include <unistd.h>
#include "securec.h"
#include "usbhost_ddk_test.h"

using namespace std;
using namespace testing::ext;

namespace {
class UsbHostSerialLoopback : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void UsbHostSerialLoopback::SetUpTestCase()
{
    const char *apiType = "-SDK";
    UsbHostDdkTestInit(const_cast<char*>(apiType));
}

void UsbHostSerialLoopback::TearDownTestCase()
{
    TestExit();
}

HWTEST_F(UsbHostSerialLoopback, HostSerialLoopback, TestSize.Level1)
{
    printf("------start HostSerialLoopback------\n");
    char data[512] = {0};
    while (true) {
        UsbHostDdkTestOpen(HOST_ACM_SYNC_READ);
        UsbHostDdkTestSyncRead(data);
        UsbHostDdkTestClose(HOST_ACM_SYNC_READ);
        if (strlen(data) > 0) {
            if (strcmp(data, "q") == 0) {
                break;
            }
            UsbHostDdkTestOpen(HOST_ACM_SYNC_WRITE);
            UsbHostDdkTestSyncWrite(data);
            UsbHostDdkTestClose(HOST_ACM_SYNC_WRITE);
            memset_s(data, sizeof(data), 0, sizeof(data));
        }
    }
    printf("------end HostSerialLoopback------\n");
}
}