/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "classic_adapter.h"

#include <algorithm>
#include <sys/time.h>
#include "adapter_manager.h"
#include "base_observer_list.h"
#include "btstack.h"
#include "class_creator.h"
#include "classic_data_structure.h"
#include "classic_data_type_defs.h"
#include "classic_defs.h"
#include "classic_utils.h"
#include "compat.h"
#include "interface_profile_manager.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
struct ClassicAdapter::impl {
    std::recursive_mutex syncMutex_ {};
    BaseObserverList<IAdapterClassicObserver> adapterObservers_ {};
    BaseObserverList<IClassicRemoteDeviceObserver> remoteObservers_ {};
};

ClassicAdapter::ClassicAdapter()
    : utility::Context(ADAPTER_NAME_CLASSIC, "5.0"),
      adapterProperties_(ClassicAdapterProperties::GetInstance()),
      batteryObserverHf_(std::make_unique<ClassicBatteryObserverHf>(*GetDispatcher())),
      batteryObserverAg_(std::make_unique<ClassicBatteryObserverAg>(*GetDispatcher())),
      pimpl(std::make_unique<ClassicAdapter::impl>())
{
    HILOGI("%{public}s Constructor", Name().c_str());
    discoveryState_ = DISCOVERY_STOPED;
    scanMode_ = SCAN_MODE_NONE;
}

ClassicAdapter::~ClassicAdapter()
{
    HILOGI("%{public}s Destructor", Name().c_str());
}

void ClassicAdapter::Enable()
{
    HILOGI("%{public}s Enable", Name().c_str());

    GetDispatcher()->PostTask(std::bind(&ClassicAdapter::StartUp, this));
}

void ClassicAdapter::StartUp()
{
    HILOGI("enter");

    isDisable_ = false;
    bool result = adapterProperties_.LoadConfigInfo();
    ClassicUtils::CheckReturnValue("ClassicAdapter", "LoadConfigInfo", result);

    btmEnableSuccess_ = (BTM_Enable(BREDR_CONTROLLER) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "BTM_Enable", btmEnableSuccess_);
    if (!btmEnableSuccess_) {
        GetContext()->OnEnable(ADAPTER_NAME_CLASSIC, false);
        return;
    }

    eirData_.SetDataMaxLength(MAX_EXTEND_INQUIRY_RESPONSE_LEN);
    timer_ = std::make_unique<utility::Timer>(std::bind(&ClassicAdapter::ScanModeTimeout, this));
    hwTimer_ = std::make_unique<utility::Timer>(std::bind(&ClassicAdapter::HwProcessTimeout, this));

    bool ret = RegisterCallback();
    ClassicUtils::CheckReturnValue("ClassicAdapter", "RegisterCallback", ret);

    ret &= adapterProperties_.ConfigProperties();
    ClassicUtils::CheckReturnValue("ClassicAdapter", "ConfigProperties", ret);

    ret &= adapterProperties_.SetSecurityMode();
    ClassicUtils::CheckReturnValue("ClassicAdapter", "SetSecurityMode", ret);

    LoadPairedDeviceInfo();

    GetContext()->OnEnable(ADAPTER_NAME_CLASSIC, ret);
}

void ClassicAdapter::Disable()
{
    HILOGI("%{public}s Disable", Name().c_str());

    GetDispatcher()->PostTask(std::bind(&ClassicAdapter::ShutDown, this));
}

void ClassicAdapter::ShutDown()
{
    HILOGI("enter");

    if (!btmEnableSuccess_) {
        GetContext()->OnDisable(ADAPTER_NAME_CLASSIC, true);
        return;
    }

    isDisable_ = true;
    SavePairedDevices();

    if (hfService_ != nullptr) {
        hfService_->DeregisterObserver(*(HfpHfServiceObserver *)batteryObserverHf_.get());
    }
    if (agService_ != nullptr) {
        agService_->DeregisterObserver(*(HfpAgServiceObserver *)batteryObserverAg_.get());
    }

    if (IsBtDiscovering()) {
        CancelBtDiscovery();
        return;
    }

    DisablePairProcess();
}

void ClassicAdapter::InitMode()
{
    if (waitPairResult_) {
        return;
    }

    bool ret = adapterProperties_.InitMode();
    ClassicUtils::CheckReturnValue("ClassicAdapter", "InitMode", ret);

    if (scanMode_ != SCAN_MODE_NONE) {
        ret = SetScanMode(SCAN_MODE_NONE);
        ClassicUtils::CheckReturnValue("ClassicAdapter", "SetScanMode", ret);
        if (ret) {
            return;
        }
    }

    DisableBTM();
}

void ClassicAdapter::DisablePairProcess()
{
    HILOGI("enter");
    for (auto &device : devices_) {
        if (device.second->GetPairedStatus() == PAIR_PAIRING) {
            bool result =
                (BTM_AclDisconnect(device.second->GetConnectionHandle(), BTM_ACL_DISCONNECT_REASON) == BT_NO_ERROR);
            ClassicUtils::CheckReturnValue("ClassicAdapter", "BTM_AclDisconnect", result);
            device.second->SetPairedStatus(PAIR_NONE);
            if (device.second->IsBondedFromLocal() && (!pinMode_)) {
                waitPairResult_ = true;
            }
        }
    }
    InitMode();
}

void ClassicAdapter::FreeMemory()
{
    devices_.clear();

    hfService_ = nullptr;
    agService_ = nullptr;

    if (hwTimer_ != nullptr) {
        hwTimer_->Stop();
    }

    if (timer_ != nullptr) {
        timer_->Stop();
    }
}

void ClassicAdapter::DisableBTM()
{
    /// Unregister Callback.
    bool ret = DeregisterCallback();
    ClassicUtils::CheckReturnValue("ClassicAdapter", "DeregisterCallback", ret);

    /// Disable BTM
    ret &= (BTM_Disable(BREDR_CONTROLLER) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "BTM_Disable", ret);

    FreeMemory();
    btmEnableSuccess_ = false;
    GetContext()->OnDisable(ADAPTER_NAME_CLASSIC, ret);
}

void ClassicAdapter::PostEnable()
{
    HILOGI("%{public}s PostEnable", Name().c_str());

    GetDispatcher()->PostTask(std::bind(&ClassicAdapter::ProcessPostEnable, this));
}

void ClassicAdapter::ProcessPostEnable()
{
    HILOGI("enter");

    UpdateSupportedUuids();

    hfService_ = (IProfileHfpHf *)IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_HFP_HF);
    if (hfService_ != nullptr) {
        hfService_->RegisterObserver(*(HfpHfServiceObserver *)batteryObserverHf_.get());
    }

    agService_ = (IProfileHfpAg *)IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_HFP_AG);
    if (agService_ != nullptr) {
        agService_->RegisterObserver(*(HfpAgServiceObserver *)batteryObserverAg_.get());
    }
}

void ClassicAdapter::UpdateSupportedUuids() const
{
    HILOGI("enter");

    /// Get Supported UUID for AdapterManager.
    std::vector<std::string> stringUuids;
    IProfileManager::GetInstance()->GetProfileServicesSupportedUuids(stringUuids);

    /// convert String to Uuid.
    std::vector<Uuid> uuids;
    for (auto &value : stringUuids) {
        Uuid uuid = Uuid::ConvertFromString(value);
        uuids.push_back(uuid);
    }

    bool ret = adapterProperties_.SaveSupportUuids(uuids);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "SaveSupportUuids", ret);
}

void ClassicAdapter::LoadPairedDeviceInfo()
{
    std::vector<std::string> pairedAddrList = adapterProperties_.GetPairedAddrList();
    for (auto &pairedAddr : pairedAddrList) {
        std::string addr = pairedAddr;
        if (addr.empty() || addr == INVALID_MAC_ADDRESS) {
            continue;
        } else {
            std::shared_ptr<ClassicRemoteDevice> remote = adapterProperties_.GetPairedDevice(addr);
            if (remote != nullptr) {
                devices_.insert(std::make_pair(addr, remote));
            }
        }
    }
}

void ClassicAdapter::SavePairedDevices() const
{
    for (auto &device : devices_) {
        if (device.second->IsPaired()) {
            adapterProperties_.SavePairedDeviceInfo(device.second);
        }
    }
    adapterProperties_.SaveConfigFile();
}

