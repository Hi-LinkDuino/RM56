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
 * @file usb_raw_api.h
 *
 * @brief Defines the data types and interface functions provided by the USB driver development kit (DDK) in
 * expert mode.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef USB_RAW_API_H
#define USB_RAW_API_H

#include "usb_ddk.h"
#include "usb_session.h"

/**
 * @brief Defines the maximum number of USB interfaces.
 */
#define USB_MAXINTERFACES   32

/**
 * @brief Defines a pointer to the USB device handle in expert mode.
 */
typedef void *UsbRawDevice;

/**
 * @brief Defines a pointer to the USB device operation handle in expert mode.
 */
typedef void *UsbRawHandle;

/**
 * @brief Defines a pointer to the callback called when a user request in expert mode is complete. This callback
 * function is used to fill the <b>#UsbRawFillRequestData</b> object.
 */
typedef void (*UsbRawRequestCallback)(const void *requestArg);

/**
 * @brief Defines a control request object.
 */
struct UsbControlRequestData {
    /** Control request type */
    uint8_t requestType;
    /** Request command */
    uint8_t requestCmd;
    /** Value set based on the request */
    uint16_t value;
    /** Index set based on the request to identify an endpoint or interface */
    uint16_t index;
    /** Length of the transmitted data */
    uint16_t length;
    /** Timeout interval of the control request */
    unsigned int timeout;
    /** Pointer to the transmitted data */
    unsigned char *data;
};

/**
 * @brief Defines request parameters for filling the <b>UsbRawSendBulkRequest</b> or <b>UsbRawSendInterruptRequest</b>
 * function.
 * Request data to be sent
 */
struct UsbRequestData {
    /** Address of the endpoint that sends the request */
    unsigned char endPoint;
    /** Pointer to the request data */
    unsigned char *data;
    /** Length of the request data */
    uint32_t length;
    /** Pointer to the transmitted bytes of the request */
    int32_t *requested;
    /** Request timeout interval */
    unsigned int timeout;
};

/**
 * @brief Defines descriptor parameters in expert mode.
 */
struct UsbRawDescriptorParam {
    /** Descriptor type */
    uint8_t descType;
    /** Descriptor index */
    uint8_t descIndex;
    /** Length of the data to read */
    int32_t length;
};

/**
 * @brief Defines a request object in expert mode.
 */
struct UsbRawRequest {
    /** Pointer to the data in the buffer */
    unsigned char *buffer;
    /** Length of the user data */
    int32_t length;
    /** Actual length of the data sent at request completion */
    int32_t actualLength;
    /** Request status. For details, see {@link UsbRequestStatus}. */
    UsbRequestStatus status;
    /** Pointer to the user data */
    void *userData;
};

/**
 * @brief Defines a request data object in expert mode.
 */
struct UsbRawFillRequestData {
    /** Endpoint of the request data */
    unsigned char endPoint;
    /** Pointer to the request data buffer */
    unsigned char *buffer;
    /** Length of the request data */
    uint32_t length;
    /** Number of transmitted data packets in isochronous transfer */
    int32_t numIsoPackets;
    /** Callback function for request completion on the user side. For details, see {@link UsbRawRequestCallback}. */
    UsbRawRequestCallback callback;
    /** Pointer to the user data */
    void *userData;
    /** Request timeout interval */
    unsigned int timeout;
};

/**
 * @brief Defines the standard USB endpoint descriptor.
 */
struct UsbRawEndpointDescriptor {
    /** Standard USB endpoint descriptor */
    struct UsbEndpointDescriptor endpointDescriptor;
    /** Pointer to the extra descriptor */
    const unsigned char *extra;
    /** Length of the extra descriptor, in bytes. The value must be a non-negative number. */
    int32_t extraLength;
};

/**
 * @brief Defines the standard USB interface descriptor.
 */
struct UsbRawInterfaceDescriptor {
    /** Standard USB interface descriptor */
    struct UsbInterfaceDescriptor interfaceDescriptor;
    /** Pointer to the endpoint descriptor array */
    const struct UsbRawEndpointDescriptor *endPoint;
    /** Pointer to the extra descriptor */
    const unsigned char *extra;
    /** Length of the extra descriptor, in bytes. The value must be a non-negative number. */
    int32_t extraLength;
};

/**
 * @brief Defines alternate settings for a particular USB interface.
 */
