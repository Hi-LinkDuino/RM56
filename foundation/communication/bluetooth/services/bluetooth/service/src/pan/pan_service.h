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

#ifndef PAN_SERVICE_H
#define PAN_SERVICE_H

#include <memory.h>
#include <list>
#include <mutex>
#include <vector>
#include <cmath>
#include <cstring>
#include "log.h"
#include "packet.h"
#include "securec.h"
#include "adapter_config.h"
#include "base_observer_list.h"
#include "class_creator.h"
#include "context.h"
#include "interface_profile_pan.h"


#include "profile_config.h"
#include "profile_service_manager.h"
#include "raw_address.h"
#include "pan_bnep.h"
#include "pan_message.h"
#include "pan_sdp.h"
#include "base_def.h"
#include "pan_statemachine.h"

namespace OHOS {
namespace bluetooth {
class PanService : public IProfilePan, public utility::Context {
public:
    /**
     * @brief Get the instance of the HfpHfService object.
     *
     * @return Returns the instance of the HfpHfService object.
     */
    static PanService *GetService();
    /**
     * @brief Construct a new Pan Service object
     *
     */
    PanService();
    /**
     * @brief Destroy the Pan Service object
     *
     */
    virtual ~PanService();
    utility::Context *GetContext() override;
    void Enable(void) override;
    void Disable(void) override;
    int Connect(const RawAddress &device) override;
    std::list<RawAddress> GetConnectDevices() override;
    int GetConnectState(void) override;
    int GetMaxConnectNum(void) override;
    int Disconnect(const RawAddress &device) override;
    std::vector<RawAddress> GetDevicesByStates(std::vector<int> states) override;
    int GetDeviceState(const RawAddress &device) override;
    void RegisterObserver(IPanObserver &PanObserver) override;
    void DeregisterObserver(IPanObserver &PanObserver) override;
    bool SetTethering(bool enable)override;
    bool IsTetheringOn()override;
    void NotifyStateChanged(const RawAddress &device, int state);
    void ShutDownDone(bool isAllDisconnected);
    void ProcessEvent(const PanMessage &event);

    /**
     * @brief Send the event of the Pan role.
     *
     * @param event The event of the Pan role.
     */
    void PostEvent(const PanMessage &event);
    void RemoveStateMachine(const std::string &device);

    static std::string GetLocalAddress();

    std::string PanFindDeviceByLcid(uint16_t lcid);
    void OpenNetwork();
    void CloseNetwork(std::string device);
    void WriteNetworkData(std::string address, EthernetHeader head, uint8_t *data, int len);
    int ReceiveRemoteBusy(bool isBusy);
    int PanSendData(EthernetHeader head, uint8_t *data, int len);

    static void ReverseAddress(uint8_t *oldAddr, uint8_t *newAddr);

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
    /**
     * @brief Get the max connection devices number.
     *
     * @return Returns the max connection devices number.
     */
    int GetMaxConnectionsDeviceNum() const;
    int GetConnectionsDeviceNum() const;
    bool IsConnected(const std::string &address) const;
    void ProcessConnectEvent(const PanMessage &event);
    void ProcessDefaultEvent(const PanMessage &event) const;
    void ProcessRemoveStateMachine(const std::string &address);
    void PanSendData(std::string address, EthernetHeader head, uint8_t *data, int len);
    //  service status
    bool isStarted_ {false};
    //  service status
    bool isShuttingDown_ {false};
    // the mutex variable
    std::recursive_mutex mutex_ {};
    // The maximum default number of connection devices
    static const int PAN_MAX_DEFAULT_CONNECTIONS_NUMR = 6;
    // the maximum number of connection devices.
    int maxConnectionsNum_ {PAN_MAX_DEFAULT_CONNECTIONS_NUMR};

    BaseObserverList<IPanObserver> panObservers_ {};
    // the map of the device and sate machine
    std::map<const std::string, std::unique_ptr<PanStateMachine>> stateMachines_ {};
    std::unique_ptr<PanSdp> panSdp_ {nullptr};
    std::unique_ptr<PanNetwork> panNetwork_ {nullptr};
    bool isTetheringOn_ {false};
    // const state map
    const std::map<const int, const int> stateMap_ = {
        {PAN_STATE_DISCONNECTED, static_cast<int>(BTConnectState::DISCONNECTED)},
        {PAN_STATE_CONNECTING, static_cast<int>(BTConnectState::CONNECTING)},
        {PAN_STATE_DISCONNECTING, static_cast<int>(BTConnectState::DISCONNECTING)},
        {PAN_STATE_CONNECTED, static_cast<int>(BTConnectState::CONNECTED)}
    };
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PAN_SERVICE_H
