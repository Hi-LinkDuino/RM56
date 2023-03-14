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

#ifndef USB_LITEOS_DDK_USB_H
#define USB_LITEOS_DDK_USB_H

#include "linux_usb.h"
#include "usbdi.h"
#include "usbdi_util.h"
#include "usb_core.h"
#include "usb_busdma.h"
#include "usb_process.h"
#include "usb_raw_api_library.h"
#include "usb_transfer.h"
#include "usb_device.h"
#include "usb_util.h"
#include "usb_debug.h"
#include "usb_request.h"
#include "usb_dynamic.h"
#include "usb_hub.h"
#include "usb_ioctl.h"
#include "usb_controller.h"
#include "usb_bus.h"

#define UsbAdapterDeviceRequest         usb_device_request
#define UsbAdapterDeviceDescriptor      usb_device_descriptor
#define UsbAdapterHostEndpoint          usb_host_endpoint
#define UsbAdapterDevice                usb_device
#define UsbAdapterIsoPacketDescriptor   usb_iso_packet_descriptor
#define UsbAdapterUrb                   urb
#define UsbAdapterSpinLock              SPIN_LOCK_S

#endif  /* USB_LITEOS_DDK_USB_H */
