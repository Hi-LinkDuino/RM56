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

#ifndef USB_DDK_REQUEST_H
#define USB_DDK_REQUEST_H

#include "usb_ddk_device.h"
#include "usb_ddk_interface.h"
#include "usb_raw_api.h"

typedef enum {
    /* select a configuration for the device */
    USB_REQUEST_SELECT_CONFIGURATION,
    /* select an alternate setting for an interface */
    USB_REQUEST_SELECT_INTERFACE,
    /* get the device descriptor from a specific USB device/interface/endpoint */
    USB_REQUEST_GET_DESCRIPTOR,
    /* set descriptor on a device/interface/endpoint */
    USB_REQUEST_SET_DESCRIPTOR,
    /* set a feature on a device/interface/endpoint/other */
    USB_REQUEST_SET_FEATURE,
    /* clear a USB-defined feature on a device/interface/endpoint/other */
    USB_REQUEST_CLEAR_FEATURE,
    /* get status form a USB device/interface/endpoint/other */
    USB_REQUEST_GET_STATUS,
    /* get the current configuration on a USB device */
    USB_REQUEST_GET_CONFIGURATION = 0x08,
    /* get the current setting for an interface */
    USB_REQUEST_GET_INTERFACE,
} UsbStandardRequest;

typedef enum {
    USB_REQUEST_TYPE_CONTROL = 0U,
    USB_REQUEST_TYPE_ISOCHRONOUS = 1U,
    USB_REQUEST_TYPE_BULK = 2U,
    USB_REQUEST_TYPE_INTERRUPT = 3U,
} UsbRequestType;

struct UsbIsoPacketDesc {
    unsigned int length;
    unsigned int actualLength;
    unsigned int status;
};

struct UsbAdapterUrb {
    unsigned char type;
    unsigned char endPoint;
    int32_t status;
    unsigned int flags;
    void *buffer;
    int32_t bufferLength;
    int32_t actualLength;
    int32_t startFrame;
    int32_t numberOfPackets;
    int32_t errorCount;
    unsigned int signr;
    void *userContext;
    struct UsbIsoPacketDesc isoFrameDesc[0];
};

/* Note:The first five fields of the definition must be consistent with the UsbRawRequest structure definition. */
struct UsbHostRequest {
    /* the address of data buffer */
    unsigned char *buffer;
    /* the length of data buffer */
    uint32_t length;
    /* the actual length of the requested data */
    int32_t actualLength;
    /* the status of the request */
    UsbRequestStatus status;
    /* private user data */
    void *userData;

    /* the following fields are not visible to user */
    struct HdfSListNode entry;
    struct DListHead list;
    struct UsbDeviceHandle *devHandle;
    UsbRawRequestCallback callback;
    UsbRequestCallback userCallback;
    /* the length of data buffer */
    int32_t bufLen;
    struct OsalSem sem;
    unsigned char endPoint;
    unsigned int timeout;
    unsigned char requestType;
    void *bulkUrb;
    union {
        void *urbs;
        void **isoUrbs;
    };
    int32_t numUrbs;
    int32_t numRetired;
    UsbRequestStatus reqStatus;
    int32_t isoPacketOffset;
    struct OsalMutex lock;
    void *privateObj;
    int32_t numIsoPackets;
    /* isopacketdesc ptr */
    struct UsbIsoPacketDesc isoPacketDesc[0];
}__attribute__((aligned(4)));

/* Note:The first seven fields of this definition must be consistent with the UsbRawFillRequestData
  * structure definition.
  */
struct UsbFillRequestData {
    unsigned char endPoint;
    unsigned char *buffer;
    uint32_t length;
    int32_t numIsoPackets;
    UsbRawRequestCallback callback;
    void *userData;
    unsigned int timeout;
    UsbRequestCallback userCallback;
};

#ifndef MIN
static inline int32_t MIN(int32_t a, int32_t b)
{
    return ((a) < (b) ? (a) : (b));
}
#endif

#ifndef MAX
static inline int32_t MAX(int32_t a, int32_t b)
{
    return ((a) > (b) ? (a) : (b));
}
#endif

#endif /* USB_DDK_REQUEST_H */
