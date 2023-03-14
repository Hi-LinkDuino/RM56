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

#ifndef MAP_MCE_PARAMETER_H
#define MAP_MCE_PARAMETER_H

#include <string>
#include <stdint.h>
#include <vector>

#include "bt_def.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief IProfile Map Event Report
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileMapEventReport {
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
 * @brief IProfile Conversation Participant
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileParticipant {
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
 * @brief IProfile Conversation
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileConversation {
    std::string id = "";
    std::string name = "";
    std::string last_activity = "";
    std::string read_status = "";
    std::string version_counter = "";
    std::string summary = "";
    std::vector<IProfileParticipant> participantList_ {};
};

/**
 * @brief IProfile ConversationListing ParamStuct
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileConversationListingParamStruct {
    std::string ConversationListingVersionCounter = "";  // 128-bit value in hex string format(max 32 bytes)
    uint16_t ListingSize = 0;
    std::string DatabaseIdentifier = "";  // 128-bit value in hex string format(max 32 bytes)
    std::string MseTime = "";             // String with current time basis and UTC-offset of the MSE. See Section 5.5.4
    std::string Version = "";
};

/**
 * @brief IProfile ConversationListing Object
 */
struct IProfileConversationListing {
    std::vector<IProfileConversation> conversationOutlineList_ {};
    IProfileConversationListingParamStruct conversationListingParam_ {};
    std::string conversationListingStringObject_ = "";
};

/**
 * @brief MessagesOutline in messageslisting
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileMessageOutline {
    std::string handle = "";   // is the message handle in hexadecimal representation with up to 16 digits
    std::string subject = "";  // is the summary of the message
    std::string datetime = "";  // is the timestamp of the message in format "YYYYMMDDTHHMMSS" or “YYYYMMDDTHHMMSS±HHMM”
    std::string sender_name = "";        // is the name of the sender of the message when it is known by the MSE device.
    std::string sender_addressing = "";  // is the addressing information of the sender
    std::string replyto_addressing = "";  // is the address information for replies to the sender
    std::string recipient_name = "";  // is the name of the recipient of the message when it is known by the MSE device.
    std::string recipient_addressing = "";          // is the addressing information of the recipient.
    MapMessageType type = MapMessageType::INVALID;  // gives the type of the message. use the type: MapMessageType.
    MapMsgReceptionStatus receptionStatus =
        MapMsgReceptionStatus::INVALID; // Gives the status of reception of the message. use the
                                        // type:MapMsgReceptionStatus
    int size = 0;             // The overall size in bytes of the original message as received from the network.
    int attachment_size = 0;  // The overall size of the attachments in bytes.
    MapBoolType text =
        MapBoolType::INVALID;   // Value "yes" indicates the original message or (in case of multipart-messages) that a
                                // part of the message includes textual content; "no" indicates that the message has
                                // no textual content, only binary.
    MapMessageStatus read =
        MapMessageStatus::INVALID;  // Value "yes" indicates that the message has already been read on the MSE; "no"
                                    // indicates that the message has not yet been read
    MapBoolType sent = MapBoolType::INVALID;    // Value "yes" indicates that the message has already been sent to the
                                                // recipient; "no" indicates that the message has not yet been sent.
    MapBoolType protected_ =
        MapBoolType::INVALID;   // Value "yes" indicates that the message or a part of the message (e.g., attachment) is
                                // protected by a DRM scheme; "no" indicates that the message is not protected by DRM.
    MapBoolType priority = MapBoolType::INVALID;    // Value "yes" indicates that the message is of high priority; "no"
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
 * @since 1.0
 * @version 1.0
 */
struct IProfileMessagesListingParamStruct {
    MapOnOffType NewMessage = MapOnOffType::INVALID;  // on off
    std::string MseTime = "";                         // String with current time basis and UTC-offset of the MSE.
    uint16_t ListingSize = 0;
    std::string DatabaseIdentifier = "";    // 128-bit value in hex string format,max 32 bytes
    std::string FolderVersionCounter = "";  // 128-bit value in hex string format,max 32 bytes
    std::string Version = "";               // "1.1" or "1.0"
};

/**
 * @brief MessagesListing  object
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileMessagesListing {
    std::vector<IProfileMessageOutline> messageOutlineList_ {};
    IProfileMessagesListingParamStruct messagesListingParam_ {};
    std::string messagesListingStringObject_ = "";  // all string of the messagelist object
};

/**
 * @brief GetMessagesListingParameters
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileGetMessagesListingParameters {
    std::u16string folder = u"";   // such as: inbox , outbox
    uint16_t MaxListCount = 0;     // maximum number of the listing
    uint16_t ListStartOffset = 0;  // the offset of the first entry of the returned listing object
    uint8_t SubjectLength = 0;
    uint32_t ParameterMask = 0;     // Bit mask:
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
    uint8_t FilterMessageType = 0;  // Bit mask:
                                    // MAP_FILTER_MESSAGE_MASK_SMS_GSM         0b00000001
                                    // MAP_FILTER_MESSAGE_MASK_SMS_CDMA        0b00000010
                                    // MAP_FILTER_MESSAGE_MASK_EMAIL       0b00000100
                                    // MAP_FILTER_MESSAGE_MASK_MMS         0b00001000
                                    // MAP_FILTER_MESSAGE_MASK_IM          0b00010000
                                    // MAP_FILTER_MESSAGE_MASK_NO_FILTERING    0b00000000
    std::string FilterPeriodBegin = ""; // String with Begin of filter period.
    std::string FilterPeriodEnd = "";   // String with End of filter period.
    uint8_t FilterReadStatus = 0;       // Bit mask:
                                        // MAP_FILTER_READ_STATUS_MASK_UNREAD          0b00000001
                                        // MAP_FILTER_READ_STATUS_MASK_READ            0b00000010
                                        // MAP_FILTER_READ_STATUS_MASK_NO_FILTERING    0b00000000

    std::string FilterRecipient = "";   // Text (UTF-8), wildcards "*" may be used if required
    std::string FilterOriginator = "";  // Text (UTF-8), wildcards "*" may be used if required
    uint8_t FilterPriority = 0;         // Bit mask:
                                        // MAP_FILTER_PRIORITY_MASK_HIGH               0b00000001
                                        // MAP_FILTER_PRIORITY_MASK_NO_HIGH            0b00000010
                                        // MAP_FILTER_PRIORITY_MASK_NO_FILTERING       0b00000000
    std::string ConversationID = "";
    std::string FilterMessageHandle = "";
};
/**
 * @brief GetMessageParameters
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileGetMessageParameters {
    MapAttachmentType Attachment = MapAttachmentType::INVALID;
    MapCharsetType Charset = MapCharsetType::INVALID;
    MapFractionRequestType FractionRequest = MapFractionRequestType::INVALID;
};
/**
 * @brief Get Conversation Listing Parameters
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileGetConversationListingParameters {
    uint16_t MaxListCount = 0;
    uint16_t ListStartOffset = 0;
    uint8_t FilterReadStatus = 0;       // mask type is:
                                        // MAP_FILTER_READ_STATUS_MASK_UNREAD          0b00000001
                                        // MAP_FILTER_READ_STATUS_MASK_READ            0b00000010
                                        // MAP_FILTER_READ_STATUS_MASK_NO_FILTERING    0b00000000
    std::string FilterRecipient = "";   // Text (UTF-8), wildcards "*" may be used if required
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
 * @since 1.0
 * @version 1.0
 */
struct IProfileOwnerStatus {
    uint8_t PresenceAvailability = 0;
    std::string PresenceText = "";
    std::string LastActivity = "";
    uint8_t ChatState = 0;
};
/**
 * @brief Owner Status Parameters
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileSetOwnerStatusParameters {
    std::string ConversationID = "";
    IProfileOwnerStatus ownerStatus_ {};
};

/**
 * @brief Vcard 3_0
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileMapVcard {
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
 * @since 1.0
 * @version 1.0
 */
struct IProfileBMessageContent {
    // <bmessage-body-part-ID>
    std::string bodyPartID = "";    // This property shall be used if and only if the content of the related message
                                    // cannot be delivered completely within one <bmessage-content> object
    // <bmessage-body-property>
    std::string body_encoding = "";     // Email/MMS [6], [9]: "8BIT" (for 8-Bit Clean encoding).
                                        // GSM-SMS [14]: "G-7BIT" (GSM 7 bit Default Alphabet), "G-7BITEXT" (GSM 7 bit
                                        // Alphabet with national language extension), "G-UCS2", and "G-8BIT". CDMA-SMS
                                        // [13]: "C-8BIT" (Octet, unspecified), "C-EPM" (Extended Protocol Message),
                                        // "C-7ASCII (7-bit ASCII), "C-IA5" (IA5), "C-UNICODE" (UNICODE), "C-SJIS"
                                        // (Shift-JIS), "C-KOREAN" (Korean),"C-LATINHEB” (Latin/Hebrew), and “C-LATIN”
                                        // (Latin). For IM: [6] "8BIT" (for 8-Bit Clean encoding). Ver1.1
    std::string body_charset = "";  // UTF-8 or SMS PDU
    std::string body_language = ""; // GSM-SMS [14]: " TURKISH", " SPANISH", "PORTUGUESE", "UNKNOWN"
                                    // CDMA-SMS [13]: "ENGLISH", "FRENCH", "SPANISH", "JAPANESE", "KOREAN", "CHINESE",
                                    // "HEBREW", "UNKNOWN"
    int body_content_length = 0;

    // <bmessage-body-content>
    std::string body_content = "";  // body-content  support V1.1 IM
};

/**
 * @brief <bmessage-envelope>
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileBMessageEnvelope {
    // [<initial-recipient>] or [<final-recipient> if level2 and level3 size is 0]
    std::vector<IProfileMapVcard> recipientLevel1_;
    // [<middle-recipient>] or [<final-recipient> if level3 size is 0]
    std::vector<IProfileMapVcard> recipientLevel2_;
    // [<final-recipient>]
    std::vector<IProfileMapVcard> recipientLevel3_;
    // The maximum level of <bmessage-envelope>
    int maxLevelOfEnvelope_ = 0;
    // <bmessage-content>
    IProfileBMessageContent msgBody_ {};
};

/**
 * @brief receive msg, support both 1.0 and 1.1
 */
struct IProfileBMessageStruct {
    // <bmessage-property>
    std::string version_property = "";
    MapMessageStatus readstatus_property = MapMessageStatus::INVALID;  // "READ" or "UNREAD"
    MapMessageType type_property =
        MapMessageType::INVALID;  // V1.0 include "EMAIL","SMS_GSM","SMS_CDMA","MMS", V1.1 add a "IM"
    std::u16string folder_property =
        u"";  // This is the folder name including the path where the bMessage is located in.
    std::string extendeddata_property = "";  // extended data only V1.1
    // [<bmessage-originator>]? , 0 or 1
    std::vector<IProfileMapVcard> originator_ {};  // This property includes a vCard identifying the originator
    // <bmessage-envelope>
    IProfileBMessageEnvelope envelope_ {};  // The maximum level of <bmessage-envelope> encapsulation shall be three.
};

/**
 * @brief IProfileBMessage
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileBMessage {
    MapFractionDeliverType FractionDeliver =
        MapFractionDeliverType::INVALID;  // MapFractionDeliverType type, 0 = "more",1 = "last"
    IProfileBMessageStruct bMessageParam_ {};
    std::string bMessageStringObject_ = "";  // all string of the bmessage object
};

/**
 * @brief Map SendMessage Parameters
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileSendMessageParameters {
    std::string ConversationID = "";
    MapAttachmentType Attachment = MapAttachmentType::INVALID;
    MapCharsetType Charset = MapCharsetType::INVALID;
    MapOnOffType Transparent = MapOnOffType::INVALID;
    MapOnOffType Retry = MapOnOffType::INVALID;                 // 1 is "ON", 0 is "OFF"
    std::string MessageHandle = "";                             // if ModifyText is “PREPEND”
    MapModifyTextType ModifyText = MapModifyTextType::INVALID;  // 0 is “REPLACE” , 1 is “PREPEND”
    IProfileBMessageStruct bmessage_ {};
};
/**
 * @brief Mas Instance Info Parameters
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileMasInstanceInfo {
    std::string OwnerUCI = "";
    std::string MASInstanceInformation = "";
    int supportedMsgTypes_ = 0; // Supported Message Types mask by one instance client
                                // MAP_MCE_SUPPORTED_MESSAGE_TYPE_EMAIL    0x01
                                // MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_GSM  0x02
                                // MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_CDMA 0x04
                                // MAP_MCE_SUPPORTED_MESSAGE_TYPE_MMS  0x08
                                // MAP_MCE_SUPPORTED_MESSAGE_TYPE_IM   0x10
                                // MAP_MCE_SUPPORTED_MESSAGE_TYPE_ALL  0x01F
    int instanceId = 0;         // instance id
};
/**
 * @brief Map MasInstanceInfo List Parameters
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileMasInstanceInfoList {
    bool isValid = false;  // data ready
    std::vector<IProfileMasInstanceInfo> masInfoList {};
};

/**
 * @brief IProfile Folder list param
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileFolderListing {
    uint16_t folderListingSize_ = 0;
    std::string version_ = "";
    std::vector<std::string> folderNameList_ {};
    std::string folderListingStringObject_ = "";
};

/**
 * @brief Map Action
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileMapAction {
    MapActionType action_ = MapActionType::UPDATE_INBOX;
    int supportedFeatures_ = 0;
    IProfileOwnerStatus ownerStatus_ {};
};

/**
 * @brief set message status
 *
 * @since 1.0
 * @version 1.0
 */
struct IProfileSetMessageStatus {
    std::u16string msgHandle {};
    MapStatusIndicatorType statusIndicator = MapStatusIndicatorType::INVALID;
    MapStatusValueType statusValue = MapStatusValueType::NO;
    std::string extendedData {};
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MCE_PARAMETER_H
