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
 * @addtogroup bluetooth
 * @{
 *
 * @brief
 *
 */

/**
 * @file map_mce_device_ctrl.h
 *
 * @brief Declares of map mce service device control.
 *
 */

#include "map_mce_device_ctrl.h"
#include "interface_profile_manager.h"
#include "interface_profile_map_mce.h"
#include "log.h"
#include "map_mce_instance_stm.h"
#include "map_mce_mns_server.h"
#include "power_manager.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
const uint8_t MapMceDeviceCtrl::mnsTargetUuidTbl_[MAX_OF_MASCLIENT_OBEX_UUID_TBL] = {
    0xbb, 0x58, 0x2b, 0x41, 0x42, 0x0c, 0x11, 0xdb, 0xb0, 0xde, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66
};

// sdp call back function declare
int MceAnalyseSdpAttribute(
    MasSdpParam &saveParam, const SdpService &servicePointer, uint16_t &l2capServerNum, bool &findL2capId)
{
    bool findMessageType = false;
    bool findInstanceFeature = false;
    bool findInstance = false;
    int ret = RET_NO_ERROR;

    SdpAttribute *tempAttribute = servicePointer.attribute;
    for (int attSeqCount = 0; attSeqCount < servicePointer.attributeNumber; attSeqCount++, tempAttribute++) {
        if (tempAttribute->attributeId == MAP_MCE_GOEP_L2CAP_PSM_ATTRIBUTE_ID) {
#ifndef MCE_DISABLE_L2CAP
            // l2cap is invalid
            saveParam.isGoepL2capPSM = true;
            findL2capId = true;
            l2capServerNum = *(uint16_t *)tempAttribute->attributeValue;
#endif
        } else if (tempAttribute->attributeId == MAP_MCE_INSTANCE_ATTRIBUTE_ID) {
            saveParam.instanceId = *(uint8_t *)(tempAttribute->attributeValue);
            findInstance = true;
        } else if (tempAttribute->attributeId == MAP_MCE_SUPPORTED_FEATURES_ATTRIBUTE_ID) {
            saveParam.supportedFeatrue = *(uint32_t *)tempAttribute->attributeValue;
            findInstanceFeature = true;
        } else if (tempAttribute->attributeId == MAP_MCE_SUPPORTED_MESSAGE_TYPE_ATTRIBUTE_ID) {
            saveParam.messageType = *(uint8_t *)(tempAttribute->attributeValue);
            findMessageType = true;
        } else {
        }
    }
    if (!findInstanceFeature) {
        // set default value
        saveParam.supportedFeatrue = 0x1F;
        LOG_ERROR("%{public}s: not found supportedFeatrue , set default value=0x1f", __PRETTY_FUNCTION__);
    }
    // check sdp param value
    if ((!findMessageType) || (!findInstance)) {
        // param error
        ret = RET_BAD_PARAM;
        LOG_ERROR("%{public}s param error:inst=%{public}d,type:%{public}d", __PRETTY_FUNCTION__, findInstance, findMessageType);
    }
    return ret;
}

bool MceAnalyseSdpDescriptorRfcomm(SdpProtocolDescriptor &descriptor, uint16_t &serverNum)
{
    bool findRfcommVal = false;
    for (int i = 0; i < descriptor.parameterNumber; i++) {
        if (descriptor.parameter[i].type != 0) {
            serverNum = uint8_t(descriptor.parameter[i].value);
            findRfcommVal = true;
            break;
        }
    }
    return findRfcommVal;
}

int MceAnalyseSdpProtocolDescriptor(
    MasSdpParam &saveParam, const SdpService &servicePointer, uint16_t &rfcommServerNum, bool findL2capId)
{
    bool findRfcommId = false;
    bool findRfcommValue = false;
    int ret = RET_NO_ERROR;

    // sdp protocol descriptor analyse
    SdpProtocolDescriptor *tempDescriptor = servicePointer.descriptor;
    for (int descriptorCount = 0; descriptorCount < servicePointer.descriptorNumber;
         descriptorCount++, tempDescriptor++) {
        // if l2cap valid , scn_ is 1;
        if (tempDescriptor->protocolUuid.uuid16 == UUID_PROTOCOL_L2CAP) {  // MCE_PROTOCOL_DESCRIPTOR_L2CAP_UUID
            LOG_INFO("%{public}s find UUID_PROTOCOL_L2CAP", __PRETTY_FUNCTION__);
        } else if (tempDescriptor->protocolUuid.uuid16 == UUID_PROTOCOL_RFCOMM) {
            findRfcommId = true;
            findRfcommValue = MceAnalyseSdpDescriptorRfcomm(*tempDescriptor, rfcommServerNum);
            LOG_INFO("%{public}s find UUID_PROTOCOL_RFCOMM", __PRETTY_FUNCTION__);
        }
    }

    if ((findRfcommId && (!findRfcommValue)) || !(findL2capId || findRfcommId)) {
        LOG_ERROR("%{public}s param error:l2id=%{public}d,rf=%{public}d,%{public}d", __PRETTY_FUNCTION__, findL2capId, findRfcommId, findRfcommValue);
        ret = RET_BAD_PARAM;
    }
    return ret;
}

int MceAnalyseSdpParam(MasSdpParam &saveParam, const SdpService &servicePointer)
{
    uint16_t rfcommServerNum = 0;
    uint16_t l2capServerNum = 0;
    bool findL2capId = false;
    int ret;

    // Start sdp analyse
    saveParam.isGoepL2capPSM = false;

    ret = MceAnalyseSdpAttribute(saveParam, servicePointer, l2capServerNum, findL2capId);
    if (ret != RET_NO_ERROR) {
        return ret;
    }
    ret = MceAnalyseSdpProtocolDescriptor(saveParam, servicePointer, rfcommServerNum, findL2capId);
    if (ret != RET_NO_ERROR) {
        return ret;
    }
    // get rfcomm
    if (saveParam.isGoepL2capPSM) {
        saveParam.scn = l2capServerNum;
    } else {
        saveParam.scn = rfcommServerNum;
    }
    // get service name
    if (servicePointer.serviceName != nullptr) {
        saveParam.serviceName = servicePointer.serviceName;
    } else {
        saveParam.serviceName = "";
    }
    LOG_INFO("%{public}s execute finish:isL2cap=%{public}d,scn=0x%x,msgType=%{public}d,gapNumb=%{public}d,servicName=%{public}s",
        __PRETTY_FUNCTION__,
        saveParam.isGoepL2capPSM,
        saveParam.scn,
        saveParam.messageType,
        saveParam.gapNumber,
        saveParam.serviceName.c_str());
    return ret;
}

void MapMceSdpSearchCb(
    const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, int finishId, int failedId)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    GAP_Service gapCounter = MAP_CLIENT_ID_START;
    IProfileManager *serviceMgr = IProfileManager::GetInstance();
    if (serviceMgr == nullptr) {
        return;
    }
    auto mceService = static_cast<MapMceService *>(serviceMgr->GetProfileService(PROFILE_NAME_MAP_MCE));
    if (mceService == nullptr) {
        LOG_ERROR("%{public}s mceService is nullptr", __PRETTY_FUNCTION__);
        return;
    }
    MapSdpMsgArgPrt *argPtr = new (std::nothrow)MapSdpMsgArgPrt;
    if (argPtr == nullptr) {
        LOG_ERROR("%{public}s argPtr is nullptr", __PRETTY_FUNCTION__);
        return;
    }
    MasSdpParam sdpSaveParam;
    // check servcie array
    utility::Message msg(failedId);
    argPtr->address = *addr;
    msg.arg2_ = (void *)argPtr;
    if ((serviceAry == nullptr) || (serviceNum == 0)) {
        mceService->PostMessage(msg);
        LOG_ERROR("%{public}s service error,serviceAry is NUL or serviceNum =%{public}d", __PRETTY_FUNCTION__, int(serviceNum));
        return;
    }
    const SdpService *servPtr = serviceAry;
    for (int serviceCount = 0; serviceCount < serviceNum; serviceCount++, servPtr++) {
        // check param array
        if ((servPtr->attribute == nullptr) || (servPtr->attributeNumber == 0) || (servPtr->descriptor == nullptr) ||
            (servPtr->descriptorNumber == 0)) {
            mceService->PostMessage(msg);
            LOG_ERROR("%{public}s error:attrPtr is NULL or attrNum=%{public}d", __PRETTY_FUNCTION__, servPtr->attributeNumber);
            LOG_ERROR("%{public}s error:script is NULL or script Num=%{public}d", __PRETTY_FUNCTION__, servPtr->descriptorNumber);
            return;
        }
        // Start sdp analyse
        sdpSaveParam.gapNumber = gapCounter;
        int ret = MceAnalyseSdpParam(sdpSaveParam, *servPtr);
        // check sdp param value
        if (ret != RET_NO_ERROR) {
            // param error
            mceService->PostMessage(msg);
            return;
        }
        // make sdp param list
        argPtr->masSdpParamListPrt.push_back(sdpSaveParam);
        gapCounter = (GAP_Service)(gapCounter + 1);  // for gap regester
    }
    // Find stm instance
    msg.what_ = finishId;
    mceService->PostMessage(msg);
}

