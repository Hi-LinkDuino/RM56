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
 * @brief Declares USB-related APIs, including the custom data types and functions
 * used to obtain descriptors, interface objects, and request objects, and to submit requests.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file usb_ddk.h
 *
 * @brief Defines the USB-related structures.
 * 
 * @since 1.0
 * @version 1.0
 */

#ifndef USB_DDK_H
#define USB_DDK_H

#include "hdf_base.h"
#include <endian.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
/**
 * @brief Implements 16-bit little-endian conversion.
 */
#define CpuToLe16(x)  (x)
/**
 * @brief Implements 32-bit little-endian conversion.
 */
#define CpuToLe32(x)  (x)
#else
/**
 * @brief Implements 16-bit little-endian conversion.
 */
#define CpuToLe16(x)  ((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8))
/**
 * @brief Implements 32-bit little-endian conversion.
 */
#define CpuToLe32(x)  \
    ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >>  8) | \
    (((x) & 0x0000ff00u) <<  8) | (((x) & 0x000000ffu) << 24))
#endif

/**
 * @brief Implements 16-bit little-endian conversion.
 */
#define Le16ToCpu               CpuToLe16
/**
 * @brief Implements 16-bit little-endian conversion.
 */
#define Le32ToCpu               CpuToLe32

/**
 * @brief Configures the bus power-on feature.
 */
#define USB_CFG_BUS_POWERED     0x80

/**
 * @brief Configures the automatic power-on feature.
 */
#define USB_CFG_SELF_POWERED    0x40

/**
 * @brief Configures the remote wakeup feature.
 */
#define USB_CFG_REMOTE_WAKEUP   0x20

/**
 * @brief Defines the data direction bit offset. For details, see {@link UsbRequestDirection}.
 */
#define USB_DIR_OFFSET          0x07

/**
 * @brief Defines the control request type offset. For details, see {@link UsbControlRequestType}.
 */
#define USB_TYPE_OFFSET         0x05

/**
 * @brief Defines the control request packet type offset. For details, see {@link UsbRequestTargetType}.
 */
#define USB_RECIP_OFFSET        0x00

/**
 * @brief Defines the USB string index.
 */
enum {
    /** Manufacturer index */
    USB_FUNC_MANUFACTURER_IDX,
    /** Product index */
    USB_FUNC_PRODUCT_IDX,
    /** Product SN index */
    USB_FUNC_SERIAL_IDX,
    /** Index of the first valid string */
    USB_FUNC_FIRST_AVAIL_IDX,
};

/**
 * @brief Renames a descriptor structure.
 */
enum {
    FUNCTIONFS_DESCRIPTORS_MAGIC = 1,
    FUNCTIONFS_STRINGS_MAGIC = 2,
    FUNCTIONFS_DESCRIPTORS_MAGIC_V2 = 3,
};

enum functionfs_flags {
    FUNCTIONFS_HAS_FS_DESC = 1,
    FUNCTIONFS_HAS_HS_DESC = 2,
    FUNCTIONFS_HAS_SS_DESC = 4,
    FUNCTIONFS_HAS_MS_OS_DESC = 8,
    FUNCTIONFS_VIRTUAL_ADDR = 16,
    FUNCTIONFS_EVENTFD = 32,
    FUNCTIONFS_ALL_CTRL_RECIP = 64,
    FUNCTIONFS_CONFIG0_SETUP = 128,
};

enum UsbDeviceSpeed {
    USB_DDK_SPEED_UNKNOWN = 0,
    USB_DDK_SPEED_LOW,
    USB_DDK_SPEED_FULL,
    USB_DDK_SPEED_HIGH,
    USB_DDK_SPEED_WIRELESS,
    USB_DDK_SPEED_SUPER,
    USB_DDK_SPEED_SUPER_PLUS,
};

struct UsbCtrlRequest {
    uint8_t  bRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__ ((packed));

struct UsbDescriptorHeader {
    uint8_t bLength;
    uint8_t bDescriptorType;
} __attribute__ ((packed));

struct UsbDeviceDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
} __attribute__ ((packed));

struct UsbConfigDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
} __attribute__ ((packed));

struct UsbStringDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wData[1];
} __attribute__ ((packed));

struct UsbInterfaceDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} __attribute__ ((packed));

