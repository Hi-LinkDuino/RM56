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

#ifndef OHOS_BLUETOOTH_STANDARD_PARCEL_BT_UUID_H
#define OHOS_BLUETOOTH_STANDARD_PARCEL_BT_UUID_H

#include "bt_uuid.h"
#include "message_parcel.h"

namespace OHOS {
namespace Bluetooth {
class ParcelBtUuid {
public:
    /**
     * @brief Write ParcelBtUuid values to parcel.
     *
     * @param parcel Parcel which store ParcelBtUuid values.
     * @return Operation result.
     * @since 6
     */
    static bool WriteToParcel(MessageParcel &parcel, bluetooth::Uuid &uuid);

    /**
     * @brief Read ParcelBtUuid values from parcel.
     *
     * @param parcel Parcel which bring ParcelBtUuid values.
     * @return Operation result.
     * @since 6
     */
    static bluetooth::Uuid ReadFromParcel(MessageParcel &parcel);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_PARCEL_BT_UUID_H