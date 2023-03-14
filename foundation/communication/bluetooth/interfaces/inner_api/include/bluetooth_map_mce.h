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
 * @brief Defines map client framework interface, including observer and api functions.
 *
 * @since 6
 */

/**
 * @file bluetooth_map_mce.h
 *
 * @brief map client interface.
 *
 * @since 6
 */

#ifndef BLUETOOTH_MAP_MCE_H
#define BLUETOOTH_MAP_MCE_H

#include "bluetooth_def.h"
#include "bluetooth_types.h"
#include "bluetooth_remote_device.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Map Event Report
 *
 * @since 6
 */
struct MapEventReport {
    std::string type = "";  // NewMessage, DeliverySuccess, SendingSuccess, DeliveryFailure ,SendingFailure ,
                            // MemoryFull, MemoryAvailable, MessageDeleted, MessageShift, ReadStatusChanged,
                            // MessageRemoved, MessageExtendedData-Changed, ParticipantPresence-Changed,
                            // ParticipantChatState-Changed, ConversationChanged
    std::string handle = "";
    std::u16string folder = u"";
    std::u16string old_folder = u"";
    MapMessageType msg_type = MapMessageType::INVALID;
    std::string datetime = "";
    std::string subject = "";
    std::string sender_name = "";
    MapBoolType priority = MapBoolType::INVALID;  // Value "yes" is high priority; "no" is not of high priority.
    std::string conversation_name = "";
    std::string conversation_id = "";
    std::string presence_availability = "";
    std::string presence_text = "";
    std::string last_activity = "";
    std::string chat_state = "";
    MapMessageStatus read_status =
        MapMessageStatus::INVALID;  // Shall be used only if the event “type” is “NewMessage” or “ReadStatusChanged”.
    std::string extended_data = "";  // be used only if the event “type” is “MessageExtendedDataChanged”.
    std::string participant_uci = "";
    std::string contact_uid = "";
    std::string version = "";
    uint8_t masInstanceId_ = 0;
    std::string eventReportStringObject_ = "";  // all string of the event report object
};

/**
 * @brief MessagesOutline in messageslisting
 *
 * @since 6
 */
struct MessageOutline {
    std::string handle = "";   // is the message handle in hexadecimal representation with up to 16 digits
    std::string subject = "";  // is the summary of the message
    std::string datetime = ""; // is the timestamp of the message in format "YYYYMMDDTHHMMSS" or “YYYYMMDDTHHMMSS±HHMM”
    std::string sender_name = "";        // is the name of the sender of the message when it is known by the MSE device.
    std::string sender_addressing = "";  // is the addressing information of the sender
    std::string replyto_addressing = "";  // is the address information for replies to the sender
    std::string recipient_name = "";  // is the name of the recipient of the message when it is known by the MSE device.
    std::string recipient_addressing = "";          // is the addressing information of the recipient.
    MapMessageType type = MapMessageType::INVALID;  // gives the type of the message. use the type: MapMessageType.
    MapMsgReceptionStatus receptionStatus =
        MapMsgReceptionStatus::INVALID;  // Gives the status of reception of the message. use the
                                         // type:MapMsgReceptionStatus
    int size = 0;             // The overall size in bytes of the original message as received from the network.
    int attachment_size = 0;  // The overall size of the attachments in bytes.
    MapBoolType text =
        MapBoolType::INVALID;  // Value "yes" indicates the original message or (in case of multipart-messages) that a
                               // part of the message includes textual content; "no" indicates that the message has no
                               // textual content, only binary.
    MapMessageStatus read =
        MapMessageStatus::INVALID;  // Value "yes" indicates that the message has already been read on the MSE; "no"
                                    // indicates that the message has not yet been read
    MapBoolType sent = MapBoolType::INVALID;  // Value "yes" indicates that the message has already been sent to the
                                              // recipient; "no" indicates that the message has not yet been sent.
    MapBoolType protected_ =
        MapBoolType::INVALID;  // Value "yes" indicates that the message or a part of the message (e.g., attachment) is
                               // protected by a DRM scheme; "no" indicates that the message is not protected by DRM.
    MapBoolType priority = MapBoolType::INVALID;  // Value "yes" indicates that the message is of high priority; "no"
                                                  // indicates that the message is not of high priority.
    MapMsgDeliveryStatus delivery_status =
        MapMsgDeliveryStatus::INVALID;   // V1.1 support , Provides the status of delivery of the message.
    std::string conversation_id = "";    // V1.1 support , The identification of the conversation.
    std::string conversation_name = "";  // V1.1 support , The human readable name of the conversation.
    MapMsgDirection direction =
        MapMsgDirection::INVALID;  // V1.1 support , This attribute shall indicate the direction of the message.
    std::string attachment_mime_types = "";  // V1.1 support , The MIME type of the attachment(s).
};
/**
 * @brief MessagesListing Param Struct
 *
 * @since 6
 */