struct UsbEndpointDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
    uint8_t  bRefresh;
    uint8_t  bSynchAddress;
} __attribute__ ((packed));

struct UsbSspIsocEpCompDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wReseved;
    uint32_t dwBytesPerInterval;
} __attribute__ ((packed));

struct UsbSsEpCompDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bMaxBurst;
    uint8_t  bmAttributes;
    uint16_t wBytesPerInterval;
} __attribute__ ((packed));

struct UsbQualifierDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint8_t  bNumConfigurations;
    uint8_t  bRESERVED;
} __attribute__ ((packed));

struct UsbOtgDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;
} __attribute__ ((packed));

struct UsbOtg20Descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bmAttributes;
    uint16_t bcdOTG;
} __attribute__ ((packed));

struct UsbInterfaceAssocDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction;
} __attribute__ ((packed));

struct UsbFunctionfsStringsHead {
    uint32_t magic;
    uint32_t length;
    uint32_t str_count;
    uint32_t lang_count;
} __attribute__((packed));

struct UsbFunctionfsDescsHeadV2 {
    uint32_t magic;
    uint32_t length;
    uint32_t flags;
} __attribute__((packed));

struct UsbSecurityDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wTotalLength;
    uint8_t  bNumEncryptionTypes;
} __attribute__((packed));

struct UsbCdcLineCoding {
    uint32_t dwDTERate;
    uint8_t  bCharFormat;

#define USB_CDC_1_STOP_BITS     0
#define USB_CDC_1_5_STOP_BITS   1
#define USB_CDC_2_STOP_BITS     2
    uint8_t bParityType;

#define USB_CDC_NO_PARITY       0
#define USB_CDC_ODD_PARITY      1
#define USB_CDC_EVEN_PARITY     2
#define USB_CDC_MARK_PARITY     3
#define USB_CDC_SPACE_PARITY    4
    uint8_t bDataBits;
} __attribute__ ((packed));

struct UsbCdcNotification {
    uint8_t  bmRequestType;
    uint8_t  bNotificationType;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__ ((packed));

struct UsbCdcHeaderDesc {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint16_t bcdCDC;
} __attribute__ ((packed));

struct UsbCdcCallMgmtDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint8_t bmCapabilities;

#define USB_CDC_CALL_MGMT_CAP_CALL_MGMT     0x01
#define USB_CDC_CALL_MGMT_CAP_DATA_INTF     0x02
    uint8_t bDataInterface;
} __attribute__ ((packed));

struct UsbCdcAcmDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint8_t bmCapabilities;
} __attribute__ ((packed));

struct UsbCdcUnionDesc {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint8_t bMasterInterface0;
    uint8_t bSlaveInterface0;
} __attribute__ ((packed));

struct UsbDebugDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDebugInEndpoint;
    uint8_t bDebugOutEndpoint;
} __attribute__((packed));

struct UsbCdcEtherDesc {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;

    uint8_t  iMACAddress;
    uint32_t bmEthernetStatistics;
    uint16_t wMaxSegmentSize;
    uint16_t wNumberMCFilters;
    uint8_t  bNumberPowerFilters;
} __attribute__ ((packed));

#define USB_DDK_ENDPOINT_NUMBER_MASK                0x0F
#define USB_DDK_ENDPOINT_DIR_MASK                   0x80
#define USB_DDK_DIR_OUT                             0x00
#define USB_DDK_DIR_IN                              0x80

