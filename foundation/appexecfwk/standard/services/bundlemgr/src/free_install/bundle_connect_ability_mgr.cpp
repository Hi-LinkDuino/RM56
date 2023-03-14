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

#include "bundle_connect_ability_mgr.h"

#include "app_log_wrapper.h"
#include "bundle_mgr_service.h"
#include "free_install_params.h"
#include "json_util.h"
#include "parcel.h"
#include "service_center_death_recipient.h"
#include "service_center_status_callback.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::u16string ATOMIC_SERVICE_STATUS_CALLBACK_TOKEN = u"ohos.aafwk.IAtomicServiceStatusCallback";
const std::string serviceCenterBundleName = "com.ohos.hag.famanager";
const std::string serviceCenterAbilityName = "com.ohos.hag.famanager.HapInstallServiceAbility";
constexpr uint32_t FREE_INSTALL_DONE = 0;
constexpr uint32_t DEFAULT_VERSION = 1;
constexpr uint32_t CALLING_TYPE_HARMONY = 2;
constexpr uint32_t BIT_ONE_COMPATIBLE = 0;
constexpr uint32_t BIT_TWO_BACK_MODE = 1;
constexpr uint32_t BIT_THREE_CUSTOM = 0;
constexpr uint32_t BIT_ONE_FIVE_AZ_DEVICE = 0;
constexpr uint32_t BIT_ONE_SEVEN_SAME_BUNDLE = 0;
constexpr uint32_t BIT_TWO = 2;
constexpr uint32_t BIT_THREE = 4;
constexpr uint32_t BIT_FOUR = 8;
constexpr uint32_t BIT_FIVE = 16;
constexpr uint32_t BIT_SIX = 32;
constexpr uint32_t BIT_SEVEN = 64;
constexpr uint32_t OUT_TIME = 30000;

void SendSysEvent(int32_t resultCode, const AAFwk::Want &want, int32_t userId)
{
    EventInfo sysEventInfo;
    ElementName element = want.GetElement();
    sysEventInfo.bundleName = element.GetBundleName();
    sysEventInfo.moduleName = element.GetModuleName();
    sysEventInfo.abilityName = element.GetAbilityName();
    sysEventInfo.isFreeInstallMode = true;
    sysEventInfo.userId = userId;
    sysEventInfo.errCode = resultCode;
    EventReport::SendSystemEvent(BMSEventType::BUNDLE_INSTALL_EXCEPTION, sysEventInfo);
}
}

void BundleConnectAbilityMgr::Init()
{
    runner_ = EventRunner::Create(true);
    if (runner_ == nullptr) {
        APP_LOGE("Create runner failed");
        return;
    }

    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    if (handler_ == nullptr) {
        APP_LOGE("Create handler failed");
    }
}

BundleConnectAbilityMgr::BundleConnectAbilityMgr()
{
    Init();
}

BundleConnectAbilityMgr::~BundleConnectAbilityMgr()
{
    if (handler_ != nullptr) {
        handler_.reset();
    }
    if (runner_ != nullptr) {
        runner_.reset();
    }
}

bool BundleConnectAbilityMgr::SilentInstall(const TargetAbilityInfo &targetAbilityInfo, const Want &want,
    const sptr<IRemoteObject> &callerToken, int32_t userId)
{
    APP_LOGI("SilentInstall");
    if (handler_ == nullptr) {
        SendCallBack(FreeInstallErrorCode::UNDEFINED_ERROR, want,
            userId, targetAbilityInfo.targetInfo.transactId);
        SendSysEvent(FreeInstallErrorCode::UNDEFINED_ERROR, want, userId);
        APP_LOGE("handler is null");
        return false;
    }
    auto silentInstallFunc = [this, targetAbilityInfo, want, userId, callerToken]() {
        int32_t flag = ServiceCenterFunction::CONNECT_SILENT_INSTALL;
        this->SendRequestToServiceCenter(flag, targetAbilityInfo, want, userId, callerToken);
    };
    handler_->PostTask(silentInstallFunc, targetAbilityInfo.targetInfo.transactId.c_str());
    return true;
}

