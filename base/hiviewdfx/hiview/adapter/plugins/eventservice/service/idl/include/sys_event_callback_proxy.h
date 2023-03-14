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

#ifndef OHOS_HIVIEWDFX_SYS_EVENT_CALLBACK_PROXY_H
#define OHOS_HIVIEWDFX_SYS_EVENT_CALLBACK_PROXY_H

#include "iremote_proxy.h"
#include "isys_event_callback.h"

namespace OHOS {
namespace HiviewDFX {
class SysEventCallbackProxy : public IRemoteProxy<ISysEventCallback> {
public:
    explicit SysEventCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<ISysEventCallback>(impl) {};
    virtual ~SysEventCallbackProxy() = default;
    DISALLOW_COPY_AND_MOVE(SysEventCallbackProxy);

    void Handle(const std::u16string& domain, const std::u16string& eventName, uint32_t eventType,
        const std::u16string& eventDetail);

private:
    static inline BrokerDelegator<SysEventCallbackProxy> delegator_;
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // OHOS_HIVIEWDFX_SYS_EVENT_CALLBACK_PROXY_H