bool ClassicAdapter::RegisterCallback()
{
    /// Register GAP discovery series callback.
    GapDiscoveryCallback discoveryCbs {};
    discoveryCbs.inquiryResult = InquiryResultCallback;
    discoveryCbs.inquiryResultRssi = InquiryResultRssiCallback;
    discoveryCbs.extendedInquiryResult = ExtendedInquiryResultCallback;
    discoveryCbs.remoteName = RemoteNameCallback;
    discoveryCbs.inquiryComplete = InquiryCompleteCallback;
    bool ret = (GAPIF_RegisterDiscoveryCallback(&discoveryCbs, this) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_RegisterDiscoveryCallback", ret);

    /// Register GAP Pair series callback.
    GapAuthenticationCallback authenticationCbs {};
    authenticationCbs.authenticationComplete = AuthenticationCompleteCallback;
    authenticationCbs.encryptionChangeCallback = EncryptionChangeCallback;
    authenticationCbs.IOCapabilityReq = IoCapabilityReqCallback;
    authenticationCbs.IOCapabilityRsp = IoCapabilityRspCallback;
    authenticationCbs.linkKeyNotification = LinkKeyNotification;
    authenticationCbs.linkKeyReq = LinkKeyReqCallback;
    authenticationCbs.pinCodeReq = PinCodeReqCallback;
    authenticationCbs.remoteOobReq = RemoteOobReqCallback;
    authenticationCbs.simplePairComplete = SimplePairCompleteCallback;
    authenticationCbs.userConfirmReq = UserConfirmReqCallback;
    authenticationCbs.userPasskeyReq = UserPasskeyReqCallback;
    authenticationCbs.userPasskeyNotification = UserPasskeyNotificationCallback;
    ret &= (GAPIF_RegisterAuthenticationCallback(&authenticationCbs, this) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_RegisterAuthenticationCallback", ret);

    /// Register GAP security callback.
    GapSecurityCallback securityCb {};
    securityCb.authorizeInd = AuthorizeIndCallback;
    ret &= (GAPIF_RegisterSecurityCallback(&securityCb, this) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_RegisterSecurityCallback", ret);

    /// Register BTM ACL Change status callback.
    btmAclCbs_.connectionComplete = ConnectionComplete;
    btmAclCbs_.disconnectionComplete = DisconnectionComplete;
    btmAclCbs_.leConnectionComplete = nullptr;
    btmAclCbs_.leDisconnectionComplete = nullptr;
    btmAclCbs_.readRssiComplete = nullptr;
    ret &= (BTM_RegisterAclCallbacks(&btmAclCbs_, this) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "BTM_RegisterAclCallbacks", ret);

    return ret;
}

bool ClassicAdapter::DeregisterCallback() const
{
    /// Deregister GAP discovery callback.
    bool ret = (GAPIF_DeregisterDiscoveryCallback() == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_DeregisterDiscoveryCallback", ret);

    /// Deregister GAP Pair series callback.
    ret &= (GAPIF_DeregisterAuthenticationCallback() == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_DeregisterAuthenticationCallback", ret);

    /// Deregister GAP security callback.
    ret &= (GAPIF_DeregisterSecurityCallback() == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_DeregisterSecurityCallback", ret);

    /// Deregister BTM ACL Change status callback.
    ret &= (BTM_DeregisterAclCallbacks(&btmAclCbs_) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "BTM_DeregisterAclCallbacks", ret);

    return ret;
}

std::string ClassicAdapter::GetLocalAddress() const
{
    return adapterProperties_.GetLocalAddress();
}

std::string ClassicAdapter::GetLocalName() const
{
    return adapterProperties_.GetLocalName();
}

bool ClassicAdapter::SetLocalName(const std::string &name) const
{
    HILOGI("name: %{public}s", name.c_str());

    if (name.empty()) {
        HILOGE("failed, because of name is NULL!");
        return false;
    }

    std::string localName = adapterProperties_.GetLocalName();
    if (localName == name) {
        HILOGW("same name!");
        return true;
    }

    return adapterProperties_.SetLocalName(name);
}

int ClassicAdapter::GetLocalDeviceClass() const
{
    return adapterProperties_.GetLocalDeviceClass();
}

bool ClassicAdapter::SetLocalDeviceClass(int deviceClass) const
{
    return adapterProperties_.SetLocalDeviceClass(deviceClass);
}

int ClassicAdapter::GetBtScanMode() const
{
    return scanMode_;
}

bool ClassicAdapter::SetBtScanMode(int mode, int duration)
{
    HILOGI("mode: %{public}d, duration: %{public}d", mode, duration);

    if (mode < SCAN_MODE_NONE || mode > SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE) {
        HILOGE("failed. Invalid Parameter[mode]");
        return false;
    }

    if (INVALID_VALUE > duration) {
        HILOGE("failed. Invalid parameter[duration].");
        return false;
    }

    bool ret = adapterProperties_.SetDiscoverableTimeout(duration);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "SetDiscoverableTimeout", ret);

    GetDispatcher()->PostTask(std::bind(&ClassicAdapter::SetScanMode, this, mode));

    return ret;
}

bool ClassicAdapter::SetScanMode(int mode)
{
    scanMode_ = mode;
    GapDiscoverModeInfo discoverMode;
    GapConnectableModeInfo connectableMode;
    GapSetScanModeResultCallback cb = SetScanModeResultCallback;
    switch (mode) {
        case SCAN_MODE_NONE:
            discoverMode.mode = GAP_DISCOVERABLE_MODE_NON;
            connectableMode.mode = GAP_CONNECTABLE_MODE_NON;
            break;
        case SCAN_MODE_CONNECTABLE:
            discoverMode.mode = GAP_DISCOVERABLE_MODE_NON;
            connectableMode.mode = GAP_CONNECTABLE_MODE;
            break;
        case SCAN_MODE_GENERAL_DISCOVERABLE:
            discoverMode.mode = GAP_DISCOVERABLE_MODE_GENERAL;
            connectableMode.mode = GAP_CONNECTABLE_MODE_NON;
            break;
        case SCAN_MODE_LIMITED_DISCOVERABLE:
            discoverMode.mode = GAP_DISCOVERABLE_MODE_LIMITED;
            connectableMode.mode = GAP_CONNECTABLE_MODE_NON;
            break;
        case SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE:
            discoverMode.mode = GAP_DISCOVERABLE_MODE_GENERAL;
            connectableMode.mode = GAP_CONNECTABLE_MODE;
            break;
        case SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE:
            discoverMode.mode = GAP_DISCOVERABLE_MODE_LIMITED;
            connectableMode.mode = GAP_CONNECTABLE_MODE;
            break;
        default:
            discoverMode.mode = GAP_DISCOVERABLE_MODE_NON;
            connectableMode.mode = GAP_CONNECTABLE_MODE_NON;
            break;
    }
    discoverMode.type = GAP_INQUIRY_SCAN_TYPE_GENERAL;
    connectableMode.type = GAP_PAGE_SCAN_TYPE_GENERAL;
    discoverMode.scanInterval = GAP_INQUIRY_SCAN_INTERVAL_DEFAULT;
    discoverMode.scanWindow = GAP_INQUIRY_SCAN_WINDOW_DEFAULT;
    connectableMode.scanInterval = GAP_PAGE_SCAN_INTERVAL_DEFAULT;
    connectableMode.scanWindow = GAP_PAGE_SCAN_WINDOW_DEFAULT;
    bool ret = (GAPIF_SetScanMode(&discoverMode, &connectableMode, cb, this) == BT_NO_ERROR);

    if (timer_ != nullptr) {
        timer_->Stop();
    }

    return ret;
}

void ClassicAdapter::SetScanModeResultCallback(uint8_t status, void *context)
{
    HILOGI("status: %{public}u", status);

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(std::bind(&ClassicAdapter::ReceiveSetScanModeCallback, adapter, status));
    }
}

void ClassicAdapter::ReceiveSetScanModeCallback(uint8_t status)
{
    HILOGI("status: %{public}u", status);

    if (status != NOERROR) {
        HILOGE("failed, status is %{public}u", status);
        return;
    }

    SendScanModeChanged(scanMode_);

    if ((scanMode_ != SCAN_MODE_NONE) && (scanMode_ != SCAN_MODE_CONNECTABLE) &&
        (adapterProperties_.GetDiscoverableTimeout() != 0) && (timer_ != nullptr)) {
        timer_->Start(adapterProperties_.GetDiscoverableTimeout());
    }

    if (isDisable_) {
        DisableBTM();
    }
}

void ClassicAdapter::SendScanModeChanged(int mode) const
{
    HILOGI("mode: %{public}d", mode);

    pimpl->adapterObservers_.ForEach([mode](IAdapterClassicObserver &observer) { observer.OnScanModeChanged(mode); });
}

void ClassicAdapter::ScanModeTimeout()
{
    HILOGI("enter");

    if (GetDispatcher() != nullptr) {
        GetDispatcher()->PostTask(std::bind(&ClassicAdapter::ResetScanMode, this));
    }
}

void ClassicAdapter::ResetScanMode()
{
    int mode = SCAN_MODE_NONE;
    switch (scanMode_) {
        case SCAN_MODE_GENERAL_DISCOVERABLE:
        case SCAN_MODE_LIMITED_DISCOVERABLE:
            mode = SCAN_MODE_NONE;
            break;
        case SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE:
        case SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE:
            mode = SCAN_MODE_CONNECTABLE;
            break;
        default:
            break;
    }
    SetScanMode(mode);
}

void ClassicAdapter::HwProcessTimeout()
{
    HILOGI("enter");

    if (GetDispatcher() != nullptr) {
        GetDispatcher()->PostTask(std::bind(&ClassicAdapter::HwTimeout, this));
    }
}

void ClassicAdapter::HwTimeout()
{
    HILOGI("enter");
    bool ret = CancelGetRemoteName();
    ClassicUtils::CheckReturnValue("ClassicAdapter", "CancelGetRemoteName", ret);
}

int ClassicAdapter::GetBondableMode() const
{
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    return adapterProperties_.GetBondableMode();
}

bool ClassicAdapter::SetBondableMode(int mode) const
{
    HILOGI("mode: %{public}d", mode);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    for (auto &device : devices_) {
        if (device.second->GetPairedStatus() == PAIR_PAIRING) {
            HILOGW("failed, because of PAIR_PAIRING.");
            return false;
        }
    }

    return adapterProperties_.SetBondableMode(mode);
}

bool ClassicAdapter::StartBtDiscovery()
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (discoveryState_ == DISCOVERYING || discoveryState_ == DISCOVERY_STARTED) {
        HILOGE("failed, because of DISCOVERYING or DISCOVERY_STARTED!");
        return false;
    }

    int ret = GAPIF_Inquiry(GAP_INQUIRY_MODE_GENERAL, DEFAULT_INQ_MAX_DURATION);
    if (ret == BT_NO_ERROR) {
        discoveryState_ = DISCOVERY_STARTED;
        struct timeval tv {};
        gettimeofday(&tv, nullptr);
        long currentTime = (tv.tv_sec * MILLISECOND_UNIT + tv.tv_usec / MILLISECOND_UNIT);
        discoveryEndMs_ = currentTime + DEFAULT_DISCOVERY_TIMEOUT_MS;
        SendDiscoveryStateChanged(discoveryState_);
    } else {
        HILOGE("failed, because of GAPIF_Inquiry failed!");
        return false;
    }

    return true;
}

bool ClassicAdapter::CancelBtDiscovery()
{
    HILOGI("enter");

    bool ret = false;
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (discoveryState_ == DISCOVERY_STOPED) {
        HILOGE("failed, because of DISCOVERY_STOPED!");
        return ret;
    }
    cancelDiscovery_ = true;

    if (!receiveInquiryComplete_) {
        ret = (GAPIF_InquiryCancel() == BT_NO_ERROR);
        ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_InquiryCancel", ret);
    } else {
        discoveryState_ = DISCOVERY_STOPED;
        ret = CancelGetRemoteName();
        ClassicUtils::CheckReturnValue("ClassicAdapter", "CancelGetRemoteName", ret);
    }

    return ret;
}

bool ClassicAdapter::IsBtDiscovering() const
{
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    HILOGI("discoveryState %{public}d", discoveryState_);

    return (DISCOVERY_STOPED > discoveryState_);
}

long ClassicAdapter::GetBtDiscoveryEndMillis() const
{
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    return discoveryEndMs_;
}

void ClassicAdapter::InquiryResultCallback(const BtAddr *addr, uint32_t classOfDevice, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.iniquiryResultParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.iniquiryResultParam_.classOfDevice = classOfDevice;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleInquiryEvent, adapter, GAP_INQUIRY_RESULT_EVT, param));
    }
}