bool BundleConnectAbilityMgr::UpgradeCheck(const TargetAbilityInfo &targetAbilityInfo, const Want &want,
    const sptr<IRemoteObject> &callerToken, int32_t userId)
{
    APP_LOGI("UpgradeCheck");
    if (handler_ == nullptr) {
        SendCallBack(FreeInstallErrorCode::UNDEFINED_ERROR, want,
            userId, targetAbilityInfo.targetInfo.transactId);
        SendSysEvent(FreeInstallErrorCode::UNDEFINED_ERROR, want, userId);
        APP_LOGE("handler is null");
        return false;
    }
    auto upgradeCheckFunc = [this, targetAbilityInfo, want, userId, callerToken]() {
        int32_t flag = ServiceCenterFunction::CONNECT_UPGRADE_CHECK;
        this->SendRequestToServiceCenter(flag, targetAbilityInfo, want, userId, callerToken);
    };
    handler_->PostTask(upgradeCheckFunc, targetAbilityInfo.targetInfo.transactId.c_str());
    return true;
}

bool BundleConnectAbilityMgr::UpgradeInstall(const TargetAbilityInfo &targetAbilityInfo, const Want &want,
    const sptr<IRemoteObject> &callerToken, int32_t userId)
{
    APP_LOGI("UpgradeInstall");
    if (handler_ == nullptr) {
        SendCallBack(FreeInstallErrorCode::UNDEFINED_ERROR, want,
            userId, targetAbilityInfo.targetInfo.transactId);
        SendSysEvent(FreeInstallErrorCode::UNDEFINED_ERROR, want, userId);
        APP_LOGE("handler is null");
        return false;
    }
    auto upgradeInstallFunc = [this, targetAbilityInfo, want, userId, callerToken]() {
        int32_t flag = ServiceCenterFunction::CONNECT_UPGRADE_INSTALL;
        this->SendRequestToServiceCenter(flag, targetAbilityInfo, want, userId, callerToken);
    };
    handler_->PostTask(upgradeInstallFunc, targetAbilityInfo.targetInfo.transactId.c_str());
    return true;
}

bool BundleConnectAbilityMgr::SendRequestToServiceCenter(int32_t flag,
    const TargetAbilityInfo &targetAbilityInfo, const Want &want,
    int32_t userId, const sptr<IRemoteObject> &callerToken)
{
    APP_LOGI("SendRequestToServiceCenter");
    Want serviceCenterWant;
    serviceCenterWant.SetElementName(serviceCenterBundleName, serviceCenterAbilityName);
    bool isConnectSuccess = ConnectAbility(serviceCenterWant, callerToken);
    if (!isConnectSuccess) {
        APP_LOGE("Fail to connect ServiceCenter");
        SendCallBack(FreeInstallErrorCode::CONNECT_ERROR, want,
            userId, targetAbilityInfo.targetInfo.transactId);
        SendSysEvent(FreeInstallErrorCode::CONNECT_ERROR, want, userId);
        return false;
    } else {
        SendRequest(flag, targetAbilityInfo, want, userId);
        return true;
    }
}

void BundleConnectAbilityMgr::DisconnectAbility()
{
    if (serviceCenterConnection_ != nullptr) {
        APP_LOGI("DisconnectAbility");
        int result = abilityMgrProxy_->DisconnectAbility(serviceCenterConnection_);
        if (result != ERR_OK) {
            APP_LOGE("BundleConnectAbilityMgr::DisconnectAbility fail, resultCode: %{public}d", result);
        }
    }
}

void BundleConnectAbilityMgr::WaitFromConnecting(std::unique_lock<std::mutex> &lock)
{
    APP_LOGI("ConnectAbility await start CONNECTING");
    while (connectState_ == ServiceCenterConnectState::CONNECTING) {
            cv_.wait(lock);
    }
    APP_LOGI("ConnectAbility await end CONNECTING");
}

void BundleConnectAbilityMgr::WaitFromConnected(std::unique_lock<std::mutex> &lock)
{
    APP_LOGI("ConnectAbility await start CONNECTED");
    while (connectState_ != ServiceCenterConnectState::CONNECTED) {
        cv_.wait(lock);
    }
    APP_LOGI("ConnectAbility await end CONNECTED");
}

