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

#include "pan_bnep.h"
#include <endian.h>
#include "pan_service.h"

namespace OHOS {
namespace bluetooth {
int PanBnep::Startup()
{
    GapServiceSecurityInfo securityInfo = {INCOMING, PAN_BNEP, SEC_PROTOCOL_L2CAP,
        { .l2capPsm = BT_BNEP_PSM }};
    if (GAPIF_RegisterServiceSecurity(nullptr, &securityInfo,
        GAP_SEC_IN_ENCRYPTION | GAP_SEC_IN_AUTHENTICATION |
        GAP_SEC_OUT_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION)) {
        LOG_ERROR("[PAN BNEP] %{public}s:GAPIF_RegisterServiceSecurity INCOMING failed.", __func__);
        return RET_BAD_STATUS;
    }

    securityInfo = {OUTGOING, PAN_BNEP, SEC_PROTOCOL_L2CAP,
        { .l2capPsm = BT_BNEP_PSM }};
    if (GAPIF_RegisterServiceSecurity(nullptr, &securityInfo,
        GAP_SEC_IN_ENCRYPTION | GAP_SEC_IN_AUTHENTICATION |
        GAP_SEC_OUT_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION)) {
            LOG_ERROR("[PAN BNEP] %{public}s:GAPIF_RegisterServiceSecurity OUTGOING failed.", __func__);
        return RET_BAD_STATUS;
    }

    /* regist pan bnep psm */
    if (L2CIF_RegisterService(BT_BNEP_PSM, (L2capService *)&BNEP_CALLBACK, nullptr, nullptr)) {
        LOG_ERROR("[PAN BNEP] %{public}s:L2CIF_RegisterService PAN_PSM failed.", __func__);
        return RET_BAD_STATUS;
    }
    return BT_NO_ERROR;
}

void PanBnep::Shutdown()
{
    L2CIF_DeregisterService(BT_BNEP_PSM, nullptr);
    GapServiceSecurityInfo securityInfo = {INCOMING, PAN_BNEP, SEC_PROTOCOL_L2CAP,
        { .l2capPsm = BT_BNEP_PSM }};
    GAPIF_DeregisterServiceSecurity(nullptr, &securityInfo);
    securityInfo = {OUTGOING, PAN_BNEP, SEC_PROTOCOL_L2CAP,
        { .l2capPsm = BT_BNEP_PSM }};
    GAPIF_DeregisterServiceSecurity(nullptr, &securityInfo);
}

PanBnep::PanBnep(std::string address)
{
    address_ = address;
    state_ = BNEP_CONN_STATE_UNUSED;
    lcid_ = 0;
    connFlags_ = 0;
    id_ = 0;
    isBusy_ = false;
}

PanBnep::~PanBnep()
{
    while (!waitingSendDataList_.empty()) {
        Packet *packet = waitingSendDataList_.front();
        waitingSendDataList_.pop();
        PacketFree(packet);
    }
}

int PanBnep::Connect()
{
    LOG_INFO("[PAN BNEP]%{public}s", __func__);
    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    if (state_ == BNEP_CONN_STATE_CONNECTED) {
        PanMessage event(PAN_INT_OPEN_EVT);
        event.dev_ = address_;
        PanService::GetService()->PostEvent(event);
        return BT_NO_ERROR;
    }

    if (state_ != BNEP_CONN_STATE_UNUSED) {
        LOG_ERROR("[PAN BNEP] %{public}s:state error state = %{public}d", __func__, state_);
        return RET_BAD_STATUS;
    }

    return SendGapRequestSecurity(false, 0, 0);
}

/**
 *
 * @brief         PanSecurityCheckCallback
 *
 * @details       Host receive the security check result
 *
 * @return        uint16_t
 *
 */
void PanBnep::BnepSecurityCheckCallback(
    uint16_t result, GapServiceSecurityInfo security, void *context)
{
    LOG_INFO("[PAN BNEP]%{public}s:result(%{public}hu), l2capPsm(0x%{public}x)",
        __func__, result, security.channelId.l2capPsm);
    std::string *address = (std::string *)context;
    if (address != nullptr) {
        PanMessage event(BNEP_L2CAP_SECURITY_RESULT_EVT, result);
        event.dev_ = *address;
        PanService::GetService()->PostEvent(event);
    } else {
        LOG_ERROR("[PAN BNEP]%{public}s PanL2capConnection is null.", __func__);
    }
}

void PanBnep::BnepSecurityCheckTask(uint16_t result)
{
    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    connFlags_ = BNEP_CONN_FLAGS_IS_ORIG;

    if (result == BT_NO_ERROR) {
        LOG_INFO("[PAN BNEP]%{public}s start connect", __func__);
        int ret = L2CIF_ConnectReq(&btAddr, BT_BNEP_PSM, BT_BNEP_PSM,
            this, BnepL2cConnectReqCallback);
        if (ret != BT_NO_ERROR) {
            LOG_ERROR("[PAN BNEP]%{public}s L2CIF_ConnectReq return value is error.", __func__);
            state_ = BNEP_CONN_STATE_UNUSED;

            PanMessage event(PAN_INT_CLOSE_EVT);
            event.dev_ = address_;
            PanService::GetService()->PostEvent(event);
            return;
        }
        state_ = BNEP_CONN_STATE_CONNECTING;
    } else {
        LOG_ERROR("[PAN BNEP]%{public}s Request Security faild!", __func__);
        state_ = BNEP_CONN_STATE_UNUSED;

        PanMessage event(PAN_INT_CLOSE_EVT);
        event.dev_ = address_;
        PanService::GetService()->PostEvent(event);
    }
}

/**
 *
 * @brief         PanIndSecurityCheckCallback
 *
 * @details       Host receive the security check result，when receive device's connect request.
 *
 * @return        uint16_t
 *
 */
void PanBnep::BnepIndSecurityCheckDeviceConnectCallback(uint16_t result,
    GapServiceSecurityInfo security, void *context)
{
    LOG_INFO("[PAN BNEP]%{public}s:result(%{public}hu), l2capPsm(0x%{public}x)",
        __func__, result, security.channelId.l2capPsm);
    std::string *address = (std::string *)context;
    if (address != nullptr) {
        PanMessage event(BNEP_L2CAP_IND_SECURITY_RESULT_EVT, result);
        event.dev_ = *address;
        PanService::GetService()->PostEvent(event);
    } else {
        LOG_ERROR("[PAN BNEP]%{public}s PanL2capConnection is null.", __func__);
    }
}

void PanBnep::BnepSecurityCheckDeviceConnectTask(uint16_t result)
{
    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    if ((result == BT_NO_ERROR) && (state_ == BNEP_CONN_STATE_SECURITY)) {
        LOG_INFO("[PAN BNEP]%{public}s start send connect response", __func__);
        state_ = BNEP_CONN_STATE_CONNECTING;
        L2CIF_ConnectRsp(lcid_, id_, L2CAP_CONNECTION_SUCCESSFUL, L2CAP_NO_FURTHER_INFORMATION_AVAILABLE, nullptr);

        L2capConfigInfo l2capConfigInfo;
        (void)memset_s(&l2capConfigInfo, sizeof(L2capConfigInfo), 0x00, sizeof(L2capConfigInfo));
        l2capConfigInfo.mtu = BNEP_MTU_SIZE;
        l2capConfigInfo.flushTimeout = 0xFFFF;
        if (L2CIF_ConfigReq(lcid_, &l2capConfigInfo, nullptr)) {
            LOG_ERROR("[PAN BNEP] %{public}s:L2CIF_ConfigReq failed.", __func__);
        }
    } else if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN BNEP]%{public}s Request Security faild!", __func__);
        state_ = BNEP_CONN_STATE_UNUSED;

