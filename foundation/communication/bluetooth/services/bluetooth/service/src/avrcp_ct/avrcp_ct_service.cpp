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

#include "avrcp_ct_service.h"
#include "adapter_config.h"
#include "avrcp_ct_internal.h"
#include "class_creator.h"
#include "profile_service_manager.h"

namespace OHOS {
namespace bluetooth {
AvrcpCtService::AvrcpCtService() : utility::Context(PROFILE_NAME_AVRCP_CT, "1.6.2")
{
    HILOGI("enter");
    /// Need to set the features of the AVRCP CT service.
    InitFeatures();
    using namespace std::placeholders;
    AvrcCtProfile::Observer observer = {
        std::bind(&AvrcpCtService::OnProfileDisabled, this, RET_NO_ERROR),
        std::bind(&AvrcpCtService::OnConnectionStateChanged, this, _1, _2),
        std::bind(&AvrcpCtService::OnButtonPressed, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnButtonReleased, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnGetCapabilities, this, _1, _2, _3, _4),
        std::bind(&AvrcpCtService::OnGetPlayerAppSettingAttribtues, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnGetPlayerAppSettingValues, this, _1, _2, _3, _4),
        std::bind(&AvrcpCtService::OnGetPlayerAppSettingCurrentValue, this, _1, _2, _3, _4),
        std::bind(&AvrcpCtService::OnSetPlayerAppSettingCurrentValue, this, _1, _2),
        std::bind(&AvrcpCtService::OnGetPlayerAppSettingAttributeText, this, _1, _2, _3, _4),
        std::bind(&AvrcpCtService::OnGetPlayerAppSettingValueText, this, _1, _2, _3, _4),
        std::bind(&AvrcpCtService::OnGetElementAttributes, this, _1, _2, _3, _4),
        std::bind(&AvrcpCtService::OnGetPlayStatus, this, _1, _2, _3, _4, _5),
        std::bind(&AvrcpCtService::OnSetAddressedPlayer, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnSetBrowsedPlayer, this, _1, _2, _3, _4, _5, _6),
        std::bind(&AvrcpCtService::OnChangePath, this, _1, _2, _3, _4),
        std::bind(&AvrcpCtService::OnGetFolderItems, this, _1, _2, _3, _4, _5, _6, _7),
        std::bind(&AvrcpCtService::OnGetItemAttributes, this, _1, _2, _3, _4, _5),
        std::bind(&AvrcpCtService::OnGetTotalNumberOfItems, this, _1, _2, _3, _4, _5),
        std::bind(&AvrcpCtService::OnPlayItem, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnAddToNowPlaying, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnSetAbsoluteVolume, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnPlaybackStatusChanged, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnTrackChanged, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnTrackReachedEnd, this, _1, _2),
        std::bind(&AvrcpCtService::OnTrackReachedStart, this, _1, _2),
        std::bind(&AvrcpCtService::OnPlaybackPosChanged, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnPlayerApplicationSettingChanged, this, _1, _2, _3, _4),
        std::bind(&AvrcpCtService::OnNowPlayingContentChanged, this, _1, _2),
        std::bind(&AvrcpCtService::OnAvailablePlayersChanged, this, _1, _2),
        std::bind(&AvrcpCtService::OnAddressedPlayerChanged, this, _1, _2, _3, _4),
        std::bind(&AvrcpCtService::OnUidChanged, this, _1, _2, _3),
        std::bind(&AvrcpCtService::OnVolumeChanged, this, _1, _2, _3),
    };
    pfObserver_ = std::make_unique<AvrcCtProfile::Observer>(observer);
}

AvrcpCtService::~AvrcpCtService()
{
    HILOGI("enter");

    SetServiceState(AVRC_CT_SERVICE_STATE_DISABLED);

    myObserver_ = nullptr;
}

utility::Context *AvrcpCtService::GetContext()
{
    HILOGI("enter");

    return this;
}

void AvrcpCtService::InitFeatures()
{
    HILOGI("enter");

    features_ |= AVRC_CT_FEATURE_CATEGORY_1;
    features_ |= AVRC_CT_FEATURE_CATEGORY_2;
    features_ |= AVRC_CT_FEATURE_BROWSING;
    features_ |= AVRC_CT_FEATURE_KEY_OPERATION;
    features_ |= AVRC_CT_FEATURE_ABSOLUTE_VOLUME;
    features_ |= AVRC_CT_FEATURE_NOTIFY_PLAYBACK_STATUS_CHANGED;
    features_ |= AVRC_CT_FEATURE_NOTIFY_TRACK_REACHED_END;
    features_ |= AVRC_CT_FEATURE_NOTIFY_TRACK_REACHED_START;
    features_ |= AVRC_CT_FEATURE_NOTIFY_PLAYBACK_POSITION_CHANGED;
    features_ |= AVRC_CT_FEATURE_NOTIFY_PLAYER_SETTING_CHANGED;
    features_ |= AVRC_CT_FEATURE_NOTIFY_NOW_PLAYING_CONTENT_CHANGED;
    features_ |= AVRC_CT_FEATURE_NOTIFY_UIDS_CHANGED;
    features_ |= AVRC_CT_FEATURE_NOTIFY_ABSOLUTE_VOLUME_CHANGED;
}

/******************************************************************
 * REGISTER / UNREGISTER OBSERVER                                 *
 ******************************************************************/

void AvrcpCtService::RegisterObserver(IObserver *observer)
{
    HILOGI("enter");

    std::lock_guard<std::mutex> lock(mutex_);

    myObserver_ = observer;
}

void AvrcpCtService::UnregisterObserver(void)
{
    HILOGI("enter");

    std::lock_guard<std::mutex> lock(mutex_);

    myObserver_ = nullptr;
}

/******************************************************************
 * ENABLE / DISABLE                                               *
 ******************************************************************/

void AvrcpCtService::Enable(void)
{
    HILOGI("enter");

    if (IsDisabled()) {
        SetServiceState(AVRC_CT_SERVICE_STATE_ENABLING);

        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::EnableNative, this));
    } else {
        LOG_ERROR("Is not disabled!");
    }
}