bool BundleConnectAbilityMgr::ConnectAbility(const Want &want, const sptr<IRemoteObject> &callerToken)
{
    APP_LOGI("ConnectAbility start target bundle = %{public}s", want.GetBundle().c_str());
    std::unique_lock<std::mutex> lock(mutex_);
    if (connectState_ == ServiceCenterConnectState::CONNECTING) {
        WaitFromConnecting(lock);
    } else if (connectState_ == ServiceCenterConnectState::DISCONNECTED) {
        connectState_ = ServiceCenterConnectState::CONNECTING;
        if (!GetAbilityMgrProxy()) {
            connectState_ = ServiceCenterConnectState::DISCONNECTED;
            cv_.notify_all();
            return false;
        }
        serviceCenterConnection_ = new (std::nothrow) ServiceCenterConnection(connectState_,
            cv_, freeInstallParamsMap_);
        if (serviceCenterConnection_ == nullptr) {
            APP_LOGE("ServiceCenterConnection is nullptr");
            connectState_ = ServiceCenterConnectState::DISCONNECTED;
            cv_.notify_all();
            return false;
        }
        APP_LOGI("ConnectAbility start");
        int result = abilityMgrProxy_->ConnectAbility(want, serviceCenterConnection_, callerToken);
        if (result == ERR_OK) {
            if (connectState_ != ServiceCenterConnectState::CONNECTED) {
                WaitFromConnected(lock);
                serviceCenterRemoteObject_ = serviceCenterConnection_->GetRemoteObject();
            }
        } else {
            APP_LOGE("ConnectAbility fail result = %{public}d", result);
        }
    }

    APP_LOGI("ConnectAbility end");
    if (connectState_ == ServiceCenterConnectState::CONNECTED) {
        return true;
    } else {
        APP_LOGE("ConnectAbility fail");
        connectState_ = ServiceCenterConnectState::DISCONNECTED;
        return false;
    }
}

bool BundleConnectAbilityMgr::GetAbilityMgrProxy()
{
    if (abilityMgrProxy_ == nullptr) {
        abilityMgrProxy_ =
            iface_cast<AAFwk::IAbilityManager>(SystemAbilityHelper::GetSystemAbility(ABILITY_MGR_SERVICE_ID));
    }
    if ((abilityMgrProxy_ == nullptr) || (abilityMgrProxy_->AsObject() == nullptr)) {
        APP_LOGE("Failed to get system ability manager services ability");
        return false;
    }
    return true;
}

void BundleConnectAbilityMgr::SendCallBack(
    int32_t resultCode, const AAFwk::Want &want, int32_t userId, std::string transactId)
{
    APP_LOGI("SendCallBack");
    sptr<IRemoteObject> amsCallBack = GetAbilityManagerServiceCallBack(transactId);
    if (amsCallBack == nullptr) {
        APP_LOGE("Abilitity manager callback is null");
        DisconnectAbility();
        return;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(ATOMIC_SERVICE_STATUS_CALLBACK_TOKEN)) {
        APP_LOGE("Write interface token failed");
        return;
    }
    if (!data.WriteInt32(resultCode)) {
        APP_LOGE("Write result code failed");
        return;
    }
    if (!data.WriteParcelable(&want)) {
        APP_LOGE("Write want failed");
        return;
    }
    if (!data.WriteInt32(userId)) {
        APP_LOGE("Write userId failed");
        return;
    }
    MessageParcel reply;
    MessageOption option;

    if (amsCallBack->SendRequest(FREE_INSTALL_DONE, data, reply, option) != ERR_OK) {
        APP_LOGE("BundleConnectAbilityMgr::SendCallBack SendRequest failed");
    }

    freeInstallParamsMap_.erase(transactId);
    if (freeInstallParamsMap_.size() == 0) {
        if (connectState_ == ServiceCenterConnectState::CONNECTED) {
            APP_LOGI("Disconnect Ability.");
            DisconnectAbility();
        }
    }
}