struct MessagesListingParamStruct {
    MapOnOffType NewMessage = MapOnOffType::INVALID;  // on off
    std::string MseTime = "";                         // String with current time basis and UTC-offset of the MSE.
    uint16_t ListingSize = 0;
    std::string DatabaseIdentifier = "";    // 128-bit value in hex string format,max 32 bytes
    std::string FolderVersionCounter = "";  // 128-bit value in hex string format,max 32 bytes
    std::string Version = "";               // "1.1" or "1.0"
};

/**
 * @brief MessagesListing class
 */
struct MessagesListing {
    std::vector<MessageOutline> messageOutlineList_ {};
    MessagesListingParamStruct messagesListingParam_ {};
    std::string messagesListingStringObject_ = "";  // all string of the messagelist object
};

/**
 * @brief Vcard 3_0
 *
 * @since 6
 */
struct MapVcard {
    std::string VERSION = "";             // shall be included
    std::string N = "";                   // shall be included
    std::vector<std::string> TEL {};       // may be used
    std::vector<std::string> EMAIL {};     // may be used
    std::vector<std::string> X_BT_UID {};  // bmsg V1.1
    std::vector<std::string> X_BT_UCI {};  // bmsg V1.1
    std::string FN = "";                  // vcard 3.0 , shall be included
};
/**
 * @brief <bmessage-content>
 *
 * @since 6
 */
struct BMessageContent {
    // <bmessage-body-part-ID>
    std::string bodyPartID = "";  // This property shall be used if and only if the content of the related message
                                  // cannot be delivered completely within one <bmessage-content> object
    // <bmessage-body-property>
    std::string body_encoding = "";  // Email/MMS [6], [9]: "8BIT" (for 8-Bit Clean encoding).
                                     // GSM-SMS [14]: "G-7BIT" (GSM 7 bit Default Alphabet), "G-7BITEXT" (GSM 7 bit
                                     // Alphabet with national language extension), "G-UCS2", and "G-8BIT". CDMA-SMS
                                     // [13]: "C-8BIT" (Octet, unspecified), "C-EPM" (Extended Protocol Message),
                                     // "C-7ASCII (7-bit ASCII), "C-IA5" (IA5), "C-UNICODE" (UNICODE), "C-SJIS"
                                     // (Shift-JIS), "C-KOREAN" (Korean),"C-LATINHEB” (Latin/Hebrew), and “C-LATIN”
                                     // (Latin). For IM: [6] "8BIT" (for 8-Bit Clean encoding). Ver1.1
    std::string body_charset = "";   // UTF-8 or SMS PDU
    std::string body_language = "";  // GSM-SMS [14]: " TURKISH", " SPANISH", "PORTUGUESE", "UNKNOWN"
                                     // CDMA-SMS [13]: "ENGLISH", "FRENCH", "SPANISH", "JAPANESE", "KOREAN", "CHINESE",
                                     // "HEBREW", "UNKNOWN"
    int body_content_length = 0;

    // <bmessage-body-content>
    std::string body_content = "";  // body-content  support V1.1 IM
};
/**
 * @brief <bmessage-envelope>
 *
 * @since 6
 */
struct BMessageEnvelope {
    // <initial-envelope> [<initial-recipient>]*
    std::vector<MapVcard> recipientLevel1_ {};  // valid only in level 2/3
    // <middle-envelope> [<middle-recipient>]*
    std::vector<MapVcard> recipientLevel2_ {};  // valid only in level 3
    // <final-envelope> [<final-recipient>]*
    std::vector<MapVcard> recipientLevel3_ {};  // Final Recipient , always valid
    // The maximum level of <bmessage-envelope>
    int maxLevelOfEnvelope_ = 0;
    // <bmessage-content>
    BMessageContent msgBody_ {};
};

/**
 * @brief receive msg, support both 1.0 and 1.1
 *
 * @since 6
 */
