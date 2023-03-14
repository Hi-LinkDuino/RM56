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

#ifndef BLUETOOTH_PARCEL_BT_UUID_H
#define BLUETOOTH_PARCEL_BT_UUID_H

#include "bt_uuid.h"
#include "parcel.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief This class provides bluetooth uuid.
 *
 * @since 6
 */
class BluetoothUuid : public Parcelable, public bluetooth::Uuid {
public:
    /**
     * @brief A constructor used to create an <b>BluetoothUuid</b> instance.
     *
     * @since 6
     */
    BluetoothUuid() = default;

    /**
     * @brief A constructor used to create an <b>BluetoothUuid</b> instance.
     *
     * @param other Other Uuid to create a new BluetoothUuid.
     * @since 6
     */
    BluetoothUuid(const bluetooth::Uuid &other) : bluetooth::Uuid(other)
    {}  // NOLINT(implicit)

    /**
     * @brief A constructor used to create an <b>BluetoothUuid</b> instance.
     *
     * @param other Other BluetoothUuid to create a new BluetoothUuid.
     * @since 6
     */
    BluetoothUuid(const BluetoothUuid &other) : bluetooth::Uuid(other)
    {}  // NOLINT(implicit)

    /**
     * @brief A destructor used to delete the <b>BluetoothUuid</b> instance.
     *
     * @since 6
     */
    ~BluetoothUuid() = default;

    bool Marshalling(Parcel &parcel) const override;

    static BluetoothUuid *Unmarshalling(Parcel &parcel);
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // BLUETOOTH_PARCEL_BT_UUID_H