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

#ifndef OHOS_BLUETOOTH_STANDARD_AVRCP_CT_OBSERVER_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_AVRCP_CT_OBSERVER_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_host.h"
#include "i_bluetooth_avrcp_ct_observer.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothAvrcpCtObserverProxy : public IRemoteProxy<IBluetoothAvrcpCtObserver> {
public:
    explicit BluetoothAvrcpCtObserverProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<IBluetoothAvrcpCtObserver>(impl) {};
    ~BluetoothAvrcpCtObserverProxy() {};

    void OnConnectionStateChanged(const RawAddress &rawAddr, int state) override;
    void OnPressButton(const RawAddress &rawAddr, uint8_t button, int result) override;
    void OnReleaseButton(const RawAddress &rawAddr, uint8_t button,  int result) override;
    void OnSetBrowsedPlayer(const RawAddress &rawAddr, uint16_t uidCounter,
        uint32_t numberOfItems, const std::vector<std::string> &folderNames, int result, int detail) override;
    void OnGetCapabilities(const RawAddress &rawAddr, const std::vector<uint32_t> &companies,
        const std::vector<uint8_t> &events, int result) override;
    void OnGetPlayerAppSettingAttributes(
        const RawAddress &rawAddr, std::vector<uint8_t> attributes, int result) override;
    void OnGetPlayerAppSettingValues(const RawAddress &rawAddr,
        int attribute, const std::vector<uint8_t> &values, int result) override;
    void OnGetPlayerAppSettingCurrentValue(
        const RawAddress &rawAddr, const std::vector<uint8_t> attributes,
            const std::vector<uint8_t> &values, int result) override;
    void OnSetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, int result) override;
    void OnGetPlayerAppSettingAttributeText(const RawAddress &rawAddr,
        const std::vector<uint8_t> attribtues, const std::vector<std::string> &attributeName, int result) override;
    void OnGetPlayerAppSettingValueText(const RawAddress &rawAddr,
        const std::vector<uint8_t> &values, const std::vector<std::string> &valueName, int result) override;
    void OnGetElementAttributes(const RawAddress &rawAddr,
        const std::vector<uint32_t> &attribtues, const std::vector<std::string> &valueName, int result) override;
    void OnGetPlayStatus(const RawAddress &rawAddr,
        uint32_t songLength, uint32_t songPosition, uint8_t playStatus, int result) override;
    void OnPlayItem(const RawAddress &rawAddr, int status, int result) override;
    void OnGetTotalNumberOfItems(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems,
        int result, int detail) override;
    void OnGetItemAttributes(const RawAddress &rawAddr,
        const std::vector<uint32_t> &attribtues, const std::vector<std::string> &values,
            int result, int detail) override;
    void OnSetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, int result) override;
    void OnPlaybackStatusChanged(const RawAddress &rawAddr, uint8_t playStatus, int result) override;
    void OnTrackChanged(const RawAddress &rawAddr, uint64_t uid, int result) override;
    void OnTrackReachedEnd(const RawAddress &rawAddr, int result) override;
    void OnTrackReachedStart(const RawAddress &rawAddr, int result) override;
    void OnPlaybackPosChanged(const RawAddress &rawAddr, uint32_t playbackPos, int result) override;
    void OnPlayerAppSettingChanged(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
        const std::vector<uint8_t> &values, int result) override;
    void OnNowPlayingContentChanged(const RawAddress &rawAddr, int result) override;
    void OnAvailablePlayersChanged(const RawAddress &rawAddr, int result) override;
    void OnAddressedPlayerChanged(
        const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, int result) override;
    void OnUidChanged(const RawAddress &rawAddr, uint16_t uidCounter, int result) override;
    void OnVolumeChanged(const RawAddress &rawAddr, uint8_t volume, int result) override;
    void OnGetMediaPlayers(const RawAddress &rawAddr, uint16_t uidCounter,
        std::vector<BluetoothAvrcpMpItem> &items, int result, int detail) override;
    void OnGetFolderItems(const RawAddress &rawAddr, uint16_t uidCounter, std::vector<BluetoothAvrcpMeItem> &items,
        int result, int detail) override;

private:
    static inline BrokerDelegator<BluetoothAvrcpCtObserverProxy> delegator_;
    ErrCode InnerTransact(uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Write the serializable data
     * @param parcelableVector The communication data of IPC
     * @param reply Serializable data
     * @return true: Write the serializable data successfully; otherwise is not.
     */
    bool WriteParcelableInt32Vector(const std::vector<int32_t> &parcelableVector, Parcel &reply);

    /**
     * @brief Write the serializable data
     * @param parcelableVector The communication data of IPC
     * @param reply Serializable data
     * @return true: Write the serializable data successfully; otherwise is not.
     */
    bool WriteParcelableUint32Vector(const std::vector<uint32_t> &parcelableVector, Parcel &reply);

    /**
     * @brief Write the serializable data
     * @param parcelableVector The communication data of IPC
     * @param reply Serializable data
     * @return true: Write the serializable data successfully; otherwise is not.
     */
    bool WriteParcelableStringVector(const std::vector<std::string> &parcelableVector, Parcel &reply);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_AVRCP_CT_OBSERVER_PROXY_H