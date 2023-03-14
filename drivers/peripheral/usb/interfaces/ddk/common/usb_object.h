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
 * @file usb_object.h
 *
 * @brief Defines USB common data types, including the enumerated values returned by
 * functions and definitions of other common data structures.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef USB_OBJECT_H
#define USB_OBJECT_H

#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <sys/time.h>
#include "securec.h"
#include "hdf_base.h"
#include "hdf_slist.h"
#include "hdf_dlist.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "osal_sem.h"
#include "osal_thread.h"
#include "osal_time.h"
#include "osal_atomic.h"

/**
 * @brief Defines the maximum object ID.
 */
#define MAX_OBJECT_ID       (0x7FFFFFFF)

/**
 * @brief Defines the USB request status type.
 *
 * You can use this function to check the request execution status.
 */
typedef enum {
    /** Request completed */
    USB_REQUEST_COMPLETED,
    /** Request completed beforehand */
    USB_REQUEST_COMPLETED_SHORT,
    /** Request error */
    USB_REQUEST_ERROR,
    /** Request timeout */
    USB_REQUEST_TIMEOUT,
    /** Request canceled */
    USB_REQUEST_CANCELLED,
    /** Request stopped */
    USB_REQUEST_STALL,
    /** Device disconnected */
    USB_REQUEST_NO_DEVICE,
    /** Overflow error. The amount of sent data is more than requested. */
    USB_REQUEST_OVERFLOW,
} UsbRequestStatus;

/**
 * @brief Defines the USB pipe direction.
 */
typedef enum {
    /** Output direction (from the host to the device) */
    USB_PIPE_DIRECTION_OUT = 0x00,
    /** Input direction (from the device to the host) */
    USB_PIPE_DIRECTION_IN = 0x80,
} UsbPipeDirection;

/**
 * @brief Defines the USB pipe type.
 */
typedef enum {
    /** Control transfer */
    USB_PIPE_TYPE_CONTROL = 0U,
    /**
     * Isochronous transfer is mainly used for transmitting time-dependent information, such as
     * audio and video data,at a constant rate. Each isochronous transfer involves one or multiple
     * isochronous transactions, each containing token packets and data packets but no handshake packets.
     * This transfer mode ensures the timeliness of transfer but does not guarantee the correctness
     * of data due to the absence of handshake.
     * Different from bulk transfer, isochronous transfer allows a certain bit error rate (BER)
     * under certain conditions, so as to ensure that
     *  audio and video data are transmitted on a real-time basis.
     */
    USB_PIPE_TYPE_ISOCHRONOUS = 1U,
    /** Bulk transfer */
    USB_PIPE_TYPE_BULK = 2U,
    /** Interrupt transfer */
    USB_PIPE_TYPE_INTERRUPT = 3U,
} UsbPipeType;

/**
 * @brief Defines the USB interface status.
 */
typedef enum {
    /** Normal */
    USB_INTERFACE_STATUS_NORMAL,
    /** Interface addition */
    USB_INTERFACE_STATUS_ADD,
    /** Interface removal */
    USB_INTERFACE_STATUS_REMOVE,
    /** Other status */
    USB_INTERFACE_STATUS_OTHER,
} UsbInterfaceStatus;

/**
 * @brief Defines the USB basic object, which is contained in the data structure provided for users.
 * It is used to mark an external object and create an object linked list.
 */
struct UsbObject {
    /** Basic object ID */
    int32_t objectId;
    /** Bidirectional linked list header */
    struct DListHead entry;
};

#endif /* USB_OBJECT_H */
/** @} */