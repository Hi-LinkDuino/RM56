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

#ifndef USB_PROTOCOL_H
#define USB_PROTOCOL_H

#include "usb_ddk_device.h"
#include "usb_ddk_interface.h"

#define USB_DEFAULT_ALTSETTING  0x0

int32_t UsbProtocalParseDescriptor(struct UsbDeviceHandle *devHandle, uint8_t busNum, uint8_t devAddr);
int32_t UsbProtocalFillControlSetup(const unsigned char *setup, const struct UsbControlRequest *ctrlReq);

#endif  /* HDF_USB_SERIAL_RAWAPI_H */
