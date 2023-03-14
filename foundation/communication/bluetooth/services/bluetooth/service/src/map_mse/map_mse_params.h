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

#ifndef MAP_MSE_PARAMS_H
#define MAP_MSE_PARAMS_H

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>
#include "../obex/obex_server.h"
#include "../obex/obex_utils.h"
#include "log.h"
#include "map_mse_types.h"

/**
 * @brief bluetooth namespace
 *
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief Application Parameters class
 *
 */
struct MapMseParams {
public:
    // Tag Id
    static const uint8_t PARAM_MAX_LIST_COUNT = 0x01;
    static const uint8_t PARAM_LIST_START_OFF_SET = 0x02;
    static const uint8_t PARAM_FILTER_MESSAGE_TYPE = 0x03;
    static const uint8_t PARAM_FILTER_PERIOD_BEGIN = 0x04;
    static const uint8_t PARAM_FILTER_PERIOD_END = 0x05;
    static const uint8_t PARAM_FILTER_READ_STATUS = 0x06;
    static const uint8_t PARAM_FILTER_RECIPIENT = 0x07;
    static const uint8_t PARAM_FILTER_ORIGINATOR = 0x08;
    static const uint8_t PARAM_FILTER_PRIORITY = 0x09;
    static const uint8_t PARAM_ATTACHMENT = 0x0A;
    static const uint8_t PARAM_TRANSPARENT = 0x0B;
    static const uint8_t PARAM_RETRY = 0x0C;
    static const uint8_t PARAM_NEW_MESSAGE = 0x0D;
    static const uint8_t PARAM_NOTIFICATION_STATUS = 0x0E;
    static const uint8_t PARAM_MAS_INSTANCEID = 0x0F;
    static const uint8_t PARAM_PARAMETER_MASK = 0x10;
    static const uint8_t PARAM_FOLDER_LISTING_SIZE = 0x11;
    static const uint8_t PARAM_LISTING_SIZE = 0x12;
    static const uint8_t PARAM_SUBJECT_LENGTH = 0x13;
    static const uint8_t PARAM_CHARSET = 0x14;
    static const uint8_t PARAM_FRACTION_REQUEST = 0x15;
    static const uint8_t PARAM_FRACTION_DELIVER = 0x16;
    static const uint8_t PARAM_STATUS_INDICATOR = 0x17;
    static const uint8_t PARAM_STATUS_VALUE = 0x18;
    static const uint8_t PARAM_MSE_TIME = 0x19;
    static const uint8_t PARAM_DATABASE_IDENTIFIER = 0x1A;
    static const uint8_t PARAM_CONVERSATION_LISTING_VERSION_COUNTER = 0x1B;
    static const uint8_t PARAM_PRESENCE_AVAILABILITY = 0x1C;
    static const uint8_t PARAM_PRESENCE_TEXT = 0x1D;
    static const uint8_t PARAM_LAST_ACTIVITY = 0x1E;
    static const uint8_t PARAM_FILTER_LAST_ACTIVITY_BEGIN = 0x1F;
    static const uint8_t PARAM_FILTER_LAST_ACTIVITY_END = 0x20;
    static const uint8_t PARAM_CHAT_STATE = 0x21;
    static const uint8_t PARAM_CONVERSATION_ID = 0x22;
    static const uint8_t PARAM_FOLDER_VERSION_COUNTER = 0x23;
    static const uint8_t PARAM_FILTER_MESSAGE_HANDLE = 0x24;
    static const uint8_t PARAM_NOTIFICATION_FILTER_MASK = 0x25;
    static const uint8_t PARAM_CONV_PARAMETER_MASK = 0x26;
    static const uint8_t PARAM_OWNER_UCI = 0x27;
    static const uint8_t PARAM_EXTENDED_DATA = 0x28;
    static const uint8_t PARAM_MAP_SUPPORTED_FEATURES = 0x29;
    static const uint8_t PARAM_MESSAGE_HANDLE = 0x2A;
    static const uint8_t PARAM_MODIFY_TEXT = 0x2B;
    // Tag Length
    static const uint8_t TAG_LEN_MAX_LIST_COUNT = 0x02;
    static const uint8_t TAG_LEN_LIST_START_OFF_SET = 0x02;
    static const uint8_t TAG_LEN_FILTER_MESSAGE_TYPE = 0x01;
    static const uint8_t TAG_LEN_FILTER_PERIOD_BEGIN = 0x10;
    static const uint8_t TAG_LEN_FILTER_PERIOD_END = 0x10;
    static const uint8_t TAG_LEN_FILTER_READ_STATUS = 0x01;
    static const uint8_t TAG_LEN_FILTER_PRIORITY = 0x01;
    static const uint8_t TAG_LEN_ATTACHMENT = 0x01;
    static const uint8_t TAG_LEN_TRANSPARENT = 0x01;
    static const uint8_t TAG_LEN_RETRY = 0x01;
    static const uint8_t TAG_LEN_NEW_MESSAGE = 0x01;
    static const uint8_t TAG_LEN_NOTIFICATION_STATUS = 0x01;
    static const uint8_t TAG_LEN_MAS_INSTANCEID = 0x01;
    static const uint8_t TAG_LEN_PARAMETER_MASK = 0x04;
    static const uint8_t TAG_LEN_FOLDER_LISTING_SIZE = 0x02;
    static const uint8_t TAG_LEN_LISTING_SIZE = 0x02;
    static const uint8_t TAG_LEN_SUBJECT_LENGTH = 0x01;
    static const uint8_t TAG_LEN_CHARSET = 0x01;
    static const uint8_t TAG_LEN_FRACTION_REQUEST = 0x01;
    static const uint8_t TAG_LEN_FRACTION_DELIVER = 0x01;
    static const uint8_t TAG_LEN_STATUS_INDICATOR = 0x01;
    static const uint8_t TAG_LEN_STATUS_VALUE = 0x01;
    static const uint8_t TAG_LEN_DATABASE_IDENTIFIER = 0x80;
    static const uint8_t TAG_LEN_CONVERSATION_LISTING_VERSION_COUNTER = 0x80;
    static const uint8_t TAG_LEN_PRESENCE_AVAILABILITY = 0x01;
    static const uint8_t TAG_LEN_CHAT_STATE = 0x01;
    static const uint8_t TAG_LEN_CONVERSATION_ID = 0x80;
    static const uint8_t TAG_LEN_FOLDER_VERSION_COUNTER = 0x80;
    static const uint8_t TAG_LEN_FILTER_MESSAGE_HANDLE = 0x40;
    static const uint8_t TAG_LEN_NOTIFICATION_FILTER_MASK = 0x04;
    static const uint8_t TAG_LEN_CONV_PARAMETER_MASK = 0x04;
    static const uint8_t TAG_LEN_MAP_SUPPORTED_FEATURES = 0x04;
    static const uint8_t TAG_LEN_MESSAGE_HANDLE = 0x40;
    static const uint8_t TAG_LEN_MODIFY_TEXT = 0x01;
    /**
     * @brief Construct a new Mse App Parameter:: Mse App Parameter object
     *
     * @param tlvTriplets Reference to the App Parameter value
     */
    explicit MapMseParams(const ObexTlvParamters &tlvParams);

