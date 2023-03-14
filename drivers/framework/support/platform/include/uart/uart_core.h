/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef UART_CORE_H
#define UART_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_sbuf.h"
#include "osal_atomic.h"
#include "uart_if.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief uart device operations.
 */
struct UartHost {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    uint32_t num;
    OsalAtomic atom;
    void *priv;
    struct UartHostMethod *method;
};

struct UartHostMethod {
    int32_t (*Init)(struct UartHost *host);
    int32_t (*Deinit)(struct UartHost *host);
    int32_t (*Read)(struct UartHost *host, uint8_t *data, uint32_t size);
    int32_t (*Write)(struct UartHost *host, uint8_t *data, uint32_t size);
    int32_t (*GetBaud)(struct UartHost *host, uint32_t *baudRate);
    int32_t (*SetBaud)(struct UartHost *host, uint32_t baudRate);
    int32_t (*GetAttribute)(struct UartHost *host, struct UartAttribute *attribute);
    int32_t (*SetAttribute)(struct UartHost *host, struct UartAttribute *attribute);
    int32_t (*SetTransMode)(struct UartHost *host, enum UartTransMode mode);
    int32_t (*pollEvent)(struct UartHost *host, void *filep, void *table);
};

struct UartHost *UartHostCreate(struct HdfDeviceObject *device);
void UartHostDestroy(struct UartHost *host);

/**
 * @brief Turn UartHost to a HdfDeviceObject.
 *
 * @param host Indicates the Uart host device.
 *
 * @return Retrns the pointer of the HdfDeviceObject on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct HdfDeviceObject *UartHostToDevice(struct UartHost *host)
{
    return (host == NULL) ? NULL : host->device;
}

/**
 * @brief Turn HdfDeviceObject to an UartHost.
 *
 * @param device Indicates a HdfDeviceObject.
 *
 * @return Retrns the pointer of the UartHost on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct UartHost *UartHostFromDevice(struct HdfDeviceObject *device)
{
    return (device == NULL) ? NULL : (struct UartHost *)device->service;
}

int32_t UartHostRequest(struct UartHost *host);

int32_t UartHostRelease(struct UartHost *host);

static inline int32_t UartHostRead(struct UartHost *host, uint8_t *data, uint32_t size)
{
    if (host == NULL || host->method == NULL || host->method->Read == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return host->method->Read(host, data, size);
}

static inline int32_t UartHostWrite(struct UartHost *host, uint8_t *data, uint32_t size)
{
    if (host == NULL || host->method == NULL || host->method->Write == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return host->method->Write(host, data, size);
}

static inline int32_t UartHostGetBaud(struct UartHost *host, uint32_t *baudRate)
{
    if (host == NULL || host->method == NULL || host->method->GetBaud == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return host->method->GetBaud(host, baudRate);
}

static inline int32_t UartHostSetBaud(struct UartHost *host, uint32_t baudRate)
{
    if (host == NULL || host->method == NULL || host->method->SetBaud == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return host->method->SetBaud(host, baudRate);
}

static inline int32_t UartHostGetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    if (host == NULL || host->method == NULL || host->method->GetAttribute == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return host->method->GetAttribute(host, attribute);
}

static inline int32_t UartHostSetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    if (host == NULL || host->method == NULL || host->method->SetAttribute == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return host->method->SetAttribute(host, attribute);
}

static inline int32_t UartHostSetTransMode(struct UartHost *host, enum UartTransMode mode)
{
    if (host == NULL || host->method == NULL || host->method->SetTransMode == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return host->method->SetTransMode(host, mode);
}

static inline int32_t UartHostPollEvent(struct UartHost *host, void *filep, void *table)
{
    if (host == NULL || host->method == NULL || host->method->pollEvent == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return host->method->pollEvent(host, filep, table);
}

int32_t UartIoDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UART_CORE_H */
