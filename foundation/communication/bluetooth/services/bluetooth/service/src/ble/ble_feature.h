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

#ifndef BLE_FEATURE_H
#define BLE_FEATURE_H

#include <vector>

#include "btm.h"
#include "gap_le_if.h"

/*
 * @brief The bluetooth system.
 */
namespace OHOS {
namespace bluetooth {
/*
 * @brief Ble feature.
 */
class BleFeature {
public:
    /**
     * @brief Get ble feature instance.
     *
     * @return @c feature instance.
     */
    static BleFeature &GetInstance()
    {
        static BleFeature instance;
        return instance;
    }

    /**
     * @brief Get ble feature extended scan supported.
     *
     * @return @c true:supported; false:not supported.
     */
    bool IsExtendedScanSupported() const
    {
        return IsFeatureSupported(EXTENDED_SCANNER_FILTER_POLICIES);
    }

    /**
     * @brief Get ble feature 2M phy supported.
     *
     * @return @c true:supported; false:not supported.
     */
    bool IsLe2MPhySupported() const
    {
        return IsFeatureSupported(LE_2M_PHY);
    }

    /**
     * @brief Get ble feature coded phy supported.
     *
     * @return @c true:supported; false:not supported.
     */
    bool IsLeCodedPhySupported() const
    {
        return IsFeatureSupported(LE_CODED_PHY);
    }

    /**
     * @brief Get ble feature extended advertising supported.
     *
     * @return @c true:supported; false:not supported.
     */
    bool IsLeExtendedAdvertisingSupported() const
    {
        return IsFeatureSupported(LE_EXTENDED_ADVERTISING);
    }

    /**
     * @brief Get ble feature periodic advertising supported.
     *
     * @return @c true:supported; false:not supported.
     */
    bool IsLePeriodicAdvertisingSupported() const
    {
        return IsFeatureSupported(LE_PERIODIC_ADVERTISING);
    }

    /**
     * @brief Get ble feature privacy supported.
     *
     * @return @c true:supported; false:not supported.
     */
    bool IsPrivacySupported() const
    {
        return IsFeatureSupported(LL_PRIVACY);
    }

    /**
     * @brief Get ble feature maximum advertising data length.
     *
     * @return @c data length.
     */
    int GetBleMaximumAdvertisingDataLength() const
    {
        if (IsFeatureSupported(LE_DATA_PACKET_LENGTH_EXTENSION)) {
            uint16_t length = BLE_LEGACY_ADV_DATA_LEN_MAX;
            int ret = GAPIF_LeExAdvGetMaxDataLen(&length);
            if (ret == BT_NO_ERROR) {
                return length;
            }
            return BLE_LEGACY_ADV_DATA_LEN_MAX;
        } else {
            return BLE_LEGACY_ADV_DATA_LEN_MAX;
        }
    }

    /**
     * @brief Get ble feature extended advertising max handle number.
     *
     * @return @c handle number.
     */
    static uint8_t GetBleExAdvGetMaxHandleNum()
    {
        uint8_t num = BLE_INVALID_ADVERTISING_HANDLE;
        int ret = GAPIF_LeExAdvGetMaxHandleNum(&num);
        if (ret == BT_NO_ERROR) {
            return num;
        }
        return num;
    }

    /**
     * @brief Get ble supported feature.
     *
     * @return @c true:supported; false:not supported.
     */
    static bool IsFeatureSupported(FEATURE_SUPPORTED feature)
    {
        switch (feature) {
            case LE_2M_PHY:
                return BTM_IsControllerSupportLe2MPhy();
                break;
            case LE_CODED_PHY:
                return BTM_IsControllerSupportLeCodedPhy();
                break;
            case LE_EXTENDED_ADVERTISING:
            case EXTENDED_SCANNER_FILTER_POLICIES:
                return BTM_IsControllerSupportLeExtendedAdvertising();
                break;
            case LE_PERIODIC_ADVERTISING:
                return BTM_IsControllerSupportLePeriodicAdvertising();
                break;
            case LL_PRIVACY:
                return BTM_IsControllerSupportLlPrivacy();
                break;
            case LE_DATA_PACKET_LENGTH_EXTENSION:
                return BTM_IsControllerSupportLeDataPacketLengthExtension();
                break;
            default:
                break;
        }
        return false;
    }

private:
    /**
     * @brief Constructor.
     */
    BleFeature()
    {}

    /**
     * @brief Destructor.
     */
    ~BleFeature()
    {}
    /**
     * @brief Constructor.
     */
    BleFeature(BleFeature &) = delete;

    /**
     * @brief Constructor.
     */
    BleFeature &operator=(const BleFeature &) = delete;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // BLE_FEATURE_H