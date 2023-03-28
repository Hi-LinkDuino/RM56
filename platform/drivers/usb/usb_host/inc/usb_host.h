/***************************************************************************
 *
 * Copyright 2015-2022 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __USBHOST__
#define __USBHOST__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_usbhost.h"

#define USING_MSGQ
// #define USB_SLEEP_TEST
// #define USB_WAKEUP_OPEN

#define TOKEN_SETUP 0
#define TOKEN_IN  1
#define TOKEN_OUT 2
//  Status flags from hub
#define PORT_CONNECTION 0
#define PORT_ENABLE  1
#define PORT_SUSPEND  2
#define PORT_OVER_CURRENT 3
#define PORT_RESET 4
#define PORT_POWER 8
#define PORT_LOW_SPEED 9

#define C_PORT_CONNECTION 16
#define C_PORT_ENABLE 17
#define C_PORT_SUSPEND 18
#define C_PORT_OVER_CURRENT 19
#define C_PORT_RESET 20

//  Hardware defines
#define  TD_ROUNDING        (uint32_t)0x00040000
#define  TD_SETUP           (uint32_t)0x00000000
#define  TD_IN              (uint32_t)0x00100000
#define  TD_OUT             (uint32_t)0x00080000
#define  TD_DELAY_INT(x)    (uint32_t)((x) << 21)
#define  TD_TOGGLE_0        (uint32_t)0x02000000
#define  TD_TOGGLE_1        (uint32_t)0x03000000
#define  TD_CC              (uint32_t)0xF0000000

//  HcControl
#define PERIODICLISTENABLE  0x00000004
#define ISOCHRONOUSENABLE   0x00000008
#define CONTROLLISTENABLE   0x00000010
#define BULKLISTENABLE      0x00000020
#define OPERATIONALMASK     0x00000080
#define HOSTCONTROLLERFUNCTIONALSTATE   0x000000C0

//  HcCommandStatus
#define HOSTCONTROLLERRESET 0x00000001
#define CONTROLLISTFILLED   0x00000002
#define BULKLISTFILLED      0x00000004

//  HcInterruptStatus Register
#define WRITEBACKDONEHEAD       0x00000002
#define STARTOFFRAME            0x00000004
#define RESUMEDETECTED          0x00000008
#define UNRECOVERABLEERROR      0x00000010
#define FRAMENUMBEROVERFLOW     0x00000020
#define ROOTHUB_STATUS_CHANGE   0x00000040
#define OWNERSHIPCHANGE         0x00000080
#define MASTERINTERRUPTENABLE   0x80000000

//  HcRhStatus
#define SETGLOBALPOWER              0x00010000
#define DEVICEREMOTEWAKEUPENABLE    0x00008000

//  HcRhPortStatus (hub 0, port 1)
#define CURRENTCONNECTSTATUS        0x00000001
#define PORTENABLESTATUS            0x00000002
#define PORTSUSPENDSTATUS           0x00000004
#define PORTOVERCURRENTINDICATOR    0x00000008
#define PORTRESETSTATUS             0x00000010

#define PORTPOWERSTATUS         0x00000100
#define LOWSPEEDDEVICE          0x00000200
#define HIGHSPEEDDEVICE         0x00000400

#define CONNECT_STATUS_CHANGE   (CURRENTCONNECTSTATUS << 16)
#define PORTRESETSTATUSCHANGE   (PORTRESETSTATUS << 16)

#define SKIP_CLEANUP_HW_CHAN        (1 << 0)
#define SKIP_CLEANUP_CLOSE          (1 << 1)
#define SKIP_CLEANUP_FILE_SYSTEM    (1 << 2)

#define REQUEST_TYPE_CLASS          0x20
#define USB_ENDPORT_DMA_BUFFER_LEN  4096
enum USB_ENDPORT_STATE_T
{
    USB_ENDPORT_STATE_FREE,
    USB_ENDPORT_STATE_NOTQUEUED,
    USB_ENDPORT_STATE_IDLE,
    USB_ENDPORT_STATE_SETUPQUEUED,
    USB_ENDPORT_STATE_DATAQUEUED,
    USB_ENDPORT_STATE_STATUSQUEUED,
    USB_ENDPORT_STATE_CALLBACK_PENDING,
    // USB_ENDPORT_STATE_WAIT_PROCESS_DONE,
};

//HostController EndPoint Descriptor
typedef struct {
    volatile uint32_t   control;
    volatile uint32_t   tail_td;
    volatile uint32_t   head_td;
    volatile uint32_t   next;
} HCED_T;

//HostController Transfer Descriptor
typedef struct {
    volatile uint32_t   control;
    volatile uint32_t   curr_buf_ptr;
    volatile uint32_t   next;
    volatile uint32_t   bufend;
} HCTD_T;

// Host Controller Communication Area
typedef struct {
    volatile uint32_t   InterruptTable[32];
    volatile uint16_t   FrameNumber;
    volatile uint16_t   FrameNumberPad;
    volatile uint32_t   DoneHead;
    volatile uint8_t Reserved[120];
} HCCA;

enum USBHOST_CLASS_CODE_T
{
    USB_CLASS_DEVICE = 0x0,
    USB_CLASS_AUDIO = 0x01,
    USB_CLASS_COMM = 0x02,
    USB_CLASS_HID = 0x03,
    USB_CLASS_PHYSICAL = 0x05,
    USB_CLASS_IMAGING = 0x06,
    USB_CLASS_PRINTER = 0x07,
    USB_CLASS_MASS_STORAGE = 0x08,
    USB_CLASS_HUB = 0x09,
    USB_CLASS_DATA = 0x0a,
    USB_CLASS_SMART_CARD = 0x0b,
    USB_CLASS_CONTENT_SECURITY = 0x0d,
    USB_CLASS_VIDEO = 0x0e,
    USB_CALSS_PERSONAL_HEALTHCARE = 0x0f,
    USB_CLASS_DIAGNOSTIC_DEVICE = 0xdc,
    USB_CLASS_WIRELESS = 0xe0,
    USB_CLASS_MISCELLANEOUS = 0xef,
    USB_CLASS_APP_SPECIFIC = 0xfe,
    USB_CLASS_VENDOR_SPECIFIC = 0xff
};

enum USBHOST_DESCRIPTOR_TYPE
{
    DESCRIPTOR_TYPE_DEVICE = 0x01,
    DESCRIPTOR_TYPE_CONFIG = 0x02,
    DESCRIPTOR_TYPE_STRING = 0x03,
    DESCRIPTOR_TYPE_INTERFACE = 0x04,
    DESCRIPTOR_TYPE_ENDPOINT = 0x05,
    DESCRIPTOR_TYPE_BOS = 0x0f,
    DESCRIPTOR_TYPE_DEVICE_CAPABILITY = 0x10,
    DESCRIPTOR_TYPE_DEVICE_HID = 0x21,
    DESCRIPTOR_TYPE_DEVICE_REPORT = 0x22,
    DESCRIPTOR_TYPE_DEVICE_PHYSICAL = 0x23,
    DESCRIPTOR_TYPE_DEVICE_HUB = 0x29,
    DESCRIPTOR_TYPE_DEVICE_SUPERSPEED_HUB = 0x2a,
    DESCRIPTOR_TYPE_DEVICE_ENDPOINT_COMPANION = 0x2a,
};

enum USBHOST_ENDPOINT_DIRECTION_T
{
    USB_ENDPOINT_DIRECTION_IN = 0x80,
    USB_ENDPOINT_DIRECTION_OUT = 0x0,
};

enum USBHOST_TRANSFER_TYPE_T
{
    USB_TRANSFER_TYPE_CONTROL = 0,
    USB_TRANSFER_TYPE_ISOCHRONOUS = 1,
    USB_TRANSFER_TYPE_BULK = 2,
    USB_TRANSFER_TYPE_INTERRUPT = 3,
};

enum USBHOST_REQUEST_T
{
    USB_REQUEST_GET_STATUS = 0x00,
    USB_REQUEST_CLEAR_FEATURE = 0x01,
    USB_REQUEST_SET_FEATURE = 0x03,
    USB_REQUEST_SET_ADDRESS = 0x05,
    USB_REQUEST_GET_DESCRIPTOR = 0x06,
    USB_REQUEST_SET_DESCRIPTOR = 0x07,
    USB_REQUEST_GET_CONFIGURATION = 0x08,
    USB_REQUEST_SET_CONFIGURATION = 0x09,
    USB_REQUEST_GET_INTERFACE = 0x0a,
    USB_REQUEST_SET_INTERFACE = 0x0b,
    USB_REQUEST_SYNCH_FRAME = 0x0c,
    USB_REQUEST_SET_SEL = 0x30,
    USB_REQUEST_SET_ISOCH_DELAY = 0x31,
};

enum USBHOST_REQUEST_RECIPIENT_T
{
    USB_REQUEST_RECIPIENT_DEVICE = 0x00,
    USB_REQUEST_RECIPIENT_INTERFACE = 0x01,
    USB_REQUEST_RECIPIENT_ENDPOINT = 0x02,
    USB_REQUEST_RECIPIENT_OTHER = 0x03,
};

#define AUTOEVT(_class,_subclass,_protocol) (((_class) << 16) | ((_subclass) << 8) | _protocol)
#define AUTO_KEYBOARD AUTOEVT(USB_CLASS_HID,1,1)
#define AUTO_MOUSE AUTOEVT(USB_CLASS_HID,1,2)

#define MAX_DEVICES 8               // Max number of devices
#define MAX_ENDPOINTS_TOTAL 16      // Max number of endpoints total
#define MAX_ENDPOINTS_PER_DEVICE 8  // Max number of endpoints for any one device
#define MAX_CALLBACK_PENDING 16

typedef struct {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint16_t    bcdUSB;
    uint8_t     bDeviceClass;
    uint8_t     bDeviceSubClass;
    uint8_t     bDeviceProtocol;
    uint8_t     bMaxPacketSize;
    uint16_t    idVendor;
    uint16_t    idProduct;
    uint16_t    bcdDevice; // version
    uint8_t     iManufacturer;
    uint8_t     iProduct;
    uint8_t     iSerialNumber;
    uint8_t     bNumConfigurations;
} USBHOST_DEVICE_DESCRIPTOR_T;  // 18 bytes

typedef struct {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint16_t    wTotalLength;
    uint8_t     bNumInterfaces;
    uint8_t     bConfigurationValue;    // Value to use as an argument to select this configuration
    uint8_t     iConfiguration;         // Index of String Descriptor describing this configuration
    uint8_t     bmAttributes;           // Bitmap D7 Reserved, set to 1. (USB 1.0 Bus Powered),D6 Self Powered,D5 Remote Wakeup,D4..0 = 0
    uint8_t     bMaxPower;              // Maximum Power Consumption in 2mA units
} USBHOST_CONFIGURATION_DICRIPTOR_T;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;             // Index of String Descriptor Describing this interface
} USBHOST_INTERFACE_DESCRIPTOR_T;

typedef struct {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint8_t     bEndpointAddress;   // Bits 0:3 endpoint, Bits 7 Direction 0 = Out, 1 = In (Ignored for Control Endpoints)
    uint8_t     bmAttributes;       // Bits 0:1 00 = Control, 01 = Isochronous, 10 = Bulk, 11 = Interrupt
    uint16_t    wMaxPacketSize;
    uint8_t     bInterval;          // Interval for polling endpoint data transfers.
    /* NOTE:  these two are _only_ in audio endpoints. */
    /* use USB_DT_ENDPOINT*_SIZE in bLength, not sizeof. */
    uint8_t     refresh;
    uint8_t     synch_address;
} USBHOST_ENDPOINT_DESCRIPTOR_T;

