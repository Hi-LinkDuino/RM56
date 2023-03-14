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

#include "map_mse_mnscli.h"
#include <memory>
#include "btm.h"
#include "log.h"
#include "map_mse_params.h"
#include "map_mse_types.h"
#include "securec.h"
#include "stub/map_service.h"

namespace OHOS {
namespace bluetooth {
const uint8_t MapMseMnscli::MNS_TARGET[16] = {
    0xbb, 0x58, 0x2b, 0x41, 0x42, 0x0c, 0x11, 0xdb, 0xb0, 0xde, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66
};
const std::string MapMseMnscli::HEADER_TYPE_EVENT = "x-bt/MAP-event-report";
const uint16_t MapMseMnscli::SERVICE_CLASS_UUID;

MapMseMnscli::MapMseMnscli(utility::Dispatcher &dispatcher, MnsObserver &mnsObserver)
    : dispatcher_(dispatcher), mnsObserver_(mnsObserver)
{
    std::lock_guard<std::recursive_mutex> lock(mnsMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    clientObserver_ = std::make_unique<MseClientObserver>(*this);
    ObexClient::RegisterL2capLPsm(GOEP_L2CAP_LPSM);
}

MapMseMnscli::~MapMseMnscli()
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    obexConfigMap_.clear();
    obexClientMap_.clear();
    connectionIdMap_.clear();
    remoteFeatures_.clear();
}

void MapMseMnscli::DeregisterL2capLPsm(void) const
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    ObexClient::DeregisterL2capLPsm(GOEP_L2CAP_LPSM);
}

uint32_t MapMseMnscli::GetConnectionId(const std::string &address) const
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    uint32_t connectionId = 0;
    if (auto iter = connectionIdMap_.find(address); iter != connectionIdMap_.end()) {
        connectionId = iter->second;
    }
    return connectionId;
}

int MapMseMnscli::Connect(const RawAddress &remoteAddr, int masId)
{
    std::lock_guard<std::recursive_mutex> lock(mnsMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string address = remoteAddr.GetAddress();
    if (masIds_.find(address) == masIds_.end()) {
        masIds_[address] = masId + 1;
    } else {
        masIds_[address] = masIds_[address] + (masId + 1);
    }
    if (connectionIdMap_.find(address) != connectionIdMap_.end()) {
        return Result::SUCCEED;
    }
    if (mnsStateMap_.find(address) != mnsStateMap_.end()) {
        if (mnsStateMap_[address] == BTConnectState::CONNECTING) {
            return Result::SUCCEED;
        }
    }
    mnsStateMap_[address] = BTConnectState::CONNECTING;
    if (auto iter = obexConfigMap_.find(address); iter != obexConfigMap_.end()) {
        if (Result::SUCCEED != RegistGap(remoteAddr, iter->second.isGoepL2capPSM_, iter->second.scn_)) {
            MSE_LOG_INFO("Gap Regist failed.");
            return Result::FAIL;
        }
        auto obexMpClient = std::make_unique<ObexMpClient>(iter->second, *clientObserver_, dispatcher_);
        if (!obexMpClient->Connect()) {
            obexClientMap_[address] = std::move(obexMpClient);
        } else {
            MSE_LOG_INFO("MNS connection failed.");
            return Result::FAIL;
        }
    }
    return Result::SUCCEED;
}

int MapMseMnscli::RegistGap(const RawAddress &remoteAddr, bool isL2cap, uint16_t scn) const
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    BtAddr address;
    address.type = BT_PUBLIC_DEVICE_ADDRESS;
    remoteAddr.ConvertToUint8(address.addr, sizeof(address.addr));
    GapSecChannel gapChannel = {.l2capPsm = scn};
    if (!isL2cap) {
        gapChannel.rfcommChannel = static_cast<uint8_t>(scn);
    }
    GapServiceSecurityInfo serviceInfo = {
        .direction = OUTGOING,
        .serviceId = GAP_Service(MAP_SERVER_ID_START + MAP_MAX_COUNT),
        .protocolId = isL2cap ? GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP
                              : GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM,
        .channelId = gapChannel
    };
    if (GAPIF_RegisterServiceSecurity(&address,
        &serviceInfo,
        GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION)) {
        MSE_LOG_INFO("Call GAP_RegisterServiceSecurity Error");
        return Result::FAIL;
    }
    return Result::SUCCEED;
}

