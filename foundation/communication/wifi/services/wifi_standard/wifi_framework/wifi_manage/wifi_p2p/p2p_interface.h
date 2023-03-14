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

#ifndef OHOS_P2P_INTERFACE_H
#define OHOS_P2P_INTERFACE_H

#include "ip2p_service.h"
#include "wifi_p2p_service.h"
#include "p2p_monitor.h"
#include "p2p_state_machine.h"
#include "authorizing_negotiation_request_state.h"
#include "group_formed_state.h"
#include "group_negotiation_state.h"
#include "invitation_received_state.h"
#include "invitation_request_state.h"
#include "p2p_default_state.h"
#include "p2p_disabled_state.h"
#include "p2p_disabling_state.h"
#include "p2p_enabled_state.h"
#include "p2p_enabling_state.h"
#include "p2p_group_formation_state.h"
#include "p2p_group_join_state.h"
#include "p2p_group_operating_state.h"
#include "p2p_idle_state.h"
#include "p2p_inviting_state.h"
#include "provision_discovery_state.h"
#include "wifi_p2p_group_manager.h"

namespace OHOS {
namespace Wifi {
class P2pInterface : public IP2pService {
public:
    /**
     * @Description Construct a new P2pInterface object.
     */
    P2pInterface();

    /**
     * @Description Destroy the P2pInterface object.
     */
    ~P2pInterface() = default;

public:
    /**
     * @Description - Enable the P2P mode.
     * @return - ErrCode
     */
    virtual ErrCode EnableP2p() override;

    /**
     * @Description - Disable the P2P mode.
     * @return - ErrCode
     */
    virtual ErrCode DisableP2p() override;

    /**
     * @Description - Start P2P device discovery.
     * @return - ErrCode
     */
    virtual ErrCode DiscoverDevices() override;

    /**
     * @Description - Stop P2P device discovery.
     * @return - ErrCode
     */
    virtual ErrCode StopDiscoverDevices() override;

    /**
     * @Description - Start P2P services discovery.
     * @return - ErrCode
     */
    virtual ErrCode DiscoverServices() override;

    /**
     * @Description - Stop P2P services discovery.
     * @return - ErrCode
     */
    virtual ErrCode StopDiscoverServices() override;

    /**
     * @Description - Register local P2P service.
     * @param  srvInfo - local service information
     * @return - ErrCode
     */
    virtual ErrCode PutLocalP2pService(const WifiP2pServiceInfo &srvInfo) override;

    /**
     * @Description - Delete local P2P service.
     * @param  srvInfo - local service information
     * @return - ErrCode
     */
    virtual ErrCode DeleteLocalP2pService(const WifiP2pServiceInfo &srvInfo) override;

    /**
     * @Description - Request specified services.
     * @param  device - requested target device
     * @param  request - initiated service request data
     * @return - ErrCode
     */
    virtual ErrCode RequestService(const WifiP2pDevice &device, const WifiP2pServiceRequest &request) override;

    /**
     * @Description - Start the P2P listening. Unit: millisecond.
     * @param  period - listening period
     * @param  interval - listening interval
     * @return - ErrCode
     */
    virtual ErrCode StartP2pListen(int period, int interval) override;

    /**
     * @Description - Stop the P2P listening.
     * @return - ErrCode
     */
    virtual ErrCode StopP2pListen() override;

    /**
     * @Description - Create a P2P group.
     * @param  config - config for creating group
     * @return - ErrCode
     */
    virtual ErrCode CreateGroup(const WifiP2pConfig &config) override;

    /**
     * @Description - Remove the current P2P group.
     * @return - ErrCode
     */
    virtual ErrCode RemoveGroup() override;

    /**
     * @Description - Delete a persistent group.
     * @param  group - specified group
     * @return - ErrCode
     */
    virtual ErrCode DeleteGroup(const WifiP2pGroupInfo &group) override;

    /**
     * @Description - Connect to a P2P device.
     * @param  config - config for connection
     * @return - ErrCode
     */
    virtual ErrCode P2pConnect(const WifiP2pConfig &config) override;

    /**
     * @Description - Canceling a P2P connection.
     * @return - ErrCode
     */
    virtual ErrCode P2pCancelConnect() override;