typedef struct {
    uint8_t    bLength;
    uint8_t    bDescriptorType;
    uint16_t   bcdHID;
    uint8_t    bCountryCode;
    uint8_t    bNumDescriptors;
    uint8_t    bDescriptorType2;
    uint16_t   wDescriptorLength;
} USBHOST_HID_DESCRIPTOR_T;

typedef struct {
    uint8_t length;
    uint8_t descriptor_type;
    uint8_t string[1];
} USBHOST_STRING_DESCRIPTION_T;

#define USB_MAXCHILDREN     (31)

typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bNbrPorts;
    uint16_t wHubCharacteristics;
    uint8_t  bPwrOn2PwrGood;
    uint8_t  bHubContrCurrent;

    /* 2.0 and 3.0 hubs differ here */
    union {
        struct {
            /* add 1 bit for hub status change; round to bytes */
            uint8_t  DeviceRemovable[(USB_MAXCHILDREN + 1 + 7) / 8];
            uint8_t  PortPwrCtrlMask[(USB_MAXCHILDREN + 1 + 7) / 8];
        } __attribute__ ((packed)) hs;

        struct {
            uint8_t     bHubHdrDecLat;
            uint16_t    wHubDelay;
            uint16_t    DeviceRemovable;
        } __attribute__ ((packed)) ss;
    } u;
} USBHOST_HUB_DESCRIPTOR_T;

