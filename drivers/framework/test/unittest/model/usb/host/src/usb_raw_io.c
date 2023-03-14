/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "usb_raw_test.h"
#include "data_fifo.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "usb_ddk_interface.h"
#ifdef LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY
#include "usb_pnp_notify.h"
#endif

bool g_writeBufFlag = 0;
struct UsbSession *g_session = NULL;
struct AcmRawDevice g_deviceService;
struct AcmRawDevice *g_acm = &g_deviceService;
UsbRawHandle *g_devHandle = NULL;
UsbRawDevice *g_dev = NULL;
int32_t g_activeConfig;
bool g_initFlag;
bool g_stopIoThreadFlag = false;

struct AcmRawDevice *UsbGetIoAcm(void)
{
    return g_acm;
}

int32_t UsbIoThread(void *data)
{
    int32_t ret;
    struct AcmRawDevice *acm = (struct AcmRawDevice *)data;

    while (true) {
        printf("%s:%d\n", __func__, __LINE__);
        if (acm == NULL) {
            printf("%s:%d acm is NULL\n", __func__, __LINE__);
            OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
            continue;
        }

        if (acm->devHandle == NULL) {
            printf("%s:%d acm->devHandle is NULL!\n", __func__, __LINE__);
            OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
            continue;
        }

        printf("%s:%d\n", __func__, __LINE__);
        ret = UsbRawHandleRequests(acm->devHandle);
        printf("%s:%d, ret=%d\n", __func__, __LINE__, ret);
        if (ret < 0) {
            printf("%s:%d UsbRawHandleRequests faile, ret=%d \n", __func__, __LINE__, ret);
            if (ret == HDF_DEV_ERR_NO_DEVICE) {
                printf("%s:%d, ret=%d\n", __func__, __LINE__, ret);
                OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
            }
        }

        if (g_stopIoThreadFlag == true) {
            printf("%s:%d\n", __func__, __LINE__);
            break;
        }
    }

    printf("%s:%d exit\n", __func__, __LINE__);

    return HDF_SUCCESS;
}


int32_t UsbStartIo(void)
{
    struct OsalThreadParam threadCfg;
    int32_t ret;

    printf("%s start\n", __func__);
    g_stopIoThreadFlag = false;

    /* create Io thread */
    (void)memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name      = (char *)("usb io thread");
    threadCfg.priority  = OSAL_THREAD_PRI_LOW;
    threadCfg.stackSize = USB_IO_THREAD_STACK_SIZE;

    ret = OsalThreadCreate(&g_acm->ioThread, \
                           (OsalThreadEntry)UsbIoThread, (void *)g_acm);
    if (ret != HDF_SUCCESS) {
        printf("%s:%d OsalThreadCreate faile, ret=%d \n", __func__, __LINE__, ret);
        return ret;
    }

    ret = OsalThreadStart(&g_acm->ioThread, &threadCfg);
    if (ret != HDF_SUCCESS) {
        printf("%s:%d OsalThreadStart faile, ret=%d \n", __func__, __LINE__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t UsbStopIo(void)
{
    int32_t ret;
    g_stopIoThreadFlag = true;
    HDF_LOGD("%s:%d", __func__, __LINE__);
    ret = OsalThreadDestroy(&g_acm->ioThread);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalThreadDestroy faile, ret=%d ", __func__, __LINE__, ret);
        return ret;
    }
    return HDF_SUCCESS;
}

void AcmWriteBulkCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;

    printf("%s:%d entry!", __func__, __LINE__);

    if (req == NULL) {
        printf("%s:%d req is NULL!", __func__, __LINE__);
        return;
    }
    struct RawWb *wb  = (struct RawWb *)req->userData;
    if (wb == NULL) {
        printf("%s:%d userData(wb) is NULL!", __func__, __LINE__);
        return;
    }

    if (req->status != USB_REQUEST_COMPLETED) {
        printf("%s: write req failed, status=%d", __func__, req->status);
    }

    wb->use = 0;
}

void AcmWriteIsoCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;
    printf("%s:%d status:%d\n", __func__, __LINE__, req->status);
}

void AcmReadBulkCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;

    printf("%s:%d entry!", __func__, __LINE__);

    if (req == NULL) {
        printf("%s:%d req is NULL!", __func__, __LINE__);
        return;
    }
    struct AcmRawDevice *acm = (struct AcmRawDevice *)req->userData;
    if (acm == NULL) {
        printf("%s:%d userData(acm) is NULL!", __func__, __LINE__);
        return;
    }
    size_t size = (size_t)req->actualLength;

    switch (req->status) {
        case USB_REQUEST_COMPLETED:
            HDF_LOGD("Bulk status: %d+size:%u\n", req->status, size);
            if (size) {
                uint8_t *data = req->buffer;
                printf("rcv:%s\n", (char *)data);
            }
            break;
        case USB_REQUEST_CANCELLED:
            return;
        default:
            printf("%s: the request is failed\n", __func__);
            break;
    }

    if (UsbRawSubmitRequest(req)) {
        printf("%s - UsbRawSubmitRequest failed", __func__);
    }
}

void AcmNotifyReqCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;

    printf("%s:%d entry!", __func__, __LINE__);

    if (req == NULL) {
        printf("%s:%d req is NULL!", __func__, __LINE__);
        return;
    }
    struct AcmRawDevice *acm = (struct AcmRawDevice *)req->userData;
    if (acm == NULL) {
        printf("%s:%d userData(acm) is NULL!", __func__, __LINE__);
        return;
    }
    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)req->buffer;
    if (dr == NULL) {
        printf("%s:%d req->buffer(dr) is NULL!", __func__, __LINE__);
        return;
    }
    unsigned int currentSize = req->actualLength;

    printf("Irqstatus:%d,actualLength:%u\n", req->status, currentSize);
}

static int32_t AcmWriteBufAllocHandle(const struct AcmRawDevice *acm)
{
    int32_t i;
    struct RawWb *wb;
    for (wb = (struct RawWb *)&acm->wb[0], i = 0; i < ACM_NW; i++, wb++) {
        wb->buf = OsalMemCalloc(acm->dataOutEp.maxPacketSize);
        if (!wb->buf) {
            while (i > 0) {
                --i;
                --wb;
                OsalMemFree(wb->buf);
                wb->buf = NULL;
            }
            return -HDF_ERR_MALLOC_FAIL;
        }
        g_writeBufFlag = true;
    }

    return HDF_SUCCESS;
}

int32_t AcmWriteBufAlloc(struct AcmRawDevice *acm)
{
    int32_t ret = HDF_SUCCESS;

    if (!g_writeBufFlag) {
        ret = AcmWriteBufAllocHandle(acm);
    }

    return ret;
}

void AcmWriteBufFree(struct AcmRawDevice *acm)
{
    int32_t i;
    struct RawWb *wb;
    for (wb = &acm->wb[0], i = 0; i < ACM_NW; i++, wb++) {
        if (wb->buf) {
            OsalMemFree(wb->buf);
            wb->buf = NULL;
        }
    }
    g_writeBufFlag = false;
    return;
}

void AcmCtrlReqCallback(const void *requestArg)
{
    (void)requestArg;
    HDF_LOGI("%{public}s:%{public}d entry!", __func__, __LINE__);
}

static void AcmParaseInterfaceClass(
    struct AcmRawDevice * const acm, const struct UsbRawInterface *interface, uint8_t number)
{
    uint8_t ifaceClass;
    uint8_t numEndpoints;

    ifaceClass = interface->altsetting->interfaceDescriptor.bInterfaceClass;
    numEndpoints = interface->altsetting->interfaceDescriptor.bNumEndpoints;

    switch (ifaceClass) {
        case USB_DDK_CLASS_COMM:
            acm->ctrlIface = number;
            /* get the first endpoint  by default */
            acm->notifyEp.addr = interface->altsetting->endPoint[0].endpointDescriptor.bEndpointAddress;
            acm->notifyEp.interval = interface->altsetting->endPoint[0].endpointDescriptor.bInterval;
            acm->notifyEp.maxPacketSize = interface->altsetting->endPoint[0].endpointDescriptor.wMaxPacketSize;
            break;
        case USB_DDK_CLASS_CDC_DATA:
            acm->dataIface = number;
            for (uint8_t j = 0; j < numEndpoints; j++) {
                const struct UsbRawEndpointDescriptor *endPoint = &interface->altsetting->endPoint[j];

                /* get bulk in endpoint  */
                if ((endPoint->endpointDescriptor.bEndpointAddress
                    & USB_DDK_ENDPOINT_DIR_MASK) == USB_DDK_DIR_IN) {
                    acm->dataInEp.addr = endPoint->endpointDescriptor.bEndpointAddress;
                    acm->dataInEp.interval = endPoint->endpointDescriptor.bInterval;
                    acm->dataInEp.maxPacketSize = endPoint->endpointDescriptor.wMaxPacketSize;
                } else { /* get bulk out endpoint  */
                    acm->dataOutEp.addr = endPoint->endpointDescriptor.bEndpointAddress;
                    acm->dataOutEp.interval = endPoint->endpointDescriptor.bInterval;
                    acm->dataOutEp.maxPacketSize = endPoint->endpointDescriptor.wMaxPacketSize;
                }
            }
            break;
        default:
            printf("%s:%d wrong descriptor type\n", __func__, __LINE__);
            break;
    }
}