void BundleConnectAbilityMgr::OnServiceCenterCall(std::string installResultStr)
{
    APP_LOGI("OnServiceCenterCall start, installResultStr = %{public}s", installResultStr.c_str());
    InstallResult installResult;
    if (!ParseInfoFromJsonStr(installResultStr.c_str(), installResult)) {
        APP_LOGE("Parse info from json fail");
        return;
    }
    APP_LOGI("OnServiceCenterCall, retCode = %{public}d", installResult.result.retCode);
    FreeInstallParams freeInstallParams;
    auto node = freeInstallParamsMap_.find(installResult.result.transactId);
    if (node == freeInstallParamsMap_.end()) {
        APP_LOGE("Can not find node in %{public}s function", __func__);
        return;
    }
    freeInstallParams = node->second;
    if (installResult.result.retCode == ServiceCenterResultCode::FREE_INSTALL_DOWNLOADING) {
        APP_LOGI("ServiceCenter is downloading, downloadSize = %{public}d, totalSize = %{public}d",
            installResult.progress.downloadSize, installResult.progress.totalSize);
        return;
    }
    APP_LOGI("serviceCenterFunction = %{public}d", freeInstallParams.serviceCenterFunction);
    SendCallBack(installResult.result.retCode, freeInstallParams.want, freeInstallParams.userId,
        installResult.result.transactId);
    APP_LOGI("OnServiceCenterCall end");
}

void BundleConnectAbilityMgr::OutTimeMonitor(std::string transactId)
{
    APP_LOGI("BundleConnectAbilityMgr::OutTimeMonitor");
    FreeInstallParams freeInstallParams;
    auto node = freeInstallParamsMap_.find(transactId);
    if (node == freeInstallParamsMap_.end()) {
        APP_LOGE("Can not find node in %{public}s function", __func__);
        return;
    }
    freeInstallParams = node->second;
    if (handler_ == nullptr) {
        APP_LOGE("OutTimeMonitor, handler is nullptr");
        return;
    }
    auto RegisterEventListenerFunc = [this, freeInstallParams, transactId]() {
        this->SendCallBack(FreeInstallErrorCode::SERVICE_CENTER_TIMEOUT,
            freeInstallParams.want, freeInstallParams.userId, transactId);
        APP_LOGI("RegisterEventListenerFunc");
    };
    handler_->PostTask(RegisterEventListenerFunc, OUT_TIME, AppExecFwk::EventQueue::Priority::LOW);
}

