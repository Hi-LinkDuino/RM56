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

#ifndef MAP_MSE_INSTANCE_H
#define MAP_MSE_INSTANCE_H

#include <cstdint>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include "adapter_manager.h"
#include "bt_def.h"
#include "gap_if.h"
#include "map_mse_mnscli.h"
#include "map_mse_types.h"
#include "message.h"
#include "raw_address.h"
#include "rfcomm.h"
#include "sdp.h"
#include "stub/map_service.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
struct MapAccountItem {
    MessageType msgType_ = MessageType::NONE;
    std::string id_ = "";
    std::string name_ = "";
    std::string pAuthority_ = "";
    std::string uci_ = "";
    std::string uciPrefix_ = "";
    std::string GetUciFullName(void);
};
class MapObserver {
public:
    // A destructor used to delete the Map Observer instance.
    virtual ~MapObserver() = default;

    // The function is called when MCE connect is incoming
    virtual void OnIncomingConnect(const RawAddress &remoteAddr, const int masInstanceId) = 0;

    // The function is called when MCE transport connect is successful
    virtual void OnTransportConnected(const RawAddress &remoteAddr, const int masInstanceId) = 0;

    // The function is called when MCE transport disconnect is incoming
    virtual void OnTransportDisconnected(const RawAddress &remoteAddr, const int masInstanceId) = 0;

    // The function is called when Connection State Changed
    virtual void OnConnectionStateChanged(
        const RawAddress &remoteAddr, const int masInstanceId, BTConnectState state) = 0;

    // The function is called when MCE connect is incoming timeout
    virtual void OnIncomingTimeout(const RawAddress &remoteAddr, const int masInstanceId) = 0;

    // The function is called when MCE connect is incoming timeout
    virtual void OnRejectConnection(const RawAddress &remoteAddr, const int masInstanceId) = 0;

    // The function is called when MSE busy or not busy
    virtual void OnBusy(const RawAddress &remoteAddr, bool isBusy) = 0;
};

class MapMseInstance {
public:
    explicit MapMseInstance(utility::Dispatcher &dispatcher, uint8_t masId, uint16_t l2capPsm,
        const MapAccountItem &accountItem, bool smsSupport);
    explicit MapMseInstance(utility::Dispatcher &dispatcher, uint8_t masId, uint16_t l2capPsm, bool smsSupport);
    virtual ~MapMseInstance();
    int RegistMapObserver(MapObserver &observer);
    void DeregisterObserver(MapObserver &observer);
    void GrantPermission(const RawAddress &remoteAddr, bool allow, bool save = false);
    int Enable(MapMseMnscli &mnsClient);
    void Disable(void);
    /// create SDP record
    int CreateMasSdpRecord(void);
    /// destroy SDP record
    void DestroyMasSdpRecord(void);
    /// regist GAP
    int RegistGap(void);
    /// unregist GAP
    void UnRegistGap(void);
    void SetConnectState(std::string, BTConnectState);
    void DestroyIncomingConnect(std::string address);
    uint64_t GetDatabaseIdentifier(void);
    void UpdateDatabaseIdentifier(void);
    uint64_t GetFolderVersionCounter(void);
    void UpdateFolderVersionCounter(void);
    uint64_t GetConversationVersionCounter(void);
    void UpdateConversationVersionCounter(void);
    uint8_t GetMasId(void);
    bool IsConnected(const std::string &addr = "");
    int GetConnectState(void);
    void GetConnectDevices(std::list<RawAddress> &devices);
    void GetDeviceState(const std::string &addr, int &result);
    void GetDevicesByStates(std::vector<RawAddress> &devices, const std::vector<int> &states);
    void DisConnect(const std::string &addr = "");
    bool AcceptedNoCallBack(bool disable);
    bool IsBusy(const std::string &addr);

private:
    /**
     * @brief call SDP function : Add ServiceClassIDList Attribute to record
     *
     * @return int
     */
    int AddServiceClassIdList(void);

    /**
     * @brief call SDP function : Add ProtocolDescriptorList Attribute to record
     *
     * @return int
     */
    int AddProtocolDescriptorList(void);

    /**
     * @brief call SDP function : Add ServiceName Attribute to record
     *
     * @return int
     */
    int AddServiceName(void);

    /**
     * @brief Gets the default supported version
     *
     */
    void GetProfileVersion(void);

    /**
     * @brief call SDP function : Add BluetoothProfileDescriptorList Attribute to record
     *
     * @return int
     */
    int AddBluetoothProfileDescriptorList(void);

    /**
     * @brief call SDP function : Add Attribute to record
     *
     * @return int
     */
    int AddAttribute(void);

    /**
     * @brief call SDP function : Add Browse Group List to record
     *
     * @return int
     */
    int AddBrowseGroupList(void);

    /**
     * @brief Register sdp service
     *
     * @return int
     */
    int RegisterServiceRecord(void);

    /**
     * @brief Get device list according to address
     *
     */
    void GetDevicesNative(std::vector<RawAddress> &devices, const std::string &address);
    struct NativeData {
    public:
        explicit NativeData(MapMseInstance &masInstance);
        virtual ~NativeData() = default;
        bool StartTimer(std::function<void()> callback, int ms);
        void StopTimer();
        void IncomingStartTimer(const RawAddress &rawAddr);
        void IncomingTimeout(const RawAddress &rawAddr);
        void ProcessIncomingTimeout(const RawAddress &rawAddr);
        bool IsAccepted = false;
        bool IsBusy = false;

    private:
        std::unique_ptr<utility::Timer> timer_ = nullptr;
        MapMseInstance &masInstance_;
        BT_DISALLOW_COPY_AND_ASSIGN(NativeData);
    };
    std::unordered_map<std::string, std::unique_ptr<NativeData>> nativeMap_ {};
    utility::Dispatcher &dispatcher_;
    uint8_t masInstanceId_ = 0;
    // SDP Register GoepL2capPsm value
    uint16_t goepL2capPsm_ = 0;
    MapAccountItem accountItem_ {};
    bool smsSupport_ = false;
    uint8_t supportedMsgTypes_ = 0;
    MapObserver *mapObserver_ = nullptr;
    std::unordered_map<std::string, BTConnectState> stateMap_ {};
    std::recursive_mutex instanceMapMutex_ {};
    // SDP Register FRCOMM No
    uint8_t rfcommNo_ = 0;
    // SDP Register Service Name
    std::string serviceName_ = "";
    // SDP handle
    uint32_t sdpHandle_ = 0;
    // SDP profile version
    uint16_t profileVersion_ = 0;
    // SDP map supported features
    uint32_t supportedFeatures_ = 0;
    std::atomic_uint64_t databaseIdentifier_ = ATOMIC_VAR_INIT();
    std::atomic_uint64_t folderVersionCounter_ = ATOMIC_VAR_INIT();
    std::atomic_uint64_t conversationVersionCounter_ = ATOMIC_VAR_INIT();
    bool shutDownEnable_ = false;
    DECLARE_IMPL();
    BT_DISALLOW_COPY_AND_ASSIGN(MapMseInstance);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MSE_INSTANCE_H