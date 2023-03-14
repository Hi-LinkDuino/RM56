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

#include "dev_attribute_serialize.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG dev_attr_serialze

#define ATTRIBUTE_PRIVATE_DATA_LENGTH_NULL 0
#define ATTRIBUTE_PRIVATE_DATA_LENGTH_NORMAL 1

bool DeviceAttributeSerialize(const struct HdfDeviceInfo *attribute, struct HdfSBuf *sbuf)
{
    if (attribute == NULL || sbuf == NULL) {
        return false;
    }

    uint8_t ret = 1;
    if (!HdfSbufWriteUint32(sbuf, attribute->deviceId) ||
        !HdfSbufWriteUint16(sbuf, attribute->policy) ||
        !HdfSbufWriteString(sbuf, attribute->svcName) ||
        !HdfSbufWriteString(sbuf, attribute->moduleName)) {
        return false;
    }

    if (attribute->deviceMatchAttr != NULL) {
        ret &= HdfSbufWriteUint32(sbuf, ATTRIBUTE_PRIVATE_DATA_LENGTH_NORMAL);
        ret &= HdfSbufWriteString(sbuf, attribute->deviceMatchAttr);
    } else {
        ret &= HdfSbufWriteUint32(sbuf, ATTRIBUTE_PRIVATE_DATA_LENGTH_NULL);
    }

    if (ret == 0) {
        HDF_LOGE("failed to serialize device attribute");
        return false;
    }
    return true;
}

struct HdfDeviceInfo *DeviceAttributeDeserialize(struct HdfSBuf *sbuf)
{
    if (sbuf == NULL) {
        return NULL;
    }

    struct HdfDeviceInfo *attribute = HdfDeviceInfoNewInstance();
    if (attribute == NULL) {
        HDF_LOGE("OsalMemCalloc failed, attribute is null");
        return NULL;
    }
    if (!HdfSbufReadUint32(sbuf, &attribute->deviceId) ||
        !HdfSbufReadUint16(sbuf, &attribute->policy)) {
        HDF_LOGE("invalid deviceId or policy");
        DeviceSerializedAttributeRelease(attribute);
        return NULL;
    }

    do {
        const char *svcName = HdfSbufReadString(sbuf);
        if (svcName == NULL) {
            HDF_LOGE("Read from parcel failed, svcName is null");
            break;
        }
        attribute->svcName = strdup(svcName);
        if (attribute->svcName == NULL) {
            HDF_LOGE("Read from parcel failed, strdup svcName fail");
            break;
        }
        const char *moduleName = HdfSbufReadString(sbuf);
        if (moduleName == NULL) {
            HDF_LOGE("Read from parcel failed, driverPath is null");
            break;
        }
        attribute->moduleName = strdup(moduleName);
        if (attribute->moduleName == NULL) {
            HDF_LOGE("Read from parcel failed, strdup moduleName fail");
            break;
        }

        uint32_t length;
        if (!HdfSbufReadUint32(sbuf, &length)) {
            HDF_LOGE("Device attribute readDeviceMatchAttr length failed");
            break;
        }
        if (length == ATTRIBUTE_PRIVATE_DATA_LENGTH_NORMAL) {
            const char *deviceMatchAttr = HdfSbufReadString(sbuf);
            if (deviceMatchAttr == NULL) {
                HDF_LOGE("%s: Read from parcel failed, deviceMatchAttr is null", __func__);
                break;
            }
            attribute->deviceMatchAttr = strdup(deviceMatchAttr);
        }
        return attribute;
    } while (0);

    DeviceSerializedAttributeRelease(attribute);
    return NULL;
}

void DeviceSerializedAttributeRelease(struct HdfDeviceInfo *attribute)
{
    if (attribute == NULL) {
        return;
    }

    if (attribute->moduleName != NULL) {
        OsalMemFree((void *)attribute->moduleName);
    }
    if (attribute->svcName != NULL) {
        OsalMemFree((void *)attribute->svcName);
    }
    if (attribute->deviceMatchAttr != NULL) {
        OsalMemFree((void *)attribute->deviceMatchAttr);
    }
    OsalMemFree(attribute);
}
