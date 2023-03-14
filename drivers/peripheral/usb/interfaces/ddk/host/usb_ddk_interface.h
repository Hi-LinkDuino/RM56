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
 *  interface objects, and request objects, and to submit requests.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file usb_ddk_interface.h
 *
 * @brief Declares the data types and interface functions provided by the host of the USB driver development kit (DDK).
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef USB_INTERFACE_H
#define USB_INTERFACE_H

#include "usb_session.h"

/**
 * @brief Defines the default ID of the USB control interface.
 */
#define USB_CTRL_INTERFACE_ID   0xFF

/**
 * @brief Defines the request object type.
 *
 * It will be filled in the corresponding field in {@link UsbControlRequest}.
 */
typedef enum {
    /** USB device object */
    USB_REQUEST_TARGET_DEVICE,
    /** Interface object */
    USB_REQUEST_TARGET_INTERFACE,
    /** Endpoint object */
    USB_REQUEST_TARGET_ENDPOINT,
    /** Other objects */
    USB_REQUEST_TARGET_OTHER,
} UsbRequestTargetType;

/**
 * @brief Defines the control request type.
 *
 * It will be filled in the corresponding field in {@link UsbControlRequest}.
 */
typedef enum {
    /** Standard request */
    USB_REQUEST_TYPE_STANDARD,
    /** Class request */
    USB_REQUEST_TYPE_CLASS,
    /** Vendor request */
    USB_REQUEST_TYPE_VENDOR,
} UsbControlRequestType;

/**
 * @brief Defines the request data direction.
 *
 * It will be filled in the corresponding field in {@link UsbControlRequest} or {@link UsbRequestParamsData}.
 */
typedef enum {
    /** Data transfer from the host to the device */
    USB_REQUEST_DIR_TO_DEVICE,
    /** Data transfer from the device to the host */
    USB_REQUEST_DIR_FROM_DEVICE,
} UsbRequestDirection;

/**
 * @brief Defines the request parameter type.
 *
 * It will be filled in the corresponding field in {@link UsbRequestParams} to indicate whether the request parameter
 * is of the control type or data type.
 */
typedef enum {
    /** Control type */
    USB_REQUEST_PARAMS_CTRL_TYPE,
    /** Data type */
    USB_REQUEST_PARAMS_DATA_TYPE,
} UsbRequestParamsType;

/**
 * @brief Defines the USB request type.
 */
typedef enum {
    /** Invalid request */
    USB_REQUEST_TYPE_INVALID,
    /** Control request */
    USB_REQUEST_TYPE_DEVICE_CONTROL,
    /** Write request */
    USB_REQUEST_TYPE_PIPE_WRITE,
    /** Read request */
    USB_REQUEST_TYPE_PIPE_READ,
} UsbRequestPipeType;

/**
 * @brief Defines a USB interface operation handle.
 */
typedef void *UsbInterfaceHandle;

/**
 * @brief Defines a USB pipe information object.
 */
struct UsbPipeInfo {
    /** Interface ID of the current pipe */
    uint8_t interfaceId;
    /** Pipe ID, which is used to search the specified pipe */
    uint8_t pipeId;
    /** Pipe address, which is used to fill the I/O request through {@link UsbRequestFill} */
    uint8_t pipeAddress;
    /** Pipe type */
    UsbPipeType pipeType;
    /** Pipe direction */
    UsbPipeDirection pipeDirection;
    /** Maximum size of packets received and sent over the pipe */
    uint16_t maxPacketSize;
    /** Interval for the host to query the pipe */
    uint8_t interval;
};

/**
 * @brief Defines a USB pipe object.
 */
struct UsbPipe {
    /** USB basic object */
    struct UsbObject object;
    /** USB pipe information. For details, see {@link UsbPipeInfo}. */
    struct UsbPipeInfo info;
};

/**
 * @brief Defines a USB interface information object.
 */
struct UsbInterfaceInfo {
    /** Interface index */
    uint8_t interfaceIndex;
    /** Index of the enabled interface */
    uint8_t altSettings;
    /** Index of the current enabled interface */
    uint8_t curAltSetting;
    /** Pipe quantity */
    uint8_t pipeNum;
    /** Interface class */
    uint8_t interfaceClass;
    /** Interface subclass */
    uint8_t interfaceSubClass;
    /** Interface protocol */
    uint8_t interfaceProtocol;
};

/**
 * @brief Defines a USB interface object.
 */
struct UsbInterface {
    /** USB basic object */
    struct UsbObject object;
    /** USB interface information. For details, see {@link UsbInterfaceInfo}. */
    struct UsbInterfaceInfo info;
};

/**
 * @brief Defines USB request completion information.
 */
struct UsbRequestCompInfo {
    /** Request pipe type. For details, see {@link UsbRequestPipeType}. */
    UsbRequestPipeType type;
    /** Pointer to the user data buffer */
    unsigned char *buffer;
    /** The address of data buffer */
    uint32_t length;
    /** Actual length of the data sent at request completion */
    uint32_t actualLength;
    /** Request status. For details, see {@link UsbRequestStatus}. */
    UsbRequestStatus status;
    /** Pointer to the user data */
    void *userData;
};

