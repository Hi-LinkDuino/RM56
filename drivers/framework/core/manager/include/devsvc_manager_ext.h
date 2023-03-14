/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVICE_SERVICE_MANAGER_EXT_H
#define DEVICE_SERVICE_MANAGER_EXT_H

struct HdfObject *DevSvcManagerExtCreate(void);
void DevSvcManagerExtRelease(struct HdfObject *object);

#endif // DEVICE_SERVICE_MANAGER_EXT_H