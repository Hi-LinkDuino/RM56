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

#ifndef SAMPLE_SERVICE_HDF_H
#define SAMPLE_SERVICE_HDF_H

#include <hdf_sbuf.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include <securec.h>
#include <hdi_smq_meta.h>

struct HdfDeviceObject;
struct HdfDeviceIoClient;

struct DataBlock {
    int a;
    int b;
    const char *str;
    int c;
};

struct SampleSmqElement {
    int32_t data32;
    uint64_t data64;
};

enum {
    SAMPLE_SERVICE_PING = 0,
    SAMPLE_SERVICE_SUM,
    SAMPLE_SERVICE_CALLBACK,
    SAMPLE_STRUCT_TRANS,
    SAMPLE_BUFFER_TRANS,
    SAMPLE_REGISTER_DEVICE,
    SAMPLE_UNREGISTER_DEVICE,
    SAMPLE_UPDATE_SERVIE,
    SAMPLE_TRANS_SMQ,
};

struct SampleHdi {
    int32_t (*ping)(struct HdfDeviceObject *device, const char* info, char** infoOut);
    int32_t (*sum)(struct HdfDeviceObject *device, int32_t x0, int32_t x1, int32_t *result);
    int32_t (*callback)(struct HdfDeviceObject *device, struct HdfRemoteService *callback, int32_t code);
    int32_t (*registerDevice)(struct HdfDeviceObject *device, const char *servName);
    int32_t (*unregisterDevice)(struct HdfDeviceObject *device, const char *servName);
    int32_t (*updateService)(struct HdfDeviceObject *device, const char *info);
    int32_t (*tansSmq)(struct HdfDeviceObject *device, OHOS::HDI::Base::SharedMemQueueMeta<SampleSmqElement> *smqMeta,
        uint32_t element);
};

const struct SampleHdi *SampleHdiImplInstance(void);

int32_t SampleServiceOnRemoteRequest(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply);


static inline void DataBlockFree(struct DataBlock *dataBlock)
{
    if (dataBlock != nullptr) {
        OsalMemFree((void *)dataBlock->str);
        OsalMemFree(dataBlock);
    }
}

static inline struct DataBlock *DataBlockBlockUnmarshalling(struct HdfSBuf *data)
{
    const struct DataBlock *dataBlock_ = nullptr;
    uint32_t readSize = 0;

    if (!HdfSbufReadBuffer(data, (const void **)&dataBlock_, &readSize)) {
        HDF_LOGE("%{public}s: failed to read dataBlock", __func__);
        return nullptr;
    }

    if (readSize != sizeof(struct DataBlock)) {
        HDF_LOGE("%{public}s: dataBlock size mismatch %{public}d", __func__, readSize);
        return nullptr;
    }

    struct DataBlock *dataBlock = (struct DataBlock *)OsalMemAlloc(sizeof(struct DataBlock));
    if (dataBlock == nullptr) {
        return nullptr;
    }
    HDF_LOGD("%{public}s: DataBlock mem: %{public}d %{public}d %{public}d", __func__,
        dataBlock_->a, dataBlock_->b, dataBlock_->c);
    if (memcpy_s(dataBlock, sizeof(*dataBlock), dataBlock_, sizeof(*dataBlock)) != EOK) {
        return nullptr;
    }

    const char *str = nullptr;
    if (!HdfSbufReadBuffer(data, (const void **)&str, &readSize)) {
        HDF_LOGE("%{public}s: failed to read dataBlock.str", __func__);
        return nullptr;
    }

    dataBlock->str = strdup(str);
    if (dataBlock->str == nullptr) {
        OsalMemFree(dataBlock);
        return nullptr;
    }

    return dataBlock;
}

static inline bool DataBlockBlockMarshalling(struct DataBlock *dataBlock, struct HdfSBuf *data)
{
    if (!HdfSbufWriteBuffer(data, dataBlock, sizeof(struct DataBlock))) {
        return false;
    }

    if (!HdfSbufWriteBuffer(data, dataBlock->str, strlen(dataBlock->str) + 1)) {
        return false;
    }

    return true;
}

#endif // SAMPLE_SERVICE_HDF_H