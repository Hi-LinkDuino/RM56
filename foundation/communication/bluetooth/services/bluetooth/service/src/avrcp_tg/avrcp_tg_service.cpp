/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

/**
 * @file avrcp_tg_service.cpp
 *
 * @brief Defines the class of the AVRCP TG service, including attributes and methods.
 */

#include "avrcp_tg_service.h"

#include <functional>
#include "adapter_config.h"
#include "class_creator.h"
#include "packet.h"
#include "profile_service_manager.h"
#include "stub/media_service.h"
#include "timer.h"
#include "log_util.h"

namespace OHOS {
namespace bluetooth {
void AvrcpTgService::ObserverImpl::OnConnectionStateChanged(const std::string &addr, int state)
{
    HILOGI("addr: %{public}s, state: %{public}d", GetEncryptAddr(addr).c_str(), state);
}

void AvrcpTgService::ObserverImpl::OnPressButton(const std::string &addr, uint8_t button)
{
    HILOGI("addr: %{public}s, button: %{public}x", GetEncryptAddr(addr).c_str(), button);
}

void AvrcpTgService::ObserverImpl::OnReleaseButton(const std::string &addr, uint8_t button)
{
    HILOGI("addr: %{public}s, button: %{public}x", GetEncryptAddr(addr).c_str(), button);
}

void AvrcpTgService::ObserverImpl::OnSetAddressedPlayer(const std::string &addr, uint8_t label, int status)
{
    HILOGI("addr: %{public}s, label: %{public}d, status: %{public}d", GetEncryptAddr(addr).c_str(), label, status);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnSetAddressedPlayer(rawAddr, label, status);
    }
}

void AvrcpTgService::ObserverImpl::OnSetBrowsedPlayer(const std::string &addr, uint16_t uidCounter, uint32_t numOfItems,
    const std::vector<std::string> &folderNames, uint8_t label, int status)
{
    HILOGI("addr: %{public}s, uidCounter: %{public}d, numOfItems: %{public}u, label: %{public}d, status: %{public}d",
        GetEncryptAddr(addr).c_str(), uidCounter, numOfItems, label, status);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnSetBrowsedPlayer(rawAddr, uidCounter, numOfItems, folderNames, label, status);
    }
}

void AvrcpTgService::ObserverImpl::OnGetPlayerAppSettingAttributes(
    const std::string &addr, const std::deque<uint8_t> &attributes, uint8_t label)
{
    HILOGI("addr: %{public}s, label: %{public}d", GetEncryptAddr(addr).c_str(), label);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetPlayerAppSettingAttributes(rawAddr, attributes, label);
    }
}

void AvrcpTgService::ObserverImpl::OnGetPlayerAppSettingValues(
    const std::string &addr, const std::deque<uint8_t> &values, uint8_t label)
{
    HILOGI("addr: %{public}s, label: %{public}d", GetEncryptAddr(addr).c_str(), label);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetPlayerAppSettingValues(rawAddr, values, label);
    }
}

void AvrcpTgService::ObserverImpl::OnGetPlayerAppSettingCurrentValue(const std::string &addr,
    const std::deque<uint8_t> &attributes, const std::deque<uint8_t> &values, uint8_t label, uint8_t context)
{
    HILOGI("addr: %{public}s, label: %{public}d, context: %{public}d", GetEncryptAddr(addr).c_str(), label, context);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetPlayerAppSettingCurrentValue(rawAddr, attributes, values, label, context);
    }
}

void AvrcpTgService::ObserverImpl::OnSetPlayerAppSettingCurrentValue(const std::string &addr, uint8_t label)
{
    HILOGI("addr: %{public}s, label: %{public}d", GetEncryptAddr(addr).c_str(), label);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnSetPlayerAppSettingCurrentValue(rawAddr, label);
    }
}

void AvrcpTgService::ObserverImpl::OnGetPlayerAppSettingAttributeText(const std::string &addr,
    const std::vector<uint8_t> &attributes, const std::vector<std::string> &attrStr, uint8_t label)
{
    HILOGI("addr: %{public}s, label: %{public}d", GetEncryptAddr(addr).c_str(), label);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetPlayerAppSettingAttributeText(rawAddr, attributes, attrStr, label);
    }
}

void AvrcpTgService::ObserverImpl::OnGetPlayerAppSettingValueText(const std::string &addr,
    const std::vector<uint8_t> &values, const std::vector<std::string> &valueStr, uint8_t label)
{
    HILOGI("addr: %{public}s, label: %{public}d", GetEncryptAddr(addr).c_str(), label);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetPlayerAppSettingValueText(rawAddr, values, valueStr, label);
    }
}

void AvrcpTgService::ObserverImpl::OnGetElementAttributes(const std::string &addr,
    const std::vector<uint32_t> &attribtues, const std::vector<std::string> &values, uint8_t label)
{
    HILOGI("addr: %{public}s, label: %{public}d", GetEncryptAddr(addr).c_str(), label);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetElementAttributes(rawAddr, attribtues, values, label);
    }
}

void AvrcpTgService::ObserverImpl::OnGetPlayStatus(const std::string &addr, uint32_t songLength, uint32_t songPosition,
    uint8_t playStatus, uint8_t label, uint8_t context)
{
    HILOGI("addr:%{public}s, songLength:%{public}u, songPosition:%{public}u, playStatus:%{public}d, label:%{public}d,"
        " context:%{public}d", GetEncryptAddr(addr).c_str(), songLength, songPosition, playStatus, label, context);
    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetPlayStatus(rawAddr, songLength, songPosition, playStatus, label, context);
    }
}

void AvrcpTgService::ObserverImpl::OnPlayItem(const std::string &addr, uint8_t label, int status)
{
    HILOGI("addr: %{public}s, label: %{public}d, status: %{public}d", GetEncryptAddr(addr).c_str(), label, status);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnPlayItem(rawAddr, label, status);
    }
}
void AvrcpTgService::ObserverImpl::OnAddToNowPlaying(const std::string &addr, uint8_t label, int status)
{
    HILOGI("addr: %{public}s, label: %{public}d, status: %{public}d", GetEncryptAddr(addr).c_str(), label, status);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnAddToNowPlaying(rawAddr, label, status);
    }
}

void AvrcpTgService::ObserverImpl::OnChangePath(const std::string &addr, uint32_t numOfItems, uint8_t label, int status)
{
    HILOGI("addr: %{public}s, numOfItems: %{public}u, label: %{public}d, status: %{public}d",
        GetEncryptAddr(addr).c_str(), numOfItems, label, status);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnChangePath(rawAddr, numOfItems, label, status);
    }
}

void AvrcpTgService::ObserverImpl::OnGetMediaPlayers(const std::string &addr, uint16_t uidCounter,
    const std::vector<stub::MediaService::MediaPlayer> &items, uint8_t label, int status)
{
    HILOGI("addr: %{public}s, uidCounter: %{public}d, label: %{public}d, status: %{public}d",
        GetEncryptAddr(addr).c_str(), uidCounter, label, status);
    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        std::vector<AvrcMpItem> mpItems;
        for (stub::MediaService::MediaPlayer item : items) {
            AvrcMpItem mpItem(item.itemType_,
                item.playerId_,
                item.majorType_,
                item.subType_,
                item.playStatus_,
                item.features_,
                item.name_);
            mpItems.push_back(mpItem);
        }

        service->OnGetMediaPlayers(rawAddr, uidCounter, mpItems, label, status);
    }
}

void AvrcpTgService::ObserverImpl::OnGetFolderItems(const std::string &addr, uint16_t uidCounter,
    const std::vector<stub::MediaService::MediaItem> &items, uint8_t label, int status)
{
    HILOGI("addr: %{public}s, uidCounter: %{public}d, label: %{public}d, status: %{public}d",
        GetEncryptAddr(addr).c_str(), uidCounter, label, status);
    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        std::vector<AvrcMeItem> meItems;
        for (stub::MediaService::MediaItem item : items) {
            AvrcMeItem meItem(
                item.itemType_, item.uid_, item.mediaType_, item.playable_, item.name_, item.attributes_, item.values_);
            meItems.push_back(meItem);
        }

        service->OnGetFolderItems(rawAddr, uidCounter, meItems, label, status);
    }
}

void AvrcpTgService::ObserverImpl::OnGetItemAttributes(const std::string &addr, const std::vector<uint32_t> &attributes,
    const std::vector<std::string> &values, uint8_t label, int status)
{
    HILOGI("addr: %{public}s, label: %{public}d, status: %{public}d", GetEncryptAddr(addr).c_str(), label, status);
    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetItemAttributes(rawAddr, attributes, values, label, status);
    }
}

void AvrcpTgService::ObserverImpl::OnGetTotalNumberOfItems(
    const std::string &addr, uint16_t uidCounter, uint32_t numOfItems, uint8_t label, int status)
{
    HILOGI("addr: %{public}s, uidCounter: %{public}d, numOfItems: %{public}u, label: %{public}d, status: %{public}d",
        GetEncryptAddr(addr).c_str(), uidCounter, numOfItems, label, status);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetTotalNumberOfItems(rawAddr, uidCounter, numOfItems, label, status);
    }
}

void AvrcpTgService::ObserverImpl::OnSetAbsoluteVolume(const std::string &addr, uint8_t volume, uint8_t label)
{
    HILOGI("addr: %{public}s, volume: %{public}d, label: %{public}d", GetEncryptAddr(addr).c_str(), volume, label);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnSetAbsoluteVolume(rawAddr, volume, label);
    }
}

void AvrcpTgService::ObserverImpl::OnGetSelectedTrack(const std::string &addr, uint64_t uid, uint8_t label)
{
    HILOGI("addr: %{public}s, uid: %{public}llu, label: %{public}d",
        GetEncryptAddr(addr).c_str(), static_cast<unsigned long long>(uid), label);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetSelectedTrack(rawAddr, uid, label);
    }
}

