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

#include "map_mse_params.h"

namespace OHOS {
namespace bluetooth {
// Tag Id
const uint8_t MapMseParams::PARAM_MAX_LIST_COUNT;
const uint8_t MapMseParams::PARAM_LIST_START_OFF_SET;
const uint8_t MapMseParams::PARAM_FILTER_MESSAGE_TYPE;
const uint8_t MapMseParams::PARAM_FILTER_PERIOD_BEGIN;
const uint8_t MapMseParams::PARAM_FILTER_PERIOD_END;
const uint8_t MapMseParams::PARAM_FILTER_READ_STATUS;
const uint8_t MapMseParams::PARAM_FILTER_RECIPIENT;
const uint8_t MapMseParams::PARAM_FILTER_ORIGINATOR;
const uint8_t MapMseParams::PARAM_FILTER_PRIORITY;
const uint8_t MapMseParams::PARAM_ATTACHMENT;
const uint8_t MapMseParams::PARAM_TRANSPARENT;
const uint8_t MapMseParams::PARAM_RETRY;
const uint8_t MapMseParams::PARAM_NEW_MESSAGE;
const uint8_t MapMseParams::PARAM_NOTIFICATION_STATUS;
const uint8_t MapMseParams::PARAM_MAS_INSTANCEID;
const uint8_t MapMseParams::PARAM_PARAMETER_MASK;
const uint8_t MapMseParams::PARAM_FOLDER_LISTING_SIZE;
const uint8_t MapMseParams::PARAM_LISTING_SIZE;
const uint8_t MapMseParams::PARAM_SUBJECT_LENGTH;
const uint8_t MapMseParams::PARAM_CHARSET;
const uint8_t MapMseParams::PARAM_FRACTION_REQUEST;
const uint8_t MapMseParams::PARAM_FRACTION_DELIVER;
const uint8_t MapMseParams::PARAM_STATUS_INDICATOR;
const uint8_t MapMseParams::PARAM_STATUS_VALUE;
const uint8_t MapMseParams::PARAM_MSE_TIME;
const uint8_t MapMseParams::PARAM_DATABASE_IDENTIFIER;
const uint8_t MapMseParams::PARAM_CONVERSATION_LISTING_VERSION_COUNTER;
const uint8_t MapMseParams::PARAM_PRESENCE_AVAILABILITY;
const uint8_t MapMseParams::PARAM_PRESENCE_TEXT;
const uint8_t MapMseParams::PARAM_LAST_ACTIVITY;
const uint8_t MapMseParams::PARAM_FILTER_LAST_ACTIVITY_BEGIN;
const uint8_t MapMseParams::PARAM_FILTER_LAST_ACTIVITY_END;
const uint8_t MapMseParams::PARAM_CHAT_STATE;
const uint8_t MapMseParams::PARAM_CONVERSATION_ID;
const uint8_t MapMseParams::PARAM_FOLDER_VERSION_COUNTER;
const uint8_t MapMseParams::PARAM_FILTER_MESSAGE_HANDLE;
const uint8_t MapMseParams::PARAM_NOTIFICATION_FILTER_MASK;
const uint8_t MapMseParams::PARAM_CONV_PARAMETER_MASK;
const uint8_t MapMseParams::PARAM_OWNER_UCI;
const uint8_t MapMseParams::PARAM_EXTENDED_DATA;
const uint8_t MapMseParams::PARAM_MAP_SUPPORTED_FEATURES;
const uint8_t MapMseParams::PARAM_MESSAGE_HANDLE;
const uint8_t MapMseParams::PARAM_MODIFY_TEXT;
// Tag Length
const uint8_t MapMseParams::TAG_LEN_MAX_LIST_COUNT;
const uint8_t MapMseParams::TAG_LEN_LIST_START_OFF_SET;
const uint8_t MapMseParams::TAG_LEN_FILTER_MESSAGE_TYPE;
const uint8_t MapMseParams::TAG_LEN_FILTER_PERIOD_BEGIN;
const uint8_t MapMseParams::TAG_LEN_FILTER_PERIOD_END;
const uint8_t MapMseParams::TAG_LEN_FILTER_READ_STATUS;
const uint8_t MapMseParams::TAG_LEN_FILTER_PRIORITY;
const uint8_t MapMseParams::TAG_LEN_ATTACHMENT;
const uint8_t MapMseParams::TAG_LEN_TRANSPARENT;
const uint8_t MapMseParams::TAG_LEN_RETRY;
const uint8_t MapMseParams::TAG_LEN_NEW_MESSAGE;
const uint8_t MapMseParams::TAG_LEN_NOTIFICATION_STATUS;
const uint8_t MapMseParams::TAG_LEN_MAS_INSTANCEID;
const uint8_t MapMseParams::TAG_LEN_PARAMETER_MASK;
const uint8_t MapMseParams::TAG_LEN_FOLDER_LISTING_SIZE;
const uint8_t MapMseParams::TAG_LEN_LISTING_SIZE;
const uint8_t MapMseParams::TAG_LEN_SUBJECT_LENGTH;
const uint8_t MapMseParams::TAG_LEN_CHARSET;
const uint8_t MapMseParams::TAG_LEN_FRACTION_REQUEST;
const uint8_t MapMseParams::TAG_LEN_FRACTION_DELIVER;
const uint8_t MapMseParams::TAG_LEN_STATUS_INDICATOR;
const uint8_t MapMseParams::TAG_LEN_STATUS_VALUE;
const uint8_t MapMseParams::TAG_LEN_DATABASE_IDENTIFIER;
const uint8_t MapMseParams::TAG_LEN_CONVERSATION_LISTING_VERSION_COUNTER;
const uint8_t MapMseParams::TAG_LEN_PRESENCE_AVAILABILITY;
const uint8_t MapMseParams::TAG_LEN_CHAT_STATE;
const uint8_t MapMseParams::TAG_LEN_CONVERSATION_ID;
const uint8_t MapMseParams::TAG_LEN_FOLDER_VERSION_COUNTER;
const uint8_t MapMseParams::TAG_LEN_FILTER_MESSAGE_HANDLE;
const uint8_t MapMseParams::TAG_LEN_NOTIFICATION_FILTER_MASK;
const uint8_t MapMseParams::TAG_LEN_CONV_PARAMETER_MASK;
const uint8_t MapMseParams::TAG_LEN_MAP_SUPPORTED_FEATURES;
const uint8_t MapMseParams::TAG_LEN_MESSAGE_HANDLE;
const uint8_t MapMseParams::TAG_LEN_MODIFY_TEXT;
MapMseParams::MapMseParams(const ObexTlvParamters &tlvParams)
{
    ParseParameter(tlvParams.GetTlvTriplets());
}

MapMseParams::~MapMseParams()
{}

void MapMseParams::SetMaxListCount(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() > TAG_LEN_MAX_LIST_COUNT) {
        MSE_LOG_ERROR("MAX_LIST_COUNT: Wrong length received: %hhx expected: less than %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_MAX_LIST_COUNT);
        return;
    }
    maxListCount_ = tlvTriplet->GetUint16();
}

void MapMseParams::SetListStartOffSet(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() > TAG_LEN_LIST_START_OFF_SET) {
        MSE_LOG_ERROR("LIST_START_OFF_SET: Wrong length received: %hhx expected: less than %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_LIST_START_OFF_SET);
        return;
    }
    listStartOffSet_ = tlvTriplet->GetUint16();
}

void MapMseParams::SetFilterMessageType(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_FILTER_MESSAGE_TYPE) {
        MSE_LOG_ERROR("FILTER_MESSAGE_TYPE: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_FILTER_MESSAGE_TYPE);
        return;
    }
    filterMessageType_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetFilterOriginator(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() == 0x00) {
        MSE_LOG_ERROR("FILTER_ORIGINATOR: Wrong length received: %hhx expected to be more than 0",
            tlvTriplet->GetLen());
        return;
    }
    filterOriginator_ = std::string((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
}

void MapMseParams::SetFilterPeriodBegin(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() > TAG_LEN_FILTER_PERIOD_BEGIN) {
        MSE_LOG_ERROR("FILTER_PERIOD_BEGIN: Wrong length received: %hhx expected: less than %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_FILTER_PERIOD_BEGIN);
        return;
    }
    std::string ymdhmsStr((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
    filterPeriodBegin_ = ymdhmsStr;
}

void MapMseParams::SetFilterPeriodEnd(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() > TAG_LEN_FILTER_PERIOD_END) {
        MSE_LOG_ERROR("END_FILTER_PERIOD_END: Wrong length received: %hhx expected: less than %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_FILTER_PERIOD_END);
        return;
    }
    std::string ymdhmsStr((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
    filterPeriodEnd_ = ymdhmsStr;
}

void MapMseParams::SetFilterReadStatus(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_FILTER_READ_STATUS) {
        MSE_LOG_ERROR("FILTER_READ_STATUS: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_FILTER_READ_STATUS);
        return;
    }
    filterReadStatus_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetFilterRecipient(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() == 0x00) {
        MSE_LOG_ERROR("FILTER_RECIPIENT: Wrong length received: %hhx expected to be more than 0",
            tlvTriplet->GetLen());
        return;
    }
    filterRecipient_ = std::string((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
}

void MapMseParams::SetFilterPriority(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_FILTER_PRIORITY) {
        MSE_LOG_ERROR("FILTER_PRIORITY: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_FILTER_PRIORITY);
        return;
    }
    filterPriority_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetAttachment(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_ATTACHMENT) {
        MSE_LOG_ERROR("ATTACHMENT: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_ATTACHMENT);
        return;
    }
    attachment_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetTransparent(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_TRANSPARENT) {
        MSE_LOG_ERROR("TRANSPARENT: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_TRANSPARENT);
        return;
    }
    transparent_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetRetry(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_RETRY) {
        MSE_LOG_ERROR("RETRY: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_RETRY);
        return;
    }
    retry_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetNotificationStatus(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_NOTIFICATION_STATUS) {
        MSE_LOG_ERROR("NOTIFICATION_STATUS: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_NOTIFICATION_STATUS);
        return;
    }
    notificationStatus_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetMasInstanceId(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_MAS_INSTANCEID) {
        MSE_LOG_ERROR("MAS_INSTANCEID: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_MAS_INSTANCEID);
        return;
    }
    masInstanceId_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetParameterMask(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() > TAG_LEN_PARAMETER_MASK) {
        MSE_LOG_ERROR("PARAMETER_MASK: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_PARAMETER_MASK);
        return;
    }
    parameterMask_ = tlvTriplet->GetUint32();
}

void MapMseParams::SetFolderListingSize(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() > TAG_LEN_FOLDER_LISTING_SIZE) {
        MSE_LOG_ERROR("FOLDER_LISTING_SIZE: Wrong length received: %hhx expected: less than %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_FOLDER_LISTING_SIZE);
        return;
    }
    folderListingSize_ = tlvTriplet->GetUint16();
}

void MapMseParams::SetListingSize(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() > TAG_LEN_LISTING_SIZE) {
        MSE_LOG_ERROR("LISTING_SIZE: Wrong length received: %hhx expected: less than %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_LISTING_SIZE);
        return;
    }
    listingSize_ = tlvTriplet->GetUint16();
}

void MapMseParams::SetSubjectLength(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_SUBJECT_LENGTH) {
        MSE_LOG_ERROR("SUBJECT_LENGTH: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_SUBJECT_LENGTH);
        return;
    }
    subjectLength_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetCharset(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_CHARSET) {
        MSE_LOG_ERROR("CHARSET: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_CHARSET);
        return;
    }
    charSet_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetFractionRequest(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_FRACTION_REQUEST) {
        MSE_LOG_ERROR("FRACTION_REQUEST: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_FRACTION_REQUEST);
        return;
    }
    fractionRequest_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetFractionDeliver(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_FRACTION_DELIVER) {
        MSE_LOG_ERROR("FRACTION_DELIVER: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_FRACTION_DELIVER);
        return;
    }
    fractionDeliver_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetStatusIndicator(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_STATUS_INDICATOR) {
        MSE_LOG_ERROR("STATUS_INDICATOR: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_STATUS_INDICATOR);
        return;
    }
    statusIndicator_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetStatusValue(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_STATUS_VALUE) {
        MSE_LOG_ERROR(
            "STATUS_VALUE: Wrong length received: %hhx expected: %hhx", tlvTriplet->GetLen(), TAG_LEN_STATUS_VALUE);
        return;
    }
    statusValue_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetExtendedData(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    extendedData_ = std::string((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
}

void MapMseParams::SetFilterLastActivityBegin(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() == 0x00) {
        MSE_LOG_ERROR("FILTER_LAST_ACTIVITY_BEGIN: Wrong length received: %hhx expected to be more than 0",
            tlvTriplet->GetLen());
        return;
    }
    filterLastActivityBegin_ = std::string((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
}

void MapMseParams::SetFilterLastActivityEnd(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() == 0x00) {
        MSE_LOG_ERROR("FILTER_LAST_ACTIVITY_END: Wrong length received: %hhx expected to be more than 0",
            tlvTriplet->GetLen());
        return;
    }
    filterLastActivityEnd_ = std::string((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
}

void MapMseParams::SetChatState(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_CHAT_STATE) {
        MSE_LOG_ERROR("CHAT_STATE: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_CHAT_STATE);
        return;
    }
    chatState_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetConversationId(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() > TAG_LEN_CONVERSATION_ID) {
        MSE_LOG_ERROR("CONVERSATION_ID: Wrong length received: %hhx expected to be less than %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_CONVERSATION_ID);
        return;
    }
    conversationId_ = std::string((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
}

void MapMseParams::SetNotificationFilterMask(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_NOTIFICATION_FILTER_MASK) {
        MSE_LOG_ERROR("NOTIFICATION_FILTER_MASK: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_NOTIFICATION_FILTER_MASK);
        return;
    }
    notificationFilterMask_ = tlvTriplet->GetUint32();
}

void MapMseParams::SetFilterMessageHandle(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() > TAG_LEN_FILTER_MESSAGE_HANDLE) {
        MSE_LOG_ERROR("FILTER_MESSAGE_HANDLE: Wrong length received: %hhx expected to be less than %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_FILTER_MESSAGE_HANDLE);
        return;
    }
    filterMessageHandle_ = std::string((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
}

void MapMseParams::SetConvParameterMask(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_CONV_PARAMETER_MASK) {
        MSE_LOG_ERROR("CONV_PARAMETER_MASK: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_CONV_PARAMETER_MASK);
        return;
    }
    convParameterMask_ = tlvTriplet->GetUint32();
}

void MapMseParams::SetOwnerUci(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() == 0x00) {
        MSE_LOG_ERROR("OWNER_UCI: Wrong length received: %hhx expected to be more than 0",
            tlvTriplet->GetLen());
        return;
    }
    ownerUci_ = std::string((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
}

void MapMseParams::SetModifyText(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() != TAG_LEN_MODIFY_TEXT) {
        MSE_LOG_ERROR("MODIFY_TEXT: Wrong length received: %hhx expected: %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_MODIFY_TEXT);
        return;
    }
    modifyText_ = std::make_unique<uint8_t>(tlvTriplet->GetVal()[0]);
}

void MapMseParams::SetMessageHandle(const std::unique_ptr<TlvTriplet> &tlvTriplet)
{
    if (tlvTriplet->GetLen() > TAG_LEN_MESSAGE_HANDLE) {
        MSE_LOG_ERROR("MESSAGE_HANDLE: Wrong length received: %hhx expected to be less than %hhx",
            tlvTriplet->GetLen(),
            TAG_LEN_MESSAGE_HANDLE);
        return;
    }
    messageHandle_ = std::string((char *)tlvTriplet->GetVal(), tlvTriplet->GetLen());
}

void MapMseParams::ParseParameter(const std::vector<std::unique_ptr<TlvTriplet>> &tlvTriplets)
{
    for (auto &para : tlvTriplets) {
        switch (para->GetTagId()) {
            case PARAM_MAX_LIST_COUNT:
                SetMaxListCount(para);
                break;
            case PARAM_LIST_START_OFF_SET:
                SetListStartOffSet(para);
                break;
            case PARAM_FILTER_MESSAGE_TYPE:
                SetFilterMessageType(para);
                break;
            case PARAM_FILTER_PERIOD_BEGIN:
                SetFilterPeriodBegin(para);
                break;
            case PARAM_FILTER_PERIOD_END:
                SetFilterPeriodEnd(para);
                break;
            case PARAM_FILTER_READ_STATUS:
                SetFilterReadStatus(para);
                break;
            case PARAM_FILTER_RECIPIENT:
                SetFilterRecipient(para);
                break;
            case PARAM_FILTER_ORIGINATOR:
                SetFilterOriginator(para);
                break;
            case PARAM_FILTER_PRIORITY:
                SetFilterPriority(para);
                break;
            case PARAM_ATTACHMENT:
                SetAttachment(para);
                break;
            case PARAM_TRANSPARENT:
                SetTransparent(para);
                break;
            case PARAM_RETRY:
                SetRetry(para);
                break;
            case PARAM_NEW_MESSAGE:
                break;
            case PARAM_NOTIFICATION_STATUS:
                SetNotificationStatus(para);
                break;
            case PARAM_MAS_INSTANCEID:
                SetMasInstanceId(para);
                break;
            case PARAM_PARAMETER_MASK:
                SetParameterMask(para);
                break;
            case PARAM_FOLDER_LISTING_SIZE:
                SetFolderListingSize(para);
                break;
            case PARAM_LISTING_SIZE:
                SetListingSize(para);
                break;
            case PARAM_SUBJECT_LENGTH:
                SetSubjectLength(para);
                break;
            case PARAM_CHARSET:
                SetCharset(para);
                break;
            case PARAM_FRACTION_REQUEST:
                SetFractionRequest(para);
                break;
            case PARAM_FRACTION_DELIVER:
                SetFractionDeliver(para);
                break;
            case PARAM_STATUS_INDICATOR:
                SetStatusIndicator(para);
                break;
            case PARAM_STATUS_VALUE:
                SetStatusValue(para);
                break;
            case PARAM_MSE_TIME:
                break;
            case PARAM_DATABASE_IDENTIFIER:
                break;
            case PARAM_CONVERSATION_LISTING_VERSION_COUNTER:
                break;
            case PARAM_PRESENCE_AVAILABILITY:
                break;
            case PARAM_PRESENCE_TEXT:
                break;
            case PARAM_LAST_ACTIVITY:
                break;
            case PARAM_FILTER_LAST_ACTIVITY_BEGIN:
                SetFilterLastActivityBegin(para);
                break;
            case PARAM_FILTER_LAST_ACTIVITY_END:
                SetFilterLastActivityEnd(para);
                break;
            case PARAM_CHAT_STATE:
                SetChatState(para);
                break;
            case PARAM_CONVERSATION_ID:
                SetConversationId(para);
                break;
            case PARAM_FOLDER_VERSION_COUNTER:
                break;
            case PARAM_FILTER_MESSAGE_HANDLE:
                SetFilterMessageHandle(para);
                break;
            case PARAM_NOTIFICATION_FILTER_MASK:
                SetNotificationFilterMask(para);
                break;
            case PARAM_CONV_PARAMETER_MASK:
                SetConvParameterMask(para);
                break;
            case PARAM_OWNER_UCI:
                SetOwnerUci(para);
                break;
            case PARAM_EXTENDED_DATA:
                SetExtendedData(para);
                break;
            case PARAM_MODIFY_TEXT:
                SetModifyText(para);
                break;
            case PARAM_MESSAGE_HANDLE:
                SetMessageHandle(para);
                break;
            default:
                break;
        }
    }
}
}  // namespace bluetooth
}  // namespace OHOS
