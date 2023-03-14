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
#include <codecvt>
#include <string>
#include <mutex>
#include <memory>

#include "bluetooth_map_mce_observer_stub.h"
#include "bluetooth_map_mce_proxy.h"
#include "bluetooth_raw_address.h"
#include "bluetooth_map_mce.h"

#include "bt_def.h"
#include "bluetooth_host.h"
#include "bluetooth_utils.h"
#include "bluetooth_remote_device.h"
#include "bluetooth_observer_list.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "i_bluetooth_host.h"
using namespace OHOS::bluetooth;

namespace OHOS {
namespace Bluetooth {
/**
 * @brief make observer for framework
 */
class BluetoothMapMceObserverImpl : public BluetoothMapMceObserverStub {
public:
    BluetoothMapMceObserverImpl(BluetoothObserverList<MapClientObserver> &observerList)
        : frameworkObserverList_(observerList) {};
    ~BluetoothMapMceObserverImpl() override = default;
    void OnConnectionStateChanged(
        const BluetoothRawAddress &deviceAddress, int state) override;
    void OnMapActionCompleted(const BluetoothRawAddress &deviceAddress,
        const BluetoothIProfileMapAction &action, int state) override;
    void OnMapEventReported(const BluetoothRawAddress &deviceAddress,
        const BluetoothIProfileMapEventReport &report) override;
    void OnBmessageCompleted(const BluetoothRawAddress &deviceAddress,
        const BluetoothIProfileBMessage &bmsg, int state) override;
    void OnMessagesListingCompleted(const BluetoothRawAddress &deviceAddress,
        const BluetoothIProfileMessagesListing &listing, int state) override;
    void OnConversationListingCompleted(
        const BluetoothRawAddress &deviceAddress,
        const BluetoothIProfileConversationListing &listing, int state) override;

private:
    MapVcard ConvertVcardToFramewrokFormat(IProfileMapVcard profileVcard);
    BluetoothObserverList<MapClientObserver> &frameworkObserverList_;
};

void BluetoothMapMceObserverImpl::OnConnectionStateChanged(
    const BluetoothRawAddress &deviceAddress, int state)
{
    BluetoothRemoteDevice remoteDevice(deviceAddress.GetAddress(), 0);
    frameworkObserverList_.ForEach([remoteDevice, state](std::shared_ptr<MapClientObserver> observer) {
        observer->OnConnectionStateChanged(remoteDevice, state);
    });
}

void BluetoothMapMceObserverImpl::OnMapActionCompleted(
    const BluetoothRawAddress &deviceAddress,
    const BluetoothIProfileMapAction &action, int state)
{
    BluetoothRemoteDevice remoteDevice(deviceAddress.GetAddress(), 0);
    MapAction faction;
    MapExecuteStatus fstate = MapExecuteStatus(state);

    // convert data
    faction.action_ = (OHOS::Bluetooth::MapActionType)(action.action_);
    faction.supportedFeatures_ = action.supportedFeatures_;
    faction.ownerStatus_.PresenceAvailability = action.ownerStatus_.PresenceAvailability;
    faction.ownerStatus_.PresenceText = action.ownerStatus_.PresenceText;
    faction.ownerStatus_.LastActivity = action.ownerStatus_.LastActivity;
    faction.ownerStatus_.ChatState = action.ownerStatus_.ChatState;

    frameworkObserverList_.ForEach([remoteDevice, faction, fstate](std::shared_ptr<MapClientObserver> observer) {
        observer->OnMapActionCompleted(remoteDevice, faction, fstate);
    });
}

MapVcard BluetoothMapMceObserverImpl::ConvertVcardToFramewrokFormat(IProfileMapVcard profileVcard)
{
    MapVcard frameworkVcard;
    frameworkVcard.VERSION = profileVcard.VERSION;    // shall be included
    frameworkVcard.N = profileVcard.N;                // shall be included
    frameworkVcard.TEL = profileVcard.TEL;            // may be used
    frameworkVcard.EMAIL = profileVcard.EMAIL;        // may be used
    frameworkVcard.X_BT_UID = profileVcard.X_BT_UID;  // bmsg V1.1
    frameworkVcard.X_BT_UCI = profileVcard.X_BT_UCI;  // bmsg V1.1
    frameworkVcard.FN = profileVcard.FN;              // vcard 3.0 , shall be included
    return frameworkVcard;
}

void BluetoothMapMceObserverImpl::OnBmessageCompleted(
    const BluetoothRawAddress &deviceAddress,
    const BluetoothIProfileBMessage &bmsg, int state)
{
    BluetoothRemoteDevice remoteDevice(deviceAddress.GetAddress(), 0);
    MapBMessage fBmsg;
    MapExecuteStatus fstate = MapExecuteStatus(state);

    // Bmessage
    fBmsg.bMessageStringObject_ = bmsg.bMessageStringObject_;
    IProfileBMessageStruct msg = bmsg.bMessageParam_;
    fBmsg.FractionDeliver = (OHOS::Bluetooth::MapFractionDeliverType)bmsg.FractionDeliver;
    fBmsg.bMessageParam_.version_property = msg.version_property;
    fBmsg.bMessageParam_.readstatus_property = (OHOS::Bluetooth::MapMessageStatus)msg.readstatus_property;
    fBmsg.bMessageParam_.type_property = (OHOS::Bluetooth::MapMessageType)msg.type_property;
    fBmsg.bMessageParam_.folder_property = msg.folder_property;
    fBmsg.bMessageParam_.extendeddata_property = msg.extendeddata_property;

    IProfileMapVcard profileVcard;
    MapVcard frameworkVcard;
    if (msg.originator_.size() == 1) {
        auto itVcard = msg.originator_.begin();
        profileVcard = *itVcard;
        frameworkVcard = ConvertVcardToFramewrokFormat(profileVcard);
        fBmsg.bMessageParam_.originator_.push_back(frameworkVcard);
    }
    for (auto it = msg.envelope_.recipientLevel1_.begin(); it != msg.envelope_.recipientLevel1_.end(); it++) {
        profileVcard = *it;
        frameworkVcard = ConvertVcardToFramewrokFormat(profileVcard);
        fBmsg.bMessageParam_.envelope_.recipientLevel1_.push_back(frameworkVcard);
    }
    for (auto it2 = msg.envelope_.recipientLevel2_.begin(); it2 != msg.envelope_.recipientLevel2_.end(); it2++) {
        profileVcard = *it2;
        frameworkVcard = ConvertVcardToFramewrokFormat(profileVcard);
        fBmsg.bMessageParam_.envelope_.recipientLevel2_.push_back(frameworkVcard);
    }
    for (auto it3 = msg.envelope_.recipientLevel3_.begin(); it3 != msg.envelope_.recipientLevel3_.end(); it3++) {
        profileVcard = *it3;
        frameworkVcard = ConvertVcardToFramewrokFormat(profileVcard);
        fBmsg.bMessageParam_.envelope_.recipientLevel3_.push_back(frameworkVcard);
    }

    fBmsg.bMessageParam_.envelope_.maxLevelOfEnvelope_ = msg.envelope_.maxLevelOfEnvelope_;
    fBmsg.bMessageParam_.envelope_.msgBody_.bodyPartID = msg.envelope_.msgBody_.bodyPartID;
    fBmsg.bMessageParam_.envelope_.msgBody_.body_encoding = msg.envelope_.msgBody_.body_encoding;
    fBmsg.bMessageParam_.envelope_.msgBody_.body_charset = msg.envelope_.msgBody_.body_charset;
    fBmsg.bMessageParam_.envelope_.msgBody_.body_language = msg.envelope_.msgBody_.body_language;
    fBmsg.bMessageParam_.envelope_.msgBody_.body_content = msg.envelope_.msgBody_.body_content;
    fBmsg.bMessageParam_.envelope_.msgBody_.body_content_length = msg.envelope_.msgBody_.body_content_length;

    frameworkObserverList_.ForEach([remoteDevice, fBmsg, fstate](std::shared_ptr<MapClientObserver> observer) {
        observer->OnBmessageCompleted(remoteDevice, fBmsg, fstate);
    });
}

void BluetoothMapMceObserverImpl::OnMessagesListingCompleted(
    const BluetoothRawAddress &deviceAddress,
    const BluetoothIProfileMessagesListing &listing, int state)
{
    BluetoothRemoteDevice remoteDevice(deviceAddress.GetAddress(), 0);
    MessagesListing fListing;
    MapExecuteStatus fstate = MapExecuteStatus(state);

    // message listing
    fListing.messagesListingStringObject_ = listing.messagesListingStringObject_;
    IProfileMessagesListingParamStruct serviceMsgListParam = listing.messagesListingParam_;
    fListing.messagesListingParam_.NewMessage = (OHOS::Bluetooth::MapOnOffType)serviceMsgListParam.NewMessage; // on off
    fListing.messagesListingParam_.MseTime =
        serviceMsgListParam.MseTime;  // String with current time basis and UTC-offset of the MSE.
    fListing.messagesListingParam_.ListingSize = serviceMsgListParam.ListingSize;
    fListing.messagesListingParam_.DatabaseIdentifier =
        serviceMsgListParam.DatabaseIdentifier;  // 128-bit value in hex string format,max 32 bytes
    fListing.messagesListingParam_.FolderVersionCounter =
        serviceMsgListParam.FolderVersionCounter;  // 128-bit value in hex string format,max 32 bytes
    fListing.messagesListingParam_.Version = serviceMsgListParam.Version;  // "1.1" or "1.0"
    IProfileMessageOutline outlineMsg;
    MessageOutline frameworkMsg;
    std::vector<IProfileMessageOutline> serviceMsgOutineList = listing.messageOutlineList_;
    for (auto it = serviceMsgOutineList.begin(); it != serviceMsgOutineList.end(); it++) {
        outlineMsg = *it;
        frameworkMsg.handle = outlineMsg.handle;
        frameworkMsg.subject = outlineMsg.subject;
        frameworkMsg.datetime = outlineMsg.datetime;
        frameworkMsg.sender_name = outlineMsg.sender_name;
        frameworkMsg.sender_addressing = outlineMsg.sender_addressing;
        frameworkMsg.replyto_addressing = outlineMsg.replyto_addressing;
        frameworkMsg.recipient_name = outlineMsg.recipient_name;
        frameworkMsg.recipient_addressing = outlineMsg.recipient_addressing;
        frameworkMsg.type = (OHOS::Bluetooth::MapMessageType)outlineMsg.type;
        frameworkMsg.receptionStatus = (OHOS::Bluetooth::MapMsgReceptionStatus)outlineMsg.receptionStatus;
        frameworkMsg.size = outlineMsg.size;
        frameworkMsg.attachment_size = outlineMsg.attachment_size;
        frameworkMsg.text = (OHOS::Bluetooth::MapBoolType)outlineMsg.text;
        frameworkMsg.read = (OHOS::Bluetooth::MapMessageStatus)outlineMsg.read;
        frameworkMsg.sent = (OHOS::Bluetooth::MapBoolType)outlineMsg.sent;
        frameworkMsg.protected_ = (OHOS::Bluetooth::MapBoolType)outlineMsg.protected_;
        frameworkMsg.priority = (OHOS::Bluetooth::MapBoolType)outlineMsg.priority;
        frameworkMsg.delivery_status = (OHOS::Bluetooth::MapMsgDeliveryStatus)outlineMsg.delivery_status;
        frameworkMsg.conversation_id = outlineMsg.conversation_id;
        frameworkMsg.conversation_name = outlineMsg.conversation_name;
        frameworkMsg.direction = (OHOS::Bluetooth::MapMsgDirection)outlineMsg.direction;
        frameworkMsg.attachment_mime_types = outlineMsg.attachment_mime_types;
        fListing.messageOutlineList_.push_back(frameworkMsg);
    }

    frameworkObserverList_.ForEach([remoteDevice, fListing, fstate](std::shared_ptr<MapClientObserver> observer) {
        observer->OnMessagesListingCompleted(remoteDevice, fListing, fstate);
    });
}

void BluetoothMapMceObserverImpl::OnConversationListingCompleted(
    const BluetoothRawAddress &deviceAddress,
    const BluetoothIProfileConversationListing &listing, int state)
{
    BluetoothRemoteDevice remoteDevice(deviceAddress.GetAddress(), 0);
    ConversationListing fListing;
    MapExecuteStatus fstate = MapExecuteStatus(state);

    // conversationList listing
    // save the object
    fListing.conversationListingStringObject_ = listing.conversationListingStringObject_;
    // save the param
    IProfileConversationListingParamStruct serviceParamStruct = listing.conversationListingParam_;
    fListing.conversationListingParam_.ConversationListingVersionCounter =
        serviceParamStruct.ConversationListingVersionCounter;  // 128-bit value in hex string format(max 32 bytes)
    fListing.conversationListingParam_.ListingSize = serviceParamStruct.ListingSize;
    fListing.conversationListingParam_.DatabaseIdentifier =
        serviceParamStruct.DatabaseIdentifier;  // 128-bit value in hex string format(max 32 bytes)
    fListing.conversationListingParam_.MseTime =
        serviceParamStruct.MseTime;  // String with current time basis and UTC-offset of the MSE. See Section 5.5.4
    fListing.conversationListingParam_.Version = serviceParamStruct.Version;
    // save the conversation list
    fListing.conversationOutlineList_.clear();
    Participant fParticipant;
    Conversation fConvoOutline;
    IProfileParticipant serviceParticipant;
    IProfileConversation serviceOutline;
    std::vector<IProfileConversation> serviceConvoList = listing.conversationOutlineList_;
    for (auto it = serviceConvoList.begin(); it != serviceConvoList.end(); it++) {
        serviceOutline = *it;
        // set value
        fConvoOutline.id = serviceOutline.id;
        fConvoOutline.name = serviceOutline.name;
        fConvoOutline.last_activity = serviceOutline.last_activity;
        fConvoOutline.read_status = serviceOutline.read_status;
        fConvoOutline.version_counter = serviceOutline.version_counter;
        fConvoOutline.summary = serviceOutline.summary;
        // init list
        fConvoOutline.participantList_.clear();
        for (auto it2 = serviceOutline.participantList_.begin(); it2 != serviceOutline.participantList_.end(); it2++) {
            serviceParticipant = *it2;
            fParticipant.uci = serviceParticipant.uci;
            fParticipant.display_name = serviceParticipant.display_name;
            fParticipant.chat_state = serviceParticipant.chat_state;
            fParticipant.last_activity = serviceParticipant.last_activity;
            fParticipant.x_bt_uid = serviceParticipant.x_bt_uid;
            fParticipant.name = serviceParticipant.name;
            fParticipant.presence_availability = serviceParticipant.presence_availability;
            fParticipant.presence_text = serviceParticipant.presence_text;
            fParticipant.priority = serviceParticipant.priority;
            fConvoOutline.participantList_.push_back(fParticipant);
        }
        fListing.conversationOutlineList_.push_back(fConvoOutline);
    }

    frameworkObserverList_.ForEach([remoteDevice, fListing, fstate](std::shared_ptr<MapClientObserver> observer) {
        observer->OnConversationListingCompleted(remoteDevice, fListing, fstate);
    });
}

void BluetoothMapMceObserverImpl::OnMapEventReported(
    const BluetoothRawAddress &deviceAddress,
    const BluetoothIProfileMapEventReport &report)
{
    BluetoothRemoteDevice remoteDevice(deviceAddress.GetAddress(), 0);
    MapEventReport fEventReport;

    fEventReport.type = report.type;
    fEventReport.handle = report.handle;
    fEventReport.folder = report.folder;
    fEventReport.old_folder = report.old_folder;
    fEventReport.msg_type = (OHOS::Bluetooth::MapMessageType)report.msg_type;
    fEventReport.datetime = report.datetime;
    fEventReport.subject = report.subject;
    fEventReport.sender_name = report.sender_name;
    fEventReport.priority = (OHOS::Bluetooth::MapBoolType)report.priority;
    fEventReport.conversation_name = report.conversation_name;
    fEventReport.conversation_id = report.conversation_id;
    fEventReport.presence_availability = report.presence_availability;
    fEventReport.presence_text = report.presence_text;
    fEventReport.last_activity = report.last_activity;
    fEventReport.chat_state = report.chat_state;
    fEventReport.read_status = (OHOS::Bluetooth::MapMessageStatus)report.read_status;
    fEventReport.extended_data = report.extended_data;
    fEventReport.participant_uci = report.participant_uci;
    fEventReport.contact_uid = report.contact_uid;
    fEventReport.version = report.version;
    fEventReport.masInstanceId_ = report.masInstanceId_;
    fEventReport.eventReportStringObject_ = report.eventReportStringObject_;

    frameworkObserverList_.ForEach([remoteDevice, fEventReport](std::shared_ptr<MapClientObserver> observer) {
        observer->OnMapEventReported(remoteDevice, fEventReport);
    });
}

struct MapClient::impl {
    impl();
    ~impl()
    {
        if (proxy_ != nullptr) {
            proxy_->DeregisterObserver(observerImp_);
            proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        }
    }
    sptr<IBluetoothMapMce> proxy_;
    class BluetoothMapMceDeathRecipient;
    sptr<BluetoothMapMceDeathRecipient> deathRecipient_ = nullptr;
    std::mutex mutex_;
    BluetoothObserverList<MapClientObserver> mapClientObserverList_;
    sptr<BluetoothMapMceObserverImpl> observerImp_ = new BluetoothMapMceObserverImpl(mapClientObserverList_);
    IProfileMapVcard ConvertVcardToProfileFormat(MapVcard frameworkVcard);
    void ConvertEnvelopeToProfileFormat(
        IProfileSendMessageParameters &iProfileMsg, const MapSendMessageParameters &msg);
};

class MapClient::impl::BluetoothMapMceDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    BluetoothMapMceDeathRecipient(MapClient::impl &MapMce) : MapMce_(MapMce) {};
    ~BluetoothMapMceDeathRecipient() final = default;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothMapMceDeathRecipient);