struct BMessageStruct {
    // <bmessage-property>
    std::string version_property = "";
    MapMessageStatus readstatus_property = MapMessageStatus::INVALID;  // "READ" or "UNREAD"
    MapMessageType type_property =
        MapMessageType::INVALID;  // V1.0 include "EMAIL","SMS_GSM","SMS_CDMA","MMS", V1.1 add a "IM"
    std::u16string folder_property =
        u"";  // This is the folder name including the path where the bMessage is located in.
    std::string extendeddata_property = "";  // extended data only V1.1
    // [<bmessage-originator>]?
    std::vector<MapVcard> originator_ {};  // This property includes a vCard identifying the originator
    // <bmessage-envelope>
    BMessageEnvelope envelope_ {};  // The maximum level of <bmessage-envelope> encapsulation shall be three.
};
/**
 * @brief BMessage
 *
 * @since 6
 */
struct MapBMessage {
    MapFractionDeliverType FractionDeliver =
        MapFractionDeliverType::INVALID;  // MapFractionDeliverType type, 0 = "more",1 = "last"
    BMessageStruct bMessageParam_ {};
    std::string bMessageStringObject_ = "";  // all string of the bmessage object
};

/**
 * @brief Conversation Participant
 */
struct Participant {
    std::string uci = "";
    std::string display_name = "";
    std::string chat_state = "";
    std::string last_activity = "";
    std::string x_bt_uid = "";
    std::string name = "";
    std::string presence_availability = "";
    std::string presence_text = "";
    std::string priority = "";
};

/**
 * @brief Conversation
 */
struct Conversation {
    std::string id = "";
    std::string name = "";
    std::string last_activity = "";
    std::string read_status = "";
    std::string version_counter = "";
    std::string summary = "";
    std::vector<Participant> participantList_ {};
};

/**
 * @brief ConversationListing ParamStuct
 */
struct ConversationListingParamStuct {
    std::string ConversationListingVersionCounter = "";  // 128-bit value in hex string format(max 32 bytes)
    uint16_t ListingSize = 0;
    std::string DatabaseIdentifier = "";  // 128-bit value in hex string format(max 32 bytes)
    std::string MseTime = "";             // String with current time basis and UTC-offset of the MSE.
    std::string Version = "";
};

/**
 * @brief IProfile ConversationListing Object
 */
struct ConversationListing {
    std::vector<Conversation> conversationOutlineList_ {};
    ConversationListingParamStuct conversationListingParam_ {};
    std::string conversationListingStringObject_ = "";
};

/**
 * @brief Get Messages Listing Parameters
 *
 * @since 6
 */
struct GetMessagesListingParameters {
    std::u16string folder = u"";   // such as: inbox , outbox
    uint16_t MaxListCount = 0;     // maximum number of the listing
    uint16_t ListStartOffset = 0;  // the offset of the first entry of the returned listing object
    uint8_t SubjectLength = 0;
    uint32_t ParameterMask = 0;     // Bit mask: Bit i=1 indicates that the parameter related to Bit i
                                    //     shall be present in the requested Messages-Listing.
                                    // If this value is 0, the MSE shall return all parameters of the Messages-
                                    //     Listing object DTD labeled as "REQUIRED" and may return all other attributes
                                    //     The message handle shall always be present.
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_PRESENT_ALL 0b000111111111111111111111
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_SUBJECT 0b000000000000000000000001
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_DATETIME 0b000000000000000000000010
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_SENDER_NAME 0b000000000000000000000100
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_SENDER_ADDRESSING 0b000000000000000000001000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_RECIPIENT_NAME 0b000000000000000000010000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_RECIPIENT_ADDRESSING 0b000000000000000000100000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_TYPE 0b000000000000000001000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_SIZE 0b000000000000000010000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_RECEPTION_STATUS 0b000000000000000100000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_TEXT 0b000000000000001000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_ATTACHMENT_SIZE 0b000000000000010000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_PRIORITY 0b000000000000100000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_READ 0b000000000001000000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_SENT 0b000000000010000000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_PROTECTED 0b000000000100000000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_REPLYTO_ADDRESSING 0b000000001000000000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_DELIVERY_STATUS 0b000000010000000000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_CONVERSATION_ID 0b000000100000000000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_CONVERSATION_NAME 0b000001000000000000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_DIRECTION 0b000010000000000000000000
                                    // MAP_GETMESSAGELIST_PARAMETER_MASK_ATTACHMENT_MIME 0b000100000000000000000000
    uint8_t FilterMessageType = 0;  // Bit mask: 0 = no filtering, get this type, 1 = filter out this type
                                    // MAP_FILTER_MESSAGE_MASK_SMS_GSM         0b00000001
                                    // MAP_FILTER_MESSAGE_MASK_SMS_CDMA        0b00000010
                                    // MAP_FILTER_MESSAGE_MASK_EMAIL       0b00000100
                                    // MAP_FILTER_MESSAGE_MASK_MMS         0b00001000
                                    // MAP_FILTER_MESSAGE_MASK_IM          0b00010000
                                    // MAP_FILTER_MESSAGE_MASK_NO_FILTERING    0b00000000
    std::string FilterPeriodBegin = "";  // String with Begin of filter period.
    std::string FilterPeriodEnd = "";    // String with End of filter period.
    uint8_t FilterReadStatus = 0;        // Bit mask: 0b01 = get unread only, 0b10 = get read only
                                         // MAP_FILTER_READ_STATUS_MASK_UNREAD          0b00000001
                                         // MAP_FILTER_READ_STATUS_MASK_READ            0b00000010
                                         // MAP_FILTER_READ_STATUS_MASK_NO_FILTERING    0b00000000

