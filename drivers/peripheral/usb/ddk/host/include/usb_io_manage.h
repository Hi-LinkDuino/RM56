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

#ifndef USB_IO_MANAGE_H
#define USB_IO_MANAGE_H

#include "usb_session.h"
#include "usb_ddk_device.h"
#include "usb_ddk_request.h"
#include "usb_interface_pool.h"

#define USB_IO_SEND_PROCESS_STACK_SIZE      10000
#define USB_IO_RECEIVE_PROCESS_STACK_SIZE   10000
#define USB_IO_SUBMIT_RETRY_TIME_CNT        3
#define USB_IO_SLEEP_MS_TIME                100
#define USB_IO_STOP_WAIT_MAX_TIME           3

HDF_STATUS UsbIoCreateQueue(const struct UsbInterfacePool *interfacePool);
HDF_STATUS UsbIoDestroyQueue(const struct UsbInterfacePool *interfacePool);
int32_t UsbIoSendRequest(const struct UsbMessageQueue *msgQueue, const struct UsbHostRequest *request);
HDF_STATUS UsbIoGetRequest(const struct UsbMessageQueue *msgQueue, struct UsbHostRequest **request);
HDF_STATUS UsbIoStart(struct UsbInterfacePool *interfacePool);
HDF_STATUS UsbIoStop(struct UsbInterfacePool *interfacePool);
void UsbIoSetRequestCompletionInfo(const void *requestArg);

#endif /* USB_IO_MANAGE_H */
