/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_service_record.h"
#include "osal_mem.h"

struct DevSvcRecord *DevSvcRecordNewInstance(void)
{
    return (struct DevSvcRecord *)OsalMemCalloc(sizeof(struct DevSvcRecord));
}

void DevSvcRecordFreeInstance(struct DevSvcRecord *inst)
{
    if (inst != NULL) {
        OsalMemFree((char *)inst->servName);
        OsalMemFree((char *)inst->servInfo);
        OsalMemFree(inst);
    }
}