struct UsbRawInterface {
    /** Number of alternate settings that belong to the interface. The value must be a non-negative number. */
    uint8_t numAltsetting;
    /** Interface descriptor array. Its length is determined by the numAltsetting field. */
    const struct UsbRawInterfaceDescriptor altsetting[];
};

/**
 * @brief Defines the standard USB configuration descriptor.
 */
struct UsbRawConfigDescriptor {
    /** Standard USB configuration descriptor */
    struct UsbConfigDescriptor configDescriptor;
    /** Pointer to the interface array supported by the configuration. The maximum number of interfaces is determined
    * by USB_MAXINTERFACES. */
    const struct UsbRawInterface *interface[USB_MAXINTERFACES];
    /** Pointer to the extra descriptor */
    const unsigned char *extra;
    /** Length of the extra descriptor, in bytes. The value must be a non-negative number. */
    int32_t extraLength;
};

/**
 * @brief Initializes the USB DDK in expert mode.
 *
 * You can use this function to allocate and initialize resources.
 *
 * @param session Indicates the pointer to the session context. It can be set to <b>NULL</b> or a value defined in
 * {@link UsbSession}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawInit(struct UsbSession **session);

/**
 * @brief Exits the expert mode of the USB DDK.
 *
 * You can use this function to release occupied resources.
 *
 * @param session Indicates the pointer to the session context. It can be set to <b>NULL</b> or a value defined in
 * {@link UsbSession}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawExit(const struct UsbSession *session);

/**
 * @brief Opens a USB device object.
 *
 * @param session Indicates the pointer to the session context. It can be set to <b>NULL</b> or a value defined in
 * {@link UsbSession}.
 * @param busNum Indicates the USB device bus number.
 * @param usbAddr Indicates the USB device address.
 *
 * @return Returns the pointer to the <b>UsbRawHandle</b> if the operation is successful; returns <b>NULL</b>
 * otherwise. 
 */
UsbRawHandle *UsbRawOpenDevice(const struct UsbSession *session, uint8_t busNum, uint8_t usbAddr);

/**
 * @brief Closes a USB device object.
 *
 * @param devHandle Indicates the pointer to the device handle.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawCloseDevice(const UsbRawHandle *devHandle);

/**
 * @brief Performs control transfer.
 *
 * @param request Indicates the pointer to the request to send.
 * @param devHandle Indicates the pointer to the device handle.
 * @param requestData Indicates the pointer to the request data to send.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawSendControlRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbControlRequestData *requestData);

/**
 * @brief Performs bulk transfer.
 *
 * @param request Indicates the pointer to the request to send.
 * @param devHandle Indicates the pointer to the device handle.
 * @param requestData Indicates the pointer to the request data to send.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawSendBulkRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRequestData *requestData);
/**
 * @brief Performs interrupt transfer.
 *
 * @param request Indicates the pointer to the request to send.
 * @param devHandle Indicates the pointer to the device handle.
 * @param requestData Indicates the pointer to the request data to send.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawSendInterruptRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRequestData *requestData);

/**
 * @brief Obtains the device configuration descriptor based on a specified device ID.
 *
 * @param rawDev Indicates the pointer to the USB raw device.
 * @param configIndex Indicates the ID of the device configuration descriptor.
 * @param config Indicates the double pointer to the device configuration descriptor.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawGetConfigDescriptor(
    const UsbRawDevice *rawDev, uint8_t configIndex, struct UsbRawConfigDescriptor **config);

/**
 * @brief Releases the memory space of the device configuration descriptor.
 *
 * @param config Indicates the pointer to the device configuration descriptor.
 *
 */
void UsbRawFreeConfigDescriptor(const struct UsbRawConfigDescriptor *config);

/**
 * @brief Obtains the active device configuration.
 *
 * @param devHandle Indicates the pointer to the device handle.
 * @param config Indicates the pointer to the device configuration descriptor.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawGetConfiguration(const UsbRawHandle *devHandle, int32_t *config);

/**
 * @brief Sets the active device configuration.
 *
 * @param devHandle Indicates the pointer to the device handle.
 * @param config Indicates the device configuration descriptor.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawSetConfiguration(const UsbRawHandle *devHandle, int32_t config);

/**
 * @brief Obtains descriptor information.
 *
 * @param request Indicates the pointer to the request to send.
 * @param devHandle Indicates the pointer to the device handle.
 * @param param Indicates the pointer to the descriptor parameter. For details, see {@link UsbRawDescriptorParam}.
 * @param data Indicates the pointer to the descriptor address.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawGetDescriptor(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRawDescriptorParam *param, const unsigned char *data);

/**
 * @brief Obtains the device pointer based on a specified device handle.
 *
 * @param devHandle Indicates the pointer to the device handle.
 *
 * @return Returns the device pointer if any; returns <b>NULL</b> otherwise. For details, see {@link UsbRawDevice}.
 */