int32_t UsbParseConfigDescriptor(struct AcmRawDevice *acm, struct UsbRawConfigDescriptor *config)
{
    uint8_t numInterfaces;
    uint8_t i;
    int32_t ret;
    const struct UsbRawInterface *interface = NULL;

    numInterfaces = config->configDescriptor.bNumInterfaces;
    printf("------numInterfaces = [%hhu]------\n", numInterfaces);
    for (i = 0; i < numInterfaces; i++) {
        interface = config->interface[i];

        printf("------UsbRawClaimInterface start------\n");
        ret = UsbRawClaimInterface(acm->devHandle, i);
        if (ret) {
            printf("%s:%d claim interface %hhu failed\n", __func__, __LINE__, i);
            continue;
        }
        printf("------UsbRawClaimInterface end------\n");
        AcmParaseInterfaceClass(acm, interface, i);
    }

    return HDF_SUCCESS;
}


struct UsbRawTestFunc g_usbRawFunc[] = {
    { USB_RAW_SDK_IF_START_IO, UsbStartIo},
    { USB_RAW_SDK_IF_INIT_001_TEST, CheckRawSdkIfInit001},
    { USB_RAW_SDK_IF_EXIT_001_TEST, CheckRawSdkIfExit001},
    { USB_RAW_SDK_IF_INIT_002_TEST, CheckRawSdkIfInit002},
    { USB_RAW_SDK_IF_EXIT_002_TEST, CheckRawSdkIfExit002},
    { USB_RAW_SDK_IF_INIT_003_TEST, CheckRawSdkIfInit003},
    { USB_RAW_SDK_IF_INIT_004_TEST, CheckRawSdkIfInit004},
    { USB_RAW_SDK_IF_INIT_005_TEST, CheckRawSdkIfInit005},
    { USB_RAW_SDK_IF_OPEN_DEVICE_001_TEST, CheckRawSdkIfOpenDevice001},
    { USB_RAW_SDK_IF_OPEN_DEVICE_002_TEST, CheckRawSdkIfOpenDevice002},
    { USB_RAW_SDK_IF_OPEN_DEVICE_003_TEST, CheckRawSdkIfOpenDevice003},
    { USB_RAW_SDK_IF_OPEN_DEVICE_004_TEST, CheckRawSdkIfOpenDevice004},
    { USB_RAW_SDK_IF_OPEN_DEVICE_005_TEST, CheckRawSdkIfOpenDevice005},
    { USB_RAW_SDK_IF_OPEN_DEVICE_006_TEST, CheckRawSdkIfOpenDevice006},
    { USB_RAW_SDK_IF_RESET_DEVICE_001_TEST, CheckRawSdkIfResetDevice001},
    { USB_RAW_SDK_IF_RESET_DEVICE_002_TEST, CheckRawSdkIfResetDevice002},
    { USB_RAW_SDK_IF_CLOSE_DEVICE_001_TEST, CheckRawSdkIfCloseDevice001},
    { USB_RAW_SDK_IF_CLOSE_DEVICE_002_TEST, CheckRawSdkIfCloseDevice002},
    { USB_RAW_SDK_IF_OPEN_DEVICE_007_TEST, CheckRawSdkIfOpenDevice007},
    { USB_RAW_SDK_IF_GET_CONFIGURATION_001_TEST, CheckRawSdkIfGetConfiguration001},
    { USB_RAW_SDK_IF_GET_CONFIGURATION_002_TEST, CheckRawSdkIfGetConfiguration002},
    { USB_RAW_SDK_IF_GET_CONFIGURATION_003_TEST, CheckRawSdkIfGetConfiguration003},
    { USB_RAW_SDK_IF_GET_CONFIGURATION_004_TEST, CheckRawSdkIfGetConfiguration004},
    { USB_RAW_SDK_IF_GET_DEVICE_001_TEST, CheckRawSdkIfGetDevice001},
    { USB_RAW_SDK_IF_GET_DEVICE_002_TEST, CheckRawSdkIfGetDevice002},
    { USB_RAW_SDK_IF_GET_CONFIG_DESC_001_TEST, CheckRawSdkIfGetConfigDescriptor001},
    { USB_RAW_SDK_IF_GET_CONFIG_DESC_002_TEST, CheckRawSdkIfGetConfigDescriptor002},
    { USB_RAW_SDK_IF_GET_CONFIG_DESC_003_TEST, CheckRawSdkIfGetConfigDescriptor003},
    { USB_RAW_SDK_IF_GET_CONFIG_DESC_004_TEST, CheckRawSdkIfGetConfigDescriptor004},
    { USB_RAW_SDK_IF_SET_CONFIGURATION_001_TEST, CheckRawSdkIfSetConfiguration001},
    { USB_RAW_SDK_IF_SET_CONFIGURATION_002_TEST, CheckRawSdkIfSetConfiguration002},
    { USB_RAW_SDK_IF_SET_CONFIGURATION_003_TEST, CheckRawSdkIfSetConfiguration003},
    { USB_RAW_SDK_IF_SET_CONFIGURATION_004_TEST, CheckRawSdkIfSetConfiguration004},
    { USB_RAW_SDK_IF_SET_CONFIGURATION_005_TEST, CheckRawSdkIfSetConfiguration005},
    { USB_RAW_SDK_IF_SET_CONFIGURATION_006_TEST, CheckRawSdkIfSetConfiguration006},
    { USB_RAW_SDK_IF_SET_CONFIGURATION_007_TEST, CheckRawSdkIfSetConfiguration007},
    { USB_RAW_SDK_IF_SET_CONFIGURATION_008_TEST, CheckRawSdkIfSetConfiguration008},
    { USB_RAW_SDK_IF_GET_DEVICE_DESC_001_TEST, CheckRawSdkIfGetDeviceDescriptor001},
    { USB_RAW_SDK_IF_GET_DEVICE_DESC_002_TEST, CheckRawSdkIfGetDeviceDescriptor002},
    { USB_RAW_SDK_IF_GET_DEVICE_DESC_003_TEST, CheckRawSdkIfGetDeviceDescriptor003},
    { USB_RAW_SDK_IF_GET_DEVICE_DESC_004_TEST, CheckRawSdkIfGetDeviceDescriptor004},
    { USB_RAW_SDK_IF_GET_CONFIG_DESC_005_TEST, CheckRawSdkIfGetConfigDescriptor005},
    { USB_RAW_SDK_IF_GET_DEVICE_DESC_005_TEST, CheckRawSdkIfGetDeviceDescriptor005},
    { USB_RAW_SDK_IF_CLAMIN_INTERFACE_001_TEST, CheckRawSdkIfClaimInterface001},
    { USB_RAW_SDK_IF_CLAMIN_INTERFACE_002_TEST, CheckRawSdkIfClaimInterface002},
    { USB_RAW_SDK_IF_CLAMIN_INTERFACE_003_TEST, CheckRawSdkIfClaimInterface003},
    { USB_RAW_SDK_IF_CLAMIN_INTERFACE_004_TEST, CheckRawSdkIfClaimInterface004},
    { USB_RAW_SDK_IF_CLAMIN_INTERFACE_005_TEST, CheckRawSdkIfClaimInterface005},
    { USB_RAW_SDK_IF_RELEASE_INTERFACE_001_TEST, CheckRawSdkIfReleaseInterface001},
    { USB_RAW_SDK_IF_RELEASE_INTERFACE_002_TEST, CheckRawSdkIfReleaseInterface002},
    { USB_RAW_SDK_IF_RELEASE_INTERFACE_003_TEST, CheckRawSdkIfReleaseInterface003},
    { USB_RAW_SDK_IF_RELEASE_INTERFACE_004_TEST, CheckRawSdkIfReleaseInterface004},
    { USB_RAW_SDK_IF_CLAMIN_INTERFACE_006_TEST,	 CheckRawSdkIfClaimInterface006},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_001_TEST, CheckRawSdkIfAllocRequest001},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_002_TEST, CheckRawSdkIfAllocRequest002},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_003_TEST, CheckRawSdkIfAllocRequest003},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_004_TEST, CheckRawSdkIfAllocRequest004},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_005_TEST, CheckRawSdkIfAllocRequest005},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_006_TEST, CheckRawSdkIfAllocRequest006},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_007_TEST, CheckRawSdkIfAllocRequest007},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_008_TEST, CheckRawSdkIfAllocRequest008},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_010_TEST, CheckRawSdkIfAllocRequest010},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_011_TEST, CheckRawSdkIfAllocRequest011},
    { USB_RAW_SDK_IF_FREE_REQUEST_006_TEST, CheckRawSdkIfFreeRequest006},
    { USB_RAW_SDK_IF_FILL_ISO_REQUEST_001_TEST, CheckRawSdkIfFillIsoRequest001},
    { USB_RAW_SDK_IF_FILL_ISO_REQUEST_002_TEST, CheckRawSdkIfFillIsoRequest002},
    { USB_RAW_SDK_IF_FILL_ISO_REQUEST_003_TEST, CheckRawSdkIfFillIsoRequest003},
    { USB_RAW_SDK_IF_FILL_ISO_REQUEST_004_TEST, CheckRawSdkIfFillIsoRequest004},
    { USB_RAW_SDK_IF_FILL_ISO_REQUEST_005_TEST, CheckRawSdkIfFillIsoRequest005},
    { USB_RAW_SDK_IF_FILL_ISO_REQUEST_006_TEST, CheckRawSdkIfFillIsoRequest006},
    { USB_RAW_SDK_IF_FREE_REQUEST_001_TEST, CheckRawSdkIfFreeRequest001},
    { USB_RAW_SDK_IF_FREE_REQUEST_002_TEST, CheckRawSdkIfFreeRequest002},
    { USB_RAW_SDK_IF_FREE_REQUEST_003_TEST, CheckRawSdkIfFreeRequest003},
    { USB_RAW_SDK_IF_FREE_REQUEST_004_TEST, CheckRawSdkIfFreeRequest004},
    { USB_RAW_SDK_IF_FREE_REQUEST_005_TEST, CheckRawSdkIfFreeRequest005},
    { USB_RAW_SDK_IF_ALLOC_REQUEST_009_TEST, CheckRawSdkIfAllocRequest009},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_001_TEST, CheckRawSdkIfGetDescriptor001},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_002_TEST, CheckRawSdkIfGetDescriptor002},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_003_TEST, CheckRawSdkIfGetDescriptor003},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_004_TEST, CheckRawSdkIfGetDescriptor004},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_005_TEST, CheckRawSdkIfGetDescriptor005},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_006_TEST, CheckRawSdkIfGetDescriptor006},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_007_TEST, CheckRawSdkIfGetDescriptor007},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_008_TEST, CheckRawSdkIfGetDescriptor008},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_009_TEST, CheckRawSdkIfGetDescriptor009},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_010_TEST, CheckRawSdkIfGetDescriptor010},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_011_TEST, CheckRawSdkIfGetDescriptor011},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_012_TEST, CheckRawSdkIfGetDescriptor012},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_013_TEST, CheckRawSdkIfGetDescriptor013},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_014_TEST, CheckRawSdkIfGetDescriptor014},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_015_TEST, CheckRawSdkIfGetDescriptor015},
    { USB_RAW_SDK_IF_GET_DESCRIPTION_016_TEST, CheckRawSdkIfGetDescriptor016},
    { USB_RAW_SDK_IF_FILL_BULK_REQUEST_001_TEST, CheckRawSdkIfFillBulkRequest001},
    { USB_RAW_SDK_IF_FILL_BULK_REQUEST_002_TEST, CheckRawSdkIfFillBulkRequest002},
    { USB_RAW_SDK_IF_FILL_INT_REQUEST_001_TEST, CheckRawSdkIfFillInterruptRequest001},
    { USB_RAW_SDK_IF_FILL_INT_REQUEST_002_TEST, CheckRawSdkIfFillInterruptRequest002},
    { USB_RAW_SDK_IF_FILL_INT_REQUEST_003_TEST, CheckRawSdkIfFillInterruptRequest003},
    { USB_RAW_SDK_IF_FILL_INT_REQUEST_004_TEST, CheckRawSdkIfFillInterruptRequest004},
    { USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_001_TEST, CheckRawSdkIfFillControlRequest001},
    { USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_002_TEST, CheckRawSdkIfFillControlRequest002},
    { USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_003_TEST, CheckRawSdkIfFillControlRequest003},
    { USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_004_TEST, CheckRawSdkIfFillControlRequest004},
    { USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_005_TEST, CheckRawSdkIfFillControlRequest005},
    { USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_006_TEST, CheckRawSdkIfFillControlRequest006},
    { USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_007_TEST, CheckRawSdkIfFillControlRequest007},
    { USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_008_TEST, CheckRawSdkIfFillControlRequest008},
    { USB_RAW_SDK_IF_FILL_CONTROL_SETUP_001_TEST, CheckRawSdkIfFillControlSetup001},
    { USB_RAW_SDK_IF_FILL_CONTROL_SETUP_002_TEST, CheckRawSdkIfFillControlSetup002},
    { USB_RAW_SDK_IF_FILL_CONTROL_SETUP_003_TEST, CheckRawSdkIfFillControlSetup003},
    { USB_RAW_SDK_IF_FILL_CONTROL_SETUP_004_TEST, CheckRawSdkIfFillControlSetup004},
    { USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_001_TEST, CheckRawSdkIfSendControlRequest001},
    { USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_002_TEST, CheckRawSdkIfSendControlRequest002},
    { USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_003_TEST, CheckRawSdkIfSendControlRequest003},
    { USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_004_TEST, CheckRawSdkIfSendControlRequest004},
    { USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_005_TEST, CheckRawSdkIfSendControlRequest005},
    { USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_006_TEST, CheckRawSdkIfSendControlRequest006},
    { USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_007_TEST, CheckRawSdkIfSendControlRequest007},
    { USB_RAW_SDK_IF_SEND_BULK_REQUEST_001_TEST, CheckRawSdkIfSendBulkRequest001},
    { USB_RAW_SDK_IF_SEND_BULK_REQUEST_002_TEST, CheckRawSdkIfSendBulkRequest002},
    { USB_RAW_SDK_IF_SEND_BULK_REQUEST_003_TEST, CheckRawSdkIfSendBulkRequest003},
    { USB_RAW_SDK_IF_SEND_BULK_REQUEST_004_TEST, CheckRawSdkIfSendBulkRequest004},
    { USB_RAW_SDK_IF_SEND_BULK_REQUEST_005_TEST, CheckRawSdkIfSendBulkRequest005},
    { USB_RAW_SDK_IF_SEND_INT_REQUEST_001_TEST, CheckRawSdkIfSendInterruptRequest001},
    { USB_RAW_SDK_IF_SEND_INT_REQUEST_002_TEST, CheckRawSdkIfSendInterruptRequest002},
    { USB_RAW_SDK_IF_SEND_INT_REQUEST_003_TEST, CheckRawSdkIfSendInterruptRequest003},
    { USB_RAW_SDK_IF_SEND_INT_REQUEST_004_TEST, CheckRawSdkIfSendInterruptRequest004},
    { USB_RAW_SDK_IF_FILL_BULK_REQUEST_003_TEST, CheckRawSdkIfFillBulkRequest003},
    { USB_RAW_SDK_IF_FILL_BULK_REQUEST_004_TEST, CheckRawSdkIfFillBulkRequest004},
    { USB_RAW_SDK_IF_FILL_INT_REQUEST_005_TEST, CheckRawSdkIfFillInterruptRequest005},
    { USB_RAW_SDK_IF_SUBMIT_REQUEST_001_TEST, CheckRawSdkIfSubmitRequest001},
    { USB_RAW_SDK_IF_SUBMIT_REQUEST_002_TEST, CheckRawSdkIfSubmitRequest002},
    { USB_RAW_SDK_IF_SUBMIT_REQUEST_003_TEST, CheckRawSdkIfSubmitRequest003},
    { USB_RAW_SDK_IF_SUBMIT_REQUEST_004_TEST, CheckRawSdkIfSubmitRequest004},
    { USB_RAW_SDK_IF_CANCEL_REQUEST_001_TEST, CheckRawSdkIfCancelRequest001},
    { USB_RAW_SDK_IF_CANCEL_REQUEST_002_TEST, CheckRawSdkIfCancelRequest002},
    { USB_RAW_SDK_IF_CANCEL_REQUEST_003_TEST, CheckRawSdkIfCancelRequest003},
    { USB_RAW_SDK_IF_CANCEL_REQUEST_004_TEST, CheckRawSdkIfCancelRequest004},
    { USB_RAW_SDK_IF_STOP_IO, UsbStopIo},

};