void AvrcpTgService::ObserverImpl::OnGetAddressedPlayer(
    const std::string &addr, uint16_t playerId, uint16_t uidCounter, uint8_t label)
{
    HILOGI("addr: %{public}s, playerId: %{public}d, uidCounter: %{public}d, label: %{public}d",
        GetEncryptAddr(addr).c_str(), playerId, uidCounter, label);

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetAddressedPlayer(rawAddr, playerId, uidCounter, label);
    }
}

void AvrcpTgService::ObserverImpl::OnGetUidCounter(const std::string &addr, uint16_t uidCounter, uint8_t label)
{
    HILOGI("addr: %{public}s, uidCounter: %{public}d, label: %{public}d",
        GetEncryptAddr(addr).c_str(), uidCounter, label);
    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetUidCounter(rawAddr, uidCounter, label);
    }
}

void AvrcpTgService::ObserverImpl::OnGetCurrentAbsoluteVolume(const std::string &addr, uint8_t volume, uint8_t label)
{
    HILOGI("addr: %{public}s, volume: %{public}d, label: %{public}d", GetEncryptAddr(addr).c_str(), volume, label);
    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        RawAddress rawAddr(addr);
        service->OnGetCurrentAbsoluteVolume(rawAddr, volume, label);
    }
}

IProfileAvrcpTg *AvrcpTgService::ObserverImpl::GetService(void)
{
    HILOGI("enter");

    auto svManager = IProfileManager::GetInstance();

    return static_cast<IProfileAvrcpTg *>(svManager->GetProfileService(PROFILE_NAME_AVRCP_TG));
}
#ifdef AVRCP_AVSESSION
void AvrcpTgService::AVSessionObserverImpl::OnSessionCreate(const OHOS::AVSession::AVSessionDescriptor& descriptor)
{
    HILOGI("enter");

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        service->OnSessionCreate(descriptor.sessionId_);
    }
}

void AvrcpTgService::AVSessionObserverImpl::OnSessionRelease(const OHOS::AVSession::AVSessionDescriptor& descriptor)
{
    HILOGI("enter");

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        service->OnSessionRelease(descriptor.sessionId_);
    }
}

void AvrcpTgService::AVSessionObserverImpl::OnTopSessionChange(const OHOS::AVSession::AVSessionDescriptor& descriptor)
{
    HILOGI("sessionId:%{public}s", descriptor.sessionId_.c_str());

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        service->OnTopSessionChange(descriptor.sessionId_);
    }
}

IProfileAvrcpTg *AvrcpTgService::AVSessionObserverImpl::GetService(void)
{
    HILOGI("enter");

    auto svManager = IProfileManager::GetInstance();

    return static_cast<IProfileAvrcpTg *>(svManager->GetProfileService(PROFILE_NAME_AVRCP_TG));
}

void AvrcpTgService::AVControllerObserverImpl::OnSessionDestroy()
{
    HILOGI("enter");
}

void AvrcpTgService::AVControllerObserverImpl::OnPlaybackStateChange(const OHOS::AVSession::AVPlaybackState &state)
{
    HILOGI("state:%{public}d", state.GetState());

    IProfileAvrcpTg *service = GetService();
    if (service != nullptr) {
        service->OnPlaybackStateChange(state.GetState());
    }
}

void AvrcpTgService::AVControllerObserverImpl::OnMetaDataChange(const OHOS::AVSession::AVMetaData &data)
{
    HILOGI("enter");
}

void AvrcpTgService::AVControllerObserverImpl::OnActiveStateChange(bool isActive)
{
    HILOGI("isActive:%{public}d", isActive);
}

void AvrcpTgService::AVControllerObserverImpl::OnValidCommandChange(const std::vector<int32_t> &cmds)
{
    HILOGI("enter");
}

IProfileAvrcpTg *AvrcpTgService::AVControllerObserverImpl::GetService(void)
{
    HILOGI("enter");

    auto svManager = IProfileManager::GetInstance();

    return static_cast<IProfileAvrcpTg *>(svManager->GetProfileService(PROFILE_NAME_AVRCP_TG));
}
#endif
AvrcpTgService::AvrcpTgService() : utility::Context(PROFILE_NAME_AVRCP_TG, "1.6.2")
{
    HILOGI("enter");

    mdObserver_ = std::make_unique<ObserverImpl>();
#ifdef AVRCP_AVSESSION
    avSessionObserver_ = std::make_shared<AVSessionObserverImpl>();
    avControllerObserver_ = std::make_shared<AVControllerObserverImpl>();
#endif

    InitFeatures();

    using namespace std::placeholders;
    AvrcTgProfile::Observer observer = {
        std::bind(&AvrcpTgService::OnProfileDisabled, this, RET_NO_ERROR),
        std::bind(&AvrcpTgService::OnConnectionStateChanged, this, _1, _2),
        std::bind(&AvrcpTgService::FindCtService, this, _1),
        std::bind(&AvrcpTgService::OnButtonPressed, this, _1, _2, _3),
        std::bind(&AvrcpTgService::OnButtonReleased, this, _1, _2, _3),
        std::bind(&AvrcpTgService::HoldButton, this, _1, _2, _3),
        std::bind(&AvrcpTgService::GetCapabilities, this, _1, _2),
        std::bind(&AvrcpTgService::GetPlayerAppSettingAttributes, this, _1, _2),
        std::bind(&AvrcpTgService::GetPlayerAppSettingValues, this, _1, _2, _3),
        std::bind(&AvrcpTgService::GetPlayerAppSettingCurrentValue, this, _1, _2, _3, _4),
        std::bind(&AvrcpTgService::SetPlayerAppSettingCurrentValue, this, _1, _2, _3, _4),
        std::bind(&AvrcpTgService::GetPlayerAppSettingAttributeText, this, _1, _2, _3),
        std::bind(&AvrcpTgService::GetPlayerAppSettingValueText, this, _1, _2, _3, _4),
        std::bind(&AvrcpTgService::GetElementAttributes, this, _1, _2, _3, _4),
        std::bind(&AvrcpTgService::GetPlayStatus, this, _1, _2, _3),
        std::bind(&AvrcpTgService::SetAddressedPlayer, this, _1, _2, _3, _4),
        std::bind(&AvrcpTgService::SetBrowsedPlayer, this, _1, _2, _3, _4),
        std::bind(&AvrcpTgService::ChangePath, this, _1, _2, _3, _4, _5),
        std::bind(&AvrcpTgService::GetFolderItems, this, _1, _2, _3, _4, _5, _6),
        std::bind(&AvrcpTgService::GetItemAttributes, this, _1, _2, _3, _4, _5, _6),
        std::bind(&AvrcpTgService::GetTotalNumberOfItems, this, _1, _2, _3),
        std::bind(&AvrcpTgService::PlayItem, this, _1, _2, _3, _4, _5),
        std::bind(&AvrcpTgService::AddToNowPlaying, this, _1, _2, _3, _4, _5),
        std::bind(&AvrcpTgService::SetAbsoluteVolume, this, _1, _2, _3),
        std::bind(&AvrcpTgService::GetSelectedTrack, this, _1, _2),
        std::bind(&AvrcpTgService::GetAddressedPlayer, this, _1, _2),
        std::bind(&AvrcpTgService::GetUidCounter, this, _1, _2),
        std::bind(&AvrcpTgService::GetCurrentAbsoluteVolume, this, _1, _2),
        std::bind(&AvrcpTgService::SetPlaybackInterval, this, _1, _2),
        std::bind(&AvrcpTgService::SetActiveDevice, this, _1),
    };
    pfObserver_ = std::make_unique<AvrcTgProfile::Observer>(observer);
}

AvrcpTgService::~AvrcpTgService()
{
    HILOGI("enter");

    myObserver_ = nullptr;
}

utility::Context *AvrcpTgService::GetContext()
{
    HILOGI("enter");

    return this;
}

void AvrcpTgService::InitFeatures()
{
    HILOGI("enter");

    features_ |= AVRC_TG_FEATURE_CATEGORY_1;
    features_ |= AVRC_TG_FEATURE_CATEGORY_2;
    features_ |= AVRC_TG_FEATURE_PLAYER_APPLICATION_SETTINGS;
    features_ |= AVRC_TG_FEATURE_BROWSING;
    features_ |= AVRC_TG_FEATURE_MULTIPLE_MEDIA_PLAYER_APPLICATIONS;
    features_ |= AVRC_TG_FEATURE_KEY_OPERATION;
    features_ |= AVRC_TG_FEATURE_ABSOLUTE_VOLUME;
    features_ |= AVRC_TG_FEATURE_NOTIFY_PLAYBACK_STATUS_CHANGED;
    features_ |= AVRC_TG_FEATURE_NOTIFY_TRACK_REACHED_END;
    features_ |= AVRC_TG_FEATURE_NOTIFY_TRACK_REACHED_START;
    features_ |= AVRC_TG_FEATURE_NOTIFY_PLAYBACK_POSITION_CHANGED;
    features_ |= AVRC_TG_FEATURE_NOTIFY_PLAYER_SETTING_CHANGED;
    features_ |= AVRC_TG_FEATURE_NOTIFY_NOW_PLAYING_CONTENT_CHANGED;
    features_ |= AVRC_TG_FEATURE_NOTIFY_UIDS_CHANGED;
    features_ |= AVRC_TG_FEATURE_NOTIFY_ABSOLUTE_VOLUME_CHANGED;
}

/******************************************************************
 * REGISTER / UNREGISTER OBSERVER                                 *
 ******************************************************************/

void AvrcpTgService::RegisterObserver(IObserver *observer)
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lock(mutex_);

    myObserver_ = observer;
}