    /**
     * @Description - Set this device name.
     *
     * @param devName - specified device name
     * @return ErrCode
     */
    virtual ErrCode SetP2pDeviceName(const std::string &devName) override;

    /**
     * @Description - Query P2P connection information.
     * @param  linkedInfo - object that stores connection information
     * @return - ErrCode
     */
    virtual ErrCode QueryP2pLinkedInfo(WifiP2pLinkedInfo &linkedInfo) override;

    /**
     * @Description set p2p wifi display info
     * @param wfdInfo - wifi display info
     * @return ErrCode - operate result
     */
    virtual ErrCode SetP2pWfdInfo(const WifiP2pWfdInfo &wfdInfo) override;

    /**
     * @Description - Get the current group information.
     * @param  group - object that stores the current group
     * @return - ErrCode
     */
    virtual ErrCode GetCurrentGroup(WifiP2pGroupInfo &group) override;

    /**
     * @Description - Obtain the P2P status.
     * @param  status - object that stores P2P status
     * @return - ErrCode
     */
    virtual ErrCode GetP2pEnableStatus(int &status) override;

    /**
     * @Description - Obtain the P2P discovery status.
     * @param  status - object that stores discovery status
     * @return - ErrCode
     */
    virtual ErrCode GetP2pDiscoverStatus(int &status) override;

    /**
     * @Description - Obtain the P2P connection status.
     * @param  status - object that stores connection status
     * @return - ErrCode
     */
    virtual ErrCode GetP2pConnectedStatus(int &status) override;

    /**
     * @Description - Query the information about the found devices.
     * @param  devices - list of queryed device
     * @return - ErrCode
     */
    virtual ErrCode QueryP2pDevices(std::vector<WifiP2pDevice> &devices) override;

    /**
     * @Description - Query the information about own device.
     * @param  device - own device
     * @return - ErrCode
     */
    virtual ErrCode QueryP2pLocalDevice(WifiP2pDevice &device) override;

    /**
     * @Description - Obtain information about all groups.
     * @param  groups - list of group information
     * @return - ErrCode
     */
    virtual ErrCode QueryP2pGroups(std::vector<WifiP2pGroupInfo> &groups) override;

    /**
     * @Description - Query the information about the found services.
     * @param  services - list of service information
     * @return - ErrCode
     */
    virtual ErrCode QueryP2pServices(std::vector<WifiP2pServiceInfo> &services) override;

    /**
     * @Description - Register all callbacks provided by the P2P.
     * @param  callbacks - all callbacks added
     * @return - ErrCode
     */
    virtual ErrCode RegisterP2pServiceCallbacks(const IP2pServiceCallbacks &callbacks) override;

    /**
     * @Description Create hid2d group, used on the GO side.
     *
     * @param frequency - frequency
     * @param type - frequency type
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dCreateGroup(const int frequency, FreqType type) override;

    /**
     * @Description Connect to a specified group using hid2d, used on the GC side.
     *
     * @param config - connection parameters
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dConnect(const Hid2dConnectConfig& config) override;

private:
    WifiP2pGroupManager groupManager;    /* group manager */
    WifiP2pDeviceManager deviceMgr;  /* device manager */
    WifiP2pServiceManager svrMgr;

    AuthorizingNegotiationRequestState p2pAuthorizingNegotiationRequestState;
    GroupFormedState p2pGroupFormedState;
    GroupNegotiationState p2pGroupNegotiationState;
    InvitationReceivedState p2pInvitationReceivedState;
    InvitationRequestState p2pInvitationRequestState;
    P2pDefaultState p2pDefaultState;
    P2pDisabledState p2pDisabledState;
    P2pDisablingState p2pDisablingState;
    P2pEnabledState p2pEnabledState;
    P2pEnablingState p2pEnablingState;
    P2pGroupFormationState p2pGroupFormationState;
    P2pGroupJoinState p2pGroupJoinState;
    P2pGroupOperatingState p2pGroupOperatingState;
    P2pIdleState p2pIdleState;
    P2pInvitingState p2pInvitingState;
    ProvisionDiscoveryState p2pProvisionDiscoveryState;

    P2pMonitor p2pMonitor;
    P2pStateMachine p2pStateMachine;
    WifiP2pService p2pService;
};
}  // namespace Wifi
}  // namespace OHOS

#endif  // OHOS_P2P_INTERFACE_H
