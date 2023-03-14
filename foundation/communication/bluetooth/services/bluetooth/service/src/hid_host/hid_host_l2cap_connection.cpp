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

#include "hid_host_l2cap_connection.h"
#include "hid_host_service.h"

namespace OHOS {
namespace bluetooth {
int HidHostL2capConnection::Startup()
{
    GapServiceSecurityInfo securityInfo = {INCOMING, HID_HOST, SEC_PROTOCOL_L2CAP,
        { .l2capPsm = HID_CONTROL_PSM }};
    if (GAPIF_RegisterServiceSecurity(nullptr, &securityInfo,
        GAP_SEC_IN_ENCRYPTION | GAP_SEC_IN_AUTHENTICATION |
        GAP_SEC_OUT_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION)) {
        LOG_ERROR("[HIDH L2CAP] %{public}s:GAPIF_RegisterServiceSecurity INCOMING failed.", __func__);
        return RET_BAD_STATUS;
    }

    securityInfo = {OUTGOING, HID_HOST, SEC_PROTOCOL_L2CAP,
        { .l2capPsm = HID_CONTROL_PSM }};
    if (GAPIF_RegisterServiceSecurity(nullptr, &securityInfo,
        GAP_SEC_IN_ENCRYPTION | GAP_SEC_IN_AUTHENTICATION |
        GAP_SEC_OUT_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION)) {
            LOG_ERROR("[HIDH L2CAP] %{public}s:GAPIF_RegisterServiceSecurity OUTGOING failed.", __func__);
        return RET_BAD_STATUS;
    }

    /* regist hid control psm */
    if (L2CIF_RegisterService(HID_CONTROL_PSM, (L2capService *)&L2CAP_HID_Host_CALLBACK, nullptr, nullptr)) {
        LOG_ERROR("[HIDH L2CAP] %{public}s:L2CIF_RegisterService HID_CONTROL_PSM failed.", __func__);
        return RET_BAD_STATUS;
    }
    /* regist hid interrupt psm */
    if (L2CIF_RegisterService(HID_INTERRUPT_PSM, (L2capService *)&L2CAP_HID_Host_CALLBACK, nullptr, nullptr)) {
        L2CIF_DeregisterService(HID_CONTROL_PSM, nullptr);
        LOG_ERROR("[HIDH L2CAP] %{public}s:L2CIF_RegisterService HID_INTERRUPT_PSM failed.", __func__);
        return RET_BAD_STATUS;
    }
    return BT_NO_ERROR;
}

void HidHostL2capConnection::Shutdown()
{
    L2CIF_DeregisterService(HID_CONTROL_PSM, nullptr);
    L2CIF_DeregisterService(HID_INTERRUPT_PSM, nullptr);
    GapServiceSecurityInfo securityInfo = {INCOMING, HID_HOST, SEC_PROTOCOL_L2CAP,
        { .l2capPsm = HID_CONTROL_PSM }};
    GAPIF_DeregisterServiceSecurity(nullptr, &securityInfo);
    securityInfo = {OUTGOING, HID_HOST, SEC_PROTOCOL_L2CAP,
        { .l2capPsm = HID_CONTROL_PSM }};
    GAPIF_DeregisterServiceSecurity(nullptr, &securityInfo);
}

HidHostL2capConnection::HidHostL2capConnection(std::string address)
{
    address_ = address;
    state_ = HID_HOST_CONN_STATE_UNUSED;
    ctrlLcid_ = 0;
    intrLcid_ = 0;
    connFlags_ = 0;
    ctrlId_ = 0;
}

int HidHostL2capConnection::Connect()
{
    LOG_INFO("[HIDH L2CAP]%{public}s", __func__);
    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    if (state_ == HID_HOST_CONN_STATE_CONNECTED) {
        HidHostMessage event(HID_HOST_INT_OPEN_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
        return BT_NO_ERROR;
    }

    if (state_ != HID_HOST_CONN_STATE_UNUSED) {
        LOG_ERROR("[HIDH L2CAP] %{public}s:state error state = %{public}d", __func__, state_);
        return RET_BAD_STATUS;
    }

    return SendGapRequestSecurity(false, 0, 0);
}

/**
 *
 * @brief         HidHostSecurityCheckCallback
 *
 * @details       Host receive the security check result
 *
 * @return        uint16_t
 *
 */
void HidHostL2capConnection::HidHostSecurityCheckCallback(
    uint16_t result, GapServiceSecurityInfo security, void *context)
{
    LOG_INFO("[HIDH L2CAP]%{public}s:result(%{public}hu), l2capPsm(0x%{public}x)",
        __func__, result, security.channelId.l2capPsm);
    std::string *address = (std::string *)context;
    if (address != nullptr) {
        HidHostMessage event(HID_HOST_L2CAP_SECURITY_RESULT_EVT, result);
        event.dev_ = *address;
        HidHostService::GetService()->PostEvent(event);
    } else {
        LOG_ERROR("[HIDH L2CAP]%{public}s HidHostL2capConnection is null.", __func__);
    }
}

void HidHostL2capConnection::HidHostSecurityCheckTask(uint16_t result)
{
    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    connFlags_ = HID_HOST_CONN_FLAGS_IS_ORIG;

    if (result == BT_NO_ERROR) {
        LOG_INFO("[HIDH L2CAP]%{public}s start connect HID control", __func__);
        int ret = L2CIF_ConnectReq(&btAddr, HID_CONTROL_PSM, HID_CONTROL_PSM,
            this, HidHostL2cConnectControlReqCallback);
        if (ret != BT_NO_ERROR) {
            LOG_ERROR("[HIDH L2CAP]%{public}s L2CIF_ConnectReq return value is error.", __func__);
            state_ = HID_HOST_CONN_STATE_UNUSED;

            HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
            event.dev_ = address_;
            HidHostService::GetService()->PostEvent(event);
            return;
        }
        state_ = HID_HOST_CONN_STATE_CONNECTING_CTRL;
    } else {
        LOG_ERROR("[HIDH L2CAP]%{public}s Request Security faild!", __func__);
        state_ = HID_HOST_CONN_STATE_UNUSED;

        HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
    }
}

/**
 *
 * @brief         HidHostIndSecurityCheckCallback
 *
 * @details       Host receive the security check result，when receive device's connect request.
 *
 * @return        uint16_t
 *
 */
void HidHostL2capConnection::HidHostIndSecurityCheckDeviceConnectCallback(uint16_t result,
    GapServiceSecurityInfo security, void *context)
{
    LOG_INFO("[HIDH L2CAP]%{public}s:result(%{public}hu), l2capPsm(0x%{public}x)",
        __func__, result, security.channelId.l2capPsm);
    std::string *address = (std::string *)context;
    if (address != nullptr) {
        HidHostMessage event(HID_HOST_L2CAP_IND_SECURITY_RESULT_EVT, result);
        event.dev_ = *address;
        HidHostService::GetService()->PostEvent(event);
    } else {
        LOG_ERROR("[HIDH L2CAP]%{public}s HidHostL2capConnection is null.", __func__);
    }
}

void HidHostL2capConnection::HidHostSecurityCheckDeviceConnectTask(uint16_t result)
{
    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    if ((result == BT_NO_ERROR) && (state_ == HID_HOST_CONN_STATE_SECURITY)) {
        LOG_INFO("[HIDH L2CAP]%{public}s start send connect response", __func__);
        state_ = HID_HOST_CONN_STATE_CONNECTING_INTR;
        L2CIF_ConnectRsp(ctrlLcid_, ctrlId_, L2CAP_CONNECTION_SUCCESSFUL, 0, nullptr);

        L2capConfigInfo l2capConfigInfo;
        (void)memset_s(&l2capConfigInfo, sizeof(L2capConfigInfo), 0x00, sizeof(L2capConfigInfo));
        l2capConfigInfo.mtu = HID_HOST_MTU;
        l2capConfigInfo.flushTimeout = 0xFFFF;
        if (L2CIF_ConfigReq(ctrlLcid_, &l2capConfigInfo, nullptr)) {
            LOG_ERROR("[HIDH L2CAP] %{public}s:L2CIF_ConfigReq failed.", __func__);
        }
    } else if (result != BT_NO_ERROR) {
        LOG_ERROR("[HIDH L2CAP]%{public}s Request Security faild!", __func__);
        state_ = HID_HOST_CONN_STATE_UNUSED;

        HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
    }
}

int HidHostL2capConnection::Disconnect()
{
    LOG_INFO("[HID L2CAP]%{public}s", __func__);

    if (state_ == HID_HOST_CONN_STATE_UNUSED) {
        HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
        return BT_NO_ERROR;
    }

    if ((ctrlLcid_ != 0) || (intrLcid_ != 0)) {
        state_ = HID_HOST_CONN_STATE_DISCONNECTING;
        if (intrLcid_) {
            L2CIF_DisconnectionReq(intrLcid_, nullptr);
        } else if (ctrlLcid_) {
            L2CIF_DisconnectionReq(ctrlLcid_, nullptr);
        }
    } else {
        state_ = HID_HOST_CONN_STATE_UNUSED;
        HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
        return BT_NO_ERROR;
    }

    return BT_NO_ERROR;
}

int HidHostL2capConnection::SendData(SendHidData sendData, int length, uint8_t* pkt)
{
    switch (sendData.type) {
        case HID_HOST_DATA_TYPE_GET_REPORT:
        case HID_HOST_DATA_TYPE_CONTROL:
        case HID_HOST_DATA_TYPE_DATA:
            SendGetReport(sendData);
            break;
        case HID_HOST_DATA_TYPE_SET_REPORT:
            SendSetReport(sendData, length, pkt);
            break;
        default:
            return RET_BAD_PARAM;
    }

    return BT_NO_ERROR;
}

void HidHostL2capConnection::SendGetReport(SendHidData sendData)
{
    Packet *packet = nullptr;
    uint8_t *buf = nullptr;
    uint16_t lcid = 0;
    uint8_t offset = 0;
    uint8_t packetLength = sizeof(uint8_t);
    if (sendData.reportId != 0) {
        packetLength += sizeof(uint8_t);
    }
    // If have max buff size,add the buff size to last
    if (sendData.data > 0) {
        packetLength += sizeof(uint16_t);
    }
    packet = PacketMalloc(packetLength, 0, 0);
    buf = (uint8_t *)BufferPtr(PacketHead(packet));
    if (sendData.data > 0) {
        buf[offset] = static_cast<uint8_t>(sendData.type << HID_HOST_SHIFT_OPRATURN_4) |
            ((sendData.param | 0x08) & 0x0f);
        offset++;
        if (sendData.reportId != 0) {
            buf[offset] = sendData.reportId;
            offset++;
        }
        buf[offset] = sendData.data & 0xff;
        offset++;
        buf[offset] = (sendData.data >> HID_HOST_SHIFT_OPRATURN_8) & 0xff;
    } else {
        buf[offset] = (sendData.type << HID_HOST_SHIFT_OPRATURN_4) | (sendData.param & 0x0f);
        offset++;
        if (sendData.reportId != 0) {
            buf[offset] = sendData.reportId;
        }
    }
    if (sendData.type == HID_HOST_DATA_TYPE_DATA) {
        lcid = intrLcid_;
    } else {
        lcid = ctrlLcid_;
    }
    L2CIF_SendData(lcid, packet, nullptr);
    PacketFree(packet);
    packet = nullptr;
}

void HidHostL2capConnection::SendSetReport(SendHidData sendData, int length, uint8_t* pkt)
{
    if ((pkt == nullptr) || (length <= 0)) {
        LOG_ERROR("[HIDH L2CAP] %{public}s:data is null.", __func__);
        HidHostMessage event(HID_HOST_INT_HANDSK_EVT, HID_HOST_HANDSHAKE_ERROR);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
        return;
    }

    Packet *packet = nullptr;
    uint8_t *buf = nullptr;
    uint16_t lcid = 0;

    packet = PacketMalloc(length + HID_HOST_PDU_HEADER_LENGTH, 0, 0);
    buf = (uint8_t *)BufferPtr(PacketHead(packet));
    buf[0] = static_cast<uint8_t>(sendData.type << HID_HOST_SHIFT_OPRATURN_4) | (sendData.param & 0x0f);
    if (memcpy_s(buf + 1, length, pkt, length) != EOK) {
        LOG_ERROR("[HIDH L2CAP] %{public}s:memcpy error.", __func__);
        return;
    }
    lcid = ctrlLcid_;

    L2CIF_SendData(lcid, packet, nullptr);
    PacketFree(packet);
    packet = nullptr;
}

uint16_t HidHostL2capConnection::GetControlLcid()
{
    return ctrlLcid_;
}

uint16_t HidHostL2capConnection::GetInterruptLcid()
{
    return intrLcid_;
}

void HidHostL2capConnection::HidHostL2cConnectControlReqCallback(
    const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);
    std::string address = RawAddress::ConvertToString(addr->addr).GetAddress();
    HidHostL2capConnection* l2capConnect = (HidHostL2capConnection *)context;
    if ((l2capConnect != nullptr) && (result == BT_NO_ERROR)) {
        l2capConnect->ctrlLcid_ = lcid;
    } else {
        LOG_ERROR("[HIDH L2CAP]%{public}s HidHostService is null!", __func__);
    }
}

void HidHostL2capConnection::HidHostL2cConnectInterruptReqCallback
    (const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    std::string address = RawAddress::ConvertToString(addr->addr).GetAddress();
    HidHostL2capConnection* l2capConnect = (HidHostL2capConnection *)context;
    if ((l2capConnect != nullptr) && (result == BT_NO_ERROR)) {
        l2capConnect->intrLcid_ = lcid;
    } else {
        LOG_ERROR("[HIDH L2CAP]%{public}s HidHostService is null!", __func__);
    }
}

void HidHostL2capConnection::HidHostRecvConnectionReqCallback(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    if (info == nullptr) {
        LOG_ERROR("[HIDH L2CAP]%{public}s L2capConnectionInfo is null!", __func__);
        return;
    }
    std::string address = RawAddress::ConvertToString(info->addr.addr).GetAddress();
    HidHostMessage event(HID_HOST_L2CAP_CONNECT_REQ_EVT);
    HidHostL2capConnectionInfo l2capInfo;
    (void)memset_s(&l2capInfo, sizeof(HidHostL2capConnectionInfo),
        0x00, sizeof(HidHostL2capConnectionInfo));
    l2capInfo.lcid = lcid;
    l2capInfo.id = id;
    l2capInfo.lpsm = lpsm;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    HidHostService::GetService()->PostEvent(event);
}

void HidHostL2capConnection::HidHostRecvConnectionRspCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    if (info == nullptr) {
        LOG_ERROR("[HIDH L2CAP]%{public}s L2capConnectionInfo is null!", __func__);
        return;
    }
    std::string address = RawAddress::ConvertToString(info->addr.addr).GetAddress();
    HidHostMessage event(HID_HOST_L2CAP_CONNECT_RSP_EVT);
    HidHostL2capConnectionInfo l2capInfo;
    (void)memset_s(&l2capInfo, sizeof(HidHostL2capConnectionInfo),
        0x00, sizeof(HidHostL2capConnectionInfo));
    l2capInfo.lcid = lcid;
    l2capInfo.result = result;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    HidHostService::GetService()->PostEvent(event);
}

void HidHostL2capConnection::HidHostRecvConfigReqCallback(
    uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    HidHostService *service = HidHostService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[HIDH L2CAP]%{public}s HidHostService is null!", __func__);
        return;
    }
    bool isControlLcid = false;
    std::string address = service->HidHostFindDeviceByLcid(lcid, &isControlLcid);
    if (address.empty()) {
        LOG_ERROR("[HIDH L2CAP]%{public}s can not find device!", __func__);
        L2CIF_ConfigRsp(lcid, id, cfg, L2CAP_REJECTED, nullptr);
        return;
    }

