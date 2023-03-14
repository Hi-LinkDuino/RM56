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

#include "bluetooth_map_mce_parameter.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothIProfileMapAction::Marshalling(Parcel &parcel) const
{
    bool status;
    status = parcel.WriteInt32((int)action_);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32(supportedFeatures_);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint8(ownerStatus_.PresenceAvailability);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(ownerStatus_.PresenceText);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(ownerStatus_.LastActivity);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint8(ownerStatus_.ChatState);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileMapAction::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileMapAction *BluetoothIProfileMapAction::Unmarshalling(Parcel &parcel)
{
    BluetoothIProfileMapAction *obj = new BluetoothIProfileMapAction();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileMapAction::ReadFromParcel(Parcel &parcel)
{
    bool status;
    int action;

    status = parcel.ReadInt32(action);
    if (!status) {
        return status;
    }
    action_ = bluetooth::MapActionType(action);

    status = parcel.ReadInt32(supportedFeatures_);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint8(ownerStatus_.PresenceAvailability);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(ownerStatus_.PresenceText);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(ownerStatus_.LastActivity);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint8(ownerStatus_.ChatState);
    return status;
}

bool BluetoothIProfileMasInstanceInfoList::Marshalling(Parcel &parcel) const
{
    bool status;

    status = parcel.WriteBool(isValid);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)masInfoList.size());
    if (!status) {
        return status;
    }

    bluetooth::IProfileMasInstanceInfo info;
    for (auto it = masInfoList.begin(); it != masInfoList.end(); it++) {
        info = *it;
        status = parcel.WriteString(info.OwnerUCI);
        if (!status)
            return status;

        status = parcel.WriteString(info.MASInstanceInformation);
        if (!status)
            return status;

        status = parcel.WriteInt32(info.supportedMsgTypes_);
        if (!status)
            return status;

        status = parcel.WriteInt32(info.instanceId);
        if (!status)
            return status;
    }
    return status;
}

bool BluetoothIProfileMasInstanceInfoList::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileMasInstanceInfoList *BluetoothIProfileMasInstanceInfoList::Unmarshalling(Parcel &parcel)
{
    BluetoothIProfileMasInstanceInfoList *obj = new BluetoothIProfileMasInstanceInfoList();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileMasInstanceInfoList::ReadFromParcel(Parcel &parcel)
{
    bool status;
    int tempInt;

    status = parcel.ReadBool(isValid);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }

    bluetooth::IProfileMasInstanceInfo tempInfo;
    for (int i = 0; i < tempInt; i++) {
        status = parcel.ReadString(tempInfo.OwnerUCI);
        if (!status) {
            return status;
        }

        status = parcel.ReadString(tempInfo.MASInstanceInformation);
        if (!status) {
            return status;
        }

        status = parcel.ReadInt32(tempInfo.supportedMsgTypes_);
        if (!status) {
            return status;
        }

        status = parcel.ReadInt32(tempInfo.instanceId);
        if (!status) {
            return status;
        }

        masInfoList.push_back(tempInfo);
    }
    return status;
}