        PanMessage event(PAN_INT_CLOSE_EVT);
        event.dev_ = address_;
        PanService::GetService()->PostEvent(event);
    }
}

int PanBnep::Disconnect()
{
    LOG_INFO("[PAN BNEP]%{public}s", __func__);

    if (state_ == BNEP_CONN_STATE_UNUSED) {
        PanMessage event(PAN_INT_CLOSE_EVT);
        event.dev_ = address_;
        PanService::GetService()->PostEvent(event);
        return BT_NO_ERROR;
    }

    if (lcid_ != 0) {
        state_ = BNEP_CONN_STATE_DISCONNECTING;
        L2CIF_DisconnectionReq(lcid_, nullptr);
    } else {
        state_ = BNEP_CONN_STATE_UNUSED;
        PanMessage event(PAN_INT_CLOSE_EVT);
        event.dev_ = address_;
        PanService::GetService()->PostEvent(event);
        return BT_NO_ERROR;
    }

    return BT_NO_ERROR;
}

int PanBnep::SendData(EthernetHeader ethernetHeader, uint8_t* pkt, int length)
{
    if (!CheckBnepEthernetDataFilter(ethernetHeader, pkt, length)) {
        return BT_NO_ERROR;
    }
    uint8_t bluetoothDestAddr[BT_ADDRESS_LENGTH];
    uint8_t bluetoothSrcAddr[BT_ADDRESS_LENGTH];
    PanService::ReverseAddress(ethernetHeader.destAddr, bluetoothDestAddr);
    PanService::ReverseAddress(ethernetHeader.srcAddr, bluetoothSrcAddr);
    std::string destAddr = RawAddress::ConvertToString(bluetoothDestAddr).GetAddress();
    BtAddr localAddr;
    RawAddress(PanService::GetLocalAddress()).ConvertToUint8(localAddr.addr);
    Packet *packet = nullptr;
    uint8_t *buf = nullptr;
    int offset = 0;
    int ret;
    uint8_t type = BNEP_COMPRESSED_ETHERNET;
    int packetLength = length;
    if (localAddr.addr[BT_ADDRESS_LENGTH - 1] & 0x01) {
        LOG_DEBUG("[PAN BNEP]%{public}s():use unicast MAC address", __FUNCTION__);
        localAddr.addr[BT_ADDRESS_LENGTH - 1] &= ~0x01;
    }
    if (memcmp(bluetoothSrcAddr, localAddr.addr, BT_ADDRESS_LENGTH) == 0) {
        if (destAddr == address_) {
            type = BNEP_COMPRESSED_ETHERNET;
            packetLength += BNEP_COMPRESSED_ETHERNET_HEAD_LENGTH;
        } else {
            type = BNEP_COMPRESSED_ETHERNET_DEST_ONLY;
            packetLength += BNEP_COMPRESSED_ETHERNET_DEST_ONLY_HEAD_LENGTH;
        }
    } else {
        if (destAddr == address_) {
            type = BNEP_COMPRESSED_ETHERNET_SRC_ONLY;
            packetLength += BNEP_COMPRESSED_ETHERNET_SRC_ONLY_HEAD_LENGTH;
        } else {
            type = BNEP_GENERAL_ETHERNET;
            packetLength += BNEP_GENERAL_ETHERNET_HEAD_LENGTH;
        }
    }
    packet = PacketMalloc(packetLength, 0, 0);
    buf = (uint8_t *)BufferPtr(PacketHead(packet));
    ret = BnepBuildEthernetPacketHeader(type, ethernetHeader, buf);
    if (ret <= 0) {
        return PAN_FAILURE;
    }
    offset += ret;
    if ((length > 0) && (pkt != nullptr)) {
        if (memcpy_s(buf + offset, length, pkt, length) != EOK) {
            LOG_ERROR("[PAN BNEP]%{public}s(): memcpy source Address error", __FUNCTION__);
            return PAN_FAILURE;
        }
    }
    AddPacketToWaitingSendDataList(packet);
    return BT_NO_ERROR;
}

void PanBnep::AddPacketToWaitingSendDataList(Packet *addPacket)
{
    if (waitingSendDataList_.size() < BNEP_MAX_WAITING_SEND_DATA_LIST_NUMBER) {
        waitingSendDataList_.push(addPacket);
    } else {
        LOG_ERROR("[PAN BNEP]%{public}s(): waiting send data list is full!", __FUNCTION__);
        PacketFree(addPacket);
    }
    while (!isBusy_ && !waitingSendDataList_.empty()) {
        Packet *packet = waitingSendDataList_.front();
        waitingSendDataList_.pop();
        L2CIF_SendData(lcid_, packet, nullptr);
        PacketFree(packet);
    }
}

bool PanBnep::CheckBnepEthernetDataFilter(EthernetHeader ethernetHeader, uint8_t* pkt, int length)
{
    if (!protocolFilterList_.empty()) {
        std::vector<BnepProtocolFilterRange>::iterator it;
        for (protocolFilterList_.begin(); it != protocolFilterList_.end(); it++) {
            if ((ethernetHeader.protocol >= it->start) && (ethernetHeader.protocol <= it->end)) {
                break;
            }
        }
        if (it == protocolFilterList_.end()) {
            return false;
        }
    }
    // Just check broadcast
    if ((ethernetHeader.destAddr[0] & 0x01) && !multycastFilterList_.empty()) {
        std::vector<BnepMultycastFilterRange>::iterator it;
        for (multycastFilterList_.begin(); it != multycastFilterList_.end(); it++) {
            if ((memcmp(ethernetHeader.destAddr, it->start, BT_ADDRESS_LENGTH) >= 0) &&
                (memcmp(ethernetHeader.destAddr, it->end, BT_ADDRESS_LENGTH) <= 0)) {
                break;
            }
        }
        if (it == multycastFilterList_.end()) {
            return false;
        }
    }
    return true;
}

