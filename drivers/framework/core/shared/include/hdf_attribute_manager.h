/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_ATTRIBUTE_MANAGER_H
#define HDF_ATTRIBUTE_MANAGER_H

#include "hdf_slist.h"
#include "devhost_service_clnt.h"

const struct DeviceResourceNode *HdfGetHcsRootNode(void);
bool HdfAttributeManagerGetHostList(struct HdfSList *hostList);
int HdfAttributeManagerGetDeviceList(struct DevHostServiceClnt *hostClnt);

#endif /* HDF_ATTRIBUTE_MANAGER_H */