void MapMceServiceSdpSearchCb(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);

    // sdp value display
    MapMceSdpSearchCb(
        addr, serviceAry, serviceNum, MSG_MCEDEVICE_SDP_GET_INSTANCE_FINISH, MSG_MCEDEVICE_SDP_GET_INSTANCE_FAILED);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

MapMceDeviceCtrl::MapMceDeviceCtrl(const std::string &device, MapMceService &service, bool notifySet,
    MasInstanceConfig config, MapMceObserverManager &callback)
    : btDevice_(device), devService_(service), notificationRegistration_(notifySet), deviceRpcCallbackMgr_(callback)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    btDeviceState_ = MAP_MCE_DEV_STATE_DISCONNECTED;
    btDeviceTargetState_ = MAP_MCE_DEV_STATE_CONNECTED;
    deviceInstanceStmMap_.clear();
    mnsServerStatus_ = MAP_MCE_DEV_STATE_DISCONNECTED;
    mnsObexSession_ = nullptr;
    mnsSessionObexheader_ = nullptr;
    supportedFeatures_ = 0;
    insDefaultConfig_ = config;
    connectId_ = config.deviceId << MCE_16BIT;
    ctrlSendFlag_ = false;
    ctrlBusyStatus_ = MAP_MCE_PWBUSY_STATUS_DISCONNECT;
    mnsContinueBusyFlag_ = false;
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

MapMceDeviceCtrl::~MapMceDeviceCtrl()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceMapMutex_);
    // check iterator in map
    deviceInstanceStmMap_.clear();
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

int MapMceDeviceCtrl::StartConnecting()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // check sdp
    RawAddress rawAddr(btDevice_);
    int ret = GetSupportedMasInstances(rawAddr);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

void MapMceDeviceCtrl::AllInstancesStartConnecting()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg(MSG_MASSTM_REQ_CONNECT);
    for (auto &it : deviceInstanceStmMap_) {
        TransClientState(MAP_MCE_DEV_STATE_CONNECTING);
        static_cast<MapMceInstanceStm *>(it.second.get())->ProcessMessage(msg);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

bool MapMceDeviceCtrl::AllInstancesStartDisConnecting()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // disconnect the instance client
    bool finish = true;
    utility::Message msg(MSG_MASSTM_REQ_DISCONNECT);
    for (auto &it : deviceInstanceStmMap_) {
        auto stm = static_cast<MapMceInstanceStm *>(it.second.get());
        if (stm->GetCurrentMceStmStateName() != MCE_DISCONNECTED_STATE) {
            stm->ProcessMessage(msg);
            finish = false;
        }
    }
    // disconnect the mns server
    if (GetMnsState() != MAP_MCE_DEV_STATE_DISCONNECTED) {
        MnsProcRequestDisconnect();
        finish = false;
    }

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return finish;
}

// call by service
int MapMceDeviceCtrl::PostMessageWithRequest(utility::Message msg, std::unique_ptr<MapMceInstanceRequest> &req)
{
    LOG_INFO("%{public}s enter,msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceMapMutex_);
    int ret = RET_BAD_STATUS;
    if (!ctrlSendFlag_) {
        ctrlSendFlag_ = true;
        ctrlRequestPtr_ = std::move(req);
        devService_.GetDispatcher()->PostTask(std::bind(&MapMceDeviceCtrl::ProcessMessage, this, msg));
        ret = RET_NO_ERROR;
        LOG_INFO("%{public}s ctrlSendFlag_ set", __PRETTY_FUNCTION__);
    } else {
        LOG_ERROR("%{public}s ctrlSendFlag_ is true", __PRETTY_FUNCTION__);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

void MapMceDeviceCtrl::PostMessage(utility::Message msg)
{
    LOG_INFO("%{public}s execute,msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    devService_.GetDispatcher()->PostTask(std::bind(&MapMceDeviceCtrl::ProcessMessage, this, msg));
}

MapMceDeviceStateType MapMceDeviceCtrl::GetCurrentDeviceState()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceMapMutex_);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return MapMceDeviceStateType(int(btDeviceState_));
}

MapMceDeviceStateType MapMceDeviceCtrl::GetTargetDeviceState() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MapMceDeviceStateType(int(btDeviceTargetState_));
}

// call by service
std::vector<IProfileMasInstanceInfo> MapMceDeviceCtrl::GetMasInstanceInfo(const RawAddress &device)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceMapMutex_);

    std::vector<IProfileMasInstanceInfo> instancInfoList;
    IProfileMasInstanceInfo profileInfo;
    MasInstanceInformation deviceInfo;
    MasSdpParam sdpInfo;
    for (auto &it : deviceInstanceStmMap_) {
        auto stm = static_cast<MapMceInstanceStm *>(it.second.get());
        deviceInfo = stm->GetMasClient().GetMasInstanceInformation();
        sdpInfo = stm->GetMasClient().GetMasSdpParam();
        profileInfo.OwnerUCI = deviceInfo.ownerUciUtf8;
        profileInfo.MASInstanceInformation = deviceInfo.masInstanceInfoUtf8;
        profileInfo.supportedMsgTypes_ = sdpInfo.messageType;
        profileInfo.instanceId = sdpInfo.instanceId;
        if (deviceInfo.isValid) {
            instancInfoList.push_back(profileInfo);
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return instancInfoList;
}

int MapMceDeviceCtrl::DeviceStateConvertToProfileState(MapMceDeviceStateType deviceState)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret = PROFILE_STATE_DISCONNECTED;
    switch (deviceState) {
        case MAP_MCE_DEV_STATE_CONNECTED:
            ret = PROFILE_STATE_CONNECTED;
            break;
        case MAP_MCE_DEV_STATE_CONNECTING:
            ret = PROFILE_STATE_CONNECTING;
            break;
        case MAP_MCE_DEV_STATE_DISCONNECTED:
            ret = PROFILE_STATE_DISCONNECTED;
            break;
        case MAP_MCE_DEV_STATE_DISCONNECTING:
            ret = PROFILE_STATE_DISCONNECTING;
            break;
        default:
            // error
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// call by service
std::string MapMceDeviceCtrl::GetBtDevice()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceMapMutex_);

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return btDevice_;
}

