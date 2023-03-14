/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup USB
 * @{
 *
 * @brief Declares USB-related APIs, including the custom data types and functions used to obtain descriptors, 
 * interface objects, and request objects, and to submit requests.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file usb_session.h
 *
 * @brief Defines the session context data.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef USB_SESSION_H
#define USB_SESSION_H

#include "usb_object.h"

/**
 * @brief Defines a session context object.
 *
 * A session context object allows a program to use the driver development kit (DDK) independently. This can prevent
 * interference between users of a single DDK. Such a session can be initiated by using <b>UsbInitHostSdk()</b> or
 * <b>UsbRawInit()</b> and destroyed by using <b>UsbExitHostSdk()</b> or <b>UsbRawExit()</b>.
 * If a program has only one user, you can ignore the session context. You just need to pass <b>NULL</b> in each
 * function call that requires context, and the default session context will be used.
 */
struct UsbSession {
    /** Device linked list */
    struct HdfSList usbDevs;
    /** Interface object pool linked list */
    struct DListHead ifacePoolList;
    /** Mutex used for operations on the device linked list and interface object pool linked list */
    struct OsalMutex lock;
    /** Number of references to session context objects */
    OsalAtomic refCount;
};

#endif /* USB_SESSION_H */
/** @} */