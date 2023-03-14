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
 * @brief Defines map client observer manager object.
 *
 */

/**
 * @file map_mce_service.h
 *
 * @brief map client observer manager header file .
 *
 */

#ifndef MAP_MCE_OBSERVER_MANAGER_H
#define MAP_MCE_OBSERVER_MANAGER_H

#include <string>
#include "base_observer_list.h"
#include "bt_def.h"
#include "interface_profile_map_mce.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief map mce observer manager
 */
class MapMceObserverManager {
public:
    /**
     * @brief Construct a new Map Mce Rcp Callback Mgr object
     * @param  mapService
     */
    MapMceObserverManager();
    /**
     * @brief Destroy the Map Mce Rcp Callback Manager object
     */
    ~MapMceObserverManager();
    /**
     * @brief  regist callback(option)
     * @param  serviceCallback  observer
     * @return int
     */
    void RegisterObserver(IProfileMapMceObserver &serviceCallback);
    /**
     * @brief  deregister callback(option)
     * @param  serviceCallback  observer
     */
    void DeregisterObserver(IProfileMapMceObserver &serviceCallback);
    /**
     * @brief  rpc callack execute
     * @param  device           bluetooth device address
     * @param  action
     * @param  masInstanceId    instance id
     */
    void ExcuteObserverOnMapActionCompleted(
        const std::string &device, const IProfileMapAction &action, MapExecuteStatus status);
    /**
     * @brief  rpc callack execute , return report request execute result
     * @param  device           bluetooth device address
     * @param  report
     */
    void ExcuteObserverOnMapEventReported(const std::string &device, const IProfileMapEventReport &report);
    /**
     * @brief  Execute Observer OnConnectionStateChanged
     * @param  device           bluetooth device address
     * @param  state            status
     */
    void ExcuteObserverOnConnectionStateChanged(const std::string &device, int state);
    /**
     * @brief  Execute Observer OnBmessageCompleted
     * @param  deviceAddress
     * @param  bmsg
     * @param  status
     */
    void ExcuteObserverOnBmessageCompleted(
        const RawAddress &deviceAddress, const IProfileBMessage &bmsg, MapExecuteStatus status);
    /**
     * @brief  Execute Observer OnMessagesListingCompleted
     * @param  deviceAddress
     * @param  listing
     * @param  status
     */
    void ExcuteObserverOnMessagesListingCompleted(
        const RawAddress &deviceAddress, const IProfileMessagesListing &listing, MapExecuteStatus status);
    /**
     * @brief  Execute Observer OnConversationListingCompleted
     * @param  deviceAddress
     * @param  listing
     * @param  status
     */
    void ExcuteObserverOnConversationListingCompleted(
        const RawAddress &deviceAddress, const IProfileConversationListing &listing, MapExecuteStatus status);

private:
    // observer list map
    BaseObserverList<IProfileMapMceObserver> observerMgrListMap_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // MAP_MCE_OBSERVER_MANAGER_H
