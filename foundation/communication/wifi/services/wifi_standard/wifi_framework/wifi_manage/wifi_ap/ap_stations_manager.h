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
#ifndef OHOS_STATIONS_MANAGER_H
#define OHOS_STATIONS_MANAGER_H

#include "ap_define.h"
#include "wifi_ap_msg.h"
#include "wifi_error_no.h"

namespace OHOS {
namespace Wifi {
class ApStateMachine;
class ApStationsManager {
public:
    FRIEND_GTEST(ApStationsManager);
    /**
     * @Description  construction.
     * @param None
     * @return None
     */
    ApStationsManager();

    /**
     * @Description  destructor.
     * @param None
     * @return None
     */
    ~ApStationsManager();

    /**
     * @Description  Add a blocklist
     * @param staInfo - Information about station to be added to the blocklist.
     * @return true: Adding succeeded.    false: Failed to add.
     */
    bool AddBlockList(const StationInfo &staInfo) const;

    /**
     * @Description  Deleting a blocklist
     * @param staInfo - Deleting the station information from the blocklist.
     * @return true: Deleted successfully    false: Deletion failed
     */
    bool DelBlockList(const StationInfo &staInfo) const;

    /**
     * @Description  Add all blocklists to the hostapd.
     * @param None
     * @return true: Set successful
               false: Set failed（Obtaining failed，Failed to set one or more blocklists.）
     */
    bool EnableAllBlockList() const;

    /**
     * @Description  Process the station disconnection reported.
     * @param mac - address of the disconnected station
     * @return None
     */
    void StationLeave(const std::string &mac) const;

    /**
     * @Description  Handling New station Connections.
     * @param staInfo - Information about the newly connected STA
     * @return None
     */
    void StationJoin(const StationInfo &staInfo) const;
    /**
     * @Description  Disconnect a specified station..
     * @param mac - address of the newly connected station
     * @return true: Disconnected successfully    false: Disconnected failed
     */
    bool DisConnectStation(const StationInfo &staInfo) const;

    /**
     * @Description  Get the MAC addresses of all connected stations.
     * @param None
     * @return mac addresses of all connected stations
     */
    std::vector<std::string> GetAllConnectedStations() const;

    /**
     * @Description  Register callback list reporting msg to state machine.
     * @param callbacks - callback function
     * @return None
     */
    void RegisterEventHandler(std::function<void(const StationInfo &, ApStatemachineEvent)> callbacks);
private:
    /**
     * @Description  When a new STA is connected, the new STA is
                     recorded in the configuration center.
     * @param staInfo - Associated STA information
     * @return true：Association added successfully
               false：Failed to added the association
     */
    bool AddAssociationStation(const StationInfo &staInfo) const;

    /**
     * @Description  When a STA is disconnected, the connection is
                     recorded in the configuration center.
     * @param staInfo - Information about station to be disconnected
     * @return true: Association deleted successfully.
               false: Failed to delete the association.
     */
    bool DelAssociationStation(const StationInfo &staInfo) const;

    std::function<void(const StationInfo &, ApStatemachineEvent)> m_stationChangeCallback;
};
}  // namespace Wifi
}  // namespace OHOS

#endif