    std::string FilterRecipient = "";   // Text (UTF-8), wildcards "*" may be used if required
    std::string FilterOriginator = "";  // Text (UTF-8), wildcards "*" may be used if required
    uint8_t FilterPriority = 0;         // Bit mask:  0b01 = get high , 0b10 = get no high
                                        // MAP_FILTER_PRIORITY_MASK_HIGH               0b00000001
                                        // MAP_FILTER_PRIORITY_MASK_NO_HIGH            0b00000010
                                        // MAP_FILTER_PRIORITY_MASK_NO_FILTERING       0b00000000
    std::string ConversationID = "";
    std::string FilterMessageHandle = "";
};
/**
 * @brief Get Message Parameters
 *
 * @since 6
 */
struct GetMessageParameters {
    MapAttachmentType Attachment = MapAttachmentType::INVALID;
    MapCharsetType Charset = MapCharsetType::INVALID;
    MapFractionRequestType FractionRequest = MapFractionRequestType::INVALID;
};
/**
 * @brief Get Conversation Listing Parameters
 *
 * @since 6
 */
struct GetConversationListingParameters {
    uint16_t MaxListCount = 0;
    uint16_t ListStartOffset = 0;
    uint8_t FilterReadStatus = 0;      // mask type is:
                                       // MAP_FILTER_READ_STATUS_MASK_UNREAD          0b00000001
                                       // MAP_FILTER_READ_STATUS_MASK_READ            0b00000010
                                       // MAP_FILTER_READ_STATUS_MASK_NO_FILTERING    0b00000000
    std::string FilterRecipient = "";  // Text (UTF-8), wildcards "*" may be used if required
    std::string ConversationID = "";
    std::string FilterLastActivityBegin = "";
    std::string FilterLastActivityEnd = "";
    uint32_t ConvParameterMask =
        0;  // ConvParameterMask 0-14 is defined, 15–31 Reserved for Future Use
            // MAP_GETCONV_PARAMETER_MASK_CONVERSATION_NAME 0b000000000000000000000001
            // MAP_GETCONV_PARAMETER_MASK_CONVERSATION_LAST_ACTIVITY 0b000000000000000000000010
            // MAP_GETCONV_PARAMETER_MASK_CONVERSATION_READ_STATUS 0b000000000000000000000100
            // MAP_GETCONV_PARAMETER_MASK_CONVERSATION_VERSION_COUNTER 0b000000000000000000001000
            // MAP_GETCONV_PARAMETER_MASK_CONVERSATION_SUMMARY 0b000000000000000000010000
            // MAP_GETCONV_PARAMETER_MASK_PARTICIPANTS 0b000000000000000000100000
            // MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_UCI 0b000000000000000001000000
            // MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_DISPLAY_NAME 0b000000000000000010000000
            // MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_CHAT_STATE 0b000000000000000100000000
            // MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_LAST_ACTIVITY 0b000000000000001000000000
            // MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_X_BT_UID 0b000000000000010000000000
            // MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_NAME 0b000000000000100000000000
            // MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_PRESENCE_AVAILABILITY
            // 0b000000000001000000000000 MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_PRESENCE_TEXT
            // 0b000000000010000000000000 MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_PRIORITY
            // 0b000000000100000000000000
};
/**
 * @brief Owner Status Parameters
 *
 * @since 6
 */
