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

#include "compat.h"

#include <algorithm>

namespace OHOS {
namespace bluetooth {
const std::map<CompatType, std::vector<std::string>> Compat::COMPAT_DATA = {
    {CompatType::COMPAT_DISABLE_BLE_SECURE_CONNECTIONS, {
        "08:62:66",
        "38:2C:4A:C9",
        "38:2C:4A:E6",
        "54:A0:50:D9",
        "AC:9E:17",
        "F0:79:59",
        "80:E4:DA:70",
        "1C:96:5A"}},
    {CompatType::COMPAT_REJECT_NAME_REQUEST, {
        "D4:7A:E2"}},
    {CompatType::COMPAT_REJECT_AUTO_PAIRING, {
        "34:C7:31",
        "00:07:04",
        "E0:75:0A",
        "Audi",
        "BMW",
        "Parrot",
        "Car",
        "NISSAN",
        "CAR"}},
    {CompatType::COMPAT_KEYBOARD_REQUIRES_FIXED_PIN, {
        "00:0F:F6"}},
    {CompatType::COMPAT_GATTC_NO_SERVICE_CHANGED_IND, {
        "Pixel C Keyboard"}},
    {CompatType::COMPAT_REJECT_AVDTP_RECONFIGURE, {
        "00:1D:86",
        "KMM-BT51*HD"}},
    {CompatType::COMPAT_REJECT_ABSOLUTE_VOLUME, {
        "44:5E:F3",
        "D4:9C:28",
        "00:24:1C",
        "A4:15:66",
        "00:26:7E",
        "90:03:B7",
        "0C:A6:94",
        "00:18:6B",
        "B8:AD:3E",
        "A0:E9:DB",
        "00:14:02",
        "00:11:B1",
        "00:14:F1",
        "94:16:25",
        "9C:64:8B"}}
};

bool Compat::CompatCheck(CompatType type, const std::string &value)
{
    auto data = COMPAT_DATA.find(type);
    if (data != COMPAT_DATA.end()) {
        for (auto it = data->second.begin(); it != data->second.end(); it++) {
            if (value.compare(0, it->size(), *it) == 0) {
                return true;
            }
        }
    }
    return false;
}
}  // namespace bluetooth
}  // namespace OHOS