int PanBnep::BnepBuildEthernetPacketHeader(uint8_t type, EthernetHeader ethernetHeader, uint8_t *buf)
{
    if (buf == nullptr) {
        LOG_ERROR("[PAN BNEP]%{public}s(): buf is null", __FUNCTION__);
        return 0;
    }
    int offset = 0;
    buf[offset] = type;
    offset++;
    if ((type == BNEP_COMPRESSED_ETHERNET_DEST_ONLY) || (type == BNEP_GENERAL_ETHERNET)) {
        if (memcpy_s(buf + offset, BT_ADDRESS_LENGTH, ethernetHeader.destAddr,
            BT_ADDRESS_LENGTH) != EOK) {
            LOG_ERROR("[PAN BNEP]%{public}s(): memcpy destination Address error", __FUNCTION__);
            return 0;
        }
        offset += BT_ADDRESS_LENGTH;
    }
    if ((type == BNEP_COMPRESSED_ETHERNET_SRC_ONLY) || (type == BNEP_GENERAL_ETHERNET)) {
        if (memcpy_s(buf + offset, BT_ADDRESS_LENGTH, ethernetHeader.srcAddr,
            BT_ADDRESS_LENGTH) != EOK) {
            LOG_ERROR("[PAN BNEP]%{public}s(): memcpy source Address error", __FUNCTION__);
            return 0;
        }
        offset += BT_ADDRESS_LENGTH;
    }

    *(reinterpret_cast<uint16_t *>(buf + offset)) = htobe16(ethernetHeader.protocol);
    offset += BNEP_UINT16_SIZE;
    return offset;
}

uint16_t PanBnep::GetLcid()
{
    return lcid_;
}

void PanBnep::BnepL2cConnectReqCallback(
    const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);
    PanBnep* l2capConnect = (PanBnep *)context;
    if ((l2capConnect != nullptr) && (result == BT_NO_ERROR)) {
        l2capConnect->lcid_ = lcid;
    } else {
        LOG_ERROR("[PAN BNEP]%{public}s PanService is null!", __func__);
    }
}

void PanBnep::BnepRecvConnectionReqCallback(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    if (info == nullptr) {
        LOG_ERROR("[PAN BNEP]%{public}s L2capConnectionInfo is null!", __func__);
        return;
    }
    std::string address = RawAddress::ConvertToString(info->addr.addr).GetAddress();
    PanMessage event(BNEP_L2CAP_CONNECT_REQ_EVT);
    PanL2capConnectionInfo l2capInfo;
    l2capInfo.lcid = lcid;
    l2capInfo.id = id;
    l2capInfo.lpsm = lpsm;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    PanService::GetService()->PostEvent(event);
}

void PanBnep::BnepRecvConnectionRspCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    if (info == nullptr) {
        LOG_ERROR("[PAN BNEP]%{public}s L2capConnectionInfo is null!", __func__);
        return;
    }
    std::string address = RawAddress::ConvertToString(info->addr.addr).GetAddress();
    PanMessage event(BNEP_L2CAP_CONNECT_RSP_EVT);
    PanL2capConnectionInfo l2capInfo;
    l2capInfo.lcid = lcid;
    l2capInfo.result = result;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    PanService::GetService()->PostEvent(event);
}

void PanBnep::BnepRecvConfigReqCallback(
    uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    PanService *service = PanService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[PAN BNEP]%{public}s PanService is null!", __func__);
        return;
    }
    std::string address = service->PanFindDeviceByLcid(lcid);
    if (address.empty()) {
        LOG_ERROR("[PAN BNEP]%{public}s can not find device!", __func__);
        L2CIF_ConfigRsp(lcid, id, cfg, L2CAP_REJECTED, nullptr);
        return;
    }

    if (cfg->rfc.mode == L2CAP_ENHANCED_RETRANSMISSION_MODE) {
        HILOGI("[PAN BNEP] for PTS BROADCAST-0/BV-01-C");
        L2capConfigInfo l2capConfigInfo;
        (void)memset_s(&l2capConfigInfo, sizeof(L2capConfigInfo), 0x00, sizeof(L2capConfigInfo));
        l2capConfigInfo.mtu = BNEP_MTU_SIZE;
        l2capConfigInfo.flushTimeout = 0xFFFF;
        L2CIF_ConfigRsp(lcid, id, &l2capConfigInfo, L2CAP_UNACCEPTABLE_PARAMETERS, nullptr);
        return;
    }

    PanMessage event(BNEP_L2CAP_CONFIG_REQ_EVT);
    PanL2capConnectionInfo l2capInfo;
    L2capConfigInfo l2capConfigInfo;
    if (memcpy_s(&l2capConfigInfo, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo)) != EOK) {
        LOG_ERROR("[PAN BNEP]%{public}s(): memcpy error", __FUNCTION__);
        return;
    }

    l2capInfo.lcid = lcid;
    l2capInfo.id = id;
    l2capInfo.cfg = l2capConfigInfo;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void PanBnep::BnepRecvConfigRspCallback(
    uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    PanService *service = PanService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[PAN BNEP]%{public}s PanService is null!", __func__);
        return;
    }
    std::string address = service->PanFindDeviceByLcid(lcid);
    if (address.empty()) {
        LOG_ERROR("[PAN BNEP]%{public}s can not find device!", __func__);
        return;
    }

    PanMessage event(BNEP_L2CAP_CONFIG_RSP_EVT);
    PanL2capConnectionInfo l2capInfo;
    L2capConfigInfo l2capConfigInfo;
    if (memcpy_s(&l2capConfigInfo, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo)) != EOK) {
        LOG_ERROR("[PAN BNEP]%{public}s(): memcpy error", __FUNCTION__);
        return;
    }

    l2capInfo.lcid = lcid;
    l2capInfo.result = result;
    l2capInfo.cfg = l2capConfigInfo;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void PanBnep::BnepRecvDisconnectionReqCallback(
    uint16_t lcid, uint8_t id, void *ctx)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    PanService *service = PanService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[PAN BNEP]%{public}s PanService is null!", __func__);
        return;
    }
    std::string address = service->PanFindDeviceByLcid(lcid);
    if (address.empty()) {
        LOG_ERROR("[PAN BNEP]%{public}s can not find device!", __func__);
        return;
    }

    PanMessage event(BNEP_L2CAP_DISCONNECT_REQ_EVT);
    PanL2capConnectionInfo l2capInfo;

    l2capInfo.lcid = lcid;
    l2capInfo.id = id;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void PanBnep::BnepRecvDisconnectionRspCallback(
    uint16_t lcid, void *ctx)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    PanService *service = PanService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[PAN BNEP]%{public}s PanService is null!", __func__);
        return;
    }
    std::string address = service->PanFindDeviceByLcid(lcid);
    if (address.empty()) {
        LOG_ERROR("[PAN BNEP]%{public}s can not find device!", __func__);
        return;
    }

    PanMessage event(BNEP_L2CAP_DISCONNECT_RSP_EVT);
    PanL2capConnectionInfo l2capInfo;

    l2capInfo.lcid = lcid;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void PanBnep::BnepDisconnectAbnormalCallback(
    uint16_t lcid, uint8_t reason, void *ctx)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    PanService *service = PanService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[PAN BNEP]%{public}s PanService is null!", __func__);
        return;
    }
    std::string address = service->PanFindDeviceByLcid(lcid);
    if (address.empty()) {
        LOG_ERROR("[PAN BNEP]%{public}s can not find device!", __func__);
        return;
    }

    PanMessage event(BNEP_L2CAP_DISCONNECT_ABNORMAL_EVT);
    PanL2capConnectionInfo l2capInfo;

    l2capInfo.lcid = lcid;
    l2capInfo.reason = reason;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void PanBnep::BnepRecvDataCallback(
    uint16_t lcid, Packet *pkt, void *ctx)
{
    PanService *service = PanService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[PAN BNEP]%{public}s PanService is null!", __func__);
        return;
    }

    std::string address = PanService::GetService()->PanFindDeviceByLcid(lcid);
    if (address.empty()) {
        LOG_ERROR("[PAN BNEP]%{public}s can not find device!", __func__);
        return;
    }

    PanMessage event(BNEP_L2CAP_DATA_EVT);
    PanL2capConnectionInfo l2capInfo;

    l2capInfo.lcid = lcid;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    int dataLength = static_cast<int>(PacketSize(pkt));
    if (dataLength > 0) {
        std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(dataLength);
        PacketRead(pkt, buff.get(), 0, dataLength);
        event.data_ = std::make_shared<std::unique_ptr<uint8_t[]>>(std::move(buff));
    }
    event.dataLength_ = dataLength;
    service->PostEvent(event);
}