bool BluetoothIProfileBMessageStruct::WriteToParcelIProfileMapVcard(
    Parcel &parcel, const bluetooth::IProfileMapVcard &vcard) const
{
    bool status;

    status = parcel.WriteString(vcard.VERSION);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(vcard.N);
    if (!status) {
        return status;
    }

    status = parcel.WriteStringVector(vcard.TEL);
    if (!status) {
        return status;
    }

    status = parcel.WriteStringVector(vcard.EMAIL);
    if (!status) {
        return status;
    }

    status = parcel.WriteStringVector(vcard.X_BT_UID);
    if (!status) {
        return status;
    }

    status = parcel.WriteStringVector(vcard.X_BT_UCI);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(vcard.FN);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileBMessageStruct::ReadFromParcelIProfileMapVcard(
    Parcel &parcel, bluetooth::IProfileMapVcard &vcard)
{
    bool status;

    status = parcel.ReadString(vcard.VERSION);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(vcard.N);
    if (!status) {
        return status;
    }

    status = parcel.ReadStringVector(&(vcard.TEL));
    if (!status) {
        return status;
    }

    status = parcel.ReadStringVector(&(vcard.EMAIL));
    if (!status) {
        return status;
    }

    status = parcel.ReadStringVector(&(vcard.X_BT_UID));
    if (!status) {
        return status;
    }

    status = parcel.ReadStringVector(&(vcard.X_BT_UCI));
    if (!status) {
        return status;
    }

    status = parcel.ReadString(vcard.FN);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileBMessageStruct::WriteToParcel(
    Parcel &parcel, const bluetooth::IProfileBMessageStruct &msgStruct) const
{
    bool status;
    bluetooth::IProfileMapVcard vcard;

    status = parcel.WriteString(msgStruct.version_property);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)msgStruct.readstatus_property);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)msgStruct.type_property);
    if (!status) {
        return status;
    }

    status = parcel.WriteString16(msgStruct.folder_property);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(msgStruct.extendeddata_property);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)msgStruct.originator_.size());
    if (!status) {
        return status;
    }

    if (msgStruct.originator_.size() != 0) {
        vcard = *(msgStruct.originator_.begin());
        status = WriteToParcelIProfileMapVcard(parcel, vcard);
        if (!status)
            return status;
    }

    status = parcel.WriteInt32((int)msgStruct.envelope_.recipientLevel1_.size());
    if (!status) {
        return status;
    }

    for (auto it = msgStruct.envelope_.recipientLevel1_.begin(); it != msgStruct.envelope_.recipientLevel1_.end();
         it++) {
        vcard = *it;
        status = WriteToParcelIProfileMapVcard(parcel, vcard);
        if (!status)
            return status;
    }

    status = parcel.WriteInt32((int)msgStruct.envelope_.recipientLevel2_.size());
    if (!status) {
        return status;
    }

    for (auto it2 = msgStruct.envelope_.recipientLevel2_.begin(); it2 != msgStruct.envelope_.recipientLevel2_.end();
         it2++) {
        vcard = *it2;
        status = WriteToParcelIProfileMapVcard(parcel, vcard);
        if (!status)
            return status;
    }

    status = parcel.WriteInt32((int)msgStruct.envelope_.recipientLevel3_.size());
    if (!status) {
        return status;
    }

    for (auto it3 = msgStruct.envelope_.recipientLevel3_.begin(); it3 != msgStruct.envelope_.recipientLevel3_.end();
         it3++) {
        vcard = *it3;
        status = WriteToParcelIProfileMapVcard(parcel, vcard);
        if (!status)
            return status;
    }
    status = parcel.WriteInt32((int)msgStruct.envelope_.maxLevelOfEnvelope_);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(msgStruct.envelope_.msgBody_.bodyPartID);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(msgStruct.envelope_.msgBody_.body_encoding);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(msgStruct.envelope_.msgBody_.body_charset);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(msgStruct.envelope_.msgBody_.body_language);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)msgStruct.envelope_.msgBody_.body_content_length);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(msgStruct.envelope_.msgBody_.body_content);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileBMessageStruct::ReadFromParcel(
    Parcel &parcel, bluetooth::IProfileBMessageStruct &msgStruct)
{
    bool status;
    int tempInt;
    bluetooth::IProfileMapVcard vcard;
    int i;

    status = parcel.ReadString(msgStruct.version_property);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    msgStruct.readstatus_property = bluetooth::MapMessageStatus(tempInt);

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    msgStruct.type_property = bluetooth::MapMessageType(tempInt);

    status = parcel.ReadString16(msgStruct.folder_property);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(msgStruct.extendeddata_property);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }

    if (tempInt != 0) {
        ReadFromParcelIProfileMapVcard(parcel, vcard);
        msgStruct.originator_.push_back(vcard);
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }

    for (i = 0; i < tempInt; i++) {
        ReadFromParcelIProfileMapVcard(parcel, vcard);
        msgStruct.envelope_.recipientLevel1_.push_back(vcard);
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }

    for (i = 0; i < tempInt; i++) {
        ReadFromParcelIProfileMapVcard(parcel, vcard);
        msgStruct.envelope_.recipientLevel2_.push_back(vcard);
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }

    for (i = 0; i < tempInt; i++) {
        ReadFromParcelIProfileMapVcard(parcel, vcard);
        msgStruct.envelope_.recipientLevel3_.push_back(vcard);
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    msgStruct.envelope_.maxLevelOfEnvelope_ = tempInt;

    status = parcel.ReadString(msgStruct.envelope_.msgBody_.bodyPartID);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(msgStruct.envelope_.msgBody_.body_encoding);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(msgStruct.envelope_.msgBody_.body_charset);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(msgStruct.envelope_.msgBody_.body_language);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    msgStruct.envelope_.msgBody_.body_content_length = tempInt;

    status = parcel.ReadString(msgStruct.envelope_.msgBody_.body_content);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileSendMessageParameters::Marshalling(Parcel &parcel) const
{
    bool status;

    status = parcel.WriteString(ConversationID);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)Attachment);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)Charset);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)Transparent);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)Retry);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(MessageHandle);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)ModifyText);
    if (!status) {
        return status;
    }

    BluetoothIProfileBMessageStruct bMessageStruct;
    status = bMessageStruct.WriteToParcel(parcel, bmessage_);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileSendMessageParameters::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileSendMessageParameters *BluetoothIProfileSendMessageParameters::Unmarshalling(Parcel &parcel)
{
    BluetoothIProfileSendMessageParameters *obj = new BluetoothIProfileSendMessageParameters();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileSendMessageParameters::ReadFromParcel(Parcel &parcel)
{
    std::string tempStr;
    bool status;
    int tempInt;

    status = parcel.ReadString(ConversationID);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    Attachment = bluetooth::MapAttachmentType(tempInt);

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    Charset = bluetooth::MapCharsetType(tempInt);

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    Transparent = bluetooth::MapOnOffType(tempInt);

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    Retry = bluetooth::MapOnOffType(tempInt);

    status = parcel.ReadString(MessageHandle);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    ModifyText = bluetooth::MapModifyTextType(tempInt);

    BluetoothIProfileBMessageStruct bMessageStruct;
    status = bMessageStruct.ReadFromParcel(parcel, bmessage_);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileSetOwnerStatusParameters::Marshalling(Parcel &parcel) const
{
    bool status;

    status = parcel.WriteString(ConversationID);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint8(ownerStatus_.PresenceAvailability);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(ownerStatus_.PresenceText);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(ownerStatus_.LastActivity);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint32(ownerStatus_.ChatState);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileSetOwnerStatusParameters::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileSetOwnerStatusParameters *BluetoothIProfileSetOwnerStatusParameters::Unmarshalling(Parcel &parcel)
{
    BluetoothIProfileSetOwnerStatusParameters *obj = new BluetoothIProfileSetOwnerStatusParameters();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileSetOwnerStatusParameters::ReadFromParcel(Parcel &parcel)
{
    bool status;

    status = parcel.ReadString(ConversationID);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint8(ownerStatus_.PresenceAvailability);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(ownerStatus_.PresenceText);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(ownerStatus_.LastActivity);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint8(ownerStatus_.ChatState);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileGetConversationListingParameters::Marshalling(Parcel &parcel) const
{
    bool status;

    status = parcel.WriteUint16(MaxListCount);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint16(ListStartOffset);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint8(FilterReadStatus);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(FilterRecipient);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(ConversationID);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(FilterLastActivityBegin);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(FilterLastActivityEnd);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint32(ConvParameterMask);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileGetConversationListingParameters::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileGetConversationListingParameters *BluetoothIProfileGetConversationListingParameters::Unmarshalling(
    Parcel &parcel)
{
    BluetoothIProfileGetConversationListingParameters *obj =
        new BluetoothIProfileGetConversationListingParameters();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileGetConversationListingParameters::ReadFromParcel(Parcel &parcel)
{
    bool status;

    status = parcel.ReadUint16(MaxListCount);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint16(ListStartOffset);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint8(FilterReadStatus);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(FilterRecipient);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(ConversationID);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(FilterLastActivityBegin);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(FilterLastActivityEnd);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint32(ConvParameterMask);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileGetMessageParameters::Marshalling(Parcel &parcel) const
{
    bool status;

    status = parcel.WriteInt32((int)Attachment);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)Charset);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)FractionRequest);
    return status;
}

bool BluetoothIProfileGetMessageParameters::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileGetMessageParameters *BluetoothIProfileGetMessageParameters::Unmarshalling(Parcel &parcel)
{
    BluetoothIProfileGetMessageParameters *obj = new BluetoothIProfileGetMessageParameters();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileGetMessageParameters::ReadFromParcel(Parcel &parcel)
{
    bool status;
    int tempInt;

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    Attachment = bluetooth::MapAttachmentType(tempInt);

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    Charset = bluetooth::MapCharsetType(tempInt);

    status = parcel.ReadInt32(tempInt);
    FractionRequest = bluetooth::MapFractionRequestType(tempInt);
    return status;
}

bool BluetoothIProfileGetMessagesListingParameters::Marshalling(Parcel &parcel) const
{
    bool status;

    status = parcel.WriteString16(folder);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint16(MaxListCount);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint16(ListStartOffset);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint8(SubjectLength);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint32(ParameterMask);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint8(FilterMessageType);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(FilterPeriodBegin);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(FilterPeriodEnd);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint8(FilterReadStatus);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(FilterRecipient);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(FilterOriginator);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint8(FilterPriority);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(ConversationID);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(FilterMessageHandle);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileGetMessagesListingParameters::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileGetMessagesListingParameters *BluetoothIProfileGetMessagesListingParameters::Unmarshalling(
    Parcel &parcel)
{
    BluetoothIProfileGetMessagesListingParameters *obj = new BluetoothIProfileGetMessagesListingParameters();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileGetMessagesListingParameters::ReadFromParcel(Parcel &parcel)
{
    bool status;

    status = parcel.ReadString16(folder);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint16(MaxListCount);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint16(ListStartOffset);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint8(SubjectLength);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint32(ParameterMask);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint8(FilterMessageType);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(FilterPeriodBegin);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(FilterPeriodEnd);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint8(FilterReadStatus);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(FilterRecipient);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(FilterOriginator);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint8(FilterPriority);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(ConversationID);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(FilterMessageHandle);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileBMessage::Marshalling(Parcel &parcel) const
{
    bool status;

    status = parcel.WriteInt32((int)FractionDeliver);
    if (!status) {
        return status;
    }

    BluetoothIProfileBMessageStruct bMessageStruct;
    status = bMessageStruct.WriteToParcel(parcel, bMessageParam_);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(bMessageStringObject_);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileBMessage::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileBMessage *BluetoothIProfileBMessage::Unmarshalling(Parcel &parcel)
{
    BluetoothIProfileBMessage *obj = new BluetoothIProfileBMessage();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileBMessage::ReadFromParcel(Parcel &parcel)
{
    bool status;
    int tmpInt;

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    FractionDeliver = bluetooth::MapFractionDeliverType(tmpInt);

    BluetoothIProfileBMessageStruct bMessageStruct;
    status = bMessageStruct.ReadFromParcel(parcel, bMessageParam_);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(bMessageStringObject_);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileMessagesListing::WriteToParcelIProfileMessageOutline(
    Parcel &parcel, const bluetooth::IProfileMessageOutline &outline) const
{
    bool status;

    status = parcel.WriteString(outline.handle);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.subject);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.datetime);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.sender_name);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.sender_addressing);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.replyto_addressing);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.recipient_name);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.recipient_addressing);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.type);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.receptionStatus);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.size);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.attachment_size);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.text);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.read);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.sent);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.protected_);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.priority);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.delivery_status);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.conversation_id);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.conversation_name);
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32((int)outline.direction);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.attachment_mime_types);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileMessagesListing::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileMessagesListing *BluetoothIProfileMessagesListing::Unmarshalling(Parcel &parcel)
{
    BluetoothIProfileMessagesListing *obj = new BluetoothIProfileMessagesListing();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileMessagesListing::ReadFromParcelIProfileMessageOutline(
    Parcel &parcel, bluetooth::IProfileMessageOutline &outline)
{
    bool status;
    int tmpInt;

    status = parcel.ReadString(outline.handle);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.subject);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.datetime);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.sender_name);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.sender_addressing);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.replyto_addressing);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.recipient_name);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.recipient_addressing);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.type = bluetooth::MapMessageType(tmpInt);

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.receptionStatus = bluetooth::MapMsgReceptionStatus(tmpInt);

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.size = tmpInt;

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.attachment_size = tmpInt;

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.text = bluetooth::MapBoolType(tmpInt);

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.read = bluetooth::MapMessageStatus(tmpInt);

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.sent = bluetooth::MapBoolType(tmpInt);

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.protected_ = bluetooth::MapBoolType(tmpInt);

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.priority = bluetooth::MapBoolType(tmpInt);

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.delivery_status = bluetooth::MapMsgDeliveryStatus(tmpInt);

    status = parcel.ReadString(outline.conversation_id);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.conversation_name);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    outline.direction = bluetooth::MapMsgDirection(tmpInt);

    status = parcel.ReadString(outline.attachment_mime_types);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileMessagesListing::Marshalling(Parcel &parcel) const
{
    bool status;

    // save size first
    status = parcel.WriteInt32((int)messageOutlineList_.size());
    if (!status) {
        return status;
    }

    // save the IProfileMessageOutline
    bluetooth::IProfileMessageOutline msgOutline;
    for (auto it = messageOutlineList_.begin(); it != messageOutlineList_.end(); it++) {
        msgOutline = *it;
        status = WriteToParcelIProfileMessageOutline(parcel, msgOutline);
        if (!status)
            return status;
    }

    status = parcel.WriteUint8((uint8_t)messagesListingParam_.NewMessage);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(messagesListingParam_.MseTime);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint16(messagesListingParam_.ListingSize);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(messagesListingParam_.DatabaseIdentifier);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(messagesListingParam_.FolderVersionCounter);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(messagesListingParam_.Version);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(messagesListingStringObject_);
    return status;
}

