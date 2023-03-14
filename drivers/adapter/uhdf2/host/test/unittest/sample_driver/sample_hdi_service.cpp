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

#include <hdf_device_object.h>
#include <hdf_dlist.h>
#include <hdf_log.h>
#include <hdf_remote_service.h>
#include <hdi_smq.h>
#include <thread>
#include "sample_hdi.h"

#define HDF_LOG_TAG sample_driver
using OHOS::HDI::Base::SharedMemQueue;
using OHOS::HDI::Base::SharedMemQueueMeta;
using OHOS::HDI::Base::SmqType;

constexpr int SMQ_TRANS_ELEMENT_MAX = 5;
constexpr int SMQ_TRANS_WAIT_TIME = 100;

struct SampleDevice {
    struct DListHead listNode;
    struct HdfDeviceObject *devobj;
};

static struct DListHead g_sampleDeviceList = { nullptr };

static int32_t SampleServicePing(struct HdfDeviceObject *device, const char *info, char **infoOut)
{
    (void)device;
    HDF_LOGI("Sample:info is %{public}s", info);
    *infoOut = strdup(info);
    return 0;
}

static int32_t SampleServiceSum(struct HdfDeviceObject *device, int32_t x0, int32_t x1, int32_t *result)
{
    (void)device;
    *result = x0 + x1;
    return 0;
}

static int32_t SampleServiceCallback(struct HdfDeviceObject *device, struct HdfRemoteService *callback, int32_t code)
{
    (void)device;
    struct HdfSBuf *dataSbuf = HdfSbufTypedObtain(SBUF_IPC);
    HdfSbufWriteInt32(dataSbuf, code);
    int ret = callback->dispatcher->Dispatch(callback, 0, dataSbuf, nullptr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to do callback, ret = %{public}d", ret);
    }
    HdfSbufRecycle(dataSbuf);
    return ret;
}

static int32_t SampleServiceRegisterDevice(struct HdfDeviceObject *device, const char *servName)
{
    struct HdfDeviceObject *dev = HdfDeviceObjectAlloc(device, "libsample_driver.z.so");
    if (dev == nullptr) {
        HDF_LOGE("failed to alloc device object");
        return HDF_DEV_ERR_NO_DEVICE;
    }

    if (HdfDeviceObjectRegister(dev) != HDF_SUCCESS) {
        HDF_LOGE("failed to register device");
        HdfDeviceObjectRelease(dev);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    if (HdfDeviceObjectPublishService(dev, servName, SERVICE_POLICY_CAPACITY, 0) != HDF_SUCCESS) {
        HDF_LOGE("failed to publish device service %{public}s", servName);
        HdfDeviceObjectRelease(dev);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    HDF_LOGE("publish device service %{public}s success", servName);
    struct SampleDevice *sampleDev = (struct SampleDevice *)OsalMemAlloc(sizeof(struct SampleDevice));
    if (sampleDev == nullptr) {
        HdfDeviceObjectRelease(dev);
        return HDF_DEV_ERR_NO_MEMORY;
    }

    sampleDev->devobj = dev;
    if (g_sampleDeviceList.next == nullptr) {
        DListHeadInit(&g_sampleDeviceList);
    }
    DListInsertTail(&sampleDev->listNode, &g_sampleDeviceList);

    HDF_LOGI("register device %{public}s success", servName);
    return HDF_SUCCESS;
}

static int32_t SampleServiceUnregisterDevice(struct HdfDeviceObject *device, const char *servName)
{
    struct SampleDevice *sampleDev = nullptr;
    struct SampleDevice *sampleDevTmp = nullptr;
    HDF_LOGI("remove device %{public}s in", servName);
    DLIST_FOR_EACH_ENTRY_SAFE(sampleDev, sampleDevTmp, &g_sampleDeviceList, struct SampleDevice, listNode)
    {
        if (sampleDev->devobj == nullptr || HdfDeviceGetServiceName(sampleDev->devobj) == nullptr) {
            DListRemove(&sampleDev->listNode);
            OsalMemFree(sampleDev);
            continue;
        }

        if (strcmp(HdfDeviceGetServiceName(sampleDev->devobj), servName) == 0) {
            HdfDeviceObjectRelease(sampleDev->devobj);
            DListRemove(&sampleDev->listNode);
            OsalMemFree(sampleDev);
            HDF_LOGI("remove device %{public}s success", servName);
        }
    }

    return HDF_SUCCESS;
}

static int32_t SampleServiceUpdateDevice(struct HdfDeviceObject *device, const char *servInfo)
{
    if (HdfDeviceObjectSetServInfo(device, servInfo) != HDF_SUCCESS) {
        HDF_LOGE("failed to set service info");
        return HDF_FAILURE;
    }
    return HdfDeviceObjectUpdate(device);
}

static int32_t SampleServiceTansSmq(
    struct HdfDeviceObject *device, SharedMemQueueMeta<SampleSmqElement> *smqMeta, uint32_t element)
{
    HDF_LOGI("SampleServiceTansSmq in, element=%{public}u", element);
    if (element > SMQ_TRANS_ELEMENT_MAX) {
        return HDF_ERR_INVALID_PARAM;
    }
    static std::shared_ptr<SharedMemQueue<SampleSmqElement>> smq = nullptr;
    smq = std::make_shared<SharedMemQueue<SampleSmqElement>>(*smqMeta);
    if (!smq->IsGood()) {
        HDF_LOGE("failed to create fmq from meta");
        return HDF_FAILURE;
    }
    static size_t elem = static_cast<size_t>(element);
    std::thread t([&]() {
        HDF_LOGI("SampleServiceTansSmq:smq read thread start, elem=%{public}zu", elem);
        std::shared_ptr<SharedMemQueue<SampleSmqElement>> smqLocal = smq;
        size_t elemLocal = elem;
        smq = nullptr;

        while (true) {
            SampleSmqElement t[SMQ_TRANS_ELEMENT_MAX] = {};
            int ret;
            if (smqLocal->GetMeta()->GetType() == SmqType::SYNCED_SMQ) {
                ret = smqLocal->Read(&t[0], elemLocal, OHOS::MillisecToNanosec(SMQ_TRANS_WAIT_TIME));
            } else {
                ret = smqLocal->ReadNonBlocking(&t[0], elemLocal);
            }
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("failed to read message from smq, %{public}d", ret);
                break;
            }
            for (size_t i = 0; i < elemLocal; i++) {
                HDF_LOGI(
                    "read message from smq, info[%{public}zu]:%{public}d, %{public}llu",
                    i, t[i].data32, (unsigned long long)t[i].data64);
            }
        }
    });
    t.detach();

    return HDF_SUCCESS;
}

static const struct SampleHdi g_sampleHdiImpl = {
    .ping = SampleServicePing,
    .sum = SampleServiceSum,
    .callback = SampleServiceCallback,
    .registerDevice = SampleServiceRegisterDevice,
    .unregisterDevice = SampleServiceUnregisterDevice,
    .updateService = SampleServiceUpdateDevice,
    .tansSmq = SampleServiceTansSmq,
};

const struct SampleHdi *SampleHdiImplInstance(void)
{
    return &g_sampleHdiImpl;
}