void PanBnep::BnepRemoteBusyCallback(
    uint16_t lcid, uint8_t isBusy, void *ctx)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    PanService *service = PanService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[PAN BNEP]%{public}s PanService is null!", __func__);
        return;
    }
    std::string address = service->PanFindDeviceByLcid(lcid);
    if (address.empty()) {
        LOG_ERROR("[PAN BNEP]%{public}s can not find device!", __func__);
        return;
    }

    PanMessage event(BNEP_L2CAP_REMOTE_BUSY_EVT);
    PanL2capConnectionInfo l2capInfo;

    l2capInfo.lcid = lcid;
    l2capInfo.isBusy = isBusy;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void PanBnep::BnepRecvConnectionReqCallbackTask(
    uint16_t lcid, uint8_t id, uint16_t lpsm)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    bool isAccept = true;
    if (state_ != BNEP_CONN_STATE_UNUSED) {
        LOG_ERROR("[PAN BNEP]%{public}s receive control connect,but not in idle state[%{public}d]",
            __func__, state_);
        isAccept = false;
    }
    if (!isAccept) {
        L2CIF_ConnectRsp(lcid, id, L2CAP_NO_RESOURCES_AVAILABLE, L2CAP_NO_FURTHER_INFORMATION_AVAILABLE, nullptr);
        return;
    }

    connFlags_ = 0;
    SendGapRequestSecurity(true, lcid, id);
}

int PanBnep::SendGapRequestSecurity(bool isIncoming, uint16_t lcid, uint8_t id)
{
    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    GapRequestSecurityParam param;
    GapServiceSecurityInfo securityInfo;
    param.context = &address_;
    if (isIncoming) {
        securityInfo = {INCOMING, PAN_BNEP, SEC_PROTOCOL_L2CAP,
                { .l2capPsm = BT_BNEP_PSM }};
        param.callback = BnepIndSecurityCheckDeviceConnectCallback;
        param.info = securityInfo;
        if (GAPIF_RequestSecurityAsync((const BtAddr *)&btAddr, &param)) {
            LOG_ERROR("[PAN BNEP] %{public}s:GAPIF_RequestSecurityAsync INCOMING failed.", __func__);
            return RET_BAD_STATUS;
        }
        lcid_ = lcid;
        id_ = id;
        state_ = BNEP_CONN_STATE_SECURITY;
    } else {
        securityInfo = {OUTGOING, PAN_BNEP, SEC_PROTOCOL_L2CAP,
            { .l2capPsm = BT_BNEP_PSM }};
        param.callback = BnepSecurityCheckCallback;
        param.info = securityInfo;
        if (GAPIF_RequestSecurityAsync((const BtAddr *)&btAddr, &param)) {
            LOG_ERROR("[PAN BNEP] %{public}s:GAPIF_RequestSecurityAsync OUTGOING failed.", __func__);
            PanMessage event(PAN_INT_CLOSE_EVT);
            event.dev_ = address_;
            PanService::GetService()->PostEvent(event);
            return RET_BAD_STATUS;
        }
    }
    return BT_NO_ERROR;
}

void PanBnep::BnepRecvConnectionRspCallbackTask(
    uint16_t lcid, int result)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    if (result == L2CAP_CONNECTION_PENDING) {
        LOG_INFO("[PAN BNEP]%{public}s: Connect RSP result is pending, do nothing!", __func__);
        return;
    }
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN BNEP]%{public}s result[%{public}d]", __func__, result);
        if (lcid == lcid_) {
            lcid_ = 0;
        }

        Disconnect();
        return;
    }

    state_ = BNEP_CONN_STATE_CONFIG;

    L2capConfigInfo l2capConfigInfo;
    (void)memset_s(&l2capConfigInfo, sizeof(L2capConfigInfo), 0x00, sizeof(L2capConfigInfo));
    l2capConfigInfo.mtu = BNEP_MTU_SIZE;
    l2capConfigInfo.flushTimeout = 0xFFFF;
    if (L2CIF_ConfigReq(lcid, &l2capConfigInfo, nullptr)) {
        LOG_ERROR("[PAN BNEP] %{public}s:L2CIF_ConfigReq failed.", __func__);
    }
}

void PanBnep::BnepRecvConfigReqCallbackTask(
    uint16_t lcid, uint8_t id, L2capConfigInfo cfg)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    if (L2CIF_ConfigRsp(lcid, id, &cfg, L2CAP_SUCCESS, nullptr)) {
        LOG_ERROR("[PAN BNEP] %{public}s:L2CIF_ConfigRsp failed.", __func__);
    }
    connFlags_ |= BNEP_CONN_FLAGS_HIS_CFG_DONE;
    if ((connFlags_ & BNEP_CONN_FLAGS_ALL_CONFIGURED) == BNEP_CONN_FLAGS_ALL_CONFIGURED) {
        state_ = BNEP_CONN_STATE_SETUP_CONNECTION;
    }
}

void PanBnep::BnepRecvConfigRspCallbackTask(
    uint16_t lcid, const L2capConfigInfo cfg, uint16_t result)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    if (result != L2CAP_SUCCESS) {
        LOG_ERROR("[PAN BNEP]%{public}s result[%{public}d]", __func__, result);
        Disconnect();
        return;
    }
    connFlags_ |= BNEP_CONN_FLAGS_MY_CFG_DONE;
    if ((connFlags_ & BNEP_CONN_FLAGS_ALL_CONFIGURED) == BNEP_CONN_FLAGS_ALL_CONFIGURED) {
        state_ = BNEP_CONN_STATE_SETUP_CONNECTION;
    }
}

void PanBnep::BnepRecvDisconnectionReqCallbackTask(
    uint16_t lcid, uint8_t id)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    L2CIF_DisconnectionRsp(lcid, id, nullptr);
    state_ = BNEP_CONN_STATE_DISCONNECTING;

    if (lcid == lcid_) {
        lcid_ = 0;
    }

    if (lcid_ == 0) {
        state_ = BNEP_CONN_STATE_UNUSED;
        PanMessage event(PAN_INT_CLOSE_EVT);
        event.dev_ = address_;
        PanService::GetService()->PostEvent(event);
    }
}

