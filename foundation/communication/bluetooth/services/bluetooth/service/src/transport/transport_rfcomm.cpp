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

#include "transport_rfcomm.h"
#include "log.h"
#include "log_util.h"
#include "securec.h"
#include "transport_def.h"

namespace OHOS {
namespace bluetooth {
const int SOCK_RFCOMM_EVENTS = RFCOMM_CHANNEL_EV_CONNECT_INCOMING | RFCOMM_CHANNEL_EV_CONNECT_SUCCESS |
                               RFCOMM_CHANNEL_EV_CONNECT_FAIL | RFCOMM_CHANNEL_EV_DISCONNECTED |
                               RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS | RFCOMM_CHANNEL_EV_DISCONNECT_FAIL |
                               RFCOMM_CHANNEL_EV_REV_DATA | RFCOMM_CHANNEL_EV_FC_ON;

RfcommTransport::RfcommTransport(
    const RawAddress *addr, uint8_t scn, uint16_t mtu, DataTransportObserver &observer, utility::Dispatcher &dispatcher)
    : scn_(scn), mtu_(mtu), observer_(observer), dispatcher_(dispatcher)
{
    if (addr != nullptr) {
        remoteAddr_ = *addr;
    }
}

RfcommTransport::~RfcommTransport()
{}

int RfcommTransport::Connect()
{
    LOG_INFO("[RfcommTransport]%{public}s", __func__);
    BtAddr btAddr;
    remoteAddr_.ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    RfcommConnectReqInfo reqInfo;
    reqInfo.mtu = mtu_;
    reqInfo.scn = scn_;
    reqInfo.eventMask = SOCK_RFCOMM_EVENTS;
    reqInfo.context = this;
    reqInfo.callback = &TransportRfcEventCallback;
    (void)memcpy_s(&reqInfo.addr, sizeof(BtAddr), &btAddr, sizeof(BtAddr));
    return RFCOMM_ConnectChannel(&reqInfo, &rfcHandle_);
}

int RfcommTransport::Disconnect()
{
    LOG_INFO("[RfcommTransport]%{public}s", __func__);

    return RFCOMM_DisconnectChannel(rfcHandle_);
}

int RfcommTransport::RegisterServer()
{
    LOG_INFO("[RfcommTransport]%{public}s", __func__);

    isServer_ = true;

    return RFCOMM_RegisterServer(scn_, mtu_, SOCK_RFCOMM_EVENTS, TransportRfcEventCallback, this);
}

int RfcommTransport::RemoveServer(bool isDisable)
{
    LOG_INFO("[RfcommTransport]%{public}s", __func__);

    RFCOMM_FreeServerNum(this->scn_);
    if (isDisable) {
        return RFCOMM_DeregisterServer(this->scn_, true);
    } else {
        return RFCOMM_DeregisterServer(this->scn_, false);
    }
}

int RfcommTransport::AcceptConnection(const RawAddress &addr, uint16_t scn)
{
    HILOGI("[RfcommTransport] RawAddress:%{public}s", GetEncryptAddr(addr.GetAddress()).c_str());
    if (this->handleMap_.find(addr) != this->handleMap_.end()) {
        this->rfcHandle_ = this->handleMap_.at(addr);
    } else {
        LOG_ERROR("[RfcommTransport]%{public}s handle:%hu: handle does not exist", __FUNCTION__, this->rfcHandle_);
    }

    return RFCOMM_AcceptConnection(this->rfcHandle_);
}

int RfcommTransport::RejectConnection(const RawAddress &addr, uint16_t scn)
{
    LOG_INFO("[RfcommTransport]%{public}s", __func__);

    if (this->handleMap_.find(addr) != this->handleMap_.end()) {
        this->rfcHandle_ = this->handleMap_.at(addr);
    } else {
        LOG_ERROR("[RfcommTransport]%{public}s handle:%hu: handle does not exist", __FUNCTION__, this->rfcHandle_);
    }

    return RFCOMM_RejectConnection(this->rfcHandle_);
}

RawAddress RfcommTransport::GetRemoteAddress()
{
    LOG_INFO("[RfcommTransport]%{public}s", __func__);

    if (IsServer()) {
        return remoteAddrMap_.at(this);
    } else {
        return remoteAddr_;
    }
}

int RfcommTransport::Read(Packet **pkt)
{
    LOG_INFO("[RfcommTransport]%{public}s", __func__);

    return RFCOMM_Read(this->rfcHandle_, pkt);
}

int RfcommTransport::Write(Packet *pkt)
{
    LOG_INFO("[RfcommTransport]%{public}s", __func__);

    return RFCOMM_Write(this->rfcHandle_, pkt);
}

RfcommTransport *RfcommTransport::AddTransportInternal(RawAddress addr, uint16_t handle)
{
    LOG_INFO("[RfcommTransport]%{public}s", __func__);

    RfcommTransport *newTransport = new (std::nothrow) RfcommTransport(&addr, scn_, mtu_, observer_, dispatcher_);
    if (newTransport != nullptr) {
        newTransport->rfcHandle_ = handle;
        {
            std::lock_guard<std::mutex> lock(transportMutex_);
            transportMap_.insert(std::pair<uint16_t, RfcommTransport *>(handle, newTransport));
        }
        return newTransport;
    } else {
        return nullptr;
    }
}

void RfcommTransport::TransportRfcEventCallback(uint16_t handle, uint32_t event,
                                                const void *eventData, void *context)
{
    LOG_INFO("[RfcommTransport]%{public}s handle:%hu event:%u", __func__, handle, event);

    auto transport = static_cast<RfcommTransport *>(context);

    switch (event) {
        case RFCOMM_CHANNEL_EV_CONNECT_INCOMING: {
            RfcommIncomingInfo incomingEvent;
            (void)memcpy_s(&incomingEvent, sizeof(RfcommIncomingInfo), eventData, sizeof(RfcommIncomingInfo));
            transport->addressMap_.insert(
                std::pair<uint16_t, RawAddress>(handle, RawAddress::ConvertToString(incomingEvent.addr.addr)));
            LOG_INFO("[RfcommTransport]%{public}s handle:%hu RawAddress:%{public}s",
                __func__,
                handle,
                RawAddress::ConvertToString(incomingEvent.addr.addr).GetAddress().c_str());
            transport->handleMap_.insert(
                std::pair<RawAddress, uint16_t>(RawAddress::ConvertToString(incomingEvent.addr.addr), handle));
            transport->observer_.OnConnectIncoming(
                RawAddress::ConvertToString(incomingEvent.addr.addr), incomingEvent.scn);
            break;
        }
        case RFCOMM_CHANNEL_EV_CONNECT_SUCCESS:
            TransportRfcConnectSuccess(handle, eventData, transport);
            break;
        case RFCOMM_CHANNEL_EV_CONNECT_FAIL:
            TransportRfcConnectFail(handle, transport);
            break;
        case RFCOMM_CHANNEL_EV_DISCONNECTED:
            TransportRfcDisconnected(handle, transport);
            break;
        case RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS:
            TransportRfcDisconnectSuccess(handle, transport);
            break;
        case RFCOMM_CHANNEL_EV_DISCONNECT_FAIL:
            TransportRfcDisconnectFail(handle, transport);
            break;
        case RFCOMM_CHANNEL_EV_FC_ON:
            TransportRfcFcOn(handle, transport);
            break;
        case RFCOMM_CHANNEL_EV_REV_DATA:
            TransportRfcRecvData(handle, transport);
            break;
        default:
            break;
    }
}

void RfcommTransport::TransportRfcConnectSuccess(uint16_t handle, const void *eventData, RfcommTransport *transport)
{
    LOG_INFO("[RfcommTransport]%{public}s handle:%hu ", __func__, handle);

    RfcommConnectedInfo connectedEvent;
    (void)memcpy_s(&connectedEvent, sizeof(RfcommConnectedInfo), eventData, sizeof(RfcommConnectedInfo));

    if (transport->IsServer()) {
        if (transport->addressMap_.find(handle) != transport->addressMap_.end()) {
            transport->remoteAddr_ = transport->addressMap_.at(handle);
        } else {
            LOG_ERROR("[RfcommTransport]%{public}s handle:%hu: address does not exist", __FUNCTION__, handle);
        }
        RfcommTransport *newTransport = transport->AddTransportInternal(transport->remoteAddr_, handle);
        transport->remoteAddrMap_.insert(
            std::pair<RfcommTransport *, RawAddress>(newTransport, transport->remoteAddr_));
        transport->observer_.OnConnected(newTransport, connectedEvent.sendMTU, connectedEvent.recvMTU);
    } else {
        transport->observer_.OnConnected(transport, connectedEvent.sendMTU, connectedEvent.recvMTU);
    }
}

void RfcommTransport::TransportRfcConnectFail(uint16_t handle, RfcommTransport *transport)
{
    LOG_INFO("[RfcommTransport]%{public}s handle:%hu ", __func__, handle);

    if (transport->IsServer()) {
        if (transport->transportMap_.find(handle) != transport->transportMap_.end()) {
            transport->observer_.OnTransportError(transport->transportMap_.at(handle), CONNECT_FAIL);
        } else {
            LOG_ERROR("[RfcommTransport]%{public}s handle:%hu: RFCOMM_CHANNEL_EV_CONNECT_FAIL transport does not exist",
                __FUNCTION__,
                handle);
            std::map<RawAddress, uint16_t>::iterator it;
            for (it = transport->handleMap_.begin(); it != transport->handleMap_.end(); ++it) {
                if (it->second == handle) {
                    LOG_INFO("[RfcommTransport]%{public}s erase handle map", __func__);
                    transport->handleMap_.erase(it);
                    break;
                }
            }
            transport->addressMap_.erase(handle);
        }
    } else {
        transport->observer_.OnTransportError(transport, CONNECT_FAIL);
    }
}

void RfcommTransport::TransportRfcDisconnected(uint16_t handle, RfcommTransport *transport)
{
    LOG_INFO("[RfcommTransport]%{public}s handle:%hu ", __func__, handle);

    if (!transport->IsServer()) {
        transport->observer_.OnDisconnected(transport);
        return;
    }
    std::lock_guard<std::mutex> lock(transport->transportMutex_);
    if (transport->transportMap_.find(handle) != transport->transportMap_.end()) {
        std::map<RawAddress, uint16_t>::iterator it;
        for (it = transport->handleMap_.begin(); it != transport->handleMap_.end(); ++it) {
            if (it->second == handle) {
                LOG_INFO("[RfcommTransport]%{public}s erase handle map", __func__);
                transport->handleMap_.erase(it);
                break;
            }
        }
        auto transportTmp = transport->transportMap_.at(handle);
        transport->transportMap_.erase(handle);
        transport->addressMap_.erase(handle);
        transport->observer_.OnDisconnected(transportTmp);
    } else {
        LOG_ERROR(
            "[RfcommTransport]%{public}s handle:%hu transport does not exist", __FUNCTION__, handle);
        if (transport->addressMap_.find(handle) != transport->addressMap_.end()) {
            transport->observer_.OnIncomingDisconnected(transport->addressMap_.at(handle));
        } else {
            LOG_ERROR("[RfcommTransport]%{public}s handle:%hu: transport and address does not exist", __FUNCTION__, handle);
        }
    }
}

void RfcommTransport::TransportRfcDisconnectSuccess(uint16_t handle, RfcommTransport *transport)
{
    LOG_INFO("[RfcommTransport]%{public}s handle:%hu ", __func__, handle);

    if (!transport->IsServer()) {
        transport->observer_.OnDisconnectSuccess(transport);
        return;
    }
    std::lock_guard<std::mutex> lock(transport->transportMutex_);
    if (transport->transportMap_.find(handle) != transport->transportMap_.end()) {
        std::map<RawAddress, uint16_t>::iterator it;
        for (it = transport->handleMap_.begin(); it != transport->handleMap_.end(); ++it) {
            if (it->second == handle) {
                LOG_INFO("[RfcommTransport]%{public}s erase handle map", __func__);
                transport->handleMap_.erase(it);
                break;
            }
        }
        auto transportTmp = transport->transportMap_.at(handle);
        transport->transportMap_.erase(handle);
        transport->addressMap_.erase(handle);
        transport->observer_.OnDisconnectSuccess(transportTmp);
    } else {
        LOG_ERROR("[RfcommTransport]%{public}s handle:%hu transport does not exist",
            __FUNCTION__,
            handle);
    }
}

void RfcommTransport::TransportRfcDisconnectFail(uint16_t handle, RfcommTransport *transport)
{
    LOG_INFO("[RfcommTransport]%{public}s handle:%hu ", __func__, handle);

    if (transport->IsServer()) {
        std::lock_guard<std::mutex> lock(transport->transportMutex_);
        if (transport->transportMap_.find(handle) != transport->transportMap_.end()) {
            transport->observer_.OnTransportError(transport->transportMap_.at(handle), DISCONNECT_FAIL);
        } else {
            LOG_ERROR("[RfcommTransport]%{public}s handle:%hu: RFCOMM_CHANNEL_EV_DISCONNECT_FAIL transport no exist",
                __FUNCTION__,
                handle);
        }
    } else {
        transport->observer_.OnTransportError(transport, DISCONNECT_FAIL);
    }
}

void RfcommTransport::TransportRfcFcOn(uint16_t handle, RfcommTransport *transport)
{
    LOG_INFO("[RfcommTransport]%{public}s handle:%hu ", __func__, handle);

    if (transport->IsServer()) {
        std::lock_guard<std::mutex> lock(transport->transportMutex_);
        if (transport->transportMap_.find(handle) != transport->transportMap_.end()) {
            transport->observer_.OnTransportError(transport->transportMap_.at(handle), EV_FC_ON);
        } else {
            LOG_ERROR("[RfcommTransport]%{public}s handle:%hu transport does not exist", __FUNCTION__, handle);
        }
    } else {
        transport->observer_.OnTransportError(transport, EV_FC_ON);
    }
}

void RfcommTransport::TransportRfcRecvData(uint16_t handle, RfcommTransport *transport)
{
    LOG_INFO("[RfcommTransport]%{public}s handle:%hu ", __func__, handle);

    if (transport->IsServer()) {
        std::lock_guard<std::mutex> lock(transport->transportMutex_);
        if (transport->transportMap_.find(handle) != transport->transportMap_.end()) {
            transport->observer_.OnDataAvailable(transport->transportMap_.at(handle));
        } else {
            LOG_ERROR(
                "[RfcommTransport]%{public}s handle:%hu transport does not exist", __FUNCTION__, handle);
        }
    } else {
        transport->observer_.OnDataAvailable(transport);
    }
}
}  // namespace bluetooth
}  // namespace OHOS
