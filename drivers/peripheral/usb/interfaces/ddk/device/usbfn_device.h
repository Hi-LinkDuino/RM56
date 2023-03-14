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
 * @file usbfn_device.h
 *
 * @brief Declares the APIs for managing USB devices.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef USBFN_DEVICE_H
#define USBFN_DEVICE_H

#include "usb_object.h"
#include "device_resource_if.h"
#include "usb_ddk.h"
#include "usbfn_interface.h"

/**
 * @brief Defines a USB device object, which is obtained when a device is created.
 *
 * <b>UsbFnDevice</b> corresponds to the device object in the USB specifications.
 * Wherein, <b>object</b> is the identifier of the device,
 * <b>numInterfaces</b>indicates the number of interfaces.
 */
struct UsbFnDevice {
    /** USB device object */
    struct UsbObject object;
    /** Number of interfaces */
    uint8_t numInterfaces;
};

/**
 * @brief Defines the descriptor type.
 *
 * The descriptor type can be defined in the HCS or the driver code.
 */
typedef enum {
    /** Descriptor defined in the HCS */
    USBFN_DESC_DATA_TYPE_PROP,
    /** Descriptor defined in the driver */
    USBFN_DESC_DATA_TYPE_DESC,
} UsbFnDescDataType;

/**
 * @brief Defines a USB string.
 */
struct UsbString {
    /** String ID */
    uint8_t                     id;
    /** String encoded in UTF-8 format */
    const char                  *s;
};

/**
 * @brief Defines a USB string in a specific language.
 *
 * 
 * @see http://www.usb.org/developers/docs/USB_LANGIDs.pdf
 */
struct UsbFnStrings {
    /** Language of USB strings, for example, 0x0409 for en-us */
    uint16_t                    language;
    /** Pointer to USB strings */
    struct UsbString            *strings;
};

/**
 * @brief Defines a USB device.
 *
 * The <b>UsbFnFunction</b> structure contains multiple strings and descriptors.
 * It is used to describe a functional device, for example, a serial port or network adapter.
 */
struct UsbFnFunction {
    /** Pointer to the function driver name:
     * Naming format: f_generic.x acm.x ecm.x
     * Use the symbol dot (.) as the separator. <b>f_generic</b> is the common driver
     * capability provided by this API,
     * whereas <b>acm</b> or <b>ecm</b> is the function drive capability provided by the kernel.
     * A value does not need to be assigned to the descriptor.
     * You can simply use <b>UsbFnCreateDevice</b> to create a device.
     */
    bool                       enable;
    const char                 *funcName;
    /** Double pointer to USB strings in a specified language */
    struct UsbFnStrings        **strings;
    /** Double pointer to Full-Speed descriptors */
    struct UsbDescriptorHeader **fsDescriptors;
    /** Double pointer to High-Speed descriptors */
    struct UsbDescriptorHeader **hsDescriptors;
    /** Double pointer to SuperSpeed descriptors */
    struct UsbDescriptorHeader **ssDescriptors;
    /** Double pointer to SuperSpeed Plus descriptors */
    struct UsbDescriptorHeader **sspDescriptors;
};

/**
 * @brief Defines a USB configuration descriptor.
 */
struct UsbFnConfiguration {
    /** Configuration ID */
    uint8_t                     configurationValue;
    /** Configuration string index */
    uint8_t                     iConfiguration;
    /** Configuration attributes */
    uint8_t                     attributes;
    /** Maximum current */
    uint16_t                    maxPower;
    /** Double pointer to USB devices */
    struct UsbFnFunction        **functions;
};

/**
 * @brief Defines a USB device descriptor.
 */
struct UsbFnDeviceDesc {
    /** Pointer to the standard USB device descriptor */
    struct UsbDeviceDescriptor *deviceDesc;
    /** Double pointer to USB strings in a specified language */
    struct UsbFnStrings          **deviceStrings;
    /** Double pointer to USB configuration descriptors */
    struct UsbFnConfiguration    **configs;
};

/**
 * @brief Defines the descriptor data of USB devices.
 */
struct UsbFnDescriptorData {
    union {
        /** Pointer to device resource node attributes */
        const struct DeviceResourceNode *property;
        /** Pointer to the USB device descriptor */
        struct UsbFnDeviceDesc *descriptor;
    };
        /** Descriptor type */
    UsbFnDescDataType type;
    uint8_t functionMask;
};

/**
 * @brief Creates a USB device.
 *
 * You can use this function to create a descriptor and bind it to a USB device of the specified UDC.
 *
 * @param udcName Indicates the pointer to the UDC name, which is obtained based on the UDC driver.
 * @param descriptor Indicates the pointer to USB device descriptor data.
 *
 * @return Returns the pointer to the <b>UsbFnDevice</b> if the operation is successful;
 * returns <b>NULL</b> otherwise.
 */
const struct UsbFnDevice *UsbFnCreateDevice(const char *udcName, struct UsbFnDescriptorData *descriptor);

/**
 * @brief Deletes a specified USB device.
 *
 * 
 *
 * @param fnDevice Indicates the pointer to the USB device object.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value
 * defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnRemoveDevice(struct UsbFnDevice *fnDevice);
const struct UsbFnDevice *UsbFnGetDevice(const char *udcName);
int32_t UsbFnGetDeviceState(struct UsbFnDevice *fnDevice, UsbFnDeviceState *devState);

/**
 * @brief Obtains a USB interface based on the specified interface index.
 *
 * 
 *
 * @param fnDevice Indicates the pointer to the USB device object.
 * @param interfaceIndex Indicates the interface index, which is numbered from <b>0</b>.
 *
 * @return Returns the pointer to the <b>UsbFnInterface</b> if the operation is successful;
 * returns <b>NULL</b> otherwise.
 */
const struct UsbFnInterface *UsbFnGetInterface(struct UsbFnDevice *fnDevice, uint8_t interfaceIndex);
int32_t UsbFnMemTestTrigger(bool enable);

#endif /* USBFN_DEVICE_H */