struct OwnerStatus {
    uint8_t PresenceAvailability = 0;
    std::string PresenceText = "";
    std::string LastActivity = "";
    uint8_t ChatState = 0;
};
/**
 * @brief Owner Status Parameters
 *
 * @since 6
 */
struct SetOwnerStatusParameters {
    std::string conversationId_ = "";
    OwnerStatus ownerStatus_ {};
};

/**
 * @brief SendMessage Parameters
 *
 * @since 6
 */
struct MapSendMessageParameters {
    std::string ConversationID = "";
    MapAttachmentType Attachment = MapAttachmentType::INVALID;
    MapCharsetType Charset = MapCharsetType::INVALID;
    MapOnOffType Transparent = MapOnOffType::INVALID;
    MapOnOffType Retry = MapOnOffType::INVALID;                 // 1 is "ON", 0 is "OFF"
    std::string MessageHandle = "";                             // if ModifyText is “PREPEND”
    MapModifyTextType ModifyText = MapModifyTextType::INVALID;  // 0 is “REPLACE” , 1 is “PREPEND”
    BMessageStruct message {};
};

/**
 * @brief Map MasInstanceInfo Parameters
 *
 * @since 6
 */
struct MapMasInstanceInfo {
    std::string OwnerUCI = "";
    std::string MASInstanceInformation = "";
    int supportedMsgTypes_ = 0;  // Supported Message Types mask by one instance client
                                 // MAP_MCE_SUPPORTED_MESSAGE_TYPE_EMAIL    0x01
                                 // MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_GSM  0x02
                                 // MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_CDMA 0x04
                                 // MAP_MCE_SUPPORTED_MESSAGE_TYPE_MMS  0x08
                                 // MAP_MCE_SUPPORTED_MESSAGE_TYPE_IM   0x10
                                 // MAP_MCE_SUPPORTED_MESSAGE_TYPE_ALL  0x01F
    int instanceId = 0;
};

/**
 * @brief Map MasInstanceInfo List Parameters
 *
 * @since 6
 */
struct MapMasInstanceInfoList {
    bool isValid = false;  // data ready
    std::vector<MapMasInstanceInfo> masInfoList {};
};
/**
 * @brief set message status
 *
 * @since 6
 */
struct MapSetMessageStatus {
    std::u16string msgHandle {};
    MapStatusIndicatorType statusIndicator = MapStatusIndicatorType::INVALID;
    MapStatusValueType statusValue = MapStatusValueType::NO;
    std::string extendedData {};
};
/**
 * @brief action complete struct
 *
 * @since 6
 */
struct MapAction {
    MapActionType action_ = MapActionType::UPDATE_INBOX;
    int supportedFeatures_ = 0;  // Map Supported features mask
                                 // MAP_SUPPORTED_FEATURES_NOTIFICATION_REGISTRATION 0x00000001
                                 // MAP_SUPPORTED_FEATURES_NOTIFICATION_FEATURE 0x00000002
                                 // MAP_SUPPORTED_FEATURES_BROWSING 0x00000004
                                 // MAP_SUPPORTED_FEATURES_UPLOADING 0x00000008
                                 // MAP_SUPPORTED_FEATURES_DELETE_FEATURE 0x00000010
                                 // MAP_SUPPORTED_FEATURES_INSTANCE_INFO_FEATURE 0x00000020
                                 // MAP_SUPPORTED_FEATURES_EVENT_REPORT_1_1 0x00000040
                                 // MAP_SUPPORTED_FEATURES_EVENT_REPORT_1_2 0x00000080
                                 // MAP_SUPPORTED_FEATURES_MESSAGE_FORMAT_1_1 0x00000100
                                 // MAP_SUPPORTED_FEATURES_MESSAGELIST_FORMAT_1_1 0x00000200
                                 // MAP_SUPPORTED_FEATURES_PERSISTENT_MESSAGE_HANDLE 0x00000400
                                 // MAP_SUPPORTED_FEATURES_DATABASE_IDENTIFIER 0x00000800
                                 // MAP_SUPPORTED_FEATURES_FOLDOR_VERSION_COUNTER 0x00001000
                                 // MAP_SUPPORTED_FEATURES_CONVERSATION_VERSION_COUNTER 0x00002000
                                 // MAP_SUPPORTED_FEATURES_PARTICIPANT_PRESENCE_CHANGE_NOTIFICATION 0x00004000
                                 // MAP_SUPPORTED_FEATURES_PARTICIPANT_CHAT_STATE_CHANGE_NOTIFICATION 0x00008000
                                 // MAP_SUPPORTED_FEATURES_PBAP_CONTACT_CROSS_REFERENCE 0x00010000
                                 // MAP_SUPPORTED_FEATURES_NOTIFICATION_FILTER 0x00020000
                                 // MAP_SUPPORTED_FEATURES_UTC_OFFSET_TIMESTAMP_FORMAT 0x00040000
                                 // MAP_SUPPORTED_FEATURES_MAPSUPPORTED_FEATURES 0x00080000
                                 // MAP_SUPPORTED_FEATURES_CONVERSATION_LISTING 0x00100000
                                 // MAP_SUPPORTED_FEATURES_OWNER_STATUS 0x00200000
                                 // MAP_SUPPORTED_FEATURES_MESSAGE_FORWARDING 0x00400000
    OwnerStatus ownerStatus_ {};
};