BtAddr MapMceDeviceCtrl::GetBtAddress()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceMapMutex_);

    BtAddr btAddress;
    btAddress.type = BT_PUBLIC_DEVICE_ADDRESS;
    RawAddress rawAddress(btDevice_);
    rawAddress.ConvertToUint8(btAddress.addr, sizeof(btAddress.addr));
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return btAddress;
}

// call by stm
MapMceObserverManager &MapMceDeviceCtrl::GetDeviceCallbackMgr() const
{
    return deviceRpcCallbackMgr_;
}

void MapMceDeviceCtrl::TransClientState(MapMceDeviceStateType state)
{
    if (btDeviceState_ != state) {
        btDeviceState_ = state;
        deviceRpcCallbackMgr_.ExcuteObserverOnConnectionStateChanged(btDevice_, state);
        LOG_INFO("%{public}s to state:%{public}d", __PRETTY_FUNCTION__, state);

        // set power busy state
        if (btDeviceState_ == MAP_MCE_DEV_STATE_CONNECTED) {
            TransPowerBusyState(MAP_MCE_PWBUSY_STATUS_CONNECT);
        } else if (btDeviceState_ == MAP_MCE_DEV_STATE_DISCONNECTED) {
            TransPowerBusyState(MAP_MCE_PWBUSY_STATUS_DISCONNECT);
        } else {
        }
    }
}

void MapMceDeviceCtrl::TransMnsState(MapMceDeviceStateType state)
{
    if (mnsServerStatus_ != state) {
        mnsServerStatus_ = state;
        LOG_INFO("%{public}s to state:%{public}d", __PRETTY_FUNCTION__, state);
    }
}

// internal
MapMceDeviceStateType MapMceDeviceCtrl::GetMnsState() const
{
    return MapMceDeviceStateType(int(mnsServerStatus_));
}

// call by service
int MapMceDeviceCtrl::GetDeviceInstanseStmMapSize()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceMapMutex_);

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return int(deviceInstanceStmMap_.size());
}

