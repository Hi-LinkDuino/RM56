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
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines map mce observer manager object.
 *
 */

/**
 * @file map_mce_observer_manager.cpp
 *
 * @brief map mce observer manager file .
 *
 */

#include "map_mce_observer_manager.h"
#include "log.h"
#include "interface_profile_map_mce.h"

namespace OHOS {
namespace bluetooth {
MapMceObserverManager::MapMceObserverManager()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MapMceObserverManager::~MapMceObserverManager()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

void MapMceObserverManager::RegisterObserver(IProfileMapMceObserver &serviceCallback)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    observerMgrListMap_.Register(serviceCallback);
}

// rpc force disconnect , distroy all instance stm client
void MapMceObserverManager::DeregisterObserver(IProfileMapMceObserver &serviceCallback)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    observerMgrListMap_.Register(serviceCallback);
}

void MapMceObserverManager::ExcuteObserverOnMapActionCompleted(
    const std::string &device, const IProfileMapAction &action, MapExecuteStatus status)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    RawAddress tmpRawAddr(device);
    observerMgrListMap_.ForEach([tmpRawAddr, action, status](IProfileMapMceObserver &observer) {
        observer.OnMapActionCompleted(tmpRawAddr, action, status);
    });
}

void MapMceObserverManager::ExcuteObserverOnMapEventReported(
    const std::string &device, const IProfileMapEventReport &report)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    RawAddress tmpRawAddr(device);
    observerMgrListMap_.ForEach(
        [tmpRawAddr, report](IProfileMapMceObserver &observer) { observer.OnMapEventReported(tmpRawAddr, report); });
}

void MapMceObserverManager::ExcuteObserverOnConnectionStateChanged(const std::string &device, int state)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    RawAddress tmpRawAddr(device);
    observerMgrListMap_.ForEach([tmpRawAddr, state](IProfileMapMceObserver &observer) {
        observer.OnConnectionStateChanged(tmpRawAddr, state);
    });
}

void MapMceObserverManager::ExcuteObserverOnBmessageCompleted(
    const RawAddress &deviceAddress, const IProfileBMessage &bmsg, MapExecuteStatus status)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    RawAddress tmpRawAddr(deviceAddress);
    observerMgrListMap_.ForEach([tmpRawAddr, bmsg, status](IProfileMapMceObserver &observer) {
        observer.OnBmessageCompleted(tmpRawAddr, bmsg, status);
    });
}

void MapMceObserverManager::ExcuteObserverOnMessagesListingCompleted(
    const RawAddress &deviceAddress, const IProfileMessagesListing &listing, MapExecuteStatus status)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    RawAddress tmpRawAddr(deviceAddress);
    observerMgrListMap_.ForEach([tmpRawAddr, listing, status](IProfileMapMceObserver &observer) {
        observer.OnMessagesListingCompleted(tmpRawAddr, listing, status);
    });
}

void MapMceObserverManager::ExcuteObserverOnConversationListingCompleted(
    const RawAddress &deviceAddress, const IProfileConversationListing &listing, MapExecuteStatus status)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    RawAddress tmpRawAddr(deviceAddress);
    observerMgrListMap_.ForEach([tmpRawAddr, listing, status](IProfileMapMceObserver &observer) {
        observer.OnConversationListingCompleted(tmpRawAddr, listing, status);
    });
}
}  // namespace bluetooth
}  // namespace OHOS
