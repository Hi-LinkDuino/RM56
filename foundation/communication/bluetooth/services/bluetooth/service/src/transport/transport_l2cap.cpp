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

#include "transport_l2cap.h"
#include "log.h"
#include "log_util.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
std::map<uint16_t, L2capTransport *> L2capTransport::g_clientTransportMap {};
std::recursive_mutex L2capTransport::g_clientTransportMutex;

L2capTransport::L2capTransport(L2capTransportInfo &createInfo)
    : lpsm_(createInfo.lpsm),
      rpsm_(createInfo.rpsm),
      localMtu_(createInfo.mtu),
      observer_(createInfo.observer),
      dispatcher_(createInfo.dispatcher)
{
    if (createInfo.addr != nullptr) {
        HILOGI("remoteAddr: %{public}s, lpsm: %{public}hu, rpsm: %{public}hu, localMtu_: %{public}hu",
            GetEncryptAddr(remoteAddr_.GetAddress()).c_str(), lpsm_, rpsm_, localMtu_);
        remoteAddr_ = *createInfo.addr;
    }
}

L2capTransport::~L2capTransport()
{
}

int L2capTransport::Connect()
{
    LOG_INFO("[L2capTransport]%{public}s lpsm:0x%04X, rpsm:0x%04X", __func__, lpsm_, rpsm_);

    BtAddr btAddr;
    remoteAddr_.ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    return L2CIF_ConnectReq(&btAddr, lpsm_, rpsm_, this, TransportL2cConnectReqCallback);
}

int L2capTransport::Disconnect()
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, l2capHandle_);

    L2CIF_DisconnectionReq(l2capHandle_, nullptr);
    return 0;
}

int L2capTransport::RegisterServer()
{
    LOG_INFO("[L2capTransport]%{public}s", __func__);
    isServer_ = true;
    L2CIF_RegisterService(lpsm_, &L2CAP_CALLBACK, this, nullptr);
    return 0;
}

int L2capTransport::RemoveServer(bool isDisable)
{
    LOG_INFO("[L2capTransport]%{public}s", __func__);

    L2CIF_DeregisterService(lpsm_, nullptr);

    this->isRemoveServer_ = true;
    std::lock_guard<std::mutex> lock(this->transportMutex_);
    std::map<uint16_t, L2capTransport *>::iterator it;
    for (it = this->transportMap_.begin(); it != this->transportMap_.end(); ++it) {
        if (!it->second->isConnected_) {
            LOG_INFO("[L2capTransport]%{public}s delete transport", __func__);
            delete it->second;
            it->second = nullptr;
        }
    }
    return 0;
}

int L2capTransport::AcceptConnection(const RawAddress &addr, uint16_t psm)
{
    LOG_INFO("[L2capTransport]%{public}s ", __func__);

    uint16_t lcid = 0;
    uint8_t id = 0;
    {
        std::lock_guard<std::mutex> lock(transportMutex_);
        if (this->handleMap_.find(addr) != this->handleMap_.end()) {
            lcid = this->handleMap_.at(addr).lcid;
            id = this->handleMap_.at(addr).id;
        } else {
            LOG_DEBUG("[L2capTransport]%{public}s handle does not exist", __FUNCTION__);
        }
    }
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu id:%hhu", __func__, lcid, id);
    AddTransportInternal(addr, lcid);

    L2CIF_ConnectRsp(lcid, id, 0, 0, nullptr);

    L2capConfigInfo l2capConfigInfo;
    (void)memset_s(&l2capConfigInfo, sizeof(L2capConfigInfo), 0x00, sizeof(L2capConfigInfo));
    l2capConfigInfo.mtu = localMtu_;
    l2capConfigInfo.flushTimeout = L2CAP_CONFIG_FLUSHTTIMEOUT;
    l2capConfigInfo.fcs = L2CAP_CONFIG_FCS;
    l2capConfigInfo.rfc.mode = L2CAP_CONFIG_RFC_MODE;

    L2CIF_ConfigReq(lcid, &l2capConfigInfo, nullptr);

    return 0;
}

int L2capTransport::RejectConnection(const RawAddress &addr, uint16_t psm)
{
    LOG_INFO("[L2capTransport]%{public}s", __func__);

    uint16_t lcid = 0;
    uint8_t id = 0;
    {
        std::lock_guard<std::mutex> lock(transportMutex_);
        if (this->handleMap_.find(addr) != this->handleMap_.end()) {
            lcid = this->handleMap_.at(addr).lcid;
            id = this->handleMap_.at(addr).id;
        } else {
            LOG_DEBUG("[L2capTransport]%{public}s handle does not exist", __FUNCTION__);
        }
    }
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu id:%hhu", __func__, lcid, id);
    L2CIF_ConnectRsp(lcid, id, L2CAP_NO_RESOURCES_AVAILABLE, 0, nullptr);
    return 0;
}

