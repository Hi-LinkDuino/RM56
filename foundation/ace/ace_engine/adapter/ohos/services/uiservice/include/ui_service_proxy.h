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
#ifndef OHOS_AAFWK_UI_SERVICE_PROXY_H
#define OHOS_AAFWK_UI_SERVICE_PROXY_H

#include <iremote_proxy.h>
#include "ui_service_interface.h"

namespace OHOS {
namespace Ace {
class UIServiceProxy : public IRemoteProxy<IUIService> {
public:
    explicit UIServiceProxy(const sptr<IRemoteObject>& remote);
    virtual ~UIServiceProxy();
    void OnPushCallBack(const AAFwk::Want& want, const std::string& name, const std::string& jsonPath,
        const std::string& data, const std::string& extraData) override;
    void OnRequestCallBack(const AAFwk::Want& want, const std::string& name, const std::string& data) override;
    void OnReturnRequest(const AAFwk::Want& want, const std::string& source, const std::string& data,
        const std::string& extraData) override;
private:
    static inline BrokerDelegator<UIServiceProxy> delegator_;
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_AAFWK_UI_SERVICE_PROXY_H