bool BluetoothIProfileMessagesListing::ReadFromParcel(Parcel &parcel)
{
    bool status;
    int tmpInt;

    // get the outlineListSize first
    status = parcel.ReadInt32(outlineListSize);
    if (!status) {
        return status;
    }

    // get the outline list
    bluetooth::IProfileMessageOutline msgOutline;
    for (int i = 0; i < outlineListSize; i++) {
        ReadFromParcelIProfileMessageOutline(parcel, msgOutline);
        messageOutlineList_.push_back(msgOutline);
    }

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    messagesListingParam_.NewMessage = bluetooth::MapOnOffType(tmpInt);

    status = parcel.ReadString(messagesListingParam_.MseTime);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint16(messagesListingParam_.ListingSize);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(messagesListingParam_.DatabaseIdentifier);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(messagesListingParam_.FolderVersionCounter);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(messagesListingParam_.Version);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(messagesListingStringObject_);
    if (!status) {
        return status;
    }
    return status;
}

bool BluetoothIProfileConversationListing::WriteToParcelIProfileConversation(
    Parcel &parcel, const bluetooth::IProfileConversation &outline) const
{
    bool status;

    status = parcel.WriteString(outline.id);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.name);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.last_activity);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.read_status);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.version_counter);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.summary);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)outline.participantList_.size());
    if (!status) {
        return status;
    }

    bluetooth::IProfileParticipant participant;
    for (auto it = outline.participantList_.begin(); it != outline.participantList_.end(); it++) {
        participant = *it;
        WriteToParcelIProfileParticipant(parcel, participant);
    }

    return status;
}

