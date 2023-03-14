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

#include "sys_event_query_rule.h"

namespace OHOS {
namespace HiviewDFX {
bool SysEventQueryRule::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint32(ruleType)) {
        return false;
    }
    if (!parcel.WriteString(domain)) {
        return false;
    }
    if (!parcel.WriteStringVector(eventList)) {
        return false;
    }
    return true;
}

SysEventQueryRule* SysEventQueryRule::Unmarshalling(Parcel& parcel)
{
    SysEventQueryRule* ret = new SysEventQueryRule();
    if (!parcel.ReadUint32(ret->ruleType)) {
        goto error;
    }
    if (!parcel.ReadString(ret->domain)) {
        goto error;
    }
    if (!parcel.ReadStringVector(&(ret->eventList))) {
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