enum ENDPOINTSTATE {
    EP_FREE,
    EP_NOTQUEUED,
    EP_IDLE,
    EP_SETUPQUEUED,
    EP_DATAQUEUED,
    EP_STATUSQUEUED,
    EP_CALLBACKPENDING
};

typedef void (*USBHOSTCALLBACK)(int device, int endpoint, int status, uint8_t* data, int len, void* userData);
typedef  void (*INTERRUPT_HANDLE_CB) (void);
typedef  void (*PLUG_HANDLE_CB) (void);

typedef struct UHEndpoint UHEndpoint;
typedef struct UHDevice UHDevice;
typedef struct UHController UHController;

typedef struct {
    void (*init)(UHEndpoint *);
    void (*hook)(UHEndpoint *);

    uint8_t (*is_free)(UHEndpoint *);
    int (*get_direct)(UHEndpoint *);
    int (*status)(UHEndpoint *);
    void (*set_current_state)(UHEndpoint *, int s);
    UHController * (*get_controller)(UHEndpoint *);

    int (*wait_io_done)(UHEndpoint *);
    int (*prepare)(UHEndpoint *, UHDevice * device, uint8_t device_id, int address, int ep_type, int mps, int index);
    int (*before_transfer)(UHEndpoint *, uint8_t flags, uint8_t* data, int length, USBHOSTCALLBACK callback, void* userData);
    int (*transfer)(UHEndpoint *, int token, uint8_t* data, int len, int state);
    void (*release_chan)(UHEndpoint *, uint8_t *chan);
    void (*release)(UHEndpoint *, uint8_t _skipCleanupBitmap);
    void (*pending_process)(UHEndpoint *);

    void (*show)(UHEndpoint *);
} UHEndpoint_actions;

