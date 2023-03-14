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
#ifndef OHOS_WIFI_STATE_MACHINE_H
#define OHOS_WIFI_STATE_MACHINE_H

#include <regex.h>
#include <sys/types.h>
#include <fstream>
#include <vector>
#include "wifi_internal_msg.h"
#include "wifi_log.h"
#include "wifi_errcode.h"
#include "wifi_msg.h"
#include "state_machine.h"
#include "sta_network_check.h"
#include "i_dhcp_result_notify.h"
#include "sta_service_callback.h"
#include "i_dhcp_service.h"
#include "sta_define.h"
#include "wifi_net_agent.h"

namespace OHOS {
namespace Wifi {
constexpr int STA_CONNECT_MODE = 1;
constexpr int STA_SCAN_ONLY_MODE = 2;
constexpr int STA_CAN_ONLY_WITH_WIFI_OFF_MODE = 3;
constexpr int STA_DISABLED_MODE = 4;

constexpr int CMD_NETWORK_CONNECT_TIMEOUT = 0X01;
constexpr int STA_NETWORK_CONNECTTING_DELAY = 20 * 1000;

constexpr int CMD_SIGNAL_POLL = 0X02;
constexpr int STA_SIGNAL_POLL_DELAY = 3 * 1000;

/* pincode length */
constexpr int PIN_CODE_LEN = 8;

/* DHCP timeout interval */
constexpr int DHCP_TIME = 15;
/* rssi thresholds */
constexpr int INVALID_RSSI_VALUE = -127;
constexpr int MAX_RSSI_VALUE = 200;
constexpr int SIGNAL_INFO = 256;

/* 2.4g and 5g frequency thresholds */
constexpr int FREQ_2G_MIN = 2412;
constexpr int FREQ_2G_MAX = 2472;
constexpr int FREQ_5G_MIN = 5170;
constexpr int FREQ_5G_MAX = 5825;
constexpr int CHANNEL_14_FREQ = 2484;
constexpr int CHANNEL_14 = 14;
constexpr int CENTER_FREQ_DIFF = 5;
constexpr int CHANNEL_2G_MIN = 1;
constexpr int CHANNEL_5G_MIN = 34;

constexpr int MULTI_AP = 0;

/*
 * During the WPS PIN connection, the WPA_SUPPLICANT blocklist is cleared every 10 seconds
 * until the network connection is successful.
 */
constexpr int BLOCK_LIST_CLEAR_TIMER = 20 * 1000;

/* Signal levels are classified into: 0 1 2 3 4 ,the max is 4. */
constexpr int MAX_LEVEL = 4;
const std::string WPA_BSSID_ANY = "any";
const std::string IF_NAME = "wlan0";

class StaStateMachine : public StateMachine {
    FRIEND_GTEST(StaStateMachine);
public:
    StaStateMachine();
    ~StaStateMachine();
    using staSmHandleFunc = void (StaStateMachine::*)(InternalMessage *msg);
    using StaSmHandleFuncMap = std::map<int, staSmHandleFunc>;
    /**
     * @Description  Definition of member function of State base class in StaStateMachine.
     *
     */
    class RootState : public State {
    public:
        explicit RootState();
        ~RootState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;
    };
    /**
     * @Description : Definition of member function of InitState class in StaStateMachine.
     *
     */
    class InitState : public State {
    public:
        explicit InitState(StaStateMachine *pStaStateMachine);
        ~InitState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;

    private:
        StaStateMachine *pStaStateMachine;
    };
    /**
     * @Description : Definition of member function of WpaStartingState class in StaStateMachine.
     *
     */
    class WpaStartingState : public State {
    public:
        explicit WpaStartingState(StaStateMachine *pStaStateMachine);
        ~WpaStartingState() override;
        void InitWpsSettings();
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;

    private:
        StaStateMachine *pStaStateMachine;
    };
    /**
     * @Description  Definition of member function of WpaStartedState class in StaStateMachine.
     *
     */
    class WpaStartedState : public State {
    public:
        explicit WpaStartedState(StaStateMachine *pStaStateMachine);
        ~WpaStartedState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;

    private:
        StaStateMachine *pStaStateMachine;
    };
    /**
     * @Description  Definition of member function of WpaStoppingState class in StaStateMachine.
     *
     */
    class WpaStoppingState : public State {
    public:
        explicit WpaStoppingState(StaStateMachine *pStaStateMachine);
        ~WpaStoppingState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;

