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

#ifndef USB_ERRORS_H
#define USB_ERRORS_H

#include <errors.h>

namespace OHOS {
namespace USB {
enum {
    /**
     *  Module type: Usb Manager Service
     */
    USB_MODULE_TYPE_SERVICE = 0,
    /**
     *  Module type: Usb Manager Kit
     */
    USB_MODULE_TYPE_KIT = 1
};

#define USB_OFFSET 100

// offset of usbmgr error, only be used in this file.
constexpr int32_t USBFWK_SERVICE_ERR_OFFSET = ErrCodeOffset(SUBSYS_USB, USB_MODULE_TYPE_SERVICE);

enum {
    UEC_OK = 0,

    UEC_INTERFACE_NO_MEMORY = USBFWK_SERVICE_ERR_OFFSET,
    UEC_INTERFACE_INVALID_OPERATION,
    UEC_INTERFACE_INVALID_VALUE,
    UEC_INTERFACE_NAME_NOT_FOUND,
    UEC_INTERFACE_PERMISSION_DENIED,
    UEC_INTERFACE_NO_INIT,
    UEC_INTERFACE_ALREADY_EXISTS,
    UEC_INTERFACE_DEAD_OBJECT,
    UEC_INTERFACE_OVERFLOW,
    UEC_INTERFACE_ENOUGH_DATA,
    UEC_INTERFACE_WOULD_BLOCK,
    UEC_INTERFACE_TIMED_OUT,
    UEC_INTERFACE_WRITE_PARCEL_ERROR,
    UEC_INTERFACE_READ_PARCEL_ERROR,
    UEC_INTERFACE_GET_SYSTEM_ABILITY_MANAGER_FAILED,
    UEC_INTERFACE_GET_USB_SERVICE_FAILED,
    UEC_INTERFACE_ADD_DEATH_RECIPIENT_FAILED,
    UEC_INTERFACE_INNER_ERR,
    UEC_INTREFACE_END,

    UEC_SERVICE_NO_MEMORY = USBFWK_SERVICE_ERR_OFFSET + USB_OFFSET,
    UEC_SERVICE_INVALID_OPERATION,
    UEC_SERVICE_INVALID_VALUE,
    UEC_SERVICE_NAME_NOT_FOUND,
    UEC_SERVICE_PERMISSION_DENIED,
    UEC_SERVICE_NO_INIT,
    UEC_SERVICE_ALREADY_EXISTS,
    UEC_SERVICE_DEAD_OBJECT,
    UEC_SERVICE_OVERFLOW,
    UEC_SERVICE_ENOUGH_DATA,
    UEC_SERVICE_WOULD_BLOCK,
    UEC_SERVICE_TIMED_OUT,
    UEC_SERVICE_WRITE_PARCEL_ERROR,
    UEC_SERVICE_READ_PARCEL_ERROR,
    UEC_SERVICE_GET_SYSTEM_ABILITY_MANAGER_FAILED,
    UEC_SERVICE_GET_USB_SERVICE_FAILED,
    UEC_SERVICE_ADD_DEATH_RECIPIENT_FAILED,
    UEC_SERVICE_INNER_ERR,
    UEC_SERVICE_END,

    UEC_HDF_FAILURE = UEC_SERVICE_NO_MEMORY + USB_OFFSET, /**< Failed to invoke the OS underlying function. */
    UEC_HDF_ERR_NOT_SUPPORT,                              /**< Not supported. */
    UEC_HDF_ERR_INVALID_PARAM,                            /**< Invalid parameter. */
    UEC_HDF_ERR_INVALID_OBJECT,                           /**< Invalid object. */
    UEC_HDF_ERR_MALLOC_FAIL,                              /**< Memory allocation fails. */
    UEC_HDF_ERR_TIMEOUT,                                  /**< Timeout occurs. */
    UEC_HDF_ERR_THREAD_CREATE_FAIL,                       /**< Failed to create a thread. */
    UEC_HDF_ERR_QUEUE_FULL,                               /**< The queue is full. */
    UEC_HDF_ERR_DEVICE_BUSY,                              /**< The device is busy. */
    UEC_HDF_ERR_IO,                                       /**< I/O error. */
    UEC_HDF_ERR_BAD_FD                                    /**< Incorrect file descriptor. */
};
} // namespace USB
} // namespace OHOS
#endif // USB_ERRORS_H