void AvrcpCtService::Disable(void)
{
    HILOGI("enter");

    if (IsEnabled()) {
        SetServiceState(AVRC_CT_SERVICE_STATE_DISABLING);
        if (profile_ != nullptr) {
            profile_->SetEnableFlag(false);
        }

        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::DisableNative, this));
    } else {
        LOG_ERROR("Is not enable!");
    }
}

void AvrcpCtService::EnableNative(void)
{
    HILOGI("enter");

    int result = RET_BAD_STATUS;

    IAdapterConfig *config = AdapterConfig::GetInstance();
    config->GetValue(SECTION_AVRCP_CT_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, maxConnection_);

    do {
        result = RegisterSecurity();
        if (result != RET_NO_ERROR) {
            break;
        }
        result = RegisterService();
        if (result != RET_NO_ERROR) {
            break;
        }
        result = EnableProfile();
        if (result != RET_NO_ERROR) {
            break;
        }
    } while (false);

    if (result == RET_NO_ERROR) {
        SetServiceState(AVRC_CT_SERVICE_STATE_ENABLED);
    } else {
        SetServiceState(AVRC_CT_SERVICE_STATE_DISABLED);
    }
    GetContext()->OnEnable(PROFILE_NAME_AVRCP_CT, IsEnabled());
    HILOGI("AvrcpCtService:: AVRCP is ENABLED");
}

void AvrcpCtService::DisableNative(void)
{
    HILOGI("enter");

    if (DisableProfile() != RET_NO_ERROR) {
        OnProfileDisabled(RET_BAD_STATUS);
    }
}

int AvrcpCtService::EnableProfile(void)
{
    HILOGI("enter");

    /// Gets the size of the MTU.
    int controlMtu = AVRC_CT_DEFAULT_CONTROL_MTU_SIZE;
    int browseMtu = AVRC_CT_DEFAULT_BROWSE_MTU_SIZE;
    bool isTgEnabled = false;

    IAdapterConfig *config = AdapterConfig::GetInstance();
    config->GetValue(SECTION_AVRCP_CT_SERVICE, PROPERTY_CONTROL_MTU, controlMtu);
    config->GetValue(SECTION_AVRCP_CT_SERVICE, PROPERTY_BROWSE_MTU, browseMtu);
    config->GetValue(SECTION_CLASSIC_ADAPTER, PROPERTY_AVRCP_TG_SERVICE, isTgEnabled);

    profile_ = std::make_unique<AvrcCtProfile>(features_,
        AVRC_CT_DEFAULT_BLUETOOTH_SIG_COMPANY_ID,
        controlMtu,
        browseMtu,
        AVRC_CT_DEFAULT_MAX_FRAGMENTS,
        GetDispatcher(),
        ChannelEventCallback,
        ChannelMessageCallback);
    profile_->RegisterObserver(pfObserver_.get());

    return profile_->Enable(isTgEnabled);
}

int AvrcpCtService::DisableProfile(void) const
{
    HILOGI("enter");

    return profile_->Disable();
}

void AvrcpCtService::OnProfileDisabled(int result)
{
    HILOGI("result: %{public}d", result);

    SetServiceState(AVRC_CT_SERVICE_STATE_DISABLED);

    profile_->UnregisterObserver();
    profile_ = nullptr;

    result |= UnregisterService();
    result |= UnregisterSecurity();

    GetContext()->OnDisable(PROFILE_NAME_AVRCP_CT, result == RET_NO_ERROR);
}

int AvrcpCtService::RegisterSecurity(void)
{
    HILOGI("enter");

    gapManager_ = std::make_unique<AvrcCtGapManager>();

    return gapManager_->RegisterSecurity();
}

int AvrcpCtService::UnregisterSecurity(void)
{
    HILOGI("enter");

    int result = gapManager_->UnregisterSecurity();
    gapManager_ = nullptr;

    return result;
}

int AvrcpCtService::RegisterService(void)
{
    HILOGI("enter");

    sdpManager_ = std::make_unique<AvrcCtSdpManager>(features_ & AVRC_CT_SDP_ALL_SUPPORTED_FEATURES);

    return sdpManager_->RegisterService();
}

int AvrcpCtService::UnregisterService(void)
{
    HILOGI("enter");

    int result = sdpManager_->UnregisterService();
    sdpManager_ = nullptr;

    return result;
}

bool AvrcpCtService::IsEnabled(void)
{
    HILOGI("enter");

    return (state_ == AVRC_CT_SERVICE_STATE_ENABLED);
}

bool AvrcpCtService::IsDisabled(void)
{
    HILOGI("enter");

    return (state_ == AVRC_CT_SERVICE_STATE_DISABLED);
}

