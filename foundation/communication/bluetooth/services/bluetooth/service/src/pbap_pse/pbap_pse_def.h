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

#ifndef PBAP_PSE_DEF_H
#define PBAP_PSE_DEF_H

#include <cstdint>
#include <string>
#include "log.h"
namespace OHOS {
namespace bluetooth {
#define PBAP_PSE_L2CAP_ENABLE
#define PBAP_PSE_LOG_DEBUG(fmt, ...) LOG_DEBUG("[PBAP_PSE]:" fmt, ##__VA_ARGS__)
#define PBAP_PSE_LOG_INFO(fmt, ...) LOG_INFO("[PBAP_PSE]:" fmt, ##__VA_ARGS__)
#define PBAP_PSE_LOG_ERROR(fmt, ...) LOG_ERROR("[PBAP_PSE]:" fmt, ##__VA_ARGS__)

static const std::string PBAP_PSE_SERVICE_NAME = "Phonebook Access PSE";
static const uint16_t PBAP_PSE_GOEP_L2CAP_PSM = 0x1001;

// incoming connect time out 25 sec (must < l2cap's timeout 30 sec)
static const uint16_t PBAP_PSE_INCOMING_TIME_OUT = 25000;

static const uint8_t PBAP_SERVICE_UUID_LEN = 16;
static constexpr uint8_t PBAP_SERVICE_UUID[PBAP_SERVICE_UUID_LEN] = {
    0x79, 0x61, 0x35, 0xf0, 0xf0, 0xc5, 0x11, 0xd8, 0x09, 0x66, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66
};

// Supported pbap repositories
static const uint8_t PBAP_SUPPORTED_LOCAL_PHONEBOOK = 1 << 0;
static const uint8_t PBAP_SUPPORTED_SIM_CARD = 1 << 1;
static const uint8_t PBAP_SUPPORTED_SPEED_DIAL = 1 << 2;
static const uint8_t PBAP_SUPPORTED_FAVORITES = 1 << 3;
static constexpr uint8_t PBAP_PSE_SUPPORTED_REPOSITORIES =
    PBAP_SUPPORTED_LOCAL_PHONEBOOK | PBAP_SUPPORTED_SIM_CARD | PBAP_SUPPORTED_SPEED_DIAL | PBAP_SUPPORTED_FAVORITES;

// Supported pbap features
static const uint32_t PBAP_FEATURES_DOWNLOAD = 1 << 0;
static const uint32_t PBAP_FEATURES_BROWSING = 1 << 1;
static const uint32_t PBAP_FEATURES_DATABASE_IDENTIFIER = 1 << 2;
static const uint32_t PBAP_FEATURES_FOLDER_VERSION_COUNTERS = 1 << 3;
static const uint32_t PBAP_FEATURES_VCARD_SELECTING = 1 << 4;
static const uint32_t PBAP_FEATURES_ENHANCED_MISSED_CALLS = 1 << 5;
static const uint32_t PBAP_FEATURES_X_BT_UCI_VCARD_PROPERTY = 1 << 6;
static const uint32_t PBAP_FEATURES_X_BT_UID_VCARD_PROPERTY = 1 << 7;
static const uint32_t PBAP_FEATURES_CONTACT_REFERENCING = 1 << 8;
static const uint32_t PBAP_FEATURES_DEFAULT_CONTACT_IMAGE_FORMAT = 1 << 9;
static constexpr uint32_t PBAP_PSE_SUPPORTED_FEATURES = PBAP_FEATURES_DOWNLOAD |
                                                        PBAP_FEATURES_BROWSING |
                                                        PBAP_FEATURES_DATABASE_IDENTIFIER |
                                                        PBAP_FEATURES_FOLDER_VERSION_COUNTERS |
                                                        PBAP_FEATURES_VCARD_SELECTING |
                                                        PBAP_FEATURES_X_BT_UCI_VCARD_PROPERTY |
                                                        PBAP_FEATURES_X_BT_UID_VCARD_PROPERTY |
                                                        PBAP_FEATURES_CONTACT_REFERENCING |
                                                        PBAP_FEATURES_DEFAULT_CONTACT_IMAGE_FORMAT;

static const std::string PBAP_PSE_TYPE_PB = "x-bt/phonebook";           // type for PullPhoneBook function
static const std::string PBAP_PSE_TYPE_LISTING = "x-bt/vcard-listing";  // type for PullvCardListing function
static const std::string PBAP_PSE_TYPE_VCARD = "x-bt/vcard";            // type for PullvCardEntry Function
static const std::u16string PBAP_PSE_X_BT_UID_PREFIX = u"X-BT-UID:";
static const std::u16string PBAP_PSE_VCARD_SUFFIX = u".vcf";
static const uint8_t PBAP_PSE_HANDLE_MAX_LENGTH = 4;
static const std::string PBAP_PSE_DEFAULT_DB_FILE = "pbap_pse.s3db";
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PSE_DEF_H