/**
 * @brief Defines a USB request.
 *
 * There are two types of request objects: <b>UsbObject</b> (USB basic object) and <b>UsbRequestCompInfo</b>
 * (USB request completion information ). You can query the current request execution status through the USB request
 * completion information.
 * The isochronous (block) request and non-isochronous (non-block) request share this data structure but use different
 * application interfaces. The isochronous (block) request uses the <b>UsbSubmitRequestSync</b> interface,
 * whereas the non-isochronous (non-block) request uses the <b>UsbSubmitRequestAsync</b> interface.
 */
struct UsbRequest {
    /** USB basic object */
    struct UsbObject object;
    /** USB request completion information. For details, see {@link UsbRequestCompInfo}. */
    struct UsbRequestCompInfo compInfo;
}__attribute__((aligned(4)));

/**
 * @brief Defines the callback function for completion of a user request. It will be used to fill the
 * {@link UsbRequestParams} object.
 */
typedef void (*UsbRequestCallback)(struct UsbRequest *request);

/**
 * @brief Defines a control request object, which will be used to fill in the {@link UsbRequestParams} object.
 */
struct UsbControlRequest {
    /** Type of the received control request packet. For details, see {@link UsbRequestTargetType}. */
    UsbRequestTargetType target;
    /** Control request type. For details, see {@link UsbControlRequestType}. */
    UsbControlRequestType reqType;
    /** Request data direction. For details, see {@link UsbRequestDirection}. */
    UsbRequestDirection directon;
    /** Request command */
    uint8_t request;
    /** Value set based on the request */
    uint16_t value;
    /** Index set based on the request to identify an endpoint or interface */
    uint16_t index;
    /** Pointer to the transmitted data */
    void *buffer;
    /** Length of the transmitted data */
    uint32_t length;
};

/**
 * @brief Defines data request parameters, which will be used to fill the {@link UsbRequestParams} object.
 */
struct UsbRequestParamsData {
    /** Number of transmitted data packets in isochronous transfer */
    int32_t numIsoPackets;
    /** Request data direction. For details, see {@link UsbRequestDirection}. */
    UsbRequestDirection directon;
    /** Pointer to the request data */
    unsigned char *buffer;
    /** Length of the request data */
    uint32_t length;
};

/**
 * @brief Defines a USB request parameter object, which can be {@link UsbControlRequest} (control request parameter)
 * or {@link UsbRequestParamsData} (data request parameter).
 */
struct UsbRequestParams {
    /** Interface ID */
    uint8_t interfaceId;
    /** Pipe ID. For details, see {@link UsbPipeInfo}. */
    uint8_t pipeId;
    /** Pipe address. For details, see {@link UsbPipeInfo}. */
    uint8_t pipeAddress;
    /** Callback function for request completion on the user side. For details, see {@link UsbRequestCallback}. */
    UsbRequestCallback callback;
    /** Pointer to the user data */
    void *userData;
    /** Request timeout interval */
    unsigned int timeout;
    /** Request parameter type */
    UsbRequestParamsType requestType;
    /** Request parameter */
    union {
        /** Control request parameter. For details, see {@link UsbControlRequest}. */
        struct UsbControlRequest ctrlReq;
        /** Data request parameter. For details, see {@link UsbRequestParamsData}. */
        struct UsbRequestParamsData dataReq;
    };
};

/**
 * @brief Initializes the USB DDK on the host side.
 *
 * You can use this function to allocate and initialize resources.
 *
 * @param session Indicates the double pointer to the session context. It can be set to <b>NULL</b> or a value defined
 * in {@link UsbSession}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbInitHostSdk(struct UsbSession **session);

/**
 * @brief Exits the USB DDK on the host side.
 *
 * You can use this function to release occupied resources.
 *
 * @param session Indicates the pointer to the session context. It can be set to <b>NULL</b> or a value defined in
 * {@link UsbSession}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbExitHostSdk(const struct UsbSession *session);

/**
 * @brief Obtains a USB interface object based on a specified interface index in unforce state.
 *
 * @param session Indicates the pointer to the session context. It can be set to <b>NULL</b> or a value defined in
 * {@link UsbSession}.
 * @param busNum Indicates the USB device bus number.
 * @param usbAddr Indicates the USB device address.
 * @param interfaceIndex Indicates the index of the interface object to be obtained. This parameter is defined in the
 * <b>UsbInterfaceInfo</b> structure.
 * The default index of the control interface is <b>0xFF</b>.
 *
 * @return Returns the pointer to the <b>UsbInterface</b> structure if the operation is successful; returns <b>NULL</b>
 * otherwise.
 */
struct UsbInterface *UsbClaimInterfaceUnforce(const struct UsbSession *session, uint8_t busNum,
    uint8_t usbAddr, uint8_t interfaceIndex);

