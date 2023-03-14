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

#ifndef OHOS_AAFWK_UI_SERVICE_MANAGER_PROXY_H
#define OHOS_AAFWK_UI_SERVICE_MANAGER_PROXY_H

#include "hilog_wrapper.h"
#include "iremote_proxy.h"
#include "ui_service_mgr_interface.h"

namespace OHOS {
namespace Ace {
/**
 * @class UIServiceMgrProxy
 * UIServiceMgrProxy.
 */
class UIServiceMgrProxy : public IRemoteProxy<IUIServiceMgr> {
public:
    explicit UIServiceMgrProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IUIServiceMgr>(impl)
    {}
    virtual ~UIServiceMgrProxy()
    {}
    virtual int RegisterCallBack(const AAFwk::Want& want, const sptr<IUIService>& uiService);

    virtual int UnregisterCallBack(const AAFwk::Want& want);

    virtual int Push(const AAFwk::Want& want, const std::string& name, const std::string& jsonPath,
        const std::string& data, const std::string& extraData);

    virtual int Request(const AAFwk::Want& want, const std::string& name, const std::string& data);

    virtual int ReturnRequest(const AAFwk::Want& want, const std::string& source, const std::string& data,
        const std::string& extraData);

    virtual int ShowDialog(const std::string& name,
                           const std::string& params,
                           OHOS::Rosen::WindowType windowType,
                           int x,
                           int y,
                           int width,
                           int height,
                           const sptr<OHOS::Ace::IDialogCallback>& dialogCallback,
                           int* id = nullptr);

    virtual int CancelDialog(int id);

private:
    bool WriteInterfaceToken(MessageParcel& data);

private:
    static inline BrokerDelegator<UIServiceMgrProxy> delegator_;
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_AAFWK_UI_SERVICE_MANAGER_PROXY_H