int L2capTransport::Read(Packet **pkt)
{
    LOG_INFO("[L2capTransport]%{public}s", __func__);
    return 0;
}

int L2capTransport::Write(Packet *pkt)
{
    LOG_INFO("[L2capTransport]%{public}s", __func__);

    return L2CIF_SendData(this->l2capHandle_, pkt, NULL);
}

RawAddress L2capTransport::GetRemoteAddress()
{
    LOG_INFO("[L2capTransport]%{public}s", __func__);

    if (IsServer()) {
        return remoteAddrMap_.at(this);
    } else {
        return remoteAddr_;
    }
}

int L2capTransport::RegisterClientPsm(uint16_t lpsm)
{
    LOG_INFO("[L2capTransport]%{public}s lpsm:0x%04X", __func__, lpsm);
    return L2CIF_RegisterService(lpsm, &L2CAP_CALLBACK, nullptr, nullptr);
}

void L2capTransport::DeregisterClientPsm(uint16_t lpsm)
{
    LOG_INFO("[L2capTransport]%{public}s lpsm:0x%04X", __func__, lpsm);
    L2CIF_DeregisterService(lpsm, nullptr);
}

L2capTransport *L2capTransport::AddTransportInternal(RawAddress addr, uint16_t lcid)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);
    L2capTransportInfo createInfo = {&addr, lpsm_, rpsm_, localMtu_, observer_, dispatcher_};
    auto newTransport = new (std::nothrow) L2capTransport(createInfo);
    if (newTransport != nullptr) {
        newTransport->l2capHandle_ = lcid;
        newTransport->l2capConfigState_ |= L2CAP_SEND_CONFIG_REQ;
        {
            std::lock_guard<std::mutex> lock(transportMutex_);
            transportMap_.insert(std::pair<uint16_t, L2capTransport *>(lcid, newTransport));
            remoteAddrMap_.insert(std::pair<L2capTransport *, RawAddress>(newTransport, addr));
        }
        return newTransport;
    } else {
        return nullptr;
    }
}

L2capTransport *L2capTransport::FindClientTransport(uint16_t lcid)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capTransport *clientTransport = nullptr;
    std::lock_guard<std::recursive_mutex> lk(L2capTransport::g_clientTransportMutex);
    if (g_clientTransportMap.find(lcid) != g_clientTransportMap.end()) {
        clientTransport = g_clientTransportMap.at(lcid);
    } else {
        LOG_DEBUG("[L2capTransport]%{public}s transport does not exist", __func__);
    }

    return clientTransport;
}

L2capTransport *L2capTransport::FindAcceptTransport(L2capTransport *transport, uint16_t lcid)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capTransport *l2capTransport = nullptr;
    if (transport->IsServer()) {
        std::lock_guard<std::mutex> lock(transport->transportMutex_);
        if (transport->transportMap_.find(lcid) != transport->transportMap_.end()) {
            l2capTransport = transport->transportMap_.at(lcid);
        }
    } else {
        l2capTransport = transport;
    }

    return l2capTransport;
}

L2capTransport *L2capTransport::GetTransport(uint16_t lcid, void *ctx)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capTransport *transport = FindClientTransport(lcid);
    if ((transport == nullptr) && (ctx != nullptr)) {
        transport = static_cast<L2capTransport *>(ctx);
    }
    return transport;
}

void L2capTransport::TransportL2cConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    if (context == nullptr) {
        LOG_ERROR("[L2capTransport]%{public}s context is null.", __func__);
        return;
    }

    auto transport = static_cast<L2capTransport *>(context);
    BtAddr btAddr;
    (void)memcpy_s(&btAddr, sizeof(BtAddr), addr, sizeof(BtAddr));

    if (result != BT_NO_ERROR) {
        LOG_ERROR("[L2capTransport]%{public}s", __func__);
        return;
    }
    if (lcid != 0) {
        transport->l2capHandle_ = lcid;
        LOG_INFO("[L2capTransport]%{public}s l2capHandle_:%hu", __func__, transport->l2capHandle_);
        std::lock_guard<std::recursive_mutex> lk(L2capTransport::g_clientTransportMutex);
        LOG_INFO("[L2capTransport]%{public}s clientTransportMap size:%{public}zu",
            __func__, g_clientTransportMap.size());
        std::map<uint16_t, L2capTransport *>::iterator it;
        for (it = g_clientTransportMap.begin(); it != g_clientTransportMap.end(); ++it) {
            LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, it->first);
        }
        g_clientTransportMap[lcid] = transport;
    }
    transport->dispatcher_.PostTask(
        std::bind(&L2capTransport::TransportL2cConnectReqCallbackNative, transport, transport, btAddr, lcid, result));
}

