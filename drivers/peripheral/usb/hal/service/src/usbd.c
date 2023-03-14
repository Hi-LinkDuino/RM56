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

#include "usbd.h"
#include "hdf_log.h"
#include "securec.h"
#include "hdf_device_object.h"
#include "usb_ddk_pnp_loader.h"
#include "usbd_dispatcher.h"

#define HDF_LOG_TAG Usbd

#define HEX_NUM_09 0x09

const int32_t DEFAULT_PORT_ID = 1;
const int32_t DEFAULT_POWER_ROLE = 2;
const int32_t DEFAULT_DATA_ROLE = 2;

struct HdfIoService *g_usbPnpServ = NULL;
struct HdfDevEventlistener g_usbPnpListener = {0};

int32_t HdfDeviceRegisterEventListener(struct HdfIoService *target, struct HdfDevEventlistener *listener);

/* function defind in usbd_publisher.c */
int32_t UsbdRealseDevices(struct UsbdService *service);
int32_t UsbdDeviceCreateAndAttach(struct UsbdService *service, uint8_t busNum, uint8_t devAddr);
int32_t UsbdDeviceDettach(struct UsbdService *service, uint8_t busNum, uint8_t devAddr);
int32_t UsbdRemoveBusDev(struct UsbdService *service, uint8_t busNum);

static int32_t UsbdPnpLoaderEventReceived(void *priv, uint32_t id, struct HdfSBuf *data)
{
    struct UsbPnpNotifyMatchInfoTable *infoTable = NULL;
    struct UsbdService *super = (struct UsbdService *)priv;
    if (super == NULL) {
        HDF_LOGE("%{public}s priv super is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (USB_PNP_DRIVER_GADGET_ADD == id) {
        NotifySubscriberDevice(super->subscriber, ACT_UPDEVICE, 0, 0);
        return HDF_SUCCESS;
    } else if (USB_PNP_DRIVER_GADGET_REMOVE == id) {
        NotifySubscriberDevice(super->subscriber, ACT_DOWNDEVICE, 0, 0);
        return HDF_SUCCESS;
    }
    uint32_t infoSize;
    bool flag = HdfSbufReadBuffer(data, (const void **)(&infoTable), &infoSize);
    if ((flag == false) || (infoTable == NULL)) {
        HDF_LOGE(
            "%{public}s: fail to read infoTable in event data, "
            "flag=%{public}d, infoTable=%{public}p",
            __func__, flag, infoTable);
        return HDF_ERR_INVALID_PARAM;
    }
    if (infoTable->deviceInfo.deviceClass == HEX_NUM_09) {
        HDF_LOGI("%{public}s:%{public}d hub device", __func__, __LINE__);
        if (USB_PNP_NOTIFY_REMOVE_DEVICE == id) {
            HDF_LOGI("%{public}s:%{public}d UsbdRemoveBusDev busNum:%{public}d", __func__, __LINE__, infoTable->busNum);
            UsbdRemoveBusDev(super, infoTable->busNum);
        }
        return HDF_SUCCESS;
    }
    if (id == USB_PNP_NOTIFY_ADD_DEVICE) {
        UsbdDeviceCreateAndAttach(super, infoTable->busNum, infoTable->devNum);
        NotifySubscriberDevice(super->subscriber, ACT_DEVUP, infoTable->busNum, infoTable->devNum);
    } else if (id == USB_PNP_NOTIFY_REMOVE_DEVICE) {
        UsbdDeviceDettach(super, infoTable->busNum, infoTable->devNum);
        NotifySubscriberDevice(super->subscriber, ACT_DEVDOWN, infoTable->busNum, infoTable->devNum);
    }
    return HDF_SUCCESS;
}

static int32_t UsbdEventHandle(const struct UsbdService *inst)
{
    g_usbPnpServ = HdfIoServiceBind(USB_PNP_NOTIFY_SERVICE_NAME);
    g_usbPnpListener.callBack = UsbdPnpLoaderEventReceived;
    g_usbPnpListener.priv = (void *)(inst);

    if (g_usbPnpServ == NULL) {
        HDF_LOGE("%{public}s: HdfIoServiceBind faile.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t status;
    status = HdfDeviceRegisterEventListener(g_usbPnpServ, &g_usbPnpListener);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("HdfDeviceRegisterEventListener faile status=%{public}d", status);
    }

    return status;
}

static int32_t UsbdDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%{public}s:%{public}d device is null", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    struct UsbdService *dev = (struct UsbdService *)OsalMemCalloc(sizeof(struct UsbdService));
    if (dev == NULL) {
        HDF_LOGE("%{public}s: Alloc dev device failed", __func__);
        return HDF_FAILURE;
    }
    if (memset_s(dev, sizeof(struct UsbdService), 0, sizeof(struct UsbdService)) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed ", __func__, __LINE__);
        OsalMemFree(dev);
        return HDF_FAILURE;
    }
    if (OsalMutexInit(&dev->lock) != HDF_SUCCESS) {
        HDF_LOGE(" init lock fail!");
        OsalMemFree(dev);
        return HDF_FAILURE;
    }
    int32_t ret = HDF_SUCCESS;
    struct UsbPnpNotifyServiceInfo *info = (struct UsbPnpNotifyServiceInfo *)device->priv;
    if (info != NULL) {
        UsbdDeviceCreateAndAttach(dev, info->busNum, info->devNum);
    }

    if (HdfDeviceObjectSetInterfaceDesc(device, HDF_USB_USBD_DESC) != HDF_SUCCESS) {
        HDF_LOGE(" Set Desc fail!");
        OsalMemFree(dev);
        return HDF_FAILURE;
    }

    device->service = &(dev->service);
    device->service->Dispatch = UsbdServiceDispatch;
    dev->device = device;
    ret = UsbdEventHandle(dev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:  UsbdEventHandle ret=%{public}d", __func__, ret);
    }
    return ret;
}

static int32_t UsbdDriverInit(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%{public}s:%{public}d device is null", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return HDF_SUCCESS;
}

static int32_t UsbdEventHandleRelease(void)
{
    int32_t ret = HDF_SUCCESS;
    if (g_usbPnpServ != NULL) {
        ret = HdfDeviceUnregisterEventListener(g_usbPnpServ, &g_usbPnpListener);
        HdfIoServiceRecycle(g_usbPnpServ);
    }
    g_usbPnpServ = NULL;
    g_usbPnpListener.callBack = NULL;
    g_usbPnpListener.priv = NULL;

    return ret;
}

static void UsbdDriverRelease(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%{public}s: device is NULL", __func__);
        return;
    }
    int32_t ret = UsbdEventHandleRelease();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:  UsbdEventHandleRelease ret=%{public}d", __func__, ret);
    }
    struct UsbdService *dev = (struct UsbdService *)device->service;
    UsbdRealseDevices(dev);
}

