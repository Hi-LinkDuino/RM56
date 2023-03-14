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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines map client service msn server object.
 *
 */

/**
 * @file map_mce_mns_server.h
 *
 * @brief map client service msn server header file .
 *
 */

#ifndef MAP_MNS_SERVER_H
#define MAP_MNS_SERVER_H

#include <atomic>
#include <memory>
#include <string>
#include "../obex/obex_headers.h"
#include "../obex/obex_mp_server.h"
#include "../obex/obex_utils.h"
#include "map_mce_service.h"
#include "map_mce_types.h"

namespace OHOS {
namespace bluetooth {
static const std::string MCE_MNS_SERVICE_NAME = "MAP MNS";  // MAP MNS-name
static const uint16_t MCE_MNS_GOEP_L2CAP_PSM_VALUE = 0x1021;
static const uint16_t MCE_MNS_SERVICE_CLASS_ID_NUMBER = 0x0001;
static const uint16_t MCE_MNS_SERVICE_CLASS_UUID = 0x1133;
static const uint16_t MCE_MNS_PROFILE_VERSION = 0x0104;
static const uint16_t MCE_MNS_PROFILE_DESCRIPTOR_UUID = 0x1134;
static const uint16_t MCE_MNS_PROFILE_DESCRIPTOR_NUMBER = 0x0001;
static const uint16_t MCE_MNS_PROTOCOL_DESCRIPTOR_NUMBER = 0x0003;
static const uint16_t MCE_MNS_PROTOCOL_DESCRIPTOR_PARAMETER_NUMBER = 0x01;

// Map Supported Features
static const uint16_t MCE_MNS_SUPPORTED_FEATURES_ATTRIBUTE_ID = 0x0317;
static const uint32_t MCE_MNS_SUPPORTED_FEATURES_V14 = 0x007EFFFF;  // only bit 16 (pbap contact cross) is 0
// GoepL2capPsm (MAP v1.2 and later)
static const uint16_t MCE_MNS_GOEP_L2CAP_PSM_ATTRIBUTE_ID = 0x0200;

/**
 * @brief map mns server object
 */
class MapMceMnsServer {
public:
    /**
     * @brief  copy operator
     */
    BT_DISALLOW_COPY_AND_ASSIGN(MapMceMnsServer);
    /**
     * @brief Construct a new Map Mce Mns Server object
     * @param  service
     * @param  config
     */
    explicit MapMceMnsServer(MapMceService &service, MasInstanceConfig config);
    /**
     * @brief Destroy the Map Mce Mns Server object
     */
    virtual ~MapMceMnsServer();
    /**
     * @brief start up
     * @return int
     */
    int StartUp(void);
    /**
     * @brief shutdown
     */
    void ShutDown(void);

private:
    /**
     * @brief Mns ObexServer Observer
     */
    class MceMnsObexServerObserver : public ObexServerObserver {
    public:
        /**
         * @brief Construct a new Mce Mns Obex Server Observer object
         * @param  mnsServer
         */
        explicit MceMnsObexServerObserver(MapMceMnsServer &mnsServer) : mceMnsServer_(mnsServer){};
        /**
         * @brief Destroy the Mce Mns Obex Server Observer object
         */
        ~MceMnsObexServerObserver() override;

    private:
        void OnTransportConnect(ObexIncomingConnect &incomingConnect) override;
        /**
         * @brief  On Transport Disconnected
         * @param  btAddr
         */
        void OnTransportDisconnected(const std::string &btAddr) override;
        /**
         * @brief  On Transport Error
         * @param  btAddr           bluetooth device address
         * @param  errCd            error code
         * @param  msg              error msg
         */
        void OnTransportError(const std::string &btAddr, int errCd, const std::string &msg) override;
        /**
         * @brief  On Connect
         * @param  session          obex session
         * @param  req              obex header
         */
        void OnConnect(ObexServerSession &session, const ObexHeader &req) override;
        /**
         * @brief  On Disconnect
         * @param  session          obex session
         * @param  req              obex header
         */
        void OnDisconnect(ObexServerSession &session, const ObexHeader &req) override;
        /**
         * @brief  receive Put cmd
         * @param  session          obex session
         * @param  req              obex header
         */
        void OnPut(ObexServerSession &session, const ObexHeader &req) override;
        /**
         * @brief  data receiving continue status
         * @param  session
         * @param  isBusy
         */
        void OnBusy(ObexServerSession &session, bool isBusy) const override;

        // mns server object
        MapMceMnsServer &mceMnsServer_;
    };
    /**
     * @brief Add Service ClassId List for sdp search
     * @return int
     */
    int AddServiceClassIdList(void);
    /**
     * @brief Add Protocol Descriptor List for sdp search
     * @return int
     */
    int AddProtocolDescriptorList(void);
    /**
     * @brief Add ServiceName
     * @return int
     */
    int AddServiceName(void);
    /**
     * @brief AddBluetooth Profile DescriptorList
     * @return int
     */
    int AddBluetoothProfileDescriptorList(void);
    /**
     * @brief Add Attribute for sdp
     * @return int
     */
    int AddAttribute(void);
    /**
     * @brief Register ServiceRecord
     * @return int
     */
    int RegisterServiceRecord(void);
    /**
     * @brief Create a Mas Sdp Record object
     * @return int
     */
    int CreateMasSdpRecord(void);
    /**
     * @brief Destroy MasSdpRecord for sdp
     */
    void DestroyMasSdpRecord(void);
    /**
     * @brief Register Service Security
     * @return int
     */
    int RegisterServiceSecurity(void) const;
    /**
     * @brief Deregister ServiceSecurity
     */
    void DeregisterServiceSecurity(void) const;
    // SDP Register RFCOMM No
    uint8_t rfcommNo_ = 0;
    // SDP Register GOEP L2cap Psm
    uint16_t goepL2capPsm_ = MCE_MNS_GOEP_L2CAP_PSM_VALUE;
    // mce obex mns observer object
    std::unique_ptr<MceMnsObexServerObserver> mceMnsObexObserver_ = nullptr;
    // obex server object
    std::unique_ptr<ObexServer> obexServer_ = nullptr;
    // SDP handle
    uint32_t sdpHandle_ = 0;
    // mns status
    std::atomic_int mnsStatus_ {MapMceServiceStateType::MAP_MCE_STATE_SHUTDOWN};
    // mce service
    MapMceService &mceService_;
    // mns config
    MasInstanceConfig mnsConfig_;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MNS_SERVER_H