void MapMseMnscli::UnRegistGap(const RawAddress &remoteAddr, bool isL2cap, uint16_t scn) const
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    BtAddr address;
    address.type = BT_PUBLIC_DEVICE_ADDRESS;
    remoteAddr.ConvertToUint8(address.addr, sizeof(address.addr));
    uint8_t rfcommno = 0;
    if (!isL2cap) {
        rfcommno = static_cast<uint8_t>(scn);
        BTM_RemoveRemoteRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, rfcommno, &address);
    } else {
        BTM_RemoveRemoteL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, scn, &address);
    }

    GapServiceSecurityInfo serviceInfo = {
        .direction = OUTGOING,
        .serviceId = GAP_Service(MAP_SERVER_ID_START + MAP_MAX_COUNT),
        .protocolId = isL2cap ? GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP
                              : GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM,
        .channelId = isL2cap ? GapSecChannel{.l2capPsm = scn} : GapSecChannel{.rfcommChannel = rfcommno}
    };
    if (GAPIF_DeregisterServiceSecurity(&address, &serviceInfo)) {
        MSE_LOG_INFO("Call GAP_UnregisterServiceSecurity Error");
    }
}

void MapMseMnscli::DisConnect(const RawAddress &remoteAddr, int masId)
{
    std::lock_guard<std::recursive_mutex> lock(mnsMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string address = remoteAddr.GetAddress();
    if (masIds_.find(address) != masIds_.end()) {
        masIds_[address] = masIds_[address] - (masId + 1);
        if (masIds_[address] == 0) {
            masIds_.erase(address);
        } else {
            return;
        }
    } else {
        return;
    }

    if (mnsStateMap_.find(address) != mnsStateMap_.end()) {
        if (mnsStateMap_[address] == BTConnectState::DISCONNECTING) {
            return;
        }
    }
    if (auto iter = obexClientMap_.find(address); iter != obexClientMap_.end()) {
        mnsStateMap_[address] = BTConnectState::DISCONNECTING;
        if (RET_NO_ERROR != iter->second->Disconnect()) {
            mnsStateMap_[address] = BTConnectState::DISCONNECTED;
        }
    }
    MSE_LOG_INFO("%{public}s End", __PRETTY_FUNCTION__);
}

int MapMseMnscli::SendEvent(const RawAddress &bda, const std::string &report, size_t reportSize, uint8_t masInstanceId)
{
    std::lock_guard<std::recursive_mutex> lock(mnsMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string addr = bda.GetAddress();
    if (mnsStateMap_.find(addr) != mnsStateMap_.end()) {
        if (mnsStateMap_[addr] != BTConnectState::CONNECTED) {
            return Result::FAIL;
        }
    }
    if (auto iter = obexClientMap_.find(addr); iter != obexClientMap_.end()) {
        ObexTlvParamters appPara {};
        std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::PUT_FINAL);
        header->AppendItemConnectionId(GetConnectionId(bda.GetAddress()));
        header->AppendItemSrm(true);
        header->AppendItemType(HEADER_TYPE_EVENT);
        ObexTlvParamters appParams {};
        appParams.AppendTlvtriplet(TlvTriplet(MapMseParams::PARAM_MAS_INSTANCEID, masInstanceId));
        header->AppendItemAppParams(appParams);
        auto bodyObj = std::make_shared<ObexArrayBodyObject>();
        bodyObj->Write((uint8_t *)report.data(), reportSize);
        auto ret = iter->second->Put(*header, bodyObj);
        return ret ? Result::SUCCEED : Result::FAIL;
    }
    return Result::FAIL;
}

void MapMseMnscli::PostCallback(const RawAddress &remoteAddr, ObexClientConfig clientConfig, uint32_t remoteFeature)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    dispatcher_.PostTask(std::bind(&MapMseMnscli::SaveClientConfig, this, remoteAddr, clientConfig, remoteFeature));
}

