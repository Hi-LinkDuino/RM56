/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "devhost_service_clnt.h"
#include "devmgr_service.h"
#include "hdf_base.h"
#include "hdf_cstring.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"

#define HDF_LOG_TAG devmgr_query_device

int DevFillQueryDeviceInfo(struct IDevmgrService *inst, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct DevmgrService *devMgrSvc = (struct DevmgrService *)inst;
    struct HdfSListIterator itDeviceInfo;
    struct HdfDeviceInfo *deviceInfo = NULL;
    struct DevHostServiceClnt *hostClnt = NULL;
    bool flag = false;
    uint16_t status = 0;

    if (devMgrSvc == NULL || data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s failed, parameter is null", __func__);
        return HDF_FAILURE;
    }

    HdfSbufReadUint16(data, &status);
    if (status != HDF_SERVICE_UNUSABLE && status != HDF_SERVICE_USABLE) {
        HDF_LOGE("%{public}s failed, status is invalid %{public}d", __func__, status);
        return HDF_FAILURE;
    }

    DLIST_FOR_EACH_ENTRY(hostClnt, &devMgrSvc->hosts, struct DevHostServiceClnt, node) {
    HdfSListIteratorInit(&itDeviceInfo, &hostClnt->unloadDevInfos);
        HDF_LOGD("%{public}s, host:%{public}s %{public}d", __func__, hostClnt->hostName, hostClnt->hostId);
        while (HdfSListIteratorHasNext(&itDeviceInfo)) {
            deviceInfo = (struct HdfDeviceInfo *)HdfSListIteratorNext(&itDeviceInfo);
            HDF_LOGD("%{public}s %{public}s policy:%{public}d status:%{public}d type:%{public}d",
                __func__, deviceInfo->svcName, deviceInfo->policy, deviceInfo->status, deviceInfo->deviceType);
            if ((deviceInfo->policy == SERVICE_POLICY_CAPACITY || deviceInfo->policy == SERVICE_POLICY_PUBLIC) &&
                deviceInfo->status == status) {
                flag = HdfSbufWriteString(reply, deviceInfo->svcName);
                if (!flag) {
                    HDF_LOGE("%{public}s writing string failed %{public}s", __func__, deviceInfo->svcName);
                    return HDF_FAILURE;
                }
                flag = HdfSbufWriteInt32(reply, deviceInfo->deviceType);
                if (!flag) {
                    HDF_LOGE("%{public}s writing int32 failed %{public}d", __func__, deviceInfo->deviceType);
                    return HDF_FAILURE;
                }
            }
        }
    }
    return HDF_SUCCESS;
}