    void OnRemoteDied(const wptr<IRemoteObject> &remote) final
    {
        HILOGI("starts");
        MapMce_.proxy_->AsObject()->RemoveDeathRecipient(MapMce_.deathRecipient_);
        MapMce_.proxy_ = nullptr;
    }

private:
    MapClient::impl &MapMce_;
};

MapClient::impl::impl()
{
    HILOGI("starts");
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);

    if (!hostRemote) {
        HILOGE("failed: no hostRemote");
        return;
    }
    sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
    sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_MAP_MCE);

    if (!remote) {
        HILOGE("failed: no remote");
        return;
    }
    HILOGI("remote obtained");

    proxy_ = iface_cast<IBluetoothMapMce>(remote);
    if (proxy_ == nullptr) {
        return;
    }
    deathRecipient_ = new BluetoothMapMceDeathRecipient(*this);
    proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
    proxy_->RegisterObserver(observerImp_);
}

MapClient *MapClient::GetProfile()
{
    HILOGI("enter");
    static MapClient mapClient;
    return &mapClient;
}

MapClient::MapClient() : pimpl(nullptr)
{
    HILOGI("excute");
    pimpl = std::make_unique<impl>();
}

MapClient::~MapClient()
{
}

void MapClient::RegisterObserver(MapClientObserver &observer)
{
    HILOGI("enter");
    std::shared_ptr<MapClientObserver> pointer(&observer, [](MapClientObserver *) {});
    pimpl->mapClientObserverList_.Register(pointer);
}