void ClassicAdapter::InquiryResultRssiCallback(const BtAddr *addr, uint32_t classOfDevice, int8_t rssi, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.iniquiryResultParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.iniquiryResultRssiParam_.classOfDevice = classOfDevice;
    param.iniquiryResultRssiParam_.rssi = rssi;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleInquiryEvent, adapter, GAP_INQUIRY_RESULT_RSSI_EVT, param));
    }
}

void ClassicAdapter::ExtendedInquiryResultCallback(const BtAddr *addr, uint32_t classOfDevice, int8_t rssi,
    const uint8_t eir[MAX_EXTEND_INQUIRY_RESPONSE_LEN], void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s(static_cast<void *>(const_cast<BtAddr *>(&param.iniquiryResultParam_.addr)),
        sizeof(BtAddr), addr, sizeof(BtAddr));
    param.extendedInquiryResultParam_.classOfDevice = classOfDevice;
    param.extendedInquiryResultParam_.rssi = rssi;
    (void)memcpy_s(static_cast<void *>(&param.extendedInquiryResultParam_.eir),
        MAX_EXTEND_INQUIRY_RESPONSE_LEN,
        eir,
        MAX_EXTEND_INQUIRY_RESPONSE_LEN);

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleInquiryEvent, adapter, GAP_EXTENDED_INQUIRY_RESULT_EVT, param));
    }
}

void ClassicAdapter::InquiryCompleteCallback(uint8_t status, void *context)
{
    HILOGI("status: %{public}u", status);

    GapCallbackParam param = {};
    param.inquiryCompleteParam_.status = status;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleInquiryEvent, adapter, GAP_INQUIRY_COMPLETE_EVT, param));
    }
}

void ClassicAdapter::RemoteNameCallback(
    uint8_t status, const BtAddr *addr, const uint8_t name[MAX_LOC_BT_NAME_LEN], void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    param.remoteNameCallbackParam_.status = status;
    (void)memcpy_s(static_cast<void *>(const_cast<BtAddr *>(&param.remoteNameCallbackParam_.addr)),
        sizeof(BtAddr), addr, sizeof(BtAddr));
    (void)memcpy_s(static_cast<void *>(const_cast<uint8_t *>(param.remoteNameCallbackParam_.name)),
        MAX_LOC_BT_NAME_LEN, name, MAX_LOC_BT_NAME_LEN);

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleInquiryEvent, adapter, GAP_REMOTE_NAME_CALLBACK_EVT, param));
    }
}

void ClassicAdapter::UserConfirmReqCallback(const BtAddr *addr, uint32_t number, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.userConfirmReqParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.userConfirmReqParam_.number = number;
    param.userConfirmReqParam_.reqType = PAIR_CONFIRM_TYPE_NUMERIC;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_SSP_CONFIRM_REQ_EVT, param));
    }
}

void ClassicAdapter::UserPasskeyReqCallback(const BtAddr *addr, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.userConfirmReqParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.userConfirmReqParam_.reqType = PAIR_CONFIRM_TYPE_PASSKEY_INPUT;
    param.userConfirmReqParam_.number = 0;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_SSP_CONFIRM_REQ_EVT, param));
    }
}

void ClassicAdapter::UserPasskeyNotificationCallback(const BtAddr *addr, uint32_t number, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.userConfirmReqParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.userConfirmReqParam_.reqType = PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY;
    param.userConfirmReqParam_.number = number;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_SSP_CONFIRM_REQ_EVT, param));
    }
}

void ClassicAdapter::RemoteOobReqCallback(const BtAddr *addr, void *context)
{
    HILOGI("enter");

    /// OOB Pair not support for current framework api.
    /// This is used for extending.
    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.remoteOobReqParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_REMOTE_OOB_REQ_EVT, param));
    }
}

void ClassicAdapter::PinCodeReqCallback(const BtAddr *addr, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.pinCodeReqParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_PIN_CODE_REQ_EVT, param));
    }
}

void ClassicAdapter::LinkKeyReqCallback(const BtAddr *addr, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.linkKeyReqParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_LINK_KEY_REQ_EVT, param));
    }
}

void ClassicAdapter::IoCapabilityReqCallback(const BtAddr *addr, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.ioCapabilityReqParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_IO_CAPABILITY_REQ_EVT, param));
    }
}

void ClassicAdapter::IoCapabilityRspCallback(const BtAddr *addr, uint8_t ioCapability, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.ioCapabilityRspParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.ioCapabilityRspParam_.ioCapability = ioCapability;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_IO_CAPABILITY_RSP_EVT, param));
    }
}

void ClassicAdapter::LinkKeyNotification(
    const BtAddr *addr, const uint8_t linkKey[GAP_LINKKEY_SIZE], uint8_t keyType, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.linkKeyNotificationParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.linkKeyNotificationParam_.keyType = keyType;
    (void)memcpy_s((void *)param.linkKeyNotificationParam_.linkKey, GAP_LINKKEY_SIZE, linkKey, GAP_LINKKEY_SIZE);

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_LINK_KEY_NOTIFICATION_EVT, param));
    }
}

void ClassicAdapter::SimplePairCompleteCallback(const BtAddr *addr, uint8_t status, void *context)
{
    HILOGI("status = %{public}u", status);

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.simplePairCompleteParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.simplePairCompleteParam_.status = status;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_SIMPLE_PAIR_COMPLETE_EVT, param));
    }
}

void ClassicAdapter::AuthenticationCompleteCallback(const BtAddr *addr, uint8_t status, void *context)
{
    HILOGI("status: %{public}u", status);

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.authenticationCompleteParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.authenticationCompleteParam_.status = status;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_AUTHENTICATION_COMPLETE_EVT, param));
    }
}

void ClassicAdapter::EncryptionChangeCallback(const BtAddr *addr, uint8_t status, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.encryptionChangeCallbackParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.encryptionChangeCallbackParam_.status = status;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_ENCRYPTION_CHANGE_CALLBACK_EVT, param));
    }
}

void ClassicAdapter::AuthorizeIndCallback(const BtAddr *addr, GAP_Service service, void *context)
{
    HILOGI("enter");

    GapCallbackParam param = {};
    (void)memcpy_s((void *)&param.authorizeIndParam_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param.authorizeIndParam_.service = service;

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::HandleSecurityEvent, adapter, GAP_AUTHORIZE_IND_CALLBACK_EVT, param));
    }
}

void ClassicAdapter::HandleInquiryEvent(GAP_CB_EVENT event, const GapCallbackParam &param)
{
    std::vector<uint8_t> eirData;
    switch (event) {
        case GAP_INQUIRY_RESULT_EVT:
            HandleInquiryResult(param.iniquiryResultParam_.addr, param.iniquiryResultParam_.classOfDevice, eirData);
            break;
        case GAP_INQUIRY_RESULT_RSSI_EVT:
            HandleInquiryResult(param.iniquiryResultRssiParam_.addr,
                param.iniquiryResultRssiParam_.classOfDevice,
                eirData,
                param.iniquiryResultRssiParam_.rssi);
            break;
        case GAP_EXTENDED_INQUIRY_RESULT_EVT:
            eirData.resize(MAX_EXTEND_INQUIRY_RESPONSE_LEN);
            (void)memcpy_s(&eirData[0],
                MAX_EXTEND_INQUIRY_RESPONSE_LEN,
                param.extendedInquiryResultParam_.eir,
                MAX_EXTEND_INQUIRY_RESPONSE_LEN);
            HandleInquiryResult(param.extendedInquiryResultParam_.addr,
                param.extendedInquiryResultParam_.classOfDevice,
                eirData,
                param.extendedInquiryResultParam_.rssi);
            break;
        case GAP_INQUIRY_COMPLETE_EVT:
            HandleInquiryComplete(param.inquiryCompleteParam_.status);
            break;
        case GAP_REMOTE_NAME_CALLBACK_EVT:
            ReceiveRemoteName(param.remoteNameCallbackParam_.status,
                param.remoteNameCallbackParam_.addr,
                param.remoteNameCallbackParam_.name);
            break;
        default:
            break;
    }
}

void ClassicAdapter::HandleSecurityEvent(GAP_CB_EVENT event, const GapCallbackParam &param)
{
    switch (event) {
        case GAP_SSP_CONFIRM_REQ_EVT:
            SSPConfirmReq(
                param.userConfirmReqParam_.addr, param.userConfirmReqParam_.reqType, param.userConfirmReqParam_.number);
            break;
        case GAP_PIN_CODE_REQ_EVT:
            PinCodeReq(param.pinCodeReqParam_.addr);
            break;
        case GAP_REMOTE_OOB_REQ_EVT:
            /// OOB Pair not support for current framework api.
            /// This is used for extending.
            break;
        case GAP_IO_CAPABILITY_REQ_EVT:
            SetIoCapability(param.ioCapabilityReqParam_.addr);
            break;
        case GAP_IO_CAPABILITY_RSP_EVT:
            SaveRemoteIoCapability(param.ioCapabilityRspParam_.addr, param.ioCapabilityRspParam_.ioCapability);
            break;
        case GAP_LINK_KEY_REQ_EVT:
            SetLinkKey(param.linkKeyReqParam_.addr);
            break;
        case GAP_LINK_KEY_NOTIFICATION_EVT:
            ReceiveLinkKeyNotification(param.linkKeyNotificationParam_.addr,
                param.linkKeyNotificationParam_.linkKey,
                param.linkKeyNotificationParam_.keyType);
            break;
        case GAP_SIMPLE_PAIR_COMPLETE_EVT:
            ReceiveSimplePairComplete(param.simplePairCompleteParam_.addr, param.simplePairCompleteParam_.status);
            break;
        case GAP_AUTHENTICATION_COMPLETE_EVT:
            ReceiveAuthenticationComplete(
                param.authenticationCompleteParam_.addr, param.authenticationCompleteParam_.status);
            break;
        case GAP_ENCRYPTION_CHANGE_CALLBACK_EVT:
            ReceiveEncryptionChange(
                param.encryptionChangeCallbackParam_.addr, param.encryptionChangeCallbackParam_.status);
            break;
        case GAP_AUTHORIZE_IND_CALLBACK_EVT:
            SetAuthorizeRes(param.authorizeIndParam_.addr, param.authorizeIndParam_.service);
            break;
        default:
            break;
    }
}

