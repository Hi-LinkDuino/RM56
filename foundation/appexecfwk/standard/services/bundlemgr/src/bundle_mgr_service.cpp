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

#include "bundle_mgr_service.h"

#include "ohos/aafwk/content/want.h"

#include "account_helper.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_permission_mgr.h"
#include "datetime_ex.h"
#include "perf_profile.h"
#include "system_ability_definition.h"
#include "system_ability_helper.h"

namespace OHOS {
namespace AppExecFwk {
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<BundleMgrService>::GetInstance().get());

BundleMgrService::BundleMgrService() : SystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, true)
{
    APP_LOGI("instance is created");
    PerfProfile::GetInstance().SetBmsLoadStartTime(GetTickCount());
}

BundleMgrService::~BundleMgrService()
{
    if (host_ != nullptr) {
        host_ = nullptr;
    }
    if (installer_ != nullptr) {
        installer_ = nullptr;
    }
    if (handler_) {
        handler_.reset();
    }
    if (runner_) {
        runner_.reset();
    }
    if (dataMgr_) {
        dataMgr_.reset();
    }
    if (cloneMgr_) {
        cloneMgr_.reset();
    }
#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
    if (connectAbilityMgr_ != nullptr) {
        connectAbilityMgr_.reset();
    }
#endif
    if (hidumpHelper_) {
        hidumpHelper_.reset();
    }
    APP_LOGI("instance is destroyed");
}

void BundleMgrService::OnStart()
{
    APP_LOGD("start is triggered");
    if (!Init()) {
        APP_LOGE("init fail");
        return;
    }

    PerfProfile::GetInstance().SetBmsLoadEndTime(GetTickCount());
    if (!needToScan_) {
        PerfProfile::GetInstance().Dump();
    }
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
#ifdef DEVICE_MANAGER_ENABLE
    AddSystemAbilityListener(DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID);
    AddSystemAbilityListener(DISTRIBUTED_BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
#endif
}

void BundleMgrService::AfterRegisterToService()
{
    if (!distributedSub_) {
        EventFwk::MatchingSkills matchingSkills;
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
        EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
        distributedSub_ = std::make_shared<DistributedMonitor>(dataMgr_, subscribeInfo);
        EventFwk::CommonEventManager::SubscribeCommonEvent(distributedSub_);
    }
}

void BundleMgrService::OnStop()
{
    APP_LOGI("OnStop is called");
    SelfClean();
    if (distributedSub_) {
        EventFwk::CommonEventManager::UnSubscribeCommonEvent(distributedSub_);
    }
}

bool BundleMgrService::IsServiceReady() const
{
    return ready_;
}

bool BundleMgrService::Init()
{
    if (ready_) {
        APP_LOGW("init more than one time");
        return false;
    }

    if (host_ == nullptr) {
        host_ = new (std::nothrow) BundleMgrHostImpl();
        if (!host_) {
            APP_LOGE("create host instance fail");
            return false;
        }
    }

    APP_LOGI("init begin");
    if (!runner_) {
        runner_ = EventRunner::Create(Constants::BMS_SERVICE_NAME);
        if (!runner_) {
            APP_LOGE("create runner fail");
            return false;
        }
    }
    APP_LOGD("create runner success");

    if (!handler_) {
        handler_ = std::make_shared<BMSEventHandler>(runner_);
        if (!handler_) {
            APP_LOGE("create bms event handler fail");
            return false;
        }
    }
    APP_LOGD("create handler success");

    if (!installer_) {
        installer_ = new (std::nothrow) BundleInstallerHost();
        if (!installer_ || !installer_->Init()) {
            APP_LOGE("init installer fail");
            return false;
        }
    }
    APP_LOGD("create installer host success");

    if (!dataMgr_) {
        APP_LOGI("Create BundledataMgr");
        dataMgr_ = std::make_shared<BundleDataMgr>();
        if (!dataMgr_) {
            APP_LOGE("create data manager fail");
            return false;
        }
    }
    APP_LOGD("create dataManager success");

    if (userMgrHost_ == nullptr) {
        userMgrHost_ = new (std::nothrow) BundleUserMgrHostImpl();
        if (!userMgrHost_) {
            APP_LOGE("create userMgrHost instance fail");
            return false;
        }
    }
    APP_LOGD("create userMgrHost success");
    if (!(dataMgr_->LoadDataFromPersistentStorage())) {
        APP_LOGW("load data from persistent storage fail");
        dataMgr_->AddUserId(Constants::DEFAULT_USERID);
        handler_->SendEvent(BMSEventHandler::BUNDLE_SCAN_START);
        needToScan_ = true;
    } else {
        APP_LOGD("Reboot start scan");
        handler_->SendEvent(BMSEventHandler::BUNDLE_REBOOT_SCAN_START);
    }

    if (!cloneMgr_) {
        APP_LOGI("Create BundleCloneMgr");
        cloneMgr_ = std::make_shared<BundleCloneMgr>();
    }
    APP_LOGI("create BundleCloneMgr success");

#ifdef DEVICE_MANAGER_ENABLE
    if (!deviceManager_) {
        APP_LOGI("Create device manager");
        deviceManager_ = std::make_shared<BmsDeviceManager>();
    }
#endif

    if (!hidumpHelper_) {
        APP_LOGI("Create hidump helper");
        hidumpHelper_ = std::make_shared<HidumpHelper>(dataMgr_);
    }

#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
    if (!agingMgr_) {
        APP_LOGI("Create aging manager");
        agingMgr_ = DelayedSingleton<BundleAgingMgr>::GetInstance();
        if (!agingMgr_) {
            APP_LOGE("Create aging manager faild.");
        }
        if (agingMgr_) {
            APP_LOGI("Create aging manager success.");
            agingMgr_->InitAgingRunner();
            agingMgr_->InitAgingtTimer();
        }
    }
    if (!connectAbilityMgr_) {
        APP_LOGI("Create BundleConnectAbility");
        connectAbilityMgr_ = std::make_shared<BundleConnectAbilityMgr>();
    }
    APP_LOGI("create BundleConnectAbility success");
#endif

    CheckAllUser();
    ready_ = true;
    APP_LOGI("init end success");
    return true;
}

sptr<IBundleInstaller> BundleMgrService::GetBundleInstaller() const
{
    return installer_;
}

const std::shared_ptr<BundleDataMgr> BundleMgrService::GetDataMgr() const
{
    return dataMgr_;
}

#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
const std::shared_ptr<BundleAgingMgr> BundleMgrService::GetAgingMgr() const
{
    return agingMgr_;
}

const std::shared_ptr<BundleConnectAbilityMgr> BundleMgrService::GetConnectAbility() const
{
    return connectAbilityMgr_;
}
#endif

const std::shared_ptr<BundleCloneMgr> BundleMgrService::GetCloneMgr() const
{
    return cloneMgr_;
}

void BundleMgrService::SelfClean()
{
    if (ready_) {
        ready_ = false;
        if (registerToService_) {
            registerToService_ = false;
        }
        if (needToScan_) {
            needToScan_ = false;
        }
    }
}

sptr<BundleUserMgrHostImpl> BundleMgrService::GetBundleUserMgr() const
{
    return userMgrHost_;
}

void BundleMgrService::CheckAllUser()
{
    if (!dataMgr_) {
        return;
    }

    APP_LOGD("Check all user start.");
    std::set<int32_t> userIds = dataMgr_->GetAllUser();
    for (auto userId : userIds) {
        bool isExists = false;
        if (AccountHelper::IsOsAccountExists(userId, isExists) != ERR_OK) {
            APP_LOGE("Failed to query whether the user(%{public}d) exists.", userId);
            continue;
        }

        if (!isExists) {
            userMgrHost_->RemoveUser(userId);
        }
    }
    APP_LOGD("Check all user end");
}

void BundleMgrService::RegisterService()
{
    if (!registerToService_) {
        if (!SystemAbilityHelper::AddSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, host_)) {
            APP_LOGE("fail to register to system ability manager");
            return;
        }
        APP_LOGI("register to sam success");
        registerToService_ = true;
    }
    AfterRegisterToService();
}