struct UHEndpoint {
    //location information under Device
    UHDevice * device;
    uint8_t device_id;
    int index;  //the index of endpoints in UHDevice

    //self information
    int address;
    uint8_t ep_transfer_type;   //control/bulk/interrupt/...

    uint8_t chan;
    uint8_t chan_ctrl_in;

    uint32_t dma_buf[4096 / 4];
    bool first_xfer;
    uint16_t mps;

    volatile uint8_t current_state;

    uint16_t     length;
    uint8_t*     data;
    USBHOSTCALLBACK callback;    // Must be a multiple of 16 bytes long
    void*  user_data;

    UHEndpoint_actions * actions;
    // HCED EndpointDescriptor;    // Pointer to EndpointDescriptor == Pointer to Endpoint
    // HCTD TDHead;
};

typedef struct {
    //function list
    void (*init)(UHDevice *);
    void (*hook)(UHDevice *);

    UHEndpoint * (*get_endpoint)(UHDevice *, int address, int *index);
    UHEndpoint * (*get_write_ep)(UHDevice *);
    UHEndpoint * (*get_read_ep)(UHDevice *);

    int (*get_descriptor)(UHDevice *, int descType,int descIndex, uint8_t* data, int length);
    int (*connect)(UHDevice *, UHController *, int hub, int port, int address, int isLowSpeed);
    int (*set_config_and_interface)(UHDevice *);
    int (*after_add_endpoint)(UHDevice *, USBHOST_INTERFACE_DESCRIPTOR_T* id, USBHOST_ENDPOINT_DESCRIPTOR_T * ed);
    void (*load_device)(UHDevice * that, USBHOST_INTERFACE_DESCRIPTOR_T* interfaceDesc);
    int (*disconnect)(UHDevice *, uint8_t _skipCleanupBitmap);
    int (*unload)(UHDevice *);
    int (*on_insert_disk)(UHDevice *);
    int (*add_endpoint)(UHDevice *, int address, int attributes, int maxPacketSize, int interval);

    int (*get_string)(UHDevice *, int device, int index, char* dst, int length);
    int (*set_address)(UHDevice *, int new_addr);
    int (*set_config)(UHDevice *, int configNum);
    int (*set_interface)(UHDevice *, int ifNum, int altNum);
    //  HUB stuff
    int (*set_port_feature)(UHDevice *, int feature, int index);
    int (*clear_port_feature)(UHDevice *, int feature, int index);
    int (*port_poweron)(UHDevice *, int port, int delayMs);
    int (*port_reset)(UHDevice *, int port);
    int (*get_port_status)(UHDevice *, int port, u32* status);
    // Transfer
    int (*control_transfer)(UHDevice *, int request_type, int request, int value, int index, uint8_t* data, int length, USBHOSTCALLBACK callback, void * userData);
    int (*interrupt_transfer)(UHDevice *, int ep, uint8_t* data, int length, USBHOSTCALLBACK callback, void* userData);
    int (*bulk_transfer)(UHDevice *, int ep, uint8_t* data, int length, USBHOSTCALLBACK callback, void* userData);
    int (*transfer)(UHDevice *, int ep, uint8_t flags, uint8_t* data, int length, USBHOSTCALLBACK callback, void* userData);
    int (*transfer_abort)(UHDevice *, int ep);
    struct HAL_USBHOST_SETUP_PKT_T * (*get_setup)(UHDevice *);

    void (*add_auto_event)(UHDevice *, USBHOST_INTERFACE_DESCRIPTOR_T* id, USBHOST_ENDPOINT_DESCRIPTOR_T* ed);
    int (*init_hub)(UHDevice *);
    int (*get_hub_port_count)(UHDevice *);
    int (*get_hub_interrupt_data)(UHDevice *);

    void (*show)(UHDevice *);
} UHDevice_actions;