void MapClient::DeregisterObserver(MapClientObserver &observer)
{
    HILOGI("enter");
    std::shared_ptr<MapClientObserver> pointer(&observer, [](MapClientObserver *) {});
    pimpl->mapClientObserverList_.Deregister(pointer);
}

bool MapClient::Connect(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int retInt = RET_BAD_PARAM;
    bool ret = false;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    BluetoothRawAddress rawAddress(device.GetDeviceAddr());
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        retInt = pimpl->proxy_->Connect(rawAddress);
        if (retInt == RET_NO_ERROR) {
            ret = true;
        }
    }
    return ret;
}

bool MapClient::Disconnect(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int retInt = RET_BAD_PARAM;
    bool ret = false;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    BluetoothRawAddress rawAddress(device.GetDeviceAddr());
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        retInt = pimpl->proxy_->Disconnect(rawAddress);
        if (retInt == RET_NO_ERROR) {
            ret = true;
        }
    }
    return ret;
}

bool MapClient::IsConnected(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int retInt = 0;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return false;
    }
    BluetoothRawAddress rawDevice(device.GetDeviceAddr());
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        retInt = pimpl->proxy_->IsConnected(rawDevice);
    }
    return (bool)retInt;
}

std::vector<BluetoothRemoteDevice> MapClient::GetConnectedDevices() const
{
    HILOGI("enter");
    std::vector<BluetoothRemoteDevice> btDeviceList;
    std::vector<BluetoothRawAddress> btDevice;
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        pimpl->proxy_->GetConnectDevices(btDevice);
        for (auto it = btDevice.begin(); it != btDevice.end(); it++) {
            btDeviceList.push_back(BluetoothRemoteDevice(it->GetAddress(), 0));
        }
    }
    return btDeviceList;
}

