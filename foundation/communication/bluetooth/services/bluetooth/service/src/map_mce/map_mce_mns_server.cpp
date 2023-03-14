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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines map mns object.
 *
 */

/**
 * @file map_mce_mns_server.cpp
 *
 * @brief map mns source file .
 *
 */

#include "map_mce_mns_server.h"
#include "btm.h"
#include "log.h"
#include "map_mce_instance_stm.h"
#include "rfcomm.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
MapMceMnsServer::MapMceMnsServer(MapMceService &service, MasInstanceConfig config)
    : mceService_(service), mnsConfig_(config)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    rfcommNo_ = 0;
    sdpHandle_ = 0;
    obexServer_ = nullptr;
    mceMnsObexObserver_ = nullptr;
    goepL2capPsm_ = MCE_MNS_GOEP_L2CAP_PSM_VALUE;
    mnsStatus_ = MAP_MCE_STATE_SHUTDOWN;
}

MapMceMnsServer::~MapMceMnsServer()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    mnsStatus_ = MapMceServiceStateType::MAP_MCE_STATE_SHUTDOWN;
    mceMnsObexObserver_ = nullptr;
}

int MapMceMnsServer::StartUp(void)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    int ret = CreateMasSdpRecord();
    if (ret != RET_NO_ERROR) {
        LOG_ERROR("%{public}s CreateMasSdpRecord error!", __PRETTY_FUNCTION__);
        return RET_NO_SUPPORT;
    }
    ret = RegisterServiceSecurity();
    if (ret != RET_NO_ERROR) {
        DestroyMasSdpRecord();
        LOG_ERROR("%{public}s RegisterServiceSecurity error!", __PRETTY_FUNCTION__);
        return RET_NO_SUPPORT;
    }
    BTM_AddLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, goepL2capPsm_);
    BTM_AddLocalRfcommScnForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, rfcommNo_);
    ObexServerConfig obexSrvConfig;
    obexSrvConfig.useRfcomm_ = true;
    obexSrvConfig.rfcommScn_ = rfcommNo_;
    obexSrvConfig.rfcommMtu_ = mnsConfig_.rfcommMtu;
#ifdef MCE_DISABLE_L2CAP
    obexSrvConfig.useL2cap_ = false;
#else
    obexSrvConfig.useL2cap_ = true;
#endif
    obexSrvConfig.l2capPsm_ = goepL2capPsm_;
    obexSrvConfig.l2capMtu_ = mnsConfig_.l2capMtu;
    obexSrvConfig.isSupportSrm_ = true;
    obexSrvConfig.isSupportReliableSession_ = false;
    mceMnsObexObserver_ = std::make_unique<MceMnsObexServerObserver>(*this);
    obexServer_ = std::make_unique<ObexMpServer>(
        MCE_MNS_SERVICE_NAME, obexSrvConfig, *mceMnsObexObserver_, *mceService_.GetDispatcher());
    ret = obexServer_->Startup();
    mnsStatus_ = MapMceServiceStateType::MAP_MCE_STATE_STARTUP;
    if (ret != BT_NO_ERROR) {
        ret = RET_BAD_STATUS;
        mnsStatus_ = MapMceServiceStateType::MAP_MCE_STATE_SHUTDOWN;
        DestroyMasSdpRecord();
        DeregisterServiceSecurity();
        BTM_RemoveLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, goepL2capPsm_);
        BTM_RemoveLocalRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, rfcommNo_);
        LOG_ERROR("%{public}s obex server startup error!", __PRETTY_FUNCTION__);
    }
    return ret;
}

void MapMceMnsServer::ShutDown(void)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    if (mnsStatus_ == MapMceServiceStateType::MAP_MCE_STATE_STARTUP) {
        obexServer_->Shutdown();
        DeregisterServiceSecurity();
        DestroyMasSdpRecord();
        BTM_RemoveLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, goepL2capPsm_);
        BTM_RemoveLocalRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, rfcommNo_);
        mnsStatus_ = MapMceServiceStateType::MAP_MCE_STATE_SHUTDOWN;
    }
}

int MapMceMnsServer::AddServiceClassIdList(void)
{
    LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    BtUuid btUuid;
    btUuid.type = BT_UUID_16;
    btUuid.uuid16 = MCE_MNS_SERVICE_CLASS_UUID;
    int ret = SDP_AddServiceClassIdList(sdpHandle_, &btUuid, MCE_MNS_SERVICE_CLASS_ID_NUMBER);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("mce mns Call SDP_AddServiceClassIdList Error");
        return RET_NO_SUPPORT;
    }
    return RET_NO_ERROR;
}

