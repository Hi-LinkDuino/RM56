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

#ifndef PCE_SERVICE_H
#define PCE_SERVICE_H

#include <cstdint>
#include <cstring>
#include <memory>
#include "../obex/obex_types.h"
#include "base_observer_list.h"
#include "btstack.h"
#include "context.h"
#include "interface_profile.h"
#include "interface_profile_pbap_pce.h"
#include "message.h"
#include "pbap_pce_def.h"
#include "pbap_pce_parameter.h"
#include "pbap_pce_sdp.h"
#include "state_machine.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief pbap pce service main state
 */
enum PbapPceServiceStateType {
    PBAP_PCE_STATE_STARTUP,
    PBAP_PCE_STATE_STARTUPING,
    PBAP_PCE_STATE_SHUTDOWN,
    PBAP_PCE_STATE_SHUTDOWNING,
};
/**
 * @brief pbap pce targe state
 */
enum PbapPceTargetStateType {
    PCE_TARGET_STATE_UNKNOWN,
    PCE_TARGET_STATE_CONNECTED,
    PCE_TARGET_STATE_DISCONNECTED,
};
/**
 * @brief pbap pce config
 */
struct PbapPceConfig {
    bool srmEnable_ = true;
    int rfcommMtu_ = OBEX_DEFAULT_MTU;
    int l2capMtu_ = OBEX_DEFAULT_MTU;
    int pceMaxDevices_ = PCE_MAXIMUM_DEVICES;
};
/**
 * @brief phone book client service
 * process connect event
 */
class PbapPceService : public IProfilePbapPce, public utility::Context {
public:
    /**
     * @brief register observer
     * @details register observer for the service of phone book client
     * @param observer the reference to a PbapClientObserver
     * @return void
     */
    void RegisterObserver(IPbapPceObserver &observer) override;

    /**
     * @brief deregister observer
     * @details deregister observer for the service of phone book client
     * @param observer reference to a PbapClientObserver
     * @return void
     */
    void DeregisterObserver(IPbapPceObserver &observer) override;

    /**
     * @brief get the remote devices
     * @details get the remote device with the specified states
     * @param states states
     * @return std::vector remote devices
     */
    std::vector<RawAddress> GetDevicesByStates(const std::vector<int> &states) override;

    /**
     * @brief get the state of device
     * @details get the state with the specified remote device
     * @param device  remote device
     * @return int @c not -1 state of the specified remote device
     *             @c -1 device is not exist
     */
    int GetDeviceState(const RawAddress &device) override;

    /**
     * @brief set the strategy of device
     * @details set the strategy with the specified remote device
     * @param device  remote device
     * @param strategy  specified strategy
     * @return int @c 0  success
     *             @c other failure
     */
    int SetConnectionStrategy(const RawAddress &device, int strategy) override;

    /**
     * @brief get the strategy of device
     * @details get the strategy with the specified remote device
     * @param device  remote device
     * @return int @c  0 success
     *             @c -1 failure
     */
    int GetConnectionStrategy(const RawAddress &device) override;

    /**
     * @brief Set device's password. please call after OnServicePasswordRequired event.
     *
     * @param device device
     * @param password device's password
     * @param userId device's userId
     */
    int SetDevicePassword(
        const RawAddress &device, const std::string &password, const std::string &userId = "") override;

public:
    /**
     * @brief Pull phone book from remote device after connected.
     * @param param app parameter
     * @return int @c 0 ok
     *             @c -1 fail
     */
    int PullPhoneBook(const RawAddress &device, const IPbapPullPhoneBookParam &param) override;

    /**
     * @brief Pull phone book from remote device after connected.
     * @param name phone book path
     * @param flag 0 back to root; 2 go down; 3 go up
     * @return int @c 0 ok
     *             @c -1 fail
     */
    int SetPhoneBook(const RawAddress &device, const std::u16string &name, int flag) override;

    /**
     * @brief Pull vCard listing from remote device after connected.
     * @param param app parameter
     * @return int @c 0 ok
     *             @c -1 fail
     */
    int PullvCardListing(const RawAddress &device, const IPbapPullvCardListingParam &param) override;

    /**
     * @brief Pull vCard entry from remote device after connected.
     * @param param app parameter
     * @return int @c 0 ok
     *             @c -1 fail
     */
    int PullvCardEntry(const RawAddress &device, const IPbapPullvCardEntryParam &param) override;

    /**
     * @brief IsDownloading
     * @param device remote address
     * @return int @c 0 ok
     *             @c -1 fail
     */
    bool IsDownloading(const RawAddress &device) override;

