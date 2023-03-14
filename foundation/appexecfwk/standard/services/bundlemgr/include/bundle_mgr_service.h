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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_H

#include <memory>

#include "singleton.h"
#include "system_ability.h"

#ifdef DEVICE_MANAGER_ENABLE
#include "bms_device_manager.h"
#endif
#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
#include "aging/bundle_aging_mgr.h"
#include "bundle_connect_ability_mgr.h"
#endif
#include "bundle_clone_mgr.h"
#include "bundle_constants.h"
#include "bundle_data_mgr.h"
#include "bundle_installer_host.h"
#include "bundle_mgr_host_impl.h"
#include "bundle_mgr_service_event_handler.h"
#include "bundle_user_mgr_host_impl.h"
#include "distributed_monitor.h"
#include "hidump_helper.h"

namespace OHOS {
namespace AppExecFwk {
class BundleMgrService : public SystemAbility {
    DECLARE_DELAYED_SINGLETON(BundleMgrService);
    DECLEAR_SYSTEM_ABILITY(BundleMgrService);

public:
    /**
     * @brief Start the bundle manager service.
     * @return
     */
    virtual void OnStart() override;
    /**
     * @brief Stop the bundle manager service.
     * @return
     */
    virtual void OnStop() override;
    /**
     * @brief Check whether if the bundle manager service is ready.
     * @return Returns true if the bundle manager service is ready; returns false otherwise.
     */
    bool IsServiceReady() const;
    /**
     * @brief Get a shared pointer to the BundleDataMgr object.
     * @return Returns the pointer of BundleDataMgr object.
     */
    const std::shared_ptr<BundleCloneMgr> GetCloneMgr() const;

    const std::shared_ptr<BundleDataMgr> GetDataMgr() const;
#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
    const std::shared_ptr<BundleAgingMgr> GetAgingMgr() const;
    /**
     * @brief Get a util object for FA  Distribution center
     * @return Returns the pointer of BundleConnectAbility object.
     */
    const std::shared_ptr<BundleConnectAbilityMgr> GetConnectAbility() const;
#endif
    /**
     * @brief Get a IBundleInstaller object for IPC
     * @return Returns the pointer of IBundleInstaller object.
     */
    sptr<IBundleInstaller> GetBundleInstaller() const;
    /**
     * @brief Get a IBundleUserMgr object for IPC
     * @return Returns the pointer of IBundleUserMgr object.
     */
    sptr<BundleUserMgrHostImpl> GetBundleUserMgr() const;
    /**
     * @brief Check all user.
     */
    void CheckAllUser();
    /**
     * @brief register bms sa to samgr
     */
    void RegisterService();
    /**
     * @brief send bundle scan finished common event
     */
    void NotifyBundleScanStatus();
    /**
     * @brief Process hidump.
     * @param args Indicates the args.
     * @param result Indicates the result.
     * @return Returns whether the interface is called successfully.
     */
    bool Hidump(const std::vector<std::string> &args, std::string& result) const;

protected:
#ifdef DEVICE_MANAGER_ENABLE
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
#endif

private:
    /**
     * @brief Initialize the bundle manager service context.
     * @return Returns true if initialized successfully; returns false otherwise.
     */
    bool Init();
    /**
     * @brief Clean the context of this bundle manager service.
     * @return
     */
    void SelfClean();
    /**
     * @brief Do some work after register to service.
     * @return
     */
    void AfterRegisterToService();

private:
    bool ready_ = false;
    bool registerToService_ = false;
    bool needToScan_ = false;
    bool notifyBundleScanStatus = false;
    std::shared_ptr<EventRunner> runner_;
    std::shared_ptr<BMSEventHandler> handler_;
    std::shared_ptr<BundleDataMgr> dataMgr_;
    std::shared_ptr<BundleCloneMgr> cloneMgr_;
#ifdef DEVICE_MANAGER_ENABLE
    std::shared_ptr<BmsDeviceManager> deviceManager_;
#endif
    std::shared_ptr<HidumpHelper> hidumpHelper_;
#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
    std::shared_ptr<BundleAgingMgr> agingMgr_;
    std::shared_ptr<BundleConnectAbilityMgr> connectAbilityMgr_;
#endif
    sptr<BundleMgrHostImpl> host_;
    sptr<BundleInstallerHost> installer_;
    sptr<BundleUserMgrHostImpl> userMgrHost_;
    std::shared_ptr<DistributedMonitor> distributedSub_;

    DISALLOW_COPY_AND_MOVE(BundleMgrService);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_H