int MapMceMnsServer::AddProtocolDescriptorList(void)
{
    int index = 0;
    rfcommNo_ = RFCOMM_AssignServerNum();
    SdpProtocolDescriptor descriptor[MCE_MNS_PROTOCOL_DESCRIPTOR_NUMBER];
    descriptor[index].protocolUuid.type = BT_UUID_16;
    descriptor[index].protocolUuid.uuid16 = UUID_PROTOCOL_L2CAP;
    descriptor[index].parameterNumber = 0;
    index++;
    descriptor[index].protocolUuid.type = BT_UUID_16;
    descriptor[index].protocolUuid.uuid16 = UUID_PROTOCOL_RFCOMM;
    descriptor[index].parameterNumber = MCE_MNS_PROTOCOL_DESCRIPTOR_PARAMETER_NUMBER;
    descriptor[index].parameter[0].type = SDP_TYPE_UINT_8;
    descriptor[index].parameter[0].value = rfcommNo_;
    index++;
    descriptor[index].protocolUuid.type = BT_UUID_16;
    descriptor[index].protocolUuid.uuid16 = UUID_PROTOCOL_OBEX;
    descriptor[index].parameterNumber = 0;

    int ret = SDP_AddProtocolDescriptorList(sdpHandle_, descriptor, MCE_MNS_PROTOCOL_DESCRIPTOR_NUMBER);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("mce mns Call SDP_AddProtocolDescriptorList Error");
        return RET_NO_SUPPORT;
    }
    LOG_INFO("%{public}s execute,rfcomm = %{public}d", __PRETTY_FUNCTION__, int(rfcommNo_));
    return RET_NO_ERROR;
}

int MapMceMnsServer::AddServiceName(void)
{
    LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    int ret = SDP_AddServiceName(
        sdpHandle_, SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE, MCE_MNS_SERVICE_NAME.c_str(), MCE_MNS_SERVICE_NAME.size());
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("mce mns Call SDP_AddServiceName Error");
        return RET_NO_SUPPORT;
    }
    return RET_NO_ERROR;
}

int MapMceMnsServer::AddBluetoothProfileDescriptorList(void)
{
    LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    SdpProfileDescriptor profileDescriptor;
    profileDescriptor.profileUuid.type = BT_UUID_16;
    profileDescriptor.profileUuid.uuid16 = MCE_MNS_PROFILE_DESCRIPTOR_UUID;
    profileDescriptor.versionNumber = MCE_MNS_PROFILE_VERSION;

    int ret = SDP_AddBluetoothProfileDescriptorList(sdpHandle_, &profileDescriptor, MCE_MNS_PROFILE_DESCRIPTOR_NUMBER);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("mce mns Call SDP_AddBluetoothProfileDescriptorList Error");
        return RET_NO_SUPPORT;
    }
    return RET_NO_ERROR;
}

int MapMceMnsServer::AddAttribute(void)
{
    LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    int ret = SDP_AddAttribute(sdpHandle_,
        MCE_MNS_GOEP_L2CAP_PSM_ATTRIBUTE_ID,
        SdpDataType::SDP_TYPE_UINT_16,
        (uint8_t *)&goepL2capPsm_,
        sizeof(uint16_t));
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("map mce mns Call SDP_AddAttribute Error");
        return RET_NO_SUPPORT;
    }
    ret = SDP_AddAttribute(sdpHandle_,
        MCE_MNS_SUPPORTED_FEATURES_ATTRIBUTE_ID,
        SdpDataType::SDP_TYPE_UINT_32,
        (uint8_t *)&MCE_MNS_SUPPORTED_FEATURES_V14,
        sizeof(uint32_t));
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("map mce mns Call SDP_AddAttribute Error");
        return RET_NO_SUPPORT;
    }
    return RET_NO_ERROR;
}

int MapMceMnsServer::RegisterServiceRecord(void)
{
    LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    int ret = SDP_RegisterServiceRecord(sdpHandle_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("mce mns Call SDP_RegisterServiceRecord Error");
        return RET_NO_SUPPORT;
    }
    return RET_NO_ERROR;
}

int MapMceMnsServer::CreateMasSdpRecord(void)
{
    LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    int retVal = RET_NO_ERROR;
    sdpHandle_ = SDP_CreateServiceRecord();
    retVal |= AddServiceClassIdList();
    retVal |= AddProtocolDescriptorList();
    retVal |= AddServiceName();
    retVal |= AddBluetoothProfileDescriptorList();
    retVal |= AddAttribute();
    if (retVal == RET_NO_ERROR) {
        retVal = RegisterServiceRecord();
    } else {
        retVal = RET_NO_SUPPORT;
    }
    return retVal;
}

void MapMceMnsServer::DestroyMasSdpRecord(void)
{
    LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (SDP_DeregisterServiceRecord(sdpHandle_)) {
        LOG_ERROR("Call SDP_DeregisterServiceRecord Error");
    }
    if (RFCOMM_FreeServerNum(rfcommNo_)) {
        LOG_ERROR("Call RFCOMM_FreeServerNum Error");
    }
    if (SDP_DestroyServiceRecord(sdpHandle_)) {
        LOG_ERROR("Call SDP_DestroyServiceRecord Error");
    }
}

