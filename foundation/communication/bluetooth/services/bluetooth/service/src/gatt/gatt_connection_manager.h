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

#ifndef GATT_CONNECTION_MANAGER_H
#define GATT_CONNECTION_MANAGER_H

#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>
#include "base_def.h"
#include "dispatcher.h"
#include "gatt_connection_observer.h"
#include "gatt_data.h"
#include "raw_address.h"
#include "state_machine.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
class GattConnectionManager {
public:
    class Device {
    public:
        struct ConnectionPriorityResult {
            uint16_t interval_;
            uint16_t latency_;
            uint16_t timeout_;
            int status;
        };
        static const std::string STATE_IDLE;
        static const std::string STATE_CONNECTING;
        static const std::string STATE_CONNECTED;
        static const std::string STATE_DISCONNECTING;
        static const std::string STATE_DISCONNECTED;

        class StateMachine : public utility::StateMachine {
        public:
            enum StateMessage {
                MSG_CONNECT,
                MSG_CONNECT_COMPLETE,
                MSG_DISCONNECT,
                MSG_DISCONNECT_COMPLETE,
                MSG_REQUEST_CONNECTION_PRIORITY,
                MSG_RESPONSE_CONNECTION_PRIORITY,
                MSG_DIRECT_CONNECT_TIMEOUT,
                MSG_RECONNECT_CAUSE_0X3E
            };
            explicit StateMachine(GattConnectionManager::Device &device);
            ~StateMachine();

        private:
            class StateBase : public utility::StateMachine::State {
            public:
                StateBase(const std::string &name, utility::StateMachine &stateMachine, GattConnectionManager::Device &device);
                virtual ~StateBase()
                {}

            protected:
                GattConnectionManager::Device &device_;
            };
            struct Idle : public StateBase {
                Idle(utility::StateMachine &stateMachine, GattConnectionManager::Device &device);
                ~Idle() override{};
                void Entry() override;
                void Exit() override{};
                bool Dispatch(const utility::Message &msg) override;
            };
            struct Connecting : public StateBase {
                Connecting(utility::StateMachine &stateMachine, GattConnectionManager::Device &device);
                ~Connecting() override{};
                void Entry() override;
                void Exit() override{};
                bool Dispatch(const utility::Message &msg) override;
            };
            struct Connected : public StateBase {
                Connected(utility::StateMachine &stateMachine, GattConnectionManager::Device &device);
                ~Connected() override{};
                void Entry() override;
                void Exit() override{};
                bool Dispatch(const utility::Message &msg) override;
            };
            struct Disconnecting : public StateBase {
                Disconnecting(utility::StateMachine &stateMachine, GattConnectionManager::Device &device);
                ~Disconnecting() override{};
                void Entry() override;
                void Exit() override{};
                bool Dispatch(const utility::Message &msg) override;
            };
            struct Disconnected : public StateBase {
                Disconnected(utility::StateMachine &stateMachine, GattConnectionManager::Device &device);
                ~Disconnected() override{};
                void Entry() override;
                void Exit() override{};
                bool Dispatch(const utility::Message &msg) override;
            };
        };

        Device(const uint8_t *addr, uint8_t transport, uint8_t type, bool autoConnect = false);
        explicit Device(const GattDevice &device, bool autoConnect = false);
        ~Device();
        bool ProcessMessage(int messageId, int arg1 = 0, void *arg2 = nullptr);
        void CheckEncryption();
        bool &AutoConnect();
        uint8_t *Addr();
        GattDevice &Info();
        StateMachine &SM();
        uint16_t &MTU();
        uint16_t &Handle();
        uint8_t &Role();
        uint8_t &RetryTimes();
        std::mutex &DeviceRWMutex();
        utility::Timer &DirectConnect();

    private:
        bool autoConnect_;
        uint16_t mtu_;
        uint16_t handle_;
        uint8_t role_;
        uint8_t retry_;
        uint8_t addr_[RawAddress::BT_ADDRESS_BYTE_LEN] = {0};
        GattDevice info_;
        std::mutex deviceRWMutex_;
        StateMachine sm_;
        utility::Timer directConnect_;
        void CopyAddress(const uint8_t *addr, size_t length);
        Device() = delete;
        BT_DISALLOW_COPY_AND_ASSIGN(Device);
        BT_DISALLOW_MOVE_AND_ASSIGN(Device);
    };

    static GattConnectionManager &GetInstance()
    {
        static GattConnectionManager instance;
        return instance;
    }
    ~GattConnectionManager();

    int Connect(const GattDevice &device, bool autoConnect = false) const;
    int Disconnect(const GattDevice &device) const;
    int RegisterObserver(GattConnectionObserver &observer) const;
    void DeregisterObserver(int registerId) const;
    const std::string &GetDeviceState(const GattDevice &device) const;
    void GetDevices(std::vector<GattDevice> &devices) const;
    std::pair<uint16_t, uint16_t> GetMaximumNumberOfConnections() const;
    std::tuple<std::string, uint16_t, uint16_t> GetDeviceInformation(const GattDevice &device) const;
    uint8_t GetDeviceTransport(uint16_t handle) const;
    int RequestConnectionPriority(uint16_t handle, int connPriority) const;
    bool GetEncryptionInfo(uint16_t connectHandle) const;
    bool GetEncryptionInfo(const GattDevice &device) const;
    int SetConnectionType(const GattDevice &device, bool autoConnect) const;

    int StartUp(utility::Dispatcher &dispatcher);
    int ShutDown() const;

private:
    static const uint8_t MAX_OBSERVER_NUM = 4;
    static const uint16_t BT_PSM_GATT = 0x001F;
    static const int DIRECT_CONNECT_TIMEOUT = 30000;
    GattConnectionManager();
    DECLARE_IMPL();

    BT_DISALLOW_COPY_AND_ASSIGN(GattConnectionManager);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // GATT_CONNECTION_MANAGER_H
