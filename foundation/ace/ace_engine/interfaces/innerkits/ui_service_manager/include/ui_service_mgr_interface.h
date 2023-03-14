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

#ifndef OHOS_AAFWK_UI_SERVICE_MANAGER_INTERFACE_H
#define OHOS_AAFWK_UI_SERVICE_MANAGER_INTERFACE_H

#include <vector>

#include <ipc_types.h>
#include <iremote_broker.h>

#include "dialog_callback_interface.h"
#include "ui_service_interface.h"
#include "uri.h"
#include "wm/window.h"

namespace OHOS {
namespace Ace {
using Uri = OHOS::Uri;
const std::string UI_MANAGER_SERVICE_NAME = "UIMgrService";
/**
 * @class IUIServiceMgr
 * IUIServiceMgr interface is used to access UIService manager services.
 */
class IUIServiceMgr : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ace.UIServiceMgr")

    IUIServiceMgr() = default;
    virtual ~IUIServiceMgr() = default;

    virtual int RegisterCallBack(const AAFwk::Want& want, const sptr<IUIService>& uiService) = 0;

    virtual int UnregisterCallBack(const AAFwk::Want& want) = 0;

    virtual int Push(const AAFwk::Want& want, const std::string& name, const std::string& jsonPath,
        const std::string& data, const std::string& extraData) = 0;

    virtual int Request(const AAFwk::Want& want, const std::string& name, const std::string& data) = 0;

    virtual int ReturnRequest(const AAFwk::Want& want, const std::string& source,  const std::string& data,
        const std::string& extraData) = 0;

    virtual int ShowDialog(const std::string& name,
                           const std::string& params,
                           OHOS::Rosen::WindowType windowType,
                           int x,
                           int y,
                           int width,
                           int height,
                           const sptr<OHOS::Ace::IDialogCallback>& dialogCallback,
                           int* id = nullptr) = 0;

    virtual int CancelDialog(int id) = 0;

    enum {
        // ipc id 1-1000 for kit
        // ipc id for RegisterCallBack (1)
        REGISTER_CALLBACK = 1,
        // ipc id for UnregisterCallBack (2)
        UNREGISTER_CALLBACK,
        PUSH,
        REQUEST,
        RETURN_REQUEST,
        SHOW_DIALOG,
        CANCEL_DIALOG,
    };
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_AAFWK_UI_SERVICE_MANAGER_INTERFACE_H
