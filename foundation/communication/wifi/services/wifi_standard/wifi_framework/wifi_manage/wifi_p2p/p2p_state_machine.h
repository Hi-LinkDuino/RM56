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
#ifndef OHOS_P2P_STATE_MACHINE_H
#define OHOS_P2P_STATE_MACHINE_H

#include "state_machine.h"
#include "ip2p_service_callbacks.h"
#include "p2p_define.h"
#include "p2p_monitor.h"
#include "wifi_p2p_service_manager.h"
#include "i_dhcp_result_notify.h"
#include "dhcp_service.h"
#include "dhcpd_interface.h"
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
#include "abstract_ui.h"
#include "wifi_hid2d_service_utils.h"

namespace OHOS {
namespace Wifi {
const int MIN_GROUP_NAME_LENGTH = 9;
const int MAX_GROUP_NAME_LENGTH = 32;
const int DISC_TIMEOUT_S = 120;
class P2pStateMachine : public StateMachine {
    class DhcpResultNotify : public IDhcpResultNotify {
    public:
        /**
         * @Description : Construct a new dhcp result notify object.
         *
         */
        explicit DhcpResultNotify(P2pStateMachine *p2pStateMachine, WifiP2pGroupManager &groupMgr);

        /**
         * @Description : Destroy the dhcp result notify object.
         *
         */
        ~DhcpResultNotify() override;

        /**
         * @Description - Asynchronous get dhcp result of specified interface success notify.
         *
         * @param status - int
         * @param ifname - interface name,eg:wlan0
         * @param result - dhcp result
         */
        void OnSuccess(int status, const std::string &ifname, DhcpResult &result) override;

        /**
         * @Description - Asynchronous get dhcp result of specified interface failed notify.
         *
         * @param status - int
         * @param ifname - interface name,eg:wlan0
         * @param reason - failed reason
         */
        void OnFailed(int status, const std::string &ifname, const std::string &reason) override;

        /**
        * @Description - Get the abnormal exit notify of dhcp server process
        *
        * @param ifname - interface name,eg:wlan0
        */
        void OnSerExitNotify(const std::string& ifname) override;
    private:
        P2pStateMachine *pP2pStateMachine;
        WifiP2pGroupManager &groupManager;
    };

    friend class AuthorizingNegotiationRequestState;
    friend class GroupFormedState;
    friend class GroupNegotiationState;
    friend class InvitationReceivedState;
    friend class InvitationRequestState;
    friend class P2pDefaultState;
    friend class P2pDisabledState;
    friend class P2pDisablingState;
    friend class P2pEnabledState;
    friend class P2pEnablingState;
    friend class P2pGroupFormationState;
    friend class P2pGroupJoinState;
    friend class P2pGroupOperatingState;
    friend class P2pIdleState;
    friend class P2pInvitingState;
    friend class ProvisionDiscoveryState;
    FRIEND_GTEST(P2pStateMachine);

public:
    /**
     * @Description Construct a new P2pStateMachine object.
     */
    P2pStateMachine(P2pMonitor &, WifiP2pGroupManager &, WifiP2pDeviceManager &, WifiP2pServiceManager &,
        AuthorizingNegotiationRequestState &, GroupFormedState &, GroupNegotiationState &, InvitationReceivedState &,
        InvitationRequestState &, P2pDefaultState &, P2pDisabledState &, P2pDisablingState &, P2pEnabledState &,
        P2pEnablingState &, P2pGroupFormationState &, P2pGroupJoinState &, P2pGroupOperatingState &, P2pIdleState &,
        P2pInvitingState &, ProvisionDiscoveryState &);
    /**
     * @Description Destroy the P2pStateMachine object.
     */
    ~P2pStateMachine();
    /**
     * @Description - Initialize the state machine.
     */
    virtual void Initialize();
public:
    /**
     * @Description - Callbacks of event registered by the p2pService.
     * @param  callback - event callback object
     */
    virtual void RegisterP2pServiceCallbacks(const IP2pServiceCallbacks &callback);

    /**
     * @Description - Set is need dhcp.
     * @param  isNeedDhcp - true: need, false: not need
     */
    void SetIsNeedDhcp(bool isNeedDhcp);

private:
    /**
     * @Description Handle event of CMD_DEVICE_DISCOVERS
     .
     *
     */
    virtual void HandlerDiscoverPeers();

private:
    /**
     * @Description - Register handler with the monitor.
     */
    virtual void RegisterEventHandler();

    /**
     * @Description - Update device status and broadcast device status change events.
     * @param  status - new device status
     */
    virtual void UpdateOwnDevice(P2pDeviceStatus status);
    /**
     * @Description - Update persistent groups and broadcast persistent groups status update event.
     */
    virtual void UpdatePersistentGroups() const;

