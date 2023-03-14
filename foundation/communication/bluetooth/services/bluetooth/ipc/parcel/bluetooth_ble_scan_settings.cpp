/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "bluetooth_ble_scan_settings.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothBleScanSettings::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt64(reportDelayMillis_)) {
        return false;
    }
    if (!parcel.WriteInt32(scanMode_)) {
        return false;
    }
    if (!parcel.WriteBool(legacy_)) {
        return false;
    }
    if (!parcel.WriteInt32(phy_)) {
        return false;
    }
    return true;
}

BluetoothBleScanSettings *BluetoothBleScanSettings::Unmarshalling(Parcel &parcel)
{
    BluetoothBleScanSettings *settings = new BluetoothBleScanSettings();
    if (settings != nullptr && !settings->ReadFromParcel(parcel)) {
        delete settings;
        settings = nullptr;
    }
    return settings;
}

bool BluetoothBleScanSettings::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothBleScanSettings::ReadFromParcel(Parcel &parcel)
{
    int64_t reportDelayMillis = 0;
    if (parcel.ReadInt64(reportDelayMillis)) {
        BluetoothBleScanSettings::SetReportDelay(reportDelayMillis);
    } else {
        return false;
    }
    if (!parcel.ReadInt32(scanMode_)) {
        return false;
    }
    if (!parcel.ReadBool(legacy_)) {
        return false;
    }
    if (!parcel.ReadInt32(phy_)) {
        return false;
    }
    return true;
}
}  // namespace Bluetooth
}  // namespace OHOS
