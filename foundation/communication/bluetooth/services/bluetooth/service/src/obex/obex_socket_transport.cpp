/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "obex_socket_transport.h"
#include <cstring>
#include <iomanip>
#include <memory>
#include <sstream>
#include "log.h"
#include "log_util.h"
#include "obex_utils.h"
#include "packet.h"
#include "transport/transport_factory.h"

namespace OHOS {
namespace bluetooth {
ObexSocketTransport::ObexSocketTransport(
    DataTransport *dataTransport, uint16_t sendMtu, uint16_t recvMtu, const std::string &parentTranKey)
    : dataTransport_(std::unique_ptr<DataTransport>(dataTransport))
{
    // sub transport pointer need be release by this class, so we use smart pointer
    sendMtu_ = sendMtu;
    recvMtu_ = recvMtu;
    isConnected_ = true;
    remoteBtAddr_ = dataTransport_->GetRemoteAddress();
    tranKey_ = parentTranKey + "-" + remoteBtAddr_.GetAddress();
    OBEX_LOG_INFO("[%{public}s] Create SubSocketTransport,sendMtu=%{public}d,recvMtu=%{public}d",
        tranKey_.c_str(), int(sendMtu), int(recvMtu));
}

ObexSocketTransport::~ObexSocketTransport()
{
    OBEX_LOG_INFO("Delete ~ObexSocketTransport");
}

// write packet
bool ObexSocketTransport::Write(Packet &pkt)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", tranKey_.c_str(), __PRETTY_FUNCTION__);
    OBEX_LOG_DEBUG("[%{public}s] Obex Transport Write:\n%{public}s", tranKey_.c_str(), ObexUtils::ToDebugString(pkt).c_str());
    return dataTransport_->Write(&pkt) == 0;
}

// the maximum allowed OBEX packet that can be send over the transport
int ObexSocketTransport::GetMaxSendPacketSize()
{
    return sendMtu_;
}

// the maximum allowed OBEX packet that can be received over the transport
int ObexSocketTransport::GetMaxReceivePacketSize()
{
    return recvMtu_;
}

// This function is used to get the Bluetooth address of the peer of the connected channel
const RawAddress &ObexSocketTransport::GetRemoteAddress()
{
    return remoteBtAddr_;
}

// is transport Connected
bool ObexSocketTransport::IsConnected()
{
    return isConnected_;
}

// get transport key
const std::string &ObexSocketTransport::GetTransportKey()
{
    return tranKey_;
}

// ObexClientSocketTransport
ObexClientSocketTransport::ObexClientSocketTransport(
    const Option &option, ObexTransportObserver &observer, utility::Dispatcher &dispatcher)
    : ObexClientTransport(observer), dispatcher_(dispatcher),
      remoteBtAddr_(RawAddress::ConvertToString(option.addr_.addr))
{
    transportObserver_ = std::make_unique<TransportObserver>(*this);
    uint16_t mtu = option.mtu_;
    if (mtu < OBEX_MINIMUM_MTU) {
        mtu = OBEX_MINIMUM_MTU;
    }
    if (option.isGoepL2capPSM_) {
        OBEX_LOG_INFO("CreateL2capTransport at lpsm:0x%04X rpsm:0x%04X", option.lpsm_, option.scn_);
        L2capTransportInfo createInfo = {
            &remoteBtAddr_, option.lpsm_, option.scn_, mtu, *transportObserver_, dispatcher
        };
        dataTransport_ = TransportFactory::CreateL2capTransport(createInfo);
    } else {
        OBEX_LOG_INFO("CreateRfcommTransport at scn:0x%04X", option.scn_);
        dataTransport_ =
            TransportFactory::CreateRfcommTransport(&remoteBtAddr_, option.scn_, mtu, *transportObserver_, dispatcher);
    }
    tranKey_ = MakeTransportKey(option.isGoepL2capPSM_, option.scn_);
    OBEX_LOG_INFO("[%{public}s] Create ObexClientSocketTransport", tranKey_.c_str());
}

ObexClientSocketTransport::~ObexClientSocketTransport()
{}

std::string ObexClientSocketTransport::MakeTransportKey(bool isGoepL2capPSM, uint16_t scn)
{
    std::stringstream keyStream;
    keyStream << "CLIENT";
    if (isGoepL2capPSM) {
        keyStream << "-L2CAP-";
    } else {
        keyStream << "-RFCOMM-";
    }
    keyStream << std::uppercase << std::hex << std::setfill('0') << std::setw(TRANSPORT_KEY_NUM_LEN) << scn;
    keyStream << "-" << PrivateGetRemoteAddress().GetAddress();
    return keyStream.str();
}

int ObexClientSocketTransport::Connect()
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", tranKey_.c_str(), __PRETTY_FUNCTION__);
    return dataTransport_->Connect();
}