// internal
int MapMceDeviceCtrl::GetSupportedMasInstances(const RawAddress &device)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // config search attribute
    SdpAttributeIdList attributeIdList = {SDP_TYPE_LIST, .attributeIdList = {0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};
    int index = 0;
    attributeIdList.attributeIdList.attributeId[index++] = MAP_MCE_GOEP_L2CAP_PSM_ATTRIBUTE_ID;
    attributeIdList.attributeIdList.attributeId[index++] = MAP_MCE_INSTANCE_ATTRIBUTE_ID;
    attributeIdList.attributeIdList.attributeId[index++] = MAP_MCE_SUPPORTED_FEATURES_ATTRIBUTE_ID;
    attributeIdList.attributeIdList.attributeId[index++] = SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST;
    attributeIdList.attributeIdList.attributeId[index++] = MAP_MCE_SUPPORTED_MESSAGE_TYPE_ATTRIBUTE_ID;
    attributeIdList.attributeIdList.attributeId[index++] = SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE;
    attributeIdList.attributeIdList.attributeIdNumber = index;
    attributeIdList.type = SDP_TYPE_LIST;
    // set uuid
    BtUuid btUuid = {0, {.uuid128 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};
    btUuid.type = BT_UUID_16;
    btUuid.uuid16 = MAP_MCE_SERVICE_CLASS_UUID;

    SdpUuid sdpUuid = {0, nullptr};
    sdpUuid.uuid = &btUuid;
    sdpUuid.uuidNum = 1;

    // get the bluetooth address
    BtAddr btAddr = {{0, 0, 0, 0, 0, 0}, 0};
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    RawAddress rawAddr(device.GetAddress());
    rawAddr.ConvertToUint8(btAddr.addr, sizeof(btAddr.addr));

    // sdp Search
    int retVal = SDP_ServiceSearchAttribute(&btAddr, &sdpUuid, attributeIdList, nullptr, MapMceServiceSdpSearchCb);
    if (retVal != BT_NO_ERROR) {
        LOG_ERROR("%{public}s error:SDP_ServiceSearchAttribute", __PRETTY_FUNCTION__);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return retVal;
}

void MapMceDeviceCtrl::ProcessMessageStateProcess(utility::Message msg)
{
    // mas client state process
    switch (btDeviceState_) {
        case MAP_MCE_DEV_STATE_CONNECTED:
            ClientConnectedProcMsg(msg);
            break;
        case MAP_MCE_DEV_STATE_CONNECTING:
            ClientConnectingProcMsg(msg);
            break;
        case MAP_MCE_DEV_STATE_DISCONNECTED:
            ClientDisconnectedProcMsg(msg);
            break;
        case MAP_MCE_DEV_STATE_DISCONNECTING:
            ClientDisConnectingProcMsg(msg);
            break;
        default:
            break;
    }
    // mns state process
    switch (mnsServerStatus_) {
        case MAP_MCE_DEV_STATE_CONNECTED:
            MnsConnectedProcMsg(msg);
            break;
        case MAP_MCE_DEV_STATE_CONNECTING:
            MnsConnectingProcMsg(msg);
            break;
        case MAP_MCE_DEV_STATE_DISCONNECTED:
            MnsDisconnectedProcMsg(msg);
            break;
        case MAP_MCE_DEV_STATE_DISCONNECTING:
            MnsDisconnectingProcMsg(msg);
            break;
        default:
            break;
    }
}

// main process
void MapMceDeviceCtrl::ProcessMessage(utility::Message msg)
{
    LOG_INFO("%{public}s enter,input msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceMapMutex_);

    // preprocess
    switch (msg.what_) {
        case MSG_MCEDEVICE_REQ_DEVICE_DISCONNECT:
            btDeviceTargetState_ = MapMceDeviceStateType::MAP_MCE_DEV_STATE_DISCONNECTED;
            LOG_INFO("%{public}s targstate=%{public}d", __PRETTY_FUNCTION__, int(btDeviceTargetState_));
            break;
        case MSG_MCEDEVICE_REQ_DEVICE_CONNECT:
            btDeviceTargetState_ = MapMceDeviceStateType::MAP_MCE_DEV_STATE_CONNECTED;
            LOG_INFO("%{public}s targstate=%{public}d", __PRETTY_FUNCTION__, int(btDeviceTargetState_));
            break;
        default:
            break;
    }
    // process message
    ProcessMessageStateProcess(msg);
    // back process
    if (msg.what_ == MSG_MCEDEVICE_REQ_SEND_REQUEST) {
        ctrlSendFlag_ = false;  // release the request resource
        LOG_INFO("%{public}s ctrlSendFlag_ clear", __PRETTY_FUNCTION__);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceDeviceCtrl::ProcessMnsObexObserverMessage(const ObexHeader &req, utility::Message msg)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    MnsChangeSessionObexHeader(req);
    ProcessMessage(msg);
}

void MapMceDeviceCtrl::ClientConnectedProcMsg(utility::Message msg)
{
    LOG_INFO("%{public}s enter,input msg=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    utility::Message outMsg(MSG_MCESERVICE_DEVICE_DISCONNECTED);
    switch (msg.what_) {
        case MSG_MCEDEVICE_REQ_DEVICE_DISCONNECT:
            TransClientState(MAP_MCE_DEV_STATE_DISCONNECTING);
            AllInstancesStartDisConnecting();
            break;
        case MSG_MCEDEVICE_REQ_DEVICE_CONNECT:
            break;
        case MSG_MCEDEVICE_SDP_GET_INSTANCE_FINISH:
            break;
        case MSG_MCEDEVICE_SDP_GET_INSTANCE_FAILED:
            break;
        case MSG_MCEDEVICE_INSCLIENT_DISCONNECTED:
        case MSG_MCEDEVICE_INSCLIENT_TRANSPORT_ERRO:
            // server disconnect or transport error
            // check disconnect finish
            TransClientState(MAP_MCE_DEV_STATE_DISCONNECTING);
            btDeviceTargetState_ = MAP_MCE_DEV_STATE_DISCONNECTED;
            if (AllInstancesStartDisConnecting()) {
                // had disconnected
                TransClientState(MAP_MCE_DEV_STATE_DISCONNECTED);
                // all disconnected ,info disconnect to service
                outMsg.what_ = MSG_MCESERVICE_DEVICE_DISCONNECTED;
                devService_.PostMessage(outMsg);
            }
            break;
        case MSG_MCEDEVICE_INSCLIENT_CONNECTED:
            break;
        case MSG_MCEDEVICE_INSCLIENT_CONNECT_FAILED:
            break;
        case MSG_MCEDEVICE_REQ_SEND_REQUEST:
            SendRequest(msg);
            break;
        case MSG_MCEDEVICE_INSCLIENT_INFO_BUSY_STATUS_CHANGE:
        case MSG_MCEDEVICE_MNS_INFO_BUSY_STATUS_CHANGE:
            CheckPowerBusyStateChange();
            break;
        default:
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceDeviceCtrl::ClientConnectingProcMsg(utility::Message msg)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret = RET_NO_ERROR;
    utility::Message outMsg(MSG_MCESERVICE_DEVICE_DISCONNECTED);
    switch (msg.what_) {
        case MSG_MCEDEVICE_REQ_DEVICE_DISCONNECT:
            break;
        case MSG_MCEDEVICE_REQ_DEVICE_CONNECT:
            break;
        case MSG_MCEDEVICE_SDP_GET_INSTANCE_FINISH:
            ret = ProcessGetInstance(msg);
            if (ret == RET_NO_ERROR) {
                AllInstancesStartConnecting();
            } else {
                // trans to disconnect
                btDeviceTargetState_ = MAP_MCE_DEV_STATE_DISCONNECTED;
                TransClientState(MAP_MCE_DEV_STATE_DISCONNECTED);
                // all disconnected ,info disconnect to service
                outMsg.what_ = MSG_MCESERVICE_DEVICE_DISCONNECTED;
                devService_.PostMessage(outMsg);
            }
            break;
        case MSG_MCEDEVICE_SDP_GET_INSTANCE_FAILED:
            // connect failed
            // set disconnect state
            TransClientState(MAP_MCE_DEV_STATE_DISCONNECTED);
            btDeviceTargetState_ = MAP_MCE_DEV_STATE_DISCONNECTED;
            // connecting error ,info disconnect to service
            outMsg.what_ = MSG_MCESERVICE_DEVICE_DISCONNECTED;
            devService_.PostMessage(outMsg);
            break;
        case MSG_MCEDEVICE_INSCLIENT_CONNECTED:
            ProcessConnected();
            break;
        case MSG_MCEDEVICE_INSCLIENT_DISCONNECTED:    // server req disconnect
        case MSG_MCEDEVICE_INSCLIENT_TRANSPORT_ERRO:  // transport error
        case MSG_MCEDEVICE_INSCLIENT_CONNECT_FAILED:  // connect failed
        case MSG_MCEDEVICE_MNS_DEVICE_DISCONNECTED_FINISH:
            LOG_INFO("%{public}s connecting failed!", __PRETTY_FUNCTION__);
            // disconnect all instance
            TransClientState(MAP_MCE_DEV_STATE_DISCONNECTING);
            btDeviceTargetState_ = MAP_MCE_DEV_STATE_DISCONNECTED;
            if (AllInstancesStartDisConnecting()) {
                TransClientState(MAP_MCE_DEV_STATE_DISCONNECTED);
                // all disconnected ,info disconnect to service
                outMsg.what_ = MSG_MCESERVICE_DEVICE_DISCONNECTED;
                devService_.PostMessage(outMsg);
            }
            break;
        default:
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceDeviceCtrl::ClientDisconnectedProcMsg(utility::Message msg)
{
    int ret;
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    switch (msg.what_) {
        case MSG_MCEDEVICE_REQ_DEVICE_DISCONNECT:
            break;
        case MSG_MCEDEVICE_REQ_DEVICE_CONNECT:
            // device connecting start
            ret = StartConnecting();
            TransClientState(MAP_MCE_DEV_STATE_CONNECTING);
            if (ret != BT_NO_ERROR) {
                LOG_INFO("%{public}s sdp error", __PRETTY_FUNCTION__);
            }
            break;
        case MSG_MCEDEVICE_SDP_GET_INSTANCE_FINISH:
            break;
        case MSG_MCEDEVICE_SDP_GET_INSTANCE_FAILED:
            break;
        case MSG_MCEDEVICE_INSCLIENT_DISCONNECTED:
            break;
        case MSG_MCEDEVICE_INSCLIENT_CONNECTED:
            break;
        case MSG_MCEDEVICE_INSCLIENT_CONNECT_FAILED:
            break;
        case MSG_MCEDEVICE_INSCLIENT_TRANSPORT_ERRO:
            break;

        default:
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceDeviceCtrl::ClientDisConnectingProcMsg(utility::Message msg)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    switch (msg.what_) {
        case MSG_MCEDEVICE_REQ_DEVICE_DISCONNECT:
            break;
        case MSG_MCEDEVICE_REQ_DEVICE_CONNECT:
            break;
        case MSG_MCEDEVICE_SDP_GET_INSTANCE_FINISH:
            break;
        case MSG_MCEDEVICE_SDP_GET_INSTANCE_FAILED:
            break;
        case MSG_MCEDEVICE_INSCLIENT_CONNECTED:
            break;
        case MSG_MCEDEVICE_INSCLIENT_CONNECT_FAILED:
        case MSG_MCEDEVICE_INSCLIENT_TRANSPORT_ERRO:
        case MSG_MCEDEVICE_INSCLIENT_DISCONNECTED:
        case MSG_MCEDEVICE_MNS_DEVICE_DISCONNECTED_FINISH:
            // check disconnect finish
            ProcessDisconnected();
            break;
        default:
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

int MapMceDeviceCtrl::ProcessGetInstance(const utility::Message &msg)
{
    LOG_INFO("%{public}s enter,insmod=%{public}d", __PRETTY_FUNCTION__, insDefaultConfig_.singleInstMode);
    LOG_INFO("%{public}s ,instance id=%{public}d", __PRETTY_FUNCTION__, insDefaultConfig_.singleInstanceId);
    int ret = RET_BAD_STATUS;
    MasSdpParam sdpParam;
    auto argPrt = static_cast<MapSdpMsgArgPrt *>(msg.arg2_);
    if (argPrt == nullptr) {
        LOG_ERROR("%{public}s error argPrt NULL", __PRETTY_FUNCTION__);
        return ret;
    }
    // remove last instance
    deviceInstanceStmMap_.clear();
    // init supported features
    supportedFeatures_ = 0;
    // insert new instance
    for (auto it = argPrt->masSdpParamListPrt.begin(); it != argPrt->masSdpParamListPrt.end(); it++) {
        sdpParam = *it;
        if (insDefaultConfig_.singleInstMode) {
            if (insDefaultConfig_.singleInstanceId == sdpParam.instanceId) {
                ret = RET_NO_ERROR;
                auto stateMachine1 = std::make_unique<MapMceInstanceStm>(
                    *this, *(devService_.GetDispatcher()), sdpParam.instanceId, insDefaultConfig_);
                stateMachine1->CreateStm();
                stateMachine1->GetMasClient().SetDefaultSdpParam(sdpParam);
                deviceInstanceStmMap_.insert(std::pair<const int, std::unique_ptr<utility::StateMachine>>(
                    sdpParam.instanceId, std::move(stateMachine1)));
                // save support features
                supportedFeatures_ |= sdpParam.supportedFeatrue;
                break;
            }
        } else {
            ret = RET_NO_ERROR;
            auto stateMachine = std::make_unique<MapMceInstanceStm>(
                *this, *(devService_.GetDispatcher()), sdpParam.instanceId, insDefaultConfig_);
            stateMachine->CreateStm();
            stateMachine->GetMasClient().SetDefaultSdpParam(sdpParam);
            deviceInstanceStmMap_.insert(std::pair<const int, std::unique_ptr<utility::StateMachine>>(
                sdpParam.instanceId, std::move(stateMachine)));
            // save support features
            supportedFeatures_ |= sdpParam.supportedFeatrue;
        }
    }
    LOG_INFO("%{public}s end,supportedFeatures_=%x", __PRETTY_FUNCTION__, supportedFeatures_);
    return ret;
}

void MapMceDeviceCtrl::ProcessDisconnected()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int notfinish = false;
    for (auto &it : deviceInstanceStmMap_) {
        auto stm = static_cast<MapMceInstanceStm *>(it.second.get());
        if (stm->GetCurrentMceStmStateName() != MCE_DISCONNECTED_STATE) {
            notfinish = true;
        }
    }
    // check mns server disconnect
    if (GetMnsState() != MAP_MCE_DEV_STATE_DISCONNECTED) {
        notfinish = true;
        // disconnect the mns server
        if (GetMnsState() == MAP_MCE_DEV_STATE_CONNECTED) {
            MnsProcRequestDisconnect();
        }
    }
    if (!notfinish) {
        TransClientState(MAP_MCE_DEV_STATE_DISCONNECTED);
        if (btDeviceTargetState_ == MAP_MCE_DEV_STATE_CONNECTED) {
            // request connect;
            utility::Message outMsg(MSG_MCEDEVICE_REQ_DEVICE_CONNECT);
            PostMessage(outMsg);
        } else {
            utility::Message msg(MSG_MCESERVICE_DEVICE_DISCONNECTED);
            devService_.PostMessage(msg);
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceDeviceCtrl::ProcessConnected()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int notfinish = false;
    for (auto &it : deviceInstanceStmMap_) {
        auto stm = static_cast<MapMceInstanceStm *>(it.second.get());
        std::string instanceState = stm->GetCurrentMceStmStateName();
        if ((instanceState == MCE_DISCONNECTED_STATE) || (instanceState == MCE_DISCONNECTING_STATE) ||
            (instanceState == MCE_CONNECTING_STATE)) {
            notfinish = true;
        }
    }

    if (!notfinish) {
        TransClientState(MAP_MCE_DEV_STATE_CONNECTED);
        // process target action
        if (btDeviceTargetState_ == MAP_MCE_DEV_STATE_DISCONNECTED) {
            // request disconnect
            utility::Message outMsg(MSG_MCEDEVICE_REQ_DEVICE_DISCONNECT);
            PostMessage(outMsg);
        } else {
            // process connected ok
            utility::Message sendServiceMsg(MSG_MCESERVICE_DEVICE_CONNECTED);
            devService_.PostMessage(sendServiceMsg);

            // if connect ok , SetNotificationRegistration
            std::map<const int, std::unique_ptr<utility::StateMachine>>::iterator it = deviceInstanceStmMap_.begin();
            if (it != deviceInstanceStmMap_.end()) {
                static_cast<MapMceInstanceStm *>(it->second.get())
                    ->GetMasClient()
                    .ClientSendReqSetNotificationRegistration(notificationRegistration_);
            }
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

MapActionType MapMceDeviceCtrl::ChangeRequestTypeToIprofileType(MceRequestType type)
{
    MapActionType actType = MapActionType::GET_MAS_INSTANCE_INFO;
    switch (type) {
        case MCE_REQUEST_TYPE_SET_NOTIFICATION_FILTER:
            actType = MapActionType::SET_NOTIFICATION_FILTER;
            break;
        case MCE_REQUEST_TYPE_GET_MESSAGE:
            actType = MapActionType::GET_MESSAGE;
            break;
        case MCE_REQUEST_TYPE_GET_MESSAGELISTING:
            actType = MapActionType::GET_MESSAGES_LISTING;
            break;
        case MCE_REQUEST_TYPE_SEND_MESSAGE:
            actType = MapActionType::SEND_MESSAGE;
            break;
        case MCE_REQUEST_TYPE_SET_MESSAGE_STATUS:
            actType = MapActionType::SET_MESSAGE_STATUS;
            break;
        case MCE_REQUEST_TYPE_GET_UNREAD_MESSAGES:
            actType = MapActionType::GET_UNREAD_MESSAGES;
            break;
        case MCE_REQUEST_TYPE_UPDATE_INBOX:
            actType = MapActionType::UPDATE_INBOX;
            break;
        case MCE_REQUEST_TYPE_SET_OWNER_STATUS:
            actType = MapActionType::SET_OWNER_STATUS;
            break;
        case MCE_REQUEST_TYPE_GET_OWNER_STATUS:
            actType = MapActionType::GET_OWNER_STATUS;
            break;
        case MCE_REQUEST_TYPE_GET_CONVERSATION_LISTING:
            actType = MapActionType::GET_CONVERSATION_LISTING;
            break;
        default:
            break;
    }
    return actType;
}

void MapMceDeviceCtrl::ProcessErrorObserver(MapMceInstanceRequest &request)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    MapExecuteStatus excuteStatus = MapExecuteStatus::NOT_SUPPORT;
    IProfileMapAction retAction;

    retAction.action_ = ChangeRequestTypeToIprofileType(request.GetRequestType());
    if (retAction.action_ != MapActionType::GET_MAS_INSTANCE_INFO) {
        if ((retAction.action_ == MapActionType::GET_MESSAGE) ||
            (retAction.action_ == MapActionType::GET_UNREAD_MESSAGES)) {
            IProfileBMessage bmsg;
            deviceRpcCallbackMgr_.ExcuteObserverOnBmessageCompleted(btDevice_, bmsg, excuteStatus);
        } else if (retAction.action_ == MapActionType::GET_MESSAGES_LISTING) {
            IProfileMessagesListing msgListing;
            deviceRpcCallbackMgr_.ExcuteObserverOnMessagesListingCompleted(btDevice_, msgListing, excuteStatus);
        } else if (retAction.action_ == MapActionType::GET_CONVERSATION_LISTING) {
            IProfileConversationListing convListing;
            deviceRpcCallbackMgr_.ExcuteObserverOnConversationListingCompleted(btDevice_, convListing, excuteStatus);
        } else {
            deviceRpcCallbackMgr_.ExcuteObserverOnMapActionCompleted(btDevice_, retAction, excuteStatus);
        }
    }
}

void MapMceDeviceCtrl::SendRequest(utility::Message msg)
{
    std::lock_guard<std::recursive_mutex> lock(mceInstanceMapMutex_);

    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    MapMceInstanceStm *stmNormalFindOkPtr = nullptr;

    bool ownerFindOk = false;
    bool normalFindOk = false;
    if ((ctrlRequestPtr_ == nullptr) || (deviceInstanceStmMap_.size() == 0)) {
        LOG_ERROR("%{public}s ctrlRequestPtr_ is NULL or size=%{public}d", __PRETTY_FUNCTION__, int(deviceInstanceStmMap_.size()));
        return;
    }
    MapMceInstanceRequest *req = ctrlRequestPtr_.get();
    // out messange setup
    utility::Message outMsg(MSG_MASSTM_REQ_SEND_REQUEST);
    for (auto &it : deviceInstanceStmMap_) {
        auto stm = static_cast<MapMceInstanceStm *>(it.second.get());

        if (stm->GetMasClient().GetMasSdpParam().messageType & req->GetSupportMessageType()) {
            stmNormalFindOkPtr = stm;
            normalFindOk = true;
            if (stm->GetMasClient().GetMasInstanceInformation().isValid &&
                (stm->GetMasClient().GetMasInstanceInformation().ownerUciUtf8 == req->GetOwnerUci()) &&
                (req->GetOwnerUci() != "")) {
                ownerFindOk = true;
                // process message
                stm->MceProcessMessageWithRequest(outMsg, ctrlRequestPtr_);
                LOG_INFO("%{public}s find owner=%{public}s", __PRETTY_FUNCTION__, req->GetOwnerUci().c_str());
                break;
            }
        }
    }
    if ((!ownerFindOk) && normalFindOk) {
        stmNormalFindOkPtr->MceProcessMessageWithRequest(outMsg, ctrlRequestPtr_);
    }
    if (normalFindOk != true) {  // error
        ProcessErrorObserver(*req);
        LOG_ERROR("%{public}s request no support", __PRETTY_FUNCTION__);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceDeviceCtrl::MnsConnectedProcMsg(utility::Message msg)
{
    LOG_INFO("%{public}s enter,input msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    int ret = BT_NO_ERROR;
    switch (msg.what_) {
        case MSG_MCEDEVICE_MNS_INFO_DISCONNECT:
            TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTING);
            ret = MnsProcDisconnectAccept(msg);
            if (ret != BT_NO_ERROR) {
                TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTED);
                MnsProcDisconnected();
                LOG_ERROR("%{public}s Disconnect error", __PRETTY_FUNCTION__);
            }
            break;
        case MSG_MCEDEVICE_MNS_INFO_TRANSPORT_ERROR:
        case MSG_MCEDEVICE_MNS_INFO_TRANSPORT_DISCONNECTED:
            TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTED);
            MnsProcDisconnected();
            break;
        case MSG_MCEDEVICE_MNS_INFO_PUT:
            MnsProcObexPut(msg);
            break;
        default:
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceDeviceCtrl::MnsDisconnectedProcMsg(utility::Message msg)
{
    LOG_INFO("%{public}s enter,input msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    switch (msg.what_) {
        case MSG_MCEDEVICE_MNS_INFO_TRANSPORT_INCOMING:
            MnsProcIncomingAccept(msg);
            break;
        case MSG_MCEDEVICE_MNS_INFO_TRANSPORT_DISCONNECTED:
        case MSG_MCEDEVICE_MNS_INFO_TRANSPORT_ERROR:
            LOG_ERROR("%{public}s TRANSPORT_DISCONNECTED state error", __PRETTY_FUNCTION__);
            MnsProcDisconnected();
            break;
        default:
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceDeviceCtrl::MnsConnectingProcMsg(utility::Message msg)
{
    LOG_INFO("%{public}s enter,input msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    switch (msg.what_) {
        case MSG_MCEDEVICE_MNS_INFO_DISCONNECT:
            LOG_ERROR("%{public}s OnDisconnect state error", __PRETTY_FUNCTION__);
            TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTED);
            MnsProcDisconnected();
            break;
        case MSG_MCEDEVICE_MNS_INFO_CONNECT:
            // if client allow , server is connected
            MnsProcConnectAccept(msg);
            break;
        case MSG_MCEDEVICE_MNS_INFO_TRANSPORT_ERROR:
        case MSG_MCEDEVICE_MNS_INFO_TRANSPORT_DISCONNECTED:
            TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTED);
            MnsProcDisconnected();
            break;
        default:
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceDeviceCtrl::MnsDisconnectingProcMsg(utility::Message msg)
{
    LOG_INFO("%{public}s enter,input msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    switch (msg.what_) {
        case MSG_MCEDEVICE_MNS_INFO_DISCONNECT:
            TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTED);
            MnsProcDisconnected();
            LOG_ERROR("%{public}s OnDisconnect state error", __PRETTY_FUNCTION__);
            break;
        case MSG_MCEDEVICE_MNS_INFO_CONNECT:
            MnsProcConnectRefuse(msg);
            break;
        case MSG_MCEDEVICE_MNS_INFO_TRANSPORT_DISCONNECTED:
        case MSG_MCEDEVICE_MNS_INFO_TRANSPORT_ERROR:
            TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTED);
            MnsProcDisconnected();
            break;
        default:
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceDeviceCtrl::MnsProcObexPut(utility::Message msg)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);

    uint8_t masInstanceId = 0;

    auto appParam = mnsSessionObexheader_->GetItemAppParams();

    if (appParam != nullptr) {
        auto tlvParam1 = appParam->GetTlvtriplet(MCE_MAS_INSTANCEID);
        if (tlvParam1 != nullptr) {
            const uint8_t *val1 = tlvParam1->GetVal();
            if ((tlvParam1->GetLen() != 0) && (val1 != nullptr)) {
                masInstanceId = *val1;
            }
        }
    } else {
        LOG_INFO("%{public}s ItemAppParams nullptr", __PRETTY_FUNCTION__);
    }

    // get body
    auto objectData = mnsSessionObexheader_->GetExtendBodyObject();
    uint8_t tempBuf[MCE_MAX_LENGTH_OF_RESPONSE_BUFFER];
    size_t bufLen = 1;
    std::ostringstream stream;

    while ((bufLen != 0) && (objectData != 0)) {
        bufLen = objectData->Read(tempBuf, MCE_MAX_LENGTH_OF_RESPONSE_BUFFER - 1);
        if (bufLen != 0) {
            tempBuf[bufLen] = 0;
            stream << (char *)tempBuf;
        }
    }
    MceTypesEventReport eventReport;
    if (stream.str().size() != 0) {
        eventReport.BuildObjectData(masInstanceId, stream.str());
    }
    deviceRpcCallbackMgr_.ExcuteObserverOnMapEventReported(btDevice_, eventReport.GetParam());
    std::unique_ptr<bluetooth::ObexHeader> resp = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
    if (mnsObexSession_ != nullptr) {
        int ret = mnsObexSession_->SendResponse(*resp);
        if (ret != BT_NO_ERROR) {
            LOG_ERROR("%{public}s SendResponse execute error", __PRETTY_FUNCTION__);
        }
    }
}

int MapMceDeviceCtrl::MnsProcConnectAcceptCheckTargetId()
{
    // accept
    if (!mnsSessionObexheader_->GetItemTarget()) {
        mnsObexSession_->SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE, true));
        LOG_ERROR("%{public}s ObexRspCode::NOT_ACCEPTABLE", __PRETTY_FUNCTION__);
        return RET_BAD_PARAM;
    }
    if (mnsSessionObexheader_->GetItemTarget()->GetHeaderDataSize() != MAX_OF_MASCLIENT_OBEX_UUID_TBL) {
        mnsObexSession_->SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE, true));
        LOG_ERROR("%{public}s ObexRspCode::NOT_ACCEPTABLE", __PRETTY_FUNCTION__);
        return RET_BAD_PARAM;
    }
    // dont need check support feature
    std::unique_ptr<uint8_t[]> masTarget = mnsSessionObexheader_->GetItemTarget()->GetBytes();
    for (int i = 0; i < MAX_OF_MASCLIENT_OBEX_UUID_TBL; i++) {
        if (masTarget[i] != mnsTargetUuidTbl_[i]) {
            mnsObexSession_->SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE, true));
            LOG_ERROR("%{public}s ObexRspCode::NOT_ACCEPTABLE", __PRETTY_FUNCTION__);
            return RET_BAD_PARAM;
        }
    }
    return RET_NO_ERROR;
}

void MapMceDeviceCtrl::CheckPowerBusyStateChange()
{
    bool findBusyFlag = false;
    // check mns server
    if (mnsContinueBusyFlag_) {
        findBusyFlag = true;
    }
    // check mas client
    for (auto &it : deviceInstanceStmMap_) {
        auto stm = static_cast<MapMceInstanceStm *>(it.second.get());
        if (stm->GetBusyStatus()) {
            findBusyFlag = true;
        }
    }
    if (findBusyFlag) {
        TransPowerBusyState(MAP_MCE_PWBUSY_STATUS_HIGH);
    } else {
        TransPowerBusyState(MAP_MCE_PWBUSY_STATUS_LOW);
    }
}

void MapMceDeviceCtrl::TransPowerBusyState(MapMcePowerBusyState state)
{
    RawAddress rwDevice(btDevice_);
    if (ctrlBusyStatus_ != state) {
        LOG_INFO("%{public}s status = %{public}d", __PRETTY_FUNCTION__, ctrlBusyStatus_);
        IPowerManager &pwMgr = IPowerManager::GetInstance();
        ctrlBusyStatus_ = state;
        LOG_INFO("%{public}s to state = %{public}d", __PRETTY_FUNCTION__, ctrlBusyStatus_);
        if (ctrlBusyStatus_ == MAP_MCE_PWBUSY_STATUS_DISCONNECT) {
            mnsContinueBusyFlag_ = false;
            pwMgr.StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_MAP_MCE, rwDevice);
        } else if (ctrlBusyStatus_ == MAP_MCE_PWBUSY_STATUS_CONNECT) {
            pwMgr.StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_MAP_MCE, rwDevice);
        } else if (ctrlBusyStatus_ == MAP_MCE_PWBUSY_STATUS_LOW) {
            pwMgr.StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_MAP_MCE, rwDevice);
        } else if (ctrlBusyStatus_ == MAP_MCE_PWBUSY_STATUS_HIGH) {
            pwMgr.StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_MAP_MCE, rwDevice);
        } else {
            pwMgr.StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_MAP_MCE, rwDevice);
        }
    }
}

void MapMceDeviceCtrl::MnsProcIncomingAccept(utility::Message msg)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    incomingPtr = static_cast<ObexIncomingConnect *>(msg.arg2_);
    if (incomingPtr != nullptr) {
        MapMceDeviceStateType state = GetCurrentDeviceState();
        if (((state == MAP_MCE_DEV_STATE_CONNECTED) || (state == MAP_MCE_DEV_STATE_CONNECTING)) &&
            (btDeviceTargetState_ == MAP_MCE_DEV_STATE_CONNECTED)) {
            TransMnsState(MAP_MCE_DEV_STATE_CONNECTING);
            incomingPtr->AcceptConnection();
        } else {
            incomingPtr->RejectConnection();
            LOG_ERROR("%{public}s state wrong incoming Reject Connection", __PRETTY_FUNCTION__);
        }
    }
}

void MapMceDeviceCtrl::MnsProcConnectAccept(utility::Message msg)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::unique_ptr<bluetooth::ObexHeader> resp;
    int ret;
    mnsObexSession_ = static_cast<ObexServerSession *>(msg.arg2_);
    if ((mnsObexSession_ == nullptr) || (mnsSessionObexheader_ == nullptr)) {
        LOG_ERROR("%{public}s mnsObexSession_ or obex header null", __PRETTY_FUNCTION__);
        return;
    }
    MapMceDeviceStateType state = GetCurrentDeviceState();
    if (((state == MAP_MCE_DEV_STATE_CONNECTED) || (state == MAP_MCE_DEV_STATE_CONNECTING)) &&
        (btDeviceTargetState_ == MAP_MCE_DEV_STATE_CONNECTED)) {
        if (MnsProcConnectAcceptCheckTargetId() != RET_NO_ERROR) {
            LOG_ERROR("%{public}s ObexRspCode::NOT_ACCEPTABLE", __PRETTY_FUNCTION__);
            return;
        }
        resp = ObexHeader::CreateResponse(ObexRspCode::SUCCESS, true);
        connectId_++;
        resp->AppendItemConnectionId(connectId_);
        resp->AppendItemWho(mnsTargetUuidTbl_, MAX_OF_MASCLIENT_OBEX_UUID_TBL);
        ret = mnsObexSession_->SendResponse(*resp);
        if (ret == BT_NO_ERROR) {
            TransMnsState(MAP_MCE_DEV_STATE_CONNECTED);
        } else {
            LOG_ERROR("%{public}s SendResponse execute error", __PRETTY_FUNCTION__);
            ret = mnsObexSession_->Disconnect();
            if (ret != BT_NO_ERROR) {
                TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTED);
                LOG_ERROR("%{public}s Disconnect error", __PRETTY_FUNCTION__);
            } else {
                TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTING);
            }
        }
    } else {  // refuse
        resp = ObexHeader::CreateResponse(ObexRspCode::FORBIDDEN, true);
        ret = mnsObexSession_->SendResponse(*resp);
        if (ret != BT_NO_ERROR) {
            LOG_ERROR("%{public}s SendResponse execute error", __PRETTY_FUNCTION__);
        }
        ret = mnsObexSession_->Disconnect();
        if (ret != BT_NO_ERROR) {
            TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTED);
            LOG_ERROR("%{public}s Disconnect error", __PRETTY_FUNCTION__);
        } else {
            TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTING);
        }
        LOG_ERROR("%{public}s state wrong, refuse on connect!", __PRETTY_FUNCTION__);
    }
}