void MapMseMnscli::SaveClientConfig(const RawAddress &remoteAddr, ObexClientConfig clientConfig, uint32_t remoteFeature)
{
    std::lock_guard<std::recursive_mutex> lock(mnsMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string address = remoteAddr.GetAddress();
    obexConfigMap_[address] = clientConfig;
    remoteFeatures_[address] = remoteFeature;
}

void MapMseMnscli::GetRfcommNo(const SdpService &serviceAry, uint8_t &rfcommNo)
{
    SdpProtocolDescriptor *descriptor = serviceAry.descriptor;
    for (auto descriptorCount = 0; descriptorCount < serviceAry.descriptorNumber; ++descriptorCount, ++descriptor) {
        if (descriptor->protocolUuid.uuid16 == UUID_PROTOCOL_RFCOMM) {
            if (&(descriptor->parameter[0]) != nullptr) {
                rfcommNo = descriptor->parameter[0].value;
                break;
            }
        }
    }
}

void MapMseMnscli::GetVersionNumber(const SdpService &serviceAry, uint16_t &versionNumber)
{
    SdpProfileDescriptor *profileDescriptor = serviceAry.profileDescriptor;
    for (auto profileCount = 0; profileCount < serviceAry.profileDescriptorNumber;
         ++profileCount, ++profileDescriptor) {
        if (profileDescriptor->profileUuid.uuid16 == MAP_PROFILE_DESCRIPTOR_UUID) {
            versionNumber = profileDescriptor->versionNumber;
            break;
        }
    }
}

void MapMseMnscli::GetPsmRemoteFeature(const SdpService &serviceAry, uint16_t &psm, uint32_t &remoteFeature)
{
    SdpAttribute *attribute = serviceAry.attribute;
    for (auto attSeqCount = 0; attSeqCount < serviceAry.attributeNumber; ++attSeqCount, ++attribute) {
        if (attribute->attributeId == GOEP_L2CAP_PSM_ATTRIBUTE_ID) {
            psm = *(uint16_t *)attribute->attributeValue;
        } else if (attribute->attributeId == MAP_SUPPORTED_FEATURES_ATTRIBUTE_ID) {
            remoteFeature = *(uint32_t *)attribute->attributeValue;
        }
    }
}

void MapMseMnscli::SdpSearchCallback(
    const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    uint8_t rfcommNo = 0;
    uint16_t psm = 0;
    uint16_t versionNumber = 0;
    ObexClientConfig obexConfig;
    obexConfig.serviceUUID_.type = BT_UUID_128;
    (void)memcpy_s(
        obexConfig.serviceUUID_.uuid128,
        sizeof(obexConfig.serviceUUID_.uuid128),
        MNS_TARGET,
        sizeof(MNS_TARGET));
    obexConfig.addr_ = *addr;
    if (serviceAry == nullptr) {
        return;
    }
    uint32_t remoteFeature = MAP_SUPPORTED_FEATURES_DEFAULT;
    for (uint16_t serviceCount = 0; serviceCount < serviceNum; ++serviceCount, ++serviceAry) {
        if (serviceAry->classId->uuid16 != SERVICE_CLASS_UUID) {
            continue;
        }
        GetRfcommNo(*serviceAry, rfcommNo);
        GetVersionNumber(*serviceAry, versionNumber);
        GetPsmRemoteFeature(*serviceAry, psm, remoteFeature);
        if (psm != 0) {
#ifdef MAP_MSE_L2CAP_ENABLE
            obexConfig.isGoepL2capPSM_ = true;
#else
            obexConfig.isGoepL2capPSM_ = false;
#endif
            obexConfig.lpsm_ = GOEP_L2CAP_LPSM;
        }
    }
    if (versionNumber != 0 && (psm != 0 || rfcommNo != 0)) {
        if (obexConfig.isGoepL2capPSM_) {
            obexConfig.scn_ = psm;
            obexConfig.mtu_ = OBEX_DEFAULT_MTU;
            BTM_AddRemoteL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, psm, addr);
        } else {
            obexConfig.scn_ = rfcommNo;
            obexConfig.mtu_ = OBEX_DEFAULT_MTU;
            BTM_AddRemoteRfcommScnForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, rfcommNo, addr);
        }
    }
    auto mnsClient = static_cast<MapMseMnscli *>(context);
    mnsClient->PostCallback(RawAddress::ConvertToString(addr->addr), obexConfig, remoteFeature);
}

uint32_t MapMseMnscli::GetRemoteFeatures(const std::string &addr)
{
    std::lock_guard<std::recursive_mutex> lock(mnsMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    uint32_t ret = 0x0;
    if (remoteFeatures_.find(addr) != remoteFeatures_.end()) {
        ret = remoteFeatures_[addr];
    }
    return ret;
}

bool MapMseMnscli::IsDisconnected(void) const
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    bool ret = true;
    for (auto &state : mnsStateMap_) {
        if (BTConnectState::DISCONNECTED != state.second) {
            ret = false;
        }
    }
    return ret;
}