    private:
        StaStateMachine *pStaStateMachine;
    };
    /**
     * @Description  Definition of member function of LinkState class in StaStateMachine.
     *
     */
    class LinkState : public State {
    public:
        explicit LinkState(StaStateMachine *pStaStateMachine);
        ~LinkState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;

    private:
        StaStateMachine *pStaStateMachine;
    };
    /**
     * @Description  Definition of member function of SeparatingState class in StaStateMachine.
     *
     */
    class SeparatingState : public State {
    public:
        explicit SeparatingState();
        ~SeparatingState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;
    };
    /**
     * @Description  Definition of member function of SeparatedState class in StaStateMachine.
     *
     */
    class SeparatedState : public State {
    public:
        explicit SeparatedState(StaStateMachine *pStaStateMachine);
        ~SeparatedState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;

    private:
        StaStateMachine *pStaStateMachine;
    };
    /**
     * @Description  Definition of member function of ApLinkedState class in StaStateMachine.
     *
     */
    class ApLinkedState : public State {
    public:
        explicit ApLinkedState(StaStateMachine *pStaStateMachine);
        ~ApLinkedState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;

    private:
        StaStateMachine *pStaStateMachine;
    };
    /**
     * @Description  Definition of member function of WpsState class in StaStateMachine.
     *
     */
    class StaWpsState : public State {
    public:
        explicit StaWpsState(StaStateMachine *pStaStateMachine);
        ~StaWpsState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;

    private:
        StaStateMachine *pStaStateMachine;
    };
    /**
     * @Description  Definition of member function of GetIpState class in StaStateMachine.
     *
     */
    class GetIpState : public State {
    public:
        explicit GetIpState(StaStateMachine *pStaStateMachine);
        ~GetIpState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;

    private:
        StaStateMachine *pStaStateMachine;
    };
    /**
     * @Description  Definition of member function of LinkedState class in StaStateMachine.
     *
     */
    class LinkedState : public State {
    public:
        explicit LinkedState();
        ~LinkedState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;
    };
    /**
     * @Description  Definition of member function of ApRoamingState class in StaStateMachine.
     *
     */
    class ApRoamingState : public State {
    public:
        explicit ApRoamingState(StaStateMachine *pStaStateMachine);
        ~ApRoamingState() override;
        void GoInState() override;
        void GoOutState() override;
        bool ExecuteStateMsg(InternalMessage *msg) override;

    private:
        StaStateMachine *pStaStateMachine;
    };

    class DhcpResultNotify : public IDhcpResultNotify {
    public:
        /**
         * @Description : Construct a new dhcp result notify object
         *
         */
        explicit DhcpResultNotify(StaStateMachine *pStaStateMachine);

        /**
         * @Description : Destroy the dhcp result notify object
         *
         */
        ~DhcpResultNotify() override;

        /**
         * @Description : Get dhcp result of specified interface success notify asynchronously
         *
         * @param status - int
         * @param ifname - interface name,eg:wlan0
         * @param result - dhcp result
         */
        void OnSuccess(int status, const std::string &ifname, DhcpResult &result) override;

        /**
         * @Description : Get dhcp result of specified interface failed notify asynchronously
         *
         * @param status - int
         * @param ifname - interface name,eg:wlan0
         * @param reason - failed reason
         */
        void OnFailed(int status, const std::string &ifname, const std::string &reason) override;

        /**
        * @Description : Get the abnormal exit notify of dhcp server process.
        *
        * @param ifname - interface name,eg:wlan0
        */
        void OnSerExitNotify(const std::string& ifname) override;
    private:
        StaStateMachine *pStaStateMachine;
    };

public:
    /**
     * @Description  Initialize StaStateMachine
     *
     * @Return:  WIFI_OPT_SUCCESS - success  WIFI_OPT_FAILED - failed
     */
    ErrCode InitStaStateMachine();
    /**
     * @Description  Start roaming connection.
     *
     * @param bssid - the mac address of network(in)
     */
    void StartRoamToNetwork(std::string bssid);
    /**
     * @Description  Connecting events
     *
     * @param networkId - the networkId of network which is going to be connected(in)
     * @param bssid - bssid - the mac address of wifi(in)
     */
    void OnNetworkConnectionEvent(int networkId, std::string bssid);

    /**
     * @Description Register sta callback function
     *
     * @param callbacks - Callback function pointer storage structure
     */
    void RegisterStaServiceCallback(const StaServiceCallback &callbacks);