    HidHostMessage event(HID_HOST_L2CAP_CONFIG_REQ_EVT);
    HidHostL2capConnectionInfo l2capInfo;
    (void)memset_s(&l2capInfo, sizeof(HidHostL2capConnectionInfo),
        0x00, sizeof(HidHostL2capConnectionInfo));
    L2capConfigInfo l2capConfigInfo;
    memcpy_s(&l2capConfigInfo, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));

    l2capInfo.lcid = lcid;
    l2capInfo.id = id;
    l2capInfo.cfg = l2capConfigInfo;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void HidHostL2capConnection::HidHostRecvConfigRspCallback(
    uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    HidHostService *service = HidHostService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[HIDH L2CAP]%{public}s HidHostService is null!", __func__);
        return;
    }
    bool isControlLcid = false;
    std::string address = service->HidHostFindDeviceByLcid(lcid, &isControlLcid);
    if (address.empty()) {
        LOG_ERROR("[HIDH L2CAP]%{public}s can not find device!", __func__);
        return;
    }

    HidHostMessage event(HID_HOST_L2CAP_CONFIG_RSP_EVT);
    HidHostL2capConnectionInfo l2capInfo;
    (void)memset_s(&l2capInfo, sizeof(HidHostL2capConnectionInfo),
        0x00, sizeof(HidHostL2capConnectionInfo));
    L2capConfigInfo l2capConfigInfo;
    memcpy_s(&l2capConfigInfo, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));

    l2capInfo.lcid = lcid;
    l2capInfo.result = result;
    l2capInfo.cfg = l2capConfigInfo;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void HidHostL2capConnection::HidHostRecvDisconnectionReqCallback(
    uint16_t lcid, uint8_t id, void *ctx)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    HidHostService *service = HidHostService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[HIDH L2CAP]%{public}s HidHostService is null!", __func__);
        return;
    }
    bool isControlLcid = false;
    std::string address = service->HidHostFindDeviceByLcid(lcid, &isControlLcid);
    if (address.empty()) {
        LOG_ERROR("[HIDH L2CAP]%{public}s can not find device!", __func__);
        return;
    }

    HidHostMessage event(HID_HOST_L2CAP_DISCONNECT_REQ_EVT);
    HidHostL2capConnectionInfo l2capInfo;
    (void)memset_s(&l2capInfo, sizeof(HidHostL2capConnectionInfo),
        0x00, sizeof(HidHostL2capConnectionInfo));

    l2capInfo.lcid = lcid;
    l2capInfo.id = id;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void HidHostL2capConnection::HidHostRecvDisconnectionRspCallback(
    uint16_t lcid, void *ctx)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    HidHostService *service = HidHostService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[HIDH L2CAP]%{public}s HidHostService is null!", __func__);
        return;
    }
    bool isControlLcid = false;
    std::string address = service->HidHostFindDeviceByLcid(lcid, &isControlLcid);
    if (address.empty()) {
        LOG_ERROR("[HIDH L2CAP]%{public}s can not find device!", __func__);
        return;
    }

    HidHostMessage event(HID_HOST_L2CAP_DISCONNECT_RSP_EVT);
    HidHostL2capConnectionInfo l2capInfo;
    (void)memset_s(&l2capInfo, sizeof(HidHostL2capConnectionInfo),
        0x00, sizeof(HidHostL2capConnectionInfo));

    l2capInfo.lcid = lcid;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void HidHostL2capConnection::HidHostDisconnectAbnormalCallback(
    uint16_t lcid, uint8_t reason, void *ctx)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    HidHostService *service = HidHostService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[HIDH L2CAP]%{public}s HidHostService is null!", __func__);
        return;
    }
    bool isControlLcid = false;
    std::string address = service->HidHostFindDeviceByLcid(lcid, &isControlLcid);
    if (address.empty()) {
        LOG_ERROR("[HIDH L2CAP]%{public}s can not find device!", __func__);
        return;
    }

    HidHostMessage event(HID_HOST_L2CAP_DISCONNECT_ABNORMAL_EVT);
    HidHostL2capConnectionInfo l2capInfo;
    (void)memset_s(&l2capInfo, sizeof(HidHostL2capConnectionInfo),
        0x00, sizeof(HidHostL2capConnectionInfo));

    l2capInfo.lcid = lcid;
    l2capInfo.reason = reason;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void HidHostL2capConnection::HidHostRecvDataCallback(
    uint16_t lcid, Packet *pkt, void *ctx)
{
    bool isControlLcid = false;
    std::string address = HidHostService::GetService()->HidHostFindDeviceByLcid(lcid, &isControlLcid);
    if (address.empty()) {
        LOG_ERROR("[HIDH L2CAP]%{public}s can not find device!", __func__);
        return;
    }

    uint8_t header[HID_HOST_PDU_HEADER_LENGTH] = {0};
    int dataLength;
    uint16_t offset = 0;
    HidHostMessage event;

    PacketExtractHead(pkt, header, sizeof(header));
    uint8_t type = HidHostGetType(header[offset]);
    uint8_t param = HidHostGetParam(header[offset]);

    switch (type) {
        case HID_HOST_DATA_TYPE_HANDSHAKE:
            LOG_INFO("[HIDH L2CAP]%{public}s HANDSHAKE lcid:%{public}hu", __func__, lcid);
            event.what_ = HID_HOST_INT_HANDSK_EVT;
            event.arg1_ = param;
            event.dev_ = address;
            HidHostService::GetService()->PostEvent(event);
            break;

        case HID_HOST_DATA_TYPE_CONTROL:
            LOG_INFO("[HIDH L2CAP]%{public}s CONTROL lcid:%{public}hu", __func__, lcid);
            break;

        case HID_HOST_DATA_TYPE_DATA:
        case HID_HOST_DATA_TYPE_DATAC:
            if (!isControlLcid) {
                event.what_ = HID_HOST_INT_DATA_EVT;
            } else {
                LOG_INFO("[HIDH L2CAP]%{public}s CTRL_DATA lcid:%{public}hu", __func__, lcid);
                event.what_ = HID_HOST_INT_CTRL_DATA;
            }
            event.dev_ = address;
            dataLength = static_cast<int>(PacketSize(pkt));
            if (dataLength > 0) {
                event.data_ = std::make_unique<uint8_t[]>(dataLength);
                PacketRead(pkt, event.data_.get(), 0, dataLength);
            }
            event.dataLength_ = dataLength;
            HidHostService::GetService()->PostEvent(event);
            break;
        default:
            LOG_INFO("[HIDH L2CAP]%{public}s other data lcid:%{public}hu", __func__, lcid);
            break;
    }
}

