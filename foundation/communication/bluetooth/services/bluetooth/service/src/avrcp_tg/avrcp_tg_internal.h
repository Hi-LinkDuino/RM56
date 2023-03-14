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

#ifndef AVRCP_TG_INTERNAL_H
#define AVRCP_TG_INTERNAL_H

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
#include "message.h"
#include "packet.h"
#include "raw_address.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
/// The macro used to safely delete the pointer to an object created by[new].
template<typename T>
void AvrcpTgSafeDelete(T *p)
{
    if (p != nullptr) {
        delete p;
        p = nullptr;
    }
}

/// The Version of AVRCP Profile is 1.6.
static const uint16_t AVRC_TG_PROFILE_REV_1_6 = 0x0106;
/// The default mtu size of the control channel.
/// 512 bytes is the smallest mtu.
static const int AVRC_TG_DEFAULT_CONTROL_MTU_SIZE = 512;
/// The default mtu size of the browse channel.
static const int AVRC_TG_DEFAULT_BROWSE_MTU_SIZE = 335;
/// The default Bluetooth SIG company ID.
static const uint32_t AVRC_TG_DEFAULT_BLUETOOTH_SIG_COMPANY_ID = 0x001958;
/// The default address of the bluetooth device.
static const std::string AVRC_TG_DEFAULT_BLUETOOTH_ADDRESS = "00:00:00:00:00:00";
/// The AVRCP TG shall return its response frame within 100 milliseconds counting from the receipt of the command frame.
static const int AVRC_TG_TIMER_T_RCP = 100;
/// The AVRCP TG shall return its response frame within 200 milliseconds counting from the receipt of the command frame.
static const int AVRC_TG_TIMER_T_MTC = 200;
/// The AVRCP TG shall return its response frame within 1000 milliseconds counting from the receipt of the command
/// frame.
static const int AVRC_TG_TIMER_T_MTP = 1000;
/// Either if the target has not received the pressed command within two seconds or the target receives the pressed
/// command with another operation id, then the target regards that the released command was sent but missed to receive.
/// @see AV/C Panel Subunit Specification 1.23 Section 9.4 PASS THROUGH control command.
static const int AVRC_TG_TIMER_PASS_THROUGH = 2000;
/// The default size of the command queue.
/// This means how many commands can be cached for a connection.
static const int AVRC_TG_DEFAULT_SIZE_OF_QUEUE = 20;
/// The maximum of number of device connections
static const int AVRC_TG_DEFAULT_MAX_OF_CONN = 6;
    /**
 * @brief This enumeration declares applicable to service class UUIDs that are registered into the SDP.
 */
using AvrcTgServiceClassUUIDs = enum {
    ///  A/V remote control target.
    AVRC_TG_AV_REMOTE_CONTROL_TARGET = 0x110C,
    /// A/V remote control.
    /// NOTE: Used as both Service Class Identifier and Profile Identifier.
    AVRC_TG_AV_REMOTE_CONTROL = 0x110E,
    /// A/V remote control controller.
    /// NOTE: The AVRCP specification v1.3 and later require that 0x110E also be included in the ServiceClassIDList
    /// before 0x110F for backwards compatibility.
    AVRC_TG_AV_REMOTE_CONTROL_CONTROLLER = 0x110F,
};


/**
 * @brief This enumeration declares a set of values associated with the offset
 * frame.
 */
enum AvrcTgOffset : uint8_t {
    AVRC_TG_OFFSET_ONE_BIT = 0x01,
    AVRC_TG_OFFSET_TWO_BITS = 0x02,
    AVRC_TG_OFFSET_THREE_BITS = 0x03,
    AVRC_TG_OFFSET_FOUR_BITS = 0x04,
    AVRC_TG_OFFSET_FIVE_BITS = 0x05,
    AVRC_TG_OFFSET_SIX_BITS = 0x06,
    AVRC_TG_OFFSET_SEVEN_BITS = 0x07,
    AVRC_TG_OFFSET_EIGHT_BITS = 0x08,
    AVRC_TG_OFFSET_SIXTEEN_BITS = 0x10,
    AVRC_TG_OFFSET_SEVENTEEN_BITS = 0x11,
    AVRC_TG_OFFSET_EIGHTEEN_BITS = 0x12,
    AVRC_TG_OFFSET_NINETEEN_BITS = 0x13,
    AVRC_TG_OFFSET_TWENTY_BITS = 0x14,
    AVRC_TG_OFFSET_TWENTYONE_BITS = 0x15,
    AVRC_TG_OFFSET_TWENTYTWO_BITS = 0x16,
    AVRC_TG_OFFSET_TWENTYTHREE_BITS = 0x17,
    AVRC_TG_OFFSET_TWENTYFOUR_BITS = 0x18,
    AVRC_TG_OFFSET_TWENTYFIVE_BITS = 0x19,
};

