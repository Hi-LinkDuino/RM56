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

/**
 * @file usbfn_request.h
 *
 * @brief Declares APIs for managing USB data requests.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef USBFN_REQUEST_H
#define USBFN_REQUEST_H

#include "usb_object.h"

/**
 * @brief Defines the USB request type.
 */
typedef enum {
    /** Invalid request */
    USB_REQUEST_TYPE_INVALID,
    /** Write request */
    USB_REQUEST_TYPE_PIPE_WRITE,
    /** Read request */
    USB_REQUEST_TYPE_PIPE_READ,
} UsbFnRequestType;

/**
 * @brief Defines a USB request.
 */
struct UsbFnRequest {
    /** USB object */
    const struct UsbObject *obj;
    /** Linked list header provided for users */
    struct DListHead list;
    /** Pointer to the request data */
    void             *buf;
    /** Length of the request data */
    uint32_t          length;
    /** Request type */
    UsbFnRequestType type;
    /** Request status */
    UsbRequestStatus status;
    /** Number of bytes transferred to or from the buffer */
    uint32_t         actual;
    /** Callback for request completion */
    void (*complete)(uint8_t pipe, struct UsbFnRequest *req);
    /** Pointer to the context for the callback */
    void             *context;
};

/**
 * @brief Defines a USB interface handle.
 */
typedef void *UsbFnInterfaceHandle;

/**
 * @brief Applies for a control (EP0) request.
 *
 * You can use this function to apply for a control (EP0)
 * request based on the specified USB interface handle and length.
 *
 * @param handle Indicates the USB interface handle. It can be set to any <b>UsbFnInterfaceHandle</b>.
 * @param len Indicates the request data length. The maximum value is <b>2048</b>. An error will be
 * thrown if the passed value is greater than <b>2048</b>.
 * You are advised to set the value to the <b>bMaxPacketSize0</b> defined by the device descriptor.
 * The data length set by <b>bMaxPacketSize0</b> prevails if a greater value is specified.
 *
 * @return Returns the pointer to the <b>UsbFnRequest</b> if the operation is successful;
 * returns <b>NULL</b> otherwise.
 */
struct UsbFnRequest *UsbFnAllocCtrlRequest(UsbFnInterfaceHandle handle, uint32_t len);

/**
 * @brief Allocates a USB request.
 *
 * You can use this function to allocate a USB request based on the
 * specified USB interface handle, pipe ID, and length.
 *
 * @param handle Indicates the USB interface handle.
 * @param pipe Indicates the pipe ID. The value ranges from 0 to the total number of pipes on the USB interface.
 * @param len Indicates the request data length. The maximum value is <b>2048</b>. An error will be thrown
 * if the passed value is greater than <b>2048</b>.
 * You are advised to set the value to the <b>wMaxPacketSize</b> defined by the endpoint descriptor.
 * The data length set by <b>wMaxPacketSize</b> prevails if a greater value is specified.
 *
 * @return Returns the pointer to the <b>UsbFnRequest</b> if the operation is successful;
 * returns <b>NULL</b> otherwise.
 */
struct UsbFnRequest *UsbFnAllocRequest(UsbFnInterfaceHandle handle, uint8_t pipe, uint32_t len);

/**
 * @brief Releases a specified USB request.
 *
 * @param req Indicates the pointer to the USB request.
 *
 * @return Returns <b>0</b> if the operation is successful;
 * returns a negative value defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnFreeRequest(struct UsbFnRequest *req);

/**
 * Obtains the status of a specified USB request.
 *
 * @param req Indicates the pointer to the USB request.
 * @param status Indicates the pointer to the status of the USB request.
 *
 * @return Returns <b>0</b> if the operation is successful;
 * returns a negative value defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnGetRequestStatus(struct UsbFnRequest *req, UsbRequestStatus *status);

/**
 * @brief Sends a non-isochronous USB request based on the passed request.
 *
 * @param req Indicates the pointer to the USB request.
 *
 * @return Returns <b>0</b> if the operation is successful;
 * returns a negative value defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnSubmitRequestAsync(struct UsbFnRequest *req);

/**
 * @brief Cancels a USB request based on the passed request.
 *
 * 
 *
 * @param req Indicates the pointer to the USB request.
 *
 * @return Returns <b>0</b> if the operation is successful;
 * returns a negative value defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnCancelRequest(struct UsbFnRequest *req);

/**
 * @brief Sends an isochronous USB request with a timeout interval based on the passed request.
 *
 * @param req Indicates the pointer to the USB request.
 * @param timeout Indicates the timeout interval for canceling a USB request.
 * The value <b>0</b> indicates that the system waits until the USB request is complete.
 *
 * @return Returns <b>0</b> if the operation is successful;
 * returns a negative value defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnSubmitRequestSync(struct UsbFnRequest *req, uint32_t timeout);

#endif /* USBFN_REQUEST_H */
