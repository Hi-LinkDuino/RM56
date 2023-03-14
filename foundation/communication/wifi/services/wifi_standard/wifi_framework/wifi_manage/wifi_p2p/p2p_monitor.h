/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_P2P_MONITOR_H
#define OHOS_P2P_MONITOR_H

#include <string>
#include <map>
#include <set>

#include "p2p_macro.h"
#include "p2p_define.h"
#include "wifi_p2p_event_callback.h"
#include "wifi_p2p_service_response_list.h"
#include "wifi_p2p_service_request_list.h"
#include "wifi_p2p_temp_disc_event.h"

namespace OHOS {
namespace Wifi {
class P2pMonitor {
    FRIEND_GTEST(P2pMonitor);

public:
    /**
     * @Description Construct a new P2pMonitor object.
     */
    P2pMonitor();
    /**
     * @Description Destroy the P2pMonitor object.
     */
    virtual ~P2pMonitor();
    /**
     * @Description - Initialize monitor.
     */
    virtual void Initialize();
    /**
     * @Description - Start listening for events.
     * @param  iface - network interface that needs to start listening for events
     */
    virtual void MonitorBegins(const std::string &iface);
    /**
     * @Description - Stop listening on events.
     * @param  iface - network interfaces that need to be stopped from listening to events
     */
    virtual void MonitorEnds(const std::string &iface);
    /**
     * @Description - Registering events handler function.
     * @param  iface - name of the network interface corresponding to the event processing function
     * @param  handler - event processing function
     */
    virtual void RegisterIfaceHandler(const std::string &iface, const std::function<HandlerMethod> &handler);
    /**
     * @Description - Deregistration event processing function.
     * @param  iface - network interfaces that need to be deregistered for event handlers
     */
    virtual void UnregisterHandler(const std::string &iface);

private:
    /**
     * @Description - Sending events and related parameters to state machine.
     * @param  iface - name of the network interface that needs to be processed
     * @param  msgName - event name
     * @param  param1 - general parameter 1
     * @param  param2 - general parameter 1
     * @param  messageObj - data objects to be transferred
     */
    virtual void MessageToStateMachine(const std::string &iface, P2P_STATE_MACHINE_CMD msgName, int param1, int param2,
        const std::any &messageObj) const;
    /**
     * @Description Converting the int type to the P2pStatus type.
     *
     * @param status - type parameter to be converted
     * @return P2pStatus - converted value
     */
    virtual P2pStatus IntStatusToP2pStatus(int status) const;

private:
    /**
     * @Description - Broadcast connection supplicant event.
     *
     * @param  iface - network interface for event processing
     * @param  status - connection result status
     */
    virtual void Broadcast2SmConnectSupplicant(const std::string &iface, int status) const;
    /**
     * @Description Broadcast device found event.
     *
     * @param iface - network interface for event processing
     * @param device - device information
     */
    virtual void Broadcast2SmDeviceFound(const std::string &iface, const WifiP2pDevice &device) const;
    /**
     * @Description Broadcast device lost event.
     *
     * @param iface - network interface for event processing
     * @param device - device information
     */
    virtual void Broadcast2SmDeviceLost(const std::string &iface, const WifiP2pDevice &device) const;
    /**
     * @Description Broadcast the event that the GO negotiation request is received.
     *
     * @param iface - network interface for event processing
     * @param config - config of P2P
     */
    virtual void Broadcast2SmGoNegRequest(const std::string &iface, const WifiP2pConfigInternal &config) const;
    /**
     * @Description Broadcast GO negotiation success event.
     *
     * @param iface - network interface for event processing
     */
    virtual void Broadcast2SmGoNegSuccess(const std::string &iface) const;
    /**
     * @Description Broadcast GO negotiation failure event.
     *
     * @param iface - network interface for event processing
     * @param p2pStatus - cause of GO negotiation failure
     */
    virtual void Broadcast2SmGoNegFailure(const std::string &iface, P2pStatus p2pStatus) const;
    /**
     * @Description Broadcast P2P invitation request event.
     *
     * @param iface - network interface for event processing
     * @param group - group that has received invitations
     */
    virtual void Broadcast2SmInvitationReceived(const std::string &iface, const WifiP2pGroupInfo &group) const;
    /**
     * @Description Broadcast the P2P invitation request result.
     *
     * @param iface - network interface for event processing
     * @param p2pStatus - results of the invitation
     */
    virtual void Broadcast2SmInvitationResult(const std::string &iface, P2pStatus p2pStatus) const;
    /**
     * @Description Broadcast group establishment success event.
     *
     * @param iface - network interface for event processing
     */
    virtual void Broadcast2SmGroupFormationSuccess(const std::string &iface) const;
    /**
     * @Description Broadcast group setup failure event.
     *
     * @param iface - network interface for event processing
     * @param reason - cause of failure
     */
    virtual void Broadcast2SmGroupFormationFailure(const std::string &iface, const std::string &reason) const;
    /**
     * @Description Broadcast group start event.
     *
     * @param iface - network interface for event processing
     * @param group - group information
     */
    virtual void Broadcast2SmGroupStarted(const std::string &iface, const WifiP2pGroupInfo &group) const;
    /**
     * @Description Broadcast group removed event.
     *
     * @param iface - network interface for event processing
     * @param group - group information
     */
    virtual void Broadcast2SmGroupRemoved(const std::string &iface, const WifiP2pGroupInfo &group) const;
    /**
     * @Description Broadcast provision discovery request event.
     *
     * @param iface - network interface for event processing
     * @param event - provision discovery request event
     */
    virtual void Broadcast2SmProvDiscPbcReq(const std::string &iface, const WifiP2pTempDiscEvent &event) const;
    /**
     * @Description Broadcast provision discovery response event.
     *
     * @param iface - network interface for event processing
     * @param event - provision discovery response event
     */
    virtual void Broadcast2SmProvDiscPbcResp(const std::string &iface, const WifiP2pTempDiscEvent &event) const;
    /**
     * @Description Broadcast provision discovery PIN input event.
     *
     * @param iface - network interface for event processing
     * @param event - provision discovery request event
     */
    virtual void Broadcast2SmProvDiscEnterPin(const std::string &iface, const WifiP2pTempDiscEvent &event) const;
    /**
     * @Description Broadcast the provision discovery PIN display event.
     *
     * @param iface - network interface for event processing
     * @param event - provision discovery response event
     */
    virtual void Broadcast2SmProvDiscShowPin(const std::string &iface, const WifiP2pTempDiscEvent &event) const;
    /**
     * @Description Broadcast provision discovery failure event.
     *
     * @param iface - network interface for event processing
     */
    virtual void Broadcast2SmProvDiscFailure(const std::string &iface) const;
    /**
     * @Description Broadcast device search stop event.
     *
     * @param iface - network interface for event processing
     */
    virtual void Broadcast2SmFindStopped(const std::string &iface) const;
    /**
     * @Description Broadcast service request event.
     *
     * @param iface - network interface for event processing
     * @param reqList discovery service request list
     */
    virtual void Broadcast2SmServDiscReq(const std::string &iface, const WifiP2pServiceRequestList &reqList) const;
    /**
     * @Description Broadcast service response event.
     *
     * @param iface - network interface for event processing
     * @param respList discovery service response list
     */
    virtual void Broadcast2SmServDiscResp(const std::string &iface, const WifiP2pServiceResponseList &respList) const;
    /**
     * @Description Broadcast the event that the STA is disconnected from the AP.
     *
     * @param iface - network interface for event processing
     * @param device - device information of the disconnected STA
     */
    virtual void Broadcast2SmApStaDisconnected(const std::string &iface, const WifiP2pDevice &device) const;
    /**
     * @Description Broadcast STA and AP connection event.
     *
     * @param iface - network interface for event processing
     * @param device - device information of the connected STA
     */
    virtual void Broadcast2SmApStaConnected(const std::string &iface, const WifiP2pDevice &device) const;
    /**
     * @Description Broadcast suplicant connects failed event.
     *
     * @param iface - network interface for event processing
     */
    virtual void Broadcast2SmConnectSupplicantFailed(const std::string &iface) const;

private:
    /**
     * @Description - Register the connection supplicant result callback function.
     *
     * @param  status - connection status
     */
    void OnConnectSupplicant(int status) const;
    /**
     * @Description Register device discovery callback function.
     *
     * @param deviceInfo - packets of device discovery
     */
    void WpaEventDeviceFound(const IdlP2pDeviceFound &deviceInfo) const;
    /**
     * @Description Register the callback function for device loss.
     *
     * @param p2pDeviceAddress - The address of the device is lost
     */
    void WpaEventDeviceLost(const std::string &p2pDeviceAddress) const;
    /**
     * @Description Register the GO negotiation request callback function.
     *
     * @param srcAddress - the MAC address of the GO negotiation request device
     * @param passwordId - the password type
     */
    void WpaEventGoNegRequest(const std::string &srcAddress, short passwordId) const;
    /**
     * @Description Register the callback function for successful GO negotiation.
     *
     */
    void WpaEventGoNegSuccess(void) const;
    /**
     * @Description Register the callback function for GO negotiation failure.
     *
     * @param status - result of GO negotiation
     */
    void WpaEventGoNegFailure(int status) const;
    /**
     * @Description Register the callback function of a P2P invite request.
     *
     * @param recvInfo - invitation requested packet
     */
    void WpaEventInvitationReceived(const IdlP2pInvitationInfo &recvInfo) const;
    /**
     * @Description Register the P2P invitation result callback function.
     *
     * @param bssid - group name
     * @param status - result of the invitation
     */
    void WpaEventInvitationResult(const std::string &bssid, int status) const;
    /**
     * @Description Register callback function for group establishment success.
     *
     */
    void WpaEventGroupFormationSuccess(void) const;
    /**
     * @Description Register callback function for group establishment failure.
     *
     * @param failureReason - cause of failure
     */
    void WpaEventGroupFormationFailure(const std::string &failureReason) const;
    /**
     * @Description Register group start callback function.
     *
     * @param groupInfo - group information data packet
     */
    void WpaEventGroupStarted(const IdlP2pGroupInfo &groupInfo) const;
    /**
     * @Description Register callback function for removing a group.
     *
     * @param groupIfName - group interface name
     * @param isGo - whether the device is GO
     */
    void WpaEventGroupRemoved(const std::string &groupIfName, bool isGo) const;
    /**
     * @Description Register provision discovery PBC request callback function.
     *
     * @param p2pDeviceAddress - MAC of the peer device
     */
    void WpaEventProvDiscPbcReq(const std::string &p2pDeviceAddress) const;
    /**
     * @Description Register provision discovery PBC response callback function.
     *
     * @param p2pDeviceAddress - MAC of the peer device
     */
    void WpaEventProvDiscPbcResp(const std::string &p2pDeviceAddress) const;
    /**
     * @Description Register the provision discovery PIN input callback function.
     *
     * @param p2pDeviceAddress - MAC of the peer device
     */
    void WpaEventProvDiscEnterPin(const std::string &p2pDeviceAddress) const;
    /**
     * @Description Register the provision discovery PIN display callback function.
     *
     * @param p2pDeviceAddress - MAC of the peer device
     * @param generatedPin - displayed PIN
     */
    void WpaEventProvDiscShowPin(const std::string &p2pDeviceAddress, const std::string &generatedPin) const;
    /**
     * @Description Register provision discovery failure event.
     *
     */
    void WpaEventProvDiscFailure(void) const;
    /**
     * @Description Register device search stop callback function.
     *
     */
    void WpaEventFindStopped(void) const;
    /**
     * @Description Register service request callback function.
     *
     * @param reqInfo - service request packet
     */
    void WpaEventServDiscReq(const IdlP2pServDiscReqInfo &reqInfo) const;
    /**
     * @Description register service response callback function.
     *
     * @param srcAddress - MAC address of the device issuing the service discovery
     * @param updateIndicator - service update indication
     * @param tlvList - tlvs packet
     */
    void WpaEventServDiscResp(
        const std::string &srcAddress, short updateIndicator, const std::vector<unsigned char> &tlvList) const;
    /**
     * @Description Register the callback function for disconnecting the STA from the AP.
     *
     * @param p2pDeviceAddress - the MAC address of the STA disconnected
     */
    void WpaEventApStaDisconnected(const std::string &p2pDeviceAddress) const;
    /**
     * @Description Register the callback function for connecting STA to AP
     *
     * @param p2pDeviceAddress - the MAC address of the STA connected
     */
    void WpaEventApStaConnected(const std::string &p2pDeviceAddress) const;
    /**
     * @Description Register Wpa_supplicant client connection failure callback function
     *
     */
    void OnConnectSupplicantFailed(void) const;

private:
    /**
     * The current implementation cannot obtain abundant HAL instances like Andoird and cannot distinguish which
     * iace is operated on. Therefore, the iace of the current operation is recorded temporarily the same as that
     * of the HAL operation.
     */
    std::string selectIfacName;
    std::set<std::string> setMonitorIface;
    std::map<std::string, std::function<HandlerMethod>> mapHandler;
};
}  // namespace Wifi
}  // namespace OHOS
#endif