void PanBnep::BnepRecvDisconnectionRspCallbackTask(uint16_t lcid)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);

    if (lcid == lcid_) {
        lcid_ = 0;
    }

    if (lcid_ == 0) {
        state_ = BNEP_CONN_STATE_UNUSED;
        PanMessage event(PAN_INT_CLOSE_EVT);
        event.dev_ = address_;
        PanService::GetService()->PostEvent(event);
    }
}

void PanBnep::BnepDisconnectAbnormalCallbackTask(
    uint16_t lcid, uint8_t reason)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu, reason=%{public}d", __func__, lcid, reason);

    if (lcid == lcid_) {
        lcid_ = 0;
    }

    if (lcid_ == 0) {
        state_ = BNEP_CONN_STATE_UNUSED;
        PanMessage event(PAN_INT_CLOSE_EVT);
        event.dev_ = address_;
        PanService::GetService()->PostEvent(event);
    } else {
        Disconnect();
    }
}

void PanBnep::BnepRecvDataCallbackTask(uint16_t lcid, uint8_t *data, int dataLength)
{
    if ((data == nullptr) || (dataLength < 1)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return;
    }
    int offset = 0, ret = 0;

    bool hasExtension = ((data[offset] & 0x80) != 0);
    uint8_t type = data[offset++] & 0x7f;
    EthernetHeader ethernetHeader;
    bool isEthernetData = false;

    switch (type) {
        case BNEP_CONTROL:
            ret = ProcessBnepControlData(data + offset, dataLength - offset, false);
            break;
        case BNEP_GENERAL_ETHERNET:
        case BNEP_COMPRESSED_ETHERNET:
        case BNEP_COMPRESSED_ETHERNET_SRC_ONLY:
        case BNEP_COMPRESSED_ETHERNET_DEST_ONLY:
            ret = ProcessBnepEthernetPacketHeader(type, ethernetHeader, data + offset, dataLength - offset);
            isEthernetData = true;
            break;
        default:
            LOG_ERROR("[PAN BNEP]%{public}s unknow bnep data type:%{public}d", __func__, type);
            break;
    }
    if (ret == 0) {
        return;
    }
    offset += ret;

    if (hasExtension) {
        ret = ProcessBnepExtensionData(data + offset, dataLength - offset);
        if (ret == 0) {
            return;
        }
        offset += ret;
    }

    if (isEthernetData) {
        SendBnepDataEvent(ethernetHeader, dataLength, offset, data);
    }
}

void PanBnep::SendBnepDataEvent(EthernetHeader ethernetHeader, int dataLength, int offset, uint8_t *data)
{
    PanMessage event(PAN_INT_DATA_EVT);
    event.dev_ = address_;
    event.ethernetHeader_ = ethernetHeader;
    if ((dataLength - offset) > 0) {
        event.dataLength_ = dataLength - offset;
        std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(event.dataLength_);
        if (memcpy_s(buff.get(), event.dataLength_, data + offset, event.dataLength_) != EOK) {
            LOG_ERROR("[PAN BNEP]%{public}s(): memcpy error", __FUNCTION__);
            return;
        }
        event.data_ = std::make_shared<std::unique_ptr<uint8_t[]>>(std::move(buff));
    }
    PanService::GetService()->PostEvent(event);
}

void PanBnep::BnepRemoteBusyCallbackTask(
    uint16_t lcid, uint8_t isBusy)
{
    LOG_INFO("[PAN BNEP]%{public}s lcid:%{public}hu", __func__, lcid);
    isBusy_ = ((isBusy != 0) ? true : false);
    while (!isBusy && !waitingSendDataList_.empty()) {
        Packet *packet = waitingSendDataList_.front();
        waitingSendDataList_.pop();
        L2CIF_SendData(lcid_, packet, nullptr);
        PacketFree(packet);
    }
    PanService::GetService()->ReceiveRemoteBusy(isBusy_);
}

int PanBnep::ProcessBnepExtensionData(uint8_t *data, int dataLength)
{
    int offset = 0;
    int ret = 0;
    if ((data == nullptr) || (dataLength < 1)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return 0;
    }
    bool hasNextExtension = ((data[offset] & 0x80) != 0);
    uint8_t extensionType = data[offset++] & 0x7f;
    if (extensionType != BNEP_EXTENTION_CONTROL) {
        LOG_ERROR("[PAN BNEP]%{public}s unknow extension control type! extensionType=%{public}d",
            __func__, extensionType);
        return 0;
    }
    ret = ProcessBnepControlData(data + offset, dataLength - offset, true);
    if (ret == 0) {
        return 0;
    }
    offset += ret;
    if (hasNextExtension) {
        ret = ProcessBnepExtensionData(data + offset, dataLength - offset);
        if (ret == 0) {
            return 0;
        }
        offset += ret;
    }
    return offset;
}

int PanBnep::ProcessBnepControlData(uint8_t *data, int dataLength, bool isExtension)
{
    int offset = 0;
    int ret = 0;
    uint8_t extensionLength = 0;
    if ((data == nullptr) || (dataLength < 1)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return 0;
    }
    if (isExtension) {
        extensionLength = data[offset++];
        if (dataLength - offset < 1) {
            LOG_ERROR("[PAN BNEP]%{public}s extension data length is error!", __func__);
            return 0;
        }
    }
    uint8_t controlType = data[offset++];
    switch (controlType) {
        case BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD:
            ret = ProcessBnepControlCommandNotUnderstood(data + offset, dataLength - offset);
            break;
        case BNEP_SETUP_CONNECTION_REQUEST_MSG:
            ret = ProcessBnepSetupConnectionRequest(data + offset, dataLength - offset);
            break;
        case BNEP_SETUP_CONNECTION_RESPONSE_MSG:
            ret = ProcessBnepSetupConnectionResponse(data + offset, dataLength - offset);
            break;
        case BNEP_FILTER_NET_TYPE_SET_MSG:
            ret = ProcessBnepFilterNetTypeSet(data + offset, dataLength - offset);
            break;
        case BNEP_FILTER_NET_TYPE_RESPONSE_MSG:
            ret = ProcessBnepFilterNetTypeResponse(data + offset, dataLength - offset);
            break;
        case BNEP_FILTER_MULTI_ADDR_SET_MSG:
            ret = ProcessBnepFilterMultiAddrSet(data + offset, dataLength - offset);
            break;
        case BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG:
            ret = ProcessBnepFilterMultiAddrResponse(data + offset, dataLength - offset);
            break;
        default:
            LOG_ERROR("[PAN BNEP]%{public}s unknow control type ", __func__);
            BnepSendControlCommandNotUnderstood(controlType);
            if ((dataLength - offset) >= extensionLength) {
                ret = extensionLength;
            }
            break;
    }
    if (ret == 0) {
        return 0;
    }
    return (offset + ret);
}