int ObexClientSocketTransport::Disconnect()
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", tranKey_.c_str(), __PRETTY_FUNCTION__);
    return dataTransport_->Disconnect();
}

bool ObexClientSocketTransport::Write(Packet &pkt)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", tranKey_.c_str(), __PRETTY_FUNCTION__);
    OBEX_LOG_DEBUG("[%{public}s] Obex Transport Write:\n%{public}s", tranKey_.c_str(), ObexUtils::ToDebugString(pkt).c_str());
    return dataTransport_->Write(&pkt) == 0;
}

// the maximum allowed OBEX packet that can be send over the transport
int ObexClientSocketTransport::GetMaxSendPacketSize()
{
    return sendMtu_;
}

// the maximum allowed OBEX packet that can be received over the transport
int ObexClientSocketTransport::GetMaxReceivePacketSize()
{
    return recvMtu_;
}

// This function is used to get the Bluetooth address of the peer of the connected channel
const RawAddress &ObexClientSocketTransport::GetRemoteAddress()
{
    return PrivateGetRemoteAddress();
}

const RawAddress &ObexClientSocketTransport::PrivateGetRemoteAddress()
{
    return remoteBtAddr_;
}

// is transport Connected
bool ObexClientSocketTransport::IsConnected()
{
    return isConnected_;
}

// get transport key
const std::string &ObexClientSocketTransport::GetTransportKey()
{
    return tranKey_;
}

ObexClientSocketTransport::TransportObserver::TransportObserver(ObexClientSocketTransport &obexTran)
    : obexTran_(obexTran)
{}

// connection success
void ObexClientSocketTransport::TransportObserver::OnConnected(
    DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.dispatcher_.PostTask(std::bind(
        &ObexClientSocketTransport::TransportObserver::ProcessOnConnected, this, transport, sendMTU, recvMTU));
}

// The event is triggered when a disconnect request is received.
void ObexClientSocketTransport::TransportObserver::OnDisconnected(DataTransport *transport)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.dispatcher_.PostTask(
        std::bind(&ObexClientSocketTransport::TransportObserver::ProcessOnDisconnected, this));
}

// The event is triggered when the disconnection process is successful.
void ObexClientSocketTransport::TransportObserver::OnDisconnectSuccess(DataTransport *transport)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.dispatcher_.PostTask(
        std::bind(&ObexClientSocketTransport::TransportObserver::ProcessOnDisconnectSuccess, this));
}

// when data is received from RFCOMM
void ObexClientSocketTransport::TransportObserver::OnDataAvailable(DataTransport *transport)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.dispatcher_.PostTask(
        std::bind(&ObexClientSocketTransport::TransportObserver::ProcessOnDataAvailable, this, transport, nullptr));
}

// when data is received from L2CAP
void ObexClientSocketTransport::TransportObserver::OnDataAvailable(DataTransport *transport, Packet *pkt)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.dispatcher_.PostTask(
        std::bind(&ObexClientSocketTransport::TransportObserver::ProcessOnDataAvailable, this, transport, pkt));
}

// The event is triggered when peer or RFCOMM/L2CAP is not available to receive data.
void ObexClientSocketTransport::TransportObserver::OnDataBusy(DataTransport *transport, uint8_t isBusy)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.dispatcher_.PostTask(
        std::bind(&ObexClientSocketTransport::TransportObserver::ProcessOnDataBusy, this, isBusy));
}