void MapMceDeviceCtrl::MnsProcConnectRefuse(utility::Message msg)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);

    mnsObexSession_ = static_cast<ObexServerSession *>(msg.arg2_);
    std::unique_ptr<bluetooth::ObexHeader> resp = ObexHeader::CreateResponse(ObexRspCode::FORBIDDEN, true);
    if (mnsObexSession_ != nullptr) {
        mnsObexSession_->SendResponse(*resp);
    }
}

void MapMceDeviceCtrl::MnsProcDisconnected()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);

    // disconnected finish
    // send msg to the device control
    MapMceDeviceStateType state = GetCurrentDeviceState();
    if (state == MAP_MCE_DEV_STATE_DISCONNECTING) {
        utility::Message outMsg(MSG_MCEDEVICE_MNS_DEVICE_DISCONNECTED_FINISH);
        PostMessage(outMsg);
    }
}

int MapMceDeviceCtrl::MnsProcDisconnectAccept(utility::Message msg)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);

    int ret = BT_OPERATION_FAILED;
    mnsObexSession_ = static_cast<ObexServerSession *>(msg.arg2_);
    std::unique_ptr<bluetooth::ObexHeader> resp;
    if (mnsObexSession_ == nullptr) {
        // error
        return ret;
    }
    resp = ObexHeader::CreateResponse(ObexRspCode::SUCCESS, true);
    ret = mnsObexSession_->SendResponse(*resp);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s SendResponse error", __PRETTY_FUNCTION__);
    }
    ret = mnsObexSession_->Disconnect();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s Disconnect error", __PRETTY_FUNCTION__);
    }
    return ret;
}