void BundleConnectAbilityMgr::SendRequest(
    int32_t flag, const TargetAbilityInfo &targetAbilityInfo, const Want &want, int32_t userId)
{
    APP_LOGI("BundleConnectAbilityMgr::SendRequest");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    const std::string dataString = GetJsonStrFromInfo(targetAbilityInfo);
    APP_LOGI("TargetAbilityInfo - > ToJsonString : %{public}s", dataString.c_str());

    if (!data.WriteString16(Str8ToStr16(dataString))) {
        APP_LOGE("%{public}s failed to WriteParcelable targetAbilityInfo", __func__);
        SendCallBack(FreeInstallErrorCode::UNDEFINED_ERROR, want, userId,
            targetAbilityInfo.targetInfo.transactId);
        SendSysEvent(FreeInstallErrorCode::UNDEFINED_ERROR, want, userId);
        return;
    }
    sptr<ServiceCenterStatusCallback> serviceCenterCallback =
        new(std::nothrow) ServiceCenterStatusCallback(weak_from_this());
    if (serviceCenterCallback == nullptr) {
        APP_LOGE("serviceCenterCallback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(serviceCenterCallback)) {
        APP_LOGE("%{public}s failed to WriteRemoteObject callbcak", __func__);
        SendCallBack(FreeInstallErrorCode::UNDEFINED_ERROR, want, userId,
            targetAbilityInfo.targetInfo.transactId);
        SendSysEvent(FreeInstallErrorCode::UNDEFINED_ERROR, want, userId);
        return;
    }
    APP_LOGI("ServiceCenterRemoteObject->SendRequest");
    serviceCenterRemoteObject_ = serviceCenterConnection_->GetRemoteObject();
    if (serviceCenterRemoteObject_ == nullptr) {
        APP_LOGE("%{public}s failed to get remote object", __func__);
        SendCallBack(
            FreeInstallErrorCode::CONNECT_ERROR, want, userId, targetAbilityInfo.targetInfo.transactId);
        SendSysEvent(FreeInstallErrorCode::CONNECT_ERROR, want, userId);
        return;
    }
    int32_t result = serviceCenterRemoteObject_->SendRequest(flag, data, reply, option);
    APP_LOGI("SendRequest result = %{public}d", result);
    if (result != ERR_OK) {
        APP_LOGE("Failed to sendRequest, result = %{public}d", result);
        SendCallBack(
            FreeInstallErrorCode::CONNECT_ERROR, want, userId, targetAbilityInfo.targetInfo.transactId);
        SendSysEvent(FreeInstallErrorCode::CONNECT_ERROR, want, userId);
        return;
    }
    OutTimeMonitor(targetAbilityInfo.targetInfo.transactId);
}

sptr<IRemoteObject> BundleConnectAbilityMgr::GetAbilityManagerServiceCallBack(std::string transactId)
{
    APP_LOGI("GetAbilityManagerServiceCallBack");
    FreeInstallParams freeInstallParams;
    auto node = freeInstallParamsMap_.find(transactId);
    if (node == freeInstallParamsMap_.end()) {
        APP_LOGE("Can not find node in %{public}s function", __func__);
        return nullptr;
    }
    freeInstallParams = node->second;
    return freeInstallParams.callback;
}

void BundleConnectAbilityMgr::GetCallingInfo(InnerBundleInfo &innerBundleInfo,
    std::vector<std::string> &bundleNames, std::vector<std::string> &callingAppIds)
{
    bundleNames.emplace_back(innerBundleInfo.GetBundleName());
    callingAppIds.emplace_back(innerBundleInfo.GetBaseBundleInfo().appId);
}

bool ExistBundleNameInCallingBundles(std::string &bundleName, std::vector<std::string> &callingBundleNames)
{
    for (auto bundleNameItem : callingBundleNames) {
        if (bundleNameItem == bundleName) {
            return true;
        }
    }
    return false;
}

void BundleConnectAbilityMgr::GetTargetAbilityInfo(const Want &want, InnerBundleInfo &innerBundleInfo,
    sptr<TargetAbilityInfo> &targetAbilityInfo, sptr<TargetInfo> &targetInfo)
{
    ElementName element = want.GetElement();
    std::string bundleName = element.GetBundleName();
    std::string moduleName = element.GetModuleName();
    std::string abilityName = element.GetAbilityName();
    std::string deviceId = element.GetDeviceID();
    std::vector<std::string> callingBundleNames;
    std::vector<std::string> callingAppids;
    targetAbilityInfo->targetExtSetting = want.GetStringParam("targetExtSetting");
    targetInfo->transactId = std::to_string(this->GetTransactId());
    targetInfo->bundleName = bundleName;
    targetInfo->moduleName = moduleName;
    targetInfo->abilityName = abilityName;
    // make int from bits.
    targetInfo->flags = BIT_ONE_COMPATIBLE + BIT_TWO_BACK_MODE * BIT_TWO + BIT_THREE_CUSTOM * BIT_THREE +
                        deviceId.empty() * BIT_FOUR + BIT_ONE_FIVE_AZ_DEVICE * BIT_FIVE +
                        !ExistBundleNameInCallingBundles(bundleName, callingBundleNames) * BIT_SIX +
                        BIT_ONE_SEVEN_SAME_BUNDLE * BIT_SEVEN;
    targetInfo->callingUid = IPCSkeleton::GetCallingUid();
    targetInfo->callingAppType = CALLING_TYPE_HARMONY;
    targetAbilityInfo->targetInfo = *targetInfo;
    this->GetCallingInfo(innerBundleInfo, callingBundleNames, callingAppids);
    targetAbilityInfo->version = DEFAULT_VERSION;
    targetInfo->callingBundleNames = callingBundleNames;
    targetInfo->callingAppIds = callingAppids;
}

void BundleConnectAbilityMgr::CallAbilityManager(
    int32_t resultCode, const Want &want, int32_t userId, const sptr<IRemoteObject> &callBack)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(ATOMIC_SERVICE_STATUS_CALLBACK_TOKEN)) {
        APP_LOGE("Write interface token failed");
        return;
    }
    if (!data.WriteInt32(resultCode)) {
        APP_LOGE("Write result code failed");
        return;
    }
    if (!data.WriteParcelable(&want)) {
        APP_LOGE("Write want failed");
        return;
    }
    if (!data.WriteInt32(userId)) {
        APP_LOGE("Write userId failed");
        return;
    }

    if (callBack->SendRequest(FREE_INSTALL_DONE, data, reply, option) != ERR_OK) {
        APP_LOGE("BundleConnectAbilityMgr::CallAbilityManager SendRequest failed");
    }
}