// when peer is not available to receive data, or RFCOMM's send queue is full
void ObexClientSocketTransport::TransportObserver::OnTransportError(DataTransport *transport, int errType)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.dispatcher_.PostTask(
        std::bind(&ObexClientSocketTransport::TransportObserver::ProcessOnTransportError, this, errType));
}

// The event is triggered when connection complete successfully.
void ObexClientSocketTransport::TransportObserver::ProcessOnConnected(
    DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.isConnected_ = true;
    obexTran_.sendMtu_ = sendMTU;
    obexTran_.recvMtu_ = recvMTU;
    obexTran_.remoteBtAddr_ = transport->GetRemoteAddress();
    OBEX_LOG_DEBUG(
        "[%{public}s] ObexClientSocketTransport Connect sendMtu=%{public}d,recvMtu=%{public}d", obexTran_.tranKey_.c_str(), sendMTU, recvMTU);
    obexTran_.observer_.OnTransportConnected(obexTran_);
}

// The event is triggered when a disconnect request is received.
void ObexClientSocketTransport::TransportObserver::ProcessOnDisconnected()
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.isConnected_ = false;
    obexTran_.observer_.OnTransportDisconnected(obexTran_);
}

// The event is triggered when the disconnection process is successful.
void ObexClientSocketTransport::TransportObserver::ProcessOnDisconnectSuccess()
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    ProcessOnDisconnected();
}

void ObexClientSocketTransport::TransportObserver::ProcessOnDataBusy(uint8_t isBusy)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.observer_.OnTransportDataBusy(obexTran_, isBusy);
}

// The event is triggered when data is received from stack.
void ObexClientSocketTransport::TransportObserver::ProcessOnDataAvailable(DataTransport *transport, Packet *pkt)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    Packet *packetForRead = nullptr;
    if (pkt != nullptr) {
        packetForRead = pkt;
    } else {
        if (transport->Read(&packetForRead) != 0) {
            OBEX_LOG_ERROR("error in transport.Read");
            PacketFree(packetForRead);
            return;
        }
        if (packetForRead == nullptr) {
            OBEX_LOG_ERROR("obexPacket can't be null!");
            return;
        }
    }
    // pkt is delete by ObexPacket
    OBEX_LOG_DEBUG("Obex Transport Read:\n%{public}s", ObexUtils::ToDebugString(*packetForRead).c_str());
    auto obexPacket = std::make_unique<ObexPacket>(*packetForRead);
    obexTran_.observer_.OnTransportDataAvailable(obexTran_, *obexPacket);
}

// The event is triggered when process is failed.
void ObexClientSocketTransport::TransportObserver::ProcessOnTransportError(int errType)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", obexTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    obexTran_.observer_.OnTransportError(obexTran_, errType);
}

// ObexServerIncomingConnect
ObexServerIncomingConnect::ObexServerIncomingConnect(
    ObexServerSocketTransport &obexTransport, const RawAddress &btAddr, int port)
    : obexTransport_(obexTransport), btAddr_(btAddr), port_(port), operDone_(false)
{
    HILOGI("Create ObexServerIncomingConnect: %{public}s", GetEncryptAddr(btAddr_.GetAddress()).c_str());
}

ObexServerIncomingConnect::~ObexServerIncomingConnect()
{
    OBEX_LOG_INFO("Delete ~ObexServerIncomingConnect");
}

//  accept the connection request
int ObexServerIncomingConnect::AcceptConnection()
{
    HILOGI("addr: %{public}s", GetEncryptAddr(btAddr_.GetAddress()).c_str());
    if (operDone_) {
        OBEX_LOG_ERROR("Operation is done!");
        return -1;
    }
    operDone_ = true;
    return obexTransport_.AcceptConnection(*this);
}

//  reject the connection request
int ObexServerIncomingConnect::RejectConnection()
{
    HILOGI("addr: %{public}s", GetEncryptAddr(btAddr_.GetAddress()).c_str());
    if (operDone_) {
        OBEX_LOG_ERROR("Operation is done!");
        return -1;
    }
    operDone_ = true;
    return obexTransport_.RejectConnection(*this);
}

