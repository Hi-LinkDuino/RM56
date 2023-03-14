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

#ifndef USB_HOST_NO_SDK_SPEED_H
#define USB_HOST_NO_SDK_SPEED_H

#define USB_ADAPTER_URB_TYPE_ISO                0
#define USB_ADAPTER_URB_TYPE_INTERRUPT          1
#define USB_ADAPTER_URB_TYPE_CONTROL            2
#define USB_ADAPTER_URB_TYPE_BULK               3
#define USB_ADAPTER_URB_SHORT_NOT_OK            0x01
#define USB_ADAPTER_URB_ISO_ASAP                0x02
#define USB_ADAPTER_URB_BULK_CONTINUATION       0x04
#define USB_ADAPTER_URB_QUEUE_BULK              0x10
#define USB_ADAPTER_URB_ZERO_PACKET             0x40
#define USB_ADAPTER_CAP_ZERO_PACKET             0x01
#define USB_ADAPTER_CAP_BULK_CONTINUATION       0x02
#define USB_ADAPTER_CAP_NO_PACKET_SIZE_LIM      0x04
#define USB_ADAPTER_CAP_BULK_SCATTER_GATHER     0x08
#define USB_ADAPTER_CAP_REAP_AFTER_DISCONNECT   0x10

#define USBDEVFS_CLAIMINTERFACE    _IOR('U', 15, unsigned int)
#define USBDEVFS_RELEASEINTERFACE  _IOR('U', 16, unsigned int)
#define USBDEVFS_SETCONFIGURATION  _IOR('U', 5, unsigned int)
#define USBDEVFS_SETINTERFACE      _IOR('U', 4, struct UsbSetInterface)
#define USBDEVFS_ALLOC_STREAMS     _IOR('U', 28, struct UsbAdapterStreams)
#define USBDEVFS_FREE_STREAMS      _IOR('U', 29, struct UsbAdapterStreams)
#define USBDEVFS_GET_CAPABILITIES  _IOR('U', 26, __u32)
#define USBDEVFS_CLEAR_HALT        _IOR('U', 21, unsigned int)
#define USBDEVFS_RELEASEINTERFACE  _IOR('U', 16, unsigned int)
#define USBDEVFS_RESET             _IO('U', 20)
#define USBDEVFS_CONTROL           _IOWR('U', 0, struct UsbCtrltransfer)
#define USBDEVFS_DISCARDURB        _IO('U', 11)
#define USBDEVFS_REAPURB           _IOW('U', 12, void *)
#define USBDEVFS_SUBMITURB         _IOR('U', 10, struct UsbAdapterUrb)
#define USBDEVFS_REAPURBNDELAY     _IOW('U', 13, void *)

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
    union {
        int32_t numberOfPackets;
        unsigned int streamId;
    };
    int32_t error_count;
    unsigned int signr;
    void *userContext;
    struct UsbIsoPacketDesc isoFrameDesc[0];
};

struct UsbAdapterUrbs {
    struct UsbAdapterUrb *urb;
    int32_t inUse;
    int32_t urbNum;
};

#endif  /* USB_HOST_NO_SDK_SPEED_H */
