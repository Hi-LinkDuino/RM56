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

#ifndef TRANSPORT_L2CAP_H
#define TRANSPORT_L2CAP_H

#include <map>
#include <mutex>
#include <stdint.h>
#include "l2cap_if.h"
#include "packet.h"
#include "raw_address.h"
#include "transport_def.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This L2capTransport class provides a set of methods that is interactive with L2CAP.
 */
class L2capTransport : public DataTransport {
public:
    /**
     * @brief Constructor.
     * @param createInfo  create info struct.
     */
    explicit L2capTransport(L2capTransportInfo &createInfo);

    /**
     * @brief Destructor.
     */
    virtual ~L2capTransport();

    /**
     * @brief The client initiates the connection.
     *
     * @return int
     */
    int Connect() override;

    /**
     * @brief The client initiates the disconnection request.
     *
     * @return int
     */
    int Disconnect() override;

    /**
     * @brief The server register to L2CAP.
     *
     * @return int
     */
    int RegisterServer() override;

    /**
     * @brief Close server.
     *
     * @return int
     */
    int RemoveServer(bool isDisable = true) override;

    /**
     * @brief The server accept the connection.
     *
     * @param addr remote device address.
     * @param scn the server channel.
     * @return int
     */
    int AcceptConnection(const RawAddress &addr, uint16_t psm) override;

    /**
     * @brief The server reject the connection.
     *
     * @param addr remote device address.
     * @param scn the server channel.
     * @return int
     */
    int RejectConnection(const RawAddress &addr, uint16_t psm) override;

    /**
     * @brief Receive data from L2CAP.
     * @param pkt  Data packet
     * @return int
     */
    int Read(Packet **pkt) override;

    /**
     * @brief Send data to L2CAP.
     *
     * @param pkt Data packet
     * @return int
     */
    int Write(Packet *pkt) override;

    /**
     * @brief Get the remote device address.
     *
     * @return RawAddress
     */
    RawAddress GetRemoteAddress() override;

    /**
     * @brief RegisterService to l2cap
     *
     * @param lpsm local channel id.
     */
    static int RegisterClientPsm(uint16_t lpsm);

    /**
     * @brief DeregisterService to l2cap
     *
     * @param lpsm local channel id.
     */
    static void DeregisterClientPsm(uint16_t lpsm);

private:
    /**
     * @brief L2cap event
     *
     * @param addr remote bluetooth address.
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportL2cConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context);

    /**
     * @brief L2cap event
     *
     * @param addr remote bluetooth address.
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportL2cSendDataCallback(uint16_t lcid, int result);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportRecvConnectionReqCallback(
        uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportRecvConnectionRspCallback(
        uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportRecvConfigReqCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportRecvConfigRspCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportRecvDisconnectionReqCallback(uint16_t lcid, uint8_t id, void *ctx);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportRecvDisconnectionRspCallback(uint16_t lcid, void *ctx);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportRecvDataCallback(uint16_t lcid, Packet *pkt, void *ctx);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    static void TransportRemoteBusyCallback(uint16_t lcid, uint8_t isBusy, void *ctx);

    /**
     * @brief L2cap event
     *
     * @param addr remote bluetooth address.
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    void TransportL2cConnectReqCallbackNative(L2capTransport *transport,
                                              const BtAddr &addr, uint16_t lcid, int result);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    void TransportRecvConnectionReqCallbackNative(
        L2capTransport *transport, uint16_t lcid, uint8_t id, const L2capConnectionInfo info, uint16_t lpsm);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    void TransportRecvConnectionRspCallbackNative(
        L2capTransport *transport, uint16_t lcid, const L2capConnectionInfo info, uint16_t result, uint16_t status);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    void TransportRecvConfigReqCallbackNative(
        L2capTransport *transport, uint16_t lcid, uint8_t id, const L2capConfigInfo cfg);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    void TransportRecvConfigRspCallbackNative(
        L2capTransport *transport, uint16_t lcid, const L2capConfigInfo cfg, uint16_t result);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    void TransportRecvDisconnectionReqCallbackNative(L2capTransport *transport, uint16_t lcid, uint8_t id);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    void TransportRecvDisconnectionRspCallbackNative(L2capTransport *transport, uint16_t lcid);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    void TransportDisconnectAbnormalCallbackNative(L2capTransport *transport, uint16_t lcid, uint8_t reason);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    void TransportRecvDataCallbackNative(L2capTransport *transport, uint16_t lcid, Packet *pkt);

    /**
     * @brief L2cap event
     *
     * @param lcid local channel id.
     * @param result if the operation is successful, otherwise the operation fails.
     */
    void TransportRemoteBusyCallbackNative(L2capTransport *transport, uint16_t lcid, uint8_t isBusy);