void AvrcpCtService::SetServiceState(uint8_t state)
{
    HILOGI("state: %{public}d", state);

    state_ = state;
}

/******************************************************************
 * CONNECTION                                                     *
 ******************************************************************/

std::vector<RawAddress> AvrcpCtService::GetConnectedDevices(void)
{
    HILOGI("enter");

    std::vector<RawAddress> result;

    if (IsEnabled()) {
        result = profile_->GetConnectedDevices();
    }

    return result;
}

std::vector<bluetooth::RawAddress> AvrcpCtService::GetDevicesByStates(const std::vector<int> &states)
{
    HILOGI("enter");

    std::vector<bluetooth::RawAddress> result;

    if (IsEnabled()) {
        result = profile_->GetDevicesByStates(states);
    }

    return result;
}

int AvrcpCtService::GetMaxConnectNum(void)
{
    HILOGI("enter");

    int result = 0;

    if (IsEnabled()) {
        result = profile_->GetMaxConnectNum();
    }

    HILOGI("result: %{public}d", result);
    return result;
}

int AvrcpCtService::GetDeviceState(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = static_cast<int>(BTConnectState::DISCONNECTED);

    if (IsEnabled()) {
        result = profile_->GetDeviceState(rawAddr);
    }

    HILOGI("result: %{public}d", result);
    return result;
}

int AvrcpCtService::Connect(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::DISCONNECTED)) {
            break;
        }

        if (!CheckConnectionNum()) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::ConnectNative, this, peerAddr));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::ConnectNative(RawAddress rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AcceptActiveConnect(rawAddr);
}

int AvrcpCtService::Disconnect(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::DisconnectNative, this, peerAddr));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::DisconnectNative(RawAddress rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->Disconnect(rawAddr) != RET_NO_ERROR) {
            HILOGI("Disconnect Failed! Addr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
        }
    } while (false);
}

int AvrcpCtService::GetConnectState(void)
{
    HILOGI("enter");

    int result = PROFILE_STATE_DISCONNECTED;

    if (IsEnabled()) {
        result = profile_->GetConnectState();
    }

    return result;
}

void AvrcpCtService::OnConnectionStateChanged(const RawAddress &rawAddr, int state) const
{
    HILOGI("Address: %{public}s, state: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), state);

    if (myObserver_ != nullptr) {
        myObserver_->OnConnectionStateChanged(rawAddr, state);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::AcceptActiveConnect(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::DISCONNECTED)) {
            break;
        }

        if (profile_->Connect(rawAddr) != RET_NO_ERROR) {
            HILOGI("Connect Failed! Addr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
        }
    } while (false);
}

void AvrcpCtService::RejectActiveConnect(const RawAddress &rawAddr) const
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

int AvrcpCtService::FindTgService(const RawAddress &rawAddr) const
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    return sdpManager_->FindTgService(rawAddr, FindTgServiceCallback);
}

void AvrcpCtService::FindTgServiceCallback(
    const BtAddr *btAddr, const uint32_t *handleArray, uint16_t handleCount, void *context)
{
    HILOGI("handleCount: %{public}d", handleCount);

    auto servManager = IProfileManager::GetInstance();
    auto service = static_cast<AvrcpCtService *>(servManager->GetProfileService(PROFILE_NAME_AVRCP_CT));
    RawAddress rawAddr(RawAddress::ConvertToString(btAddr->addr));
    if (service != nullptr) {
        if (handleCount > 0) {
            service->GetDispatcher()->PostTask(std::bind(&AvrcpCtService::AcceptActiveConnect, service, rawAddr));
        } else {
            service->GetDispatcher()->PostTask(std::bind(&AvrcpCtService::RejectActiveConnect, service, rawAddr));
        }
    }
}

/******************************************************************
 * BUTTON OPERATION                                               *
 ******************************************************************/

int AvrcpCtService::PressButton(const RawAddress &rawAddr, uint8_t button)
{
    HILOGI("address: %{public}s, button: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsPassQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::PressButtonNative, this, peerAddr, button));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::PressButtonNative(RawAddress rawAddr, uint8_t button)
{
    HILOGI("address: %{public}s, button: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendPressButtonCmd(rawAddr, button);
    } while (false);
}

void AvrcpCtService::OnButtonPressed(const RawAddress &rawAddr, uint8_t button, int result) const
{
    HILOGI("Address: %{public}s, button: %{public}d, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr),
        button, result);

    if (myObserver_ != nullptr) {
        myObserver_->OnPressButton(rawAddr, button, result);
    } else {
        HILOGI("The observer[onPressButton] is not registered!");
    }
}

int AvrcpCtService::ReleaseButton(const RawAddress &rawAddr, uint8_t button)
{
    HILOGI("address: %{public}s, button: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsPassQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::ReleaseButtonNative, this, peerAddr, button));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::ReleaseButtonNative(RawAddress rawAddr, uint8_t button)
{
    HILOGI("address: %{public}s, button: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendReleaseButtonCmd(rawAddr, button);
    } while (false);
}

void AvrcpCtService::OnButtonReleased(const RawAddress &rawAddr, uint8_t button, int result) const
{
    HILOGI("Address: %{public}s, button: %{public}d, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr),
        button, result);

    if (myObserver_ != nullptr) {
        myObserver_->OnReleaseButton(rawAddr, button, result);
    } else {
        HILOGI("The observer[onReleaseButton] is not registered!");
    }
}

/******************************************************************
 * UNIT INFO / SUB UNIT INFO                                      *
 ******************************************************************/

int AvrcpCtService::GetUnitInfo(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::UnitInfoNative, this, peerAddr));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::UnitInfoNative(RawAddress rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendUnitCmd(rawAddr);
    } while (false);
}

int AvrcpCtService::GetSubUnitInfo(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::SubUnitInfoNative, this, peerAddr));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::SubUnitInfoNative(RawAddress rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendSubUnitCmd(rawAddr);
    } while (false);
}

