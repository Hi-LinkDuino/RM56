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
 * @file avrcp_ct_internal.h
 *
 * @brief Declares macros of the AVRCP CT service.
 */

#ifndef AVRCP_CT_INTERNAL_H
#define AVRCP_CT_INTERNAL_H

#include <cmath>
#include <cstring>
#include <deque>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <vector>

#include "avctp.h"
#include "base_def.h"
#include "btstack.h"
#include "interface_profile.h"
#include "log.h"
#include "log_util.h"
#include "message.h"
#include "packet.h"
#include "raw_address.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
/// The macro used to safely delete the pointer to an object created by[new].
template<typename T>
void AvrcpCtSafeDelete(T *p)
{
    if (p != nullptr) {
        delete p;
        p = nullptr;
    }
}

/// The macro used to safely delete the pointer to an array of the object created by[new].
template<typename T>
void AvrcpCtSafeDeleteArray(T *p, int length)
{
    if (p != nullptr && length > 0) {
        delete[] p;
        p = nullptr;
    }
}
/// The default address of the bluetooth device.
static const std::string AVRC_CT_DEFAULT_BLUETOOTH_ADDRESS = "00:00:00:00:00:00";
/// The Version of AVRCP Profile is 1.6.
static const uint16_t AVRC_CT_PROFILE_REV_1_6 = 0x0106;
/// The default mtu size of the control channel.
/// 512 bytes is the smallest mtu.
static const int AVRC_CT_DEFAULT_CONTROL_MTU_SIZE = 512;
/// The default mtu size of the browse channel.
static const int AVRC_CT_DEFAULT_BROWSE_MTU_SIZE = 335;
/// The default max fragments of the control channel.
static const int AVRC_CT_DEFAULT_MAX_FRAGMENTS = 20;
/// The default Bluetooth SIG company ID.
static const int AVRC_CT_DEFAULT_BLUETOOTH_SIG_COMPANY_ID = 0x001958;
/// The AVRCP TG shall return its response frame within 100 milliseconds counting from the receipt of the command
/// frame.
/// Therefor, add an additional 1000 milliseconds, and use it as the timer for preventing that the AVRCP TG has not
/// a
/// response.
static const int AVRC_CT_TIMER_T_RCP = 100;

/// The AVRCP TG shall return its response frame within 200 milliseconds counting from the receipt of the command
/// frame.
/// Therefor, add an additional 1000 milliseconds, and use it as the timer for preventing that the AVRCP TG has not
/// a
/// response.
static const int AVRC_CT_TIMER_T_MTC = 200;

/// The AVRCP TG shall return its response frame within 1000 milliseconds counting from the receipt of the command
/// frame. Therefor, add an additional 1000 milliseconds, and use it as the timer for preventing that the AVRCP TG
/// has
/// not a response.
static const int AVRC_CT_TIMER_T_MTP = 1000;

/// A command with the pressed value is valid for two seconds from the time when a target sends back a response of
/// the
/// command.
/// @see AV/C Panel Subunit Specification 1.23 Section 9.4 PASS THROUGH control command.
static const int AVRC_CT_TIMER_PASS_THROUGH = 2000;

/// The default size of the command queue.
/// This means how many commands can be cached for a connection.
static const int AVRC_CT_DEFAULT_SIZE_OF_QUEUE = 20;

/// The maximum of number of device connections
static const int AVRC_CT_DEFAULT_MAX_OF_CONN = 6;
    /**
 * @brief This enumeration declares applicable to service class UUIDs that are registered into the SDP.
 */
using AvrcCtServiceClassUUIDs = enum {
    ///  A/V remote control target.
    AVRC_CT_AV_REMOTE_CONTROL_TARGET = 0x110C,
    /// A/V remote control.
    /// NOTE: Used as both Service Class Identifier and Profile Identifier.
    AVRC_CT_AV_REMOTE_CONTROL = 0x110E,
    /// A/V remote control controller.
    /// NOTE: The AVRCP specification v1.3 and later require that 0x110E also be included in the ServiceClassIDList
    /// before 0x110F for backwards compatibility.
    AVRC_CT_AV_REMOTE_CONTROL_CONTROLLER = 0x110F,
};

/**
 * @brief This enumeration declares the values of the moving bit
 */