int MapMceMnsServer::RegisterServiceSecurity(void) const
{
    LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    // register for l2cap
    GapSecChannel secChannel;
    secChannel.l2capPsm = goepL2capPsm_;
    GapServiceSecurityInfo serviceInfo;
    serviceInfo.direction = INCOMING;
    serviceInfo.serviceId = GAP_Service(MAP_CLIENT_ID_START + MAP_MAX_COUNT);
    serviceInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP;
    serviceInfo.channelId = secChannel;

    int ret = GAPIF_RegisterServiceSecurity(nullptr,
        &serviceInfo,
        GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("map mce mns Call GAP_RegisterServiceSecurity l2cap Error");
        return RET_NO_SUPPORT;
    }
    // register for rfcomm
    secChannel.rfcommChannel = rfcommNo_;
    serviceInfo.channelId = secChannel;
    serviceInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM;

    ret = GAPIF_RegisterServiceSecurity(nullptr,
        &serviceInfo,
        GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("map mce mns Call GAP_RegisterServiceSecurity Error rfcomm");
        return RET_NO_SUPPORT;
    }
    return RET_NO_ERROR;
}

void MapMceMnsServer::DeregisterServiceSecurity(void) const
{
    LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    // deregister for l2cap
    GapSecChannel secChannel;
    secChannel.l2capPsm = goepL2capPsm_;

    GapServiceSecurityInfo serviceInfo;
    serviceInfo.direction = INCOMING;
    serviceInfo.serviceId = GAP_Service(MAP_CLIENT_ID_START + MAP_MAX_COUNT),
    serviceInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP;
    serviceInfo.channelId = secChannel;

    int ret = GAPIF_DeregisterServiceSecurity(nullptr, &serviceInfo);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("MAP mce mns Call GAP_UnregisterServiceSecurity l2cap Error");
    }
    // deregister for rfcomm
    secChannel.rfcommChannel = rfcommNo_;
    serviceInfo.channelId = secChannel;
    serviceInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM;
    ret = GAPIF_DeregisterServiceSecurity(nullptr, &serviceInfo);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("MAP mce mns Call GAPIF_UnregisterServiceSecurity rfcomm Error");
    }
}

MapMceMnsServer::MceMnsObexServerObserver::~MceMnsObexServerObserver()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

void MapMceMnsServer::MceMnsObexServerObserver::OnTransportConnect(ObexIncomingConnect &incomingConnect)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    if (mceMnsServer_.mnsStatus_ == MapMceServiceStateType::MAP_MCE_STATE_STARTUP) {
        incomingConnect.GetRemoteAddress().GetAddress();
        utility::Message outMsg(MSG_MCEDEVICE_MNS_INFO_TRANSPORT_INCOMING);
        outMsg.arg2_ = (void *)(&incomingConnect);
        mceMnsServer_.mceService_.MnsPostMessage(outMsg, incomingConnect.GetRemoteAddress().GetAddress());
    } else {
        incomingConnect.RejectConnection();
    }
}

void MapMceMnsServer::MceMnsObexServerObserver::OnTransportDisconnected(const std::string &btAddr)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    utility::Message outMsg(MSG_MCEDEVICE_MNS_INFO_TRANSPORT_DISCONNECTED);
    mceMnsServer_.mceService_.MnsPostMessage(outMsg, btAddr);
}

void MapMceMnsServer::MceMnsObexServerObserver::OnTransportError(
    const std::string &btAddr, int errCd, const std::string &msg)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    utility::Message outMsg(MSG_MCEDEVICE_MNS_INFO_TRANSPORT_ERROR);
    outMsg.arg1_ = errCd;
    mceMnsServer_.mceService_.MnsPostMessage(outMsg, btAddr);
}

void MapMceMnsServer::MceMnsObexServerObserver::OnConnect(ObexServerSession &session, const ObexHeader &req)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::string btDevice = session.GetRemoteAddr().GetAddress();
    utility::Message outMsg(MSG_MCEDEVICE_MNS_INFO_CONNECT);
    outMsg.arg2_ = (void *)(&session);
    mceMnsServer_.mceService_.MnsPostMessageWithHeader(outMsg, btDevice, req);
}

void MapMceMnsServer::MceMnsObexServerObserver::OnDisconnect(ObexServerSession &session, const ObexHeader &req)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::string btDevice = session.GetRemoteAddr().GetAddress();
    utility::Message outMsg(MSG_MCEDEVICE_MNS_INFO_DISCONNECT);
    outMsg.arg2_ = (void *)(&session);
    mceMnsServer_.mceService_.MnsPostMessageWithHeader(outMsg, btDevice, req);
}

void MapMceMnsServer::MceMnsObexServerObserver::OnPut(ObexServerSession &session, const ObexHeader &req)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::string btDevice = session.GetRemoteAddr().GetAddress();
    utility::Message outMsg(MSG_MCEDEVICE_MNS_INFO_PUT);
    mceMnsServer_.mceService_.MnsPostMessageWithHeader(outMsg, btDevice, req);
}

void MapMceMnsServer::MceMnsObexServerObserver::OnBusy(ObexServerSession &session, bool isBusy) const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::string btDevice = session.GetRemoteAddr().GetAddress();
    utility::Message outMsg(MSG_MCEDEVICE_MNS_INFO_BUSY_STATUS_CHANGE);
    outMsg.arg1_ = int(isBusy);
    outMsg.arg2_ = (void *)(&session);
    mceMnsServer_.mceService_.MnsPostMessage(outMsg, btDevice);
}
}  // namespace bluetooth
}  // namespace OHOS