const RawAddress &ObexServerIncomingConnect::GetRemoteAddress()
{
    return btAddr_;
}

int ObexServerIncomingConnect::GetPort()
{
    return port_;
}

ObexServerSocketTransport::ObexServerSocketTransport(
    const Option &option, ObexTransportObserver &observer, utility::Dispatcher &dispatcher)
    : ObexServerTransport(observer), dispatcher_(dispatcher)
{
    transportObserver_ = std::make_unique<TransportObserver>(*this);
    uint16_t mtu = option.mtu_;
    if (mtu < OBEX_MINIMUM_MTU) {
        mtu = OBEX_MINIMUM_MTU;
    }
    if (option.isGoepL2capPSM_) {
        OBEX_LOG_INFO("ObexServerSocketTransport:CreateL2capTransport at psm:0x%04X", option.scn_);
        L2capTransportInfo createInfo = {nullptr, option.scn_, uint16_t(0), mtu, *transportObserver_, dispatcher};
        dataTransport_ = TransportFactory::CreateL2capTransport(createInfo);
    } else {
        OBEX_LOG_INFO("ObexServerSocketTransport:CreateRfcommTransport at scn:0x%04X", option.scn_);
        dataTransport_ =
            TransportFactory::CreateRfcommTransport(nullptr, option.scn_, mtu, *transportObserver_, dispatcher);
    }
    tranKey_ = MakeTransportKey(option.isGoepL2capPSM_, option.scn_);
    OBEX_LOG_INFO("[%{public}s] Create ObexServerSocketTransport", tranKey_.c_str());
}

std::string ObexServerSocketTransport::MakeTransportKey(bool isGoepL2capPSM, uint16_t scn)
{
    std::stringstream keyStream;
    keyStream << "SERVER";
    if (isGoepL2capPSM) {
        keyStream << "-L2CAP-";
    } else {
        keyStream << "-RFCOMM-";
    }
    keyStream << std::uppercase << std::hex << std::setfill('0') << std::setw(TRANSPORT_KEY_NUM_LEN) << scn;
    return keyStream.str();
}

int ObexServerSocketTransport::Listen()
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", tranKey_.c_str(), __PRETTY_FUNCTION__);
    isOnListening_ = true;
    return dataTransport_->RegisterServer();
}

int ObexServerSocketTransport::Disconnect()
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", tranKey_.c_str(), __PRETTY_FUNCTION__);
    isOnListening_ = false;
    for (auto &it : incomingConnectMap_) {
        HILOGI("%{public}s: incomingConnect %{public}s hasn't been disconnected.", tranKey_.c_str(), it.first.c_str());
    }
    for (auto &it : subTranMap_) {
        HILOGI("[%{public}s] subStransport %{public}s hasn't been disconnected.",
            tranKey_.c_str(), GetEncryptAddr(it.second->GetRemoteAddress().GetAddress()).c_str());
    }
    return dataTransport_->RemoveServer(true);
}

int ObexServerSocketTransport::Disconnect(ObexTransport &subTransport)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", tranKey_.c_str(), __PRETTY_FUNCTION__);
    DataTransport *findTransport = nullptr;
    for (auto it = subTranMap_.begin(); it != subTranMap_.end(); ++it) {
        if (it->second.get() == &subTransport) {
            findTransport = it->first;
            break;
        }
    }
    if (findTransport != nullptr) {
        OBEX_LOG_DEBUG("[%{public}s] To Disconnect sub transport", tranKey_.c_str());
        return findTransport->Disconnect();
    }
    return -1;
}

// Server accept the connection request
int ObexServerSocketTransport::AcceptConnection(ObexIncomingConnect &incomingConnect)
{
    const RawAddress &addr = incomingConnect.GetRemoteAddress();
    HILOGI("Call%{public}s , addr: %{public}s", tranKey_.c_str(), GetEncryptAddr(addr.GetAddress()).c_str());
    return dataTransport_->AcceptConnection(addr, incomingConnect.GetPort());
}