bool BundleConnectAbilityMgr::CheckIsModuleNeedUpdate(
    InnerBundleInfo &innerBundleInfo, const Want &want, int32_t userId, const sptr<IRemoteObject> &callBack)
{
    if (innerBundleInfo.GetModuleUpgradeFlag(want.GetModuleName()) != 0) {
        sptr<TargetAbilityInfo> targetAbilityInfo = new(std::nothrow) TargetAbilityInfo();
        if (targetAbilityInfo == nullptr) {
            APP_LOGE("targetAbilityInfo is nullptr");
            return false;
        }
        sptr<TargetInfo> targetInfo = new(std::nothrow) TargetInfo();
        if (targetInfo == nullptr) {
            APP_LOGE("targetInfo is nullptr");
            return false;
        }
        GetTargetAbilityInfo(want, innerBundleInfo, targetAbilityInfo, targetInfo);
        sptr<FreeInstallParams> freeInstallParams = new(std::nothrow) FreeInstallParams();
        if (freeInstallParams == nullptr) {
            APP_LOGE("freeInstallParams is nullptr");
            return false;
        }
        freeInstallParams->callback = callBack;
        freeInstallParams->want = want;
        freeInstallParams->userId = userId;
        freeInstallParams->serviceCenterFunction = ServiceCenterFunction::CONNECT_UPGRADE_INSTALL;
        auto ret = freeInstallParamsMap_.emplace(targetInfo->transactId, *freeInstallParams);
        if (!ret.second) {
            APP_LOGE("BundleConnectAbilityMgr::QueryAbilityInfo map emplace error");
            CallAbilityManager(FreeInstallErrorCode::UNDEFINED_ERROR, want, userId, callBack);
        }
        this->UpgradeInstall(*targetAbilityInfo, want, nullptr, userId);
        return true;
    }
    APP_LOGI("Module is not need update");
    return false;
}

bool BundleConnectAbilityMgr::IsObtainAbilityInfo(const Want &want, int32_t flags, int32_t userId,
    AbilityInfo &abilityInfo, const sptr<IRemoteObject> &callBack, InnerBundleInfo &innerBundleInfo)
{
    APP_LOGI("IsObtainAbilityInfo");
    std::string bundleName = want.GetElement().GetBundleName();
    std::string abilityName = want.GetElement().GetAbilityName();
    if (bundleName == "" || abilityName == "") {
        CallAbilityManager(FreeInstallErrorCode::UNDEFINED_ERROR, want, userId, callBack);
        APP_LOGE("bundle name or ability name is null");
        return false;
    }
    std::shared_ptr<BundleMgrService> bms = DelayedSingleton<BundleMgrService>::GetInstance();
    std::shared_ptr<BundleDataMgr> bundleDataMgr_ = bms->GetDataMgr();
    bool innerBundleInfoResult = bundleDataMgr_->GetInnerBundleInfoWithFlags(bundleName,
        flags, innerBundleInfo, userId);
    bool abilityInfoResult = bundleDataMgr_->QueryAbilityInfo(want, flags, userId, abilityInfo);
    if (innerBundleInfoResult && abilityInfoResult) {
        bool isModuleNeedUpdate = CheckIsModuleNeedUpdate(innerBundleInfo, want, userId, callBack);
        if (!isModuleNeedUpdate) {
            CallAbilityManager(ServiceCenterResultCode::FREE_INSTALL_OK, want, userId, callBack);
        }
        return true;
    }
    return false;
}

