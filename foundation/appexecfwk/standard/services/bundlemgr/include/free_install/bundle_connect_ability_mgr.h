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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_BUNDLE_CONNECT_ABILITY_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_BUNDLE_CONNECT_ABILITY_H

#include <condition_variable>
#include <mutex>
#include <semaphore.h>
#include <string>

#include "ability_connect_callback_interface.h"
#include "ability_manager_interface.h"
#include "event_handler.h"
#include "event_runner.h"
#include "free_install_params.h"
#include "inner_bundle_info.h"
#include "install_result.h"
#include "ipc_skeleton.h"
#include "iremote_broker.h"
#include "ohos/aafwk/content/want.h"
#include "service_center_connection.h"
#include "system_ability_definition.h"
#include "system_ability_helper.h"

namespace OHOS {
namespace AppExecFwk {
class BundleConnectAbilityMgr : public std::enable_shared_from_this<BundleConnectAbilityMgr> {
public:
    BundleConnectAbilityMgr();
    ~BundleConnectAbilityMgr();

    /**
     * @brief Query the AbilityInfo by the given Want.
     * @param want Indicates the information of the ability.
     * @param flags Indicates the information contained in the AbilityInfo object to be returned.
     * @param userId Indicates the user ID.
     * @param abilityInfo Indicates the obtained AbilityInfo object.
     * @param callBack Indicates the callback object for ability manager service.
     * @return Returns true if the AbilityInfo is successfully obtained; returns false otherwise.
     */
    bool QueryAbilityInfo(const Want &want, int32_t flags, int32_t userId,
        AbilityInfo &abilityInfo, const sptr<IRemoteObject> &callBack);
    /**
     * @brief Upgrade atomic service status
     * @param want Query the AbilityInfo by the given Want.
     * @param userId Indicates the user ID.
     */
    void UpgradeAtomicService(const Want &want, int32_t userId);
    /**
     * @brief Executed when a service callback is retrieved
     * @param installResult The json string of InstallResult
     */
    void OnServiceCenterCall(std::string installResult);

private:
    void Init();
    /**
     * @brief Notify the service center center to start the installation free process.
     * @param targetAbilityInfo Indicates the information which will be send to service center.
     * @param want Indicates the information of the need start ability.
     * @param callerToken Caller form extension token.
     * @param userId Designation User ID.
     * @return Returns true if create async task successfully called; returns false otherwise.
     */
    bool SilentInstall(const TargetAbilityInfo &targetAbilityInfo, const Want &want,
        const sptr<IRemoteObject> &callerToken, int32_t userId);

    /**
     * @brief Notify the service center to check for updates.
     * @param targetAbilityInfo Indicates the information which will be send to service center.
     * @param want Indicates the information of the need start ability.
     * @param callerToken Caller form extension token.
     * @param userId Designation User ID.
     * @return Returns true if create async task successfully called; returns false otherwise.
     */
    bool UpgradeCheck(const TargetAbilityInfo &targetAbilityInfo, const Want &want,
        const sptr<IRemoteObject> &callerToken, int32_t userId);

    /**
     * @brief Notify the service center to install new ability.
     * @param targetAbilityInfo Indicates the information which will be send to service center.
     * @param want Indicates the information of the need start ability.
     * @param callerToken Caller form extension token.
     * @param userId Designation User ID.
     * @return Returns true if create async task successfully called; returns false otherwise.
     */
    bool UpgradeInstall(const TargetAbilityInfo &targetAbilityInfo, const Want &want,
        const sptr<IRemoteObject> &callerToken, int32_t userId);

    /**
     * @brief Obtains the Calling Info object
     * @param innerBundleInfo Indicates the innerBundleInfo of the bundle which will be returned.
     * @param bundleNames Indicates the obtained bundle names.
     * @param callingAppids Indicates the ids of teh calling app.
     * @return Returns true if get callingInfo successfully; returns false otherwise.
     */
    void GetCallingInfo(InnerBundleInfo &innerBundleInfo,
        std::vector<std::string> &bundleNames, std::vector<std::string>& callingAppIds);

    /**
     * @brief Obtains the target ability Info object which will be send to service center.
     * @param want Indicates the information of the ability.
     * @param innerBundleInfo Indicates the innerBundleInfo of the bundle which will be using.
     * @param targetAbilityInfo Indicates the targetAbilityInfo of the bundle which will be returned.
     * @param targetInfo Indicates the targetInfo of the bundle which will be returned.
     */
    void GetTargetAbilityInfo(const Want &want, InnerBundleInfo &innerBundleInfo,
        sptr<TargetAbilityInfo> &targetAbilityInfo, sptr<TargetInfo> &targetInfo);