struct HdfDriverEntry g_usbdDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "usbd",
    .Bind = UsbdDriverBind,
    .Init = UsbdDriverInit,
    .Release = UsbdDriverRelease,
};
HDF_INIT(g_usbdDriverEntry);

static int32_t HdfReadDevice(struct UsbdService *service, int32_t *count, int32_t *size, struct HdfSBuf *reply)
{
    int32_t busNum;
    int32_t devNum;
    uint8_t devClass;
    uint8_t subClass;
    uint8_t protocol;
    uint8_t status;
    if (!HdfSbufReadInt32(reply, &busNum)) {
        HDF_LOGE("%{public}s: fail to get service call reply", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(reply, &devNum)) {
        HDF_LOGE("%{public}s: fail to get service call reply", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(reply, &devClass)) {
        HDF_LOGE("%{public}s:%{public}d fail to get service call reply", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (!HdfSbufReadUint8(reply, &subClass)) {
        HDF_LOGE("%{public}s:%{public}d fail to get service call reply", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (!HdfSbufReadUint8(reply, &protocol)) {
        HDF_LOGE("%{public}s:%{public}d fail to get service call reply", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (!HdfSbufReadUint8(reply, &status)) {
        HDF_LOGE("%{public}s:%{public}d fail to get service call reply", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    HDF_LOGI(
        "%{public}s:%{public}d OnStart get "
        "device[%{public}d]:%{public}d:%{public}d status:%{pubic}d "
        "class:%{public}d subClass:%{public}d protocol:%{pubic}d",
        __func__, __LINE__, *count, busNum, devNum, status, devClass, subClass, protocol);
    if (devClass != HEX_NUM_09) {
        UsbdDeviceCreateAndAttach(service, busNum, devNum);
        NotifySubscriberDevice(service->subscriber, ACT_DEVUP, busNum, devNum);
        ++(*size);
    }
    ++(*count);
    return HDF_SUCCESS;
}

static int32_t UsbdAddDevicesOnStart(struct UsbdService *service)
{
    struct HdfIoService *usbPnpServ = HdfIoServiceBind(USB_PNP_NOTIFY_SERVICE_NAME);
    if (service == NULL || usbPnpServ == NULL) {
        HDF_LOGE(
            "%{public}s:%{public}d service is NULL or HdfIoServiceBind.faile "
            "serv:%{public}s.",
            __func__, __LINE__, USB_PNP_NOTIFY_SERVICE_NAME);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL) {
        HDF_LOGE("%{public}s: fail to obtain sbuf data", __func__);
        return HDF_DEV_ERR_NO_MEMORY;
    }
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (reply == NULL) {
        HDF_LOGE("%{public}s: fail to obtain sbuf reply", __func__);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        HdfIoServiceRecycle(usbPnpServ);
        return HDF_DEV_ERR_NO_MEMORY;
    }
    int32_t ret = usbPnpServ->dispatcher->Dispatch(&usbPnpServ->object, USB_PNP_DRIVER_GETDEVICES, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: fail to send serivice call, ret=%{public}d", __func__, ret);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        HdfIoServiceRecycle(usbPnpServ);
        return ret;
    }

    int32_t count = 0;
    int32_t size = 0;
    do {
        ret = HdfReadDevice(service, &count, &size, reply);
    } while (ret == HDF_SUCCESS);

    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    HdfIoServiceRecycle(usbPnpServ);
    return ret;
}

int32_t BindUsbSubscriber(struct UsbdService *service, struct UsbdSubscriber *subscriber)
{
    if (service == NULL) {
        HDF_LOGE("%{public}s  service is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    service->subscriber = subscriber;
    UsbdAddDevicesOnStart(service);
    return HDF_SUCCESS;
}

int32_t UnbindUsbSubscriber(struct UsbdService *service)
{
    if (service == NULL) {
        HDF_LOGE("%{public}s service is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (service->subscriber != NULL) {
        HDF_LOGW("%{public}s:%{public}d entry", __func__, __LINE__);
        OsalMemFree(service->subscriber);
        service->subscriber = NULL;
    }
    return HDF_SUCCESS;
}