int PanBnep::ProcessBnepControlCommandNotUnderstood(uint8_t *data, int dataLength)
{
    int offset = 0;
    if ((data == nullptr) || (dataLength < 1)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return 0;
    }
    uint8_t controlCode = data[offset++];
    LOG_ERROR("[PAN BNEP]%{public}s receive BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD code=%{public}d",
        __func__, controlCode);
    return offset;
}

int PanBnep::ProcessBnepSetupConnectionRequest(uint8_t *data, int dataLength)
{
    int offset = 0;
    Uuid localUuid;
    Uuid remoteUuid;
    if ((data == nullptr) || (dataLength < 1)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return 0;
    }
    uint8_t uuidType = data[offset++];
    if ((dataLength - offset) < (uuidType + uuidType)) {
        LOG_ERROR("[PAN BNEP]%{public}s data length is error!", __func__);
        return 0;
    }
    if (uuidType == Uuid::UUID16_BYTES_TYPE) {
        uint16_t uuid = be16toh(*reinterpret_cast<uint16_t *>(data + offset));
        offset += uuidType;
        localUuid = Uuid::ConvertFrom16Bits(uuid);
        uuid = be16toh(*reinterpret_cast<uint16_t *>(data + offset));
        offset += uuidType;
        remoteUuid = Uuid::ConvertFrom16Bits(uuid);
    } else if (uuidType == Uuid::UUID32_BYTES_TYPE) {
        uint32_t uuid = be32toh(*reinterpret_cast<uint32_t *>(data + offset));
        offset += uuidType;
        localUuid = Uuid::ConvertFrom32Bits(uuid);
        uuid = be32toh(*reinterpret_cast<uint32_t *>(data + offset));
        offset += uuidType;
        remoteUuid = Uuid::ConvertFrom32Bits(uuid);
    } else if (uuidType == Uuid::UUID128_BYTES_TYPE) {
        localUuid = Uuid::ConvertFromBytesLE(data + offset);
        offset += uuidType;
        remoteUuid = Uuid::ConvertFromBytesLE(data + offset);
        offset += uuidType;
    } else {
        LOG_ERROR("[PAN BNEP]%{public}s error uuid type(%{public}d).", __func__, uuidType);
        BnepSendSetupConnectionResponse(BNEP_SETUP_FAILED_INVALID_UUID_SIZE);
        return offset;
    }
    CheckRoleUuid(localUuid, remoteUuid);

    return offset;
}

int PanBnep::ProcessBnepSetupConnectionResponse(uint8_t *data, int dataLength)
{
    int offset = 0;
    if ((data == nullptr) || (dataLength < BNEP_UINT16_SIZE)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return 0;
    }
    uint16_t responseMessage = be16toh(*reinterpret_cast<uint16_t *>(data + offset));
    offset += BNEP_UINT16_SIZE;
    LOG_ERROR("[PAN BNEP]%{public}s we are not origination! responseMessage=%{public}d",
        __func__, responseMessage);
    return offset;
}

int PanBnep::ProcessBnepFilterNetTypeSet(uint8_t *data, int dataLength)
{
    int offset = 0;
    int ret = 0;
    uint16_t filterNum = 0;
    uint16_t responseMessage = BNEP_FILTER_SUCCESSFULL;
    if ((data == nullptr) || (dataLength < BNEP_UINT16_SIZE)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return 0;
    }
    uint16_t listLength = be16toh(*reinterpret_cast<uint16_t *>(data + offset));
    offset += BNEP_UINT16_SIZE;
    ret = offset + listLength;
    if ((dataLength - offset) < listLength) {
        LOG_ERROR("[PAN BNEP]%{public}s data length is error!", __func__);
        return 0;
    }
    if ((listLength % BNEP_PROTOCOL_FILTER_RANGE_LENGTH) != 0) {
        LOG_ERROR("[PAN BNEP]%{public}s filter length is error!", __func__);
        responseMessage = BNEP_FILTER_FAILED_INVALID_NET_TYPE_RANGE;
    } else {
        filterNum = listLength / BNEP_PROTOCOL_FILTER_RANGE_LENGTH;
        if (filterNum > BNEP_MAX_PROTOCOL_FILTER) {
            responseMessage = BNEP_FILTER_FAILED_TOO_MANY_FILTER;
        }
    }
    if (responseMessage != BNEP_FILTER_SUCCESSFULL) {
        BnepSendFilterNetTypeResponse(responseMessage);
        return ret;
    }
    std::vector<BnepProtocolFilterRange> tempFilters_;
    for (uint16_t i = 0; i < filterNum; i++) {
        BnepProtocolFilterRange range;
        range.start = be16toh(*reinterpret_cast<uint16_t *>(data + offset));
        range.end = be16toh(*reinterpret_cast<uint16_t *>(data + offset + BNEP_UINT16_SIZE));
        if (range.start > range.end) {
            LOG_ERROR("[PAN BNEP]%{public}s range.start > range.end!", __func__);
            BnepSendFilterNetTypeResponse(BNEP_FILTER_FAILED_INVALID_NET_TYPE_RANGE);
            return ret;
        }
        tempFilters_.push_back(range);
        offset += BNEP_PROTOCOL_FILTER_RANGE_LENGTH;
    }
    protocolFilterList_ = tempFilters_;
    BnepSendFilterNetTypeResponse(responseMessage);
    return ret;
}

int PanBnep::ProcessBnepFilterNetTypeResponse(uint8_t *data, int dataLength)
{
    int offset = 0;
    if ((data == nullptr) || (dataLength < BNEP_UINT16_SIZE)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return 0;
    }
    uint16_t responseMessage = be16toh(*reinterpret_cast<uint16_t *>(data + offset));
    offset += BNEP_UINT16_SIZE;
    LOG_ERROR("[PAN BNEP]%{public}s we are not set filter! responseMessage=%{public}d",
        __func__, responseMessage);
    return offset;
}

