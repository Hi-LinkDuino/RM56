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

#ifndef OHOS_HIVIEWDFX_QUERY_SYS_EVENT_CALLBACK_PROXY_H
#define OHOS_HIVIEWDFX_QUERY_SYS_EVENT_CALLBACK_PROXY_H

#include "iquery_sys_event_callback.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace HiviewDFX {
class QuerySysEventCallbackProxy : public IRemoteProxy<IQuerySysEventCallback> {
public:
    explicit QuerySysEventCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IQuerySysEventCallback>(impl) {};
    virtual ~QuerySysEventCallbackProxy() = default;
    DISALLOW_COPY_AND_MOVE(QuerySysEventCallbackProxy);

    void OnQuery(const std::vector<std::u16string>& sysEvent, const std::vector<int64_t>& seq);
    void OnComplete(int32_t reason, int32_t total);

private:
    static inline BrokerDelegator<QuerySysEventCallbackProxy> delegator_;
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // OHOS_HIVIEWDFX_QUERY_SYS_EVENT_CALLBACK_PROXY_H