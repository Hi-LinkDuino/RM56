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

#ifndef HID_HOST_SERVICE_H
#define HID_HOST_SERVICE_H

#include <list>
#include <mutex>
#include <vector>
#include <memory.h>
#include <cmath>
#include <cstring>
#include <mutex>
#include "log.h"
#include "packet.h"
#include "securec.h"
#include "adapter_config.h"
#include "base_observer_list.h"
#include "class_creator.h"
#include "context.h"
#include "interface_profile_hid_host.h"

#include "profile_config.h"
#include "profile_service_manager.h"
#include "raw_address.h"
#include "hid_host_message.h"
#include "base_def.h"
#include "hid_host_statemachine.h"

namespace OHOS {
namespace bluetooth {
class HidHostService : public IProfileHidHost, public utility::Context {
public:
    /**
     * @brief Get the instance of the HfpHfService object.
     *
     * @return Returns the instance of the HfpHfService object.
     */
    static HidHostService *GetService();
    /**
     * @brief Construct a new Hid Host Service object
     *
     */
    explicit HidHostService();
    /**
     * @brief Destroy the Hid Host Service object
     *
     */
    virtual ~HidHostService();
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
    void RegisterObserver(IHidHostObserver &hidHostObserver) override;
    void DeregisterObserver(IHidHostObserver &hidHostObserver) override;
    void NotifyStateChanged(const RawAddress &device, int state);
    void ShutDownDone(bool isAllDisconnected);
    void ProcessEvent(const HidHostMessage &event);

    /**
     * @brief Send the event of the Hid Host role.
     *
     * @param event The event of the Hid Host role.
     */
    void PostEvent(const HidHostMessage &event);
    void RemoveStateMachine(const std::string &device);

    std::string HidHostFindDeviceByLcid(uint16_t lcid, bool *isControlLcid);
    int HidHostVCUnplug(std::string device, uint8_t id, uint16_t size, uint8_t type) override;
    int HidHostSendData(std::string device, uint8_t id, uint16_t size, uint8_t type) override;
    int HidHostSetReport(std::string device, uint8_t type, uint16_t size, const uint8_t* report) override;
    int HidHostGetReport(std::string device, uint8_t id, uint16_t size, uint8_t type) override;

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
    void ProcessConnectEvent(const HidHostMessage &event);
    void ProcessDefaultEvent(const HidHostMessage &event) const;
    void ProcessRemoveStateMachine(const std::string &address);
    //  service status
    bool isStarted_ {false};
    //  service status
    bool isShuttingDown_ {false};
    // the mutex variable
    std::recursive_mutex mutex_ {};

    // The maximum default number of connection devices
    static const int HID_HOST_MAX_DEFAULT_CONNECTIONS_NUMR = 6;
    // the maximum number of connection devices.
    int maxConnectionsNum_ {HID_HOST_MAX_DEFAULT_CONNECTIONS_NUMR};
    BaseObserverList<IHidHostObserver> hidHostObservers_ {};
    // the map of the device and sate machine
    std::map<const std::string, std::unique_ptr<HidHostStateMachine>> stateMachines_ {};
    // const state map
    const std::map<const int, const int> stateMap_ = {
        {HID_HOST_STATE_DISCONNECTED, static_cast<int>(BTConnectState::DISCONNECTED)},
        {HID_HOST_STATE_CONNECTING, static_cast<int>(BTConnectState::CONNECTING)},
        {HID_HOST_STATE_DISCONNECTING, static_cast<int>(BTConnectState::DISCONNECTING)},
        {HID_HOST_STATE_CONNECTED, static_cast<int>(BTConnectState::CONNECTED)}
    };
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // HID_HOST_SERVICE_H