    MapMseParams() = default;

    /**
     * @brief Destroy the Mse App Parameter:: Mse App Parameter object
     *
     */
    virtual ~MapMseParams();

    std::unique_ptr<uint8_t> transparent_ = nullptr;
    std::unique_ptr<uint8_t> retry_ = nullptr;
    std::string messageHandle_ = "";
    std::unique_ptr<uint8_t> modifyText_ = nullptr;
    uint16_t maxListCount_ = 0x400;
    uint16_t listStartOffSet_ = 0x0;
    std::unique_ptr<uint8_t> subjectLength_ = nullptr;
    long parameterMask_ = -1;
    uint16_t folderListingSize_ = 0;
    uint16_t listingSize_ = 0;
    std::unique_ptr<uint8_t> filterMessageType_ = nullptr;
    std::string filterPeriodBegin_ = "";
    std::string filterPeriodEnd_ = "";
    std::unique_ptr<uint8_t> filterReadStatus_ = nullptr;
    std::string filterRecipient_ = "";
    std::string filterOriginator_ = "";
    std::unique_ptr<uint8_t> filterPriority_ = nullptr;
    std::string conversationId_ = "";
    std::string filterMessageHandle_ = "";
    std::unique_ptr<uint8_t> attachment_ = nullptr;
    std::unique_ptr<uint8_t> charSet_ = nullptr;
    std::unique_ptr<uint8_t> fractionRequest_ = nullptr;
    std::unique_ptr<uint8_t> fractionDeliver_ = nullptr;
    std::string filterLastActivityBegin_ = "";
    std::string filterLastActivityEnd_ = "";
    long convParameterMask_ = -1;
    std::unique_ptr<uint8_t> presenceAvailability_ = nullptr;
    std::string presenceText_ = "";
    std::string lastActivity_ = "";
    std::unique_ptr<uint8_t> chatState_ = nullptr;
    std::unique_ptr<uint8_t> notificationStatus_ = nullptr;
    std::unique_ptr<uint8_t> masInstanceId_ = nullptr;
    std::unique_ptr<uint8_t> statusIndicator_ = nullptr;
    std::unique_ptr<uint8_t> statusValue_ = nullptr;
    std::string extendedData_ = "";
    long notificationFilterMask_ = -1;
    std::string ownerUci_ = "";

private:
    /**
     * @brief Parse an application parameter
     *
     * @param tlvTriplets Reference to the application parameters
     */
    void ParseParameter(const std::vector<std::unique_ptr<TlvTriplet>> &tlvTriplets);