void ClassicAdapter::HandleInquiryResult(
    const BtAddr &addr, uint32_t classOfDevice, std::vector<uint8_t> eir, int8_t rssi)
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    discoveryState_ = DISCOVERYING;

    RawAddress device = RawAddress::ConvertToString(addr.addr);
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
    int cod = (classOfDevice & CLASS_OF_DEVICE_RANGE);
    if (cod != remoteDevice->GetDeviceClass()) {
        remoteDevice->SetDeviceClass(cod);
        SendRemoteCodChanged(device, cod);
    }

    remoteDevice->SetDeviceType(REMOTE_TYPE_BREDR);
    remoteDevice->SetRssi(rssi);
    if (!eir.empty()) {
        ParserEirData(remoteDevice, eir);
    } else {
        if (remoteDevice->GetRemoteName().empty()) {
            remoteDevice->SetNameNeedGet(true);
        }
    }

    SendDiscoveryResult(device);
}

std::shared_ptr<ClassicRemoteDevice> ClassicAdapter::FindRemoteDevice(const RawAddress &device)
{
    std::shared_ptr<ClassicRemoteDevice> remoteDevice;
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        remoteDevice = it->second;
    } else {
        remoteDevice = std::make_shared<ClassicRemoteDevice>(device.GetAddress());
        devices_.insert(std::make_pair(device.GetAddress(), remoteDevice));
    }

    return remoteDevice;
}

void ClassicAdapter::HandleInquiryComplete(uint8_t status)
{
    HILOGI("status: %{public}u", status);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    receiveInquiryComplete_ = true;
    struct timeval tv {};
    gettimeofday(&tv, nullptr);
    long currentTime = (tv.tv_sec * MILLISECOND_UNIT + tv.tv_usec / MILLISECOND_UNIT);
    discoveryEndMs_ = currentTime;

    if ((cancelDiscovery_) || (!DiscoverRemoteName())) {
        discoveryState_ = DISCOVERY_STOPED;
        SendDiscoveryStateChanged(discoveryState_);
        receiveInquiryComplete_ = false;
        cancelDiscovery_ = false;
    }

    if (isDisable_) {
        DisablePairProcess();
        return;
    }
}

bool ClassicAdapter::DiscoverRemoteName()
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (cancelDiscovery_) {
        HILOGI("failed, because of CancelDiscovery");
        return false;
    }

    for (auto device : devices_) {
        if (device.second->GetNameNeedGet()) {
            device.second->SetNameNeedGet(false);
            remoteNameAddr_ = device.second->GetAddress();
            RawAddress rawAddr = RawAddress(remoteNameAddr_);
            BtAddr btAddr = ConvertToBtAddr(rawAddr);
            bool ret = GetRemoteName(btAddr);
            ClassicUtils::CheckReturnValue("ClassicAdapter", "GetRemoteName", ret);
            return true;
        }
    }

    return false;
}

BtAddr ClassicAdapter::ConvertToBtAddr(const RawAddress &device) const
{
    BtAddr btAddr;
    device.ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    return btAddr;
}

void ClassicAdapter::ReceiveRemoteName(uint8_t status, const BtAddr &addr, const uint8_t name[MAX_LOC_BT_NAME_LEN])
{
    HILOGI("status: %{public}u", status);

    if (hwTimer_ != nullptr) {
        hwTimer_->Stop();
    }

    if (status == BT_NO_ERROR) {
        std::vector<uint8_t> nameVec(name, (name + MAX_LOC_BT_NAME_LEN));
        std::string deviceName(nameVec.begin(), nameVec.end());
        deviceName = deviceName.c_str();
        RawAddress device = RawAddress::ConvertToString(addr.addr);
        std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
        if (deviceName != remoteDevice->GetRemoteName()) {
            remoteDevice->SetRemoteName(deviceName);
            SendRemoteNameChanged(device, deviceName);
        }
    }

    if (receiveInquiryComplete_) {
        if (!DiscoverRemoteName()) {
            std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
            discoveryState_ = DISCOVERY_STOPED;
            SendDiscoveryStateChanged(discoveryState_);
            receiveInquiryComplete_ = false;
        }
    }

    if (isDisable_) {
        DisablePairProcess();
        return;
    }
}