    /**
     * @Description  Convert the deviceConfig structure and set it to wpa_supplicant
     *
     * @param config -The Network info(in)
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    ErrCode ConvertDeviceCfg(const WifiDeviceConfig &config) const;

    /**
     * @Description Get linked info.
     *
     * @param linkedInfo - linked info
     * @return int - operation result
     */
    int GetLinkedInfo(WifiLinkedInfo& linkedInfo);

private:
    /**
     * @Description  Destruct state.
     *
     */
    template<typename T>
    inline void ParsePointer(T *&pointer)
    {
        if (pointer != nullptr) {
            delete pointer;
            pointer = nullptr;
        }
    }
    /**
     * @Description  Build state tree
     *
     */
    void BuildStateTree();
    /**
     * @Description  Determine whether it is empty during initialization
     *
     */
    template<typename T>
    inline ErrCode JudgmentEmpty(T *&pointer)
    {
        if (pointer == nullptr) {
            return WIFI_OPT_FAILED;
        }
        return WIFI_OPT_SUCCESS;
    }
    /**
     * @Description  Initializing state of Sta.
     *
     */
    ErrCode InitStaStates();
    /**
     * @Description  The process of initializing connected wifi information.
     *
     */
    void InitWifiLinkedInfo();
    /**
     * @Description  The process of initializing the last connected wifi information.
     *
     */
    void InitLastWifiLinkedInfo();
    /**
     * @Description  Setting linkedInfo in case of when wpa connects
                     automatically there isn't any connection information.
     *
     * @param networkId - the nerworkId of network which is saved in the WifiLinkedInfo.(in)
     */
    void SetWifiLinkedInfo(int networkId);
    /**
     * @Description  Save the current connected state into WifiLinkedInfo.
     *
     * @param state - current connecting state(in)
     * @param detailState - the current detail state of StaStateMachine.(in)
     */
    void SaveLinkstate(ConnState state, DetailedState detailState);
    /**
     * @Description  Translate frequency to band(2.4G or 5G).
     *
     * @param freQuency -the frequency needed to be translted into band.(in)
     */
    void GetBandFromFreQuencies(const int &freQuency);

    /**
     * @Description  Processing after a success response is returned after Wi-Fi
                     is enabled successfully, such as setting the MAC address and
                     saving the connection information.
     *
     */
    void StartWifiProcess();
    /**
     * @Description  Synchronize the deviceConfig structure to wpa_supplicant
     */
    void SyncDeviceConfigToWpa() const;
    /**
     * @Description  Update wifi status and save connection information.
     *
     * @param networkId - the networkId of selected network which is going to be connected(in)
     * @param bssid - the mac address of wifi(in)
     */
    void ConnectToNetworkProcess(InternalMessage *msg);