struct UHDevice {
    //Location information Under controller
    UHController * controller;
    int index;      //the index of devices in UHController

    //self information
    uint8_t hub;
    uint8_t port;
    uint8_t device_id;
    uint8_t pad;
    uint8_t  is_low_speed;

    uint8_t scsi_read_ep;
    uint8_t scsi_write_ep;
    uint8_t scsi_itf_num;

    USBHOST_DEVICE_DESCRIPTOR_T desc;

    //  Only if this device is a hub
    uint8_t hub_port_count; // nonzero if this is a hub
    uint8_t hub_interrupt_data;
    uint8_t hub_map;
    uint8_t hub_mask;

    struct HAL_USBHOST_SETUP_PKT_T setup_buffer;
    UHEndpoint * zero;

    unsigned int usb_dma_in_buffer[64 * 4 /4];
    unsigned int usb_dma_len;

    int endpoint_status[MAX_ENDPOINTS_PER_DEVICE];
    UHEndpoint endpoints[MAX_ENDPOINTS_PER_DEVICE];

    UHDevice_actions * actions;
};

typedef struct {
    //function list
    void (*init)(UHController *, INTERRUPT_HANDLE_CB interrupt_handler, PLUG_HANDLE_CB plug_handler);
    void (*hook)(UHController *);
    void (*hw_init)(UHController *);

    UHEndpoint * (*get_ep0)(UHController *);
    struct HAL_USBHOST_SETUP_PKT_T * (*get_setup)(UHController *);

    void (*trig_event)(UHController *, int event_type, int evt);

    int (*connect)(UHController *, int hub, int port, bool isLowSpeed);
    int (*disconnect)(UHController *, int hub, int port);
    int (*disconnect_device)(UHController *, int device_index);

    void (*reset_port)(UHController *, int hub, int port);
    int32_t (*resume_port)(UHController *, int hub, int port);
    int32_t (*suspend_port)(UHController *, int hub, int port);
    void (*reconnect_cleanup)(UHController *, int hub, int port);
    UHDevice * (*get_device)(UHController *, int device_id);
    int (*get_device_index)(UHController *, int hub, int port);
    // Called from interrupt
    void (*hub_status_change)(UHController *, int hub, int port, u32 status);
    void (*countdown_process)(UHController *, uint8_t hub, uint8_t port, int connect);
#ifdef USB_SLEEP_TEST
    void (*sleep_countdown_process)(UHController * that, int hub, int port);
    void (*wakeup_countdown_process)(UHController * that, int hub, int port);
    void (*fs_test_countdown_process)(UHController *that, int hub, int port);
#endif
    void (*loop)(UHController *);

    //HubInterrupt - bitmap in dev->HubInterruptData
    void (*hub_interrupt)(UHController *, int device);

    //Called from interrupt...
    //Control endpoints use a state machine to progress through the transfers
    void (*process_done_q)(UHController *, UHEndpoint* endpoint, int status);

    void (*show)(UHController *);
    void (*deinit)(UHController *);
} UHController_actions;

