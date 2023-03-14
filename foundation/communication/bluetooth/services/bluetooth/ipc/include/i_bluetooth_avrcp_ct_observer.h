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

#ifndef OHOS_BLUETOOTH_STANDARD_AVRCP_CT_OBSERVER_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_AVRCP_CT_OBSERVER_INTERFACE_H

#include <cstdint>
#include <vector>
#include "iremote_broker.h"
#include "../parcel/bluetooth_avrcp_meItem.h"
#include "../parcel/bluetooth_avrcp_mpItem.h"
#include "raw_address.h"
#include "avrcp_media.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
class IBluetoothAvrcpCtObserver: public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothAvrcpCtObserver");

    enum Code {
        AVRCP_CT_CONNECTION_STATE_CHANGED = 0,
        AVRCP_CT_PRESS_BUTTON,
        AVRCP_CT_RELEASE_BUTTON,
        AVRCP_CT_SET_BROWSED_PLAYER,
        AVRCP_CT_GET_CAPABILITIES,
        AVRCP_CT_GET_PLAYER_APP_SETTING_ATTRIBUTES,
        AVRCP_CT_GET_PLAYER_APP_SETTING_VALUES,
        AVRCP_CT_GET_PLAYER_APP_SETTING_CURRENT_VALUE,
        AVRCP_CT_SET_PLAYER_APP_SETTING_CURRENT_VALUE,
        AVRCP_CT_GET_PLAYER_APP_SETTING_ATTRIBUTE_TEXT,
        AVRCP_CT_GET_PLAYER_APP_SETTING_VALUE_TEXT,
        AVRCP_CT_ELEMENT_ATTRIBUTRES,
        AVRCP_CT_GET_PLAY_STATUS,
        AVRCP_CT_PLAY_ITEM,
        AVRCP_CT_GET_TOTAL_NUMBER_OF_ITEMS,
        AVRCP_CT_GET_ITEM_ATTRIBUTES,
        AVRCP_CT_SET_ABSOLUTE_VOLUME,
        AVRCP_CT_PLAYBACK_STATUS_CHANGED,
        AVRCP_CT_TRACK_CHANGED,
        AVRCP_CT_TRACK_REACHED_END,
        AVRCP_CT_TRACK_REACHED_START,
        AVRCP_CT_PLAYBACK_POS_CHANGED,
        AVRCP_CT_PLAY_APP_SETTING_CHANGED,
        AVRCP_CT_NOW_PLAYING_CONTENT_CHANGED,
        AVRCP_CT_AVAILABLE_PLAYER_CHANGED,
        AVRCP_CT_ADDRESSED_PLAYER_CHANGED,
        AVRCP_CT_UID_CHANGED,
        AVRCP_CT_VOLUME_CHANGED,
        AVRCP_CT_GET_MEDIA_PLAYERS,
        AVRCP_CT_GET_FOLDER_ITEMS,
    };

    virtual void OnConnectionStateChanged(const RawAddress &rawAddr, int state) = 0;
    virtual void OnPressButton(const RawAddress &rawAddr, uint8_t button, int result) = 0;
    virtual void OnReleaseButton(const RawAddress &rawAddr, uint8_t button,  int result) = 0;
    virtual void OnSetBrowsedPlayer(const RawAddress &rawAddr, uint16_t uidCounter,
        uint32_t numberOfItems, const std::vector<std::string> &folderNames, int result, int detail) = 0;
    virtual void OnGetCapabilities(const RawAddress &rawAddr, const std::vector<uint32_t> &companies,
        const std::vector<uint8_t> &events, int result) = 0;
    virtual void OnGetPlayerAppSettingAttributes(
        const RawAddress &rawAddr, std::vector<uint8_t> attributes, int result) = 0;
    virtual void OnGetPlayerAppSettingValues(const RawAddress &rawAddr,
        int attribute, const std::vector<uint8_t> &values, int result) = 0;
    virtual void OnGetPlayerAppSettingCurrentValue(
        const RawAddress &rawAddr, const std::vector<uint8_t> attributes, const std::vector<uint8_t> &values, int result) = 0;
    virtual void OnSetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, int result) = 0;
    virtual void OnGetPlayerAppSettingAttributeText(const RawAddress &rawAddr,
        const std::vector<uint8_t> attribtues, const std::vector<std::string> &attributeName, int result) = 0;
    virtual void OnGetPlayerAppSettingValueText(const RawAddress &rawAddr,
        const std::vector<uint8_t> &values, const std::vector<std::string> &valueName, int result) = 0;
    virtual void OnGetElementAttributes(const RawAddress &rawAddr,
        const std::vector<uint32_t> &attribtues, const std::vector<std::string> &valueName, int result) = 0;
    virtual void OnGetPlayStatus(const RawAddress &rawAddr,
        uint32_t songLength, uint32_t songPosition, uint8_t playStatus, int result) = 0;
    virtual void OnPlayItem(const RawAddress &rawAddr, int status, int result) = 0;
    virtual void OnGetTotalNumberOfItems(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems,
        int result, int detail) = 0;
    virtual void OnGetItemAttributes(const RawAddress &rawAddr,
        const std::vector<uint32_t> &attribtues, const std::vector<std::string> &values, int result, int detail) = 0;
    virtual void OnSetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, int result) = 0;
    virtual void OnPlaybackStatusChanged(const RawAddress &rawAddr, uint8_t playStatus, int result) = 0;
    virtual void OnTrackChanged(const RawAddress &rawAddr, uint64_t uid, int result) = 0;
    virtual void OnTrackReachedEnd(const RawAddress &rawAddr, int result) = 0;
    virtual void OnTrackReachedStart(const RawAddress &rawAddr, int result) = 0;
    virtual void OnPlaybackPosChanged(const RawAddress &rawAddr, uint32_t playbackPos, int result) = 0;
    virtual void OnPlayerAppSettingChanged(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
        const std::vector<uint8_t> &values, int result) = 0;
    virtual void OnNowPlayingContentChanged(const RawAddress &rawAddr, int result) = 0;
    virtual void OnAvailablePlayersChanged(const RawAddress &rawAddr, int result) = 0;
    virtual void OnAddressedPlayerChanged(
        const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, int result) = 0;
    virtual void OnUidChanged(const RawAddress &rawAddr, uint16_t uidCounter, int result) = 0;
    virtual void OnVolumeChanged(const RawAddress &rawAddr, uint8_t volume, int result) = 0;
    virtual void OnGetMediaPlayers(const RawAddress &rawAddr, uint16_t uidCounter,
        std::vector<BluetoothAvrcpMpItem> &items, int result, int detail) = 0;
    virtual void OnGetFolderItems(const RawAddress &rawAddr, uint16_t uidCounter, std::vector<BluetoothAvrcpMeItem> &items,
           int result, int detail) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_AVRCP_CT_OBSERVER_INTERFACE_H