void AvrcpTgService::UnregisterObserver(void)
{
    HILOGI("enter");

    std::lock_guard<std::recursive_mutex> lock(mutex_);

    myObserver_ = nullptr;
}

/******************************************************************
 * ENABLE / DISABLE                                               *
 ******************************************************************/

void AvrcpTgService::Enable(void)
{
    HILOGI("enter");

    if (IsDisabled()) {
        SetServiceState(AVRC_TG_SERVICE_STATE_ENABLING);

        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::EnableNative, this));
    } else {
        HILOGE("Is not disabled!");
    }
}

void AvrcpTgService::Disable(void)
{
    HILOGI("enter");

    if (IsEnabled()) {
        SetServiceState(AVRC_TG_SERVICE_STATE_DISABLING);
        if (profile_ != nullptr) {
            profile_->SetEnableFlag(false);
        }
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::DisableNative, this));
    } else {
        HILOGE("Is not enable!");
    }
}

void AvrcpTgService::EnableNative(void)
{
    HILOGI("enter");

    int result = RET_NO_ERROR;

    stub::MediaService::GetInstance()->RegisterObserver(mdObserver_.get());
#ifdef AVRCP_AVSESSION
    RegisterAvSessionControl();
#endif
    IAdapterConfig *config = AdapterConfig::GetInstance();
    config->GetValue(SECTION_AVRCP_TG_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, maxConnection_);

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
        SetServiceState(AVRC_TG_SERVICE_STATE_ENABLED);
    } else {
        SetServiceState(AVRC_TG_SERVICE_STATE_DISABLED);
    }

    GetContext()->OnEnable(PROFILE_NAME_AVRCP_TG, IsEnabled());
}

void AvrcpTgService::DisableNative(void)
{
    HILOGI("enter");

    if (DisableProfile() != RET_NO_ERROR) {
        OnProfileDisabled(RET_BAD_STATUS);
    }
}

#ifdef AVRCP_AVSESSION
void AvrcpTgService::RegisterAvSessionControl(void)
{
    // register avSession observer
    auto res = OHOS::AVSession::AVSessionManager::GetInstance().RegisterSessionListener(avSessionObserver_);
    if (res != OHOS::AVSession::AVSESSION_SUCCESS) {
        HILOGE("RegisterSessionListener fail");
        return;
    }

    res = OHOS::AVSession::AVSessionManager::GetInstance().GetAllSessionDescriptors(avSessionDescriptor_);
    if (res != OHOS::AVSession::AVSESSION_SUCCESS) {
        HILOGE("GetAllSessionDescriptors fail");
        return;
    }

    // parse top descriptor
    std::string sessionId;
    for (auto descriptor : avSessionDescriptor_) {
        HILOGI("isTopSession:%{public}d", descriptor.isTopSession_);
        if (descriptor.isTopSession_) {
            sessionId = descriptor.sessionId_;
        }
    }

    if (sessionId.empty()) {
        HILOGI("no valid avsession");
    } else {
        res = OHOS::AVSession::AVSessionManager::GetInstance().CreateController(sessionId, avSessionController_);
        if (res == OHOS::AVSession::AVSESSION_SUCCESS && avSessionController_ != NULL) {
            avSessionController_->RegisterCallback(avControllerObserver_);
        } else {
            HILOGE("avSessionController_ is NULL");
        }
    }
}
#endif

int AvrcpTgService::RegisterSecurity(void)
{
    HILOGI("enter");

    gapManager_ = std::make_unique<AvrcTgGapManager>();

    return gapManager_->RegisterSecurity();
}

int AvrcpTgService::UnregisterSecurity(void)
{
    HILOGI("enter");

    int result = gapManager_->UnregisterSecurity();
    gapManager_ = nullptr;

    return result;
}

int AvrcpTgService::RegisterService(void)
{
    HILOGI("enter");

    sdpManager_ = std::make_unique<AvrcTgSdpManager>(features_ & AVRC_TG_SDP_ALL_SUPPORTED_FEATURES);

    return sdpManager_->RegisterService();
}

int AvrcpTgService::UnregisterService(void)
{
    HILOGI("enter");

    int result = sdpManager_->UnregisterService();
    sdpManager_ = nullptr;

    return result;
}

int AvrcpTgService::EnableProfile(void)
{
    HILOGI("enter");

    /// Gets the size of the MTU.
    int controlMtu = AVRC_TG_DEFAULT_CONTROL_MTU_SIZE;
    int browseMtu = AVRC_TG_DEFAULT_BROWSE_MTU_SIZE;

    IAdapterConfig *config = AdapterConfig::GetInstance();
    config->GetValue(SECTION_AVRCP_TG_SERVICE, PROPERTY_CONTROL_MTU, controlMtu);
    config->GetValue(SECTION_AVRCP_TG_SERVICE, PROPERTY_BROWSE_MTU, browseMtu);

    profile_ = std::make_unique<AvrcTgProfile>(features_,
        AVRC_TG_DEFAULT_BLUETOOTH_SIG_COMPANY_ID,
        controlMtu,
        browseMtu,
        GetDispatcher(),
        ChannelEventCallback,
        ChannelMessageCallback);
    profile_->RegisterObserver(pfObserver_.get());

    return profile_->Enable();
}

int AvrcpTgService::DisableProfile(void) const
{
    HILOGI("enter");

    return profile_->Disable();
}

void AvrcpTgService::OnProfileDisabled(int result)
{
    HILOGI("result:%{public}d", result);

    SetServiceState(AVRC_TG_SERVICE_STATE_DISABLED);

    profile_->UnregisterObserver();
    profile_ = nullptr;

    result |= UnregisterService();
    result |= UnregisterSecurity();
    stub::MediaService::GetInstance()->UnregisterObserver(mdObserver_.get());

    GetContext()->OnDisable(PROFILE_NAME_AVRCP_TG, result == RET_NO_ERROR);
}

bool AvrcpTgService::IsEnabled(void)
{
    bool enable = (state_ == AVRC_TG_SERVICE_STATE_ENABLED);
    HILOGI("enable:%{public}d", enable);

    return enable;
}

bool AvrcpTgService::IsDisabled(void)
{
    bool disable = (state_ == AVRC_TG_SERVICE_STATE_DISABLED);
    HILOGI("disable:%{public}d", disable);

    return disable;
}

void AvrcpTgService::SetServiceState(uint8_t state)
{
    HILOGI("state:%{public}d", state);

    state_ = state;
}

/******************************************************************
 * CONNECTION                                                     *
 ******************************************************************/

void AvrcpTgService::SetActiveDevice(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    if (IsEnabled()) {
        profile_->SetActiveDevice(rawAddr);
        stub::MediaService::GetInstance()->SetActiveDevice(rawAddr.GetAddress());
    }
}

std::vector<RawAddress> AvrcpTgService::GetConnectedDevices(void)
{
    HILOGI("enter");

    std::vector<RawAddress> result;

    if (IsEnabled()) {
        result = profile_->GetConnectedDevices();
    }

    return result;
}

std::vector<bluetooth::RawAddress> AvrcpTgService::GetDevicesByStates(const std::vector<int> &states)
{
    HILOGI("enter");

    std::vector<bluetooth::RawAddress> result;

    if (IsEnabled()) {
        result = profile_->GetDevicesByStates(states);
    }

    return result;
}

int AvrcpTgService::GetDeviceState(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = static_cast<int>(BTConnectState::DISCONNECTED);

    if (IsEnabled()) {
        result = profile_->GetDeviceState(rawAddr);
    }

    HILOGI("result: %{public}d", result);

    return result;
}

int AvrcpTgService::GetMaxConnectNum(void)
{
    HILOGI("enter");

    int result = 0;

    if (IsEnabled()) {
        result = profile_->GetMaxConnectNum();
    }

    return result;
}

int AvrcpTgService::Connect(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

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
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::ConnectNative, this, peerAddr));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpTgService::ConnectNative(RawAddress rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AcceptActiveConnect(rawAddr);
}

int AvrcpTgService::Disconnect(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_BAD_STATUS;

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::DisconnectNative, this, peerAddr));
        result = RET_NO_ERROR;
    } while (false);

    return result;
}

void AvrcpTgService::DisconnectNative(RawAddress rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (profile_->Disconnect(rawAddr) != RET_NO_ERROR) {
            HILOGI("Call AvrcTgProfile::Disconnect Failed Address%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
        }
    } while (false);
}

int AvrcpTgService::GetConnectState(void)
{
    HILOGI("enter");

    int result = PROFILE_STATE_DISCONNECTED;

    if (IsEnabled()) {
        result = profile_->GetConnectState();
    }

    return result;
}

void AvrcpTgService::OnConnectionStateChanged(const RawAddress &rawAddr, int state)
{
    HILOGI("Address: %{public}s, state: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), state);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (myObserver_ != nullptr) {
        myObserver_->OnConnectionStateChanged(rawAddr.GetAddress(), state);
    }
}

void AvrcpTgService::AcceptActiveConnect(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::DISCONNECTED)) {
            break;
        }

        if (profile_->Connect(rawAddr) != RET_NO_ERROR) {
            HILOGI("Call Connect Failed! Address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
        }
    } while (false);
}

void AvrcpTgService::AcceptPassiveConnect(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        profile_->AcceptPassiveConnect(rawAddr);
    } while (false);
}

void AvrcpTgService::RejectPassiveConnect(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    do {
        if (!IsEnabled()) {
            break;
        }

        profile_->RejectPassiveConnect(rawAddr);
    } while (false);
}

void AvrcpTgService::FindCtService(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    if (sdpManager_->FindCtService(rawAddr, FindCtServiceCallback) != RET_NO_ERROR) {
        RejectPassiveConnect(rawAddr);
    }
}

