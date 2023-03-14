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
 * @brief Defines map mce service object.
 *
 */

/**
 * @file map_mce_service.cpp
 *
 * @brief map mce source file .
 *
 */

#include "map_mce_service.h"
#include "adapter_config.h"
#include "class_creator.h"
#include "log.h"
#include "log_util.h"
#include "map_mce_types.h"
#include "map_mce_instance_stm.h"
#include "map_mce_mns_server.h"
#include "profile_config.h"
#include "profile_service_manager.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
// c language function declare
int MceAnalyseSupportedFeatrueAttribute(int &supportedFeatrue, const SdpService &servicePointer)
{
    bool findInstanceFeature = false;
    int ret = RET_NO_ERROR;
    SdpAttribute *tempAttribute = servicePointer.attribute;
    for (int attSeqCount = 0; attSeqCount < servicePointer.attributeNumber; attSeqCount++, tempAttribute++) {
        if (tempAttribute->attributeId == MAP_MCE_SUPPORTED_FEATURES_ATTRIBUTE_ID) {
            supportedFeatrue = *(uint32_t *)tempAttribute->attributeValue;
            findInstanceFeature = true;
            break;
        }
    }
    // check param value
    if (!findInstanceFeature) {
        // param error
        ret = RET_BAD_PARAM;
        LOG_ERROR("%{public}s param error:feature=%{public}d", __PRETTY_FUNCTION__, findInstanceFeature);
    }
    return ret;
}

void MapMceGetSupportFeatureSdpSearchCb(
    const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);

    int supportedFeatrue = 0;
    int supportedFeatrueAll = 0;
    int finishId = MSG_MCESERVICE_GET_SURPORT_FEATURES_FINISH;
    int failedId = MSG_MCESERVICE_GET_SURPORT_FEATURES_FAILED;
    const SdpService *tempServicePointer = serviceAry;
    IProfileManager *serviceMgr = IProfileManager::GetInstance();
    if ((serviceMgr == nullptr) || (addr == nullptr) || (serviceNum == 0)) {
        LOG_ERROR("%{public}s no service mgr, serviceNum= %{public}d", __PRETTY_FUNCTION__, int(serviceNum));
        return;
    }
    auto mnsService = static_cast<MapMceService *>(serviceMgr->GetProfileService(PROFILE_NAME_MAP_MCE));
    if (mnsService == nullptr) {
        LOG_ERROR("%{public}s mnsService is nullptr", __PRETTY_FUNCTION__);
        return;
    }
    BtAddr *argPrt = new (std::nothrow)BtAddr;
    if (argPrt == nullptr) {
        LOG_ERROR("%{public}s argPrt is nullptr", __PRETTY_FUNCTION__);
        return;
    }
    utility::Message msg(failedId);
    *argPrt = *addr;
    msg.arg2_ = (void *)argPrt;
    for (int serviceCount = 0; serviceCount < serviceNum; serviceCount++, tempServicePointer++) {
        // check param array
        if (tempServicePointer->attributeNumber == 0) {
            mnsService->PostMessage(msg);
            LOG_ERROR("%{public}s param error:attributeNumber is null", __PRETTY_FUNCTION__);
            return;
        }
        int ret = MceAnalyseSupportedFeatrueAttribute(supportedFeatrue, *tempServicePointer);
        // check sdp param value
        if (ret != RET_NO_ERROR) {
            mnsService->PostMessage(msg);
            LOG_ERROR("%{public}s mot found supportedFeatrue", __PRETTY_FUNCTION__);
            return;
        }
        supportedFeatrueAll |= supportedFeatrue;  // make param
    }
    msg.what_ = finishId;
    msg.arg1_ = supportedFeatrueAll;
    mnsService->PostMessage(msg);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

MapMceService::MapMceService() : utility::Context(PROFILE_NAME_MAP_MCE, "1.4.2")
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    serviceState_ = MAP_MCE_STATE_SHUTDOWN;
    serviceDispatcher_ = GetDispatcher();
    if (serviceDispatcher_ == nullptr) {
        LOG_INFO("%{public}s dispatch error", __PRETTY_FUNCTION__);
    }
    mnsServer_ = nullptr;
    serviceBtDeviceInstMgrMap_.clear();
    notificationRegistration_ = false;
    insDefaultConfig_.l2capMtu = MCE_INSTANCE_CLIENT_OBEX_MTU;
    insDefaultConfig_.rfcommMtu = MCE_INSTANCE_CLIENT_OBEX_MTU;
    insDefaultConfig_.isSupportSrm = true;
    insDefaultConfig_.deviceId = 0;
    insDefaultConfig_.singleInstMode = false;
    insDefaultConfig_.singleInstanceId = 0;
    insDefaultConfig_.maxOfDevice = MCE_MAX_OF_CONNECTED_DEVICES;
    insDefaultConfig_.maxOfGetUnread = MAP_MAX_LIST_COUNT_FOR_GET_UNREAD_MESSAGE;

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

MapMceService::~MapMceService()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // release all client stm memory
    RemoveAllDevice();
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

utility::Context *MapMceService::GetContext()
{
    return this;
}

void MapMceService::RegisterObserver(IProfileMapMceObserver &observer)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    serviceRpcCallbackMgr_.RegisterObserver(observer);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceService::DeregisterObserver(IProfileMapMceObserver &observer)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    serviceRpcCallbackMgr_.DeregisterObserver(observer);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

