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

#ifndef OHOS_AAFWK_UI_SERVICE_MANAGER_CLIENT_H
#define OHOS_AAFWK_UI_SERVICE_MANAGER_CLIENT_H

#include <mutex>

#include "ability_info.h"
#include "iremote_object.h"
#include "dialog_callback_interface.h"
#include "ui_service_interface.h"
#include "ui_service_mgr_errors.h"
#include "ui_service_mgr_interface.h"
#include "uri.h"

namespace OHOS {
namespace Ace {
/**
 * @class UIServiceMgrClient
 * UIServiceMgrClient is used to access UIService manager services.
 */
class UIServiceMgrClient {
public:
    UIServiceMgrClient();
    virtual ~UIServiceMgrClient();
    static std::shared_ptr<UIServiceMgrClient> GetInstance();

    ErrCode RegisterCallBack(const AAFwk::Want& want, const sptr<IUIService>& uiService);

    ErrCode UnregisterCallBack(const AAFwk::Want& want);

    ErrCode Push(const AAFwk::Want& want,
        const std::string& name, const std::string& jsonPath, const std::string& data, const std::string& extraData);

    ErrCode Request(const AAFwk::Want& want, const std::string& name, const std::string& data);

    ErrCode ReturnRequest(
        const AAFwk::Want& want, const std::string& source,  const std::string& data,  const std::string& extraData);

    ErrCode ShowDialog(const std::string& name,
                       const std::string& params,
                       OHOS::Rosen::WindowType windowType,
                       int x,
                       int y,
                       int width,
                       int height,
                       DialogCallback callback,
                       int* id = nullptr);

    ErrCode CancelDialog(int32_t id);

    ErrCode ShowAppPickerDialog(
        const AAFwk::Want& want, const std::vector<AppExecFwk::AbilityInfo>& abilityInfos, int32_t userId);

private:
    /**
     * Connect UIService manager service.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode Connect();

    const std::string GetPickerDialogParam(const AAFwk::Want& want,
        const std::vector<AppExecFwk::AbilityInfo>& abilityInfos, bool wideScreen = true) const;
    void GetDisplayPosition(
        int32_t& offsetX, int32_t& offsetY, int32_t& width, int32_t& height, bool& wideScreen);

    static std::mutex mutex_;
    static std::shared_ptr<UIServiceMgrClient> instance_;
    sptr<IRemoteObject> remoteObject_;
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_AAFWK_UI_SERVICE_MANAGER_CLIENT_H