bool ClassicAdapter::CancelGetRemoteName() const
{
    HILOGI("enter");

    RawAddress rawAddr = RawAddress(remoteNameAddr_);
    BtAddr btAddr = ConvertToBtAddr(rawAddr);
    bool ret = (GAPIF_GetRemoteNameCancel(&btAddr) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_GetRemoteNameCancel", ret);

    return ret;
}

void ClassicAdapter::SendDiscoveryStateChanged(int discoveryState) const
{
    HILOGI("state: %{public}d", discoveryState);

    pimpl->adapterObservers_.ForEach(
        [discoveryState](IAdapterClassicObserver &observer) { observer.OnDiscoveryStateChanged(discoveryState); });
}

void ClassicAdapter::SendDiscoveryResult(const RawAddress &device) const
{
    HILOGI("address: %{public}s", GetEncryptAddr(device.GetAddress()).c_str());

    pimpl->adapterObservers_.ForEach(
        [device](IAdapterClassicObserver &observer) { observer.OnDiscoveryResult(device); });
}

void ClassicAdapter::SendRemoteCodChanged(const RawAddress &device, int cod) const
{
    HILOGI("address: %{public}s, cod: %{public}d", GetEncryptAddr(device.GetAddress()).c_str(), cod);

    pimpl->remoteObservers_.ForEach(
        [device, cod](IClassicRemoteDeviceObserver &observer) { observer.OnRemoteCodChanged(device, cod); });
}

void ClassicAdapter::SSPConfirmReq(const BtAddr &addr, int reqType, int number)
{
    HILOGI("reqTyep: %{public}d", reqType);

    RawAddress device = RawAddress::ConvertToString(addr.addr);
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
    remoteDevice->SetPairConfirmState(PAIR_CONFIRM_STATE_USER_CONFIRM);
    remoteDevice->SetPairConfirmType(reqType);
    int remoteIo = remoteDevice->GetIoCapability();
    if (remoteDevice->GetPairedStatus() == PAIR_CANCELING) {
        UserConfirmAutoReply(device, reqType, false);
    } else if (CheckAutoReply(remoteIo) == true) {
        UserConfirmAutoReply(device, reqType, true);
    } else {
        reqType = CheckSspConfirmType(remoteIo, reqType);
        SendPairConfirmed(device, reqType, number);
    }
}

void ClassicAdapter::PinCodeReq(const BtAddr &addr)
{
    HILOGI("enter");

    RawAddress device = RawAddress::ConvertToString(addr.addr);
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
    remoteDevice->SetPairConfirmState(PAIR_CONFIRM_STATE_USER_CONFIRM);
    remoteDevice->SetPairConfirmType(PAIR_CONFIRM_TYPE_PIN_CODE);
    pinMode_ = true;
    if (remoteDevice->GetRemoteName().empty()) {
        GetRemoteName(addr);
    }

    if (remoteDevice->GetPairedStatus() == PAIR_CANCELING) {
        UserConfirmAutoReply(device, PAIR_CONFIRM_TYPE_PIN_CODE, false);
        return;
    }

    if ((remoteDevice->GetPairedStatus() != PAIR_PAIRING) && (remoteDevice->GetPairedStatus() != PAIR_CANCELING)) {
        remoteDevice->SetPairedStatus(PAIR_PAIRING);
    }
    bool bondFromLocal = false;
    bool ret = (GAPIF_PairIsFromLocal(&addr, &bondFromLocal) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_PairIsFromLocal", ret);
    remoteDevice->SetBondedFromLocal(bondFromLocal);
    if (bondFromLocal) {
        if (remoteDevice->CheckCod(CLASS_OF_DEVICE_AV_HEADSETS) ||
            remoteDevice->CheckCod(CLASS_OF_DEVICE_AV_HEADPHONES) ||
            remoteDevice->CheckCod(CLASS_OF_DEVICE_AV_PORTABLE_AUDIO) ||
            remoteDevice->CheckCod(CLASS_OF_DEVICE_AV_HIFI_AUDIO) ||
            remoteDevice->CheckCod(CLASS_OF_DEVICE_HID_POINTING)) {
            if ((!Compat::CompatCheck(CompatType::COMPAT_REJECT_AUTO_PAIRING, remoteDevice->GetAddress())) &&
                (!Compat::CompatCheck(CompatType::COMPAT_REJECT_AUTO_PAIRING, remoteDevice->GetRemoteName()))) {
                UserConfirmAutoReply(device, PAIR_CONFIRM_TYPE_PIN_CODE, true);
                return;
            }
        }
        if (remoteDevice->CheckCod(CLASS_OF_DEVICE_HID_KEYBOARD) || remoteDevice->CheckCod(CLASS_OF_DEVICE_HID_COMBO)) {
            if (Compat::CompatCheck(CompatType::COMPAT_KEYBOARD_REQUIRES_FIXED_PIN, remoteDevice->GetAddress())) {
                UserConfirmAutoReply(device, PAIR_CONFIRM_TYPE_PIN_CODE, true);
                return;
            }
        }
    }

    SendPairConfirmed(device, PAIR_CONFIRM_TYPE_PIN_CODE, 0);
}

void ClassicAdapter::SendPairConfirmed(const RawAddress &device, int reqType, int number) const
{
    HILOGI("reqType = %{public}d, number = %{public}d", reqType, number);
    BTTransport transport = ADAPTER_BREDR;
    pimpl->adapterObservers_.ForEach([transport, device, reqType, number](IAdapterClassicObserver &observer) {
        observer.OnPairConfirmed(transport, device, reqType, number);
    });
}

void ClassicAdapter::UserConfirmAutoReply(const RawAddress &device, int reqType, bool accept) const
{
    HILOGI("address: %{public}s, accept: %{public}d", GetEncryptAddr(device.GetAddress()).c_str(), accept);

    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        it->second->SetPairConfirmState(PAIR_CONFIRM_STATE_USER_CONFIRM_REPLY);
        it->second->SetPairConfirmType(PAIR_CONFIRM_TYPE_INVALID);
    }

    switch (reqType) {
        case PAIR_CONFIRM_TYPE_PIN_CODE: {
            std::string passkey = adapterProperties_.GetPasskey();
            HILOGI("PAIR_CONFIRM_TYPE_PIN_CODE, value = %{public}s", passkey.c_str());
            std::vector<uint8_t> pinCode(passkey.begin(), passkey.end());
            SetPinCode(device, pinCode, accept);
            break;
        }
        case PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY: {
            HILOGI("PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY, do nothing!!");
            break;
        }
        case PAIR_CONFIRM_TYPE_PASSKEY_INPUT: {
            HILOGI("PAIR_CONFIRM_TYPE_PASSKEY_INPUT");
            int passkey = 0;
            SetDevicePasskey(device, passkey, accept);
            break;
        }
        case PAIR_CONFIRM_TYPE_NUMERIC:
        case PAIR_CONFIRM_TYPE_CONSENT: {
            HILOGI("PAIR_CONFIRM_TYPE_NUMERIC");
            SetDevicePairingConfirmation(device, accept);
            break;
        }
        default:
            HILOGI("default case. reqType = %{public}d", reqType);
            break;
    }
}

void ClassicAdapter::ParserEirData(std::shared_ptr<ClassicRemoteDevice> remote, const std::vector<uint8_t> &data) const
{
    std::vector<ClassicDataStructure> dataList = eirData_.ParserData(data);
    std::vector<Uuid> uuids;
    bool isGetRemoteName = false;

    for (auto &dataStruc : dataList) {
        int type = dataStruc.GetType();
        std::vector<uint8_t> valueVec = dataStruc.GetDataValue();
        switch (type) {
            case BLUETOOTH_DATA_TYPE_SHORTENED_LOCAL_NAME:
            case BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME:
                isGetRemoteName = ParserRemoteNameFromEir(remote, type, valueVec);
                break;
            case BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS:
            case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS:
            case BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS:
            case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS:
            case BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS:
            case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS: {
                std::vector<Uuid> parsedUuids = ParserUuidFromEir(type, valueVec);
                if (!parsedUuids.empty()) {
                    uuids.insert(uuids.end(), parsedUuids.begin(), parsedUuids.end());
                }
                break;
            }
            /// Flags, TxPower, ManufacturerData, Uri are not support for current framework api.
            /// This is used for extending.
            case BLUETOOTH_DATA_TYPE_FLAGS:
                remote->SetFlags(valueVec[0]);
                break;
            case BLUETOOTH_DATA_TYPE_TX_POWER_LEVEL:
                remote->SetTxPower(valueVec[0]);
                break;
            case BLUETOOTH_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA:
                remote->SetManufacturerSpecificData(valueVec);
                break;
            case BLUETOOTH_DATA_TYPE_URI: {
                std::string uri(valueVec.begin(), valueVec.end());
                remote->SetURI(uri);
                break;
            }
            default:
                break;
        }
    }

    if (!isGetRemoteName) {
        remote->SetNameNeedGet(true);
    }

    if (!uuids.empty()) {
        SaveRemoteDeviceUuids(remote, uuids);
        uuids.clear();
    }
}

bool ClassicAdapter::ParserRemoteNameFromEir(
    std::shared_ptr<ClassicRemoteDevice> remote, int type, const std::vector<uint8_t> &value) const
{
    HILOGI("type %{public}d", type);

    if (value.empty()) {
        return false;
    }

    if (type == BLUETOOTH_DATA_TYPE_SHORTENED_LOCAL_NAME) {
        remote->SetNameNeedGet(true);
    } else {
        remote->SetNameNeedGet(false);
    }

    std::string remoteName(value.begin(), value.end());
    HILOGI("remoteName is %{public}s", remoteName.c_str());
    if (remoteName != remote->GetRemoteName()) {
        remote->SetRemoteName(remoteName);
        SendRemoteNameChanged(RawAddress(remote->GetAddress()), remoteName);
    }

    return true;
}

std::vector<Uuid> ClassicAdapter::ParserUuidFromEir(int type, const std::vector<uint8_t> &value) const
{
    HILOGI("enter");

    std::vector<Uuid> uuids;
    if (value.empty()) {
        HILOGI("ParserUuidFromEir invalid parameter.");
        return uuids;
    }

    switch (type) {
        case BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS:
        case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS: {
            uuids = Parser16BitUuidFromEir(value);
            break;
        }
        case BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS:
        case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS: {
            uuids = Parser32BitUuidFromEir(value);
            break;
        }
        case BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS:
        case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS: {
            uuids = Parser128BitUuidFromEir(value);
            break;
        }
        default:
            break;
    }

    return uuids;
}

std::vector<Uuid> ClassicAdapter::Parser16BitUuidFromEir(const std::vector<uint8_t> &value) const
{
    HILOGI("enter");

    std::vector<Uuid> uuids;
    uint8_t offset = 0;
    while ((value.size() - offset) > 1) {
        uint8_t unitLen = sizeof(uint16_t);
        if (unitLen <= (value.size() - offset)) {
            uint16_t uuid16 = 0;
            (void)memcpy_s(&uuid16, unitLen, &value[offset], unitLen);
            Uuid uuid = Uuid::ConvertFrom16Bits(uuid16);
            if (uuid16 != 0) {
                uuids.push_back(uuid);
                HILOGI("remote 16bit UUID");
            }
            offset += unitLen;
        } else {
            break;
        }
    }

    return uuids;
}

std::vector<Uuid> ClassicAdapter::Parser32BitUuidFromEir(const std::vector<uint8_t> &value) const
{
    HILOGI("enter");

    std::vector<Uuid> uuids;
    int offset = 0;
    while ((value.size() - offset) > 1) {
        uint8_t unitLen = sizeof(uint32_t);
        if (unitLen <= (value.size() - offset)) {
            uint32_t uuid32 = 0;
            (void)memcpy_s(&uuid32, unitLen, &value[offset], unitLen);
            Uuid uuid = Uuid::ConvertFrom32Bits(uuid32);
            if (uuid32 != 0) {
                uuids.push_back(uuid);
                HILOGI("remote 32bit UUID");
            }
            offset += unitLen;
        } else {
            break;
        }
    }

    return uuids;
}

std::vector<Uuid> ClassicAdapter::Parser128BitUuidFromEir(const std::vector<uint8_t> &value) const
{
    HILOGI("enter");

    std::vector<Uuid> uuids;
    int offset = 0;
    while ((value.size() - offset) > 1) {
        if (UUID128_BYTES_TYPE <= (value.size() - offset)) {
            Uuid uuid = Uuid::ConvertFromBytesLE(&value[offset]);
            bluetooth::Uuid::UUID128Bit uuidCheck = {0};
            if (uuid.ConvertTo128Bits() != uuidCheck) {
                uuids.push_back(uuid);
                HILOGI("remote 128bit UUID");
            }
            offset += UUID128_BYTES_TYPE;
        } else {
            break;
        }
    }

    return uuids;
}

void ClassicAdapter::SaveRemoteDeviceUuids(
    std::shared_ptr<ClassicRemoteDevice> remote, const std::vector<Uuid> &uuids) const
{
    HILOGI("enter");

    if ((remote == nullptr) || (uuids.empty())) {
        return;
    }

    std::vector<Uuid> prevUuids = remote->GetDeviceUuids();
    if (!IsUuidsEqual(uuids, prevUuids)) {
        std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
        remote->SetDeviceUuids(uuids);
        SendRemoteUuidChanged(RawAddress(remote->GetAddress()), uuids);
    }
}

bool ClassicAdapter::IsUuidsEqual(const std::vector<Uuid> &uuids1, const std::vector<Uuid> &uuids2) const
{
    HILOGI("enter");
    if (uuids1.size() != uuids2.size()) {
        return false;
    }

    for (auto uuid : uuids1) {
        auto it = std::find(uuids2.begin(), uuids2.end(), uuid);
        if (it == uuids2.end()) {
            return false;
        }
    }

    for (auto uuid : uuids2) {
        auto it = std::find(uuids1.begin(), uuids1.end(), uuid);
        if (it == uuids1.end()) {
            return false;
        }
    }

    return true;
}

void ClassicAdapter::SendRemoteNameChanged(const RawAddress &device, const std::string &deviceName) const
{
    HILOGI("enter");
    pimpl->remoteObservers_.ForEach([device, deviceName](IClassicRemoteDeviceObserver &observer) {
        observer.OnRemoteNameChanged(device, deviceName);
    });
}

void ClassicAdapter::SendPairStatusChanged(const BTTransport transport, const RawAddress &device, int status) const
{
    HILOGI("status: %{public}d", status);
    pimpl->remoteObservers_.ForEach([transport, device, status](IClassicRemoteDeviceObserver &observer) {
        observer.OnPairStatusChanged(transport, device, status);
    });
}

void ClassicAdapter::ReceiveLinkKeyNotification(
    const BtAddr &addr, const uint8_t linkKey[GAP_LINKKEY_SIZE], uint8_t keyType)
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    /// Pairing success, save link key and sent notification to APP.
    if (pinMode_ == true) {
        pinMode_ = false;
    }

    RawAddress device = RawAddress::ConvertToString(addr.addr);
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
    if (remoteDevice->GetRemoteName().empty()) {
        GetRemoteName(addr);
    }
    SearchRemoteUuids(device, PNP_SERVICE_CLASS_UUID);

    /// Bondable mode is on, then save the linkKey and keyType
    if (adapterProperties_.GetBondableMode() == BONDABLE_MODE_ON) {
        std::vector<uint8_t> linkKeyVec(linkKey, (linkKey + GAP_LINKKEY_SIZE));
        remoteDevice->SetLinkKey(linkKeyVec);
        remoteDevice->SetLinkKeyType(keyType);
        remoteDevice->SetPairedStatus(PAIR_PAIRED);
        if (remoteDevice != nullptr) {
            adapterProperties_.SavePairedDeviceInfo(remoteDevice);
            adapterProperties_.SaveConfigFile();
        }
    } else {
        remoteDevice->SetPairedStatus(PAIR_NONE);
        HILOGW("Bondmode is off, so not save the link key");
    }
}