bool MapMceService::IsConnected(const RawAddress &device)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    bool ret = false;
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    if (serviceState_ != MAP_MCE_STATE_STARTUP) {
        LOG_ERROR("%{public}s error:RET_BAD_STATUS", __PRETTY_FUNCTION__);
        return false;
    }
    // find the device
    std::string tempDev = device.GetAddress();
    auto it = serviceBtDeviceInstMgrMap_.find(tempDev);
    if (it != serviceBtDeviceInstMgrMap_.end()) {
        if (it->second->GetCurrentDeviceState() == MAP_MCE_DEV_STATE_CONNECTED) {
            ret = true;
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

std::vector<RawAddress> MapMceService::GetDevicesByStates(const std::vector<int> &statusList)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    std::vector<RawAddress> deviceList;
    if (serviceState_ != MAP_MCE_STATE_STARTUP) {
        LOG_ERROR("%{public}s error:RET_BAD_STATUS", __PRETTY_FUNCTION__);
        return deviceList;
    }
    // find the device
    std::unique_ptr<RawAddress> rawAddressPtr = nullptr;
    for (auto it = serviceBtDeviceInstMgrMap_.begin(); it != serviceBtDeviceInstMgrMap_.end(); it++) {
        for (auto itState = statusList.begin(); itState != statusList.end(); itState++) {
            if (it->second->GetCurrentDeviceState() == *itState) {
                rawAddressPtr = std::make_unique<RawAddress>(it->second->GetBtDevice());
                deviceList.push_back(*rawAddressPtr);
                break;
            }
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return deviceList;
}

int MapMceService::SetConnectionStrategy(const RawAddress &device, int strategy)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    IProfileConfig *config = ProfileConfig::GetInstance();
    if ((serviceState_ != MAP_MCE_STATE_STARTUP) || (config == nullptr)) {
        LOG_ERROR(
            "%{public}s state error: service state=%{public}d or IProfileConfig is NULL", __PRETTY_FUNCTION__, int(serviceState_));
        return RET_BAD_STATUS;
    }
    // param error
    if ((BTStrategyType(strategy) != BTStrategyType::CONNECTION_ALLOWED) &&
        (BTStrategyType(strategy) != BTStrategyType::CONNECTION_FORBIDDEN)) {
        LOG_ERROR("%{public}s strategy type unknown", __PRETTY_FUNCTION__);
        return RET_BAD_PARAM;
    }
    // save the value
    int value;
    if (BTStrategyType(strategy) == BTStrategyType::CONNECTION_ALLOWED) {
        value = 1;
    } else {
        value = 0;
    }
    bool ret = config->SetValue(
        device.GetAddress(), SECTION_CONNECTION_POLICIES, PROPERTY_MAP_CLIENT_CONNECTION_POLICY, value);
    // set success
    if (ret) {
        // check dispatcher
        utility::Dispatcher *dispatcher = GetDispatcher();
        if (dispatcher != nullptr) {
            if (value == 1) {
                // service function execute
                dispatcher->PostTask(std::bind(&MapMceService::ConnectInternal, this, device, false, 0));
            } else {
                // service function execute
                dispatcher->PostTask(std::bind(&MapMceService::DisconnectInternal, this, device));
            }
        } else {
            LOG_ERROR("%{public}s dispatcher error", __PRETTY_FUNCTION__);
        }
        LOG_INFO("%{public}s success end", __PRETTY_FUNCTION__);
        return RET_NO_ERROR;
    } else {
        LOG_ERROR("%{public}s error end", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
}

int MapMceService::GetConnectionStrategy(const RawAddress &device) const
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    if (serviceState_ != MAP_MCE_STATE_STARTUP) {
        LOG_ERROR("%{public}s error: startup error", __PRETTY_FUNCTION__);
        return int(BTStrategyType::CONNECTION_FORBIDDEN);
    }
    IProfileConfig *config = ProfileConfig::GetInstance();
    int value = 0;
    const std::string dev = device.GetAddress();
    if (!config->GetValue(dev, SECTION_CONNECTION_POLICIES, PROPERTY_MAP_CLIENT_CONNECTION_POLICY, value)) {
        LOG_DEBUG("%{public}s %{public}s not found", dev.c_str(), PROPERTY_MAP_CLIENT_CONNECTION_POLICY.c_str());
        return int(BTStrategyType::CONNECTION_UNKNOWN);
    }
    if (value) {
        return int(BTStrategyType::CONNECTION_ALLOWED);
    } else {
        return int(BTStrategyType::CONNECTION_FORBIDDEN);
    }
}

int MapMceService::GetUnreadMessages(const RawAddress &device, MapMessageType type, uint8_t max)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    IProfileGetMessagesListingParameters para;
    para.folder = u"inbox";
    para.MaxListCount = max;  // max of items
    para.ListStartOffset = 0;
    para.SubjectLength = MCE_MAX_LENGTH_10;  // 1-255
    para.ParameterMask = MAP_GETMESSAGELIST_PARAMETER_MASK_SUBJECT | MAP_GETMESSAGELIST_PARAMETER_MASK_TYPE |
                         MAP_GETMESSAGELIST_PARAMETER_MASK_RECEPTION_STATUS | MAP_GETMESSAGELIST_PARAMETER_MASK_READ |
                         MAP_GETMESSAGELIST_PARAMETER_MASK_SENT;
    para.FilterMessageType = MAP_FILTER_MESSAGE_MASK_NO_FILTERING;  // get alltype
    para.FilterPeriodBegin = "";
    para.FilterPeriodEnd = "";
    para.FilterReadStatus = MAP_FILTER_READ_STATUS_MASK_UNREAD;
    para.FilterRecipient = "";
    para.FilterOriginator = "";
    para.FilterPriority = MAP_FILTER_PRIORITY_MASK_NO_FILTERING;
    para.ConversationID = "";
    para.FilterMessageHandle = "";

    MapMceRequestConfig cfg;
    cfg.maxOfGetUnread = insDefaultConfig_.maxOfGetUnread;

    utility::Message outMsg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestGetUreadMessages>(type, para);
    reqPtr->SetRequestConfig(cfg);
    int ret = SendRequestToConnectedDevice(device, outMsg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::GetSupportedFeatures(const RawAddress &device)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret = RET_NO_ERROR;
    if (serviceState_ != MAP_MCE_STATE_STARTUP) {
        LOG_ERROR("%{public}s error: startup error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    // check dispatcher
    utility::Dispatcher *dispatcher = GetDispatcher();
    if (dispatcher == nullptr) {
        LOG_INFO("%{public}s dispatcher error", __PRETTY_FUNCTION__);
        return RET_NO_SUPPORT;
    }
    dispatcher->PostTask(std::bind(&MapMceService::CheckSdpForGetSupportedFeatures, this, device));
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::SendMessage(const RawAddress &device, const IProfileSendMessageParameters &msg)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // if version_property is 1.1, need check the device's version
    if ((msg.Charset == MapCharsetType::INVALID) || (msg.bmessage_.folder_property == u"")) {
        LOG_ERROR("%{public}s Charset or Folder null", __PRETTY_FUNCTION__);
        return RET_BAD_PARAM;
    }
    if (msg.MessageHandle != "") {
        if ((msg.Attachment == MapAttachmentType::INVALID) || (msg.ModifyText == MapModifyTextType::INVALID)) {
            LOG_ERROR("%{public}s have MessageHandle ,but no Attachment or ModifyText", __PRETTY_FUNCTION__);
            return RET_BAD_PARAM;
        }
    }
    if (msg.bmessage_.envelope_.maxLevelOfEnvelope_ == 0 ||
        msg.bmessage_.envelope_.maxLevelOfEnvelope_ > MCE_RECIPIENT_LEVEL3) {
        LOG_ERROR("%{public}s envelope_ level error = %{public}d", __PRETTY_FUNCTION__, msg.bmessage_.envelope_.maxLevelOfEnvelope_);
        return RET_BAD_PARAM;
    }
    utility::Message outMsg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestPushMessage>(msg);
    int ret = SendRequestToConnectedDevice(device, outMsg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::SetNotificationFilter(const RawAddress &device, const int mask)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    if (!notificationRegistration_) {
        LOG_ERROR("%{public}s error: MNS server FALSE", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    utility::Message msg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestSetNotificationFilter>(mask);
    int ret = SendRequestToConnectedDevice(device, msg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::SetNotificationRegistration(const RawAddress &device, const bool on)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    if (!notificationRegistration_) {
        LOG_ERROR("%{public}s error: MNS server FALSE", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    utility::Message msg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestSetNotificationRegistration>(on);
    int ret = SendRequestToConnectedDevice(device, msg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::GetMessagesListing(const RawAddress &device, const IProfileGetMessagesListingParameters &para)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestGetMessagesListing>(para);
    int ret = SendRequestToConnectedDevice(device, msg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::GetMessage(const RawAddress &device, MapMessageType type, const std::u16string &msgHandle,
    const IProfileGetMessageParameters &para)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestGetMessage>(msgHandle, para);
    reqPtr->SetSupportMessageType(MapMessageTypeToIprofileMask(type));
    int ret = SendRequestToConnectedDevice(device, msg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::GetMessageWithReqCfg(const RawAddress &device, MapMessageType type, const std::u16string &msgHandle,
    const IProfileGetMessageParameters &para, const MapMceRequestConfig &cfg)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestGetMessage>(msgHandle, para);
    reqPtr->SetSupportMessageType(MapMessageTypeToIprofileMask(type));
    reqPtr->SetRequestConfig(cfg);
    int ret = SendRequestToConnectedDevice(device, msg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::UpdateInbox(const RawAddress &device, MapMessageType type)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestUpdateInbox>();
    reqPtr->SetSupportMessageType(MapMessageTypeToIprofileMask(type));
    int ret = SendRequestToConnectedDevice(device, msg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::GetConversationListing(
    const RawAddress &device, const IProfileGetConversationListingParameters &para)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestGetConversationListing>(para);
    reqPtr->SetSupportMessageType(MAP_MCE_SUPPORTED_MESSAGE_TYPE_IM);
    int ret = SendRequestToConnectedDevice(device, msg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::SetMessageStatus(
    const RawAddress &device, MapMessageType type, const IProfileSetMessageStatus &msgStatus)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestSetMessageStatus>(
        msgStatus.msgHandle, msgStatus.statusIndicator, msgStatus.statusValue, msgStatus.extendedData);
    reqPtr->SetSupportMessageType(MapMessageTypeToIprofileMask(type));
    int ret = SendRequestToConnectedDevice(device, msg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::SetOwnerStatus(const RawAddress &device, const IProfileSetOwnerStatusParameters &para)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestSetOwnerStatus>(para);
    reqPtr->SetSupportMessageType(MAP_MCE_SUPPORTED_MESSAGE_TYPE_IM);
    int ret = SendRequestToConnectedDevice(device, msg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::GetOwnerStatus(const RawAddress &device, const std::string &conversationId)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestGetOwnerStatus>(conversationId);
    reqPtr->SetSupportMessageType(MAP_MCE_SUPPORTED_MESSAGE_TYPE_IM);
    int ret = SendRequestToConnectedDevice(device, msg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::Connect(const RawAddress &device)  // override
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // check service status
    if (serviceState_ != MAP_MCE_STATE_STARTUP) {
        LOG_ERROR("%{public}s connect startup error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    // check dispatcher
    utility::Dispatcher *dispatcher = GetDispatcher();
    if (dispatcher == nullptr) {
        LOG_ERROR("%{public}s Connect dispatcher error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    RawAddress rawDevice(device.GetAddress());
    BTStrategyType strate = BTStrategyType(GetConnectionStrategy(rawDevice));
    if (strate == BTStrategyType::CONNECTION_FORBIDDEN) {
        return RET_BAD_STATUS;
    }

    // check device connected status
    int checkResult = CheckDeviceConnectStatus(rawDevice);
    if (checkResult != RET_NO_ERROR) {
        LOG_INFO("%{public}s device status error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    // service function execute
    dispatcher->PostTask(std::bind(&MapMceService::ConnectInternal, this, rawDevice, false, 0));
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int MapMceService::ConnectInstance(const RawAddress &device, int instanceId)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // check service status
    if (serviceState_ != MAP_MCE_STATE_STARTUP) {
        LOG_ERROR("%{public}s ConnectInstance startup error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    // check dispatcher
    utility::Dispatcher *dispatcher = GetDispatcher();
    if (dispatcher == nullptr) {
        LOG_INFO("%{public}s ConnectInstance dispatcher error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    RawAddress rawDevice(device.GetAddress());
    BTStrategyType strate = BTStrategyType(GetConnectionStrategy(rawDevice));
    if (strate == BTStrategyType::CONNECTION_FORBIDDEN) {
        return RET_BAD_STATUS;
    }
    // check device connected status
    int checkResult = CheckDeviceConnectStatus(rawDevice);
    if (checkResult != RET_NO_ERROR) {
        LOG_INFO("%{public}s device status error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    // service function execute
    dispatcher->PostTask(std::bind(&MapMceService::ConnectInternal, this, rawDevice, true, instanceId));
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int MapMceService::Disconnect(const RawAddress &device)  // override
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // check service status
    if (serviceState_ != MAP_MCE_STATE_STARTUP) {
        LOG_ERROR("%{public}s Disconnect error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    // check dispatcher
    utility::Dispatcher *dispatcher = GetDispatcher();
    if (dispatcher == nullptr) {
        LOG_INFO("%{public}s Disconnect dispatcher error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    // check device connected status
    RawAddress rawDevice(device.GetAddress());
    int checkResult = CheckDeviceDisconnectStatus(rawDevice);
    if (checkResult != RET_NO_ERROR) {
        LOG_INFO("%{public}s device status error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    // service function execute
    dispatcher->PostTask(std::bind(&MapMceService::DisconnectInternal, this, rawDevice));
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int MapMceService::CountSendingRequest(const RawAddress &device, MceRequestType requestType)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    int retSum = 0;
    // find the device
    std::string tempDev = device.GetAddress();
    auto it = serviceBtDeviceInstMgrMap_.find(tempDev);
    // if device in the map
    if (it != serviceBtDeviceInstMgrMap_.end()) {
        retSum = it->second->CountSendingRequest(requestType);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return retSum;
}

int MapMceService::CheckDeviceConnectStatus(const RawAddress &device)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    int ret = RET_NO_ERROR;
    // find the device
    std::string tempDev = device.GetAddress();
    auto it = serviceBtDeviceInstMgrMap_.find(tempDev);
    // if device in the map
    if (it != serviceBtDeviceInstMgrMap_.end()) {
        if (it->second->GetCurrentDeviceState() != MAP_MCE_DEV_STATE_DISCONNECTED) {
            ret = RET_BAD_STATUS;
        }
        LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
        return ret;
    }
    if (serviceBtDeviceInstMgrMap_.size() >= insDefaultConfig_.maxOfDevice) {
        ret = RET_NO_SUPPORT;
        for (auto it2 = serviceBtDeviceInstMgrMap_.begin(); it2 != serviceBtDeviceInstMgrMap_.end(); ++it2) {
            // if it have disconnect devcie in the list
            if (it2->second->GetCurrentDeviceState() == MAP_MCE_DEV_STATE_DISCONNECTED) {
                ret = RET_NO_ERROR;
            }
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::CheckDeviceDisconnectStatus(const RawAddress &device)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    int ret = RET_NO_ERROR;
    // find the device
    std::string tempDev = device.GetAddress();
    auto it = serviceBtDeviceInstMgrMap_.find(tempDev);
    // if device in the map
    if (it != serviceBtDeviceInstMgrMap_.end()) {
        if (it->second->GetCurrentDeviceState() != MAP_MCE_DEV_STATE_CONNECTED) {
            ret = RET_BAD_STATUS;
        }
    } else {
        // device is not in the list
        ret = RET_NO_SUPPORT;
    }

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::SendRequestToConnectedDevice(
    const RawAddress &device, utility::Message msg, std::unique_ptr<MapMceInstanceRequest> &req)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    int ret = RET_BAD_STATUS;
    if (serviceState_ != MAP_MCE_STATE_STARTUP) {
        LOG_ERROR("%{public}s error:RET_BAD_STATUS", __PRETTY_FUNCTION__);
        return ret;
    }
    // ready
    std::string tempDev = device.GetAddress();
    // find bt device
    auto it = serviceBtDeviceInstMgrMap_.find(tempDev);
    // find ok
    if (it != serviceBtDeviceInstMgrMap_.end()) {
        // check max send request
        int retSum = it->second->CountSendingRequest(MCE_REQUEST_TYPE_ALL);
        if (retSum < MCE_MAX_OF_SENDING_REQUEST) {
            // api only valid in connected state, if not valid return error at now
            if (it->second->GetCurrentDeviceState() == MAP_MCE_DEV_STATE_CONNECTED) {
                // send command to the state machine
                ret = it->second->PostMessageWithRequest(msg, req);
            } else {
                LOG_ERROR("%{public}s device not connected", __PRETTY_FUNCTION__);
            }
        } else {
            LOG_ERROR("%{public}s error, MCE_MAX_OF_SENDING_REQUEST", __PRETTY_FUNCTION__);
        }
    } else {
        LOG_ERROR("%{public}s not fount the device", __PRETTY_FUNCTION__);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::ConnectInternal(const RawAddress &device, bool sInsMode, int sInsId)
{
    HILOGI("address=%{public}s", GET_ENCRYPT_ADDR(device));
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    insDefaultConfig_.singleInstMode = sInsMode;
    insDefaultConfig_.singleInstanceId = sInsId;
    LOG_INFO("%{public}s singleInstMode=%{public}d,singleInstanceId=%{public}d ", __PRETTY_FUNCTION__, sInsMode, sInsId);

    int ret = RET_NO_ERROR;
    if (serviceState_ != MAP_MCE_STATE_STARTUP) {
        LOG_ERROR("%{public}s error return", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    // find the device
    std::string tempDev = device.GetAddress();
    auto it = serviceBtDeviceInstMgrMap_.find(tempDev);
    if (it == serviceBtDeviceInstMgrMap_.end()) {
        // device did not in the map
        // remove the no useful device and instance if the map is max
        RemoveNoUseDeviceAndInstance();
        // insert the instance and create the client stm
        if (serviceBtDeviceInstMgrMap_.size() < insDefaultConfig_.maxOfDevice) {
            insDefaultConfig_.deviceId++;
            auto deviceCtl1 = std::make_unique<MapMceDeviceCtrl>(
                tempDev, *this, notificationRegistration_, insDefaultConfig_, serviceRpcCallbackMgr_);
            utility::Message outMsg1(MSG_MCEDEVICE_REQ_DEVICE_CONNECT);
            deviceCtl1->ProcessMessage(outMsg1);
            serviceBtDeviceInstMgrMap_.insert(
                std::pair<const std::string, std::unique_ptr<MapMceDeviceCtrl>>(tempDev, std::move(deviceCtl1)));
            // start device connect
        } else {
            // error , device max
            ret = RET_NO_SUPPORT;
            LOG_ERROR("%{public}s error:connect max", __PRETTY_FUNCTION__);
        }
    } else {
        // device is in the map , refresh config and connect again
        it->second->SetDeviceCtlConfig(insDefaultConfig_);
        utility::Message outMsg2(MSG_MCEDEVICE_REQ_DEVICE_CONNECT);
        it->second->ProcessMessage(outMsg2);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::DisconnectInternal(const RawAddress &device)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    int ret = RET_NO_ERROR;
    if (serviceState_ != MAP_MCE_STATE_STARTUP) {
        LOG_ERROR("%{public}s DisconnectInternal startup error", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    // find the device
    std::string tempDev = device.GetAddress();
    auto it = serviceBtDeviceInstMgrMap_.find(tempDev);
    // device is in the map
    if (it != serviceBtDeviceInstMgrMap_.end()) {
        // request disconnect
        utility::Message outMsg(MSG_MCEDEVICE_REQ_DEVICE_DISCONNECT);
        it->second->PostMessage(outMsg);
    }
    // find device in the device map
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// find and remove the disconnect bluetooth device
void MapMceService::RemoveNoUseDeviceAndInstance()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // check map length
    if (serviceBtDeviceInstMgrMap_.size() < insDefaultConfig_.maxOfDevice) {
        return;
    }
    // check iterator in map
    for (auto it = serviceBtDeviceInstMgrMap_.begin(); it != serviceBtDeviceInstMgrMap_.end(); ++it) {
        if (it->second->GetCurrentDeviceState() == MAP_MCE_DEV_STATE_DISCONNECTED) {
            serviceBtDeviceInstMgrMap_.erase(it);
            break;
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceService::RemoveAllDevice()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    serviceBtDeviceInstMgrMap_.clear();
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return;
}

int MapMceService::MapMessageTypeToIprofileMask(MapMessageType type)
{
    uint8_t serviceMask;
    switch (type) {
        case MapMessageType::SMS_GSM:
            serviceMask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_GSM;
            break;
        case MapMessageType::SMS_CDMA:
            serviceMask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_CDMA;
            break;
        case MapMessageType::MMS:
            serviceMask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_MMS;
            break;
        case MapMessageType::EMAIL:
            serviceMask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_EMAIL;
            break;
        case MapMessageType::IM:
            serviceMask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_IM;
            break;
        default:
            serviceMask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_ALL;
            break;
    }
    return int(serviceMask);
}

std::list<RawAddress> MapMceService::GetConnectDevices()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    std::list<RawAddress> devList;
    // check iterator in client device map
    for (auto it = serviceBtDeviceInstMgrMap_.begin(); it != serviceBtDeviceInstMgrMap_.end(); ++it) {
        if (it->second != nullptr) {
            if (it->second->GetCurrentDeviceState() == MAP_MCE_DEV_STATE_CONNECTED) {
                RawAddress rawAddress(it->second->GetBtDevice());
                devList.push_back(rawAddress);
            }
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return devList;
}

int MapMceService::GetConnectState()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    MapMceDeviceStateType state;
    int profileState = 0;
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    for (auto it = serviceBtDeviceInstMgrMap_.begin(); it != serviceBtDeviceInstMgrMap_.end(); it++) {
        if (it->second != nullptr) {
            state = it->second->GetCurrentDeviceState();
            profileState |= it->second->DeviceStateConvertToProfileState(state);
        }
    }
    // not device connect
    if (profileState == 0) {
        profileState = PROFILE_STATE_DISCONNECTED;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return profileState;
}

int MapMceService::GetDeviceConnectState(const RawAddress &device)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    MapMceDeviceStateType ret = MAP_MCE_DEV_STATE_DISCONNECTED;
    auto it = serviceBtDeviceInstMgrMap_.find(device.GetAddress());
    if (it != serviceBtDeviceInstMgrMap_.end()) {
        if (it->second != nullptr) {
            ret = it->second->GetCurrentDeviceState();
        }
    }
    LOG_INFO("%{public}s end,status=%{public}d", __PRETTY_FUNCTION__, ret);
    return ret;  // return state
}

int MapMceService::GetFolderListing(const RawAddress &device, uint16_t maxOfListCount, uint16_t startOffset)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;
    utility::Message outMsg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr =
        std::make_unique<MapMceRequestGetFolderListing>(maxOfListCount, startOffset);
    ret = SendRequestToConnectedDevice(device, outMsg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceService::SetPath(const RawAddress &device, const uint8_t flags,
    const std::u16string &folder, std::vector<std::u16string> &folderList)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;
    utility::Message outMsg(MSG_MCEDEVICE_REQ_SEND_REQUEST);
    std::unique_ptr<MapMceInstanceRequest> reqPtr =
        std::make_unique<MapMceRequestSetPath>(flags, folder, folderList);
    ret = SendRequestToConnectedDevice(device, outMsg, reqPtr);
    if (ret != RET_NO_ERROR) {
        reqPtr = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

IProfileMasInstanceInfoList MapMceService::GetMasInstanceInfo(const RawAddress &device)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    IProfileMasInstanceInfoList masInfoList;
    masInfoList.isValid = false;
    std::vector<IProfileMasInstanceInfo> instanceinfoList;
    auto it = serviceBtDeviceInstMgrMap_.find(device.GetAddress());
    if (it != serviceBtDeviceInstMgrMap_.end()) {
        if (it->second != nullptr) {
            instanceinfoList = it->second->GetMasInstanceInfo(device);
            if (int(instanceinfoList.size()) == it->second->GetDeviceInstanseStmMapSize()) {
                masInfoList.isValid = true;
                masInfoList.masInfoList = instanceinfoList;
            }
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return masInfoList;
}

void MapMceService::Enable()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    utility::Dispatcher *dispatcher = GetDispatcher();
    if (dispatcher != nullptr) {
        dispatcher->PostTask(std::bind(&MapMceService::StartUpInternal, this));
    } else {
        LOG_ERROR("%{public}s dispatcher error", __PRETTY_FUNCTION__);
    }
}

int MapMceService::StartUpInternal()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    serviceState_ = MAP_MCE_STATE_STARTUP;
    // send response to adapt manager
    utility::Context *context = GetContext();
    if (context != nullptr) {
        context->OnEnable(PROFILE_NAME_MAP_MCE, true);
    }
    // get config parameter
    GetConfigFromXml();
    // mns startup
    mnsServer_ = std::make_unique<MapMceMnsServer>(*this, insDefaultConfig_);
    // start up mns server
    int ret = mnsServer_->StartUp();
    if (ret == RET_NO_ERROR) {
        notificationRegistration_ = true;
    } else {
        notificationRegistration_ = false;
        LOG_ERROR("%{public}s mns start error", __PRETTY_FUNCTION__);
    }
#ifndef MCE_DISABLE_L2CAP
    ObexMpClient::RegisterL2capLPsm(MAP_MCE_GOEP_L2CAP_PSM_VALUE);
#endif
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int MapMceService::GetMaxConnectNum()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return serviceBtDeviceInstMgrMap_.size();
}

void MapMceService::Disable()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    utility::Dispatcher *dispatcher = GetDispatcher();
    if (dispatcher != nullptr) {
        dispatcher->PostTask(std::bind(&MapMceService::ShutDownInternal, this));
    } else {
        LOG_ERROR("%{public}s dispatcher error", __PRETTY_FUNCTION__);
    }
}

int MapMceService::ShutDownInternal()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ShutDownInternalCommon();
}

int MapMceService::ShutDownInternalCommon()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    MapMceServiceStateType targetState = MAP_MCE_STATE_SHUTDOWN;
    serviceState_ = MAP_MCE_STATE_SHUTDOWNING;
    // check iterator in client device map
    for (auto it = serviceBtDeviceInstMgrMap_.begin(); it != serviceBtDeviceInstMgrMap_.end(); ++it) {
        if (it->second->GetCurrentDeviceState() != MAP_MCE_DEV_STATE_DISCONNECTED) {
            // request disconnect
            utility::Message outMsg(MSG_MCEDEVICE_REQ_DEVICE_DISCONNECT);
            it->second->PostMessage(outMsg);
            targetState = MAP_MCE_STATE_SHUTDOWNING;
        }
    }
    // all client have shutdown
    if (targetState == MAP_MCE_STATE_SHUTDOWN) {
        serviceState_ = MAP_MCE_STATE_SHUTDOWN;
#ifndef MCE_DISABLE_L2CAP
        ObexMpClient::DeregisterL2capLPsm(MAP_MCE_GOEP_L2CAP_PSM_VALUE);
#endif
        // release mns server
        if (mnsServer_ != nullptr) {
            mnsServer_->ShutDown();
        }
        // send response to adapt manager right now
        utility::Context *context = GetContext();
        if (context != nullptr) {
            LOG_INFO("%{public}s context->OnDisable execute", __PRETTY_FUNCTION__);
            context->OnDisable(PROFILE_NAME_MAP_MCE, true);
        }
    } else {  // if some client is working
        // need wait all client shutdown response callback
        serviceState_ = MAP_MCE_STATE_SHUTDOWNING;
        // send response to adapt manager when callback all come
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

void MapMceService::PostMessage(utility::Message msg)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Dispatcher *dispatcher = GetDispatcher();
    if (dispatcher == nullptr) {
        LOG_INFO("%{public}s dispatcher error", __PRETTY_FUNCTION__);
        return;
    }
    dispatcher->PostTask(std::bind(&MapMceService::ProcessMessage, this, msg));
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

MapMceServiceStateType MapMceService::GetServiceStates()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    LOG_INFO("%{public}s end,state=%{public}d", __PRETTY_FUNCTION__, int(serviceState_));
    return MapMceServiceStateType(int(serviceState_));
}

void MapMceService::MnsPostMessage(const utility::Message &msg, std::string btAddr)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Dispatcher *dispatcher = GetDispatcher();
    if (dispatcher == nullptr) {
        LOG_INFO("%{public}s dispatcher error", __PRETTY_FUNCTION__);
        return;
    }
    dispatcher->PostTask(std::bind(&MapMceService::MnsProcessMessage, this, msg, btAddr));
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceService::MnsPostMessageWithHeader(const utility::Message &msg, std::string btAddr, const ObexHeader &req)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Dispatcher *dispatcher = GetDispatcher();
    if (dispatcher == nullptr) {
        LOG_INFO("%{public}s dispatcher error", __PRETTY_FUNCTION__);
        return;
    }
    dispatcher->PostTask(std::bind(&MapMceService::MnsProcessMessageWithHeader, this, msg, btAddr, req));
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceService::MnsProcessMessage(utility::Message msg, std::string btAddr)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    auto it = serviceBtDeviceInstMgrMap_.find(btAddr);
    if (it != serviceBtDeviceInstMgrMap_.end()) {
        it->second->ProcessMessage(msg);
    } else {
        LOG_ERROR("%{public}s address error = %{public}s", __PRETTY_FUNCTION__, btAddr.c_str());
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceService::MnsProcessMessageWithHeader(utility::Message msg, std::string btAddr, ObexHeader req)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    auto it = serviceBtDeviceInstMgrMap_.find(btAddr);
    if (it != serviceBtDeviceInstMgrMap_.end()) {
        it->second->ProcessMnsObexObserverMessage(req, msg);
    } else {
        LOG_ERROR("%{public}s address error = %{public}s", __PRETTY_FUNCTION__, btAddr.c_str());
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceService::ProcessMessage(const utility::Message &msg)
{
    LOG_INFO("%{public}s enter,input msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    switch (msg.what_) {
        case MSG_MCEDEVICE_SDP_GET_INSTANCE_FINISH:
        case MSG_MCEDEVICE_SDP_GET_INSTANCE_FAILED:
            SdpDispatchToDeviceInstMgr(msg);
            break;
        case MSG_MCESERVICE_GET_SURPORT_FEATURES_FINISH:
        case MSG_MCESERVICE_GET_SURPORT_FEATURES_FAILED:
            ProcessGetSupportedFeaturesRespones(msg);
            break;
        case MSG_MCESERVICE_DEVICE_CONNECTED:
            break;
        case MSG_MCESERVICE_DEVICE_DISCONNECTED:
            break;
        default:
            break;
    }
    if (serviceState_ == MAP_MCE_STATE_STARTUP) {
        // empty
    } else if (serviceState_ == MAP_MCE_STATE_SHUTDOWNING) {
        switch (msg.what_) {
            case MSG_MCESERVICE_DEVICE_DISCONNECTED:
                // shutdown again;
                ShutDownInternalCommon();
                break;
            default:
                break;
        }
    } else {
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceService::SdpDispatchToDeviceInstMgr(const utility::Message &msg)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    auto argPrt = static_cast<MapSdpMsgArgPrt *>(msg.arg2_);
    if (argPrt == nullptr) {
        LOG_INFO("%{public}s error return", __PRETTY_FUNCTION__);
        return;
    }
    // check iterator in map
    for (auto it = serviceBtDeviceInstMgrMap_.begin(); it != serviceBtDeviceInstMgrMap_.end(); ++it) {
        BtAddr btAddress = it->second->GetBtAddress();
        if (memcmp(&(argPrt->address), &btAddress, sizeof(BtAddr)) == 0) {  // same bt address
            // send to the bt device
            it->second->ProcessMessage(msg);
            break;
        }
    }
    delete argPrt;
    argPrt = nullptr;
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

// call by post task
int MapMceService::CheckSdpForGetSupportedFeatures(const RawAddress &device)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);
    int index = 0;
    int retServiceVal;
    int retStackVal;
    IProfileMapAction retAction;
    MapExecuteStatus resCode;
    SdpAttributeIdList attributeIdList = {SDP_TYPE_LIST, .attributeIdList = {0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};
    BtUuid btUuid = {0, {.uuid128 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};
    SdpUuid sdpUuid = {0, nullptr};

    // get the bluetooth address
    BtAddr btAddr = {{0, 0, 0, 0, 0, 0}, 0};
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    RawAddress rawAddr(device.GetAddress());
    rawAddr.ConvertToUint8(btAddr.addr, sizeof(btAddr.addr));

    // config search attribute
    attributeIdList.attributeIdList.attributeId[index++] = MAP_MCE_SUPPORTED_FEATURES_ATTRIBUTE_ID;
    attributeIdList.attributeIdList.attributeIdNumber = index;
    attributeIdList.type = SDP_TYPE_LIST;
    // set uuid
    btUuid.type = BT_UUID_16;
    btUuid.uuid16 = MAP_MCE_SERVICE_CLASS_UUID;

    sdpUuid.uuid = &btUuid;
    sdpUuid.uuidNum = 1;
    // search sdp
    retStackVal =
        SDP_ServiceSearchAttribute(&btAddr, &sdpUuid, attributeIdList, nullptr, MapMceGetSupportFeatureSdpSearchCb);
    if (retStackVal != BT_NO_ERROR) {
        LOG_ERROR("%{public}s error:SDP_ServiceSearchAttribute", __PRETTY_FUNCTION__);
        retServiceVal = RET_NO_SUPPORT;
        // action complete fail
        resCode = MapExecuteStatus::NOT_SUPPORT;
        retAction.action_ = MapActionType::GET_SUPPORTED_FEATURES;
        serviceRpcCallbackMgr_.ExcuteObserverOnMapActionCompleted(device.GetAddress(), retAction, resCode);
    } else {
        retServiceVal = RET_NO_ERROR;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return retServiceVal;
}

// internal
void MapMceService::ProcessGetSupportedFeaturesRespones(utility::Message const &msg)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    uint32_t devSupportedFeatrue = 0;
    IProfileMapAction retAction;
    BtAddr *argPrt = (BtAddr *)msg.arg2_;
    if (argPrt == nullptr) {
        LOG_ERROR("%{public}s argPrt null", __PRETTY_FUNCTION__);
        return;
    }
    MapExecuteStatus resCode;
    if (msg.what_ == MSG_MCESERVICE_GET_SURPORT_FEATURES_FINISH) {
        devSupportedFeatrue = msg.arg1_;
        // call observer
        resCode = MapExecuteStatus::SUCCEED;
    } else {
        resCode = MapExecuteStatus::NOT_SUPPORT;
    }
    retAction.supportedFeatures_ = devSupportedFeatrue;
    retAction.action_ = MapActionType::GET_SUPPORTED_FEATURES;
    RawAddress btDevice = RawAddress::ConvertToString(argPrt->addr);
    serviceRpcCallbackMgr_.ExcuteObserverOnMapActionCompleted(btDevice.GetAddress(), retAction, resCode);
    // delete memory
    delete argPrt;
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceService::SetDefaultConfig(const MasInstanceConfig &configSave)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceDeviceMapMutex_);

    int saveId = insDefaultConfig_.deviceId;
    insDefaultConfig_ = configSave;
    insDefaultConfig_.deviceId = saveId;
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceService::GetConfigFromXml()
{
    LOG_DEBUG("%{public}s enter", __PRETTY_FUNCTION__);
    int value = 0;
    bool boolValue = 0;
    IAdapterConfig *config = AdapterConfig::GetInstance();

    if (!config->GetValue(SECTION_MAP_MCE_SERVICE, "MaxConnectedDevices", value)) {
        LOG_ERROR("%{public}s error:MaxConnectedDevices not found", __PRETTY_FUNCTION__);
    } else {
        insDefaultConfig_.maxOfDevice = value;
    }
    if (!config->GetValue(SECTION_MAP_MCE_SERVICE, PROPERTY_L2CAP_MTU, value)) {
        LOG_ERROR("%{public}s error:l2capMtu not found", __PRETTY_FUNCTION__);
    } else {
        insDefaultConfig_.l2capMtu = value;
    }
    if (!config->GetValue(SECTION_MAP_MCE_SERVICE, PROPERTY_RFCOMM_MTU, value)) {
        LOG_ERROR("%{public}s error:rfcomm Mtu not found", __PRETTY_FUNCTION__);
    } else {
        insDefaultConfig_.rfcommMtu = value;
    }
    if (!config->GetValue(SECTION_MAP_MCE_SERVICE, PROPERTY_MAP_MAX_OF_GET_UREAD, value)) {
        LOG_ERROR("%{public}s error:MaxOfGetUreadMessage not found", __PRETTY_FUNCTION__);
    } else {
        insDefaultConfig_.maxOfGetUnread = value;
    }
    if (!config->GetValue(SECTION_MAP_MCE_SERVICE, PROPERTY_SRM_ENABLE, boolValue)) {
        LOG_ERROR("%{public}s error:SrmEnable not found", __PRETTY_FUNCTION__);
    } else {
        insDefaultConfig_.isSupportSrm = boolValue;
    }
    HILOGI("maxOfDevice = %{public}d, l2capMtu = %{public}d, rfcommMtu = %{public}d, UreadNumber = %{public}d, "
        "EnableSrm_ = %{public}d", insDefaultConfig_.maxOfDevice, insDefaultConfig_.l2capMtu,
        insDefaultConfig_.rfcommMtu, insDefaultConfig_.maxOfGetUnread, insDefaultConfig_.isSupportSrm);
}

REGISTER_CLASS_CREATOR(MapMceService);
}  // namespace bluetooth
}  // namespace OHOS