void HidHostL2capConnection::HidHostRemoteBusyCallback(
    uint16_t lcid, uint8_t isBusy, void *ctx)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    HidHostService *service = HidHostService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[HIDH L2CAP]%{public}s HidHostService is null!", __func__);
        return;
    }
    bool isControlLcid = false;
    std::string address = service->HidHostFindDeviceByLcid(lcid, &isControlLcid);
    if (address.empty()) {
        LOG_ERROR("[HIDH L2CAP]%{public}s can not find device!", __func__);
        return;
    }

    HidHostMessage event(HID_HOST_L2CAP_REMOTE_BUSY_EVT);
    HidHostL2capConnectionInfo l2capInfo;
    (void)memset_s(&l2capInfo, sizeof(HidHostL2capConnectionInfo),
        0x00, sizeof(HidHostL2capConnectionInfo));

    l2capInfo.lcid = lcid;
    l2capInfo.isBusy = isBusy;
    event.dev_ = address;
    event.l2capInfo_ = l2capInfo;
    service->PostEvent(event);
}

void HidHostL2capConnection::HidHostRecvConnectionReqCallbackTask(
    uint16_t lcid, uint8_t id, uint16_t lpsm)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    bool isAccept = true;
    if (lpsm == HID_INTERRUPT_PSM) {
        if (ctrlLcid_ == 0) {
            LOG_ERROR("[HIDH L2CAP]%{public}s receive interrupt connect,but control is not connected", __func__);
            isAccept = false;
        }
        if (state_ != HID_HOST_CONN_STATE_CONNECTING_INTR) {
            LOG_ERROR("[HIDH L2CAP]%{public}s receive interrupt connect,but not in connecting state[%{public}d",
                __func__, state_);
            isAccept = false;
        }
    } else {
        // Control channel connect
        if (state_ != HID_HOST_CONN_STATE_UNUSED) {
            LOG_ERROR("[HIDH L2CAP]%{public}s receive control connect,but not in idle state[%{public}d]",
                __func__, state_);
            isAccept = false;
        }
    }
    if (!isAccept) {
        L2CIF_ConnectRsp(lcid, id, L2CAP_NO_RESOURCES_AVAILABLE, 0, nullptr);
        return;
    }

    if (lpsm == HID_INTERRUPT_PSM) {
        state_ = HID_HOST_CONN_STATE_CONFIG;
        intrLcid_ = lcid;
        L2CIF_ConnectRsp(lcid, id, L2CAP_CONNECTION_SUCCESSFUL, 0, nullptr);

        L2capConfigInfo l2capConfigInfo;
        (void)memset_s(&l2capConfigInfo, sizeof(L2capConfigInfo), 0x00, sizeof(L2capConfigInfo));
        l2capConfigInfo.mtu = HID_HOST_MTU;
        l2capConfigInfo.flushTimeout = 0xFFFF;
        if (L2CIF_ConfigReq(lcid, &l2capConfigInfo, nullptr)) {
            LOG_ERROR("[HIDH L2CAP] %{public}s:L2CIF_ConfigReq failed.", __func__);
        }
    } else {
        // Control channel connect
        connFlags_ = 0;

        SendGapRequestSecurity(true, lcid, id);
    }
}