void MapMceDeviceCtrl::MnsProcRequestDisconnect()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);

    if (GetMnsState() == MAP_MCE_DEV_STATE_CONNECTED) {
        TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTING);
        // disconnect the mns
        int ret = mnsObexSession_->Disconnect();

        if (ret != BT_NO_ERROR) {
            LOG_ERROR("%{public}s Disconnect error", __PRETTY_FUNCTION__);
            TransMnsState(MAP_MCE_DEV_STATE_DISCONNECTED);
            MnsProcDisconnected();
        }
    } else if (GetMnsState() == MAP_MCE_DEV_STATE_CONNECTING) {
        // waiting
    }
}

void MapMceDeviceCtrl::MnsChangeSessionObexHeader(const ObexHeader &req)
{
    mnsSessionObexheader_ = std::make_unique<ObexHeader>(req);
}

// call by service
int MapMceDeviceCtrl::CountSendingRequest(MceRequestType requestType)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceMapMutex_);

    int retSum = 0;

    if (GetCurrentDeviceState() == MAP_MCE_DEV_STATE_CONNECTED) {
        for (auto &it : deviceInstanceStmMap_) {
            auto stm = static_cast<MapMceInstanceStm *>(it.second.get());
            if (stm->GetCurrentMceStmStateName() == MCE_CONNECTED_STATE_S_REQSENDING) {

                retSum += stm->GetMasClient().ClientCountSendingRequest(requestType);
            }
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return retSum;
}

void MapMceDeviceCtrl::SetDeviceCtlConfig(const MasInstanceConfig &config)
{
    insDefaultConfig_ = config;
}
}  // namespace bluetooth
}  // namespace OHOS