std::vector<BluetoothRemoteDevice> MapClient::GetDevicesByStates(const std::vector<int> &statesList) const
{
    HILOGI("enter");
    std::vector<BluetoothRemoteDevice> btDeviceList;
    std::vector<BluetoothRawAddress> btDevice;
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        pimpl->proxy_->GetDevicesByStates(statesList, btDevice);
        for (auto it = btDevice.begin(); it != btDevice.end(); it++) {
            btDeviceList.push_back(BluetoothRemoteDevice(it->GetAddress(), 0));
        }
    }
    return btDeviceList;
}

int MapClient::GetConnectionState(const BluetoothRemoteDevice &device) const
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int retState = (int)BTConnectState::DISCONNECTED;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return retState;
    }
    BluetoothRawAddress rawDevice(device.GetDeviceAddr());
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        retState = pimpl->proxy_->GetConnectionState(rawDevice);
    }
    return retState;
}

bool MapClient::SetConnectionStrategy(const BluetoothRemoteDevice &device, const int strategy)
{
    HILOGI("enter, device: %{public}s, device: %{public}d", GET_ENCRYPT_ADDR(device), strategy);
    int retInt = RET_BAD_PARAM;
    bool ret = false;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = RET_BAD_STATUS;
    BluetoothRawAddress rawDevice(device.GetDeviceAddr());
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        retInt = pimpl->proxy_->SetConnectionStrategy(rawDevice, strategy);
        if (retInt == RET_NO_ERROR) {
            ret = true;
        }
    }
    return ret;
}