int HidHostL2capConnection::SendGapRequestSecurity(bool isIncoming, uint16_t lcid, uint8_t id)
{
    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    GapRequestSecurityParam param;
    GapServiceSecurityInfo securityInfo;
    param.context = &address_;
    if (isIncoming) {
        securityInfo = {INCOMING, HID_HOST, SEC_PROTOCOL_L2CAP,
                { .l2capPsm = HID_CONTROL_PSM }};
        param.callback = HidHostIndSecurityCheckDeviceConnectCallback;
        param.info = securityInfo;
        if (GAPIF_RequestSecurityAsync((const BtAddr *)&btAddr, &param)) {
            LOG_ERROR("[HIDH L2CAP] %{public}s:GAPIF_RequestSecurityAsync INCOMING failed.", __func__);
            return RET_BAD_STATUS;
        }
        ctrlLcid_ = lcid;
        ctrlId_ = id;
        state_ = HID_HOST_CONN_STATE_SECURITY;
    } else {
        securityInfo = {OUTGOING, HID_HOST, SEC_PROTOCOL_L2CAP,
            { .l2capPsm = HID_CONTROL_PSM }};
        param.callback = HidHostSecurityCheckCallback;
        param.info = securityInfo;
        if (GAPIF_RequestSecurityAsync((const BtAddr *)&btAddr, &param)) {
            LOG_ERROR("[HIDH L2CAP] %{public}s:GAPIF_RequestSecurityAsync OUTGOING failed.", __func__);
            HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
            event.dev_ = address_;
            HidHostService::GetService()->PostEvent(event);
            return RET_BAD_STATUS;
        }
    }
    return BT_NO_ERROR;
}