struct UHController {
    uint8_t connect_hub;
    uint8_t connect_port;
    u32 connect_countdown;
    u32 disconn_countdown;
#ifdef USB_SLEEP_TEST
    u32 sleep_count_down;
    u32 wakeup_count_down;
    u32 fs_test_count_down;
#endif

    int device_hubport[MAX_DEVICES]; //hubport<-->device map
    UHDevice devices[MAX_DEVICES];

    struct HAL_USBHOST_SETUP_PKT_T setup_buffer;
    UHEndpoint zero;
    volatile UHEndpoint * pending_endpoints[MAX_CALLBACK_PENDING];
    int callbacks_pending; //how many pending callback functions
    uint8_t connect_pending;

    INTERRUPT_HANDLE_CB interrupt_handler;
    PLUG_HANDLE_CB plug_handler;

    uint8_t root_hub_status_change; //  Root hub status has changed, set from ISR
    enum HAL_USBHOST_PORT_EVENT_T root_hub_event;

    uint8_t plug_status_change;
    enum HAL_USBHOST_PLUG_STATUS_T plug_status;

    volatile int frame_number;

    UHController_actions * actions;
};

int usbhost_control_transfer(int device, int request_type, int request, int value, int index, uint8_t* data, int length, USBHOSTCALLBACK callback, void * userData);
int usbhost_interrupt_transfer(int device, int ep, uint8_t* data, int length, USBHOSTCALLBACK callback, void* userData);
int usbhost_bulk_transfer(int device, int ep, uint8_t* data, int length, USBHOSTCALLBACK callback, void* userData);
void usbhost_plug_handler(enum HAL_USBHOST_PLUG_STATUS_T status);

#define USB_PLUG_STATUS_MESSAGE_PLUG    0x0
#define USB_ROOT_HUB_STATUS_CHANGE      0x1
#define USB_TRANSFER_COMPLETED          0x2
#define USB_DEVICE_LOADED               0x3
#define DISCONN_COUNTDOWN_EVENT         0x4
#define CONN_COUNTDOWN_EVENT            0x5
#define DISK_INSERTED                   0x6
#define DISK_PLUGOUT                    0x7
#define WAKEUP_COUNTDOWN_EVENT          0x8
#define SLEEP_COUNTDOWN_EVENT           0x9
#define FS_TEST_COUNTDOWN_EVENT         0xA

#ifdef USING_MSGQ
#define USBHOST_MAILBOX_MAX   20
#define MSG_CALLBACK_MAX 20

typedef int (*USBHOST_MSG_CALLBACK)(void *msg, void *context);
typedef int (*USBHOST_NOMSGCALLBACK)(void *context);

typedef struct {
    int msg_id;
    USBHOST_MSG_CALLBACK cb;
} ID_CB_MAP;

typedef struct {
    int msg_id;
    USBHOST_NOMSGCALLBACK cb;
} ID_NOMSGCB_MAP;

typedef struct {
    int id;
    uint32_t param0;
} USBHOST_MESSAGE;

int usbhost_register_callback(int msg_id, USBHOST_MSG_CALLBACK cb);
#endif

void usbhost_init(INTERRUPT_HANDLE_CB interrupt_handler, PLUG_HANDLE_CB plug_handler);
void usbhost_loop();

#define IO_PENDING -100
#define ERR_ENDPOINT_NONE_LEFT      -101
#define ERR_ENDPOINT_NOT_FOUND      -102
#define ERR_DEVICE_NOT_FOUND        -103
#define ERR_DEVICE_NONE_LEFT        -104
#define ERR_HUB_INIT_FAILED         -105
#define ERR_INTERFACE_NOT_FOUND     -106
#define ERR_DATA_SIZE_IS_ZERO       -107
#define ERR_DATA_SIZE_TOO_LARGE     -108
#define ERR_DATA_SIZE_NOT_ALIGNMENT -109

#ifdef __cplusplus
}
#endif

#endif