/******************************************************************
 * Media Player Selection                                         *
 ******************************************************************/

int AvrcpCtService::SetAddressedPlayer(const RawAddress &rawAddr, uint16_t playerId)
{
    HILOGI("address: %{public}s, playerId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::SetAddressedPlayerNative, this, peerAddr, playerId));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::SetAddressedPlayerNative(RawAddress rawAddr, uint16_t playerId)
{
    HILOGI("address: %{public}s, playerId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendSetAddressedPlayerCmd(rawAddr, playerId);
    } while (false);
}

void AvrcpCtService::OnSetAddressedPlayer(const RawAddress &rawAddr, int result, int detail) const
{
    HILOGI("address: %{public}s, result: %{public}d, detail: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), result, detail);

    if (myObserver_ != nullptr) {
        myObserver_->OnSetAddressedPlayer(rawAddr, result, detail);
        HILOGI("enter");
    } else {
        HILOGI("The observer is not registered!");
    }
}

int AvrcpCtService::SetBrowsedPlayer(const RawAddress &rawAddr, uint16_t playerId)
{
    HILOGI("address: %{public}s, playerId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        if (!profile_->IsBrowsingConnected(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::SetBrowsedPlayerNative, this, peerAddr, playerId));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::SetBrowsedPlayerNative(RawAddress rawAddr, uint16_t playerId)
{
    HILOGI("address: %{public}s, playerId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendSetBrowsedPlayerCmd(rawAddr, playerId);
    } while (false);
}

void AvrcpCtService::OnSetBrowsedPlayer(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems,
    const std::vector<std::string> &folderNames, int result, int detail) const
{
    HILOGI("addr: %{public}s, uidCounter: %{public}hu, numOfItems: %{public}d, folderNames.size: %{public}zu, "
        "result: %{public}d, detail: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, numOfItems,
        folderNames.size(), result, detail);

    if (myObserver_ != nullptr) {
        myObserver_->OnSetBrowsedPlayer(rawAddr, uidCounter, numOfItems, folderNames, result, detail);
        HILOGI("enter");
    } else {
        HILOGI("The observer is not registered!");
    }
}

/******************************************************************
 * Capabilities                                                   *
 ******************************************************************/

int AvrcpCtService::GetSupportedCompanies(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::GetCapabilitiesNative, this, peerAddr, AVRC_CAPABILITY_COMPANYID));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

int AvrcpCtService::GetSupportedEvents(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::GetCapabilitiesNative, this, peerAddr, AVRC_CAPABILITY_EVENTID));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetCapabilitiesNative(RawAddress rawAddr, uint8_t capabilityId)
{
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetCapabilitiesCmd(rawAddr, capabilityId);
    } while (false);
}

void AvrcpCtService::OnGetCapabilities(const RawAddress &rawAddr, const std::vector<uint32_t> &companies,
    const std::vector<uint8_t> &events, int result) const
{
    HILOGI("address: %{public}s, companies.size: %{public}zu, events.size: %{public}zu, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), companies.size(), events.size(), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnGetCapabilities(rawAddr, companies, events, result);
        HILOGI("enter");
    } else {
        HILOGI("The observer is not registered!");
    }
}
/******************************************************************
 * PLAYER APPLICATION SETTINGS                                    *
 ******************************************************************/

int AvrcpCtService::GetPlayerAppSettingAttributes(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::GetPlayerAppSettingAttributesNative, this, peerAddr));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetPlayerAppSettingAttributesNative(RawAddress rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendListPlayerApplicationSettingAttributesCmd(rawAddr);
    } while (false);
}

void AvrcpCtService::OnGetPlayerAppSettingAttribtues(
    const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, int result) const
{
    HILOGI("addr: %{public}s, attribute.size: %{public}zu, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), attributes.size(), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnGetPlayerAppSettingAttributes(rawAddr, attributes, result);
        HILOGI("enter");
    } else {
        HILOGI("The observer is not registered!");
    }
}

int AvrcpCtService::GetPlayerAppSettingValues(const RawAddress &rawAddr, uint8_t attribute)
{
    HILOGI("address: %{public}s, attribute: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), attribute);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::GetPlayerAppSettingValuesNative, this, peerAddr, attribute));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetPlayerAppSettingValuesNative(RawAddress rawAddr, uint8_t attribute)
{
    HILOGI("address: %{public}s, attribute: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), attribute);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendListPlayerApplicationSettingValuesCmd(rawAddr, attribute);
    } while (false);
}

void AvrcpCtService::OnGetPlayerAppSettingValues(
    const RawAddress &rawAddr, uint8_t attribute, const std::vector<uint8_t> &values, int result) const
{
    HILOGI("addr: %{public}s, attribute: %{public}d, values.size: %{public}zu, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), attribute, values.size(), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnGetPlayerAppSettingValues(rawAddr, attribute, values, result);
        HILOGI("enter");
    } else {
        HILOGI("The observer is not registered!");
    }
}

int AvrcpCtService::GetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::GetPlayerAppSettingCurrentValueNative, this, peerAddr, attributes));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetPlayerAppSettingCurrentValueNative(RawAddress rawAddr, std::vector<uint8_t> attributes)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetCurrentPlayerApplicationSettingValueCmd(rawAddr, attributes);
    } while (false);
}