void AvrcpTgService::FindCtServiceCallback(
    const BtAddr *btAddr, const uint32_t *handleArray, uint16_t handleCount, void *context)
{
    HILOGI("handleCount: %{public}d", handleCount);

    auto servManager = IProfileManager::GetInstance();
    auto service = static_cast<AvrcpTgService *>(servManager->GetProfileService(PROFILE_NAME_AVRCP_TG));
    RawAddress rawAddr(RawAddress::ConvertToString(btAddr->addr));
    if (service != nullptr) {
        if (handleCount > 0) {
            service->GetDispatcher()->PostTask(std::bind(&AvrcpTgService::AcceptPassiveConnect, service, rawAddr));
        } else {
            service->GetDispatcher()->PostTask(std::bind(&AvrcpTgService::RejectPassiveConnect, service, rawAddr));
        }
    }
}

#ifdef AVRCP_AVSESSION
/******************************************************************
 * PASS THROUGH COMMAND                                           *
 ******************************************************************/
static void SetAvControlCommand(uint8_t button, OHOS::AVSession::AVControlCommand &command, bool buttonStatus)
{
    switch (button) {
        case AVRC_KEY_OPERATION_VOLUME_UP:
            break;
        case AVRC_KEY_OPERATION_VOLUME_DOWN:
            break;
        case AVRC_KEY_OPERATION_MUTE:
            break;
        case AVRC_KEY_OPERATION_PLAY:
            command.SetCommand(OHOS::AVSession::AVControlCommand::SESSION_CMD_PLAY);
            break;
        case AVRC_KEY_OPERATION_STOP:
            command.SetCommand(OHOS::AVSession::AVControlCommand::SESSION_CMD_STOP);
            break;
        case AVRC_KEY_OPERATION_PAUSE:
            command.SetCommand(OHOS::AVSession::AVControlCommand::SESSION_CMD_PAUSE);
            break;
        case AVRC_KEY_OPERATION_REWIND:
            if (!buttonStatus) {
                command.SetCommand(OHOS::AVSession::AVControlCommand::SESSION_CMD_REWIND);
            } else {
                command.SetCommand(OHOS::AVSession::AVControlCommand::SESSION_CMD_PLAY);
            }
            break;
        case AVRC_KEY_OPERATION_FAST_FORWARD:
            if (!buttonStatus) {
                command.SetCommand(OHOS::AVSession::AVControlCommand::SESSION_CMD_FAST_FORWARD);
            } else {
                command.SetCommand(OHOS::AVSession::AVControlCommand::SESSION_CMD_PLAY);
            }
            break;
        case AVRC_KEY_OPERATION_FORWARD:
            command.SetCommand(OHOS::AVSession::AVControlCommand::SESSION_CMD_PLAY_NEXT);
            break;
        case AVRC_KEY_OPERATION_BACKWARD:
            command.SetCommand(OHOS::AVSession::AVControlCommand::SESSION_CMD_PLAY_PREVIOUS);
            break;
        default:
            break;
    }
}
#endif
void AvrcpTgService::OnButtonPressed(const RawAddress &rawAddr, uint8_t button, uint8_t label) const
{
    HILOGI("rawAddr:%{public}s, button:%{public}d, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button, label);
#ifndef AVRCP_AVSESSION
    int result = stub::MediaService::GetInstance()->PressButton(rawAddr.GetAddress(), button, label);
    profile_->SendPressButtonRsp(rawAddr, button, label, result);
#else
    OHOS::AVSession::AVControlCommand command;
    SetAvControlCommand(button, command, false);
    profile_->SendPressButtonRsp(rawAddr, button, label, RET_NO_ERROR);
#endif
}

void AvrcpTgService::OnButtonReleased(const RawAddress &rawAddr, uint8_t button, uint8_t label) const
{
    HILOGI("rawAddr:%{public}s, button:%{public}d, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button, label);
#ifndef AVRCP_AVSESSION
    int result = stub::MediaService::GetInstance()->ReleaseButton(rawAddr.GetAddress(), button, label);
#else
    OHOS::AVSession::AVControlCommand command;
    SetAvControlCommand(button, command, true);
    int result = OHOS::AVSession::AVSessionManager::GetInstance().SendSystemControlCommand(command);
#endif
    profile_->SendReleaseButtonRsp(rawAddr, button, label, result);
}

void AvrcpTgService::HoldButton(const RawAddress &rawAddr, uint8_t button, uint8_t label) const
{
    HILOGI("rawAddr:%{public}s, button:%{public}d, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button, label);

    int result = stub::MediaService::GetInstance()->PressButton(rawAddr.GetAddress(), button, label);
    profile_->SendPressButtonRsp(rawAddr, button, label, result);
}

/******************************************************************
 * Media Player Selection                                         *
 ******************************************************************/

void AvrcpTgService::SetAddressedPlayer(
    const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, uint8_t label) const
{
    HILOGI("rawAddr:%{public}s, playerId:%{public}d, uidCounter:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId, uidCounter, label);

    int result =
        stub::MediaService::GetInstance()->SetAddressedPlayer(rawAddr.GetAddress(), playerId, uidCounter, label);
    if (result != RET_NO_ERROR) {
        profile_->SendSetAddressedPlayerRsp(rawAddr, AVRC_ES_CODE_INTERNAL_ERROR, label, result);
    }
}

void AvrcpTgService::OnSetAddressedPlayer(const RawAddress &rawAddr, uint8_t label, int status)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, status:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, status);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnSetAddressedPlayerNative, this, peerAddr, label, status));
    } while (false);
}

void AvrcpTgService::OnSetAddressedPlayerNative(RawAddress rawAddr, uint8_t label, int status)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, status:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, status);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendSetAddressedPlayerRsp(rawAddr, status, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::SetBrowsedPlayer(const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter,
    uint8_t label) const
{
    HILOGI("rawAddr:%{public}s, playerId:%{public}d, uidCounter:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId, uidCounter, label);

    int result = stub::MediaService::GetInstance()->SetBrowsedPlayer(rawAddr.GetAddress(), playerId, uidCounter, label);
    if (result != RET_NO_ERROR) {
        std::vector<std::string> folderNames;
        profile_->SendSetBrowsedPlayerRsp(rawAddr, 0x00, 0x00, folderNames, label, AVRC_ES_CODE_INTERNAL_ERROR);
    }
}

void AvrcpTgService::OnSetBrowsedPlayer(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems,
    const std::vector<std::string> &folderNames, uint8_t label, int status)
{
    HILOGI("rawAddr:%{public}s, uidCounter:%{public}d, numOfItems:%{public}u, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, numOfItems, label, status);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::OnSetBrowsedPlayerNative,
            this,
            peerAddr,
            uidCounter,
            numOfItems,
            folderNames,
            label,
            status));
    } while (false);
}

void AvrcpTgService::OnSetBrowsedPlayerNative(RawAddress rawAddr, uint16_t uidCounter, uint32_t numOfItems,
    std::vector<std::string> folderNames, uint8_t label, int status)
{
    HILOGI("rawAddr:%{public}s, uidCounter:%{public}d, numOfItems:%{public}u, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, numOfItems, label, status);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendSetBrowsedPlayerRsp(rawAddr, uidCounter, numOfItems, folderNames, label, status);
    } while (false);
}

/******************************************************************
 * Capabilities                                                   *
 ******************************************************************/
void AvrcpTgService::GetCapabilities(const RawAddress &rawAddr, uint8_t label)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);
#ifndef AVRCP_AVSESSION
    std::vector<uint8_t> events = stub::MediaService::GetInstance()->GetCapabilities(rawAddr.GetAddress(), label);
#else
    std::vector<uint8_t> events;
    events.push_back(AVRC_EVENT_ID_PLAYBACK_STATUS_CHANGED);
    events.push_back(AVRC_EVENT_ID_TRACK_CHANGED);
#endif
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetCapabilitiesRsp(rawAddr, events, label, RET_NO_ERROR);
    } while (false);
}

/******************************************************************
 * PLAYER APPLICATION SETTINGS                                    *
 ******************************************************************/

void AvrcpTgService::GetPlayerAppSettingAttributes(const RawAddress &rawAddr, uint8_t label) const
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    int result = stub::MediaService::GetInstance()->GetPlayerAppSettingAttributes(rawAddr.GetAddress(), label);
    if (result != RET_NO_ERROR) {
        std::deque<uint8_t> attribtues;
        profile_->SendListPlayerApplicationSettingAttributesRsp(rawAddr, attribtues, label, result);
    }
}

void AvrcpTgService::OnGetPlayerAppSettingAttributes(
    const RawAddress &rawAddr, const std::deque<uint8_t> &attributes, uint8_t label)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnGetPlayerAppSettingAttributesNative, this, peerAddr, attributes, label));
    } while (false);
}

void AvrcpTgService::OnGetPlayerAppSettingAttributesNative(
    RawAddress rawAddr, std::deque<uint8_t> attributes, uint8_t label)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendListPlayerApplicationSettingAttributesRsp(rawAddr, attributes, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::GetPlayerAppSettingValues(const RawAddress &rawAddr, uint8_t attribute, uint8_t label) const
{
    HILOGI("rawAddr:%{public}s, attribute:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), attribute, label);

    int result = stub::MediaService::GetInstance()->GetPlayerAppSettingValues(rawAddr.GetAddress(), attribute, label);
    if (result != RET_NO_ERROR) {
        std::deque<uint8_t> values;
        profile_->SendListPlayerApplicationSettingValuesRsp(rawAddr, values, label, result);
    }
}

void AvrcpTgService::OnGetPlayerAppSettingValues(
    const RawAddress &rawAddr, const std::deque<uint8_t> &values, uint8_t label)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnGetPlayerAppSettingValuesNative, this, peerAddr, values, label));
    } while (false);
}