void HidHostL2capConnection::HidHostRecvConnectionRspCallbackTask(
    uint16_t lcid, int result)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    if (result == L2CAP_CONNECTION_PENDING) {
        LOG_INFO("[HIDH L2CAP]%{public}s: Connect RSP result is pending, do nothing!", __func__);
        return;
    }
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[HIDH L2CAP]%{public}s result[%{public}d]", __func__, result);
        if (lcid == ctrlLcid_) {
            ctrlLcid_ = 0;
        } else {
            intrLcid_ = 0;
        }

        Disconnect();
        return;
    }

    state_ = HID_HOST_CONN_STATE_CONFIG;

    L2capConfigInfo l2capConfigInfo;
    (void)memset_s(&l2capConfigInfo, sizeof(L2capConfigInfo), 0x00, sizeof(L2capConfigInfo));
    l2capConfigInfo.mtu = HID_HOST_MTU;
    l2capConfigInfo.flushTimeout = 0xFFFF;
    if (L2CIF_ConfigReq(lcid, &l2capConfigInfo, nullptr)) {
        LOG_ERROR("[HIDH L2CAP] %{public}s:L2CIF_ConfigReq failed.", __func__);
    }
}

void HidHostL2capConnection::HidHostRecvConfigReqCallbackTask(
    uint16_t lcid, uint8_t id, L2capConfigInfo cfg)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    if (cfg.rfc.mode == L2CAP_ENHANCED_RETRANSMISSION_MODE) {
        LOG_INFO("[HIDH L2CAP]%{public}s for PTS HCT/BI-01-C", __func__);
        L2capConfigInfo l2capConfigInfo;
        (void)memset_s(&l2capConfigInfo, sizeof(L2capConfigInfo), 0x00, sizeof(L2capConfigInfo));
        l2capConfigInfo.mtu = HID_HOST_MTU;
        l2capConfigInfo.flushTimeout = 0xFFFF;
        L2CIF_ConfigRsp(lcid, id, &l2capConfigInfo, L2CAP_UNACCEPTABLE_PARAMETERS, nullptr);
        return;
    }

    if (L2CIF_ConfigRsp(lcid, id, &cfg, 0, nullptr)) {
        LOG_ERROR("[HIDH L2CAP] %{public}s:L2CIF_ConfigRsp failed.", __func__);
    }
    if (lcid == ctrlLcid_) {
        connFlags_ |= HID_HOST_CONN_FLAGS_HIS_CTRL_CFG_DONE;
        if ((connFlags_ & HID_HOST_CONN_FLAGS_MY_CTRL_CFG_DONE) &&
            (connFlags_ & HID_HOST_CONN_FLAGS_IS_ORIG)) {
            int ret = L2CIF_ConnectReq(&btAddr, HID_INTERRUPT_PSM, HID_INTERRUPT_PSM,
                this, HidHostL2cConnectInterruptReqCallback);
            if (ret != BT_NO_ERROR) {
                LOG_ERROR("%{public}s L2CIF_ConnectReq return value is error.", __func__);
                return;
            }
            state_ = HID_HOST_CONN_STATE_CONNECTING_INTR;
        }
    } else {
        connFlags_ |= HID_HOST_CONN_FLAGS_HIS_INTR_CFG_DONE;
        if ((connFlags_ & HID_HOST_CONN_FLAGS_ALL_CONFIGURED) ==
                HID_HOST_CONN_FLAGS_ALL_CONFIGURED) {
            state_ = HID_HOST_CONN_STATE_CONNECTED;
            HidHostMessage event(HID_HOST_INT_OPEN_EVT);
            event.dev_ = address_;
            HidHostService::GetService()->PostEvent(event);
        }
    }
}