#define USB_DDK_DT_DEVICE                           0x01
#define USB_DDK_DT_CONFIG                           0x02
#define USB_DDK_DT_STRING                           0x03
#define USB_DDK_DT_INTERFACE                        0x04
#define USB_DDK_DT_ENDPOINT                         0x05
#define USB_DDK_DT_DEVICE_QUALIFIER                 0x06
#define USB_DDK_DT_OTHER_SPEED_CONFIG               0x07
#define USB_DDK_DT_INTERFACE_POWER                  0x08
#define USB_DDK_DT_OTG                              0x09
#define USB_DDK_DT_DEBUG                            0x0A
#define USB_DDK_DT_INTERFACE_ASSOCIATION            0x0B
#define USB_DDK_DT_SECURITY                         0x0C
#define USB_DDK_DT_KEY                              0x0D
#define USB_DDK_DT_ENCRYPTION_TYPE                  0x0E
#define USB_DDK_DT_BOS                              0x0F
#define USB_DDK_DT_DEVICE_CAPABILITY                0x10
#define USB_DDK_DT_WIRELESS_ENDPOINT_COMP           0x11
#define USB_DDK_DT_WIRE_ADAPTER                     0x21
#define USB_DDK_DT_RPIPE                            0x22
#define USB_DDK_DT_CS_RADIO_CONTROL                 0x23
#define USB_DDK_DT_PIPE_USAGE                       0x24
#define USB_DDK_DT_SS_ENDPOINT_COMP                 0x30
#define USB_DDK_DT_SSP_ISOC_ENDPOINT_COMP           0x31
#define USB_DDK_DT_CS_DEVICE                        (USB_DDK_TYPE_CLASS | USB_DDK_DT_DEVICE)
#define USB_DDK_DT_CS_CONFIG                        (USB_DDK_TYPE_CLASS | USB_DDK_DT_CONFIG)
#define USB_DDK_DT_CS_STRING                        (USB_DDK_TYPE_CLASS | USB_DDK_DT_STRING)
#define USB_DDK_DT_CS_INTERFACE                     (USB_DDK_TYPE_CLASS | USB_DDK_DT_INTERFACE)
#define USB_DDK_DT_CS_ENDPOINT                      (USB_DDK_TYPE_CLASS | USB_DDK_DT_ENDPOINT)

#define USB_DDK_DT_SS_EP_COMP_SIZE                  0x06
#define USB_DDK_DT_ENDPOINT_SIZE                    0x07
#define USB_DDK_DT_SSP_ISOC_EP_COMP_SIZE            0x08
#define USB_DDK_DT_INTERFACE_ASSOCIATION_SIZE       0x08
#define USB_DDK_DT_CONFIG_SIZE                      0x09
#define USB_DDK_DT_INTERFACE_SIZE                   0x09
#define USB_DDK_DT_ENDPOINT_AUDIO_SIZE              0x09
#define USB_DDK_DT_DEVICE_SIZE                      0x12

#define USB_DDK_CLASS_PER_INTERFACE                 0x00
#define USB_DDK_CLASS_AUDIO                         0x01
#define USB_DDK_CLASS_COMM                          0x02
#define USB_DDK_CLASS_HID                           0x03
#define USB_DDK_CLASS_PHYSICAL                      0x05
#define USB_DDK_CLASS_STILL_IMAGE                   0x06
#define USB_DDK_CLASS_PRINTER                       0x07
#define USB_DDK_CLASS_MASS_STORAGE                  0x08
#define USB_DDK_CLASS_HUB                           0x09
#define USB_DDK_CLASS_CDC_DATA                      0x0A
#define USB_DDK_CLASS_CSCID                         0x0B
#define USB_DDK_CLASS_CONTENT_SEC                   0x0D
#define USB_DDK_CLASS_VIDEO                         0x0E
#define USB_DDK_CLASS_WIRELESS_CONTROLLER           0xE0
#define USB_DDK_CLASS_MISC                          0xEF
#define USB_DDK_CLASS_APP_SPEC                      0xFE
#define USB_DDK_CLASS_VENDOR_SPEC                   0xFF
#define USB_DDK_SUBCLASS_VENDOR_SPEC                0xFF

#define USB_DDK_CDC_NOTIFY_NETWORK_CONNECTION       0x00
#define USB_DDK_CDC_NOTIFY_RESPONSE_AVAILABLE       0x01
#define USB_DDK_CDC_NOTIFY_SERIAL_STATE             0x20
#define USB_DDK_CDC_NOTIFY_SPEED_CHANGE             0x2A