int PanBnep::ProcessBnepFilterMultiAddrSet(uint8_t *data, int dataLength)
{
    int offset = 0;
    int ret = 0;
    int filterNum = 0;
    uint16_t responseMessage = BNEP_FILTER_SUCCESSFULL;
    if ((data == nullptr) || (dataLength < BNEP_UINT16_SIZE)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return 0;
    }
    uint16_t listLength = be16toh(*reinterpret_cast<uint16_t *>(data + offset));
    offset += BNEP_UINT16_SIZE;
    ret = offset + listLength;
    if ((dataLength - offset) < listLength) {
        LOG_ERROR("[PAN BNEP]%{public}s data length is error!", __func__);
        return 0;
    }
    filterNum = listLength / BNEP_MULTYCAST_FILTER_RANGE_LENGTH;
    if ((listLength % BNEP_MULTYCAST_FILTER_RANGE_LENGTH) != 0) {
        LOG_ERROR("[PAN BNEP]%{public}s filter length is error!", __func__);
        responseMessage = BNEP_FILTER_FAILED_INVALID_NET_TYPE_RANGE;
    } else if (filterNum > BNEP_MAX_MULTYCAST_FILTER) {
        responseMessage = BNEP_FILTER_FAILED_TOO_MANY_FILTER;
    } else {
        std::vector<BnepMultycastFilterRange> tempFilters_;
        for (int i = 0; i < filterNum; i++) {
            BnepMultycastFilterRange range;
            if (memcpy_s(range.start, BT_ADDRESS_LENGTH, data + offset, BT_ADDRESS_LENGTH) != EOK) {
                LOG_ERROR("[PAN BNEP]%{public}s(): memcpy range start error", __FUNCTION__);
                BnepSendFilterMultiAddrResponse(BNEP_FILTER_FAILED_INVALID_NET_TYPE_RANGE);
                return ret;
            }
            offset += BT_ADDRESS_LENGTH;
            if (memcpy_s(range.end, BT_ADDRESS_LENGTH, data + offset, BT_ADDRESS_LENGTH) != EOK) {
                LOG_ERROR("[PAN BNEP]%{public}s(): memcpy range end error", __FUNCTION__);
                BnepSendFilterMultiAddrResponse(BNEP_FILTER_FAILED_INVALID_NET_TYPE_RANGE);
                return ret;
            }
            offset += BT_ADDRESS_LENGTH;
            if (memcmp(range.start, range.end, BT_ADDRESS_LENGTH) > 0) {
                LOG_ERROR("[PAN BNEP]%{public}s range.start > range.end!", __func__);
                BnepSendFilterMultiAddrResponse(BNEP_FILTER_FAILED_INVALID_NET_TYPE_RANGE);
                return ret;
            }
            tempFilters_.push_back(range);
        }
        multycastFilterList_ = tempFilters_;
    }
    BnepSendFilterMultiAddrResponse(responseMessage);
    return ret;
}

int PanBnep::ProcessBnepFilterMultiAddrResponse(uint8_t *data, int dataLength)
{
    int offset = 0;
    if ((data == nullptr) || (dataLength < BNEP_UINT16_SIZE)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return 0;
    }
    uint16_t responseMessage = be16toh(*reinterpret_cast<uint16_t *>(data + offset));
    offset += BNEP_UINT16_SIZE;
    LOG_ERROR("[PAN BNEP]%{public}s we are not set filter! responseMessage=%{public}d",
        __func__, responseMessage);
    return offset;
}

void PanBnep::CheckRoleUuid(Uuid localUuid, Uuid remoteUuid)
{
    if (remoteUuid.GetUuidType() != Uuid::UUID16_BYTES_TYPE) {
        BnepSendSetupConnectionResponse(BNEP_SETUP_FAILED_INVALID_SRC_UUID);
    } else if (localUuid.GetUuidType() != Uuid::UUID16_BYTES_TYPE) {
        BnepSendSetupConnectionResponse(BNEP_SETUP_FAILED_INVALID_DEST_UUID);
    } else if ((remoteUuid.ConvertTo16Bits() != BNEP_UUID_PANU) ||
        (localUuid.ConvertTo16Bits() != BNEP_UUID_NAP)) {
        BnepSendSetupConnectionResponse(BNEP_SETUP_FAILED_CONN_NOT_ALLOWED);
        LOG_ERROR("[PAN BNEP]%{public}s error role uuid remote(%{public}x), local(%{public}x).", __func__,
            remoteUuid.ConvertTo16Bits(), localUuid.ConvertTo16Bits());
    } else {
        BnepSendSetupConnectionResponse(BNEP_SETUP_SUCCESSFULL);
        if (state_ == BNEP_CONN_STATE_SETUP_CONNECTION) {
            state_ = BNEP_CONN_STATE_CONNECTED;
            PanMessage event(PAN_INT_OPEN_EVT);
            event.dev_ = address_;
            PanService::GetService()->PostEvent(event);
        }
    }
}

void PanBnep::BnepSendControlCommandNotUnderstood(uint8_t controlCommand)
{
    Packet *packet = nullptr;
    uint8_t *buf = nullptr;
    uint8_t offset = 0;
    uint8_t packetLength = BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD_LENGTH;

    packet = PacketMalloc(packetLength, 0, 0);
    buf = (uint8_t *)BufferPtr(PacketHead(packet));
    buf[offset] = BNEP_CONTROL;
    offset++;
    buf[offset] = BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD;
    offset++;
    buf[offset] = controlCommand;

    L2CIF_SendData(lcid_, packet, nullptr);
    LOG_DEBUG("[PAN BNEP] %{public}s send control command not understood ", __FUNCTION__);
    PacketFree(packet);
    packet = nullptr;
}

void PanBnep::BnepSendSetupConnectionResponse(uint16_t responseMessage)
{
    Packet *packet = nullptr;
    uint8_t *buf = nullptr;
    uint8_t offset = 0;
    uint8_t packetLength = BNEP_CONTROL_RESPONSE_LENGTH;

    packet = PacketMalloc(packetLength, 0, 0);
    buf = (uint8_t *)BufferPtr(PacketHead(packet));
    buf[offset] = BNEP_CONTROL;
    offset++;
    buf[offset] = BNEP_SETUP_CONNECTION_RESPONSE_MSG;
    offset++;
    *(reinterpret_cast<uint16_t *>(buf + offset)) = htobe16(responseMessage);

    L2CIF_SendData(lcid_, packet, nullptr);
    LOG_DEBUG("[PAN BNEP] %{public}s send bnep setup connection response ", __FUNCTION__);
    PacketFree(packet);
    packet = nullptr;
}

void PanBnep::BnepSendFilterNetTypeResponse(uint16_t responseMessage)
{
    Packet *packet = nullptr;
    uint8_t *buf = nullptr;
    uint8_t offset = 0;
    uint8_t packetLength = BNEP_CONTROL_RESPONSE_LENGTH;

    packet = PacketMalloc(packetLength, 0, 0);
    buf = (uint8_t *)BufferPtr(PacketHead(packet));
    buf[offset] = BNEP_CONTROL;
    offset++;
    buf[offset] = BNEP_FILTER_NET_TYPE_RESPONSE_MSG;
    offset++;
    *(reinterpret_cast<uint16_t *>(buf + offset)) = htobe16(responseMessage);

    L2CIF_SendData(lcid_, packet, nullptr);
    LOG_DEBUG("[PAN BNEP] %{public}s send BNEP_FILTER_NET_TYPE_RESPONSE_MSG ", __FUNCTION__);
    PacketFree(packet);
    packet = nullptr;
}

void PanBnep::BnepSendFilterMultiAddrResponse(uint16_t responseMessage)
{
    Packet *packet = nullptr;
    uint8_t *buf = nullptr;
    uint8_t offset = 0;
    uint8_t packetLength = BNEP_CONTROL_RESPONSE_LENGTH;

    packet = PacketMalloc(packetLength, 0, 0);
    buf = (uint8_t *)BufferPtr(PacketHead(packet));
    buf[offset] = BNEP_CONTROL;
    offset++;
    buf[offset] = BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG;
    offset++;
    *(reinterpret_cast<uint16_t *>(buf + offset)) = htobe16(responseMessage);

    L2CIF_SendData(lcid_, packet, nullptr);
    LOG_DEBUG("[PAN BNEP] %{public}s send BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG ", __FUNCTION__);
    PacketFree(packet);
    packet = nullptr;
}