void ClassicAdapter::ReceiveSimplePairComplete(const BtAddr &addr, uint8_t status)
{
    HILOGI("status: %{public}u", status);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (status != SUCCESS) {
        RawAddress device = RawAddress::ConvertToString(addr.addr);
        std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
        bool bondFromLocal = remoteDevice->IsBondedFromLocal();
        /// Passive pairing failed, delete the link key.
        if (bondFromLocal == false) {
            DeleteLinkKey(remoteDevice);
            /// Set the pair flag and pair state.
            remoteDevice->SetPairedStatus(PAIR_NONE);
            /// Send the failed notification to APP.
            SendPairStatusChanged(ADAPTER_BREDR, device, PAIR_NONE);
        }
    }
}

void ClassicAdapter::DeleteLinkKey(std::shared_ptr<ClassicRemoteDevice> remoteDevice) const
{
    int keyType = remoteDevice->GetLinkKeyType();
    if (keyType != PAIR_INVALID_LINK_KEY_TYPE) {
        remoteDevice->DeleteLinkKey();
        remoteDevice->SetLinkKeyType(PAIR_INVALID_LINK_KEY_TYPE);
    }
}

void ClassicAdapter::ReceiveAuthenticationComplete(const BtAddr &addr, uint8_t status)
{
    HILOGI("status: %{public}u", status);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    RawAddress device = RawAddress::ConvertToString(addr.addr);
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
    remoteDevice->SetPairConfirmState(PAIR_CONFIRM_STATE_INVALID);
    remoteDevice->SetPairConfirmType(PAIR_CONFIRM_TYPE_INVALID);

    if (status != SUCCESS) {
        /// Active pairing failed, delete the link key.
        DeleteLinkKey(remoteDevice);
        /// Set the pair flag and pair state.
        remoteDevice->SetPairedStatus(PAIR_NONE);
        /// Send the failed notification to APP.
        SendPairStatusChanged(ADAPTER_BREDR, device, PAIR_NONE);
    } else {
        if (remoteDevice->GetPairedStatus() == PAIR_PAIRING) {
            remoteDevice->SetPairedStatus(PAIR_PAIRED);
            SendPairStatusChanged(ADAPTER_BREDR, device, PAIR_PAIRED);
        }
    }

    if (waitPairResult_) {
        waitPairResult_ = false;
        InitMode();
    }
}

void ClassicAdapter::ReceiveEncryptionChange(const BtAddr &addr, uint8_t status)
{
    HILOGI("status: %{public}u", status);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    RawAddress device = RawAddress::ConvertToString(addr.addr);
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
    if (status == GAP_ENCRYPTION_ON) {
        remoteDevice->SetAclConnectState(CONNECTION_STATE_ENCRYPTED_BREDR);
    } else {
        remoteDevice->SetAclConnectState(CONNECTION_STATE_CONNECTED);
    }
}

bool ClassicAdapter::GetRemoteName(const BtAddr &addr) const
{
    HILOGI("enter");

    bool ret = (GAPIF_GetRemoteName(&addr) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_GetRemoteName", ret);
    if (ret && (hwTimer_ != nullptr)) {
        hwTimer_->Start(DEFAULT_HW_TIMEOUT);
    }

    return ret;
}

std::string ClassicAdapter::GetDeviceName(const RawAddress &device) const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    std::string remoteName = "";
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        remoteName = it->second->GetRemoteName();
    }

    return remoteName;
}

std::vector<Uuid> ClassicAdapter::GetDeviceUuids(const RawAddress &device) const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    std::vector<Uuid> uuids;
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        uuids = it->second->GetDeviceUuids();
    }
    return uuids;
}

void ClassicAdapter::SearchRemoteUuids(const RawAddress &device, uint16_t uuid)
{
    BtAddr btAddr = ConvertToBtAddr(device);

    BtUuid btUuid[SDP_UUDIID_NUM];
    btUuid[0].type = BT_UUID_16;
    btUuid[0].uuid16 = uuid;
    searchUuid_ = uuid;

    SdpUuid uuidArray;
    uuidArray.uuid = &btUuid[0];
    uuidArray.uuidNum = 1;

    SdpAttributeIdList attributeIdList;
    attributeIdList.type = SDP_TYPE_RANGE;
    attributeIdList.attributeIdRange.start = 0x0000;
    attributeIdList.attributeIdRange.end = 0xFFFF;
    int ret = SDP_ServiceSearchAttribute(&btAddr, &uuidArray, attributeIdList, (void *)this, ServiceSearchAttributeCb);
    if (ret != BT_NO_ERROR) {
        HILOGE("SDP_ServiceSearchAttribute failed!");
    }
}

void ClassicAdapter::ServiceSearchAttributeCb(
    const BtAddr *addr, const SdpService *serviceArray, uint16_t serviceNum, void *context)
{
    HILOGI("enter");

    auto adapter = static_cast<ClassicAdapter *>(context);
    for (int index = 0; index < serviceNum; index++) {
        for (int num = 0; num < serviceArray[index].classIdNumber; num++) {
            Uuid uuid = adapter->GetUuidFromBtUuid(serviceArray[index].classId[num]);
            auto it = std::find(adapter->uuids_.begin(), adapter->uuids_.end(), uuid);
            if (it == adapter->uuids_.end()) {
                adapter->uuids_.push_back(uuid);
            }
        }
    }

    RawAddress device = RawAddress::ConvertToString(addr->addr);
    if (adapter->searchUuid_ == UUID_PROTOCOL_L2CAP) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::SearchAttributeEnd, adapter, device, adapter->uuids_));
    } else {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::SearchRemoteUuids, adapter, device, UUID_PROTOCOL_L2CAP));
    }
}

void ClassicAdapter::SearchAttributeEnd(const RawAddress &device, const std::vector<Uuid> &uuids)
{
    if (isDisable_) {
        return;
    }
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
    SaveRemoteDeviceUuids(remoteDevice, uuids);
    if (remoteDevice != nullptr) {
        adapterProperties_.SavePairedDeviceInfo(remoteDevice);
        adapterProperties_.SaveConfigFile();
    }
    SendPairStatusChanged(ADAPTER_BREDR, device, PAIR_PAIRED);
}

Uuid ClassicAdapter::GetUuidFromBtUuid(const BtUuid &inUuid) const
{
    Uuid uuid;
    if (inUuid.type == BT_UUID_16) {
        uuid = Uuid::ConvertFrom16Bits(inUuid.uuid16);
    } else if (inUuid.type == BT_UUID_32) {
        uuid = Uuid::ConvertFrom32Bits(inUuid.uuid32);
    } else {
        uuid = Uuid::ConvertFromBytesLE(inUuid.uuid128);
    }

    return uuid;
}

void ClassicAdapter::SendRemoteUuidChanged(const RawAddress &device, const std::vector<Uuid> &uuids) const
{
    pimpl->remoteObservers_.ForEach(
        [device, uuids](IClassicRemoteDeviceObserver &observer) { observer.OnRemoteUuidChanged(device, uuids); });
}

bool ClassicAdapter::IsAclConnected(const RawAddress &device) const
{
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool isAclConnected = false;
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        isAclConnected = it->second->IsAclConnected();
    }

    HILOGI("isAclConnected: %{public}d", isAclConnected);
    return isAclConnected;
}

bool ClassicAdapter::IsAclEncrypted(const RawAddress &device) const
{
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool isAclEncrypted = false;
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        isAclEncrypted = it->second->IsAclEncrypted();
    }

    HILOGI("isAclEncrypted: %{public}d", isAclEncrypted);
    return isAclEncrypted;
}

utility::Context *ClassicAdapter::GetContext()
{
    return this;
}

bool ClassicAdapter::IsBondedFromLocal(const RawAddress &device) const
{
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool isBondedFromLocal = false;
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        isBondedFromLocal = it->second->IsBondedFromLocal();
    }

    HILOGI("isBondedFromLocal: %{public}d", isBondedFromLocal);
    return isBondedFromLocal;
}

std::vector<RawAddress> ClassicAdapter::GetPairedDevices() const
{
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    std::vector<RawAddress> pairedList;
    for (auto &device : devices_) {
        if (device.second->IsPaired() == true) {
            RawAddress rawAddr(device.second->GetAddress());
            pairedList.push_back(rawAddr);
        }
    }

    return pairedList;
}

bool ClassicAdapter::StartPair(const RawAddress &device)
{
    HILOGI("enter");

    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (remoteDevice->GetPairedStatus() == PAIR_PAIRING) {
        HILOGE("StartPair failed, because of PAIR_NONE or PAIRING!");
        return false;
    }

    if (discoveryState_ == DISCOVERYING) {
        bool ret = CancelBtDiscovery();
        ClassicUtils::CheckReturnValue("ClassicAdapter", "CancelBtDiscovery", ret);
    }

    BtAddr btAddr = ConvertToBtAddr(device);
    int ret = GAPIF_AuthenticationReq(&btAddr);
    if (ret != BT_NO_ERROR) {
        HILOGE("GAPIF_AuthenticationReq failed!");
        return false;
    }

    DeleteLinkKey(remoteDevice);
    remoteDevice->SetPairedStatus(PAIR_PAIRING);
    SendPairStatusChanged(ADAPTER_BREDR, device, PAIR_PAIRING);

    return true;
}

bool ClassicAdapter::CancelPairing(const RawAddress &device)
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    auto it = devices_.find(device.GetAddress());
    if (it == devices_.end() || (it->second->GetPairedStatus() != PAIR_PAIRING)) {
        HILOGE("failed, because of not in PAIR_PAIRING!");
        return false;
    }

    it->second->SetPairedStatus(PAIR_CANCELING);
    int pairConfirmState = it->second->GetPairConfirmState();
    if (pairConfirmState == PAIR_CONFIRM_STATE_USER_CONFIRM_REPLY) {
        BtAddr btAddr = ConvertToBtAddr(device);
        bool ret = (GAPIF_CancelAuthenticationReq(&btAddr) == BT_NO_ERROR);
        ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_CancelAuthenticationReq", ret);
        return ret;
    }

    if (pairConfirmState == PAIR_CONFIRM_STATE_USER_CONFIRM) {
        int pairConfirmType = it->second->GetPairConfirmType();
        RawAddress address(it->second->GetAddress());
        UserConfirmAutoReply(address, pairConfirmType, false);
    }
    return true;
}

