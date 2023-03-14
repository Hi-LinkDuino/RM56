/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_MANAGER_H
#define PLATFORM_MANAGER_H

#include "hdf_base.h"
#include "hdf_dlist.h"
#include "osal_spinlock.h"
#include "platform_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum PlatformModuleType;
struct PlatformManager {
    struct PlatformDevice device;
    struct DListHead devices;  /* list to keep all it's device instances */
    int32_t (*add)(struct PlatformManager *manager, struct PlatformDevice *device);
    int32_t (*del)(struct PlatformManager *manager, struct PlatformDevice *device);
};

/**
 * @brief Create a platform manager.
 *
 * Create a platform manager with member initialized.
 *
 * @param name Indicates the name of the manager.
 * @param manager Indicates the double pointer to receive the created manager instance.
 *
 * @return Returns 0 if create successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformManagerCreate(const char *name, struct PlatformManager **manager);

/**
 * @brief Destroy a platform manager.
 *
 * @param name Indicates the name of the manager.
 *
 * @since 1.0
 */
void PlatformManagerDestroy(struct PlatformManager *manager);

/**
 * @brief Get a platform manager by module type.
 *
 * @param module Indicates the module type of the manager.
 *
 * @return Returns the pointer to the paltform manager on success; returns NULL otherwise.
 * @since 1.0
 */
struct PlatformManager *PlatformManagerGet(enum PlatformModuleType module);

/**
 * @brief Add a platform device to a platform manager.
 *
 * Add a platform device to make it managed by platform core.
 *
 * @param manager Indicates the pointer to the platform manager.
 * @param device Indicates the pointer to the platform device.
 *
 * @return Returns 0 if the devcie added successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformManagerAddDevice(struct PlatformManager *manager, struct PlatformDevice *device);

/**
 * @brief Remove a platform device from a platform manager.
 *
 * Remove a platform device to make it away from management of platform core.
 *
 * @param manager Indicates the pointer to the platform manager.
 * @param device Indicates the pointer to the platform device.
 *
 * @return Returns 0 if the remove successfully; returns a negative value otherwise.
 * @since 1.0
 */
int32_t PlatformManagerDelDevice(struct PlatformManager *manager, struct PlatformDevice *device);

/**
 * @brief Find a particular device from the manager.
 *
 * Locate a particular device from the manager by a matching function, witch will be called for
 * each device, until it returns true indicatting a device is "found".
 * The device found will be returned with reference count increased.
 *
 * @param manager Indicates the pointer to the platform manager.
 * @param data Indicates the pointer to the data passed to match function.
 * @param match Indicates the pointer to the match function.
 *
 * @return Returns the pointer to the paltform device on success; returns NULL otherwise.
 * @since 1.0
 */
struct PlatformDevice *PlatformManagerFindDevice(struct PlatformManager *manager, void *data,
    bool (*match)(struct PlatformDevice *pdevice, void *data));

/**
 * @brief Get a platform device from the manager by number.
 *
 * The device got will be returned with reference count increased.
 *
 * @param manager Indicates the pointer to the platform manager.
 * @param number Indicates the number of the target platform device.
 *
 * @return Returns the pointer to the paltform device on success; returns NULL otherwise.
 * @since 1.0
 */
struct PlatformDevice *PlatformManagerGetDeviceByNumber(struct PlatformManager *manager, uint32_t number);

/**
 * @brief Get a platform device from the manager by device name.
 *
 * The device got will be returned with reference count increased.
 *
 * @param manager Indicates the pointer to the platform manager.
 * @param number Indicates the name of the target platform device.
 *
 * @return Returns the pointer to the paltform device on success; returns NULL otherwise.
 * @since 1.0
 */
struct PlatformDevice *PlatformManagerGetDeviceByName(struct PlatformManager *manager, const char *name);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PLATFORM_MANAGER_H */