int32_t UsbRawTestEntry(int32_t cmd)
{
    int32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;

    if (g_initFlag == false) {
#ifdef LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY
        struct UsbGetDevicePara paraData;
        struct usb_device *usbPnpDevice = NULL;
        paraData.type = USB_PNP_DEVICE_VENDOR_PRODUCT_TYPE;
        paraData.vendorId = USB_DEVICE_VENDOR_ID;
        paraData.productId = USB_DEVICE_PRODUCT_ID;
        usbPnpDevice = UsbPnpNotifyGetUsbDevice(paraData);
        if (usbPnpDevice == NULL) {
            HDF_LOGE("%s:%d UsbPnpNotifyGetUsbDevice is NULL!", __func__, __LINE__);
            return HDF_ERR_INVALID_PARAM;
        }
        g_acm->busNum = usbPnpDevice->address;
        g_acm->devAddr = usbPnpDevice->port_no;
        HDF_LOGE("%s: busNum %d++devAddr %d", __func__, g_acm->busNum, g_acm->devAddr);
        g_initFlag = true;
#else
        HDF_LOGE("%s:%d pnp notify module is not support!", __func__, __LINE__);
        return HDF_ERR_NOT_SUPPORT;
#endif
    }
    for (i = 0; i < sizeof(g_usbRawFunc) / sizeof(g_usbRawFunc[0]); i++) {
        if (cmd == g_usbRawFunc[i].type && g_usbRawFunc[i].Func != NULL) {
            ret = g_usbRawFunc[i].Func();
            HDF_LOGE("%s: cmd %d ret %d", __func__, cmd, ret);
            break;
        }
    }
    return ret;
}