/**
 * @brief MapClient Observer
 *
 * @since 6
 */
class MapClientObserver {
public:
    /**
     * @brief Destroy the Map Client Observer object
     *
     * @since 6
     */
    virtual ~MapClientObserver() = default;
    /**
     * @brief  Map ActionCompleted observer
     *
     * @param  device           bluetooth device address
     * @param  action           excute action type and Parameters
     * @param  status           excute status
     * @since 6
     */
    virtual void OnMapActionCompleted(
        const BluetoothRemoteDevice &device, const MapAction &action, MapExecuteStatus status) {};
    /**
     * @brief  Map EventReported observer
     *
     * @param  device           bluetooth device address
     * @param  report           EventReported string
     * @since 6
     */
    virtual void OnMapEventReported(const BluetoothRemoteDevice &device, const MapEventReport &report) {};
    /**
     * @brief  ConnectionState Changed observer
     * @param  device           bluetooth device address
     * @param  state            EventReported string
     */
    virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) {};
    /**
     * @brief  Bmessage completed observer
     *
     * @param  deviceAddress     bluetooth address
     * @param  bmsg
     * @param  status
     * @since 6
     */
    virtual void OnBmessageCompleted(
        const BluetoothRemoteDevice &deviceAddress, const MapBMessage &bmsg, MapExecuteStatus status) {};
    /**
     * @brief  MessagesListing completed observer
     *
     * @param  deviceAddress     bluetooth address
     * @param  bmsg
     * @param  status
     * @since 6
     */
    virtual void OnMessagesListingCompleted(
        const BluetoothRemoteDevice &deviceAddress, const MessagesListing &listing, MapExecuteStatus status) {};
    /**
     * @brief  ConversationListing completed observer
     *
     * @param  deviceAddress     bluetooth address
     * @param  listing
     * @param  status
     * @since 6
     */
    virtual void OnConversationListingCompleted(
        const BluetoothRemoteDevice &deviceAddress, const ConversationListing &listing, MapExecuteStatus status) {};
};
/**
 * @brief MapClient class
 *
 * @since 6
 */
