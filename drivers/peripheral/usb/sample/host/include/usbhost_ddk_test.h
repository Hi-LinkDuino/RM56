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

#ifndef USB_HOST_DDK_TEST_H
#define USB_HOST_DDK_TEST_H

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <hdf_sbuf.h>
#ifdef __LITEOS_USB_HOST_DDK_TEST__
#include "utils/hdf_base.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_device_desc.h"
#include "osal_mem.h"
#else
#include <servmgr_hdi.h>
#include <hdf_remote_service.h>
#include "usb_serial.h"
#endif
#include "osal_time.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DATA_MAX_LEN        512

#ifdef __LITEOS_USB_HOST_DDK_TEST__
typedef enum {
    HOST_ACM_SYNC_READ = 1,
    HOST_ACM_SYNC_WRITE,
    HOST_ACM_ASYNC_READ,
    HOST_ACM_ASYNC_WRITE,
    HOST_ACM_CTRL_READ,
    HOST_ACM_CTRL_WRITE,
    HOST_ACM_CTRL_CLASS_SYNC,
    HOST_ACM_CTRL_GET_STATUS,
    HOST_ACM_CTRL_SYNC_DESCRIPTOR,
    HOST_ACM_CTRL_ASYNC_DESCRIPTOR,
    HOST_ACM_CTRL_GET_CONFIGURATION,
    HOST_ACM_CTRL_GET_INTERFACE,
    HOST_ACM_SPEED_TEST,
    HOST_ACM_SET_BAUDRATE,
    HOST_ACM_GET_BAUDRATE,
    HOST_ACM_ADD_INTERFACE,
    HOST_ACM_REMOVE_INTERFACE,
} AcmModuleTestCmdType;
#endif

int32_t UsbHostDdkTestInit(const char *apiType);
int32_t UsbHostDdkTestAsyncRead(char *readSbuf);
void UsbHostDdkTestAsyncWrite(const char *buf);
void UsbHostDdkTestSyncRead(char *readSbuf);
void UsbHostDdkTestSyncWrite(const char *buf);
void UsbHostDdkTestCtrlClass(char *readSbuf);
void UsbHostDdkTestStdGetDes(char *readSbuf);
void UsbHostDdkTestStdGetDesAsync(char *readSbuf);
void UsbHostDdkTestStdGetStatus(char *readSbuf);
void UsbHostDdkTestSetBaudrate(uint32_t value);
void UsbHostDdkTestGetBaudrate(char *readSbuf);
void UsbHostDdkTestAddInterface(uint32_t value);
void UsbHostDdkTestRemoveInterface(uint32_t value);
int32_t UsbHostDdkTestOpen(int32_t cmdType);
int32_t UsbHostDdkTestClose(int32_t cmdType);

void TestStdGetConf(void);
void TestStdGetInterface(void);
void TestSpeed(void);
void TestExit(void);
bool TestGetExitFlag(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