int MapClient::GetConnectionStrategy(const BluetoothRemoteDevice &device) const
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int ret = (int)BTStrategyType::CONNECTION_FORBIDDEN;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = (int)BTStrategyType::CONNECTION_FORBIDDEN;
    BluetoothRawAddress rawDevice(device.GetDeviceAddr());
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        ret = pimpl->proxy_->GetConnectionStrategy(rawDevice);
    }
    return ret;
}

int MapClient::GetUnreadMessages(const BluetoothRemoteDevice &device, MapMessageType type, uint8_t max)
{
    HILOGI("enter, device: %{public}s, max: %{public}d", GET_ENCRYPT_ADDR(device), max);
    int ret = RET_BAD_PARAM;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = RET_BAD_STATUS;
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        BluetoothRawAddress rawDevice(device.GetDeviceAddr());
        ret = pimpl->proxy_->GetUnreadMessages(rawDevice, (int)type, max);
    }
    return ret;
}

int MapClient::GetSupportedFeatures(const BluetoothRemoteDevice &device) const
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int ret = RET_BAD_STATUS;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    BluetoothRawAddress rawDevice(device.GetDeviceAddr());
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        ret = pimpl->proxy_->GetSupportedFeatures(rawDevice);
    }
    return ret;
}


IProfileMapVcard MapClient::impl::ConvertVcardToProfileFormat(MapVcard frameVcard)
{
    HILOGI("enter");
    IProfileMapVcard serviceVcard;
    serviceVcard.VERSION = frameVcard.VERSION;    // shall be included
    serviceVcard.N = frameVcard.N;                // shall be included
    serviceVcard.TEL = frameVcard.TEL;            // may be used
    serviceVcard.EMAIL = frameVcard.EMAIL;        // may be used
    serviceVcard.X_BT_UID = frameVcard.X_BT_UID;  // bmsg V1.1
    serviceVcard.X_BT_UCI = frameVcard.X_BT_UCI;  // bmsg V1.1
    serviceVcard.FN = frameVcard.FN;              // vcard 3.0 , shall be included
    return serviceVcard;
}