void BundleMgrService::NotifyBundleScanStatus()
{
    APP_LOGD("PublishCommonEvent for bundle scan finished");
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BUNDLE_SCAN_FINISHED);
    EventFwk::CommonEventData commonEventData { want };
    if (!EventFwk::CommonEventManager::PublishCommonEvent(commonEventData)) {
        notifyBundleScanStatus = true;
        APP_LOGE("PublishCommonEvent for bundle scan finished failed.");
    } else {
        APP_LOGD("PublishCommonEvent for bundle scan finished succeed.");
    }
}

void BundleMgrService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    APP_LOGD("OnAddSystemAbility systemAbilityId:%{public}d added!", systemAbilityId);
#ifdef DEVICE_MANAGER_ENABLE
    if (deviceManager_) {
        deviceManager_->OnAddSystemAbility(systemAbilityId, deviceId);
    }
#endif
    if (COMMON_EVENT_SERVICE_ID == systemAbilityId && notifyBundleScanStatus) {
        NotifyBundleScanStatus();
    }
}

void BundleMgrService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    APP_LOGD("OnRemoveSystemAbility systemAbilityId:%{public}d removed!", systemAbilityId);
#ifdef DEVICE_MANAGER_ENABLE
    if (deviceManager_) {
        deviceManager_->OnRemoveSystemAbility(systemAbilityId, deviceId);
    }
#endif
}

bool BundleMgrService::Hidump(const std::vector<std::string> &args, std::string& result) const
{
    if (hidumpHelper_ && hidumpHelper_->Dump(args, result)) {
        return true;
    }

    APP_LOGD("HidumpHelper failed");
    return false;
}
}  // namespace AppExecFwk
}  // namespace OHOS
