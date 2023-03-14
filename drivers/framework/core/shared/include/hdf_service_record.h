/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVSVC_RECORD_H
#define DEVSVC_RECORD_H

#include "hdf_object.h"
#include "hdf_dlist.h"

struct DevSvcRecord {
    struct DListHead entry;
    uint32_t key;
    struct HdfDeviceObject *value;
    const char *servName;
    const char *servInfo;
    uint16_t devClass;
};

struct DevSvcRecord *DevSvcRecordNewInstance(void);
void DevSvcRecordFreeInstance(struct DevSvcRecord *inst);

#endif /* DEVSVC_RECORD_H */