int PanBnep::ProcessBnepEthernetPacketHeader(uint8_t type, EthernetHeader &ethernetHeader,
    uint8_t *data, int dataLength)
{
    if ((data == nullptr) || (dataLength < 1)) {
        LOG_ERROR("[PAN BNEP]%{public}s data is null!", __func__);
        return 0;
    }
    int offset = 0;
    if ((type == BNEP_COMPRESSED_ETHERNET_DEST_ONLY) || (type == BNEP_GENERAL_ETHERNET)) {
        if (dataLength < (BT_ADDRESS_LENGTH + offset)) {
            LOG_ERROR("[PAN BNEP]%{public}s copy destination Address,data length is error!", __func__);
            return 0;
        }
        if (memcpy_s(ethernetHeader.destAddr, BT_ADDRESS_LENGTH, data + offset,
            BT_ADDRESS_LENGTH) != EOK) {
            LOG_ERROR("[PAN BNEP]%{public}s(): memcpy destination Address error", __FUNCTION__);
            return 0;
        }
        offset += BT_ADDRESS_LENGTH;
    } else {
        uint8_t bluetoothDestAddr[BT_ADDRESS_LENGTH];
        RawAddress(PanService::GetLocalAddress()).ConvertToUint8(bluetoothDestAddr);
        PanService::ReverseAddress(bluetoothDestAddr, ethernetHeader.destAddr);
        if (ethernetHeader.destAddr[0] & 0x01) {
            LOG_DEBUG("[PAN BNEP]%{public}s():ethernetHeader.destAddr use unicast MAC address", __FUNCTION__);
            ethernetHeader.destAddr[0] &= ~0x01;
        }
    }

    if ((type == BNEP_COMPRESSED_ETHERNET_SRC_ONLY) || (type == BNEP_GENERAL_ETHERNET)) {
        if (dataLength < (BT_ADDRESS_LENGTH + offset)) {
            LOG_ERROR("[PAN BNEP]%{public}s copy source Address,data length is error!", __func__);
            return 0;
        }
        if (memcpy_s(ethernetHeader.srcAddr, BT_ADDRESS_LENGTH, data + offset,
            BT_ADDRESS_LENGTH) != EOK) {
            LOG_ERROR("[PAN BNEP]%{public}s(): memcpy source Address error", __FUNCTION__);
            return 0;
        }
        offset += BT_ADDRESS_LENGTH;
    } else {
        uint8_t bluetoothSrcAddr[BT_ADDRESS_LENGTH];
        RawAddress(address_).ConvertToUint8(bluetoothSrcAddr);
        PanService::ReverseAddress(bluetoothSrcAddr, ethernetHeader.srcAddr);
    }

    if (dataLength < (BNEP_UINT16_SIZE + offset)) {
        LOG_ERROR("[PAN BNEP]%{public}s get protocol,data length is error!", __func__);
        return 0;
    }
    ethernetHeader.protocol = be16toh(*reinterpret_cast<uint16_t *>(data + offset));
    offset += BNEP_UINT16_SIZE;
    return offset;
}

void PanBnep::ProcessEvent(const PanMessage &event)
{
    LOG_DEBUG("[PAN BNEP]%{public}s(): event %{public}s[%{public}d]",
        __FUNCTION__, PanBnep::GetEventName(event.what_).c_str(), event.what_);

    switch (event.what_) {
        case BNEP_L2CAP_CONNECT_REQ_EVT:
            BnepRecvConnectionReqCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.id, event.l2capInfo_.lpsm);
            break;
        case BNEP_L2CAP_CONNECT_RSP_EVT:
            BnepRecvConnectionRspCallbackTask(
                event.l2capInfo_.lcid, event.l2capInfo_.result);
            break;
        case BNEP_L2CAP_CONFIG_REQ_EVT:
            BnepRecvConfigReqCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.id, event.l2capInfo_.cfg);
            break;
        case BNEP_L2CAP_CONFIG_RSP_EVT:
            BnepRecvConfigRspCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.cfg, event.l2capInfo_.result);
            break;
        case BNEP_L2CAP_DISCONNECT_REQ_EVT:
            BnepRecvDisconnectionReqCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.id);
            break;
        case BNEP_L2CAP_DISCONNECT_RSP_EVT:
            BnepRecvDisconnectionRspCallbackTask(event.l2capInfo_.lcid);
            break;
        case BNEP_L2CAP_DISCONNECT_ABNORMAL_EVT:
            BnepDisconnectAbnormalCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.reason);
            break;
        case BNEP_L2CAP_DATA_EVT:
            if (event.data_ != nullptr) {
                BnepRecvDataCallbackTask(event.l2capInfo_.lcid, event.data_->get(), event.dataLength_);
            } else {
                BnepRecvDataCallbackTask(event.l2capInfo_.lcid, nullptr, 0);
            }
            break;
        case BNEP_L2CAP_REMOTE_BUSY_EVT:
            BnepRemoteBusyCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.isBusy);
            break;
        case BNEP_L2CAP_SECURITY_RESULT_EVT:
            BnepSecurityCheckTask(event.arg1_);
            break;
        case BNEP_L2CAP_IND_SECURITY_RESULT_EVT:
            BnepSecurityCheckDeviceConnectTask(event.arg1_);
            break;
        default:
            break;
    }
}

std::string PanBnep::GetEventName(int what)
{
    switch (what) {
        case BNEP_L2CAP_CONNECT_REQ_EVT:
            return "BNEP_L2CAP_CONNECT_REQ_EVT";
        case BNEP_L2CAP_CONNECT_RSP_EVT:
            return "BNEP_L2CAP_CONNECT_RSP_EVT";
        case BNEP_L2CAP_CONFIG_REQ_EVT:
            return "BNEP_L2CAP_CONFIG_REQ_EVT";
        case BNEP_L2CAP_CONFIG_RSP_EVT:
            return "BNEP_L2CAP_CONFIG_RSP_EVT";
        case BNEP_L2CAP_DISCONNECT_REQ_EVT:
            return "BNEP_L2CAP_DISCONNECT_REQ_EVT";
        case BNEP_L2CAP_DISCONNECT_RSP_EVT:
            return "BNEP_L2CAP_DISCONNECT_RSP_EVT";
        case BNEP_L2CAP_DISCONNECT_ABNORMAL_EVT:
            return "BNEP_L2CAP_DISCONNECT_ABNORMAL_EVT";
        case BNEP_L2CAP_DATA_EVT:
            return "BNEP_L2CAP_DATA_EVT";
        case BNEP_L2CAP_REMOTE_BUSY_EVT:
            return "BNEP_L2CAP_REMOTE_BUSY_EVT";
        case BNEP_L2CAP_SECURITY_RESULT_EVT:
            return "BNEP_L2CAP_SECURITY_RESULT_EVT";
        case BNEP_L2CAP_IND_SECURITY_RESULT_EVT:
            return "BNEP_L2CAP_IND_SECURITY_RESULT_EVT";
        default:
            return "Unknown";
    }
}
}  // namespace bluetooth
}  // namespace OHOS