bool BundleConnectAbilityMgr::QueryAbilityInfo(const Want &want, int32_t flags,
    int32_t userId, AbilityInfo &abilityInfo, const sptr<IRemoteObject> &callBack)
{
    APP_LOGI("QueryAbilityInfo");
    InnerBundleInfo innerBundleInfo;
    if (IsObtainAbilityInfo(want, flags, userId, abilityInfo, callBack, innerBundleInfo)) {
        return true;
    }
    sptr<FreeInstallParams> freeInstallParams = new(std::nothrow) FreeInstallParams();
    if (freeInstallParams == nullptr) {
        APP_LOGD("freeInstallParams is nullptr");
        return false;
    }
    freeInstallParams->callback = callBack;
    freeInstallParams->want = want;
    freeInstallParams->userId = userId;
    sptr<TargetAbilityInfo> targetAbilityInfo = new(std::nothrow) TargetAbilityInfo();
    if (targetAbilityInfo == nullptr) {
        APP_LOGD("targetAbilityInfo is nullptr");
        return false;
    }
    sptr<TargetInfo> targetInfo = new(std::nothrow) TargetInfo();
    if (targetInfo == nullptr) {
        APP_LOGD("targetInfo is nullptr");
        return false;
    }
    GetTargetAbilityInfo(want, innerBundleInfo, targetAbilityInfo, targetInfo);
    freeInstallParams->serviceCenterFunction = ServiceCenterFunction::CONNECT_SILENT_INSTALL;
    auto ret = freeInstallParamsMap_.emplace(targetInfo->transactId, *freeInstallParams);
    if (!ret.second) {
        APP_LOGE("BundleConnectAbilityMgr::QueryAbilityInfo map emplace error");
        CallAbilityManager(FreeInstallErrorCode::UNDEFINED_ERROR, want, userId, callBack);
        return false;
    }
    this->SilentInstall(*targetAbilityInfo, want, nullptr, userId);
    return false;
}

void BundleConnectAbilityMgr::UpgradeAtomicService(const Want &want, int32_t userId)
{
    APP_LOGI("UpgradeAtomicService");
    std::shared_ptr<BundleMgrService> bms = DelayedSingleton<BundleMgrService>::GetInstance();
    std::shared_ptr<BundleDataMgr> bundleDataMgr_ = bms->GetDataMgr();
    ElementName element = want.GetElement();
    std::string bundleName = element.GetBundleName();
    InnerBundleInfo innerBundleInfo;
    sptr<TargetAbilityInfo> targetAbilityInfo = new(std::nothrow) TargetAbilityInfo();
    if (targetAbilityInfo == nullptr) {
        APP_LOGD("targetAbilityInfo is nullptr");
        return;
    }
    sptr<TargetInfo> targetInfo = new(std::nothrow) TargetInfo();
    if (targetInfo == nullptr) {
        APP_LOGD("targetInfo is nullptr");
        return;
    }
    sptr<FreeInstallParams> freeInstallParams = new(std::nothrow) FreeInstallParams();
    if (freeInstallParams == nullptr) {
        APP_LOGD("freeInstallParams is nullptr");
        return;
    }
    bundleDataMgr_->GetInnerBundleInfoWithFlags(bundleName, want.GetFlags(), innerBundleInfo, userId);
    GetTargetAbilityInfo(want, innerBundleInfo, targetAbilityInfo, targetInfo);
    freeInstallParams->want = want;
    freeInstallParams->userId = userId;
    freeInstallParams->serviceCenterFunction = ServiceCenterFunction::CONNECT_UPGRADE_CHECK;
    auto ret = freeInstallParamsMap_.emplace(targetInfo->transactId, *freeInstallParams);
    if (!ret.second) {
        APP_LOGE("BundleConnectAbilityMgr::QueryAbilityInfo map emplace error");
        return;
    }
    this->UpgradeCheck(*targetAbilityInfo, want, nullptr, userId);
}
}  // namespace AppExecFwk
}  // namespace OHOS