void L2capTransport::TransportRecvConnectionReqCallback(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    if (ctx == nullptr) {
        LOG_ERROR("[L2capTransport]%{public}s context is null.", __func__);
        return;
    }

    auto transport = static_cast<L2capTransport *>(ctx);
    L2capConnectionInfo l2capConnectionInfo;
    (void)memcpy_s(&l2capConnectionInfo, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));
    transport->dispatcher_.PostTask(std::bind(&L2capTransport::TransportRecvConnectionReqCallbackNative,
        transport,
        transport,
        lcid,
        id,
        l2capConnectionInfo,
        lpsm));
}

void L2capTransport::TransportRecvConnectionRspCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capTransport *transport = GetTransport(lcid, ctx);
    if (transport == nullptr) {
        LOG_DEBUG("[L2capTransport]%{public}s transport is null.", __func__);
        return;
    }

    L2capConnectionInfo l2capConnectionInfo;
    (void)memcpy_s(&l2capConnectionInfo, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));
    transport->dispatcher_.PostTask(std::bind(&L2capTransport::TransportRecvConnectionRspCallbackNative,
        transport,
        transport,
        lcid,
        l2capConnectionInfo,
        result,
        status));
}

void L2capTransport::TransportRecvConfigReqCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capTransport *transport = GetTransport(lcid, ctx);

    if (transport == nullptr) {
        LOG_DEBUG("[L2capTransport]%{public}s transport does not exist", __func__);
        return;
    }

    L2capConfigInfo l2capConfigInfo;
    (void)memcpy_s(&l2capConfigInfo, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    transport->dispatcher_.PostTask(std::bind(
        &L2capTransport::TransportRecvConfigReqCallbackNative, transport, transport, lcid, id, l2capConfigInfo));
}

void L2capTransport::TransportRecvConfigRspCallback(
    uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capTransport *transport = GetTransport(lcid, ctx);

    if (transport == nullptr) {
        LOG_DEBUG("[L2capTransport]%{public}s transport does not exist", __func__);
        return;
    }
    L2capConfigInfo l2capConfigInfo;
    memcpy_s(&l2capConfigInfo, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    transport->dispatcher_.PostTask(std::bind(
        &L2capTransport::TransportRecvConfigRspCallbackNative, transport, transport, lcid, l2capConfigInfo, result));
}

void L2capTransport::TransportRecvDisconnectionReqCallback(uint16_t lcid, uint8_t id, void *ctx)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capTransport *transport = GetTransport(lcid, ctx);

    if (transport == nullptr) {
        LOG_DEBUG("[L2capTransport]%{public}s transport does not exist", __func__);
        return;
    }

    {
        std::lock_guard<std::recursive_mutex> lk(L2capTransport::g_clientTransportMutex);
        g_clientTransportMap.erase(lcid);
    }
    transport->dispatcher_.PostTask(
        std::bind(&L2capTransport::TransportRecvDisconnectionReqCallbackNative, transport, transport, lcid, id));
}

void L2capTransport::TransportRecvDisconnectionRspCallback(uint16_t lcid, void *ctx)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capTransport *transport = GetTransport(lcid, ctx);

    if (transport == nullptr) {
        LOG_DEBUG("[L2capTransport]%{public}s transport does not exist", __func__);
        return;
    }

    {
        std::lock_guard<std::recursive_mutex> lk(L2capTransport::g_clientTransportMutex);
        g_clientTransportMap.erase(lcid);
    }
    transport->dispatcher_.PostTask(
        std::bind(&L2capTransport::TransportRecvDisconnectionRspCallbackNative, transport, transport, lcid));
}