    /**
     * @brief AbortDownloading.
     * @param device remote address
     * @return int @c 0 ok
     *             @c -1 fail
     */
    int AbortDownloading(const RawAddress &device) override;

    /**
     * @brief constructor
     * @details constructor
     */
    explicit PbapPceService();

    /**
     * @brief deconstructor
     * @details deconstructor
     */
    ~PbapPceService();

    /**
     * @brief get context
     * @details get context
     * @return Context*
     */
    utility::Context *GetContext() override;

    /**
     * @brief start client
     * @details start phone book client
     */
    void Enable() override;

    /**
     * @brief shutdown client
     * @details shutdown phone book client
     */
    void Disable() override;

    /**
     * @brief connect to server
     * @details connect to phone book server
     * @param device  remote device
     * @return int  @c  0 success
     *              @c -1 failure
     */
    int Connect(const RawAddress &device) override;

    /**
     * @brief disconnect device
     * @details disconnect from remote device
     * @param device  remote device
     * @return int @c  0 success
     *              @c -1 failure
     */
    int Disconnect(const RawAddress &device) override;

    /**
     * @brief get the remote devices
     * @details get devices
     * @return std::list remote devices
     */
    std::list<RawAddress> GetConnectDevices() override;

    /**
     * @brief get connection state
     * @details get connection state
     * @return int connection state, multiple states by bit operation OR
     */
    int GetConnectState() override;

    /**
     * @brief get max number of connections
     * @details get max number of connections
     * @return int max number
     */
    int GetMaxConnectNum() override;

