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

#ifndef COMPAT_H
#define COMPAT_H

#include <map>
#include <string>
#include <vector>

namespace OHOS {
namespace bluetooth {
enum class CompatType : int {
    COMPAT_DISABLE_BLE_SECURE_CONNECTIONS,
    COMPAT_REJECT_NAME_REQUEST,
    COMPAT_REJECT_AUTO_PAIRING,
    COMPAT_KEYBOARD_REQUIRES_FIXED_PIN,
    COMPAT_GATTC_NO_SERVICE_CHANGED_IND,
	COMPAT_REJECT_AVDTP_RECONFIGURE,
    COMPAT_REJECT_ABSOLUTE_VOLUME,
};

class Compat {
public:
    static bool CompatCheck(CompatType type, const std::string &value);
private:
    static const std::map<CompatType, std::vector<std::string>> COMPAT_DATA;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // COMPAT_H