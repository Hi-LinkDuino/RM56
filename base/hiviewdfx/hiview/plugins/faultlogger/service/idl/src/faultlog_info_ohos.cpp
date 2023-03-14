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
#include "faultlog_info_ohos.h"

#include "parcel.h"

namespace OHOS {
namespace HiviewDFX {
bool FaultLogInfoOhos::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteInt64(time)) {
        return false;
    }

    if (!parcel.WriteInt32(uid)) {
        return false;
    }

    if (!parcel.WriteInt32(pid)) {
        return false;
    }

    if (!parcel.WriteInt32(faultLogType)) {
        return false;
    }

    if (!parcel.WriteString(module)) {
        return false;
    }

    if (!parcel.WriteString(reason)) {
        return false;
    }

    if (!parcel.WriteString(summary)) {
        return false;
    }

    if (!parcel.WriteString(logPath)) {
        return false;
    }

    uint32_t size = sectionMaps.size();
    if (!parcel.WriteUint32(size)) {
        return false;
    }

    for (const auto [key, value] : sectionMaps) {
        if (!parcel.WriteString(key)) {
            return false;
        }

        if (!parcel.WriteString(value)) {
            return false;
        }
    }
    return true;
}

FaultLogInfoOhos* FaultLogInfoOhos::Unmarshalling(Parcel& parcel)
{
    const uint32_t maxSize = 128;
    uint32_t size;
    uint32_t i;
    FaultLogInfoOhos* ret = new FaultLogInfoOhos();
    if (!parcel.ReadInt64(ret->time)) {
        goto error;
    }

    if (!parcel.ReadInt32(ret->uid)) {
        goto error;
    }

    if (!parcel.ReadInt32(ret->pid)) {
        goto error;
    }

    if (!parcel.ReadInt32(ret->faultLogType)) {
        goto error;
    }

    if (!parcel.ReadString(ret->module)) {
        goto error;
    }

    if (!parcel.ReadString(ret->reason)) {
        goto error;
    }

    if (!parcel.ReadString(ret->summary)) {
        goto error;
    }

    if (!parcel.ReadString(ret->logPath)) {
        goto error;
    }

    if (!parcel.ReadUint32(size)) {
        goto error;
    }

    if (size > maxSize) {
        goto error;
    }

    for (i = 0; i < size; i++) {
        std::string key;
        std::string value;
        if (!parcel.ReadString(key)) {
            goto error;
        }

        if (!parcel.ReadString(value)) {
            goto error;
        }

        ret->sectionMaps[key] = value;
    }
    return ret;

error:
    delete ret;
    ret = nullptr;
    return nullptr;
}
}  // namespace hiview
}  // namespace OHOS