void HidHostL2capConnection::HidHostRecvConfigRspCallbackTask(
    uint16_t lcid, const L2capConfigInfo cfg, uint16_t result)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    BtAddr btAddr;
    RawAddress(address_).ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;

    if (result != L2CAP_SUCCESS) {
        LOG_ERROR("[HIDH L2CAP]%{public}s result[%{public}d]", __func__, result);
        Disconnect();
        return;
    }
    if (lcid == ctrlLcid_) {
        connFlags_ |= HID_HOST_CONN_FLAGS_MY_CTRL_CFG_DONE;
        if ((connFlags_ & HID_HOST_CONN_FLAGS_HIS_CTRL_CFG_DONE) &&
            (connFlags_ & HID_HOST_CONN_FLAGS_IS_ORIG)) {
            int ret = L2CIF_ConnectReq(&btAddr, HID_INTERRUPT_PSM, HID_INTERRUPT_PSM,
                this, HidHostL2cConnectInterruptReqCallback);
            if (ret != BT_NO_ERROR) {
                LOG_ERROR("[HIDH L2CAP]%{public}s L2CIF_ConnectReq return value is error.", __func__);
                return;
            }
            state_ = HID_HOST_CONN_STATE_CONNECTING_INTR;
        }
    } else {
        connFlags_ |= HID_HOST_CONN_FLAGS_MY_INTR_CFG_DONE;
        if ((connFlags_ & HID_HOST_CONN_FLAGS_ALL_CONFIGURED) ==
                HID_HOST_CONN_FLAGS_ALL_CONFIGURED) {
            state_ = HID_HOST_CONN_STATE_CONNECTED;
            HidHostMessage event(HID_HOST_INT_OPEN_EVT);
            event.dev_ = address_;
            HidHostService::GetService()->PostEvent(event);
        }
    }
}