    /**
     * @Description  Start to connect to network.
     *
     * @param networkId - the networkId of network which is going to be connected.(in)
     */
    void StartConnectToNetwork(int networkId);
    /**
     * @Description  Disable network
     *
     * @param networkId - the networkId of network which is going to be disabled.(in)
     */
    ErrCode DisableNetwork(int networkId);
    /**
     * @Description  Disconnect network
     *
     */
    void DisConnectProcess();
    /**
     * @Description  Disable wifi process.
     *
     */
    void StopWifiProcess();
    /**
     * @Description  Setting statemachine status during the process of enable or disable wifi.
     *
     * @param mode - operating mode(in)
     */
    void SetOperationalMode(int mode);
    void SetSuspendMode(bool enabled);
    void SetPowerSave(bool enabled);
    /**
     * @Description  Configure static ipaddress.
     *
     * @param staticIpAddress- static ip address(in)
     */
    bool ConfigStaticIpAddress(StaticIpAddress &staticIpAddress);
    int PortalHttpDetection();
    /**
     * @Description  the process of handling network check results.
     *
     * @param netState - the state of connecting network(in)
     */
    void HandleNetCheckResult(StaNetState netState, const std::string portalUrl);
    /**
     * @Description  Remove all device configurations before enabling WPS.
     *
     */
    void RemoveAllDeviceConfigs();
    /**
     * @Description  Synchronize all networks saved in the configuration center to the WPA.
     *
     */
    void SyncAllDeviceConfigs();
    /**
     * @Description  Initialize the connection state processing message map
     *
     */
    int InitStaSMHandleMap();
    /**
     * @Description : Deal SignalPoll Result.
     *
     * @param  msg - Message body received by the state machine[in]
     */
    void DealSignalPollResult(InternalMessage *msg);
    /**
     * @Description : Converting frequencies to channels.
     *
     */
    void ConvertFreqToChannel();
    /**
     * @Description  Connect to selected network.
     *
     * @param  msg - Message body received by the state machine[in]
     */
    void DealConnectToSelectedNetCmd(InternalMessage *msg);
    /**
     * @Description : Ready to connect to the network selected by user.
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealConnectToUserSelectedNetwork(InternalMessage *msg);
    /**
     * @Description  Operations after the disconnection Event is reported.
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealDisconnectEvent(InternalMessage *msg);
    /**
     * @Description  Operations after the Connection Event is reported.
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealConnectionEvent(InternalMessage *msg);
    /**
     * @Description  Operations after Disable specified network commands.
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealConnectTimeOutCmd(InternalMessage *msg);
    /**
     * @Description  Operations after Clear blocklist is reported.
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealWpaBlockListClearEvent(InternalMessage *msg);
    /**
     * @Description  Operations after StartWps commands.
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealStartWpsCmd(InternalMessage *msg);
    /**
     * @Description  Operations after the Wps Connect TimeOut Event is reported.
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealWpsConnectTimeOutEvent(InternalMessage *msg);
    /**
     * @Description  Cancel wps connection
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealCancelWpsCmd(InternalMessage *msg);
    /**
     * @Description  Reconnect network
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealReConnectCmd(InternalMessage *msg);
    /**
     * @Description  Operations after the Reassociate lead is issued
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealReassociateCmd(InternalMessage *msg);
    /**
     * @Description  Roaming connection.
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealStartRoamCmd(InternalMessage *msg);
    /**
     * @Description  Operation after the password error is reported
     *
     * @param msg - Message body received by the state machine[in]
     */
    void DealWpaLinkFailEvent(InternalMessage *msg);
    /**
     * @Description  Wps mode is ON
     *
     * @param msg - Message body received by the state machine[in]
     */
    void StartWpsMode(InternalMessage *msg);
    /**
     * @Description  Reassociate network.
     *
     */
    void ReassociateProcess();

    /**
     * @Description  Set a random MAC address.
     *
     * @param networkId - network id[in]
     */
    bool SetRandomMac(int networkId);
    /**
     * @Description  Generate a random MAC address.
     *
     * @param strMac - Randomly generated MAC address[out]
     */
    void MacAddressGenerate(std::string &strMac);
    /**
     * @Description  Compare the encryption mode of the current network with that of the network in the scanning result.
     *
     * @param scanInfoKeymgmt - Network encryption mode in the scanning result[in]
     * @param deviceKeymgmt - Encryption mode of the current network[in]
     */
    bool ComparedKeymgmt(const std::string scanInfoKeymgmt, const std::string deviceKeymgmt);

private:
    StaSmHandleFuncMap staSmHandleFuncMap;
    StaServiceCallback staCallback;
    sptr<NetManagerStandard::NetSupplierInfo> NetSupplierInfo;

    int lastNetworkId;
    int operationalMode;
    int targetNetworkId;
    int pinCode;
    SetupMethod wpsState;
    int lastSignalLevel;
    std::string targetRoamBssid;
    int currentTpType;
    IsWpsConnected isWpsConnect;
    int getIpSucNum;
    int getIpFailNum;
    bool isRoam;
    WifiLinkedInfo linkedInfo;
    WifiLinkedInfo lastLinkedInfo;
    IDhcpService *pDhcpService;
    DhcpResultNotify *pDhcpResultNotify;
    StaNetworkCheck *pNetcheck;

    RootState *pRootState;
    InitState *pInitState;
    WpaStartingState *pWpaStartingState; /* Starting wpa_supplicant state. */
    WpaStartedState *pWpaStartedState;   /* Started wpa_supplicant state. */
    WpaStoppingState *pWpaStoppingState; /* Stopping wpa_supplicant state. */
    LinkState *pLinkState;
    SeparatingState *pSeparatingState;
    SeparatedState *pSeparatedState;
    ApLinkedState *pApLinkedState;
    StaWpsState *pWpsState;
    GetIpState *pGetIpState;
    LinkedState *pLinkedState;
    ApRoamingState *pApRoamingState;
};
}  // namespace Wifi
}  // namespace OHOS
#endif
