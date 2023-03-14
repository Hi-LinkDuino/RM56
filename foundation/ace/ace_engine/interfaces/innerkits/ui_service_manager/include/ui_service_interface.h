/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_AAFWK_UI_SERVICE_INTERFACE_H
#define OHOS_AAFWK_UI_SERVICE_INTERFACE_H

#include <iremote_broker.h>
#include "want.h"

namespace OHOS {
namespace Ace {
class IUIService : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ace.UIService");

    enum {
        UI_SERVICE_PUSH_CALL_BACK,
        UI_SERVICE_REQUEST_CALL_BACK,
        UI_SERVICE_RETURN_REQUEST,
    };

    IUIService() = default;
    virtual ~IUIService() = default;
    
    /**
     * @brief Called back to OnPush
     *
     */
    virtual void OnPushCallBack(const AAFwk::Want& want, const std::string& name,
        const std::string& jsonPath, const std::string& data, const std::string& extraData) = 0;

    /**
     * @brief Called back to OnRequest
     *
     */
    virtual void OnRequestCallBack(const AAFwk::Want& want, const std::string& name,  const std::string& data) = 0;

    virtual void OnReturnRequest(
        const AAFwk::Want& want, const std::string& source,  const std::string& data, const std::string& extraData) = 0;
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_AAFWK_UI_SERVICE_INTERFACE_H