class BLUETOOTH_API MapClient {
public:
    /**
     * @brief Get the Profile object
     *
     * @return MapClient* pointer
     * @since 6
     */
    static MapClient *GetProfile();
    /**
     * @brief Register Observer
     *
     * @param  observer         observer pointer
     * @since 6
     */
    void RegisterObserver(MapClientObserver &observer);
    /**
     * @brief Deregister Observer
     *
     * @param  observer         observer pointer
     * @since 6
     */
    void DeregisterObserver(MapClientObserver &observer);
    /**
     * @brief  Connect to map server
     *
     * @param  device           bluetooth device address
     * @return bool             api accept status, return true if accept ok
     * @since 6
     */
    bool Connect(const BluetoothRemoteDevice &device);
    /**
     * @brief  disconnect from map server
     *
     * @param  device           bluetooth device address
     * @return bool             api accept status, return true if accept ok
     * @since 6
     */
    bool Disconnect(const BluetoothRemoteDevice &device);
    /**
     * @brief  display connect status
     *
     * @param  device           bluetooth device address
     * @return bool             Returns true if the device status is connected, otherwise return false.
     * @since 6
     */
    bool IsConnected(const BluetoothRemoteDevice &device);
    /**
     * @brief Get the Connected Devices object
     *
     * @return Returns the bluetooth device address
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetConnectedDevices() const;
    /**
     * @brief Get the device list through the connection status.
     *
     * @param statusList  Reference to the connection status.
     *     @c BTConnectState::CONNECTING : the state is connecting.
     *     @c BTConnectState::CONNECTED : the state is connected.
     *     @c BTConnectState::DISCONNECTING : the state is disconnecting.
     *     @c BTConnectState::DISCONNECTED : the state is disconnected.
     * @return Returns the bluetooth device address in the specified status.
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(const std::vector<int> &statusList) const;
    /**
     * @brief Get the Connection State object
     *
     * @param  device           bluetooth device address
     * @return int              return connection status
     *     @c BTConnectState::CONNECTING : the state is connecting.
     *     @c BTConnectState::CONNECTED : the state is connected.
     *     @c BTConnectState::DISCONNECTING : the state is disconnecting.
     *     @c BTConnectState::DISCONNECTED : the state is disconnected.
     * @since 6
     */
    int GetConnectionState(const BluetoothRemoteDevice &device) const;
    /**
     * @brief Set the connection policy of the specified device.
     *
     * @param device Reference to the remote bluetooth device.
     * @param strategy Reference to the connection policy,
     *     @c BTStrategyType::CONNECTION_UNKNOWN : the connection policy for unknown state.
     *     @c BTStrategyType::CONNECTION_ALLOWED : the connection policy for allowed state.
     *     @c BTStrategyType::CONNECTION_FORBIDDEN : the connection policy for forbidden state.
     * @return Returns true if the operation is success;returns false if the operation failed.
     * @since 6
     */
    bool SetConnectionStrategy(const BluetoothRemoteDevice &device, const int strategy);
    /**
     * @brief Get the Connection Strategy object
     *
     * @param  device           bluetooth device address
     * @return int Reference to the connection policy,
     *     @c BTStrategyType::CONNECTION_UNKNOWN : the connection policy for unknown state.
     *     @c BTStrategyType::CONNECTION_ALLOWED : the connection policy for allowed state.
     *     @c BTStrategyType::CONNECTION_FORBIDDEN : the connection policy for forbidden state.
     * @since 6
     */
    int GetConnectionStrategy(const BluetoothRemoteDevice &device) const;
    /**
     * @brief Get the Unread Messages object
     *
     * @param  device           bluetooth device address
     * @param  list             uread message list param
     * @return int              api accept status, return RET_NO_ERROR if accept ok,
     *                          the excute result will return by observer
     * @since 6
     */
    int GetUnreadMessages(const BluetoothRemoteDevice &device, MapMessageType type, uint8_t max);
    /**
     * @brief Get the Supported Features object
     *
     * @param  device           bluetooth device address
     * @return int              api accept status, return RET_NO_ERROR if accept ok,
     *                          the excute result will return by action complete observer
     * @since 6
     */
    int GetSupportedFeatures(const BluetoothRemoteDevice &device) const;
    /**
     * @brief  Send Message
     *
     * @param  device           bluetooth device address
     * @param  msg              message
     * @return int              api accept status, return RET_NO_ERROR if accept ok,
     *                          the excute result will return by observer
     * @since 6
     */
    int SendMessage(const BluetoothRemoteDevice &device, const MapSendMessageParameters &msg);
    /**
     * @brief Set the Notification Filter object
     *
     * @param  device           bluetooth device address
     * @param  mask             maskvalue of Notification Filter
     *          MAP_NOTIFICATION_FILTER_MASK_NEW_MESSAGE        0b000000000000000000000001
     *          MAP_NOTIFICATION_FILTER_MASK_MESSAGE_DELETED    0b000000000000000000000010
     *          MAP_NOTIFICATION_FILTER_MASK_MESSAGE_SHIFT      0b000000000000000000000100
     *          MAP_NOTIFICATION_FILTER_MASK_SENDING_SUCCESS    0b000000000000000000001000
     *          MAP_NOTIFICATION_FILTER_MASK_SENDING_FAILURE    0b000000000000000000010000
     *          MAP_NOTIFICATION_FILTER_MASK_DELIVERY_SUCCESS   0b000000000000000000100000
     *          MAP_NOTIFICATION_FILTER_MASK_DELIVERY_FAILURE   0b000000000000000001000000
     *          MAP_NOTIFICATION_FILTER_MASK_MEMORY_FULL        0b000000000000000010000000
     *          MAP_NOTIFICATION_FILTER_MASK_MEMORY_AVAILABLE   0b000000000000000100000000
     *          MAP_NOTIFICATION_FILTER_MASK_READ_STATUS_CHANGED    0b000000000000001000000000
     *          MAP_NOTIFICATION_FILTER_MASK_CONVERSATION_CHANGED   0b000000000000010000000000
     *          MAP_NOTIFICATION_FILTER_MASK_PARTICIPANT_PRESENCE_CHANGED   0b000000000000100000000000
     *          MAP_NOTIFICATION_FILTER_MASK_PARTICIPANT_CHAT_STATE_CHANGED 0b000000000001000000000000
     * @return int              api accept status, return RET_NO_ERROR if accept ok,
     *                          the excute result will return by observer
     * @since 6
     */
    int SetNotificationFilter(const BluetoothRemoteDevice &device, const int mask);
    /**
     * @brief Get the Messages Listing object
     *
     * @param  device           bluetooth device address
     * @param  para             GetMessagesListing Parameters
     * @return int              api accept status, return RET_NO_ERROR if accept ok,
     *                          the excute result will return by observer
     * @since 6
     */
    int GetMessagesListing(const BluetoothRemoteDevice &device, const GetMessagesListingParameters &para) const;
    /**
     * @brief Get the Message object
     *
     * @param  device           bluetooth device address
     * @param  msgHandle        msgHandle of message
     * @param  para             GetMessage Parameters
     * @return int              api accept status, return RET_NO_ERROR if accept ok,
     *                          the excute result will return by observer
     * @since 6
     */
    int GetMessage(const BluetoothRemoteDevice &device, MapMessageType type, const std::u16string &msgHandle,
        const GetMessageParameters &para) const;
    /**
     * @brief  Update Inbox
     *
     * @param  device           bluetooth device address
     * @return int              api accept status, return RET_NO_ERROR if accept ok,
     *                          the excute result will return by observer
     * @since 6
     */
    int UpdateInbox(const BluetoothRemoteDevice &device, MapMessageType type);
    /**
     * @brief Get the Conversation Listing object
     *
     * @param  device           bluetooth device address
     * @param  para             GetConversationListing Parameters
     * @return int              api accept status, return RET_NO_ERROR if accept ok,
     *                          the excute result will return by observer
     * @since 6
     */
    int GetConversationListing
        (const BluetoothRemoteDevice &device, const GetConversationListingParameters &para) const;
    /**
     * @brief Set the Message Status object
     * @param  device
     * @param  type
     * @param  msgStatus
     * @return int
     */
    int SetMessageStatus(
        const BluetoothRemoteDevice &device, MapMessageType type, const MapSetMessageStatus &msgStatus);
    /**
     * @brief Set the Owner Status object
     *
     * @param  device           bluetooth device address
     * @param  para             OwnerStatus Parameters
     * @return int              api accept status, return RET_NO_ERROR if accept ok,
     *                          the excute result will return by observer
     * @since 6
     */
    int SetOwnerStatus(const BluetoothRemoteDevice &device, const SetOwnerStatusParameters &para);
    /**
     * @brief Get the Owner Status object
     *
     * @param  device           bluetooth device address
     * @param  conversationId   conversationId Param
     * @return int              api accept status, return RET_NO_ERROR if accept ok,
     *                          the excute result will return by observer
     * @since 6
     */
    int GetOwnerStatus(const BluetoothRemoteDevice &device, const std::string &conversationId) const;
    /**
     * @brief Get the Mas Instance Info object
     *
     * @param  device           bluetooth device address
     * @return std::vector<MapMasInstanceInfo>  instanceinfo list
     * @since 6
     */
    MapMasInstanceInfoList GetMasInstanceInfo(const BluetoothRemoteDevice &device) const;

private:
    /**
     * @brief Construct a new Map Client object
     *
     * @since 6
     */
    MapClient();
    /**
     * @brief Destroy the Map Client object
     *
     * @since 6
     */
    ~MapClient();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(MapClient);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // BLUETOOTH_MAP_MCE_H