void MapClient::impl::ConvertEnvelopeToProfileFormat(
    IProfileSendMessageParameters &iProfileMsg, const MapSendMessageParameters &msg)
{
    HILOGI("enter");
    MapVcard frameVcard;
    IProfileMapVcard serviceVcard;
    if (msg.message.originator_.size() == 1) {
        auto itVcard = msg.message.originator_.begin();
        frameVcard = *itVcard;
        serviceVcard = ConvertVcardToProfileFormat(frameVcard);
        iProfileMsg.bmessage_.originator_.push_back(serviceVcard);
    }
    for (auto it = msg.message.envelope_.recipientLevel1_.begin(); it != msg.message.envelope_.recipientLevel1_.end();
        it++) {
        frameVcard = *it;
        serviceVcard = ConvertVcardToProfileFormat(frameVcard);
        iProfileMsg.bmessage_.envelope_.recipientLevel1_.push_back(serviceVcard);
    }
    for (auto it2 = msg.message.envelope_.recipientLevel2_.begin();
        it2 != msg.message.envelope_.recipientLevel2_.end();
        it2++) {
        frameVcard = *it2;
        serviceVcard = ConvertVcardToProfileFormat(frameVcard);
        iProfileMsg.bmessage_.envelope_.recipientLevel2_.push_back(serviceVcard);
    }
    for (auto it3 = msg.message.envelope_.recipientLevel3_.begin();
        it3 != msg.message.envelope_.recipientLevel3_.end();
        it3++) {
        frameVcard = *it3;
        serviceVcard = ConvertVcardToProfileFormat(frameVcard);
        iProfileMsg.bmessage_.envelope_.recipientLevel3_.push_back(serviceVcard);
    }

    iProfileMsg.bmessage_.envelope_.maxLevelOfEnvelope_ = msg.message.envelope_.maxLevelOfEnvelope_;
    iProfileMsg.bmessage_.envelope_.msgBody_.bodyPartID = msg.message.envelope_.msgBody_.bodyPartID;
    iProfileMsg.bmessage_.envelope_.msgBody_.body_encoding = msg.message.envelope_.msgBody_.body_encoding;
    iProfileMsg.bmessage_.envelope_.msgBody_.body_charset = msg.message.envelope_.msgBody_.body_charset;
    iProfileMsg.bmessage_.envelope_.msgBody_.body_language = msg.message.envelope_.msgBody_.body_language;
    iProfileMsg.bmessage_.envelope_.msgBody_.body_content = msg.message.envelope_.msgBody_.body_content;
    iProfileMsg.bmessage_.envelope_.msgBody_.body_content_length = msg.message.envelope_.msgBody_.body_content_length;
}

