/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DEVICE_OBJECT_H
#define HDF_DEVICE_OBJECT_H

#include "hdf_device_desc.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
void HdfDeviceObjectConstruct(struct HdfDeviceObject *deviceObject);
struct HdfDeviceObject *HdfDeviceObjectAlloc(struct HdfDeviceObject *parent, const char *driverName);
void HdfDeviceObjectRelease(struct HdfDeviceObject *deviceObject);
int HdfDeviceObjectRegister(struct HdfDeviceObject *deviceObject);
int HdfDeviceObjectUnRegister(struct HdfDeviceObject *deviceObject);
int HdfDeviceObjectPublishService(struct HdfDeviceObject *deviceObject,
    const char *servName, uint8_t policy, uint32_t perm);
int HdfRemoveService(struct HdfDeviceObject *deviceObject);
int HdfDeviceObjectSetServInfo(struct HdfDeviceObject *dev, const char *info);
int HdfDeviceObjectUpdate(struct HdfDeviceObject *dev);
int HdfDeviceObjectSetInterfaceDesc(struct HdfDeviceObject *dev, const char *interfaceDesc);
bool HdfDeviceObjectCheckInterfaceDesc(struct HdfDeviceObject *dev, struct HdfSBuf *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* HDF_DEVICE_OBJECT_H */