    /**
     * @brief ForwardMsgToStm
     * @details Forward Msg To StateMachine
     * @param device RawAddress
     * @param msg message
     * @return void
     */
    void ForwardMsgToStm(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief post message
     * @details post message to self thread
     * @param device RawAddress
     * @param msg message from other threads
     * @return void
     */
    void PostMessage(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief Process Obex Response message
     * @details Process Obex Response message
     * @param device RawAddress
     * @param msg message from other threads
     * @return void
     */
    void ProcessObexRespMessage(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief get the connected devices
     * @details get the remote device with the connected states
     * @return std::vector connected remote devices
     */
    std::vector<RawAddress> GetConnectedDevices();

    /**
     * @brief GetDownloadFileName
     * @details Get Download File Name
     * @param device remote address
     * @return string download file name
     */
    std::string GetDownloadFileName(const RawAddress &device);

    /**
     * @brief GetPceConfig
     * @details Get PceConfig
     * @return PbapPceConfig
     */
    const PbapPceConfig &GetPceConfig() const;

private:
    /**
     * @brief sdp gap failed process
     * @details when sdp gap search failed, call it
     * @param device remote address
     * @param msg message from other threads
     * @return void
     */
    void ProcessSdpGapFailed(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief sdp gap finish process
     * @details when sdp gap search success, call it
     * @param device remote address
     * @param sdpMsg message from other threads
     * @return void
     */
    void ProcessSdpGapFinish(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief  unauthorized process
     * @details when obex return unauthorized, call it
     * @param device remote address
     * @param msg message from other threads
     * @return void
     */
    void ProcessConnectFailed(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief connected process
     * @details when connected obex server, call it
     * @param device remote address
     * @param msg message from other threads
     * @return void
     */
    void ProcessConnected(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief disconnected process
     * @details when disconnected obex server, call it
     * @param device remote address
     * @param msg message from other threads
     * @return void
     */
    void ProcessDisconnected(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief transported failed process
     * @details when transported failed obex server, call it
     * @param device remote address
     * @param msg message from other threads
     * @return void
     */
    void ProcessTransportFailed(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief user input process
     * @details when user input password, call it
     * @param device remote address
     * @param msg message from other threads
     * @return void
     */
    void ProcessUserInput(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief pull phone book process
     * @details when call pull phone book, call it
     * @param device remote address
     * @param msg message from other threads
     * @return void
     */
    void ProcessPhoneBookActionCompleted(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief enable service
     * @details enable service
     * @return void
     */
    void EnableService();

    /**
     * @brief disable service
     * @details disable service
     * @return void
     */
    void DisableService();

    /**
     * @brief connect to server
     * @details connect to phone book server
     * @param device  remote device
     * @return int @c  0 success
     *             @c other failure
     */
    int ConnectInternal(const RawAddress &device);

    /**
     * @brief PullPhoneBook Internal
     * @details PullPhoneBook Internal
     */
    int PullPhoneBookInternal(const RawAddress &device, const IPbapPullPhoneBookParam &param);

    /**
     * @brief SetPhoneBook Internal
     * @details SetPhoneBook Internal
     */
    int SetPhoneBookInternal(const RawAddress &device, const std::u16string &name, int flag);

    /**
     * @brief PullvCardListing Internal
     * @details PullvCardListing Internal
     */
    int PullvCardListingInternal(const RawAddress &device, const IPbapPullvCardListingParam &param);

    /**
     * @brief PullvCardEntry Internal
     * @details PullvCardEntry Internal
     */
    int PullvCardEntryInternal(const RawAddress &device, const IPbapPullvCardEntryParam &param);

    /**
     * @brief Abort Downloading Internal
     * @details Abort Downloading Internal
     */
    int AbortDownloadingInternal(const RawAddress &device);

    /**
     * @brief disconnect to server
     * @details disconnect to phone book server
     * @param device  remote device
     * @return int  @c  0 success
     *              @c other failure
     */
    int DisconnectInternal(const RawAddress &device);

    /**
     * @brief disconnect all server
     * @details disconnect all server
     * @return int  @c  0 success
     *              @c -1 failure
     */
    int DisconnectAllDevices();

    /**
     * @brief is connected
     * @details check connected
     * @param device  remote device
     * @return bool @c  true connected
     *              @c false not connected
     */
    bool IsConnected(const RawAddress &device);

private:
    /**
     * @brief Load PceConfig
     * @details Load PceConfig
     * @return
     */
    void LoadPceConfig();

    /**
     * @brief Try to ShutDown
     * @details Try to ShutDown
     * @return bool @c true success
     *              @c false failure
     */
    bool TryShutDown(bool ret);

    /**
     * @brief Is busy
     * @details Is busy, check if executing action
     * @param device  remote device
     * @return bool @c true busy
     *              @c false not busy
     */
    bool IsBusy(const RawAddress &device);

    /**
     * @brief set busy
     * @details set busy, when executing action
     * @param device  remote device
     * @param busy  set busy
     * @return bool @c true success
     *              @c false failure
     */
    bool SetBusy(const RawAddress &device, bool busy);

    /**
     * @brief SetPowerStatusBusy
     * @details SetPowerStatusBusy, when executing action
     * @param device  remote device
     * @param busy  set busy
     */
    void SetPowerStatusBusy(const RawAddress &device, bool busy);

    /**
     * @brief CheckAndSetDeviceBusy
     * @details CheckAndSetDeviceBusy
     * @param device  remote device
     * @return bool @c true busy
     *              @c false not busy
     */
    bool CheckAndSetDeviceBusy(const RawAddress &device);

    /**
     * @brief CheckDeviceStatusForPbapAction
     * @details CheckDeviceStatusForPbapAction
     * @param device  remote device
     * @return int  @c  0 success
     *              @c other failure
     */
    int CheckDeviceStatusForPbapAction(const RawAddress &device);

    /**
     * @brief save pbap connection policy
     * @details save pbap connection policy
     * @param device  remote device
     * @param strategy connection policy,
     *     @c UNKNOWN : the connection policy for unkown state.
     *     @c ALLOWED : the connection policy for allowed state.
     *     @c FORBIDDEN : the connection policy for forbidden state.
     * @return bool  @c  true success
     *               @c false failure
     */
    bool SaveConnectPolicy(const std::string &addr, int strategy);

    /**
     * @brief get pbap connection policy
     * @details get pbap connection policy
     * @param addr  addr
     * @param strategy Reference to the connection policy, return strategy,
     *     @c UNKNOWN : the connection policy for unkown state.
     *     @c ALLOWED : the connection policy for allowed state.
     *     @c FORBIDDEN : the connection policy for forbidden state.
     * @return bool  @c  true success
     *               @c false failure
     */
    bool LoadConnectPolicy(const std::string &addr, int &strategy);

    // statemachine for remote device
    std::map<std::string, std::unique_ptr<utility::StateMachine>> machineMap_ {};

    // pce observer
    BaseObserverList<IPbapPceObserver> observerMgrList_ {};

    // Pbap Pce Service State
    PbapPceServiceStateType serviceState_ = PBAP_PCE_STATE_SHUTDOWN;

    // sdp service
    std::unique_ptr<PbapPceSdp> pbapPceSdp_ {nullptr};

    // machine map mutex
    std::recursive_mutex machineMapMutex_ {};

    // Pbap Pce Service config
    PbapPceConfig pbapPceConfig_ {};

    BT_DISALLOW_COPY_AND_ASSIGN(PbapPceService);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // PCE_SERVICE_H