int MapClient::SendMessage(const BluetoothRemoteDevice &device, const MapSendMessageParameters &msg)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int ret = RET_BAD_STATUS;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return RET_BAD_PARAM;
    }
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        BluetoothIProfileSendMessageParameters iProfileMsg;
        iProfileMsg.Charset = (bluetooth::MapCharsetType)msg.Charset;
        iProfileMsg.ConversationID = msg.ConversationID;
        iProfileMsg.MessageHandle = msg.MessageHandle;
        iProfileMsg.Attachment = (bluetooth::MapAttachmentType)msg.Attachment;
        iProfileMsg.ModifyText = (bluetooth::MapModifyTextType)msg.ModifyText;
        iProfileMsg.Retry = (bluetooth::MapOnOffType)msg.Retry;
        iProfileMsg.Transparent = (bluetooth::MapOnOffType)msg.Transparent;
        iProfileMsg.bmessage_.version_property = msg.message.version_property;
        iProfileMsg.bmessage_.readstatus_property = (bluetooth::MapMessageStatus)msg.message.readstatus_property;
        iProfileMsg.bmessage_.type_property = (bluetooth::MapMessageType)msg.message.type_property;
        iProfileMsg.bmessage_.folder_property = msg.message.folder_property;
        iProfileMsg.bmessage_.extendeddata_property = msg.message.extendeddata_property;

        pimpl->ConvertEnvelopeToProfileFormat(iProfileMsg, msg);

        BluetoothRawAddress rawDevice(device.GetDeviceAddr());
        ret = pimpl->proxy_->SendMessage(rawDevice, iProfileMsg);
    }
    return ret;
}

int MapClient::SetNotificationFilter(const BluetoothRemoteDevice &device, const int mask)
{
    HILOGI("enter, device: %{public}s, mask: %{public}d", GET_ENCRYPT_ADDR(device), mask);
    int ret = RET_BAD_PARAM;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = RET_BAD_STATUS;
    BluetoothRawAddress rawDevice(device.GetDeviceAddr());
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        ret = pimpl->proxy_->SetNotificationFilter(rawDevice, mask);
    }
    return ret;
}

int MapClient::GetMessagesListing(const BluetoothRemoteDevice &device, const GetMessagesListingParameters &para) const
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int ret = RET_BAD_PARAM;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = RET_BAD_STATUS;
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        BluetoothRawAddress rawDevice(device.GetDeviceAddr());
        BluetoothIProfileGetMessagesListingParameters servicePara;
        servicePara.folder = para.folder;
        servicePara.MaxListCount = para.MaxListCount;
        servicePara.ListStartOffset = para.ListStartOffset;
        servicePara.SubjectLength = para.SubjectLength;
        servicePara.ParameterMask = para.ParameterMask;
        servicePara.FilterMessageType = para.FilterMessageType;
        servicePara.FilterPeriodBegin = para.FilterPeriodBegin;
        servicePara.FilterPeriodEnd = para.FilterPeriodEnd;
        servicePara.FilterReadStatus = para.FilterReadStatus;
        servicePara.FilterRecipient = para.FilterRecipient;
        servicePara.FilterOriginator = para.FilterOriginator;
        servicePara.FilterPriority = para.FilterPriority;
        servicePara.ConversationID = para.ConversationID;
        servicePara.FilterMessageHandle = para.FilterMessageHandle;
        ret = pimpl->proxy_->GetMessagesListing(rawDevice, servicePara);
    }
    return ret;
}

int MapClient::GetMessage(const BluetoothRemoteDevice &device, MapMessageType type, const std::u16string &msgHandle,
    const GetMessageParameters &para) const
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    std::string u8Str;
    int ret = RET_BAD_PARAM;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = RET_BAD_STATUS;
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        BluetoothRawAddress rawDevice(device.GetDeviceAddr());
        BluetoothIProfileGetMessageParameters servicePara;
        servicePara.Attachment = (bluetooth::MapAttachmentType)para.Attachment;
        servicePara.Charset = (bluetooth::MapCharsetType)para.Charset;
        servicePara.FractionRequest = (bluetooth::MapFractionRequestType)para.FractionRequest;

        ret = pimpl->proxy_->GetMessage(
            rawDevice, (int)type, msgHandle, servicePara);
    }
    return ret;
}

int MapClient::UpdateInbox(const BluetoothRemoteDevice &device, MapMessageType type)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int ret = RET_BAD_PARAM;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = RET_BAD_STATUS;
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        BluetoothRawAddress rawDevice(device.GetDeviceAddr());
        ret = pimpl->proxy_->UpdateInbox(rawDevice, (int)type);
    }
    return ret;
}