bool ClassicAdapter::RemovePair(const RawAddress &device)
{
    HILOGI("address %{public}s", GetEncryptAddr(device.GetAddress()).c_str());

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    auto it = devices_.find(device.GetAddress());
    if ((it == devices_.end()) || (it->second->IsPaired() == false)) {
        HILOGW("RemovePair failed, because of not find the paired device!");
        return false;
    } else {
        it->second->SetPairedStatus(PAIR_NONE);
        DeleteLinkKey(it->second);
        adapterProperties_.RemovePairedDeviceInfo(it->second->GetAddress());
        adapterProperties_.SaveConfigFile();
        if (it->second->IsAclConnected()) {
            bool ret = (BTM_AclDisconnect(it->second->GetConnectionHandle(), BTM_ACL_DISCONNECT_REASON) == BT_NO_ERROR);
            ClassicUtils::CheckReturnValue("ClassicAdapter", "BTM_AclDisconnect", ret);
        }
    }

    std::vector<RawAddress> removeDevices;
    removeDevices.push_back(device);
    AdapterManager::GetInstance()->OnPairDevicesRemoved(BTTransport::ADAPTER_BREDR, removeDevices);
    SendPairStatusChanged(BTTransport::ADAPTER_BREDR, device, PAIR_NONE);
    return true;
}

bool ClassicAdapter::RemoveAllPairs()
{
    HILOGI("enter");
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    std::vector<RawAddress> removeDevices;
    for (auto it = devices_.begin(); it != devices_.end(); it++) {
        if (it->second->IsPaired() == true) {
            it->second->SetPairedStatus(PAIR_NONE);
            DeleteLinkKey(it->second);
            adapterProperties_.RemovePairedDeviceInfo(it->second->GetAddress());
            RawAddress device = RawAddress(it->second->GetAddress());
            removeDevices.push_back(device);
            if (it->second->IsAclConnected()) {
                bool ret =
                    (BTM_AclDisconnect(it->second->GetConnectionHandle(), BTM_ACL_DISCONNECT_REASON) == BT_NO_ERROR);
                ClassicUtils::CheckReturnValue("ClassicAdapter", "BTM_AclDisconnect", ret);
            }
        }
    }

    if (!removeDevices.empty()) {
        adapterProperties_.SaveConfigFile();
        AdapterManager::GetInstance()->OnPairDevicesRemoved(BTTransport::ADAPTER_BREDR, removeDevices);
    }

    return true;
}

int ClassicAdapter::GetPairState(const RawAddress &device) const
{
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    int pairState = PAIR_NONE;
    auto it = devices_.find(device.GetAddress());
    if (it == devices_.end()) {
        return pairState;
    } else {
        pairState = it->second->GetPairedStatus();
    }

    HILOGI("state: %{public}d", pairState);
    return pairState;
}

bool ClassicAdapter::SetDevicePairingConfirmation(const RawAddress &device, bool accept) const
{
    HILOGI("accept = %{public}d", accept);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool ret = false;
    auto it = devices_.find(device.GetAddress());
    if ((it == devices_.end()) || (it->second->GetPairedStatus() == PAIR_PAIRED) ||
        (it->second->GetPairedStatus() == PAIR_NONE)) {
        HILOGE("failed, not in pairing state.");
        return ret;
    }

    it->second->SetPairConfirmState(PAIR_CONFIRM_STATE_USER_CONFIRM_REPLY);
    it->second->SetPairConfirmType(PAIR_CONFIRM_TYPE_INVALID);

    BtAddr btAddr = ConvertToBtAddr(device);
    if (it->second->GetPairedStatus() == PAIR_CANCELING || accept == false) {
        ret = (GAPIF_UserConfirmRsp(&btAddr, GAP_NOT_ACCEPT) == BT_NO_ERROR);
    } else {
        ret = (GAPIF_UserConfirmRsp(&btAddr, GAP_ACCEPT) == BT_NO_ERROR);
    }
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_UserConfirmRsp", ret);

    return ret;
}

bool ClassicAdapter::SetDevicePasskey(const RawAddress &device, int passkey, bool accept) const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool ret = false;
    auto it = devices_.find(device.GetAddress());
    if ((it == devices_.end()) || (it->second->GetPairedStatus() == PAIR_NONE) ||
        (it->second->GetPairedStatus() == PAIR_PAIRED)) {
        HILOGE("failed, not in pairing state.");
        return ret;
    }

    it->second->SetPairConfirmType(PAIR_CONFIRM_TYPE_INVALID);
    it->second->SetPairConfirmState(PAIR_CONFIRM_STATE_USER_CONFIRM_REPLY);

    BtAddr btAddr = ConvertToBtAddr(device);
    if (it->second->GetPairedStatus() == PAIR_CANCELING || accept == false) {
        ret = (GAPIF_UserPasskeyRsp(&btAddr, GAP_NOT_ACCEPT, passkey) == BT_NO_ERROR);
    } else {
        ret = (GAPIF_UserPasskeyRsp(&btAddr, GAP_ACCEPT, passkey) == BT_NO_ERROR);
    }
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_UserPasskeyRsp", ret);

    return ret;
}

bool ClassicAdapter::PairRequestReply(const RawAddress &device, bool accept) const
{
    HILOGI("accept = %{public}d", accept);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    auto it = devices_.find(device.GetAddress());
    if ((it == devices_.end()) || (it->second->GetPairedStatus() != PAIR_PAIRING)) {
        HILOGE("failed, not in pairing state.");
        return false;
    }

    BtAddr btAddr = ConvertToBtAddr(device);
    int io = adapterProperties_.GetIoCapability();
    bool ret = (GAPIF_IOCapabilityRsp(&btAddr, accept, io, GAP_OOB_DATA_NOT_PRESENT, GAP_MITM_REQUIRED) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_IOCapabilityRsp", ret);

    return ret;
}

void ClassicAdapter::ConnectionComplete(const BtmAclConnectCompleteParam *param, void *context)
{
    HILOGI("status: %{public}u", param->status);

    auto adapter = static_cast<ClassicAdapter *>(context);
    RawAddress device = RawAddress::ConvertToString(param->addr->addr);
    uint32_t classOfDevice = 0;
    (void)memcpy_s(&classOfDevice, COD_SIZE, param->classOfDevice, COD_SIZE);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(std::bind(&ClassicAdapter::ReceiveConnectionComplete,
            adapter,
            param->status,
            param->connectionHandle,
            device,
            classOfDevice,
            param->encyptionEnabled));
    }
}

void ClassicAdapter::ReceiveConnectionComplete(uint8_t status, uint16_t connectionHandle, const RawAddress &device,
    uint32_t classOfDevice, bool encyptionEnabled)
{
    if (status == BTM_ACL_CONNECT_PAGE_TIMEOUT) {
        HILOGE("ACL Connection failed. Reason: ACL Page Timeout!");
    } else if (status == BTM_ACL_CONNECTION_TIMEOUT) {
        HILOGE("ACL Connection failed. Reason: ACL Supervision Timeout!");
    } else {
        HILOGI("ACL Connection result: %{public}u", status);
    }

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);

    /// For compatibility
    /// Passive pairing failed and pair mode is PinCode.
    if (status != SUCCESS) {
        bool bondFromLocal = remoteDevice->IsBondedFromLocal();
        HILOGI("pinMode = %{public}d, bondFromLocal = %{public}d", pinMode_, bondFromLocal);
        if (pinMode_ == true && bondFromLocal == false) {
            pinMode_ = false;
            /// Passive pairing failed, delete the link key.
            DeleteLinkKey(remoteDevice);
            /// Set the pair flag and pair state.
            remoteDevice->SetPairedStatus(PAIR_NONE);
            /// Send the failed notification to APP.
            SendPairStatusChanged(ADAPTER_BREDR, device, PAIR_NONE);
        }
        return;
    }

    int connectState = CONNECTION_STATE_CONNECTED;
    if (encyptionEnabled == true) {
        connectState = CONNECTION_STATE_ENCRYPTED_BREDR;
    }
    remoteDevice->SetAclConnectState(connectState);
    remoteDevice->SetConnectionHandle(connectionHandle);

    int cod = (classOfDevice & CLASS_OF_DEVICE_RANGE);
    if (cod != INVALID_VALUE) {
        remoteDevice->SetDeviceClass(cod);
    }
}

void ClassicAdapter::ReceiveDisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason)
{
    HILOGI("status: %{public}u", status);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    if (status != SUCCESS) {
        HILOGW("failed");
        return;
    }

    for (auto &device : devices_) {
        if (connectionHandle != device.second->GetConnectionHandle()) {
            continue;
        }
        device.second->SetAclConnectState(CONNECTION_STATE_DISCONNECTED);

        HILOGI("pinMode = %{public}d", pinMode_);
        /// Passive pairing failed and pair mode is PinCode.
        /// For 960 compatibility
        /// When ACL disconnect and current pari state is in PAIR_PAIRING or PAIR_CANCELING, set pair state to
        /// PAIR_NONE.
        if ((pinMode_) || (device.second->GetPairedStatus() == PAIR_PAIRING) ||
            (device.second->GetPairedStatus() == PAIR_CANCELING)) {
            pinMode_ = false;
            /// Passive pairing failed, delete the link key.
            DeleteLinkKey(device.second);
            /// Set the pair flag and pair state.
            device.second->SetPairedStatus(PAIR_NONE);
            /// Send the failed notification to APP.
            bool bondFromLocal = device.second->IsBondedFromLocal();
            HILOGI("bondFromLocal = %{public}d", bondFromLocal);
            if (!bondFromLocal) {
                RawAddress address(device.second->GetAddress());
                SendPairStatusChanged(ADAPTER_BREDR, address, PAIR_NONE);
            }
        }
        break;
    }
}

void ClassicAdapter::DisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context)
{
    HILOGI("status: %{public}u", status);

    auto adapter = static_cast<ClassicAdapter *>(context);
    if (adapter != nullptr) {
        adapter->GetDispatcher()->PostTask(
            std::bind(&ClassicAdapter::ReceiveDisconnectionComplete, adapter, status, connectionHandle, reason));
    }
}