void L2capTransport::TransportDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capTransport *transport = GetTransport(lcid, ctx);

    if (transport == nullptr) {
        LOG_DEBUG("[L2capTransport]%{public}s transport does not exist", __func__);
        return;
    }

    {
        std::lock_guard<std::recursive_mutex> lk(L2capTransport::g_clientTransportMutex);
        g_clientTransportMap.erase(lcid);
    }
    transport->dispatcher_.PostTask(
        std::bind(&L2capTransport::TransportDisconnectAbnormalCallbackNative, transport, transport, lcid, reason));
}

void L2capTransport::TransportRecvDataCallback(uint16_t lcid, Packet *pkt, void *ctx)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capTransport *transport = GetTransport(lcid, ctx);

    if (transport == nullptr) {
        LOG_DEBUG("[L2capTransport]%{public}s transport does not exist", __func__);
        return;
    }
    Packet *pktref = PacketRefMalloc(pkt);
    transport->dispatcher_.PostTask(
        std::bind(&L2capTransport::TransportRecvDataCallbackNative, transport, transport, lcid, pktref));
}

void L2capTransport::TransportRemoteBusyCallback(uint16_t lcid, uint8_t isBusy, void *ctx)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu isBusy:%hhu", __func__, lcid, isBusy);

    L2capTransport *transport = GetTransport(lcid, ctx);

    if (transport == nullptr) {
        LOG_DEBUG("[L2capTransport]%{public}s transport does not exist", __func__);
        return;
    }
    transport->dispatcher_.PostTask(
        std::bind(&L2capTransport::TransportRemoteBusyCallbackNative, transport, transport, lcid, isBusy));
}

void L2capTransport::TransportL2cConnectReqCallbackNative(
    L2capTransport *transport, const BtAddr &addr, uint16_t lcid, int result)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu result:%{public}d", __func__, lcid, result);

    if (lcid == 0) {
        transport->observer_.OnTransportError(transport, CONNECT_FAIL);
        return;
    }
}

void L2capTransport::TransportRecvConnectionReqCallbackNative(
    L2capTransport *transport, uint16_t lcid, uint8_t id, const L2capConnectionInfo info, uint16_t lpsm)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    if (transport->isRemoveServer_) {
        LOG_INFO("[L2capTransport]%{public}s server is removed", __func__);
        L2CIF_ConnectRsp(lcid, id, L2CAP_NO_RESOURCES_AVAILABLE, 0, nullptr);
        return;
    }

    L2capConnectionInfo l2capConnectionInfo;
    memcpy_s(&l2capConnectionInfo, sizeof(L2capConnectionInfo), &info, sizeof(L2capConnectionInfo));
    ConnectReqInfo connectReqInfo = {};
    connectReqInfo.id = id;
    connectReqInfo.lcid = lcid;
    {
        std::lock_guard<std::mutex> lock(transport->transportMutex_);
        transport->handleMap_[RawAddress::ConvertToString(l2capConnectionInfo.addr.addr)] = connectReqInfo;
    }
    transport->observer_.OnConnectIncoming(RawAddress::ConvertToString(l2capConnectionInfo.addr.addr), lpsm);
}

void L2capTransport::TransportRecvConnectionRspCallbackNative(
    L2capTransport *transport, uint16_t lcid, const L2capConnectionInfo info, uint16_t result, uint16_t status)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu result:%hu", __func__, lcid, result);

    if (result == L2CAP_CONNECTION_PENDING) {
        LOG_DEBUG("[L2capTransport]%{public}s L2CAP Pending.", __func__);
        return;
    }

    if (result != L2CAP_CONNECTION_SUCCESSFUL) {
        LOG_DEBUG("[L2capTransport]%{public}s L2CAP Connect fail.", __func__);
        transport->observer_.OnTransportError(transport, CONNECT_FAIL);
        {
            std::lock_guard<std::recursive_mutex> lk(L2capTransport::g_clientTransportMutex);
            g_clientTransportMap.erase(lcid);
        }
        return;
    }

    LOG_INFO("[L2capTransport]%{public}s lcid:%hu l2capHandle_:%hu", __func__, lcid, transport->l2capHandle_);
    if (lcid != transport->l2capHandle_) {
        LOG_ERROR("[L2capTransport]%{public}s L2CAP lcid error.", __func__);
        return;
    }

    L2capConfigInfo l2capConfigInfo;
    (void)memset_s(&l2capConfigInfo, sizeof(L2capConfigInfo), 0x00, sizeof(L2capConfigInfo));
    l2capConfigInfo.mtu = localMtu_;
    l2capConfigInfo.flushTimeout = 0xFFFF;
    l2capConfigInfo.fcs = 0x01;
    l2capConfigInfo.rfc.mode = 0x03;
    l2capConfigState_ |= L2CAP_SEND_CONFIG_REQ;
    L2CIF_ConfigReq(lcid, &l2capConfigInfo, nullptr);
}

