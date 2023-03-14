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

#ifndef MAP_MSE_SERVICE_H
#define MAP_MSE_SERVICE_H

#include <cstdint>
#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "adapter_config.h"
#include "base_observer_list.h"
#include "class_creator.h"
#include "context.h"
#include "interface_profile_map_mse.h"
#include "map_mse_instance.h"
#include "power_manager.h"
#include "profile_config.h"
#include "profile_service_manager.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
class MapMseService : public IProfileMapMse, public utility::Context {
public:
    enum { INIT_INSTANCE, DISCONNECT, START_INSTANCE, START_UP_COMPLETE, SHUT_DOWN, SHUT_DOWN_COMPLETE };
    /**
     * @brief Construct a new Map Mse Service object
     *
     */
    explicit MapMseService();
    /**
     * @brief Destroy the Map Mse Service object
     *
     */
    virtual ~MapMseService();
    utility::Context *GetContext() override;
    void Enable(void) override;
    void Disable(void) override;
    int Connect(const RawAddress &device) override;
    std::list<RawAddress> GetConnectDevices() override;
    int GetConnectState(void) override;
    int GetMaxConnectNum(void) override;
    int GetState(void) override;
    int Disconnect(const RawAddress &device) override;
    bool IsConnected(const RawAddress &device) override;
    std::vector<RawAddress> GetDevicesByStates(std::vector<int> states) override;
    int GetConnectionState(const RawAddress &device) override;
    bool SetConnectionStrategy(const RawAddress &device, int strategy) override;
    int GetConnectionStrategy(const RawAddress &device) override;
    void GrantPermission(const RawAddress &device, bool allow, bool save = false) override;
    void RegisterObserver(IMapMseObserver &mapMseObserver) override;
    void DeregisterObserver(IMapMseObserver &mapMseObserver) override;

private:
    // The initial value of the Map instance Id
    static const int MAS_ID_SMS_MMS = 0;
    // The initial value of the GoepL2capPsm
    static const uint8_t LPSM_SIZE = 12;
    static const uint16_t GOEP_L2CAP_PSM_VALUE[LPSM_SIZE];
    // The maximum default number of connection devices
    static const int MAP_MSE_MAX_DEFAULT_CONNECTIONS_NUMR = 6;
    std::vector<MapAccountItem> GetAccountItems(void) const;
    void PostMessage(int what);
    void ProcessMessage(const utility::Message &msg);
    void InitMasInstances(void);
    void InstancesStarting(void);
    void ServiceStartUpComplete(bool whether);
    void ServiceShutDown(void);
    void DisableComplete(void);
    void ServiceShutDownComplete(void);
    void DisConnectInternal(const utility::Message &msg) const;
    void RegisterObserver(MapMseInstance &masInstance, const int masId);
    void DeregisterObserver(MapMseInstance &masInstance, const int masId);
    void GrantPermissionNative(const RawAddress &device, bool allow, bool save);
    void NotifyConnectionState(const RawAddress &remoteAddr, const int masInstanceId, BTConnectState state);
    void ProcessGrantPermission(const RawAddress &device, bool allow, bool save);
    void GrantPermissionTimer(const RawAddress &device, bool allow, bool save);
    void RejectConnection(std::string &addr, const int masInstanceId);
    void ProcessReject(std::string &addr, const int masInstanceId);
    std::unordered_map<int, std::unique_ptr<MapMseInstance>> masInstanceMap_ {};
    std::unordered_map<std::string, int> incomingConnectMap_ {};
    std::recursive_mutex mseMapMutex_ {};
    bool shutDownEnable_ = false;
    std::unordered_map<std::string, std::unique_ptr<utility::Timer>> grantTimer_ {};
    std::unordered_map<std::string, std::unique_ptr<utility::Timer>> rejectTimer_ {};
    std::unique_ptr<MapMseMnscli> mnsClient_ = nullptr;
    class MnsCallback : MnsObserver {
    public:
        /**
         * @brief Construct a new ObServer object
         *
         * @param mseService Reference to the Mse Service
         */
        explicit MnsCallback(MapMseService &mseService);

    private:
        /**
         * @brief Called OnDisconnected
         *
         * @param client ObexClient
         */
        void OnDisconnected() override;
        // Reference to the Mse Service
        MapMseService &mseService_;
        BT_DISALLOW_COPY_AND_ASSIGN(MnsCallback);
    };
    std::unique_ptr<MnsCallback> mnsObserver_ = nullptr;
    class Observer : public MapObserver {
    public:
        /**
         * @brief Construct a new ObServer object
         *
         * @param mseService Reference to the Mse Service
         */
        explicit Observer(MapMseService &mseService);

    private:
        // The function is called when MCE connect is incoming
        void OnIncomingConnect(const RawAddress &remoteAddr, const int masInstanceId) override;

        // The function is called when MCE transport connect is successful
        void OnTransportConnected(const RawAddress &remoteAddr, const int masInstanceId) override;

        // The function is called when MSE disconnect is incoming
        void OnTransportDisconnected(const RawAddress &remoteAddr, const int masInstanceId) override;

        // The function is called when MSE busy or not busy
        void OnBusy(const RawAddress &remoteAddr, bool isBusy) override;

        // The function is called when Connection State Changed
        void OnConnectionStateChanged(
            const RawAddress &remoteAddr, const int masInstanceId, BTConnectState state) override;

        // The function is called when MCE connect is incoming timeout
        virtual void OnIncomingTimeout(const RawAddress &remoteAddr, const int masInstanceId) override;

        // The function is called when MCE connect is incoming timeout
        virtual void OnRejectConnection(const RawAddress &remoteAddr, const int masInstanceId) override;

        // Reference to the Mse Service
        MapMseService &mseService_;
        BT_DISALLOW_COPY_AND_ASSIGN(Observer);
    };
    std::unordered_map<int, std::unique_ptr<Observer>> observerMap_ {};
    BaseObserverList<IMapMseObserver> mapMseObservers_ {};
    BT_DISALLOW_COPY_AND_ASSIGN(MapMseService);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MSE_SERVICE_H