void HidHostL2capConnection::HidHostRecvDisconnectionReqCallbackTask(
    uint16_t lcid, uint8_t id)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    L2CIF_DisconnectionRsp(lcid, id, nullptr);
    state_ = HID_HOST_CONN_STATE_DISCONNECTING;

    if (lcid == ctrlLcid_) {
        ctrlLcid_ = 0;
    } else {
        intrLcid_ = 0;
    }

    if ((ctrlLcid_ == 0) && (intrLcid_ == 0)) {
        state_ = HID_HOST_CONN_STATE_UNUSED;
        HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
    }
}

void HidHostL2capConnection::HidHostRecvDisconnectionRspCallbackTask(uint16_t lcid)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);

    if (lcid == ctrlLcid_) {
        ctrlLcid_ = 0;
    } else {
        intrLcid_ = 0;
        if (ctrlLcid_) {
            L2CIF_DisconnectionReq(ctrlLcid_, nullptr);
        }
    }

    if ((ctrlLcid_ == 0) && (intrLcid_ == 0)) {
        state_ = HID_HOST_CONN_STATE_UNUSED;
        HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
    }
}

void HidHostL2capConnection::HidHostDisconnectAbnormalCallbackTask(
    uint16_t lcid, uint8_t reason)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu, reason=%{public}d", __func__, lcid, reason);

    if (lcid == ctrlLcid_) {
        ctrlLcid_ = 0;
    } else {
        intrLcid_ = 0;
    }

    if ((ctrlLcid_ == 0) && (intrLcid_ == 0)) {
        state_ = HID_HOST_CONN_STATE_UNUSED;
        HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
    } else {
        Disconnect();
    }
}

