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

#ifndef OHOS_HIVIEWDFX_IQUERY_SYS_EVENT_CALLBACK_H
#define OHOS_HIVIEWDFX_IQUERY_SYS_EVENT_CALLBACK_H

#include <vector>

#include "iremote_broker.h"

namespace OHOS {
namespace HiviewDFX {
class IQuerySysEventCallback : public IRemoteBroker {
public:
    virtual void OnQuery(const std::vector<std::u16string>& sysEvent, const std::vector<int64_t>& seq) = 0;
    virtual void OnComplete(int32_t reason, int32_t total) = 0;

    enum {
        ON_QUERY = 0,
        ON_COMPLETE,
    };

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.hiviewdfx.IQuerySysEventCallback");
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // OHOS_HIVIEWDFX_IQUERY_SYS_EVENT_CALLBACK_H