void AvrcpCtService::OnGetPlayerAppSettingCurrentValue(const RawAddress &rawAddr,
    const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values, int result) const
{
    HILOGI("addr: %{public}s, attributes.size: %{public}zu, values.size: %{public}zu, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), attributes.size(), values.size(), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnGetPlayerAppSettingCurrentValue(rawAddr, attributes, values, result);
        HILOGI("enter");
    } else {
        HILOGI("The observer is not registered!");
    }
}

int AvrcpCtService::SetPlayerAppSettingCurrentValue(
    const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::SetPlayerAppSettingCurrentValueNative, this, peerAddr, attributes, values));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::SetPlayerAppSettingCurrentValueNative(
    RawAddress rawAddr, std::vector<uint8_t> attributes, std::vector<uint8_t> values)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendSetPlayerApplicationSettingValueCmd(rawAddr, attributes, values);
    } while (false);
}

void AvrcpCtService::OnSetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, int result) const
{
    HILOGI("address: %{public}s, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnSetPlayerAppSettingCurrentValue(rawAddr, result);
        HILOGI("enter");
    } else {
        HILOGI("The observer is not registered!");
    }
}

int AvrcpCtService::GetPlayerAppSettingAttributeText(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::GetPlayerAppSettingAttributeTextNative, this, peerAddr, attributes));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetPlayerAppSettingAttributeTextNative(RawAddress rawAddr, std::vector<uint8_t> attributes)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetPlayerApplicationSettingAttributeTextCmd(rawAddr, attributes);
    } while (false);
}

void AvrcpCtService::OnGetPlayerAppSettingAttributeText(const RawAddress &rawAddr,
    const std::vector<uint8_t> &attributes, const std::vector<std::string> &attrStr, int result) const
{
    HILOGI("address: %{public}s, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnGetPlayerAppSettingAttributeText(rawAddr, attributes, attrStr, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

int AvrcpCtService::GetPlayerAppSettingValueText(
    const RawAddress &rawAddr, uint8_t attributeId, const std::vector<uint8_t> &values)
{
    HILOGI("address: %{public}s, attributeId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), attributeId);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::GetPlayerAppSettingValueTextNative, this, peerAddr, attributeId, values));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetPlayerAppSettingValueTextNative(
    RawAddress rawAddr, uint8_t attributeId, std::vector<uint8_t> values)
{
    HILOGI("address: %{public}s, attributeId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), attributeId);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetPlayerApplicationSettingValueTextCmd(rawAddr, attributeId, values);
    } while (false);
}

void AvrcpCtService::OnGetPlayerAppSettingValueText(const RawAddress &rawAddr, const std::vector<uint8_t> &values,
    const std::vector<std::string> &valueStr, int result) const
{
    HILOGI("address: %{public}s, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnGetPlayerAppSettingValueText(rawAddr, values, valueStr, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

/******************************************************************
 * MEDIA INFORMATION                                              *
 ******************************************************************/

int AvrcpCtService::GetElementAttributes(
    const RawAddress &rawAddr, uint64_t identifier, const std::vector<uint32_t> &attributes)
{
    HILOGI("address: %{public}s, identifier: %{public}llu",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), static_cast<unsigned long long>(identifier));

    int result = RET_BAD_STATUS;
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::GetElementAttributesNative, this, peerAddr, identifier, attributes));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetElementAttributesNative(
    RawAddress rawAddr, uint64_t identifier, std::vector<uint32_t> attributes)
{
    HILOGI("address: %{public}s, identifier: %{public}llu",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), static_cast<unsigned long long>(identifier));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetElementAttributesCmd(rawAddr, identifier, attributes);
    } while (false);
}

void AvrcpCtService::OnGetElementAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
    const std::vector<std::string> &values, int result) const
{
    HILOGI("address: %{public}s, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnGetElementAttributes(rawAddr, attributes, values, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

/******************************************************************
 * PLAY                                                           *
 ******************************************************************/

int AvrcpCtService::GetPlayStatus(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::GetPlayStatusNative, this, peerAddr));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetPlayStatusNative(RawAddress rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetPlayStatusCmd(rawAddr);
    } while (false);
}

