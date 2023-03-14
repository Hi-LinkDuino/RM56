/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "uart_dispatch_sample.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "uart_core.h"
#include "uart_pl011_sample.h"

#define HDF_LOG_TAG uart_dispatch_sample

static int32_t SampleDispatchWrite(struct UartDevice *device, struct HdfSBuf *txBuf)
{
    uint32_t idx;
    uint32_t dataSize = 0;
    const uint8_t *data = NULL;
    struct UartRegisterMap *regMap = (struct UartRegisterMap *)device->resource.physBase;

    if (regMap == NULL) {
        HDF_LOGE("%s: regMap is NULL", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufReadBuffer(txBuf, (const void **)&data, &dataSize)) {
        HDF_LOGE("%s: Failed to read sbuf", __func__);
        return HDF_FAILURE;
    }
    regMap = (struct UartRegisterMap *)device->resource.physBase;
    for (idx = 0; idx < dataSize; idx++) {
        UartPl011Write(regMap, data[idx]);
    }
    return HDF_SUCCESS;
}

int32_t SampleDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t result = HDF_FAILURE;
    if (client == NULL || client->device == NULL) {
        HDF_LOGE("%s: client or client->device is NULL", __func__);
        return result;
    }
    struct UartHost *uartHost = (struct UartHost *)client->device->service;
    if (uartHost == NULL) {
        HDF_LOGE("%s: uartHost is NULL", __func__);
        return result;
    }
    struct UartDevice *uartDevice = (struct UartDevice *)uartHost->priv;
    if (uartDevice == NULL) {
        HDF_LOGE("%s: uartDevice is NULL", __func__);
        return result;
    }
    switch (cmdId) {
        case UART_WRITE: {
            result = SampleDispatchWrite(uartDevice, data);
            break;
        }
        default:
            break;
    }
    return result;
}