UsbRawDevice *UsbRawGetDevice(const UsbRawHandle *devHandle);

/**
 * @brief Obtains the USB device descriptor of a specified device.
 *
 * @param rawDev Indicates the pointer to the USB raw device.
 * @param desc Indicates the pointer to the device descriptor.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawGetDeviceDescriptor(const UsbRawDevice *rawDev, struct UsbDeviceDescriptor *desc);

/**
 * @brief Declares the interface on the given device handle.
 *
 * @param devHandle Indicates the pointer to the device handle of the interface to declare.
 * @param interfaceNumber Indicates the number of the interface to declare.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawClaimInterface(const UsbRawHandle *devHandle, int32_t interfaceNumber);

/**
 * @brief Releases the previously declared interface.
 *
 * @param devHandle Indicates the pointer to the device handle of the interface to release.
 * @param interfaceNumber Indicates the number of the interface to release.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawReleaseInterface(const UsbRawHandle *devHandle, int32_t interfaceNumber);

/**
 * @brief Resets a device.
 *
 * @param devHandle Indicates the pointer to the device handle.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawResetDevice(const UsbRawHandle *devHandle);

/**
 * @brief Allocates a transfer request with a specified number of isochronous transfer packet descriptors.
 *
 * For details about isochronous transfer, see {@link UsbPipeType}.
 *
 * @param devHandle Indicates the pointer to the device handle.
 * @param isoPackets Indicates the number of isochronous transfer packet descriptors to allocate. The value must be a
 * non-negative number.
 * @param length Indicates the size of the user space to allocate.
 *
 * @return Returns the pointer to the <b>UsbHostRequest</b> structure if the operation is successful; returns
 * <b>NULL</b> otherwise. 
 */
struct UsbRawRequest *UsbRawAllocRequest(const UsbRawHandle *devHandle, int32_t isoPackets, int32_t length);

/**
 * @brief Releases the previously allocated transfer request.
 *
 * @param request Indicates the pointer to the transfer request to release.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawFreeRequest(const struct UsbRawRequest *request);

/**
 * @brief Fills required information in a bulk transfer request.
 *
 * @param request Indicates the pointer to the request to send.
 * @param devHandle Indicates the pointer to the device handle.
 * @param fillRequestData Indicates the pointer to the request data to fill.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawFillBulkRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRawFillRequestData *fillData);

/**
 * @brief Fills required information in control transfer configuration packets.
 *
 * @param setup Indicates the pointer to the control information.
 * @param requestData Indicates the pointer to the request data to fill.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawFillControlSetup(const unsigned char *setup, const struct UsbControlRequestData *requestData);

/**
 * @brief Fills required information in a control transfer request.
 *
 * @param request Indicates the pointer to the request to send.
 * @param devHandle Indicates the pointer to the device handle.
 * @param fillRequestData Indicates the pointer to the request data to fill.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawFillControlRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRawFillRequestData *fillData);

/**
 * @brief Fills required information in an interrupt transfer request.
 *
 * @param request Indicates the pointer to the request to send.
 * @param devHandle Indicates the pointer to the device handle.
 * @param fillRequestData Indicates the pointer to the request data to fill.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawFillInterruptRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRawFillRequestData *fillData);

/**
 * @brief Fills required information in an isochronous transfer request.
 *
 * @param request Indicates the pointer to the request to send.
 * @param devHandle Indicates the pointer to the device handle.
 * @param fillRequestData Indicates the pointer to the request data to fill.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawFillIsoRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRawFillRequestData *fillData);

/**
 * @brief Submits a transfer request.
 *
 * @param request Indicates the pointer to the request to submit.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawSubmitRequest(const struct UsbRawRequest *request);

/**
 * @brief Cancels a transfer request.
 *
 * @param request Indicates the pointer to the request to cancel.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawCancelRequest(const struct UsbRawRequest *request);

/**
 * @brief Defines the handle for a transfer request event.
 *
 * @param devHandle Indicates the pointer to the device handle.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbRawHandleRequests(const UsbRawHandle *devHandle);

#endif /* USB_RAW_API_H */
/** @} */