void L2capTransport::TransportRecvConfigReqCallbackNative(
    L2capTransport *transport, uint16_t lcid, uint8_t id, const L2capConfigInfo cfg)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2capConfigInfo l2capConfigInfo;
    memcpy_s(&l2capConfigInfo, sizeof(L2capConfigInfo), &cfg, sizeof(L2capConfigInfo));
    remoteMtu_ = l2capConfigInfo.mtu;
    L2CIF_ConfigRsp(lcid, id, &l2capConfigInfo, 0, nullptr);

    L2capTransport *l2capTransport = FindAcceptTransport(transport, lcid);

    if (l2capTransport == nullptr) {
        LOG_INFO("[L2capTransport]%{public}s l2capTransport does not exist.", __func__);
        return;
    }

    l2capTransport->l2capConfigState_ |= L2CAP_RECV_CONFIG_REQ;
    if (l2capTransport->l2capConfigState_ == L2CAP_CONFIG_COMPLETE) {
        l2capTransport->isConnected_ = true;
        transport->observer_.OnConnected(l2capTransport, transport->remoteMtu_, transport->localMtu_);
    }
}

void L2capTransport::TransportRecvConfigRspCallbackNative(
    L2capTransport *transport, uint16_t lcid, const L2capConfigInfo cfg, uint16_t result)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu result:%hu", __func__, lcid, result);

    L2capTransport *l2capTransport = FindAcceptTransport(transport, lcid);

    if (l2capTransport == nullptr) {
        LOG_INFO("[L2capTransport]%{public}s l2capTransport does not exist.", __func__);
        return;
    }

    if (result != L2CAP_SUCCESS) {
        std::map<RawAddress, ConnectReqInfo>::iterator it;
        if (!transport->IsServer()) {
            transport->observer_.OnTransportError(transport, CONNECT_FAIL);
            {
                std::lock_guard<std::recursive_mutex> lk(L2capTransport::g_clientTransportMutex);
                g_clientTransportMap.erase(lcid);
            }

            L2CIF_DisconnectionReq(lcid, nullptr);
            return;
        }

        std::lock_guard<std::mutex> lock(transport->transportMutex_);
        for (it = transport->handleMap_.begin(); it != transport->handleMap_.end(); ++it) {
            if (it->second.lcid == lcid) {
                transport->remoteAddr_ = it->first;
                transport->observer_.OnIncomingDisconnected(transport->remoteAddr_);
                transport->handleMap_.erase(it);
                break;
            }
        }
        transport->remoteAddrMap_.erase(l2capTransport);
        transport->transportMap_.erase(lcid);
        delete l2capTransport;
        L2CIF_DisconnectionReq(lcid, nullptr);
        return;
    }

    l2capTransport->l2capConfigState_ |= L2CAP_RECV_CONFIG_RSP;
    if (l2capTransport->l2capConfigState_ == L2CAP_CONFIG_COMPLETE) {
        l2capTransport->isConnected_ = true;
        transport->observer_.OnConnected(l2capTransport, transport->remoteMtu_, transport->localMtu_);
    }
}

void L2capTransport::TransportRecvDisconnectionReqCallbackNative(L2capTransport *transport, uint16_t lcid, uint8_t id)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    L2CIF_DisconnectionRsp(lcid, id, nullptr);

    if (!transport->IsServer()) {
        transport->observer_.OnDisconnected(transport);
        return;
    }

    std::lock_guard<std::mutex> lock(transport->transportMutex_);
    if (transport->transportMap_.find(lcid) != transport->transportMap_.end()) {
        std::map<RawAddress, ConnectReqInfo>::iterator it;
        for (it = transport->handleMap_.begin(); it != transport->handleMap_.end(); ++it) {
            if (it->second.lcid == lcid) {
                transport->handleMap_.erase(it);
                break;
            }
        }

        auto transportTmp = transport->transportMap_.at(lcid);
        transport->transportMap_.erase(lcid);
        transport->remoteAddrMap_.erase(transportTmp);
        transport->observer_.OnDisconnected(transportTmp);
    } else {
        LOG_ERROR("[L2capTransport]%{public}s handle:%hu transport does not exist", __FUNCTION__, lcid);
        std::map<RawAddress, ConnectReqInfo>::iterator it;
        for (it = transport->handleMap_.begin(); it != transport->handleMap_.end(); ++it) {
            if (it->second.lcid == lcid) {
                transport->remoteAddr_ = it->first;
                transport->observer_.OnIncomingDisconnected(transport->remoteAddr_);
                break;
            }
        }
    }
}