int MapClient::GetConversationListing(
    const BluetoothRemoteDevice &device, const GetConversationListingParameters &para) const
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int ret = RET_BAD_PARAM;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = RET_BAD_STATUS;
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        BluetoothRawAddress rawDevice(device.GetDeviceAddr());
        BluetoothIProfileGetConversationListingParameters servicePara;
        servicePara.MaxListCount = para.MaxListCount;
        servicePara.ListStartOffset = para.ListStartOffset;
        servicePara.FilterReadStatus = para.FilterReadStatus;
        servicePara.FilterRecipient = para.FilterRecipient;
        servicePara.ConversationID = para.ConversationID;
        servicePara.FilterLastActivityBegin = para.FilterLastActivityBegin;
        servicePara.FilterLastActivityEnd = para.FilterLastActivityEnd;
        servicePara.ConvParameterMask = para.ConvParameterMask;

        ret = pimpl->proxy_->GetConversationListing(rawDevice, servicePara);
    }
    return ret;
}

int MapClient::SetMessageStatus(
    const BluetoothRemoteDevice &device, MapMessageType type, const MapSetMessageStatus &msgStatus)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int ret = RET_BAD_PARAM;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = RET_BAD_STATUS;
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        BluetoothRawAddress rawDevice(device.GetDeviceAddr());

        ret = pimpl->proxy_->SetMessageStatus(rawDevice,
            (int)type,
            msgStatus.msgHandle,
            (int)msgStatus.statusIndicator,
            (int)msgStatus.statusValue,
            msgStatus.extendedData);
    }
    return ret;
}

int MapClient::SetOwnerStatus(const BluetoothRemoteDevice &device, const SetOwnerStatusParameters &para)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    int ret = RET_BAD_PARAM;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = RET_BAD_STATUS;
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        BluetoothIProfileSetOwnerStatusParameters servicePara;
        servicePara.ConversationID = para.conversationId_;
        servicePara.ownerStatus_.PresenceAvailability = para.ownerStatus_.PresenceAvailability;
        servicePara.ownerStatus_.PresenceText = para.ownerStatus_.PresenceText;  // utf8
        servicePara.ownerStatus_.LastActivity = para.ownerStatus_.LastActivity;  // utf8
        servicePara.ownerStatus_.ChatState = para.ownerStatus_.ChatState;

        BluetoothRawAddress rawDevice(device.GetDeviceAddr());

        ret = pimpl->proxy_->SetOwnerStatus(rawDevice, servicePara);
    }
    return ret;
}

int MapClient::GetOwnerStatus(const BluetoothRemoteDevice &device, const std::string &conversationId) const
{
    HILOGI("enter, device: %{public}s, conversationId: %{public}s", GET_ENCRYPT_ADDR(device), conversationId.c_str());
    int ret = RET_BAD_PARAM;
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return ret;
    }
    ret = RET_BAD_STATUS;
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        BluetoothRawAddress rawDevice(device.GetDeviceAddr());
        ret = pimpl->proxy_->GetOwnerStatus(rawDevice, conversationId);
    }
    return ret;
}

MapMasInstanceInfoList MapClient::GetMasInstanceInfo(const BluetoothRemoteDevice &device) const
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    MapMasInstanceInfoList frameworkMasInfoList;
    frameworkMasInfoList.isValid = false;
    BluetoothIProfileMasInstanceInfoList infoList;
    std::vector<MapMasInstanceInfo> instanceinfoList;

    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("BluetoothRemoteDevice error");
        return frameworkMasInfoList;
    }
    // process
    if ((pimpl->proxy_ != nullptr) && IS_BT_ENABLED()) {
        BluetoothRawAddress rawDevice(device.GetDeviceAddr());
        BluetoothIProfileMasInstanceInfoList infoList = pimpl->proxy_->GetMasInstanceInfo(rawDevice);

        HILOGI("Test!");
        if (infoList.isValid == true) {
            MapMasInstanceInfo info;
            frameworkMasInfoList.isValid = true;
            for (auto it = infoList.masInfoList.begin(); it != infoList.masInfoList.end(); it++) {
                info.OwnerUCI = it->OwnerUCI;
                info.MASInstanceInformation = it->MASInstanceInformation;
                info.supportedMsgTypes_ = it->supportedMsgTypes_;
                info.instanceId = it->instanceId;
                frameworkMasInfoList.masInfoList.push_back(info);
            }
        }
    }
    return frameworkMasInfoList;
}
}  // namespace Bluetooth
}  // namespace OHOS
