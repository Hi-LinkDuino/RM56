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

#ifndef MAP_MSE_MNSCLI_H
#define MAP_MSE_MNSCLI_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include "../obex/obex_mp_client.h"
#include "dispatcher.h"
#include "gap_if.h"
#include "map_mse_types.h"
#include "raw_address.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Callback function of MnsObserver
 *
 */
class MnsObserver {
public:
    /**
     * @brief A destructor used to delete the Mns Observer instance.
     *
     */
    virtual ~MnsObserver() = default;

    /**
     * @brief Called OnDisconnected
     *
     */
    virtual void OnDisconnected() = 0;
};

class MapMseMnscli {
public:
    explicit MapMseMnscli(utility::Dispatcher &dispatcher, MnsObserver &mnsObserver);

    virtual ~MapMseMnscli();

    /**
     * @brief The function start MNS Client in asynchronization mode
     *
     * @return int
     */
    int Connect(const RawAddress &remoteAddr, int masId);

    /**
     * @brief The function shutdown MNS Client
     *
     */
    void DisConnect(const RawAddress &remoteAddr, int masId);

    /**
     * @brief If the MCE supports the Message Notification feature and the the MCE had
     *        function to notify the MCE on events affecting the messages listings within
     *        the MSE's folder structure
     *        MAP specification #5.2 SendEvent Function
     * @param bda Bluetooth address
     * @param report Reference to the event
     * @param report_size Reference to the event size
     * @return int
     */
    int SendEvent(const RawAddress &bda, const std::string &report, size_t reportSize, uint8_t masInstanceId);

    int SdpSearch(const RawAddress &remoteAddr);

    void PostCallback(const RawAddress &remoteAddr, ObexClientConfig clientConfig, uint32_t remoteFeature);

    void SaveClientConfig(const RawAddress &remoteAddr, ObexClientConfig clientConfig, uint32_t remoteFeature);

    static void SdpSearchCallback(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context);
    static void GetRfcommNo(const SdpService &serviceAry, uint8_t &rfcommNo);
    static void GetVersionNumber(const SdpService &serviceAry, uint16_t &versionNumber);
    static void GetPsmRemoteFeature(const SdpService &serviceAry, uint16_t &psm, uint32_t &remoteFeature);
    uint32_t GetRemoteFeatures(const std::string &addr);
    bool IsDisconnected(void) const;
    void DeregisterL2capLPsm(void) const;

private:
    static const uint8_t MNS_TARGET[16];
    static const std::string HEADER_TYPE_EVENT;
    static const uint16_t SERVICE_CLASS_UUID = 0x1133;
    static const uint16_t GOEP_L2CAP_LPSM = 0x101D;
    std::recursive_mutex mnsMapMutex_ {};
    int RegistGap(const RawAddress &remoteAddr, bool isL2cap, uint16_t scn) const;
    void UnRegistGap(const RawAddress &remoteAddr, bool isL2cap, uint16_t scn) const;
    uint32_t GetConnectionId(const std::string &address) const;
    utility::Dispatcher &dispatcher_;
    MnsObserver &mnsObserver_;
    std::unordered_map<std::string, std::unique_ptr<ObexMpClient>> obexClientMap_ {};
    std::unordered_map<std::string, int> masIds_ {};
    std::unordered_map<std::string, ObexClientConfig> obexConfigMap_ {};
    std::unordered_map<std::string, BTConnectState> mnsStateMap_ {};
    std::unordered_map<std::string, uint32_t> connectionIdMap_ {};
    std::unordered_map<std::string, uint32_t> remoteFeatures_ {};
    void RemoveOnConnected(std::string address);
    class MseClientObserver : public ObexClientObserver {
    public:
        /**
         * @brief Construct a new Connect Observer object
         *
         * @param mnsClient Reference to the Mns instance
         */
        explicit MseClientObserver(MapMseMnscli &mnsClient);

        /**
         * @brief A destructor used to delete the Mns Client Observer object.
         *
         */
        ~MseClientObserver() override = default;

    private:
        /**
         * @brief Called OnTransportFailed
         * @param client ObexClient
         * @param errCd error code
         */
        void OnTransportFailed(ObexClient &client, int errCd) override;
        /**
         * @brief Called OnConnected
         *
         * @param client ObexClient
         * @param resp The Response from Server
         */
        void OnConnected(ObexClient &client, const ObexHeader &resp) override;
        /**
         * @brief Called OnConnectFailed
         *
         * @param client ObexClient
         * @param resp The Response from Server
         */
        void OnConnectFailed(ObexClient &client, const ObexHeader &resp) override;
        /**
         * @brief Called OnDisconnected
         *
         * @param client ObexClient
         */
        void OnDisconnected(ObexClient &client) override;
        /**
         * @brief Called OnActionCompleted
         *
         * @param client ObexClient
         * @param resp The Response from Server
         */
        void OnActionCompleted(ObexClient &client, const ObexHeader &resp) override;

        MapMseMnscli &mnsClient_;
        BT_DISALLOW_COPY_AND_ASSIGN(MseClientObserver);
    };
    std::unique_ptr<MseClientObserver> clientObserver_ = nullptr;
    BT_DISALLOW_COPY_AND_ASSIGN(MapMseMnscli);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MSE_MNSCLI_H