void AvrcpCtService::OnGetPlayStatus(
    const RawAddress &rawAddr, uint32_t songLength, uint32_t songPosition, uint8_t playStatus, int result) const
{
    HILOGI("address: %{public}s, songLength: %{public}d, songPosition: %{public}d, playStatus: %{public}d, "
        "result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), songLength, songPosition, playStatus, result);

    if (myObserver_ != nullptr) {
        myObserver_->OnGetPlayStatus(rawAddr, songLength, songPosition, playStatus, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

int AvrcpCtService::PlayItem(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter)
{
    HILOGI("address: %{public}s, scope: %{public}d, uid: %{public}llu, uidCounter: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, static_cast<unsigned long long>(uid), uidCounter);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::PlayItemNative, this, peerAddr, scope, uid, uidCounter));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::PlayItemNative(RawAddress rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter)
{
    HILOGI("address: %{public}s, scope: %{public}d, uid: %{public}llu, uidCounter: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, static_cast<unsigned long long>(uid), uidCounter);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendPlayItemCmd(rawAddr, scope, uid, uidCounter);
    } while (false);
};

void AvrcpCtService::OnPlayItem(const RawAddress &rawAddr, int result, int detail) const
{
    HILOGI("address: %{public}s, result: %{public}d, detail: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), result, detail);
    if (myObserver_ != nullptr) {
        myObserver_->OnPlayItem(rawAddr, result, detail);
    } else {
        HILOGI("The observer is not registered!");
    }
};

int AvrcpCtService::AddToNowPlaying(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter)
{
    HILOGI("address: %{public}s, scope: %{public}d, uid: %{public}llu, uidCounter: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, static_cast<unsigned long long>(uid), uidCounter);
    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::AddToNowPlayingNative, this, peerAddr, scope, uid, uidCounter));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::AddToNowPlayingNative(RawAddress rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter)
{
    HILOGI("address: %{public}s, scope: %{public}d, uid: %{public}llu, uidCounter: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, static_cast<unsigned long long>(uid), uidCounter);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendAddToNowPlayingCmd(rawAddr, scope, uid, uidCounter);
    } while (false);
}

void AvrcpCtService::OnAddToNowPlaying(const RawAddress &rawAddr, int result, int detail) const
{
    HILOGI("address: %{public}s, result: %{public}d, detail: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), result, detail);

    if (myObserver_ != nullptr) {
        myObserver_->OnAddToNowPlaying(rawAddr, result, detail);
    } else {
        HILOGI("The observer is not registered!");
    }
}

/******************************************************************
 * CONTINUING RESPONSE / ABORT CONTINUING RESPONSE                *
 ******************************************************************/

int AvrcpCtService::RequestContinuingResponse(const RawAddress &rawAddr, uint8_t pduId)
{
    HILOGI("address: %{public}s, pduId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), pduId);
    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::RequestContinuingResponseNative, this, peerAddr, pduId));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::RequestContinuingResponseNative(RawAddress rawAddr, uint8_t pduId)
{
    HILOGI("address: %{public}s, pduId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), pduId);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendRequestContinuingResponseCmd(rawAddr, pduId);
    } while (false);
}

int AvrcpCtService::AbortContinuingResponse(const RawAddress &rawAddr, uint8_t pduId)
{
    HILOGI("address: %{public}s, pduId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), pduId);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::AbortContinuingResponseNative, this, peerAddr, pduId));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::AbortContinuingResponseNative(RawAddress rawAddr, uint8_t pduId)
{
    HILOGI("address: %{public}s, pduId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), pduId);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendAbortContinuingResponseCmd(rawAddr, pduId);
    } while (false);
}

/******************************************************************
 * OPERATE THE VIRTUAL FILE SYSTEM                                *
 ******************************************************************/

int AvrcpCtService::ChangePath(const RawAddress &rawAddr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid)
{
    HILOGI("address: %{public}s, uidCounter: %{public}d, direction: %{public}d, folderUid: %{public}llu",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, direction, static_cast<unsigned long long>(folderUid));
    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsBrowseQueueFull(rawAddr)) {
            break;
        }

        if (!profile_->IsBrowsingConnected(rawAddr)) {
            break;
        }
        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::ChangePathNative, this, peerAddr, uidCounter, direction, folderUid));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::ChangePathNative(RawAddress rawAddr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid)
{
    HILOGI("address: %{public}s, uidCounter: %{public}d, direction: %{public}d, folderUid: %{public}llu",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, direction, static_cast<unsigned long long>(folderUid));
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendChangePathCmd(rawAddr, uidCounter, direction, folderUid);
    } while (false);
}

void AvrcpCtService::OnChangePath(const RawAddress &rawAddr, uint32_t numOfItems, int result, int detail) const
{
    HILOGI("address: %{public}s, numOfItems: %{public}u, result: %{public}d, detail: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), numOfItems, result, detail);

    if (myObserver_ != nullptr) {
        myObserver_->OnChangePath(rawAddr, numOfItems, result, detail);
    } else {
        HILOGI("The observer is not registered!");
    }
}

int AvrcpCtService::GetFolderItems(const RawAddress &rawAddr, uint8_t scope, uint32_t startItem, uint32_t endItem,
    const std::vector<uint32_t> &attributes)
{
    HILOGI("address: %{public}s, scope: %{public}d, startItem: %{public}u, endItem: %{public}u",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, startItem, endItem);
    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsBrowseQueueFull(rawAddr)) {
            break;
        }

        if (!profile_->IsBrowsingConnected(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::GetFolderItemsNative, this, peerAddr, scope, startItem, endItem, attributes));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetFolderItemsNative(
    RawAddress rawAddr, uint8_t scope, uint32_t startItem, uint32_t endItem, std::vector<uint32_t> attributes)
{
    HILOGI("address: %{public}s, scope: %{public}d, startItem: %{public}u, endItem: %{public}u",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, startItem, endItem);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetFolderItemsCmd(rawAddr, scope, startItem, endItem, attributes);
    } while (false);
}

void AvrcpCtService::OnGetFolderItems(const RawAddress &rawAddr, uint8_t scope, uint16_t uidCounter,
    const std::vector<AvrcMpItem> &mpItems, const std::vector<AvrcMeItem> &meItems, int result, int detail) const
{
    HILOGI("address: %{public}s, scope: %{public}d, uidCounter: %{public}d, result: %{public}d, detail: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, uidCounter, result, detail);

    if (myObserver_ != nullptr) {
        if (scope == AVRC_MEDIA_SCOPE_PLAYER_LIST) {
            myObserver_->OnGetMediaPlayers(rawAddr, uidCounter, mpItems, result, detail);
        } else {
            myObserver_->OnGetFolderItems(rawAddr, uidCounter, meItems, result, detail);
        }
    } else {
        HILOGI("The observer is not registered!");
    }
}

int AvrcpCtService::GetItemAttributes(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter,
    const std::vector<uint32_t> &attributes)
{
    HILOGI("address: %{public}s, scope: %{public}d, uid: %{public}llu, uidCounter: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, static_cast<unsigned long long>(uid), uidCounter);
    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsBrowseQueueFull(rawAddr)) {
            break;
        }

        if (!profile_->IsBrowsingConnected(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::GetItemAttributesNative, this, peerAddr, scope, uid, uidCounter, attributes));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetItemAttributesNative(
    RawAddress rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter, std::vector<uint32_t> attributes)
{
    HILOGI("address: %{public}s, scope: %{public}d, uid: %{public}llu, uidCounter: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, static_cast<unsigned long long>(uid), uidCounter);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetItemAttributesCmd(rawAddr, scope, uid, uidCounter, attributes);
    } while (false);
}

void AvrcpCtService::OnGetItemAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
    const std::vector<std::string> &values, int result, int detail) const
{
    HILOGI("addr:%{public}s, result:%{public}d, detail:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result, detail);

    if (myObserver_ != nullptr) {
        myObserver_->OnGetItemAttributes(rawAddr, attributes, values, result, detail);
    } else {
        HILOGI("The observer is not registered!");
    }
}

int AvrcpCtService::GetTotalNumberOfItems(const RawAddress &rawAddr, uint8_t scope)
{
    HILOGI("addr:%{public}s, scope:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsBrowseQueueFull(rawAddr)) {
            break;
        }

        if (!profile_->IsBrowsingConnected(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::GetTotalNumberOfItemsNative, this, peerAddr, scope));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::GetTotalNumberOfItemsNative(RawAddress rawAddr, uint8_t scope)
{
    HILOGI("addr:%{public}s, scope:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetTotalNumberOfItemsCmd(rawAddr, scope);
    } while (false);
}

void AvrcpCtService::OnGetTotalNumberOfItems(
    const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems, int result, int detail) const
{
    HILOGI("addr:%{public}s, uidCounter:%{public}d, numOfItems:%{public}u, result:%{public}d, detail:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, numOfItems, result, detail);

    if (myObserver_ != nullptr) {
        myObserver_->OnGetTotalNumberOfItems(rawAddr, uidCounter, numOfItems, result, detail);
    } else {
        HILOGI("The observer is not registered!");
    }
}

/******************************************************************
 * ABSOLUTE VOLUME                                                *
 ******************************************************************/

int AvrcpCtService::SetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume)
{
    HILOGI("addr: %{public}s, volume: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        if (profile_->IsDisableAbsoluteVolume(rawAddr)) {
            result = RET_NO_SUPPORT;
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::SetAbsoluteVolumeNative, this, peerAddr, volume));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::SetAbsoluteVolumeNative(RawAddress rawAddr, uint8_t volume)
{
    HILOGI("addr: %{public}s, volume: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendSetAbsoluteVolumeCmd(rawAddr, volume);
    } while (false);
}

void AvrcpCtService::OnSetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, int result) const
{
    HILOGI("addr:%{public}s, volume:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume, result);

    if (myObserver_ != nullptr) {
        myObserver_->OnSetAbsoluteVolume(rawAddr, volume, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

/******************************************************************
 * NOTIFICATION                                                   *
 ******************************************************************/

int AvrcpCtService::EnableNotification(const RawAddress &rawAddr, const std::vector<uint8_t> &events, uint8_t interval)
{
    HILOGI("addr: %{public}s, interval: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), interval);

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::EnableNotificationNative, this, peerAddr, events, interval));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

int AvrcpCtService::DisableNotification(const RawAddress &rawAddr, const std::vector<uint8_t> &events)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->IsVendorQueueFull(rawAddr)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpCtService::DisableNotificationNative, this, peerAddr, events));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpCtService::EnableNotificationNative(RawAddress rawAddr, std::vector<uint8_t> events, uint8_t interval)
{
    HILOGI("addr: %{public}s, interval: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), interval);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->EnableNotification(rawAddr, events, interval);
    } while (false);
}

void AvrcpCtService::DisableNotificationNative(RawAddress rawAddr, std::vector<uint8_t> events)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->DisableNotification(rawAddr, events);
    } while (false);
}

void AvrcpCtService::OnPlaybackStatusChanged(const RawAddress &rawAddr, uint8_t playStatus, int result) const
{
    HILOGI("addr: %{public}s, playStatus: %{public}d, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), playStatus, result);

    if (myObserver_ != nullptr) {
        myObserver_->OnPlaybackStatusChanged(rawAddr, playStatus, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::OnTrackChanged(const RawAddress &rawAddr, uint64_t uid, int result) const
{
    HILOGI("addr: %{public}s, uid: %{public}jx, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), uid, result);

    if (myObserver_ != nullptr) {
        myObserver_->OnTrackChanged(rawAddr, uid, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::OnTrackReachedEnd(const RawAddress &rawAddr, int result) const
{
    HILOGI("addr: %{public}s, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnTrackReachedEnd(rawAddr, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::OnTrackReachedStart(const RawAddress &rawAddr, int result) const
{
    HILOGI("addr: %{public}s, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnTrackReachedStart(rawAddr, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::OnPlaybackPosChanged(const RawAddress &rawAddr, uint32_t playbackPos, int result) const
{
    HILOGI("addr: %{public}s, playbackPos: %{public}d, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), playbackPos, result);

    if (myObserver_ != nullptr) {
        myObserver_->OnPlaybackPosChanged(rawAddr, playbackPos, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::OnPlayerApplicationSettingChanged(const RawAddress &rawAddr,
    const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values, int result) const
{
    HILOGI("addr: %{public}s, attributes.size: %{public}zu, values.size: %{public}zu, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), attributes.size(), values.size(), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnPlayerAppSettingChanged(rawAddr, attributes, values, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::OnNowPlayingContentChanged(const RawAddress &rawAddr, int result) const
{
    HILOGI("addr: %{public}s, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnNowPlayingContentChanged(rawAddr, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::OnAvailablePlayersChanged(const RawAddress &rawAddr, int result) const
{
    HILOGI("addr: %{public}s, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result);

    if (myObserver_ != nullptr) {
        myObserver_->OnAvailablePlayersChanged(rawAddr, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::OnAddressedPlayerChanged(
    const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, int result) const
{
    HILOGI("addr: %{public}s, playerId: %{public}d, uidCounter: %{public}d, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId, uidCounter, result);

    if (myObserver_ != nullptr) {
        myObserver_->OnAddressedPlayerChanged(rawAddr, playerId, uidCounter, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::OnUidChanged(const RawAddress &rawAddr, uint16_t uidCounter, int result) const
{
    HILOGI("addr: %{public}s, uidCounter: %{public}d, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, result);

    if (myObserver_ != nullptr) {
        myObserver_->OnUidChanged(rawAddr, uidCounter, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::OnVolumeChanged(const RawAddress &rawAddr, uint8_t volume, int result) const
{
    HILOGI("addr: %{public}s, volume: %{public}d, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume, result);

    if (myObserver_ != nullptr) {
        myObserver_->OnVolumeChanged(rawAddr, volume, result);
    } else {
        HILOGI("The observer is not registered!");
    }
}

void AvrcpCtService::ProcessChannelEvent(
    RawAddress rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context)
{
    HILOGI("addr: %{public}s, connectId: %{public}d, event: %{public}d, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), connectId, event, result);
    if (!IsDisabled()) {
        profile_->ProcessChannelEvent(rawAddr, connectId, event, result, context);
    }
}

void AvrcpCtService::ProcessChannelMessage(
    uint8_t connectId, uint8_t label, uint8_t crType, uint8_t chType, Packet *pkt, void *context)
{
    HILOGI("connectId: %{public}d, label: %{public}d, crType: %{public}d, chType: %{public}d",
        connectId, label, crType, chType);

    if (!IsDisabled()) {
        profile_->ProcessChannelMessage(connectId, label, crType, chType, pkt, context);
    }
}

void AvrcpCtService::ChannelEventCallback(
    uint8_t connectId, uint8_t event, uint16_t result, const BtAddr *btAddr, void *context)
{
    HILOGI("connectId: %{public}d, event: %{public}d, result: %{public}d", connectId, event, result);
    auto servManager = IProfileManager::GetInstance();
    auto service = static_cast<AvrcpCtService *>(servManager->GetProfileService(PROFILE_NAME_AVRCP_CT));
    RawAddress rawAddr(RawAddress::ConvertToString(btAddr->addr));

    if (service != nullptr) {
        switch (event) {
            case AVCT_CONNECT_IND_EVT:
            case AVCT_CONNECT_CFM_EVT:
                if (result != RET_NO_ERROR) {
                    service->DecConnectionNum();
                }
                break;
            case AVCT_DISCONNECT_IND_EVT:
            case AVCT_DISCONNECT_CFM_EVT:
                service->DecConnectionNum();
                break;
            default:
                break;
        }
        service->GetDispatcher()->PostTask(
            std::bind(&AvrcpCtService::ProcessChannelEvent, service, rawAddr, connectId, event, result, context));
    }
}

void AvrcpCtService::ChannelMessageCallback(
    uint8_t connectId, uint8_t label, uint8_t crType, uint8_t chType, Packet *pkt, void *context)
{
    HILOGI("connectId: %{public}d, label: %{public}d, crType: %{public}d, chType: %{public}d",
        connectId, label, crType, chType);

    auto servManager = IProfileManager::GetInstance();
    auto service = static_cast<AvrcpCtService *>(servManager->GetProfileService(PROFILE_NAME_AVRCP_CT));
    auto myPkt = PacketRefMalloc(pkt);

    if (service != nullptr) {
        service->GetDispatcher()->PostTask(std::bind(
            &AvrcpCtService::ProcessChannelMessage, service, connectId, label, crType, chType, myPkt, context));
    }
}

bool AvrcpCtService::CheckConnectionNum()
{
    if (++currentConn_ > maxConnection_) {
        currentConn_ = maxConnection_;
        return false;
    } else {
        return true;
    }
}

REGISTER_CLASS_CREATOR(AvrcpCtService);
}  // namespace bluetooth
}  // namespace OHOS