// Server reject the connection request
int ObexServerSocketTransport::RejectConnection(ObexIncomingConnect &incomingConnect)
{
    const RawAddress &addr = incomingConnect.GetRemoteAddress();
    HILOGI("Call %{public}s , addr: %{public}s", tranKey_.c_str(), GetEncryptAddr(addr.GetAddress()).c_str());
    int ret = dataTransport_->RejectConnection(addr, incomingConnect.GetPort());
    if (incomingConnectMap_.find(addr.GetAddress()) != incomingConnectMap_.end()) {
        HILOGI("[%{public}s] Remove incomingConnect %{public}s.", tranKey_.c_str(),
            GetEncryptAddr(addr.GetAddress()).c_str());
        incomingConnectMap_.erase(addr.GetAddress());
    }
    return ret;
}

// get transport key
const std::string &ObexServerSocketTransport::GetTransportKey()
{
    return tranKey_;
}

ObexServerSocketTransport::TransportObserver::TransportObserver(ObexServerSocketTransport &mainTran)
    : mainTran_(mainTran)
{}

// connection success
void ObexServerSocketTransport::TransportObserver::OnConnectIncoming(const RawAddress &addr, uint16_t port)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    mainTran_.dispatcher_.PostTask(
        std::bind(&ObexServerSocketTransport::TransportObserver::ProcessOnConnectIncoming, this, addr, port));
}

void ObexServerSocketTransport::TransportObserver::OnIncomingDisconnected(const RawAddress &addr)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    mainTran_.dispatcher_.PostTask(
        std::bind(&ObexServerSocketTransport::TransportObserver::ProcessOnIncomingDisconnected, this, addr));
}

// connection success
void ObexServerSocketTransport::TransportObserver::OnConnected(
    DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    mainTran_.dispatcher_.PostTask(std::bind(
        &ObexServerSocketTransport::TransportObserver::ProcessOnConnected, this, transport, sendMTU, recvMTU));
}

// connection failed
void ObexServerSocketTransport::TransportObserver::OnDisconnected(DataTransport *transport)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    mainTran_.dispatcher_.PostTask(
        std::bind(&ObexServerSocketTransport::TransportObserver::ProcessOnDisconnected, this, transport));
}

void ObexServerSocketTransport::TransportObserver::OnDisconnectSuccess(DataTransport *transport)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    mainTran_.dispatcher_.PostTask(
        std::bind(&ObexServerSocketTransport::TransportObserver::ProcessOnDisconnectSuccess, this, transport));
}

// when data is received from RFCOMM
void ObexServerSocketTransport::TransportObserver::OnDataAvailable(DataTransport *transport)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    mainTran_.dispatcher_.PostTask(
        std::bind(&ObexServerSocketTransport::TransportObserver::ProcessOnDataAvailable, this, transport, nullptr));
}

// when data is received from L2CAP
void ObexServerSocketTransport::TransportObserver::OnDataAvailable(DataTransport *transport, Packet *pkt)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    mainTran_.dispatcher_.PostTask(
        std::bind(&ObexServerSocketTransport::TransportObserver::ProcessOnDataAvailable, this, transport, pkt));
}

// when peer is not available to receive data, or RFCOMM's send queue is full
void ObexServerSocketTransport::TransportObserver::OnTransportError(DataTransport *transport, int errType)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    mainTran_.dispatcher_.PostTask(
        std::bind(&ObexServerSocketTransport::TransportObserver::ProcessOnTransportError, this, transport, errType));
}

// The event is triggered when peer or RFCOMM/L2CAP is not available to receive data.
void ObexServerSocketTransport::TransportObserver::OnDataBusy(DataTransport *transport, uint8_t isBusy)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    mainTran_.dispatcher_.PostTask(
        std::bind(&ObexServerSocketTransport::TransportObserver::ProcessOnDataBusy, this, transport, isBusy));
}

