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


#ifndef _HDI_DISPLAY_REGISTER_CALLBACK_PROXY_H_
#define _HDI_DISPLAY_REGISTER_CALLBACK_PROXY_H_

#include <iremote_proxy.h>
#include "idisplay_device_callback.h"

class DisplayDeviceCallbackProxy : public OHOS::IRemoteProxy<DisplayRegisterCallbackBase> {
public:
    explicit DisplayDeviceCallbackProxy(const OHOS::sptr<OHOS::IRemoteObject> &impl)
        : OHOS::IRemoteProxy<DisplayRegisterCallbackBase>(impl) {}
    virtual ~DisplayDeviceCallbackProxy() = default;
    int32_t OnHotplugIn(uint32_t outputId, bool connected) override;
    int32_t OnVBlankCallback(unsigned int sequence, uint64_t ns) override;
    int32_t OnRefreshCallback(uint32_t devId) override;
private:
    static inline OHOS::BrokerDelegator<DisplayDeviceCallbackProxy> delegator_;
};
#endif // _HDI_DISPLAY_REGISTER_CALLBACK_PROXY_H_
