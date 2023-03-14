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

#ifndef OHOS_AAFWK_UI_SERVICE_MANAGER_SERVICE_H
#define OHOS_AAFWK_UI_SERVICE_MANAGER_SERVICE_H

#include <memory>
#include <singleton.h>
#include <thread_ex.h>
#include <unordered_map>

#include "ability.h"
#include "element_name.h"
#include "event_handler.h"
#include "event_runner.h"
#include "hilog_wrapper.h"
#include "iremote_object.h"
#include "resource_manager.h"
#include "runtime.h"
#include "system_ability.h"
#include "ui_service_mgr_stub.h"
#include "uri.h"

namespace OHOS {
namespace Ace {
using EventRunner = OHOS::AppExecFwk::EventRunner;
using EventHandler = OHOS::AppExecFwk::EventHandler;
enum class UIServiceRunningState { STATE_NOT_START, STATE_RUNNING };

class UIMgrService : public SystemAbility,
                          public UIServiceMgrStub,
                          public std::enable_shared_from_this<UIMgrService> {
    DECLARE_DELAYED_SINGLETON(UIMgrService)
    DECLEAR_SYSTEM_ABILITY(UIMgrService)
public:
    void OnStart() override;
    void OnStop() override;
    UIServiceRunningState QueryServiceState() const;

    int RegisterCallBack(const AAFwk::Want& want, const sptr<IUIService>& uiService) override;
    int UnregisterCallBack(const AAFwk::Want& want) override;
    int Push(const AAFwk::Want& want, const std::string& name, const std::string& jsonPath,
        const std::string& data, const std::string& extraData) override;

    int Request(const AAFwk::Want& want, const std::string& name, const std::string& data) override;
    int ReturnRequest(const AAFwk::Want& want, const std::string& source, const std::string& data,
        const std::string& extraData) override;

    int ShowDialog(const std::string& name,
                           const std::string& params,
                           OHOS::Rosen::WindowType windowType,
                           int x,
                           int y,
                           int width,
                           int height,
                           const sptr<OHOS::Ace::IDialogCallback>& dialogCallback,
                           int* id = nullptr) override;

    int CancelDialog(int id) override;

    /**
     * GetEventHandler, get the ui_service manager service's handler.
     *
     * @return Returns EventHandler ptr.
     */
    std::shared_ptr<EventHandler> GetEventHandler();

private:
    bool Init();
    bool CheckCallBackFromMap(const std::string& key);
    int HandleRegister(const AAFwk::Want& want,  const sptr<IUIService>& uiService);
    int HandleUnregister(const AAFwk::Want& want);
    void InitResourceManager();
    std::shared_ptr<OHOS::AppExecFwk::Ability> CreateAbility();
    std::string GetCallBackKeyStr(const AAFwk::Want& want);
    std::shared_ptr<EventRunner> eventLoop_;
    std::shared_ptr<EventHandler> handler_;
    UIServiceRunningState state_;
    std::map<std::string, sptr<IUIService>> callbackMap_;
    std::mutex uiMutex_;
    std::unordered_map<int32_t, std::shared_ptr<OHOS::AppExecFwk::Ability>> abilityMaps_;

    const std::string PUSH_STRING = "PUSH";
    const std::string REQUEST_STRING = "REQUEST";
    float density_ = 2.0f; // 2.0 for wgr, 3.0 for phone
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager_ = nullptr;
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_AAFWK_UI_SERVICE_MANAGER_SERVICE_H