// The event is triggered when server accept a new connection.
void ObexServerSocketTransport::TransportObserver::ProcessOnConnectIncoming(RawAddress btAddr, uint16_t port)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    std::string tranKey = mainTran_.tranKey_;
    const std::string &strAddr = btAddr.GetAddress();
    OBEX_LOG_INFO("[%{public}s] BtAddr:%{public}s : 0x%04X", tranKey.c_str(), strAddr.c_str(), port);
    if (!mainTran_.isOnListening_) {
        OBEX_LOG_ERROR("[%{public}s] Server has been stopped! Reject incoming %{public}s .", tranKey.c_str(), strAddr.c_str());
        mainTran_.dataTransport_->RejectConnection(btAddr, port);
        return;
    }
    if (mainTran_.incomingConnectMap_.find(strAddr) != mainTran_.incomingConnectMap_.end()) {
        HILOGI("[%{public}s] Incoming connect is already exists, So Reject: %{public}s", tranKey.c_str(),
            GetEncryptAddr(btAddr.GetAddress()).c_str());
        mainTran_.dataTransport_->RejectConnection(btAddr, port);
        return;
    }
    OBEX_LOG_DEBUG("[%{public}s] Create incoming connect :%{public}s, %{public}d", tranKey.c_str(), strAddr.c_str(), port);
    mainTran_.incomingConnectMap_[strAddr] = std::make_unique<ObexServerIncomingConnect>(mainTran_, btAddr, port);
    mainTran_.observer_.OnTransportConnectIncoming(*mainTran_.incomingConnectMap_.at(strAddr).get());
}

// The event is triggered when new connection disconnect before accept.
void ObexServerSocketTransport::TransportObserver::ProcessOnIncomingDisconnected(RawAddress btAddr)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    std::string strAddr = btAddr.GetAddress();
    OBEX_LOG_INFO("[%{public}s] BtAddr:%{public}s", mainTran_.tranKey_.c_str(), strAddr.c_str());
    if (mainTran_.incomingConnectMap_.find(strAddr) != mainTran_.incomingConnectMap_.end()) {
        mainTran_.incomingConnectMap_.erase(strAddr);
        mainTran_.observer_.OnTransportIncomingDisconnected(strAddr);
    }
}

// The event is triggered when connection complete successfully.
void ObexServerSocketTransport::TransportObserver::ProcessOnConnected(
    DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    OBEX_LOG_DEBUG("[%{public}s] transport", mainTran_.tranKey_.c_str());
    int i = 0;
    for (auto &it : mainTran_.subTranMap_) {
        std::string addr = it.second->GetRemoteAddress().GetAddress();
        HILOGI("%{public}s subStransport: %{public}d, addr: %{public}s",
            mainTran_.tranKey_.c_str(), i, GetEncryptAddr(addr).c_str());
        i++;
    }
    std::string strAddr = transport->GetRemoteAddress().GetAddress();
    mainTran_.incomingConnectMap_.erase(strAddr);
    auto subTransport = std::make_unique<ObexSocketTransport>(transport, sendMTU, recvMTU, mainTran_.GetTransportKey());
    ObexTransport *subTran = subTransport.get();
    OBEX_LOG_INFO("[%{public}s] Add sub transport  %{public}s", mainTran_.tranKey_.c_str(), subTransport->GetTransportKey().c_str());
    mainTran_.subTranMap_[transport] = std::move(subTransport);

    mainTran_.observer_.OnTransportConnected(*subTran);
}

// The event is triggered when a disconnect request is received.
void ObexServerSocketTransport::TransportObserver::ProcessOnDisconnected(DataTransport *transport)
{
    std::string tranKey = mainTran_.tranKey_;
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", tranKey.c_str(), __PRETTY_FUNCTION__);
    OBEX_LOG_DEBUG("[%{public}s] OnDisconnected", tranKey.c_str());

    uint32_t i = 0;
    for (auto &it : mainTran_.subTranMap_) {
        std::string addr = it.second->GetRemoteAddress().GetAddress();
        HILOGI("%{public}s subStransport: %{public}d address: %{public}s", tranKey.c_str(), i, GetEncryptAddr(addr).c_str());
        i++;
    }
    if (mainTran_.subTranMap_.find(transport) != mainTran_.subTranMap_.end()) {
        ObexSocketTransport *subTran = mainTran_.subTranMap_.at(transport).get();
        mainTran_.observer_.OnTransportDisconnected(*subTran);
        OBEX_LOG_INFO("[%{public}s] Remove sub transport %{public}s", tranKey.c_str(), subTran->GetTransportKey().c_str());
        mainTran_.subTranMap_.erase(transport);
    } else {
        OBEX_LOG_ERROR("[%{public}s] DataTransport isn't exists!", tranKey.c_str());
    }
}