/**
 * @brief Obtains a USB interface object based on a specified interface index in force state.
 *
 * @param session Indicates the pointer to the session context. It can be set to <b>NULL</b> or a value defined in
 * {@link UsbSession}.
 * @param busNum Indicates the USB device bus number.
 * @param usbAddr Indicates the USB device address.
 * @param interfaceIndex Indicates the index of the interface object to be obtained. This parameter is defined in the
 * <b>UsbInterfaceInfo</b> structure.
 * The default index of the control interface is <b>0xFF</b>.
 *
 * @return Returns the pointer to the <b>UsbInterface</b> structure if the operation is successful; returns <b>NULL</b>
 * otherwise.
 */
struct UsbInterface *UsbClaimInterface(const struct UsbSession *session, uint8_t busNum,
    uint8_t usbAddr, uint8_t interfaceIndex);

/**
 * @brief Releases a USB interface object.
 *
 * @param interfaceObj Indicates the pointer to the USB interface object to release.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbReleaseInterface(const struct UsbInterface *interfaceObj);

/**
 * @brief Adds or removes an interface.
 *
 * @param status Indicates the interface operation status.
 * @param interfaceObj Indicates the pointer to the USB interface object to add or remove.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbAddOrRemoveInterface(const struct UsbSession *session, uint8_t busNum, uint8_t usbAddr,
    uint8_t interfaceIndex, UsbInterfaceStatus status);

/**
 * @brief Opens a USB interface object.
 *
 * @param interfaceObj Indicates the pointer to the USB interface object to open.
 *
 * @return Returns the pointer to the <b>UsbInterfaceHandle</b> if the operation is successful; returns <b>NULL</b>
 * otherwise.
 */
UsbInterfaceHandle *UsbOpenInterface(const struct UsbInterface *interfaceObj);

/**
 * @brief Closes a USB interface object.
 *
 * @param interfaceHandle Indicates the pointer to the handle of the USB interface object to close.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbCloseInterface(const UsbInterfaceHandle *interfaceHandle);

/**
 * @brief Sets the optional configuration.
 *
 * @param interfaceHandle Indicates the pointer to the USB interface handle.
 * @param settingIndex Indicates the index of the optional configuration.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbSelectInterfaceSetting(const UsbInterfaceHandle *interfaceHandle, uint8_t settingIndex,
    struct UsbInterface **interfaceObj);

/**
 * @brief Obtains pipe information from the specified optional configuration.
 *
 * @param interfaceHandle Indicates the pointer to the USB interface handle.
 * @param settingIndex Indicates the index of the optional configuration.
 * @param pipeIndex Indicates the pipe index.
 * @param pipeInfo Indicates the pointer to the obtained pipe information.
 *
 */
int32_t UsbGetPipeInfo(const UsbInterfaceHandle *interfaceHandle, uint8_t settingIndex,
    uint8_t pipeId, struct UsbPipeInfo *pipeInfo);

/**
 * @brief Clears the status of a specified pipe.
 *
 * @param interfaceHandle Indicates the pointer to the USB interface handle.
 * @param pipeAddress Indicates the pipe address.
 *
 */
int32_t UsbClearInterfaceHalt(const UsbInterfaceHandle *interfaceHandle, uint8_t pipeAddress);

/**
 * @brief Allocates a USB request.
 *
 * I/O requests are allocated based on the size of the user space. If they are used to send isochronous transfer
 * packets, extra space will be allocated.
 *
 * @param interfaceHandle Indicates the pointer to the USB interface handle.
 * @param isoPackets Indicates the number of isochronous transfer packets. For details, see {@link UsbPipeType}.
 * @param length Indicates the size of the user space to allocate.
 *
 * @return Returns the pointer to the <b>UsbRequest</b> structure if the operation is successful; returns <b>NULL</b>
 * otherwise.
 */
struct UsbRequest *UsbAllocRequest(const UsbInterfaceHandle *interfaceHandle, int32_t isoPackets, int32_t length);

/**
 * @brief Releases a request object.
 *
 * @param request Indicates the pointer to the USB request.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbFreeRequest(const struct UsbRequest *request);

/**
 * @brief Sends an asynchronous request.
 *
 * @param request Indicates the pointer to the USB request.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbSubmitRequestAsync(const struct UsbRequest *request);

/**
 * @brief Fills an I/O request based on specified parameters.
 *
 * @param request Indicates the pointer to the USB request.
 * @param interfaceHandle Indicates the pointer to the USB interface handle.
 * @param params Indicates the pointer to a list of parameters to fill. For details, see {@link UsbRequestParams}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbFillRequest(const struct UsbRequest *request, const UsbInterfaceHandle *interfaceHandle,
    const struct UsbRequestParams *params);

/**
 * @brief Cancels an asynchronous USB request.
 
 * @param request Indicates the pointer to the USB request.
 *
 * @return Returns <b>UsbInterfaceHandle</b> if the operation is successful; returns <b>NULL</b> otherwise.
 */
int32_t UsbCancelRequest(const struct UsbRequest *request);

/**
 * @brief Sends a synchronous USB request.
 *
 * @param request Indicates the pointer to the USB request.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value defined in {@link HDF_STATUS}
 * otherwise.
 */
int32_t UsbSubmitRequestSync(const struct UsbRequest *request);
int32_t UsbMemTestTrigger(bool enable);

#endif /* USB_INTERFACE_H */
/** @} */