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

#ifndef BLUETOOTH_PARCEL_MAP_MCE_PARAMETER_H
#define BLUETOOTH_PARCEL_MAP_MCE_PARAMETER_H

#include "map_mce_parameter.h"
#include "parcel.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothIProfileMapAction : public Parcelable, public bluetooth::IProfileMapAction {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileMapAction *Unmarshalling(Parcel &parcel);
    /**
     * @brief Write IProfileMapAction values to parcel.
     *
     * @param parcel Parcel which store IProfileMapAction values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileMapAction values from parcel.
     *
     * @param parcel Parcel which bring IProfileMapAction values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief MasInstanceInfo Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileMasInstanceInfoList : public Parcelable, public bluetooth::IProfileMasInstanceInfoList {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileMasInstanceInfoList *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write IProfileMasInstanceInfo values to parcel.
     *
     * @param parcel Parcel which store IProfileMasInstanceInfo values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileMasInstanceInfo values from parcel.
     *
     * @param parcel Parcel which bring IProfileMasInstanceInfo values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief Map SendMessage Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileSendMessageParameters : public Parcelable, public bluetooth::IProfileSendMessageParameters {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileSendMessageParameters *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write IProfileSendMessageParameters values to parcel.
     *
     * @param parcel Parcel which store IProfileSendMessageParameters values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileSendMessageParameters values from parcel.
     *
     * @param parcel Parcel which bring IProfileSendMessageParameters values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief Map IProfileSetOwnerStatusParameters Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileSetOwnerStatusParameters : public Parcelable,
                                                  public bluetooth::IProfileSetOwnerStatusParameters {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileSetOwnerStatusParameters *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write IProfileSetOwnerStatusParameters values to parcel.
     *
     * @param parcel Parcel which store IProfileSetOwnerStatusParameters values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileSetOwnerStatusParameters values from parcel.
     *
     * @param parcel Parcel which bring IProfileSetOwnerStatusParameters values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief Map IProfileGetConversationListingParameters Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileGetConversationListingParameters : public Parcelable,
                                                          public bluetooth::IProfileGetConversationListingParameters {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileGetConversationListingParameters *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write IProfileGetConversationListingParameters values to parcel.
     *
     * @param parcel Parcel which store IProfileGetConversationListingParameters values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileGetConversationListingParameters values from parcel.
     *
     * @param parcel Parcel which bring IProfileGetConversationListingParameters values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief Map IProfileGetMessageParameters Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileGetMessageParameters : public Parcelable, public bluetooth::IProfileGetMessageParameters {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileGetMessageParameters *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write IProfileGetMessageParameters values to parcel.
     *
     * @param parcel Parcel which store IProfileGetMessageParameters values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileGetMessageParameters values from parcel.
     *
     * @param parcel Parcel which bring IProfileGetMessageParameters values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief Map IProfileGetMessagesListingParameters Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileGetMessagesListingParameters : public Parcelable,
                                                      public bluetooth::IProfileGetMessagesListingParameters {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileGetMessagesListingParameters *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write IProfileGetMessagesListingParameters values to parcel.
     *
     * @param parcel Parcel which store IProfileGetMessagesListingParameters values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileGetMessagesListingParameters values from parcel.
     *
     * @param parcel Parcel which bring IProfileGetMessagesListingParameters values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief Map IProfileBMessage Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileBMessage : public Parcelable, public bluetooth::IProfileBMessage {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileBMessage *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write IProfileBMessage values to parcel.
     *
     * @param parcel Parcel which store IProfileBMessage values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileBMessage values from parcel.
     *
     * @param parcel Parcel which bring IProfileBMessage values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief Map IProfileMessagesListing Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileMessagesListing : public Parcelable, public bluetooth::IProfileMessagesListing {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileMessagesListing *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write IProfileMessagesListing values to parcel.
     *
     * @param parcel Parcel which store IProfileMessagesListing values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileMessagesListing values from parcel.
     *
     * @param parcel Parcel which bring IProfileMessagesListing values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);

private:
    bool WriteToParcelIProfileMessageOutline(
        Parcel &parcel, const bluetooth::IProfileMessageOutline &outline) const;
    bool ReadFromParcelIProfileMessageOutline(Parcel &parcel, bluetooth::IProfileMessageOutline &outline);
    int outlineListSize;
};

/**
 * @brief Map IProfileConversationListing Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileConversationListing : public Parcelable, public bluetooth::IProfileConversationListing {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileConversationListing *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write IProfileConversationListing values to parcel.
     *
     * @param parcel Parcel which store IProfileConversationListing values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileConversationListing values from parcel.
     *
     * @param parcel Parcel which bring IProfileConversationListing values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);

private:
    bool WriteToParcelIProfileConversation(Parcel &parcel, const bluetooth::IProfileConversation &outline) const;
    bool ReadFromParcelIProfileConversation(Parcel &parcel, bluetooth::IProfileConversation &outline);
    bool WriteToParcelIProfileParticipant(Parcel &parcel, const bluetooth::IProfileParticipant &outline) const;
    bool ReadFromParcelIProfileParticipant(Parcel &parcel, bluetooth::IProfileParticipant &outline);
    int participantListNum_ = 0;
    int conversationOutlineListNum_ = 0;
};

/**
 * @brief Map IProfileMapEventReport Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileMapEventReport : public Parcelable, public bluetooth::IProfileMapEventReport {
public:
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIProfileMapEventReport *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write IProfileMapEventReport values to parcel.
     *
     * @param parcel Parcel which store IProfileMapEventReport values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read IProfileMapEventReport values from parcel.
     *
     * @param parcel Parcel which bring IProfileMapEventReport values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief Map IProfileBMessageStruct Parameters
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIProfileBMessageStruct {
public:
    /*
     * @brief Write IProfileBMessageStruct values to parcel.
     *
     * @param parcel Parcel which store IProfileBMessageStruct values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel, const bluetooth::IProfileBMessageStruct &msgStruct) const;

    /**
     * @brief Read IProfileBMessageStruct values from parcel.
     *
     * @param parcel Parcel which bring IProfileBMessageStruct values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel, bluetooth::IProfileBMessageStruct &msgStruct);

private:
    bool WriteToParcelIProfileMapVcard(Parcel &parcel, const bluetooth::IProfileMapVcard &vcard) const;
    bool ReadFromParcelIProfileMapVcard(Parcel &parcel, bluetooth::IProfileMapVcard &vcard);
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif  // BLUETOOTH_PARCEL_MAP_MCE_PARAMETER_H