// The event is triggered when the disconnection process is successful.
void ObexServerSocketTransport::TransportObserver::ProcessOnDisconnectSuccess(DataTransport *transport)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    ProcessOnDisconnected(transport);
}

// The event is triggered when data is received from stack.
void ObexServerSocketTransport::TransportObserver::ProcessOnDataAvailable(DataTransport *transport, Packet *pkt)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    uint32_t i = 0;
    for (auto &it : mainTran_.subTranMap_) {
        std::string addr = it.second->GetRemoteAddress().GetAddress();
        HILOGI("%{public}s subStransport: %{public}d, addr: %{public}s",
            mainTran_.tranKey_.c_str(), i, GetEncryptAddr(addr).c_str());
        i++;
    }
    if (mainTran_.subTranMap_.find(transport) != mainTran_.subTranMap_.end()) {
        Packet *packetForRead = nullptr;
        if (pkt != nullptr) {
            packetForRead = pkt;
        } else {
            if (transport->Read(&packetForRead) != 0) {
                OBEX_LOG_ERROR("[%{public}s] error in transport.Read", mainTran_.tranKey_.c_str());
                PacketFree(packetForRead);
                return;
            }
            if (packetForRead == nullptr) {
                OBEX_LOG_ERROR("[%{public}s] obexPacket can't be null!", mainTran_.tranKey_.c_str());
                return;
            }
        }
        // packetForRead is delete by ObexPacket
        OBEX_LOG_DEBUG("[%{public}s] Obex Transport Read:\n%{public}s",
            mainTran_.tranKey_.c_str(),
            ObexUtils::ToDebugString(*packetForRead).c_str());
        auto obexPacket = std::make_unique<ObexPacket>(*packetForRead);
        ObexSocketTransport *subTran = mainTran_.subTranMap_.at(transport).get();
        OBEX_LOG_INFO(
            "[%{public}s] Receive data on sub transport %{public}s", mainTran_.tranKey_.c_str(), subTran->GetTransportKey().c_str());
        mainTran_.observer_.OnTransportDataAvailable(*subTran, *obexPacket);
    } else {
        OBEX_LOG_ERROR(
            "[%{public}s] Receive data on sub transport, but it isn't exists in subTransportMap!", mainTran_.tranKey_.c_str());
        if (pkt != nullptr) {
            PacketFree(pkt);
        }
    }
}

void ObexServerSocketTransport::TransportObserver::ProcessOnDataBusy(DataTransport *transport, uint8_t isBusy)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s, isBusy %{public}d", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__, int(isBusy));
    if (transport != nullptr && mainTran_.subTranMap_.find(transport) != mainTran_.subTranMap_.end()) {
        ObexSocketTransport *subTran = mainTran_.subTranMap_.at(transport).get();
        mainTran_.observer_.OnTransportDataBusy(*subTran, isBusy);
    }
}

// The event is triggered when process is failed.
void ObexServerSocketTransport::TransportObserver::ProcessOnTransportError(DataTransport *transport, int errType)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", mainTran_.tranKey_.c_str(), __PRETTY_FUNCTION__);
    if (transport != nullptr && mainTran_.subTranMap_.find(transport) != mainTran_.subTranMap_.end()) {
        ObexSocketTransport *subTran = mainTran_.subTranMap_.at(transport).get();
        OBEX_LOG_ERROR("[%{public}s] Error on sub transport %{public}s, errorCode=%{public}d",
            mainTran_.tranKey_.c_str(),
            subTran->GetTransportKey().c_str(),
            errType);
        mainTran_.observer_.OnTransportError(*subTran, errType);
    } else {
        OBEX_LOG_ERROR("[%{public}s] Error occur in server socket transport? error code:%{public}d",
            mainTran_.tranKey_.c_str(), errType);
    }
}
}  // namespace bluetooth
}  // namespace OHOS