    /**
     * @brief Check is need update module.
     * @param innerBundleInfo Indicates the innerBundleInfo of the bundle which will be using.
     * @param want Indicates the information of the ability.
     * @param userId Indicates the user ID.
     * @param callBack Indicates the callback object for ability manager service.
     * @return Returns true if module need update.
     */
    bool CheckIsModuleNeedUpdate(
        InnerBundleInfo &innerBundleInfo, const Want &want, int32_t userId, const sptr<IRemoteObject> &callBack);

    /**
     * @brief Connecte service center
     * @param callerToken Caller form extension token.
     * @return Returns true if successfully connected to service center; returns false otherwise.
     */
    bool ConnectAbility(const Want &want, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Disconnect service center
     */
    void DisconnectAbility();

    /**
     * @brief Send atomic service status callback to ability manager service
     * @param resultCode The result code to ability manager service call back
     * @param want Indicates the information of the need start ability.
     * @param userId Designation User ID.
     * @param transactId The key of ability manager service Call Back Map
     */
    void SendCallBack(int32_t resultCode, const Want &want, int32_t userId, std::string transactId);

    /**
     * @brief Determine whether there are reusable connection
     * @param flag Indicates service function
     * @param targetAbilityInfo Indicates the information of the ability.
     * @param want Indicates the information of the need start ability.
     * @param userId Designation User ID.
     * @param callerToken Caller form extension token.
     * @return Returns true if successfully Send request with RemoteObject
     */
    bool SendRequestToServiceCenter(int32_t flag, const TargetAbilityInfo &targetAbilityInfo, const Want &want,
        int32_t userId, const sptr<IRemoteObject> &callerToken);

    /**
     * @brief Send request with RemoteObject,this is a asynchronous function.
     * @param flag Indicates service function
     * @param targetAbilityInfo Indicates the information of the ability.
     * @param want Indicates the information of the need start ability.
     * @param userId Designation User ID.
     */
    void SendRequest(int32_t flag, const TargetAbilityInfo &targetAbilityInfo, const Want &want, int32_t userId);

    /**
     * @brief Get the ability manager service Call Back with transactId
     * @param transactId The key of ability manager service callback.
     * @return Return the Indicates callback to be invoked for return ability manager service the operation result.
     */
    sptr<IRemoteObject> GetAbilityManagerServiceCallBack(std::string transactId);

    /**
     * @brief Listening service center processing timeout
     * @param transactId The key of ability manager service callback.
     */
    void OutTimeMonitor(std::string transactId);

    int GetTransactId() const
    {
        transactId_++;
        return transactId_.load();
    }

    /**
     * @brief Send callback to ability manager service
     * @param resultCode The result code to ability manager service call back
     * @param want Indicates the information of the ability.
     * @param userId Indicates the user ID.
     * @param callBack Indicates the callback object for ability manager service.
     */
    void CallAbilityManager(int32_t resultCode, const Want &want, int32_t userId, const sptr<IRemoteObject> &callBack);

    /**
     * @brief Judge whether the ability information has been queried
     * @param want Indicates the information of the ability.
     * @param flags Indicates the information contained in the AbilityInfo object to be returned.
     * @param userId Indicates the user ID.
     * @param abilityInfo Indicates the obtained AbilityInfo object.
     * @param callBack Indicates the callback object for ability manager service.
     * @return Returns true if the ability information has been queried; returns false otherwise.
     */
    bool IsObtainAbilityInfo(const Want &want, int32_t flags, int32_t userId, AbilityInfo &abilityInfo,
        const sptr<IRemoteObject> &callBack, InnerBundleInfo &innerBundleInfo);

    bool GetAbilityMgrProxy();
    void WaitFromConnecting(std::unique_lock<std::mutex> &lock);
    void WaitFromConnected(std::unique_lock<std::mutex> &lock);

    mutable std::atomic<int> transactId_ = 0;
    std::condition_variable cv_;
    std::mutex mutex_;
    sptr<AAFwk::IAbilityManager> abilityMgrProxy_;
    sptr<ServiceCenterConnection> serviceCenterConnection_;
    std::map<std::string, FreeInstallParams> freeInstallParamsMap_;
    sptr<IRemoteObject> serviceCenterRemoteObject_;
    int32_t connectState_ = ServiceCenterConnectState::DISCONNECTED;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    std::shared_ptr<EventRunner> runner_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_BUNDLE_CONNECT_ABILITY_H
