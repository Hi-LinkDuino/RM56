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

#include "sys_event_rule.h"

namespace OHOS {
namespace HiviewDFX {
bool SysEventRule::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteString(domain)) {
        return false;
    }
    if (!parcel.WriteString(eventName)) {
        return false;
    }
    if (!parcel.WriteString(tag)) {
        return false;
    }
    if (!parcel.WriteUint32(ruleType)) {
        return false;
    }
    return true;
}

SysEventRule* SysEventRule::Unmarshalling(Parcel& parcel)
{
    SysEventRule* ret = new SysEventRule();
    if (!parcel.ReadString(ret->domain)) {
        goto error;
    }
    if (!parcel.ReadString(ret->eventName)) {
        goto error;
    }
    if (!parcel.ReadString(ret->tag)) {
        goto error;
    }
    if (!parcel.ReadUint32(ret->ruleType)) {
        goto error;
    }
    return ret;
error:
    delete ret;
    ret = nullptr;
    return nullptr;
}
} // namespace HiviewDFX
} // namespace OHOS