int32_t CheckRawSdkIfInit001(void)
{
    int32_t ret;

    ret = UsbRawInit(&g_session);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->session = g_session;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfExit001(void)
{
    int32_t ret;

    ret = UsbRawExit(g_acm->session);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_session = NULL;
    g_acm->session = g_session;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfInit002(void)
{
    int32_t ret;

    ret = UsbRawInit(NULL);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfExit002(void)
{
    int32_t ret;

    ret = UsbRawExit(NULL);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfInit003(void)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < USB_LOOP_NUM; i++) {
        ret = UsbRawInit(&g_session);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        g_acm->session = g_session;
        ret = UsbRawExit(g_acm->session);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        g_session = NULL;
        g_acm->session = g_session;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfInit004(void)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < USB_LOOP_NUM; i++) {
        ret = UsbRawInit(NULL);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        ret = UsbRawExit(NULL);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfInit005(void)
{
    int32_t ret;

    ret = UsbRawInit(&g_session);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->session = g_session;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfOpenDevice001(void)
{
    g_devHandle = UsbRawOpenDevice(NULL, g_acm->busNum, g_acm->devAddr);
    if (g_devHandle) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->devHandle = g_devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfOpenDevice002(void)
{
    uint8_t busNum = 1U;
    uint8_t devAddr = 100U;
    g_devHandle = UsbRawOpenDevice(g_acm->session, busNum, devAddr);
    if (g_devHandle) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->devHandle = g_devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfOpenDevice003(void)
{
    uint8_t busNum = 1U;
    uint8_t devAddr = 255U;
    g_devHandle = UsbRawOpenDevice(g_acm->session, busNum, devAddr);
    if (g_devHandle) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->devHandle = g_devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfOpenDevice004(void)
{
    uint8_t busNum = 100U;
    uint8_t devAddr = 2U;
    g_devHandle = UsbRawOpenDevice(g_acm->session, busNum, devAddr);
    if (g_devHandle) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->devHandle = g_devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfOpenDevice005(void)
{
    uint8_t busNum = 255U;
    uint8_t devAddr = 2U;
    g_devHandle = UsbRawOpenDevice(g_acm->session, busNum, devAddr);
    if (g_devHandle) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->devHandle = g_devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfOpenDevice006(void)
{
    g_devHandle = UsbRawOpenDevice(g_acm->session, g_acm->busNum, g_acm->devAddr);
    if (g_devHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->devHandle = g_devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfResetDevice001(void)
{
    int32_t ret;

    ret = UsbRawResetDevice(NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfResetDevice002(void)
{
    int32_t ret;

    ret = UsbRawResetDevice(g_acm->devHandle);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfCloseDevice001(void)
{
    int32_t ret;

    ret = UsbRawCloseDevice(NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfCloseDevice002(void)
{
    int32_t ret;

    ret = UsbRawCloseDevice(g_acm->devHandle);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->devHandle = NULL;
    g_devHandle = g_acm->devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfOpenDevice007(void)
{
    g_devHandle = UsbRawOpenDevice(g_session, g_acm->busNum, g_acm->devAddr);
    if (g_devHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->devHandle = g_devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetConfiguration001(void)
{
    int32_t ret;

    ret = UsbRawGetConfiguration(NULL, &g_activeConfig);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetConfiguration002(void)
{
    int32_t ret;

    ret = UsbRawGetConfiguration(g_acm->devHandle, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetConfiguration003(void)
{
    int32_t ret;

    ret = UsbRawGetConfiguration(NULL, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetConfiguration004(void)
{
    int32_t ret;

    ret = UsbRawGetConfiguration(g_acm->devHandle, &g_activeConfig);
    printf("------g_activeConfig = [%d]------\n", g_activeConfig);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDevice001(void)
{
    g_dev = UsbRawGetDevice(NULL);
    if (g_dev) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDevice002(void)
{
    g_dev = UsbRawGetDevice(g_acm->devHandle);
    if (g_dev == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}


int32_t CheckRawSdkIfClaimInterface002(void)
{
    int32_t ret;
    int32_t interfaceNumber = 1;

    ret = UsbRawClaimInterface(g_devHandle, interfaceNumber);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfClaimInterface003(void)
{
    int32_t ret;
    int32_t interfaceNumber = 0;

    ret = UsbRawClaimInterface(g_devHandle, interfaceNumber);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfClaimInterface004(void)
{
    int32_t ret;
    int32_t interfaceNumber = USB_MAX_BYTE;

    ret = UsbRawClaimInterface(g_devHandle, interfaceNumber);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDeviceDescriptor002(void)
{
    int32_t ret;

    ret = UsbRawGetDeviceDescriptor(g_dev, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDeviceDescriptor003(void)
{
    int32_t ret;

    ret = UsbRawGetDeviceDescriptor(NULL, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDeviceDescriptor004(void)
{
    struct UsbDeviceDescriptor desc;
    int32_t ret;

    ret = UsbRawGetDeviceDescriptor(g_dev, &desc);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetConfigDescriptor005(void)
{
    int32_t ret;

    printf("------g_activeConfig = [%d]------\n", g_activeConfig);
    ret = UsbRawGetConfigDescriptor(g_dev, g_activeConfig, &g_acm->config);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDeviceDescriptor005(void)
{
    struct UsbDeviceDescriptor desc;
    int32_t ret;

    ret = UsbRawGetDeviceDescriptor(g_dev, &desc);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetConfigDescriptor001(void)
{
    int32_t ret;

    ret = UsbRawGetConfigDescriptor(NULL, g_activeConfig, &g_acm->config);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetConfigDescriptor002(void)
{
    int32_t ret;

    ret = UsbRawGetConfigDescriptor(g_dev, g_activeConfig, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetConfigDescriptor003(void)
{
    int32_t ret;

    ret = UsbRawGetConfigDescriptor(NULL, g_activeConfig, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetConfigDescriptor004(void)
{
    int32_t ret;

    printf("------g_activeConfig = [%d]------\n", g_activeConfig);
    ret = UsbRawGetConfigDescriptor(g_dev, g_activeConfig, &g_acm->config);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSetConfiguration001(void)
{
    int32_t ret;
    int32_t config = 0;

    ret = UsbRawSetConfiguration(NULL, config);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSetConfiguration002(void)
{
    int32_t ret;
    int32_t config = 0;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSetConfiguration003(void)
{
    int32_t ret;
    int32_t config = 1;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSetConfiguration004(void)
{
    int32_t ret;
    int32_t config = 10;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

int32_t CheckRawSdkIfSetConfiguration005(void)
{
    int32_t ret;
    int32_t config = 100;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

int32_t CheckRawSdkIfSetConfiguration006(void)
{
    int32_t ret;
    int32_t config = 200;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

int32_t CheckRawSdkIfSetConfiguration007(void)
{
    int32_t ret;
    int32_t config = USB_MAX_BYTE;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

int32_t CheckRawSdkIfSetConfiguration008(void)
{
    int32_t ret;
    int32_t config = 0;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}