#define USB_DDK_CDC_SEND_ENCAPSULATED_COMMAND       0x00
#define USB_DDK_CDC_GET_ENCAPSULATED_RESPONSE       0x01
#define USB_DDK_CDC_REQ_SET_LINE_CODING             0x20
#define USB_DDK_CDC_REQ_GET_LINE_CODING             0x21
#define USB_DDK_CDC_REQ_SET_CONTROL_LINE_STATE      0x22
#define USB_DDK_CDC_REQ_SEND_BREAK                  0x23
#define USB_DDK_CDC_SET_ETHERNET_MULTICAST_FILTERS  0x40
#define USB_DDK_CDC_SET_ETHERNET_PM_PATTERN_FILTER  0x41
#define USB_DDK_CDC_GET_ETHERNET_PM_PATTERN_FILTER  0x42
#define USB_DDK_CDC_SET_ETHERNET_PACKET_FILTER      0x43
#define USB_DDK_CDC_GET_ETHERNET_STATISTIC          0x44
#define USB_DDK_CDC_GET_NTB_PARAMETERS              0x80
#define USB_DDK_CDC_GET_NET_ADDRESS                 0x81
#define USB_DDK_CDC_SET_NET_ADDRESS                 0x82
#define USB_DDK_CDC_GET_NTB_FORMAT                  0x83
#define USB_DDK_CDC_SET_NTB_FORMAT                  0x84
#define USB_DDK_CDC_GET_NTB_INPUT_SIZE              0x85
#define USB_DDK_CDC_SET_NTB_INPUT_SIZE              0x86
#define USB_DDK_CDC_GET_MAX_DATAGRAM_SIZE           0x87
#define USB_DDK_CDC_SET_MAX_DATAGRAM_SIZE           0x88
#define USB_DDK_CDC_GET_CRC_MODE                    0x89
#define USB_DDK_CDC_SET_CRC_MODE                    0x8A

#define USB_DDK_CDC_COMM_FEATURE                    0x01
#define USB_DDK_CDC_CAP_LINE                        0x02
#define USB_DDK_CDC_CAP_BRK                         0x04
#define USB_DDK_CDC_CAP_NOTIFY                      0x08

#define USB_DDK_CDC_HEADER_TYPE                     0x00
#define USB_DDK_CDC_CALL_MANAGEMENT_TYPE            0x01
#define USB_DDK_CDC_ACM_TYPE                        0x02
#define USB_DDK_CDC_UNION_TYPE                      0x06
#define USB_DDK_CDC_COUNTRY_TYPE                    0x07
#define USB_DDK_CDC_NETWORK_TERMINAL_TYPE           0x0a
#define USB_DDK_CDC_ETHERNET_TYPE                   0x0f
#define USB_DDK_CDC_WHCM_TYPE                       0x11
#define USB_DDK_CDC_MDLM_TYPE                       0x12
#define USB_DDK_CDC_MDLM_DETAIL_TYPE                0x13
#define USB_DDK_CDC_DMM_TYPE                        0x14
#define USB_DDK_CDC_OBEX_TYPE                       0x15
#define USB_DDK_CDC_NCM_TYPE                        0x1A
#define USB_DDK_CDC_MBIM_TYPE                       0x1B
#define USB_DDK_CDC_MBIM_EXTENDED_TYPE              0x1C

#define USB_DDK_CDC_PROTO_NONE                      0x00
#define USB_DDK_CDC_ACM_PROTO_AT_V25TER             0x01
#define USB_DDK_CDC_ACM_PROTO_AT_PCCA101            0x02
#define USB_DDK_CDC_ACM_PROTO_AT_PCCA101_WAKE       0x03
#define USB_DDK_CDC_ACM_PROTO_AT_GSM                0x04
#define USB_DDK_CDC_ACM_PROTO_AT_3G                 0x05
#define USB_DDK_CDC_ACM_PROTO_AT_CDMA               0x06
#define USB_DDK_CDC_PROTO_EEM                       0x07

#define USB_DDK_CDC_NCM_PROTO_NTB                   0x01
#define USB_DDK_CDC_MBIM_PROTO_NTB                  0x02

#define USB_DDK_CDC_SUBCLASS_ACM                    0x02
#define USB_DDK_CDC_SUBCLASS_ETHERNET               0x06
#define USB_DDK_CDC_SUBCLASS_WHCM                   0x08
#define USB_DDK_CDC_SUBCLASS_DMM                    0x09
#define USB_DDK_CDC_SUBCLASS_MDLM                   0x0A
#define USB_DDK_CDC_SUBCLASS_OBEX                   0x0B
#define USB_DDK_CDC_SUBCLASS_EEM                    0x0C
#define USB_DDK_CDC_SUBCLASS_NCM                    0x0D
#define USB_DDK_CDC_SUBCLASS_MBIM                   0x0E