    /**
     * @Description - Determines whether the group is a persistent group.
                      If the group is a persistent group, the system attempts to reinvoke the group.
     * @param  config - determine the configurations related to the persistent group
     * @return - bool  true:It is a persistent group and the reinvoke succeeds.
                       false:Not a persistent group or reinvoke failure.
     */
    virtual bool ReawakenPersistentGroup(WifiP2pConfigInternal &config) const;

    /**
     * @Description - Updates the latest device information based on the device address and returns.
     * @param  deviceAddr - the device address
     * @return - WifiP2pDevice
     */
    virtual WifiP2pDevice FetchNewerDeviceInfo(const std::string &deviceAddr) const;
    /**
     * @Description - Handle group creation failures.
     */
    virtual void DealGroupCreationFailed();

    /**
     * @Description - Remove a group based on the network ID.
     * @param  networkId - specifies the network ID of a group
     */
    virtual void RemoveGroupByNetworkId(int networkId) const;
    /**
     * @Description Updating the WifiP2pLinkedInfo information when a group is formed.
     * @param  groupOwnerAddress - address of the group owner
     */
    virtual void SetWifiP2pInfoWhenGroupFormed(const std::string &groupOwnerAddress);

    /**
     * @Description - Initialize own device information.
     */
    virtual void InitializeThisDevice();
    /**
     * @Description Check whether the specified network name is available.
     *
     * @param nwName - specified network name
     * @return true - available
     * @return false - not available
     */
    virtual bool IsUsableGroupName(std::string nwName);
    /**
     * @Description Check whether the specified P2P configuration is unavailable.
     *
     * @param config - specified P2P configuration
     * @return P2pConfigErrCode - error code of WifiP2pConfig
     */
    virtual P2pConfigErrCode IsConfigUnusable(const WifiP2pConfigInternal &config);
    /**
     * @Description If the P2P is configured with a network name and passphrase, the configuration is valid as a group.
     *
     * @param config - P2P config
     * @return true - valid
     * @return false - invalid
     */
    virtual bool IsConfigUsableAsGroup(WifiP2pConfigInternal config);
    /**
     * @Description Purging service discovery requests in WPAS.
     *
     */
    virtual void CancelSupplicantSrvDiscReq();
    /**
     * @Description Change the current P2P connection status.
     *
     * @param connectedState status of the connection
     */
    virtual void ChangeConnectedStatus(P2pConnectedState connectedState);
    /**
     * @Description Clear P2P connection information.
     *
     */
    virtual void ClearWifiP2pInfo();
    /**
     * @Description Start dhcp client.
     *
     */
    virtual void StartDhcpClient();
    /**
     * @Description Processing the p2p service response result.
     *
     * @param resp - result of service response
     * @param dev - source device
     */
    virtual void HandleP2pServiceResp(const WifiP2pServiceResponse &resp, const WifiP2pDevice &dev) const;
    /**
     * @Description Get the list of frequencies by band.
     *
     * @param band - specified band
     * @return int - a random available frequency
     */
    virtual int GetAvailableFreqByBand(GroupOwnerBand band) const;
    /**
     * @Description Setting the group's configuration to WPA.
     *
     * @param config - p2p group's configuration
     * @param newGroup - indicates whether the group is a new group
     * @return true - all settings succeeded
     * @return false - one of settings failed
     */
    virtual bool SetGroupConfig(const WifiP2pConfigInternal &config, bool newGroup) const;
    /**
     * @Description Processing function of using configuration to create a group.
     *
     * @param config - p2p group's configuration
     * @param freq - the frequency when starting a group
     * @return true - created successfully
     * @return false - creation failed
     */
    virtual bool DealCreateNewGroupWithConfig(const WifiP2pConfigInternal &config, int freq) const;
    /**
     * @Description Update persistent group's info to wpa.
     *
     */
    virtual void UpdateGroupInfoToWpa() const;