/**
 * @brief This enumeration declares features that are supported by the AVRCP TG profile.
 *
 * @detail Some of the features will be registered into the SDP.
 * @see Audio/Video Remote Control 1.6.2 Section 8 Service discovery interoperability requirements ->
 *      Table 8.2: Service record for TG.
 */
using AvrcTgFeatures = enum {
    /// Invalid value.
    AVRC_TG_FEATURE_INVALID_FEATURE = 0x0000,

    /// Supported category 1: player/recorder.
    AVRC_TG_FEATURE_CATEGORY_1 = 0x0001,
    /// Supported category 2: monitor/amplifier.
    AVRC_TG_FEATURE_CATEGORY_2 = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_ONE_BIT),
    /// Supported category 3: tuner.
    AVRC_TG_FEATURE_CATEGORY_3 = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_TWO_BITS),
    /// Supported category 4: menu.
    AVRC_TG_FEATURE_CATEGORY_4 = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_THREE_BITS),
    /// Supported player application settings.
    AVRC_TG_FEATURE_PLAYER_APPLICATION_SETTINGS = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_FOUR_BITS),
    /// Supported group navigation.
    AVRC_TG_FEATURE_GROUP_NAVIGATION = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_FIVE_BITS),
    /// Supported browsing.
    /// This in the SDP record shall only be supported if browsing of the "Media Player Virtual File system" is
    /// supported.
    AVRC_TG_FEATURE_BROWSING = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_SIX_BITS),
    /// Supported multiple media player applications.
    AVRC_TG_FEATURE_MULTIPLE_MEDIA_PLAYER_APPLICATIONS = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_SEVEN_BITS),
    /// Supported cover art.
    AVRC_TG_FEATURE_COVER_ART = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_EIGHT_BITS),

    /// Supported the key operation.
    AVRC_TG_FEATURE_KEY_OPERATION = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_SIXTEEN_BITS),
    /// Supported the absolute volume.
    AVRC_TG_FEATURE_ABSOLUTE_VOLUME = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_SEVENTEEN_BITS),
    /// Supported the notification of "PLAYBACK_STATUS_CHANGED"
    AVRC_TG_FEATURE_NOTIFY_PLAYBACK_STATUS_CHANGED = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_EIGHTEEN_BITS),
    /// Supported the notification of "TRACK_REACHED_END"
    AVRC_TG_FEATURE_NOTIFY_TRACK_REACHED_END = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_NINETEEN_BITS),
    /// Supported the notification of "TRACK_REACHED_START"
    AVRC_TG_FEATURE_NOTIFY_TRACK_REACHED_START = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_TWENTY_BITS),
    /// Supported the notification of "PLAYBACK_POSITION_CHANGED"
    AVRC_TG_FEATURE_NOTIFY_PLAYBACK_POSITION_CHANGED = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_TWENTYONE_BITS),
    /// Supported the notification of "PLAYER_SETTING_CHANGED"
    AVRC_TG_FEATURE_NOTIFY_PLAYER_SETTING_CHANGED = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_TWENTYTWO_BITS),
    /// Supported the notification of "NOW_PLAYING_CONTENT_CHANGED"
    AVRC_TG_FEATURE_NOTIFY_NOW_PLAYING_CONTENT_CHANGED =
        (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_TWENTYTHREE_BITS),
    /// Supported the notification of "UIDS_CHANGED"
    AVRC_TG_FEATURE_NOTIFY_UIDS_CHANGED = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_TWENTYFOUR_BITS),
    /// Supported the notification of "ABSOLUTE_VOLUME_CHANGED"
    AVRC_TG_FEATURE_NOTIFY_ABSOLUTE_VOLUME_CHANGED = (AVRC_TG_FEATURE_CATEGORY_1 << AVRC_TG_OFFSET_TWENTYFIVE_BITS),
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_INTERNAL_H