void L2capTransport::TransportRecvDisconnectionRspCallbackNative(L2capTransport *transport, uint16_t lcid)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    if (!transport->IsServer()) {
        transport->observer_.OnDisconnectSuccess(transport);
        return;
    }

    std::lock_guard<std::mutex> lock(transport->transportMutex_);
    if (transport->transportMap_.find(lcid) != transport->transportMap_.end()) {
        std::map<RawAddress, ConnectReqInfo>::iterator it;
        for (it = transport->handleMap_.begin(); it != transport->handleMap_.end(); ++it) {
            if (it->second.lcid == lcid) {
                transport->handleMap_.erase(it);
                break;
            }
        }

        auto transportTmp = transport->transportMap_.at(lcid);
        transport->transportMap_.erase(lcid);
        transport->remoteAddrMap_.erase(transportTmp);
        transport->observer_.OnDisconnectSuccess(transportTmp);
    } else {
        LOG_ERROR("[L2capTransport]%{public}s handle:%hu transport does not exist", __FUNCTION__, lcid);
    }
}

void L2capTransport::TransportDisconnectAbnormalCallbackNative(L2capTransport *transport, uint16_t lcid, uint8_t reason)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    if (!transport->IsServer()) {
        if (reason == L2CAP_STATE_COLLISION) {
            transport->observer_.OnTransportError(transport, CONNECT_COLLISION);
        } else {
            transport->observer_.OnDisconnected(transport);
        }
        return;
    }

    std::lock_guard<std::mutex> lock(transport->transportMutex_);
    if (transport->transportMap_.find(lcid) != transport->transportMap_.end()) {
        std::map<RawAddress, ConnectReqInfo>::iterator it;
        for (it = transport->handleMap_.begin(); it != transport->handleMap_.end(); ++it) {
            if (it->second.lcid == lcid) {
                transport->handleMap_.erase(it);
                break;
            }
        }

        auto transportTmp = transport->transportMap_.at(lcid);
        transport->transportMap_.erase(lcid);
        transport->remoteAddrMap_.erase(transportTmp);
        transport->observer_.OnDisconnected(transportTmp);
    } else {
        LOG_ERROR("[L2capTransport]%{public}s handle:%hu transport does not exist", __FUNCTION__, lcid);
        std::map<RawAddress, ConnectReqInfo>::iterator it;
        for (it = transport->handleMap_.begin(); it != transport->handleMap_.end(); ++it) {
            if (it->second.lcid == lcid) {
                transport->remoteAddr_ = it->first;
                transport->observer_.OnIncomingDisconnected(transport->remoteAddr_);
                break;
            }
        }
    }
}

void L2capTransport::TransportRecvDataCallbackNative(L2capTransport *transport, uint16_t lcid, Packet *pkt)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu", __func__, lcid);

    if (!transport->IsServer()) {
        transport->observer_.OnDataAvailable(transport, pkt);
        return;
    }

    std::lock_guard<std::mutex> lock(transport->transportMutex_);
    if (transport->transportMap_.find(lcid) != transport->transportMap_.end()) {
        transport->observer_.OnDataAvailable(transport->transportMap_.at(lcid), pkt);
    } else {
        LOG_ERROR("[L2capTransport]%{public}s handle:%hu transport does not exist", __FUNCTION__, lcid);
    }
}

void L2capTransport::TransportRemoteBusyCallbackNative(L2capTransport *transport, uint16_t lcid, uint8_t isBusy)
{
    LOG_INFO("[L2capTransport]%{public}s lcid:%hu isBusy:%hhu", __func__, lcid, isBusy);

    if (!transport->IsServer()) {
        transport->observer_.OnDataBusy(transport, isBusy);
        return;
    }

    std::lock_guard<std::mutex> lock(transport->transportMutex_);
    if (transport->transportMap_.find(lcid) != transport->transportMap_.end()) {
        transport->observer_.OnDataBusy(transport->transportMap_.at(lcid), isBusy);
    } else {
        LOG_ERROR("[L2capTransport]%{public}s handle:%hu transport does not exist", __FUNCTION__, lcid);
    }
}
}  // namespace bluetooth
}  // namespace OHOS