    /**
     * @brief Transparent setting
     *
     * @param tlvTriplets Reference to the Transparent value
     */
    void SetTransparent(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief SetListStartOffSet setting
     *
     * @param tlvTriplet Reference to the SetListStartOffSet value
     */
    void SetListStartOffSet(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Retry object
     *
     * @param tlvTriplet Reference to the Retry value
     */
    void SetRetry(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Message Handle object
     *
     * @param tlvTriplet Reference to the Message Handle Value
     */
    void SetMessageHandle(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Modify Text object
     *
     * @param tlvTriplet Reference to the Modify Text Value
     */
    void SetModifyText(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Max List Count object
     *
     * @param tlvTriplet Reference to  Max List Count Value
     */
    void SetMaxListCount(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Subject Length object
     *
     * @param tlvTriplet Reference to the Subject Length value
     */
    void SetSubjectLength(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Parameter Mask object
     *
     * @param tlvTriplet Reference to the Parameter Mask value
     */
    void SetParameterMask(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Folder Listing Size object
     *
     * @param tlvTriplet Reference to the Folder Listing Size value
     */
    void SetFolderListingSize(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Listing Size object
     *
     * @param tlvTriplet Reference to the Listing Size value
     */
    void SetListingSize(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief SetFilterMessageType setting
     *
     * @param tlvTriplet Reference to the SetListStartOffSet value
     */
    void SetFilterMessageType(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Filter Period Begin object
     *
     * @param tlvTriplet Reference to the SetFilterOriginator value
     */
    void SetFilterPeriodBegin(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Filter Period End object
     *
     * @param tlvTriplet Reference to the SetFilterPeriodEnd value
     */
    void SetFilterPeriodEnd(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Filter Read Status object
     *
     * @param tlvTriplet Reference to the SetFilterPeriodEnd value
     */
    void SetFilterReadStatus(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Filter Recipient object
     *
     * @param tlvTriplet  Reference to the SetFilterRecipient 32_t parameterMask_;
    std::unique_ptr<uint8_t> filterMessageType_;value
     */
    void SetFilterRecipient(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Filter Originator object
     *
     * @param tlvTriplet Reference to the SetFilterOriginator value
     */
    void SetFilterOriginator(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Filter Priority object
     *
     * @param tlvTriplet  Reference to the Filter Priority value
     */
    void SetFilterPriority(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Conversation Id object
     *
     * @param tlvTriplet Reference to the Conversation Id Value
     */
    void SetConversationId(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Filter Message Handle object
     *
     * @param tlvTriplet Reference to the Filter Message Handle Value
     */
    void SetFilterMessageHandle(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Attachment object
     *
     * @param tlvTriplet Reference to the Attachment value
     */
    void SetAttachment(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Charset object
     *
     * @param tlvTriplet Reference to the Charset value
     */
    void SetCharset(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Fraction Request object
     *
     * @param tlvTriplet Reference to the Fraction Request value
     */
    void SetFractionRequest(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Fraction Deliver object
     *
     * @param tlvTriplet Reference to the Fraction Deliver value
     */
    void SetFractionDeliver(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Filter Last Activity Begin object
     *
     * @param tlvTriplet Reference to the Filter Last Activity Begin Value
     */
    void SetFilterLastActivityBegin(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set Filter Last Activity End object
     *
     * @param tlvTriplet Reference to the Filter Last Activity End Value
     */
    void SetFilterLastActivityEnd(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Conv Parameter Mask object
     *
     * @param tlvTriplet Reference to the Conv Parameter Mask Value
     */
    void SetConvParameterMask(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Chat State object
     *
     * @param tlvTriplet Reference to the Chat State Value
     */
    void SetChatState(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Notification Status object
     *
     * @param tlvTriplet Reference to the Notification Status value
     */
    void SetNotificationStatus(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the MapMseInstance Id object
     *
     * @param tlvTriplet Reference to the MapMseInstance Id value
     */
    void SetMasInstanceId(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Status Indicator object
     *
     * @param tlvTriplet Reference to the Status Indicator value
     */
    void SetStatusIndicator(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Status Value object
     *
     * @param tlvTriplet Reference to the Status Value
     */
    void SetStatusValue(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief set the Extended Data
     *
     * @param tlvTriplet Reference to the Extended Data value
     */
    void SetExtendedData(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Notification Filter Mask object
     *
     * @param tlvTriplet Reference to the Notification Filter Mask Value
     */
    void SetNotificationFilterMask(const std::unique_ptr<TlvTriplet> &tlvTriplet);

    /**
     * @brief Set the Owner Uci object
     *
     * @param tlvTriplet Reference to the Owner Uci Value
     */
    void SetOwnerUci(const std::unique_ptr<TlvTriplet> &tlvTriplet);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // MAP_MSE_PARAMS_H