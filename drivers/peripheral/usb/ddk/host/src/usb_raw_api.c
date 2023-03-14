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

#include "usb_raw_api.h"
#include "usb_raw_api_library.h"

#define HDF_LOG_TAG USB_RAW_API

int32_t UsbRawInit(struct UsbSession **session)
{
    return RawInit(session);
}

int32_t UsbRawExit(const struct UsbSession *session)
{
    return RawExit(session);
}

UsbRawHandle *UsbRawOpenDevice(const struct UsbSession *session, uint8_t busNum, uint8_t usbAddr)
{
    return (UsbRawHandle *)RawOpenDevice(session, busNum, usbAddr);
}

int32_t UsbRawCloseDevice(const UsbRawHandle *devHandle)
{
    if (devHandle == NULL) {
        HDF_LOGE("%s:%d devHandle is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawCloseDevice((const struct UsbDeviceHandle *)devHandle);
}

int32_t UsbRawSendControlRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbControlRequestData *requestData)
{
    if ((request == NULL) || (devHandle == NULL) || (requestData == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawSendControlRequest((struct UsbHostRequest *)request, (const struct UsbDeviceHandle *)devHandle,
        requestData);
}

int32_t UsbRawSendBulkRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRequestData *requestData)
{
    if ((request == NULL) || (devHandle == NULL) || (requestData == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawSendBulkRequest((const struct UsbHostRequest *)request, (const struct UsbDeviceHandle *)devHandle,
        requestData);
}

int32_t UsbRawSendInterruptRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRequestData *requestData)
{
    if ((request == NULL) || (devHandle == NULL) || (requestData == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawSendInterruptRequest((const struct UsbHostRequest *)request, (const struct UsbDeviceHandle *)devHandle,
        requestData);
}

int32_t UsbRawGetConfigDescriptor(const UsbRawDevice *rawDev, uint8_t configIndex,
    struct UsbRawConfigDescriptor **config)
{
    if ((rawDev == NULL) || (config == NULL)) {
        HDF_LOGE("%s:%d rawDev or config is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawGetConfigDescriptor((const struct UsbDevice *)rawDev, configIndex, config);
}

void UsbRawFreeConfigDescriptor(const struct UsbRawConfigDescriptor *config)
{
    if (config == NULL) {
        HDF_LOGE("%s:%d config is NULL", __func__, __LINE__);
        return;
    }

    RawClearConfiguration((struct UsbRawConfigDescriptor *)config);
    RawUsbMemFree((void *)config);
}

int32_t UsbRawGetConfiguration(const UsbRawHandle *devHandle, int32_t *config)
{
    if ((devHandle == NULL) || (config == NULL)) {
        HDF_LOGE("%s:%d dev or config is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawGetConfiguration((const struct UsbDeviceHandle *)devHandle, config);
}

int32_t UsbRawSetConfiguration(const UsbRawHandle *devHandle, int32_t config)
{
    if (devHandle == NULL) {
        HDF_LOGE("%s:%d dev is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawSetConfiguration((const struct UsbDeviceHandle *)devHandle, config);
}

int32_t UsbRawGetDescriptor(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRawDescriptorParam *param, const unsigned char *data)
{
    if ((request == NULL) || (devHandle == NULL) || (param == NULL) || (data == NULL)) {
        HDF_LOGE("%s:%d request or devHandle is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawGetDescriptor((const struct UsbHostRequest *)request, (const struct UsbDeviceHandle *)devHandle,
        param, data);
}

UsbRawDevice *UsbRawGetDevice(const UsbRawHandle *devHandle)
{
    if (devHandle == NULL) {
        HDF_LOGE("%s:%d devHandle is NULL ", __func__, __LINE__);
        return NULL;
    }

    return (UsbRawDevice *)RawGetDevice((const struct UsbDeviceHandle *)devHandle);
}

int32_t UsbRawGetDeviceDescriptor(const UsbRawDevice *rawDev, struct UsbDeviceDescriptor *desc)
{
    if ((rawDev == NULL) || (desc == NULL)) {
        HDF_LOGE("%s:%d rawDev or desc is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawGetDeviceDescriptor((const struct UsbDevice *)rawDev, desc);
}

int32_t UsbRawClaimInterface(const UsbRawHandle *devHandle, int32_t interfaceNumber)
{
    if (devHandle == NULL) {
        HDF_LOGE("%s:%d devHandle is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawClaimInterface((struct UsbDeviceHandle *)devHandle, interfaceNumber);
}

int32_t UsbRawReleaseInterface(const UsbRawHandle *devHandle, int32_t interfaceNumber)
{
    if (devHandle == NULL) {
        HDF_LOGE("%s:%d devHandle is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawReleaseInterface((struct UsbDeviceHandle *)devHandle, interfaceNumber);
}

int32_t UsbRawResetDevice(const UsbRawHandle *devHandle)
{
    if (devHandle == NULL) {
        HDF_LOGE("%s:%d devHandle is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawResetDevice((const struct UsbDeviceHandle *)devHandle);
}

struct UsbRawRequest *UsbRawAllocRequest(const UsbRawHandle *devHandle, int32_t isoPackets, int32_t length)
{
    if (devHandle == NULL) {
        HDF_LOGE("%s:%d devHandle is NULL", __func__, __LINE__);
        return NULL;
    }

    return (struct UsbRawRequest *)RawAllocRequest((const struct UsbDeviceHandle *)devHandle, isoPackets, length);
}

int32_t UsbRawFreeRequest(const struct UsbRawRequest *request)
{
    if (request == NULL) {
        HDF_LOGE("%s:%d request is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawFreeRequest((const struct UsbHostRequest *)request);
}

int32_t UsbRawFillBulkRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRawFillRequestData *fillData)
{
    if ((request == NULL) || (devHandle == NULL) || (fillData == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawFillBulkRequest((struct UsbHostRequest *)request, (const struct UsbDeviceHandle *)devHandle,
        (const struct UsbFillRequestData *)fillData);
}

int32_t UsbRawFillControlSetup(const unsigned char *setup, const struct UsbControlRequestData *requestData)
{
    if ((setup == NULL) || (requestData == NULL)) {
        HDF_LOGE("%s:%d setup or requestData is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawFillControlSetup(setup, requestData);
}

int32_t UsbRawFillControlRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRawFillRequestData *fillData)
{
    if ((request == NULL) || (devHandle == NULL) || (fillData == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawFillControlRequest((struct UsbHostRequest *)request, (const struct UsbDeviceHandle *)devHandle,
        (const struct UsbFillRequestData *)fillData);
}

int32_t UsbRawFillInterruptRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRawFillRequestData *fillData)
{
    if ((request == NULL) || (devHandle == NULL) || (fillData == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawFillInterruptRequest((struct UsbHostRequest *)request, (const struct UsbDeviceHandle *)devHandle,
        (const struct UsbFillRequestData *)fillData);
}

int32_t UsbRawFillIsoRequest(const struct UsbRawRequest *request, const UsbRawHandle *devHandle,
    const struct UsbRawFillRequestData *fillData)
{
    if ((request == NULL) || (devHandle == NULL) || (fillData == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawFillIsoRequest((struct UsbHostRequest *)request, (const struct UsbDeviceHandle *)devHandle,
        (const struct UsbFillRequestData *)fillData);
}

int32_t UsbRawSubmitRequest(const struct UsbRawRequest *request)
{
    if (request == NULL) {
        HDF_LOGE("%s:%d request is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawSubmitRequest((const struct UsbHostRequest *)request);
}

int32_t UsbRawCancelRequest(const struct UsbRawRequest *request)
{
    if (request == NULL) {
        HDF_LOGE("%s:%d request is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawCancelRequest((const struct UsbHostRequest *)request);
}

int32_t UsbRawHandleRequests(const UsbRawHandle *devHandle)
{
    if (devHandle == NULL) {
        HDF_LOGE("%s:%d devHandle is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawHandleRequest((const struct UsbDeviceHandle *)devHandle);
}