int MapMseMnscli::SdpSearch(const RawAddress &remoteAddr)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (obexConfigMap_.find(remoteAddr.GetAddress()) != obexConfigMap_.end()) {
        return Result::FAIL;
    }
    BtAddr address;
    address.type = BT_PUBLIC_DEVICE_ADDRESS;
    remoteAddr.ConvertToUint8(address.addr, sizeof(address.addr));
    BtUuid classid;
    classid.type = BT_UUID_16;
    classid.uuid16 = SERVICE_CLASS_UUID;
    SdpUuid sdpUUid;
    sdpUUid.uuidNum = 0x0001;
    sdpUUid.uuid = &classid;
    SdpAttributeIdList attributeIdList;
    attributeIdList.type = SDP_TYPE_LIST;
    attributeIdList.attributeIdList.attributeIdNumber = 0x5;
    attributeIdList.attributeIdList.attributeId[0x0] = SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST;
    attributeIdList.attributeIdList.attributeId[0x1] = SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST;
    attributeIdList.attributeIdList.attributeId[0x2] = SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST;
    attributeIdList.attributeIdList.attributeId[0x3] = GOEP_L2CAP_PSM_ATTRIBUTE_ID;
    attributeIdList.attributeIdList.attributeId[0x4] = MAP_SUPPORTED_FEATURES_ATTRIBUTE_ID;
    if (SDP_ServiceSearchAttribute(&address, &sdpUUid, attributeIdList, this, MapMseMnscli::SdpSearchCallback)) {
        MSE_LOG_ERROR("Mns client Call SDP_ServiceSearchAttribute Error");
        return Result::FAIL;
    }
    return Result::SUCCEED;
}

MapMseMnscli::MseClientObserver::MseClientObserver(MapMseMnscli &mnsClient) : mnsClient_(mnsClient)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
}

void MapMseMnscli::MseClientObserver::OnTransportFailed(ObexClient &client, int errCd)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    const RawAddress& remoteAddr = client.GetClientSession().GetRemoteAddr();
    std::string address = remoteAddr.GetAddress();
    {
        std::lock_guard<std::recursive_mutex> lock(mnsClient_.mnsMapMutex_);
        mnsClient_.mnsStateMap_[address] = BTConnectState::DISCONNECTED;
    }
    mnsClient_.RemoveOnConnected(address);
    mnsClient_.mnsObserver_.OnDisconnected();

    if (errCd == CONNECT_COLLISION) {
        mnsClient_.Connect(remoteAddr, mnsClient_.masIds_[address]);
    }
}

void MapMseMnscli::MseClientObserver::OnConnected(ObexClient &client, const ObexHeader &resp)
{
    std::lock_guard<std::recursive_mutex> lock(mnsClient_.mnsMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string address = client.GetClientSession().GetRemoteAddr().GetAddress();
    mnsClient_.mnsStateMap_[address] = BTConnectState::CONNECTED;
    if (resp.GetItemConnectionId()) {
        mnsClient_.connectionIdMap_[address] = resp.GetItemConnectionId()->GetWord();
    }
    stub::MapService::GetInstance()->EventConnected(address);
}

void MapMseMnscli::MseClientObserver::OnConnectFailed(ObexClient &client, const ObexHeader &resp)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    client.Disconnect(false);
}

void MapMseMnscli::MseClientObserver::OnDisconnected(ObexClient &client)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string address = client.GetClientSession().GetRemoteAddr().GetAddress();
    {
        std::lock_guard<std::recursive_mutex> lock(mnsClient_.mnsMapMutex_);
        mnsClient_.mnsStateMap_[address] = BTConnectState::DISCONNECTED;
    }
    mnsClient_.RemoveOnConnected(address);
    mnsClient_.mnsObserver_.OnDisconnected();
    stub::MapService::GetInstance()->EventDisConnected(address);
}

void MapMseMnscli::MseClientObserver::OnActionCompleted(ObexClient &client, const ObexHeader &resp)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    MSE_LOG_INFO("MapMseMnscli : Mns Event sent successfully.");
    std::string address = client.GetClientSession().GetRemoteAddr().GetAddress();
    stub::MapService::GetInstance()->EventSendComplete(address);
}

void MapMseMnscli::RemoveOnConnected(std::string address)
{
    std::lock_guard<std::recursive_mutex> lock(mnsMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (auto iter = connectionIdMap_.find(address); iter != connectionIdMap_.end()) {
        connectionIdMap_.erase(iter);
    }
    if (auto config = obexConfigMap_.find(address); config != obexConfigMap_.end()) {
        UnRegistGap(RawAddress(address), config->second.isGoepL2capPSM_, config->second.scn_);
    }
}
}  // namespace bluetooth
}  // namespace OHOS