void AvrcpTgService::OnGetPlayerAppSettingValuesNative(RawAddress rawAddr, std::deque<uint8_t> values, uint8_t label)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendListPlayerApplicationSettingValuesRsp(rawAddr, values, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::GetPlayerAppSettingCurrentValue(
    const RawAddress &rawAddr, const std::deque<uint8_t> &attributes, uint8_t label, uint8_t context) const
{
    HILOGI("addr:%{public}s, label:%{public}d, context:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, context);

    int result = stub::MediaService::GetInstance()->GetPlayerAppSettingCurrentValue(
        rawAddr.GetAddress(), attributes, label, context);
    if (result != RET_NO_ERROR) {
        std::deque<uint8_t> attrs;
        std::deque<uint8_t> vals;
        if (context == AVRC_ACTION_TYPE_GET_PLAYER_APP_SETTING_CURRENT_VALUE) {
            profile_->SendGetCurrentPlayerApplicationSettingValueRsp(rawAddr, attrs, vals, label, result);
        } else {
            profile_->SendPlayerApplicationSettingChangedRsp(true, attrs, vals, label, result);
        }
    }
}

void AvrcpTgService::OnGetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, const std::deque<uint8_t> &attributes,
    const std::deque<uint8_t> &values, uint8_t label, uint8_t context)
{
    HILOGI("addr:%{public}s, label:%{public}d, context:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, context);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::OnGetCurrentPlayerAppSettingValueNative,
            this,
            peerAddr,
            attributes,
            values,
            label,
            context));
    } while (false);
}

void AvrcpTgService::OnGetCurrentPlayerAppSettingValueNative(
    RawAddress rawAddr, std::deque<uint8_t> attributes, std::deque<uint8_t> values, uint8_t label, uint8_t context)
{
    HILOGI("addr:%{public}s, label:%{public}d, context:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, context);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (context == AVRC_ACTION_TYPE_GET_PLAYER_APP_SETTING_CURRENT_VALUE) {
            profile_->SendGetCurrentPlayerApplicationSettingValueRsp(rawAddr, attributes, values, label, RET_NO_ERROR);
        } else {
            profile_->SendPlayerApplicationSettingChangedRsp(true, attributes, values, label, RET_NO_ERROR);
        }
    } while (false);
}

void AvrcpTgService::SetPlayerAppSettingCurrentValue(
    const RawAddress &rawAddr, const std::deque<uint8_t> &attributes,
    const std::deque<uint8_t> &values, uint8_t label) const
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    int result = stub::MediaService::GetInstance()->SetPlayerAppSettingCurrentValue(
        rawAddr.GetAddress(), attributes, values, label);
    if (result != RET_NO_ERROR) {
        profile_->SendSetPlayerApplicationSettingValueRsp(rawAddr, label, result);
    }
}

void AvrcpTgService::OnSetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, uint8_t label)
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnSetPlayerAppSettingCurrentValueNative, this, peerAddr, label));
    } while (false);
}

void AvrcpTgService::OnSetPlayerAppSettingCurrentValueNative(RawAddress rawAddr, uint8_t label)
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendSetPlayerApplicationSettingValueRsp(rawAddr, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::GetPlayerAppSettingAttributeText(
    const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, uint8_t label) const
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    int result =
        stub::MediaService::GetInstance()->GetPlayerAppSettingAttributeText(rawAddr.GetAddress(), attributes, label);
    if (result != RET_NO_ERROR) {
        profile_->SendSetPlayerApplicationSettingValueRsp(rawAddr, label, result);
    }
}

void AvrcpTgService::OnGetPlayerAppSettingAttributeText(const RawAddress &rawAddr,
    const std::vector<uint8_t> &attributes, const std::vector<std::string> &attrStr, uint8_t label)
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(
            &AvrcpTgService::OnGetPlayerAppSettingAttributeTextNative, this, peerAddr, attributes, attrStr, label));
    } while (false);
}

void AvrcpTgService::OnGetPlayerAppSettingAttributeTextNative(
    RawAddress rawAddr, std::vector<uint8_t> attributes, std::vector<std::string> attrStr, uint8_t label)
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetPlayerApplicationSettingAttributeTextRsp(rawAddr, attributes, attrStr, label, BT_NO_ERROR);
    } while (false);
}

void AvrcpTgService::GetPlayerAppSettingValueText(
    const RawAddress &rawAddr, uint8_t attributeId, const std::vector<uint8_t> &values, uint8_t label) const
{
    HILOGI("addr:%{public}s, attributeId:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), attributeId, label);

    int result = stub::MediaService::GetInstance()->GetPlayerAppSettingValueText(
        rawAddr.GetAddress(), attributeId, values, label);
    if (result != RET_NO_ERROR) {
        profile_->SendSetPlayerApplicationSettingValueRsp(rawAddr, label, result);
    }
}

void AvrcpTgService::OnGetPlayerAppSettingValueText(const RawAddress &rawAddr, const std::vector<uint8_t> &values,
    const std::vector<std::string> &valueStr, uint8_t label)
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnGetPlayerAppSettingValueTextNative, this, peerAddr, values, valueStr, label));
    } while (false);
}

void AvrcpTgService::OnGetPlayerAppSettingValueTextNative(
    RawAddress rawAddr, std::vector<uint8_t> values, std::vector<std::string> valueStr, uint8_t label)
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetPlayerApplicationSettingValueTextRsp(rawAddr, values, valueStr, label, BT_NO_ERROR);
    } while (false);
}

/******************************************************************
 * MEDIA INFORMATION PDUS                                         *
 ******************************************************************/

void AvrcpTgService::GetElementAttributes(
    const RawAddress &rawAddr, uint64_t identifier, const std::vector<uint32_t> &attributes, uint8_t label) const
{
    HILOGI("addr:%{public}s, identifier:%{public}llu, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), static_cast<unsigned long long>(identifier), label);
    int result =
        stub::MediaService::GetInstance()->GetElementAttributes(rawAddr.GetAddress(), identifier, attributes, label);
    if (result != RET_NO_ERROR) {
        std::vector<std::string> values;
        profile_->SendGetElementAttributesRsp(rawAddr, attributes, values, label, result);
    }
}

void AvrcpTgService::OnGetElementAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attribtues,
    const std::vector<std::string> &values, uint8_t label)
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }
        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }
        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnGetElementAttributesNative, this, peerAddr, attribtues, values, label));
    } while (false);
}

void AvrcpTgService::OnGetElementAttributesNative(
    RawAddress rawAddr, std::vector<uint32_t> attributes, std::vector<std::string> values, uint8_t label)
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetElementAttributesRsp(rawAddr, attributes, values, label, BT_NO_ERROR);
    } while (false);
}

/******************************************************************
 * PLAY                                                           *
 ******************************************************************/

void AvrcpTgService::GetPlayStatus(const RawAddress &rawAddr, uint8_t label, uint8_t context) const
{
    HILOGI("addr:%{public}s, label:%{public}d, context:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, context);
#ifndef AVRCP_AVSESSION
    int result = stub::MediaService::GetInstance()->GetPlayStatus(rawAddr.GetAddress(), label, context);
    if (result != RET_NO_ERROR) {
        if (context == AVRC_ACTION_TYPE_GET_PLAY_STATUS) {
            profile_->SendGetPlayStatusRsp(rawAddr,
                AVRC_PLAY_STATUS_INVALID_SONG_LENGTH,
                AVRC_PLAY_STATUS_INVALID_SONG_POSITION,
                AVRC_PLAY_STATUS_ERROR,
                label,
                result);
        } else if (context == AVRC_ACTION_TYPE_NOTIFY_PLAYBACK_STATUS_CHANGED) {
            profile_->SendPlaybackStatusChangedRsp(true, AVRC_PLAY_STATUS_ERROR, label, result);
        } else if (context == AVRC_ACTION_TYPE_NOTIFY_PLAYBACK_POS_CHANGED) {
            profile_->SendPlaybackPosChangedRsp(true, AVRC_PLAY_STATUS_INVALID_SONG_POSITION, label, result);
        }
#else
    OHOS::AVSession::AVPlaybackState state;
    if (avSessionController_ == NULL) {
        HILOGE("avSessionController_ is NULL");
        return;
    }
    int32_t result = avSessionController_->GetAVPlaybackState(state);
    uint8_t playstate = AvrcpTgService::ConvertPlayState(state.GetState());

    if (context == AVRC_ACTION_TYPE_GET_PLAY_STATUS) {
        profile_->SendGetPlayStatusRsp(rawAddr,
            AVRC_PLAY_STATUS_INVALID_SONG_LENGTH,
            AVRC_PLAY_STATUS_INVALID_SONG_POSITION,
            AVRC_PLAY_STATUS_ERROR,
            label,
            result);
    } else if (context == AVRC_ACTION_TYPE_NOTIFY_PLAYBACK_STATUS_CHANGED) {
        profile_->SendPlaybackStatusChangedRsp(true, playstate, label, result);
    } else if (context == AVRC_ACTION_TYPE_NOTIFY_PLAYBACK_POS_CHANGED) {
        profile_->SendPlaybackPosChangedRsp(true, AVRC_PLAY_STATUS_INVALID_SONG_POSITION, label, result);
#endif
    }
}

void AvrcpTgService::OnGetPlayStatus(const RawAddress &rawAddr, uint32_t songLength, uint32_t songPosition,
    uint8_t playStatus, uint8_t label, uint8_t context)
{
    HILOGI("addr:%{public}s, songLength:%{public}u, songPosition:%{public}u, playStatus:%{public}d, label:%{public}d,"
        " context:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), songLength, songPosition, playStatus, label, context);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::OnGetPlayStatusNative,
            this,
            peerAddr,
            songLength,
            songPosition,
            playStatus,
            label,
            context));
    } while (false);
}