bool BluetoothIProfileConversationListing::ReadFromParcelIProfileConversation(
    Parcel &parcel, bluetooth::IProfileConversation &outline)
{
    bool status;
    int tmpInt;

    status = parcel.ReadString(outline.id);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.name);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.last_activity);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.read_status);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.version_counter);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.summary);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    participantListNum_ = tmpInt;

    bluetooth::IProfileParticipant participant;
    for (int i = 0; i < participantListNum_; i++) {
        ReadFromParcelIProfileParticipant(parcel, participant);
        outline.participantList_.push_back(participant);
    }

    return status;
}

bool BluetoothIProfileConversationListing::WriteToParcelIProfileParticipant(
    Parcel &parcel, const bluetooth::IProfileParticipant &outline) const
{
    bool status;

    status = parcel.WriteString(outline.uci);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.display_name);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.chat_state);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.last_activity);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.x_bt_uid);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.name);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.presence_availability);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.presence_text);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(outline.priority);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileConversationListing::ReadFromParcelIProfileParticipant(
    Parcel &parcel, bluetooth::IProfileParticipant &outline)
{
    bool status;

    status = parcel.ReadString(outline.uci);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.display_name);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.chat_state);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.last_activity);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.x_bt_uid);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.name);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.presence_availability);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.presence_text);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(outline.priority);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileConversationListing::Marshalling(Parcel &parcel) const
{
    bool status;

    status = parcel.WriteInt32((int)conversationOutlineList_.size());
    if (!status) {
        return status;
    }

    bluetooth::IProfileConversation conversation;
    for (auto it = conversationOutlineList_.begin(); it != conversationOutlineList_.end(); it++) {
        conversation = *it;
        WriteToParcelIProfileConversation(parcel, conversation);
    }

    status = parcel.WriteString(conversationListingParam_.ConversationListingVersionCounter);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint16(conversationListingParam_.ListingSize);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(conversationListingParam_.DatabaseIdentifier);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(conversationListingParam_.MseTime);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(conversationListingParam_.Version);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(conversationListingStringObject_);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileConversationListing::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileConversationListing *BluetoothIProfileConversationListing::Unmarshalling(Parcel &parcel)
{
    BluetoothIProfileConversationListing *obj = new BluetoothIProfileConversationListing();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileConversationListing::ReadFromParcel(Parcel &parcel)
{
    bool status;
    int tmpInt;

    status = parcel.ReadInt32(tmpInt);
    if (!status) {
        return status;
    }
    conversationOutlineListNum_ = tmpInt;

    bluetooth::IProfileConversation conversation;
    for (int i = 0; i < conversationOutlineListNum_; i++) {
        ReadFromParcelIProfileConversation(parcel, conversation);
        conversationOutlineList_.push_back(conversation);
    }

    status = parcel.ReadString(conversationListingParam_.ConversationListingVersionCounter);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint16(conversationListingParam_.ListingSize);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(conversationListingParam_.DatabaseIdentifier);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(conversationListingParam_.MseTime);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(conversationListingParam_.Version);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(conversationListingStringObject_);
    if (!status) {
        return status;
    }

    return status;
}

bool BluetoothIProfileMapEventReport::Marshalling(Parcel &parcel) const
{
    bool status;
    std::string u8Str;
    status = parcel.WriteString(type);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(handle);
    if (!status) {
        return status;
    }

    status = parcel.WriteString16(folder);
    if (!status) {
        return status;
    }

    status = parcel.WriteString16(old_folder);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)msg_type);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(datetime);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(subject);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(sender_name);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)priority);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(conversation_name);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(conversation_id);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(presence_availability);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(presence_text);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(last_activity);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(chat_state);
    if (!status) {
        return status;
    }

    status = parcel.WriteInt32((int)read_status);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(extended_data);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(participant_uci);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(contact_uid);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(version);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint8(masInstanceId_);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(eventReportStringObject_);
    if (!status) {
        return status;
    }
    return status;
}