    /**
     * @brief When server accept a connection request, generate a new transport.
     *
     * @param addr remote device address.
     * @param handle l2cap connection handle.
     * @return L2capTransport*
     */
    L2capTransport *AddTransportInternal(RawAddress addr, uint16_t lcid);

    /**
     * @brief Find client transport.
     *
     * @param lcid l2cap connect id.
     * @return L2capTransport*
     */
    static L2capTransport *FindClientTransport(uint16_t lcid);

    /**
     * @brief Find accept transport.
     *
     * @param lcid l2cap connect id.
     * @return L2capTransport*
     */
    static L2capTransport *FindAcceptTransport(L2capTransport *transport, uint16_t lcid);

    /**
     * @brief get transport.
     *
     * @param lcid l2cap connect id.
     * @return L2capTransport*
     */
    static L2capTransport *GetTransport(uint16_t lcid, void *ctx);

    /**
     * @brief is server or not.
     *
     * @return true the role is server.
     * @return false the role is client
     */
    bool IsServer() const
    {
        return isServer_;
    }

    struct ConnectReqInfo {
        uint16_t lcid;
        uint8_t id;
    };

    // remote device address.
    RawAddress remoteAddr_ {};
    // local psm
    uint16_t lpsm_ {0};
    // remote psm
    uint16_t rpsm_ {0};
    // mtu
    uint16_t localMtu_ {0};
    uint16_t remoteMtu_ {0};
    // is server or not.
    bool isServer_ {false};
    // is connected or not.
    bool isConnected_ {false};
    // is remove server or not.
    bool isRemoveServer_ {false};
    // L2CAP handle
    uint16_t l2capHandle_ {0};
    // L2CAP config handle
    int l2capConfigState_ {0};
    // L2CAP id
    uint8_t l2capId_ {0};
    // The map manages the correspondence between new transport and rfcomm handle.
    std::map<uint16_t, L2capTransport *> transportMap_ {};
    // The map manages the correspondence between remote address and l2cap handle.
    std::map<RawAddress, ConnectReqInfo> handleMap_ {};
    // The map manages the correspondence between new transport and rfcomm handle.
    std::map<L2capTransport *, RawAddress> remoteAddrMap_ {};
    // the pointer of the DataTransportObserver
    DataTransportObserver &observer_;
    utility::Dispatcher &dispatcher_;
    // std::mutex mutex_;
    std::mutex transportMutex_ {};
    static std::recursive_mutex g_clientTransportMutex;

    static constexpr L2capService L2CAP_CALLBACK = {
        TransportRecvConnectionReqCallback,
        TransportRecvConnectionRspCallback,
        TransportRecvConfigReqCallback,
        TransportRecvConfigRspCallback,
        TransportRecvDisconnectionReqCallback,
        TransportRecvDisconnectionRspCallback,
        TransportDisconnectAbnormalCallback,
        TransportRecvDataCallback,
        TransportRemoteBusyCallback
    };

    static std::map<uint16_t, L2capTransport *> g_clientTransportMap;

    BT_DISALLOW_COPY_AND_ASSIGN(L2capTransport);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // TRANSPORT_L2CAP_H