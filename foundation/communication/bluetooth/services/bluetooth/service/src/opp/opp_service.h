/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OPP_SERVICE_H
#define OPP_SERVICE_H

#include "base_observer_list.h"
#include "context.h"
#include "interface_profile.h"
#include "interface_profile_opp.h"
#include "opp_gap_server.h"
#include "opp_message.h"
#include "opp_obex_server.h"
#include "opp_sdp_server.h"
#include "opp_statemachine.h"
#include "opp_transfer.h"
#include "opp_transfer_information.h"

namespace OHOS {
namespace bluetooth {
class OppService : public IProfileOpp, public utility::Context {
public:
    /**
     * @brief Get the instance of the OppService object.
     *
     * @return Returns the instance of the OppService object.
     */
    static OppService *GetService();
    /**
     * @brief Construct a new Opp Service object
     *
     */
    OppService();
    /**
     * @brief Destroy the Opp Service object
     *
     */
    virtual ~OppService();
    utility::Context *GetContext() override;
    void Enable(void) override;
    void Disable(void) override;
    int Connect(const RawAddress &device) override;
    std::list<RawAddress> GetConnectDevices() override;
    int GetConnectState(void) override;
    int GetMaxConnectNum(void) override;
    std::vector<RawAddress> GetDevicesByStates(std::vector<int> states) override;
    int GetDeviceState(const RawAddress &device) override;
    int Disconnect(const RawAddress &device) override;
    void RegisterObserver(IOppObserver &oppObserver) override;
    void DeregisterObserver(IOppObserver &oppObserver) override;
    int SendFile(const RawAddress &device, const std::vector<std::string> filePaths,
        const std::vector<std::string> mimeTypes) override;
    int SetIncomingFileConfirmation(const bool accept) override;
    IOppTransferInformation GetCurrentTransferInformation() override;
    int CancelTransfer() override;
    int CancelTransfer(const std::string &device);
    OppConfig &GetOppConfig();

    void ShutDownDone(bool isAllDisconnected);
    void ProcessEvent(const OppMessage &event);
    void PostEvent(const OppMessage &event);
    void RemoveStateMachine(const std::string &device);
    void ConnectObex(const std::string &device, const ObexClientConfig &config);
    void DisconnectObex(const std::string &device);
    void OnReceiveIncomingConnect(ObexServerSession &session, uint32_t connectId);
    void OnReceiveIncomingFile(IOppTransferInformation info);
    void OnObexConnected(const std::string &device);
    void OnObexDisconnected(const std::string &device);
    void OnTransferStateChange(const std::string &device, int state, int reason);
    void OnTransferPositionChange(const std::string &device, size_t position);
    void NotifyReceiveIncomingFile(IOppTransferInformation info);
    void NotifyTransferStateChanged(IOppTransferInformation info);
    void NotifyStateChanged(const RawAddress &device, int state);

private:
    /**
     * @brief Service startup.
     *
     */
    void StartUp();

    /**
     * @brief Service shutdown.
     *
     */
    void ShutDown();
    bool IsConnected(const std::string &address) const;
    void ProcessConnectEvent(const OppMessage &event);
    void ProcessDefaultEvent(const OppMessage &event) const;
    void ProcessRemoveStateMachine(const std::string &address);
    void StartNextTransfer();
    void LoadOppConfig();
    //  service status
    bool isStarted_ {false};
    //  service status
    bool isShuttingDown_ {false};
    // the mutex variable
    std::recursive_mutex mutex_ {};

    BaseObserverList<IOppObserver> oppObservers_ {};
    // the map of the device and sate machine
    std::map<const std::string, std::unique_ptr<OppStateMachine>> stateMachines_ {};
    std::list<std::shared_ptr<OppTransfer>> oppTransferList_ {};
    std::unique_ptr<OppSdpServer> oppSdpServer_ {nullptr};
    std::unique_ptr<OppGapServer> oppGapServer_ {nullptr};
    std::unique_ptr<OppObexServer> oppObexServer_ {nullptr};
    OppConfig oppConfig_ {};
    // const state map
    const std::map<const int, const int> stateMap_ = {
        {OPP_STATE_DISCONNECTED, static_cast<int>(BTConnectState::DISCONNECTED)},
        {OPP_STATE_CONNECTING, static_cast<int>(BTConnectState::CONNECTING)},
        {OPP_STATE_DISCONNECTING, static_cast<int>(BTConnectState::DISCONNECTING)},
        {OPP_STATE_CONNECTED, static_cast<int>(BTConnectState::CONNECTED)}
    };
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OPP_SERVICE_H
