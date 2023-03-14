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

#include "usbd_publisher.h"
#include "utils/hdf_log.h"

#define HDF_LOG_TAG usbd_publisher

void NotifySubscriberDevice(const struct UsbdSubscriber *subscriber, enum UsbdDeviceAction act, int32_t busNum,
    int32_t devNum)
{
    if (subscriber == NULL) {
        HDF_LOGE("%{public}s: subscriber is NULL", __func__);
        return;
    }
    struct HdfRemoteService *service = subscriber->remoteService;

    if (HdfRemoteServiceSetInterfaceDesc(service, "hdf.usb.usbdsubscriber") == false) {
        HDF_LOGE("%{public}s:%{public}d set desc fail\n", __func__, __LINE__);
        return;
    }

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s failed to obtain hdf sbuf", __func__);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return;
    }

    if (HdfRemoteServiceWriteInterfaceToken(service, data) == false) {
        HDF_LOGE("%{public}s:%{public}d write interface token fail\n", __func__, __LINE__);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return;
    }

    HdfSbufWriteInt32(data, act);
    HdfSbufWriteInt32(data, busNum);
    HdfSbufWriteInt32(data, devNum);
    int32_t ret = service->dispatcher->Dispatch(service, CMD_NOTIFY_SUBSCRIBER_DEVICE_EVENT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed to notify subscriber, ret: %{public}d", __func__, ret);
    }
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
}

void NotifyUsbPortSubscriber(const struct UsbdSubscriber *subscriber, int32_t portId, int32_t powerRole,
    int32_t dataRole, int32_t mode)
{
    if (subscriber == NULL) {
        HDF_LOGE("%{public}s: subscriber is NULL", __func__);
        return;
    }
    int32_t ret;
    struct HdfRemoteService *service = subscriber->remoteService;

    if (HdfRemoteServiceSetInterfaceDesc(service, "hdf.usb.usbdsubscriber") == false) {
        HDF_LOGE("%{public}s:%{public}d set desc fail\n", __func__, __LINE__);
        return;
    }

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s failed to obtain hdf sbuf", __func__);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return;
    }

    if (HdfRemoteServiceWriteInterfaceToken(service, data) == false) {
        HDF_LOGE("%{public}s:%{public}d write interface token fail\n", __func__, __LINE__);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return;
    }

    HdfSbufWriteInt32(data, portId);
    HdfSbufWriteInt32(data, powerRole);
    HdfSbufWriteInt32(data, dataRole);
    HdfSbufWriteInt32(data, mode);

    ret = service->dispatcher->Dispatch(service, CMD_NOTIFY_PORT_CHANGED, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed to notify subscriber, ret: %{public}d", __func__, ret);
    } else {
        HDF_LOGI("%{public}s: succeed to notify subscriber", __func__);
    }
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
}