bool ClassicAdapter::RegisterClassicAdapterObserver(IAdapterClassicObserver &observer) const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    adapterProperties_.RegisterClassicAdapterObserver(observer);
    return pimpl->adapterObservers_.Register(observer);
}

bool ClassicAdapter::DeregisterClassicAdapterObserver(IAdapterClassicObserver &observer) const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    adapterProperties_.DeregisterClassicAdapterObserver(observer);
    return pimpl->adapterObservers_.Deregister(observer);
}

void ClassicAdapter::SetLinkKey(const BtAddr &addr)
{
    HILOGI("enter");

    RawAddress device = RawAddress::ConvertToString(addr.addr);
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);

    int keyType = remoteDevice->GetLinkKeyType();
    uint8_t key[PAIR_LINK_KEY_SIZE];
    (void)memset_s(key, PAIR_LINK_KEY_SIZE, 0x00, PAIR_LINK_KEY_SIZE);
    uint8_t accept = GAP_NOT_ACCEPT;
    if (keyType != PAIR_INVALID_LINK_KEY_TYPE) {
        std::vector<uint8_t> linkKey = remoteDevice->GetLinkKey();
        if (memcpy_s(key, linkKey.size(), &linkKey[0], linkKey.size()) != EOK) {
            HILOGI("memcpy_s fail");
            return;
        }
        accept = GAP_ACCEPT;
    }

    bool ret = (GAPIF_LinkKeyRsp(&addr, accept, key, keyType) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_LinkKeyRsp", ret);
    (void)memset_s(key, PAIR_LINK_KEY_SIZE, 0x00, PAIR_LINK_KEY_SIZE);
}

void ClassicAdapter::SendPairRequested(const BTTransport transport, const RawAddress &device) const
{
    HILOGI("enter");
    pimpl->adapterObservers_.ForEach(
        [transport, device](IAdapterClassicObserver &observer) { observer.OnPairRequested(transport, device); });
}

void ClassicAdapter::SetIoCapability(const BtAddr &addr)
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    RawAddress rawAddr = RawAddress::ConvertToString(addr.addr);
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(rawAddr);
    if (remoteDevice->GetRemoteName().empty()) {
        GetRemoteName(addr);
    }

    bool bondFromLocal = false;
    bool ret = (GAPIF_PairIsFromLocal(&addr, &bondFromLocal) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_PairIsFromLocal", ret);
    remoteDevice->SetBondedFromLocal(bondFromLocal);

    if ((remoteDevice->GetPairedStatus() != PAIR_PAIRING) && (remoteDevice->GetPairedStatus() != PAIR_CANCELING)) {
        remoteDevice->SetPairedStatus(PAIR_PAIRING);
    }

    if (bondFromLocal == false) {
        SendPairRequested(ADAPTER_BREDR, rawAddr);
    } else {
        int io = adapterProperties_.GetIoCapability();
        if (remoteDevice->GetPairedStatus() == PAIR_CANCELING) {
            ret = (GAPIF_IOCapabilityRsp(&addr, false, io, GAP_OOB_DATA_NOT_PRESENT, GAP_MITM_REQUIRED) == BT_NO_ERROR);
        } else {
            ret = (GAPIF_IOCapabilityRsp(&addr, true, io, GAP_OOB_DATA_NOT_PRESENT, GAP_MITM_REQUIRED) == BT_NO_ERROR);
        }
        ClassicUtils::CheckReturnValue("ClassicAdapter", "GAPIF_IOCapabilityRsp", ret);
    }
}

void ClassicAdapter::SaveRemoteIoCapability(const BtAddr &addr, uint8_t ioCapability)
{
    HILOGI("enter");
    RawAddress device = RawAddress::ConvertToString(addr.addr);
    std::shared_ptr<ClassicRemoteDevice> remoteDevice = FindRemoteDevice(device);
    remoteDevice->SetIoCapability(ioCapability);
}

bool ClassicAdapter::SetDevicePin(const RawAddress &device, const std::string &pinCode) const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    auto it = devices_.find(device.GetAddress());
    if ((it == devices_.end()) || (it->second->GetPairedStatus() == PAIR_NONE) ||
        (it->second->GetPairedStatus() == PAIR_PAIRED)) {
        HILOGE("failed, not in pairing state.");
        return false;
    }

    std::vector<uint8_t> pin(pinCode.begin(), pinCode.end());
    if (pin.empty()) {
        HILOGW("length is 0, reject it.");
        return SetPinCode(device, pin, GAP_NOT_ACCEPT);
    }

    uint8_t accept = GAP_ACCEPT;
    it->second->SetPairConfirmState(PAIR_CONFIRM_STATE_USER_CONFIRM_REPLY);
    it->second->SetPairConfirmType(PAIR_CONFIRM_TYPE_INVALID);
    if (it->second->GetPairedStatus() == PAIR_CANCELING) {
        accept = GAP_NOT_ACCEPT;
    }

    return SetPinCode(device, pin, accept);
}

bool ClassicAdapter::SetPinCode(const RawAddress &device, const std::vector<uint8_t> &pin, bool accept) const
{
    HILOGI("accept: %{public}d", accept);

    BtAddr btAddr;
    device.ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    int result = GAPIF_PinCodeRsp(&btAddr, accept, pin.data(), pin.size());
    if (result != BT_NO_ERROR) {
        HILOGE("GAPIF_PinCodeRsp failed!");
        return false;
    }
    return true;
}

void ClassicAdapter::SetAuthorizeRes(const BtAddr &addr, GAP_Service service) const
{
    HILOGI("enter");
    int result = GAPIF_AuthorizeRes(&addr, service, true);
    if (result != BT_NO_ERROR) {
        HILOGE("GAPIF_AuthorizeRes failed!");
    }
}

int ClassicAdapter::ClassicAdapter::GetDeviceType(const RawAddress &device) const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    int type = INVALID_VALUE;
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        type = it->second->GetDeviceType();
    }

    return type;
}

int ClassicAdapter::GetDeviceClass(const RawAddress &device) const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    int cod = INVALID_VALUE;
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        cod = it->second->GetDeviceClass();
    }

    return cod;
}

std::string ClassicAdapter::GetAliasName(const RawAddress &device) const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    std::string alias = INVALID_NAME;
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        alias = it->second->GetAliasName();
    }

    return alias;
}

bool ClassicAdapter::SetAliasName(const RawAddress &device, const std::string &name) const
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    bool ret = false;
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        if (name != it->second->GetAliasName()) {
            ret = it->second->SetAliasName(name);
            if (ret == false) {
                HILOGE("failed");
            } else {
                SendRemoteAliasChanged(device, name);
            }
        }
    }
    return ret;
}

void ClassicAdapter::SendRemoteAliasChanged(const RawAddress &device, const std::string &aliasName) const
{
    HILOGI("enter");

    pimpl->remoteObservers_.ForEach([device, aliasName](IClassicRemoteDeviceObserver &observer) {
        observer.OnRemoteAliasChanged(device, aliasName);
    });
}

bool ClassicAdapter::RegisterRemoteDeviceObserver(IClassicRemoteDeviceObserver &observer) const
{
    HILOGI("enter");
    return pimpl->remoteObservers_.Register(observer);
}

bool ClassicAdapter::DeregisterRemoteDeviceObserver(IClassicRemoteDeviceObserver &observer) const
{
    HILOGI("enter");
    return pimpl->remoteObservers_.Deregister(observer);
}

bool ClassicAdapter::CheckAutoReply(int remoteIo) const
{
    HILOGI("enter");

    bool autoReply = false;
    int localIo = adapterProperties_.GetIoCapability();
    HILOGI("local io capability = %{public}d <==> remote io capability = %{public}d", localIo, remoteIo);
    switch (localIo) {
        case GAP_IO_DISPLAYONLY:
            autoReply = (remoteIo != GAP_IO_KEYBOARDONLY) ? true : false;
            break;
        case GAP_IO_KEYBOARDONLY:
            autoReply = (remoteIo == GAP_IO_NOINPUTNOOUTPUT) ? true : false;
            break;
        case GAP_IO_NOINPUTNOOUTPUT:
            autoReply = true;
            break;
        default:
            break;
    }
    return autoReply;
}

int ClassicAdapter::CheckSspConfirmType(int remoteIo, int type) const
{
    HILOGI("enter");

    int confirmType = PAIR_CONFIRM_TYPE_CONSENT;
    int localIo = adapterProperties_.GetIoCapability();
    HILOGI("local io capability = %{public}d <==> remote io capability = %{public}d, type = %{public}d",
        localIo, remoteIo, type);
    if (type == PAIR_CONFIRM_TYPE_NUMERIC) {
        if ((localIo == GAP_IO_DISPLAYYESNO) && (remoteIo == GAP_IO_DISPLAYYESNO)) {
            confirmType = PAIR_CONFIRM_TYPE_NUMERIC;
        }
    } else {
        confirmType = type;
    }
    return confirmType;
}

int ClassicAdapter::GetDeviceBatteryLevel(const RawAddress &device) const
{
    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    int batteryLevel = 0;
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        batteryLevel = it->second->GetBatteryLevel();
    }
    HILOGI("batteryLevel: %{public}d", batteryLevel);
    return batteryLevel;
}

void ClassicAdapter::SetDeviceBatteryLevel(const RawAddress &device, int batteryLevel) const
{
    HILOGI("addr: %{public}s, batteryLevel: %{public}d", GetEncryptAddr(device.GetAddress()).c_str(), batteryLevel);

    std::lock_guard<std::recursive_mutex> lk(pimpl->syncMutex_);
    auto it = devices_.find(device.GetAddress());
    if (it != devices_.end()) {
        it->second->SetBatteryLevel(batteryLevel);
    }

    SendRemoteBatteryLevelChanged(device, batteryLevel);
}

void ClassicAdapter::SendRemoteBatteryLevelChanged(const RawAddress &device, int batteryLevel) const
{
    HILOGI("addr: %{public}s, batteryLevel: %{public}d", GetEncryptAddr(device.GetAddress()).c_str(), batteryLevel);

    pimpl->remoteObservers_.ForEach([device, batteryLevel](IClassicRemoteDeviceObserver &observer) {
        observer.OnRemoteBatteryLevelChanged(device, batteryLevel);
    });
}

REGISTER_CLASS_CREATOR(ClassicAdapter);
}  // namespace bluetooth
}  // namespace OHOS