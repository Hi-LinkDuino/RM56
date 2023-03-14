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

#ifndef PBAP_PCE_DEF_H
#define PBAP_PCE_DEF_H

#include <cstdint>
#include <cstring>
#include "base_def.h"
#include "log.h"
#include "log_util.h"

#define PBAP_PCE_ENABLE_L2CAP_PSM
#define PBAP_PCE_LOG_DEBUG(fmt, ...) LOG_DEBUG("[PBAP_PCE]:" fmt, ##__VA_ARGS__)
#define PBAP_PCE_LOG_INFO(fmt, ...) LOG_INFO("[PBAP_PCE]:" fmt, ##__VA_ARGS__)
#define PBAP_PCE_LOG_ERROR(fmt, ...) LOG_ERROR("[PBAP_PCE]:" fmt, ##__VA_ARGS__)

namespace OHOS {
namespace bluetooth {
/// phone book supported features
enum class PbapSupportedFeatures : uint8_t {
    PCE_ORDER = 0x01,
    PCE_SEARCH_VALUE = 0x02,
    PCE_SEARCH_PROPERTY = 0x03,
    PCE_MAX_LIST_COUNT = 0x04,
    PCE_LIST_START_OFFSET = 0x05,
    PCE_PROPERTY_SELECTOR = 0x06,
    PCE_FORMAT = 0x07,
    PCE_PHONE_BOOK_SIZE = 0x08,
    PCE_NEW_MISSED_CALLS = 0x09,
    PCE_PRIMARY_FOLDER_VERSION = 0x0A,
    PCE_SECONDARY_FOLDER_VERSION = 0x0B,
    PCE_VCARD_SELECTOR = 0x0C,
    PCE_DATABASE_IDENTIFIER = 0x0D,
    PCE_VCARD_SELECTOR_OPERATOR = 0x0E,
    PCE_RESET_NEW_MISSED_CALLS = 0x0F,
    PCE_PBAP_SUPPORTED_FEATURES = 0x10
};

/// download feature
static const uint32_t PBAP_PCE_FEATURES_DOWNLOAD = 1 << 0;
/// browsing feature
static const uint32_t PBAP_PCE_FEATURES_BROWSING = 1 << 1;
/// database identifier feature
static const uint32_t PBAP_PCE_FEATURES_DATABASE_IDENTIFIER = 1 << 2;
/// folder version counter feature
static const uint32_t PBAP_PCE_FEATURES_FOLDER_VERSION_COUNTERS = 1 << 3;
/// vcard selecting feature
static const uint32_t PBAP_PCE_FEATURES_VCARD_SELECTING = 1 << 4;
/// enhanced missed calls feature
static const uint32_t PBAP_PCE_FEATURES_ENHANCED_MISSED_CALLS = 1 << 5;
/// uci property feature
static const uint32_t PBAP_PCE_FEATURES_X_BT_UCI_VCARD_PROPERTY = 1 << 6;
/// uid property feature
static const uint32_t PBAP_PCE_FEATURES_X_BT_UID_VCARD_PROPERTY = 1 << 7;
/// contact referencing feature
static const uint32_t PBAP_PCE_FEATURES_CONTACT_REFERENCING = 1 << 8;
/// contact image feature
static const uint32_t PBAP_PCE_FEATURES_DEFAULT_CONTACT_IMAGE_FORMAT = 1 << 9;

/// max device
static const int PCE_MAXIMUM_DEVICES = 10;

/// uuid length
static const uint8_t PBAP_PCE_SERVICE_UUID_LEN = 16;

/// pbap uuid
static constexpr uint8_t PBAP_PCE_SERVICE_UUID[PBAP_PCE_SERVICE_UUID_LEN] = {
    0x79, 0x61, 0x35, 0xf0, 0xf0, 0xc5, 0x11, 0xd8, 0x09, 0x66, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66
};

static const std::string PCE_SERVICE_NAME = "Phonebook Access PCE";
static const uint16_t PBAP_PCE_LOCAL_GOEP_L2CAP_PSM = 0x1003;

// PBAP v1.2.3 Sec. 7.1.2
static const int PBAP_PCE_SUPPORTED_REPOS_LOCALPB = 1 << 0;
static const int PBAP_PCE_SUPPORTED_REPOS_SIMCARD = 1 << 1;
static const int PBAP_PCE_SUPPORTED_REPOS_FAV = 1 << 3;
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PCE_DEF_H