enum AvrcCtFeatureMoveBit {
    AVRC_CT_FEATURE_MOVE_BIT_1 = 0x01,
    AVRC_CT_FEATURE_MOVE_BIT_2 = 0x02,
    AVRC_CT_FEATURE_MOVE_BIT_3 = 0x03,
    AVRC_CT_FEATURE_MOVE_BIT_6 = 0x06,
    AVRC_CT_FEATURE_MOVE_BIT_7 = 0x07,
    AVRC_CT_FEATURE_MOVE_BIT_8 = 0x08,
    AVRC_CT_FEATURE_MOVE_BIT_9 = 0x09,
    AVRC_CT_FEATURE_MOVE_BIT_16 = 0x10,
    AVRC_CT_FEATURE_MOVE_BIT_17 = 0x11,
    AVRC_CT_FEATURE_MOVE_BIT_18 = 0x12,
    AVRC_CT_FEATURE_MOVE_BIT_19 = 0x13,
    AVRC_CT_FEATURE_MOVE_BIT_20 = 0x14,
    AVRC_CT_FEATURE_MOVE_BIT_21 = 0x15,
    AVRC_CT_FEATURE_MOVE_BIT_22 = 0x16,
    AVRC_CT_FEATURE_MOVE_BIT_23 = 0x17,
    AVRC_CT_FEATURE_MOVE_BIT_24 = 0x18,
    AVRC_CT_FEATURE_MOVE_BIT_25 = 0x19,
};

/**
 * @brief This enumeration declares features that are supported by the AVRCP CT profile.
 *
 * @detail Some of the features will be registered into the SDP.
 * @see Audio/Video Remote Control 1.6.2 Section 8 Service discovery interoperability requirements ->
 *      Table 8.2: Service record for TG.
 */
using AvrcCtFeatures = enum {
    /// Invalid value.
    AVRC_CT_FEATURE_INVALID_FEATURE = 0x0000,

    /// Supported category 1: player/recorder.
    AVRC_CT_FEATURE_CATEGORY_1 = 0x0001,
    /// Supported category 2: monitor/amplifier.
    AVRC_CT_FEATURE_CATEGORY_2 = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_1),
    /// Supported category 3: tuner.
    AVRC_CT_FEATURE_CATEGORY_3 = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_2),
    /// Supported category 4: menu.
    AVRC_CT_FEATURE_CATEGORY_4 = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_3),
    /// Supported browsing.
    /// This in the SDP record shall only be supported if browsing of the "Media Player Virtual File system" is
    /// supported.
    AVRC_CT_FEATURE_BROWSING = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_6),
    /// Supported cover art GetImageProperties.
    AVRC_CT_FEATURE_COVER_ART_IMAGE_PROPERTIES = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_7),
    /// Supported cover art GetImage.
    AVRC_CT_FEATURE_COVER_ART_IMAGE = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_8),
    /// Supported covert art GetLinkedThumbnail.
    AVRC_CT_FEATURE_COVER_ART_THUMBNAIL = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_9),

    /// Supported the key operation.
    AVRC_CT_FEATURE_KEY_OPERATION = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_16),
    /// Supported the absolute volume.
    AVRC_CT_FEATURE_ABSOLUTE_VOLUME = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_17),
    /// Supported the notification of "PLAYBACK_STATUS_CHANGED"
    AVRC_CT_FEATURE_NOTIFY_PLAYBACK_STATUS_CHANGED = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_18),
    /// Supported the notification of "TRACK_REACHED_END"
    AVRC_CT_FEATURE_NOTIFY_TRACK_REACHED_END = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_19),
    /// Supported the notification of "TRACK_REACHED_START"
    AVRC_CT_FEATURE_NOTIFY_TRACK_REACHED_START = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_20),
    /// Supported the notification of "PLAYBACK_POSITION_CHANGED"
    AVRC_CT_FEATURE_NOTIFY_PLAYBACK_POSITION_CHANGED = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_21),
    /// Supported the notification of "PLAYER_SETTING_CHANGED"
    AVRC_CT_FEATURE_NOTIFY_PLAYER_SETTING_CHANGED = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_22),
    /// Supported the notification of "NOW_PLAYING_CONTENT_CHANGED"
    AVRC_CT_FEATURE_NOTIFY_NOW_PLAYING_CONTENT_CHANGED = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_23),
    /// Supported the notification of "UIDS_CHANGED"
    AVRC_CT_FEATURE_NOTIFY_UIDS_CHANGED = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_24),
    /// Supported the notification of "ABSOLUTE_VOLUME_CHANGED"
    AVRC_CT_FEATURE_NOTIFY_ABSOLUTE_VOLUME_CHANGED = (AVRC_CT_FEATURE_CATEGORY_1 << AVRC_CT_FEATURE_MOVE_BIT_25),
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_CT_INTERNAL_H
