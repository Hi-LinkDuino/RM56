/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef USB_PNP_MANAGE_H
#define USB_PNP_MANAGE_H

#include "hdf_base.h"
#include "hdf_sbuf.h"

struct UsbPnpManagerDeviceInfo {
    struct HdfDeviceObject *usbPnpManager;
    const char *moduleName;
    const char *serviceName;
    const char *deviceMatchAttr;
    const void *privateData;
    bool isReg;
};

bool UsbPnpManagerWriteModuleName(struct HdfSBuf *sbuf, const char *moduleName);
int32_t UsbPnpManagerRegisterOrUnregisterDevice(struct UsbPnpManagerDeviceInfo *managerInfo);

#endif /* USB_PNP_MANAGE_H */
