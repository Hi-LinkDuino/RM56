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

#ifndef USB_RAW_API_LIBRARY_H
#define USB_RAW_API_LIBRARY_H

#include "hdf_device_desc.h"
#include "hdf_usb_pnp_manage.h"
#include "usb_session.h"
#include "usb_ddk_device.h"
#include "usb_ddk_request.h"
#include "usb_raw_api.h"

#define BYTE_LENGTH         8

#define USB_HOST_PNP_SERVICE_NAME "hdf_usb_pnp_notify_service"

#define USB_RAW_REQUEST_TIME_ZERO_MS        (0)
#define USB_RAW_REQUEST_DEFAULT_TIMEOUT     (1000)
#define USB_RAW_REQUEST_TIMEOUT_MAX         (0xFFFFFFFF)

#define DESC_HEADER_LENGTH  2

typedef pid_t UsbRawTidType;

struct UsbRawControlSetup {
    uint8_t requestType;
    uint8_t request;
    uint16_t value;
    uint16_t index;
    uint16_t length;
} __attribute__((packed));

#define USB_RAW_CONTROL_SETUP_SIZE (sizeof(struct UsbRawControlSetup))

union UsbiConfigDescBuf {
    struct UsbiConfigurationDescriptor desc;
    uint8_t buf[USB_DDK_DT_CONFIG_SIZE];
    uint16_t align;     /* Force 2-byte alignment */
};

enum UsbRawDescriptorType {
    USB_RAW_CONFIG_DESCRIPTOR_TYPE,
    USB_RAW_INTERFACE_DESCRIPTOR_TYPE,
    USB_RAW_ENDPOINT_DESCRIPTOR_TYPE,
    USB_RAW_AUDIO_ENDPOINT_DESCRIPTOR_TYPE,
};

enum RawRequestTimeoutFlags {
    RAW_REQUEST_OS_HANDLES_TIMEOUT = 1U << 0,
    RAW_REQUEST_TIMEOUT_HANDLED = 1U << 1,
    RAW_REQUEST_TIMED_OUT = 1U << 2,
};

struct UsbMessageQueue {
    struct DListHead entry;
    struct OsalMutex mutex;
    struct OsalSem sem;
};

struct RawUsbRamTestList {
    uintptr_t address;
    uint32_t size;
    struct DListHead list;
    struct OsalMutex lock;
};

struct UsbSession *RawGetSession(const struct UsbSession *session);
int32_t RawInit(struct UsbSession **session);
int32_t RawExit(const struct UsbSession *session);
struct UsbDeviceHandle *RawOpenDevice(const struct UsbSession *session, uint8_t busNum, uint8_t usbAddr);
int32_t RawCloseDevice(const struct UsbDeviceHandle *devHandle);
int32_t RawClaimInterface(struct UsbDeviceHandle *devHandle, int32_t interfaceNumber);
struct UsbHostRequest *AllocRequest(const struct UsbDeviceHandle *devHandle,  int32_t isoPackets, size_t length);
int32_t FreeRequest(const struct UsbHostRequest *request);
int32_t RawFillBulkRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbFillRequestData *fillRequestData);
int32_t RawFillControlSetup(const unsigned char *setup, const struct UsbControlRequestData *requestData);
int32_t RawFillControlRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbFillRequestData *fillRequestData);
int32_t RawFillInterruptRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbFillRequestData *fillRequestData);
int32_t RawFillIsoRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbFillRequestData *fillRequestData);
int32_t RawSendControlRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbControlRequestData *requestData);
int32_t RawSendBulkRequest(const struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbRequestData *requestData);
int32_t RawSendInterruptRequest(const struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbRequestData *requestData);
struct UsbHostRequest *RawAllocRequest(const struct UsbDeviceHandle *devHandle, int32_t isoPackets, int32_t length);
int32_t RawFreeRequest(const struct UsbHostRequest *request);
int32_t RawGetConfigDescriptor(const struct UsbDevice *dev, uint8_t configIndex,
    struct UsbRawConfigDescriptor **config);
void RawClearConfiguration(struct UsbRawConfigDescriptor *config);
int32_t RawGetConfiguration(const struct UsbDeviceHandle *devHandle, int32_t *config);
int32_t RawSetConfiguration(const struct UsbDeviceHandle *devHandle, int32_t configuration);
int32_t RawGetDescriptor(const struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbRawDescriptorParam *param, const unsigned char *data);
struct UsbDevice *RawGetDevice(const struct UsbDeviceHandle *devHandle);
int32_t RawGetDeviceDescriptor(const struct UsbDevice *dev, struct UsbDeviceDescriptor *desc);
int32_t RawReleaseInterface(struct UsbDeviceHandle *devHandle, int32_t interfaceNumber);
int32_t RawResetDevice(const struct UsbDeviceHandle *devHandle);
int32_t RawSubmitRequest(const struct UsbHostRequest *request);
int32_t RawCancelRequest(const struct UsbHostRequest *request);
int32_t RawHandleRequest(const struct UsbDeviceHandle *devHandle);
int32_t RawClearHalt(const struct UsbDeviceHandle *devHandle, uint8_t pipeAddress);
int32_t RawHandleRequestCompletion(struct UsbHostRequest *request, UsbRequestStatus status);
int32_t RawSetInterfaceAltsetting(
    const struct UsbDeviceHandle *devHandle, uint8_t interfaceNumber, uint8_t settingIndex);
UsbRawTidType RawGetTid(void);
int32_t RawRegisterSignal(void);
int32_t RawKillSignal(struct UsbDeviceHandle *devHandle, UsbRawTidType tid);
int32_t RawInitPnpService(enum UsbPnpNotifyServiceCmd cmdType, struct UsbPnpAddRemoveInfo infoData);
void RawRequestListInit(struct UsbDevice *deviceObj);
void *RawUsbMemAlloc(size_t size);
void *RawUsbMemCalloc(size_t size);
void RawUsbMemFree(void *mem);
int32_t RawUsbMemTestTrigger(bool enable);

#endif /* USB_RAW_API_LIBRARY_H */