bool BluetoothIProfileMapEventReport::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothIProfileMapEventReport *BluetoothIProfileMapEventReport::Unmarshalling(Parcel &parcel)
{
    BluetoothIProfileMapEventReport *obj = new BluetoothIProfileMapEventReport();
    if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
        delete obj;
        obj = nullptr;
    }
    return obj;
}

bool BluetoothIProfileMapEventReport::ReadFromParcel(Parcel &parcel)
{
    bool status;
    int tempInt;

    status = parcel.ReadString(type);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(handle);
    if (!status) {
        return status;
    }

    status = parcel.ReadString16(folder);
    if (!status) {
        return status;
    }

    status = parcel.ReadString16(old_folder);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    msg_type = bluetooth::MapMessageType(tempInt);

    status = parcel.ReadString(datetime);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(subject);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(sender_name);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    priority = bluetooth::MapBoolType(tempInt);

    status = parcel.ReadString(conversation_name);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(conversation_id);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(presence_availability);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(presence_text);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(last_activity);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(chat_state);
    if (!status) {
        return status;
    }

    status = parcel.ReadInt32(tempInt);
    if (!status) {
        return status;
    }
    read_status = bluetooth::MapMessageStatus(tempInt);

    status = parcel.ReadString(extended_data);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(participant_uci);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(contact_uid);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(version);
    if (!status) {
        return status;
    }

    status = parcel.ReadUint8(masInstanceId_);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(eventReportStringObject_);
    if (!status) {
        return status;
    }

    return status;
}

}  // namespace Bluetooth
}  // namespace OHOS