void AvrcpTgService::OnGetPlayStatusNative(
    RawAddress rawAddr, uint32_t songLength, uint32_t songPosition, uint8_t playStatus, uint8_t label, uint8_t context)
{
    HILOGI("addr:%{public}s, songLength:%{public}u, songPosition:%{public}u, playStatus:%{public}d, label:%{public}d,"
        " context:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), songLength, songPosition, playStatus, label, context);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        if (context == AVRC_ACTION_TYPE_GET_PLAY_STATUS) {
            profile_->SendGetPlayStatusRsp(rawAddr, songLength, songPosition, playStatus, label, RET_NO_ERROR);
        } else if (context == AVRC_ACTION_TYPE_NOTIFY_PLAYBACK_STATUS_CHANGED) {
            profile_->SendPlaybackStatusChangedRsp(true, AVRC_PLAY_STATUS_ERROR, label, RET_NO_ERROR);
        } else if (context == AVRC_ACTION_TYPE_NOTIFY_PLAYBACK_POS_CHANGED) {
            profile_->SendPlaybackPosChangedRsp(true, AVRC_PLAY_STATUS_INVALID_SONG_POSITION, label, RET_NO_ERROR);
        }
    } while (false);
}

void AvrcpTgService::PlayItem(
    const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter, uint8_t label) const
{
    HILOGI("addr:%{public}s, scope:%{public}d, uid:%{public}llu, uidCounter:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, static_cast<unsigned long long>(uid), uidCounter, label);
    int result = stub::MediaService::GetInstance()->PlayItem(rawAddr.GetAddress(), scope, uid, uidCounter, label);
    if (result != RET_NO_ERROR) {
        profile_->SendPlayItemRsp(rawAddr, AVRC_ES_CODE_INTERNAL_ERROR, label, result);
    }
}

void AvrcpTgService::OnPlayItem(const RawAddress &rawAddr, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, label:%{public}d, status:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, status);

    do {
        if (!IsEnabled()) {
            break;
        }
        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }
        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::OnPlayItemNative, this, peerAddr, label, status));
    } while (false);
}

void AvrcpTgService::OnPlayItemNative(RawAddress rawAddr, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, label:%{public}d, status:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, status);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendPlayItemRsp(rawAddr, status, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::AddToNowPlaying(
    const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter, uint8_t label) const
{
    HILOGI("addr:%{public}s, scope:%{public}d, uid:%{public}llu, uidCounter:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, static_cast<unsigned long long>(uid), uidCounter, label);
    int result =
        stub::MediaService::GetInstance()->AddToNowPlaying(rawAddr.GetAddress(), scope, uid, uidCounter, label);
    if (result != RET_NO_ERROR) {
        profile_->SendAddToNowPlayingRsp(rawAddr, AVRC_ES_CODE_INTERNAL_ERROR, label, result);
    }
}

void AvrcpTgService::OnAddToNowPlaying(const RawAddress &rawAddr, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, label:%{public}d, status:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, status);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::OnAddToNowPlayingNative, this, peerAddr, label, status));
    } while (false);
}

void AvrcpTgService::OnAddToNowPlayingNative(RawAddress rawAddr, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, label:%{public}d, status:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, status);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendAddToNowPlayingRsp(rawAddr, status, label, RET_NO_ERROR);
    } while (false);
}

/******************************************************************
 * OPERATE THE VIRTUAL FILE SYSTEM                                *
 ******************************************************************/

void AvrcpTgService::ChangePath(
    const RawAddress &rawAddr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid, uint8_t label) const
{
    HILOGI("addr:%{public}s, uidCounter:%{public}d, direction:%{public}d, folderUid:%{public}llu, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, direction, static_cast<unsigned long long>(folderUid), label);
    int result =
        stub::MediaService::GetInstance()->ChangePath(rawAddr.GetAddress(), uidCounter, direction, folderUid, label);
    if (result != RET_NO_ERROR) {
        profile_->SendChangePathRsp(rawAddr, AVRC_TG_CP_NUMBER_OF_ITEMS, label, AVRC_ES_CODE_INTERNAL_ERROR);
    }
}

void AvrcpTgService::OnChangePath(const RawAddress &rawAddr, uint32_t numOfItems, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, numOfItems:%{public}u, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), numOfItems, label, status);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnChangePathNative, this, peerAddr, numOfItems, label, status));
    } while (false);
}

void AvrcpTgService::OnChangePathNative(RawAddress rawAddr, uint32_t numOfItems, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, numOfItems:%{public}u, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), numOfItems, label, status);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendChangePathRsp(rawAddr, numOfItems, label, status);
    } while (false);
}

void AvrcpTgService::GetFolderItems(const RawAddress &rawAddr, uint8_t scope, uint32_t startItem, uint32_t endItem,
    const std::vector<uint32_t> &attributes, uint8_t label) const
{
    HILOGI("addr:%{public}s, scope:%{public}d, startItem:%{public}u, endItem:%{public}u, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, startItem, endItem, label);
    int result = stub::MediaService::GetInstance()->GetFolderItems(
        rawAddr.GetAddress(), scope, startItem, endItem, attributes, label);
    if (result != RET_NO_ERROR) {
        if (scope == AVRC_MEDIA_SCOPE_PLAYER_LIST) {
            std::vector<AvrcMpItem> items;
            profile_->SendGetFolderItemsRsp(rawAddr, 0x00, items, label, AVRC_ES_CODE_INTERNAL_ERROR);
        } else {
            std::vector<AvrcMeItem> items;
            profile_->SendGetFolderItemsRsp(rawAddr, 0x00, items, label, AVRC_ES_CODE_INTERNAL_ERROR);
        }
    }
}

void AvrcpTgService::OnGetMediaPlayers(
    const RawAddress &rawAddr, uint16_t uidCounter, const std::vector<AvrcMpItem> &items, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, uidCounter:%{public}d, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, label, status);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnGetMediaPlayersNative, this, peerAddr, uidCounter, items, label, status));
    } while (false);
}

void AvrcpTgService::OnGetMediaPlayersNative(
    RawAddress rawAddr, uint16_t uidCounter, std::vector<AvrcMpItem> items, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, uidCounter:%{public}d, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, label, status);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetFolderItemsRsp(rawAddr, uidCounter, items, label, status);
    } while (false);
}

void AvrcpTgService::OnGetFolderItems(
    const RawAddress &rawAddr, uint16_t uidCounter, const std::vector<AvrcMeItem> &items, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, uidCounter:%{public}d, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, label, status);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnGetFolderItemsNative, this, peerAddr, uidCounter, items, label, status));
    } while (false);
}

void AvrcpTgService::OnGetFolderItemsNative(
    RawAddress rawAddr, uint16_t uidCounter, std::vector<AvrcMeItem> items, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, uidCounter:%{public}d, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, label, status);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetFolderItemsRsp(rawAddr, uidCounter, items, label, status);
    } while (false);
}

void AvrcpTgService::GetItemAttributes(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter,
    std::vector<uint32_t> attributes, uint8_t label) const
{
    HILOGI("addr:%{public}s, scope:%{public}d, uid:%{public}llu, uidCounter:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, static_cast<unsigned long long>(uid), uidCounter, label);
    int result = stub::MediaService::GetInstance()->GetItemAttributes(
        rawAddr.GetAddress(), scope, uid, uidCounter, attributes, label);
    if (result != RET_NO_ERROR) {
        std::vector<uint32_t> attrs;
        std::vector<std::string> vals;
        profile_->SendGetItemAttributesRsp(rawAddr, attrs, vals, label, AVRC_ES_CODE_INTERNAL_ERROR);
    }
}

void AvrcpTgService::OnGetItemAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
    const std::vector<std::string> &values, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, label:%{public}d, status:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, status);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnGetItemAttributesNative, this, peerAddr, attributes, values, label, status));
    } while (false);
}

void AvrcpTgService::OnGetItemAttributesNative(
    RawAddress rawAddr, std::vector<uint32_t> attributes, std::vector<std::string> values, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, label:%{public}d, status:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, status);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetItemAttributesRsp(rawAddr, attributes, values, label, status);
    } while (false);
}

void AvrcpTgService::GetTotalNumberOfItems(const RawAddress &rawAddr, uint8_t scope, uint8_t label) const
{
    int result = stub::MediaService::GetInstance()->GetTotalNumberOfItems(rawAddr.GetAddress(), scope, label);
    if (result != RET_NO_ERROR) {
        profile_->SendGetTotalNumberOfItemsRsp(rawAddr, 0x00, 0x00, label, AVRC_ES_CODE_INTERNAL_ERROR);
    }
}

void AvrcpTgService::OnGetTotalNumberOfItems(
    const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, uidCounter:%{public}d, numOfItems:%{public}u, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, numOfItems, label, status);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(
            &AvrcpTgService::OnGetTotalNumberOfItemsNative, this, peerAddr, uidCounter, numOfItems, label, status));
    } while (false);
}

void AvrcpTgService::OnGetTotalNumberOfItemsNative(
    RawAddress rawAddr, uint16_t uidCounter, uint32_t numOfItems, uint8_t label, int status)
{
    HILOGI("addr:%{public}s, uidCounter:%{public}d, numOfItems:%{public}u, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, numOfItems, label, status);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendGetTotalNumberOfItemsRsp(rawAddr, uidCounter, numOfItems, label, status);
    } while (false);
}

/******************************************************************
 * ABSOLUTE VOLUME                                                *
 ******************************************************************/

void AvrcpTgService::SetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, uint8_t label) const
{
    HILOGI("addr:%{public}s, volume:%{public}d, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume, label);
    int result = stub::MediaService::GetInstance()->SetAbsoluteVolume(rawAddr.GetAddress(), volume, label);
    if (result != RET_NO_ERROR) {
        profile_->SendSetAbsoluteVolumeRsp(rawAddr, AVRC_ABSOLUTE_VOLUME_PERCENTAGE_0, label, result);
    }
}

void AvrcpTgService::OnSetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, uint8_t label)
{
    HILOGI("addr:%{public}s, volume:%{public}d, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::OnSetAbsoluteVolumeNative, this, peerAddr, volume, label));
    } while (false);
}