#define USB_DDK_CDC_PACKET_TYPE_PROMISCUOUS         (1 << 0)
#define USB_DDK_CDC_PACKET_TYPE_ALL_MULTICAST       (1 << 1)
#define USB_DDK_CDC_PACKET_TYPE_DIRECTED            (1 << 2)
#define USB_DDK_CDC_PACKET_TYPE_BROADCAST           (1 << 3)
#define USB_DDK_CDC_PACKET_TYPE_MULTICAST           (1 << 4)

#define USB_DDK_CDC_ACM_PROTO_VENDOR                0xFF

#define USB_DDK_TYPE_MASK                           (0x03 << 5)
#define USB_DDK_TYPE_STANDARD                       (0x00 << 5)
#define USB_DDK_TYPE_CLASS                          (0x01 << 5)
#define USB_DDK_TYPE_VENDOR                         (0x02 << 5)
#define USB_DDK_TYPE_RESERVED                       (0x03 << 5)

#define USB_DDK_ENDPOINT_XFERTYPE_MASK              0x03
#define USB_DDK_ENDPOINT_XFER_CONTROL               0x00
#define USB_DDK_ENDPOINT_XFER_ISOC                  0x01
#define USB_DDK_ENDPOINT_XFER_BULK                  0x02
#define USB_DDK_ENDPOINT_XFER_INT                   0x03
#define USB_DDK_ENDPOINT_MAX_ADJUSTABLE             0x80

#define USB_DDK_RECIP_MASK                          0x1F
#define USB_DDK_RECIP_DEVICE                        0x00
#define USB_DDK_RECIP_INTERFACE                     0x01
#define USB_DDK_RECIP_ENDPOINT                      0x02
#define USB_DDK_RECIP_OTHER                         0x03

#define USB_DDK_REQ_GET_STATUS                      0x00
#define USB_DDK_REQ_CLEAR_FEATURE                   0x01
#define USB_DDK_REQ_SET_FEATURE                     0x03
#define USB_DDK_REQ_SET_ADDRESS                     0x05
#define USB_DDK_REQ_GET_DESCRIPTOR                  0x06
#define USB_DDK_REQ_SET_DESCRIPTOR                  0x07
#define USB_DDK_REQ_GET_CONFIGURATION               0x08
#define USB_DDK_REQ_SET_CONFIGURATION               0x09
#define USB_DDK_REQ_GET_INTERFACE                   0x0A
#define USB_DDK_REQ_SET_INTERFACE                   0x0B
#define USB_DDK_REQ_SYNCH_FRAME                     0x0C
#define USB_DDK_REQ_SET_SEL                         0x30
#define USB_DDK_REQ_SET_ISOCH_DELAY                 0x31

#define UsbiDescriptorHeader                        UsbDescriptorHeader
#define UsbiConfigurationDescriptor                 UsbConfigDescriptor
#define UsbiInterfaceDescriptor                     UsbInterfaceDescriptor

/**
 * @brief Checks whether the specified endpoint is in the input direction (the direction in
 * which data is transferred from the device to the host). For details, see {@link UsbRequestDirection}.
 *
 * @param ep Indicates the endpoint address, which is in the <b>uint8_t</b> format.
 *
 * @return Returns <b>1</b> if the specified endpoint is in the input direction; returns <b>0</b> otherwise.
 */
static inline int32_t UsbEndpointDirIn(uint8_t ep)
{
    return ((ep & USB_DDK_ENDPOINT_DIR_MASK) == USB_DDK_DIR_IN);
}

/**
 * @brief Checks whether the specified endpoint is in the output direction (the direction in which data is
 * transferred from the host to the device). For details, see {@link UsbRequestDirection}.
 *
 * @param ep Indicates the endpoint address, which is in the <b>uint8_t</b> format.
 *
 * @return Returns <b>1</b> if the specified endpoint is in the output direction; returns <b>0</b> otherwise.
 */
static inline int32_t UsbEndpointDirOut(uint8_t ep)
{
    return ((ep & USB_DDK_ENDPOINT_DIR_MASK) == USB_DDK_DIR_OUT);
}

#endif /* USB_DDK_H */
/** @} */
