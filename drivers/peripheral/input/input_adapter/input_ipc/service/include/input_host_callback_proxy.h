/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef INPUT_HOST_CALLBACK_PROXY_H
#define INPUT_HOST_CALLBACK_PROXY_H

#include <hdf_log.h>
#include <iremote_proxy.h>
#include "iinput_host_callbck.h"

namespace OHOS {
namespace Input {
class InputCallbackProxy : public IRemoteProxy<InputReportEventCb> {
public:
    explicit InputCallbackProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<InputReportEventCb>(impl) {}
    virtual ~InputCallbackProxy() = default;
    virtual void ReportEventPkgCallback(const EventPackage *pkgs, uint32_t count, uint32_t devIndex);

private:
    static constexpr int32_t CMD_INPUT_CALLBACK_REMOTE_REPROT_EVENT_PROXY = 100;
    static inline BrokerDelegator<InputCallbackProxy> delegator_;
};

class InputHotCallbackProxy : public IRemoteProxy<InputReportHostCb> {
public:
    explicit InputHotCallbackProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<InputReportHostCb>(impl) {}
    virtual ~InputHotCallbackProxy() = default;
    virtual void ReportHotPlugEventCallback(const HotPlugEvent *event);

private:
    static constexpr int32_t CMD_INPUT_CALLBACK_REMOTE_REPROT_HOT_PLUG_EVENT_PROXY = 101;
    static inline BrokerDelegator<InputHotCallbackProxy> delegator_;
};
}  // namespace Input
}  // namespace OHOS

#endif // INPUT_HOST_CALLBACK_PROXY_H