void HidHostL2capConnection::HidHostRemoteBusyCallbackTask(
    uint16_t lcid, uint8_t isBusy)
{
    LOG_INFO("[HIDH L2CAP]%{public}s lcid:%{public}hu", __func__, lcid);
}

uint8_t HidHostL2capConnection::HidHostGetType(uint8_t type)
{
    uint8_t result = 0;
    result = ((type) >> HID_HOST_SHIFT_OPRATURN_4) & 0x0f;
    return result;
}

uint8_t HidHostL2capConnection::HidHostGetParam(uint8_t type)
{
    uint8_t result = 0;
    result = (type)&0x0f;
    return result;
}


void HidHostL2capConnection::ProcessEvent(
    const HidHostMessage &event)
{
    LOG_DEBUG("[HIDH L2CAP]%{public}s(): event %{public}s[%{public}d]",
        __FUNCTION__, HidHostL2capConnection::GetEventName(event.what_).c_str(), event.what_);

    switch (event.what_) {
        case HID_HOST_L2CAP_CONNECT_REQ_EVT:
            HidHostRecvConnectionReqCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.id, event.l2capInfo_.lpsm);
            break;
        case HID_HOST_L2CAP_CONNECT_RSP_EVT:
            HidHostRecvConnectionRspCallbackTask(
                event.l2capInfo_.lcid, event.l2capInfo_.result);
            break;
        case HID_HOST_L2CAP_CONFIG_REQ_EVT:
            HidHostRecvConfigReqCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.id, event.l2capInfo_.cfg);
            break;
        case HID_HOST_L2CAP_CONFIG_RSP_EVT:
            HidHostRecvConfigRspCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.cfg, event.l2capInfo_.result);
            break;
        case HID_HOST_L2CAP_DISCONNECT_REQ_EVT:
            HidHostRecvDisconnectionReqCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.id);
            break;
        case HID_HOST_L2CAP_DISCONNECT_RSP_EVT:
            HidHostRecvDisconnectionRspCallbackTask(event.l2capInfo_.lcid);
            break;
        case HID_HOST_L2CAP_DISCONNECT_ABNORMAL_EVT:
            HidHostDisconnectAbnormalCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.reason);
            break;
        case HID_HOST_L2CAP_REMOTE_BUSY_EVT:
            HidHostRemoteBusyCallbackTask(event.l2capInfo_.lcid, event.l2capInfo_.isBusy);
            break;
        case HID_HOST_L2CAP_SECURITY_RESULT_EVT:
            HidHostSecurityCheckTask(event.arg1_);
            break;
        case HID_HOST_L2CAP_IND_SECURITY_RESULT_EVT:
            HidHostSecurityCheckDeviceConnectTask(event.arg1_);
            break;
        default:
            break;
    }
}

std::string HidHostL2capConnection::GetEventName(int what)
{
    switch (what) {
        case HID_HOST_L2CAP_CONNECT_REQ_EVT:
            return "HID_HOST_L2CAP_CONNECT_REQ_EVT";
        case HID_HOST_L2CAP_CONNECT_RSP_EVT:
            return "HID_HOST_L2CAP_CONNECT_RSP_EVT";
        case HID_HOST_L2CAP_CONFIG_REQ_EVT:
            return "HID_HOST_L2CAP_CONFIG_REQ_EVT";
        case HID_HOST_L2CAP_CONFIG_RSP_EVT:
            return "HID_HOST_L2CAP_CONFIG_RSP_EVT";
        case HID_HOST_L2CAP_DISCONNECT_REQ_EVT:
            return "HID_HOST_L2CAP_DISCONNECT_REQ_EVT";
        case HID_HOST_L2CAP_DISCONNECT_RSP_EVT:
            return "HID_HOST_L2CAP_DISCONNECT_RSP_EVT";
        case HID_HOST_L2CAP_DISCONNECT_ABNORMAL_EVT:
            return "HID_HOST_L2CAP_DISCONNECT_ABNORMAL_EVT";
        case HID_HOST_L2CAP_REMOTE_BUSY_EVT:
            return "HID_HOST_L2CAP_REMOTE_BUSY_EVT";
        case HID_HOST_L2CAP_SECURITY_RESULT_EVT:
            return "HID_HOST_L2CAP_SECURITY_RESULT_EVT";
        case HID_HOST_L2CAP_IND_SECURITY_RESULT_EVT:
            return "HID_HOST_L2CAP_IND_SECURITY_RESULT_EVT";
        default:
            return "Unknown";
    }
}
}  // namespace bluetooth
}  // namespace OHOS