void AvrcpTgService::OnSetAbsoluteVolumeNative(RawAddress rawAddr, uint8_t volume, uint8_t label)
{
    HILOGI("addr:%{public}s, volume:%{public}d, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendSetAbsoluteVolumeRsp(rawAddr, volume, label, RET_NO_ERROR);
    } while (false);
}

/******************************************************************
 * NOTIFICATION                                                   *
 ******************************************************************/

void AvrcpTgService::GetSelectedTrack(const RawAddress &rawAddr, uint8_t label) const
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    int result = stub::MediaService::GetInstance()->GetSelectedTrack(rawAddr.GetAddress(), label);
    if (result != RET_NO_ERROR) {
        profile_->SendTrackChangedRsp(true, 0xFFFFFFFFFFFFFFFF, label, result);
    }
}

void AvrcpTgService::OnGetSelectedTrack(const RawAddress &rawAddr, uint64_t uid, uint8_t label)
{
    HILOGI("addr:%{public}s, uid:%{public}llu, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), static_cast<unsigned long long>(uid), label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::OnGetSelectedTrackNative, this, peerAddr, uid, label));
    } while (false);
}

void AvrcpTgService::OnGetSelectedTrackNative(RawAddress rawAddr, uint64_t uid, uint8_t label)
{
    HILOGI("addr:%{public}s, uid:%{public}llu, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), static_cast<unsigned long long>(uid), label);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendTrackChangedRsp(true, uid, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::GetAddressedPlayer(const RawAddress &rawAddr, uint8_t label) const
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    int result = stub::MediaService::GetInstance()->GetAddressedPlayer(rawAddr.GetAddress(), label);
    if (result != RET_NO_ERROR) {
        profile_->SendAddressedPlayerChangedRsp(true, 0xFFFF, 0xFFFF, label, result);
    }
}

void AvrcpTgService::OnGetAddressedPlayer(
    const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, uint8_t label)
{
    HILOGI("addr:%{public}s, playerId:%{public}d, uidCounter:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId, uidCounter, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnGetAddressedPlayerNative, this, peerAddr, playerId, uidCounter, label));
    } while (false);
}

void AvrcpTgService::OnGetAddressedPlayerNative(
    RawAddress rawAddr, uint16_t playerId, uint16_t uidCounter, uint8_t label)
{
    HILOGI("addr:%{public}s, playerId:%{public}d, uidCounter:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId, uidCounter, label);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendAddressedPlayerChangedRsp(true, playerId, uidCounter, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::GetUidCounter(const RawAddress &rawAddr, uint8_t label) const
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    int result = stub::MediaService::GetInstance()->GetUidCounter(rawAddr.GetAddress(), label);
    if (result != RET_NO_ERROR) {
        profile_->SendUidsChangedRsp(true, 0xFFFF, label, result);
    }
}

void AvrcpTgService::OnGetUidCounter(const RawAddress &rawAddr, uint16_t uidCounter, uint8_t label)
{
    HILOGI("addr:%{public}s, uidCounter:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::OnGetUidCounterNative, this, peerAddr, uidCounter, label));
    } while (false);
}

void AvrcpTgService::OnGetUidCounterNative(const RawAddress &rawAddr, uint16_t uidCounter, uint8_t label)
{
    HILOGI("addr:%{public}s, uidCounter:%{public}d, label:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, label);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendUidsChangedRsp(true, uidCounter, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::GetCurrentAbsoluteVolume(const RawAddress &rawAddr, uint8_t label) const
{
    HILOGI("addr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    int result = stub::MediaService::GetInstance()->GetCurrentAbsoluteVolume(rawAddr.GetAddress(), label);
    if (result != RET_NO_ERROR) {
        profile_->SendVolumeChangedRsp(true, AVRC_ABSOLUTE_VOLUME_INVALID, label, result);
    }
}

void AvrcpTgService::OnGetCurrentAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, uint8_t label)
{
    HILOGI("addr:%{public}s, volume:%{public}d, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume, label);
    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        RawAddress peerAddr(rawAddr.GetAddress());
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnGetCurrentAbsoluteVolumeNative, this, peerAddr, volume, label));
    } while (false);
}

void AvrcpTgService::OnGetCurrentAbsoluteVolumeNative(const RawAddress &rawAddr, uint8_t volume, uint8_t label)
{
    HILOGI("addr:%{public}s, volume:%{public}d, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        if (GetDeviceState(rawAddr) != static_cast<int>(BTConnectState::CONNECTED)) {
            break;
        }

        profile_->SendVolumeChangedRsp(true, volume, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::SetPlaybackInterval(const RawAddress &rawAddr, uint32_t interval) const
{
    HILOGI("addr:%{public}s, interval:%{public}u", GET_ENCRYPT_AVRCP_ADDR(rawAddr), interval);

    stub::MediaService::GetInstance()->SetPlaybackInterval(rawAddr.GetAddress(), interval);
}

void AvrcpTgService::NotifyPlaybackStatusChanged(uint8_t playStatus, uint32_t playbackPos, uint8_t label)
{
    HILOGI("playStatus:%{public}d, playbackPos:%{public}u, label:%{public}d", playStatus, playbackPos, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::NotifyPlaybackStatusChangedNative, this, playStatus, playbackPos, label));
    } while (false);
}
void AvrcpTgService::NotifyPlaybackStatusChangedNative(uint8_t playStatus, uint32_t playbackPos, uint8_t label)
{
    HILOGI("playStatus:%{public}d, playbackPos:%{public}u, label:%{public}d", playStatus, playbackPos, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        profile_->SendPlaybackStatusChangedRsp(false, playStatus, label, RET_NO_ERROR);
        profile_->SendPlaybackPosChangedRsp(false, playbackPos, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::NotifyTrackChanged(uint64_t uid, uint32_t playbackPos, uint8_t label)
{
    HILOGI("uid:%{public}llu, playbackPos:%{public}u, label:%{public}d",
        static_cast<unsigned long long>(uid), playbackPos, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::NotifyTrackChangedNative, this, uid, playbackPos, label));
    } while (false);
}

void AvrcpTgService::NotifyTrackChangedNative(uint64_t uid, uint32_t playbackPos, uint8_t label)
{
    HILOGI("uid:%{public}llu, playbackPos:%{public}u, label:%{public}d",
        static_cast<unsigned long long>(uid), playbackPos, label);
    do {
        if (!IsEnabled()) {
            break;
        }

        profile_->SendTrackChangedRsp(false, uid, label, RET_NO_ERROR);
        profile_->SendPlaybackPosChangedRsp(false, playbackPos, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::NotifyTrackReachedEnd(uint32_t playbackPos, uint8_t label)
{
    HILOGI("playbackPos:%{public}u, label:%{public}d", playbackPos, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::NotifyTrackReachedEndNative, this, playbackPos, label));
    } while (false);
}

void AvrcpTgService::NotifyTrackReachedEndNative(uint32_t playbackPos, uint8_t label)
{
    HILOGI("playbackPos:%{public}u, label:%{public}d", playbackPos, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        profile_->SendTrackReachedEndRsp(false, label, RET_NO_ERROR);
        profile_->SendPlaybackPosChangedRsp(false, playbackPos, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::NotifyTrackReachedStart(uint32_t playbackPos, uint8_t label)
{
    HILOGI("playbackPos:%{public}u, label:%{public}d", playbackPos, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::NotifyTrackReachedStartNative, this, playbackPos, label));
    } while (false);
}

void AvrcpTgService::NotifyTrackReachedStartNative(uint32_t playbackPos, uint8_t label)
{
    HILOGI("playbackPos:%{public}u, label:%{public}d", playbackPos, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        profile_->SendTrackReachedStartRsp(false, label, RET_NO_ERROR);
        profile_->SendPlaybackPosChangedRsp(false, playbackPos, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::NotifyPlaybackPosChanged(uint32_t playbackPos, uint8_t label)
{
    HILOGI("playbackPos:%{public}u, label:%{public}d", playbackPos, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::NotifyPlaybackPosChangedNative, this, playbackPos, label));
    } while (false);
}

void AvrcpTgService::NotifyPlaybackPosChangedNative(uint32_t playbackPos, uint8_t label)
{
    HILOGI("playbackPos:%{public}u, label:%{public}d", playbackPos, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        profile_->SendPlaybackPosChangedRsp(false, playbackPos, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::NotifyPlayerAppSettingChanged(
    const std::deque<uint8_t> &attributes, const std::deque<uint8_t> &values, uint8_t label)
{
    HILOGI("label:%{public}d", label);

    do {
        if (!IsEnabled()) {
            break;
        }

        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::NotifyPlayerAppSettingChangedNative, this, attributes, values, label));
    } while (false);
}

void AvrcpTgService::NotifyPlayerAppSettingChangedNative(
    std::deque<uint8_t> attributes, std::deque<uint8_t> values, uint8_t label)
{
    HILOGI("label:%{public}d", label);

    do {
        if (!IsEnabled()) {
            break;
        }

        profile_->SendPlayerApplicationSettingChangedRsp(false, attributes, values, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::NotifyNowPlayingContentChanged(uint8_t label)
{
    HILOGI("label:%{public}d", label);

    do {
        if (!IsEnabled()) {
            break;
        }

        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::NotifyNowPlayingContentChangedNative, this, label));
    } while (false);
}

void AvrcpTgService::NotifyNowPlayingContentChangedNative(uint8_t label)
{
    HILOGI("label:%{public}d", label);

    do {
        if (!IsEnabled()) {
            break;
        }
        profile_->SendNowPlayingContentChangedRsp(false, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::NotifyAvailablePlayersChanged(uint8_t label)
{
    HILOGI("label:%{public}d", label);

    do {
        if (!IsEnabled()) {
            break;
        }

        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::NotifyAvailablePlayersChangedNative, this, label));
    } while (false);
}

void AvrcpTgService::NotifyAvailablePlayersChangedNative(uint8_t label)
{
    HILOGI("label:%{public}d", label);

    do {
        if (!IsEnabled()) {
            break;
        }
        profile_->SendAvailablePlayersChangedRsp(false, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::NotifyAddressedPlayerChanged(uint16_t playerId, uint16_t uidCounter, uint8_t label)
{
    HILOGI("playerId:%{public}d, uidCounter:%{public}d, label:%{public}d", playerId, uidCounter, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::NotifyAddressedPlayerChangedNative, this, playerId, uidCounter, label));
    } while (false);
}

void AvrcpTgService::NotifyAddressedPlayerChangedNative(uint16_t playerId, uint16_t uidCounter, uint8_t label)
{
    HILOGI("playerId:%{public}d, uidCounter:%{public}d, label:%{public}d", playerId, uidCounter, label);

    do {
        if (!IsEnabled()) {
            break;
        }
        profile_->SendAddressedPlayerChangedRsp(false, playerId, uidCounter, label, RET_NO_ERROR);
        profile_->SendPlaybackStatusChangedRsp(false, AVRC_PLAY_STATUS_ERROR, label, RET_BAD_STATUS);
        profile_->SendTrackChangedRsp(true, 0xFFFFFFFFFFFFFFFF, label, RET_BAD_STATUS);
        profile_->SendTrackReachedEndRsp(false, label, RET_BAD_STATUS);
        profile_->SendTrackReachedStartRsp(false, label, RET_BAD_STATUS);
        profile_->SendPlaybackPosChangedRsp(false, AVRC_PLAY_STATUS_INVALID_SONG_POSITION, label, RET_BAD_STATUS);
        std::deque<uint8_t> attrs;
        std::deque<uint8_t> vals;
        profile_->SendPlayerApplicationSettingChangedRsp(true, attrs, vals, label, RET_BAD_STATUS);
        profile_->SendNowPlayingContentChangedRsp(true, label, RET_BAD_STATUS);
    } while (false);
}

void AvrcpTgService::NotifyUidChanged(uint16_t uidCounter, uint8_t label)
{
    HILOGI("uidCounter:%{public}d, label:%{public}d", uidCounter, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        GetDispatcher()->PostTask(std::bind(&AvrcpTgService::NotifyUidChangedNative, this, uidCounter, label));
    } while (false);
}

void AvrcpTgService::NotifyUidChangedNative(uint16_t uidCounter, uint8_t label)
{
    HILOGI("uidCounter:%{public}d, label:%{public}d", uidCounter, label);

    do {
        if (!IsEnabled()) {
            break;
        }

        profile_->SendUidsChangedRsp(false, uidCounter, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::NotifyVolumeChanged(uint8_t volume, uint8_t label)
{
    {
        HILOGI("volume:%{public}d, label:%{public}d", volume, label);

        do {
            if (!IsEnabled()) {
                break;
            }

            GetDispatcher()->PostTask(std::bind(&AvrcpTgService::NotifyVolumeChangedNative, this, volume, label));
        } while (false);
    }
}

void AvrcpTgService::NotifyVolumeChangedNative(uint8_t volume, uint8_t label)
{
    HILOGI("volume:%{public}d, label:%{public}d", volume, label);

    do {
        if (!IsEnabled()) {
            break;
        }
        profile_->SendVolumeChangedRsp(false, volume, label, RET_NO_ERROR);
    } while (false);
}

void AvrcpTgService::OnSessionCreate(std::string sessionId)
{
    HILOGI("sessionId:%{public}s", sessionId.c_str());
}

void AvrcpTgService::OnSessionRelease(std::string sessionId)
{
    HILOGI("sessionId:%{public}s", sessionId.c_str());
#ifdef AVRCP_AVSESSION
    if (avSessionController_ != NULL && avSessionController_->GetSessionId().compare(sessionId) == 0) {
        avSessionController_->Destroy();
        avSessionController_ = NULL;
    }
#endif
}

void AvrcpTgService::OnTopSessionChange(std::string sessionId)
{
    HILOGI("sessionId:%{public}s", sessionId.c_str());
#ifdef AVRCP_AVSESSION
    if (avSessionController_ != NULL) {
        if (avSessionController_->GetSessionId().compare(sessionId) == 0) {
            return;
        }
        avSessionController_->Destroy();
    }
    auto res = OHOS::AVSession::AVSessionManager::GetInstance().CreateController(sessionId, avSessionController_);
    if (res == OHOS::AVSession::AVSESSION_SUCCESS && avSessionController_ != NULL) {
        avSessionController_->RegisterCallback(avControllerObserver_);
        OHOS::AVSession::AVPlaybackState::PlaybackStateMaskType filter;
        filter.set(OHOS::AVSession::AVPlaybackState::PLAYBACK_KEY_STATE);
        avSessionController_->SetPlaybackFilter(filter);
    } else {
        HILOGE("avSessionController_ is NULL");
    }
#endif
}

void AvrcpTgService::OnPlaybackStateChange(const int32_t state)
{
    HILOGI("state:%{public}d", state);

    do {
        if (!IsEnabled()) {
            break;
        }
        GetDispatcher()->PostTask(
            std::bind(&AvrcpTgService::OnPlaybackStateChangeNative, this, AvrcpTgService::ConvertPlayState(state)));
    } while (false);
}

void AvrcpTgService::OnPlaybackStateChangeNative(int32_t state)
{
    HILOGI("state:%{public}d", state);
    std::pair<bool, uint8_t> playback = profile_->GetNotificationLabel(AVRC_EVENT_ID_PLAYBACK_STATUS_CHANGED);
    if (!playback.first) {
        HILOGE("device is not register for playStatus updates");
        return;
    }

    NotifyPlaybackStatusChanged(state, 0, playback.second);
}

void AvrcpTgService::ProcessChannelEvent(
    RawAddress rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context)
{
    HILOGI("addr:%{public}s, connectId:%{public}d, event:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), connectId, event, result);

    if (!IsDisabled()) {
        profile_->ProcessChannelEvent(rawAddr, connectId, event, result, context);
    }
}

void AvrcpTgService::ProcessChannelMessage(
    uint8_t connectId, uint8_t label, uint8_t crType, uint8_t chType, Packet *pkt, void *context)
{
    HILOGI("connectId:%{public}d, label:%{public}d, crType:%{public}d, chType:%{public}d",
        connectId, label, crType, chType);
    if (!IsDisabled()) {
        profile_->ProcessChannelMessage(connectId, label, crType, chType, pkt, context);
    }
}

void AvrcpTgService::ChannelEventCallback(
    uint8_t connectId, uint8_t event, uint16_t result, const BtAddr *btAddr, void *context)
{
    HILOGI("connectId:%{public}d, event:%{public}d, result:%{public}d", connectId, event, result);
    auto servManager = IProfileManager::GetInstance();
    auto service = static_cast<AvrcpTgService *>(servManager->GetProfileService(PROFILE_NAME_AVRCP_TG));
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
            std::bind(&AvrcpTgService::ProcessChannelEvent, service, rawAddr, connectId, event, result, context));
    }
}

void AvrcpTgService::ChannelMessageCallback(
    uint8_t connectId, uint8_t label, uint8_t crType, uint8_t chType, Packet *pkt, void *context)
{
    HILOGI("connectId:%{public}d, label:%{public}d, crType:%{public}d, chType:%{public}d",
        connectId, label, crType, chType);
    auto servManager = IProfileManager::GetInstance();
    auto service = static_cast<AvrcpTgService *>(servManager->GetProfileService(PROFILE_NAME_AVRCP_TG));
    auto myPkt = PacketRefMalloc(pkt);

    if (service != nullptr) {
        service->GetDispatcher()->PostTask(std::bind(
            &AvrcpTgService::ProcessChannelMessage, service, connectId, label, crType, chType, myPkt, context));
    }
}
bool AvrcpTgService::CheckConnectionNum()
{
    if (++currentConn_ > maxConnection_) {
        currentConn_ = maxConnection_;
        return false;
    } else {
        return true;
    }
}

uint8_t AvrcpTgService::ConvertPlayState(const int32_t state) const
{
    HILOGI("state:%{public}d", state);
    uint8_t ret = AVRC_PLAY_STATUS_ERROR;
#ifdef AVRCP_AVSESSION
    switch (state) {
        case OHOS::AVSession::AVPlaybackState::PLAYBACK_STATE_INITIAL:
        case OHOS::AVSession::AVPlaybackState::PLAYBACK_STATE_PREPARING:
        case OHOS::AVSession::AVPlaybackState::PLAYBACK_STATE_MAX:
            break;
        case OHOS::AVSession::AVPlaybackState::PLAYBACK_STATE_PLAYING:
            ret = AVRC_PLAY_STATUS_PLAYING;
            break;
        case OHOS::AVSession::AVPlaybackState::PLAYBACK_STATE_PAUSED:
            ret = AVRC_PLAY_STATUS_PAUSED;
            break;
        case OHOS::AVSession::AVPlaybackState::PLAYBACK_STATE_FAST_FORWARD:
            ret = AVRC_PLAY_STATUS_FWD_SEEK;
            break;
        case OHOS::AVSession::AVPlaybackState::PLAYBACK_STATE_REWIND:
            ret = AVRC_PLAY_STATUS_REV_SEEK;
            break;
        case OHOS::AVSession::AVPlaybackState::PLAYBACK_STATE_STOP:
            ret = AVRC_PLAY_STATUS_STOPPED;
            break;
        default:
            break;
    }
#endif
    return ret;
}
REGISTER_CLASS_CREATOR(AvrcpTgService);
}  // namespace bluetooth
}  // namespace OHOS