    /**
     * @Description Get is need dhcp.
     *
     */
    bool GetIsNeedDhcp() const;

private:
    /**
     * @Description - Broadcast state change event.
     * @param  state - current state
     */
    virtual void BroadcastP2pStatusChanged(P2pState state) const;
    /**
     * @Description - Peers update detected by broadcast.
     */
    virtual void BroadcastP2pPeersChanged() const;
    /**
     * @Description - Broadcast the scanned service information update.
     */
    virtual void BroadcastP2pServicesChanged() const;
    /**
     * @Description - Broadcast connection event changes.
     */
    virtual void BroadcastP2pConnectionChanged() const;
    /**
     * @Description - Broadcast own device change event.
     * @param  device - own device information
     */
    virtual void BroadcastThisDeviceChanaged(const WifiP2pDevice &device) const;
    /**
     * @Description - Broadcast Discovery status.
     * @param  isActive - current status
     */
    virtual void BroadcastP2pDiscoveryChanged(bool isActive) const;
    /**
     * @Description - Broadcast persistent group update event.
     */
    virtual void BroadcastPersistentGroupsChanged() const;
    /**
     * @Description - For asynchronous operations, the operation result needs to be responded asynchronously.
     * @param  action - corresponding action
     * @param  result - action confirmation result
     */
    virtual void BroadcastActionResult(P2pActionCallback action, ErrCode result) const;
    /**
     * @Description Broadcast receive p2p service response result in addition to bonjour and upnp.
     *
     * @param serviceType - protocol type
     * @param respData - service response data
     * @param srcDevice - source device
     */
    virtual void BroadcastServiceResult(P2pServicerProtocolType serviceType,
        const std::vector<unsigned char> &respData, const WifiP2pDevice &srcDevice) const;
    /**
     * @Description Broadcast receive bonjour service response result.
     *
     * @param instName - instance name
     * @param regType - registration type
     * @param srcDevice - source device
     */
    virtual void BroadcastDnsSdServiceResult(
        const std::string &instName, const std::string &regType, const WifiP2pDevice &srcDevice) const;
    /**
     * @Description Broadcast receive the result of bonjour txt record for a service.
     *
     * @param wholeDomainName - the whole domain name
     * @param txtMap - txt record data as a map of key/value pairs
     * @param srcDevice source device
     */
    virtual void BroadcastDnsSdTxtRecordResult(const std::string &wholeDomainName,
        const std::map<std::string, std::string> &txtMap, const WifiP2pDevice &srcDevice) const;
    /**
     * @Description Broadcast receive upnp service response result.
     *
     * @param uniqueServiceNames - the list of unique service names
     * @param srcDevice - source device
     */
    virtual void BroadcastUpnpServiceResult(
        const std::vector<std::string> &uniqueServiceNames, const WifiP2pDevice &srcDevice) const;

    /**
     * @Description - Start the dhcp server and save the IP address to be assigned.
     * @return true: success   false: fail
     */
    virtual bool StartDhcpServer();

    /**
     * @Description - Stop the dhcp server.
     * @return true: success   false: fail
     */
    virtual bool StopDhcpServer();
private:
    virtual void NotifyUserInvitationSentMessage(const std::string &pin, const std::string &peerAddress) const;
    virtual void NotifyUserProvDiscShowPinRequestMessage(const std::string &pin, const std::string &peerAddress);
    virtual void NotifyUserInvitationReceivedMessage();

private:
    virtual void P2pConnectByShowingPin(const WifiP2pConfigInternal &config) const;

private:
    IP2pServiceCallbacks p2pServiceCallbacks;  /* state machine -> service callback */
    std::string p2pIface;               /* P2P iface */
    WifiP2pConfigInternal savedP2pConfig;    /* record P2P config when communicating with the peer device */
    P2pMonitor &p2pMonitor;          /* P2P monitor */
    WifiP2pGroupManager &groupManager;   /* group manager */
    WifiP2pDeviceManager &deviceManager; /* device manager */
    WifiP2pServiceManager &serviceManager;   /* service manager */
    std::unique_ptr<IDhcpService> pDhcpService;
    std::unique_ptr<DhcpResultNotify> pDhcpResultNotify;
    DhcpdInterface m_DhcpdInterface;
    AuthorizingNegotiationRequestState &p2pAuthorizingNegotiationRequestState;
    GroupFormedState &p2pGroupFormedState;
    GroupNegotiationState &p2pGroupNegotiationState;
    InvitationReceivedState &p2pInvitationReceivedState;
    InvitationRequestState &p2pInvitationRequestState;
    P2pDefaultState &p2pDefaultState;
    P2pDisabledState &p2pDisabledState;
    P2pDisablingState &p2pDisablingState;
    P2pEnabledState &p2pEnabledState;
    P2pEnablingState &p2pEnablingState;
    P2pGroupFormationState &p2pGroupFormationState;
    P2pGroupJoinState &p2pGroupJoinState;
    P2pGroupOperatingState &p2pGroupOperatingState;
    P2pIdleState &p2pIdleState;
    P2pInvitingState &p2pInvitingState;
    ProvisionDiscoveryState &p2pProvisionDiscoveryState;
    static bool m_isNeedDhcp;
};
}  // namespace Wifi
}  // namespace OHOS

#endif  // OHOS_P2P_STATE_MACHINE_H
