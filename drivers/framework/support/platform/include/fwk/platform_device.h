/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_DEVICE_H
#define PLATFORM_DEVICE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "hdf_sref.h"
#include "osal_sem.h"
#include "osal_spinlock.h"
#include "platform_event.h"
#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct PlatformManager;

struct PlatformDevice {
    struct HdfDeviceObject *hdfDev;    /* related to a hdf device object */
    struct IDeviceIoService *service;  /* related to a hdf io service object */
    struct PlatformManager *manager;   /* the platform manager it belongs to */
    int32_t number;                    /* number of the device instance */
    const char *name;                  /* name of the device instance */
    struct HdfSRef ref;                /* used for reference count */
    struct DListHead node;             /* linked to the list of a manager */
    struct PlatformEvent event;        /* platform event obj of this device */
    OsalSpinlock spin;                 /* for member protection */
    uint32_t irqSave;                  /* for spinlock irq save */
    struct OsalSem released;           /* for death notification */
    void *priv;                        /* private data of the device */
};

/**
 * @brief Initialize a platform device.
 *
 * Initialize members of a platform device.
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @since 1.0
 */
int32_t PlatformDeviceInit(struct PlatformDevice *device);

/**
 * @brief Uninitialize a platform device.
 *
 * Uninitialize members of a platform device.
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @since 1.0
 */
void PlatformDeviceUninit(struct PlatformDevice *device);

/**
 * @brief Set the name of a platform device.
 *
 * Use PlatformDeviceClearName to clean the name if no need anymore.
 *
 * @param device Indicates the pointer to the platform device.
 * @param fmt Indicates the pointer to the format string.
 *
 * @since 1.0
 */
int32_t PlatformDeviceSetName(struct PlatformDevice *device, const char *fmt, ...);

/**
 * @brief Clear the name of a platform device.
 *
 * Can only clear the name set by PlatformDeviceSetName.
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @since 1.0
 */
void PlatformDeviceClearName(struct PlatformDevice *device);

/**
 * @brief Increase reference count for a platform device.
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @return Returns 0 if get successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformDeviceGet(struct PlatformDevice *device);

/**
 * @brief Decrease reference count for a platform device.
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @since 1.0
 */
void PlatformDevicePut(struct PlatformDevice *device);

/**
 * @brief Get the reference count for a platform device.
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @return Returns the reference count on success; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformDeviceRefCount(struct PlatformDevice *device);

/**
 * @brief Wait for specified events of a platform device.
 *
 * @param device Indicates the pointer to the platform device.
 * @param mask Mask the events interested.
 * @param tms The timeout value specified.
 * @param events Pointer to receive the events, NULL if not needed.
 *
 * @return Returns 0 if wait successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformDeviceWaitEvent(struct PlatformDevice *device, uint32_t mask, uint32_t tms, uint32_t *events);

/**
 * @brief Post a platform event.
 *
 * @param device Indicates the pointer to the platform device.
 * @param events The platform event to post.
 *
 * @return Returns 0 if the event post successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformDevicePostEvent(struct PlatformDevice *device, uint32_t events);

/**
 * @brief Register a listener to a platform device.
 *
 * The events will be notified by callback function specified by the platform listener object.
 *
 * @param device Indicates the pointer to the platform device.
 * @param listener Indicates the pointer to the platform listener.
 *
 * @return Returns 0 if registered successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformDeviceListenEvent(struct PlatformDevice *device, struct PlatformEventListener *listener);

/**
 * @brief Unregister a listener to a platform device.
 *
 * @param device Indicates the pointer to the platform device.
 * @param listener Indicates the pointer to the platform listener.
 *
 * @since 1.0
 */
void PlatformDeviceUnListenEvent(struct PlatformDevice *device, struct PlatformEventListener *listener);

/**
 * @brief Add a platform device by module type.
 *
 * do not call in irq context cause can sleep
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @return Returns 0 if add successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformDeviceAdd(struct PlatformDevice *device);

/**
 * @brief Remove a platform device by module type.
 *
 * do not call in irq context cause can sleep
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @since 1.0
 */
void PlatformDeviceDel(struct PlatformDevice *device);

/**
 * @brief Get device resource node of the device.
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @return Returns 0 if add successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformDeviceGetDrs(struct PlatformDevice *device, struct DeviceResourceNode **drs);

/**
 * @brief Create a hdf device service for the platform device.
 *
 * @param device Indicates the pointer to the platform device.
 * @param dispatch Dispatch function for the service.
 *
 * @return Returns 0 if create successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformDeviceCreateService(struct PlatformDevice *device,
    int32_t (*dispatch)(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply));

/**
 * @brief Destroy the hdf device service for the platform device.
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @since 1.0
 */
void PlatformDeviceDestroyService(struct PlatformDevice *device);

/**
 * @brief Bind to a hdf device object.
 *
 * @param device Indicates the pointer to the platform device.
 * @param hdfDevice Indicates the pointer to the hdf device object.
 *
 * @return Returns 0 if bind successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformDeviceBind(struct PlatformDevice *device, struct HdfDeviceObject *hdfDevice);

/**
 * @brief Unbind from a hdf device object.
 *
 * @param device Indicates the pointer to the platform device.
 * @param hdfDevice Indicates the pointer to the hdf device object.
 *
 * @since 1.0
 */
void PlatformDeviceUnbind(struct PlatformDevice *device, struct HdfDeviceObject *hdfDevice);


/**
 * @brief Transform a hdf device object to a platform device object.
 *
 * @param device Indicates the pointer to the platform device.
 *
 * @return Returns the pointer to the platform device object on success; otherwise null.
 * @since 1.0
 */
struct PlatformDevice *PlatformDeviceFromHdfDev(struct HdfDeviceObject *hdfDev);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PLATFORM_DEVICE_H */
